/*							       	    HTFWrite.c
**	BASIC FILE WRITER STREAM
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This version of the stream object just writes to a C file.
**	The file is assumed open and left open.
**
**	Bugs:
**		strings written must be less than buffer size.
**
**      History:
**         HFN: wrote it
**         HWL: converted the caching scheme to be hierachical by taking
**              AL code from Deamon
**	   HFN: moved cache code to HTCache module
**
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTFWrite.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *isa;
    FILE *		fp;
    BOOL		leave_open;		    /* Close file when free? */
};

/* ------------------------------------------------------------------------- */

PRIVATE int HTFWriter_put_character (HTStream * me, char c)
{
    return (fputc(c, me->fp) == EOF) ? HT_ERROR : HT_OK;
}

PRIVATE int HTFWriter_put_string (HTStream * me, const char* s)
{
    if (*s)				             /* For vms :-( 10/04-94 */
	return (fputs(s, me->fp) == EOF) ? HT_ERROR : HT_OK;
    return HT_OK;
}

PRIVATE int HTFWriter_flush (HTStream * me)
{
    return (fflush(me->fp) == EOF) ? HT_ERROR : HT_OK;
}

PRIVATE int HTFWriter_write (HTStream * me, const char* s, int l)
{
    int status ;
    status = (fwrite(s, 1, l, me->fp) != l) ? HT_ERROR : HT_OK ;
    if (l > 1 && status == HT_OK) (void) HTFWriter_flush(me);
    return status ;
}

PRIVATE int HTFWriter_free (HTStream * me)
{
    if (me) {
	if (me->leave_open != YES)
	    fclose(me->fp);
	else
	    HTFWriter_flush(me);
	HT_FREE(me);
    }
    return HT_OK;
}

PRIVATE int HTFWriter_abort (HTStream * me, HTList * e)
{
    HTTRACE(STREAM_TRACE, "FileWriter.. ABORTING...\n");
    if (me) {
	if (me->leave_open != YES) fclose(me->fp);
	HT_FREE(me);
    }
    return HT_ERROR;
}

PRIVATE const HTStreamClass HTFWriter = /* As opposed to print etc */
{		
    "FileWriter",
    HTFWriter_flush,
    HTFWriter_free,
    HTFWriter_abort,
    HTFWriter_put_character,
    HTFWriter_put_string,
    HTFWriter_write
};

PUBLIC HTStream * HTFWriter_new (HTRequest * request, FILE * fp,
				 BOOL leave_open)
{
    HTStream * me = NULL;
    if (!fp) {
	HTTRACE(STREAM_TRACE, "FileWriter.. Bad file descriptor\n");
	return HTErrorStream();
    }
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTFWriter_new");
    me->isa = &HTFWriter;       
    me->fp = fp;
    me->leave_open = leave_open;
    return me;
}

