/*									 HTTP.c
**	MULTITHREADED IMPLEMENTATION OF HTTP CLIENT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
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
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTTCP.h"
#include "HTFormat.h"
#include "HTSocket.h"
#include "HTReqMan.h"
#include "HTAlert.h"
#include "HTMIME.h"
#include "HTAABrow.h"		/* Access Authorization */
#include "HTTee.h"		/* Tee off a cache stream */
#include "HTFWrite.h"		/* Write to cache file */
#include "HTCache.h"
#include "HTWriter.h"
#include "HTError.h"
#include "HTAccess.h"
#include "HTChunk.h"
#include "HTGuess.h"
#include "HTNetMan.h"
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
    HTTP_REDIRECTION,
    HTTP_NOT_MODIFIED,
    HTTP_EXPIRED,
    HTTP_AA
} HTTPState;

/* This is the context structure for the this module */
typedef struct _http_info {
    HTTPState		state;		  /* Current State of the connection */
    HTTPState		next;				       /* Next state */
} http_info;

#define MAX_STATUS_LEN		100   /* Max nb of chars to check StatusLine */

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    http_info *			http;
    HTSocketEOL			state;
    BOOL			transparent;
    char *			version;	     /* Should we save this? */
    int				status;
    char *			reason;
    char 			buffer[MAX_STATUS_LEN+1];
    int				buflen;
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
    HTNet *net = req->net;
    http_info *http = (http_info *) net->context;

    /* Free stream with data TO network */
    if (!HTRequest_isDestination(req) && req->input_stream) {
	if (status == HT_INTERRUPTED)
	    (*req->input_stream->isa->abort)(req->input_stream, NULL);
	else
	    (*req->input_stream->isa->_free)(req->input_stream);
    }

    /* Remove the request object and our own context structure for http */
    HTNet_delete(net, status);
    FREE(http);
    return YES;
}


PRIVATE BOOL HTTPAuthentication (HTRequest * request)
{
    HTAAScheme scheme;
    HTList *valid_schemes = HTList_new();
    HTAssocList **scheme_specifics = NULL;
    char *tmplate = NULL;

    if (request->WWWAAScheme) {
	if ((scheme = HTAAScheme_enum(request->WWWAAScheme)) != HTAA_UNKNOWN) {
	    HTList_addObject(valid_schemes, (void *) scheme);
	    if (!scheme_specifics) {
		int i;
		scheme_specifics = (HTAssocList**)
		    malloc(HTAA_MAX_SCHEMES * sizeof(HTAssocList*));
		if (!scheme_specifics)
		    outofmem(__FILE__, "HTTPAuthentication");
		for (i=0; i < HTAA_MAX_SCHEMES; i++)
		    scheme_specifics[i] = NULL;
	    }
	    scheme_specifics[scheme] = HTAA_parseArgList(request->WWWAARealm);
	} else if (PROT_TRACE) {
	    HTErrorAdd(request, ERR_INFO, NO, HTERR_UNKNOWN_AA,
		       (void *) request->WWWAAScheme, 0, "HTTPAuthentication");
	    return NO;
	}
    }
    if (request->WWWprotection) {
	if (PROT_TRACE)
	    fprintf(TDEST, "Protection template set to `%s'\n",
		    request->WWWprotection);
	StrAllocCopy(tmplate, request->WWWprotection);
    }
    request->valid_schemes = valid_schemes;
    request->scheme_specifics = scheme_specifics;
    request->prot_template = tmplate;
    return YES;
}


/*
**	This is a big switch handling all HTTP return codes. It puts in any
**	appropiate error message and decides whether we should expect data
**	or not.
*/
PRIVATE void HTTPNextState ARGS1(HTStream *, me)
{
    switch (me->status) {

      case 0:						     /* 0.9 response */
      case 200:
      case 201:
      case 202:
      case 203:
	me->http->next = HTTP_GOT_DATA;
	break;

      case 204:						      /* No Response */
	me->http->next = HTTP_NO_DATA;
	break;

      case 301:						   	    /* Moved */
      case 302:							    /* Found */
	me->http->next = HTTP_REDIRECTION;
	break;
	
      case 303:							   /* Method */
	HTAlert(me->request,
		"This client doesn't support automatic redirection of type `Method'");
	me->http->next = HTTP_ERROR;
	break;

      case 304:						     /* Not Modified */
	me->http->next = HTTP_NOT_MODIFIED;
	break;
	
      case 400:						      /* Bad Request */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_BAD_REQUEST,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;

      case 401:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_UNAUTHORIZED,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_AA;
	break;
	
      case 402:						 /* Payment required */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_PAYMENT_REQUIRED,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;
	
      case 403:							/* Forbidden */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_FORBIDDEN,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;
	
      case 404:							/* Not Found */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_NOT_FOUND,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;
	
      case 405:						      /* Not Allowed */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_NOT_ALLOWED,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;

      case 406:						  /* None Acceptable */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_NONE_ACCEPTABLE,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;

      case 407:			       	    /* Proxy Authentication Required */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_PROXY,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;

      case 408:						  /* Request Timeout */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_TIMEOUT,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;

      case 500:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_INTERNAL,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;
	
      case 501:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_NOT_IMPLEMENTED,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;

      case 502:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_BAD_GATE,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
	me->http->next = HTTP_ERROR;
	break;

      case 503:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_DOWN,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");

	/* If Retry-After header is found then return HT_RETRY else HT_ERROR */
	if (me->request->retry_after)
	    me->http->next = HTTP_RETRY;
	else
	    me->http->next = HTTP_ERROR;
	break;

      case 504:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_GATE_TIMEOUT,
		   me->reason, (int) strlen(me->reason), "HTLoadHTTP");
        me->http->next = HTTP_ERROR;
	break;

      default:						       /* bad number */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_BAD_REPLY,
		   (void *) me->buffer, me->buflen, "HTLoadHTTP");
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
PRIVATE int stream_pipe ARGS1(HTStream *, me)
{
    HTRequest *req = me->request;
    HTNet *net = req->net;
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
	HTErrorAdd(req, ERR_INFO, NO, HTERR_HTTP09,
		   (void *) me->buffer, me->buflen, "HTTPStatusStream");
	me->target = HTGuess_new(req, NULL, WWW_UNKNOWN,
				 req->output_format, req->output_stream);
	me->http->next = HTTP_GOT_DATA;
	if ((status = PUTBLOCK(me->buffer, me->buflen)) == HT_OK)
	    me->transparent = YES;
	HTDNS_setServerVersion(net->dns, HTTP_09, HT_TCP_SINGLE);
	return status;
    } else {
	char *ptr = me->buffer+5;		       /* Skip the HTTP part */
	me->version = HTNextField(&ptr);

	/* here we want to find out when to use persistent connection */
	HTDNS_setServerVersion(net->dns, HTTP_10, HT_TCP_SINGLE);

	me->status = atoi(HTNextField(&ptr));
	me->reason = ptr;
	if ((ptr = strchr(me->reason, '\r')) != NULL)	  /* Strip \r and \n */
	    *ptr = '\0';
	else if ((ptr = strchr(me->reason, '\n')) != NULL)
	    *ptr = '\0';

	/* Set up the streams */
	if (me->status==200) {
	    HTStream *s;
	    if (req->output_format == WWW_SOURCE) {
		me->target = HTMIMEConvert(req, NULL, WWW_MIME,
					   req->output_format,
					   req->output_stream);
	    } else {
		me->target = HTStreamStack(WWW_MIME, req->output_format,
					   req->output_stream, req, NO);
	    
		/* howcome: test for return value from HTCacheWriter 12/1/95 */
		if (req->method==METHOD_GET && HTCache_isEnabled() &&
		    (s = HTCacheWriter(req, NULL, WWW_MIME, req->output_format,
				       req->output_stream))) {
		    me->target = HTTee(me->target, s);
		}
	      }
	} else if (req->debug_stream) {
	    me->target = HTStreamStack(WWW_MIME, req->debug_format,
				       req->debug_stream, req, NO);
	} else {
	    me->target = HTMIMEConvert(req, NULL, WWW_MIME, WWW_PRESENT,
				       HTBlackHole());
	}
	if (!me->target)
	    me->target = HTBlackHole();				/* What else */
    }
    HTTPNextState(me);					   /* Get next state */
    me->transparent = YES;
    return HT_OK;
}

/*
**	Searches for HTTP header line until buffer fills up or a CRLF or LF
**	is found
*/
PRIVATE int HTTPStatus_put_block ARGS3(HTStream *, me, CONST char*, b, int, l)
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
    if (me->target) {				    /* Is the stream set up? */
	if (l > 0)					   /* Anything left? */
	    return PUTBLOCK(b, l);
	return HT_OK;
    }
    return HT_WOULD_BLOCK;
}

PRIVATE int HTTPStatus_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    return HTTPStatus_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTTPStatus_put_character ARGS2(HTStream *, me, char, c)
{
    return HTTPStatus_put_block(me, &c, 1);
}

PRIVATE int HTTPStatus_flush ARGS1(HTStream *, me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTTPStatus_free ARGS1(HTStream *, me)
{
    int status = HT_OK;
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    free(me);
    return HT_OK;
}

PRIVATE int HTTPStatus_abort ARGS2(HTStream *, me, HTError, e)
{
    if (me->target)
	ABORT_TARGET;
    free(me);
    if (PROT_TRACE)
	fprintf(TDEST, "HTTPStatus.. ABORTING...\n");
    return HT_ERROR;
}

/*	HTTPStatus Stream
**	-----------------
*/
PRIVATE CONST HTStreamClass HTTPStatusClass =
{		
    "HTTPStatus",
    HTTPStatus_flush,
    HTTPStatus_free,
    HTTPStatus_abort,
    HTTPStatus_put_character,
    HTTPStatus_put_string,
    HTTPStatus_put_block
};

PRIVATE HTStream * HTTPStatus_new ARGS2(HTRequest *, request,
					http_info *, http)
{
    HTStream * me = (HTStream *) calloc(1, sizeof(HTStream));
    if (!me) outofmem(__FILE__, "HTTPStatus_new");
    me->isa = &HTTPStatusClass;
    me->request = request;
    me->http = http;
    me->state = EOL_BEGIN;
    return me;
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
    HTNet *net = request->net;		     /* Generic protocol information */
    http_info *http;			    /* Specific protocol information */
    
    /*
    ** Initiate a new http structure and bind to request structure
    ** This is actually state HTTP_BEGIN, but it can't be in the state
    ** machine as we need the structure first.
    */
    if (ops == FD_NONE) {
	if (PROT_TRACE) fprintf(TDEST, "HTTP........ Looking for `%s\'\n",
				HTAnchor_physical(request->anchor));
	if ((http = (http_info *) calloc(1, sizeof(http_info))) == NULL)
	    outofmem(__FILE__, "HTLoadHTTP");
	http->state = HTTP_BEGIN;
	http->next = HTTP_ERROR;
	net->context = http;
    } if (ops == FD_CLOSE) {				      /* Interrupted */
	if(HTRequest_isPostWeb(request)&&!HTRequest_isMainDestination(request))
	    HTTPCleanup(request, HT_IGNORE);
	else
	    HTTPCleanup(request, HT_INTERRUPTED);
	return HT_OK;
    } else
	http = (http_info *) net->context;		/* Get existing copy */
 
    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (http->state) {
	  case HTTP_BEGIN:
	    /*
	     ** Compose authorization information (this was moved here
	     ** from after the making of the connection so that the connection
	     ** wouldn't have to wait while prompting username and password
	     ** from the user).				-- AL 13.10.93
	     */
	    HTAA_composeAuth(request);
	    if (PROT_TRACE) {
		if (request->authorization)
		    fprintf(TDEST, "HTTP........ Sending Authorization: %s\n",
			    request->authorization);
		else
		    fprintf(TDEST,
			    "HTTP........ Not sending authorization (yet)\n");
	    }
	    http->state = HTTP_NEED_CONNECTION;
	    break;
	    
	  case HTTP_NEED_CONNECTION: 	    /* Now let's set up a connection */
	    status = HTDoConnect(net, HTAnchor_physical(request->anchor),
				 HTTP_PORT);
	    if (status == HT_OK) {

		/* Check the protocol class */
		char *class = HTDNS_serverClass(net->dns);
		if (class && strcasecomp(class, "http")) {
		    HTErrorAdd(request, ERR_FATAL, NO, HTERR_CLASS,
			       NULL, 0, "HTLoadHTTP");
		    http->state = HTTP_ERROR;
		    break;
		}
		HTDNS_setServerClass(net->dns, "http");

		if (PROT_TRACE)
		    fprintf(TDEST, "HTTP........ Connected, socket %d\n",
			    net->sockfd);

		/* Set up stream TO network */
		request->input_stream =
		    HTTPRequest_new(request, HTWriter_new(net, YES));

		/*
		** Set up concurrent read/write if this request isn't the
		** source for a PUT or POST. As source we don't start reading
		** before all destinations are ready. If destination then
		** register the input stream and get ready for read
		*/
		if (HTRequest_isPostWeb(request)) {
		    HTEvent_Register(net->sockfd, request, (SockOps) FD_READ,
				     HTLoadHTTP, net->priority);
		    HTRequest_linkDestination(request);
		}

		/* Set up stream FROM network and corresponding read buffer */
		net->isoc = HTInputSocket_new(net->sockfd);
		net->target = HTImProxy ?
		    request->output_stream : HTTPStatus_new(request, http);

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
		    HTNet *srcnet = request->source->net;
		    HTEvent_Register(srcnet->sockfd, request->source,
				     (SockOps) FD_READ,
				     HTLoadHTTP, srcnet->priority);
		    return HT_OK;
		}
		status = request->PostCallBack ?
		    request->PostCallBack(request, request->input_stream) :
			(*request->input_stream->isa->flush)(request->input_stream);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else
		    ops = FD_READ;	  /* Trick to ensure that we do READ */
	    } else if (ops == FD_READ) {
		status = HTSocketRead(request, net->target);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_LOADED)
		    http->state = http->next;	       /* Jump to next state */
		else if (status == HT_RELOAD)
		    http->state = HTTP_EXPIRED;
		else
		    http->state = HTTP_ERROR;
	    } else {
		http->state = HTTP_ERROR;
	    }
	    break;
	    
	  case HTTP_NOT_MODIFIED:
	    http->state = HTTP_ERROR;
	    break;

	  case HTTP_EXPIRED:
	    /* Dirty hack and fall through */
	    request->redirect = request->anchor->address;

	  case HTTP_REDIRECTION:
	    /* Clean up the other connections or just this one */
	    if (HTRequest_isPostWeb(request))
		HTRequest_killPostWeb(request);
	    else
		HTTPCleanup(request, HT_IGNORE);

	    /* If we found a new URL in the response */
	    if (request->redirect) {
		if (status == 301) {
		    HTErrorAdd(request, ERR_INFO, NO, HTERR_MOVED,
			       (void *) request->redirect,
			       (int) strlen(request->redirect), "HTLoadHTTP");
		} else if (status == 302) {
		    HTErrorAdd(request, ERR_INFO, NO, HTERR_FOUND,
			       (void *) request->redirect,
			       (int) strlen(request->redirect), "HTLoadHTTP");
		}

		/* If we haven't reached the limit for redirection */
		if (HTRequest_retry(request)) {
		    HTAnchor *anchor = HTAnchor_findAddress(request->redirect);
		    if (HTRequest_isPostWeb(request)) {
			HTRequest *dest = HTRequest_mainDestination(request);
			char *msg=(char*)malloc(strlen(request->redirect)+100);
			if (!msg) outofmem(__FILE__, "HTLoadHTTP");
			sprintf(msg, "\nLocation of %s has changed to %s for method %s, continue operation?",
				HTRequest_isDestination(request) ?
				"destination" : "source", request->redirect,
				HTMethod_name(request->method));
			if (HTConfirm(request, msg)) {
			    free(msg);

			    /* The new anchor inherits the Post Web */
			    HTAnchor_moveAllLinks((HTAnchor *) request->anchor,
						  anchor);
			    if (HTRequest_isSource(request))
				HTRequest_delete(request);
			    return HTCopyAnchor((HTAnchor *) anchor, dest) ?
				HT_OK : HT_ERROR;
			}
			free(msg);
			return HT_OK;
		    } if (request->PostCallBack) {
#if 0
			return HTUploadAnchor((HTAnchor*) anchor, request) ?
			    HT_OK : HT_ERROR;
#endif
		    } else {
			return HTLoadAnchor((HTAnchor *) anchor, request) ?
			    HT_OK : HT_ERROR;
		    }
		} else {
		    HTErrorAdd(request, ERR_FATAL, NO, HTERR_MAX_REDIRECT,
			       NULL, 0, "HTLoadHTTP");
		    if (HTRequest_isPostWeb(request)) {
			BOOL main = HTRequest_isMainDestination(request);
			if (HTRequest_isDestination(request)) {
			    HTLink *link =
				HTAnchor_findLink((HTAnchor *) request->source->anchor,
						  (HTAnchor *) request->anchor);
			    HTAnchor_setLinkResult(link, HT_LINK_ERROR);
			}
			HTNet_callback(request, main ? HT_ERROR : HT_IGNORE);
			HTRequest_removeDestination(request);
		    }
		    return HT_OK;
		}
	    } else {
		HTErrorAdd(request, ERR_FATAL, NO, HTERR_BAD_REPLY,
			   NULL, 0, "HTLoadHTTP");
		return HT_OK;
	    }
	    break;
	    
	  case HTTP_AA:
	    /* Clean up the other connections or just this one */
	    if (HTRequest_isPostWeb(request))
		HTRequest_killPostWeb(request);
	    else
		HTTPCleanup(request, HT_IGNORE);

	    /* Ask the user for a UserID and a passwd */
	    if (HTTPAuthentication(request) && HTAA_retryWithAuth(request)) {
		int ret;
		if (HTRequest_isPostWeb(request)) {
		    HTRequest *dest = HTRequest_mainDestination(request);
		    HTAnchor_appendMethods(request->anchor, request->method);
		    ret=HTCopyAnchor((HTAnchor*)request->source->anchor, dest);
		    return ret ? HT_OK : HT_ERROR;
		} else if (request->PostCallBack) {
#if 0
		    ret = HTUploadAnchor((HTAnchor*) request->anchor,request);
		    return ret ? HT_OK : HT_ERROR;
#endif
		} else {
		    ret = HTLoadAnchor((HTAnchor *) request->anchor, request);
		    return ret ? HT_OK : HT_ERROR;
		}
	    } else {				   /* If the guy said no :-( */
		HTErrorAdd(request, ERR_FATAL, NO, HTERR_UNAUTHORIZED,
			   NULL, 0, "HTLoadHTTP");
		if (HTRequest_isPostWeb(request)) {
		    BOOL main = HTRequest_isMainDestination(request);
		    if (HTRequest_isDestination(request)) {
			HTLink *link =
			    HTAnchor_findLink((HTAnchor *) request->source->anchor,
					      (HTAnchor *) request->anchor);
			HTAnchor_setLinkResult(link, HT_LINK_ERROR);
		    }
		    HTNet_callback(request, main ? HT_ERROR : HT_IGNORE);
		    HTRequest_removeDestination(request);
		}
		return HT_OK;
	    }
	    break;
	    
	  case HTTP_GOT_DATA:
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		if (HTRequest_isDestination(request)) {
		    HTLink *link =
			HTAnchor_findLink((HTAnchor *) request->source->anchor,
					  (HTAnchor *) request->anchor);
		    HTAnchor_setLinkResult(link, HT_LINK_OK);
		}
		HTRequest_removeDestination(request);
		HTTPCleanup(request, main ? HT_LOADED : HT_IGNORE);
	    } else
		HTTPCleanup(request, HT_LOADED);
	    return HT_OK;
	    break;
	    
	  case HTTP_NO_DATA:
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		if (HTRequest_isDestination(request)) {
		    HTLink *link =
			HTAnchor_findLink((HTAnchor *) request->source->anchor,
					  (HTAnchor *) request->anchor);
		    HTAnchor_setLinkResult(link, HT_LINK_OK);
		}
		HTRequest_removeDestination(request);
		HTTPCleanup(request, main ? HT_NO_DATA : HT_IGNORE);
	    } else
		HTTPCleanup(request, HT_NO_DATA);
	    return HT_OK;
	    break;
	    
	  case HTTP_RETRY:
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_killPostWeb(request);
		if (HTRequest_isDestination(request)) {
		    HTLink *link = 
			HTAnchor_findLink((HTAnchor*) request->source->anchor,
					  (HTAnchor*) request->anchor);
		    HTAnchor_setLinkResult(link, HT_LINK_ERROR);
		}
		HTRequest_removeDestination(request);
		HTTPCleanup(request, main ? HT_RETRY : HT_IGNORE);
	    } else
		HTTPCleanup(request, HT_RETRY);
	    return HT_OK;
	    break;

	  case HTTP_ERROR:
	    /* Clean up the other connections or just this one */
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_killPostWeb(request);
		if (HTRequest_isDestination(request)) {
		    HTLink *link =
			HTAnchor_findLink((HTAnchor *) request->source->anchor,
					  (HTAnchor *) request->anchor);
		    HTAnchor_setLinkResult(link, HT_LINK_ERROR);
		}
		HTRequest_removeDestination(request);
		HTTPCleanup(request, main ? HT_ERROR : HT_IGNORE);
	    } else
		HTTPCleanup(request, HT_ERROR);
	    return HT_OK;
	    break;
	}
    } /* End of while(1) */
}    


