/*								     HTReader.c
**	READ STREAM FROM THE NETWORK USING TCP
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** HISTORY:
**	6 June 95  HFN	Written
*/

/* Library Include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTNetMan.h"
#include "HTReader.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
    /* ... */
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
    HTChannel *			ch;
    HTNet *			net;
    HTStream *			target;		 /* Target for incoming data */
    char *			write;			/* Last byte written */
    char *			read;			   /* Last byte read */
    char			data [INPUT_BUFFER_SIZE];	   /* buffer */
};

/* ------------------------------------------------------------------------- */

PRIVATE int HTReader_flush (HTInputStream * me)
{
    return me->target ? (*me->target->isa->flush)(me->target) : HT_OK;
}

PRIVATE int HTReader_free (HTInputStream * me)
{
    if (me->target) {
	int status = (*me->target->isa->_free)(me->target);
	if (status != HT_WOULD_BLOCK) me->target = NULL;
	return status;
    }
    return HT_OK;
}

PRIVATE int HTReader_abort (HTInputStream * me, HTList * e)
{
    if (me->target) {
	(*me->target->isa->abort)(me->target, NULL);
	me->target = NULL;
    }
    return HT_ERROR;
}

/*	Push data from a socket down a stream
**	-------------------------------------
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the file. As this function
**   max reads a chunk of data on size INPUT_BUFFER_SIZE, it can be used
**   with both blocking or non-blocking sockets. It will always return to
**   the event loop, however if we are using blocking I/O then we get a full
**   buffer read, otherwise we get what's available.
**
** Returns      HT_LOADED	if finished reading
**		HT_OK		if OK, but more to read
**	      	HT_ERROR	if error,
**     		HT_WOULD_BLOCK	if read or write would block
**		HT_PAUSE	if stream is paused
*/
PRIVATE int HTReader_read (HTInputStream * me)
{
    HTNet * net = me->net;
    SOCKET soc = net->sockfd;
    int b_read = me->read - me->data;
    int status;

    /* Read from socket if we got rid of all the data previously read */
    do {
	if (me->write >= me->read) {
	    if ((b_read = NETREAD(soc, me->data, INPUT_BUFFER_SIZE)) < 0) {
#ifdef EAGAIN
		if (socerrno==EAGAIN || socerrno==EWOULDBLOCK)      /* POSIX */
#else
		if (socerrno==EWOULDBLOCK) 			      /* BSD */
#endif	
		{
		    if (PROT_TRACE)
			HTTrace("Read Socket. WOULD BLOCK fd %d\n",soc);
		    HTEvent_register(soc, net->request, (SockOps) FD_READ,
				     net->cbf, net->priority);
		    return HT_WOULD_BLOCK;
#ifdef __svr4__
    /* 
    ** In Solaris envirnoment, SIGPOLL is used to signal end of buffer for
    ** /dev/audio.  If your process is also doing a socket read, it will cause
    ** an EINTR error.  This error will cause the www library request to 
    ** terminate prematurly.
    */
                } else if (socerrno == EINTR) {
                    continue;
#endif /* __svr4__ */
		} else { 			     /* We have a real error */

		    /* HERE WE SHOULD RETURN target abort */

		    HTRequest_addSystemError(net->request, ERR_FATAL, socerrno,
					     NO, "NETREAD");
		    return HT_ERROR;
		}
	    } else if (!b_read || socerrno==ECONNRESET) {
		HTAlertCallback *cbf = HTAlert_find(HT_PROG_DONE);
		if (PROT_TRACE)
		    HTTrace("Read Socket. FIN received on socket %d\n", soc);
		if (cbf) (*cbf)(net->request, HT_PROG_DONE,
				HT_MSG_NULL, NULL, NULL, NULL);
	        HTEvent_unregister(soc, FD_ALL);
		return HT_CLOSED;
	    }

	    /* Remember how much we have read from the input socket */
	    me->write = me->data;
	    me->read = me->data + b_read;

#ifdef NOT_ASCII
	    {
		char *p = me->data;
		while (p < me->read) {
		    *p = FROMASCII(*p);
		    p++;
		}
	    }
#endif /* NOT_ASCII */

	    if (PROT_TRACE) 
		HTTrace("Read Socket. %d bytes read from socket %d\n",
			b_read, soc);
	    {
		HTAlertCallback * cbf = HTAlert_find(HT_PROG_READ);
		net->bytes_read += b_read;
		if (cbf) (*cbf)(net->request, HT_PROG_READ,
				HT_MSG_NULL, NULL, NULL, NULL);
	    }
	}

	/* Now push the data down the stream */
	if ((status = (*me->target->isa->put_block)
	     (me->target, me->data, b_read)) != HT_OK) {
	    if (status == HT_WOULD_BLOCK) {
		if (PROT_TRACE) HTTrace("Read Socket. Target WOULD BLOCK\n");
		HTEvent_unregister(soc, FD_READ);
		return HT_WOULD_BLOCK;
	    } else if (status == HT_PAUSE) {
		if (PROT_TRACE) HTTrace("Read Socket. Target PAUSED\n");
		HTEvent_unregister(soc, FD_READ);
		return HT_PAUSE;
	    } else if (status == HT_CONTINUE) {
		if (PROT_TRACE) HTTrace("Read Socket. CONTINUE\n");
		me->write = me->data + b_read;
		return HT_CONTINUE;
	    } else if (status>0) {	      /* Stream specific return code */
		if (PROT_TRACE)
		    HTTrace("Read Socket. Target returns %d\n", status);
		me->write = me->data + b_read;
		return status;
	    } else {				     /* We have a real error */
		if (PROT_TRACE) HTTrace("Read Socket. Target ERROR\n");
		return status;
	    }
	}
	me->write = me->data + b_read;
    } while (net->preemptive);
    HTEvent_register(soc, net->request, (SockOps) FD_READ,
		     net->cbf, net->priority);
    return HT_WOULD_BLOCK;
}

/*
**	The difference between the close and the free method is that we don't
**	close the connection in the free method - we only call the free method
**	of the target stream. That way, we can keep the input stream as long 
**	as the channel itself.
*/
PRIVATE int HTReader_close (HTInputStream * me)
{
    int status = HT_OK;
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    if (PROT_TRACE) HTTrace("Socket read. FREEING....\n");
    HT_FREE(me);
    return status;
}

PRIVATE const HTInputStreamClass HTReader =
{
    "SocketReader",
    HTReader_flush,
    HTReader_free,
    HTReader_abort,
    HTReader_read,
    HTReader_close
}; 

/*
**	Create a new input read stream. Before we actually create it we check
**	to see whether we already have an input stream for this channel and if
**	so we just return that. This means that we can reuse input streams 
**	in persistent connections, for example.
*/
PUBLIC HTInputStream * HTReader_new (HTNet * net, HTChannel * ch,
				     HTStream * target, void * param, int mode)
{
    if (net && ch) {
	HTInputStream * me = HTChannel_input(ch);
	if (me == NULL) {
	    if ((me=(HTInputStream *) HT_CALLOC(1, sizeof(HTInputStream))) == NULL)
	    HT_OUTOFMEM("HTReader_new");
	    me->isa = &HTReader;
	    me->ch = ch;
	}
	me->target = target;
	me->net = net;
	return me;
    }
    return NULL;
}
