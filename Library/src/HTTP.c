/*									 HTTP.c
**	MULTITHREADED IMPLEMENTATION OF HTTP CLIENT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module implements the HTTP protocol as a state machine
**
** History:
**    < May 24 94 ??	Unknown - but obviously written
**	May 24 94 HF	Made reentrent and cleaned up a bit. Implemented
**			Forward, redirection, error handling and referer field
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**	Jul 94 HFN	Written on top of HTTP.c, Henrik Frystyk
**      Fev 02 MKP      WebDAV status codes, Manuele Kirsch Pinheiro
**                      (Manuele.Kirsch_Pinheiro@inrialpes.fr)
**      Mar 29 MKP      Correcting WebDAV's 207 Multi-Status status code, that
**                      was returning HT_LOADED (200), and the right code is
**                      HT_MULTI_STATUS (207).
**
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTHeader.h"
#include "HTMIMERq.h"
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

#ifdef HTDEBUG
#include "WWWStream.h"
#define HTTP_OUTPUT     "w3chttp.out"
PRIVATE FILE * htfp = NULL;
#endif

/* Type definitions and global variables etc. local to this module */

/* Final states have negative value */
typedef enum _HTTPState {
    HTTP_KILL_PIPE	= -4,
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
    HTRequest *		request;
    HTTimer *		timer;
    BOOL		usedTimer;
    BOOL		repetitive_writing;
} http_info;

#define MAX_STATUS_LEN		100   /* Max nb of chars to check StatusLine */

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *		  	target;
    HTStream *		  	info_target;	/* For 100 codes */
    HTRequest *			request;
    http_info *			http;
    HTEOLState			state;
    BOOL			transparent;
    BOOL			cont;
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

/* How long to wait before writing the body in PUT and POST requests */
#define DEFAULT_FIRST_WRITE_DELAY	2000
#define DEFAULT_SECOND_WRITE_DELAY	3000
#define DEFAULT_REPEAT_WRITE		30

PRIVATE ms_t HTFirstWriteDelay = DEFAULT_FIRST_WRITE_DELAY;
PRIVATE ms_t HTSecondWriteDelay = DEFAULT_SECOND_WRITE_DELAY;
PRIVATE ms_t HTRepeatWrite = DEFAULT_REPEAT_WRITE;

#ifdef HT_NO_PIPELINING
PRIVATE HTTPConnectionMode ConnectionMode = HTTP_11_NO_PIPELINING;
#else
#ifdef HT_MUX
PRIVATE HTTPConnectionMode ConnectionMode = HTTP_11_MUX;
#else
#ifdef HT_FORCE_10
PRIVATE HTTPConnectionMode ConnectionMode = HTTP_FORCE_10;
#else
PRIVATE HTTPConnectionMode ConnectionMode = HTTP_11_PIPELINING;
#endif
#endif
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

    HTTRACE(PROT_TRACE, "HTTP Clean.. Called with status %d, net %p\n" _ status _ net);

    if (status == HT_INTERRUPTED) {
    	HTAlertCallback * cbf = HTAlert_find(HT_PROG_INTERRUPT);
    	if (cbf) (*cbf)(req, HT_PROG_INTERRUPT,
	    HT_MSG_NULL, NULL, NULL, NULL);
    } else if (status == HT_TIMEOUT) {
    	HTAlertCallback * cbf = HTAlert_find(HT_PROG_TIMEOUT);
    	if (cbf) (*cbf)(req, HT_PROG_TIMEOUT,
	    HT_MSG_NULL, NULL, NULL, NULL);
    }	

    /* Free stream with data TO network */
    if (input) {
	if (input->isa) {
	    if (status==HT_INTERRUPTED || status==HT_RECOVER_PIPE || status==HT_TIMEOUT)
		(*input->isa->abort)(input, NULL);
	    else
		(*input->isa->_free)(input);
	}
	HTRequest_setInputStream(req, NULL);
    }

    /*
    **  Remove if we have registered an upload function as a callback
    */
    if (http && http->timer) {
	HTTimer_delete(http->timer);
	http->timer = NULL;
	http->lock = NO;
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
#if 0
	HTRequest_addError(me->request, ERR_INFO, NO, HTERR_CONTINUE,
			   me->reason, (int) strlen(me->reason),
			   "HTTPInformation");
#endif
	http->result = HT_CONTINUE;
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
	return YES;
	break;

#ifdef HT_DAV
   case 102:            /* 102 Processing */
        /*
        ** MKP: 102 Processing indicates that the server is processing the
        **      request, and a final response will be sent later. So the client
        **      should wait for this final response.
        ** MKP: I'm not sure that it will work. Any suggestion??
        */      
        http->result = HT_CONTINUE;
        http->next = HTTP_CONNECTED;
        return YES;
        break;
#endif

        
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
    int error_class = me->status / 100;
    switch (error_class) {

    case 0:							     /* 0.9 response */
    case 2:

	switch (me->status) {

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

	case 207:						/* Partial Update OK */
	    HTRequest_addError(me->request, ERR_INFO, NO, HTERR_PARTIAL_OK,
			       me->reason, (int) strlen(me->reason),
			       "HTTPNextState");
	    http->next = HTTP_OK;
#ifdef HT_DAV                                   /* WebDAV : Multistatus status code */
            http->result = HT_MULTI_STATUS;
#else 
	    http->result = HT_PARTIAL_CONTENT;
#endif
	    break;

	default:
	    HTRequest_addError(me->request, ERR_INFO, NO, HTERR_OK,
			       me->reason, (int) strlen(me->reason),
			       "HTTPNextState");
	    http->next = HTTP_OK;
	    http->result = HT_LOADED;
	    break;
	}
	break;

    case 3:

	switch (me->status) {

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
	    http->result = HT_FOUND;
	    break;
	
	case 303:							   /* Method */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_METHOD,
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
	
#if 0
	case 306:						        /* Use proxy */
	    HTRequest_addError(me->request, ERR_INFO, NO, HTERR_PROXY_REDIRECT,
			       me->reason, (int) strlen(me->reason),
			       "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = HT_USE_PROXY;
	    break;
#endif

	case 307:						        /* Use proxy */
	    HTRequest_addError(me->request, ERR_INFO, NO, HTERR_TEMP_REDIRECT,
			       me->reason, (int) strlen(me->reason),
			       "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = HT_TEMP_REDIRECT;
	    break;

	default:
	    HTRequest_addError(me->request, ERR_INFO, NO, HTERR_MULTIPLE,
			       me->reason, (int) strlen(me->reason),
			       "HTTPNextState");
	    http->next = HTTP_OK;
	    http->result = HT_LOADED;
	    break;
	}	
	break;

    case 4:

	switch (me->status) {
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
	    http->result = -402;
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
	    http->result = -404;
	    break;
	
	case 405:						      /* Not Allowed */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_ALLOWED,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -405;
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
	    http->result = -408;
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
	    http->result = -410;
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
	    http->result = -412;
	    break;

	case 413:					 /* Request entity too large */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_TOO_BIG,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -413;
	    break;

	case 414:					     /* Request-URI too long */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_URI_TOO_BIG,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -414;
	    break;

	case 415:						      /* Unsupported */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_UNSUPPORTED,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -415;
	    break;

	case 416:				    /* Request Range not satisfiable */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_RANGE,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -416;
	    break;

	case 417:				               /* Expectation Failed */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_EXPECTATION_FAILED,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -417;
	    break;

	case 418:				        /* Reauthentication required */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_REAUTH,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -418;
	    break;

	case 419:				  /* Proxy Reauthentication required */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_PROXY_REAUTH,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -419;
	    break;

#ifdef HT_DAV
        case 422:                       /* WebDAV Unprocessable Entity */
            HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_UNPROCESSABLE,
                               me->reason, (int) strlen(me->reason), "HTTPNextState");
            http->next = HTTP_ERROR;
            http->result = HT_UNPROCESSABLE;
            break;

        case 423:                       /* WebDAV Locked */
            HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_LOCKED,
                               me->reason, (int) strlen(me->reason), "HTTPNextState");
            http->next = HTTP_ERROR;
            http->result = HT_LOCKED;
            break;

        case 424:                       /* WebDAV Failed Dependency */
            HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_FAILED_DEPENDENCY,
                               me->reason, (int) strlen(me->reason), "HTTPNextState");
            http->next = HTTP_ERROR;
            http->result = HT_FAILED_DEPENDENCY;
            break;
#endif
            
	default:
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_REQUEST,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -400;
	    break;
	}
	break;

    case 5:

	switch (me->status) {
	case 501:
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_IMPLEMENTED,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -501;
	    break;

	case 502:
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_GATE,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -502;
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
		    http->result = -500;
	    }
	    break;

	case 504:
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_GATE_TIMEOUT,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -504;
	    break;

	case 505:				     /* Unsupported protocol version */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_VERSION,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = HT_BAD_VERSION;
	    break;

	case 506:				   /* Partial update Not Implemented */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NO_PARTIAL_UPDATE,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = HT_BAD_VERSION;
	    break;

#ifdef HT_DAV
        case 507:                       /* WebDAV Insufficient Storage */
            HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_INSUFFICIENT_STORAGE,
                               me->reason, (int) strlen(me->reason), "HTTPNextState");
            http->next = HTTP_ERROR;
            http->result = HT_INSUFFICIENT_STORAGE;
            break;
#endif

	default:						       /* bad number */
	    HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_INTERNAL,
			       me->reason, (int) strlen(me->reason), "HTTPNextState");
	    http->next = HTTP_ERROR;
	    http->result = -500;
	    break;
	}
	break;

    default:
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_BAD_REPLY,
			   (void *) me->buffer, me->buflen, "HTTPNextState");
	http->next = HTTP_ERROR;
	http->result = -(me->status);
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
PRIVATE int stream_pipe (HTStream * me, int length)
{
    HTRequest * request = me->request;
    HTNet * net = HTRequest_net(request);
    HTHost * host = HTNet_host(net);

#if 0
    {
	char * uri = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
	fprintf(stderr, "HTTP header: %s for '%s'\n", me->buffer, uri);
	HT_FREE(uri);
    }
#endif
    
    /*
    ** Just check for HTTP and not HTTP/ as NCSA server chokes on 1.1 replies
    ** Thanks to Markku Savela <msa@msa.tte.vtt.fi>

    */
    if (strncasecomp(me->buffer, "http", 4)) {
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
        /* JK: in 2000, we don't expect many HTTP/0.9 servers to remain.
           I removed this line which made the backward change as most of
           the time we fall here more due to a network or server problem,
           rather than because we are accessing an old server. */
	/* HTHost_setVersion(host, HTTP_09); */
	if (length > 0) HTHost_setConsumed(host, length);
	HTTRACE(PROT_TRACE, "HTTP Status. `%s\' is probably a broken 1.0 server that doesn't understand HEAD\n" _ 
		    HTHost_name(host));
	return HT_ERROR;
    } else {
	HTResponse * response = HTRequest_response(request);
	char * ptr = me->buffer+5;		       /* Skip the HTTP part */
	char * vptr = NULL;
	int major = 0;
	int minor = 0;
	me->version = vptr = HTNextField(&ptr);
	if (vptr) {
	    major = (int) strtol(me->version, &vptr, 10);
	    if (vptr++) minor = strtol(vptr, NULL, 10);
	}

	/* Here we want to find out when to use persistent connection */
	if (major > 1 && major < 100) {
	    HTTRACE(PROT_TRACE, "HTTP Status. Major version number is %d\n" _ major);
	    me->target = HTErrorStream();
	    me->status = 9999;
	    HTTPNextState(me);					   /* Get next state */
	    return HT_ERROR;
	} else if (minor <= 0) {
	    if (major > 100) {
		HTTRACE(PROT_TRACE, "HTTP Status. This is a *BROKEN* HTTP/1.0 server\n");
		me->status = 200;
	    } else {
		HTTRACE(PROT_TRACE, "HTTP Status. This is an HTTP/1.0 server\n");
		me->status = atoi(HTNextField(&ptr));
	    }
	    HTHost_setVersion(host, HTTP_10);
	} else {					/* 1.x, x>0 family */
	    HTHost_setVersion(host, HTTP_11);		/* Best we can do */
	    if (ConnectionMode & HTTP_11_NO_PIPELINING) {
		HTTRACE(PROT_TRACE, "HTTP........ Mode is HTTP/1.1 with NO PIPELINING\n");
		HTNet_setPersistent(net, YES, HT_TP_SINGLE);
	    } else if (ConnectionMode & HTTP_11_MUX) {
		HTTRACE(PROT_TRACE, "HTTP........ Mode is HTTP/1.1 with MUXING\n");
		HTNet_setPersistent(net, YES, HT_TP_INTERLEAVE);
	    } else if (ConnectionMode & HTTP_FORCE_10) {
		HTTRACE(PROT_TRACE, "HTTP........ Mode is FORCE HTTP/1.0\n");
		HTHost_setVersion(host, HTTP_10);
		HTNet_setPersistent(net, NO, HT_TP_SINGLE);
	    } else
		HTNet_setPersistent(net, YES, HT_TP_PIPELINE);
	    me->status = atoi(HTNextField(&ptr));
	}

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
		if (me->status==100) {
		    me->buflen = 0;
		    me->state = EOL_BEGIN;
		    if (me->info_target) (*me->info_target->isa->_free)(me->info_target);
		    me->info_target = HTStreamStack(WWW_MIME_CONT,
						    HTRequest_debugFormat(request),
						    HTRequest_debugStream(request),
						    request, NO);
		    if (length > 0) HTHost_setConsumed(host, length);
		    return HT_OK;
		} else if (me->status==101) {
		    if (me->info_target) (*me->info_target->isa->_free)(me->info_target);
		    me->target = HTStreamStack(WWW_MIME_UPGRADE,
					       HTRequest_outputFormat(request),
					       HTRequest_outputStream(request),
					       request, NO);
		    if (length > 0) HTHost_setConsumed(host, length);
		    me->transparent = YES;
		    return HT_OK;
		}
	    }
	}

	/* 2000/Oct/27 JK: copying the current reason info into the 
	   response object.  */
	HTResponse_setReason (response, me->reason);

	/*
	**  As we are getting fresh metainformation in the HTTP response,
	**  we clear the old metainfomation in order not to mix it with the new
	**  one. This is particularly important for the content-length and the
	**  like. The TRACE and OPTIONS method just adds to the current 
	**  metainformation so in that case we don't clear the anchor.
	*/
#ifdef HT_DAV
        if (me->status==200 || me->status==203 ||
            me->status==207 ||  me->status==300) {
#else
	if (me->status==200 || me->status==203 || me->status==300) {
#endif
	    /*
	    **  200, 203 and 300 are all fully cacheable responses. No byte 
	    **  ranges or anything else make life hard in this case.
	    */
	    HTAnchor_clearHeader(HTRequest_anchor(request));
	    HTResponse_setCachable(response, HT_CACHE_ALL);
	    me->target = HTStreamStack(WWW_MIME,
				       HTRequest_outputFormat(request),
				       HTRequest_outputStream(request),
				       request, NO);
	} else if (me->status==204) {
	    HTResponse_setCachable(response, HT_CACHE_ALL);
	    me->target = HTStreamStack(WWW_MIME_HEAD,
				       HTRequest_debugFormat(request),
				       HTRequest_debugStream(request),
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
		HTResponse_setCachable(response, HT_CACHE_ALL);
		me->target = HTStreamStack(WWW_MIME_PART,
					   HTRequest_outputFormat(request),
					   HTRequest_outputStream(request),
					   request, NO);
	    } else {
		HTAnchor_clearHeader(HTRequest_anchor(request));
		me->target = HTStreamStack(WWW_MIME,
					   HTRequest_outputFormat(request),
					   HTRequest_outputStream(request),
					   request, NO);
	    }
	} else if (me->status==304) {
	    HTResponse_setCachable(response, HT_CACHE_NOT_MODIFIED);
	    me->target = HTStreamStack(WWW_MIME_HEAD,
				       HTRequest_debugFormat(request),
				       HTRequest_debugStream(request),
				       request, NO);
	} else if (HTRequest_debugStream(request)) {
	    HTResponse_setCachable(response,
				   (me->status == 201) ? HT_CACHE_ETAG : HT_NO_CACHE);
	    me->target = HTStreamStack(WWW_MIME,
				       HTRequest_debugFormat(request),
				       HTRequest_debugStream(request),
				       request, NO);
	} else {
	    /*
	    **  We still need to parse the MIME part in order to find any
	    **  valuable meta information which is needed from the response.
	    */
	    HTResponse_setCachable(response,
				   (me->status == 201) ? HT_CACHE_ETAG : HT_NO_CACHE);
	    me->target = HTStreamStack(WWW_MIME,
				       HTRequest_debugFormat(request),
				       HTRequest_debugStream(request),
				       request, NO);
	}
    }
    if (!me->target) me->target = HTErrorStream();
    HTTPNextState(me);					   /* Get next state */
    me->transparent = YES;
    if (length > 0) HTHost_setConsumed(HTNet_host(HTRequest_net(me->request)), length);
    return HT_OK;
}

/*
**	Searches for HTTP header line until buffer fills up or a CRLF or LF
**	is found
*/
PRIVATE int HTTPStatus_put_block (HTStream * me, const char * b, int l)
{
    int status = HT_OK;
    int length = l;
    me->startLen = me->buflen;
    while (!me->transparent && l-- > 0) {
	if (me->info_target) {

	    /* Put data down the 1xx return code parser until we are done. */
	    status = (*me->info_target->isa->put_block)(me->info_target, b, l+1);
	    if (status != HT_CONTINUE) return status;

	    /* Now free the info stream */
	    (*me->info_target->isa->_free)(me->info_target);
	    me->info_target = NULL;		

	    /* Update where we are in the stream */
	    l = HTHost_remainingRead(HTNet_host(HTRequest_net(me->request)));
	    b += (length-l);	    
	    length = l;
	    if (l <= 0) break;

	} else {
	    *(me->buffer+me->buflen++) = *b;
	    if (me->state == EOL_FCR) {
		if (*b == LF) {	/* Line found */
		    if ((status = stream_pipe(me, length-l)) != HT_OK) return status;
		} else {
		    me->state = EOL_BEGIN;
		}
	    } else if (*b == CR) {
		me->state = EOL_FCR;
	    } else if (*b == LF) {
		if ((status = stream_pipe(me, length-l)) != HT_OK) return status;
	    } else {
		if (me->buflen >= MAX_STATUS_LEN) {
		    if ((status = stream_pipe(me, length-l)) != HT_OK) return status;
		}
	    }
	    b++;
	}
    }

    if (!me->transparent && length != l)
	HTHost_setConsumed(HTNet_host(HTRequest_net(me->request)), length-l);

    if (l > 0) return PUTBLOCK(b, l);
    return status;
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
    HTTRACE(PROT_TRACE, "HTTPStatus.. ABORTING...\n");
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
    HTTRACE(PROT_TRACE, "HTTP........ Looking for `%s\'\n" _ 
			    HTAnchor_physical(anchor));
    if ((http = (http_info *) HT_CALLOC(1, sizeof(http_info))) == NULL)
      HT_OUTOFMEM("HTLoadHTTP");
    http->net = net;
    http->request = request;
    HTNet_setContext(net, http);
    HTNet_setEventCallback(net, HTTPEvent);
    HTNet_setEventParam(net, http);  /* callbacks get http* */

    return HTTPEvent(soc, http, HTEvent_BEGIN);	    /* get it started - ops is ignored */
}

PRIVATE int FlushPutEvent (HTTimer * timer, void * param, HTEventType type)
{
    http_info * http = (http_info *) param;
    HTStream * input = HTRequest_inputStream(http->request);
    HTPostCallback * pcbf = HTRequest_postCallback(http->request);
    int status = HT_ERROR;

    http->usedTimer = YES;
    if (timer != http->timer)
	HTDEBUGBREAK("HTTP timer %p not in sync\n" _ timer);
    HTTRACE(PROT_TRACE, "Uploading... Flushing %p with timer %p\n" _ http _ timer);

    /*
    **  Call the callback that will provide the data to save
    **  If the callback returns HT_OK then call it again until
    **  it returns something else than HT_OK.
    */
    if (http && input && pcbf) {
	status = (*pcbf)(http->request, input);
	HTTRACE(PROT_TRACE, "Uploading... Callback returned %d\n" _ status);	
    }

    /*
    **  If the callback returned something else than HT_OK then delete
    **  the timer, otherwise update it to a much shorter expiration
    **  time so that we can write some more data to the net.
    */
    if (status != HT_OK) {
	HTTimer_delete(http->timer);
	http->timer = NULL;
    } else if (!http->repetitive_writing) {
	http->timer = HTTimer_new(NULL, FlushPutEvent, http, HTRepeatWrite, YES, YES);
	http->repetitive_writing = YES;
    }
    return HT_OK;
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
    **  Check whether we have been interrupted or timed out
    */
    if (type == HTEvent_BEGIN) {
	http->next = HTTP_OK;
	http->result = HT_ERROR;
    } else if (type == HTEvent_CLOSE) {
        long read_len = HTNet_bytesRead(net);
        long doc_len = HTAnchor_length(anchor);

        /*
        ** It is OK to get a close if a) we don't pipeline and b)
        ** we have the expected amount of data, and c) we haven't
	** recieved a 100 Continue code. In case we don't
        ** know how much data to expect, we must accept it asis.
        */
        if (HTHost_numberOfOutstandingNetObjects(host) == 1 &&
	    http->result != HT_CONTINUE && (doc_len<0 || doc_len==read_len)) {
	    HTTPCleanup(request, http->result);		/* Raffaele Sena: was HT_LOADED */
        } else {
            HTRequest_addError(request, ERR_FATAL, NO, HTERR_INTERRUPTED,
			       NULL, 0, "HTLoadHTTP");
	    HTTPCleanup(request, HT_INTERRUPTED);
        }
	return HT_OK;
    } else if (type == HTEvent_TIMEOUT) {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_TIME_OUT,
			   NULL, 0, "HTLoadHTTP");
	HTTPCleanup(request, HT_TIMEOUT);
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
	    status = HTHost_connect(host, net, HTAnchor_physical(anchor));
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
			if (HTRequest_proxy(request) == NULL) {
			    HTAssocList * alist = HTRequest_connection(request);
			    if (!(alist && HTAssocList_findObject(alist, "close")))
				HTRequest_addConnection(request, "Keep-Alive", "");
			}
			HTHost_setClass(host, "http");
		    } else if (strcasecomp(s_class, "http")) {
			HTRequest_addError(request, ERR_FATAL, NO, HTERR_CLASS,
					   NULL, 0, "HTLoadHTTP");
			http->state = HTTP_ERROR;
			break;
		    }
		}

		if (ConnectionMode & HTTP_11_NO_PIPELINING) {
		    HTTRACE(PROT_TRACE, "HTTP........ Mode is HTTP/1.1 WITH NO PIPELINING\n");
		    HTRequest_setFlush(request, YES);
		} else if (ConnectionMode & HTTP_FORCE_10) {
		    HTTRACE(PROT_TRACE, "HTTP........ Mode is FORCE HTTP/1.0\n");
		    HTHost_setVersion(host, HTTP_10);
		}

		if (HTNet_preemptive(net)) {
		    HTTRACE(PROT_TRACE, "HTTP........ Force flush on preemptive load\n");
		    HTRequest_setFlush(request, YES);
		}

		/* Jump to next state */
		http->state = HTTP_NEED_STREAM;
	    } else if (status == HT_WOULD_BLOCK || status == HT_PENDING) {
		return HT_OK;
	    } else if (status == HT_NO_HOST) {
		http->result = HT_NO_HOST;
		http->state = HTTP_ERROR;
	    } else	
		http->state = HTTP_ERROR;	       /* Error or interrupt */
	    break;
	    
	case HTTP_NEED_STREAM:

	    /* 
	    ** Create the stream pipe FROM the channel to the application.
	    ** The target for the input stream pipe is set up using the
	    ** stream stack.
	    */
	    {
            /*
            **  during a recovery, we might keep the same HTNet object.
            **  if so, reuse it's read stream 
            */
	    HTStream * me = HTNet_readStream( net );
            if ( me == NULL ) {
                me = HTStreamStack(WWW_HTTP,
				   HTRequest_outputFormat(request),
				   HTRequest_outputStream(request),
				   request, YES);
#ifdef HTDEBUG
		if (PROT_TRACE) {
		    if (!htfp) htfp = fopen(HTTP_OUTPUT, "ab");
		    if (htfp) {
			me = HTTee(me, HTFWriter_new(request, htfp, YES), NULL);
			HTTRACE(PROT_TRACE, "HTTP........ Dumping response to `%s\'\n" _ HTTP_OUTPUT);
		    }
		}
#endif /* HTDEBUG */

		HTNet_setReadStream(net, me);
            }
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
		
#ifdef HTDEBUG
		if (PROT_TRACE) {
		    if (!htfp) htfp = fopen(HTTP_OUTPUT, "ab");
		    if (htfp) {
			output = (HTOutputStream *)
			    HTTee((HTStream *) output, HTFWriter_new(request, htfp, YES), NULL);
			HTTRACE(PROT_TRACE, "HTTP........ Dumping request to `%s\'\n" _ HTTP_OUTPUT);
		    }
		}	
#endif /* HTDEBUG */
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
		  HTStream * input = HTRequest_inputStream(request);
		  HTPostCallback * pcbf = HTRequest_postCallback(request);
		  status = HTRequest_flush(request) ?
		      HTHost_forceFlush(host) : (*input->isa->flush)(input);

		  /*
		  **  Check to see if we are uploading something or just a normal
		  **  GET kind of thing.
		  */
		  
		  /*
		  ** JK: don't continue sending things thru the network
		  ** if the flush resulted in an error or if the connection
		  ** is closed 
		  */
		  if ((status != HT_ERROR) && status != HT_CLOSED) {
		      if (pcbf) {
		          if (http->lock == NO) {
			      int retrys = HTRequest_retrys(request);
			      ms_t delay = retrys > 3 ? HTSecondWriteDelay : HTFirstWriteDelay;
			      if (!http->timer && !http->usedTimer) {
				  http->timer = HTTimer_new(NULL, FlushPutEvent,
							http, delay, YES, NO);
				  HTTRACE(PROT_TRACE, "Uploading... Holding %p for %lu ms using time %p\n" _ 
					  http _ delay _ http->timer);
				  HTHost_register(host, net, HTEvent_READ);
			      }
			      http->lock = YES;
			  }
			  type = HTEvent_READ;
		      } else {

			/*
			**  Check to see if we can start a new request
			**  pending in the host object.
			*/
			HTHost_launchPending(host);
			type = HTEvent_READ;
		      }
		  }

		  /* Now check the status code */
		  if (status == HT_WOULD_BLOCK)
		      return HT_OK;
		  else if (status == HT_PAUSE || status == HT_LOADED) {
		      type = HTEvent_READ;
		  } else if (status==HT_ERROR)
		      http->state = HTTP_RECOVER_PIPE;
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
		      HTTRACE(PROT_TRACE, "HTTP........ Continuing\n");
		      http->lock = NO;
		      continue;
		  } else if (status==HT_LOADED)
		      http->state = http->next;	/* Jump to next state (OK or ERROR) */
		  else if (status==HT_CLOSED)
		      http->state = HTTP_RECOVER_PIPE;
		  else if (status == HT_ERROR)
		      http->state = HTTP_KILL_PIPE;
		  else
		      http->state = HTTP_ERROR;
	      } else {
		  http->state = HTTP_ERROR;	/* don't know how to handle OOB */
	      }
	      break;

	  case HTTP_OK:
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
	      if (HTHost_isPersistent(host) && !HTHost_closeNotification(host)) {
		  if (host == NULL) return HT_ERROR;
		  HTRequest_setFlush(request, YES);

		  /*
		  **  If we already have recovered more than we want and
		  **  this call returns NO then simply kill the pipe.
		  **  Otherwise we may loop forever.
		  */
		  if (HTHost_recoverPipe(host) != YES) {
		      HTRequest_addError(request, ERR_FATAL, NO, HTERR_BAD_REPLY,
					 NULL, 0, "HTTPEvent");
		      http->state = HTTP_KILL_PIPE;
		      break;
		  }
		  return HT_OK;
	      } else
		  http->state = HTTP_OK;
	  }
	  break;

          case HTTP_KILL_PIPE:
	      if (host == NULL) return HT_ERROR;
	      HTHost_killPipe(host);
	      return HT_OK;
	      break;

	  case HTTP_ERROR:
	      HTTPCleanup(request, http->result);
	      return HT_OK;
	      break;

	default:
	    HTDEBUGBREAK("Bad http state %d\n" _ http->state);
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

PUBLIC BOOL HTTP_setBodyWriteDelay (ms_t first_try, ms_t second_try)
{
	if (first_try > 20 && second_try >= first_try) {
	    HTFirstWriteDelay = first_try;
		HTSecondWriteDelay = second_try;
		return YES;
	}
	return NO;
}

PUBLIC void HTTP_bodyWriteDelay (ms_t * first_try, ms_t * second_try)
{
	*first_try = HTFirstWriteDelay;
	*second_try = HTSecondWriteDelay;
}

