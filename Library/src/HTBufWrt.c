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
#include "sysdep.h"
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
    int				size;			      /* Buffer size */
    int				bb;
    char *			block;
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
    int status = HT_ERROR;
    if (me && me->read > me->data) {
	if ((status = PUTBLOCK(me->data, me->read - me->data))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	me->lastFlushTime = HTGetTimeInMillis();
	me->read = me->data;
	me->block = NULL;
    }
    return status;
}

PRIVATE int FlushEvent (HTTimer * timer, void * param, HTEventType type)
{
    HTOutputStream * me = (HTOutputStream *) param;
    if (timer != me->timer) HTDebugBreak();
    if (PROT_TRACE) HTTrace("Buffer...... Timeout Flushing...\n");

    /*
    **  We ignore the return code here which we shouldn't!!!
    */
    HTBufferWriter_flush(me);

    /*
    **  Delete the timer
    */
    me->timer = NULL;
    return HT_OK;
}

PRIVATE int HTBufferWriter_lazyFlush (HTOutputStream * me)
{
    HTNet * net;
    int delay;

    if (me->read <= me->data)
	return HT_OK;			/* nothing to flush */
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
	if (PROT_TRACE) HTTrace("Buffer...... Flushing\n");
	if ((status = HTBufferWriter_flush(me)) && me->timer) {
	    HTTimer_delete(me->timer);
	    me->timer = NULL;
	}
	return status;
    }

    /*
    **	Set a timer and tell the host we've done the write if
    **  we have not already started a timer earlier.
    */
    if (!me->timer) {
	ms_t exp = HTGetTimeInMillis() + delay;
	net = HTHost_getWriteNet(me->host);
	me->timer = HTTimer_new(NULL, FlushEvent, me, exp, NO);
	HTHost_unregister(me->host, net, HTEvent_WRITE);
	if (PROT_TRACE) HTTrace("Buffer...... Waiting...\n");
    }
    return HT_OK;
}

PRIVATE int HTBufferWriter_free (HTOutputStream * me)
{
    return HTBufferWriter_lazyFlush(me);
}

PRIVATE int HTBufferWriter_abort (HTOutputStream * me, HTList * e)
{
    if (PROT_TRACE) HTTrace("Buffer...... ABORTING...\n");
    if (me->target) (*me->target->isa->abort)(me->target, e);
    if (me->timer) {
	HTTimer_delete(me->timer);
	me->timer = NULL;
    }
    return HT_ERROR;
}

PRIVATE int HTBufferWriter_write (HTOutputStream * me, const char * buf, int len)
{
    HTNet * net = HTHost_getWriteNet(me->host);
    long total = len;
    int status;

    if (me->bb > 0) {
	len -= (me->block - buf);
	if ((status = PUTBLOCK(me->block, me->bb)) != HT_OK) return status;
	me->lastFlushTime = HTGetTimeInMillis();
	me->block += me->bb;
	len -= me->bb;
	me->bb = 0;
    } else {
	int available = me->data + me->size - me->read;

	/* Still room in buffer */
	if (len <= available) {
	    memcpy(me->read, buf, len);
	    me->read += len;
	    return HT_OK;
	}

	/* If already data in buffer then fill it and flush */
	if (me->read > me->data) {
	    memcpy(me->read, buf, available);
	    me->block = (char *) buf+available;
	    if ((status = PUTBLOCK(me->data, me->size))!=HT_OK) return status;
	    me->lastFlushTime = HTGetTimeInMillis();
	}

	/* If more data then write n times buffer size */
	if (!me->block)
	    me->block = (char *) buf;
	else {
	    len -= (me->block - buf);
	}
#if 0
	me->bb = len - len%me->size;
#else
	me->bb = (len >= me->size) ? len : len - len%me->size;
#endif
	if (me->bb) {
	    if ((status = PUTBLOCK(me->block, me->bb)) != HT_OK) return status;
	    me->lastFlushTime = HTGetTimeInMillis();
	}
	me->block += me->bb;
	len -= me->bb;
	me->bb = 0;
    }

    /* If data is not aligned then save the rest in our buffer */
    if (len > 0) {
	memcpy(me->data, me->block, len);
	me->read = me->data + len;
	if (PROT_TRACE) HTTrace("Buffer...... Carrying %d bytes over...\n", len);
#if 0
	if ((status = HTBufferWriter_lazyFlush(me)) != HT_OK) return status;
#endif
    } else
	me->read = me->data;
    me->block = NULL;
    HTNet_addBytesWritten(net, total);
    return HT_OK;
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
#if 0
	/*
	** The channel has been closed so we shouldn't do any more on
	** it at all
	*/
	HTBufferWriter_flush(me);
#endif
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

PUBLIC HTOutputStream * HTBufferWriter_new (HTHost * host, HTChannel * ch,
					    void * param, int bufsize)
{
    if (host && ch) {
	HTOutputStream * me = HTChannel_output(ch);
	if (!me) {
	    HTOutputStream * me;
	    if (bufsize <= 0) bufsize = OUTPUT_BUFFER_SIZE;
	    if ((me = (HTOutputStream *) HT_CALLOC(1, sizeof(HTOutputStream)))==NULL ||
		(me->data = (char *) HT_MALLOC(bufsize)) == NULL)
		HT_OUTOFMEM("HTBufferWriter_new");
	    me->isa = &HTBufferWriter;
	    me->read = me->data;
	    me->size = bufsize;
	    me->target = HTWriter_new(host, ch, param, 0);
	    me->host = host;
	    return me;
	}
    }
    return NULL;
}
