/*								     HTSocket.c
**	LOAD A SOCKET
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**
** HISTORY:
**	6 June 95  HFN	Written
*/

/* Library Include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWTrans.h"
#include "HTNetMan.h"
#include "HTSocket.h"					 /* Implemented here */

#ifndef RAW_PORT
#define RAW_PORT 1024
#endif

/* Final states have negative value */
typedef enum _RAWState {
    RAW_ERROR		= -2,
    RAW_OK		= -1,
    RAW_BEGIN		= 0,
    RAW_NEED_STREAM,
    RAW_READ
} RawState;

/* This is the context structure for the this module */
typedef struct _raw_info {
    RawState		state;		  /* Current State of the connection */
    HTNet *		net;
    HTRequest *		request;
} raw_info;

struct _HTStream {
    const HTStreamClass *	isa;
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

/* ------------------------------------------------------------------------- */

PRIVATE int RawCleanup (HTRequest * request, int status)
{
    HTNet * net = HTRequest_net(request);
    raw_info * raw = (raw_info *) HTNet_context(net);

    HTTRACE(PROT_TRACE, "Raw clean... Called with status %d, net %p\n" _ status _ net);

    if (status == HT_INTERRUPTED) {
    	HTAlertCallback * cbf = HTAlert_find(HT_PROG_INTERRUPT);
    	if (cbf) (*cbf)(request, HT_PROG_INTERRUPT,
	    HT_MSG_NULL, NULL, NULL, NULL);
    } else if (status == HT_TIMEOUT) {
    	HTAlertCallback * cbf = HTAlert_find(HT_PROG_TIMEOUT);
    	if (cbf) (*cbf)(request, HT_PROG_TIMEOUT,
	    HT_MSG_NULL, NULL, NULL, NULL);
    }	

    /* Delete the Net object */
    HTNet_delete(net, HT_ERROR);

    HT_FREE(raw);
    return YES;
}

/*	HTLoadSocket
**	------------
**	Given an open socket, this routine loads what ever is on the socket
**
** On entry,
**      request		This is the request structure
** On Exit
**	returns		HT_ERROR	Error has occured in call back
**			HT_OK		Call back was OK
*/
PRIVATE int SocketEvent (SOCKET soc, void * pVoid, HTEventType type);

PUBLIC int HTLoadSocket (SOCKET soc, HTRequest * request)
{
    raw_info * raw;			    /* Specific protocol information */
    HTNet * net = HTRequest_net(request);
    HTTRACE(PROT_TRACE, "Load socket. Setting up socket for accept\n");
    if ((raw = (raw_info *) HT_CALLOC(1, sizeof(raw_info))) == NULL)
      HT_OUTOFMEM("HTLoadSocket");
    raw->state = RAW_BEGIN;
    raw->net = net;
    raw->request = request;
    HTNet_setContext(net, raw);
    HTNet_setEventCallback(net, SocketEvent);
    HTNet_setEventParam(net, raw);

    /* Start listening on a socket */
    if (HTHost_listen(NULL, net, HTAnchor_physical(HTRequest_anchor(request))) == HT_ERROR)
	return SocketEvent(soc, raw, HTEvent_CLOSE);

    /* Get it started - ops is ignored */
    return SocketEvent(soc, raw, HTEvent_BEGIN);
}

PRIVATE int SocketEvent (SOCKET soc, void * pVoid, HTEventType type)
{
    raw_info * raw = (raw_info *) pVoid;
    int status = HT_ERROR;
    HTNet * net = raw->net;
    HTRequest * request = raw->request;
    HTHost * host = HTNet_host(net);

    /*
    **  Check whether we have been interrupted or timed out
    */
    if (type == HTEvent_BEGIN) {
	raw->state = RAW_BEGIN;
    } else if (type == HTEvent_CLOSE) {			      /* Interrupted */
	RawCleanup(request, HT_INTERRUPTED);
	return HT_OK;
    } else if (type == HTEvent_TIMEOUT) {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_TIME_OUT,
			   NULL, 0, "HTLoadSocket");
	RawCleanup(request, HT_TIMEOUT);
	return HT_OK;
    } else if (type == HTEvent_END) {
	RawCleanup(request, HT_OK);
	return HT_OK;
    }
	
    /* Now jump into the state machine */
    while (1) {
	switch(raw->state) {
	case RAW_BEGIN:
	    status = HTHost_accept(host, net, NULL);
	    host = HTNet_host(net);
            if (status == HT_OK) {
		raw->state = RAW_NEED_STREAM;
	    } else if (status == HT_WOULD_BLOCK || status == HT_PENDING) {
		return HT_OK;
	    } else	
		raw->state = RAW_ERROR;	       /* Error or interrupt */
	    break;

	case RAW_NEED_STREAM:
	{
	    /* 
	    ** Create the stream pipe FROM the channel to the application.
	    ** The target for the input stream pipe is set up using the
	    ** stream stack.
	    */
            HTStream * in_stream =
		HTStreamStack(WWW_RAW,
			      HTRequest_outputFormat(request),
			      HTRequest_outputStream(request),
			      request, YES);
	    HTNet_setReadStream(net, in_stream);
            HTRequest_setOutputConnected(request, YES);

	    raw->state = RAW_READ;
	    break;
	}

	case RAW_READ:
	    status = HTHost_read(host, net);
	    if (status == HT_WOULD_BLOCK)
		return HT_OK;
	    else if (status==HT_CLOSED)
		raw->state = RAW_OK;
	    else 
		raw->state = RAW_ERROR;
	    break;

	case RAW_OK:
	    RawCleanup(request, HT_OK);
	    return HT_OK;
	    break;

	case RAW_ERROR:
	    RawCleanup(request, HT_ERROR);
	    return HT_OK;
	    break;

	default:
	    HTDEBUGBREAK("Bad raw state %d\n" _ raw->state);
	}
    }
    return HT_OK;
}

