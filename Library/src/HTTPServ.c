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
#include "HTConLen.h"
#include "HTNetMan.h"
#include "HTMIMERq.h"
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
    char *	version;			    /* Version for the reply */
    HTRequest *	client;			       /* This is out client request */
} https_info;

/* The HTTP Receive Stream */
struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    https_info *		http;
    HTRequest *			client;
    HTSocketEOL			state;
    HTChunk *			buffer;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */

/*	ServerCleanup
**	-------------
**      This function cleans up after the request
**      Returns YES on OK, else NO
*/
PRIVATE int ServerCleanup (HTRequest * req, HTNet * net, int status)
{
    https_info * http = (https_info *) net->context;

    /* Free stream with data TO network */
    if (req->input_stream) {
	if (status == HT_INTERRUPTED)
	    (*req->input_stream->isa->abort)(req->input_stream, NULL);
	else
	    (*req->input_stream->isa->_free)(req->input_stream);
	req->input_stream = NULL;
    }

    /* Remove the net object and our own context structure for http */
    HTNet_delete(net, req->internal ? HT_IGNORE : status);
    HT_FREE(http->version);
    HT_FREE(http);
    return YES;
}

/* ------------------------------------------------------------------------- */
/*				REPLY STREAM				     */
/* ------------------------------------------------------------------------- */

/*
**	This is our handle to the server reply stream when data is coming
**	back from our "client" request. It is responsible for setting up the
**	remaining streams in order to produce a complete HTTP output.
**	If we have a HTTP 1.x response then forward untouched.
**
**	BUG: We should look at the version string before generating response!
*/
PRIVATE int MakeReplyPipe (HTStream *me, HTRequest *server, HTRequest *client)
{
    char * response_line = NULL;
    me->transparent = YES;

    /* Check if we can forward the response untouched */
    if (client->net) {
	HTdns * cdns = client->net->dns;
	char * s_class = HTDNS_serverClass(cdns);
	int version = HTDNS_serverVersion(cdns);
	/* We are not using the version info for the moment */
	if (s_class && !strcasecomp(s_class, "http")) {
	    if (STREAM_TRACE) TTYPrint(TDEST, "HTTP Reply.. Direct output\n");
	    return HT_OK;
	}
    }

    /* Set up a small buffer for the response headers */
    me->target = HTBuffer_new(me->target, server, 256);

    /* Generate the Response line */
    {
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) {
	    HTAlertPar * reply = HTAlert_newReply();
	    if ((*cbf)(client, HT_A_MESSAGE, HT_MSG_NULL, NULL,
		       client->error_stack, reply))
		response_line = HTAlert_replyMessage(reply);
	    HTAlert_deleteReply(reply);
	}

	/* Output the response */
	if (response_line) {
	    PUTS(response_line);
	    HT_FREE(response_line);
	} else {	
	    PUTS("500 Internal");
	    PUTC(CR);
	    PUTC(LF);
	}
    }

    /*
    ** We now have to create the rest of the response stream. We see whether
    ** there is a data object or not by looking at the Content Type of the
    ** client anchor.
    */
    me->target = (HTAnchor_format(client->anchor) == WWW_UNKNOWN) ?
	HTTPResponse_new(server, me->target, YES) :
	HTMIMERequest_new(server, HTTPResponse_new(server,me->target,NO), YES);

    /* Here we should put out any BODY for the error message */

    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTTPReply_put_block (HTStream * me, CONST char * b, int l)
{
    if (me->transparent)
	return b ? PUTBLOCK(b, l) : HT_OK;
    else {
	MakeReplyPipe(me, me->request, me->client);
	return b ? PUTBLOCK(b, l) : HT_OK;
    }
}

PRIVATE int HTTPReply_put_string (HTStream * me, CONST char * s)
{
    return HTTPReply_put_block(me, s, strlen(s));
}

PRIVATE int HTTPReply_put_character (HTStream * me, char c)
{
    return HTTPReply_put_block(me, &c, 1);
}

PRIVATE int HTTPReply_flush (HTStream * me)
{
    int status = HTTPReply_put_block(me, NULL, 0);
    return status==HT_OK ? (*me->target->isa->flush)(me->target) : status;
}

PRIVATE int HTTPReply_free (HTStream * me)
{
    int status = HTTPReply_flush(me);
    if (STREAM_TRACE) TTYPrint(TDEST, "HTTPReply... Freeing server stream\n");
    if (status != HT_WOULD_BLOCK) {
	HTNet * snet = me->request->net;
	if ((status = FREE_TARGET) == HT_WOULD_BLOCK) return HT_WOULD_BLOCK;
#if 0
	/* We can't do this until we get a better event loop */
	if (snet->persistent) {
	    if (STREAM_TRACE)
		TTYPrint(TDEST, "HTTPReply... Persistent conenction\n");
	    HTEvent_Register(snet->sockfd, me->request, (SockOps) FD_READ,
			     snet->cbf, snet->priority);
	    HTRequest_clear(me->request);
	} else {
	    ServerCleanup(me->request, snet, HT_IGNORE);
	    HTRequest_delete(me->request);
	    HTRequest_removeDest(me->client);
	}
#else
	ServerCleanup(me->request, snet, HT_IGNORE);
	HTRequest_delete(me->request);
#endif
    }
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int HTTPReply_abort (HTStream * me, HTList * e)
{
    HTNet * snet = me->request->net;
    if (!me->transparent) {
	HTRequest_addError(me->client, ERR_FATAL, NO, HTERR_INTERNAL,
			   NULL, 0, "HTTPReply_abort");
	MakeReplyPipe(me, me->request, me->client);
    }
    if (STREAM_TRACE) TTYPrint(TDEST, "HTTPReply... ABORTING\n");
    if (me->target) ABORT_TARGET;
#if 0
    /* We can't do this until we get a better event loop */
    if (snet->persistent) {
	if (STREAM_TRACE)
	    TTYPrint(TDEST, "HTTPReply... Persistent conenction\n");
	HTEvent_Register(snet->sockfd, me->request, (SockOps) FD_READ,
			 snet->cbf, snet->priority);
	HTRequest_clear(me->request);
    } else {
	ServerCleanup(me->request, snet, HT_IGNORE);
	HTRequest_delete(me->request);
	HTRequest_removeDest(me->client);
    }
#else
    ServerCleanup(me->request, snet, HT_IGNORE);
    HTRequest_delete(me->request);
#endif
    HT_FREE(me);
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

PRIVATE HTStream * HTTPReply_new (HTRequest * request, HTRequest * client,
				  HTStream * target)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTTPReply_new");
    me->isa = &HTTPReplyClass;
    me->target = target;
    me->request = request;
    me->client = client;
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
	HT_FREE(uri);
    } else {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_BAD_REQUEST,
			   NULL, 0, "ParseRequest");
	return HT_ERROR;
    }

    /* Get ready to get the rest of the request */
    if (version_str) {
	me->target = HTStreamStack(WWW_MIME_HEAD, request->debug_format,
				   request->debug_stream, request, NO);
	StrAllocCopy(me->http->version, version_str);
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
    HT_FREE(me);
    return status;
}

PRIVATE int HTTPReceive_abort (HTStream * me, HTList * e)
{
    if (me->target) ABORT_TARGET;
    HTChunk_delete(me->buffer);
    HT_FREE(me);
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
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTTPReceive_new");
    me->isa = &HTTPReceiveClass;
    me->request = request;
    me->http = http;
    me->state = EOL_BEGIN;    
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
	if ((http = (https_info *) HT_CALLOC(1, sizeof(https_info))) == NULL)
	    HT_OUTOFMEM("HTServHTTP");
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
		else if (status == HT_PAUSE) {
		    http->client = HTRequest_dup(request);

		    /* Set the right client headers */
		    HTRequest_setGnHd(http->client, DEFAULT_GENERAL_HEADERS);

		    /*
		    ** If we have a data object in the request then link the
		    ** two request objects together
		    */
		    if (HTMethod_hasEntity(request->method))
			HTRequest_addDestination(request, http->client);

		    /*
		    ** Set up our reply stream. This is responsible for
		    ** generating a HTTP reply
		    */
		    http->client->output_stream = request->output_stream =
			HTTPReply_new(request, http->client,
				      HTWriter_new(request->net,YES));

		    /* Start the load of the "client" request */
		    return HTLoad(http->client, NO) == YES ? HT_OK : HT_ERROR;
		} else if (status == HT_CLOSED)
		    http->state = HTTPS_OK;
		else
		    http->state = HTTPS_ERROR;
	    } else if (ops == FD_WRITE) {
#if 0
		if (HTRequest_mainDestination(request)) {
		    HTNet * dest = request->mainDestination->net;
		    HTEvent_Register(dest->sockfd, dest->request,
				     (SockOps) FD_READ,
				     dest->cbf, dest->priority);
		}
#else
		if (http->client && http->client->net) {
		    HTNet * dnet = http->client->net;
		    HTEvent_Register(dnet->sockfd, http->client,
				     (SockOps) FD_READ, dnet->cbf, dnet->priority);
		}
#endif		    
		return HT_OK;
	    } else
		http->state = HTTPS_ERROR;
	    break;

	  case HTTPS_OK:
	    ServerCleanup(request, net, HT_IGNORE);
	    break;

	  case HTTPS_ERROR:
	    if (HTRequest_isPostWeb(request)) HTRequest_killPostWeb(request);
	    ServerCleanup(request, net, HT_ERROR);
	    return HT_OK;
	    break;
	}
    }
}

