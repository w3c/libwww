/*								      HTPlain.c
**	PLAIN TEXT OBJECT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This version of the stream object just writes to a socket.
**	The socket is assumed open and left open.
**
**	Bugs:
**		strings written must be less than buffer size.
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HText.h"
#include "HTStyle.h"
#include "HTPlain.h"

#define BUFFER_SIZE 4096;	/* Tradeoff */

extern HTStyleSheet * styleSheet;



/*		HTML Object
**		-----------
*/

struct _HTStream {
	CONST HTStreamClass *	isa;

	HText * 		text;
};

/*	Write the buffer out to the socket
**	----------------------------------
*/


/*_________________________________________________________________________
**
**			A C T I O N 	R O U T I N E S
*/

/*	Character handling
**	------------------
*/

PRIVATE int HTPlain_put_character (HTStream * me, char c)
{
    HText_appendCharacter(me->text, c);
    return HT_OK;
}


/*	String handling
**	---------------
**
*/
PRIVATE int HTPlain_put_string (HTStream * me, CONST char * s)
{
    HText_appendText(me->text, s);
    return HT_OK;
}


PRIVATE int HTPlain_write (HTStream * me, CONST char* b, int l)
{
    while (l-- > 0)
	HText_appendCharacter(me->text, *b++);
    return HT_OK;
}



/*	Flush an Plain object
**	--------------------
*/
PRIVATE int HTPlain_flush (HTStream * me)
{
    return HT_OK;
}

/*	Free an HTML object
**	-------------------
**
**	Note that the SGML parsing context is freed, but the created object is not,
**	as it takes on an existence of its own unless explicitly freed.
*/
PRIVATE int HTPlain_free (HTStream * me)
{
    if (me) {
	HText_endAppend(me->text);
	HT_FREE(me);
    }
    return HT_OK;
}

/*	End writing
*/

PRIVATE int HTPlain_abort (HTStream * me, HTList * e)
{
    HTPlain_free(me);
    return HT_ERROR;
}



/*		Structured Object Class
**		-----------------------
*/
PRIVATE CONST HTStreamClass HTPlain =
{
    "PlainText",
    HTPlain_flush,
    HTPlain_free,
    HTPlain_abort,
    HTPlain_put_character,
    HTPlain_put_string,
    HTPlain_write,
}; 


/*		New object
**		----------
*/
PUBLIC HTStream* HTPlainPresent (HTRequest *	request,
				 void *		param,
				 HTFormat	input_format,
				 HTFormat	output_format,
				 HTStream *	output_stream)
{
    HTStream* me;
    if ((me = (HTStream *) HT_MALLOC(sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTPlain_new");
    me->isa = &HTPlain;       
    me->text = HText_new2(request, HTRequest_anchor(request), output_stream);
    HText_beginAppend(me->text);
    HText_setStyle(me->text, HTStyleNamed(styleSheet, "Example"));
    return me;
}


