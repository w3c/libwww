/*								     HTFormat.c
**	MANAGE DIFFERENT FILE FORMATS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** Bugs:
**	Assumes the incoming stream is ASCII, rather than a local file
**	format, and so ALWAYS converts from ASCII on non-ASCII machines.
**	Therefore, non-ASCII machines can't read local files.
**
** HISTORY:
**	8 Jul 94  FM	Insulate free from _free structure element.
**	8 Nov 94  HFN	Changed a lot to make reentrant
*/

/* Library Include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "HTReqMan.h"
#include "HTNetMan.h"
#include "HTStream.h"
#include "HTFWrite.h"
#include "HTError.h"
#include "HTFormat.h"					 /* Implemented here */

#define NO_VALUE_FOUND	-1e30		 /* Stream Stack Value if none found */

PRIVATE HTList * HTConversions = NULL;			    /* Content types */
PRIVATE HTList * HTEncodings = NULL;			 /* Content encoders */
PRIVATE HTList * HTCharsets = NULL;
PRIVATE HTList * HTLanguages = NULL;

PRIVATE double HTMaxSecs = 1e10;		/* No effective limit */

struct _HTStream {
    const HTStreamClass *	isa;
};

struct _HTContentCoding {
    HTEncoding		coding;
    HTContentCoder *	encoder;
    HTContentCoder *	decoder;
    double		quality;
};

/* ------------------------------------------------------------------------- */
/*				BASIC CONVERTERS			     */
/* ------------------------------------------------------------------------- */

PUBLIC HTStream * HTBlackHoleConverter (HTRequest *	request,
					void *		param,
					HTFormat	input_format,
					HTFormat	output_format,
					HTStream *	output_stream)
{
    return HTBlackHole();
}
/*	HTThroughLine
**	-------------
**
** This function is a dummy function that returns the same output stream
** as given as a parameter. Henrik 01/03-94
*/
PUBLIC HTStream* HTThroughLine (HTRequest *	request,
				void *		param,
				HTFormat	input_format,
				HTFormat	output_format,
				HTStream *	output_stream)
{
    return output_stream;
}

/* ------------------------------------------------------------------------- */
/* 	  ACCEPT LISTS OF CONVERSIONS, ENCODINGS, LANGUAGE, AND CHARSET	     */
/* ------------------------------------------------------------------------- */

/*
**	For all `accept lists' there is a local list and a global list. The
**	local list is a part of the request structure and the global list is
**	internal to the HTFormat module. The global lists can be used when
**	specifying accept lists for ALL requests and the local list can be 
**	used to add specific accept headers to the request.
*/


/*	Define a presentation system command for a content-type
**	-------------------------------------------------------
** INPUT:
**	conversions:	The list of conveters and presenters
**	representation:	the MIME-style format name
**	command:	the MAILCAP-style command template
**	quality:	A degradation faction [0..1]
**	maxbytes:	A limit on the length acceptable as input (0 infinite)
**	maxsecs:	A limit on the time user will wait (0 for infinity)
*/
PUBLIC void HTPresentation_add (HTList *	conversions,
				const char *	representation,
				const char *	command,
				const char *	test_command,
				double		quality,
				double		secs, 
				double		secs_per_byte)
{
    HTPresentation * pres;
    if ((pres = (HTPresentation  *) HT_CALLOC(1,sizeof(HTPresentation))) == NULL)
        HT_OUTOFMEM("HTSetPresentation");
    
    pres->rep = HTAtom_for(representation);
    pres->rep_out = WWW_PRESENT;		/* Fixed for now ... :-) */
    pres->converter = HTSaveAndExecute;		/* Fixed for now ...     */
    pres->quality = quality;
    pres->secs = secs;
    pres->secs_per_byte = secs_per_byte;
    pres->rep = HTAtom_for(representation);
    pres->command = NULL;
    StrAllocCopy(pres->command, command);
    pres->test_command = NULL;
    StrAllocCopy(pres->test_command, test_command);
    HTList_addObject(conversions, pres);
}

PUBLIC void HTPresentation_deleteAll (HTList * list)
{
    if (list) {
	HTList *cur = list;
	HTPresentation *pres;
	while ((pres = (HTPresentation*) HTList_nextObject(cur))) {
	    HT_FREE(pres->command);
	    HT_FREE(pres);
	}
	HTList_delete(list);
    }
}

/*	Define a built-in function for a content-type
**	---------------------------------------------
*/
PUBLIC void HTConversion_add (HTList *		conversions,
			      const char *	representation_in,
			      const char *	representation_out,
			      HTConverter *	converter,
			      double		quality,
			      double		secs, 
			      double		secs_per_byte)
{
    HTPresentation * pres;
    if ((pres = (HTPresentation*) HT_CALLOC(1,sizeof(HTPresentation))) == NULL)
        HT_OUTOFMEM("HTSetPresentation");
    pres->rep = HTAtom_for(representation_in);
    pres->rep_out = HTAtom_for(representation_out);
    pres->converter = converter;
    pres->command = NULL;		/* Fixed */
    pres->test_command = NULL;
    pres->quality = quality;
    pres->secs = secs;
    pres->secs_per_byte = secs_per_byte;
    HTList_addObject(conversions, pres);
}

PUBLIC void HTConversion_deleteAll (HTList * list)
{
    HTPresentation_deleteAll(list);
}

PUBLIC BOOL HTContentCoding_add (HTList * 		list,
				 const char *		coding,
				 HTContentCoder *	encoder,
				 HTContentCoder *	decoder,
				 double			quality)
{
    if (list && coding && (encoder || decoder)) {
	HTContentCoding * me;
	if ((me=(HTContentCoding*)HT_CALLOC(1, sizeof(HTContentCoding)))==NULL)
	    HT_OUTOFMEM("HTContentCoding_add");
	me->coding = HTAtom_for(coding);
	me->encoder = encoder;
	me->decoder = decoder;
	me->quality = quality;
	if (CORE_TRACE)
	    HTTrace("Codings..... Adding %s with quality %.2f\n",
		    coding, quality);
	return HTList_addObject(list, (void *) me);
    }
    if (CORE_TRACE) HTTrace("Codings..... Bad argument\n");
    return NO;
}

PUBLIC void HTContentCoding_deleteAll (HTList * list)
{
    if (list) {
	HTList * cur = list;
	HTContentCoding * pres;
	while ((pres = (HTContentCoding *) HTList_nextObject(cur)))
	    HT_FREE(pres);
	HTList_delete(list);
    }
}

PUBLIC const char * HTContentCoding_name (HTContentCoding * me)
{
    return me ? HTAtom_name(me->coding) : NULL;
}

PUBLIC void HTLanguage_add (HTList *		list,
			    const char *	lang,
			    double		quality)
{
    HTAcceptNode * node;
    if (!list || !lang || !*lang)  {
	if (CORE_TRACE)
	    HTTrace("Languages... Bad argument\n");
	return;
    }
    if ((node = (HTAcceptNode *) HT_CALLOC(1, sizeof(HTAcceptNode))) == NULL)
        HT_OUTOFMEM("HTAcceptLanguage");

    HTList_addObject(list, (void*)node);
    node->atom = HTAtom_for(lang);
    node->quality = quality;
}

PUBLIC void HTLanguage_deleteAll (HTList * list)
{
    if (list) {
	HTList *cur = list;
	HTAcceptNode *pres;
	while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
	    HT_FREE(pres);
	}
	HTList_delete(list);
    }
}

PUBLIC void HTCharset_add (HTList *		list,
			   const char *		charset,
			   double		quality)
{
    HTAcceptNode * node;
    if (!list || !charset || !*charset)  {
	if (CORE_TRACE)
	    HTTrace("Charset..... Bad argument\n");
	return;
    }
    if ((node = (HTAcceptNode *) HT_CALLOC(1, sizeof(HTAcceptNode))) == NULL)
        HT_OUTOFMEM("HTAcceptCharsetuage");

    HTList_addObject(list, (void*)node);
    node->atom = HTAtom_for(charset);
    node->quality = quality;
}

PUBLIC void HTCharset_deleteAll (HTList * list)
{
    if (list) {
	HTList *cur = list;
	HTAcceptNode *pres;
	while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
	    HT_FREE(pres);
	}
	HTList_delete(list);
    }
}

/* ------------------------------------------------------------------------- */
/* 			GLOBAL LIST OF CONVERTERS ETC.			     */
/* ------------------------------------------------------------------------- */

/*
**	Global Accept Format Types Conversions
**	list can be NULL
*/
PUBLIC void HTFormat_setConversion (HTList * list)
{
    HTConversions = list;
}

PUBLIC HTList * HTFormat_conversion (void)
{
    return HTConversions;
}

/*
**	Global Content Encodings
**	list can be NULL
*/
PUBLIC void HTFormat_setEncoding (HTList *list)
{
    HTEncodings = list;
}

PUBLIC HTList * HTFormat_encoding (void)
{
    return HTEncodings;
}

/*
**	Global Accept Languages
**	list can be NULL
*/
PUBLIC void HTFormat_setLanguage (HTList *list)
{
    HTLanguages = list;
}

PUBLIC HTList * HTFormat_language (void)
{
    return HTLanguages;
}

/*
**	Global Accept Charsets
**	list can be NULL
*/
PUBLIC void HTFormat_setCharset (HTList *list)
{
    HTCharsets = list;
}

PUBLIC HTList * HTFormat_charset (void)
{
    return HTCharsets;
}

/*
**	Convenience function to clean up
*/
PUBLIC void HTFormat_deleteAll (void)
{
    if (HTConversions) {
	HTConversion_deleteAll(HTConversions);
	HTConversions = NULL;
    }
    if (HTLanguages) {
	HTLanguage_deleteAll(HTLanguages);
	HTLanguages = NULL;
    }
    if (HTEncodings) {
	HTContentCoding_deleteAll(HTEncodings);
	HTEncodings = NULL;
    }
    if (HTCharsets) {
	HTCharset_deleteAll(HTCharsets);
	HTCharsets = NULL;
    }
}

/* ------------------------------------------------------------------------- */
/* 				FORMAT NEGOTIATION			     */
/* ------------------------------------------------------------------------- */

PRIVATE BOOL better_match (HTFormat f, HTFormat g)
{
    const char *p, *q;

    if (f && g  &&  (p = HTAtom_name(f))  &&  (q = HTAtom_name(g))) {
	int i,j;
	for(i=0 ; *p; p++) if (*p == '*') i++;
	for(j=0 ; *q; q++) if (*q == '*') j++;
	if (i < j) return YES;
    }
    return NO;
}


PRIVATE BOOL wild_match (HTAtom * tmplate, HTAtom * actual)
{
    char *t, *a, *st, *sa;
    BOOL match = NO;

    if (tmplate && actual && (t = HTAtom_name(tmplate))) {
	if (!strcmp(t, "*"))
	    return YES;

	if (strchr(t, '*') &&
	    (a = HTAtom_name(actual)) &&
	    (st = strchr(t, '/')) && (sa = strchr(a,'/'))) {

	    *sa = 0;
	    *st = 0;

	    if ((*(st-1)=='*' &&
		 (*(st+1)=='*' || !strcasecomp(st+1, sa+1))) ||
		(*(st+1)=='*' && !strcasecomp(t,a)))
		match = YES;

	    *sa = '/';
	    *st = '/';
	}    
    }
    return match;
}

/*
 * Added by takada@seraph.ntt.jp (94/04/08)
 */
PRIVATE BOOL lang_match (HTAtom * tmplate, HTAtom * actual)
{
    char *t, *a, *st, *sa;
    BOOL match = NO;

    if (tmplate && actual &&
	(t = HTAtom_name(tmplate)) && (a = HTAtom_name(actual))) {
	st = strchr(t, '_');
	sa = strchr(a, '_');
	if ((st != NULL) && (sa != NULL)) {
	    if (!strcasecomp(t, a))
	      match = YES;
	    else
	      match = NO;
	}
	else {
	    if (st != NULL) *st = 0;
	    if (sa != NULL) *sa = 0;
	    if (!strcasecomp(t, a))
	      match = YES;
	    else
	      match = NO;
	    if (st != NULL) *st = '_';
	    if (sa != NULL) *sa = '_';
	}
    }
    return match;
}
/* end of addition */



PRIVATE double type_value (HTAtom * content_type, HTList * accepted)
{
    HTList * cur = accepted;
    HTPresentation * pres;
    HTPresentation * wild = NULL;

    if (!content_type || !accepted) return -1;

    while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
	if (pres->rep == content_type)
	    return pres->quality;
	else if (wild_match(pres->rep, content_type))
	    wild = pres;
    }
    if (wild) return wild->quality;
    else return -1;
}


PRIVATE double lang_value (HTAtom * language, HTList * accepted)
{
    HTList * cur = accepted;
    HTAcceptNode * node;
    HTAcceptNode * wild = NULL;

    if (!language || !accepted || HTList_isEmpty(accepted)) {
	return 0.1;
    }

    while ((node = (HTAcceptNode*)HTList_nextObject(cur))) {
	if (node->atom == language) {
	    return node->quality;
	}
	/*
	 * patch by takada@seraph.ntt.jp (94/04/08)
	 * the original line was
	 * else if (wild_match(node->atom, language)) {
	 * and the new line is
	 */
	else if (lang_match(node->atom, language)) {
	    wild = node;
	}
    }

    if (wild) {
	return wild->quality;
    }
    else {
	return 0.1;
    }
}


PRIVATE double encoding_value (HTAtom * encoding, HTList * accepted)
{
    HTList * cur = accepted;
    HTAcceptNode * node;
    HTAcceptNode * wild = NULL;
    char * e;

    if (!encoding || !accepted || HTList_isEmpty(accepted))
	return 1;

    e = HTAtom_name(encoding);
    if (!strcmp(e, "7bit") || !strcmp(e, "8bit") || !strcmp(e, "binary"))
	return 1;

    while ((node = (HTAcceptNode*)HTList_nextObject(cur))) {
	if (node->atom == encoding)
	    return node->quality;
	else if (wild_match(node->atom, encoding))
	    wild = node;
    }
    if (wild) return wild->quality;
    else return 1;
}


PUBLIC BOOL HTRank (HTList * possibilities,
		    HTList * accepted_content_types,
		    HTList * accepted_languages,
		    HTList * accepted_encodings)
{
    int accepted_cnt = 0;
    HTList * accepted;
    HTList * sorted;
    HTList * cur;
    HTContentDescription * d;

    if (!possibilities) return NO;

    accepted = HTList_new();
    cur = possibilities;
    while ((d = (HTContentDescription*)HTList_nextObject(cur))) {
	double tv = type_value(d->content_type, accepted_content_types);
	double lv = lang_value(d->content_language, accepted_languages);
	double ev = encoding_value(d->content_encoding, accepted_encodings);

	if (tv > 0) {
	    d->quality *= tv * lv * ev;
	    HTList_addObject(accepted, d);
	    accepted_cnt++;
	}
	else {
	    HT_FREE(d->filename);
	    HT_FREE(d);
	}
    }

    if (CORE_TRACE) HTTrace("Ranking.....\n");
    if (CORE_TRACE) HTTrace(
	   "\nRANK QUALITY CONTENT-TYPE         LANGUAGE ENCODING    FILE\n");

    sorted = HTList_new();
    while (accepted_cnt-- > 0) {
	HTContentDescription * worst = NULL;
	cur = accepted;
	while ((d = (HTContentDescription*)HTList_nextObject(cur))) {
	    if (!worst || d->quality < worst->quality)
		worst = d;
	}
	if (worst) {
	    if (CORE_TRACE)
		HTTrace("%d.   %.4f  %-20.20s %-8.8s %-10.10s %s\n",
			accepted_cnt+1,
			worst->quality,
			(worst->content_type
		         ? HTAtom_name(worst->content_type)      : "-"),
			(worst->content_language
		         ? HTAtom_name(worst->content_language)  :"-"),
			(worst->content_encoding
		         ? HTAtom_name(worst->content_encoding)  :"-"),
			(worst->filename
		         ? worst->filename                       :"-"));
	    HTList_removeObject(accepted, (void*)worst);
	    HTList_addObject(sorted, (void*)worst);
	}
    }
    if (CORE_TRACE) HTTrace("\n");
    HTList_delete(accepted);
    HTList_delete(possibilities->next);
    possibilities->next = sorted->next;
    sorted->next = NULL;
    HTList_delete(sorted);

    if (!HTList_isEmpty(possibilities)) return YES;
    else return NO;
}


/*	Create a Content Type filter stack
**	----------------------------------
**	If a wildcard match is made, a temporary HTPresentation
**	structure is made to hold the destination format while the
**	new stack is generated. This is just to pass the out format to
**	MIME so far.  Storing the format of a stream in the stream might
**	be a lot neater.
**
**	The star/star format is special, in that if you can take
**	that you can take anything.
*/
PUBLIC HTStream * HTStreamStack (HTFormat	rep_in,
				 HTFormat	rep_out,
				 HTStream *	output_stream,
				 HTRequest *	request,
				 BOOL		guess)
{
    HTList * conversion[2];
    int which_list;
    double best_quality = -1e30;		/* Pretty bad! */
    HTPresentation *pres, *best_match=NULL;
    if (rep_out == WWW_RAW) {
	if (CORE_TRACE) HTTrace("StreamStack. Raw output...\n");
	return output_stream ? output_stream : HTErrorStream();
    }

    if (rep_out == rep_in) {
	if (CORE_TRACE)
	    HTTrace("StreamStack. Identical input/output format (%s)\n",
		     HTAtom_name(rep_out));
	return output_stream ? output_stream : HTErrorStream();
    }
    if (CORE_TRACE) {
	char *p = HTAtom_name(rep_in);
	char *q = HTAtom_name(rep_out); 
	HTTrace("StreamStack. Constructing stream stack for %s to %s\n",
		 p ? p : "<NULL>", q ? q : "<NULL>");
    }

    conversion[0] = request->conversions;
    conversion[1] = HTConversions;

    for(which_list = 0; which_list<2; which_list++) {
	HTList * cur = conversion[which_list];
	while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
	    if ((pres->rep==rep_in || wild_match(pres->rep, rep_in)) &&
		(pres->rep_out==rep_out || wild_match(pres->rep_out,rep_out))){
		if (!best_match || better_match(pres->rep, best_match->rep) ||
		    (!better_match(best_match->rep, pres->rep) &&
		     pres->quality > best_quality)) {
#ifdef HAVE_SYSTEM
		    int result=0;
		    if (pres->test_command) {
			result = system(pres->test_command);
			if (CORE_TRACE) 
			    HTTrace("StreamStack. system(%s) returns %d\n", pres->test_command, result);
		    }
		    if (!result) {
			best_match = pres;
			best_quality = pres->quality;
		    }
#else
		    best_match = pres;
		    best_quality = pres->quality;
#endif /* HAVE_SYSTEM */
		}
	    }
	}
    }

    if (best_match) {
 	if (rep_out == WWW_SOURCE && best_match->rep_out != WWW_SOURCE) {
	    if (CORE_TRACE) HTTrace("StreamStack. Source output\n");
	    return output_stream ? output_stream : HTErrorStream();
	}
	return (*best_match->converter)(request, best_match->command,
					rep_in, rep_out, output_stream);
    }
    if (rep_out == WWW_SOURCE) {
	if (CORE_TRACE) HTTrace("StreamStack. Source output\n");
	return output_stream ? output_stream : HTErrorStream();
    }

    if (CORE_TRACE)
	HTTrace("StreamStack. No match found, dumping to local file\n");
    return HTSaveLocally(request, NULL, rep_in, rep_out, output_stream);
}
	

/*		Find the cost of a filter stack
**		-------------------------------
**
**	Must return the cost of the same stack which StreamStack would set up.
**
** On entry,
**	length	The size of the data to be converted
*/
PUBLIC double HTStackValue (HTList *	theseConversions,
			    HTFormat	rep_in,
			    HTFormat	rep_out,
			    double	initial_value,
			    long int	length)
{
    int which_list;
    HTList* conversion[2];
    
    if (CORE_TRACE) {
	HTTrace("StackValue.. Evaluating stream stack for %s worth %.3f to %s\n",
		HTAtom_name(rep_in),	initial_value,
		HTAtom_name(rep_out));
    }
    if (rep_out == WWW_SOURCE ||
    	rep_out == rep_in) return 0.0;

    conversion[0] = theseConversions;
    conversion[1] = HTConversions;
    
    for(which_list = 0; which_list<2; which_list++)
     if (conversion[which_list]) {
        HTList * cur = conversion[which_list];
	HTPresentation * pres;
	while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
	    if (pres->rep == rep_in &&
		(pres->rep_out == rep_out || wild_match(pres->rep_out, rep_out))) {
	        double value = initial_value * pres->quality;
		if (HTMaxSecs != 0.0)
		    value = value - (length*pres->secs_per_byte + pres->secs)
			                 /HTMaxSecs;
		return value;
	    }
	}
    }
    return NO_VALUE_FOUND;		/* Really bad */
}

/*	Create a new coder and insert it into stream chain
**	--------------------------------------------------
**	Creating the content decoding stack is not based on quality factors as
**	we don't have the freedom as with content types. Specify whether you
**	you want encoding or decoding using the BOOL "encoding" flag.
*/
PUBLIC HTStream * HTCodingStack (HTEncoding	coding,
				 HTStream *	target,
				 HTRequest *	request,
				 void *		param,
				 BOOL		encoding)
{
    HTList * coders[2];
    HTStream * top = target;
    HTContentCoding * pres = NULL;
    int cnt;
    if (!coding || !request) {
	if (CORE_TRACE) HTTrace("Codings... Nothing applied...\n");
	return target ? target : HTErrorStream();
    }
    coders[0] = request->encodings;
    coders[1] = HTEncodings;
    if (CORE_TRACE)
	HTTrace("Codings..... Looking for %s\n", HTAtom_name(coding));
    for (cnt=0; cnt < 2; cnt++) {
	HTList * cur = coders[cnt];
	while ((pres = (HTContentCoding *) HTList_nextObject(cur))) {
	    if (pres->coding == coding) {
		if (CORE_TRACE) HTTrace("Codings..... Found...\n");
		if (encoding) {
		    if (pres->encoder)
			top = (*pres->encoder)(request, param, coding, top);
		    break;
		} else if (pres->decoder) {
		    top = (*pres->decoder)(request, param, coding, top);
		    break;
		}
	    }
	}
    }
    return top;
}

/*
**  Here you can provide a complete list instead of a single token.
**  The list has to filled up in the order the _encodings_ are to be applied
*/
PUBLIC HTStream * HTEncodingStack (HTList *	encodings,
				   HTStream *	target,
				   HTRequest *	request,
				   void *	param)
{
    if (encodings) {
	HTList * cur = encodings;
	HTEncoding pres;
	HTStream * top = target;
	while ((pres = (HTEncoding) HTList_nextObject(cur)))
	    top = HTCodingStack(pres, top, request, param, YES);
	return top;
    }
    return HTErrorStream();
}

/*
**  Here you can provide a complete list instead of a single token.
**  The list has to be in the order the _encodings_ were applied - that
**  is, the same way that _encodings_ are to be applied. This is all consistent
**  with the order of the Content-Encoding header.
*/
PUBLIC HTStream * HTDecodingStack (HTList *	encodings,
				   HTStream *	target,
				   HTRequest *	request,
				   void *	param)
{
    if (encodings) {
	HTEncoding pres;
	int cnt = HTList_count(encodings);
	HTStream * top = target;
	while (cnt > 0) {
	    pres = (HTEncoding) HTList_objectAt(encodings, --cnt);
	    top = HTCodingStack(pres, top, request, param, NO);
	}
	return top;
    }
    return HTErrorStream();
}
