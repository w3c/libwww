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
    int 			state;    
    char *			url;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */
/* 			    HTTP Output Request Stream			     */
/* ------------------------------------------------------------------------- */

/*	HTTP09Request
**	-------------
**	Makes a HTTP/0.9 request
*/
PRIVATE int HTTP09Request (HTStream * me, HTRequest * request)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * addr = HTAnchor_physical(anchor);
    if (!me->url) me->url = HTParse(addr, "", PARSE_PATH|PARSE_PUNCTUATION);
    if (me->state == 0) {
	PUTS("GET ");
	me->state++;
    }
    if (me->state == 1) {
	int status = PUTS(me->url);
	if (status != HT_OK) return status;
	me->state++;
    }
    PUTC(CR);
    PUTC(LF);
    return HT_OK;
}

/*	HTTPMakeRequest
**	---------------
**	Makes a HTTP/1.0-1.1 request header.
*/
PRIVATE int HTTPMakeRequest (HTStream * me, HTRequest * request)
{
    HTMethod method = HTRequest_method(request);
    HTRqHd request_mask = HTRequest_rqHd(request);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * etag = HTAnchor_etag(anchor);
    char crlf[3];
    char qstr[10];
    *crlf = CR; *(crlf+1) = LF; *(crlf+2) = '\0';

    /* Generate the HTTP/1.x RequestLine */
    if (me->state == 0) {
	if (method != METHOD_INVALID) {
	    PUTS(HTMethod_name(method));
	    PUTC(' ');
	} else
	    PUTS("GET ");
	me->state++;
    }

    /*
    ** Generate the Request URI. If we are using full request URI then it's
    ** easy. Otherwise we must filter out the path part of the URI.
    ** In case it's a OPTIONS request then if there is no pathinfo then use
    ** a * instead.
    */
    if (me->state == 1) {	
	char * addr = HTAnchor_physical(anchor);
	int status = HT_OK;
	if (HTRequest_fullURI(request)) {
	    status = PUTS(addr);
	} else {
	    if (method == METHOD_OPTIONS) {
		/*
		** We don't preserve the final slash or lack of same through
		** out the code. This is mainly for optimization reasons
		** but it gives a problem OPTIONS. We can either send a "*"
		** or a "/" but not both. For now we send a "*".
		*/
		if (!me->url) {
		    me->url = HTParse(addr, "", PARSE_PATH|PARSE_PUNCTUATION);
		    if (!strcmp(me->url, "/")) *me->url = '*';
		}
		status = PUTS(me->url);
	    } else {
		if (!me->url)
		    me->url = HTParse(addr, "", PARSE_PATH|PARSE_PUNCTUATION);
		status = PUTS(me->url);
	    }
	}
	if (status != HT_OK) return status;
	me->state++;
    }
    PUTC(' ');

    /*
    **  Send out the version number. If we know it is a HTTP/1.0 server we
    **  are talking to then use HTTP/1.0, else use HTTP/1.1 as default version
    **  number
    */
    if (me->version == HTTP_10)
	PUTS(HTTP_VERSION_10);
    else
	PUTS(HTTP_VERSION);
    PUTBLOCK(crlf, 2);

    /* Request Headers */
    if (request_mask & HT_C_ACCEPT_TYPE) {
	/*
	** If caller has specified a specific output format then use this.
	** Otherwise use all the registered converters to generate the 
	** accept header
	*/
	if (HTRequest_outputFormat(request) == WWW_PRESENT) {
	    int list;
	    HTList *cur;
	    BOOL first=YES;
	    for (list=0; list<2; list++) {
		if ((!list && ((cur = HTFormat_conversion()) != NULL)) ||
		    (list && ((cur = HTRequest_conversion(request))!=NULL))) {
		    HTPresentation * pres;
		    while ((pres=(HTPresentation *) HTList_nextObject(cur))) {
			if (pres->rep_out==WWW_PRESENT && pres->quality<=1.0) {
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
	} else {
	    PUTS("Accept: ");
	    PUTS(HTAtom_name(HTRequest_outputFormat(request)));
	    PUTBLOCK(crlf, 2);
	}	
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
    if (request_mask & HT_C_AUTH) {
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

    /*
    **  In the "If-*" series of headers, the ones related to etags have higher
    **  priority than the date relates ones. That is, if we have a etag then
    **  use that, otherwise use the date
    */
    if (request_mask & HT_C_IF_MATCH && etag) {
	PUTS("If-Match: \"");
	PUTS(etag);
	PUTC('"');
	PUTBLOCK(crlf, 2);
	if (PROT_TRACE) HTTrace("HTTP........ If-Match using etag `%s\'\n", etag);
    } else if (request_mask & HT_C_IF_UNMOD_SINCE) {
	time_t lm = HTAnchor_lastModified(anchor);
	if (lm > 0) {
	    PUTS("If-Unmodified-Since: ");
	    PUTS(HTDateTimeStr(&lm, NO));
	    PUTBLOCK(crlf, 2);
	    if (PROT_TRACE) HTTrace("HTTP........ If-Unmodified-Since\n");
	}
    }
    if (request_mask & HT_C_IF_NONE_MATCH && etag) {
	PUTS("If-None-Match: \"");
	PUTS(etag);
	PUTC('"');
	PUTBLOCK(crlf, 2);
	if (PROT_TRACE) HTTrace("HTTP........ If-None-Match using etag `%s\'\n", etag);
    } else if (request_mask & HT_C_IMS) {
	time_t lm = HTAnchor_lastModified(anchor);
	if (lm > 0) {
	    PUTS("If-Modified-Since: ");
	    PUTS(HTDateTimeStr(&lm, NO));
	    PUTBLOCK(crlf, 2);
	    if (PROT_TRACE) HTTrace("HTTP........ If-Modified-Since\n");
	}
    }

    if (request_mask & HT_C_IF_RANGE) {

	/* Not supported */

    }
    if (request_mask & HT_C_MAX_FORWARDS) {
	int hops = HTRequest_maxForwards(request);
	if (hops >= 0) {
	    sprintf(qstr, "%d", hops);
	    PUTS("Max-Forwards: ");
	    PUTS(qstr);
	    PUTBLOCK(crlf, 2);
	}
    }
    if (request_mask & HT_C_RANGE) {

	/* Not supported */

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
    return HT_OK;
}

PRIVATE int HTTPRequest_put_block (HTStream * me, const char * b, int l)
{
    if (!me->target) {
	return HT_WOULD_BLOCK;
    } else if (me->transparent)
	return b ? PUTBLOCK(b, l) : HT_OK;
    else {
	int status = HT_OK;
	if (me->version == HTTP_09) {
	    status = HTTP09Request(me, me->request);
	    if (status != HT_OK) return status;
	} else {
	    status = HTTPMakeRequest(me, me->request);
	    if (status != HT_OK) return status;
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
	HT_FREE(me->url);
	HT_FREE(me);
    }
    return status;
}

PRIVATE int HTTPRequest_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HT_FREE(me->url);
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
