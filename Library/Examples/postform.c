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
**      Posts form data to an HTTP server and saves the result in a chunk
*/

#include "WWWLib.h"
#include "WWWInit.h"

PRIVATE HTChunk * result = NULL;

PRIVATE int printer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stdout, fmt, pArgs));
}

PRIVATE int tracer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}

PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    if (status == HT_LOADED && result && HTChunk_data(result)) {
	HTPrint("%s", HTChunk_data(result));
	HTChunk_delete(result);
    }

    /* We are done with this request */
    HTRequest_delete(request);

    /* Terminate libwww */
    HTProfile_delete();

    exit(0);
}

int main (int argc, char ** argv)
{
    HTRequest * request = NULL;
    HTAnchor * anchor = NULL;
    HTAssocList * formfields = NULL;
    char * uri = NULL;

    /* Create a new premptive client */
    HTProfile_newNoCacheClient("TestApp", "1.0");

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Get trace messages */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Add our own filter to update the history list */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Set the timeout for long we are going to wait for a response */
    HTHost_setEventTimeout(20000);

    /* Handle command line args */
    if (argc >= 2) {
	int arg;
	uri = argv[1];
	for (arg=2; arg<argc; arg++) {
	    char * string = argv[arg];
	    
	    /* Create a list to hold the form arguments */
	    if (!formfields) formfields = HTAssocList_new();

	    /* Parse the content and add it to the association list */
	    HTParseFormInput(formfields, string);
	}
    }

    if (uri && formfields) {

	/* Create a request */
	request = HTRequest_new();

	/* Set the default output to "asis" */
	HTRequest_setOutputFormat(request, WWW_SOURCE);

	/* Get an anchor object for the URI */
	anchor = HTAnchor_findAddress(uri);

	/* Post the data and get the result in a chunk */
	result = HTPostFormAnchorToChunk(formfields, anchor, request);

	/* Clean up the form fields */
	HTAssocList_delete(formfields);

	/* Go into the event loop... */
	HTEventList_loop(request);

    } else {
	HTPrint("Type the URI to post to and the arguments for the POST operation. Encode spaces as '+'\n");
	HTPrint("\t%s <uri> 'a=1' 'b=+2+' 'c=3 ...'\n", argv[0]);
    }

    return 0;
}
