/*								      HTTPReq.c
**	HTTP REQUEST GENERATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module implements the output stream for HTTP used for sending
**	requests with or without a entity body.
**
** History:
**	Jan 95 HFN	Written from scratch
*/

/* Library Includes */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"

#include "HTTPGen.h"
#include "HTTPUtil.h"
#include "HTTPReq.h"					       /* Implements */

#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    const HTStreamClass *	isa;
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
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * addr = HTAnchor_physical(anchor);
    char * fullurl = HTParse(addr, "", PARSE_PATH|PARSE_PUNCTUATION);
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
    HTRqHd request_mask = HTRequest_rqHd(request);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    *crlf = CR; *(crlf+1) = LF; *(crlf+2) = '\0';

    /* Generate the HTTP/1.0 RequestLine */
    {
	HTMethod method = HTRequest_method(request);
	if (method != METHOD_INVALID) {
	    PUTS(HTMethod_name(method));
	    PUTC(' ');
	} else
	    PUTS("GET ");
    }

    /* If we are using a proxy then only take the `path' info in the URL */
    {
	char *addr = HTAnchor_physical(anchor);
	char *fullurl = HTParse(addr, "", PARSE_PATH|PARSE_PUNCTUATION);
	if (HTRequest_usingProxy(request)) {
	    PUTS(fullurl+1);
	} else {
	    PUTS(fullurl);
	}
	HT_FREE(fullurl);
    }
    PUTC(' ');
    PUTS(HTTP_VERSION);
    PUTBLOCK(crlf, 2);

    /* Request Headers */
    if (request_mask & HT_C_ACCEPT_TYPE) {
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
	    }
	}
	if (!first) PUTBLOCK(crlf, 2);
    }
    if (request_mask & HT_C_ACCEPT_CHAR) {
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
	    }
	}
	if (!first) PUTBLOCK(crlf, 2);
    }
    if (request_mask & HT_C_ACCEPT_ENC) {
	int list;
	HTList *cur;
	BOOL first=YES;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur = HTFormat_contentCoding()) != NULL)) ||
		(list && ((cur = HTRequest_encoding(request)) != NULL))) {
		HTCoding * pres;
		while ((pres = (HTCoding *) HTList_nextObject(cur))) {
		    if (first) {
			PUTS("Accept-Encoding: ");
			first = NO;
		    } else
			PUTC(',');
		    PUTS(HTCoding_name(pres));
		}
	    }
	}
	if (!first) PUTBLOCK(crlf, 2);
    }
    if (request_mask & HT_C_ACCEPT_LAN) {
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
	    }
	}
	if (!first) PUTBLOCK(crlf, 2);
    }
    {
	HTAssocList * cur = HTRequest_credentials(request);
	if (cur) {				    /* Access authentication */
	    HTAssoc * pres;
	    while ((pres = (HTAssoc *) HTAssocList_nextObject(cur))) {
		PUTS(HTAssoc_name(pres));
		PUTS(": ");
		PUTS(HTAssoc_value(pres));
		PUTBLOCK(crlf, 2);
	    }
	}
    }
    if (request_mask & HT_C_FROM) {
	HTUserProfile * up = HTRequest_userProfile(request);
	const char * mailaddress = HTUserProfile_email(up);
	if (mailaddress) {
	    PUTS("From: ");
	    PUTS(mailaddress);
	    PUTBLOCK(crlf, 2);
	}
    }
    if (request_mask & HT_C_IMS) {
	time_t lm = HTAnchor_lastModified(anchor);
	if (lm > 0) {
	    PUTS("If-Modified-Since: ");
	    PUTS(HTDateTimeStr(&lm, NO));
	    PUTBLOCK(crlf, 2);
	}
    }
    if (request_mask & HT_C_HOST) {
	char *orig = HTAnchor_address((HTAnchor *) anchor);
	char *host = HTParse(orig, "", PARSE_HOST);
#if 0
	/* Keep the port number for HTTP/1.1 compliance */
	char *ptr = strchr(host, ':');		     /* Chop off port number */
	if (ptr) *ptr = '\0';
#endif
	PUTS("Host: ");
	PUTS(host);
	PUTBLOCK(crlf, 2);
	HT_FREE(orig);
	HT_FREE(host);
    }
    if (request_mask & HT_C_REFERER) {
	HTParentAnchor * parent_anchor = HTRequest_parent(request);
	if (parent_anchor) {
	    char * act = HTAnchor_address((HTAnchor *) anchor);
	    char * parent = HTAnchor_address((HTAnchor *) parent_anchor);
	    char * relative = HTParse(parent, act,
				      PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	    if (relative && *relative) {
		PUTS("Referer: ");
		PUTS(parent);
		PUTBLOCK(crlf, 2);
	    }
	    HT_FREE(act);
	    HT_FREE(parent);
	    HT_FREE(relative);
	}
    }
    if (request_mask & HT_C_USER_AGENT) {
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
    if (PROT_TRACE)HTTrace("HTTP........ Generating Request Headers\n");
}

PRIVATE int HTTPRequest_put_block (HTStream * me, const char * b, int l)
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

PRIVATE int HTTPRequest_put_string (HTStream * me, const char * s)
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
	HT_FREE(me);
    }
    return status;
}

PRIVATE int HTTPRequest_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HT_FREE(me);
    if (PROT_TRACE) HTTrace("HTTPRequest. ABORTING...\n");
    return HT_ERROR;
}

/*	HTTPRequest Stream
**	-----------------
*/
PRIVATE const HTStreamClass HTTPRequestClass =
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
    HTNet * net = HTRequest_net(request);
    HTHost * host = HTNet_host(net);
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTTPRequest_new");
    me->isa = &HTTPRequestClass;
    me->target = target;
    me->request = request;
    me->version = HTHost_version(host);
    me->transparent = NO;

    /* Return general HTTP header stream */
    return HTTPGen_new(request, me, endHeader);
}
