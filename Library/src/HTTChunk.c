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
#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)

struct _HTStream {
    const HTStreamClass *	isa;
    HTEncoding			coding;
    HTStream *			target;
    HTRequest *			request;
    char * 			param;	    /* Extra parameters for encoding */
    long			left;	    /* Remaining bytes in this chunk */
    long			total;			      /* Full length */
    BOOL			done;
    HTEOLState			state;    
    HTChunk *			buf;
};

/* ------------------------------------------------------------------------- */

/*
**	Chunked Decoder stream
*/
PRIVATE BOOL HTChunkDecode_header (HTStream * me) 
{
    char * line = HTChunk_data(me->buf);
    if (line) {
	me->left = strtol(line, (char **) NULL, 16);    /* hex! */
	if (STREAM_TRACE) HTTrace("Chunked..... chunk size: %X\n", me->left);
	if (me->left) {
	    me->total += me->left;

	    /* Look for arguments */
	
	    HTChunk_clear(me->buf);
	} else {					       /* Last chunk */
	    HTRequest * request = me->request;
	    me->target = HTStreamStack(WWW_MIME_FOOT, WWW_SOURCE,
				       me->target, request, NO);
	}
	return YES;
    }
    return NO;
}

PRIVATE int HTChunkDecode_block (HTStream * me, const char * b, int l)
{
    while (l > 0) {
	if (me->left <= 0) {
	    while (l > 0) {
		if (me->state == EOL_FLF) {
		    HTChunkDecode_header(me);
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

PRIVATE int HTChunkDecode_string (HTStream * me, const char * s)
{
    return HTChunkDecode_block(me, s, (int) strlen(s));
}

PRIVATE int HTChunkDecode_character (HTStream * me, char c)
{
    return HTChunkDecode_block(me, &c, 1);
}

PRIVATE int HTChunkDecode_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTChunkDecode_free (HTStream * me)
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

PRIVATE int HTChunkDecode_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;
    if (me->target) status = (*me->target->isa->abort)(me->target, e);
    if (PROT_TRACE) HTTrace("Chunked..... ABORTING...\n");
    HTChunk_delete(me->buf);
    HT_FREE(me);
    return status;
}

PRIVATE const HTStreamClass HTChunkDecodeClass =
{
    "ChunkDecoder",
    HTChunkDecode_flush,
    HTChunkDecode_free,
    HTChunkDecode_abort,
    HTChunkDecode_character,
    HTChunkDecode_string,
    HTChunkDecode_block
};

PUBLIC HTStream * HTChunkedDecoder   (HTRequest *	request,
				      void *		param,
				      HTEncoding	coding,
				      HTStream *	target)
{
    HTStream * me;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTChunkDecoder");
    me->isa = &HTChunkDecodeClass;
    me->coding = coding;
    me->target = target;
    me->request = request;
    me->state = EOL_BEGIN;
    me->buf = HTChunk_new(64);
    
    /* Adjust information in anchor */
    HTAnchor_setLength(anchor, -1);
    HTAnchor_setTransfer(anchor, NULL);

    if (STREAM_TRACE) HTTrace("Chunked..... Decoder stream created\n");
    return me;
}

/*
**	Chunked Encoder Stream
*/
PRIVATE int HTChunkEncode_block (HTStream * me, const char * b, int l)
{
    char * chunky = HTChunk_data(me->buf);
    if (me->done) return HT_LOADED;
    if (me->param) {
	if (me->total)
	    sprintf(chunky, "%c%c%x %s %c%c", CR, LF, l, me->param, CR, LF);
	else
	    sprintf(chunky, "%x %s %c%c", l, me->param, CR, LF);
    } else {
	if (me->total)
	    sprintf(chunky, "%c%c%x%c%c", CR, LF, l, CR, LF);
	else
	    sprintf(chunky, "%x%c%c", l, CR, LF);
    }
    me->total += l;
    PUTBLOCK(chunky, (int) strlen(chunky));
    if (STREAM_TRACE) HTTrace("Chunked..... chunk size 0x%X\n", l);
    if (l > 0) return PUTBLOCK(b, l);

    /* Here we should provide a footer */

    PUTC(CR);
    PUTC(LF);
    me->done = YES;
    (*me->target->isa->flush)(me->target);
    return HT_LOADED;
}

PRIVATE int HTChunkEncode_string (HTStream * me, const char * s)
{
    return HTChunkEncode_block(me, s, (int) strlen(s));
}

PRIVATE int HTChunkEncode_character (HTStream * me, char c)
{
    return HTChunkEncode_block(me, &c, 1);
}

PRIVATE int HTChunkEncode_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTChunkEncode_free (HTStream * me)
{
#if 0
    int status = HTChunkEncode_block(me, NULL, 0);
    if (status != HT_WOULD_BLOCK) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	HT_FREE(me);
    }
    return status;
#else
    int status = me->target ? (*me->target->isa->_free)(me->target) : HT_OK;
    HT_FREE(me);
    return status;
#endif
}

PRIVATE int HTChunkEncode_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;
    if (me->target) status = (*me->target->isa->_free)(me->target);
    if (PROT_TRACE) HTTrace("Chunked..... ABORTING...\n");
    HT_FREE(me);
    return status;
}

PRIVATE const HTStreamClass HTChunkEncoderClass =
{
    "ChunkEncoder",
    HTChunkEncode_flush,
    HTChunkEncode_free,
    HTChunkEncode_abort,
    HTChunkEncode_character,
    HTChunkEncode_string,
    HTChunkEncode_block
};

PUBLIC HTStream * HTChunkedEncoder   (HTRequest *	request,
				      void *		param,
				      HTEncoding	coding,
				      HTStream *	target)
{
    HTStream * me;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTChunkEncoder");
    me->isa = &HTChunkEncoderClass;
    me->coding = coding;
    me->target = target;
    me->request = request;
    me->param = (char *) param;
    me->state = EOL_BEGIN;
    {
	int length = me->param ? strlen(me->param)+20 : 20;
	me->buf = HTChunk_new(length);
	HTChunk_ensure(me->buf, length);
    }

    /* Adjust information in anchor */
    HTAnchor_setTransfer(anchor, NULL);

    if (STREAM_TRACE) HTTrace("Chunked..... Encoder stream created\n");
    return me;
}



