/*
**	MUX HEADER PARSER STREAM
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This stream parses MUX headers.
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
#include "HTDemux.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
    HTHost *			host;
    HTMuxChannel *		muxch;
    HTMuxSession *		session;
    int				next;			      /* Next header */
};

/* ------------------------------------------------------------------------- */

/*
**  Parse through the buffer, identify the headers, and distribute the
**  data on the virtual session input streams
*/
PRIVATE int HTDemux_write (HTStream * me, const char * buf, int len)
{
    HTMuxChannel * muxch = me->muxch;
    int length = 0;
    while (len > 0) {

	/*
	**  Look for the next header. If this is the first time through then
	**  we expect the stream of data to start with a header.
	*/
	if (!me->next) {
	    HTMuxHeader header[2];
	    HTMuxSessionId sid = INVSID;
	    header[0] = HT_WORDSWAP(*(HTMuxHeader *) buf);

	    /*
	    **  If the long bit is set then we expect another 32 bits as the
	    **  length of the payload
	    */
	    if (MUX_IS_LONG(header[0])) {
		header[1] = HT_WORDSWAP(*(HTMuxHeader *) (buf+1));
		me->next = MUX_LONG_ALIGN(header[1]);
		buf += 8, len -= 8;
	    } else {
		me->next = MUX_ALIGN(MUX_GET_LEN(header[0]));
		buf += 4, len -= 4;
	    }
	    length = HTMIN(len, me->next);
	    sid = MUX_GET_SID(header[0]);
	    HTTRACE(MUX_TRACE, "Demux stream Header: %x, sid %d, length %d\n" _ header[0] _ length _ sid);

	    /*
	    **  If this is a control message then handle it here
	    */
	    if (header[0] & MUX_CONTROL) {
		if (header[0] & MUX_STRING) {

		    ; /* Define the string */
		    
		} else if (header[0] & MUX_STACK) {
		
		    ; /* Define the stack */

		} else if (header[0] & MUX_FRAGMENT) {
		    HTMuxSession_setFragment(muxch, sid, MUX_GET_LEN(header[0]));
		} else if (header[0] & MUX_CREDIT) {
		    HTMuxSession_setCredit(muxch, sid, header[1]);
		}
	    } else if (header[0] & MUX_SYN) {
		me->session = HTMuxSession_register(muxch, sid, MUX_GET_PID(header[0]));
	    } else if (header[0] & MUX_FIN) {
		me->session = HTMuxChannel_findSession(muxch, sid);
		HTMuxSession_setClose(me->muxch, me->session, MUX_S_END_READ);
	    } else
		me->session = HTMuxChannel_findSession(muxch, sid);
	}

	/*
	**  If there is data on this session then handle it here.
	*/
	if (me->next && me->session) {
	    if (HTMuxSession_disposeData(me->session, buf, length) == 1) {
		HTMuxChannel_sendControl(muxch, HTMuxSession_id(me->session),
					 MUX_CREDIT, DEFAULT_CREDIT, NULL, 0);
	    }
 	}
	buf += length, len -= length, me->next -= length;
    }
    return HT_OK;
}

PRIVATE int HTDemux_put_character (HTStream * me, char c)
{
    return HTDemux_write(me, &c, 1);
}

PRIVATE int HTDemux_put_string (HTStream * me, const char * s)
{
    return HTDemux_write(me, s, (int) strlen(s));
}

PRIVATE int HTDemux_flush (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTDemux_free (HTStream * me)
{
    return HT_IGNORE;
}

PRIVATE int HTDemux_abort (HTStream * me, HTList * e)
{
    return HT_IGNORE;
}

PRIVATE const HTStreamClass HTDemux =
{		
    "Demux",
    HTDemux_flush,
    HTDemux_free,
    HTDemux_abort,
    HTDemux_put_character,
    HTDemux_put_string,
    HTDemux_write
}; 

PUBLIC HTStream * HTDemux_new (HTHost * host, HTMuxChannel * muxch)
{
    HTStream * me = NULL;
    if (muxch) {
	if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	    HT_OUTOFMEM("HTDemux_new");
	me->isa = &HTDemux;
	me->host = host;
	me->muxch = muxch;	
    }
    return me;
}
