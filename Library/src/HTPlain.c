/*								      HTPlain.c
**	PLAIN TEXT OBJECT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This version of the stream object just writes to a socket.
**	The socket is assumed open and left open.
**
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTStyle.h"
#include "HTPlain.h"
#include "HTextImp.h"

struct _HTStream {
    const HTStreamClass *	isa;
    HTextImp * 			text;
};

/* -------------------------------------------------------------------------- */

PRIVATE int HTPlain_put_character (HTStream * me, char c)
{
    HTextImp_addText(me->text, &c, 1);
    return HT_OK;
}

PRIVATE int HTPlain_put_string (HTStream * me, const char * s)
{
    HTextImp_addText(me->text, s, strlen(s));
    return HT_OK;
}

PRIVATE int HTPlain_write (HTStream * me, const char* b, int l)
{
    HTextImp_addText(me->text, b, l);
    return HT_OK;
}

PRIVATE int HTPlain_flush (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTPlain_free (HTStream * me)
{
    if (me) {
	HTextImp_build(me->text, HTEXT_END);
	HTextImp_delete(me->text);
	HT_FREE(me);
    }
    return HT_OK;
}

PRIVATE int HTPlain_abort (HTStream * me, HTList * e)
{
    if (me) {
	HTextImp_build(me->text, HTEXT_ABORT);
	HTextImp_delete(me->text);
	HT_FREE(me);
    }
    return HT_ERROR;
}

PRIVATE const HTStreamClass HTPlain =
{
    "PlainText",
    HTPlain_flush,
    HTPlain_free,
    HTPlain_abort,
    HTPlain_put_character,
    HTPlain_put_string,
    HTPlain_write,
}; 

PUBLIC HTStream* HTPlainPresent (HTRequest *	request,
				 void *		param,
				 HTFormat	input_format,
				 HTFormat	output_format,
				 HTStream *	output_stream)
{
    HTStream * me;
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTPlain_new");
    me->isa = &HTPlain;       
    me->text = HTextImp_new(request, HTRequest_anchor(request), output_stream);
    HTextImp_build(me->text, HTEXT_BEGIN);
    return me;
}


