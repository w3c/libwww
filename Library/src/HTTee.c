/*									 HTee.c
**	TEE CLASS STREAM DEFINITION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	The Tee class just writes to two streams.  Easy.
**	See also the Black Hole stream which is even easier.
**
** HISTORY:
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTTee.h"

/*		Stream Object
**		------------
*/

struct _HTStream {
	CONST HTStreamClass *	isa;
	
	HTStream *		s1;
	HTStream *		s2;
};


PRIVATE int HTTee_put_character ARGS2(HTStream *, me, char, c)
{
    int ret1 = (*me->s1->isa->put_character)(me->s1, c);
    int ret2 = (*me->s2->isa->put_character)(me->s2, c);
    return (!(ret1+ret2) ? HT_OK :
	    (ret1==HT_ERROR || ret2==HT_ERROR) ? HT_ERROR :
	    HT_WOULD_BLOCK);
}

PRIVATE int HTTee_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    int ret1 = (*me->s1->isa->put_string)(me->s1, s);
    int ret2 = (*me->s2->isa->put_string)(me->s2, s);
    return (!(ret1+ret2) ? HT_OK :
	    (ret1==HT_ERROR || ret2==HT_ERROR) ? HT_ERROR :
	    HT_WOULD_BLOCK);
}

PRIVATE int HTTee_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    int ret1 = (*me->s1->isa->put_block)(me->s1, s, l);
    int ret2 = (*me->s2->isa->put_block)(me->s2, s, l);
    return (!(ret1+ret2) ? HT_OK :
	    (ret1==HT_ERROR || ret2==HT_ERROR) ? HT_ERROR :
	    HT_WOULD_BLOCK);
}

PRIVATE int HTTee_flush ARGS1(HTStream *, me)
{
    int ret1 = (*me->s1->isa->flush)(me->s1);
    int ret2 = (*me->s2->isa->flush)(me->s2);
    return (!(ret1+ret2) ? HT_OK :
	    (ret1==HT_ERROR || ret2==HT_ERROR) ? HT_ERROR :
	    HT_WOULD_BLOCK);
}

PRIVATE int HTTee_free ARGS1(HTStream *, me)
{
    int ret1 = (*me->s1->isa->_free)(me->s1);
    int ret2 = (*me->s2->isa->_free)(me->s2);
    free(me);
    return (!(ret1+ret2) ? HT_OK :
	    (ret1==HT_ERROR || ret2==HT_ERROR) ? HT_ERROR :
	    HT_WOULD_BLOCK);
}

PRIVATE int HTTee_abort ARGS2(HTStream *, me, HTError, e)
{
    (*me->s1->isa->abort)(me->s1, e);
    (*me->s2->isa->abort)(me->s2, e);
    free(me);
    return HT_ERROR;
}


/*	Tee stream
**	----------
*/
PRIVATE CONST HTStreamClass HTTeeClass =
{		
	"Tee",
	HTTee_flush,
	HTTee_free,
	HTTee_abort,
	HTTee_put_character, 	HTTee_put_string,
	HTTee_write
}; 


/*	Tee creation
*/
PUBLIC HTStream * HTTee ARGS2(HTStream *, s1,HTStream *, s2)
{
    HTStream * me = (HTStream *) calloc(1, sizeof(*me));
    if (!me) outofmem(__FILE__, "HTTee");
    me->isa = &HTTeeClass;
    me->s1 = s1;
    me->s2 = s2;
    return me;
}


