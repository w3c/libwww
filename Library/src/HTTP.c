/*									 HTTP.c
**	MULTITHREADED IMPLEMENTATION OF HTTP CLIENT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module implments the HTTP protocol as a state machine
**
** History:
**    < May 24 94 ??	Unknown - but obviously written
**	May 24 94 HF	Made reentrent and cleaned up a bit. Implemented
**			Forward, redirection, error handling and referer field
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**	Jul 94 HFN	Written on top of HTTP.c, Henrik Frystyk
**
*/

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWCache.h"
#include "WWWMIME.h"
#include "WWWStream.h"
#include "WWWTrans.h"
#include "HTReqMan.h"
#include "HTNetMan.h"
#include "HTTPUtil.h"
#include "HTTPReq.h"
#include "HTTP.h"					       /* Implements */

/* Macros and other defines */
#ifndef HTTP_PORT
#define HTTP_PORT 80	/* Allocated to http by Jon Postel/ISI 24-Jan-92 */
#endif

#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)
#define FREE_TARGET	(*me->target->isa->_free)(me->target)
#define ABORT_TARGET	(*me->target->isa->abort)(me->target, e)

/* Type definitions and global variables etc. local to this module */

/* Final states have negative value */
typedef enum _HTTPState {
    HTTP_RETRY		= -4,
    HTTP_ERROR		= -3,
    HTTP_NO_DATA	= -2,
    HTTP_GOT_DATA	= -1,
    HTTP_BEGIN		= 0,
    HTTP_NEED_CONNECTION,
    HTTP_NEED_REQUEST,
    HTTP_PERM_REDIRECT,
    HTTP_TEMP_REDIRECT,
    HTTP_NOT_MODIFIED,
    HTTP_AA
} HTTPState;

/* This is the context structure for the this module */
typedef struct _http_info {
    HTTPState		state;		  /* Current State of the connection */
    HTTPState		next;				       /* Next state */
} http_info;

#define MAX_STATUS_LEN		100   /* Max nb of chars to check StatusLine */

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    http_info *			http;
    HTEOLState			state;
    BOOL			transparent;
    char *			version;	     /* Should we save this? */
    int				status;
    char *			reason;
    char 			buffer[MAX_STATUS_LEN+1];
    int				buflen;
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

/* ------------------------------------------------------------------------- */
/* 			          Help Functions			     */
/* ------------------------------------------------------------------------- */

/*	HTTPCleanup
**	-----------
**      This function closes the connection and frees memory.
**      Returns YES on OK, else NO
*/
PRIVATE int HTTPCleanup (HTRequest *req, int status)
{
    HTNet * net = HTRequest_net(req);
    http_info * http = (http_info *) HTNet_context(net);
    HTStream * input = HTRequest_inputStream(req);

    /* Free stream with data TO network */
    if (HTRequest_isDestination(req))
	HTRequest_removeDestination(req);
    else if (input) {
	if (status == HT_INTERRUPTED)
	    (*input->isa->abort)(input, NULL);
	else
	    (*input->isa->_free)(input);
	HTRequest_setInputStream(req, NULL);
    }

    /* Remove the request object and our own context structure for http */
    HTNet_delete(net, HTRequest_internal(req) ? HT_IGNORE : status);
    HT_FREE(http);
    return YES;
}

/*
**	Informational 1xx codes are handled separately
**	Returns YES if we should stop, NO if we should continue
*/
PRIVATE BOOL HTTPInformation (HTStream * me)
{
    BOOL result = NO;
    switch (me->status) {

    case 101:
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_SWITCHING,
			   me->reason, (int) strlen(me->reason),
			   "HTTPInformation");

	/* Here we should swap to new protocol */

	result = YES;
	break;

    default:
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_CONTINUE,
			   me->reason, (int) strlen(me->reason),
			   "HTTPInformation");
	break;
    }
    return result;
}

/*
**	This is a big switch handling all HTTP return codes. It puts in any
**	appropiate error message and decides whether we should expect data
**	or not.
*/
PRIVATE void HTTPNextState (HTStream * me)
{
    switch (me->status) {

      case 0:						     /* 0.9 response */
      case 200:							       /* OK */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_OK,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	me->http->next = HTTP_GOT_DATA;
	break;

      case 201:							  /* Created */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_CREATED,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	me->http->next = HTTP_GOT_DATA;
	break;

      case 202:							 /* Accepted */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_ACCEPTED,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	me->http->next = HTTP_GOT_DATA;
	break;

      case 203:				    /* Non-authoritative information */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_NON_AUTHORITATIVE,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	me->http->next = HTTP_GOT_DATA;
	break;

      case 204:						      /* No Response */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_NO_CONTENT,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	me->http->next = HTTP_NO_DATA;
	break;

      case 301:						   	    /* Moved */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_MOVED,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	me->http->next = HTTP_PERM_REDIRECT;
	break;

      case 302:							    /* Found */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_FOUND,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	me->http->next = HTTP_TEMP_REDIRECT;
	break;
	
      case 303:							   /* Method */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_IMPLEMENTED,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;

      case 304:						     /* Not Modified */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_MODIFIED,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	me->http->next = HTTP_NOT_MODIFIED;
	break;
	
      case 400:						      /* Bad Request */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_REQUEST,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;

      case 401:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_UNAUTHORIZED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_AA;
	break;
	
      case 402:						 /* Payment required */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_PAYMENT_REQUIRED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;
	
      case 403:							/* Forbidden */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_FORBIDDEN,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;
	
      case 404:							/* Not Found */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_FOUND,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;
	
      case 405:						      /* Not Allowed */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_ALLOWED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;

      case 406:						  /* None Acceptable */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NONE_ACCEPTABLE,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;

      case 407:			       	    /* Proxy Authentication Required */
	HTRequest_addError(me->request, ERR_FATAL, NO,HTERR_PROXY_UNAUTHORIZED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;

      case 408:						  /* Request Timeout */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_TIMEOUT,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;

      case 500:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_INTERNAL,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;
	
      case 501:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_IMPLEMENTED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;

      case 502:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_GATE,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;

      case 503:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_DOWN,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");

	/* If Retry-After header is found then return HT_RETRY else HT_ERROR */
	if (HTRequest_retryTime(me->request))
	    me->http->next = HTTP_RETRY;
	else
	    me->http->next = HTTP_ERROR;
	break;

      case 504:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_GATE_TIMEOUT,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
        me->http->next = HTTP_ERROR;
	break;

      default:						       /* bad number */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_REPLY,
		   (void *) me->buffer, me->buflen, "HTTPNextState");
	me->http->next = HTTP_ERROR;
	break;
    }
}

/* ------------------------------------------------------------------------- */
/* 			    HTTP Status Line Stream			     */
/* ------------------------------------------------------------------------- */

/*
**	Analyse the stream we have read. If it is a HTTP 1.0 or higher
**	then create a MIME-stream, else create a Guess stream to find out
**	what the 0.9 server is sending. We need to copy the buffer as we don't
**	know if we can modify the contents or not.
**
**	Stream handling is a function of the status code returned from the 
**	server:
**		200:	 Use `output_stream' in HTRequest structure
**		else:	 Use `debug_stream' in HTRequest structure
**
**	Return: YES if buffer should be written out. NO otherwise
*/
PRIVATE int stream_pipe (HTStream * me)
{
    HTRequest * req = me->request;
    HTNet * net = req->net;
    HTHost * host = HTNet_host(net);
    if (me->target) {
	int status = PUTBLOCK(me->buffer, me->buflen);
	if (status == HT_OK)
	    me->transparent = YES;
	return status;
    } else if (HTRequest_isSource(req) && !req->output_stream) {
	/*
	**  We need to wait for the destinations to get ready
	*/
	return HT_WOULD_BLOCK;
    }
	
    if (strncasecomp(me->buffer, "http/", 5)) {
	int status;
	HTRequest_addError(req, ERR_INFO, NO, HTERR_HTTP09,
		   (void *) me->buffer, me->buflen, "HTTPStatusStream");
	me->target = HTStreamStack(WWW_UNKNOWN, req->output_format,
				   req->output_stream, req, NO);
	me->http->next = HTTP_GOT_DATA;
	if ((status = PUTBLOCK(me->buffer, me->buflen)) == HT_OK)
	    me->transparent = YES;
	HTHost_setVersion(host, HTTP_09);
	return status;
    } else {
	char *ptr = me->buffer+5;		       /* Skip the HTTP part */
	me->version = HTNextField(&ptr);

	/* Here we want to find out when to use persistent connection */
	if (!strncasecomp(me->version, "1.0", 3)) {
	    if (PROT_TRACE)HTTrace("HTTP Status. This is a HTTP/1.0 server\n");
	    HTHost_setVersion(host, HTTP_10);
	} else {
	    HTHost_setVersion(host, HTTP_11);
	    HTNet_setPersistent(net, YES);		       /* By default */
	}

	me->status = atoi(HTNextField(&ptr));
	me->reason = ptr;
	if ((ptr = strchr(me->reason, '\r')) != NULL)	  /* Strip \r and \n */
	    *ptr = '\0';
	else if ((ptr = strchr(me->reason, '\n')) != NULL)
	    *ptr = '\0';

	/* 
	**  If it is a 1xx code then find out what to do and return until we
	**  get the next code. In the case of Upgrade we may not get back here
	**  at all. If we are uploading an entity then continue doing that
	*/
	if (me->status/100 == 1) {
	    if (HTTPInformation(me)) me->transparent = YES;
	    me->buflen = 0;
	    me->state = EOL_BEGIN;
	    return HTMethod_hasEntity(HTRequest_method(req)) ?
		HT_CONTINUE : HT_OK;
	}

	/* Set up the streams */
	if (me->status==200) {
	    HTStream *s;
	    me->target = HTStreamStack(WWW_MIME, req->output_format,
				       req->output_stream, req, NO);
	    
	    /* howcome: test for return value from HTCacheWriter 12/1/95 */
	    if (req->method==METHOD_GET && HTCache_isEnabled() &&
		(s = HTCacheWriter(req, NULL, WWW_MIME, req->output_format,
				   req->output_stream))) {
		me->target = HTTee(me->target, s, NULL);
	    }
	} else if (req->debug_stream) {
	    me->target = HTStreamStack(WWW_MIME, req->debug_format,
				       req->debug_stream, req, NO);
	} else {
	    /* We still need to parser the MIME part */
	    me->target = HTStreamStack(WWW_MIME, req->debug_format,
				       req->debug_stream, req, NO);
	}
    }
    if (!me->target) me->target = HTErrorStream();
    HTTPNextState(me);					   /* Get next state */
    me->transparent = YES;
    return HT_OK;
}

/*
**	Searches for HTTP header line until buffer fills up or a CRLF or LF
**	is found
*/
PRIVATE int HTTPStatus_put_block (HTStream * me, const char * b, int l)
{
    while (!me->transparent && l-- > 0) {
	int status;
	if (me->target) {
	    if ((status = stream_pipe(me)) != HT_OK)
		return status;
	} else {
	    *(me->buffer+me->buflen++) = *b;
	    if (me->state == EOL_FCR) {
		if (*b == LF) {	/* Line found */
		    if ((status = stream_pipe(me)) != HT_OK)
			return status;
		} else {
		    me->state = EOL_BEGIN;
		}
	    } else if (*b == CR) {
		me->state = EOL_FCR;
	    } else if (*b == LF) {
		if ((status = stream_pipe(me)) != HT_OK)
		    return status;
	    } else {
		if (me->buflen >= MAX_STATUS_LEN) {
		    if ((status = stream_pipe(me)) != HT_OK)
			return status;
		}
	    }
	    b++;
	}
    }
    if (l > 0) return PUTBLOCK(b, l);
    return HT_OK;
}

PRIVATE int HTTPStatus_put_string (HTStream * me, const char * s)
{
    return HTTPStatus_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTTPStatus_put_character (HTStream * me, char c)
{
    return HTTPStatus_put_block(me, &c, 1);
}

PRIVATE int HTTPStatus_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTTPStatus_free (HTStream * me)
{
    int status = HT_OK;
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    HT_FREE(me);
    return status;
}

PRIVATE int HTTPStatus_abort (HTStream * me, HTList * e)
{
    if (me->target)
	ABORT_TARGET;
    HT_FREE(me);
    if (PROT_TRACE)
	HTTrace("HTTPStatus.. ABORTING...\n");
    return HT_ERROR;
}

/*	HTTPStatus Stream
**	-----------------
*/
PRIVATE const HTStreamClass HTTPStatusClass =
{		
    "HTTPStatus",
    HTTPStatus_flush,
    HTTPStatus_free,
    HTTPStatus_abort,
    HTTPStatus_put_character,
    HTTPStatus_put_string,
    HTTPStatus_put_block
};

#if 0
PRIVATE HTStream * HTTPStatus_new (HTRequest * request, http_info * http)
#endif

PUBLIC HTStream * HTTPStatus_new (HTRequest *	request,
				  void *	param,
				  HTFormat	input_format,
				  HTFormat	output_format,
				  HTStream *	output_stream)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTTPStatus_new");
    me->isa = &HTTPStatusClass;
    if (request) {
	HTNet * net = HTRequest_net(request);
        /* Get existing copy */
	http_info * http = (http_info *) HTNet_context(net);
	me->request = request;
	me->http = http;
	http->next = HTTP_ERROR;
	me->state = EOL_BEGIN;
	return me;
    } else
	return HTErrorStream();
}

/* ------------------------------------------------------------------------- */

/*		Load Document from HTTP Server			     HTLoadHTTP
**		==============================
**
**	Given a hypertext address, this routine loads a document.
**
** On entry,
**      request		This is the request structure
**	returns		HT_ERROR	Error has occured in call back
**			HT_OK		Call back was OK
*/
PUBLIC int HTLoadHTTP (SOCKET soc, HTRequest * request, SockOps ops)
{
    int status = HT_ERROR;
    HTNet * net = HTRequest_net(request);    /* Generic protocol information */
    http_info *http;			    /* Specific protocol information */
    HTParentAnchor *anchor = HTRequest_anchor(request);

    /*
    ** Initiate a new http structure and bind to request structure
    ** This is actually state HTTP_BEGIN, but it can't be in the state
    ** machine as we need the structure first.
    */
    if (ops == FD_NONE) {
	if (PROT_TRACE) HTTrace("HTTP........ Looking for `%s\'\n",
				HTAnchor_physical(anchor));
	if ((http = (http_info *) HT_CALLOC(1, sizeof(http_info))) == NULL)
	    HT_OUTOFMEM("HTLoadHTTP");
	http->state = HTTP_BEGIN;
	http->next = HTTP_GOT_DATA;
	HTNet_setContext(net, http);
    } else if (ops == FD_CLOSE) {			      /* Interrupted */
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_INTERRUPTED,
			   NULL, 0, "HTLoadHTTP");
	HTTPCleanup(request, HT_INTERRUPTED);
	return HT_OK;
    } else
	http = (http_info *) HTNet_context(net);	/* Get existing copy */
 
    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (http->state) {
	  case HTTP_BEGIN:
#if 0
	    /*
	     ** Compose authorization information (this was moved here
	     ** from after the making of the connection so that the connection
	     ** wouldn't have to wait while prompting username and password
	     ** from the user).				-- AL 13.10.93
	     */
	    if (HTAuth_generate(request)) {
		if (PROT_TRACE)HTTrace("HTTP........ Credentials generated\n");
		http->state = HTTP_NEED_CONNECTION;
	    } else {
		HTRequest_addError(request, ERR_FATAL, NO, HTERR_UNAUTHORIZED,
				   NULL, 0, "HTLoadHTTP");
		http->state = HTTP_ERROR;
	    }
#else
	    http->state = HTTP_NEED_CONNECTION;
#endif
	    break;
	    
	case HTTP_NEED_CONNECTION: 	    /* Now let's set up a connection */
	    status = HTDoConnect(net, HTAnchor_physical(anchor), HTTP_PORT);
	    if (status == HT_OK) {
		/*
		** Check the protocol class to see if we have connected to a
		** the right class of server, in this case HTTP.
		*/
		{
		    HTHost * host = HTNet_host(net);
		    char * s_class = HTHost_class(host);
		    if (s_class && strcasecomp(s_class, "http")) {
			HTRequest_addError(request, ERR_FATAL, NO, HTERR_CLASS,
					   NULL, 0, "HTLoadHTTP");
			http->state = HTTP_ERROR;
			break;
		    }
		    HTHost_setClass(host, "http");
		}

		/* 
		** Create the stream pipe FROM the channel to the application.
		** The target for the input stream pipe is set up using the
		** stream stack.
		*/
		HTNet_getInput(net,
			       HTStreamStack(WWW_HTTP,
					     HTRequest_outputFormat(request),
					     HTRequest_outputStream(request),
					     request, YES), NULL, 0);
		HTRequest_setOutputConnected(request, YES);

		/*
		** Create the stream pipe TO the channel from the application
		** and hook it up to the request object
		*/
		{
		    HTOutputStream * output = HTNet_getOutput(net, NULL, 0);
		    HTStream * app =
			HTMethod_hasEntity(HTRequest_method(request)) ?
		        HTMIMERequest_new(request,
			  HTTPRequest_new(request,(HTStream*) output,NO),YES) :
			    HTTPRequest_new(request, (HTStream*) output, YES);
		    HTRequest_setInputStream(request, app);
		}

		/*
		** Set up concurrent read/write if this request isn't the
		** source for a PUT or POST. As source we don't start reading
		** before all destinations are ready. If destination then
		** register the input stream and get ready for read
		*/
		if (HTRequest_isDestination(request)) {
		    HTEvent_register(net->sockfd, request, (SockOps) FD_READ,
				     HTLoadHTTP, net->priority);
		    HTRequest_linkDestination(request);
		}
		http->state = HTTP_NEED_REQUEST;
	    } else if (status == HT_WOULD_BLOCK || status == HT_PERSISTENT)
		return HT_OK;
	    else
		http->state = HTTP_ERROR;	       /* Error or interrupt */
	    break;

	    /* As we can do simultanous read and write this is now one state */
	  case HTTP_NEED_REQUEST:
	    if (ops == FD_WRITE || ops == FD_NONE) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTNet * srcnet = HTRequest_net(source);
		    if (srcnet) {
			SOCKET sockfd = HTNet_socket(srcnet);
			HTEvent_register(sockfd, source,
					 (SockOps) FD_READ,
					 srcnet->cbf, srcnet->priority);
			HTEvent_unregister(sockfd, FD_WRITE);
		    }
		    return HT_OK;
		}

		/*
		**  Should we use the input stream directly or call the post
		**  callback function to send data down to the network?
		*/
		{
		    HTStream * input = HTRequest_inputStream(request);
		    HTPostCallback * pcbf = HTRequest_postCallback(request);
		    status = pcbf ? pcbf(request, input) :
			(*input->isa->flush)(input);
		    if (status == HT_WOULD_BLOCK)
			return HT_OK;
		    else
			ops = FD_READ;	  /* Trick to ensure that we do READ */
		}
	    } else if (ops == FD_READ) {
		status = (*net->input->isa->read)(net->input);
		if (status == HT_WOULD_BLOCK || status == HT_PAUSE)
		    return HT_OK;
		else if (status==HT_CONTINUE) {
		    if (PROT_TRACE) HTTrace("HTTP........ Continuing\n");
		    ops = FD_WRITE;
		    continue;
		} else if (status==HT_LOADED || status==HT_CLOSED)
		    http->state = http->next;	       /* Jump to next state */
		else
		    http->state = HTTP_ERROR;
	    } else {
		http->state = HTTP_ERROR;
	    }
	    break;
	    
	  case HTTP_NOT_MODIFIED:
	    http->state = HTTP_ERROR;
	    break;

	  case HTTP_PERM_REDIRECT:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink * link =
			HTLink_find((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_ERROR);
		}
		HTRequest_killPostWeb(request);
	    }
	    HTTPCleanup(request, HT_PERM_REDIRECT);
	    return HT_OK;
	    break;


	  case HTTP_TEMP_REDIRECT:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link =
			HTLink_find((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_ERROR);
		}
		HTRequest_killPostWeb(request);
	    }
	    HTTPCleanup(request, HT_TEMP_REDIRECT);
	    return HT_OK;
	    break;

	  case HTTP_AA:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link =
			HTLink_find((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_ERROR);
		}
		HTRequest_killPostWeb(request);
	    }
	    HTTPCleanup(request, HT_NO_ACCESS);
	    return HT_OK;
	    break;

	  case HTTP_GOT_DATA:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link =
			HTLink_find((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_OK);
		}
	    }
	    HTTPCleanup(request, HT_LOADED);
	    return HT_OK;
	    break;
	    
	  case HTTP_NO_DATA:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link =
			HTLink_find((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_OK);
		}
	    }
	    HTTPCleanup(request, HT_NO_DATA);
	    return HT_OK;
	    break;
	    
	  case HTTP_RETRY:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link = 
			HTLink_find((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_ERROR);
		}
		HTRequest_killPostWeb(request);
	    }
	    HTTPCleanup(request, HT_RETRY);
	    return HT_OK;
	    break;

	  case HTTP_ERROR:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link =
			HTLink_find((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_ERROR);
		}
		HTRequest_killPostWeb(request);
	    }
	    HTTPCleanup(request, status);
	    return HT_OK;
	    break;
	}
    } /* End of while(1) */
}    


