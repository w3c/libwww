/*
**	BUFFERED MUX OUTPUT STREAM
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	A buffered output MUX stream.
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
#include "WWWTrans.h"
#include "HTMuxHeader.h"
#include "HTMuxCh.h"
#include "HTMuxTx.h"					 /* Implemented here */

struct _HTOutputStream {
    const HTOutputStreamClass *	isa;
    HTOutputStream *		target;		 /* Target for outgoing data */
    HTChannel *			ch;
    int				size;			      /* Buffer size */
    int				bb;
    char *			block;
    char *			read;		       /* Position in 'data' */
    char *			data;				   /* buffer */
};

#define PUTBLOCK(b,l) (*me->target->isa->put_block)(me->target,(b),(l))

/* ------------------------------------------------------------------------- */

PRIVATE int HTMuxBuffer_write (HTOutputStream *me, const char *buf, int len)
{
    int status;

    if (me->bb > 0) {
	len -= (me->block - buf);
	if ((status = PUTBLOCK(me->block, me->bb)) != HT_OK) return status;
	me->block += me->bb;
	len -= me->bb;
	me->bb = 0;
    } else {
	int available = me->data + me->size - me->read;

	/* Still room in buffer */
	if (len <= available) {
	    memcpy(me->read, buf, len);
	    me->read += len;
	    return HT_OK;
	}

	/* If already data in buffer then fill it and flush */
	if (me->read > me->data) {
	    memcpy(me->read, buf, available);
	    me->block = (char *) buf+available;
	    if ((status = PUTBLOCK(me->data, me->size))!=HT_OK) return status;
	}

	/* If more data then write n times buffer size */
	if (!me->block)
	    me->block = (char *) buf;
	else {
	    len -= (me->block - buf);
	}
	me->bb = len - len%me->size;
	if ((status = PUTBLOCK(me->block, me->bb)) != HT_OK) return status;
	me->block += me->bb;
	len -= me->bb;
	me->bb = 0;
    }

    /* If data is not aligned then save the rest in our buffer */
    if (len > 0) {
	memcpy(me->data, me->block, len);
	me->read = me->data + len;
    } else
	me->read = me->data;
    me->block = NULL;
    return HT_OK;
}

PRIVATE int HTMuxBuffer_put_character (HTOutputStream * me, char c)
{
    return HTMuxBuffer_write(me, &c, 1);
}

PRIVATE int HTMuxBuffer_put_string (HTOutputStream * me, const char * s)
{
    return HTMuxBuffer_write(me, s, (int) strlen(s));
}

PRIVATE int HTMuxBuffer_flush (HTOutputStream * me)
{
    int status = HT_OK;
    if (me->read > me->data) {
	if ((status = PUTBLOCK(me->data, me->read - me->data))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	me->read = me->data;
	me->block = NULL;
    }
    return status;
}

PRIVATE int HTMuxBuffer_free (HTOutputStream * me)
{
#if 0
    return HTMuxBuffer_flush(me);
#else
    return HT_OK;
#endif
}

PRIVATE int HTMuxBuffer_abort (HTOutputStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HTTRACE(PROT_TRACE, "MUX Tx...... ABORTING...\n");
    return HT_ERROR;
}

/*
**	The difference between the close and the free method is that we don't
**	close the connection in the free method - we only call the free method
**	of the target stream. That way, we can keep the output stream as long 
**	as the channel itself.
*/
PRIVATE int HTMuxBuffer_close (HTOutputStream * me)
{
    if (me) {
	HTMuxBuffer_flush(me);
	if (me->target) (*me->target->isa->close)(me->target);
	HT_FREE(me->data);
	HT_FREE(me);
    }
    return HT_OK;
}

PRIVATE const HTOutputStreamClass HTMuxBuffer =
{		
    "MuxBuffer",
    HTMuxBuffer_flush,
    HTMuxBuffer_free,
    HTMuxBuffer_abort,
    HTMuxBuffer_put_character,
    HTMuxBuffer_put_string,
    HTMuxBuffer_write,
    HTMuxBuffer_close
}; 

PUBLIC HTOutputStream * HTMuxBuffer_new (HTHost * host, HTChannel * ch,
					 void * param, int bufsize)
{
    if (host && ch) {
	HTOutputStream * me = HTChannel_output(ch);
	if (me == NULL) {
	    if (bufsize <= 0) bufsize = MUX_BUFFER_SIZE;
	    if ((me = (HTOutputStream *) HT_CALLOC(1, sizeof(HTOutputStream))) == NULL ||
		(me->data = (char *) HT_MALLOC(bufsize)) == NULL)
		HT_OUTOFMEM("HTMuxBuffer_new");
	    me->isa = &HTMuxBuffer;
	    me->target = HTWriter_new(host, ch, param, 0);
	    me->ch = ch;
	    me->size = bufsize;
	    me->read = me->data;
	}
	return me;
    }
    return NULL;
}
