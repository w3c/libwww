/*							              HTWrite.c
**	FILE WRITER BASED ON A SOCKET
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This is a try with a non-buffered output stream which remembers
**	state using the write_pointer. As normally we have a big buffer
**	somewhere else in the stream chain an extra output buffer will often
**	not be needed. There is also a small buffer stream that can be used
**	if athis is not the case.
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTReq.h"
#include "HTNetMan.h"
#include "HTConLen.h"
#include "HTAlert.h"
#include "HTWriter.h"					 /* Implemented here */

struct _HTStream {
	CONST HTStreamClass *	isa;
	SOCKET			sockfd;
	HTNet *			net;
	char			*wptr;
	BOOL			leave_open;
#ifdef NOT_ASCII
	BOOL			make_ascii;    /* Are we writing to the net? */
	char *			ascbuf;	    /* Buffer for TOASCII conversion */
#endif
};

/* ------------------------------------------------------------------------- */

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
PRIVATE int HTWriter_write (HTStream * me, CONST char * buf, int len)
{
    int b_write;
    CONST char *limit = buf+len;
    HTRequest * request = me->net->request;
    HTNet * net = me->net;

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
	me->wptr = me->ascbuf;
	limit = me->ascbuf+len;
    }
#else
    if (!me->wptr)
	me->wptr = (char *) buf;
    else
	len -= (me->wptr - buf);
#endif

    /* Write data to the network */
    while (me->wptr < limit) {
	if ((b_write = NETWRITE(me->sockfd, me->wptr, len)) < 0) {

#ifdef EAGAIN
	    if (socerrno == EAGAIN || socerrno == EWOULDBLOCK)/* POSIX, SVR4 */
#else
	    if (socerrno == EWOULDBLOCK)			      /* BSD */
#endif
	    {
		if (PROT_TRACE)
		    TTYPrint(TDEST,"Write Socket WOULD BLOCK %d\n",me->sockfd);
		HTEvent_Register(me->sockfd, request, (SockOps) FD_WRITE,
				 net->cbf, net->priority);
		return HT_WOULD_BLOCK;
	    } else {
		HTRequest_addSystemError(request,  ERR_FATAL, socerrno, NO,
					 "NETWRITE");
		return HT_ERROR;
	    }
	}
	HTEvent_UnRegister(me->sockfd, (SockOps) FD_WRITE);
	me->wptr += b_write;
	len -= b_write;
	if (PROT_TRACE)
	    TTYPrint(TDEST, "Write Socket %d bytes written to socket %d\n",
		    b_write, me->sockfd);
	net->bytes_written += b_write;
	{
	    HTAlertCallback *cbf = HTAlert_find(HT_PROG_READ);
	    if (cbf) (*cbf)(request, HT_PROG_WRITE,HT_MSG_NULL,NULL,NULL,NULL);
	}
    }
#ifdef NOT_ASCII
    HT_FREE(me->ascbuf);
#else
    me->wptr = NULL;
#endif
    return HT_OK;
}

/*	Character handling
**	------------------
*/
PRIVATE int HTWriter_put_character (HTStream * me, char c)
{
    return HTWriter_write(me, &c, 1);
}

/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE int HTWriter_put_string (HTStream * me, CONST char * s)
{
    return HTWriter_write(me, s, (int) strlen(s));
}

PRIVATE int HTWriter_flush (HTStream * me)
{
    return HT_OK;	       /* As we don't keep any buffer in this stream */
}

PRIVATE int HTWriter_free (HTStream * me)
{
    int status = HT_OK;
    if (!me->leave_open) {
	if (NETCLOSE(me->sockfd) < 0)
	    status = HT_ERROR;
    }
    HT_FREE(me);
    return status;
}

PRIVATE int HTWriter_abort (HTStream * me, HTList * e)
{
    if (!me->leave_open)
	NETCLOSE(me->sockfd);
    HT_FREE(me);
    return HT_ERROR;
}


/*	Structured Object Class
**	-----------------------
*/
PRIVATE CONST HTStreamClass HTWriter =
{		
    "SocketWriter",
    HTWriter_flush,
    HTWriter_free,
    HTWriter_abort,
    HTWriter_put_character,
    HTWriter_put_string,
    HTWriter_write
}; 


/*	Subclass-specific Methods
**	-------------------------
*/
PUBLIC HTStream* HTWriter_new (HTNet *net, BOOL leave_open)
{
    HTStream* me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTWriter_new");
    me->isa = &HTWriter;       
    me->leave_open = leave_open;
    me->sockfd = net->sockfd;
    me->net = net;
    return me;
}

/*
**	Set up stream to write to a socket. If buf_size > 0 then we set up
**	buffered output used for at most buf_size bytes. From that point we 
**	switch to unbuffered mode. Otherwise we'll use nonbuffered output.
*/
PUBLIC HTStream* HTBufWriter_new (HTNet *net, BOOL leave_open, int buf_size)
{
    return HTBuffer_new(HTWriter_new(net, leave_open), net->request, buf_size);
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
