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
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"

#include "HTTPGen.h"
#include "HTTPUtil.h"
#include "HTDNS.h"
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
	int status = PUTS("GET ");
	if (status != HT_OK) return status;
	me->state++;
    }
    if (me->state == 1) {
	int status = PUTS(me->url);
	if (status != HT_OK) return status;
	me->state++;
    }
    PUTC(CR);
    PUTC(LF);
    HTTRACE(PROT_TRACE, "HTTP........ Generating HTTP/0.9 Request\n");
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
    **  Generate the Request URI. If we are using full request URI then it's
    **  easy. Otherwise we must filter out the path part of the URI.
    **  In case it's a OPTIONS request then if there is no pathinfo then use
    **  a * instead. If we use a method different from GET or HEAD then use
    **  the content-location if available.
    */
    if (me->state == 1) {
	char * abs_location = NULL;
	char * addr = HTAnchor_physical(anchor);
	char * location;

        /* JK: If the application specified a content-location (which is
           stored in the request in default put-name!), we use it instead
           of the URL that's being saved to. This is like having a user
           defined Content-Location */
        location = HTRequest_defaultPutName (request);
        if (location)
	  {
	    if (HTURL_isAbsolute (location))
	      {
		char * relative;
		relative = HTRelative (location, location);
		abs_location = HTParse (relative + 2, addr, PARSE_ALL);
		HT_FREE (relative);
	      }
	    else
	      abs_location = HTParse (location, addr, PARSE_ALL);
	    addr = abs_location;
	  }

#if 0
	/*
	**  We don't use the content-location any more as it is superseeded
	**  by etags and the combination of the two might do more harm than
	**  good (The etag is not guaranteed to be unique over multiple URIs)
	*/

	/*
	**  If we are using a method different from HEAD and GET then use
	**  the Content-Location if available, else the Request-URI.
	*/
	if (!HTMethod_isSafe(method)) {
	    char * location = HTAnchor_location(anchor);
	    if (location) {
		if (HTURL_isAbsolute(location))
		    addr = location;
		else {
		    /*
		    **  We have a content-location but it is relative and
		    **  must expand it either to the content-base or to
		    **  the Request-URI itself.
		    */
		    char * base = HTAnchor_base(anchor);
		    abs_location = HTParse(location, base, PARSE_ALL);
		    addr = abs_location;
		}
	    }
	}
#endif

	/*
	**  If we are using a proxy or newer versions of HTTP then we can
	**  send the full URL. Otherwise we only send the path.
	*/
	if (HTRequest_fullURI(request))
	    StrAllocCopy(me->url, addr);
	else {
	    me->url = HTParse(addr, "", PARSE_PATH | PARSE_PUNCTUATION);
	    if (method == METHOD_OPTIONS) {
		/*
		** We don't preserve the final slash or lack of same through
		** out the code. This is mainly for optimization reasons
		** but it gives a problem OPTIONS. We can either send a "*"
		** or a "/" but not both. For now we send a "*".
		*/
		if (!strcmp(me->url, "/")) *me->url = '*';
	    }
	}
	HT_FREE(abs_location);
	me->state++;
    }

    /*
    **  Now send the URL that we have put together
    */
    if (me->state == 2) {
	int status = HT_OK;
	if ((status = PUTS(me->url)) != HT_OK) return status;
	me->state++;

#if 0
	fprintf(stderr, "Requesting '%s'\n", me->url);
#endif

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
	HTFormat format = HTRequest_outputFormat(request);
	
	/*
	** If caller has specified a specific output format then use this.
	** Otherwise use all the registered converters to generate the 
	** accept header
	*/
	if (format == WWW_PRESENT) {
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
			    if (pres->quality < 1.0 && pres->quality >= 0.0) {
				sprintf(qstr, ";q=%1.1f", pres->quality);
				PUTS(qstr);
			    }
			}
		    }
		}
	    }
	    if (!first) PUTBLOCK(crlf, 2);
	} else {

	    /*
	    **  If we have an explicit output format then only send
	    **  this one if not this is an internal libwww format
	    **	of type www/<star>
	    */
	    if (!HTMIMEMatch(WWW_INTERNAL, format)) {
		PUTS("Accept: ");
		PUTS(HTAtom_name(format));
		PUTBLOCK(crlf, 2);
	    }
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
		    if (pres->quality < 1.0 && pres->quality >= 0.0) {
			sprintf(qstr, ";q=%1.1f", pres->quality);
			PUTS(qstr);
		    }
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
		    double quality = HTCoding_quality(pres);
		    if (first) {
			PUTS("Accept-Encoding: ");
			first = NO;
		    } else
			PUTC(',');
		    PUTS(HTCoding_name(pres));
		    if (quality < 1.0 && quality >= 0.0) {
			sprintf(qstr, ";q=%1.1f", quality);
			PUTS(qstr);
		    }
		}
	    }
	}
	if (!first) PUTBLOCK(crlf, 2);
    }
    if (request_mask & HT_C_ACCEPT_TE) {
	int list;
	HTList *cur;
	BOOL first=YES;
	for (list=0; list<2; list++) {
	    if ((!list && ((cur = HTFormat_transferCoding()) != NULL)) ||
		(list && ((cur = HTRequest_transfer(request)) != NULL))) {
		HTCoding * pres;
		while ((pres = (HTCoding *) HTList_nextObject(cur))) {
		    double quality = HTCoding_quality(pres);
		    const char * coding = HTCoding_name(pres);
		    if (first) {
			PUTS("TE: ");
			first = NO;
		    } else
			PUTC(',');

		    /* Special check for "chunked" which is translated to "trailers" */
		    if (!strcasecomp(coding, "chunked"))
			PUTS("trailers");
		    else
			PUTS(coding);
		    if (quality < 1.0 && quality >= 0.0) {
			sprintf(qstr, ";q=%1.1f", quality);
			PUTS(qstr);
		    }
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
		    if (pres->quality < 1.0 && pres->quality >= 0.0) {
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
    if (request_mask & HT_C_EXPECT) {
	HTAssocList * cur = HTRequest_expect(request);
	if (cur) {
	    BOOL first=YES;
	    HTAssoc * pres;
	    while ((pres = (HTAssoc *) HTAssocList_nextObject(cur))) {
		char * value = HTAssoc_value(pres);
		if (first) {
		    PUTS("Expect: ");
		    first = NO;
		} else
		    PUTC(',');

		/* Output the name */
		PUTS(HTAssoc_name(pres));

		/* Only output the value if not empty string */
		if (*value) {
		    PUTS("=");
		    PUTS(value);
		}
	    }
	    PUTBLOCK(crlf, 2);
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
	char hostace[256];
#if 0
	/* Keep the port number for HTTP/1.1 compliance */
	char *ptr = strchr(host, ':');		     /* Chop off port number */
	if (ptr) *ptr = '\0';
#endif
        PUTS("Host: ");
    /****** still have to check UTF8toACE with port number */
    if (!HTACEfromUTF8 (host, hostace, 255)) {
	    PUTS(hostace);
	}
	else {
	    PUTS(host); /* this may be dangerous, but helps server side debugging */
        HTTRACE(PROT_TRACE, "HTTP........ Error: Cannot convert to ACE: `%s\'\n" _ host);
	}
	PUTBLOCK(crlf, 2);
	HT_FREE(orig);
	HT_FREE(host);
    }

    /*
    **  In the "If-*" series of headers, the ones related to etags have higher
    **  priority than the date relates ones. That is, if we have a etag then
    **  use that, otherwise use the date. First we check for range, match, and
    **  unmodified-since.
    */
    if (request_mask & HT_C_IF_RANGE && etag) {
	PUTS("If-Range: \"");
	PUTS(etag);
	PUTC('"');
	PUTBLOCK(crlf, 2);
	HTTRACE(PROT_TRACE, "HTTP........ If-Range using etag `%s\'\n" _ etag);
    } else if (request_mask & HT_C_IF_MATCH_ANY) {
	PUTS("If-Match: *");
	PUTBLOCK(crlf, 2);
	HTTRACE(PROT_TRACE, "HTTP........ If-Match using `*\'\n");
    } else if (request_mask & HT_C_IF_MATCH && etag) {
	PUTS("If-Match: \"");
	PUTS(etag);
	PUTC('"');
	PUTBLOCK(crlf, 2);
	HTTRACE(PROT_TRACE, "HTTP........ If-Match using etag `%s\'\n" _ etag);
    } else if (request_mask & HT_C_IF_UNMOD_SINCE) {
	time_t lm = HTAnchor_lastModified(anchor);
	if (lm > 0) {
	    PUTS("If-Unmodified-Since: ");
	    PUTS(HTDateTimeStr(&lm, NO));
	    PUTBLOCK(crlf, 2);
	    HTTRACE(PROT_TRACE, "HTTP........ If-Unmodified-Since `%s\'\n" _ HTDateTimeStr(&lm, NO));
	}
    }

    /*
    **  If-None-Match and If-Modified-Since are equivalent except that the
    **  first uses etags and the second uses dates. Etags have precedence over
    **  dates.
    */
    if (request_mask & HT_C_IF_NONE_MATCH_ANY) {
	PUTS("If-None-Match: *");
	PUTBLOCK(crlf, 2);
	HTTRACE(PROT_TRACE, "HTTP........ If-None-Match using `*\'\n");
    } else if (request_mask & HT_C_IF_NONE_MATCH && etag) {
	PUTS("If-None-Match: \"");
	PUTS(etag);
	PUTC('"');
	PUTBLOCK(crlf, 2);
	HTTRACE(PROT_TRACE, "HTTP........ If-None-Match `%s\'\n" _ etag);
    }
    if (request_mask & HT_C_IMS) {
	time_t lm = HTAnchor_lastModified(anchor);
	if (lm > 0) {
	    PUTS("If-Modified-Since: ");
	    PUTS(HTDateTimeStr(&lm, NO));
	    PUTBLOCK(crlf, 2);
	    HTTRACE(PROT_TRACE, "HTTP........ If-Modified-Since `%s\'\n" _ HTDateTimeStr(&lm, NO));
	}
    }

    /*
    **  Max forwards is mainly for TRACE where we want to be able to stop the
    **  TRACE at a specific location un the message path.
    */
    if (request_mask & HT_C_MAX_FORWARDS) {
	int hops = HTRequest_maxForwards(request);
	if (hops >= 0) {
	    sprintf(qstr, "%d", hops);
	    PUTS("Max-Forwards: ");
	    PUTS(qstr);
	    PUTBLOCK(crlf, 2);
	}
    }

    /*
    **  Range requests. For now, we only take the first entry registered for
    **  this request. This means that you can only send a single "unit" and
    **  then a set of range within this unit. This is in accordance with 
    **  HTTP/1.1. Multiple units will go on multiple lines.
    */
    if (request_mask & HT_C_RANGE) {
	HTAssocList * cur = HTRequest_range(request);
	if (cur) {				    	   /* Range requests */
	    HTAssoc * pres;
	    while ((pres = (HTAssoc *) HTAssocList_nextObject(cur))) {
		PUTS("Range: ");
		PUTS(HTAssoc_name(pres));			     /* Unit */
		PUTS("=");
		PUTS(HTAssoc_value(pres));	  /* Ranges within this unit */
		PUTBLOCK(crlf, 2);
	    }
	}
    }
    if (request_mask & HT_C_REFERER) {
	HTParentAnchor * parent_anchor = HTRequest_parent(request);
	if (parent_anchor) {
	    char * act = HTAnchor_address((HTAnchor *) anchor);
	    char * parent = HTAnchor_address((HTAnchor *) parent_anchor);
#if 1
	    char * relative = HTRelative(parent, act);
#else
	    char * relative = HTParse(parent, act,
				      PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
#endif
	    if (relative && *relative) {
		PUTS("Referer: ");
		PUTS(relative);
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
    HTTRACE(PROT_TRACE, "HTTP........ Generating HTTP/1.x Request Headers\n");
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
    HTTRACE(PROT_TRACE, "HTTPRequest. ABORTING...\n");
    /* JK: Added protection against NULL pointers */
    if (me) {
	if (me->target && me->target->isa)
	  (*me->target->isa->abort)(me->target, e);
	if (me->url)
	  HT_FREE(me->url);
	HT_FREE(me);
    }
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
				   BOOL endHeader, int version)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTTPRequest_new");
    me->isa = &HTTPRequestClass;
    me->target = target;
    me->request = request;
    me->version = version;
    me->transparent = NO;

    /*
    ** If sending a body in the request then we want a 100 code!
    */
    if (HTMethod_hasEntity(HTRequest_method(request)))
	HTRequest_addExpect(request, "100-continue", "");


    /* Return general HTTP header stream */
    return HTTPGen_new(request, me, endHeader, version);
}
