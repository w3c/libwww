/*							             HTConlen.c
**	CONTENT LENGTH COUNTER STREAM
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This stream counts the number of bytes in a stream until it reaches
**	max number of bytes it can occupy. If this happens then	it gives up.
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTConLen.h"					 /* Implemented here */

#define HT_MIN_BLOCK 	0x100
#define HT_MAX_BLOCK	0x2000
#define HT_MAX_SIZE	0x10000
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target,(b),(l))

typedef struct _HTBufItem {
    int			len;
    char *		buf;
    struct _HTBufItem *	next;
} HTBufItem;

typedef enum _BufferMode {
    HT_BM_PLAIN		= 0x0,
    HT_BM_DELAY		= 0x1,		/* Buffer full and we pause */
    HT_BM_COUNT		= 0x2,		/* Content length counter */
    HT_BM_PIPE		= 0x4		/* Pipe buffer */
} BufferMode;

typedef enum _BufferState {
    HT_BS_OK		= 0,
    HT_BS_PAUSE		= 1,
    HT_BS_TRANSPARENT	= 2
} BufferState;

struct _HTStream {
    HTStreamClass *	isa;
    HTRequest *		request;
    HTStream *		target;

    char *		tmp_buf;
    int			tmp_ind;
    int			tmp_max;
    HTBufItem *		head;
    HTBufItem *		tail;

    int			max_size;
    int			cur_size;
    int			conlen;
    BufferMode		mode;			   /* State of the buffering */
    BufferState		state;
};

/* ------------------------------------------------------------------------- */

/*
**	MIME output with content-length calculation
**	-------------------------------------------
**	This stream also buffers the result to find out the content length.
**	If a maximum buffer limit is reached Content-Length is calculated
**	for logs but it is not sent to the client -- rather the buffer is
**	flushed right away.
**	Code taken from HTRequest.c written by Ari Luotonen and modified to
**	fit new stream model
*/

PRIVATE BOOL free_buf (HTBufItem * me)
{
    if (me) {
	HT_FREE(me->buf);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PRIVATE void free_buf_all (HTStream * me)
{
    HTBufItem * cur = me->head;
    HTBufItem * killme;
    me->tmp_ind = 0;
    me->tmp_max = 0;
    HT_FREE(me->tmp_buf);
    while (cur) {
	killme = cur;
	cur = cur->next;
	free_buf(cur);
    }
    me->head = me->tail = NULL;
}

PRIVATE void append_buf (HTStream * me)
{
    HTBufItem * b;
    if ((b = (HTBufItem  *) HT_CALLOC(1, sizeof(HTBufItem))) == NULL)
        HT_OUTOFMEM("append_buf");
    b->len = me->tmp_ind;
    b->buf = me->tmp_buf;
    me->tmp_ind = 0;
    me->tmp_max = 0;
    me->tmp_buf = 0;
    if (me->tail)
	me->tail->next = b;
    else
	me->head = b;
    me->tail = b;
}

PRIVATE BOOL alloc_new (HTStream * me, int size)
{
    if (me->conlen >= me->max_size) {
	HTTRACE(STREAM_TRACE, "Buffer...... size %d reached, going transparent\n" _ 
		    me->max_size);
	return NO;
    } else if (size) {
	me->tmp_ind = 0;
	me->tmp_max = size;
	if ((me->tmp_buf = (char  *) HT_MALLOC(size)) == NULL)
	    HT_OUTOFMEM("buf_put_char");
	HTTRACE(STREAM_TRACE, "Buffer...... created with len %d\n" _ size);
	return YES;
    }
    return NO;
}

/*
**	After flushing the buffer we go into transparent mode so that we still
**	can handle incoming data. If we already are in transparent mode then
**	don't do anything.
*/
PRIVATE int buf_flush (HTStream * me)
{
    if (me->state != HT_BS_TRANSPARENT) {
	HTBufItem * cur;
	if (me->tmp_buf) append_buf(me);
	while ((cur = me->head)) {
	    int status;
	    if ((status = PUTBLOCK(cur->buf, cur->len)) != HT_OK) {
		return status;
	    }
	    me->head = cur->next;
	    free_buf(cur);
	}

	/* If we are not a pipe then do no more buffering */
	if ((me->mode & HT_BM_PIPE)) me->state = HT_BS_TRANSPARENT;
    }
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int buf_put_block (HTStream * me, const char * b, int l)
{
    /*
    **  If we are in pause mode then don't write anything but return PAUSE.
    **  The upper stream should then respect it and don't write any more data.
    */
    if (me->state == HT_BS_PAUSE) return HT_PAUSE;

    /*
    **  Start handling the incoming data. If we are still buffering then add
    **  it to the buffer. Otherwise just pump it through. Note that we still
    **  count the length - even if we have given up buffering!
    */
    me->conlen += l;
    if (me->state != HT_BS_TRANSPARENT) {

	/*
	**  If there is still room in the existing chunk then fill it up.
	**  Otherwise create a new chunk and add it to the linked list of
	**  chunks. If the buffer fills up then either return HT_PAUSE or
	**  flush it and go transparent.
	*/
	if (me->tmp_buf && me->tmp_max-me->tmp_ind >= l) {     /* Still room */
	    memcpy(me->tmp_buf + me->tmp_ind, b, l);
	    me->tmp_ind += l;
	    return HT_OK;
	} else {

	    /*
	    **  Add the temporary buffer (if any) to the list of chunks
	    */
	    if (me->tmp_buf) append_buf(me);

	    /*
	    **  Find the right size of the next chunk. We increase the size
	    **  exponentially until we reach HT_MAX_BLOCK in order to minimize
	    **  the number of mallocs.
	    */
	    if (me->cur_size < HT_MAX_BLOCK) {
		int newsize = me->cur_size ? me->cur_size : HT_MIN_BLOCK;
		while (l > newsize && newsize < HT_MAX_BLOCK) newsize *= 2;
		me->cur_size = newsize;
	    }

	    if (alloc_new(me, me->cur_size)) {
		/* Buffer could accept the new data */
		memcpy(me->tmp_buf, b, l);
		me->tmp_ind = l;
	    } else if (me->mode & HT_BM_DELAY) {
		/* Buffer ran full and we pause */
		me->state = HT_BS_PAUSE;
		HTTRACE(STREAM_TRACE, "Buffer....... Paused\n");
		return HT_PAUSE;
	    } else {
		/* Buffer ran full and we flush and go transparent */
		int status = buf_flush(me);
		if (status != HT_OK) return status;
	    }
	}
    }

    /*
    **  If we couldn't buffer the data then check whether we should give up
    **  or pause the stream. If we are in transparent mode then put the rest
    **  of the data down the pipe.
    */
    if (me->state == HT_BS_TRANSPARENT) return PUTBLOCK(b, l);
    return HT_OK;
}

PRIVATE int buf_put_char (HTStream * me, char c)
{
    return buf_put_block(me, &c, 1);
}

PRIVATE int buf_put_string (HTStream * me, const char * s)
{
    return buf_put_block(me, s, (int) strlen(s));
}

PRIVATE int buf_free (HTStream * me)
{
    int status = HT_OK;

    /*
    **  If the buffer has not been flushed explicit and we are a pipe buffer
    **  then we don't free it.
    */ 
    if (me->mode & HT_BM_PIPE && me->state != HT_BS_TRANSPARENT) {
	HTTRACE(STREAM_TRACE, "PipeBuffer Waiting to be flushed\n");
	return HT_OK;
    }

    /*
    **  Should we count the content length and assign it to the
    **  anchor?
    */
    if (me->mode & HT_BM_COUNT && me->request) {
	HTParentAnchor * anchor = HTRequest_anchor(me->request);
	HTTRACE(STREAM_TRACE, "Buffer........ Calculated content-length: %d\n" _ me->conlen);
	HTAnchor_setLength(anchor, me->conlen);
    }

    /*
    **  Flush the buffered data - even if we are paused. That is, we always
    **  flush - except if we have already flushed the buffer, of course.
    **  Also, we don't flush if we are a PIPE buffer. Then the flush MUST be
    **  called explicitly and the buffer can not be freed before this 
    **  happens.
    */
    if ((status = buf_flush(me)) != HT_OK)
	return status;
    if ((status = (*me->target->isa->_free)(me->target)) != HT_OK)
	return status;
    HT_FREE(me);
    return status;
}

PRIVATE int buf_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target,e);
    free_buf_all(me);
    HT_FREE(me);
    HTTRACE(PROT_TRACE, "Buffer...... ABORTING...\n");
    return HT_ERROR;
}

HTStreamClass HTBufferClass = {
    "Buffer",
    buf_flush,
    buf_free,
    buf_abort,
    buf_put_char,
    buf_put_string,
    buf_put_block
};

PUBLIC HTStream * HTBuffer_new (HTStream *	target,
				HTRequest *	request,
				int		max_size)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTBufferStream");
    me->isa = &HTBufferClass;
    me->target = target;
    me->request = request;
    me->max_size = (max_size > 0) ? max_size : HT_MAX_SIZE;
    me->mode = HT_BM_PLAIN;
    HTTRACE(STREAM_TRACE, "Buffer...... Created with size %d\n" _ me->max_size);
    return me;
}

PUBLIC HTStream * HTDelayBuffer (HTStream * target, int max_size)
{
    HTStream * me = HTBuffer_new(target, NULL, max_size);
    if (me) {
	me->mode = HT_BM_DELAY;
	return me;
    }
    return HTErrorStream();
}

PUBLIC HTStream * HTContentCounter (HTStream *	target,
				    HTRequest *	request,
				    int		max_size)
{
    HTStream * me = HTBuffer_new(target, NULL, max_size);
    if (me) {
	me->mode = HT_BM_COUNT;
	return me;
    }
    return HTErrorStream();
}

PUBLIC HTStream * HTPipeBuffer (HTStream * target, int max_size)
{
    HTStream * me = HTBuffer_new(target, NULL, max_size);
    if (me) {
	me->mode = HT_BM_PIPE;
	return me;
    }
    return HTErrorStream();
}
