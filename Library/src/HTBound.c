/*								     HTBound.c
**	MIME MULTIPART PARSER STREAM
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This stream parses a MIME multipart stream and builds a set of new
**	streams via the stream stack each time we encounter a boundary start.
**	We get the boundary from the normal MIME parser via the Request object
**
** Authors
**	HF	Henrik Frystyk <frystyk@w3.org>
**
** History:
**	Nov 95	Written from scratch
**
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTSocket.h"
#include "HTStream.h"
#include "HTFWrite.h"
#include "HTReqMan.h"
#include "HTBound.h"					 /* Implemented here */

#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)
#define PUTDEBUG(b, l)	(*me->debug->isa->put_block)(me->debug, b, l)
#define FREE_TARGET	(*me->target->isa->_free)(me->target)

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *			target;
    HTStream *			orig_target;
    HTFormat			format;
    HTStream *			debug;		  /* For preamble and epilog */
    HTRequest *			request;
    BOOL			body;		  /* Body or preamble|epilog */
    HTSocketEOL			state;
    int				dash;			 /* Number of dashes */
    char *			boundary;
    char *			bpos;
};

/* ------------------------------------------------------------------------- */

/*
**	Searches for FTP line until buffer fills up or a CRLF or LF is found
*/
PRIVATE int HTBoundary_put_block (HTStream * me, CONST char * b, int l)
{
    CONST char *start = b;
    CONST char *end = b;
    while (l-- > 0) {
	if (me->state == EOL_FCR) {
	    me->state = (*b == LF) ? EOL_FLF : EOL_BEGIN;
	} else if (me->state == EOL_FLF) {
	    if (me->dash == 2) {
		while (l>0 && *me->bpos && *me->bpos==*b) l--, me->bpos++, b++;
		if (!*me->bpos) {
		    if (STREAM_TRACE && !*me->bpos)
			TTYPrint(TDEST, "Boundary.... `%s\' found\n", me->boundary);
		    me->bpos = me->boundary;
		    me->body = YES;
		    me->state = EOL_DOT;
		} else if (l>0) {
		    me->dash = 0;
		    me->bpos = me->boundary;
		    me->state = EOL_BEGIN;
		}
	    }
	    if (*b == '-') {
		me->dash++;
	    } else if (*b != LF) {
		me->dash = 0;
		me->state = EOL_BEGIN;
	    }
	} else if (me->state == EOL_SLF) {	    /* Look for closing '--' */
	    if (me->dash == 4) {
		if (end > start) {
		    int status = PUTBLOCK(start, end-start);
		    if (status != HT_OK) return status;
		}
		if (STREAM_TRACE) TTYPrint(TDEST, "Boundary.... Ending\n");
		start = b;
		me->dash = 0;
		me->state = EOL_BEGIN;
	    }
	    if (*b == '-') {
		me->dash++;
	    } else if (*b != LF) {
		me->dash = 0;
		me->state = EOL_BEGIN;
	    }
	    me->body = NO;
	} else if (me->state == EOL_DOT) {
	    int status;
	    if (me->body) {
		if (me->target) FREE_TARGET;
		me->target = HTStreamStack(WWW_MIME,me->format,me->orig_target,
					   me->request, YES);
		if (end > start) {
		    if ((status = PUTBLOCK(start, end-start)) != HT_OK)
			return status;
		}
	    } else {
		if (me->debug)
		    if ((status = PUTDEBUG(start, end-start)) != HT_OK)
			return status;
	    }
	    start = b;
	    if (*b == '-') me->dash++;
	    me->state = EOL_SLF;
	} else if (*b == CR) {
	    me->state = EOL_FCR;
	    end = b;
	} else if (*b == LF) {
	    if (me->state != EOL_FCR) end = b;
	    me->state = EOL_FLF;
	}
	b++;
    }
    return (start<b && me->body) ? PUTBLOCK(start, b-start) : HT_OK;
}

PRIVATE int HTBoundary_put_string (HTStream * me, CONST char * s)
{
    return HTBoundary_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTBoundary_put_character (HTStream * me, char c)
{
    return HTBoundary_put_block(me, &c, 1);
}

PRIVATE int HTBoundary_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTBoundary_free (HTStream * me)
{
    int status = HT_OK;
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    if (PROT_TRACE) TTYPrint(TDEST, "Boundary.... FREEING....\n");
    HT_FREE(me->boundary);
    HT_FREE(me);
    return status;
}

PRIVATE int HTBoundary_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;
    if (me->target) status = (*me->target->isa->abort)(me->target, e);
    if (PROT_TRACE) TTYPrint(TDEST, "Boundary.... ABORTING...\n");
    HT_FREE(me->boundary);
    HT_FREE(me);
    return status;
}

PRIVATE CONST HTStreamClass HTBoundaryClass =
{		
    "HTBoundary",
    HTBoundary_flush,
    HTBoundary_free,
    HTBoundary_abort,
    HTBoundary_put_character,
    HTBoundary_put_string,
    HTBoundary_put_block
};

PUBLIC HTStream * HTBoundary   (HTRequest *	request,
				void *		param,
				HTFormat	input_format,
				HTFormat	output_format,
				HTStream *	output_stream)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTBoundary");
    me->isa = &HTBoundaryClass;
    me->request = request;
    me->format = output_format;
    me->orig_target = output_stream;
    me->debug = HTRequest_debugStream(request);
    me->state = EOL_FLF;
    if (request->boundary) {
	StrAllocCopy(me->boundary, request->boundary);	       /* Local copy */
	me->bpos = me->boundary;
	if (STREAM_TRACE)
	    TTYPrint(TDEST,"Boundary.... Stream created with boundary '%s\'\n", me->boundary);
	return me;
    } else {
	if (STREAM_TRACE) TTYPrint(TDEST, "Boundary.... <UNKNOWN>\n");
	HT_FREE(me);
	return HTErrorStream();
    }
}
