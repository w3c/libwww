/*
**	STREAM TO CHUNK CONVERTER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This stream converts a stream to a chunk object.
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTSChunk.h"					 /* Implemented here */

#define HT_MAXSIZE	0x10000		      /* Max size of allocated chunk */
#define HT_MAXGROWSIZE	0x4000		 /* Increment buffer by no more than */

struct _HTStream {
    HTStreamClass *	isa;
    HTRequest *		request;
    HTChunk *		chunk;

    int			cur_size;
    int			max_size;
    BOOL		give_up;
    BOOL		ignore;
    BOOL		ensure;
};

/* ------------------------------------------------------------------------- */

PRIVATE int HTSC_flush (HTStream * me)
{
    return HT_OK;			/* Nothing to flush */
}

/*
**	We don't free the chunk as this is for the caller to do
*/
PRIVATE int HTSC_free (HTStream * me)
{
    HTTRACE(STREAM_TRACE, "Chunkstream. FREEING...\n");
    HT_FREE(me);
    return HT_OK;
}

/*
**	We don't free the chunk as this is for the caller to do
*/
PRIVATE int HTSC_abort (HTStream * me, HTList * errorlist)
{
    HTTRACE(STREAM_TRACE, "Chunkstream. ABORTING...\n");
    HT_FREE(me);
    return HT_ERROR;
}

PRIVATE int HTSC_putBlock (HTStream * me, const char * b, int l)
{
    me->cur_size += l;

    /*
    ** If we get a buffer overflow and we are going to PUT or POST the document
    ** then ask the user whether it is OK to proceed buffering. Otherwise we
    ** must give up the request. In all other cases we stop if the buffer fills
    ** up.
    */
    if (!me->ignore && me->max_size > 0 && me->cur_size > me->max_size) {
	HTMethod method = HTRequest_method(me->request);
	if (HTMethod_hasEntity(method)) {
	    HTAlertCallback *cbf = HTAlert_find(HT_A_CONFIRM);
	    if ((cbf && (*cbf)(me->request, HT_A_CONFIRM, HT_MSG_BIG_PUT,
			       NULL, NULL, NULL)))
		me->ignore = YES;
	    else
		me->give_up = YES;
	} else {
	    me->give_up = YES;
	}
    } else if (!me->ensure) {
	HTParentAnchor * anchor = HTRequest_anchor(me->request);
	int cl = HTAnchor_length(anchor);
	if (cl > 0) HTChunk_ensure(me->chunk, cl);
	me->ensure = YES;
    }
    if (!me->give_up) {
	HTChunk_putb(me->chunk, b, l);
	return HT_OK;
    }    
    return HT_ERROR;
}

PRIVATE int HTSC_putCharacter (HTStream * me, char c)
{
    return HTSC_putBlock(me, &c, 1);
}

PRIVATE int HTSC_putString (HTStream * me, const char * s)
{
    return HTSC_putBlock(me, s, (int) strlen(s));
}

HTStreamClass HTStreamToChunkClass = {
    "StreamToChunk",
    HTSC_flush,
    HTSC_free,
    HTSC_abort,
    HTSC_putCharacter,
    HTSC_putString,
    HTSC_putBlock
};

PUBLIC HTStream * HTStreamToChunk (HTRequest * 	request,
				   HTChunk **	chunk,
				   int 		max_size)
{
    if (request) {
	HTStream * me;
	*chunk = NULL;
	if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	    HT_OUTOFMEM("HTStreamToChunk");
	me->isa = &HTStreamToChunkClass;
	me->request = request;
	me->max_size = (!max_size) ? max_size : HT_MAXSIZE;
	me->chunk = *chunk = HTChunk_new(me->max_size > 0 ?
					 HTMIN(me->max_size, HT_MAXGROWSIZE) :
					 HT_MAXGROWSIZE);
	HTTRACE(STREAM_TRACE, "ChunkStream. Chunk %p created with max size %d\n" _ 
		    me->chunk _ me->max_size);
	return me;
    }
    return HTErrorStream();
}

