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
#include "HTAlert.h"
#include "HTMIME.h"
#include "HTAccess.h"		/* HTRequest */
#include "HTAABrow.h"		/* Access Authorization */
#include "HTTee.h"		/* Tee off a cache stream */
#include "HTFWrite.h"		/* Write to cache file */
#include "HTWriter.h"
#include "HTError.h"
#include "HTChunk.h"
#include "HTGuess.h"
#include "HTThread.h"
#include "HTTPReq.h"
#include "HTTP.h"					       /* Implements */

/* Macros and other defines */
#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)
#define FREE_TARGET	(*me->target->isa->_free)(me->target)
#define ABORT_TARGET	(*me->target->isa->abort)(me->target, e)

/* Type definitions and global variables etc. local to this module */
/* This is the local definition of HTRequest->net_info */
typedef enum _HTTPState {
    HTTP_RETRY		= -4,
    HTTP_ERROR		= -3,
    HTTP_NO_DATA	= -2,
    HTTP_GOT_DATA	= -1,
    HTTP_BEGIN		= 0,
    HTTP_NEED_CONNECTION,
    HTTP_NEED_REQUEST,
    HTTP_REDIRECTION,
    HTTP_AA
} HTTPState;

typedef struct _http_info {
    SOCKFD		sockfd;				/* Socket descripter */
    SockA 		sock_addr;		/* SockA is defined in tcp.h */
    HTInputSocket *	isoc;				     /* Input buffer */
    SocAction		action;			/* Result of the select call */
    HTStream *		target;				    /* Target stream */
    int 		addressCount;	     /* Attempts if multi-homed host */
    time_t		connecttime;		 /* Used on multihomed hosts */
    struct _HTRequest *	request;	   /* Link back to request structure */

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

/*                                                                  HTTPCleanup
**
**      This function closes the connection and frees memory.
**
**      Returns 0 on OK, else -1
*/
PRIVATE int HTTPCleanup ARGS2(HTRequest *, req, BOOL, abort)
{
    http_info *http;
    int status = 0;
    if (!req || !req->net_info) {
	if (PROT_TRACE) fprintf(TDEST, "HTTPCleanup. Bad argument!\n");
	status = -1;
    } else {
	http = (http_info *) req->net_info;
	if (http->sockfd != INVSOC) {

	    if (http->target) {

		/* Free stream with data TO network */
		if (!HTRequest_isDestination(req)) {
		    if (abort)
			(*req->input_stream->isa->abort)(req->input_stream, NULL);
		    else
			(*req->input_stream->isa->_free)(req->input_stream);
		}

		/* Free stream with data FROM network */
		if (abort)
		    (*http->target->isa->abort)(http->target, NULL);
		else
		    (*http->target->isa->_free)(http->target);
	    }
	    if (PROT_TRACE)
		fprintf(TDEST,"HTTP........ Closing socket %d\n",http->sockfd);
	    if ((status = NETCLOSE(http->sockfd)) < 0)
		HTErrorSysAdd(http->request, ERR_FATAL, socerrno, NO,
			      "NETCLOSE");
	    HTThreadState(http->sockfd, THD_CLOSE);
	    http->sockfd = INVSOC;
	}
	HTThread_clear((HTNetInfo *) http);
	if (http->isoc)
	    HTInputSocket_free(http->isoc);
	free(http);
	req->net_info = NULL;
    }	
    return status;
}


PRIVATE BOOL HTTPAuthentication ARGS1(HTRequest *, request)
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
	HTAlert("This client doesn't support automatic redirection of type `Method'");
	me->http->next = HTTP_ERROR;
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
**		else:	 Use `error_stream' in HTRequest structure
**
**	Return: YES if buffer should be written out. NO otherwise
*/
PRIVATE int stream_pipe ARGS1(HTStream *, me)
{
    HTRequest *req = me->request;
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
	return status;
    } else {
	char *ptr = me->buffer+5;		       /* Skip the HTTP part */
	me->version = HTNextField(&ptr);
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
	} else {
	    me->target = HTMIMEConvert(req, NULL, WWW_MIME, req->error_format,
				       req->error_stream);
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

PUBLIC HTStream * HTTPStatus_new ARGS2(HTRequest *, request,
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
** On exit,
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_OK		Generic dummy: We are not finished!
**			HT_WOULD_BLOCK  if operation would have blocked
**			HT_LOADED	if return status 200 OK
**			HT_NO_DATA	if return status 204 No Response
**			HT_RETRY	if return status 503 Service Unavail.
*/
PUBLIC int HTLoadHTTP ARGS1 (HTRequest *, request)
{
    int status = HT_ERROR;
    char *url;				  /* Gets initialized on every entry */
    http_info *http;			    /* Specific protocol information */

    if (!request || !request->anchor) {
        if (PROT_TRACE) fprintf(TDEST, "HTLoadHTTP.. Bad argument\n");
        return HT_ERROR;
    }
    url = HTAnchor_physical(request->anchor);
    
    /* Only do the setup first time through. This is actually state HTTP_BEGIN
       but it can't be in the state machine as we need the structure first */
    if (!request->net_info) {
	/*
	** Initiate a new http structure and bind to request structure
	** This is actually state HTTP_BEGIN, but it can't be in the state
	** machine as we need the structure first.
	*/
	if (PROT_TRACE) fprintf(TDEST, "HTTP........ Looking for `%s\'\n",url);
	if ((http = (http_info *) calloc(1, sizeof(http_info))) == NULL)
	    outofmem(__FILE__, "HTLoadHTTP");
	http->sockfd = INVSOC;			    /* Invalid socket number */
	http->request = request;
	http->state = HTTP_BEGIN;
	http->next = HTTP_ERROR;
	request->net_info = (HTNetInfo *) http;
	HTThread_new((HTNetInfo *) http);
    } else {
	http = (http_info *) request->net_info;		/* Get existing copy */
	if (HTThreadIntr(http->sockfd)) {
	    HTTPCleanup(request, YES);
	    return HTRequest_isMainDestination(request) ? HT_ERROR : HT_OK;
	}
    }
 
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
	    status = HTDoConnect((HTNetInfo *) http, url, TCP_PORT,
				 NULL, NO);
	    if (status == HT_OK) {
		if (PROT_TRACE)
		    fprintf(TDEST, "HTTP........ Connected, socket %d\n",
			    http->sockfd);

		/* Set up stream TO network */
		request->input_stream =
		    HTTPRequest_new(request, HTWriter_new(http->sockfd, YES));

		/*
		** Set up concurrent read/write if this request isn't the
		** source for a PUT or POST. As source we don't start reading
		** before all destinations are ready. If destination then
		** register the input stream and get ready for read
		*/
		if (HTRequest_isPostWeb(request)) {
		    HTThreadState(http->sockfd, THD_SET_READ);
		    HTRequest_linkDestination(request);
		}

		/* Set up stream FROM network and corresponding read buffer */
		http->isoc = HTInputSocket_new(http->sockfd);
		http->target = HTImProxy ?
		    request->output_stream : HTTPStatus_new(request, http);

		http->state = HTTP_NEED_REQUEST;
	    } else if (status == HT_WOULD_BLOCK)
		return status;
	    else
		http->state = HTTP_ERROR;	       /* Error or interrupt */
	    break;

	    /* As we can do simultanous read and write this is now one state */
	  case HTTP_NEED_REQUEST:
	    if (http->action == SOC_WRITE) {
		if (HTRequest_isDestination(request)) {
		    HTThreadStateByRequest(request->source, THD_SET_READ);
		    return HT_WOULD_BLOCK;
		}
		status = request->PostCallBack ?
		    request->PostCallBack(request, request->input_stream) :
			(*request->input_stream->isa->flush)(request->input_stream);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_INTERRUPTED)
		    http->state = HTTP_ERROR;
		else {
		    http->action = SOC_READ;
		}
	    } else if (http->action == SOC_READ) {
		status = HTSocketRead(request, http->target);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_INTERRUPTED)
		    http->state = HTTP_ERROR;
		else {
		    http->state = http->next;	       /* Jump to next state */
		}
	    } else
		http->state = HTTP_ERROR;
	    break;
	    
	  case HTTP_REDIRECTION:
	    /* Clean up the other connections or just this one */
	    if (HTRequest_isPostWeb(request))
		HTRequest_killPostWeb(request);
	    else
		HTTPCleanup(request, NO);

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
		if (++request->redirections < HTMaxRedirections) {
		    HTAnchor *anchor = HTAnchor_findAddress(request->redirect);
		    if (HTRequest_isPostWeb(request)) {
			HTRequest *dest = HTRequest_mainDestination(request);
			char *msg=(char*)malloc(strlen(request->redirect)+100);
			if (!msg) outofmem(__FILE__, "HTLoadHTTP");
			sprintf(msg, "\nLocation of %s has changed to %s for method %s, continue operation?",
				HTRequest_isDestination(request) ?
				"destination" : "source", request->redirect,
				HTMethod_name(request->method));
			if (HTConfirm(msg)) {
			    free(msg);

			    /* The new anchor inherits the Post Web */
			    HTAnchor_moveLinks((HTAnchor *) request->anchor,
					       anchor);
			    if (HTRequest_isSource(request))
				HTRequest_delete(request);
			    return HTCopyAnchor((HTAnchor *) anchor, dest);
			}
			free(msg);
			return HT_ERROR;
		    } if (request->PostCallBack) {
#if 0
			return HTUploadAnchor((HTAnchor*) anchor, request);
#endif
		    } else {
			return HTLoadAnchor((HTAnchor *) anchor, request);
		    }
		} else {
		    HTErrorAdd(request, ERR_FATAL, NO, HTERR_MAX_REDIRECT,
			       NULL, 0, "HTLoadHTTP");
		    if (HTRequest_isPostWeb(request)) {
			BOOL main = HTRequest_isMainDestination(request);
			HTRequest_removeDestination(request);
			return main ? HT_ERROR : HT_OK;
		    }
		    return HT_ERROR;
		}
	    } else {
		HTErrorAdd(request, ERR_FATAL, NO, HTERR_BAD_REPLY,
			   NULL, 0, "HTLoadHTTP");
		return HT_ERROR;
	    }
	    break;
	    
	  case HTTP_AA:
	    /* Clean up the other connections or just this one */
	    if (HTRequest_isPostWeb(request))
		HTRequest_killPostWeb(request);
	    else
		HTTPCleanup(request, NO);

	    /* Ask the user for a UserID and a passwd */
	    if (HTTPAuthentication(request) && HTAA_retryWithAuth(request)) {
		if (HTRequest_isPostWeb(request)) {
		    HTRequest *dest = HTRequest_mainDestination(request);
		    HTAnchor_appendMethods(request->anchor, request->method);
		    return HTCopyAnchor((HTAnchor *) request->source->anchor,
					dest);
		} else if (request->PostCallBack) {
#if 0
		    return HTUploadAnchor((HTAnchor*) request->anchor,request);
#endif
		} else {
		    return HTLoadAnchor((HTAnchor *) request->anchor, request);
		}
	    } else {				   /* If the guy said no :-( */
		char *unescaped = NULL;
		StrAllocCopy(unescaped, url);
		HTUnEscape(unescaped);
		HTErrorAdd(request, ERR_FATAL, NO, HTERR_UNAUTHORIZED,
			   (void *) unescaped,
			   (int) strlen(unescaped), "HTLoadHTTP");
		free(unescaped);
		if (HTRequest_isPostWeb(request)) {
		    BOOL main = HTRequest_isMainDestination(request);
		    HTRequest_removeDestination(request);
		    return main ? HT_ERROR : HT_OK;
		}
		return HT_ERROR;
	    }
	    break;
	    
	  case HTTP_GOT_DATA:
	    HTTPCleanup(request, NO);
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_removeDestination(request);
		return main ? HT_LOADED : HT_OK;
	    }
	    return HT_LOADED;
	    break;
	    
	  case HTTP_NO_DATA:
	    HTTPCleanup(request, NO);
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_removeDestination(request);
		return main ? HT_NO_DATA : HT_OK;
	    }
	    return HT_NO_DATA;
	    break;
	    
	  case HTTP_RETRY:	     /* Treat this as an error state for now */
	    if (HTRequest_isPostWeb(request))
		HTRequest_killPostWeb(request);
	    else
		HTTPCleanup(request, YES);
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_removeDestination(request);
		return main ? HT_RETRY : HT_OK;
	    }
	    return HT_RETRY;
	    break;

	  case HTTP_ERROR:
	    /* Clean up the other connections or just this one */
	    if (HTRequest_isPostWeb(request)) {
		if (http->sockfd == INVSOC)
		    HTTPCleanup(request, YES);         /* If no valid socket */
		HTRequest_killPostWeb(request);
	    } else
		HTTPCleanup(request, YES);
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_removeDestination(request);
		return main ? HT_ERROR : HT_OK;
	    }
	    return HT_ERROR;
	    break;
	}
    } /* End of while(1) */
}    

/* Protocol descriptor */

GLOBALDEF PUBLIC HTProtocol HTTP = {
    "http", SOC_NON_BLOCK, HTLoadHTTP, NULL, NULL
};


