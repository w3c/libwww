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
**      Parses RDF triples and spits out RDF triples. Example RDF files can be
**	found in the ./RDF folder and at "http://www.w3.org/Library/Examples/RDF"
**	Difference from stri is that we handle the HTRDF Object ourselves here.
**
**	Author: John Punin
*/

#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWXML.h"

PRIVATE HTRDF * rdf_parser = NULL;

/* ------------------------------------------------------------------------- */
/*				LIBWWW FILTERS				     */
/* ------------------------------------------------------------------------- */

PRIVATE void new_triple_handler (HTRDF * rdfp, HTTriple * t, void * context)
{
    if (rdfp && t) {
	HTPrint("Received new triple:\n");
	if (HTTriple_predicate(t)) HTPrint("\tPredicate: `%s\'\n", HTTriple_predicate(t));
	if (HTTriple_subject(t))   HTPrint("\tSubject  : `%s\'\n", HTTriple_subject(t));
	if (HTTriple_object(t))    HTPrint("\tObject   : `%s\'\n", HTTriple_object(t));
	HTPrint("\n");
    }
}

PRIVATE void new_parser_handler (HTStream *		me,
				 HTRequest *		request,
				 HTFormat 		target_format,
				 HTStream *		target_stream,
				 HTRDF *    	        rdfparser,
				 void *         	context)
{
    HTPrint("Received new RDF parser instance %p\n", rdfparser);
    if (rdfparser) {

	/* Remember the new RDF instance */
	rdf_parser = rdfparser;

	/* Register own triple callback just to follow what is going on */
	HTRDF_registerNewTripleCallback (rdf_parser, new_triple_handler, NULL);
    }
}

PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    /* We are done with this request */
    HTRequest_delete(request);
    
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
    char * cwd = HTGetCurrentDirectoryURL();

    /* Create a new premptive client */
    HTProfile_newHTMLNoCacheClient ("ParseRDF", "1.0");

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Set trace messages */
#if 0
    HTSetTraceMessageMask("sopx");
#endif

    /* Add our own termination filter */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Setup a timeout on the request for 15 secs */
    HTHost_setEventTimeout(15000);

    /* Handle command line args */
    if (argc >= 2) uri = HTParse(argv[1], cwd, PARSE_ALL);
    HT_FREE(cwd);

    /* Set up our RDF To Triple conveter stream */
    HTFormat_addConversion("text/rdf", "*/*", HTRDFParser_new, 1.0, 0.0, 0.0);

    /* Set our new RDF parser instance callback */
    HTRDF_registerNewParserCallback (new_parser_handler, NULL);

    if (uri) {
	HTRequest * request = HTRequest_new();
	HTAnchor * anchor = HTAnchor_findAddress(uri);
	BOOL status = NO;

	/* Start the GET request */
	status = HTLoadAnchor(anchor, request);

	/* Go into the event loop... */
	if (status == YES) HTEventList_loop(request);

    } else {
	HTPrint("Type the URI to parse\n");
	HTPrint("\t%s <uri>\n", argv[0]);
	HTPrint("For example:\n");
	HTPrint("\t%s http://www.yoursite.com/your.rdf\n", argv[0]);
    }

    return 0;
}
