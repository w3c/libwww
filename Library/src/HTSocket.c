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
PUBLIC int HTLoadSocket (SOCKET soc, HTRequest * request, SockOps ops)
{
    HTNet * net = HTRequest_net(request);
    if (!net || !request) {
	if (PROT_TRACE) HTTrace("Load Socket. invalid argument\n");
	return HT_ERROR;
    }
    if (ops == FD_NONE) {
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
	    HTStream * target = HTRequest_outputStream(request);
	    if (!target) target = HTErrorStream();
	    HTNet_getInput(net, target, NULL, 0);
	    HTRequest_setOutputConnected(request, YES);
	}
    } else if (ops == FD_CLOSE) {			      /* Interrupted */
	HTNet_delete(net, HT_INTERRUPTED);
	return HT_OK;
    }

    /* In this load function we only have one state: READ */
    {
	int status = (*net->input->isa->read)(net->input);
	if (PROT_TRACE) HTTrace("Load Socket. Read returns %d\n", status);
    }
    return HT_OK;
}

