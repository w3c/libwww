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
#include "HTBufWrt.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
    /* ... */
};

struct _HTOutputStream {
    const HTOutputStreamClass *	isa;
    HTOutputStream *		target;		 /* Target for outgoing data */
    HTChannel *			ch;
    HTNet *			net;
    int				size;			      /* Buffer size */
    int				bb;
    char *			block;
    char *			read;		       /* Position in 'data' */
    char *			data;				   /* buffer */
};

#define PUTBLOCK(b,l) (*me->target->isa->put_block)(me->target,(b),(l))

/* ------------------------------------------------------------------------- */

PRIVATE int HTBufferWriter_flush (HTOutputStream * me)
{
    int status = HT_OK;
    if (me->read > me->data) {
	if ((status = PUTBLOCK(me->data, me->read - me->data))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	me->read = me->data;
	me->block = NULL;
    }
    return status;
}

PRIVATE int HTBufferWriter_free (HTOutputStream * me)
{
    return HTBufferWriter_flush(me);
}

PRIVATE int HTBufferWriter_abort (HTOutputStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    if (PROT_TRACE) HTTrace("Buffer...... ABORTING...\n");
    return HT_ERROR;
}

PRIVATE int HTBufferWriter_write (HTOutputStream *me, const char *buf, int len)
{
    int status;
    if (me->bb > 0) {
	len -= (me->block - buf);
	if ((status = PUTBLOCK(me->block, me->bb)) != HT_OK) return status;
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
	}
	if ((status = PUTBLOCK(me->data, me->size)) != HT_OK) return status;

	/* If more data then write n times buffer size */
	if (!me->block)
	    me->block = (char *) buf;
	else {
	    len -= (me->block - buf);
	}
	me->bb = len - len%me->size;
	if ((status = PUTBLOCK(me->block, me->bb)) != HT_OK) return status;
	me->block += me->bb;
	len -= me->bb;
	me->bb = 0;
    }

    /* If data is not aligned then save the rest in our buffer */
    if (len > 0) {
	memcpy(me->data, me->block, len);
	me->read = me->data + len;
    } else
	me->read = me->data;
    me->block = NULL;
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
	HTBufferWriter_flush(me);
	if (me->target) (*me->target->isa->close)(me->target);
	HT_FREE(me->data);
	HT_FREE(me);
    }
    return HT_OK;
}

PRIVATE const HTOutputStreamClass HTBufferWriter =
{		
    "SocketWriter",
    HTBufferWriter_flush,
    HTBufferWriter_free,
    HTBufferWriter_abort,
    HTBufferWriter_put_character,
    HTBufferWriter_put_string,
    HTBufferWriter_write,
    HTBufferWriter_close
}; 

PUBLIC HTOutputStream * HTBufferWriter_new (HTNet * net, HTChannel * ch,
					    void * param, int bufsize)
{
    if (net && ch) {
	HTOutputStream * me = HTChannel_output(ch);
	if (me == NULL) {
	    if (bufsize <= 0) bufsize = OUTPUT_BUFFER_SIZE;
	    if ((me = (HTOutputStream *) HT_CALLOC(1, sizeof(HTOutputStream))) == NULL ||
		(me->data = (char *) HT_MALLOC(bufsize)) == NULL)
		HT_OUTOFMEM("HTBufferWriter_new");
	    me->isa = &HTBufferWriter;
	    me->read = me->data;
	    me->size = bufsize;
	    me->target = HTWriter_new(net, ch, param, 0);
	    me->ch = ch;
	} else
	    HTWriter_set(me->target, net, ch, param, 0);
	me->net = net;
	return me;
    }
    return NULL;
}
