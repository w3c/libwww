/*									 HTTP.c
**	MULTITHREADED IMPLEMENTATION OF HTTP CLIENT
**
**	(c) COPYRIGHT CERN 1994.
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
#include "HTAlert.h"
#include "HTMIME.h"
#include "HTAccess.h"		/* HTRequest */
#include "HTAABrow.h"		/* Access Authorization */
#include "HTTee.h"		/* Tee off a cache stream */
#include "HTFWrite.h"		/* Write to cache file */
#include "HTError.h"
#include "HTChunk.h"
#include "HTGuess.h"
#include "HTThread.h"
#include "HTTP.h"					       /* Implements */

/* Macros and other defines */
#define HTTP_VERSION	"HTTP/1.0"
#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)
#define FREE_TARGET	(*me->target->isa->_free)(me->target)
#define ABORT_TARGET	(*me->target->isa->abort)(me->target, e)

/* Globals */
extern char * HTAppName;		  /* Application name: please supply */
extern char * HTAppVersion;	       /* Application version: please supply */
extern BOOL using_proxy;		    /* are we using a proxy gateway? */

PUBLIC int  HTMaxRedirections = 10;	       /* Max number of redirections */
PUBLIC BOOL HTEnableFrom = NO;			      /* Enable From header? */
PUBLIC char * HTProxyHeaders = NULL;		    /* Headers to pass as-is */

/* Type definitions and global variables etc. local to this module */
/* This is the local definition of HTRequest->net_info */
typedef enum _HTTPState {
    HTTP_ERROR		= -3,
    HTTP_NO_DATA	= -2,
    HTTP_GOT_DATA	= -1,
    HTTP_BEGIN		= 0,
    HTTP_NEED_CONNECTION,
    HTTP_NEED_REQUEST,
    HTTP_SENT_REQUEST,
    HTTP_NEED_BODY,
    HTTP_REDIRECTION,
    HTTP_AA
} HTTPState;

typedef struct _http_info {
    SOCKFD		sockfd;				/* Socket descripter */
    SockA 		sock_addr;		/* SockA is defined in tcp.h */
    HTInputSocket *	isoc;				     /* Input buffer */
    HTStream *		target;			            /* Output stream */
    HTChunk *		transmit;			  /* Line to be send */
    int 		addressCount;	     /* Attempts if multi-homed host */
    time_t		connecttime;		 /* Used on multihomed hosts */
    struct _HTRequest *	request;	   /* Link back to request structure */

    HTTPState		state;			  /* State of the connection */
} http_info;

#define MAX_STATUS_LEN		150	   /* Max number of chars to look at */

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    http_info *			http;
    int				cnt;
    HTSocketEOL			state;
    BOOL			transparent;
    double			version;		 /* @@@ DOESN'T WORK */
    int				status;
    char 			buffer[MAX_STATUS_LEN+1];
    char *			bufptr;
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
PRIVATE int HTTPCleanup ARGS1(HTRequest *, request)
{
    http_info *http;
    int status = 0;
    if (!request || !request->net_info) {
	if (PROT_TRACE) fprintf(TDEST, "HTTPCleanup. Bad argument!\n");
	status = -1;
    } else {
	http = (http_info *) request->net_info;
	if (http->sockfd != INVSOC) {
	    if (PROT_TRACE)
		fprintf(TDEST,"HTTP........ Closing socket %d\n",http->sockfd);
	    if ((status = NETCLOSE(http->sockfd)) < 0)
		HTErrorSysAdd(http->request, ERR_FATAL, socerrno, NO,
			      "NETCLOSE");
	    HTThreadState(http->sockfd, THD_CLOSE);
	    HTThread_clear((HTNetInfo *) http);
	    http->sockfd = INVSOC;
	}
	if (http->isoc)
	    HTInputSocket_free(http->isoc);
	if (http->transmit)
	    HTChunkFree(http->transmit);
    }	
    free(http);
    request->net_info = NULL;
    return status;
}


/*                                                              HTTPSendRequest
**
**      This function composes and sends a request to the connected server
**      specified.
**
**	Returns		<0		Error has occured or interrupted
**			HT_WOULD_BLOCK  if operation would have blocked
**			HT_INTERRUPTED  if interrupted
**
**      Note: The function does NEVER close the connection
*/
PRIVATE int HTTPSendRequest ARGS3(HTRequest *, request,
				  http_info *, http, char *, url)
{
    int status = 0;

    /* If first time through then generate HTTP request */
    if (!http->transmit) {
	HTChunk *command = HTChunkCreate(2048);
	http->transmit = command;
	if (request->method != METHOD_INVALID) {
	    HTChunkPuts(command, HTMethod_name(request->method));
	    HTChunkPutc(command, ' ');
	}
	else
	    HTChunkPuts(command, "GET ");
	
	/* if we are using a proxy gateway don't copy in the first slash
	 ** of say: /gopher://a;lkdjfl;ajdf;lkj/;aldk/adflj
	 ** so that just gohper://.... is sent. */
	{
	    char *p1 = HTParse(url, "", PARSE_PATH|PARSE_PUNCTUATION);
	    if (using_proxy)
		HTChunkPuts(command, p1+1);
	    else
		HTChunkPuts(command, p1);
	    free(p1);
	}
	HTChunkPutc(command, ' ');
	HTChunkPuts(command, HTTP_VERSION);
	HTChunkPutc(command, CR);		     /* CR LF, as in rfc 977 */
	HTChunkPutc(command, LF);
	
	if (HTImProxy && HTProxyHeaders) {
	    HTChunkPuts(command, HTProxyHeaders);
	} else {
	    char line[256];    /*@@@@ */
	    
	    /* If no conversion list, then put it up, but leave initialization
	       to the client */
	    if (!HTConversions)
		HTConversions = HTList_new();
	    
	    /* Run through both lists and generate `accept' lines */
	    {
		int i;
		HTList *conversions[2];
		conversions[0] = HTConversions;
		conversions[1] = request->conversions;
		
		for (i=0; i<2; i++) {
		    HTList *cur = conversions[i];
		    HTPresentation *pres;
		    while ((pres =(HTPresentation *) HTList_nextObject(cur))) {
			if (pres->rep_out == WWW_PRESENT) {
			    if (pres->quality != 1.0) {
				sprintf(line, "Accept: %s; q=%.3f%c%c",
					HTAtom_name(pres->rep),
					pres->quality, CR, LF);
			    } else {
				sprintf(line, "Accept: %s%c%c",
					HTAtom_name(pres->rep), CR, LF);
			    }
			    HTChunkPuts(command, line);
			}
		    }
		}
	    }
	    
	    /* Put out referer field if any parent */
	    if (request->parentAnchor) {
		char *me = HTAnchor_address((HTAnchor *) request->anchor);
		char *parent = HTAnchor_address((HTAnchor *)
						request->parentAnchor);
		char *relative = HTParse(parent, me,
					 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
		if (relative && *relative) {
		    sprintf(line, "Referer: %s%c%c", parent, CR, LF);
		    HTChunkPuts(command, line);
		}
		free(me);
		free(parent);
		free(relative);
	    }
	    
	    /* Put out from field if enabled by client */
	    if (HTEnableFrom) {
		CONST char *mailaddress = HTGetMailAddress();
		if (mailaddress != NULL) {
		    sprintf(line, "From: %s%c%c", mailaddress, CR, LF);
		    HTChunkPuts(command, line);
		}
	    }
	    
	    /* Put out user-agent */
	    sprintf(line, "User-Agent: %s/%s  libwww/%s%c%c",
		    HTAppName ? HTAppName : "unknown",
		    HTAppVersion ? HTAppVersion : "0.0",
		    HTLibraryVersion, CR, LF);
	    HTChunkPuts(command, line);
	    
	    /* Put out authorization */
	    if (request->authorization != NULL) {
		HTChunkPuts(command, "Authorization: ");
		HTChunkPuts(command, request->authorization);
		HTChunkPutc(command, CR);
		HTChunkPutc(command, LF);
	    }
	}
	HTChunkPutc(command, CR);		   /* Blank line means "end" */
	HTChunkPutc(command, LF);
	HTChunkTerminate(command);
	if (PROT_TRACE) fprintf(TDEST, "HTTP Tx..... %s", command->data);
	
	/* Translate into ASCII if necessary */
#ifdef NOT_ASCII
	{
	    char * p;
	    for(p = command->data; *p; p++) {
		*p = TOASCII(*p);
	    }
	}
#endif
    }

    /* Now, we are ready for sending the request */
    status = NETWRITE(http->sockfd, http->transmit->data,
		      http->transmit->size-1);
    if (status < 0) {
#ifdef EAGAIN
	if (socerrno == EAGAIN || socerrno == EWOULDBLOCK) 
#else
	if (socerrno == EWOULDBLOCK)
#endif
	{
	    if (PROT_TRACE)
		fprintf(TDEST, "HTTP Tx..... Write operation would block\n");
	    HTThreadState(http->sockfd, THD_SET_WRITE);
	    return HT_WOULD_BLOCK;
	} else {	    			 /* A real error has occured */
	    char *unescaped = NULL;
	    HTErrorSysAdd(request, ERR_FATAL, socerrno, NO, "NETWRITE");
	    StrAllocCopy(unescaped, url);
	    HTUnEscape(unescaped);
	    HTErrorAdd(request, ERR_FATAL, NO, HTERR_INTERNAL,
		       (void *) unescaped, (int) strlen(unescaped),
		       "HTTPSendRequest");
	    free(unescaped);
	    return -1;
	}
    }
    HTThreadState(http->sockfd, THD_CLR_WRITE);			/* Write OK */
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
PRIVATE void HTTPResponse ARGS1(HTStream *, me)
{
    switch (me->status) {

      case 0:						     /* 0.9 response */
      case 200:
      case 201:
      case 202:
      case 203:
      case 205:
      case 206:
	break;

      case 204:						      /* No Response */
	me->http->state = HTTP_NO_DATA;
	break;

      case 301:						   	    /* Moved */
      case 302:							    /* Found */
	me->http->state = HTTP_REDIRECTION;
	break;
	
      case 303:							   /* Method */
	HTAlert("This client doesn't support automatic redirection of type `Method'");
	me->http->state = HTTP_ERROR;
	break;
	
      case 400:						      /* Bad Request */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_BAD_REQUEST,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;

      case 401:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_UNAUTHORIZED,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_AA;
	break;
	
      case 402:						 /* Payment required */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_PAYMENT_REQUIRED,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;
	
      case 403:							/* Forbidden */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_FORBIDDEN,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;
	
      case 404:							/* Not Found */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_NOT_FOUND,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;
	
      case 405:						      /* Not Allowed */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_NOT_ALLOWED,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;

      case 406:						  /* None Acceptable */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_NONE_ACCEPTABLE,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;

      case 407:			       	    /* Proxy Authentication Required */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_PROXY,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;

      case 408:						  /* Request Timeout */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_TIMEOUT,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;

      case 500:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_INTERNAL,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;
	
      case 501:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_NOT_IMPLEMENTED,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;

      case 502:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_BAD_GATE,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;

      case 503:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_DOWN,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;

      case 504:
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_GATE_TIMEOUT,
		   NULL, 0, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;

      default:						       /* bad number */
	HTErrorAdd(me->request, ERR_FATAL, NO, HTERR_BAD_REPLY,
		   (void *) me->buffer, me->cnt, "HTLoadHTTP");
	me->http->state = HTTP_ERROR;
	break;
    }
}

/* ------------------------------------------------------------------------- */
/* 			    HTTP Status Line Stream			     */
/* ------------------------------------------------------------------------- */

/*
**	Analyse the string we have read. If it is a HTTP 1.0 or higher
**	then create a MIME-stream, else create a Guess stream to find out
**	what the 0.9 server is sending. We need to copy the buffer as we don't
**	know if we can modify the contents or not.
**
**	Stream handling is a function of the status code returned from the 
**	server:
**		200:	 Use `output_stream' in HTRequest structure
**		else:	 Use `output_flush' in HTRequest structure
*/
PRIVATE void flush ARGS1(HTStream *, me)
{
    HTRequest *req = me->request;
    me->transparent = YES;				/* Only do this once */
    if (me->state == EOL_FLF) {
	if (strncasecomp(me->buffer, "http/", 5) ||
	    sscanf(me->buffer+5, "%lf %d", &me->version, &me->status) < 2) {
	    HTErrorAdd(req, ERR_INFO, NO, HTERR_HTTP09,
		       (void *) me->buffer, me->cnt, "HTTPStatusStream");
	    me->target = HTGuess_new(req, NULL, WWW_UNKNOWN,
				     req->output_format, req->output_stream);
	    PUTBLOCK(me->buffer, me->cnt);
	} else {
	    if (req->output_format == WWW_SOURCE) {
		me->target = HTMIMEConvert(req, NULL, WWW_MIME,
					   req->output_format,
					   req->output_stream);
	    } else if (me->status==200 && req->method==METHOD_GET) {
		HTStream *s;

		me->target = HTStreamStack(WWW_MIME,req->output_format,
					   req->output_stream, req, NO);

		/* Cache HTTP 1.0 responses */
		/* howcome added test for return value from HTCacheWriter 12/1/95 */

		if (HTCacheDir && (s = HTCacheWriter(req, NULL, WWW_MIME,
							req->output_format,
							req->output_stream)))
		    {
			me->target = HTTee(me->target, s);
		    }
	    } else {
		me->target = HTMIMEConvert(req, NULL, WWW_MIME,
					   WWW_SOURCE, req->output_flush ?
					   req->output_flush : HTBlackHole());
	    }
	    if (!me->target)
		me->target = HTBlackHole();			/* What else */
	}
    } else {
	me->target = HTGuess_new(req, NULL, WWW_UNKNOWN, req->output_format,
				 req->output_stream);
	PUTBLOCK(me->buffer, me->cnt);
    }
}

PRIVATE void HTTPStatus_put_character ARGS2(HTStream *, me, char, c)
{
    if (me->transparent)
	PUTC(c);
    else {
	if (me->state == EOL_FCR) {
	    if (c == LF) {
		me->state = EOL_FLF;			       /* Line found */
		flush(me);
	    } else {
		me->state = EOL_BEGIN;
		me->cnt += 2;
		*me->bufptr++ = CR;		      /* Need to put it back */
		*me->bufptr++ = c;
	    }
	} else if (c == CR) {
	    me->state = EOL_FCR;
	} else if (c == LF) {
	    me->state = EOL_FLF;			       /* Line found */
	    me->cnt++;
	    *me->bufptr++ = LF;
	    flush(me);
	} else {
	    me->cnt++;
	    *me->bufptr++ = c;
	    if (me->cnt >= MAX_STATUS_LEN)
		flush(me);
	}
    }
}

PRIVATE void HTTPStatus_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    while (!me->transparent && *s)
	HTTPStatus_put_character(me, *s++);
    if (*s) PUTS(s);
}

PRIVATE void HTTPStatus_put_block ARGS3(HTStream *, me, CONST char*, b, int, l)
{
    while (!me->transparent && l-- > 0)
	HTTPStatus_put_character(me, *b++);
    if (l > 0) PUTBLOCK(b, l);
}

PRIVATE int HTTPStatus_free ARGS1(HTStream *, me)
{
    int status = me->status;
    HTTPResponse(me);					   /* Get next state */
    if (!me->transparent)
	flush(me);
    if (me->target)
	FREE_TARGET;
    free(me);
    return status;	     /* Return the HTTP status value - 0 if HTTP 0.9 */
}

PRIVATE int HTTPStatus_abort ARGS2(HTStream *, me, HTError, e)
{
    if (me->target)
	ABORT_TARGET;
    free(me);
    if (PROT_TRACE)
	fprintf(TDEST, "HTTPStatus.. ABORTING LOAD...\n");
    return EOF;
}

/*	HTTPStatus Stream
**	-----------------
*/
PRIVATE CONST HTStreamClass HTTPStatusClass =
{		
    "HTTPStatus",
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
    me->bufptr = me->buffer;
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
**	returns		<0		Error has occured or interrupted
**			HT_WOULD_BLOCK  if operation would have blocked
**			HT_LOADED	if return status 200 OK
**			HT_NO_DATA	if return status 204 No Response
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
	if (PROT_TRACE) fprintf(TDEST, "HTTP........ Looking for `%s\'\n", url);
	if ((http = (http_info *) calloc(1, sizeof(http_info))) == NULL)
	    outofmem(__FILE__, "HTLoadHTTP");
	http->sockfd = INVSOC;			    /* Invalid socket number */
	http->request = request;
	http->state = HTTP_BEGIN;
	request->net_info = (HTNetInfo *) http;
	HTThread_new((HTNetInfo *) http);
    } else
	http = (http_info *) request->net_info;		/* Get existing copy */
 
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
	    if (!status) {
		if (PROT_TRACE)
		    fprintf(TDEST, "HTTP........ Connected, socket %d\n",
			    http->sockfd);
		http->isoc = HTInputSocket_new(http->sockfd);
		http->state = HTTP_NEED_REQUEST;
	    } else if (status == HT_WOULD_BLOCK)
		return status;
	    else
		http->state = HTTP_ERROR;	       /* Error or interrupt */
	    break;

	  case HTTP_NEED_REQUEST:	  /* Compose the request and send it */
	    if ((status = HTTPSendRequest(request, http, url)) < 0) {
		if (status == HT_WOULD_BLOCK)
		    return status;
		else
		    http->state = HTTP_ERROR;
	    } else {
		http->state = HTTP_SENT_REQUEST;	    
	    }
	    break;

	  case HTTP_SENT_REQUEST:			    /* Put up stream */
	    http->target = HTImProxy ?
		request->output_stream : HTTPStatus_new(request, http);
	    http->state = HTTP_NEED_BODY;
	    break;

	  case HTTP_NEED_BODY:				/* Read the response */
	    status = HTInputSocket_read(http->isoc, http->target);
	    if (status == HT_WOULD_BLOCK)
		return HT_WOULD_BLOCK;
	    else if (status == HT_INTERRUPTED) {
		(*http->target->isa->abort)(http->target, NULL);
		http->state = HTTP_ERROR;
	    } else if (status == HT_LOADED) {
		(*http->target->isa->_free)(http->target);
		if (http->state == HTTP_NEED_BODY)
		    http->state = HTTP_GOT_DATA;
	    } else {
		(*http->target->isa->_free)(http->target);
		http->state = HTTP_ERROR;
	    }
	    break;

          case HTTP_REDIRECTION:
	    if (request->redirect) {
		HTAnchor *anchor;
		if (status == 301) {
		    HTErrorAdd(request, ERR_INFO, NO, HTERR_MOVED,
			       (void *) request->redirect,
			       (int) strlen(request->redirect), "HTLoadHTTP");
		} else if (status == 302) {
		    HTErrorAdd(request, ERR_INFO, NO, HTERR_FOUND,
			       (void *) request->redirect,
			       (int) strlen(request->redirect), "HTLoadHTTP");
		}
		anchor = HTAnchor_findAddress(request->redirect);
		if (++request->redirections < HTMaxRedirections) {
		    HTTPCleanup(request);
		    return HTLoadAnchorRecursive((HTAnchor *) anchor, request);
		} else {
		    HTErrorAdd(request, ERR_FATAL, NO, HTERR_MAX_REDIRECT,
			       NULL, 0, "HTLoadHTTP");
		    http->state = HTTP_ERROR;
		}
	    } else {
		HTErrorAdd(request, ERR_FATAL, NO, HTERR_BAD_REPLY,
			   NULL, 0, "HTLoadHTTP");
		http->state = HTTP_ERROR;
	    }
	    break;

	  case HTTP_AA:
	    if (HTTPAuthentication(request) == YES &&
		HTAA_retryWithAuth(request) == YES) {
		HTTPCleanup(request);
		return HTLoadAnchor((HTAnchor *) request->anchor, request);
	    } else {
		char *unescaped = NULL;
		StrAllocCopy(unescaped, url);
		HTUnEscape(unescaped);
		HTErrorAdd(request, ERR_FATAL, NO, HTERR_UNAUTHORIZED,
			   (void *) unescaped,
			   (int) strlen(unescaped), "HTLoadHTTP");
		free(unescaped);
		http->state = HT_ERROR;
	    }
	    break;

	  case HTTP_GOT_DATA:
	    HTTPCleanup(request);
	    return HT_LOADED;
	    break;
	    
	  case HTTP_NO_DATA:
	    HTTPCleanup(request);
	    return HT_NO_DATA;
	    break;

	  case HTTP_ERROR:
	    HTTPCleanup(request);
	    return HT_ERROR;
	    break;
	}
    } /* End of while(1) */
}    

/* Protocol descriptor */

GLOBALDEF PUBLIC HTProtocol HTTP = {
    "http", SOC_NON_BLOCK, HTLoadHTTP, NULL, NULL
};

