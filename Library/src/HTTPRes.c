/*								      HTTPRes.c
**	HTTP RESPONSE GENERATION
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
#include "HTFWrite.h"
#include "HTEvntrg.h"
#include "HTNetMan.h"
#include "HTReqMan.h"
#include "HTTPGen.h"
#include "HTTPUtil.h"
#include "HTTPRes.h"					       /* Implements */

#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    CONST HTStreamClass *	isa;
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
    char linebuf[256];
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
    if(PROT_TRACE)TTYPrint(TDEST,"HTTP........ Generating Response Headers\n");
    return HT_OK;
}

PRIVATE int HTTPResponse_put_block (HTStream * me, CONST char * b, int l)
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

PRIVATE int HTTPResponse_put_string (HTStream * me, CONST char * s)
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
    TTYPrint(TDEST, "TESTTESTTEST FREEING RESPONSE STREAM\n");
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

/*
**	This stream is responsible for generating a full HTTP/1.x response. It
**	may either do so by forwarding a response directly from an origin 
**	server (if we are proxying) - or it can generate the response itself.
*/
PUBLIC HTStream * HTTPResponse_new (HTRequest *		request,
				    void *		param,
				    HTFormat		input_format,
				    HTFormat		output_format,
				    HTStream *		output_stream)
{
    HTStream * me = NULL;
    HTdns * dns;
    if (!request || !request->net) return HTErrorStream();
    /*
    ** If we have a HTTP 1.x response then forward untouched
    */
    if ((dns = request->net->dns)) {
	char * s_class = HTDNS_serverClass(dns);
	int version = HTDNS_serverVersion(dns);
	/* We are not using the version info for the moment */
	if (s_class && !strcasecomp(s_class, "http")) {
	    if (STREAM_TRACE) TTYPrint(TDEST, "HTTPResponse Direct output\n");
	    return output_stream;
	}
    }
    if ((me = (HTStream *) calloc(1, sizeof(HTStream))) == NULL)
	outofmem(__FILE__, "HTTPResponse_new");
    me->isa = &HTTPResponseClass;
    me->target = output_stream;    
    me->request = request;

    TTYPrint(TDEST, "TESTTESTTEST CREATING RESPONSE STREAM\n");

    /* Here we should also check whether we have content length etc. */

    return me;
}
