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
#include "HTUtils.h"
#include "HTString.h"
#include "HTReqMan.h"
#include "HTProt.h"
#include "HTIOStream.h"
#include "HTChannl.h"
#include "HTAlert.h"
#include "HTFormat.h"
#include "HTNetMan.h"
#include "HTError.h"
#include "HTSocket.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
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
    HTNet * net = NULL;
    if (!request) return HT_ERROR;
    if (ops == FD_NONE) {
	HTNet * me;
	if (soc==INVSOC) {
	    if (PROT_TRACE) HTTrace("Load Socket. invalid socket\n");
	    return HT_ERROR;
	}
	if (PROT_TRACE) HTTrace("Load Socket. Loading socket %d\n",soc);
	me = HTNet_new(request, soc);
	me->sockfd = soc;
#if 0
	me->target = request->output_stream;
	HTChannel_new(net, HT_CH_UNBUFFERED, 0, NO);
#endif
	net = me;
    } else if (ops == FD_CLOSE) {			      /* Interrupted */
	HTNet_delete(request->net, HT_INTERRUPTED);
	return HT_OK;
    } else
	net = request->net;
    if (!net) {
	if (PROT_TRACE) HTTrace("Load Socket. invalid argument\n");
	return HT_ERROR;
    }

    /* In this load function we only have one state: READ */
    {
#if 0
	int status = HTChannel_readSocket(request, net);
	if (status == HT_WOULD_BLOCK)
	    return HT_OK;
	else if (status == HT_CLOSED)
	    HTNet_delete(request->net, HT_LOADED);
	else
	    HTNet_delete(request->net, HT_ERROR);
#endif
    }
    return HT_OK;
}

