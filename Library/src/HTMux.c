/*								     HTMux.c
**	W3MUX MULTIPLEX AND DEMULTIPLEX STREAMS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This stream generates and parses SCP Mux headers.
**
** Authors
**	HF	Henrik Frystyk <frystyk@w3.org>
**
** History:
**	Mar 96	Written from scratch
**
*/

/* Library include files */
#include "sysdep.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTAlert.h"
#include "HTNetMan.h"
#include "HTFWrite.h"
#include "HTStream.h"
#include "HTMux.h"					 /* Implemented here */

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#if (!defined(hpux))
#define HT_SWAP_WORD(src,dst) { \
     out = ROTATE_LEFT((src),16); \
     other = out >> 8; \
     other &= 0x00ff00ff; \
     out &= 0x00ff00ff; \
     out <<= 8; \
     (dst) = out | other; \
     }
#else
#define HT_SWAP_WORD(src,dst) { \
     (dst) = (ROTATE_LEFT((src),8) & 0x00ff00ff) | ROTATE_LEFT((src) & 0x00ff00ff,24); \
     }
#endif

#if 0
#define SWAP_WORD(a) ( ((a) << 24) | \
		      (((a) << 8) & 0x00ff0000) | \
		      (((a) >> 8) & 0x0000ff00) | \
	((ilu_cardinal)(a) >>24) )
#endif

#define MUX_LONG_LENGTH	0x80000000
#define MUX_SYN		0x40000000
#define MUX_FIN		0x20000000
#define MUX_RST		0x10000000
#define MUX_PSH 	0x08000000
#define MUX_UNUSED	0x04000000
#define MUX_SESSION	0x03FB0000
#define MUX_LENGTH	0x0003FFFF

#define MAX_SESSIONS		0xFF			 /* Max 256 sessions */
#define CLIENT_CTRL		0		   /* Client control session */
#define SERVER_CTRL		1		   /* Server control session */

typedef int HTSessionId;
typedef unsigned int flagbit;
typedef struct _HTW3Mux HTW3Mux;

typedef struct _HTMuxHdr {
    union {
	struct {
	    flagbit		long_len: 1;
	    flagbit		syn	: 1;
	    flagbit		fin	: 1;
	    flagbit		rst	: 1;
	    flagbit		psh	: 1;
	    unsigned int 	sid	: 8;
	    unsigned int 	size	: 18;
	} data;
	struct {
	    flagbit		ctrl	: 1;
	    unsigned int	type	: 2;
	    /* more to come */
	} ctrl;
    } c;
} HTMuxHdr;

struct _HTW3Mux {
    HTW3Mux *		next;
    SOCKET 		sockfd;			       /* Socket  identifier */
    HTSessionId		ctrl;			       /* Control session id */
    int			nexthead;   /* number of bytes until next mux header */
    HTStream *		sessions [MAX_SESSIONS];
};

struct _HTStream {
    const HTStreamClass *	isa;
    HTSessionId			sid;
    HTW3Mux *			scp;
    HTStream *			target;    
    HTStream *			overflow;	/* Buffer for input overflow */
    int				state;		/* State of the input stream */
    BOOL			transparent;
};

/* Number of entries in our hash table of channels */
#define PRIME_TABLE_SIZE	67
#define HASH(s)			((s) % PRIME_TABLE_SIZE) 

PRIVATE HTW3Mux * SesCtrl [PRIME_TABLE_SIZE];

/* ------------------------------------------------------------------------- */
/*				W3MUX CONTROL				     */
/* ------------------------------------------------------------------------- */

/*
**	A session is uniquely identified by a (socket, session id) pair.
**	Before creating a new session and or a channel, we check to see if
**	either already exists.
**	Returns YES if OK else NO.
*/
PRIVATE BOOL HTW3Mux_new (SOCKET sockfd, BOOL active)
{
    if (sockfd != INVSOC) {
	int hash = HASH(sockfd);
	HTW3Mux *ss=NULL, **ssp=NULL;
	for (ssp = &SesCtrl[hash]; (ss = *ssp) != NULL ; ssp = &ss->next) {
	    if (ss->sockfd == sockfd) break;
	}
	if (!ss) {
	    if ((*ssp = ss =(HTW3Mux *) HT_CALLOC(1, sizeof(HTW3Mux))) == NULL)
		HT_OUTOFMEM("HTW3Mux_new");	    
	    ss->sockfd = sockfd;
	    ss->ctrl = active ? CLIENT_CTRL : SERVER_CTRL;
	    if (PROT_TRACE)
		HTTrace("W3mux...... Created %p with id %d\n", ss,ss->sockfd);
	}
    }
    return YES;
}

/*
**	Find a new session ID for this stream. If no entries are available then
**	return o which is always an invalid session number
*/
PRIVATE HTSessionId HTW3Mux_sid (HTW3Mux * scp, HTStream * stream)
{
    if (scp && stream) {
	int sid = scp->ctrl+2;
	HTStream ** mp = scp->sessions;
	for (;sid < MAX_SESSIONS && *mp==NULL; sid+=2, mp+=2);
	if (!*mp) *mp = stream;
	return sid;
    }
    if (PROT_TRACE) HTTrace("W3mux...... No available entries\n");
    return 0;				      /* Not a valid data session ID */
}

/*
**	Look for a session object with this session id and return the 
**	associated stream if found
*/
PRIVATE HTStream * HTW3Mux_find (HTW3Mux * scp, HTSessionId sid)
{
    return scp ? *(scp->sessions+sid) : NULL;
}

PRIVATE BOOL HTW3Mux_delete (HTW3Mux * scp)
{

    /* @@@ */

}

/* ------------------------------------------------------------------------- */
/*			      MULTIPLEXING STREAM 			     */
/* ------------------------------------------------------------------------- */

PRIVATE int HTMux_put_block (HTStream * me, const char * b, int len)
{
    HTMuxHdr hdr;
    memset(&hdr, '\0', sizeof(hdr));
    if (!me->transparent) {
	if ((me->sid = HTW3Mux_sid(me->scp, me)) == 0) return HT_PAUSE;
	hdr.c.data.syn = 1;
	me->transparent = YES;
    }
    hdr.c.data.sid = me->sid;
    if (len <= MUX_LENGTH) {
	hdr.c.data.size = 1;
	hdr.c.data.size = len;
    } else {
	hdr.c.data.long_len = 1;
    }
    return HT_OK;
}

PRIVATE int HTMux_put_string (HTStream * me, const char * s)
{
    return HTMux_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTMux_put_character (HTStream * me, char c)
{
    return HTMux_put_block(me, &c, 1);
}

PRIVATE int HTMux_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTMux_free (HTStream * me)
{
    int status = HT_OK;

    /* Here we should send a FIN header */

    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    if (PROT_TRACE) HTTrace("Mux stream.. FREEING....\n");
    HT_FREE(me);
    return status;
}

PRIVATE int HTMux_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;

    /* Here we should send a RST header */

    if (me->target) status = (*me->target->isa->abort)(me->target, e);
    if (PROT_TRACE) HTTrace("Mux stream.. ABORTING...\n");
    HTW3Mux_delete(me->scp);
    HT_FREE(me);
    return status;
}

PRIVATE const HTStreamClass HTMuxClass =
{		
    "HTMux",
    HTMux_flush,
    HTMux_free,
    HTMux_abort,
    HTMux_put_character,
    HTMux_put_string,
    HTMux_put_block
};

PUBLIC HTStream * HTMux_new (HTNet * net, HTStream * target,
			     BOOL active)
{
    if (net && net->sockfd!=INVSOC) {
	if (HTW3Mux_new(net->sockfd, active) == YES) {
	    HTStream * me;
	    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
		HT_OUTOFMEM("HTMux");
	    me->isa = &HTMuxClass;
	    return me;
	}
    }
    return HTErrorStream();
}

/* ------------------------------------------------------------------------- */
/*			    DEMULTIPLEXING STREAM			     */
/* ------------------------------------------------------------------------- */

PRIVATE int HTDemux_put_block (HTStream * me, const char * b, int l)
{
    /* if not transparent then send a SYN bit */

    return HT_OK;
}

PRIVATE int HTDemux_put_string (HTStream * me, const char * s)
{
    return HTDemux_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTDemux_put_character (HTStream * me, char c)
{
    return HTDemux_put_block(me, &c, 1);
}

PRIVATE int HTDemux_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int HTDemux_free (HTStream * me)
{
    int status = HT_OK;

    /* Here we should send a FIN header */

    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    if (PROT_TRACE) HTTrace("Demux stream FREEING....\n");
    HT_FREE(me);
    return status;
}

PRIVATE int HTDemux_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;

    /* Here we should send a RST header */

    if (me->target) status = (*me->target->isa->abort)(me->target, e);
    if (PROT_TRACE) HTTrace("Demux stream ABORTING...\n");
    HTW3Mux_delete(me->scp);
    HT_FREE(me);
    return status;
}

PRIVATE const HTStreamClass HTDemuxClass =
{		
    "HTDemux",
    HTDemux_flush,
    HTDemux_free,
    HTDemux_abort,
    HTDemux_put_character,
    HTDemux_put_string,
    HTDemux_put_block
};

PUBLIC HTStream * HTDemux_new (HTNet * net, HTStream * target)
{
    HTStream * me;
    if (net && net->sockfd!=INVSOC) {
	if (HTW3Mux_new(net->sockfd, NO) == YES) {
	    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	    HT_OUTOFMEM("HTDemux");
	    me->isa = &HTDemuxClass;
	    return me;
	}
    }
    return HTErrorStream();
}
