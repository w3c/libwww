/*							              HTWrite.c
**	FILE WRITER BASED ON A SOCKET
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This is a try with a non-buffered output stream which remembers
**	state using the write_pointer. As normally we have a big buffer
**	somewhere else in the stream chain an extra output buffer will often
**	not be needed. There is also a small buffer stream that can be used
**	if athis is not the case.
*/

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "HTReq.h"
#include "HTNetMan.h"
#include "HTConLen.h"
#include "HTAlert.h"
#include "HTWriter.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
    /* ... */
};

struct _HTOutputStream {
    const HTOutputStreamClass *	isa;
    HTChannel *			ch;
    HTNet *			net;
    char *			write;
#ifdef NOT_ASCII
    BOOL			make_ascii;    /* Are we writing to the net? */
    char *			ascbuf;	    /* Buffer for TOASCII conversion */
#endif
    BOOL			leave_open;
};

/* ------------------------------------------------------------------------- */

PRIVATE int HTWriter_flush (HTOutputStream * me)
{
    return HT_OK;		       /* As we don't have any output buffer */
}

PRIVATE int HTWriter_free (HTOutputStream * me)
{
    return HT_OK;
}

PRIVATE int HTWriter_abort (HTOutputStream * me, HTList * e)
{
    return HT_ERROR;
}

/*	Write to the socket
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
PRIVATE int HTWriter_write (HTOutputStream * me, const char * buf, int len)
{
    HTNet * net = me->net;
    SOCKET soc = net->sockfd;
    int b_write;
    const char *limit = buf+len;

#ifdef NOT_ASCII
    if (me->make_ascii && len && !me->ascbuf) {	      /* Generate new buffer */
	char *orig=buf;
	char *dest;
	int cnt;
	if ((me->ascbuf = (char  *) HT_MALLOC(len)) == NULL)
	    HT_OUTOFMEM("me->ascbuf ");
	dest = me->ascbuf;
	for (cnt=0; cnt<len; cnt++) {
	    *dest = TOASCII(*orig);
	    dest++, orig++;
	}
	me->write = me->ascbuf;
	limit = me->ascbuf+len;
    }
#else
    if (!me->write)
	me->write = (char *) buf;
    else
	len -= (me->write - buf);
#endif

    /* Write data to the network */
    while (me->write < limit) {
	if ((b_write = NETWRITE(soc, me->write, len)) < 0) {

#ifdef EAGAIN
	    if (socerrno == EAGAIN || socerrno == EWOULDBLOCK)/* POSIX, SVR4 */
#else
	    if (socerrno == EWOULDBLOCK)			      /* BSD */
#endif
	    {
		if (PROT_TRACE)
		    HTTrace("Write Socket WOULD BLOCK %d\n",soc);
		HTEvent_Register(soc, net->request, (SockOps) FD_WRITE,
				 net->cbf, net->priority);
		return HT_WOULD_BLOCK;
	    } else {
		HTRequest_addSystemError(net->request, ERR_FATAL, socerrno, NO,
					 "NETWRITE");
		return HT_ERROR;
	    }
	}
	HTEvent_UnRegister(soc, (SockOps) FD_WRITE);
	me->write += b_write;
	len -= b_write;
	if (PROT_TRACE)
	    HTTrace("Write Socket %d bytes written to socket %d\n",
		    b_write, soc);
	net->bytes_written += b_write;
	{
	    HTAlertCallback *cbf = HTAlert_find(HT_PROG_READ);
	    if (cbf) (*cbf)(net->request, HT_PROG_WRITE,
			    HT_MSG_NULL, NULL, NULL, NULL);
	}
    }
#ifdef NOT_ASCII
    HT_FREE(me->ascbuf);
#else
    me->write = NULL;
#endif
    return HT_OK;
}

/*	Character handling
**	------------------
*/
PRIVATE int HTWriter_put_character (HTOutputStream * me, char c)
{
    return HTWriter_write(me, &c, 1);
}

/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE int HTWriter_put_string (HTOutputStream * me, const char * s)
{
    return HTWriter_write(me, s, (int) strlen(s));
}
/*
**	The difference between the close and the free method is that we don't
**	close the connection in the free method - we only call the free method
**	of the target stream. That way, we can keep the output stream as long 
**	as the channel itself.
*/
PRIVATE int HTWriter_close (HTOutputStream * me)
{
    if (PROT_TRACE) HTTrace("Socket write FREEING....\n");
    HT_FREE(me);
    return HT_OK;
}

PRIVATE const HTOutputStreamClass HTWriter =
{		
    "SocketWriter",
    HTWriter_flush,
    HTWriter_free,
    HTWriter_abort,
    HTWriter_put_character,
    HTWriter_put_string,
    HTWriter_write,
    HTWriter_close
}; 

PUBLIC HTOutputStream * HTWriter_new (HTNet * net, HTChannel * ch,
				      void * param, int mode)
{
    if (net && ch) {
	HTOutputStream * me = HTChannel_output(ch);
	if (me == NULL) {
	    if ((me=(HTOutputStream *) HT_CALLOC(1, sizeof(HTOutputStream)))==NULL)
		HT_OUTOFMEM("HTWriter_new");
	    me->isa = &HTWriter;
	    me->ch = ch;
	}
	me->net = net;
	return me;
    }
    return NULL;
}

/*
**	Set up stream to write to a socket. If buf_size > 0 then we set up
**	buffered output used for at most buf_size bytes. From that point we 
**	switch to unbuffered mode. Otherwise we'll use nonbuffered output.
*/
PUBLIC HTStream* HTBufWriter_new (HTNet *net, BOOL leave_open, int buf_size)
{
#if 0
    return HTBuffer_new(HTWriter_new(net, leave_open), net->request, buf_size);
#else
    return HTErrorStream();
#endif
}


/*	Subclass-specific Methods
**	-------------------------
*/
#ifdef NOT_ASCII
PUBLIC HTStream * HTASCIIWriter (HTNet *net, BOOL leave_open)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(*me))) == NULL)
        HT_OUTOFMEM("HTASCIIWriter_new");
    me->isa = &HTWriter;       
    me->leave_open = leave_open;
    me->make_ascii = YES;
    me->sockfd = net->sockfd;
    me->net = net;
    return me;
}
#else
#ifdef WWW_WIN_DLL
PUBLIC HTStream * HTASCIIWriter (HTNet *net, BOOL leave_open)
{
    return NULL;
}
#endif /* WWW_WIN_DLL */
#endif /* NOT_ASCII */
