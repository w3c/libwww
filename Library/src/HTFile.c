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
**	   Sep 93 (MD)  Access to VMS files allows sharing.
**	15 Nov 93 (MD)	Moved HTVMSname to HTVMSUTILS.C
**	22 Feb 94 (MD)  Excluded two routines if we are not READING directories
**	18 May 94 (HF)	Directory stuff removed and stream handling updated,
**			error messages introduced etc.
**
** Bugs:
**	FTP: Cannot access VMS files from a unix machine.
**      How can we know that the
**	target machine runs VMS?
*/

#ifdef VMS
#define GOT_READ_DIR
#endif /* VMS */

#ifdef VMS
#include "HTVMSUtils.h"
#endif /* VMS */

/* System dependent stuff */
#include "tcp.h"

/* Library Includes */
#include "HTUtils.h"
#include "HTParse.h"
#include "HTTCP.h"
#ifndef DECNET			/* Temporary ? */
#include "HTFTP.h"
#endif
#include "HTAnchor.h"
#include "HTAtom.h"
#include "HTWriter.h"
#include "HTFWriter.h"
#include "HTInit.h"
#include "HTBTree.h"
#include "HTFormat.h"
#include "HTMulti.h"
#include "HTError.h"
#include "HTFile.h"		/* Implemented here */

#ifdef VMS
PRIVATE char * suffix_separators = "._";
#else
PRIVATE char * suffix_separators = ".,_";
#endif /* VMS */

#ifdef ISC3	/* Lauren */		
#define _POSIX_SOURCE       
#include <sys/limits.h>		
#endif

typedef struct _HTSuffix {
	char *		suffix;
	HTAtom *	rep;		/* Content-Type */
	HTAtom *	encoding;	/* Content-Encoding */
	HTAtom *	language;	/* Content-Language */
	float		quality;
} HTSuffix;


/*                   Controlling globals
**
*/

PUBLIC BOOL HTTakeBackup = YES;
PUBLIC BOOL HTSuffixCaseSense = NO;	/* Are suffixes case sensitive */

PRIVATE char *HTMountRoot = "/Net/";		/* Where to find mounts */
#ifdef VMS
PRIVATE char *HTCacheRoot = "/WWW$SCRATCH";   /* Where to cache things */
#else
PRIVATE char *HTCacheRoot = "/tmp/W3_Cache_";   /* Where to cache things */
#endif

/* PRIVATE char *HTSaveRoot  = "$(HOME)/WWW/";*/    /* Where to save things */


/*	Suffix registration
*/

PRIVATE HTList * HTSuffixes = 0;
PRIVATE HTSuffix no_suffix = { "*", NULL, NULL, NULL, 1.0 };
PRIVATE HTSuffix unknown_suffix = { "*.*", NULL, NULL, NULL, 1.0};

/* ------------------------------------------------------------------------- */

/*	Define the representation associated with a file suffix
**	-------------------------------------------------------
**
**	Calling this with suffix set to "*" will set the default
**	representation.
**	Calling this with suffix set to "*.*" will set the default
**	representation for unknown suffix files which contain a ".".
**
**	If filename suffix is already defined its previous
**	definition is overridden.
*/
PUBLIC void HTAddType ARGS4(CONST char *,	suffix,
			    CONST char *,	representation,
			    CONST char *,	encoding,
			    float,		value)
{
    HTSetSuffix(suffix, representation, encoding, NULL, value);
}


PUBLIC void HTAddEncoding ARGS3(CONST char *,	suffix,
				CONST char *,	encoding,
				float,		value)
{
    HTSetSuffix(suffix, NULL, encoding, NULL, value);
}


PUBLIC void HTAddLanguage ARGS3(CONST char *,	suffix,
				CONST char *,	language,
				float,		value)
{
    HTSetSuffix(suffix, NULL, NULL, language, value);
}


PUBLIC void HTSetSuffix ARGS5(CONST char *,	suffix,
			      CONST char *,	representation,
			      CONST char *,	encoding,
			      CONST char *,	language,
			      float,		value)
{
    HTSuffix * suff;

    if (strcmp(suffix, "*")==0) suff = &no_suffix;
    else if (strcmp(suffix, "*.*")==0) suff = &unknown_suffix;
    else {
	HTList *cur = HTSuffixes;

	while (NULL != (suff = (HTSuffix*)HTList_nextObject(cur))) {
	    if (suff->suffix && 0==strcmp(suff->suffix, suffix))
		break;
	}
	if (!suff) { /* Not found -- create a new node */
	    suff = (HTSuffix*) calloc(1, sizeof(HTSuffix));
	    if (suff == NULL) outofmem(__FILE__, "HTSetSuffix");

	    if (!HTSuffixes) HTSuffixes = HTList_new();
	    HTList_addObject(HTSuffixes, suff);

	    StrAllocCopy(suff->suffix, suffix);
	}
    }

    if (representation)
	suff->rep = HTAtom_for(representation);
    if (language)
	suff->language = HTAtom_for(language);
    if (encoding) {
	char * enc = NULL;
	char * p;
	StrAllocCopy(enc, encoding);
	for (p=enc; *p; p++) *p = TOLOWER(*p);
	suff->encoding = HTAtom_for(enc);
	free(enc);	/* Leak fixed AL 6 Feb 1994 */
    }

    suff->quality = value;
}


PRIVATE BOOL is_separator ARGS1(char, ch)
{
    if (strchr(suffix_separators, ch)) return YES;
    else return NO;
}


/* PUBLIC						HTSplitFilename()
**
**	Split the filename to an array of suffixes.
**	Return the number of parts placed to the array.
**	Array should have MAX_SUFF+1 items.
*/
PUBLIC int HTSplitFilename ARGS2(char *,	s_str,
				 char **,	s_arr)
{
    char * start = s_str;
    char * end;
    char save;
    int i;

    if (!s_str || !s_arr) return 0;

    for (i=0; i < MAX_SUFF && *start; i++) {
	for(end=start+1; *end && !is_separator(*end); end++);
	save = *end;
	*end = 0;
	StrAllocCopy(s_arr[i], start);	/* Frees the previous value */
	*end = save;
	start = end;
    }
    FREE(s_arr[i]);	/* Terminating NULL */
    return i;
}



PUBLIC HTContentDescription * HTGetContentDescription ARGS2(char **,	actual,
							    int,	n)
{
    HTContentDescription * cd;
    int i;

#ifndef NO_INIT    
    if (!HTSuffixes) HTFileInit();
#endif

    if (n < 2) return NULL;	/* No suffix */

    cd = (HTContentDescription*)calloc(1, sizeof(HTContentDescription));
    if (!cd) outofmem(__FILE__, "HTContentDescription");

    cd->quality = 1.0;

    for (i=n-1; i>0; i--) {
	HTList * cur = HTSuffixes;
	HTSuffix * suff;
	BOOL found = NO;

	CTRACE(stderr, "Searching... for suffix %d: \"%s\"\n", i, actual[i]);

	while ((suff = (HTSuffix*)HTList_nextObject(cur))) {
	    if ((HTSuffixCaseSense && !strcmp(suff->suffix, actual[i])) ||
		(!HTSuffixCaseSense && !strcasecomp(suff->suffix, actual[i]))){

		if (!cd->content_type)
		    cd->content_type = suff->rep;
		if (!cd->content_encoding)
		    cd->content_encoding = suff->encoding;
		if (!cd->content_language)
		    cd->content_language = suff->language;
		if (suff->quality > 0.0000001)
		    cd->quality *= suff->quality;

		found = YES;
		break;
	    }
	}
	if (!found) {
	    if (i < n-1)
		break;
	    else {
		free(cd);
		return NULL;
	    }
	}
    }
    return cd;
}


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
**
**	BUG: FILENAME IS NOT UNESCAPED!!!!!!
**
*/
PUBLIC char * HTLocalName ARGS1(CONST char *,name)
{
    char * access = HTParse(name, "", PARSE_ACCESS);
    char * host = HTParse(name, "", PARSE_HOST);
    char * path = HTParse(name, "", PARSE_PATH+PARSE_PUNCTUATION);
    
    HTUnEscape(path);	/* Interpret % signs */

    /*
     * CERN httpd has it's own enhanced rule system which has already
     * done the mappings, and the local filename ("physical address")
     * is stored in HTImServer.
     */
    if (HTImServer && !*access) {
	free(access);
	free(host);
	StrAllocCopy(path, HTImServer);
	HTUnEscape(path);

#if VMS
        HTVMS_checkDecnet(path);
#endif /* VMS */

	CTRACE(stderr, "Local filename is \"%s\"\n", path);
	return(path);
    }

    if (0==strcmp(access, "file")) { /* local file */
        free(access);	
	if ((0==strcasecomp(host, HTGetHostName())) ||
	    (0==strcasecomp(host, "localhost")) || !*host) {
	    free(host);

#if VMS
        HTVMS_checkDecnet(path);
#endif /* VMS */

	    if (TRACE) fprintf(stderr, "Node........ `%s' means path `%s'\n", name, path);
	    return(path);
	} else {
	    char * result = (char *)malloc(
	    			strlen("/Net/")+strlen(host)+strlen(path)+1);
              if (result == NULL) outofmem(__FILE__, "HTLocalName");
	    sprintf(result, "%s%s%s", "/Net/", host, path);
	    free(host);
	    free(path);
	    if (TRACE) fprintf(stderr, "Node........ `%s' means file `%s'\n", name, result);
	    return result;
	}
    } else {  /* other access */
	char * result;
        CONST char * home =  (CONST char*)getenv("HOME");
#ifdef VMS
	if (!home) 
	    home = HTCacheRoot; 
	else
   	    home = HTVMS_wwwName(home);
#else /* not VMS */
	if (!home) home = "/tmp"; 
#endif /* not VMS */
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
        result = (char *)malloc(7+strlen(HTGetHostName())+strlen(name)+1);
	if (result == NULL) outofmem(__FILE__, "WWW_nameOfFile");
	sprintf(result, "file://%s%s", HTGetHostName(), name);
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
    HTList * cur;

#ifndef NO_INIT    
    if (!HTSuffixes) HTFileInit();
#endif
    cur = HTSuffixes;
    while ((suff = (HTSuffix*)HTList_nextObject(cur))) {
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

PUBLIC HTFormat HTFileFormat ARGS3(CONST char *,	filename,
				   HTAtom **,		pencoding,
				   HTAtom **,		planguage)
{
    char * actual[MAX_SUFF+1];
    int n;
    HTContentDescription * cd;
    HTFormat content_type = NULL;

    if (!filename) return WWW_BINARY;

#ifndef NO_INIT    
    if (!HTSuffixes) HTFileInit();
#endif

    for (n=0; n<MAX_SUFF+1; n++)  actual[n] = NULL;
    n = HTSplitFilename((char*)filename, actual);
    cd = HTGetContentDescription(actual, n);
    while(n-- > 0) if (actual[n]) free(actual[n]);

    if (cd) {
	if (pencoding) *pencoding = cd->content_encoding;
	if (planguage) *planguage = cd->content_language;
	content_type = cd->content_type;
	free(cd);
    }
    else {
	HTSuffix * suff = strchr(filename,'.') ?
	    (unknown_suffix.rep ? &unknown_suffix : &no_suffix) :
		&no_suffix;

	if (pencoding) *pencoding = suff->encoding;
	if (planguage) *planguage = suff->language;
	content_type = suff->rep;
    }

    if (pencoding && !*pencoding)
	*pencoding = HTAtom_for("binary");
    return content_type ? content_type : WWW_BINARY;
}


/*	Determine value from file name
**	------------------------------
**
*/

PUBLIC float HTFileValue ARGS1 (CONST char *,filename)

{
    HTSuffix * suff;
    HTList * cur;
    int lf = strlen(filename);

#ifndef NO_INIT    
    if (!HTSuffixes) HTFileInit();
#endif
    cur = HTSuffixes;
    while ((suff = (HTSuffix*)HTList_nextObject(cur))) {
        int ls = strlen(suff->suffix);
	if ((ls <= lf) && 0==strcmp(suff->suffix, filename + lf - ls)) {
	    if (TRACE) fprintf(stderr, "File Value.. Value of %s is %.3f\n",
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
#ifndef NO_UNIX_IO
PUBLIC BOOL HTEditable ARGS1 (CONST char *,filename)
{
#ifdef NO_GROUPS
    return NO;		/* Safe answer till we find the correct algorithm */
#else
#ifdef NeXT
    int		groups[NGROUPS];
#else
    gid_t 	groups[NGROUPS];
#endif	
    uid_t	myUid;
    int		ngroups;			/* The number of groups  */
    struct stat	fileStatus;
    int		i;
        
    if (HTStat(filename, &fileStatus))		/* Get details of filename */
    	return NO;				/* Can't even access file! */

    ngroups = getgroups(NGROUPS, groups);	/* Groups to which I belong  */
    myUid = geteuid();				/* Get my user identifier */

    if (TRACE) {
        int i;
	fprintf(stderr, 
	    "File mode is 0%o, uid=%d, gid=%d. My uid=%d, %d groups (",
    	    (unsigned int) fileStatus.st_mode, (int) fileStatus.st_uid,
	    (int) fileStatus.st_gid, (int) myUid, ngroups);
	for (i=0; i<ngroups; i++) fprintf(stderr, " %d", (int) groups[i]);
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
#endif /* NO_UNIX_IO */

/*	Make a save stream
**	------------------
**
**	The stream must be used for writing back the file.
**	@@@ no backup done
*/
PUBLIC HTStream * HTFileSaveStream ARGS1(HTRequest *, request)
{

    CONST char * addr = HTAnchor_address((HTAnchor*)request->anchor);
    char *  filename = HTLocalName(addr);
    FILE* fp;

/*	@ Introduce CVS handling here one day
*/
/*	Take a backup before we do anything silly   931205
*/        
    if (HTTakeBackup) {
    	char * backup_filename = 0;
	char * p = backup_filename;
	backup_filename = malloc(strlen(filename)+2);
	if (!backup_filename) outofmem(__FILE__, "taking backup");
	strcpy(backup_filename, filename);
	for(p=backup_filename+strlen(backup_filename);; p--) {
	    if ((*p=='/') || (p<backup_filename)) {
	        p[1]=',';		/* Insert comma after slash */
		break;
	    }
	    p[1] = p[0];	/* Move up everything to the right of it */
	}
	

#ifdef VMS
	if ((fp=fopen(filename, "r", "shr=put", "shr=upd"))) {	/* File exists */
#else /* not VMS */
	if ((fp=fopen(filename, "r"))) {		/* File exists */
#endif /* not VMS */
	    fclose(fp);
	    if (TRACE) fprintf(stderr, "File `%s' exists\n", filename);
	    if (remove(backup_filename)) {
		if (TRACE) fprintf(stderr, "Backup file `%s' removed\n",
				   backup_filename);
	    }
	    if (rename(filename, backup_filename)) {	/* != 0 => Failure */
		if (TRACE) fprintf(stderr, "Rename `%s' to `%s' FAILED!\n",
				   filename, backup_filename);
	    } else {					/* Success */
		if (TRACE) fprintf(stderr, "Renamed `%s' to `%s'\n", filename,
				   backup_filename);
	    }
	}
    	free(backup_filename);
    } /* if take backup */    
    
    if ((fp = fopen(filename, "w")) == NULL) {
	HTErrorSysAdd(request, ERR_FATAL, NO, "fopen");
	return NULL;
    } else
    	return HTFWriter_new(fp, NO);
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
PUBLIC int HTLoadFile ARGS1 (HTRequest *, request)
{
    int status = -1;
    char *url;

    if (!request || !request->anchor) {
	if (TRACE) fprintf(stderr, "HTLoadFile.. Called with bad argument\n");
	return HT_INTERNAL;
    }
    url = HTAnchor_physical(request->anchor);
    if (TRACE) fprintf(stderr, "LoadFile.... Looking for `%s\'\n", url);


/*	For unix, we try to translate the name into the name of a transparently
**	mounted file.
**
**	Not allowed in secure (HTClientHost) situations TBL 921019
*/
#ifndef NO_UNIX_IO
    /*  Need protection here for telnet server but not httpd server */
	 
    if (!HTSecure) {
	char * localname = HTLocalName(url);		   /* Does unescape! */
	struct stat stat_info;
	char * multi;
	HTFormat format = HTFileFormat(localname, &request->content_encoding,
				       &request->content_language);
	if (TRACE) fprintf(stderr, "HTLoadFile.. Accessing local file system.\n");

#ifdef GOT_READ_DIR

/*	Multiformat handling. If suffix matches MULTI_SUFFIX then scan
**	directory to find a good file.
**
**	Bug:  we don't stat the file to find the length
*/
	multi = strrchr(localname, MULTI_SUFFIX[0]);
	if (multi && !strcmp(multi, MULTI_SUFFIX)) {
	    char * new_path = HTMulti(request, localname, &stat_info);
	    if (new_path) {
		FREE(localname);
		localname = new_path;
		HTAnchor_setPhysical(request->anchor, localname);
		goto open_file;
	    }
	    else {			       /* If not found suitable file */
		FREE(localname);
		goto cleanup;
	    }
	}

/*	Check to see if the 'localname' is in fact a directory.  If it is
**	create a new hypertext object containing a list of files and 
**	subdirectories contained in the directory.  All of these are links
**      to the directories or files listed.
*/
	if (HTStat(localname, &stat_info) == -1) {
	    if (TRACE)
		fprintf(stderr, "HTLoadFile.. Can't stat %s\n", localname);
	} else {
	    if (((stat_info.st_mode) & S_IFMT) == S_IFDIR) {
		status = HTBrowseDirectory(request, localname);
		FREE(localname);
		goto cleanup;
	    }
	}

#endif /* GOT_READ_DIR */

open_file:
	{
#ifdef VMS
	    FILE * fp = fopen(localname,"r","shr=put","shr=upd");
#else /* not VMS */
	    FILE * fp = fopen(localname,"r");
#endif /* not VMS */

	    if (fp) {
		if(TRACE) fprintf (stderr, "HTLoadFile.. Opened `%s' on local file system\n", localname);
		if (HTEditable(localname)) {
		    HTAtom * put = HTAtom_for("PUT");
		    HTList * methods = HTAnchor_methods(request->anchor);
		    if (HTList_indexOf(methods, put) == (-1)) {
			HTList_addObject(methods, put);
		    }
		}
		status = HTParseFile(format, fp, request);
		fclose(fp);

		FREE(localname);
		goto cleanup;
	    } else
		HTErrorSysAdd(request, ERR_FATAL, NO, "fopen");
	}
	FREE(localname);
    } /* End of local file system */
#endif /* NO_UNIX_IO */

#ifndef DECNET
    /* Now, as transparently mounted access has failed, we try FTP. */
    {
	char *nodename = HTParse(url, "", PARSE_HOST);
	if (nodename && *nodename && strcmp(nodename, HTGetHostName())!=0) {
	    char * newname = NULL;
	    char *unescaped = NULL;
	    StrAllocCopy(unescaped, url);
	    HTUnEscape(unescaped);
	    HTErrorAdd(request, ERR_FATAL, NO, HTERR_FILE_TO_FTP,
		       (void *) unescaped,
		       (int) strlen(unescaped), "HTLoadFile");
	    StrAllocCopy(newname, "ftp:");
	    if (!strncmp(url, "file:", 5))
		StrAllocCat(newname, url+5);
	    else
		StrAllocCat(newname, url);
	    HTAnchor_setPhysical(request->anchor, newname);
	    free(newname);
	    free(nodename);
	    free(unescaped);
	    return HTLoad(request, YES);      /* Jump directly to FTP module */
	}
	free(nodename);
    }
#endif

  cleanup:
#if 0
    if (status < 0 && status != HT_INTERRUPTED) {
        char *unescaped = NULL;
        StrAllocCopy(unescaped, url);
        HTUnEscape(unescaped);
        HTErrorAdd(request, ERR_FATAL, NO, HTERR_NOT_FOUND, (void *) unescaped,
                   (int) strlen(unescaped), "HTLoadFile");
        free(unescaped);
    }
#endif
    return status;
}

/*		Protocol descriptors
*/
GLOBALDEF PUBLIC HTProtocol HTFile = { "file", HTLoadFile,
					HTFileSaveStream, 0 };
