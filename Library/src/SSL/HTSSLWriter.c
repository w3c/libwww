/*
**	WRITER STREAM FROM APP TO NETWORK VIA SSL
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

/* System include files */
#include "wwwsys.h"

/* SSL include files */
#include <openssl/ssl.h>

/* Library include files */
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWTrans.h"
#include "HTNet.h"
#include "HTNetMan.h"
#include "HTHstMan.h"

#include "HTSSL.h"
#include "HTSSLWriter.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
    /* ... */
};

struct _HTOutputStream {
    const HTOutputStreamClass *	isa;
    HTChannel *			ch;
    HTHost *			host;
    int				offset;
#ifdef NOT_ASCII
    char *			ascbuf;	    /* Buffer for TOASCII conversion */
#endif
    HTSSL *         		htssl;
};

/* ------------------------------------------------------------------------- */

PRIVATE int HTSSLWriter_flush (HTOutputStream * me)
{
    return HT_OK;		       /* As we don't have any output buffer */
}

PRIVATE int HTSSLWriter_free (HTOutputStream * me)
{
    HTTRACE(STREAM_TRACE, "HTSSLWriter. Freeing %p\n" _ me);
    if (me->htssl){    
        HTSSL_free(me->htssl);
        me->htssl = NULL;
    }
    return HT_OK;
}

PRIVATE int HTSSLWriter_abort (HTOutputStream * me, HTList * e)
{
    HTTRACE(STREAM_TRACE, "HTSSLWriter. Abort %p\n" _ me);
    return HT_ERROR;
}

/*	Write to SSL
**
** According to Solaris 2.3 man on write:
**
**    o	If O_NONBLOCK and O_NDELAY are clear, write() blocks
**	until the data can be accepted.
**
**    o	If O_NONBLOCK or O_NDELAY is set, write()  does  not
**	block  the  process.   If  some  data  can be written
**	without blocking the process, write() writes what  it
**	can  and returns the number of bytes written.  Other-
**	wise, if O_NONBLOCK is set, it returns - 1  and  sets
**	errno to EAGAIN or if O_NDELAY is set, it returns 0.
**
** According to SunOS 4.1.1 man on write:
**
**   +	If the descriptor is  marked  for  non-blocking  I/O
**	using  fcntl()  to  set  the FNONBLOCK or O_NONBLOCK
**	flag (defined in  <sys/fcntl.h>),  write()  requests
**	for  {PIPE_BUF}  (see  pathconf(2V))  or fewer bytes
**	either  succeed  completely  and  return  nbyte,  or
**	return -1 and set errno to EAGAIN. A write() request
**	for greater than {PIPE_BUF} bytes  either  transfers
**	what it can and returns the number of bytes written,
**	or transfers no data and returns -1 and  sets  errno
**	to  EAGAIN.  If  a  write()  request is greater than
**	{PIPE_BUF} bytes and all data previously written  to
**	the  pipe  has been read, write() transfers at least
**	{PIPE_BUF} bytes.
*/
PRIVATE int HTSSLWriter_write (HTOutputStream * me, const char * buf, int len)
{
    HTHost * host = me->host;
    SOCKET soc = HTChannel_socket(HTHost_channel(host));
    HTNet * net = HTHost_getWriteNet(host);
    int b_write;
    char * wrtp;
    int status;
    const char *limit = buf+len;

    /* If we don't have a Net object then return right away */
    if (!net) {
	HTTRACE(STREAM_TRACE, "HTSSLWriter. No Net Object %d (offset %d)\n" _ soc _ me->offset);
	return HT_ERROR;
    }

#ifdef NOT_ASCII
    if (len && !me->ascbuf) {			      /* Generate new buffer */
	const char *orig = buf;
	char *dest;
	int cnt;
	if ((me->ascbuf = (char  *) HT_MALLOC(len)) == NULL)
	    HT_OUTOFMEM("HTSSLWriter_write");
	dest = me->ascbuf;
	for (cnt=0; cnt<len; cnt++) {
	    *dest = TOASCII(*orig);
	    dest++, orig++;
	}
	wrtp = me->ascbuf;
	limit = me->ascbuf+len;
    }
#else
    if (!me->offset)
	wrtp = (char *) buf;
    else {
	wrtp = (char *) buf + me->offset;
	len -= me->offset;
	me->offset = 0;
    }
#endif

    /* Setting SSL */
    if (!me->htssl) {
        if ((me->htssl = HTSSL_new(soc)) == NULL) {
            HTRequest_addSystemError(net->request, ERR_FATAL, socerrno, NO, "SSLWRITE");
            return HT_ERROR;
        }
    }

    /* Write data to the network */
    while (wrtp < limit) {
        b_write = HTSSL_write(me->htssl, soc, wrtp, len);
	status = HTSSL_getError(me->htssl, b_write);
	HTTRACE(STREAM_TRACE, "HTSSLWriter. SSL returned %d\n" _ status);

	/* Check what we got done */
	switch (status) {

	case SSL_ERROR_NONE:

	    /* We do this unconditionally, should we check to see if we ever blocked? */
	    HTTRACEDATA(wrtp, b_write, "Writing to socket %d" _ soc);
	    HTNet_addBytesWritten(net, b_write);
	    wrtp += b_write;
	    len -= b_write;
	    HTTRACE(STREAM_TRACE, "HTSSLWriter. %d bytes written to %d\n" _ b_write _ soc);

	    /* Make progress notification */
	    {
		HTAlertCallback *cbf = HTAlert_find(HT_PROG_WRITE);
		if (cbf) {
		    int tw = HTNet_bytesWritten(net);
		    (*cbf)(net->request, HT_PROG_WRITE, HT_MSG_NULL, NULL, &tw, NULL);
		}
	    }
	    break;

	case SSL_ERROR_WANT_WRITE:
	    HTHost_register(host, net, HTEvent_WRITE);
	    me->offset = wrtp - buf;
	    HTTRACE(STREAM_TRACE, "HTSSLWriter. WOULD BLOCK %d (offset %d)\n" _ soc _ me->offset);
	    return HT_WOULD_BLOCK;

	case SSL_ERROR_WANT_READ:
	    return HTHost_read(host, net);

	case SSL_ERROR_WANT_X509_LOOKUP:
	    /* @@@ what to do here? @@@ */
	    return HT_OK;

	case SSL_ERROR_ZERO_RETURN:
	case SSL_ERROR_SSL:
	case SSL_ERROR_SYSCALL:
	    host->broken_pipe = YES;
	    HTRequest_addSystemError(net->request, ERR_FATAL, socerrno, NO, "SSLWRITE");
	    /* 
	    ** Notify HTSSL (shared) object that sock connection is broken 
	    ** so that next read/write will reconnect
	    */
	    HTSSL_close(me->htssl);

 	    return HT_ERROR;
	case -1:
 	    /* This code is returned by HTSSL_getError() itself,
 	       when it's unable to access the ssl object.
 	       So just exiting here is the only thing to do.
 	       If we don't, we get an eternal loop
 	    */
	    return HT_ERROR;
	}
    }
#ifdef NOT_ASCII
    HT_FREE(me->ascbuf);
#endif
    return HT_OK;
}

/*	Character handling
**	------------------
*/
PRIVATE int HTSSLWriter_put_character (HTOutputStream * me, char c)
{
    return HTSSLWriter_write(me, &c, 1);
}

/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE int HTSSLWriter_put_string (HTOutputStream * me, const char * s)
{
    return HTSSLWriter_write(me, s, (int) strlen(s));
}
/*
**	The difference between the close and the free method is that we don't
**	close the connection in the free method - we only call the free method
**	of the target stream. That way, we can keep the output stream as long 
**	as the channel itself.
*/
PRIVATE int HTSSLWriter_close (HTOutputStream * me)
{
    HTTRACE(STREAM_TRACE, "HTSSLWriter. FREEING....\n");
    if (me->htssl) {
	HTSSL_free(me->htssl);
	me->htssl = NULL;
    }
    HT_FREE(me);
    return HT_OK;
}

PRIVATE const HTOutputStreamClass HTSSLWriter =
{
    "SocketWriter",
    HTSSLWriter_flush,
    HTSSLWriter_free,
    HTSSLWriter_abort,
    HTSSLWriter_put_character,
    HTSSLWriter_put_string,
    HTSSLWriter_write,
    HTSSLWriter_close
}; 


PUBLIC HTOutputStream * HTSSLWriter_new (HTHost * host, HTChannel * ch,
					 void * param, int mode)
{
    if (host && ch) {
        HTOutputStream * me = HTChannel_output(ch);
        if (!me) {
            if ((me=(HTOutputStream *) HT_CALLOC(1, sizeof(HTOutputStream)))==NULL)
		HT_OUTOFMEM("HTSSLWriter_new");
            me->isa = &HTSSLWriter;
            me->ch = ch;
            me->host = host;
            me->htssl = NULL;
        }
	HTTRACE(STREAM_TRACE, "HTSSLWriter. Created %p\n" _ me);
        return HTBufferConverter_new(host, ch, param, mode, me);
    }
    return NULL;
}
