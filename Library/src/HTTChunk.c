/*
**	CHUNKED TRANSFER CODING
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This stream parses a chunked transfer encoding using multiple chunks.
**
** Authors
**	HF	Henrik Frystyk <frystyk@w3.org>
**
** History:
**	Apr 96	Written from scratch
**
*/

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTTChunk.h"					 /* Implemented here */

#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)
#define PUTDEBUG(b, l)	(*me->debug->isa->put_block)(me->debug, b, l)
#define FREE_TARGET	(*me->target->isa->_free)(me->target)

struct _HTStream {
    const HTStreamClass *	isa;
    HTEncoding			coding;
    HTStream *			target;
    HTRequest *			request;
    long			left;	    /* Remaining bytes in this chunk */
    long			total;			      /* Full length */
    HTEOLState			state;    
    HTChunk *			buf;
};

/* ------------------------------------------------------------------------- */

/*
**
*/
PRIVATE BOOL HTTChunk_header (HTStream * me) 
{
    me->left = strtol(HTChunk_data(me->buf), (char **) NULL, 16);    /* hex! */
    if (STREAM_TRACE) HTTrace("Chunked..... chunk size: %X\n", me->left);
    if (me->left) {
	me->total += me->left;

	/* Look for arguments */
	
	HTChunk_clear(me->buf);
    } else {						       /* Last chunk */
	HTRequest * request = me->request;
	me->target = HTStreamStack(WWW_MIME_FOOT, WWW_SOURCE,
				   me->target, request, NO);
    }
    return YES;
}

PRIVATE int HTTChunk_put_block (HTStream * me, const char * b, int l)
{
    while (l > 0) {
	if (me->left <= 0) {
	    while (l > 0) {
		if (me->state == EOL_FLF) {
		    HTTChunk_header(me);
		    me->state = EOL_DOT;
		    break;
		} else if (*b == CR) {
		    me->state = me->state == EOL_DOT ? EOL_SCR : EOL_FCR;
		} else if (*b == LF) {
		    me->state = me->state == EOL_SCR ? EOL_BEGIN : EOL_FLF;
		} else
		    HTChunk_putc(me->buf, *b);
		b++, l--;
	    }
	}
	if (l > 0) {
	    int bytes = me->left ? HTMIN(l, me->left) : l;
	    int status = (*me->target->isa->put_block)(me->target, b, bytes);
	    if (status == HT_OK) {
		me->left -= bytes;
		l -= bytes, b+= bytes;
	    } else
		return status;
	}
    }
    return HT_OK;
}

PRIVATE int HTTChunk_put_string (HTStream * me, const char * s)
{
    return HTTChunk_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTTChunk_put_character (HTStream * me, char c)
{
    return HTTChunk_put_block(me, &c, 1);
}

PRIVATE int HTTChunk_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTTChunk_free (HTStream * me)
{
    int status = HT_OK;
    HTParentAnchor * anchor = HTRequest_anchor(me->request);
    HTAnchor_setLength(anchor, me->total);
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    if (PROT_TRACE) HTTrace("Chunked..... FREEING....\n");
    HTChunk_delete(me->buf);
    HT_FREE(me);
    return status;
}

PRIVATE int HTTChunk_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;
    if (me->target) status = (*me->target->isa->abort)(me->target, e);
    if (PROT_TRACE) HTTrace("Chunked..... ABORTING...\n");
    HT_FREE(me);
    return status;
}

PRIVATE const HTStreamClass HTChunkedClass =
{
    "HTChunked",
    HTTChunk_flush,
    HTTChunk_free,
    HTTChunk_abort,
    HTTChunk_put_character,
    HTTChunk_put_string,
    HTTChunk_put_block
};

PUBLIC HTStream * HTChunkedDecoder   (HTRequest *	request,
				      void *		param,
				      HTEncoding	coding,
				      HTStream *	target)
{
    HTStream * me;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTTChunk");
    me->isa = &HTChunkedClass;
    me->coding = coding;
    me->target = target;
    me->request = request;
    me->state = EOL_BEGIN;
    me->buf = HTChunk_new(64);
    
    /* Adjust information in anchor */
    HTAnchor_setTransfer(anchor, NULL);

    if (STREAM_TRACE) HTTrace("Chunked..... Decoder stream created\n");
    return me;
}
