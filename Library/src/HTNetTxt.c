/*								     HTNetTxt.c
**	NETWORK TELNET TO INTERNAL CHARACTER TEXT AND VISE VERSA
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	The input is assumed to be in local representation with lines
**	delimited by (CR,LF) pairs in the local representation.
**	The (CR,LF) sequenc when found is changed to a '\n' character,
**	the internal C representation of a new line.
**
** HISTORY:
**	27 Mar 95  HFN	Spawned off from HTFormat.c
*/

/* Library Include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTStream.h"
#include "HTNetTxt.h"					 /* Implemented here */

/* Typedefs and global variable local to this module */
struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream * 			target;
    CONST char *		start;
    BOOL			had_cr;
};

#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

/* ------------------------------------------------------------------------- */

PRIVATE int NetToText_put_character ARGS2(HTStream *, me, char, c)
{
    return PUTBLOCK(&c, 1);
}

PRIVATE int NetToText_put_string ARGS2(HTStream *, me, CONST char *, s)
{    
    return PUTBLOCK(s, (int) strlen(s));
}

PRIVATE int NetToText_put_block ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    int status;
    if (!me->start)
	me->start = s;
    else {
	l -= (me->start - s);
	s = me->start;
    }
    while (l-- > 0) {
	if (me->had_cr && *s == LF) {
	    if (s > me->start+1) {
		if ((status = PUTBLOCK(me->start, s - me->start-1)) != HT_OK)
		    return status;
	    }
	    me->start = s+1;
	    if ((status = PUTC('\n')) != HT_OK)
		return status;
	}
	me->had_cr = (*s++ == CR);
    }
    if (me->start < s && (status = PUTBLOCK(me->start, s-me->start)) != HT_OK)
	return status;
    me->start = NULL;			      /* Whole buffer is written :-) */
    return HT_OK;
}

PRIVATE int NetToText_flush ARGS1(HTStream *, me)
{
    return me->target->isa->flush(me->target);
}

PRIVATE int NetToText_free ARGS1(HTStream *, me)
{
    me->target->isa->_free(me->target);		       /* Close rest of pipe */
    free(me);
    return HT_OK;
}

PRIVATE int NetToText_abort ARGS2(HTStream *, me, HTError, e)
{
    me->target->isa->abort(me->target,e);	       /* Abort rest of pipe */
    free(me);
    return HT_ERROR;
}

PRIVATE HTStreamClass NetToTextClass = {
    "NetToText",
    NetToText_flush,
    NetToText_free,
    NetToText_abort,
    NetToText_put_character,
    NetToText_put_string,
    NetToText_put_block
};

PUBLIC HTStream * HTNetToText ARGS1(HTStream *, target)
{
    HTStream* me = (HTStream *) calloc(1, sizeof(*me));
    if (me == NULL) outofmem(__FILE__, "NetToText");
    me->isa = &NetToTextClass;
    
    me->had_cr = NO;
    me->target = target;
    return me;
}

