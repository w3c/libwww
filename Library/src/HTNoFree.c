/*
**	NO FREE STREAM
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This stream is a throughline for all methods except FREE and ABORT
**
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTStream.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *			target;
};

/* ------------------------------------------------------------------------- */

PRIVATE int HTNoFreeStream_put_character (HTStream * me, char c)
{
    return (*me->target->isa->put_character)(me->target, c);
}

PRIVATE int HTNoFreeStream_put_string (HTStream * me, const char * s)
{
    return (*me->target->isa->put_string)(me->target, s);
}

PRIVATE int HTNoFreeStream_write (HTStream * me, const char * s, int l)
{
    return (*me->target->isa->put_block)(me->target, s, l);
}

PRIVATE int HTNoFreeStream_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTNoFreeStream_free (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTNoFreeStream_abort (HTStream * me, HTList * e)
{
    return HT_ERROR;
}

PRIVATE const HTStreamClass HTNoFreeStreamClass =
{		
    "NoFreeStream",
    HTNoFreeStream_flush,
    HTNoFreeStream_free,
    HTNoFreeStream_abort,
    HTNoFreeStream_put_character,
    HTNoFreeStream_put_string,
    HTNoFreeStream_write
}; 

PUBLIC HTStream * HTNoFreeStream_new (HTStream * target)
{
    if (target) {
	HTStream * me;
	if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	    HT_OUTOFMEM("HTNoFree");
	me->isa = &HTNoFreeStreamClass;
	me->target = target;
	return me;
    }
    return HTErrorStream();
}

PUBLIC int HTNoFreeStream_delete (HTStream * me) 
{
    if (me) HT_FREE(me);
    return HT_OK;
}
