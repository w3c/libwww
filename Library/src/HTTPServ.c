/*								    HTTPServ.c
**	HTTP SERVER MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This module implments the HTTP protocol as a state machine
**
** History:
**	Dec 95 HFN	Written with Christmas in my eyes
**
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTWWWStr.h"
#include "HTTCP.h"
#include "HTSocket.h"
#include "HTAlert.h"
#include "HTError.h"
#include "HTAccess.h"
#include "HTChunk.h"
#include "HTReqMan.h"
#include "HTNetMan.h"
#include "HTTPUtil.h"
#include "HTTPServ.h"					       /* Implements */

/* Macros and other defines */
#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)
#define FREE_TARGET	(*me->target->isa->_free)(me->target)
#define ABORT_TARGET	(*me->target->isa->abort)(me->target, e)

/* Final states have negative value */
typedef enum _HTTPState {
    HTTPS_ERROR		= -2,
    HTTPS_SENT_RESPONSE	= -1,
    HTTPS_BEGIN		= 0,
    HTTPS_NEED_STREAM,
    HTTPS_NEED_REQUEST,
    HTTPS_HANDLE_REQUEST
} HTTPState;

/* This is the context object for the this module */
typedef struct _https_info {
    HTTPState	state;			  /* Current State of the connection */
    char *	uri;		     /* As we receive it in the Request Line */
} https_info;

/* The HTTP Receive Stream */
struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    https_info *		http;
    HTSocketEOL			state;
    char *			method;
    char *		       	version;
    HTChunk *			buffer;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */

/*	ServerCleanup
**	-------------
**      This function cleans up after the request
**      Returns YES on OK, else NO
*/
PRIVATE int ServerCleanup (HTRequest *req, int status)
{
    HTNet *net = req->net;
    https_info *http = (https_info *) net->context;

    /* Free stream with data TO network */
    if (!HTRequest_isDestination(req) && req->input_stream) {
	if (status == HT_INTERRUPTED)
	    (*req->input_stream->isa->abort)(req->input_stream, NULL);
	else
	    (*req->input_stream->isa->_free)(req->input_stream);
	req->input_stream = NULL;
    }

    /* Remove the request object and our own context object for http */
    HTNet_delete(net, status);
    FREE(http);
    return YES;
}

/*
**	Scan the request line for METHOD, URI and VERSION
**	Returns:	HT_OK		if 1.x request and OK
**			HT_LOADED     	if 0.9 request and OK
**			HT_ERROR	if invalid request line
*/
PRIVATE int ParseRequest (HTStream * me)
{
    HTRequest * request = me->request;
    char * line = HTChunkData(me->buffer);
    char * method;
    char * uri;

    /* Handle method and URI */
    if ((method = HTNextField(&line)) && (uri = HTNextField(&line))) {
	if ((request->method = HTMethod_enum(method)) == METHOD_INVALID) {
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_ALLOWED,
			       NULL, 0, "ParseRequest");
	    return HT_ERROR;
	}
	StrAllocCopy(me->http->uri, uri);
    } else {
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_REQUEST,
			   NULL, 0, "ParseRequest");
	return HT_ERROR;
    }

    /* Handle version */
    if ((me->version = HTNextField(&line))) {
	HTNet * net = request->net;
	net->target = HTStreamStack(WWW_MIME, request->output_format,
				    request->output_stream, request, NO);
	me->transparent = YES;
	if (PROT_TRACE) TTYPrint(TDEST, "Request Line is a 1.x request\n");
	return HT_OK;
    }
    if (PROT_TRACE) TTYPrint(TDEST, "Request Line is a - YRK - 0.9 request\n");
    return HT_LOADED;
}

/*
**	Searches for HTTP Request Line before going into transparent mode
*/
PRIVATE int HTTPReceive_put_block (HTStream * me, CONST char * b, int l)
{
    int status;
    while (!me->transparent && l-- > 0) {
	if (me->state == EOL_FCR) {
	    if (*b == LF) {
		if ((status = ParseRequest(me)) != HT_OK) return status;
	    } else if (*b == CR) {
		status = ParseRequest(me);
		return status == HT_OK ? HT_LOADED : status;
	    } else {
		if ((status = ParseRequest(me)) != HT_OK) return status;
		l++;
	    }
	} else if (*b == CR) {
	    me->state = EOL_FCR;
	} else if (*b == LF) {
	    if ((status = ParseRequest(me)) != HT_OK) return status;
	} else {
	    HTChunkPutc(me->buffer, *b);		/* @@@ */
	}
	b++;
    }
    if (l > 0 && me->target) return PUTBLOCK(b, l);
    return HT_OK;
}

PRIVATE int HTTPReceive_put_string (HTStream * me, CONST char * s)
{
    return HTTPReceive_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTTPReceive_put_character (HTStream * me, char c)
{
    return HTTPReceive_put_block(me, &c, 1);
}

PRIVATE int HTTPReceive_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTTPReceive_free (HTStream * me)
{
    int status = HT_OK;
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    HTChunkFree(me->buffer);
    free(me);
    return status;
}

PRIVATE int HTTPReceive_abort (HTStream * me, HTList * e)
{
    if (me->target) ABORT_TARGET;
    HTChunkFree(me->buffer);
    free(me);
    if (PROT_TRACE) TTYPrint(TDEST, "HTTPReceive. ABORTING...\n");
    return HT_ERROR;
}

/*	HTTPReceive Stream
**	-----------------
*/
PRIVATE CONST HTStreamClass HTTPReceiveClass =
{		
    "HTTPReceive",
    HTTPReceive_flush,
    HTTPReceive_free,
    HTTPReceive_abort,
    HTTPReceive_put_character,
    HTTPReceive_put_string,
    HTTPReceive_put_block
};

PRIVATE HTStream * HTTPReceive_new (HTRequest * request, https_info * http)
{
    HTStream * me = (HTStream *) calloc(1, sizeof(HTStream));
    if (!me) outofmem(__FILE__, "HTTPReceive_new");
    me->isa = &HTTPReceiveClass;
    me->request = request;
    me->http = http;
    me->state = EOL_BEGIN;    
    me->method = METHOD_INVALID;
    me->buffer = HTChunkCreate(128);		 /* Sufficiant for most URLs */
    return me;
}

/* ------------------------------------------------------------------------- */

/*	HTServHTTP
**	----------
**	Serv Document using HTTP.
**	returns	HT_ERROR or HT_OK
*/
PUBLIC int HTServHTTP (SOCKET soc, HTRequest * request, SockOps ops)
{
    int status = HT_ERROR;
    HTNet * net = request->net;		     /* Generic protocol information */
    https_info * http;			    /* Specific protocol information */
    
    /*
    ** Initiate a new https object and bind to request object
    ** This is actually state HTTPS_BEGIN, but it can't be in the state
    ** machine as we need the object first (chicken and egg problem).
    */
    if (soc == INVSOC) return HT_ERROR;
    if (ops == FD_NONE) {
	if (PROT_TRACE)
	    TTYPrint(TDEST,"HTTP........ Serving request %p on socket %d\n",
		     request, soc);
	if ((http = (https_info *) calloc(1, sizeof(https_info))) == NULL)
	    outofmem(__FILE__, "HTServHTTP");
	http->state = HTTPS_BEGIN;
	net->context = http;
    } else if (ops == FD_CLOSE) {			      /* Interrupted */
	ServerCleanup(request, HT_INTERRUPTED);
	return HT_OK;
    } else
	http = (https_info *) net->context;		/* Get existing copy */
 
    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (http->state) {
	  case HTTPS_BEGIN:
	    status = HTDoAccept(net);
	    if (status == HT_OK) {
		net->isoc = HTInputSocket_new(net->sockfd);	
		http->state = HTTPS_NEED_STREAM;
	    } else {
		ServerCleanup(request, HT_ERROR);
		return HT_ERROR;
	    }	    
	    break;

	  case HTTPS_NEED_STREAM:
	    net->target = HTTPReceive_new(request, http);
	    http->state = HTTPS_NEED_REQUEST;
	    break;

	  case HTTPS_NEED_REQUEST:
	    status = HTSocketRead(request, net);
	    if (status == HT_WOULD_BLOCK)
		return HT_OK;
	    else
		http->state = HTTPS_HANDLE_REQUEST;
	    break;

	  case HTTPS_HANDLE_REQUEST:
	    http->state = HTTPS_SENT_RESPONSE;
	    break;

	  case HTTPS_SENT_RESPONSE:

	    /* if persistent connection then jump to NEED_REQUEST */

	    ServerCleanup(request, HT_LOADED);
	    return HT_OK;
	    break;

	  case HTTPS_ERROR:
	    ServerCleanup(request, HT_ERROR);
	    return HT_OK;
	    break;
	}
    }
}    


