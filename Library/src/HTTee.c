/*									 HTee.c
**	TEE CLASS STREAM DEFINITION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	The Tee class just writes to two streams.  Easy.
**	See also the Black Hole stream which is even easier.
**
** HISTORY:
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTTee.h"

/*		Stream Object
**		------------
*/

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *			s1;
    HTStream *			s2;
    HTComparer *		resolver;
};

/*
**	Algorithm produced by H&kon
*/
PRIVATE int default_resolver (const void *a, const void *b)
{
    if (*(int *) a < 0)
        return *(int *) a;
    if (*(int *) b < 0)
        return *(int *) b;
    if (*(int *) a == 0)
        return *(int *) b;
    return *(int *) a;
}

PRIVATE int HTTee_put_character (HTStream * me, char c)
{
    int ret1 = (*me->s1->isa->put_character)(me->s1, c);
    int ret2 = (*me->s2->isa->put_character)(me->s2, c);
    return me->resolver(&ret1, &ret2);
}

PRIVATE int HTTee_put_string (HTStream * me, const char* s)
{
    int ret1 = (*me->s1->isa->put_string)(me->s1, s);
    int ret2 = (*me->s2->isa->put_string)(me->s2, s);
    return me->resolver(&ret1, &ret2);
}

PRIVATE int HTTee_write (HTStream * me, const char* s, int l)
{
    int ret1 = (*me->s1->isa->put_block)(me->s1, s, l);
    int ret2 = (*me->s2->isa->put_block)(me->s2, s, l);
    return me->resolver(&ret1, &ret2);
}

PRIVATE int HTTee_flush (HTStream * me)
{
    int ret1 = (*me->s1->isa->flush)(me->s1);
    int ret2 = (*me->s2->isa->flush)(me->s2);
    return me->resolver(&ret1, &ret2);
}

/*
**	BUGS: We do not handle WOULD_BLOCK here. This is tricky as we don't
**	know which one actually blocked.
*/
PRIVATE int HTTee_free (HTStream * me)
{
    if (me) {
	int ret1 = me->s1 ? (*me->s1->isa->_free)(me->s1) : HT_OK;
	int ret2 = me->s2 ? (*me->s2->isa->_free)(me->s2) : HT_OK;
	int ret = me->resolver(&ret1, &ret2);
	me->s1 = me->s2 = NULL;
	HT_FREE(me);
	return ret;
    }
    return HT_OK;
}

PRIVATE int HTTee_abort (HTStream * me, HTList * e)
{
    if (me) {
	if (me->s1) (*me->s1->isa->abort)(me->s1, e);
	if (me->s2) (*me->s2->isa->abort)(me->s2, e);
	me->s1 = me->s2 = NULL;
	HT_FREE(me);
    }
    return HT_ERROR;
}


/*	Tee stream
**	----------
*/
PRIVATE const HTStreamClass HTTeeClass =
{		
	"Tee",
	HTTee_flush,
	HTTee_free,
	HTTee_abort,
	HTTee_put_character, 	HTTee_put_string,
	HTTee_write
}; 


/*	Tee Stream creation
**	-------------------
**	You can create a T stream using this method. Each stream returns a
**	return value and in order to resolve conflicts in the return code
**	you can specify a resolver callback function. Each time any of the 
**	data methods are called the resolver function is then called with
**	the return codes from the two streams. The return code of the T stream
**	itself will be the result of the resolver function. If you pass NULL
**	as the resolver routine then a default resolver is used.
*/
PUBLIC HTStream * HTTee(HTStream * s1, HTStream * s2, HTComparer * resolver)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(*me))) == NULL)
        HT_OUTOFMEM("HTTee");
    me->isa = &HTTeeClass;
    me->s1 = s1 ? s1 : HTBlackHole();
    me->s2 = s2 ? s2 : HTBlackHole();
    me->resolver = resolver ? resolver : default_resolver;
    HTTRACE(STREAM_TRACE, "Tee......... Created stream %p with resolver %p\n" _ 
		me _ me->resolver);
    return me;
}


