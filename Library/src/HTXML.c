/*
**	LIBWWW STREAM WRAPPER FOR EXPAT XML PARSER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module requires expat in order to compile/link
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTXML.h"					 /* Implemented here */

#include <xmlparse.h>

struct _HTStream {
    const HTStreamClass *	isa;
    int				state;
    HTRequest *			request;
    HTStream *			target;
    XML_Parser 			xmlstream;
};

/* ------------------------------------------------------------------------- */

PRIVATE int HTXML_flush (HTStream * me)
{
    return me->target ? (*me->target->isa->flush)(me->target) : HT_OK;
}

PRIVATE int HTXML_free (HTStream * me)
{
    int status = HT_OK;
    XML_ParserFree(me->xmlstream);
    if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	return HT_WOULD_BLOCK;
    if (STREAM_TRACE) HTTrace("XML Parser.. FREEING...\n");
    HT_FREE(me);
    return status;
}

PRIVATE int HTXML_abort (HTStream * me, HTList * e)
{
    if (STREAM_TRACE) HTTrace("XML Parser.. ABORTING...\n");
    XML_ParserFree(me->xmlstream);
    (*me->target->isa->abort)(me->target, NULL);
    HT_FREE(me);
    return HT_ERROR;
}

PRIVATE int HTXML_write (HTStream * me, const char * buf, int len)
{
    if (me->state == HT_OK) {
	int status = XML_Parse(me->xmlstream, buf, len, 0);
	if (!status) {
	    HTTrace("XML Parser.. `%s\'\n", XML_ErrorString(status));
	    me->state = HT_ERROR;
	}
    }
    return me->state;
}

PRIVATE int HTXML_putCharacter (HTStream * me, char c)
{
    return HTXML_write(me, &c, 1);
}

PRIVATE int HTXML_putString (HTStream * me, const char * s)
{
    return HTXML_write(me, s, (int) strlen(s));
}

PRIVATE const HTStreamClass HTXMLClass =
{		
    "xml",
    HTXML_flush,
    HTXML_free,
    HTXML_abort,
    HTXML_putCharacter,
    HTXML_putString,
    HTXML_write
}; 

PUBLIC HTStream * HTXML_new (HTRequest *	request,
			     void *		param,
			     HTFormat		input_format,
			     HTFormat		output_format,
			     HTStream *		output_stream)
{
    HTStream * me = NULL;
    HTCharset charset = HTResponse_charset (HTRequest_response(request));
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	HT_OUTOFMEM("HTXML_new");
    me->isa = &HTXMLClass;
    me->state = HT_OK;
    me->request = request;
    me->target = output_stream ? output_stream : HTErrorStream();

    /* Now create the XML parser instance */
    if ((me->xmlstream = XML_ParserCreate(HTAtom_name(charset))) == NULL) {
	HT_FREE(me);
	return HTErrorStream();
    }

    if (STREAM_TRACE) HTTrace("XML Parser.. Stream created\n");
    return me;
}
