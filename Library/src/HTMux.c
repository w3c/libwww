/*
**	MUX HEADER GENERATOR STREAM
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This stream generates MUX headers.
**
** Authors
**	HFN	Henrik Frystyk Nielsen <frystyk@w3.org>
**
** History:
**	Oct 96	Written
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTMuxHeader.h"
#include "HTMuxCh.h"
#include "HTMux.h"					 /* Implemented here */

#include "HTNetMan.h"

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *			target;
    HTMuxChannel *		muxch;
    HTMuxSession *		session;
    HTMuxSessionId		sid;
    HTNet *			net;
    BOOL			syn;
};

#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

/* ------------------------------------------------------------------------- */

PRIVATE int HTMuxWriter_putBlock (HTStream * me, const char * b, int len)
{
    HTMuxHeader header[2];

    /*
    **  If this is the first time through then also send a SYN bit
    */
    if (me->syn == YES) {
	header[0] = HT_WORDSWAP(MUX_SYN | MUX_LONG_LENGTH |
				MUX_SET_SID(me->sid) | MUX_SET_PID(80));
	header[1] = HT_WORDSWAP(len);

	/* Write the header */
	PUTBLOCK((const char *) header, 8);

	/* Dirty trick to do padding */
	PUTBLOCK(b, MUX_LONG_ALIGN(len));

	me->syn = NO;
    } else if (len <= MUX_LENGTH) {
	header[0] = HT_WORDSWAP(MUX_PUSH | MUX_SET_SID(me->sid) | MUX_SET_LEN(len));

	/* Write the header */
	PUTBLOCK((const char *) header, 4);

	/* Dirty trick to do padding */
	PUTBLOCK(b, MUX_ALIGN(len));
    } else {
	header[0] = HT_WORDSWAP(MUX_LONG_LENGTH | MUX_PUSH | MUX_SET_SID(me->sid));
	header[1] = HT_WORDSWAP(len);

	/* Write the header */
	PUTBLOCK((const char *) header, 8);

	/* Dirty trick to do padding */
	PUTBLOCK(b, MUX_LONG_ALIGN(len));
    }
    return HT_OK;
}

PRIVATE int HTMuxWriter_putString (HTStream * me, const char * s)
{
    return HTMuxWriter_putBlock(me, s, (int) strlen(s));
}

PRIVATE int HTMuxWriter_putCharacter (HTStream * me, char c)
{
    return HTMuxWriter_putBlock(me, &c, 1);
}

PRIVATE int HTMuxWriter_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

/*
**  Sends a FIN packet and frees itself
*/
PRIVATE int HTMuxWriter_free (HTStream * me)
{
    int status = HT_OK;
    HTMuxHeader header = HT_WORDSWAP((MUX_FIN | MUX_SET_SID(me->sid)));
    if ((status = PUTBLOCK((const char *) &header, sizeof(int))) != HT_OK)
	return status;
    HTMuxSession_setClose(me->muxch, me->session, MUX_S_END_WRITE);
    if (PROT_TRACE) HTTrace("MUX Session. FREEING....\n");
    HT_FREE(me);
    return HT_OK;
}

/*
**  Sends a RST packet and frees itself
*/
PRIVATE int HTMuxWriter_abort (HTStream * me, HTList * e)
{
    int status = HT_OK;
    HTMuxHeader header = HT_WORDSWAP((MUX_RST | MUX_SET_SID(me->sid)));
    if ((status = PUTBLOCK((const char *) &header, sizeof(int))) != HT_OK)
	return status;
    HTMuxSession_setClose(me->muxch, me->session, MUX_S_END);
    if (PROT_TRACE) HTTrace("MUX Session. ABORTING....\n");
    HT_FREE(me);
    return HT_OK;
}

PRIVATE const HTStreamClass HTMuxWriterClass =
{		
    "MuxWriter",
    HTMuxWriter_flush,
    HTMuxWriter_free,
    HTMuxWriter_abort,
    HTMuxWriter_putCharacter,
    HTMuxWriter_putString,
    HTMuxWriter_putBlock
};

PUBLIC HTStream * HTMuxWriter_new (HTHost * 	host,
				   HTNet * 	net,
				   HTStream *	target)
{
    if (host && net && target) {
	HTMuxChannel * muxch = HTMuxChannel_find(host);
	HTMuxSession * session = HTMuxChannel_findSession(muxch, net->session);
	HTStream * me = NULL;
	if (!muxch || !session) {
	    if (PROT_TRACE)HTTrace("MUX Session. Bad parameters for output\n");
	    return NULL;
	}
	if (!(me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))))
	    HT_OUTOFMEM("HTMuxWriter_new");
	me->isa = &HTMuxWriterClass;
	me->target = target;
	me->muxch = muxch;
	me->session = session;
	me->sid = HTMuxSession_id(session);
	me->net = net;
	me->syn = me->sid%2;
	if (PROT_TRACE)
	    HTTrace("MUX Session. Created output stream %p for session %p\n",
		    me, me->session);
	return me;
    }
    return NULL;
}
