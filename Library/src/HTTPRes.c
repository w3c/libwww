/*								      HTTPRes.c
**	HTTP MESSAGES GENERATION
**
**	This module implements the output stream for HTTP used for sending
**	responces with or without a entity body. It is the server equivalent
**	to the HTTPReq module
**
** History:
**	Dec 95 HFN	Written from scratch
*/

/* Library Includes */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTWWWStr.h"
#include "HTAccess.h"
#include "HTWriter.h"
#include "HTEvntrg.h"
#include "HTNetMan.h"
#include "HTReqMan.h"
#include "HTMIMERq.h"
#include "HTTPUtil.h"
#include "HTTPRes.h"					       /* Implements */

#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    SOCKET			sockfd;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */
/* 			    HTTP Output Request Stream			     */
/* ------------------------------------------------------------------------- */

/*	HTTPMakeResponse
**	----------------
**	Makes a HTTP/1.0-1.1 response header.
*/
PRIVATE void HTTPMakeResponse (HTStream * me, HTRequest * request)
{
    char linebuf[256];
    HTParentAnchor *anchor = HTRequest_anchor(request);

    /* Generate the HTTP/1.0 ResponseLine */
    if (request->error_stack) {

	/* @@@ WRITE A SMALL HTError_response() function */
	sprintf(linebuf, "HTTP/1.0 404 Not Found%c%c", CR, LF);
	
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    } else {
	sprintf(linebuf, "HTTP/1.0 200 OK%c%c", CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));

	/* Response Headers */
	if (request->ResponseMask & HT_S_SERVER) {
	    sprintf(linebuf, "Server: %s/%s %s/%s%c%c",
		    HTLib_appName(), HTLib_appVersion(),
		    HTLib_name(), HTLib_version(), CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
    }
}

PRIVATE int HTTPResponse_put_block (HTStream * me, CONST char * b, int l)
{
    if (!me->target) {
	return HT_WOULD_BLOCK;
    } else if (me->transparent)
	return b ? PUTBLOCK(b, l) : HT_OK;
    else {
	HTTPMakeResponse(me, me->request);		  /* Generate header */
	me->transparent = YES;
	return b ? PUTBLOCK(b, l) : HT_OK;
    }
}

PRIVATE int HTTPResponse_put_character (HTStream * me, char c)
{
    return HTTPResponse_put_block(me, &c, 1);
}

PRIVATE int HTTPResponse_put_string (HTStream * me, CONST char * s)
{
    return HTTPResponse_put_block(me, s, strlen(s));
}

/*
**	Flushes data but doesn't free stream object
*/
PRIVATE int HTTPResponse_flush (HTStream * me)
{
    int status = HTTPResponse_put_block(me, NULL, 0);
    return status==HT_OK ? (*me->target->isa->flush)(me->target) : status;
}

/*
**	Flushes data and frees stream object
*/
PRIVATE int HTTPResponse_free (HTStream * me)
{
    int status = HTTPResponse_flush(me);
    if (status != HT_WOULD_BLOCK) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	free(me);
    }
    return status;
}

PRIVATE int HTTPResponse_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    free(me);
    if (PROT_TRACE) TTYPrint(TDEST, "HTTPResponse ABORTING...\n");
    return HT_ERROR;
}

/*	HTTPResponse Stream
**	-----------------
*/
PRIVATE CONST HTStreamClass HTTPResponseClass =
{		
    "HTTPResponse",
    HTTPResponse_flush,
    HTTPResponse_free,
    HTTPResponse_abort,
    HTTPResponse_put_character,
    HTTPResponse_put_string,
    HTTPResponse_put_block
};

PUBLIC HTStream * HTTPResponse_new (HTRequest *	request, HTStream * target)
{
    HTStream * me = (HTStream *) calloc(1, sizeof(HTStream));
    if (!me) outofmem(__FILE__, "HTTPResponse_new");
    me->isa = &HTTPResponseClass;
    me->target = target;
    me->request = request;
    me->transparent = NO;
    return HTMIMERequest_new(request, me);
}
