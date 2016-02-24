/*

  					W3C Sample Code Library libwww Expat XML Parser Wrapper


!
  Expat XML Parser Wrapper
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module is implemented by HTXML.c, and is a part
of the  W3C Sample Code Library.

We use James Clark's expat
XML parser which is very neat indeed. As the code doesn't come as a separate
library, I included it in the libwww CVS code base where I compile is as
one library: libexpat.a. See
the external modules that libwww works with for
details. Thanks so much to John Punin for writing this code!
*/

#ifndef HTXML_H
#define HTXML_H

#include "HTFormat.h"
#include "HTStream.h"
#ifdef HT_STRUCT_XML_STREAM
#include "HTStruct.h"
#include "SGML.h"
#endif /* HT_STRUCT_XML_STREAM */

#include <expat.h>

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Libwww Stream Converter
.

This stream is a libwww converter which
calls and creates a expat stream instance. In order to tell the application
that a new stream instance has been created.
*/

extern HTConverter HTXML_new;

/*
.
  Callback Handler Announcing a new Expat Stream object
.

When a libwww to expat XML stream converter instance
is created, the stream checks to see if there are any callbacks registered
which should be notified about the new stream instance. If that is the case
then this callback is called and a pointer to the XML parser passed along.
The output stream is the target that was originally set for the request object
before the request was issued.
*/

typedef void HTXMLCallback_new (
	HTStream *		me,
	HTRequest *		request,
	HTFormat 		target_format,
	HTStream *		target_stream,
	XML_Parser              xmlparser,
	void *                  context);

/*
(
  Register Creation notification Callback
)
@@@Should be handled via XML names spaces@@@
*/

extern BOOL HTXMLCallback_registerNew (HTXMLCallback_new *, void * context);

/*
.
  XML Expat Stream to Libwww Structured Stream
.

This is a stream that converts from the expat stream to a
libwww structured stream. Again, the application
can
*/

#ifdef HT_STRUCT_XML_STREAM
extern BOOL HTXMLStructured_setHandlers(
	HTStream *			me,
	XML_StartElementHandler		start,
	XML_EndElementHandler		end, 
	XML_CharacterDataHandler	char_data,
	XML_DefaultHandler		def_handler);

extern BOOL HTXMLStructured_setUserData (HTStream * me, void * user_data);
extern HTStream * HTXMLStructured_new (const SGML_dtd * dtd, HTStructured * starget);
#endif  /* HT_STRUCT_XML_STREAM */

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTXML_H */

/*

  

  @(#) $Id$

*/
