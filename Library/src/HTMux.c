/*								     HTMux.c
**	W3MUX MULTIPLEX AND DEMULTIPLEX STREAMS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
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
#include "HTStream.h"
#include "HTMux.h"					 /* Implemented here */

#define HT_SWAP_WORD(a) ( ((a) << 24) | \
		      (((a) << 8) & 0x00ff0000) | \
		      (((a) >> 8) & 0x0000ff00) | \
	((unsigned int)(a) >>24) )

#define MUX_LONG_LENGTH	0x80000000
#define MUX_SYN		0x40000000
#define MUX_FIN		0x20000000
#define MUX_RST		0x10000000
#define MUX_PSH 	0x08000000
#define MUX_UNUSED	0x04000000
#define MUX_SESSION	0x03FB0000
#define MUX_LENGTH	0x0003FFFF

#define MAX_SESSIONS		0xFF			 /* Max 256 sessions */
#define CLIENT_CTRL_SID		0		   /* Client control session */
#define SERVER_CTRL_SID		1		   /* Server control session */

#define SD_READ		1		   /* How to shutdown a w3mux object */
#define SD_WRITE	2
#define SD_CLOSE	4

/* Definition of the W3mux protocol */
typedef unsigned int flagbit;
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
	unsigned int		len	: 32;
    } c;
} HTMuxHdr;

typedef int HTSessionId;
#define INVSID	-1				       /* Invalid session id */

typedef enum _HTMuxState {
    MUX_HEAD	= 0,
    MUX_BODY	= 1,
    MUX_PAD	= 2
} HTMuxState;

struct _HTW3mux {
    unsigned int	next_head;  /* Number of bytes until next mux header */
    HTStream *		mux;			/* Target for our mux stream */
    HTMuxState		mux_state;
    int			pads;			   /* Number of bytes to pad */
    HTSessionId		ctrl_sid;		       /* Control session id */
    int			shutdown;
    HTStream *		sessions [MAX_SESSIONS];
};

#define MUX_BLOCK(b,l)		(*w3mux->mux->isa->put_block)(w3mux->mux, b, l)
#define DEMUX_BLOCK(b, l)	(*target->isa->put_block)(target, b, l)

/*
**	Thus is the stream for each session. Notice that we don't have a 
**	target for this strem directly associated with it. It is stored in the
**	w3mux object instead.
*/
struct _HTStream {
    const HTStreamClass *	isa;
    HTSessionId			sid;
    HTW3mux *			w3mux;
    BOOL			transparent;
};

/* We have to able to padd up until the nearest 64 bit boundary */
PRIVATE char padding[] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0'};

/* ------------------------------------------------------------------------- */
/*				SESSION CONTROL				     */
/* ------------------------------------------------------------------------- */

/*
**	Register a new session with this w3mux object, If no free entries
**	are available then return INVSID which always is an invalid session 
**	number.
*/
PRIVATE HTSessionId HTSession_add (HTW3mux * w3mux, HTStream * output)
{
    if (w3mux && output) {
	int sid = w3mux->ctrl_sid+2;
	HTStream ** mp = w3mux->sessions+sid;
	for (;sid < MAX_SESSIONS && *mp!=NULL; sid+=2, mp+=2);
	if (!*mp) *mp = output;
	return sid;
    }
    if (PROT_TRACE) HTTrace("Session..... No available entries\n");
    return INVSID;
}

/*
**	Unregister a session from this w3mux object.
**	Returns YES if OK else NO
*/
PRIVATE BOOL HTSession_delete (HTW3mux * w3mux, HTSessionId sid)
{
    if (w3mux && sid!=INVSID) {
	*(w3mux->sessions+sid) = NULL;
	return (w3mux->shutdown & SD_CLOSE) && HTW3mux_isEmpty(w3mux) ?
	    HTW3mux_delete(w3mux) : YES;
    }
    return NO;
}

/*
**	Return the stream associated with this session ID
*/
PRIVATE HTStream * HTSession_stream (HTW3mux * w3mux, HTSessionId sid)
{
    return w3mux ? *(w3mux->sessions+sid) : NULL;
}

/*
**	Set the stream associated with this session ID
*/
PRIVATE BOOL HTSession_setStream (HTW3mux * w3mux, HTSessionId sid,
				  HTStream * output)
{
    if (w3mux && sid!=INVSID) {
	*(w3mux->sessions+sid)=output;
	return YES;
    }
    return NO;
}

/* ------------------------------------------------------------------------- */
/*			      MULTIPLEXING STREAM 			     */
/* ------------------------------------------------------------------------- */

PRIVATE int HTMux_put_block (HTStream * me, const char * b, int len)
{
    int status = HT_OK;
    HTMuxHdr hdr[2];
    HTW3mux * w3mux = me->w3mux;
    while (1) {
	switch (w3mux->mux_state) {

	  case MUX_HEAD:
	    hdr[0].c.len = 0;
	    if (!me->transparent) {
		hdr[0].c.data.syn = 1;
		me->transparent = YES;
	    }
	    hdr[0].c.data.sid = me->sid;
#ifndef HT_MUX_ALIGN_64
	    if (len <= MUX_LENGTH) {
		hdr[0].c.data.size = len;	
#ifdef WORDS_BIGENDIAN
		{
		    register unsigned int tmp = hdr[0].c.len;
		    hdr[0].c.len = HT_SWAP_WORD(tmp);
		}
#endif
		w3mux->pads = len % 4;
#ifdef MUX_WRITE
		status = MUX_BLOCK((const char *) &hdr, sizeof(int));
#endif
	    } else
#endif /* HT_MUX_ALIGN_64 */
	    {
		hdr[0].c.data.long_len = 1;
		hdr[1].c.len = len;
#ifdef WORDS_BIGENDIAN
		{
		    register unsigned int tmp = hdr[0].c.len;
		    hdr[0].c.len = HT_SWAP_WORD(tmp);
		    tmp = hdr[1].c.len;
		    hdr[1].c.len = HT_SWAP_WORD(tmp);
		}
#endif
		w3mux->pads = len % 8;
#ifdef MUX_WRITE
		status = MUX_BLOCK((const char *)&hdr,sizeof(int)+sizeof(int));
#endif
	    }
	    if (PROT_TRACE) HTTrace("Mux......... header %d\n", hdr[0].c.len);
	    if (status != HT_OK) return status;
	    w3mux->mux_state = MUX_BODY;
	    break;
	    
	  case MUX_BODY:
#ifdef MUX_WRITE
	    status = MUX_BLOCK(b, len);
#endif
	    if (status != HT_OK) return status;
	    w3mux->mux_state = MUX_PAD;
	    break;
	    
	  case MUX_PAD:
	    if (PROT_TRACE) HTTrace("Mux......... pad %d bytes\n",w3mux->pads);
#ifdef MUX_WRITE
	    status = MUX_BLOCK(padding, w3mux->pads);
#endif
	    if (status != HT_OK) return status;
	    w3mux->mux_state = MUX_HEAD;
	    return HT_OK;

	}
    } /* End of while (1) */
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
    return (*me->w3mux->mux->isa->flush)(me->w3mux->mux);
}

PRIVATE int HTMux_free (HTStream * me)
{
    int status = HT_OK;
    HTW3mux * w3mux = me->w3mux;
    if (me->transparent) {			 /* Only FIN active sessions */
	HTMuxHdr hdr;
	hdr.c.len = 0;
	hdr.c.data.fin = 1;
#ifdef WORDS_BIGENDIAN
	{
	    register unsigned int tmp = hdr.c.len;
	    hdr.c.len = HT_SWAP_WORD(tmp);
	}
#endif
	MUX_BLOCK((const char *) &hdr, sizeof(int));
    }
    HTSession_delete(w3mux, me->sid);
    if (PROT_TRACE) HTTrace("Session stream FREEING....\n");
    HT_FREE(me);
    return status;
}

PRIVATE int HTMux_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;
    HTW3mux * w3mux = me->w3mux;
    if (me->transparent) {			 /* Only RST active sessions */
	HTMuxHdr hdr;
	hdr.c.len = 0;
	hdr.c.data.rst = 1;
#ifdef WORDS_BIGENDIAN
	{
	    register unsigned int tmp = hdr.c.len;
	    hdr.c.len = HT_SWAP_WORD(tmp);
	}
#endif
	MUX_BLOCK((const char *) &hdr, sizeof(int));
    }
    HTSession_delete(w3mux, me->sid);
    if (PROT_TRACE) HTTrace("Session stream ABORTING...\n");
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

/*	HTSession_new
**	-------------
**	Create a new session and register it with a w3mux object.
**	The output stream is where to output data returning from the session
**	and the return stream is where to input data to the stream.
**	If the output stream is NULL then we use a blackhole instead
*/
PUBLIC HTStream * HTSession_new (HTW3mux * w3mux, HTStream * output)
{
    if (w3mux && !w3mux->shutdown) { 
	HTStream * me;
	if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	    HT_OUTOFMEM("HTSession");
	me->isa = &HTMuxClass;	
	me->w3mux = w3mux;
	if ((me->sid = HTSession_add(w3mux, output)) == INVSID) {
	    HT_FREE(me);
	} else {
	    return me;
	}
    }
    if (STREAM_TRACE) HTTrace("Session..... Can't create session\n");
    return HTErrorStream();
}

/* ------------------------------------------------------------------------- */
/*			    DEMULTIPLEXING STREAM			     */
/* ------------------------------------------------------------------------- */

PRIVATE HTStream * HTDemux_new (HTW3mux * w3mux, HTSessionId);

/*
**	We assume that we always get a full mux header (either 32 or 64 bits)!
*/
PRIVATE int HTDemux_put_block (HTStream * me, const char * b, int len)
{
    int status;
    HTStream * target;
    HTW3mux * w3mux = me->w3mux;
    while (1) {
	if (w3mux->next_head < len) {	      /* Can we see the next header? */
	    HTMuxHdr hdr;

	    /* Read header */
#ifdef WORDS_BIGENDIAN
	    register unsigned int tmp = *(b+w3mux->next_head);
	    hdr.c.len = HT_SWAP_WORD(tmp);
#else
	    hdr.c.len = *(b+w3mux->next_head);
#endif

	    /* Check for long header */
	    if (hdr.c.data.long_len) {
#ifdef WORDS_BIGENDIAN
		register unsigned int tmp = *(b+w3mux->next_head);
		w3mux->next_head = HT_SWAP_WORD(tmp);
#else
		w3mux->next_head = *(b+w3mux->next_head);
#endif
	    }

	    /* Find stream */
	    if ((target = HTSession_stream(w3mux, hdr.c.data.sid)) == NULL)
		target = HTDemux_new(w3mux, hdr.c.data.sid);
	    
	    /* Write to stream */
	    status = DEMUX_BLOCK(b, w3mux->next_head);
	    if (status != HT_OK) {

		/* Set target stream to Buffer stream */

	    }
	    b += w3mux->next_head;
    	} else {
	    w3mux->next_head += len;
	    status = DEMUX_BLOCK(b, len);
	}
    }
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

    /* This stream is the demux stream */
    /* Here we should flush all targets */

    return HT_OK;
}

PRIVATE int HTDemux_free (HTStream * me)
{
    if (PROT_TRACE) HTTrace("Demux stream FREEING....\n");

    /* This stream is the demux stream */
    /* Here we should free all targets */

    return HT_OK;
}

PRIVATE int HTDemux_abort (HTStream * me, HTList * e)
{
    if (PROT_TRACE) HTTrace("Demux stream ABORTING...\n");

    /* This stream is the demux stream */
    /* Here we should abort all targets */

    return HT_ERROR;
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

/*	HTDemux_new
**	-----------
**	Create a new session and register it with this w3mux object.
**	This stream can only be created by w3mux object itself and it happens
**	when we encounter a session id that we haven't seen before.
**	Creating a demux stream means finding a target for outputing data.
**	with this session id.
*/
PRIVATE HTStream * HTDemux_new (HTW3mux * w3mux, HTSessionId sid)
{
    if (w3mux && sid!=INVSID) { 
	HTStream * me;
	if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	    HT_OUTOFMEM("HTDemux_new");
	me->isa = &HTDemuxClass;
	me->w3mux = w3mux;
	me->sid = sid;

	/* HERE WE MUST FIND THE TARGET */

	HTSession_setStream(w3mux, sid, HTErrorStream());
	
    }
    return HTErrorStream();
}

/*	HTW3mux_new
**	-----------
**	Create a new W3mux protocol object.
**	Returns new W3Mux object or NULL on error
*/
PUBLIC HTW3mux * HTW3mux_new (HTStream * target, BOOL active)
{
    HTW3mux * w3mux = NULL;
    if (target) {
	if ((w3mux = (HTW3mux *) HT_CALLOC(1, sizeof(HTW3mux))) == NULL)
	    HT_OUTOFMEM("HTW3mux_new");	    
	w3mux->mux = target;
	w3mux->ctrl_sid = active ? CLIENT_CTRL_SID : SERVER_CTRL_SID;
	if (PROT_TRACE) HTTrace("W3mux....... Created %p\n", w3mux);
    }
    return w3mux;
}

/*	HTW3mux_delete
**	--------------
**	Delete a W3mux protocol object. If there are any remaining sessions
**	then cancel them by turning the output into a error stream. When no
**	more sessions are referencing this object we can delete it for good.
**	Until that point we turn this object into a zombie. However, the user
**	does not need to call it more than once.
*/
PUBLIC BOOL HTW3mux_delete (HTW3mux * w3mux)
{
    if (w3mux) {
	if (w3mux->shutdown < SD_CLOSE) {
	    HTW3mux_shutdown(w3mux, 2);
	    w3mux->shutdown = SD_CLOSE;
	}
	if (HTW3mux_isEmpty(w3mux)) HT_FREE(w3mux);
	return YES;
    }
    return NO;
}

/*
**	Do this w3mux object have any more sessions?
*/
PUBLIC BOOL HTW3mux_isEmpty (HTW3mux * w3mux)
{
    if (w3mux) {
	int sid = w3mux->ctrl_sid+2;
	HTStream ** mp = w3mux->sessions+sid;
	for (;sid < MAX_SESSIONS && *mp!=NULL; sid+=2, mp+=2);
	return mp ? NO : YES;
    }
    return NO;
}

/*	HTW3mux_shutdown
**	----------------
**	Shutdown a w3mux object in one or both directions. This is equivalent
**	to a socket shutdown() system call. That is, we have the following 
**	options:
**		0	Read half
**		1	Write half
**		2	Both read and write halves
*/
PUBLIC BOOL HTW3mux_shutdown (HTW3mux * w3mux, int how)
{
    if (w3mux) {
	int sid;
	HTStream ** mp;
	how++;
	if (how & SD_READ) {
	    for (sid=w3mux->ctrl_sid+SERVER_CTRL_SID, mp=w3mux->sessions+sid;
		 sid < MAX_SESSIONS; sid+=2, mp+=2) {
		if (*mp) {
		    (*(*mp)->isa->abort)(*mp, NULL);
		    *mp = HTErrorStream();
		}
	    }
	}
	if (how & SD_WRITE) {
	    (*w3mux->mux->isa->abort)(w3mux->mux, NULL);
	    w3mux->mux = HTErrorStream();
	}
	w3mux->shutdown = how;
	return YES;
    }
    return NO;
}

