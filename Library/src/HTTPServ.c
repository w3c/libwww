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
#include "HTWriter.h"
#include "HTChunk.h"
#include "HTReqMan.h"
#include "HTNetMan.h"
#include "HTTPUtil.h"
#include "HTTPRes.h"
#include "HTTPServ.h"					       /* Implements */

/* Macros and other defines */
#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)
#define FREE_TARGET	(*me->target->isa->_free)(me->target)
#define ABORT_TARGET	(*me->target->isa->abort)(me->target, e)

/* Final states have negative value */
typedef enum _HTTPState {
    HTTPS_ERROR		= -1,
    HTTPS_BEGIN		= 0,
    HTTPS_NEED_STREAM,
    HTTPS_NEED_REQUEST
} HTTPState;

/* This is the context object for the this module */
typedef struct _https_info {
    HTTPState	state;			  /* Current State of the connection */
    HTRequest *	serve;			/* For handling the incoming request */
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
#if 0
    if (!HTRequest_isDestination(req) && req->input_stream) {
	if (status == HT_INTERRUPTED)
	    (*req->input_stream->isa->abort)(req->input_stream, NULL);
	else
	    (*req->input_stream->isa->_free)(req->input_stream);
	req->input_stream = NULL;
    }

    /* Remove the request object and our own context object for http */
    HTNet_delete(net, status);
#endif
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
    HTRequest * newreq = me->http->serve = HTRequest_dup(request);
    char * line = HTChunk_data(me->buffer);
    char * method;
    char * request_uri;

    /* Bind the two request objects together */
    newreq->source = request;
    request->output_format = WWW_SOURCE;
    request->source = request;			 	  /* Point to myself */
    HTRequest_addDestination(request, newreq);
    newreq->input_format = WWW_SOURCE;

    /* Handle method and URI */
    if ((method = HTNextField(&line)) && (request_uri = HTNextField(&line))) {
	if ((newreq->method = HTMethod_enum(method)) == METHOD_INVALID) {
	    HTRequest_addError(newreq, ERR_FATAL, NO, HTERR_NOT_ALLOWED,
			       NULL, 0, "ParseRequest");
	    HTRequest_delete(newreq);
	    return HT_ERROR;
	}

	{
	    char * uri = HTParse(request_uri, "file:", PARSE_ALL);
	    newreq->anchor = (HTParentAnchor *) HTAnchor_findAddress(uri);
#if 0
	    /* Take copy of the original uri */
	    StrAllocCopy(newreq->request_uri, request_uri);
#endif
	}
    } else {
	HTRequest_addError(newreq, ERR_FATAL, NO, HTERR_BAD_REQUEST,
			   NULL, 0, "ParseRequest");
	return HT_ERROR;
    }

    /*
    ** Handle version. If we have a 1.x request then always parse headers.
    ** We might find a persistent connection request in which case we don't
    ** want to loose it.
    */
    me->transparent = YES;
    if ((me->version = HTNextField(&line))) {
	request->input_stream =
	    HTTPResponse_new(newreq, HTBufWriter_new(newreq->net, YES, 512));
	newreq->output_stream = request->input_stream;
	me->target = HTStreamStack(WWW_MIME, newreq->output_format,
				   newreq->output_stream, newreq, NO);
	return HT_OK;
    } else {
	if (PROT_TRACE) TTYPrint(TDEST, "Request Line is formatted as 0.9\n");
	request->input_stream = HTBufWriter_new(request->net, YES, 512);
	newreq->output_stream = request->input_stream;
	return HT_LOADED;
    }
}

/*
**	Searches for HTTP Request Line before going into transparent mode
*/
PRIVATE int HTTPReceive_put_block (HTStream * me, CONST char * b, int l)
{
    int status = HT_OK;
    while (!me->transparent && l > 0) {
	if (me->state == EOL_FCR) {
	    if ((status = ParseRequest(me)) != HT_OK) return status;
	    else if (*b != LF) break;
	} else if (*b == CR) {
	    me->state = EOL_FCR;
	} else if (*b == LF) {
	    if ((status = ParseRequest(me)) < 0) return status;
	} else
	    HTChunk_putc(me->buffer, *b);		/* @@@ */
	b++, l--;
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
    HTChunk_delete(me->buffer);
    free(me);
    return status;
}

PRIVATE int HTTPReceive_abort (HTStream * me, HTList * e)
{
    if (me->target) ABORT_TARGET;
    HTChunk_delete(me->buffer);
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
    me->buffer = HTChunk_new(128);		 /* Sufficiant for most URLs */
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
	    TTYPrint(TDEST,"HTTP Serve.. request %p on socket %d\n",
		     request, soc);
	if ((http = (https_info *) calloc(1, sizeof(https_info))) == NULL)
	    outofmem(__FILE__, "HTServHTTP");
	http->state = HTTPS_BEGIN;
	net->context = http;
    } else if (ops == FD_CLOSE) {			      /* Interrupted */
	ServerCleanup(request, http->serve ? HT_IGNORE : HT_INTERRUPTED);
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
		ServerCleanup(request, http->serve ? HT_IGNORE : HT_ERROR);
		return HT_ERROR;
	    }	    
	    break;

	  case HTTPS_NEED_STREAM:
	    net->target = HTTPReceive_new(request, http);
	    http->state = HTTPS_NEED_REQUEST;
	    break;

	  case HTTPS_NEED_REQUEST:
	    if (ops == FD_READ || ops == FD_NONE) {
		status = HTSocketRead(request, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_LOADED) {
		    HTRequest * server = http->serve;
		    if (HTLoad(server, NO) != YES) {
			if (PROT_TRACE) TTYPrint(TDEST,"HTTP Serve. Error!\n");
			http->state = HTTPS_ERROR;
		    }
		} else
		    http->state = HTTPS_ERROR;
	    } else if (ops == FD_WRITE) {		
		if (HTRequest_isSource(request)) {
		    HTNet * dest = request->mainDestination->net;
		    TTYPrint(TDEST,"HTTP Serve. HERE!\n");		
		    HTEvent_Register(dest->sockfd, dest->request,
				     (SockOps) FD_READ,
				     dest->cbf, dest->priority);
		}
		return HT_OK;
	    } else
		http->state = HTTPS_ERROR;
	    break;

	  case HTTPS_ERROR:
	    ServerCleanup(request, http->serve ? HT_IGNORE : HT_ERROR);
	    return HT_OK;
	    break;
	}
    }
}
