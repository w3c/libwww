/*								      HTTPReq.c
**	HTTP MESSAGES GENERATION
**
**	This module implements the output stream for HTTP used for sending
**	requests with or without a entity body.
**
** History:
**	Jan 95 HFN	Written from scratch
*/

/* Library Includes */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTFormat.h"
#include "HTNetMan.h"
#include "HTDNS.h"
#include "HTTCP.h"
#include "HTAccess.h"
#include "HTWWWStr.h"
#include "HTWriter.h"
#include "HTReqMan.h"
#include "HTChunk.h"
#include "HTMIMERq.h"
#include "HTTPUtil.h"
#include "HTTPReq.h"					       /* Implements */

#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    SOCKET			sockfd;
    HTChunk *  			buffer;
    int				version;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */
/* 			    HTTP Output Request Stream			     */
/* ------------------------------------------------------------------------- */

/*	HTTP09Request
**	-------------
**	Makes a HTTP/0.9 request
*/
PRIVATE void HTTP09Request (HTStream * me, HTRequest * request)
{
    char *addr = HTAnchor_physical(request->anchor);
    char *fullurl = HTParse(addr, "", PARSE_PATH|PARSE_PUNCTUATION);
    HTChunk *header = me->buffer;
    HTChunk_puts(header, "GET ");
    HTChunk_puts(header, fullurl);
    HTChunk_putc(header, ' ');
    HTChunk_putc(header, CR);
    HTChunk_putc(header, LF);
    if (PROT_TRACE) TTYPrint(TDEST, "HTTP Tx..... %s", HTChunk_data(header));
}

/*	HTTPMakeRequest
**	---------------
**	Makes a HTTP/1.0-1.1 request header.
*/
PRIVATE void HTTPMakeRequest (HTStream * me, HTRequest * request)
{
    char linebuf[256];			/* @@@ */
    HTChunk *header = me->buffer;
    HTParentAnchor *anchor = HTRequest_anchor(request);

    /* Generate the HTTP/1.0 RequestLine */
    if (request->method != METHOD_INVALID) {
	HTChunk_puts(header, HTMethod_name(request->method));
	HTChunk_putc(header, ' ');
    } else
	HTChunk_puts(header, "GET ");

    /* If we are using a proxy then only take the `path' info in the URL */
    {
	char *addr = HTAnchor_physical(anchor);
	char *fullurl = HTParse(addr, "", PARSE_PATH|PARSE_PUNCTUATION);
	if (request->using_proxy) {
	    HTChunk_puts(header, fullurl+1);
	} else {
	    HTChunk_puts(header, fullurl);
	}
	free(fullurl);
    }
    HTChunk_puts(header, " HTTP/1.0");
    HTChunk_putc(header, CR);
    HTChunk_putc(header, LF);

    /* Request Headers */
    if (request->RequestMask & HT_C_ACCEPT_TYPE) {
	int list;
	HTList *cur;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur = HTFormat_conversion()) != NULL)) ||
		(list && ((cur = HTRequest_conversion(request)) != NULL))) {
		HTPresentation  *pres;
		while ((pres =(HTPresentation *) HTList_nextObject(cur))) {
		    if (pres->rep_out==WWW_PRESENT && pres->quality <= 1.0) {
			if (pres->quality != 1.0) {
			    sprintf(linebuf, "Accept: %s; q=%1.1f%c%c",
				    HTAtom_name(pres->rep),
				    pres->quality, CR, LF);
			} else {
			    sprintf(linebuf, "Accept: %s%c%c",
				    HTAtom_name(pres->rep), CR, LF);
			}
			HTChunk_puts(header, linebuf);
		    }
		}
	    }
	}
    }
    if (request->RequestMask & HT_C_ACCEPT_CHAR) {
	BOOL first=YES;
	int list;
	HTList *cur;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur = HTFormat_charset()) != NULL)) ||
		(list && ((cur = HTRequest_charset(request)) != NULL))) {
		HTAcceptNode *pres;
		while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
		    if (first) {
			HTChunk_puts(header, "Accept-Charset: ");
			first=NO;
		    }
		    if (cur->next)
			sprintf(linebuf, "%s,", HTAtom_name(pres->atom));
		    else
			sprintf(linebuf, "%s%c%c", HTAtom_name(pres->atom),
				CR, LF);
		    HTChunk_puts(header, linebuf);
		}
	    }
	}
    }
    if (request->RequestMask & HT_C_ACCEPT_ENC) {
	BOOL first=YES;
	int list;
	HTList *cur;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur = HTFormat_encoding()) != NULL)) ||
		(list && ((cur = HTRequest_encoding(request)) != NULL))) {
		HTAcceptNode *pres;
		while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
		    if (first) {
			HTChunk_puts(header, "Accept-Encoding: ");
			first=NO;
		    }
		    if (cur->next)
			sprintf(linebuf, "%s,", HTAtom_name(pres->atom));
		    else
			sprintf(linebuf, "%s%c%c", HTAtom_name(pres->atom),
				CR, LF);
		    HTChunk_puts(header, linebuf);
		}
	    }
	}
    }
    if (request->RequestMask & HT_C_ACCEPT_LAN) {
	BOOL first=YES;
	int list;
	HTList *cur;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur = HTFormat_language()) != NULL)) ||
		(list && ((cur = HTRequest_language(request)) != NULL))) {
		HTAcceptNode *pres;
		while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
		    if (first) {
			HTChunk_puts(header, "Accept-Language: ");
			first=NO;
		    }
		    if (cur->next)
			sprintf(linebuf, "%s,", HTAtom_name(pres->atom));
		    else
			sprintf(linebuf, "%s%c%c", HTAtom_name(pres->atom),
				CR, LF);
		    HTChunk_puts(header, linebuf);
		}
	    }
	}
    }
    if (request->authorization != NULL) {	    /* Put out authorization */
	sprintf(linebuf, "Authorization: %s%c%c", request->authorization,
		CR, LF);
	HTChunk_puts(header, linebuf);
    }
    if (request->RequestMask & HT_C_FROM) {
	CONST char *mailaddress = HTGetMailAddress();
	if (mailaddress) {
	    sprintf(linebuf, "From: %s%c%c", mailaddress, CR, LF);
	    HTChunk_puts(header, linebuf);
	}
    }
    if (request->RequestMask & HT_C_IMS) {
	time_t lm = HTAnchor_lastModified(anchor);
	if (lm != -1) {
	    sprintf(linebuf, "If-Modified-Since: %s%c%c",
		    HTDateTimeStr(&lm, NO), CR, LF);
	    HTChunk_puts(header, linebuf);
	}
    }
    if (request->RequestMask & HT_C_HOST) {
	char *orig = HTAnchor_address((HTAnchor *) anchor);
	char *host = HTParse(orig, "", PARSE_HOST);
	char *ptr = strchr(host, ':');		     /* Chop off port number */
	if (ptr) *ptr = '\0';
	sprintf(linebuf, "Host: %s%c%c", host, CR, LF);
	HTChunk_puts(header, linebuf);
	free(orig);
	free(host);
    }
    if (request->RequestMask & HT_C_REFERER && request->parentAnchor) {
	char *act = HTAnchor_address((HTAnchor *) anchor);
	char *parent = HTAnchor_address((HTAnchor *) request->parentAnchor);
	char *relative = HTParse(parent, act,
				 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	if (relative && *relative) {
	    sprintf(linebuf, "Referer: %s%c%c", parent, CR, LF);
	    HTChunk_puts(header, linebuf);
	}
	free(act);
	free(parent);
	    free(relative);
    }
    if (request->RequestMask & HT_C_USER_AGENT) {
	sprintf(linebuf, "User-Agent: %s/%s %s/%s%c%c",
		HTLib_appName(), HTLib_appVersion(),
		HTLib_name(), HTLib_version(), CR, LF);
	HTChunk_puts(header, linebuf);
    }
    if (PROT_TRACE) TTYPrint(TDEST, "HTTP Tx..... %s", header->data);
}

PRIVATE int HTTPRequest_put_block (HTStream * me, CONST char * b, int l)
{
    if (!me->target) {
	return HT_WOULD_BLOCK;
    } else if (me->transparent)
	return b ? PUTBLOCK(b, l) : HT_OK;
    else {
	int status;
	if (me->version == HTTP_09)
	    HTTP09Request(me, me->request);
	else
	    HTTPMakeRequest(me, me->request);		  /* Generate header */
	if ((status = PUTBLOCK(HTChunk_data(me->buffer),
			       HTChunk_size(me->buffer))) == HT_OK) {
	    me->transparent = YES;
	    return b ? PUTBLOCK(b, l) : HT_OK;
	}
	return status;
    }
}

PRIVATE int HTTPRequest_put_character (HTStream * me, char c)
{
    return HTTPRequest_put_block(me, &c, 1);
}

PRIVATE int HTTPRequest_put_string (HTStream * me, CONST char * s)
{
    return HTTPRequest_put_block(me, s, strlen(s));
}

/*
**	Flushes data but doesn't free stream object
*/
PRIVATE int HTTPRequest_flush (HTStream * me)
{
    int status = HTTPRequest_put_block(me, NULL, 0);
    return status==HT_OK ? (*me->target->isa->flush)(me->target) : status;
}

/*
**	Flushes data and frees stream object
*/
PRIVATE int HTTPRequest_free (HTStream * me)
{
    int status = HTTPRequest_flush(me);
    if (status != HT_WOULD_BLOCK) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	HTChunk_delete(me->buffer);
	free(me);
    }
    return status;
}

PRIVATE int HTTPRequest_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HTChunk_delete(me->buffer);
    free(me);
    if (PROT_TRACE) TTYPrint(TDEST, "HTTPRequest. ABORTING...\n");
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

PUBLIC HTStream * HTTPRequest_new (HTRequest *	request,
				   HTStream *	target)
{
    HTStream * me = (HTStream *) calloc(1, sizeof(HTStream));
    HTdns *dns = HTNet_dns(request->net);
    if (!me) outofmem(__FILE__, "HTTPRequest_new");
    me->isa = &HTTPRequestClass;
    me->target = target;
    me->request = request;
    me->buffer = HTChunk_new(512);
    me->version = HTDNS_serverVersion(dns);
    me->transparent = NO;
    return HTMIMERequest_new(request, me);		/* @@@ */
}
