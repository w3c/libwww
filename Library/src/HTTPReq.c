/*								      HTTPReq.c
**	MULTITHREADED IMPLEMENTATION OF HTTP CLIENT
**
**	This module implements the output stream for HTTP used for sending
**	requests with or without a entity body.
**
** History:
**	Jan 95 HFN	Written from scratch
**
*/

/* Library Includes */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTFormat.h"
#include "HTThread.h"
#include "HTTCP.h"
#include "HTWriter.h"
#include "HTChunk.h"
#include "HTTPReq.h"					       /* Implements */

/* Type definitions and global variables etc. local to this module */ 
extern char * HTAppName;		  /* Application name: please supply */
extern char * HTAppVersion;	       /* Application version: please supply */
PUBLIC char * HTProxyHeaders = NULL;		    /* Headers to pass as-is */

/* Macros and other defines */
#define HTTP_VERSION	"HTTP/1.0"
#define MIME_VERSION	"MIME/1.0"
#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)
#define FREE_TARGET	
#define ABORT_TARGET	(*me->target->isa->abort)(me->target, e)

/* Type definitions and global variables etc. local to this module */

PRIVATE char linebuf[256];	/* @@@ */

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    SOCKFD			sockfd;
    HTChunk *  			buffer;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */
/* 			    HTTP Output Request Stream			     */
/* ------------------------------------------------------------------------- */

/*                                                              HTTPMakeRequest
**
**	This function composes the HTTP request header.
*/
PRIVATE void HTTPMakeRequest ARGS2(HTStream *, me, HTRequest *, request)
{
    HTChunk *header = me->buffer;
    HTParentAnchor *entity =
	(request->source && request->source->anchor) ?
	    request->source->anchor : request->anchor;

    /* Generate the HTTP/1.0 RequestLine */
    if (request->method != METHOD_INVALID) {
	HTChunkPuts(header, HTMethod_name(request->method));
	HTChunkPutc(header, ' ');
    } else
	HTChunkPuts(header, "GET ");

    /* If we are using a proxy then only take the `path' info in the URL */
    {
	char *addr = HTAnchor_physical(request->anchor);
	char *fullurl = HTParse(addr, "", PARSE_PATH|PARSE_PUNCTUATION);
	if (request->using_proxy) {
	    HTChunkPuts(header, fullurl+1);
	} else {
	    HTChunkPuts(header, fullurl);
	}
	free(fullurl);
    }
    HTChunkPutc(header, ' ');
    HTChunkPuts(header, HTTP_VERSION);
    HTChunkPutc(header, CR);
    HTChunkPutc(header, LF);

    /* General Headers */
    if (request->GenMask & HT_DATE) {
	time_t local = time(NULL);
	sprintf(linebuf, "Date: %s%c%c", HTDateTimeStr(&local, NO), CR,LF);
	HTChunkPuts(header, linebuf);
    }
    if (request->GenMask & HT_FORWARDED) {		/* @@@@@@ */
    }
    if (request->GenMask & HT_MESSAGE_ID) {
	CONST char *msgid = HTMessageIdStr();
	if (msgid) {
	    sprintf(linebuf, "Message-ID: %s%c%c", msgid, CR, LF);
	    HTChunkPuts(header, linebuf);
	}
    }
    if (request->GenMask & HT_MIME) {
	sprintf(linebuf, "MIME-Version: %s%c%c", MIME_VERSION, CR, LF);
	HTChunkPuts(header, linebuf);
    }

    /* Request Headers */
    if (request->RequestMask & HT_ACCEPT_TYPE) {
	int list;
	HTList *cur;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur=HTConversions) != NULL)) ||
		(list && ((cur=request->conversions) != NULL))) {
		HTPresentation  *pres;
		while ((pres =(HTPresentation *) HTList_nextObject(cur))) {
		    if (pres->rep_out == WWW_PRESENT) {
			if (pres->quality != 1.0) {
			    sprintf(linebuf, "Accept: %s; q=%1.1f%c%c",
				    HTAtom_name(pres->rep),
				    pres->quality, CR, LF);
			} else {
			    sprintf(linebuf, "Accept: %s%c%c",
				    HTAtom_name(pres->rep), CR, LF);
			}
			HTChunkPuts(header, linebuf);
		    }
		}
	    }
	}
    }
    if (request->RequestMask & HT_ACCEPT_CHAR) {
	BOOL first=YES;
	int list;
	HTList *cur;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur=HTCharsets) != NULL)) ||
		(list && ((cur=request->charsets) != NULL))) {
		HTAcceptNode *pres;
		while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
		    if (first) {
			HTChunkPuts(header, "Accept-Charset: ");
			first=NO;
		    }
		    if (cur->next)
			sprintf(linebuf, "%s,", HTAtom_name(pres->atom));
		    else
			sprintf(linebuf, "%s%c%c", HTAtom_name(pres->atom),
				CR, LF);
		    HTChunkPuts(header, linebuf);
		}
	    }
	}
    }
    if (request->RequestMask & HT_ACCEPT_ENC) {
	BOOL first=YES;
	int list;
	HTList *cur;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur=HTEncodings) != NULL)) ||
		(list && ((cur=request->encodings) != NULL))) {
		HTAcceptNode *pres;
		while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
		    if (first) {
			HTChunkPuts(header, "Accept-Encoding: ");
			first=NO;
		    }
		    if (cur->next)
			sprintf(linebuf, "%s,", HTAtom_name(pres->atom));
		    else
			sprintf(linebuf, "%s%c%c", HTAtom_name(pres->atom),
				CR, LF);
		    HTChunkPuts(header, linebuf);
		}
	    }
	}
    }
    if (request->RequestMask & HT_ACCEPT_LAN) {
	BOOL first=YES;
	int list;
	HTList *cur;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur=HTLanguages) != NULL)) ||
		(list && ((cur=request->languages) != NULL))) {
		HTAcceptNode *pres;
		while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
		    if (first) {
			HTChunkPuts(header, "Accept-Language: ");
			first=NO;
		    }
		    if (cur->next)
			sprintf(linebuf, "%s,", HTAtom_name(pres->atom));
		    else
			sprintf(linebuf, "%s%c%c", HTAtom_name(pres->atom),
				CR, LF);
		    HTChunkPuts(header, linebuf);
		}
	    }
	}
    }
    if (request->authorization != NULL) {	    /* Put out authorization */
	sprintf(linebuf, "Authorization: %s%c%c", request->authorization,
		CR, LF);
	HTChunkPuts(header, linebuf);
    }
    if (request->RequestMask & HT_FROM) {
	CONST char *mailaddress = HTGetMailAddress();
	if (mailaddress) {
	    sprintf(linebuf, "From: %s%c%c", mailaddress, CR, LF);
	    HTChunkPuts(header, linebuf);
	}
    }
    if (request->RequestMask & HT_PRAGMA) {
	sprintf(linebuf, "Pragma: %s%c%c", "no-cache", CR, LF);
	HTChunkPuts(header, linebuf);
    }
    if (request->RequestMask & HT_REFERER && request->parentAnchor) {
	char *act = HTAnchor_address((HTAnchor *) request->anchor);
	char *parent = HTAnchor_address((HTAnchor *) request->parentAnchor);
	char *relative = HTParse(parent, act,
				 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	if (relative && *relative) {
	    sprintf(linebuf, "Referer: %s%c%c", parent, CR, LF);
	    HTChunkPuts(header, linebuf);
	}
	free(act);
	free(parent);
	    free(relative);
    }
    if (request->RequestMask & HT_USER_AGENT) {
	sprintf(linebuf, "User-Agent: %s/%s libwww/%s%c%c",
		HTAppName ? HTAppName : "unknown",
		HTAppVersion ? HTAppVersion : "0.0",
		HTLibraryVersion, CR, LF);
	HTChunkPuts(header, linebuf);
    }

    /* Now put out entity headers if we are using PUT or POST. If we have a
    ** PostAnchor then we take the information from this and uses the
    ** destination anchor to contain the reply. Otherwise, we have created an
    ** anchor (using internal editing etc) and we can use the destination
    ** anchor directly.
    */
    if (request->method==METHOD_PUT || request->method==METHOD_POST) {
	if (request->EntityMask & HT_ALLOW) {		/* @@@@@@@@@@ */

	}
	if (request->EntityMask & HT_CONTENT_ENCODING &&
	    entity->content_encoding) {
	    sprintf(linebuf, "Content-Encoding: %s%c%c",
		    HTAtom_name(entity->content_encoding), CR, LF);
	    HTChunkPuts(header, linebuf);
	}

	/* @@@ SHOULD BE A LIST @@@ */
	if (request->EntityMask & HT_CONTENT_LANGUAGE &&
	    entity->content_language) {
	    sprintf(linebuf, "Content-Language: %s%c%c",
		    HTAtom_name(entity->content_language), CR, LF);
	    HTChunkPuts(header, linebuf);
	}
	if (request->EntityMask & HT_CONTENT_LENGTH) {   /* Must be there!!! */
	    sprintf(linebuf, "Content-Length: %ld%c%c",
		    entity->content_length, CR, LF);
	    HTChunkPuts(header, linebuf);	
	}
	if (request->EntityMask & HT_CTE && entity->cte) {
	    sprintf(linebuf, "Content-Transfer-Encoding: %s%c%c",
		    HTAtom_name(entity->cte), CR, LF);
	    HTChunkPuts(header, linebuf);
	}
	if (request->EntityMask & HT_CONTENT_TYPE && entity->content_type) {
	    sprintf(linebuf, "Content-Type: %s",
		    HTAtom_name(entity->content_type));
	    if (entity->charset) {
		strcat(linebuf, "; charset=");
		strcat(linebuf, HTAtom_name(entity->charset));
	    }
	    if (entity->level) {
		strcat(linebuf, "; level=");
		strcat(linebuf, HTAtom_name(entity->level));
	    }
	    HTChunkPuts(header, linebuf);
	    HTChunkPutc(header, CR);
	    HTChunkPutc(header, LF);
	}
	if (request->EntityMask & HT_DERIVED_FROM && entity->derived_from) {
	    sprintf(linebuf, "Derived-From: %s%c%c", entity->derived_from,
		    CR, LF);
	    HTChunkPuts(header, linebuf);
	}
	if (request->EntityMask & HT_EXPIRES) {		/* @@@@@@@@@@ */

	}
	if (request->EntityMask & HT_LAST_MODIFIED) {	/* @@@@@@@@@@ */

	}
	if (request->EntityMask & HT_LINK) {		/* @@@@@@@@@@ */

	}
	if (request->EntityMask & HT_TITLE) {		/* @@@@@@@@@@ */

	}
	if (request->EntityMask & HT_URI) {		/* @@@@@@@@@@ */

	}
	if (request->EntityMask & HT_VERSION && entity->version) {
	    sprintf(linebuf, "Version: %s%c%c", entity->version, CR, LF);
	    HTChunkPuts(header, linebuf);
	}
    }

    /* Put out extra information if any */
    if (request->ExtraHeaders)
	HTChunkPuts(header, request->ExtraHeaders);
    
    HTChunkPutc(header, CR);			   /* Blank line means "end" */
    HTChunkPutc(header, LF);
    HTChunkTerminate(header);
    if (PROT_TRACE)
	fprintf(TDEST, "HTTP Tx..... %s", header->data);
}

PRIVATE int HTTPRequest_put_character ARGS2(HTStream *, me, char, c)
{
    if (!me->target) {
	return HT_WOULD_BLOCK;
    } else if (me->transparent)
	return PUTC(c);
    else {
	int status;
	HTTPMakeRequest(me, me->request);		  /* Generate header */
	if ((status=PUTBLOCK(me->buffer->data, me->buffer->size-1)) == HT_OK) {
	    me->transparent = YES;
	    return PUTC(c);
	}
	return status;
    }
}

PRIVATE int HTTPRequest_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    if (!me->target) {
	return HT_WOULD_BLOCK;
    } else if (me->transparent)
	return PUTS(s);
    else {
	int status;
	HTTPMakeRequest(me, me->request);		  /* Generate header */
	if ((status=PUTBLOCK(me->buffer->data, me->buffer->size-1)) == HT_OK) {
	    me->transparent = YES;
	    return PUTS(s);
	}
	return status;
    }
}

PRIVATE int HTTPRequest_put_block ARGS3(HTStream *, me, CONST char*, b, int, l)
{
    if (!me->target) {
	return HT_WOULD_BLOCK;
    } else if (me->transparent)
	return PUTBLOCK(b, l);
    else {
	int status;
	HTTPMakeRequest(me, me->request);		  /* Generate header */
	if ((status=PUTBLOCK(me->buffer->data, me->buffer->size-1)) == HT_OK) {
	    me->transparent = YES;
	    return PUTBLOCK(b, l);
	}
	return status;
    }
}

/*
**	Flushes data but doesn't free stream object
*/
PRIVATE int HTTPRequest_flush ARGS1(HTStream *, me)
{
    if (!me->target) {
	return HT_WOULD_BLOCK;
    } else if (!me->transparent) {
	int status;
	HTTPMakeRequest(me, me->request);		  /* Generate header */
	if ((status=PUTBLOCK(me->buffer->data, me->buffer->size-1)) == HT_OK)
	    me->transparent = YES;
	else
	    return status;
    }
    return HT_OK;
}

/*
**	Flushes data and frees stream object
*/
PRIVATE int HTTPRequest_free ARGS1(HTStream *, me)
{
    int status;
    if (!me->target) {
	return HT_WOULD_BLOCK;
    } else if (!me->transparent) {
	HTTPMakeRequest(me, me->request);		  /* Generate header */
	if ((status = PUTBLOCK(me->buffer->data, me->buffer->size-1)) == HT_OK)
	    me->transparent = YES;
	else
	    return status;
    }
    if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	return HT_WOULD_BLOCK;
    HTChunkFree(me->buffer);
    free(me);
    return status;
}

PRIVATE int HTTPRequest_abort ARGS2(HTStream *, me, HTError, e)
{
    if (me->target)
	ABORT_TARGET;
    HTChunkFree(me->buffer);
    free(me);
    if (PROT_TRACE)
	fprintf(TDEST, "HTTPRequest. ABORTING...\n");
    return HT_ERROR;
}

/*	HTTPRequest Stream
**	-----------------
*/
PRIVATE CONST HTStreamClass HTTPRequestClass =
{		
    "HTTPRequest",
    HTTPRequest_flush,
    HTTPRequest_free,
    HTTPRequest_abort,
    HTTPRequest_put_character,
    HTTPRequest_put_string,
    HTTPRequest_put_block
};

PUBLIC HTStream * HTTPRequest_new ARGS2(HTRequest *,	request,
					HTStream *,	target)
{
    HTStream * me = (HTStream *) calloc(1, sizeof(HTStream));
    if (!me) outofmem(__FILE__, "HTTPRequest_new");
    me->isa = &HTTPRequestClass;
    me->target = target;
    me->request = request;
    me->buffer = HTChunkCreate(512);
    me->transparent = NO;
    return me;
}


