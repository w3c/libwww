/*								    HTTPServ.c
**	HTTP SERVER MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module implments the HTTP protocol as a state machine
**
** History:
**	Dec 95 HFN	Written with Christmas in my eyes
**
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTHeader.h"
#include "HTMIMERq.h"
#include "HTNetMan.h"
#include "HTTPUtil.h"
#include "HTTPRes.h"
#include "HTTPServ.h"					       /* Implements */

/* Macros and other defines */
#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

/* Final states have negative value */
typedef enum _HTTPState {
    HTTPS_ERROR		= -2,
    HTTPS_OK		= -1,
    HTTPS_BEGIN		= 0,
    HTTPS_NEED_REQUEST,
    HTTPS_LOAD_CLIENT
} HTTPState;

/* This is the context object for the this module */
typedef struct _https_info {
    HTRequest *	server;				       /* The server request */
    HTList *	clients;		          /* List of client requests */
    HTTPState	state;			  /* Current State of the connection */
    HTNet *	net;
} https_info;

/* The HTTP Receive Stream */
struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    https_info *		http;
    HTEOLState			state;
    HTChunk *			buffer;
    BOOL			transparent;
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

/* ------------------------------------------------------------------------- */

/*	ServerCleanup
**	-------------
**      This function cleans up after the request
**      Returns YES on OK, else NO
*/
PRIVATE int ServerCleanup (HTRequest * req, HTNet * net, int status)
{
    https_info * http = (https_info *) HTNet_context(net);
    HTStream * input = HTRequest_inputStream(req);
    HTChannel * channel = HTNet_channel(net);

    /* Free stream with data TO network */
    if (input) {
	if (status == HT_INTERRUPTED)
	    (*input->isa->abort)(input, NULL);
	else
	    (*input->isa->_free)(input);
	HTRequest_setInputStream(req, NULL);
    }

    /* Kill all remaining requests */
    if (http->clients) {
	HTList * cur = http->clients;
	HTRequest * pres;
	while ((pres = HTList_nextObject(cur)) != NULL)
	    HTRequest_kill(pres);
	HTList_delete(http->clients);
    }

    /*
    **  Remove the net object and our own context structure for http.
    **	Also unregister all pending requests and close the connection
    */
    HTChannel_setSemaphore(channel, 0);
    HTNet_delete(net, HT_IGNORE);

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
*/
PRIVATE int MakeReplyPipe (HTStream * me, HTRequest * client)
{
    char * response_line = NULL;

    /* Generate the Response line */
    {
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) {
	    HTAlertPar * reply = HTAlert_newReply();
	    if ((*cbf)(client, HT_A_MESSAGE, HT_MSG_NULL, NULL,
		       HTRequest_error(client), reply))
		response_line = HTAlert_replyMessage(reply);
	    HTAlert_deleteReply(reply);
	}

	if (response_line) {
	    PUTS(response_line);
	    HT_FREE(response_line);
	} else {	
	    PUTS(HTTP_VERSION);
	    PUTS(" 500 Internal");
	    PUTC(CR);
	    PUTC(LF);
	}
    }

    /*
    ** We now have to create the rest of the response stream. We see whether
    ** there is a data object or not by looking at the Content Type of the
    ** client anchor.
    */
    {
	HTParentAnchor * anchor = HTRequest_anchor(client);
	HTFormat format = HTAnchor_format(anchor);
	me->target = (format == WWW_UNKNOWN) ?
	    HTTPResponse_new(client, me->target, YES, HTTP_11) :
	    HTMIMERequest_new(client,
	        HTTPResponse_new(client,me->target, NO, HTTP_11), YES);
    }
    return HT_OK;
}

PRIVATE int HTTPReply_put_block (HTStream * me, const char * b, int l)
{
    if (me->transparent)
	return b ? PUTBLOCK(b, l) : HT_OK;
    else {
	MakeReplyPipe(me, me->request);
	me->transparent = YES;
	return b ? PUTBLOCK(b, l) : HT_OK;
    }
}

PRIVATE int HTTPReply_put_string (HTStream * me, const char * s)
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
    int status = HTTPReply_put_block(me, NULL, 0);
    HTTRACE(STREAM_TRACE, "HTTPReply... Freeing server stream\n");
    return status==HT_OK ? (*me->target->isa->_free)(me->target) : status;
}

PRIVATE int HTTPReply_abort (HTStream * me, HTList * e)
{
    HTTRACE(STREAM_TRACE, "HTTPReply... ABORTING\n");
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HT_FREE(me);
    return HT_ERROR;
}

/*	HTTPReply Stream
**	-----------------
*/
PRIVATE const HTStreamClass HTTPReplyClass =
{		
    "HTTPReply",
    HTTPReply_flush,
    HTTPReply_free,
    HTTPReply_abort,
    HTTPReply_put_character,
    HTTPReply_put_string,
    HTTPReply_put_block
};

PRIVATE HTStream * HTTPReply_new (HTRequest * request, https_info * http,
				  HTStream * target)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTTPReply_new");
    me->isa = &HTTPReplyClass;
    me->request = request;
    me->http = http;
    me->target = target;
    HTTRACE(STREAM_TRACE, "HTTP Reply.. Stream %p created\n" _ me);
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
    HTRequest * client = HTList_firstObject(me->http->clients);
    char * line = HTChunk_data(me->buffer);
    char * method_str = HTNextField(&line);
    char * request_uri = HTNextField(&line);
    char * version_str = HTNextField(&line);
    HTMethod method;

    /* Check if method is allowed */
    if (!method_str || (method = HTMethod_enum(method_str))==METHOD_INVALID) {
	HTRequest_addError(client, ERR_FATAL, NO, HTERR_NOT_ALLOWED,
			   NULL, 0, "ParseRequest");
	return HT_ERROR;
    }
    HTRequest_setMethod(client, method);

    /* Find an anchor for the request URI */
    if (request_uri) {
	char * uri = HTParse(request_uri, "file:", PARSE_ALL);
	HTRequest_setAnchor(client, HTAnchor_findAddress(uri));
	HT_FREE(uri);
    } else {
	HTRequest_addError(client, ERR_FATAL, NO, HTERR_BAD_REQUEST,
			   NULL, 0, "ParseRequest");
	return HT_ERROR;
    }

    /* Get ready to get the rest of the request */
    if (version_str) {
	me->target = HTStreamStack(WWW_MIME_HEAD,
				   HTRequest_debugFormat(client),
				   HTRequest_debugStream(client),
				   client, NO);
	return HT_OK;
    } else {
	HTRequest_addError(client, ERR_FATAL, NO, HTERR_BAD_VERSION,
			   NULL, 0, "ParseRequest");
	return HT_ERROR;
    }
}

/*
**	Searches for HTTP Request Line before going into transparent mode
*/
PRIVATE int HTTPReceive_put_block (HTStream * me, const char * b, int l)
{
    if (!me->transparent) {
	const char *p=b;
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

PRIVATE int HTTPReceive_put_string (HTStream * me, const char * s)
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
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HTChunk_delete(me->buffer);
    HT_FREE(me);
    HTTRACE(PROT_TRACE, "HTTPReceive. ABORTING...\n");
    return HT_ERROR;
}

/*	HTTPReceive Stream
**	-----------------
*/
PRIVATE const HTStreamClass HTTPReceiveClass =
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
    HTTRACE(STREAM_TRACE, "HTTP Request Stream %p created\n" _ me);
    return me;
}

/* ------------------------------------------------------------------------- */

/*	HTServHTTP
**	----------
**	Serv Document using HTTP.
**	returns	HT_ERROR or HT_OK
*/
PRIVATE int ServEvent (SOCKET soc, void * pVoid, HTEventType type);

PUBLIC int HTServHTTP (SOCKET soc, HTRequest * request)
{
    HTNet * net = HTRequest_net(request);
    https_info * http;			    /* Specific protocol information */

    /*
    ** Initiate a new https object and bind to request object
    ** This is actually state HTTPS_BEGIN, but it can't be in the state
    ** machine as we need the object first (chicken and egg problem).
    */
    HTTRACE(PROT_TRACE, "Serv HTTP... on socket %d\n" _ soc);
    if ((http = (https_info *) HT_CALLOC(1, sizeof(https_info))) == NULL)
	HT_OUTOFMEM("HTServHTTP");
    http->server = request;
    http->state = HTTPS_BEGIN;
    http->clients = HTList_new();
    HTNet_setContext(net, http);

    /* 
    ** Create the stream pipe FROM the channel to the server request.
    */
    net->readStream = HTTPReceive_new(request, http);
    HTRequest_setOutputConnected(request, YES);
    http->state = HTTPS_BEGIN;

    HTNet_setEventCallback(net, ServEvent);
    HTNet_setEventParam(net, http);  /* callbacks get http* */

    return ServEvent(soc, http, HTEvent_BEGIN);		/* get it started - ops is ignored */
}

PRIVATE int ServEvent (SOCKET soc, void * pVoid, HTEventType type)
{
    https_info * http = (https_info *)pVoid;
    int status = HT_ERROR;
    HTNet * net = http->net;
    HTRequest * request = HTNet_request(net);

    if (!net || !request) {
	HTTRACE(PROT_TRACE, "Serv HTTP... Invalid argument\n");
	return HT_ERROR;
    }

    if (type == HTEvent_CLOSE) {			      /* Interrupted */
	ServerCleanup(request, net, HT_INTERRUPTED);
	return HT_OK;
    } else
	http = (https_info *) HTNet_context(net);	/* Get existing copy */
 
    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (http->state) {
	case HTTPS_BEGIN:
	{
	    /*
	    ** Create the request to handle the request and inherit the old
	    ** context
	    */
	    HTRequest * client = HTRequest_new();
	    void * context = HTRequest_context(request);
	    if (context) HTRequest_setContext(client, context);
	    HTRequest_setOutputConnected(client, NO);
	    HTRequest_setGnHd(client, HTRequest_gnHd(request));
	    HTRequest_setRsHd(client, HTRequest_rsHd(request));
	    HTRequest_setEnHd(client, HTRequest_enHd(request));
	    HTList_addObject(http->clients, client);
	    
	    /*
	    ** Create the HTTP output stream for generating the reply
	    ** FROM the client request to the channel
	    */
	    {
		HTOutputStream * output = HTNet_getOutput(net, NULL, 0);
		HTStream * app = HTTPReply_new(client, http,(HTStream*)output);
		HTRequest_setOutputStream(client, app);
		HTRequest_setOutputFormat(client, WWW_SOURCE);
	    }
	    http->state = HTTPS_NEED_REQUEST;
	}
	break;

	case HTTPS_NEED_REQUEST:
	    if (type == HTEvent_READ || type == HTEvent_BEGIN) {
		status = HTHost_read(net->host, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_CLOSED)
		    http->state = HTTPS_OK;
		else if (status==HT_LOADED || status==HT_PAUSE) {
		    http->state = HTTPS_LOAD_CLIENT;
		} else
		    http->state = HTTPS_ERROR;
	    } else
		http->state = HTTPS_ERROR;
	    break;

	case HTTPS_LOAD_CLIENT:
	{
	    HTRequest * client = HTList_removeFirstObject(http->clients);
	    HTLoad(client, NO);
	    http->state = HTTPS_BEGIN;
	    break;
	}

	case HTTPS_OK:
	    ServerCleanup(request, net, HT_IGNORE);
	    return HT_OK;

	case HTTPS_ERROR:
	    ServerCleanup(request, net, HT_ERROR);
	    return HT_OK;
	}
    }
}

