/*

  					W3C Sample Code Library libwww Rich Hypertext Object


!
  HyperText Object Builder
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is the interface that &nbsp;you can use to build a parsed HTML object
(called an hypertext object) as output of the libwww
HTML parser. It is based on callback functions that the application can
register as needed in order to get the various events out of the HTML parser.
These are the different events that can be registered:

	 
	   o 
	     When parsing is about to start or terminate
  o 
	     When a chunk of plain text has been found
  o 
	     When a link has been found in some HTML element
  o 
	     When an HTML element is about to start or end
  o 
	     Handler for unparsed elements
  o 
	     Handler for unparsed entities

	 
Building an hypertext object can contain references to stylesheets. Libwww
doesn't come with a stylesheet implementation but it does have a
generic stylesheet manager which can be used for
registering and looking up styles. As an example, you can see how the libwww
robot uses the link callout whereas
the Line Mode Browser has a complete
implementation in the 
GridText module.
*/

#ifndef HTEXT_H
#define HTEXT_H

#include "HTAnchor.h"
#include "HTStream.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  The HText Object
.

The HText object is only known as a name to libwww - it isn't defined
or used anywhere as an actual structure. The application can use the HText
object to contain the information needed to build a parsed object - a hypertext
object - as data arrives from the libwww HTML parser.
When a HText object is to be created, this module calls out to any registered
creation method and in turn uses the newly created HText object in every
subsequent call to any registered callback routine as the HTML stream is
parsed.
*/

typedef struct _HText HText;

/*
.
  Callback for Creating and Deleting an HText Object
.

When an HTML stream is created, the
HTML parser checks to see if there are any callbacks
registered for handling the HText object. If this is
the case then the creation callback is called so that the application can
create a new HText object. After that the various callbacks
are called (if registered) as text comes in and
gets parsed.
*/

typedef HText * HText_new (
	HTRequest *		request,
	HTParentAnchor *	anchor,
	HTStream *		output_stream);

typedef BOOL HText_delete (HText * me);

/*
(
  Register Creation/Deletion Callbacks
)
*/

extern BOOL HText_registerCDCallback (HText_new *, HText_delete *);
extern BOOL HText_unregisterCDCallback (void);

/*
.
  Callback for Start and Terminate Parsing
.

The call sequence is the following: First HText_build is called
with the status of HTEXT_BEGIN, then any combination of other append
calls, and at the end HText_build is called with the termination
status - if it terminated normally then the status is HTEXT_END,
if the download was aborted the status is HTEXT_ABORT.
*/

typedef enum _HTextStatus {
    HTEXT_BEGIN,
    HTEXT_END,
    HTEXT_ABORT
} HTextStatus;

typedef void HText_build (HText * text, HTextStatus status);

/*
(
  Register Start/End Callback
)
*/

extern BOOL HText_registerBuildCallback (HText_build *);
extern BOOL HText_unregisterBuildCallback (void);

/*
.
  Callback for Handling Chunks of Text
.

THE STRING IS NOT ZERO-TERMINATED!!! When plain text is
found then it can be passed to the application via this function. HTML text
is already stripped for unneeded white space whereas literal text isn't.
The text is of course to be taken "relative" to which element is is found
within - this is not checked by the HTML parser but can be checked by the
application by using the Element callback function.
*/

typedef void HText_addText (
	HText * 	text,
	const char * 	buffer,
	int		length);

/*
(
  Register Text Callback
)
*/

extern BOOL HText_registerTextCallback (HText_addText *);
extern BOOL HText_unregisterTextCallback (void);

/*
.
  Callback for Handling Hypertext Links
.

Whenever a link is found in the HTML stream, be it from an anchor element,
an inlined image, etc. then it is picked up and this called out to this function
with the parameters necessary to be able to see what type of link it is (the
element and the name of the attribute where it was found). The element number
and attribute number are integers that are defined by the
HTML DTD used by the HTMl parser. The
present and value arrays contains all the
SGML attributes and values found in the element
untouched. Again, the array is defined by the HTML
DTD used by the HTML parser.
*/

typedef void HText_foundLink (
	HText * 	text,
	int		element_number,
	int		attribute_number,
	HTChildAnchor *	anchor,
	const BOOL *	present,
	const char **	value);

/*
(
  Register Link Callback
)
*/

extern BOOL HText_registerLinkCallback (HText_foundLink *);
extern BOOL HText_unregisterLinkCallback (void);

/*
.
  Callback for Handling HTML Elements
.

Whenever an element is found in the HTML stream, be it an IMG element, a
BODY element, etc. then it is picked up and this called out to this function
with the parameters necessary to be able to see what type of element it is.
The element number and attribute number are integers that are defined by
the HTML DTD used by the HTMl parser. The
present and value arrays contains all the
SGML attributes and values found in the element
untouched. Again, the array is defined by the HTML
DTD used by the HTML parser.
*/

typedef void HText_beginElement (
	HText * 	text,
	int		element_number,
	const BOOL *	present,
	const char **	value);

typedef void HText_endElement (
	HText * 	text,
	int		element_number);

/*
(
  Register HTML Element Callback
)
*/

extern BOOL HText_registerElementCallback (HText_beginElement *, HText_endElement *);
extern BOOL HText_unregisterElementCallback (void);

/*
.
  Callback for Unparsed Elements
.

Whenever an element is found which is not known by the HTML DTD, then the
information is passed to the unparsed element handlers.
*/

typedef void HText_unparsedBeginElement (
	HText *		HText,
	const char * 	buffer,
	int		length);

typedef void HText_unparsedEndElement (
	HText *		HText,
	const char * 	buffer,
	int		length);

/*
(
  Register Unparsed Element Callback
)
*/

extern BOOL HText_registerUnparsedElementCallback (
	HText_unparsedBeginElement *,
	HText_unparsedEndElement *);
extern BOOL HText_unregisterUnparsedElementCallback (void);

/*
.
  Callback for Unparsed Entity
.

Whenever an entity is found which is not known by the HTML DTD, then the
information is passed to the default handler.
*/

typedef void HText_unparsedEntity (
	HText *		HText,
	const char * 	buffer,
	int		length);

/*
(
  Register Unparsed Entity Callback
)
*/

extern BOOL HText_registerUnparsedEntityCallback (HText_unparsedEntity *);
extern BOOL HText_unregisterUnparsedEntityCallback (void);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTEXT_H */

/*

  

  @(#) $Id$

*/
