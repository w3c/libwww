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
#include "wwwsys.h"
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
    HTHost *			host;
    char *			write;			/* Last byte written */
    char *			read;			   /* Last byte read */
    int				b_read;
    char			data [INPUT_BUFFER_SIZE];	   /* buffer */
};

/* ------------------------------------------------------------------------- */

PRIVATE int HTReader_flush (HTInputStream * me)
{
    HTNet * net = HTHost_getReadNet(me->host);
    return net && net->readStream ? (*net->readStream->isa->flush)(net->readStream) : HT_OK;
}

PRIVATE int HTReader_free (HTInputStream * me)
{
    HTNet * net = HTHost_getReadNet(me->host);
    if (net && net->readStream) {
	int status = (*net->readStream->isa->_free)(net->readStream);
        if (status == HT_OK) net->readStream = NULL;
	return status;
    }
    return HT_OK;
}

PRIVATE int HTReader_abort (HTInputStream * me, HTList * e)
{
    HTNet * net = HTHost_getReadNet(me->host);
    if (net && net->readStream) {
	int status = (*net->readStream->isa->abort)(net->readStream, NULL);
	if (status != HT_IGNORE) net->readStream = NULL;
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
char * strnstr(char * haystack, int *pLen, char * needle)
{
    int found = 0;
    int need = strlen(needle);
    int i, start;
    for (start = i = 0; i < *pLen; i++)
	if (haystack[i] == needle[found]) {
	    if (++found == need) {
		i -= need - 1; /* beginning of string */
		*pLen -= i;
		return haystack+i;
	    }
	} else {
	    found = 0;
	}
    *pLen = 0;
    return NULL;
}

/* int DebugBufferSize = INPUT_BUFFER_SIZE; */

PRIVATE int HTReader_read (HTInputStream * me)
{
    HTHost * host = me->host;
    SOCKET soc = HTChannel_socket(me->ch);
    HTNet * net = HTHost_getReadNet(host);
    HTRequest * request = HTNet_request(net);
    int status;
    if (!net->readStream) {
	if (STREAM_TRACE)
	    HTTrace("Read Socket. No read stream for net object %p\n", net);
        return HT_ERROR;
    }
        
    /*    me->b_read = me->read - me->data; */
    /* Read from socket if we got rid of all the data previously read */
    do {
	/* don't read if we have to push unwritten data from last call */
	if (me->write >= me->read) {
	    if ((me->b_read = NETREAD(soc, me->data, INPUT_BUFFER_SIZE)) < 0) {
#ifdef EAGAIN
		if (socerrno==EAGAIN || socerrno==EWOULDBLOCK)      /* POSIX */
#else
		if (socerrno==EWOULDBLOCK) 			      /* BSD */
#endif	
		{
		    if (STREAM_TRACE)
			HTTrace("Read Socket. WOULD BLOCK fd %d\n",soc);
		    HTHost_register(host, net, HTEvent_READ);
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
#ifdef EPIPE
		} else if (socerrno == EPIPE) {
		    if (STREAM_TRACE) HTTrace("Read Socket. got EPIPE\n", soc);
		    goto socketClosed;
#endif /* EPIPE */
#ifdef ECONNRESET
		} else if (socerrno == ECONNRESET) {
		    if (STREAM_TRACE) HTTrace("Read Socket. got ECONNRESET\n", soc);
		    goto socketClosed;
#endif /* ECONNRESET */
		} else { 			     /* We have a real error */

		    if (request)
			HTRequest_addSystemError(request, ERR_FATAL, socerrno,
						 NO, "NETREAD");
		    return HT_ERROR;
		}
	    } else if (!me->b_read) {

	    socketClosed:
		if (STREAM_TRACE)
		    HTTrace("Read Socket. FIN received on socket %d\n", soc);
		HTHost_unregister(host, net, HTEvent_READ);
		HTHost_register(host, net, HTEvent_CLOSE);
		return HT_CLOSED;
	    }

	    /* Remember how much we have read from the input socket */
	    HTTraceData(me->data, me->b_read, "Reading from socket %d", soc);
	    me->write = me->data;
	    me->read = me->data + me->b_read;
#ifdef FIND_SIGNATURES
	    {
		char * ptr = me->data;
		int len = me->b_read;
		while ((ptr = strnstr(ptr, &len, "HTTP/1.1 200 OK")) != NULL) {
		    if (STREAM_TRACE)
			HTTrace("Read Socket. Signature found at 0x%x of 0x%x.\n", ptr - me->data, me->b_read);
		    ptr++;
		    len--;
		}
	    }
#endif /* FIND_SIGNATURES */
#ifdef NOT_ASCII
	    {
		char *p = me->data;
		while (p < me->read) {
		    *p = FROMASCII(*p);
		    p++;
		}
	    }
#endif /* NOT_ASCII */

	    if (STREAM_TRACE)
		HTTrace("Read Socket. %d bytes read from socket %d\n",
			me->b_read, soc);
	    if (request) {
		HTAlertCallback * cbf = HTAlert_find(HT_PROG_READ);
		if (HTNet_rawBytesCount(net))
		    HTNet_addBytesRead(net, me->b_read);
		if (cbf) (*cbf)(request, HT_PROG_READ,
				HT_MSG_NULL, NULL, NULL, NULL);
	    }
	}

	/* Now push the data down the stream */
	if ((status = (*net->readStream->isa->put_block)
	     (net->readStream, me->write, me->b_read)) != HT_OK) {
	    if (status == HT_WOULD_BLOCK) {
		if (STREAM_TRACE) HTTrace("Read Socket. Target WOULD BLOCK\n");
		HTHost_unregister(host, net, HTEvent_READ);
		return HT_WOULD_BLOCK;
	    } else if (status == HT_PAUSE) {
		if (STREAM_TRACE) HTTrace("Read Socket. Target PAUSED\n");
		HTHost_unregister(host, net, HTEvent_READ);
		return HT_PAUSE;
	    /* CONTINUE code or stream code means data was consumed */
	    } else if (status == HT_CONTINUE || status > 0) {
		if (status == HT_CONTINUE) {
		    if (STREAM_TRACE) HTTrace("Read Socket. CONTINUE\n");
		} else
		    if (STREAM_TRACE)
			HTTrace("Read Socket. Target returns %d\n", status);
/*		me->write = me->read; */
		return status;
	    } else {				     /* We have a real error */
		if (STREAM_TRACE) HTTrace("Read Socket. Target ERROR %d\n", status);
		return status;
	    }
	}
	me->write = me->read;
	{
	    int remaining = HTHost_remainingRead(host);
	    if (remaining > 0) {
		if (STREAM_TRACE)
		    HTTrace("Read Socket. DIDN'T CONSUME %d BYTES: `%s\'\n",
			    remaining, me->read);
		HTHost_setConsumed(host, remaining);
	    }
	}
    } while (net->preemptive);
    HTHost_register(host, net, HTEvent_READ);
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
    HTNet * net = HTHost_getReadNet(me->host);
    if (net && net->readStream) {
	if ((status = (*net->readStream->isa->_free)(net->readStream))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	net->readStream = NULL;
    }
    if (STREAM_TRACE) HTTrace("Socket read. FREEING....\n");
    HT_FREE(me);
    return status;
}

PUBLIC int HTReader_consumed (HTInputStream * me, size_t bytes)
{
    me->write += bytes;
    me->b_read -= bytes;
    HTHost_setRemainingRead(me->host, me->b_read);
    return HT_OK;
}

PRIVATE const HTInputStreamClass HTReader =
{
    "SocketReader",
    HTReader_flush,
    HTReader_free,
    HTReader_abort,
    HTReader_read,
    HTReader_close,
    HTReader_consumed
}; 

/*
**	Create a new input read stream. Before we actually create it we check
**	to see whether we already have an input stream for this channel and if
**	so we just return that. This means that we can reuse input streams 
**	in persistent connections, for example.
*/
PUBLIC HTInputStream * HTReader_new (HTHost * host, HTChannel * ch,
				     void * param, int mode)
{
    if (host && ch) {
	HTInputStream * me = HTChannel_input(ch);
	if (me == NULL) {
	    if ((me=(HTInputStream *) HT_CALLOC(1, sizeof(HTInputStream))) == NULL)
	    HT_OUTOFMEM("HTReader_new");
	    me->isa = &HTReader;
	    me->ch = ch;
	    me->host = host;
	    if (STREAM_TRACE) HTTrace("Reader...... Created reader stream %p\n", me);
	}
	return me;
    }
    return NULL;
}

