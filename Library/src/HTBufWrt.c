/*
**	BUFFERED TRANSPORT WRITER STREAM
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	A buffered output stream. This stream lets you write characters to a
**	stream without causing a write every time.  The data is first written
**	into a buffer. Data is written to the actual stream only when the
**	buffer is full, or when the stream is flushed.
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTNetMan.h"
#include "HTWriter.h"
#include "HTTimer.h"
#include "HTBufWrt.h"					 /* Implemented here */

struct _HTOutputStream {
    const HTOutputStreamClass *	isa;
    HTOutputStream *		target;		 /* Target for outgoing data */
    HTHost *			host;

    int				allocated;  	    /* Allocated Buffer size */
    int                         growby;
    int				expo;

    char *			read;		       /* Position in 'data' */
    char *			data;				   /* buffer */

    ms_t			lastFlushTime;	/* polar coordinates of the moon */
    HTTimer *			timer;
};

#define PUTBLOCK(b,l) (*me->target->isa->put_block)(me->target,(b),(l))

/* ------------------------------------------------------------------------- */

/*
**  This function is only called from either FlushEvent or HTBufferWriter_lazyFlush
**  which means that only the host object or timeout can cause a flush
*/
PRIVATE int HTBufferWriter_flush (HTOutputStream * me)
{
    int status = HT_OK;
    if (me && me->read > me->data) {
	me->lastFlushTime = HTGetTimeInMillis();
        if ((status = PUTBLOCK(me->data, me->read - me->data))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	me->read = me->data;
    }
    return status;
}

PRIVATE int FlushEvent (HTTimer * timer, void * param, HTEventType type)
{
    HTOutputStream * me = (HTOutputStream *) param;
    if (me->timer && timer != me->timer)
	HTDEBUGBREAK("Buffer Writer timer %p not in sync\n" _ timer);
    HTTRACE(PROT_TRACE, "Buffer...... Timeout flushing %p with timer %p\n" _ me _ timer);

    /*
    **  We ignore the return code here which we shouldn't!!!
    */
    HTBufferWriter_flush(me);

    /*
    **  Delete the timer
    */
    HTTimer_delete(me->timer);
    me->timer = NULL;
    return HT_OK;
}

PRIVATE int HTBufferWriter_lazyFlush (HTOutputStream * me)
{
    HTNet * net;
    int delay;

    if (me->read <= me->data) {
	return HT_OK;			/* nothing to flush */
    }
    /*
    **  If we are allowed to delay the flush then set a timer with the
    **  delay descibed by our delay variable. If we can't delay then flush 
    **  right away.
    */
    delay = HTHost_findWriteDelay(me->host, me->lastFlushTime, me->read - me->data);

    /*
    **	Flush immediately
    */
    if (!delay) {
	int status;
	HTTRACE(STREAM_TRACE, "Buffer...... Flushing %p\n" _ me);
	if ((status = HTBufferWriter_flush(me)) && me->timer) {
	    HTTimer_delete(me->timer);
	    me->timer = NULL;
	}
	return status;
    }

    /*
    **	Set a timer and tell the host we've done the write if
    **  we have not already started a timer earlier. If a timer
    **  does already exist then make sure that it hasn't expired.
    **  This can be the case if we have a really slow client that
    **  can't parse the data fast enough.
    */
    if (!me->timer) {
	net = HTHost_getWriteNet(me->host);
	me->timer = HTTimer_new(NULL, FlushEvent, me, delay, YES, NO);
	HTHost_unregister(me->host, net, HTEvent_WRITE);
	HTTRACE(STREAM_TRACE, "Buffer...... Waiting %dms on %p\n" _ delay _ me);
    } else {
	if (HTTimer_hasTimerExpired(me->timer)) {
	    HTTRACE(STREAM_TRACE, "Buffer...... Dispatching old timer %p\n" _ me->timer);
	    HTTimer_dispatch(me->timer);
	    me->timer = NULL;
	} else {
	    HTTRACE(STREAM_TRACE, "Buffer...... Waiting on unexpired timer %p\n" _ me->timer);
	}
    }
    return HT_OK;
}

PRIVATE int HTBufferWriter_free (HTOutputStream * me)
{
    return HTBufferWriter_lazyFlush(me);
}

PRIVATE BOOL HTBufferWriter_addBuffer(HTOutputStream * me, int addthis)
{
    if (me) {
        me->allocated += (addthis - addthis%me->growby + (me->growby*me->expo));
	me->expo *= 2;
	HTTRACE(STREAM_TRACE, "Buffer...... Increasing buffer to %d bytes\n" _ me->allocated);
        if (me->data) {
            int size = me->read-me->data;
            if ((me->data = (char *) HT_REALLOC(me->data, me->allocated)) == NULL)
                HT_OUTOFMEM("HTBufferWriter_addBuffer");
            me->read = me->data + size;
        } else {
            if ((me->data = (char *) HT_CALLOC(1, me->allocated)) == NULL)
                HT_OUTOFMEM("HTBufferWriter_addBuffer");
            me->read = me->data;
        }
       return YES;
    }
    return NO;
}

PRIVATE int HTBufferWriter_abort (HTOutputStream * me, HTList * e)
{
    HTTRACE(STREAM_TRACE, "Buffer...... ABORTING...\n");
    if (me->timer) {
	HTTimer_delete(me->timer);
	me->timer = NULL;
    }
    if (me->target) (*me->target->isa->abort)(me->target, e);
    return HT_ERROR;
}

PRIVATE int HTBufferWriter_write (HTOutputStream * me, const char * buf, int len)
{
    int status;
    while (1) {
	int available = me->data + me->allocated - me->read;

	/* If we have enough buffer space */
	if (len <= available) {
	    int size = 0;
	    memcpy(me->read, buf, len);
	    me->read += len;
        
	    /* If we have accumulated enough data then flush */
	    if ((size = me->read - me->data) > me->growby) {
		me->lastFlushTime = HTGetTimeInMillis();
		status = PUTBLOCK(me->data, size);
		if (status == HT_OK) {
		    me->read = me->data;
		} else {
		    return (status == HT_WOULD_BLOCK) ? HT_OK : HT_ERROR;
		}
	    }
	    return HT_OK;
	} else {

	    /* Fill the existing buffer (if not already) and flush */
	    if (available) {
		memcpy(me->read, buf, available);
		buf += available;
		len -= available;
		me->read += available;
	    }
	    me->lastFlushTime = HTGetTimeInMillis();
	    status = PUTBLOCK(me->data, me->allocated);
	    if (status == HT_OK) {
		me->read = me->data;
	    } else if (status == HT_WOULD_BLOCK) {
		HTBufferWriter_addBuffer(me, len);
		memcpy(me->read, buf, len);
		me->read += len;
		return HT_OK;
	    }
	}
    }
}

/*	Character handling
**	------------------
*/
PRIVATE int HTBufferWriter_put_character (HTOutputStream * me, char c)
{
    return HTBufferWriter_write(me, &c, 1);
}

/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE int HTBufferWriter_put_string (HTOutputStream * me, const char * s)
{
    return HTBufferWriter_write(me, s, (int) strlen(s));
}
/*
**	The difference between the close and the free method is that we don't
**	close the connection in the free method - we only call the free method
**	of the target stream. That way, we can keep the output stream as long 
**	as the channel itself.
*/
PRIVATE int HTBufferWriter_close (HTOutputStream * me)
{
    if (me) {
	if (me->timer) {
	    HTTimer_delete(me->timer);
	    me->timer = NULL;
	}
	if (me->target) (*me->target->isa->close)(me->target);
	HT_FREE(me->data);
	HT_FREE(me);
    }
    return HT_OK;
}

PRIVATE const HTOutputStreamClass HTBufferWriter =
{		
    "BufferedSocketWriter",
    HTBufferWriter_lazyFlush,
    HTBufferWriter_free,
    HTBufferWriter_abort,
    HTBufferWriter_put_character,
    HTBufferWriter_put_string,
    HTBufferWriter_write,
    HTBufferWriter_close
}; 

PRIVATE HTOutputStream * buffer_new (HTHost * host, HTChannel * ch,
				     void * param, int bufsize)
{
    if (host && ch) {
	HTOutputStream * me = HTChannel_output(ch);
	if (!me) {
	    HTOutputStream * me;
	    int tcpbufsize = 0;

#if defined(HAVE_GETSOCKOPT) && defined(SO_SNDBUF)
	    /*
	    ** Get the TCP socket buffer size
	    */
	    {
		SOCKET sockfd = HTChannel_socket(ch);
		int size = sizeof(int);
		int status = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF,
					(void *) &tcpbufsize, &size);
		if (status == -1) {
		    HTTRACE(STREAM_TRACE, "Socket...... Could not get TCP send buffer size for socket %d\n" _ sockfd);
		} else {
		    HTTRACE(STREAM_TRACE, "Socket...... TCP send buffer size is %d for socket %d\n" _ tcpbufsize _ sockfd);
		}		
	    }
#endif
	    if (bufsize <= 0) bufsize = tcpbufsize ? tcpbufsize : OUTPUT_BUFFER_SIZE;
	    if ((me = (HTOutputStream *) HT_CALLOC(1, sizeof(HTOutputStream)))==NULL ||
		(me->data = (char *) HT_MALLOC(bufsize)) == NULL)
		HT_OUTOFMEM("HTBufferWriter_new");
	    me->isa = &HTBufferWriter;
	    me->read = me->data;
	    me->allocated = bufsize;
            me->growby = bufsize;
	    me->expo = 1;
	    me->host = host;
           return me;
	}
    }
    return NULL;
}

PUBLIC HTOutputStream * HTBufferWriter_new (HTHost *	       	host,
					    HTChannel * 	ch,
					    void * 		param,
					    int 		bufsize)
{
    HTOutputStream * me = buffer_new(host, ch, param, bufsize);
    if (me) {
	me->target = HTWriter_new(host, ch, param, 0);
	return me;
    }
    return NULL;
}

PUBLIC HTOutputStream * HTBufferConverter_new (HTHost * 	host,
					       HTChannel * 	ch,
					       void * 		param,
					       int 		bufsize,
					       HTOutputStream * target)
{
    if (target) {
	HTOutputStream * me = buffer_new(host, ch, param, bufsize);
	if (me) {
	    me->target = target;
	    return me;
	}
    }
    return NULL;
}
