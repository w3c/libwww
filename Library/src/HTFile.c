/*			File Access				HTFile.c
**			===========
**
**	This is unix-specific code in general, with some VMS bits.
**	These are routines for file access used by browsers.
**
** History:
**	   Feb 91	Written Tim Berners-Lee CERN/CN
**	   Apr 91	vms-vms access included using DECnet syntax
**	26 Jun 92 (JFG) When running over DECnet, suppressed FTP.
**			Fixed access bug for relative names on VMS.
**
** Bugs:
**	FTP: Cannot access VMS files from a unix machine.
**      How can we know that the
**	target machine runs VMS?
*/

#include "HTFile.h"		/* Implemented here */


#define INFINITY 512		/* file name length @@ FIXME */
#define MULTI_SUFFIX ".multi"   /* Extension for scanning formats */

#include "HTUtils.h"

#include "HTParse.h"
#include "tcp.h"
#include "HTTCP.h"
#ifndef DECNET
#include "HTFTP.h"
#endif
#include "HTAnchor.h"
#include "HTAtom.h"
#include "HTWriter.h"
#include "HTFWriter.h"
#include "HTInit.h"
#include "HTBTree.h"

typedef struct _HTSuffix {
	char *		suffix;
	HTAtom *	rep;
	HTAtom *	encoding;
	float		quality;
} HTSuffix;


#ifdef USE_DIRENT		/* Set this for Sys V systems */
#define STRUCT_DIRENT struct dirent
#else
#define STRUCT_DIRENT struct direct
#endif

#include "HTML.h"		/* For directory object building */

#define PUTC(c) (*target->isa->put_character)(target, c)
#define PUTS(s) (*target->isa->put_string)(target, s)
#define START(e) (*target->isa->start_element)(target, e, 0, 0)
#define END(e) (*target->isa->end_element)(target, e)
#define FREE_TARGET (*target->isa->free)(target)
struct _HTStructured {
	CONST HTStructuredClass *	isa;
	/* ... */
};


/*                   Controlling globals
**
*/

PUBLIC int HTDirAccess = HT_DIR_OK;
PUBLIC int HTDirReadme = HT_DIR_README_TOP;

PRIVATE char *HTMountRoot = "/Net/";		/* Where to find mounts */
#ifdef vms
PRIVATE char *HTCacheRoot = "/WWW$SCRATCH/";   /* Where to cache things */
#else
PRIVATE char *HTCacheRoot = "/tmp/W3_Cache_";   /* Where to cache things */
#endif

/* PRIVATE char *HTSaveRoot  = "$(HOME)/WWW/";*/    /* Where to save things */


/*	Suffix registration
*/

PRIVATE HTList * HTSuffixes = 0;
PRIVATE HTSuffix no_suffix = { "*", NULL, NULL, 1.0 };
PRIVATE HTSuffix unknown_suffix = { "*.*", NULL, NULL, 1.0};


/*	Define the representation associated with a file suffix
**	-------------------------------------------------------
**
**	Calling this with suffix set to "*" will set the default
**	representation.
**	Calling this with suffix set to "*.*" will set the default
**	representation for unknown suffix files which contain a ".".
*/
PUBLIC void HTSetSuffix ARGS4(
	CONST char *,	suffix,
	CONST char *,	representation,
	CONST char *,	encoding,
	float,		value)
{
    
    HTSuffix * suff;
    
    if (strcmp(suffix, "*")==0) suff = &no_suffix;
    else if (strcmp(suffix, "*.*")==0) suff = &unknown_suffix;
    else {
	suff = (HTSuffix*) calloc(1, sizeof(HTSuffix));
	if (suff == NULL) outofmem(__FILE__, "HTSetSuffix");
	
	if (!HTSuffixes) HTSuffixes = HTList_new();
	HTList_addObject(HTSuffixes, suff);
	
	StrAllocCopy(suff->suffix, suffix);
    }

    suff->rep = HTAtom_for(representation);
    
    {
    	char * enc = NULL;
	char * p;
	StrAllocCopy(enc, encoding);
	for (p=enc; *p; p++) *p = TOLOWER(*p);
	suff->encoding = HTAtom_for(encoding);
    }
    
    suff->quality = value;
}




#ifdef vms
/*	Convert unix-style name into VMS name
**	-------------------------------------
**
** Bug:	Returns pointer to static -- non-reentrant
*/
PRIVATE char * vms_name(CONST char * nn, CONST char * fn)
{

/*	We try converting the filename into Files-11 syntax. That is, we assume
**	first that the file is, like us, on a VMS node. We try remote
**	(or local) DECnet access. Files-11, VMS, VAX and DECnet
**	are trademarks of Digital Equipment Corporation. 
**	The node is assumed to be local if the hostname WITHOUT DOMAIN
**	matches the local one. @@@
*/
    static char vmsname[INFINITY];	/* returned */
    char * filename = (char*)malloc(strlen(fn)+1);
    char * nodename = (char*)malloc(strlen(nn)+2+1);	/* Copies to hack */
    char *second;		/* 2nd slash */
    char *last;			/* last slash */
    
    char * hostname = HTHostName();

    if (!filename || !nodename) outofmem(__FILE__, "vms_name");
    strcpy(filename, fn);
    strcpy(nodename, "");	/* On same node? Yes if node names match */
    {
        char *p, *q;
        for (p=hostname, q=nn; *p && *p!='.' && *q && *q!='.'; p++, q++){
	    if (TOUPPER(*p)!=TOUPPER(*q)) {
	        strcpy(nodename, nn);
		q = strchr(nodename, '.');	/* Mismatch */
		if (q) *q=0;			/* Chop domain */
		strcat(nodename, "::");		/* Try decnet anyway */
		break;
	    }
	}
    }

    second = strchr(filename+1, '/');		/* 2nd slash */
    last = strrchr(filename, '/');	/* last slash */
        
    if (!second) {				/* Only one slash */
	sprintf(vmsname, "%s%s", nodename, filename + 1);
    } else if(second==last) {		/* Exactly two slashes */
	*second = 0;		/* Split filename from disk */
	sprintf(vmsname, "%s%s:%s", nodename, filename+1, second+1);
	*second = '/';	/* restore */
    } else { 				/* More than two slashes */
	char * p;
	*second = 0;		/* Split disk from directories */
	*last = 0;		/* Split dir from filename */
	sprintf(vmsname, "%s%s:[%s]%s",
		nodename, filename+1, second+1, last+1);
	*second = *last = '/';	/* restore filename */
	for (p=strchr(vmsname, '['); *p!=']'; p++)
	    if (*p=='/') *p='.';	/* Convert dir sep.  to dots */
    }
    free(nodename);
    free(filename);
    return vmsname;
}


#endif /* vms */



/*	Send README file
**
**  If a README file exists, then it is inserted into the document here.
*/

#ifdef GOT_READ_DIR
PRIVATE void do_readme ARGS2(HTStructured *, target, CONST char *, localname)
{ 
    FILE * fp;
    char * readme_file_name = 
	malloc(strlen(localname)+ 1 + strlen(HT_DIR_README_FILE) + 1);
    strcpy(readme_file_name, localname);
    strcat(readme_file_name, "/");
    strcat(readme_file_name, HT_DIR_README_FILE);
    
    fp = fopen(readme_file_name,  "r");
    
    if (fp) {
	HTStructuredClass targetClass;
	
	targetClass =  *target->isa;	/* (Can't init agregate in K&R) */
	START(HTML_PRE);
	for(;;){
	    char c = fgetc(fp);
	    if (c == (char)EOF) break;
	    switch (c) {
	    	case '&':
		case '<':
		case '>':
			PUTC('&');
			PUTC('#');
			PUTC((char)(c / 10));
			PUTC((char) (c % 10));
			PUTC(';');
			break;
/*	    	case '\n':
			PUTC('\r');    
Bug removed thanks to joe@athena.mit.edu */			
		default:
			PUTC(c);
	    }
	}
	END(HTML_PRE);
	fclose(fp);
    } 
}
#endif


/*	Make the cache file name for a W3 document
**	------------------------------------------
**	Make up a suitable name for saving the node in
**
**	E.g.	/tmp/WWW_Cache_news/1234@cernvax.cern.ch
**		/tmp/WWW_Cache_http/crnvmc/FIND/xx.xxx.xx
**
** On exit,
**	returns	a malloc'ed string which must be freed by the caller.
*/
PUBLIC char * HTCacheFileName ARGS1(CONST char *,name)
{
    char * access = HTParse(name, "", PARSE_ACCESS);
    char * host = HTParse(name, "", PARSE_HOST);
    char * path = HTParse(name, "", PARSE_PATH+PARSE_PUNCTUATION);
    
    char * result;
    result = (char *)malloc(
	    strlen(HTCacheRoot)+strlen(access)
	    +strlen(host)+strlen(path)+6+1);
    if (result == NULL) outofmem(__FILE__, "HTCacheFileName");
    sprintf(result, "%s/WWW/%s/%s%s", HTCacheRoot, access, host, path);
    free(path);
    free(access);
    free(host);
    return result;
}


/*	Open a file for write, creating the path
**	----------------------------------------
*/
#ifdef NOT_IMPLEMENTED
PRIVATE int HTCreatePath ARGS1(CONST char *,path)
{
    return -1;
}
#endif

/*	Convert filenames between local and WWW formats
**	-----------------------------------------------
**	Make up a suitable name for saving the node in
**
**	E.g.	$(HOME)/WWW/news/1234@cernvax.cern.ch
**		$(HOME)/WWW/http/crnvmc/FIND/xx.xxx.xx
**
** On exit,
**	returns	a malloc'ed string which must be freed by the caller.
*/
PUBLIC char * HTLocalName ARGS1(CONST char *,name)
{
    char * access = HTParse(name, "", PARSE_ACCESS);
    char * host = HTParse(name, "", PARSE_HOST);
    char * path = HTParse(name, "", PARSE_PATH+PARSE_PUNCTUATION);
    
    HTUnEscape(path);	/* Interpret % signs */
    
    if (0==strcmp(access, "file")) {
        free(access);	
	if ((0==strcasecomp(host, HTHostName())) ||
	    (0==strcasecomp(host, "localhost")) || !*host) {
	    free(host);
	    if (TRACE) fprintf(stderr, "Node `%s' means path `%s'\n", name, path);
	    return(path);
	} else {
	    char * result = (char *)malloc(
	    			strlen("/Net/")+strlen(host)+strlen(path)+1);
              if (result == NULL) outofmem(__FILE__, "HTLocalName");
	    sprintf(result, "%s%s%s", "/Net/", host, path);
	    free(host);
	    free(path);
	    if (TRACE) fprintf(stderr, "Node `%s' means file `%s'\n", name, result);
	    return result;
	}
    } else {  /* other access */
	char * result;
        CONST char * home =  (CONST char*)getenv("HOME");
	if (!home) home = "/tmp"; 
	result = (char *)malloc(
		strlen(home)+strlen(access)+strlen(host)+strlen(path)+6+1);
      if (result == NULL) outofmem(__FILE__, "HTLocalName");
	sprintf(result, "%s/WWW/%s/%s%s", home, access, host, path);
	free(path);
	free(access);
	free(host);
	return result;
    }
}


/*	Make a WWW name from a full local path name
**
** Bugs:
**	At present, only the names of two network root nodes are hand-coded
**	in and valid for the NeXT only. This should be configurable in
**	the general case.
*/

PUBLIC char * WWW_nameOfFile ARGS1 (CONST char *,name)
{
    char * result;
#ifdef NeXT
    if (0==strncmp("/private/Net/", name, 13)) {
	result = (char *)malloc(7+strlen(name+13)+1);
	if (result == NULL) outofmem(__FILE__, "WWW_nameOfFile");
	sprintf(result, "file://%s", name+13);
    } else
#endif
    if (0==strncmp(HTMountRoot, name, 5)) {
	result = (char *)malloc(7+strlen(name+5)+1);
	if (result == NULL) outofmem(__FILE__, "WWW_nameOfFile");
	sprintf(result, "file://%s", name+5);
    } else {
        result = (char *)malloc(7+strlen(HTHostName())+strlen(name)+1);
	if (result == NULL) outofmem(__FILE__, "WWW_nameOfFile");
	sprintf(result, "file://%s%s", HTHostName(), name);
    }
    if (TRACE) fprintf(stderr, "File `%s'\n\tmeans node `%s'\n", name, result);
    return result;
}


/*	Determine a suitable suffix, given the representation
**	-----------------------------------------------------
**
** On entry,
**	rep	is the atomized MIME style representation
**
** On exit,
**	returns	a pointer to a suitable suffix string if one has been
**		found, else "".
*/
PUBLIC CONST char * HTFileSuffix ARGS1(HTAtom*, rep)
{
    HTSuffix * suff;
    int n;
    int i;

#ifndef NO_INIT    
    if (!HTSuffixes) HTFileInit();
#endif
    n = HTList_count(HTSuffixes);
    for(i=0; i<n; i++) {
	suff = HTList_objectAt(HTSuffixes, i);
	if (suff->rep == rep) {
	    return suff->suffix;		/* OK -- found */
	}
    }
    return "";		/* Dunno */
}


/*	Determine file format from file name
**	------------------------------------
**
**	This version will return the representation and also set
**	a variable for the encoding.
**
**	It will handle for example  x.txt, x.txt,Z, x.Z
*/

PUBLIC HTFormat HTFileFormat ARGS2 (
			CONST char *,	filename,
			HTAtom **,	pencoding)

{
    HTSuffix * suff;
    int n;
    int i;
    int lf = strlen(filename);

#ifndef NO_INIT    
    if (!HTSuffixes) HTFileInit();
#endif
    *pencoding = NULL;
    n = HTList_count(HTSuffixes);
    for(i=0; i<n; i++) {
        int ls;
	suff = HTList_objectAt(HTSuffixes, i);
	ls = strlen(suff->suffix);
	if ((ls <= lf) && 0==strcmp(suff->suffix, filename + lf - ls)) {
	    int j;
	    *pencoding = suff->encoding;
	    if (suff->rep) return suff->rep;		/* OK -- found */
	    
	    for(j=0; j<n; j++) {  /* Got encoding, need representation */
		int ls2;
		suff = HTList_objectAt(HTSuffixes, j);
		ls2 = strlen(suff->suffix);
		if ((ls <= lf) && 0==strncmp(
			suff->suffix, filename + lf - ls -ls2, ls2)) {
		    if (suff->rep) return suff->rep;
		}
	    }
	    
	}
    }
    
    /* defaults tree */
    
    suff = strchr(filename, '.') ? 	/* Unknown suffix */
    	 ( unknown_suffix.rep ? &unknown_suffix : &no_suffix)
	 : &no_suffix;
	 
    /* set default encoding unless found with suffix already */
    if (!*pencoding) *pencoding = suff->encoding ? suff->encoding
				    : HTAtom_for("binary");
    return suff->rep ? suff->rep : WWW_BINARY;
}


/*	Determine value from file name
**	------------------------------
**
*/

PUBLIC float HTFileValue ARGS1 (CONST char *,filename)

{
    HTSuffix * suff;
    int n;
    int i;
    int lf = strlen(filename);

#ifndef NO_INIT    
    if (!HTSuffixes) HTFileInit();
#endif
    n = HTList_count(HTSuffixes);
    for(i=0; i<n; i++) {
        int ls;
	suff = HTList_objectAt(HTSuffixes, i);
	ls = strlen(suff->suffix);
	if ((ls <= lf) && 0==strcmp(suff->suffix, filename + lf - ls)) {
	    if (TRACE) fprintf(stderr, "File: Value of %s is %.3f\n",
			       filename, suff->quality);
	    return suff->quality;		/* OK -- found */
	}
    }
    return 0.3;		/* Dunno! */
}


/*	Determine write access to a file
**	--------------------------------
**
** On exit,
**	return value	YES if file can be accessed and can be written to.
**
** Bugs:
**	1.	No code for non-unix systems.
**	2.	Isn't there a quicker way?
*/

#ifdef vms
#define NO_GROUPS
#endif
#ifdef NO_UNIX_IO
#define NO_GROUPS
#endif
#ifdef PCNFS
#define NO_GROUPS
#endif

PUBLIC BOOL HTEditable ARGS1 (CONST char *,filename)
{
#ifdef NO_GROUPS
    return NO;		/* Safe answer till we find the correct algorithm */
#else
    int 	groups[NGROUPS];	
    uid_t	myUid;
    int		ngroups;			/* The number of groups  */
    struct stat	fileStatus;
    int		i;
        
    if (stat(filename, &fileStatus))		/* Get details of filename */
    	return NO;				/* Can't even access file! */

    ngroups = getgroups(NGROUPS, groups);	/* Groups to which I belong  */
    myUid = geteuid();				/* Get my user identifier */

    if (TRACE) {
        int i;
	fprintf(stderr, 
	    "File mode is 0%o, uid=%d, gid=%d. My uid=%d, %d groups (",
    	    (unsigned int) fileStatus.st_mode, fileStatus.st_uid,
	    fileStatus.st_gid,
	    myUid, ngroups);
	for (i=0; i<ngroups; i++) fprintf(stderr, " %d", groups[i]);
	fprintf(stderr, ")\n");
    }
    
    if (fileStatus.st_mode & 0002)		/* I can write anyway? */
    	return YES;
	
    if ((fileStatus.st_mode & 0200)		/* I can write my own file? */
     && (fileStatus.st_uid == myUid))
    	return YES;

    if (fileStatus.st_mode & 0020)		/* Group I am in can write? */
    {
   	for (i=0; i<ngroups; i++) {
            if (groups[i] == fileStatus.st_gid)
	        return YES;
	}
    }
    if (TRACE) fprintf(stderr, "\tFile is not editable.\n");
    return NO;					/* If no excuse, can't do */
#endif
}


/*	Make a save stream
**	------------------
**
**	The stream must be used for writing back the file.
**	@@@ no backup done
*/
PUBLIC HTStream * HTFileSaveStream ARGS1(HTParentAnchor *, anchor)
{

    CONST char * addr = HTAnchor_address((HTAnchor*)anchor);
    char *  localname = HTLocalName(addr);
    
    FILE* fp = fopen(localname, "w");
    if (!fp) return NULL;
    
    return HTFWriter_new(fp);
    
}

/*      Output one directory entry
**
*/
PUBLIC void HTDirEntry ARGS3(HTStructured *, target,
		 CONST char * , tail,
		 CONST char *,  entry)
{
    char * relative;
    char * escaped = HTEscape(entry, URL_XPALPHAS);

    /* If empty tail, gives absolute ref below */
    relative = (char*) malloc(
			      strlen(tail) + strlen(escaped)+2);
    if (relative == NULL) outofmem(__FILE__, "DirRead");
    sprintf(relative, "%s/%s", tail, escaped);
    HTStartAnchor(target, NULL, relative);
    free(escaped);
    free(relative);
    PUTS(entry);
    END(HTML_A);
}
 
/*      Output parent directory entry
**
**    This gives the TITLE and H1 header, and also a link
**    to the parent directory if appropriate.
*/
PUBLIC void HTDirTitles ARGS2(HTStructured *, target,
		 HTAnchor * , anchor)

{
    char * logical = HTAnchor_address(anchor);
    char * path = HTParse(logical, "", PARSE_PATH + PARSE_PUNCTUATION);
    char * current;

    current = strrchr(path, '/');	/* last part or "" */
    free(logical);

    {
      char * printable = NULL;
      StrAllocCopy(printable, (current + 1));
      HTUnEscape(printable);
      START(HTML_TITLE);
      PUTS(*printable ? printable : "Welcome ");
      PUTS(" directory");
      END(HTML_TITLE);    
    
      START(HTML_H1);
      PUTS(*printable ? printable : "Welcome");
      END(HTML_H1);
      free(printable);
    }

    /*  Make link back to parent directory
     */

    if (current && current[1]) {   /* was a slash AND something else too */
        char * parent;
	char * relative;
	*current++ = 0;
      parent = strrchr(path, '/');  /* penultimate slash */

	relative = (char*) malloc(strlen(current) + 4);
	if (relative == NULL) outofmem(__FILE__, "DirRead");
	sprintf(relative, "%s/..", current);
	HTStartAnchor(target, "", relative);
	free(relative);

	PUTS("Up to ");
	if (parent) {
	  char * printable = NULL;
	  StrAllocCopy(printable, parent + 1);
	  HTUnEscape(printable);
	  PUTS(printable);
	  free(printable);
	} else {
	  PUTS("/");
	}

	END(HTML_A);

    }
    free(path);
}
		


/*	Load a document
**	---------------
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**			This is the physsical address of the file
**
** On exit,
**	returns		<0		Error has occured.
**			HTLOADED	OK 
**
*/
PUBLIC int HTLoadFile ARGS4 (
	CONST char *,		addr,
	HTParentAnchor *,	anchor,
	HTFormat,		format_out,
	HTStream *,		sink
)
{
    char * filename;
    HTFormat format;
    char * nodename = 0;
    char * newname=0;	/* Simplified name of file */
    HTAtom * encoding;	/* @@ not used yet */
    
/*	Reduce the filename to a basic form (hopefully unique!)
*/
    StrAllocCopy(newname, addr);
    filename=HTParse(newname, "", PARSE_PATH|PARSE_PUNCTUATION);
    nodename=HTParse(newname, "", PARSE_HOST);
    free(newname);
    
    format = HTFileFormat(filename, &encoding);


#ifdef vms
/* Assume that the file is in Unix-style syntax if it contains a '/'
   after the leading one @@ */
    {
	char * vmsname = strchr(filename + 1, '/') ?
	  vms_name(nodename, filename) : filename + 1;
	fd = open(vmsname, O_RDONLY, 0);
	
/*	If the file wasn't VMS syntax, then perhaps it is ultrix
*/
	if (fd<0) {
	    char ultrixname[INFINITY];
	    if (TRACE) fprintf(stderr, "HTFile: Can't open as %s\n", vmsname);
	    sprintf(ultrixname, "%s::\"%s\"", nodename, filename);
	    fd = open(ultrixname, O_RDONLY, 0);
	    if (fd<0) {
		if (TRACE) fprintf(stderr, 
				   "HTFile: Can't open as %s\n", ultrixname);
	    }
	}
    }
#else

    free(filename);
    
/*	For unix, we try to translate the name into the name of a transparently
**	mounted file.
**
**	Not allowed in secure (HTClienntHost) situations TBL 921019
*/
#ifndef NO_UNIX_IO
    /*  Need protection here for telnet server but not httpd server */
	 
    if (!HTSecure) {		/* try local file system */
	char * localname = HTLocalName(addr);
	struct stat dir_info;
	
#ifdef GOT_READ_DIR

/*			  Multiformat handling
**
**	If needed, scan directory to find a good file.
**  Bug:  we don't stat the file to find the length
*/
	if ( (strlen(localname) > strlen(MULTI_SUFFIX))
	   && (0==strcmp(localname + strlen(localname) - strlen(MULTI_SUFFIX),
	                  MULTI_SUFFIX))) {
	    DIR *dp;

	    STRUCT_DIRENT * dirbuf;
	    float best = NO_VALUE_FOUND;	/* So far best is bad */
	    HTFormat best_rep = NULL;	/* Set when rep found */
	    STRUCT_DIRENT best_dirbuf;	/* Best dir entry so far */

	    char * base = strrchr(localname, '/');
	    int baselen;

	    if (!base || base == localname) goto forget_multi;
	    *base++ = 0;		/* Just got directory name */
	    baselen = strlen(base)- strlen(MULTI_SUFFIX);
	    base[baselen] = 0;	/* Chop off suffix */

	    dp = opendir(localname);
	    if (!dp) {
forget_multi:
		free(localname);
		return HTLoadError(sink, 500,
			"Multiformat: directory scan failed.");
	    }
	    
	    while ((dirbuf = readdir(dp))!=0) {
			/* while there are directory entries to be read */
		if (dirbuf->d_ino == 0) continue;
				/* if the entry is not being used, skip it */
		
		if (dirbuf->d_namlen > baselen &&      /* Match? */
		    !strncmp(dirbuf->d_name, base, baselen)) {	
		    HTFormat rep = HTFileFormat(dirbuf->d_name, &encoding);
		    float value = HTStackValue(rep, format_out,
		    				HTFileValue(dirbuf->d_name),
						0.0  /* @@@@@@ */);
		    if (value != NO_VALUE_FOUND) {
		        if (TRACE) fprintf(stderr,
				"HTFile: value of presenting %s is %f\n",
				HTAtom_name(rep), value);
			if  (value > best) {
			    best_rep = rep;
			    best = value;
			    best_dirbuf = *dirbuf;
		       }
		    }	/* if best so far */ 		    
		 } /* if match */  
		    
	    } /* end while directory entries left to read */
	    closedir(dp);
	    
	    if (best_rep) {
		format = best_rep;
		base[-1] = '/';		/* Restore directory name */
		base[0] = 0;
		StrAllocCat(localname, best_dirbuf.d_name);
		goto open_file;
		
	    } else { 			/* If not found suitable file */
		free(localname);
		return HTLoadError(sink, 403,	/* List formats? */
		   "Could not find suitable representation for transmission.");
	    }
	    /*NOTREACHED*/
	} /* if multi suffix */
/*
**	Check to see if the 'localname' is in fact a directory.  If it is
**	create a new hypertext object containing a list of files and 
**	subdirectories contained in the directory.  All of these are links
**      to the directories or files listed.
**      NB This assumes the existance of a type 'STRUCT_DIRENT', which will
**      hold the directory entry, and a type 'DIR' which is used to point to
**      the current directory being read.
*/
	
	
	if (stat(localname,&dir_info) == -1) {     /* get file information */
	                               /* if can't read file information */
	    if (TRACE) fprintf(stderr, "HTFile: can't stat %s\n", localname);

	}  else {		/* Stat was OK */
		

	    if (((dir_info.st_mode) & S_IFMT) == S_IFDIR) {
		/* if localname is a directory */	

		HTStructured* target;		/* HTML object */
		HTStructuredClass targetClass;

		DIR *dp;
		STRUCT_DIRENT * dirbuf;
		
		char * logical;
		char * tail;
		
		BOOL present[HTML_A_ATTRIBUTES];
		
		char * tmpfilename = NULL;
		struct stat file_info;
		
		if (TRACE)
		    fprintf(stderr,"%s is a directory\n",localname);
			
/*	Check directory access.
**	Selective access means only those directories containing a
**	marker file can be browsed
*/
		if (HTDirAccess == HT_DIR_FORBID) {
		    free(localname);
		    return HTLoadError(sink, 403,
		    "Directory browsing is not allowed.");
		}


		if (HTDirAccess == HT_DIR_SELECTIVE) {
		    char * enable_file_name = 
			malloc(strlen(localname)+ 1 +
			 strlen(HT_DIR_ENABLE_FILE) + 1);
		    strcpy(enable_file_name, localname);
		    strcat(enable_file_name, "/");
		    strcat(enable_file_name, HT_DIR_ENABLE_FILE);
		    if (stat(enable_file_name, &file_info) != 0) {
			free(localname);
			return HTLoadError(sink, 403,
			"Selective access is not enabled for this directory");
		    }
		}

 
		dp = opendir(localname);
		if (!dp) {
		    free(localname);
		    return HTLoadError(sink, 403, "This directory is not readable.");
		}


 /*	Directory access is allowed and possible
 */
		logical = HTAnchor_address((HTAnchor*)anchor);
		tail = strrchr(logical, '/') +1;	/* last part or "" */
		
		target = HTML_new(anchor, format_out, sink);
		targetClass = *target->isa;	/* Copy routine entry points */
		    
  		{ int i;
			for(i=0; i<HTML_A_ATTRIBUTES; i++)
				present[i] = (i==HTML_A_HREF);
		}
		
                HTDirTitles(target, (HTAnchor *)anchor);

                if (HTDirReadme == HT_DIR_README_TOP)
		    do_readme(target, localname);
		{
		    HTBTree * bt = HTBTree_new((HTComparer)strcasecomp);

		    while ((dirbuf = readdir(dp))!=0)
		    {
		        HTBTElement * dirname = NULL;

			    /* while there are directory entries to be read */
		        if (dirbuf->d_ino == 0)
				  /* if the entry is not being used, skip it */
			    continue;
		    

				/* if the current entry is parent directory */
			if ((*(dirbuf->d_name)=='.') ||
				(*(dirbuf->d_name)==','))
			    continue;    /* skip those files whose name begins
					    with '.' or ',' */

			dirname = (HTBTElement *)malloc(
					strlen(dirbuf->d_name) + 2);
			if (dirname == NULL) outofmem(__FILE__,"DirRead");
			StrAllocCopy(tmpfilename,localname);
			if (strcmp(localname,"/")) 

					/* if filename is not root directory */
			    StrAllocCat(tmpfilename,"/"); 


			StrAllocCat(tmpfilename,dirbuf->d_name);
			stat(tmpfilename, &file_info);
			if (((file_info.st_mode) & S_IFMT) == S_IFDIR)
		                sprintf((char *)dirname,"D%s",dirbuf->d_name);
			else sprintf((char *)dirname,"F%s",dirbuf->d_name);
			    /* D & F to have first directories, then files */
			HTBTree_add(bt,dirname); /* Sort dirname in the tree bt */
		    }

		    /*    Run through tree printing out in order
		     */
		    {
		        HTBTElement * next_element = HTBTree_next(bt,NULL);
			    /* pick up the first element of the list */
			char state;
			    /* I for initial (.. file),
			       D for directory file,
			       F for file */
			
			state = 'I';

			while (next_element != NULL)
		        {
			    StrAllocCopy(tmpfilename,localname);
			    if (strcmp(localname,"/")) 

					/* if filename is not root directory */
			        StrAllocCat(tmpfilename,"/"); 

			    StrAllocCat(tmpfilename,
					(char *)HTBTree_object(next_element)+1);
			    /* append the current entry's filename to the path */
			    HTSimplify(tmpfilename);
			    /* Output the directory entry */
			    if (strcmp((char *)
					     (HTBTree_object(next_element)),"D.."))
			    {			    
				if (state != *(char *)(HTBTree_object(next_element))) 
				{
				    if (state == 'D')
				        END(HTML_DIR);
				    state = *(char *)
				        (HTBTree_object(next_element))=='D'?'D':'F';
				    START(HTML_H2);
				    PUTS(state == 'D'?"Subdirectories:":"Files");
				    END(HTML_H2);
				    START(HTML_DIR);
				}
			        START(HTML_LI);
			    }
			    HTDirEntry(target, tail,
				       (char*)HTBTree_object(next_element) +1);

			    next_element = HTBTree_next(bt,next_element);
			        /* pick up the next element of the list; 
				 if none, return NULL*/
			}
			if (state == 'I')
			{
			    START(HTML_P);
			    PUTS("Empty Directory");
			}
			else
			    END(HTML_DIR);
		    }

		        /* end while directory entries left to read */
		    closedir(dp);
		    free(logical);
		    free(tmpfilename);
		    HTBTreeAndObject_free(bt);

		    if (HTDirReadme == HT_DIR_README_BOTTOM)
			  do_readme(target, localname);
		    FREE_TARGET;
		    free(localname);
		    return HT_LOADED;	/* document loaded */
		}

	    } /* end if localname is directory */
	
	} /* end if file stat worked */
	
/* End of directory reading section
*/
#endif
open_file:
	{
	    FILE * fp = fopen(localname,"r");
	    if(TRACE) fprintf (stderr, "HTFile: Opening `%s' gives %p\n",
				localname, (void*)fp);
	    if (fp) {		/* Good! */
		if (HTEditable(localname)) {
		    HTAtom * put = HTAtom_for("PUT");
		    HTList * methods = HTAnchor_methods(anchor);
		    if (HTList_indexOf(methods, put) == (-1)) {
			HTList_addObject(methods, put);
		    }
		}
		free(localname);
		HTParseFile(format, format_out, anchor, fp, sink);
		fclose(fp);
		return HT_LOADED;
	    }  /* If succesfull open */
	}    /* scope of fp */
    }  /* local unix file system */    
#endif
#endif

#ifndef DECNET
/*	Now, as transparently mounted access has failed, we try FTP.
*/
    {
	if (strcmp(nodename, HTHostName())!=0)
	    return HTFTPLoad(addr, anchor, format_out, sink);
    }
#endif

/*	All attempts have failed.
*/
    {
    	if (TRACE)
    	printf("Can't open `%s', errno=%d\n", addr, errno);
	return HTLoadError(sink, 403, "Can't access requested file.");
    }
    
 
}

/*		Protocol descriptors
*/
PUBLIC HTProtocol HTFTP  = { "ftp", HTLoadFile, 0 };
PUBLIC HTProtocol HTFile = { "file", HTLoadFile, HTFileSaveStream };
