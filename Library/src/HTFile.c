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
**
** Bugs:
**	FTP: Cannot access VMS files from a unix machine.
**      How can we know that the
**	target machine runs VMS?
*/

#include "HTFile.h"		/* Implemented here */


#define INFINITY 512		/* file name length @@ FIXME */
#define MULTI_SUFFIX ".multi"   /* Extension for scanning formats */
#define MAX_SUFF 15		/* Maximum number of suffixes for a file */

#ifdef VMS
PRIVATE char * suffix_separators = "_,";
#else
PRIVATE char * suffix_separators = ".,";
#endif


#include "HTUtils.h"

#ifdef VMS
#include "HTVMSUtils.h"
#endif /* VMS */

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
	HTAtom *	rep;		/* Content-Type */
	HTAtom *	encoding;	/* Content-Encoding */
	HTAtom *	language;	/* Content-Language */
	float		quality;
} HTSuffix;


typedef struct _HTContentDescription {
    char *	filename;
    HTAtom *	content_type;
    HTAtom *	content_language;
    HTAtom *	content_encoding;
    int		content_length;
    float	quality;
} HTContentDescription;



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
PUBLIC BOOL HTTakeBackup = YES;

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
	suff->encoding = HTAtom_for(encoding);
    }

    suff->quality = value;
}


PRIVATE BOOL is_separator ARGS1(char, ch)
{
    if (strchr(suffix_separators, ch)) return YES;
    else return NO;
}


/* PRIVATE						split_filename()
**
**	Split the filename to an array of suffixes.
**	Return the number of parts placed to the array.
**	Array should have MAX_SUFF+1 items.
*/
PRIVATE int split_filename ARGS2(char *,	s_str,
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
	s_arr[i] = NULL;
	StrAllocCopy(s_arr[i], start);
	*end = save;
	start = end;
    }
    FREE(s_arr[i]);	/* Terminating NULL */
    return i;
}


/* PRIVATE						multi_match()
**
**	Check if actual filename (split in parts) fulfills
**	the requirements.
*/
PRIVATE BOOL multi_match ARGS4(char **, required,
			       int,	m,
			       char **, actual,
			       int,	n)
{
    int c;
    int i,j;

    for(c=0;  c<m && c<n && !strcmp(required[c], actual[c]);  c++);

    if (!c) return NO;		/* Names differ rigth from start */

    for(i=c; i<m; i++) {
	BOOL found = NO;
	for(j=c; j<n; j++) {
	    if (!strcmp(required[i], actual[j])) {
		found = YES;
		break;
	    }
	}
	if (!found) return NO;
    }
    return YES;
}


PRIVATE HTContentDescription * content_description ARGS2(char **, actual,
							 int,	  n)
{
    HTContentDescription * cd;
    int i;

#ifndef NO_INIT    
    if (!HTSuffixes) HTFileInit();
#endif

    cd = (HTContentDescription*)calloc(1, sizeof(HTContentDescription));
    if (!cd) outofmem(__FILE__, "HTContentDescription");

    cd->quality = 1.0;

    for (i=n-1; i>0; i--) {
	HTList * cur = HTSuffixes;
	HTSuffix * suff;
	BOOL found = NO;

	CTRACE(stderr,
	       " ** content_description: LOOKING FOR SUFFIX \"%s\"\n",
	       actual[i]);

	while ((suff = (HTSuffix*)HTList_nextObject(cur))) {
	    if (!strcmp(suff->suffix, actual[i])) {

		CTRACE(stderr, " ** SUFFIX FOUND\n");

		if (!suff->rep && !suff->language && suff->encoding)
		    cd->content_encoding = suff->encoding;
		else if (suff->rep)
		    cd->content_type = suff->rep;
		else if (suff->language)
		    cd->content_language = suff->language;
		else if (suff->encoding)
		    cd->content_encoding = suff->encoding;

		if (suff->quality > 0.000001)
		    cd->quality *= suff->quality;

		if (!cd->content_type)
		    cd->content_type = suff->rep;
		if (!cd->content_encoding)
		    cd->content_encoding = suff->encoding;
		if (!cd->content_language)
		    cd->content_language = suff->language;

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



PUBLIC BOOL HTGetBest ARGS1(HTRequest *, req)
{
    static char * required[MAX_SUFF+1];
    static char * actual[MAX_SUFF+1];
    int m,n;

    char * dirname = NULL;
    char * basename = NULL;
    int baselen;
    char * multi = NULL;
    DIR * dp;
    STRUCT_DIRENT * dirbuf;
    HTList * poss = NULL;

    if (!req || !req->localname || !*req->localname) return NO;

    HTAddLanguage(".fin", "Finnish", 1.0);	/* These are just for @@@@ */
    HTAddLanguage(".en",  "English", 1.0);	/* testing purposes.  @@@@ */
    HTAddLanguage(".fr",  "French" , 1.0);
    HTAddLanguage(".sw",  "Swedish", 1.0);

    StrAllocCopy(dirname, req->localname);
    basename = (strrchr(dirname, '/'));
    if (!basename)
	goto clean_multi;
    *basename++ = 0;

    multi = strrchr(basename, MULTI_SUFFIX[0]);
    if (multi && !strcmp(multi, MULTI_SUFFIX))
	*multi = 0;
    baselen = strlen(basename);

    m = split_filename(basename, required);

    dp = opendir(dirname);
    if (!dp) {
	CTRACE(stderr,"Multi: Can't open directory %s\n", dirname);
	goto clean_multi;
    }

    poss = HTList_new();
    while ((dirbuf = readdir(dp))) {
	if (!dirbuf->d_ino) continue;	/* Not in use */
	if (dirbuf->d_namlen >= baselen) {
	    n = split_filename(dirbuf->d_name, actual);
	    if (multi_match(required, m, actual, n)) {
		HTContentDescription * cd;
		cd = content_description(actual, n);
		if (cd) {
		    if (cd->content_type) {
			cd->quality = HTStackValue(req->conversions,
						   cd->content_type,
						   req->output_format,
						   cd->quality,
						   0.0);
			if (cd->quality > NO_VALUE_FOUND) {
			    StrAllocCopy(cd->filename, dirbuf->d_name);
			    HTList_addObject(poss, (void*)cd);
			    CTRACE(stderr,"HTFile: value of presenting %s is %f\n",
				   cd->content_type ? HTAtom_name(cd->content_type) : "??",
				   cd->quality);
			}
			else free(cd);
		    }
		    else free(cd);
		}
		else CTRACE(stderr, "HTGetPoss: couldn't get file description\n");
	    }
	    else CTRACE(stderr, "HTGetPoss: didn't match \"%s\"\n", basename);
	}
    }
    closedir(dp);

    {	/* BEGIN DEBUG */
	HTList * cur = poss;
	HTContentDescription * cd;

	CTRACE(stderr, "POSSIBILITIES IN DIRECTORY \"%s\"\n", dirname);
	CTRACE(stderr, "FOR \"%s\":\n", basename);
	CTRACE(stderr,
	       "%sContent-Type  Content-Language  Content-Encoding  Quality  File%s",
	       "\n---------------------------------------------------------------\n",
	       "\n---------------------------------------------------------------\n");
	while ((cd = (HTContentDescription*)HTList_nextObject(cur))) {
	    CTRACE(stderr, "%s\t%s\t\t%s\t\t  %.5f  %s\n",
		   cd->content_type    ?HTAtom_name(cd->content_type)  :"-\t",
		   cd->content_language?HTAtom_name(cd->content_language):"-",
		   cd->content_encoding?HTAtom_name(cd->content_encoding):"-",
		   cd->quality,
		   cd->filename        ?cd->filename                     :"-");
	}
	CTRACE(stderr,
	       "---------------------------------------------------------------\n");
    }	/* END DEBUG */

    {	/* Select best */
	HTList * cur = poss;
	HTContentDescription * cd;
	HTContentDescription * best = NULL;
	float best_value = -1;

	/*
	** This algorithm should be enhanced -- now it's very stupid.
	*/
	while ((cd = (HTContentDescription*)HTList_nextObject(cur))) {
	    if (cd->quality > best_value) {
		if (best) {
		    CTRACE(stderr, " ## \"%s\" (%f) is no longer best\n",
			   best->filename, best_value);
		    if (best->filename) free(best->filename);
		    free(best);
		}
		CTRACE(stderr, " ## \"%s\" (%f) is now best\n",
		       cd->filename, cd->quality);
		best_value = cd->quality;
		best = cd;
	    }
	    else {
		CTRACE(stderr, " ## \"%s\" (%f) causes no change\n",
		       cd->filename, cd->quality);
		if (cd->filename) free(cd->filename);
		free(cd);
	    }
	}
	if (best) {
	    StrAllocCopy(req->localname, dirname);
	    StrAllocCat(req->localname, "/");
	    StrAllocCat(req->localname, best->filename);
	    if (best->filename) free(best->filename);
	    free(best);
	    return YES;
	}
    }	/* Select best */

  clean_multi:
    free(dirname);
    return NO;
}


PRIVATE char * weekday[7] =
{
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

PRIVATE char * month[12] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

PUBLIC void HTSetAttributes ARGS2(HTRequest *,		req,
				  struct stat *,	stat_info)
{
    struct tm * gmt;

    if (!req || !stat_info) return;

    req->content_length = stat_info->st_size;
#ifndef VMS
    gmt = gmtime(&stat_info->st_mtime);
    req->last_modified = (char*)malloc(40);
    sprintf(req->last_modified, "%s, %02d-%s-%02d %02d:%02d:%02d GMT",
	    weekday[gmt->tm_wday],
	    gmt->tm_mday,
	    month[gmt->tm_mon],
	    gmt->tm_year % 100,
	    gmt->tm_hour,
	    gmt->tm_min,
	    gmt->tm_sec);
#endif
}


PUBLIC BOOL HTGetAttributes ARGS1(HTRequest *, req)
{
    struct stat stat_info;

    if (!req || !req->localname || stat(req->localname, &stat_info) == -1) {
	if (!req)
	    CTRACE(stderr, "HTGetAttributes bug: called with req==NULL\n");
	else if (!req->localname)
	    CTRACE(stderr, "HTGetAttributes bug: req->localname==NULL\n");
	else CTRACE(stderr,
		    "HTGetAttributes: stat(\"%s\") failed (errno %d)\n",
		    req->localname, errno);
	return NO;
    }
    HTSetAttributes(req, &stat_info);
    return YES;
}




PUBLIC BOOL HTMulti ARGS1(HTRequest *, req)
{
    char * multi;
    struct stat stat_info;
    int stat_status = -1;

    if (!req) {
	CTRACE(stderr, "HTMulti bug: called with req==NULL\n");
	return NO;
    }
    else if (!req->localname) {
	CTRACE(stderr, "HTGetAttributes bug: req->localname==NULL\n");
	return NO;
    }

#ifdef GOT_READ_DIR
    multi = strrchr(req->localname, MULTI_SUFFIX[0]);
    if (multi && !strcmp(multi, MULTI_SUFFIX)) {
	CTRACE(stderr,
	       "Filename ends in .multi, doing multiformat handling\n");
	if (!HTGetBest(req)) {
	    CTRACE(stderr, "Multi failed -- giving up\n");
	    return NO;
	}
    }
    else {
	stat_status = stat(req->localname, &stat_info);
	if (stat_status == -1) {
	    CTRACE(stderr,
		   "Couldn't stat \"%s\" (errno %d) - doing automatic multi\n",
		   req->localname, errno);
	    if (!HTGetBest(req)) {
		CTRACE(stderr, "Automatic multi failed -- giving up\n");
		return NO;
	    }
	}
    }
#endif /* GOT_READ_DIR */

    if (stat_status == -1)
	stat_status = stat(req->localname, &stat_info);
    if (stat_status == -1) {
	CTRACE(stderr, "Couldn't stat \"%s\" -- givin up (errno %d)\n",
	       req->localname, errno);
	return NO;
    }
    else {
	HTSetAttributes(req, &stat_info);
	return YES;
    }
}






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

    if (0==strcmp(access, "file")) { /* local file */
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

PUBLIC HTFormat HTFileFormat ARGS2 (
			CONST char *,	filename,
			HTAtom **,	pencoding)

{
    HTSuffix * suff;
    HTList * cur;
    int lf = strlen(filename);

#ifndef NO_INIT    
    if (!HTSuffixes) HTFileInit();
#endif
    if (pencoding) *pencoding = NULL;
    cur = HTSuffixes;
    while ((suff = (HTSuffix*)HTList_nextObject(cur))) {
        int ls = strlen(suff->suffix);
	if ((ls <= lf) && 0==strcmp(suff->suffix, filename + lf - ls)) {
	    if (pencoding) *pencoding = suff->encoding;
	    if (suff->rep) return suff->rep;		/* OK -- found */

	    /* Got encoding, need representation */
	    cur = HTSuffixes;
	    while ((suff = (HTSuffix*)HTList_nextObject(cur))) {
		int ls2 = strlen(suff->suffix);
		if ((ls <= lf) &&
		    0==strncmp(suff->suffix, filename + lf - ls -ls2, ls2)) {
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
    if (pencoding && !*pencoding) *pencoding = suff->encoding ? suff->encoding
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
    HTList * cur;
    int lf = strlen(filename);

#ifndef NO_INIT    
    if (!HTSuffixes) HTFileInit();
#endif
    cur = HTSuffixes;
    while ((suff = (HTSuffix*)HTList_nextObject(cur))) {
        int ls = strlen(suff->suffix);
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

#ifdef VMS
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
	
	if ((fp=fopen(filename, "r"))) {		/* File exists */
	    fclose(fp);
	    if (TRACE) printf("File `%s' exists\n", filename);
	    if (remove(backup_filename)) {
		if (TRACE) printf("Backup file `%s' removed\n",
			 backup_filename);
	    }
	    if (rename(filename, backup_filename)) {	/* != 0 => Failure */
		if (TRACE) printf("Rename `%s' to `%s' FAILED!\n",
				    filename, backup_filename);
	    } else {					/* Success */
		if (TRACE) printf("Renamed `%s' to `%s'\n", filename,
				backup_filename);
	    }
	}
    	free(backup_filename);
    } /* if take backup */    
    
    {
        fp = fopen(filename, "w");
        if (!fp) return NULL;
    
    	return HTFWriter_new(fp);
    }
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
PUBLIC int HTLoadFile ARGS1 (HTRequest *, request)

{
    CONST char * addr = HTAnchor_physical(request->anchor);
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


#ifdef VMS
/* Assume that the file is in Unix-style syntax if it contains a '/'
   after the leading one @@ */
    {
        FILE * fp;
	char * vmsname = strchr(filename + 1, '/') ?
	  HTVMS_name(nodename, filename) : filename + 1;
	fp = fopen(vmsname, "r", "shr=put", "shr=upd");
	
/*	If the file wasn't VMS syntax, then perhaps it is ultrix
*/
	if (!fp) {
	    char ultrixname[INFINITY];
	    if (TRACE) fprintf(stderr, "HTFile: Can't open as %s\n", vmsname);
	    sprintf(ultrixname, "%s::\"%s\"", nodename, filename);
  	    fp = fopen(ultrixname, "r", "shr=put", "shr=upd");
	    if (!fp) {
		if (TRACE) fprintf(stderr, 
				   "HTFile: Can't open as %s\n", ultrixname);
	    }
	}
        if (fp)
        {
	    if (HTEditable(vmsname)) {
		HTAtom * put = HTAtom_for("PUT");
		HTList * methods = HTAnchor_methods(request->anchor);
		if (HTList_indexOf(methods, put) == (-1)) {
	   	    HTList_addObject(methods, put);
	        }
	    }
	    HTParseFile(format, request->output_format, request->anchor, fp, request->output_stream);
	    fclose(fp);
            return HT_LOADED;
        }  /* If successfull open */
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
	struct stat dir_info;
	char * multi;

	if (!request->localname)
	    request->localname = HTLocalName(addr);


#ifdef GOT_READ_DIR

/*			  Multiformat handling
**
**	If needed, scan directory to find a good file.
**  Bug:  we don't stat the file to find the length
*/
	multi = strrchr(request->localname, MULTI_SUFFIX[0]);
	if (multi && !strcmp(multi, MULTI_SUFFIX)) {
	    if (HTGetBest(request)) {
		format = HTFileFormat(request->localname, &encoding);
		goto open_file;
	    }
	    else { 			/* If not found suitable file */
		FREE(request->localname);
		return HTLoadError(request, 403,	/* List formats? */
		   "Could not find suitable representation for transmission.");
	    }
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
	
	
	if (stat(request->localname,&dir_info) == -1) {     /* get file information */
	                               /* if can't read file information */
	    if (TRACE) fprintf(stderr, "HTFile: can't stat %s\n", request->localname);

	}  else {		/* Stat was OK */
		

	    if (((dir_info.st_mode) & S_IFMT) == S_IFDIR) {
		/* if request->localname is a directory */	

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
		    fprintf(stderr,"%s is a directory\n",request->localname);
			
/*	Check directory access.
**	Selective access means only those directories containing a
**	marker file can be browsed
*/
		if (HTDirAccess == HT_DIR_FORBID) {
		    FREE(request->localname);
		    return HTLoadError(request, 403,
		    "Directory browsing is not allowed.");
		}


		if (HTDirAccess == HT_DIR_SELECTIVE) {
		    char * enable_file_name = 
			malloc(strlen(request->localname)+ 1 +
			 strlen(HT_DIR_ENABLE_FILE) + 1);
		    strcpy(enable_file_name, request->localname);
		    strcat(enable_file_name, "/");
		    strcat(enable_file_name, HT_DIR_ENABLE_FILE);
		    if (stat(enable_file_name, &file_info) != 0) {
			FREE(request->localname);
			return HTLoadError(request, 403,
			"Selective access is not enabled for this directory");
		    }
		}

 
		dp = opendir(request->localname);
		if (!dp) {
		    FREE(request->localname);
		    return HTLoadError(request, 403,
				       "This directory is not readable.");
		}


 /*	Directory access is allowed and possible
 */
		logical = HTAnchor_address((HTAnchor*)request->anchor);
		tail = strrchr(logical, '/') +1;	/* last part or "" */

		/*
		** Fix AL 26.1.94: make dir.indexing work also if
		** there is a trailing slash:
		*/
		if (!*tail) tail = ".";

		target = HTML_new(request, NULL, WWW_HTML, 
			request->output_format, request->output_stream);
		targetClass = *target->isa;	/* Copy routine entry points */
		    
  		{ int i;
			for(i=0; i<HTML_A_ATTRIBUTES; i++)
				present[i] = (i==HTML_A_HREF);
		}
		
                HTDirTitles(target, (HTAnchor *)request->anchor);

                if (HTDirReadme == HT_DIR_README_TOP)
		    do_readme(target, request->localname);
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
			StrAllocCopy(tmpfilename,request->localname);
			if (strcmp(request->localname,"/")) 

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
			    StrAllocCopy(tmpfilename,request->localname);
			    if (strcmp(request->localname,"/")) 

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
			  do_readme(target, request->localname);
		    FREE_TARGET;
		    FREE(request->localname);
		    return HT_LOADED;	/* document loaded */
		}

	    } /* end if request->localname is directory */
	
	} /* end if file stat worked */
	
/* End of directory reading section
*/
#endif
open_file:
	{
	    FILE * fp = fopen(request->localname,"r");

	    if(TRACE) fprintf (stderr, "HTFile: Opening `%s' gives %p\n",
				request->localname, (void*)fp);
	    if (fp) {		/* Good! */
		if (HTEditable(request->localname)) {
		    HTAtom * put = HTAtom_for("PUT");
		    HTList * methods = HTAnchor_methods(request->anchor);
		    if (HTList_indexOf(methods, put) == (-1)) {
			HTList_addObject(methods, put);
		    }
		}
		FREE(request->localname);
		HTParseFile(format, fp, request);
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
	    return HTFTPLoad(request, NULL, addr,
	    request->anchor, request->output_format, request->output_stream);
    }
#endif

/*	All attempts have failed.
*/
    {
    	if (TRACE)
	    printf("Can't open `%s', errno=%d\n", addr, errno);

	return HTLoadError(request, 403, "Can't access requested file.");
    }
    
 
}

/*		Protocol descriptors
*/
GLOBALDEF PUBLIC HTProtocol HTFTP  = { "ftp", HTLoadFile, 0 , 0 };
GLOBALDEF PUBLIC HTProtocol HTFile = { "file", HTLoadFile,
					HTFileSaveStream, 0 };
