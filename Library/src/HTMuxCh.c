/*
**	MUX CHANNEL, SESSION AND PROTOCOL MANAGEMENT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Handles a MUX Channel with sessions and protocols
**
** Authors
**	HFN	Henrik Frystyk Nielsen <frystyk@w3.org>
**
** HISTORY:
**	Oct 96 HFN	Written
*/

/* Library Include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWTrans.h"
#include "WWWStream.h"
#include "HTMuxTx.h"
#include "HTMuxHeader.h"
#include "HTDemux.h"
#include "HTMuxCh.h"					 /* Implemented here */

#define MAX_SESSIONS	0xFF	   			 /* Max 256 sessions */

#define SID_BASE	2
#define RECEIVER_OFFSET	0			   /* Client control session */
#define SENDER_OFFSET	1			   /* Server control session */

struct _HTStream {
    const HTStreamClass *	isa;
    /* ... */
};

struct _HTOutputStream {
    const HTOutputStreamClass *	isa;
    /* ... */
};

#define PUTBLOCK(b,l) (*me->isa->put_block)(me,(b),(l))

struct _HTMuxProtocol {
    HTAtom *	       	name;
    HTProtocolId	pid;
};

struct _HTMuxSession {
    HTMuxSessionId	sid;
    HTProtocolId	pid;
    HTNet *		net;

    /* State */
    HTMuxClose		close;			  /* State of the connection */
    int			credit;				 /* Available credit */
    int			fragment;			    /* Fragment size */
    int			read;	               /* Data read sine last credit */

    /* Input */
    HTStream *		buffer;			/* If we have to buffer data */
    BOOL		buffering;
};

struct _HTMuxChannel {
    int			hash;
    HTHost *		host;
    int			max_sid;		/* A la max_sockfd in select */
    HTNet *		net;
    HTList *		protocols;              /* List of defined protocols */
    HTMuxSession *	sessions[MAX_SESSIONS];
};

PRIVATE HTList	** muxchs = NULL;		       /* List of mux muxchs */

/* ------------------------------------------------------------------------- */

PRIVATE HTMuxSession * session_new (void)
{
    HTMuxSession * me;
    if ((me = (HTMuxSession *) HT_CALLOC(1, sizeof(HTMuxSession))) == NULL)
	HT_OUTOFMEM("HTMuxSession_new");
    me->credit = DEFAULT_CREDIT;
    return me;
}

PRIVATE BOOL session_delete (HTMuxSession * session)
{
    if (session) {
	HT_FREE(session);
	return YES;
    }
    return NO;
}

PUBLIC HTMuxSession * HTMuxSession_register (HTMuxChannel * muxch,
					     HTMuxSessionId sid, HTProtocolId pid)
{
    if (muxch) {
	HTMuxSession * session = muxch->sessions[sid];
	if (session == NULL) {
	    session = session_new();
	    session->sid = sid;
	    session->pid = pid;
	    muxch->sessions[sid] = session;
	    HTTRACE(MUX_TRACE, "Mux Channel. Registered session %d on channel %p\n" _ 
			sid _ muxch);
	}
	return session;
    }
    HTTRACE(MUX_TRACE, "Mux Channel. Can't register new session\n");
    return NULL;
}

PUBLIC HTMuxSessionId HTMuxSession_accept (HTMuxChannel * muxch, HTNet * net,
					   HTProtocolId pid)
{
    if (muxch && net) {
	HTMuxSession * session;
	HTMuxSessionId sid = SID_BASE + RECEIVER_OFFSET;
	for (; sid<MAX_SESSIONS; sid+=2) {
	    if ((session = muxch->sessions[sid]) &&
		session->net == NULL && session->pid == pid) {
		HTTRACE(MUX_TRACE, "Mux Channel. Accepting session %d on channel %p\n" _ 
			    sid _ muxch);
		return sid;
	    }
	}
    }
    HTTRACE(MUX_TRACE, "Mux Channel. Can't accept new session\n");
    return INVSID;
}

PUBLIC HTMuxSessionId HTMuxSession_connect (HTMuxChannel * muxch, HTNet * net,
					    HTProtocolId pid)
{
    if (muxch && net) {
	HTMuxSessionId sid = SID_BASE + SENDER_OFFSET;
	for (; sid<MAX_SESSIONS; sid+=2) {
	    if (muxch->sessions[sid] == NULL) {
		HTMuxSession * session = session_new();
		session->sid = sid;
		session->pid = pid;
		session->net = net;		
		muxch->sessions[sid] = session;
		HTTRACE(MUX_TRACE, "Mux Channel. Creating session %d on channel %p\n" _ 
			    sid _ muxch);
		return sid;
	    }
	}
    }
    HTTRACE(MUX_TRACE, "Mux Channel. Can't create new session\n");
    return INVSID;
}

PUBLIC int HTMuxSession_close (HTMuxChannel * muxch, HTMuxSessionId sid)
{
    if (muxch) {
	HTMuxSession * session = muxch->sessions[sid];
	HTMuxSession_setClose(muxch, session, MUX_S_END_WRITE);
	return YES;
    }
    return NO;
}

PUBLIC HTMuxSessionId HTMuxSession_id (HTMuxSession * session)
{
    return session ? session->sid : INVSID;
}

PUBLIC HTProtocolId HTMuxSession_pid (HTMuxSession * session)
{
    return session ? session->pid : INVPID;
}

PUBLIC HTNet * HTMuxSession_net (HTMuxSession * session)
{
    return session ? session->net : NULL;
}

PUBLIC BOOL HTMuxSession_setClose (HTMuxChannel * muxch,
				   HTMuxSession * session, HTMuxClose close)
{
    if (muxch && session) {
	session->close |= close;

	/*
	**  If both directions are closed down then we can put the session
	**  to sleep.
	*/
	if (session->close == MUX_S_END) {
	    HTTRACE(MUX_TRACE, "Mux Channel. Closing session %d on channel %p\n" _ 
			session->sid _ muxch);
	    muxch->sessions[session->sid] = NULL;
	    session_delete(session);
	}
	return YES;
    }
    return NO;
}

PUBLIC int  HTMuxSession_credit (HTMuxSession * session)
{
    return session ? session->credit : -1;
}

PUBLIC BOOL HTMuxSession_setCredit (HTMuxChannel * muxch,
				    HTMuxSessionId sid, int credit)
{
    HTMuxSession * session;
    if (muxch && (session = muxch->sessions[sid])) {
	session->credit = credit;
	return YES;
    }
    return NO;
}

PUBLIC int  HTMuxSession_fragment (HTMuxSession * session)
{
    return session ? session->fragment : -1;
}

PUBLIC BOOL HTMuxSession_setFragment (HTMuxChannel * muxch,
				      HTMuxSessionId sid, int fragment)
{
    HTMuxSession * session;
    if (muxch && (session = muxch->sessions[sid])) {
	session->fragment = fragment;
	return YES;
    }
    return NO;
}

/*
**  Tries really hard to get rid of the data.
**  Returns:
**	-1 Error
**	 0 Buffered the data
**       1 Got rid of the data
*/
PUBLIC int HTMuxSession_disposeData (HTMuxSession * me, const char * buf, int len)
{
    HTTRACE(MUX_TRACE, "Mux Channel. Writing %d bytes to session %p\n" _ len _ me);

    /*
    **  There are two situations that can occur: Either we have an accepted session
    **  with a Net object or we have an unaccepted session with no Net object. In
    **  the former case we try to get rid of the data by pushing it directly to the
    **  read stream of the Net object. In the latter case we buffer as much as we
    **  can.
    */
    if (me) {	
	HTNet * net;
	HTStream * sink = NULL;
	int status;
	if ((net = me->net) && (sink = HTNet_readStream(net))) {

	    /*
	    **  Look first to see if we have old data that we can dispose down
	    **  the sink. We keep the buffer stream so that we can reuse it later.
	    */
	    if (me->buffer && me->buffering) {
		if ((*me->buffer->isa->flush)(me->buffer) == HT_OK) {
		    HTTRACE(MUX_TRACE, "Mux Channel. Flushed buffered data\n");
		    me->buffering = NO;
		} else if ((*me->buffer->isa->put_block)(me->buffer, buf, len) >= 0) {
		    HTTRACE(MUX_TRACE, "Mux Channel. Buffer accepted data\n");
		    return 0;
		}
		HTTRACE(MUX_TRACE, "Mux Channel. Can't buffer data\n");
		return (-1);		    
	    }

	    /*
	    **  See if we can get rid of the new data. If not then try to buffer it.
	    **  If this also fails then we reset the channel. A positive return code
	    **  from the stream means that we got rid of the data successfully.
	    */
	    if ((status = (*sink->isa->put_block)(sink, buf, len)) >= 0) {
		HTTRACE(MUX_TRACE, "Mux Channel. Stream returned %d\n" _ status);
		
		/*
		**  If we get back a HT_LOADED then we have all the data we need
		**  and we can terminate the request
		*/
		if (status == HT_LOADED) {
		    HTNet_execute (net, HTEvent_END);
		    return 0;
		}

		/*
		**  Decide whether we should send a credit message
		**  MORE TO COME
		*/
		me->read += len;
		if (me->read >= DEFAULT_CREDIT / 2) {
		    me->read = 0;
		    return 1;
		}
		return 0;
	    }
	}

	/*
	**  The stream is not ready and we try to buffer the data in
	**  the meantime.
	*/
	if (!me->buffer) {
	    me->buffer = HTPipeBuffer(sink, DEFAULT_CREDIT);
	    me->buffering = YES;
	}
	status = (*me->buffer->isa->put_block)(me->buffer, buf, len);
	if (status >= 0) {
	    HTTRACE(MUX_TRACE, "Mux Channel. Buffer accepted data\n");
	    return 0;
	}
	HTTRACE(MUX_TRACE, "Mux Channel. Buffer returned %d\n" _ status);
    }
    return (-1);
}

/* ------------------------------------------------------------------------- */

PRIVATE BOOL channel_delete (HTMuxChannel * ch)
{
    if (ch) {
	HT_FREE(ch);
	return YES;
    }
    return NO;
}

PUBLIC HTMuxChannel * HTMuxChannel_new (HTHost * host)
{
    if (host) {
	HTMuxChannel * me = NULL;

	/* Create new object */
	if ((me = (HTMuxChannel *) HT_CALLOC(1, sizeof(HTMuxChannel))) == NULL)
	    HT_OUTOFMEM("HTMuxChannel_new");
	me->hash = HTHost_hash(host);
	me->host = host;

	/*
	**  Make sure that we are in interleave mode
	*/
	HTHost_setMode(host, HT_TP_INTERLEAVE);

	/*
	**  Get a special MUX Net object that we keep to our selves. We don't
	**  associate a request object as the Net object lives longer.
	*/
	me->net = HTNet_new(NULL);
	HTNet_setReadStream(me->net, HTDemux_new(host, me));

	/* Insert into hash table */
	if (!muxchs) {
	    if ((muxchs=(HTList **) HT_CALLOC(HOST_HASH_SIZE, sizeof(HTList *))) == NULL)
		HT_OUTOFMEM("HTMuxChannel_new");
	}
	if (!muxchs[me->hash]) muxchs[me->hash] = HTList_new();
	HTList_addObject(muxchs[me->hash], (void *) me);
	HTTRACE(MUX_TRACE, "Mux Channel. %p created with hash %d\n" _ me _ me->hash);
	return me;
    }
    return NULL;
}

PUBLIC HTMuxChannel * HTMuxChannel_find (HTHost * host)
{
    if (muxchs) {
	int hash = HTHost_hash(host);
	HTList * list = muxchs[hash];
	if (list) {
	    HTMuxChannel * pres = NULL;
	    while ((pres = (HTMuxChannel *) HTList_nextObject(list)))
		if (pres->host == host) return pres;
	}
    }
    return NULL;
}

PUBLIC BOOL HTMuxChannel_delete (HTMuxChannel * me)
{
    if (me) {
	HTList * list = NULL;
	HTTRACE(MUX_TRACE, "Mux Channel. Deleting %p\n" _ me);
	if (muxchs && (list = muxchs[me->hash])) {
	    HTList_removeObject(list, (void *) me);
	    channel_delete(me);
	    return YES;
	}
    }
    return NO;
}

PUBLIC BOOL HTMuxChannel_deleteAll (void)
{
    if (muxchs) {
	HTList * cur;
	int cnt;
	for (cnt=0; cnt<HOST_HASH_SIZE; cnt++) {
	    if ((cur = muxchs[cnt])) { 
		HTMuxChannel * pres;
		while ((pres = (HTMuxChannel *) HTList_nextObject(cur)))
		    channel_delete(pres);
	    }
	    HTList_delete(muxchs[cnt]);
	}
	HT_FREE(muxchs);
    }
    return YES;
}

PUBLIC HTNet * HTMuxChannel_net (HTMuxChannel * me)
{
    return me ? me->net : NULL;
}

PUBLIC HTMuxSession * HTMuxChannel_findSession (HTMuxChannel * me, HTMuxSessionId sid)
{
    return (me) ? me->sessions[sid] : NULL;
}

#if 0
PRIVATE HTMuxSession * HTMuxChannel_findSessionFromNet (HTMuxChannel * me, HTNet * net)
{
    if (me && net) {
	int cnt = 0;
	HTMuxSession **session = me->sessions;
	while (cnt<MAX_SESSIONS) {
	    if (**session->net == net) return *session;
	    session++, cnt++;
	}	    
    }
    return NULL;
}
#endif

PUBLIC HTHost * HTMuxChannel_host (HTMuxChannel * muxch)
{
    return muxch ? muxch->host : NULL;
}

PUBLIC int HTMuxChannel_sendControl (HTMuxChannel * muxch, HTMuxSessionId sid,
				     HTMuxHeader opcode, int value,
				     const void * param, int param_size)
{
    if (muxch && muxch->host) {
	HTOutputStream * me = HTChannel_output(HTHost_channel(muxch->host));
	HTMuxHeader header[2];
	switch (opcode) {
	case MUX_STRING:
	    if (param && param_size) {
		header[0] = HT_WORDSWAP(MUX_CONTROL | MUX_LONG_LENGTH | MUX_SET_LEN(value));
		header[1] = HT_WORDSWAP(param_size);
		PUTBLOCK((const char *) header, 8);
		PUTBLOCK((const char *) param, MUX_LONG_ALIGN(param_size));
	    }
	    break;
	case MUX_STACK:
	    if (param && param_size) {
		header[0] = HT_WORDSWAP(MUX_CONTROL | MUX_LONG_LENGTH | MUX_SET_LEN(value));
		header[1] = HT_WORDSWAP(param_size);
		PUTBLOCK((const char *) header, 8);
		PUTBLOCK((const char *) param, MUX_LONG_ALIGN(param_size));
	    }
	    break;
	case MUX_FRAGMENT:
	    header[0] = HT_WORDSWAP(MUX_CONTROL | MUX_SET_SID(sid) | MUX_SET_LEN(value));
	    PUTBLOCK((const char *) header, 4);
	    break;
	case MUX_CREDIT:
	    header[0] = HT_WORDSWAP(MUX_CONTROL | MUX_LONG_LENGTH | MUX_SET_SID(sid));
	    header[1] = HT_WORDSWAP(value);
	    PUTBLOCK((const char *) header, 8);
	    break;
	default:
	    HTTRACE(MUX_TRACE, "Mux Channel. UNKNOWN OPCODE %d\n" _ opcode);
	    return HT_ERROR;
	}

	/* Flush for now */
#if 1
	return (*me->isa->flush)(me);
#else
	return HT_OK;
#endif
    }
    return HT_ERROR;
}

/* ------------------------------------------------------------------------- */

PUBLIC BOOL HTMuxProtocol_add (HTMuxChannel * muxch,
			       HTProtocolId pid, const char * protocol)
{
    if (muxch && protocol) {	
	HTMuxProtocol * ms;
	if ((ms = (HTMuxProtocol *) HT_CALLOC(1, sizeof(HTMuxProtocol))) == NULL)
	    HT_OUTOFMEM("HTMuxProtocol_new");
	ms->name = HTAtom_caseFor(protocol);
	ms->pid = pid;
	if (!muxch->protocols) muxch->protocols = HTList_new();
	return HTList_addObject(muxch->protocols, ms);
    }
    return NO;
}

PUBLIC BOOL HTMuxProtocol_delete (HTMuxChannel * muxch, HTProtocolId pid)
{
    if (muxch && muxch->protocols) {
	HTList * cur = muxch->protocols;
	HTMuxProtocol * pres;
	while ((pres = (HTMuxProtocol *) HTList_nextObject(cur))) {
	    if (pres->pid == pid) {
		HTList_removeObject(muxch->protocols, pres);
		HT_FREE(pres);
		return YES;
	    }
	}
    }
    return NO;
}
