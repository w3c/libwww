/*
**	MERGE CLASS STREAM DEFINITION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	The Merge class just is a n entry stream. Easy. The reason for having
**	this stream is that we must be able to handle free and abort in an
**	organized manner.
**	You can also check out the. Tee stream and the Black hole stream.
**
** HISTORY:
**	25 Sep 96  HFN	Written
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTMerge.h"

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *			target;
    int				feeds;			  /* Number of feeds */
};

PRIVATE int HTMerge_putCharacter (HTStream * me, char c)
{
    return (me->feeds > 0) ?
	(*me->target->isa->put_character)(me->target, c) : HT_OK;
}

PRIVATE int HTMerge_putString (HTStream * me, const char * s)
{
    return (me->feeds > 0) ?
	(*me->target->isa->put_string)(me->target, s) : HT_OK;
}

PRIVATE int HTMerge_putBlock (HTStream * me, const char * s, int l)
{
    return (me->feeds > 0) ?
	(*me->target->isa->put_block)(me->target, s, l) : HT_OK;
}

PRIVATE int HTMerge_flush (HTStream * me)
{
    return (me->feeds > 0) ?
	(*me->target->isa->flush)(me->target) : HT_OK;
}

/*
**  We expect n number of calls to this method but as long as our number
**  of feeds is not down to 1, we just ignore it. Abort works the same way
*/
PRIVATE int HTMerge_free (HTStream * me)
{
    if (me) {
	HTTRACE(STREAM_TRACE, "Merge Free.. Called with %d feeds\n" _ me->feeds);
	if (--me->feeds <= 0) {
	    (*me->target->isa->_free)(me->target);
	    HT_FREE(me);
	}
	return HT_OK;
    }
    return HT_ERROR;
}

PRIVATE int HTMerge_abort (HTStream * me, HTList * e)
{
    if (me) {
	HTTRACE(STREAM_TRACE, "Merge Abort. Called with %d feeds\n" _ me->feeds);
	if (--me->feeds <= 0) {
	    (*me->target->isa->abort)(me->target, e);
	    HT_FREE(me);
	}
    }
    return HT_ERROR;
}

PRIVATE const HTStreamClass HTMergeClass =
{		
    "MergeStream",
    HTMerge_flush,
    HTMerge_free,
    HTMerge_abort,
    HTMerge_putCharacter,
    HTMerge_putString,
    HTMerge_putBlock
}; 

PUBLIC HTStream * HTMerge (HTStream * target, int feeds)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(*me))) == NULL)
        HT_OUTOFMEM("HTMerge");
    me->isa = &HTMergeClass;
    me->target = target ? target : HTBlackHole();
    me->feeds = feeds >= 1 ? feeds : 1;			       /* Min 1 feed */
    HTTRACE(STREAM_TRACE, "Merge....... Created stream %p\n" _ me);
    return me;
}
