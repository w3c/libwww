/*								     Htbind.c
**	FILE SUFFIX BIND MANAGER
**
**	(c) COPYRIGHT MIT 1995
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
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
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTAnchor.h"
#include "HTResponse.h"
#include "HTParse.h"
#include "HTBind.h"					 /* Implemented here */

typedef struct _HTBind {
    char *	suffix;
    HTFormat	type;			/* Content-Type */
    HTEncoding	encoding;		/* Content-Encoding */
    HTEncoding	transfer;		/* Content-Transfer-Encoding */
    HTLanguage	language;		/* Content-Language */
    double	quality;
} HTBind;

/* Suffix registration */
PRIVATE BOOL HTCaseSen = YES;		      /* Are suffixes case sensitive */
PRIVATE char *HTDelimiters = NULL;			  /* Set of suffixes */

PRIVATE HTList **HTBindings = NULL;   /* Point to table of lists of bindings */

PRIVATE HTBind no_suffix = { "*", NULL, NULL, NULL, NULL, 0.5 };
PRIVATE HTBind unknown_suffix = { "*.*", NULL, NULL, NULL, NULL, 0.5 };

/* ------------------------------------------------------------------------- */

/*	
**	Set up the list of suffix bindings. Done by HTLibInit
*/
PUBLIC BOOL HTBind_init (void)
{
    if (!HTBindings) {
	if (!(HTBindings = (HTList **) HT_CALLOC(HT_L_HASH_SIZE, sizeof(HTList *))))
	    HT_OUTOFMEM("HTBind_init");
    }
    StrAllocCopy(HTDelimiters, DEFAULT_SUFFIXES);
    no_suffix.type = WWW_UNKNOWN;
    no_suffix.encoding = WWW_CODING_BINARY;
    unknown_suffix.type = WWW_UNKNOWN;
    unknown_suffix.encoding = WWW_CODING_BINARY;
    return YES;
}


/*
**	Cleans up the memory allocated by file bindings
**	Done by HTLibTerminate().
**	Written by Eric Sink, eric@spyglass.com, and Henrik
*/
PUBLIC BOOL HTBind_deleteAll (void)
{
    int cnt;
    HTList *cur;
    if (!HTBindings)
	return NO;
    for (cnt=0; cnt<HT_L_HASH_SIZE; cnt++) {
	if ((cur = HTBindings[cnt])) { 
	    HTBind *pres;
	    while ((pres = (HTBind *) HTList_nextObject(cur)) != NULL) {
		HT_FREE(pres->suffix);
		HT_FREE(pres);
	    }
	}
	HTList_delete(HTBindings[cnt]);
	HTBindings[cnt] = NULL;
    }
    HT_FREE(HTBindings);
    HT_FREE(HTDelimiters);
    return YES;
}


/*	Make suffix bindings case sensitive
**	-----------------------------------
*/
PUBLIC void HTBind_caseSensitive (BOOL sensitive)
{
    HTCaseSen = sensitive;
}


/*	Get set of suffixes
**	-------------------
*/
PUBLIC const char *HTBind_delimiters (void)
{
    return HTDelimiters;
}


/*	Change set of suffixes
**	----------------------
*/
PUBLIC void HTBind_setDelimiters (const char * new_suffixes)
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
PUBLIC BOOL HTBind_addType (const char *	suffix,
			    const char *	representation,
			    double		value)
{
    return HTBind_add(suffix, representation, NULL, NULL, NULL, value);
}

PUBLIC BOOL HTBind_addEncoding (const char *	suffix,
				const char *	encoding,
				double		value)
{
    return HTBind_add(suffix, NULL, encoding, NULL, NULL, value);
}

PUBLIC BOOL HTBind_addTransfer (const char *	suffix,
				const char *	transfer,
				double		value)
{
    return HTBind_add(suffix, NULL, NULL, transfer, NULL, value);
}

PUBLIC BOOL HTBind_addLanguage (const char *	suffix,
				const char *	language,
				double		value)
{
    return HTBind_add(suffix, NULL, NULL, NULL, language, value);
}

PUBLIC BOOL HTBind_add (const char *	suffix,
			const char *	representation,
			const char *	encoding,
			const char *	transfer,
			const char *	language,
			double		value)
{
    HTBind * suff;
    if (!suffix)
	return NO;
    if (!strcmp(suffix, "*"))
	suff = &no_suffix;
    else if (!strcmp(suffix, "*.*"))
	suff = &unknown_suffix;
    else {
	HTList * suflist;
	int hash;
	const unsigned char * p;

	/* Select list from hash table */
	for (p=suffix, hash=0; *p; p++) {
	    hash = (hash * 3 + TOLOWER(*p)) % HT_L_HASH_SIZE;
	}

	if (!HTBindings) HTBind_init();
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
	    if ((suff = (HTBind *) HT_CALLOC(1, sizeof(HTBind))) == NULL)
	        HT_OUTOFMEM("HTBind_add");
	    HTList_addObject(suflist, (void *) suff);
	    StrAllocCopy(suff->suffix, suffix);
	}
    }

    /* Set the appropriate values */
    {
	HTChunk * chunk = HTChunk_new(32);
	char *ptr;
	if (representation) {
	    HTChunk_puts(chunk, representation);
	    ptr = HTChunk_data(chunk);
	    for (; *ptr; ptr++)
		*ptr = TOLOWER(*ptr);
	    suff->type = HTAtom_for(HTChunk_data(chunk));
	    HTChunk_truncate(chunk,0);
	}
	if (encoding) {
	    HTChunk_puts(chunk, encoding);
	    ptr = HTChunk_data(chunk);
	    for (; *ptr; ptr++)
		*ptr = TOLOWER(*ptr);
	    suff->encoding = HTAtom_for(HTChunk_data(chunk));
	    HTChunk_truncate(chunk,0);
	}
	if (transfer) {
	    HTChunk_puts(chunk, transfer);
	    ptr = HTChunk_data(chunk);
	    for (; *ptr; ptr++)
		*ptr = TOLOWER(*ptr);
	    suff->transfer = HTAtom_for(HTChunk_data(chunk));
	    HTChunk_truncate(chunk,0);
	}
	if (language) {
	    HTChunk_puts(chunk, language);
	    ptr = HTChunk_data(chunk);
	    for (; *ptr; ptr++)
		*ptr = TOLOWER(*ptr);
	    suff->language = HTAtom_for(HTChunk_data(chunk));
	    HTChunk_truncate(chunk,0);
	}
	HTChunk_delete(chunk);
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
PUBLIC char * HTBind_getSuffix (HTParentAnchor * anchor)
{
    int cnt;
    HTList * cur;
    HTChunk * suffix = HTChunk_new(48);
    char delimiter = *HTDelimiters;
    char * ct=NULL, * ce=NULL, * cl=NULL;
    HTFormat format = HTAnchor_format(anchor);
    HTList * encoding = HTAnchor_encoding(anchor);
    HTList * language = HTAnchor_language(anchor);
    if (!HTBindings) HTBind_init();
    if (anchor) {
	for (cnt=0; cnt<HT_L_HASH_SIZE; cnt++) {
	    if ((cur = HTBindings[cnt])) { 
		HTBind *pres;
		while ((pres = (HTBind *) HTList_nextObject(cur))) {
		    if (!ct && (pres->type && pres->type == format)){
			ct = pres->suffix;
		    } else if (!ce && pres->encoding && encoding) {
			HTList * cur_enc = encoding;
			HTEncoding pres_enc;
			while ((pres_enc = (HTEncoding) HTList_nextObject(cur_enc))) {
			    if (pres_enc == pres->encoding) {
				ce = pres->suffix;
				break;
			    }
			}
		    } else if (!cl && pres->language && language) {
			HTList * cur_lang = language;
			HTLanguage pres_lang;
			while ((pres_lang = (HTLanguage) HTList_nextObject(cur_lang))) {
			    if (pres_lang == pres->language) {
				cl = pres->suffix;
				break;
			    }
			}
		    }
		}
	    }
	}

	/* Put the found suffixes together */
	if (ct) {
	    HTChunk_putc(suffix, delimiter);
	    HTChunk_puts(suffix, ct);
	}
	if (ce) {
	    HTChunk_putc(suffix, delimiter);
	    HTChunk_puts(suffix, ce);
	}
	if (cl) {
	    HTChunk_putc(suffix, delimiter);
	    HTChunk_puts(suffix, cl);
	}
    }
    return HTChunk_toCString(suffix);
}

/*
**  Use the set of bindings to find the combination of language,
**  media type and encoding of a given object. This information can either be
**  stored in the anchor obejct or in the response object depending on which
**  function is called.
**
**  We comprise here as bindings only can have one language and one encoding.
**  If more than one suffix is found they are all searched. The last suffix
**  has highest priority, the first one lowest. See also HTBind_getFormat()
*/
PUBLIC BOOL HTBind_getAnchorBindings (HTParentAnchor * anchor)
{
    BOOL status = NO;
    double quality=1.0;		  /* @@@ Should we add this into the anchor? */
    if (anchor) {
	char *addr = HTAnchor_address((HTAnchor *) anchor);
	char *path = HTParse(addr, "", PARSE_PATH+PARSE_PUNCTUATION);
	char *file;
	char *end;
	if ((end = strchr(path, ';')) || (end = strchr(path, '?')) ||
	    (end = strchr(path, '#')))
	    *end = '\0';
	if ((file = strrchr(path, '/'))) {
	    HTFormat format = NULL;
	    HTEncoding encoding = NULL;
	    HTEncoding transfer = NULL;
	    HTLanguage language = NULL;
 	    HTTRACE(BIND_TRACE, "Anchor...... Get bindings for `%s\'\n" _ path);
	    status = HTBind_getFormat(file, &format, &encoding, &transfer,
				      &language, &quality);
	    if (status) {
		HTAnchor_setFormat(anchor, format);
		HTAnchor_setContentTransferEncoding(anchor, transfer);

                HTAnchor_deleteEncodingAll(anchor);
                HTAnchor_addEncoding(anchor, encoding);

                HTAnchor_deleteLanguageAll(anchor);
                HTAnchor_addLanguage(anchor, language);
	    }
	}
        HT_FREE(addr);
        HT_FREE(path);
    }
    return status;
}

PUBLIC BOOL HTBind_getResponseBindings (HTResponse * response, const char * url)
{
    BOOL status = NO;
    double quality = 1.0;
    if (response) {
	char * path = HTParse(url, "", PARSE_PATH + PARSE_PUNCTUATION);
	char * file;
	char * end;
	if ((end = strchr(path, ';')) || (end = strchr(path, '?')) ||
	    (end = strchr(path, '#')))
	    *end = '\0';
	if ((file = strrchr(path, '/'))) {
	    HTFormat format = NULL;
	    HTEncoding encoding = NULL;
	    HTEncoding transfer = NULL;
	    HTLanguage language = NULL;
 	    HTTRACE(BIND_TRACE, "Response.... Get Bindings for `%s\'\n" _ path);
	    status = HTBind_getFormat(file, &format, &encoding, &transfer,
				      &language, &quality);
	    if (status) {
		HTResponse_setFormat(response, format);
		HTResponse_setContentTransferEncoding(response, transfer);
		HTResponse_addEncoding(response, encoding);
#if 0
		HTResponse_addLanguage(response, language);
#endif
	    }
	}
	HT_FREE(path);
    }
    return status;
}

/*	Determine the content of an file name
**	-------------------------------------
**  Use the set of bindings to find the combination of language,
**  media type, encoding, and transfer encoding  of a given anchor.
**  If more than one suffix is found they are all searched. The last suffix
**  has highest priority, the first one lowest. See also HTBind_getBindings()
**  Either of format, encoding, or language can be NULL
**  Returns the format, encoding, and language found
*/
PUBLIC BOOL HTBind_getFormat (const char *	filename,
			      HTFormat *	format,
			      HTEncoding *	enc,
			      HTEncoding *	cte,
			      HTLanguage *	lang,
			      double *		quality)
{
    int sufcnt=0;
    char *file=NULL;
#ifdef HT_REENTRANT
    char *lasts;					     /* For strtok_r */
#endif
    if (!HTBindings) HTBind_init();
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
	    int hash;
	    unsigned char * p;
	    HTTRACE(BIND_TRACE, "Get Binding. Look for '%s\' " _ suffix);
	    sufcnt++;

	    /* Select list from hash table */
	    for (p=suffix, hash=0; *p; p++) {
		hash = (hash * 3 + TOLOWER(*p)) % HT_L_HASH_SIZE;
	    }

	    /* Now search list for entries (case or non case sensitive) */
	    if (HTBindings[hash]) {
		HTList *cur = HTBindings[hash];
		while ((suff = (HTBind *) HTList_nextObject(cur))) {
		    if ((HTCaseSen && !strcmp(suff->suffix, suffix)) ||
			!strcasecomp(suff->suffix, suffix)) {
			HTTRACE(BIND_TRACE, "Found!\n");
			if (suff->type && format) *format = suff->type;
			if (suff->encoding && enc) *enc = suff->encoding;
			if (suff->transfer && cte) *cte = suff->transfer;
			if (suff->language && lang) *lang = suff->language;
			if (suff->quality > HT_EPSILON)
			    *quality *= suff->quality;
			break;
		    }
		}
	    }
	    if (!suff) {	/* We don't have this suffix - use default */
		HTTRACE(BIND_TRACE, "Not found - use default for \'*.*\'\n");
		if (format) *format = unknown_suffix.type;
		if (enc) *enc = unknown_suffix.encoding;
		if (cte) *cte = unknown_suffix.transfer;
		if (lang) *lang = unknown_suffix.language;
		*quality = unknown_suffix.quality;
	    }
	} /* while we still have suffixes */
    }
    if (!sufcnt) {		/* No suffix so use default value */
	HTTRACE(BIND_TRACE, "Get Binding. No suffix found - using default '%s\'\n" _ filename);
	if (format) *format = no_suffix.type;
	if (enc) *enc = no_suffix.encoding;
	if (cte) *cte = no_suffix.transfer;
	if (lang) *lang = no_suffix.language;
	*quality = no_suffix.quality;
    }
    HTTRACE(BIND_TRACE, "Get Binding. Result for '%s\' is: type='%s\', encoding='%s\', cte='%s\', language='%s\' with quality %.2f\n" _ 
		filename _ 
		(format && *format) ? HTAtom_name(*format) : "unknown" _ 
		(enc && *enc) ? HTAtom_name(*enc) : "unknown" _ 
		(cte && *cte) ? HTAtom_name(*cte) : "unknown" _ 
		(lang && *lang) ? HTAtom_name(*lang) : "unknown" _ 
		*quality);
    HT_FREE(file);
    return YES;
}

