/*							              HTWrite.c
**	FILE WRITER BASED ON A SOCKFD
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This is a try with a non-buffered output stream which remembers
**	state using the write_pointer. As normally we have a big buffer
**	somewhere else in the stream chain an extra output buffer will often
**	not be needed.
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTThread.h"
#include "HTWriter.h"					 /* Implemented here */

struct _HTStream {
	CONST HTStreamClass *	isa;

	SOCKFD	soc;
	char	*write_pointer;
	BOOL	leave_open;
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
PRIVATE int HTWriter_write ARGS3(HTStream *, me, CONST char *, buf, int, len)
{
    int b_write;
    CONST char *limit = buf+len;
    if (HTThreadIntr(me->soc))
	return HT_INTERRUPTED;

#ifdef NOT_ASCII
    if (me->make_ascii && len && !me->ascbuf) {	      /* Generate new buffer */
	char *orig=buf;
	char *dest;
	int cnt;
	me->ascbuf = (char *) malloc(len);
	dest = me->ascbuf;
	for (cnt=0; cnt<len; cnt++)
	    *dest++ = TOASCII(*orig++);
	me->write_pointer = me->ascbuf;
	limit = me->ascbuf+len;
    }
#else
    if (!me->write_pointer)
	me->write_pointer = (char *) buf;
    else
	len -= (me->write_pointer - buf);
#endif

    /* Write data to the network */
    while (me->write_pointer < limit) {
	if ((b_write = NETWRITE(me->soc, me->write_pointer, len)) < 0) {

#ifdef EAGAIN
	    if (errno == EAGAIN || errno == EWOULDBLOCK)      /* POSIX, SVR4 */
#else
	    if (errno == EWOULDBLOCK)				      /* BSD */
#endif
	    {
		if (PROT_TRACE)
		    fprintf(TDEST, "Write Socket WOULD BLOCK %d\n", me->soc);
		HTThreadState(me->soc, THD_SET_WRITE);
		return HT_WOULD_BLOCK;
	    } else {
		if (PROT_TRACE)
		    fprintf(TDEST, "Write Socket WRITE ERROR %d\n", errno);
		return HT_ERROR;
	    }
	}
	HTThreadState(me->soc, THD_CLR_WRITE);
	me->write_pointer += b_write;
	len -= b_write;
	if (PROT_TRACE)
	    fprintf(TDEST, "Write Socket %d bytes written to socket %d\n",
		    b_write, me->soc);
    }
#ifdef NOT_ASCII
    FREE(me->ascbuf);
#else
    me->write_pointer = NULL;
#endif
    return HT_OK;
}

/*	Character handling
**	------------------
*/
PRIVATE int HTWriter_put_character ARGS2(HTStream *, me, char, c)
{
    return HTWriter_write(me, &c, 1);
}


/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE int HTWriter_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    return HTWriter_write(me, s, (int) strlen(s));
}

PRIVATE int HTWriter_flush ARGS1(HTStream *, me)
{
    return HT_OK;	       /* As we don't keep any buffer in this stream */
}

PRIVATE int HTWriter_free ARGS1(HTStream *, me)
{
    int status = HT_OK;
    if (!me->leave_open) {
	if (NETCLOSE(me->soc) < 0)
	    status = HT_ERROR;
    }
    free(me);
    return status;
}

PRIVATE int HTWriter_abort ARGS2(HTStream *, me, HTError, e)
{
    if (!me->leave_open)
	NETCLOSE(me->soc);
    free(me);
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
PUBLIC HTStream* HTWriter_new ARGS2(SOCKFD, soc, BOOL, leave_open)
{
    HTStream* me = (HTStream *) calloc(1, sizeof(*me));
    if (me == NULL) outofmem(__FILE__, "HTWriter_new");
    me->isa = &HTWriter;       
    me->leave_open = leave_open;
    me->soc = soc;
    return me;
}

/*	Subclass-specific Methods
**	-------------------------
*/
#ifdef NOT_ASCII
PUBLIC HTStream* HTASCIIWriter ARGS2(SOCKFD, soc, BOOL, leave_open)
{
    HTStream* me = (HTStream *) calloc(1, sizeof(*me));
    if (me == NULL) outofmem(__FILE__, "HTASCIIWriter_new");
    me->isa = &HTWriter;       
    me->leave_open = leave_open;
    me->make_ascii = YES;
    me->soc = soc;
    return me;
}
#endif
