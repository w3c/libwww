/*
**	READ STREAM FROM THE NETWORK VIA SSL
**
**	@(#) $Id$
**	
**	Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**	This module requires a SSL library in order to compile/link
**
**	AUTHOR:
**		Olga Antropova <olga@eai.com>
**
*/

/* System Include files */
#include "wwwsys.h"

/* SSL include files */
#include <openssl/ssl.h>

/* Library include files */
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTNet.h"
#include "HTNetMan.h"

#include "HTSSL.h"
#include "HTSSLReader.h"				 /* Implemented here */

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
    HTSSL *                     htssl;
};

/* ------------------------------------------------------------------------- */

PRIVATE int HTSSLReader_flush (HTInputStream * me)
{
    HTNet * net = HTHost_getReadNet(me->host);
    return net && net->readStream ? (*net->readStream->isa->flush)(net->readStream) : HT_OK;
}

PRIVATE int HTSSLReader_free (HTInputStream * me)
{
    HTNet * net = HTHost_getReadNet(me->host);
    HTTRACE(PROT_TRACE, "HTSSLReader. Free %p\n" _ me);
    if (me->htssl) {
	HTSSL_free(me->htssl);
	me->htssl = NULL;
    }

    if (net && net->readStream) {
	int status = (*net->readStream->isa->_free)(net->readStream);
        if (status == HT_OK) net->readStream = NULL;
	return status;
    }
    return HT_OK;
}

PRIVATE int HTSSLReader_abort (HTInputStream * me, HTList * e)
{
    HTNet * net = HTHost_getReadNet(me->host);
    HTTRACE(PROT_TRACE, "HTSSLReader. Abort %p\n" _ me);
    if (net && net->readStream) {
	int status = (*net->readStream->isa->abort)(net->readStream, NULL);
	if (status != HT_IGNORE) net->readStream = NULL;
    }
    return HT_ERROR;
}

#ifdef FIND_SIGNATURES
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
PRIVATE char * strnstr(char * haystack, int *pLen, char * needle)
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
#endif /* FIND_SIGNATURES */

PRIVATE int HTSSLReader_read (HTInputStream * me)
{
    HTHost * host = me->host;
    SOCKET soc = HTChannel_socket(me->ch);
    HTNet * net = HTHost_getReadNet(host);
    HTRequest * request = HTNet_request(net);
    int status;
    if (!net->readStream) {
	HTTRACE(STREAM_TRACE, "HTSSLReader. No read stream for net object %p\n" _ net);
        return HT_ERROR;
    }
        
    /* Setting SSL */
    if (!me->htssl) {
	if ((me->htssl = HTSSL_new(soc)) == NULL) {
	    HTRequest_addSystemError(net->request, ERR_FATAL, socerrno, NO, "SSLREAD");
	    return HT_ERROR;
	}
    }

    /* Read from socket if we got rid of all the data previously read */
    do {

	/* Don't read if we have to push unwritten data from last call */
        if (me->write >= me->read) {
            me->b_read = 0;
            me->data[0] ='\0';
 	    me->b_read = HTSSL_read(me->htssl, soc, me->data, INPUT_BUFFER_SIZE);     
	    status = HTSSL_getError(me->htssl, me->b_read);
	    HTTRACE(STREAM_TRACE, "HTSSLReader. SSL returned %d\n" _ status);

	    /* Check what we got done */
	    switch (status) {

	    case SSL_ERROR_NONE:

		HTTRACEDATA(me->data, me->b_read, "Reading from socket %d" _ soc);
		me->write = me->data;
		me->read = me->data + me->b_read;
		HTTRACE(STREAM_TRACE, "HTSSLReader. %d bytes read from socket %d\n" _ 
			me->b_read _ soc);

		/* Make progress notification */
		if (request) {
		    HTAlertCallback * cbf = HTAlert_find(HT_PROG_READ);
		    if (HTNet_rawBytesCount(net))
			HTNet_addBytesRead(net, me->b_read);
		    if (cbf) {
			int tr = HTNet_bytesRead(net);
			(*cbf)(request, HT_PROG_READ, HT_MSG_NULL, NULL, &tr, NULL);
		    }
		}
		break;

	    case SSL_ERROR_WANT_READ:
		HTTRACE(STREAM_TRACE, "HTSSLReader. WOULD BLOCK fd %d\n" _ soc);
		HTHost_register(host, net, HTEvent_READ);

		/*
		**  There seems to be a bug as even though it says "read finished"
		**  it doesn't say that it wants to write. We therefore have to make
		**  an explicit flush to make sure that we don't block forever.
		*/
		HTHost_forceFlush(host);

		return HT_WOULD_BLOCK;

	    case SSL_ERROR_WANT_WRITE:
		return HTHost_forceFlush(host);

	    case SSL_ERROR_WANT_X509_LOOKUP:
		/* @@@ what to do here? @@@ */
		return HT_OK;

	    case SSL_ERROR_ZERO_RETURN:
	    case SSL_ERROR_SSL:
	    case SSL_ERROR_SYSCALL:
		HTTRACE(PROT_TRACE, "HTSSLReader. FIN received on socket %d\n" _ soc);
                HTHost_unregister(host, net, HTEvent_READ);
                HTHost_register(host, net, HTEvent_CLOSE);

                HTSSL_close(me->htssl);    
                HTSSL_free(me->htssl);
                me->htssl = NULL;

                return HT_CLOSED;
	    }
	}

#ifdef FIND_SIGNATURES
	{
	    char * ptr = me->data;
	    int len = me->b_read;
	    while ((ptr = strnstr(ptr, &len, "HTTP/1.1 200 OK")) != NULL) {
		HTTRACE(STREAM_TRACE, "HTSSLReader. Signature found at 0x%x of 0x%x.\n" _ ptr - me->data _ me->b_read);
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
	
	/* Now push the data down the stream */
	if ((status = (*net->readStream->isa->put_block)
	     (net->readStream, me->write, me->b_read)) != HT_OK) {
	    if (status == HT_WOULD_BLOCK) {
		HTTRACE(STREAM_TRACE, "HTSSLReader. Target WOULD BLOCK\n");
		HTHost_unregister(host, net, HTEvent_READ);
		return HT_WOULD_BLOCK;
	    } else if (status == HT_PAUSE) {
		HTTRACE(STREAM_TRACE, "HTSSLReader. Target PAUSED\n");
		HTHost_unregister(host, net, HTEvent_READ);
		return HT_PAUSE;
	    /* CONTINUE code or stream code means data was consumed */
	    } else if (status == HT_CONTINUE || status > 0) {
		if (status == HT_CONTINUE) {
		    HTTRACE(STREAM_TRACE, "HTSSLReader. CONTINUE\n");
		} else
		    HTTRACE(STREAM_TRACE, "HTSSLReader. Target returns %d\n" _ status);
/*		me->write = me->read; */
		return status;
	    } else {				     /* We have a real error */
		HTTRACE(STREAM_TRACE, "HTSSLReader. Target ERROR %d\n" _ status);
		return status;
	    }
	}
	me->write = me->read;
	{
	    int remaining = HTHost_remainingRead(host);
	    if (remaining > 0) {
		HTTRACE(STREAM_TRACE, "HTSSLReader. DIDN'T CONSUME %d BYTES: `%s\'\n" _ 
			    remaining _ me->read);
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
PRIVATE int HTSSLReader_close (HTInputStream * me)
{
    int status = HT_OK;
    HTNet * net = HTHost_getReadNet(me->host);
    if (net && net->readStream) {
	if ((status = (*net->readStream->isa->_free)(net->readStream))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	net->readStream = NULL;
    }
    HTTRACE(STREAM_TRACE, "HTSSLReader. FREEING....\n");
    HT_FREE(me);
    return status;
}

PUBLIC int HTSSLReader_consumed (HTInputStream * me, size_t bytes)
{
    me->write += bytes;
    me->b_read -= bytes;
    HTHost_setRemainingRead(me->host, me->b_read);
    return HT_OK;
}

PRIVATE const HTInputStreamClass HTSSLReader =
{
    "SocketReader",
    HTSSLReader_flush,
    HTSSLReader_free,
    HTSSLReader_abort,
    HTSSLReader_read,
    HTSSLReader_close,
    HTSSLReader_consumed
}; 

/*
**	Create a new input read stream. Before we actually create it we check
**	to see whether we already have an input stream for this channel and if
**	so we just return that. This means that we can reuse input streams 
**	in persistent connections, for example.
*/

PUBLIC HTInputStream * HTSSLReader_new (HTHost * host, HTChannel * ch,
					void * param, int mode)
{
    if (host && ch) {
        HTInputStream * me = HTChannel_input(ch);
        if (me == NULL) {
            if ((me=(HTInputStream *) HT_CALLOC(1, sizeof(HTInputStream))) == NULL)
		HT_OUTOFMEM("HTSSLReader_new");
            me->isa = &HTSSLReader;
            me->ch = ch;
            me->host = host;
            me->htssl = NULL;
        }
        return me;
    }
    return NULL;
}

