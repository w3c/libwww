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
**      Issues a range request to an HTTP server and saves the result in a chunk
*/
#include "WWWLib.h"
#include "WWWHTTP.h"
#include "WWWInit.h"

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
    /* Check for status */
    /* HTPrint("Load resulted in status %d\n", status); */
	
	/* we're not handling other requests */
	HTEventList_stopLoop ();
 
	/* stop here */
    return HT_ERROR;
}

int main (int argc, char ** argv)
{
    HTRequest * request = HTRequest_new();
    HTList * converters = HTList_new();		/* List of converters */
    HTList * encodings = HTList_new();		/* List of encoders */
    HTChunk * chunk = NULL;
    char * url = argc==2 ? argv[1] : NULL;

    /* Initialize libwww core */
    HTLibInit("TestApp", "1.0");

    /* Gotta set up our own traces */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Turn on TRACE so we can see what is going on */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* On windows we must always set up the eventloop */
#ifdef WWW_WIN_ASYNC
    HTEventInit();
#endif
    
    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of protocol modules */
    HTProtocolInit();

    /* Register the default set of BEFORE and AFTER callback functions */
    HTNetInit();

    /* Register the default set of converters */
    HTConverterInit(converters);
    HTFormat_setConversion(converters);

    /* Register the default set of transfer encoders and decoders */
    HTTransferEncoderInit(encodings);
    HTFormat_setTransferCoding(encodings);

    /* Register the default set of MIME header parsers */
    HTMIMEInit();

    /* Add our own filter to handle termination */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Set up the request and pass it to the Library */
    HTRequest_setOutputFormat(request, WWW_SOURCE);
    HTRequest_setPreemptive(request, YES);

    if (url) {
	char * cwd = HTGetCurrentDirectoryURL();
	char * absolute_url = HTParse(url, cwd, PARSE_ALL);
	HTAnchor * anchor = HTAnchor_findAddress(absolute_url);
	chunk = HTLoadAnchorToChunk(anchor, request);
	HT_FREE(absolute_url);
	HT_FREE(cwd);

	/* If chunk != NULL then we have the data */
	if (chunk) {
	   char * string;
	   /* wait until the request is over */
	   HTEventList_loop (request);
	   string = HTChunk_toCString(chunk);
	   HTPrint("%s", string ? string : "no text");
	   HT_FREE(string);
	}
    } else {
	HTPrint("Type the URL you want to accces on the command line\n");
    }

    /* Clean up the request */
    HTRequest_delete(request);
    HTFormat_deleteAll();

    /* On windows, shut down eventloop as well */
#ifdef WWW_WIN_ASYNC
    HTEventTerminate();
#endif
   
    /* Terminate the Library */
    HTLibTerminate();
    return 0;
}
