/*							       	       HTANSI.c
**	ANSI C FILE DESCRIPTOR TRANSPORT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**      History:
**         HFN: writtem
*/

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "HTError.h"
#include "HTBind.h"
#include "HTAlert.h"
#include "HTParse.h"
#include "HTReq.h"
#include "HTChannl.h"
#include "HTIOStream.h"
#include "HTNetMan.h"
#include "HTHstMan.h"
#include "HTANSI.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
    /* ... */
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;    
    HTChannel *			ch;
    HTHost *			host;
    FILE *			fp;
    HTStream *			target;
    char *			write;			/* Last byte written */
    char *			read;			   /* Last byte read */
    char			data [FILE_BUFFER_SIZE];
};

struct _HTOutputStream {
    const HTOutputStreamClass *	isa;
    HTChannel *			ch;
    HTHost *			host;
    FILE *			fp;
};

/* ------------------------------------------------------------------------- */
/*				 READ STREAM				     */
/* ------------------------------------------------------------------------- */

PRIVATE int HTANSIReader_flush (HTInputStream * me)
{
    return me->target ? (*me->target->isa->flush)(me->target) : HT_OK;
}

PRIVATE int HTANSIReader_free (HTInputStream * me)
{
    if (me->target) {
	int status = (*me->target->isa->_free)(me->target);
	if (status != HT_WOULD_BLOCK) me->target = NULL;
	return status;
    }
    return HT_OK;
}

PRIVATE int HTANSIReader_abort (HTInputStream * me, HTList * e)
{
    if (me->target) {
	(*me->target->isa->abort)(me->target, NULL);
	me->target = NULL;
    }
    return HT_ERROR;
}

PRIVATE int HTANSIReader_read (HTInputStream * me)
{
    int b_read;
    int status;
    HTNet * net = HTHost_getReadNet(me->host);
    FILE * fp = me->host->fp;

    /* Read the file desriptor */
    while (fp) {
	if ((b_read = fread(me->data, 1, FILE_BUFFER_SIZE, fp)) == 0){
	    if (ferror(fp)) {
		if (PROT_TRACE) HTTrace("ANSI read... READ ERROR\n");
	    } else {
		HTAlertCallback *cbf = HTAlert_find(HT_PROG_DONE);
		if (PROT_TRACE)
		    HTTrace("ANSI read... Finished loading file %p\n", fp);
		if (cbf) (*cbf)(net->request, HT_PROG_DONE,
				HT_MSG_NULL, NULL, NULL, NULL);
		return HT_CLOSED;
	    }
	}

	/* Remember how much we have read from the input socket */
	me->write = me->data;
	me->read = me->data + b_read;

	if (PROT_TRACE)
	    HTTrace("ANSI read... %d bytes read from file %p\n", b_read, fp);
	{
	    HTAlertCallback * cbf = HTAlert_find(HT_PROG_READ);
	    net->bytesRead += b_read;
	    if (cbf) (*cbf)(net->request, HT_PROG_READ,
			    HT_MSG_NULL, NULL, NULL, NULL);
	}

	/* Now push the data down the stream */
	if ((status = (*me->target->isa->put_block)
	     (me->target, me->data, b_read)) != HT_OK) {
	    if (status == HT_WOULD_BLOCK) {
		if (PROT_TRACE) HTTrace("ANSI read... Target WOULD BLOCK\n");
#if 0
		HTEvent_unregister(soc, HTEvent_READ);
#endif
		return HT_WOULD_BLOCK;
	    } else if (status == HT_PAUSE) {
		if (PROT_TRACE) HTTrace("ANSI read... Target PAUSED\n");
#if 0
		HTEvent_unregister(soc, HTEvent_READ);
#endif
		return HT_PAUSE;
	    } else if (status > 0) {	      /* Stream specific return code */
		if (PROT_TRACE)
		    HTTrace("ANSI read... Target returns %d\n", status);
		me->write = me->data + b_read;
		return status;
	    } else {				     /* We have a real error */
		if (PROT_TRACE) HTTrace("ANSI read... Target ERROR\n");
		return status;
	    }
	}
	me->write = me->data + b_read;
    }
    if (PROT_TRACE) HTTrace("ANSI read... File descriptor is NULL...\n");
    return HT_ERROR;
}

/*
**	The difference between the close and the free method is that we don't
**	close the connection in the free method - we only call the free method
**	of the target stream. That way, we can keep the output stream as long 
**	as the channel itself.
*/
PRIVATE int HTANSIReader_close (HTInputStream * me)
{
    if (PROT_TRACE) HTTrace("ANSI read... FREEING...\n");
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int HTANSIReader_consumed (HTInputStream * me, size_t bytes)
{
    if (PROT_TRACE) HTTrace("ANSI read... consumed %d bytes\n", bytes);
    return HT_OK;
}

PRIVATE const HTInputStreamClass HTANSIReader =
{		
    "ANSIReader",
    HTANSIReader_flush,
    HTANSIReader_free,
    HTANSIReader_abort,
    HTANSIReader_read,
    HTANSIReader_close,
    HTANSIReader_consumed
};

PUBLIC HTInputStream * HTANSIReader_new (HTHost * host, HTChannel * ch,
					 void * param,
					 int mode)
{
    if (host && ch) {
	HTInputStream * me = HTChannel_input(ch);
	if (me == NULL) {
	    if ((me=(HTInputStream *) HT_CALLOC(1, sizeof(HTInputStream))) == NULL)
	    HT_OUTOFMEM("HTANSIReader_new");
	    me->isa = &HTANSIReader;
	    me->ch = ch;
	}
	me->host = host;
	me->fp = host->fp;
	return me;
    }
    return NULL;
}

/* ------------------------------------------------------------------------- */
/*				WRITE STREAM				     */
/* ------------------------------------------------------------------------- */

PRIVATE int HTANSIWriter_flush (HTOutputStream * me)
{
    return (fflush(me->fp) == EOF) ? HT_ERROR : HT_OK;
}

PRIVATE int HTANSIWriter_free (HTOutputStream * me)
{
    return HT_OK;
}

PRIVATE int HTANSIWriter_abort (HTOutputStream * me, HTList * e)
{
    if (PROT_TRACE) HTTrace("ANSI write.. ABORTING...\n");
    return HT_ERROR;
}

PRIVATE int HTANSIWriter_character (HTOutputStream * me, char c)
{
    return (fputc(c, me->fp) == EOF) ? HT_ERROR : HT_OK;
}

PRIVATE int HTANSIWriter_string (HTOutputStream * me, const char* s)
{
    if (*s)				             /* For vms :-( 10/04-94 */
	return (fputs(s, me->fp) == EOF) ? HT_ERROR : HT_OK;
    return HT_OK;
}

PRIVATE int HTANSIWriter_block (HTOutputStream * me, const char* s, int l)
{
    int status ;
    status = (fwrite(s, 1, l, me->fp) != l) ? HT_ERROR : HT_OK ;
    if (l > 1 && status == HT_OK) (void) HTANSIWriter_flush(me);
    return status;
}

/*
**	The difference between the close and the free method is that we don't
**	close the connection in the free method - we only call the free method
**	of the target stream. That way, we can keep the output stream as long 
**	as the channel itself.
*/
PRIVATE int HTANSIWriter_close (HTOutputStream * me)
{
    if (PROT_TRACE) HTTrace("ANSI write.. FREEING...\n");
    HT_FREE(me);
    return HT_OK;
}

PRIVATE const HTOutputStreamClass HTANSIWriter =
{		
    "ANSIWriter",
    HTANSIWriter_flush,
    HTANSIWriter_free,
    HTANSIWriter_abort,
    HTANSIWriter_character,
    HTANSIWriter_string,
    HTANSIWriter_block,
    HTANSIWriter_close,
};

PUBLIC HTOutputStream * HTANSIWriter_new (HTHost * host, HTChannel * ch,
					  void * param, int mode)
{
    if (host && ch) {
	HTOutputStream * me = HTChannel_output(ch);
	if (me == NULL) {
	    if ((me=(HTOutputStream *) HT_CALLOC(1, sizeof(HTOutputStream)))==NULL)
		HT_OUTOFMEM("HTANSIWriter_new");
	    me->isa = &HTANSIWriter;
	    me->ch = ch;
	    me->host = host;
	    me->fp = HTChannel_file(HTHost_channel(host));
	}
	return me;
    }
    return NULL;
}
