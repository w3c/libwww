/*								      HTTPRes.c
**	HTTP RESPONSE GENERATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module implements the output stream for HTTP used for sending
**	responces with or without a entity body. It is the server equivalent
**	to the HTTPReq module
**
** History:
**	Dec 95 HFN	Written from scratch
*/

/* Library Includes */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTNetMan.h"
#include "HTTPGen.h"
#include "HTTPUtil.h"
#include "HTTPRes.h"					       /* Implements */

#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */
/* 			    HTTP Output Request Stream			     */
/* ------------------------------------------------------------------------- */

/*	HTTPMakeResponse
**	----------------
**	Makes a HTTP/1.0-1.1 response header.
*/
PRIVATE int HTTPMakeResponse (HTStream * me, HTRequest * request)
{
    char crlf[3];
    HTRsHd response_mask = HTRequest_rsHd(request);
    *crlf = CR; *(crlf+1) = LF; *(crlf+2) = '\0';

    if (response_mask & HT_S_LOCATION) {		/* @@@ */

    }
    if (response_mask & HT_S_PROXY_AUTH) {		/* @@@ */

    }
    if (response_mask & HT_S_PUBLIC) {			/* @@@ */

    }
    if (response_mask & HT_S_RETRY_AFTER) {		/* @@@ */

    }
    if (response_mask & HT_S_SERVER) {
	PUTS("Server: ");
	PUTS(HTLib_appName());
	PUTC('/');
	PUTS(HTLib_appVersion());
	PUTC(' ');
	PUTS(HTLib_name());
	PUTC('/');
	PUTS(HTLib_version());
	PUTBLOCK(crlf, 2);
    }
    if (response_mask & HT_S_WWW_AUTH) {		/* @@@ */

    }
    HTTRACE(PROT_TRACE, "HTTP........ Generating Response Headers\n");
    return HT_OK;
}

PRIVATE int HTTPResponse_put_block (HTStream * me, const char * b, int l)
{
    if (me->target) {
	if (me->transparent)
	    return PUTBLOCK(b, l);
	else {
	    HTTPMakeResponse(me, me->request);		  /* Generate header */
	    me->transparent = YES;
	    return b ? PUTBLOCK(b, l) : HT_OK;
	}
    }
    return HT_WOULD_BLOCK;
}

PRIVATE int HTTPResponse_put_character (HTStream * me, char c)
{
    return HTTPResponse_put_block(me, &c, 1);
}

PRIVATE int HTTPResponse_put_string (HTStream * me, const char * s)
{
    return HTTPResponse_put_block(me, s, strlen(s));
}

/*
**	Flushes data but doesn't free stream object
*/
PRIVATE int HTTPResponse_flush (HTStream * me)
{
    if (!me->transparent) {
	int status = HTTPMakeResponse(me, me->request);
	if (status != HT_OK) return status;
    }
    return (*me->target->isa->flush)(me->target);
}

/*
**	Flushes data and frees stream object
*/
PRIVATE int HTTPResponse_free (HTStream * me)
{
    if (me->target) {
	int status;
	if (!me->transparent)
	    if ((status = HTTPMakeResponse(me, me->request)) != HT_OK)
		return status;
	if ((status = (*me->target->isa->_free)(me->target)) != HT_OK)
	    return status;
    }
    return HT_OK;
}

PRIVATE int HTTPResponse_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HTTRACE(PROT_TRACE, "HTTPResponse ABORTING...\n");
    return HT_ERROR;
}

/*	HTTPResponse Stream
**	-----------------
*/
PRIVATE const HTStreamClass HTTPResponseClass =
{		
    "HTTPResponse",
    HTTPResponse_flush,
    HTTPResponse_free,
    HTTPResponse_abort,
    HTTPResponse_put_character,
    HTTPResponse_put_string,
    HTTPResponse_put_block
};

/*
**	This stream generates server specific headers
*/
PUBLIC HTStream * HTTPResponse_new (HTRequest *	request, HTStream * target,
				    BOOL endHeader, int version)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTTPResponse_new");
    me->isa = &HTTPResponseClass;
    me->target = target;
    me->request = request;
    me->transparent = NO;
    return HTTPGen_new(request, me, endHeader, version);
}
