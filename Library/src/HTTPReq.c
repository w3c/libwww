/*								      HTTPReq.c
**	HTTP REQUEST GENERATION
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
#include "HTTPGen.h"
#include "HTTPUtil.h"
#include "HTTPReq.h"					       /* Implements */

#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    SOCKET			sockfd;
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
    PUTS("GET ");
    PUTS(fullurl);
    PUTC(CR);
    PUTC(LF);
}

/*	HTTPMakeRequest
**	---------------
**	Makes a HTTP/1.0-1.1 request header.
*/
PRIVATE void HTTPMakeRequest (HTStream * me, HTRequest * request)
{
    char crlf[3];
    char qstr[10];
    HTParentAnchor *anchor = HTRequest_anchor(request);
    *crlf = CR; *(crlf+1) = LF; *(crlf+2) = '\0';

    /* Generate the HTTP/1.0 RequestLine */
    if (request->method != METHOD_INVALID) {
	PUTS(HTMethod_name(request->method));
	PUTC(' ');
    } else
	PUTS("GET ");

    /* If we are using a proxy then only take the `path' info in the URL */
    {
	char *addr = HTAnchor_physical(anchor);
	char *fullurl = HTParse(addr, "", PARSE_PATH|PARSE_PUNCTUATION);
	if (request->using_proxy) {
	    PUTS(fullurl+1);
	} else {
	    PUTS(fullurl);
	}
	free(fullurl);
    }
    PUTC(' ');
    PUTS(HTTP_VERSION);
    PUTBLOCK(crlf, 2);

    /* Request Headers */
    if (request->RequestMask & HT_C_ACCEPT_TYPE) {
	int list;
	HTList *cur;
	BOOL first=YES;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur = HTFormat_conversion()) != NULL)) ||
		(list && ((cur = HTRequest_conversion(request)) != NULL))) {
		HTPresentation  *pres;
		while ((pres = (HTPresentation *) HTList_nextObject(cur))) {
		    if (pres->rep_out==WWW_PRESENT && pres->quality <= 1.0) {
			if (first) {
			    PUTS("Accept: ");
			    first=NO;
			} else
			    PUTC(',');
			PUTS(HTAtom_name(pres->rep));
			if (pres->quality != 1.0) {
			    sprintf(qstr, ";q=%1.1f", pres->quality);
			    PUTS(qstr);
			}
		    }
		}
		if (!first) PUTBLOCK(crlf, 2);
	    }
	}
    }
    if (request->RequestMask & HT_C_ACCEPT_CHAR) {
	int list;
	HTList *cur;
	BOOL first=YES;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur = HTFormat_charset()) != NULL)) ||
		(list && ((cur = HTRequest_charset(request)) != NULL))) {
		HTAcceptNode *pres;
		while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
		    if (first) {
			PUTS("Accept-Charset: ");
			first=NO;
		    } else
			PUTC(',');
		    PUTS(HTAtom_name(pres->atom));
		}
		if (!first) PUTBLOCK(crlf, 2);
	    }
	}
    }
    if (request->RequestMask & HT_C_ACCEPT_ENC) {
	int list;
	HTList *cur;
	BOOL first=YES;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur = HTFormat_encoding()) != NULL)) ||
		(list && ((cur = HTRequest_encoding(request)) != NULL))) {
		HTAcceptNode *pres;
		while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
		    if (first) {
			PUTS("Accept-Encoding: ");
			first=NO;
		    } else
			PUTC(',');
		    PUTS(HTAtom_name(pres->atom));
		    if (pres->quality != 1.0) {
			sprintf(qstr, ";q=%1.1f", pres->quality);
			PUTS(qstr);
		    }
		}
		if (!first) PUTBLOCK(crlf, 2);
	    }
	}
    }
    if (request->RequestMask & HT_C_ACCEPT_LAN) {
	int list;
	HTList *cur;
	BOOL first=YES;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur = HTFormat_language()) != NULL)) ||
		(list && ((cur = HTRequest_language(request)) != NULL))) {
		HTAcceptNode *pres;
		while ((pres = (HTAcceptNode *) HTList_nextObject(cur))) {
		    if (first) {
			PUTS("Accept-Language: ");
			first=NO;
		    } else
			PUTC(',');
		    PUTS(HTAtom_name(pres->atom));
		    if (pres->quality != 1.0) {
			sprintf(qstr, ";q=%1.1f", pres->quality);
			PUTS(qstr);
		    }
		}
		if (!first) PUTBLOCK(crlf, 2);
	    }
	}
    }
    if (request->authorization != NULL) {	    /* Put out authorization */
	PUTS("Authorization: ");
	PUTS(request->authorization);
	PUTBLOCK(crlf, 2);
    }
    if (request->RequestMask & HT_C_FROM) {
	CONST char *mailaddress = HTGetMailAddress();
	if (mailaddress) {
	    PUTS("From: ");
	    PUTS(mailaddress);
	    PUTBLOCK(crlf, 2);
	}
    }
    if (request->RequestMask & HT_C_IMS) {
	time_t lm = HTAnchor_lastModified(anchor);
	if (lm > 0) {
	    PUTS("If-Modified-Since: ");
	    PUTS(HTDateTimeStr(&lm, NO));
	    PUTBLOCK(crlf, 2);
	}
    }
    if (request->RequestMask & HT_C_HOST) {
	char *orig = HTAnchor_address((HTAnchor *) anchor);
	char *host = HTParse(orig, "", PARSE_HOST);
	char *ptr = strchr(host, ':');		     /* Chop off port number */
	if (ptr) *ptr = '\0';
	PUTS("Host: ");
	PUTS(host);
	PUTBLOCK(crlf, 2);
	free(orig);
	free(host);
    }
    if (request->RequestMask & HT_C_REFERER && request->parentAnchor) {
	char *act = HTAnchor_address((HTAnchor *) anchor);
	char *parent = HTAnchor_address((HTAnchor *) request->parentAnchor);
	char *relative = HTParse(parent, act,
				 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	if (relative && *relative) {
	    PUTS("Referer: ");
	    PUTS(parent);
	    PUTBLOCK(crlf, 2);
	}
	FREE(act);
	FREE(parent);
	FREE(relative);
    }
    if (request->RequestMask & HT_C_USER_AGENT) {
	PUTS("User-Agent: ");
	PUTS(HTLib_appName());
	PUTC('/');
	PUTS(HTLib_appVersion());
	PUTC(' ');
	PUTS(HTLib_name());
	PUTC('/');
	PUTS(HTLib_version());
	PUTBLOCK(crlf, 2);
    }
    if (PROT_TRACE)TTYPrint(TDEST,"HTTP........ Generating Request Headers\n");
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
	else {
	    HTTPMakeRequest(me, me->request);		  /* Generate header */
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
	free(me);
    }
    return status;
}

PRIVATE int HTTPRequest_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
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

PUBLIC HTStream * HTTPRequest_new (HTRequest * request, HTStream * target,
				   BOOL endHeader)
{
    HTStream * me = (HTStream *) calloc(1, sizeof(HTStream));
    HTdns *dns = HTNet_dns(request->net);
    if (!me) outofmem(__FILE__, "HTTPRequest_new");
    me->isa = &HTTPRequestClass;
    me->target = target;
    me->request = request;
    me->version = HTDNS_serverVersion(dns);
    me->transparent = NO;

    /* Return general HTTP header stream */
    return HTTPGen_new(request, me, endHeader);
}
