/*								     HTNetTxt.c
**	NETWORK TELNET TO INTERNAL CHARACTER TEXT AND VISE VERSA
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module provides two basic streams that converts from CRLF to
**	an internal C representation (\n) and from the C representation to
**	CRLF.
**
** HISTORY:
**	27 Mar 95  HFN	Spawned off from HTFormat.c
*/

/* Library Include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTStream.h"
#include "HTNetTxt.h"					 /* Implemented here */

/* Typedefs and global variable local to this module */
struct _HTStream {
    const HTStreamClass *	isa;
    HTStream * 			target;
    const char *		start;
    BOOL			had_cr;
};

#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

/* ------------------------------------------------------------------------- */

/*	Converter from CRLF to \n
**	-------------------------
**	The input is assumed to be in local representation with lines
**	delimited by (CR,LF) pairs in the local representation.
**	The conversion to local representation is always done in HTSocket.c
**	The (CR,LF) sequence when found is changed to a '\n' character,
**	the internal C representation of a new line.
*/
PRIVATE int NetToText_put_block (HTStream * me, const char * s, int l)
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

PRIVATE int NetToText_put_character (HTStream * me, char c)
{
    return NetToText_put_block(me, &c, 1);
}

PRIVATE int NetToText_put_string (HTStream * me, const char * s)
{    
    return NetToText_put_block(me, s, (int) strlen(s));
}

PRIVATE int Net_flush (HTStream * me)
{
    return me->target ? (*me->target->isa->flush)(me->target) : HT_OK;
}

PRIVATE int Net_free (HTStream * me)
{
    int status = HT_OK;
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    HT_FREE(me);
    return status;
}

PRIVATE int Net_abort (HTStream * me, HTList * e)
{
    if (me->target)
	(*me->target->isa->abort)(me->target, e);
    HT_FREE(me);
    return HT_ERROR;
}

PRIVATE HTStreamClass NetToTextClass = {
    "NetToText",
    Net_flush,
    Net_free,
    Net_abort,
    NetToText_put_character,
    NetToText_put_string,
    NetToText_put_block
};

PUBLIC HTStream * HTNetToText (HTStream * target)
{
    HTStream* me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("NetToText");
    me->isa = &NetToTextClass;
    me->had_cr = NO;
    me->target = target;
    return me;
}

/*	Converter from \n to CRLF
**	-------------------------
**	The input is assumed to be in local representation with lines
**	delimited by \n. The \n when found is changed to a CRLF sequence,
**	the network representation of a new line.
**	Conversion: '\r' is stripped and \n => CRLF
*/
PRIVATE int TextToNet_put_block (HTStream * me, const char* b, int len)
{
    int status;
#if 0
    const char *limit = b+len;
#endif    
    if (!me->start)
	me->start = b;
    else {
	len -= (me->start - b);
	b = me->start;
    }
    while (len-- > 0) {
	if (me->had_cr && *b == LF) {
	    if (b > me->start+1) {
		if ((status = PUTBLOCK(me->start, b - me->start-1)) != HT_OK)
		    return status;
	    }
	    me->start = b+1;
	    if ((status = PUTC('\n')) != HT_OK)
		return status;
	}
	me->had_cr = (*b++ == CR);
    }
    if (me->start < b && (status = PUTBLOCK(me->start, b-me->start)) != HT_OK)
	return status;
    me->start = NULL;			      /* Whole buffer is written :-) */
    return HT_OK;
}

PRIVATE int TextToNet_put_character (HTStream * me, char c)
{
    return TextToNet_put_block(me, &c, 1);
}

PRIVATE int TextToNet_put_string (HTStream * me, const char * s)
{    
    return TextToNet_put_block(me, s, (int) strlen(s));
}

PRIVATE HTStreamClass TextToNetClass = {
    "TextToNet",
    Net_flush,
    Net_free,
    Net_abort,
    TextToNet_put_character,
    TextToNet_put_string,
    TextToNet_put_block
};

PUBLIC HTStream * HTTextToNet (HTStream * target)
{
    HTStream* me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("TextToNet");
    me->isa = &TextToNetClass;
    me->had_cr = NO;
    me->target = target;
    return me;
}




