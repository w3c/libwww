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

#define HTTP_DUMP
#ifdef HTTP_DUMP
#define HTTP_OUTPUT     "w3chttp.out"
#endif

/* Type definitions and global variables etc. local to this module */

/* Final states have negative value */
typedef enum _HTTPState {
    HTTP_RECOVER_PIPE	= -3,
    HTTP_ERROR		= -2,
    HTTP_OK		= -1,
    HTTP_BEGIN		= 0,
    HTTP_NEED_STREAM,
    HTTP_CONNECTED
} HTTPState;

/* This is the context structure for the this module */
typedef struct _http_info {
    HTTPState		state;		  /* Current State of the connection */
    HTTPState		next;				       /* Next state */
    int			result;	     /* Result to report to the after filter */
    BOOL		lock;				/* Block for writing */
    HTNet *		net;
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
    int				startLen;/* buflen when put_block was called */
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

#ifdef HT_NO_PIPELINING
PRIVATE HTTPConnectionMode ConnectionMode = HTTP_NO_PIPELINING;
#else
PRIVATE HTTPConnectionMode ConnectionMode = 0;
#endif

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

    if (PROT_TRACE)
	HTTrace("HTTP Clean.. Called with status %d, net %p\n", status, net);

    /* Free stream with data TO network */
    if (HTRequest_isDestination(req))
	HTRequest_removeDestination(req);
    else if (input) {
	if (status == HT_INTERRUPTED || status == HT_RECOVER_PIPE)
	    (*input->isa->abort)(input, NULL);
	else
	    (*input->isa->_free)(input);
	HTRequest_setInputStream(req, NULL);
    }

    /*
    ** Remove the request object and our own context structure for http.
    */
    if (status != HT_RECOVER_PIPE) {
	HTNet_delete(net, status);
	HT_FREE(http);
    }
    return YES;
}

/*
**	Informational 1xx codes are handled separately
**	Returns YES if we should continue, NO if we should stop
*/
PRIVATE BOOL HTTPInformation (HTStream * me)
{
    http_info * http = me->http;
    switch (me->status) {

    case 100:
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_CONTINUE,
			   me->reason, (int) strlen(me->reason),
			   "HTTPInformation");
	return YES;
	break;

    case 101:
	/*
	**  We consider 101 Switching as a final state and exit this request
	*/
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_SWITCHING,
			   me->reason, (int) strlen(me->reason),
			   "HTTPInformation");
	http->next = HTTP_OK;
	http->result = HT_UPGRADE;
	break;

    default:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_REPLY,
		   (void *) me->buffer, me->buflen, "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;
    }
    return NO;
}

/*
**	This is a big switch handling all HTTP return codes. It puts in any
**	appropiate error message and decides whether we should expect data
**	or not.
*/
PRIVATE void HTTPNextState (HTStream * me)
{
    http_info * http = me->http;
    switch (me->status) {

      case 0:						     /* 0.9 response */
      case 200:							       /* OK */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_OK,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_OK;
	http->result = HT_LOADED;
	break;

      case 201:							  /* Created */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_CREATED,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_OK;
	http->result = HT_CREATED;
	break;

      case 202:							 /* Accepted */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_ACCEPTED,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_OK;
	http->result = HT_ACCEPTED;
	break;

      case 203:				    /* Non-authoritative information */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_NON_AUTHORITATIVE,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_OK;
	http->result = HT_LOADED;
	break;

      case 204:						      /* No Response */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_NO_CONTENT,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_OK;
	http->result = HT_NO_DATA;
	break;

      case 205:						    /* Reset Content */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_RESET,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_OK;
	http->result = HT_RESET_CONTENT;
	break;

      case 206:						  /* Partial Content */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_PARTIAL,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_OK;
	http->result = HT_PARTIAL_CONTENT;
	break;

      case 300:						 /* Multiple Choices */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_MULTIPLE,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_OK;
	http->result = HT_LOADED;
	break;

      case 301:						   	    /* Moved */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_MOVED,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_PERM_REDIRECT;
	break;

      case 302:							    /* Found */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_FOUND,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_TEMP_REDIRECT;
	break;
	
      case 303:							   /* Method */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_IMPLEMENTED,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_SEE_OTHER;
	break;

      case 304:						     /* Not Modified */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_NOT_MODIFIED,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_OK;
	http->result = HT_NOT_MODIFIED;
	break;
	
      case 305:						        /* Use proxy */
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_USE_PROXY,
			   me->reason, (int) strlen(me->reason),
			   "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_USE_PROXY;
	break;
	
      case 400:						      /* Bad Request */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_REQUEST,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 401:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_UNAUTHORIZED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_NO_ACCESS;
	break;
	
      case 402:						 /* Payment required */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_PAYMENT_REQUIRED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;
	
      case 403:							/* Forbidden */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_FORBIDDEN,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_FORBIDDEN;
	break;
	
      case 404:							/* Not Found */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_FOUND,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;
	
      case 405:						      /* Not Allowed */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_ALLOWED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 406:						  /* None Acceptable */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NONE_ACCEPTABLE,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_NOT_ACCEPTABLE;
	break;

      case 407:			       	    /* Proxy Authentication Required */
	HTRequest_addError(me->request, ERR_FATAL, NO,HTERR_PROXY_UNAUTHORIZED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_NO_PROXY_ACCESS;
	break;

      case 408:						  /* Request Timeout */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_TIMEOUT,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 409:						  	 /* Conflict */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_CONFLICT,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_CONFLICT;
	break;

      case 410:						  	     /* Gone */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_GONE,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 411:						  /* Length Required */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_LENGTH_REQUIRED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_LENGTH_REQUIRED;
	break;

      case 412:					      /* Precondition failed */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_PRECON_FAILED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 413:					 /* Request entity too large */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_TOO_BIG,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 414:					     /* Request-URI too long */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_URI_TOO_BIG,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 415:						      /* Unsupported */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_UNSUPPORTED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 500:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_INTERNAL,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;
	
      case 501:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_IMPLEMENTED,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 502:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_GATE,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 503:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_DOWN,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
	http->next = HTTP_ERROR;

	/*
	** If Retry-After header is found then return HT_RETRY else HT_ERROR.
	** The caller may want to reissue the request at a later point in time.
	*/
	{
	    HTResponse * response = HTRequest_response(me->request);
	    if (HTResponse_retryTime(response))
		http->result = HT_RETRY;
	    else
		http->result = HT_ERROR;
	}
	break;

      case 504:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_GATE_TIMEOUT,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
        http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;

      case 505:				     /* Unsupported protocol version */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_VERSION,
		   me->reason, (int) strlen(me->reason), "HTTPNextState");
        http->next = HTTP_ERROR;
	http->result = HT_BAD_VERSION;
	break;

      default:						       /* bad number */
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_REPLY,
		   (void *) me->buffer, me->buflen, "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = HT_ERROR;
	break;
    }
}

/* ------------------------------------------------------------------------- */
/* 			    HTTP Status Line Stream			     */
/* ------------------------------------------------------------------------- */

/*
**	Analyze the stream we have read. If it is a HTTP 1.0 or higher
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
    HTRequest * request = me->request;
    HTNet * net = HTRequest_net(request);
    HTHost * host = HTNet_host(net);

    if (me->target) {
	int status = PUTBLOCK(me->buffer, me->buflen);
	if (status == HT_OK)
	    me->transparent = YES;
	return status;
    } else if (HTRequest_isSource(request)&&!HTRequest_outputStream(request)) {
	/*
	**  We need to wait for the destinations to get ready
	*/
	return HT_WOULD_BLOCK;
    }
	
    /*
    ** Just check for HTTP and not HTTP/ as NCSA server chokes on 1.1 replies
    ** Thanks to Markku Savela <msa@msa.tte.vtt.fi>

    */
#if 0
    if (strncasecomp(me->buffer, "http/", 5)) {
#else
    if (strncasecomp(me->buffer, "http", 4)) {
#endif
	int status;
	HTRequest_addError(request, ERR_INFO, NO, HTERR_HTTP09,
		   (void *) me->buffer, me->buflen, "HTTPStatusStream");
	me->target = HTStreamStack(WWW_UNKNOWN,
				   HTRequest_outputFormat(request),
				   HTRequest_outputStream(request),
				   request, NO);
	me->http->next = HTTP_OK;
	if ((status = PUTBLOCK(me->buffer, me->buflen)) == HT_OK)
	    me->transparent = YES;
	HTHost_setVersion(host, HTTP_09);
	return status;
    } else {
	HTResponse * response = HTRequest_response(request);
	char *ptr = me->buffer+4;		       /* Skip the HTTP part */
	me->version = HTNextField(&ptr);
	HTHost_setConsumed(net->host, me->buflen - me->startLen);

	/* Here we want to find out when to use persistent connection */
	if (!strncasecomp(me->version, "/1.0", 4)) {
	    if (PROT_TRACE)HTTrace("HTTP Status. This is a HTTP/1.0 server\n");
	    HTHost_setVersion(host, HTTP_10);
	} else if (!strncasecomp(me->version, "/1.", 3)) {     /* 1.x family */
	    HTHost_setVersion(host, HTTP_11);
#ifdef HT_MUX
	    HTNet_setPersistent(net, YES, HT_TP_INTERLEAVE);
#else
	    if (ConnectionMode & HTTP_NO_PIPELINING) {
		if (PROT_TRACE) HTTrace("HTTP........ Mode is HTTP/1.1 WITH NO PIPELINING\n");
		HTNet_setPersistent(net, YES, HT_TP_SINGLE);
	    } else if (ConnectionMode & HTTP_FORCE_10) {
		if (PROT_TRACE) HTTrace("HTTP........ Mode is FORCE HTTP/1.0\n");
		HTHost_setVersion(host, HTTP_10);
		HTNet_setPersistent(net, NO, HT_TP_SINGLE);
	    } else
		HTNet_setPersistent(net, YES, HT_TP_PIPELINE);
#endif /* HT_MUX */
	} else { 
	    if (PROT_TRACE)HTTrace("HTTP Status. No 1.x version number - treat it as a HTTP/1.0 server\n");
	    HTHost_setVersion(host, HTTP_10);
	}

	me->status = atoi(HTNextField(&ptr));
	/* Kludge to fix the NCSA server version bug */
	if (me->status == 0) me->status = atoi(me->version);

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
	    if (HTTPInformation(me) == YES) {
		me->buflen = 0;
		me->state = EOL_BEGIN;
		return HTMethod_hasEntity(HTRequest_method(request)) ?
		    HT_CONTINUE : HT_OK;
	    }
	}

	/*
	**  As we are getting fresh metainformation in the HTTP response,
	**  we clear the old metainfomation in order not to mix it with the new
	**  one. This is particularly important for the content-length and the
	**  like. The TRACE and OPTIONS method just adds to the current 
	**  metainformation so in that case we don't clear the anchor.
	*/
	if (me->status==200 || me->status==203 || me->status==300) {
	    /*
	    **  200, 203 and 300 are all fully cacheable responses. No byte 
	    **  ranges or anything else make life hard in this case.
	    */
	    HTAnchor_clearHeader(HTRequest_anchor(request));
	    HTResponse_setCachable(response, YES);
	    me->target = HTStreamStack(WWW_MIME,
				       HTRequest_outputFormat(request),
				       HTRequest_outputStream(request),
				       request, NO);
	} else if (me->status==206) {
	    /*
	    **  We got a partial response and now we must check whether
	    **  we issued a cache If-Range request or it was a new 
	    **  partial response which we don't have in cache. In the latter
	    **  case, we don't cache the object and in the former we append
	    **  the result to the already existing cache entry.
	    */
	    HTReload reload = HTRequest_reloadMode(request);
	    if (reload == HT_CACHE_RANGE_VALIDATE) {
		HTResponse_setCachable(response, YES);
		me->target = HTStreamStack(WWW_MIME_PART,
					   HTRequest_outputFormat(request),
					   HTRequest_outputStream(request),
					   request, NO);
	    } else {
		me->target = HTStreamStack(WWW_MIME,
					   HTRequest_outputFormat(request),
					   HTRequest_outputStream(request),
					   request, NO);
	    }
	} else if (me->status==204 || me->status==304) {
	    HTResponse_setCachable(response, YES);
	    me->target = HTStreamStack(WWW_MIME_HEAD,
				       HTRequest_debugFormat(request),
				       HTRequest_debugStream(request),
				       request, NO);
	} else if (HTRequest_debugStream(request)) {
	    me->target = HTStreamStack(WWW_MIME,
				       HTRequest_debugFormat(request),
				       HTRequest_debugStream(request),
				       request, NO);
	} else {
	    /*
	    **  We still need to parse the MIME part in order to find any
	    **  valuable meta information which is needed from the response.
	    */
	    me->target = HTStreamStack(WWW_MIME,
				       HTRequest_debugFormat(request),
				       HTRequest_debugStream(request),
				       request, NO);
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
    me->startLen = me->buflen;
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
PRIVATE int HTTPEvent (SOCKET soc, void * pVoid, HTEventType type);

PUBLIC int HTLoadHTTP (SOCKET soc, HTRequest * request)
{
    http_info *http;			    /* Specific protocol information */
    HTParentAnchor *anchor = HTRequest_anchor(request);
    HTNet * net = HTRequest_net(request);

    /*
    ** Initiate a new http structure and bind to request structure
    ** This is actually state HTTP_BEGIN, but it can't be in the state
    ** machine as we need the structure first.
    */
    if (PROT_TRACE) HTTrace("HTTP........ Looking for `%s\'\n",
			    HTAnchor_physical(anchor));
    if ((http = (http_info *) HT_CALLOC(1, sizeof(http_info))) == NULL)
      HT_OUTOFMEM("HTLoadHTTP");
    http->net = net;
    HTNet_setContext(net, http);
    HTNet_setEventCallback(net, HTTPEvent);
    HTNet_setEventParam(net, http);  /* callbacks get http* */

    return HTTPEvent(soc, http, HTEvent_BEGIN);		/* get it started - ops is ignored */
}

PRIVATE int HTTPEvent (SOCKET soc, void * pVoid, HTEventType type)
{
    http_info * http = (http_info *)pVoid;
    int status = HT_ERROR;
    HTNet * net = http->net;
    HTRequest * request = HTNet_request(net);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTHost * host = HTNet_host(net);

    /*
    **  Check whether we have been interrupted
    */
    if (type == HTEvent_BEGIN) {
	http->next = HTTP_OK;
	http->result = HT_ERROR;
    } else if (type == HTEvent_CLOSE) {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_INTERRUPTED,
			   NULL, 0, "HTLoadHTTP");
	HTTPCleanup(request, HT_INTERRUPTED);
	return HT_OK;
    } else if (type == HTEvent_END) {
	HTTPCleanup(request, http->result);
	return HT_OK;
    } else if (type == HTEvent_RESET) {
	HTTPCleanup(request, HT_RECOVER_PIPE);
	http->state = HTTP_BEGIN;
	return HT_OK;
    }

    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (http->state) {
	case HTTP_BEGIN:
	    status = HTHost_connect(host, net, HTAnchor_physical(anchor), HTTP_PORT);
	    host = HTNet_host(net);
	    if (status == HT_OK) {

		/*
		**  Check the protocol class to see if we have connected to a
		**  the right class of server, in this case HTTP. If we don't 
		**  know the server then assume a HTTP/1.0
		*/
		{
		    char * s_class = HTHost_class(host);
		    if (!s_class) {
			if (HTRequest_proxy(request) == NULL)
			    HTRequest_addConnection(request, "Keep-Alive", "");
			HTHost_setClass(host, "http");
		    } else if (strcasecomp(s_class, "http")) {
			HTRequest_addError(request, ERR_FATAL, NO, HTERR_CLASS,
					   NULL, 0, "HTLoadHTTP");
			http->state = HTTP_ERROR;
			break;
		    }
		}

		if (ConnectionMode & HTTP_NO_PIPELINING) {
		    if (PROT_TRACE) HTTrace("HTTP........ Mode is HTTP/1.1 WITH NO PIPELINING\n");
		    HTRequest_setFlush(request, YES);
		} else if (ConnectionMode & HTTP_FORCE_10) {
		    if (PROT_TRACE) HTTrace("HTTP........ Mode is FORCE HTTP/1.0\n");
		    HTHost_setVersion(host, HTTP_10);
		}

		/* Jump to next state */
		http->state = HTTP_NEED_STREAM;
	    } else if (status == HT_WOULD_BLOCK || status == HT_PENDING)
		return HT_OK;
	    else	
		http->state = HTTP_ERROR;	       /* Error or interrupt */
	    break;
	    
	case HTTP_NEED_STREAM:

#if 0
	    HTChannel_upSemaphore(host->channel);
#endif

	    /* 
	    ** Create the stream pipe FROM the channel to the application.
	    ** The target for the input stream pipe is set up using the
	    ** stream stack.
	    */
	    {
		HTStream *me=HTStreamStack(WWW_HTTP,
					   HTRequest_outputFormat(request),
					   HTRequest_outputStream(request),
					   request, YES);
		HTNet_setReadStream(net, me);
		HTRequest_setOutputConnected(request, YES);
	    }

	    /*
	    ** Create the stream pipe TO the channel from the application
	    ** and hook it up to the request object
	    */
	    {
		HTChannel * channel = HTHost_channel(host);
		HTOutputStream * output = HTChannel_getChannelOStream(channel);
		int version = HTHost_version(host);
		HTStream * app = NULL;
		
		/*
		**  Instead of calling this directly we could have a 
		**  stream stack builder on the output stream as well
		*/
#ifdef HT_MUX
		output = (HTOutputStream *)
		    HTBuffer_new(HTMuxWriter_new(host, net, output), request, 512);
#endif
		    
#ifdef HTTP_DUMP
		if (PROT_TRACE) {
		    FILE * htfp = NULL;
		    if ((htfp = fopen(HTTP_OUTPUT, "ab")) != NULL) {
			output = (HTOutputStream *)
			    HTTee((HTStream *) output, HTFWriter_new(request, htfp, NO), NULL);
			HTTrace("HTTP........ Dumping request to `%s\'\n", HTTP_OUTPUT);
		    }
		}	
#endif /* HTTP_DUMP */
		app = HTMethod_hasEntity(HTRequest_method(request)) ?
		    HTMIMERequest_new(request,
				      HTTPRequest_new(request, (HTStream *) output, NO,
						      version),
				      YES) :
		    HTTPRequest_new(request, (HTStream *) output, YES, version);
		HTRequest_setInputStream(request, app);
	    }

	    /*
	    ** Set up concurrent read/write if this request isn't the
	    ** source for a PUT or POST. As source we don't start reading
	    ** before all destinations are ready. If destination then
	    ** register the input stream and get ready for read
	    */
	    if (HTRequest_isDestination(request)) {
		HTHost_register(host, net, HTEvent_READ);
		HTRequest_linkDestination(request);
	    }
	    http->state = HTTP_CONNECTED;
	    type = HTEvent_WRITE;			    /* fresh, so try a write */
	    break;

	    /* As we can do simultanous read and write this is now one state */
	  case HTTP_CONNECTED:
	      if (type == HTEvent_WRITE) {
		 if (HTRequest_isDestination(request)) {
		     HTRequest * source = HTRequest_source(request);
		     HTNet * srcnet = HTRequest_net(source);
		     if (srcnet) {
			 HTHost_register(srcnet->host, srcnet, HTEvent_READ);
			 HTHost_unregister(srcnet->host, srcnet, HTEvent_WRITE);
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
		    if (pcbf) {
			if (http->lock)
			    return HT_OK;
			else {
			    status = (*pcbf)(request, input);
			    if (status == HT_PAUSE || status == HT_LOADED) {
				type = HTEvent_READ;
				http->lock = YES;
			    } else if (status==HT_CLOSED)
				http->state = HTTP_RECOVER_PIPE;
			    else if (status == HT_ERROR) {
				http->result = HT_INTERRUPTED;
				http->state = HTTP_ERROR;
				break;
			    }
			}
		    } else {
			/*
			**  Check to see if we can start a new request
			**  pending in the host object.
			*/
			HTHost_launchPending(host);
			status = HTRequest_flush(request) ?
			    HTHost_forceFlush(host) : (*input->isa->flush)(input);
			type = HTEvent_READ;
		    }
		    if (status == HT_WOULD_BLOCK) return HT_OK;
		}
	    } else if (type == HTEvent_FLUSH) {
		HTStream * input = HTRequest_inputStream(request);
		if (input == NULL)
		    return HT_ERROR;
		return (*input->isa->flush)(input);
	    } else if (type == HTEvent_READ) {
		status = HTHost_read(host, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_CONTINUE) {
		    if (PROT_TRACE) HTTrace("HTTP........ Continuing\n");
		    http->lock = NO;
		    type = HTEvent_WRITE;
		    continue;
		} else if (status==HT_LOADED)
		    http->state = http->next;	/* Jump to next state (OK or ERROR) */
		else if (status==HT_CLOSED)
		    http->state = HTTP_RECOVER_PIPE;
		else
		    http->state = HTTP_ERROR;
	    } else {
		http->state = HTTP_ERROR;	/* don't know how to handle OOB */
	    }
	    break;

	  case HTTP_OK:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link =
			HTLink_find((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_OK);
		}
	    }
	    HTTPCleanup(request, http->result);
	    return HT_OK;
	    break;

          case HTTP_RECOVER_PIPE:
	  {
	      /*
	      ** If this is a persistent connection and we get a close
	      ** then it is an error and we should recover from it by
	      ** restarting the pipe line of requests if any
	      */
	      if (HTHost_isPersistent(host)) {
		  if (host == NULL) return HT_ERROR;
		  HTRequest_setFlush(request, YES);
		  HTHost_recoverPipe(host);
		  http->state = HTTP_BEGIN;
		  HTHost_launchPending(host);
		  return HT_OK;
	      } else
		  http->state = HTTP_OK;
	  }
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
	      HTTPCleanup(request, http->result);
	      return HT_OK;
	      break;

	default:
	    HTTrace("bad http state %d.\n", http->state);
	    HTDebugBreak();
	}
    } /* End of while(1) */
}    

PUBLIC void HTTP_setConnectionMode (HTTPConnectionMode mode)
{
    ConnectionMode = mode;
}

PUBLIC HTTPConnectionMode HTTP_connectionMode (void)
{
    return ConnectionMode;
}

