/*
**	@(#) $Id$
**	
**	Other libwww samples can be found at "http://www.w3.org/Library/Examples"
**	
**	Copyright (c© 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**      Parses an XNL element using libwww and expat
*/

#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWXML.h"

/* ------------------------------------------------------------------------- */
/*				EXPAT HANDLERS				     */
/* ------------------------------------------------------------------------- */

PRIVATE void XML_startElement (void * userData,
	   	    	       const XML_Char *	name, const XML_Char ** atts)
{
    HTPrint("Start....... %s\n", name ? name : "<null>");
}

PRIVATE void XML_endElement (void * userData,
			     const XML_Char * name)
{
    HTPrint("End......... %s\n", name ? name : "<null>");
}

PRIVATE void XML_characterData (void * userData,
				const XML_Char * s, int len)
{
    return;
}

PRIVATE void XML_processingInstruction (void * userData,
					const XML_Char * target,
					const XML_Char * data)
{
    return;
}

/*
** This is called for any characters in the XML document for
** which there is no applicable handler.  This includes both
** characters that are part of markup which is of a kind that is
** not reported (comments, markup declarations), or characters
** that are part of a construct which could be reported but
** for which no handler has been supplied. The characters are passed
** exactly as they were in the XML document except that
** they will be encoded in UTF-8.  Line boundaries are not normalized.
** Note that a byte order mark character is not passed to the default handler.
** If a default handler is set, internal entity references
** are not expanded. There are no guarantees about
** how characters are divided between calls to the default handler:
** for example, a comment might be split between multiple calls.
*/
PRIVATE void XML_default (void * userData,
			  const XML_Char * s, int len)
{
    return;
}

/* 
** This is called for a declaration of an unparsed (NDATA)
** entity.  The base argument is whatever was set by XML_SetBase.
** The entityName, systemId and notationName arguments will never be null.
** The other arguments may be.
*/
PRIVATE void XML_unparsedEntityDecl (void * userData,
				     const XML_Char * entityName,
				     const XML_Char * base,
				     const XML_Char * systemId,
				     const XML_Char * publicId,
				     const XML_Char * notationName)
{
    return;
}

/* 
** This is called for a declaration of notation.
** The base argument is whatever was set by XML_SetBase.
** The notationName will never be null.  The other arguments can be.
*/
PRIVATE void XML_notationDecl (void * userData,
			       const XML_Char * notationName,
			       const XML_Char * base,
			       const XML_Char * systemId,
			       const XML_Char * publicId)
{
    return;
}

/* 
** This is called for a reference to an external parsed general entity.
** The referenced entity is not automatically parsed.
** The application can parse it immediately or later using
** XML_ExternalEntityParserCreate.
** The parser argument is the parser parsing the entity containing the reference;
** it can be passed as the parser argument to XML_ExternalEntityParserCreate.
** The systemId argument is the system identifier as specified in the entity 
** declaration; it will not be null.
** The base argument is the system identifier that should be used as the base for
** resolving systemId if systemId was relative; this is set by XML_SetBase;
** it may be null.
** The publicId argument is the public identifier as specified in the entity declaration,
** or null if none was specified; the whitespace in the public identifier
** will have been normalized as required by the XML spec.
** The openEntityNames argument is a space-separated list of the names of the entities
** that are open for the parse of this entity (including the name of the referenced
** entity); this can be passed as the openEntityNames argument to
** XML_ExternalEntityParserCreate; openEntityNames is valid only until the handler
** returns, so if the referenced entity is to be parsed later, it must be copied.
** The handler should return 0 if processing should not continue because of
** a fatal error in the handling of the external entity.
** In this case the calling parser will return an XML_ERROR_EXTERNAL_ENTITY_HANDLING
** error.
** Note that unlike other handlers the first argument is the parser, not userData.
*/
PRIVATE int XML_externalEntityRef (XML_Parser parser,
				   const XML_Char * openEntityNames,
				   const XML_Char * base,
				   const XML_Char * systemId,
				   const XML_Char * publicId)
{
    return 0;
}

/* 
** This is called for an encoding that is unknown to the parser.
** The encodingHandlerData argument is that which was passed as the
** second argument to XML_SetUnknownEncodingHandler.
** The name argument gives the name of the encoding as specified in
** the encoding declaration.
** If the callback can provide information about the encoding,
** it must fill in the XML_Encoding structure, and return 1.
** Otherwise it must return 0.
** If info does not describe a suitable encoding,
** then the parser will return an XML_UNKNOWN_ENCODING error.
*/
PRIVATE int XML_unknownEncoding (void * encodingHandlerData,
				 const XML_Char * name,
				 XML_Encoding * info)
{
    return 0;
}

/* ------------------------------------------------------------------------- */
/*			     HTXML STREAM HANDLERS			     */
/* ------------------------------------------------------------------------- */

PRIVATE void HTXML_setHandlers (XML_Parser me)
{
    XML_SetElementHandler(me, XML_startElement, XML_endElement);
    XML_SetCharacterDataHandler(me, XML_characterData);
    XML_SetProcessingInstructionHandler(me, XML_processingInstruction);
    XML_SetDefaultHandler(me, XML_default);
    XML_SetUnparsedEntityDeclHandler(me, XML_unparsedEntityDecl);
    XML_SetNotationDeclHandler(me, XML_notationDecl);
    XML_SetExternalEntityRefHandler(me, XML_externalEntityRef);
    XML_SetUnknownEncodingHandler(me, XML_unknownEncoding, NULL);
}

PRIVATE void HTXML_newInstance (HTStream *		me,
				HTRequest *		request,
				HTFormat 		target_format,
				HTStream *		target_stream,
				XML_Parser              xmlparser,
				void * 			context)
{
    if (me && xmlparser) HTXML_setHandlers(xmlparser);
}

/* ------------------------------------------------------------------------- */
/*				LIBWWW FILTERS				     */
/* ------------------------------------------------------------------------- */

PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    /* We are done with this request */
    HTRequest_delete(request);

    HTPrint("If you didn't see any tags then it is likely that your document wasn't xml\n");

    /* Terminate libwww */
    HTProfile_delete();

    exit(0);
}

/* ------------------------------------------------------------------------- */

PRIVATE int printer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stdout, fmt, pArgs));
}

PRIVATE int tracer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}

int main (int argc, char ** argv)
{
    char * uri = NULL;

    /* Create a new premptive client */
    HTProfile_newHTMLNoCacheClient ("ShowXML", "1.0");

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Set trace messages */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Add our own termination filter */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Register our new XML Instance handler */
    HTXMLCallback_registerNew (HTXML_newInstance, NULL);

    /* Setup a timeout on the request for 15 secs */
    HTHost_setEventTimeout(15000);

    /* Handle command line args */
    if (argc >= 2)
	uri = HTParse(argv[1], NULL, PARSE_ALL);

    if (uri) {
	HTRequest * request = NULL;
	HTAnchor * anchor = NULL;
	BOOL status = NO;

	/* Create a request */
	request = HTRequest_new();

	/* Get an anchor object for the URI */
	anchor = HTAnchor_findAddress(uri);

	/* Issue the GET and store the result in a chunk */
	status = HTLoadAnchor(anchor, request);

	/* Go into the event loop... */
	if (status == YES) HTEventList_loop(request);

    } else {
	HTPrint("Type the URI to parse\n");
	HTPrint("\t%s <uri>\n", argv[0]);
	HTPrint("For example:\n");
	HTPrint("\t%s http://www.yoursite.com/your.xml\n", argv[0]);
    }

    return 0;
}
