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
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTStream.h"
#include "HTWWWStr.h"
#include "HTFWrite.h"
#include "HTError.h"
#include "HTFormat.h"					 /* Implemented here */

#define NO_VALUE_FOUND	-1e30		 /* Stream Stack Value if none found */

PRIVATE HTList * HTConversions = NULL;			    /* Content types */
PRIVATE HTList * HTContentCoders = NULL;		   /* Content coders */
PRIVATE HTList * HTTransferCoders = NULL;	  /* Content transfer coders */
PRIVATE HTList * HTCharsets = NULL;
PRIVATE HTList * HTLanguages = NULL;

PRIVATE double HTMaxSecs = 1e10;		/* No effective limit */

PRIVATE HTConverter * presentation_converter = NULL;

struct _HTStream {
    const HTStreamClass *	isa;
};

struct _HTCoding {
    HTEncoding		encoding;
    HTCoder *		encoder;
    HTCoder *		decoder;
    double		quality;
};

PRIVATE HTStream	HTBaseConverterStreamInstance;

/* ------------------------------------------------------------------------- */
/*				BASIC CONVERTERS			     */
/* ------------------------------------------------------------------------- */

PRIVATE int HTBlackHoleConverter_put_character (HTStream * me, char c)
{
    return HT_OK;
}

PRIVATE int HTBlackHoleConverter_put_string (HTStream * me, const char * s)
{
    return HT_OK;
}

PRIVATE int HTBlackHoleConverter_write (HTStream * me, const char * s, int l)
{
    return HT_OK;
}

PRIVATE int HTBlackHoleConverter_flush (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTBlackHoleConverter_free (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTBlackHoleConverter_abort (HTStream * me, HTList * e)
{
    return HT_ERROR;
}


PRIVATE const HTStreamClass HTBlackHoleConverterClass =
{		
    "BlackHoleConverter",
    HTBlackHoleConverter_flush,
    HTBlackHoleConverter_free,
    HTBlackHoleConverter_abort,
    HTBlackHoleConverter_put_character,
    HTBlackHoleConverter_put_string,
    HTBlackHoleConverter_write
}; 

PUBLIC HTStream * HTBlackHoleConverter (HTRequest *	request,
					void *		param,
					HTFormat	input_format,
					HTFormat	output_format,
					HTStream *	output_stream)
{
    HTTRACE(STREAM_TRACE, "BlackHole... Converter Created\n");
    HTBaseConverterStreamInstance.isa = &HTBlackHoleConverterClass;
    return &HTBaseConverterStreamInstance;
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

/*	HTSaveConverter
**
** This function is a place holder for a save to local file converter
*/
PUBLIC HTStream * HTSaveConverter (HTRequest *	request,
				   void *	param,
				   HTFormat	input_format,
				   HTFormat	output_format,
				   HTStream *	output_stream)
{
    return HTBlackHole();
}

PUBLIC HTStream * HTIdentityCoding (HTRequest *	request,
				    void *	param,
				    HTEncoding	coding,
				    HTStream *	output_stream)
{
    return output_stream;
}

/*
**	For all `accept lists' there is a local list and a global list. The
**	local list is a part of the request structure and the global list is
**	internal to the HTFormat module. The global lists can be used when
**	specifying accept lists for ALL requests and the local list can be 
**	used to add specific accept headers to the request.
*/

/* ------------------------------------------------------------------------- */
/* 	  			CONTENT TYPES				     */
/* ------------------------------------------------------------------------- */

PUBLIC void HTPresentation_setConverter (HTConverter * pconv)
{
    presentation_converter = pconv;
}

PUBLIC HTConverter * HTPresentation_converter (void)
{
    return presentation_converter;
}

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
    if (presentation_converter) {
	if ((pres = (HTPresentation  *) HT_CALLOC(1,sizeof(HTPresentation))) == NULL)
	    HT_OUTOFMEM("HTSetPresentation");
        pres->rep = HTAtom_for(representation);
	pres->rep_out = WWW_PRESENT;		/* Fixed for now ... :-) */
	pres->converter = presentation_converter;
	pres->quality = quality;
	pres->secs = secs;
	pres->secs_per_byte = secs_per_byte;
	pres->rep = HTAtom_for(representation);
	pres->command = NULL;
	StrAllocCopy(pres->command, command);
	pres->test_command = NULL;
	StrAllocCopy(pres->test_command, test_command);
	HTTRACE(CORE_TRACE, "Presentation Adding `%s\' with quality %.2f\n" _ 
		    command _ quality);
	HTList_addObject(conversions, pres);
    }
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
    HTTRACE(CORE_TRACE, "Conversions. Adding %p with quality %.2f\n" _ 
		converter _ quality);
    HTList_addObject(conversions, pres);
}

PUBLIC void HTConversion_deleteAll (HTList * list)
{
    HTPresentation_deleteAll(list);
}

/* ------------------------------------------------------------------------- */
/*		CONTENT ENCODING AND CONTENT TRANSFER ENCODING 		     */
/* ------------------------------------------------------------------------- */

PUBLIC BOOL HTCoding_add (HTList * 	list,
			  const char *	encoding,
			  HTCoder *	encoder,
			  HTCoder *	decoder,
			  double	quality)
{
    if (list && encoding && (encoder || decoder)) {
	HTCoding * me;
	if ((me = (HTCoding *) HT_CALLOC(1, sizeof(HTCoding))) == NULL)
	    HT_OUTOFMEM("HTCoding_add");
	me->encoding = HTAtom_for(encoding);
	me->encoder = encoder;
	me->decoder = decoder;
	me->quality = quality;
	HTTRACE(CORE_TRACE, "Codings..... Adding %s with quality %.2f\n" _ 
		    encoding _ quality);
	return HTList_addObject(list, (void *) me);
    }
    HTTRACE(CORE_TRACE, "Codings..... Bad argument\n");
    return NO;
}

PUBLIC void HTCoding_deleteAll (HTList * list)
{
    if (list) {
	HTList * cur = list;
	HTCoding * pres;
	while ((pres = (HTCoding *) HTList_nextObject(cur)))
	    HT_FREE(pres);
	HTList_delete(list);
    }
}

PUBLIC const char * HTCoding_name (HTCoding * me)
{
    return me ? HTAtom_name(me->encoding) : NULL;
}

PUBLIC double HTCoding_quality (HTCoding * me)
{
    return me ? me->quality : 0.0;
}

/* ------------------------------------------------------------------------- */
/* 	  			CONTENT LANGUAGE			     */
/* ------------------------------------------------------------------------- */

PUBLIC void HTLanguage_add (HTList *		list,
			    const char *	lang,
			    double		quality)
{
    HTAcceptNode * node;
    if (!list || !lang || !*lang)  {
	HTTRACE(CORE_TRACE, "Languages... Bad argument\n");
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

/* ------------------------------------------------------------------------- */
/* 	  			CONTENT CHARSET				     */
/* ------------------------------------------------------------------------- */

PUBLIC void HTCharset_add (HTList *		list,
			   const char *		charset,
			   double		quality)
{
    HTAcceptNode * node;
    if (!list || !charset || !*charset)  {
	HTTRACE(CORE_TRACE, "Charset..... Bad argument\n");
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

PUBLIC void HTFormat_addConversion (const char *	input_format,
				    const char *	output_format,
				    HTConverter *	converter,
				    double		quality,
				    double		secs, 
				    double		secs_per_byte)
{
    if (!HTConversions) HTConversions = HTList_new();
    HTConversion_add(HTConversions, input_format, output_format,
		     converter, quality, secs, secs_per_byte);
}

/*
**	Global list of Content Encodings
**	list can be NULL
*/
PUBLIC void HTFormat_setContentCoding (HTList *list)
{
    HTContentCoders = list;
}

PUBLIC HTList * HTFormat_contentCoding (void)
{
    return HTContentCoders;
}

PUBLIC BOOL HTFormat_addCoding ( char *		encoding,
				 HTCoder *	encoder,
				 HTCoder *	decoder,
				 double		quality)
{
    if (!HTContentCoders) HTContentCoders = HTList_new();
    return HTCoding_add(HTContentCoders, encoding, encoder, decoder, quality);
}

/*
**	Global list of Content Transfer Encodings
**	list can be NULL
*/
PUBLIC void HTFormat_setTransferCoding (HTList *list)
{
    HTTransferCoders = list;
}

PUBLIC HTList * HTFormat_transferCoding (void)
{
    return HTTransferCoders;
}

PUBLIC BOOL HTFormat_addTransferCoding ( char *		encoding,
					 HTCoder *	encoder,
					 HTCoder *	decoder,
					 double		quality)
{
    if (!HTTransferCoders) HTTransferCoders = HTList_new();
    return HTCoding_add(HTTransferCoders, encoding, encoder, decoder, quality);
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
    if (HTContentCoders) {
	HTCoding_deleteAll(HTContentCoders);
	HTContentCoders = NULL;
    }
    if (HTTransferCoders) {
	HTCoding_deleteAll(HTTransferCoders);
	HTTransferCoders = NULL;
    }
    if (HTCharsets) {
	HTCharset_deleteAll(HTCharsets);
	HTCharsets = NULL;
    }
}

/* ------------------------------------------------------------------------- */
/*				STREAM STACKS				     */
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
	HTTRACE(CORE_TRACE, "StreamStack. Raw output...\n");
	return output_stream ? output_stream : HTErrorStream();
    }

    if (rep_out == rep_in) {
	HTTRACE(CORE_TRACE, "StreamStack. Identical input/output format (%s)\n" _ 
		     HTAtom_name(rep_out));
	return output_stream ? output_stream : HTErrorStream();
    }

#ifdef HTDEBUG
    if (CORE_TRACE) {
	const char *p = HTAtom_name(rep_in);
	const char *q = HTAtom_name(rep_out); 
	HTTRACE(CORE_TRACE, "StreamStack. Constructing stream stack for %s to %s\n" _
		p ? p : "<NULL>" _ q ? q : "<NULL>");
    }
#endif /* HTDEBUG */

    conversion[0] = HTRequest_conversion(request);
    conversion[1] = HTConversions;

    for(which_list = 0; which_list<2; which_list++) {
	HTList * cur = conversion[which_list];
	while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
	    if ((pres->rep==rep_in || HTMIMEMatch(pres->rep, rep_in)) &&
		(pres->rep_out==rep_out || HTMIMEMatch(pres->rep_out,rep_out))){
		if (!best_match || better_match(pres->rep, best_match->rep) ||
		    (!better_match(best_match->rep, pres->rep) &&
		     pres->quality > best_quality)) {
#ifdef HAVE_SYSTEM
		    int result=0;
		    if (pres->test_command) {
			result = system(pres->test_command);
			HTTRACE(CORE_TRACE, "StreamStack. system(%s) returns %d\n" _ pres->test_command _ result);
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
	    HTTRACE(CORE_TRACE, "StreamStack. Source output\n");
	    return output_stream ? output_stream : HTErrorStream();
	}
	return (*best_match->converter)(request, best_match->command,
					rep_in, rep_out, output_stream);
    }
    if (rep_out == WWW_SOURCE) {
	HTTRACE(CORE_TRACE, "StreamStack. Source output\n");
	return output_stream ? output_stream : HTErrorStream();
    }

    HTTRACE(CORE_TRACE, "StreamStack. NOT FOUND - error!\n");
    return HTBlackHole();
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
    
    HTTRACE(CORE_TRACE, "StackValue.. Evaluating stream stack for %s worth %.3f to %s\n" _
	    HTAtom_name(rep_in) _ initial_value _ HTAtom_name(rep_out));

    if (rep_out == WWW_SOURCE || rep_out == rep_in) return 0.0;

    conversion[0] = theseConversions;
    conversion[1] = HTConversions;
    
    for(which_list = 0; which_list<2; which_list++)
     if (conversion[which_list]) {
        HTList * cur = conversion[which_list];
	HTPresentation * pres;
	while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
	    if (pres->rep == rep_in &&
		(pres->rep_out == rep_out || HTMIMEMatch(pres->rep_out, rep_out))) {
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
**	you want encoding or decoding using the BOOL "encode" flag.
*/
PUBLIC HTStream * HTContentCodingStack (HTEncoding	encoding,
					HTStream *	target,
					HTRequest *	request,
					void *		param,
					BOOL		encode)
{
    HTList * coders[2];
    HTStream * top = target;
    HTCoding * pres = NULL;
    HTCoding * best_match = NULL;
    double best_quality = -1e30;		/* Pretty bad! */
    int cnt;
    if (!encoding || !request) {
	HTTRACE(CORE_TRACE, "Codings... Nothing applied...\n");
	return target ? target : HTErrorStream();
    }
    coders[0] = HTRequest_encoding(request);
    coders[1] = HTContentCoders;
    HTTRACE(CORE_TRACE, "C-E......... Looking for `%s\'\n" _ HTAtom_name(encoding));
    for (cnt=0; cnt < 2; cnt++) {
	HTList * cur = coders[cnt];
	while ((pres = (HTCoding *) HTList_nextObject(cur))) {
	    if ((pres->encoding == encoding || HTMIMEMatch(pres->encoding, encoding)) &&
		pres->quality > best_quality) {
		best_match = pres;
		best_quality = pres->quality;
	    }
	}
    }

    if (best_match) {
	HTTRACE(CORE_TRACE, "C-E......... Found `%s\'\n" _ HTAtom_name(best_match->encoding));
	if (encode) {
	    if (best_match->encoder)
		top = (*best_match->encoder)(request, param, encoding, top);
	} else {
	    if (best_match->decoder)
		top = (*best_match->decoder)(request, param, encoding, top);
	}
    } else if (!HTFormat_isUnityContent(encoding)) {

	/*
	**  If this is not a unity coding and we didn't find any coders
	**  that could handle it then put in a local file save stream
	**  instead of the stream that we got.
	*/
	if (encode) {
	    HTTRACE(CORE_TRACE, "C-E......... NOT FOUND - can't encode stream!\n");
	} else {
	    HTTRACE(CORE_TRACE, "C-E......... NOT FOUND - error!\n");
	    top = HTBlackHole();
	}
    }
    return top;
}

/*
**  Here you can provide a complete list instead of a single token.
**  The list has to filled up in the order the _encodings_ are to be applied
*/
PUBLIC HTStream * HTContentEncodingStack (HTList *	encodings,
					  HTStream *	target,
					  HTRequest *	request,
					  void *	param)
{
    if (encodings) {
	HTList * cur = encodings;
	HTEncoding pres;
	HTStream * top = target;
	while ((pres = (HTEncoding) HTList_nextObject(cur))) {
	    top = HTContentCodingStack(pres, top, request, param, YES);
	    if (top == HTBlackHole()) break;
	}
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
PUBLIC HTStream * HTContentDecodingStack (HTList *	encodings,
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
	    top = HTContentCodingStack(pres, top, request, param, NO);
	    if (top == HTBlackHole()) break;
	}
	return top;
    }
    return HTErrorStream();
}

/*	Create a new coder and insert it into stream chain
**	--------------------------------------------------
**	Creating the transfer decoding stack is not based on quality factors as
**	we don't have the freedom as with content types. Specify whether you
**	you want encoding or decoding using the BOOL "encode" flag.
*/
PUBLIC HTStream * HTTransferCodingStack (HTEncoding	encoding,
					 HTStream *	target,
					 HTRequest *	request,
					 void *		param,
					 BOOL		encode)
{
    HTList * coders[2];
    HTStream * top = target;
    HTCoding * pres = NULL;
    int cnt;
    if (!encoding || !request) {
	HTTRACE(CORE_TRACE, "Codings... Nothing applied...\n");
	return target ? target : HTErrorStream();
    }
    coders[0] = HTRequest_transfer(request);
    coders[1] = HTTransferCoders;
    HTTRACE(CORE_TRACE, "C-E......... Looking for `%s\'\n" _ HTAtom_name(encoding));
    for (cnt=0; cnt < 2; cnt++) {
	HTList * cur = coders[cnt];
	while ((pres = (HTCoding *) HTList_nextObject(cur))) {
	    if (pres->encoding == encoding || HTMIMEMatch(pres->encoding, encoding)) {
		HTTRACE(CORE_TRACE, "C-E......... Found...\n");
		if (encode) {
		    if (pres->encoder)
			top = (*pres->encoder)(request, param, encoding, top);
		    break;
		} else if (pres->decoder) {
		    top = (*pres->decoder)(request, param, encoding, top);
		    break;
		}
	    }
	}
    }

    /*
    **  If this is not a unity coding and we didn't find any coders
    **  that could handle it then put in a local file save stream
    **  instead of the stream that we got.
    */
    if (!HTFormat_isUnityContent(encoding) && target==top) {
	if (encode) {
	    HTTRACE(CORE_TRACE, "C-E......... NOT FOUND - can't encode stream!\n");
	} else {
	    HTTRACE(CORE_TRACE, "C-E......... NOT FOUND - error!\n");
	    top = HTBlackHole();
	}
    }
    return top;
}

/*
**  Here you can provide a complete list instead of a single token.
**  The list has to filled up in the order the _encodings_ are to be applied
*/
PUBLIC HTStream * HTTransferEncodingStack (HTList *	encodings,
					   HTStream *	target,
					   HTRequest *	request,
					   void *	param)
{
    if (encodings) {
	HTList * cur = encodings;
	HTEncoding pres;
	HTStream * top = target;
	while ((pres = (HTEncoding) HTList_nextObject(cur))) {
	    top = HTTransferCodingStack(pres, top, request, param, YES);
	    if (top == HTBlackHole()) break;
	}
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
PUBLIC HTStream * HTTransferDecodingStack (HTList *	encodings,
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
	    top = HTTransferCodingStack(pres, top, request, param, NO);
	    if (top == HTBlackHole()) break;
	}
	return top;
    }
    return HTErrorStream();
}

/*	Create a new transfer coder and insert it into stream chain
**	-----------------------------------------------------------
**	Creating the content decoding stack is not based on quality factors as
**	we don't have the freedom as with content types. Specify whether you
**	you want encoding or decoding using the BOOL "encode" flag.
*/
PUBLIC HTStream * HTContentTransferCodingStack (HTEncoding	encoding,
						HTStream *	target,
						HTRequest *	request,
						void *		param,
						BOOL		encode)
{
    HTList * coders[2];
    HTStream * top = target;
    HTCoding * pres = NULL;
    int cnt;
    if (!encoding || !request) {
	HTTRACE(CORE_TRACE, "C-T-E..... Nothing applied...\n");
	return target ? target : HTErrorStream();
    }

    /*
    **  We use the same encoders/decoders as for Transfer-Encodings
    */
    coders[0] = HTRequest_transfer(request);
    coders[1] = HTTransferCoders;
    HTTRACE(CORE_TRACE, "C-T-E....... Looking for %s\n" _ HTAtom_name(encoding));
    for (cnt=0; cnt < 2; cnt++) {
	HTList * cur = coders[cnt];
	while ((pres = (HTCoding *) HTList_nextObject(cur))) {
	    if (pres->encoding == encoding) {
		HTTRACE(CORE_TRACE, "C-T-E....... Found...\n");
		if (encode) {
		    if (pres->encoder)
			top = (*pres->encoder)(request, param, encoding, top);
		    break;
		} else if (pres->decoder) {
		    top = (*pres->decoder)(request, param, encoding, top);
		    break;
		}
	    }
	}
    }

    /*
    **  If this is not a unity coding and we didn't find any coders
    **  that could handle it then put in a local file save stream
    **  instead of the stream that we got.
    */
    if (!HTFormat_isUnityTransfer(encoding) && target==top) {
	if (encode) {	    
	    HTTRACE(CORE_TRACE, "C-T-E....... NOT FOUND - removing encoding!\n");
	    HTAnchor_setContentTransferEncoding(HTRequest_anchor(request), NULL);
	} else {
	    HTTRACE(CORE_TRACE, "C-T-E....... NOT FOUND - error!\n");
	    top = HTBlackHole();
	}
    }
    return top;
}

