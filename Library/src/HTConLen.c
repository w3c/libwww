/*							             HTConlen.c
**	CONTENT LENGTH COUNTER STREAM
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This stream counts the number of bytes in a stream until it reaches
**	max number of bytes it can occupy. If this happens then	it gives up.
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTReq.h"
#include "HTConLen.h"					 /* Implemented here */

#define HT_MIN_BLOCK 	0x100
#define HT_MAX_BLOCK	0x2000
#define HT_MAX_SIZE	0x10000
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)	     (me->target, b, l)

typedef struct _HTBufItem {
    int			len;
    char *		buf;
    struct _HTBufItem *	next;
} HTBufItem;

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

    BOOL		ignore;
    BOOL		give_up;
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
	if (STREAM_TRACE)
	    TTYPrint(TDEST,"StreamBuffer size %d reached, going transparent\n",
		    me->max_size);
	return NO;
    } else if (size) {
	me->tmp_ind = 0;
	me->tmp_max = size;
	if ((me->tmp_buf = (char  *) HT_MALLOC(size)) == NULL)
	    HT_OUTOFMEM("buf_put_char");
	if (STREAM_TRACE)
	    TTYPrint(TDEST, "StreamBuffer created with len %d\n", size);
	return YES;
    }
    return NO;
}

PRIVATE int buf_flush (HTStream * me)
{
    HTBufItem * cur;
    if (me->tmp_buf) append_buf(me);
    while ((cur = me->head)) {
	int status;
	if ((status = PUTBLOCK(cur->buf, cur->len)) != HT_OK) return status;
	me->head = cur->next;
	free_buf(cur);
    }
    me->give_up = YES;
    return HT_OK;
}

PRIVATE int buf_put_block (HTStream * me, CONST char * b, int l)
{
    me->conlen += l;
    if (!me->give_up) {
	if (me->tmp_buf && me->tmp_max-me->tmp_ind >= l) {     /* Still room */
	    memcpy(me->tmp_buf + me->tmp_ind, b, l);
	    me->tmp_ind += l;
	    return HT_OK;
	} else {
	    if (me->tmp_buf) append_buf(me);
	    if (me->cur_size < HT_MAX_BLOCK) {
		int newsize = me->cur_size ? me->cur_size : HT_MIN_BLOCK;
		while (l > newsize && newsize < HT_MAX_BLOCK) newsize *= 2;
		me->cur_size = newsize;
	    }
	    if (!alloc_new(me, me->cur_size)) {
		int status = buf_flush(me);
		if (status != HT_OK) return status;
		me->give_up = YES;
	    } else {
		memcpy(me->tmp_buf, b, l);
		me->tmp_ind = l;
	    }
	}
    }
    if (me->give_up) return PUTBLOCK(b, l);
    return HT_OK;
}

PRIVATE int buf_put_char (HTStream * me, char c)
{
    return buf_put_block(me, &c, 1);
}

PRIVATE int buf_put_string (HTStream * me, CONST char * s)
{
    return buf_put_block(me, s, (int) strlen(s));
}

PRIVATE int buf_free (HTStream * me)
{
    int status = HT_OK;
    if (!me->ignore) {
	HTParentAnchor *anchor = HTRequest_anchor(me->request);
	if (STREAM_TRACE)
	    TTYPrint(TDEST,"\nCalculated.. content-length: %d\n", me->conlen);
	HTAnchor_setLength(anchor, me->conlen);
    }
    if (!me->give_up && (status = buf_flush(me)) != HT_OK)
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
    if (PROT_TRACE) TTYPrint(TDEST, "Length...... ABORTING...\n");
    return HT_ERROR;
}

HTStreamClass HTContentCounterClass = {
    "ContentCounter",
    buf_flush,
    buf_free,
    buf_abort,
    buf_put_char,
    buf_put_string,
    buf_put_block
};

PUBLIC HTStream * HTContentCounter (HTStream *	target,
				    HTRequest *	request,
				    int		max_size)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTContentCounter");
    me->isa = &HTContentCounterClass;
    me->target = target;
    me->request = request;
    me->max_size = (max_size > 0) ? max_size : HT_MAX_SIZE;
    if (STREAM_TRACE)
	TTYPrint(TDEST, "Buffer...... Created with size %d\n", me->max_size);
    return me;
}

PUBLIC HTStream * HTBuffer_new (HTStream *	target,
				HTRequest *	request,
				int		max_size)
{
    HTStream * me = HTContentCounter(target, request, max_size);
    if (me) me->ignore = YES;
    return me;
}
