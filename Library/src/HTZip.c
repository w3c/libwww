/*
**	ZIP ENCODING / DECODING MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module requires zlib in order to compile/link
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTZip.h"					 /* Implemented here */

#ifdef WWW_MSWINDOWS
#define ZLIB_DLL
#endif

#include <zlib.h>

#define OUTBUF_SIZE		32768

struct _HTStream {
    const HTStreamClass *	isa;
    int				state;
    HTRequest *			request;
    HTStream *			target;			/* Our output target */
    z_stream *			zstream;		      /* Zlib stream */
    char 			outbuf [OUTBUF_SIZE]; 	    /* Inflated data */
};

PRIVATE int CompressionLevel = Z_DEFAULT_COMPRESSION;

/* ------------------------------------------------------------------------- */

PRIVATE BOOL Zlib_init (HTStream * me, int level)
{
    if (me && me->zstream &&
	(level == Z_DEFAULT_COMPRESSION ||
	 (level >= Z_BEST_SPEED && level <= Z_BEST_COMPRESSION))) {
	int status;
	HTTRACE(STREAM_TRACE, "Zlib Inflate Init stream %p with compression level %d\n" _ me _ level);
	if ((status = inflateInit(me->zstream)) != Z_OK) {
	    HTTRACE(STREAM_TRACE, "Zlib........ Failed with status %d\n" _ status);
	    return NO;
	}
	return YES;
    }
    return NO;
}

PRIVATE BOOL ZLib_terminate (HTStream * me)
{
    HTTRACE(STREAM_TRACE, "Zlib Inflate Terminating stream %p\n" _ me);
    if (me) {
	int status;
	HTTRACE(STREAM_TRACE, "Results..... Inflated incoming data: deflated %lu, inflated %lu, factor %.2f\n" _ 
		    me->zstream->total_in _ me->zstream->total_out _ 
		    me->zstream->total_in == 0 ? 0.0 :
		    (double) me->zstream->total_out / me->zstream->total_in);
	if ((status = inflateEnd(me->zstream)) != Z_OK) {
	    HTTRACE(STREAM_TRACE, "Zlib........ Failed with status %d\n" _ status);
	    return NO;
	}
	return YES;
    }
    return NO;
}

PRIVATE int HTZLibInflate_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTZLibInflate_free (HTStream * me)
{
    int status = HT_OK;
    ZLib_terminate(me);
    if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	return HT_WOULD_BLOCK;
    HTTRACE(STREAM_TRACE, "Zlib Inflate FREEING...\n");
    HT_FREE(me->zstream);
    HT_FREE(me);
    return status;
}

PRIVATE int HTZLibInflate_abort (HTStream * me, HTList * e)
{
    HTTRACE(STREAM_TRACE, "Zlib Inflate ABORTING...\n");
    ZLib_terminate(me);
    (*me->target->isa->abort)(me->target, NULL);
    HT_FREE(me->zstream);
    HT_FREE(me);
    return HT_ERROR;
}

PRIVATE int HTZLibInflate_write (HTStream * me, const char * buf, int len)
{
    if (me->state != HT_OK) {
	me->state = (*me->target->isa->put_block)(me->target,
						   me->outbuf,
						   OUTBUF_SIZE - me->zstream->avail_out);
	if (me->state != HT_OK) return me->state;
    }

    /*
    **  Setup the zstream buffers to handle the new data
    */
    me->zstream->next_in = (unsigned char *) buf;
    me->zstream->avail_in = len;
    me->zstream->next_out = (unsigned char *) me->outbuf;
    me->zstream->avail_out = OUTBUF_SIZE;
    
    /*
    ** Now run through the data and inflate it. We use Z_PARTIAL_FLUSH
    ** in order to put as much data in the output buffer as quick as
    ** possible.
    */
    for (;;) {
	int status = inflate(me->zstream, Z_PARTIAL_FLUSH);
	switch (status) {
	case Z_OK:
	    me->state = (*me->target->isa->put_block)(me->target,
						       me->outbuf,
						       OUTBUF_SIZE - me->zstream->avail_out);
	    if (me->state != HT_OK) return me->state;

	    /*
	    **  All data was consumed and we are ready to handle the
	    **  next data.
	    */
	    me->zstream->next_out = (unsigned char *) me->outbuf;
	    me->zstream->avail_out = OUTBUF_SIZE;
	    break;

	case Z_MEM_ERROR:
	    HT_OUTOFMEM("HTZLibInflate_write");
	    return HT_ERROR;

	case Z_BUF_ERROR:
	    /*
	    ** No action was taken anf we can take this as a sign ot it
	    ** having finished all data in the input buffer.
	    */
	    return HT_OK;

	case Z_STREAM_END:
	    me->state = (*me->target->isa->put_block)(me->target,
						       me->outbuf,
						       OUTBUF_SIZE - me->zstream->avail_out);
	    if (me->state != HT_OK) return me->state;

	    HTTRACE(STREAM_TRACE, "Zlib Inflate End of Stream\n");
	    return HT_OK;

	default:
	    HTTRACE(STREAM_TRACE, "Zlib Inflate Inflate returned %d\n" _ status);
	    return HT_ERROR;
	}
    }
    return HT_OK;
}

PRIVATE int HTZLibInflate_put_character (HTStream * me, char c)
{
    return HTZLibInflate_write(me, &c, 1);
}

PRIVATE int HTZLibInflate_put_string (HTStream * me, const char * s)
{
    return HTZLibInflate_write(me, s, (int) strlen(s));
}

PRIVATE const HTStreamClass HTInflate =
{		
    "ZlibInflate",
    HTZLibInflate_flush,
    HTZLibInflate_free,
    HTZLibInflate_abort,
    HTZLibInflate_put_character,
    HTZLibInflate_put_string,
    HTZLibInflate_write
}; 

PUBLIC BOOL HTZLib_setCompressionLevel (int level)
{
    if (level >= Z_BEST_SPEED && level <= Z_BEST_COMPRESSION) {
	CompressionLevel = level;
	HTTRACE(STREAM_TRACE, "Zlib........ Compression level set to %d\n" _ level);
    }
    return NO;
}

PUBLIC int HTZLib_compressionLevel (void)
{
    return CompressionLevel;
}

PUBLIC HTStream * HTZLib_inflate (HTRequest *	request,
				  void *	param,
				  HTEncoding	coding,
				  HTStream *	target)
{
    HTStream * me = NULL;
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL ||
	(me->zstream = (z_stream *) HT_CALLOC(1, sizeof(z_stream))) == NULL)
	HT_OUTOFMEM("HTZLib_inflate");
    me->isa = &HTInflate;
    me->state = HT_OK;
    me->request = request;
    me->target = target ? target : HTErrorStream();
    if (Zlib_init(me, CompressionLevel) != YES) {
	HT_FREE(me);
	return HTErrorStream();
    }
    HTTRACE(STREAM_TRACE, "Zlib Inflate Stream created\n");
    return me;
}
