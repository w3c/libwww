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

/* 2000-29-08 JK : pre-pruning code out of libwww */
#ifdef HT_STRUCT_XML_STREAM 
#undef HT_STRUCT_XML_STREAM
#endif

#ifdef HT_STRUCT_XML_STREAM
#include "SGML.h"
#endif /* HT_STRUCT_XML_STREAM */
#include "HTXML.h"     				 /* Implemented here */

#define XML_MAX_ATTRIBUTES 50

struct _HTStream {
    const HTStreamClass *	isa;
    int				state;
    HTRequest *			request;
    HTStream *                  target;
    HTStructuredClass *         actions;	
    HTStructured *              starget;	
    XML_Parser 			xmlstream;
#ifdef HT_STRUCT_XML_STREAM
    SGML_dtd *                  dtd;
#endif /* HT_STRUCT_XML_STREAM */
    XML_StartElementHandler     xml_start_element;
    XML_EndElementHandler       xml_end_element;
    XML_CharacterDataHandler    xml_character_data;
    XML_DefaultHandler          xml_default_handler;
    void *                      xml_user_data;
};

/* @@@ SHould not be global but controlled by name spaces @@@ */
PRIVATE HTXMLCallback_new *	XMLInstance = NULL;
PRIVATE void *			XMLInstanceContext = NULL;

/* ------------------------------------------------------------------------- */

PRIVATE int HTXML_flush (HTStream * me)
{
    if(me->target)
	return (*me->target->isa->flush)(me->target);
    else if (me->starget)
	return (*me->actions->flush)(me->starget);
    return HT_OK;
}

PRIVATE int HTXML_free (HTStream * me)
{
    int status = HT_OK;
    XML_ParserFree(me->xmlstream);
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    } else if(me->starget) {
      if ((status = (*me->actions->_free)(me->starget)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    HTTRACE(XML_TRACE, "XML Parser.. FREEING...\n");
    HT_FREE(me);
    return status;
}

PRIVATE int HTXML_abort (HTStream * me, HTList * e)
{
    HTTRACE(XML_TRACE, "XML Parser.. ABORTING...\n");
    XML_ParserFree(me->xmlstream);
    if (me->target)	
	(*me->target->isa->abort)(me->target, NULL);
    else if (me->starget)	
	(*me->actions->abort)(me->starget, e);
    HT_FREE(me);
    return HT_ERROR;
}

PRIVATE int HTXML_write (HTStream * me, const char * buf, int len)
{
    if (me->state == HT_OK) {
	int status = XML_Parse(me->xmlstream, buf, len, 0);
	if (!status) {
	    HTTRACE(XML_TRACE, "XML Parser..  `%s\'\n" _
		    (char *)XML_ErrorString(XML_GetErrorCode(me->xmlstream)));
	    me->state = HT_ERROR;
	}
    }

    /* 
    **  We don't want to return an error here as this kills
    **  a potential pipeline of requests we might have
    */
    return HT_OK;
}

PRIVATE int HTXML_putCharacter (HTStream * me, char c)
{
    return HTXML_write(me, &c, 1);
}

PRIVATE int HTXML_putString (HTStream * me, const char * s)
{
    return HTXML_write(me, s, (int) strlen(s));
}

#ifdef HT_STRUCT_XML_STREAM

PRIVATE BOOL set_attributes_values(HTTag *tag,BOOL *present,char **value,
				   const char *nameatt,const char *valueatt)
{
    if(tag && nameatt && valueatt) {
	int i;
	HTAttr *attributes= tag->attributes;
	for(i = 0; i< tag->number_of_attributes ; i++) {
	    if(!strcasecomp(attributes[i].name,nameatt)) {
		present[i] = TRUE;
		value[i] = (char *)valueatt;
		return TRUE;
	    }
	}
    }
    return FALSE;
}

PRIVATE void start_element(HTStream *	me,
			   const XML_Char *name,
			   const XML_Char **atts)
{
    int element_number = 
	SGML_findElementNumber((SGML_dtd *)me->dtd,(char *)name);
    if(element_number >= 0) {
	HTTag *tag = SGML_findTag (me->dtd,element_number);
	int i = 0;  
	BOOL present[XML_MAX_ATTRIBUTES];
	const char *value[XML_MAX_ATTRIBUTES];
	memset((void *) present, '\0', XML_MAX_ATTRIBUTES);
	memset((void *) value, '\0', XML_MAX_ATTRIBUTES*sizeof(char *));

	while(atts[i]) {
	   set_attributes_values(tag,present,(char **)value,atts[i],atts[i+1]);
	    i+=2;
	    /* attributes that are not in the dtd will be lost */
	}
	(*me->actions->start_element)
	    (me->starget,element_number,present,value); 
    } else {
	/* elements that are not in the dtd will be lost */	
    }
}

PRIVATE void end_element(HTStream * me, const XML_Char *name)
{
    int element_number = SGML_findElementNumber(me->dtd,(char *)name);

    if(element_number > 0) {
	(*me->actions->end_element)(me->starget, element_number);
    } else {
	/* elements that are not in the dtd will be lost */	
    }
}
PRIVATE void character_data(HTStream *me, const XML_Char *s, int len)
{
    (*me->actions->put_block)(me->starget, s, len);
}

PRIVATE void default_handler(HTStream *me, const XML_Char *s, int len)
{
    if(s[0] == '&' && s[len-1]==';') {
       (*me->actions->unparsed_entity)(me->starget, s,len);
    } else {
	/* characters that can not be parsed are lost */
    }
}

#endif /* HT_STRUCT_XML_STREAM */

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
    HTTRACE(XML_TRACE, "XML Parser.. Stream created\n");
    
    /* Call the stream callback handler (if any) with this new stream */
    if (XMLInstance)
	(*XMLInstance)(me, request, output_format, output_stream, me->xmlstream, XMLInstanceContext);

    return me;
}

PUBLIC BOOL HTXMLCallback_registerNew (HTXMLCallback_new * me, void * context)
{
    XMLInstance = me;
    XMLInstanceContext = context;
    return YES;
}

#ifdef HT_STRUCT_XML_STREAM

PRIVATE HTStream * HTXMLStructured_new (const SGML_dtd * dtd, HTStructured * starget)
{
    HTStream * me = NULL;
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	HT_OUTOFMEM("HTXML_new");
    me->isa = &HTXMLClass;
    me->state = HT_OK;
    me->starget = starget;
    me->dtd = (SGML_dtd *)dtd; 
    me->actions = (HTStructuredClass*)(((HTStream*)starget)->isa);

    /* Now create the XML parser instance */
    if ((me->xmlstream = XML_ParserCreate(NULL)) == NULL) {
	HT_FREE(me);
	return HTErrorStream();
    }
    XML_SetUserData(me->xmlstream,me);
    XML_SetElementHandler(me->xmlstream,
			  (XML_StartElementHandler)start_element, 
			  (XML_EndElementHandler)end_element);
    XML_SetCharacterDataHandler(me->xmlstream,  
				(XML_CharacterDataHandler)character_data);
    XML_SetDefaultHandler(me->xmlstream,
			  (XML_DefaultHandler)default_handler);

    HTTRACE(XML_TRACE, "XML Parser.. Stream created\n");
    return me;
}

PRIVATE BOOL HTXMLStructured_setHandlers (HTStream *			me,
					 XML_StartElementHandler	start,
					 XML_EndElementHandler		end, 
					 XML_CharacterDataHandler	char_data,
					 XML_DefaultHandler		def_handler)
{
    if(me) {
	me->xml_start_element = start;
	me->xml_end_element = end;
	me->xml_character_data = char_data;
	me->xml_default_handler = def_handler;
	XML_SetElementHandler(me->xmlstream,start,end);
	XML_SetCharacterDataHandler(me->xmlstream,char_data);
	XML_SetDefaultHandler(me->xmlstream,def_handler);
	return YES;
    }
    return NO;
}
			
PRIVATE BOOL HTXMLStructured_setUserData(HTStream *me, void *user_data)
{
    if(me) {
	me->xml_user_data = user_data;
	XML_SetUserData(me->xmlstream,me->xml_user_data);
	return YES;
    }
    return NO;
}

#endif /* HT_STRUCT_XML_STREAM */
