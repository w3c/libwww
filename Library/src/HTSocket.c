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
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWTrans.h"
#include "HTNetMan.h"
#include "HTSocket.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

/* ------------------------------------------------------------------------- */

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
    HTNet * net = HTRequest_net(request);
    if (soc==INVSOC) {
	if (PROT_TRACE) HTTrace("Load Socket. invalid socket\n");
	return HT_ERROR;
    }
    if (PROT_TRACE) HTTrace("Load Socket. Loading socket %d\n",soc);

    /* 
    ** Create the stream pipe FROM the channel to the application.
    ** The target for the input stream pipe is set up using the
    ** stream stack.
    */
    {
	net->readStream = HTRequest_outputStream(request);
	if (!net->readStream) net->readStream = HTErrorStream();
	HTRequest_setOutputConnected(request, YES);
    }
    HTNet_setEventCallback(net, SocketEvent);
    HTNet_setEventParam(net, net);  /* callbacks get http* */

    return SocketEvent(soc, net, HTEvent_BEGIN);		/* get it started - ops is ignored */
}

PRIVATE int SocketEvent (SOCKET soc, void * pVoid, HTEventType type)
{
    HTNet * net = (HTNet *)pVoid;
    if (type == HTEvent_CLOSE) {			      /* Interrupted */
	HTNet_delete(net, HT_INTERRUPTED);
	return HT_OK;
    }

    /* In this load function we only have one state: READ */
    {
	int status = HTHost_read(net->host, net);
	if (PROT_TRACE) HTTrace("Load Socket. Read returns %d\n", status);
    }
    return HT_OK;
}

