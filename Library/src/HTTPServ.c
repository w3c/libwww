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
    HTTPS_ERROR		= -2,
    HTTPS_OK		= -1,
    HTTPS_BEGIN		= 0,
    HTTPS_NEED_REQUEST
} HTTPState;

/* This is the context object for the this module */
typedef struct _https_info {
    HTTPState	state;			  /* Current State of the connection */
} https_info;

/* The HTTP Receive Stream */
struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    HTSocketEOL			state;
    HTChunk *			buffer;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */
/*				REPLY STREAM				     */
/* ------------------------------------------------------------------------- */

/*
**	This is our handle to the server reply stream when data is coming
**	back from our request
*/

/*
**	Searches for HTTP Request Line before going into transparent mode
*/
PRIVATE int HTTPReply_put_block (HTStream * me, CONST char * b, int l)
{
    return PUTBLOCK(b, l);
}

PRIVATE int HTTPReply_put_string (HTStream * me, CONST char * s)
{
    return PUTBLOCK(s, strlen(s));
}

PRIVATE int HTTPReply_put_character (HTStream * me, char c)
{
    return PUTBLOCK(&c, 1);
}

PRIVATE int HTTPReply_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTTPReply_free (HTStream * me)
{
    HTNet * net = me->request->net;
    if (me->target) FREE_TARGET;
    if (STREAM_TRACE) TTYPrint(TDEST, "HTTPReply... Freing server stream\n");
    if (net) {
	if (HTDNS_socket(net->dns) == INVSOC)
	    HTNet_delete(net, HT_IGNORE);
	else
	    HTEvent_Register(net->sockfd, me->request, (SockOps) FD_READ,
			     net->cbf, net->priority);
    }
    free(me);
    return HT_OK;
}

PRIVATE int HTTPReply_abort (HTStream * me, HTList * e)
{
    if (me->target) ABORT_TARGET;
    free(me);
    if (PROT_TRACE) TTYPrint(TDEST, "HTTPReply... ABORTING...\n");
    return HT_ERROR;
}

/*	HTTPReply Stream
**	-----------------
*/
PRIVATE CONST HTStreamClass HTTPReplyClass =
{		
    "HTTPReply",
    HTTPReply_flush,
    HTTPReply_free,
    HTTPReply_abort,
    HTTPReply_put_character,
    HTTPReply_put_string,
    HTTPReply_put_block
};

PRIVATE HTStream * HTTPReply_new (HTRequest * request, HTStream * target)
{
    HTStream * me = (HTStream *) calloc(1, sizeof(HTStream));
    if (!me) outofmem(__FILE__, "HTTPReply_new");
    me->isa = &HTTPReplyClass;
    me->target = target;
    me->request = request;
    return me;
}

/* ------------------------------------------------------------------------- */
/*				RECEIVE STREAM				     */
/* ------------------------------------------------------------------------- */

/*
**	Scan the request line for METHOD, URI and VERSION
**	Returns:	HT_OK		if 1.x request and OK
**			HT_LOADED     	if 0.9 request and OK
**			HT_ERROR	if invalid request line
*/
PRIVATE int ParseRequest (HTStream * me)
{
    HTRequest * request = me->request;
    char * line = HTChunk_data(me->buffer);
    char * method_str = HTNextField(&line);
    char * request_uri = HTNextField(&line);
    char * version_str = HTNextField(&line);

    /* Set up an aouput stream */
    request->output_stream =
	HTTPReply_new(request,HTWriter_new(request->net, YES));

    /* Check if method is allowed */
    if (!method_str || (request->method=HTMethod_enum(method_str))==METHOD_INVALID) {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_NOT_ALLOWED,
			   NULL, 0, "ParseRequest");
	return HT_ERROR;
    }

    /* Find an anchor for the request URI */
    if (request_uri) {
	char * uri = HTParse(request_uri, "file:", PARSE_ALL);
	request->anchor = (HTParentAnchor *) HTAnchor_findAddress(uri);
	FREE(uri);
    } else {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_BAD_REQUEST,
			   NULL, 0, "ParseRequest");
	return HT_ERROR;
    }

    /* Get ready to get the rest of the request */
    if (version_str) {
	me->target = HTStreamStack(WWW_MIME_HEAD, request->output_format,
				   request->output_stream, request, NO);
	return HT_OK;
    } else {
	if (PROT_TRACE) TTYPrint(TDEST, "Request Line is formatted as 0.9\n");
	return HT_LOADED;
    }
}

/*
**	Searches for HTTP Request Line before going into transparent mode
*/
PRIVATE int HTTPReceive_put_block (HTStream * me, CONST char * b, int l)
{
    if (!me->transparent) {
	CONST char *p=b;
	while (l>0 && *p!=CR && *p!=LF) l--, p++;
	HTChunk_putb(me->buffer, b, p-b);
	if (*p==CR || *p==LF) {
	    int status = ParseRequest(me);
	    HTChunk_clear(me->buffer);
	    if (status != HT_OK) return status;
	    me->transparent = YES;
	    b=p;
	}
    }
    if (l > 0) {
	int status = PUTBLOCK(b, l);
	if (status == HT_LOADED) me->transparent = NO;
	return status;
    }
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
    me->state = EOL_BEGIN;    
    me->buffer = HTChunk_new(128);		 /* Sufficiant for most URLs */
    return me;
}

/* ------------------------------------------------------------------------- */

/*	ServerCleanup
**	-------------
**      This function cleans up after the request
**      Returns YES on OK, else NO
*/
PRIVATE int ServerCleanup (HTRequest * request, HTNet * net, int status)
{
    https_info * http = (https_info *) net->context;
    HTNet_delete(net, status);
    request->net = NULL;
    FREE(http);
    return YES;
}

/*	HTServHTTP
**	----------
**	Serv Document using HTTP.
**	returns	HT_ERROR or HT_OK
*/
PUBLIC int HTServHTTP (SOCKET soc, HTRequest * request, SockOps ops)
{
    int status = HT_ERROR;
    HTNet * net = request->net;		       /* Server information */
    https_info * http;			    /* Specific protocol information */
    
    /*
    ** Initiate a new https object and bind to request object
    ** This is actually state HTTPS_BEGIN, but it can't be in the state
    ** machine as we need the object first (chicken and egg problem).
    */
    if (ops == FD_NONE) {
	if (PROT_TRACE)
	    TTYPrint(TDEST,"HTTP Serve.. request %p on socket %d\n",
		     request, soc);
	if ((http = (https_info *) calloc(1, sizeof(https_info))) == NULL)
	    outofmem(__FILE__, "HTServHTTP");
	http->state = HTTPS_BEGIN;
	net->context = http;
    } else if (ops == FD_CLOSE) {			      /* Interrupted */
	ServerCleanup(request, net, HT_INTERRUPTED);
	return HT_OK;
    } else
	http = (https_info *) net->context;		/* Get existing copy */
 
    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (http->state) {
	  case HTTPS_BEGIN:
	    status = HTDoAccept(net);
	    if (status == HT_OK) {

		/* Setup read buffer */
		net->isoc = HTInputSocket_new(net->sockfd);	

		/* Setup Request parser stream */
		net->target = HTTPReceive_new(request, http);

		http->state = HTTPS_NEED_REQUEST;
	    } else {
		ServerCleanup(request, net, HT_ERROR);
		return HT_ERROR;
	    }	    
	    break;

	  case HTTPS_NEED_REQUEST:
	    if (ops == FD_READ || ops == FD_NONE) {
		status = HTSocketRead(request, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_LOADED) {
		    HTRequest * newreq = HTRequest_dup(request);
		    HTEvent_Register(net->sockfd, request, (SockOps) FD_READ,
				     net->cbf, net->priority);
		    return HTLoad(newreq, NO)==YES ? HT_OK : HT_ERROR;
		} else if (status == HT_PAUSE) {
		    HTRequest * newreq = HTRequest_dup(request);
		    HTRequest_addDestination(request, newreq);
		    return HTLoad(newreq, NO)==YES ? HT_OK : HT_ERROR;
		} else if (status == HT_CLOSED)
		    http->state = HTTPS_OK;
		else
		    http->state = HTTPS_ERROR;
	    } else if (ops == FD_WRITE) {
		if (HTRequest_mainDestination(request)) {
		    HTNet * dest = request->mainDestination->net;
		    HTEvent_Register(dest->sockfd, dest->request,
				     (SockOps) FD_READ,
				     dest->cbf, dest->priority);
		}
		return HT_OK;
	    } else
		http->state = HTTPS_ERROR;
	    break;

	  case HTTPS_OK:
	    return HT_OK;
	    break;

	  case HTTPS_ERROR:

	   /* Do the POSTWeb stuff */

	    ServerCleanup(request, net, HT_ERROR);
	    return HT_OK;
	    break;
	}
    }
}

