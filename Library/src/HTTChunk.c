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
#include "wwwsys.h"
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
    BOOL			lastchunk;	  /* Is this the last chunk? */
    BOOL			trailer;	    /* Do we have a trailer? */
    HTEOLState			state;    
    HTChunk *			buf;
    int				status;	     /* return code from down stream */
};

/* ------------------------------------------------------------------------- */

/*
**	Chunked Decoder stream
*/
PRIVATE BOOL HTChunkDecode_header (HTStream * me) 
{
    char * line = HTChunk_data(me->buf);
    if (line) {
	char *errstr = NULL;
	me->left = strtol(line, &errstr, 16);    /* hex! */
	HTTRACE(STREAM_TRACE, "Chunked..... `%s\' chunk size: %X\n" _ line _ me->left);
	if (errstr == line)
	    HTDEBUGBREAK("Chunk decoder received illigal chunk size: `%s\'\n" _ line);
	if (me->left > 0) {
	    me->total += me->left;

	    /* Look for arguments */
	
	    HTChunk_clear(me->buf);
	} else if (me->left == 0)	       		      /* Last chunk */
	    me->lastchunk = YES;
	else if (me->left < 0)
	    return NO;
	return YES;
    }
    return NO;
}

PRIVATE int HTChunkDecode_block (HTStream * me, const char * b, int l)
{
    while (l > 0) {
	int length = l;
	if (me->left <= 0 && !me->trailer) {
	    while (l > 0) {
		if (me->state == EOL_FLF) {
		    if (HTChunkDecode_header(me) == NO) return HT_ERROR;
		    if (me->lastchunk) if (*b != CR && *b != LF) me->trailer = YES;
		    me->state = EOL_DOT;
		    break;
		} else if (me->state == EOL_SLF) {
		    if (me->lastchunk) break;
		    me->state = EOL_BEGIN;
		    HTChunk_putc(me->buf, *b);
		} else if (*b == CR) {
		    me->state = me->state == EOL_DOT ? EOL_SCR : EOL_FCR;
		} else if (*b == LF) {
		    me->state = me->state == EOL_SCR ? EOL_SLF : EOL_FLF;
		} else
		    HTChunk_putc(me->buf, *b);
		b++, l--;
	    }
	}

	/*
	** Account for the parts we read in the chunk header +
	** the chunk that we are reading.
	*/
	if (length != l)
	    HTHost_setConsumed(HTNet_host(HTRequest_net(me->request)), length - l);

	/*
	** If we have to read trailers. Otherwise we are done.
	*/
	if (me->trailer) {
	    me->target = HTStreamStack(WWW_MIME_FOOT, WWW_SOURCE,
				       me->target, me->request, NO);
	} else if (me->state == EOL_SLF) {
            if (me->lastchunk) {
                HTAlertCallback * cbf = HTAlert_find(HT_PROG_DONE);
                if (cbf) (*cbf)(me->request, HT_PROG_DONE, HT_MSG_NULL,
                                NULL, NULL, NULL);
                return HT_LOADED;
            }
	    me->state = EOL_BEGIN;
	}

	/*
	**  Handle the rest of the data including trailers
	*/
	if (l > 0 && me->left) {
	    int bytes = HTMIN(l, me->left);
	    int status = (*me->target->isa->put_block)(me->target, b, bytes);
	    if (status != HT_OK) return status;
	    HTHost_setConsumed(HTNet_host(HTRequest_net(me->request)), bytes);
	    me->left -= bytes;
	    l -= bytes, b+= bytes;
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

    /*
    **  Update the amount of data that we read in all.
    */
    HTAnchor_setLength(anchor, me->total);

    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    HTTRACE(PROT_TRACE, "Chunked..... FREEING....\n");
    HTChunk_delete(me->buf);
    HT_FREE(me);
    return status;
}

PRIVATE int HTChunkDecode_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;
    if (me->target) status = (*me->target->isa->abort)(me->target, e);
    HTTRACE(PROT_TRACE, "Chunked..... ABORTING...\n");
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
    me->status = HT_ERROR;
    
    /* Adjust information in anchor */
    HTAnchor_setLength(anchor, -1);

    HTTRACE(STREAM_TRACE, "Chunked..... Decoder stream created\n");
    return me;
}

/*
**	Chunked Encoder Stream
*/
PRIVATE int HTChunkEncode_block (HTStream * me, const char * b, int l)
{
    char * chunky = HTChunk_data(me->buf);
    if (me->lastchunk) return HT_LOADED;
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
    HTTRACE(STREAM_TRACE, "Chunked..... chunk size 0x%X\n" _ l);
    if (l > 0) return PUTBLOCK(b, l);

    /* Here we should provide a footer */

    PUTC(CR);
    PUTC(LF);
    me->lastchunk = YES;
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
    HTTRACE(PROT_TRACE, "Chunked..... ABORTING...\n");
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
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTChunkEncoder");
    me->isa = &HTChunkEncoderClass;
    me->coding = coding;
    me->target = target;
    me->request = request;
    me->param = (char *) param;
    me->state = EOL_BEGIN;
    me->status = HT_ERROR;
    {
	int length = me->param ? strlen(me->param)+20 : 20;
	me->buf = HTChunk_new(length);
	HTChunk_ensure(me->buf, length);
    }

    HTTRACE(STREAM_TRACE, "Chunked..... Encoder stream created\n");
    return me;
}



