/*								     HTBind.c
**	FILE SUFFIX BIND MANAGER
**
**	(c) COPYRIGHT MIT 1995
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This module sets up the binding between a file Bind and a media
**	type, language, encoding etc. In a client application the Binds
**	are used in protocols that does not support media types etc., like
**	FTP, and in server applications they are used to make the bindings
**	between the server and the local file store that the server can
**	serve to the rest of the world (well almost). The HTFormat module
**	holds this information against the accept headers received in a
**	request and uses if for format negotiation. All the binding management
**	can all be replace by a database interface. 
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
**	10 Maj 95 HF	Spawned off from HTFile in order to make it easier to
**			override by a new module. It's now based on anchors
**			and hash tables
** Bugs:
*/

/* Library Includes */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTAnchor.h"
#include "HTAtom.h"
#include "HTParse.h"

#if 0
#include "HTTCP.h"
#include "HTWriter.h"
#include "HTFWrite.h"
#include "HTInit.h"
#include "HTBTree.h"
#include "HTFormat.h"
#include "HTMulti.h"
#include "HTError.h"
#endif

#ifdef VMS
#include "HTVMSUtils.h"
#endif

#include "HTBind.h"					 /* Implemented here */

typedef struct _HTBind {
    char *	suffix;
    HTFormat	type;			/* Content-Type */
    HTEncoding	encoding;		/* Content-Encoding */
    HTLanguage	language;		/* Content-Language */
    double	quality;
} HTBind;

#define HASH_SIZE	101	   /* Arbitrary prime. Memory/speed tradeoff */

/* Suffix registration */
PRIVATE BOOL HTCaseSen = YES;		      /* Are suffixes case sensitive */
PRIVATE char *HTDelimiters = NULL;			  /* Set of suffixes */

PRIVATE HTList **HTBindings = NULL;   /* Point to table of lists of bindings */

PRIVATE HTBind no_suffix = { "*", NULL, NULL, NULL, 1.0 };
PRIVATE HTBind unknown_suffix = { "*.*", NULL, NULL, NULL, 1.0};

/* ------------------------------------------------------------------------- */

/*	
**	Set up the list of suffix bindings. Done by HTLibInit
*/
PUBLIC BOOL HTBind_init NOARGS
{
    if (!HTBindings)
	HTBindings = (HTList**) calloc(HASH_SIZE, sizeof(HTList *));
    StrAllocCopy(HTDelimiters, DEFAULT_SUFFIXES);
    return YES;
}


/*
**	Cleans up the memory allocated by file bindings
**	Done by HTLibTerminate().
**	Written by Eric Sink, eric@spyglass.com, and Henrik
*/
PUBLIC BOOL HTBind_deleteAll NOARGS
{
    int cnt;
    HTList *cur;
    if (!HTBindings)
	return NO;
    for (cnt=0; cnt<HASH_SIZE; cnt++) {
	if ((cur = HTBindings[cnt])) { 
	    HTBind *pres;
	    while ((pres = (HTBind *) HTList_nextObject(cur)) != NULL) {
		FREE(pres->suffix);
		free(pres);
	    }
	}
	HTList_delete(HTBindings[cnt]);
	HTBindings[cnt] = NULL;
    }
    FREE(HTDelimiters);
    return YES;
}


/*	Make suffix bindings case sensitive
**	-----------------------------------
*/
PUBLIC void HTBind_setCaseSensitive ARGS1(BOOL, sensitive)
{
    HTCaseSen = sensitive;
}


/*	Get set of suffixes
**	-------------------
*/
PUBLIC CONST char *HTBind_getDelimiters NOARGS
{
    return HTDelimiters;
}


/*	Change set of suffixes
**	----------------------
*/
PUBLIC void HTBind_setDelimiters ARGS1(CONST char *, new_suffixes)
{
    if (new_suffixes && *new_suffixes)
	StrAllocCopy(HTDelimiters, new_suffixes);
}


/*	Define the representation associated with a file suffix
**	-------------------------------------------------------
**
**	Calling this with suffix set to "*" will set the default
**	representation.
**	Calling this with suffix set to "*.*" will set the default
**	representation for unknown suffix files which contain a "."
**
**	If filename suffix is already defined its previous
**	definition is overridden (or modified)
*/
PUBLIC BOOL HTBind_setType ARGS3(CONST char *,	suffix,
				 CONST char *,	representation,
				 double,	value)
{
    return HTBind_setBinding(suffix, representation, NULL, NULL, value);
}

PUBLIC BOOL HTBind_setEncoding ARGS3(CONST char *,	suffix,
				     CONST char *,	encoding,
				     double,		value)
{
    return HTBind_setBinding(suffix, NULL, encoding, NULL, value);
}

PUBLIC BOOL HTBind_setLanguage ARGS3(CONST char *,	suffix,
				     CONST char *,	language,
				     double,		value)
{
    return HTBind_setBinding(suffix, NULL, NULL, language, value);
}

PUBLIC BOOL HTBind_setBinding ARGS5(CONST char *,	suffix,
				    CONST char *,	representation,
				    CONST char *,	encoding,
				    CONST char *,	language,
				    double,		value)
{
    HTBind * suff;
    if (!suffix)
	return NO;
    if (!strcmp(suffix, "*"))
	suff = &no_suffix;
    else if (!strcmp(suffix, "*.*"))
	suff = &unknown_suffix;
    else {
	HTList *suflist;
	int hash=0;
	CONST char *ptr=suffix;

	/* Select list from hash table */
	for( ; *ptr; ptr++)
	    hash = (int) ((hash * 3 + (*(unsigned char*)ptr)) % HASH_SIZE);

	if (!HTBindings[hash]) HTBindings[hash] = HTList_new();
	suflist = HTBindings[hash];

	/* Look for existing binding */
	{
	    HTList *cur = suflist;
	    while ((suff = (HTBind *) HTList_nextObject(cur)) != NULL) {
		if (!strcmp(suff->suffix, suffix))
		    break;
	    }
	}

	/* If not found -- create a new node */
	if (!suff) {
	    if ((suff = (HTBind *) calloc(1, sizeof(HTBind))) == NULL)
		outofmem(__FILE__, "HTHTBind_setBinding");
	    HTList_addObject(suflist, (void *) suff);
	    StrAllocCopy(suff->suffix, suffix);
	}
    }

    /* Set the appropriate values */
    {
	char *str = NULL;
	char *ptr;
	if (representation) {
	    StrAllocCopy(str, representation);
	    for (ptr=str; *ptr; ptr++)
		*ptr = TOLOWER(*ptr);
	    suff->type = HTAtom_for(str);
	}
	if (language) {
	    StrAllocCopy(str, language);
	    for (ptr=str; *ptr; ptr++)
		*ptr = TOLOWER(*ptr);
	    suff->language = HTAtom_for(str);
	}
	if (encoding) {
	    StrAllocCopy(str, encoding);
	    for (ptr=str; *ptr; ptr++)
		*ptr = TOLOWER(*ptr);
	    suff->encoding = HTAtom_for(str);
	}
	FREE(str);
	suff->quality = value;
    }
    return YES;
}


/*	Determine a suitable suffix
**	---------------------------
**  Use the set of bindings to find a suitable suffix (or index)
**  for a certain combination of language, media type and encoding
**  given in the anchor.
**
**  Returns a pointer to a suitable suffix string that must be freed 
**  by the caller. If more than one suffix is found they are all
**  concatenated using the first delimiter in HTDelimiters.
**  If no suffix is found, NULL is returned.
*/
PUBLIC char * HTBind_getSuffix ARGS1(HTParentAnchor *, anchor)
{
    int cnt;
    HTList *cur;
    char *suffix = NULL;
    char delimiter[2];
    *delimiter = *HTDelimiters;
    *(delimiter+1) = '\0';
    if (anchor) {
	for (cnt=0; cnt<HASH_SIZE; cnt++) {
	    if ((cur = HTBindings[cnt])) { 
		HTBind *pres;
		while ((pres = (HTBind *) HTList_nextObject(cur)) != NULL) {
		    if ((pres->type && pres->type==anchor->content_type) ||
			(pres->encoding &&
			 pres->encoding!=WWW_ENC_7BIT &&
			 pres->encoding!=WWW_ENC_8BIT &&
			 pres->encoding!=WWW_ENC_BINARY &&
			 pres->encoding==anchor->content_encoding) ||
			(pres->language &&
			 pres->language == anchor->content_language)) {
			StrAllocCat(suffix, delimiter);
			StrAllocCat(suffix, pres->suffix);
		    }
		}
	    }
	}
    }
    return suffix;
}

/*	Determine the description of a file
**	-----------------------------------
**  Use the set of bindings to find the combination of language,
**  media type and encoding of a given file name.
**
**  If more than one suffix is found they are all searched. The last suffix
**  has highest priority, the first one lowest. See also HTBind_getFormat()
**
**  Returns a contentdescription object with the representations found. This
**  must be free by the caller
*/
PUBLIC HTContentDescription * HTBind_getDescription ARGS1(char *, file)
{
    HTContentDescription * cd;
    cd = (HTContentDescription *) calloc(1, sizeof(HTContentDescription));
    if (!cd) outofmem(__FILE__, "HTContentDescription");
    if (HTBind_getFormat(file, &cd->content_type, &cd->content_encoding,
			 &cd->content_language, &cd->quality))
	return cd;
    else {
	free(cd);
	return NULL;
    }
}

/*	Determine the content of an Anchor
**	----------------------------------
**  Use the set of bindings to find the combination of language,
**  media type and encoding of a given anchor.
**
**  If more than one suffix is found they are all searched. The last suffix
**  has highest priority, the first one lowest. See also HTBind_getFormat()
**
**  Returns the anchor object with the representations found
*/
PUBLIC BOOL HTBind_getBindings ARGS1(HTParentAnchor *, anchor)
{
    BOOL status = NO;
    double quality=1.0;		  /* @@@ Should we add this into the anchor? */
    if (anchor) {
	char *addr = HTAnchor_physical(anchor);
	char *path = HTParse(addr, "", PARSE_PATH+PARSE_PUNCTUATION);
	char *file = strrchr(path, '/');
	if (!file) {
	    if (BIND_TRACE)
		fprintf(TDEST,"Get Binding. No file name found `%s\'\n", path);
	} else {
	    status = HTBind_getFormat(file, &anchor->content_type,
				      &anchor->content_encoding,
				      &anchor->content_language,
				      &quality);
	}
	FREE(path);
    }
    return status;
}


/*	Determine the content of an file name
**	-------------------------------------
**  Use the set of bindings to find the combination of language,
**  media type and encoding of a given anchor.
**
**  If more than one suffix is found they are all searched. The last suffix
**  has highest priority, the first one lowest. See also HTBind_getBindings()
**
**  Returns the format, encoding, and language found
*/
PUBLIC BOOL HTBind_getFormat ARGS5(CONST char *,filename,
				   HTFormat *,	format,
				   HTEncoding *,encoding,
				   HTLanguage *,language,
				   double *, quality)
{
    int sufcnt=0;
    char *file=NULL;
#ifdef HT_REENTRANT
    char *lasts;					     /* For strtok_r */
#endif
    if (*quality < HT_EPSILON)
	*quality = 1.0;			           /* Set to a neutral value */
    StrAllocCopy(file, filename);
    HTUnEscape(file);				   /* Unescape the file name */
#ifdef HT_REENTRANT
    if (strtok_r(file, HTDelimiters, &lasts)) {	 /* Do we have any suffixes? */
#else
    if (strtok(file, HTDelimiters)) { 		 /* Do we have any suffixes? */
#endif /* HT_REENTRANT */
	char *suffix;
#ifdef HT_REENTRANT
	while ((suffix=(char*)strtok_r(NULL, HTDelimiters, &lasts)) != NULL) {
#else
	while ((suffix=strtok(NULL, HTDelimiters)) != NULL) {
#endif /* HT_REENTRANT */
	    HTBind *suff=NULL;
	    int hash=0;
	    char *ptr=suffix;
	    if (BIND_TRACE)
		fprintf(TDEST, "Get Binding. Look for '%s\' ", suffix);
	    sufcnt++;

	    /* Select list from hash table */
	    for( ; *ptr; ptr++)
		hash = (int)((hash*3+(*(unsigned char*)ptr)) % HASH_SIZE);

	    /* Now search list for entries (case or non case sensitive) */
	    if (HTBindings[hash]) {
		HTList *cur = HTBindings[hash];
		while ((suff = (HTBind *) HTList_nextObject(cur))) {
		    if ((HTCaseSen && !strcmp(suff->suffix, suffix)) ||
			!strcasecomp(suff->suffix, suffix)) {
			if (BIND_TRACE) fprintf(TDEST, "Found!\n");
			if (suff->type) *format = suff->type;
			if (suff->encoding) *encoding = suff->encoding;
			if (suff->language) *language = suff->language;
			if (suff->quality > HT_EPSILON)
			    *quality *= suff->quality;
			break;
		    }
		}
	    }
	    if (!suff) {	/* We don't have this suffix - use default */
		if (BIND_TRACE)
		    fprintf(TDEST,"Not found - use default for \'*.*\'\n");
		*format = unknown_suffix.type;
		*encoding = unknown_suffix.encoding;
		*language = unknown_suffix.language;
		*quality = unknown_suffix.quality;
	    }
	} /* while we still have suffixes */
    }
    if (!sufcnt) {		/* No suffix so use default value */
	if (BIND_TRACE)
	    fprintf(TDEST, "Get Binding. No suffix found - using default '%s\'\n", filename);
	*format = no_suffix.type;
	*encoding = no_suffix.encoding;
	*language = no_suffix.language;
	*quality = no_suffix.quality;
    }
    if (BIND_TRACE)
	fprintf(TDEST, "Get Binding. Result for '%s\' is: type='%s\', encoding='%s\', language='%s\' with quality %.2f\n",
		filename,
		*format ? HTAtom_name(*format) : "unknown",
		*encoding ? HTAtom_name(*encoding) : "unknown",
		*language ? HTAtom_name(*language) : "unknown",
		*quality);
    free(file);
    return YES;
}

