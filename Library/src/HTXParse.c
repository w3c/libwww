/*								   HTXParse.c
**	EXTPARSE CLASS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**  AUTHORS:
**	HWL 23/8/94
**
**  HISTORY:
**      HWL 5/12/95: Added NUL-terminatio, changed name from Ext to X
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTXParse.h"                 /* defines HTStreamClass */
#include "HTEPtoCl.h"         /* defines dummy routine for talking to client */

struct _HTStream {
	const HTStreamClass *	isa;
	HTXParseStruct *      eps;
};

PRIVATE int HTXParse_put_character (HTStream * me, char c)
{
    while ((me->eps->used + 1) > (me->eps->length + 1)) {
	me->eps->length += INPUT_BUFFER_SIZE;
    }
    if ((me->eps->buffer = (char  *) HT_REALLOC(me->eps->buffer, (me->eps->length + 1))) == NULL)
        HT_OUTOFMEM("me->eps->buffer ");
    *(me->eps->buffer + me->eps->used) = c;
    me->eps->used++;
    me->eps->buffer[me->eps->used] = '\0'; /* null-terminate string */
    return HT_OK;
}

PRIVATE int HTXParse_put_string (HTStream * me, const char * s)
{
    int l = strlen(s);

    HTTRACE(STREAM_TRACE, "HTXParse_put_string, %s\n" _ s);

    while ((me->eps->used + l) > (me->eps->length + 1)) {
	me->eps->length += INPUT_BUFFER_SIZE;
    }
    if ((me->eps->buffer = (char  *) HT_REALLOC(me->eps->buffer, (me->eps->length + 1))) == NULL)
        HT_OUTOFMEM("me->eps->buffer ");
    memcpy( (me->eps->buffer + me->eps->used), s, l); 
    me->eps->used += l;
    me->eps->buffer[me->eps->used] = '\0'; /* null-terminate string */
    return HT_OK;
}

PRIVATE int HTXParse_write (HTStream * me, const char * s, int l)
{
    while ((me->eps->used + l) > (me->eps->length + 1)) {
	me->eps->length += INPUT_BUFFER_SIZE;
    }
    if ((me->eps->buffer = (char  *) HT_REALLOC(me->eps->buffer, (me->eps->length + 1))) == NULL)
        HT_OUTOFMEM("me->eps->buffer ");
    memcpy( (me->eps->buffer + me->eps->used), s, l); 
    me->eps->used += l;
    me->eps->buffer[me->eps->used] = '\0'; /* null-terminate string */
    (*(me->eps->call_client))(me->eps);       /* client can give status info */
    HTTRACE(STREAM_TRACE, "HTXParse_write, l=%d, used = %d\n" _ l _ me->eps->used);
    return HT_OK;
}


PRIVATE int HTXParse_flush (HTStream * me)
{
    HTTRACE(STREAM_TRACE, "HTXParse_flush\n");
    return HT_OK;
}

PRIVATE int HTXParse_free (HTStream * me)
{
    HTTRACE(STREAM_TRACE, "HTXParse_free\n");
    me->eps->finished = YES;
    (*(me->eps->call_client))(me->eps);           /* client will free buffer */
    HT_FREE(me->eps);
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int HTXParse_abort (HTStream * me, HTList * e)
{
    HTTRACE(STREAM_TRACE, "HTXParse_abort\n");
    HTXParse_free(me);				  /* Henrik Nov 2 94 */
    return HT_ERROR;
}


/*	XParse stream
**	-----------------
*/


PRIVATE const HTStreamClass HTXParseClass =
{		
	"XParse",
	HTXParse_flush,
	HTXParse_free,
	HTXParse_abort,
	HTXParse_put_character,
	HTXParse_put_string,
	HTXParse_write
}; 

PUBLIC HTStream* HTXParse (HTRequest *	request,
			     void *		param,
			     HTFormat		input_format,
			     HTFormat		output_format,
			     HTStream *		output_stream)
{
    HTStream* me;
  
#ifdef HTDEBUG
    if (STREAM_TRACE) {
	HTTRACE(STREAM_TRACE, "HTXConvert..");
	if (input_format && input_format->name)
            HTTRACE(STREAM_TRACE, ".. input format is %s" _ input_format->name);
	if (output_format && output_format->name)
            HTTRACE(STREAM_TRACE, ".. output format is %s" _ output_format->name);
	HTTRACE(STREAM_TRACE, "\n");
    }
#endif /* HTDEBUG */

    if ((me = (HTStream *) HT_CALLOC(1, sizeof(*me))) == NULL)
        HT_OUTOFMEM("HTXConvert");
    me->isa = &HTXParseClass;

    if ((me->eps = (HTXParseStruct  *) HT_CALLOC(1, sizeof(HTXParseStruct))) == NULL)
        HT_OUTOFMEM("HTXConvert");

    if (input_format)
        me->eps->content_type = input_format->name;
    me->eps->call_client = HTCallClient;
    if ((me->eps->buffer = (char  *) HT_CALLOC(INPUT_BUFFER_SIZE,1)) == NULL)
        HT_OUTOFMEM("HTXParse");
    me->eps->used = 0;
    me->eps->finished = NO;
    me->eps->length = INPUT_BUFFER_SIZE;
    me->eps->request = request;
    return me;
}
