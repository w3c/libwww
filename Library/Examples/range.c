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
    if (status == HT_PARTIAL_CONTENT) {
	HTAssocList * ranges = HTResponse_range(response);
	if (ranges) {
	    HTAssoc * pres;
	    BOOL first = YES;
	    while ((pres = (HTAssoc *) HTAssocList_nextObject(ranges))) {
		if (first) {
		    HTPrint("Ranges received: ");
		    first = NO;
		} else
		    HTPrint(", ");
		HTPrint("%s %s",
			HTAssoc_value(pres) ? HTAssoc_value(pres) : "<null>",
			HTAssoc_value(pres) ? HTAssoc_name(pres) : "<null>");
	    }
	}
    } else if (status == HT_LOADED)
	HTPrint("Total length of document: %ld", HTResponse_length(response));
     
    if (result && HTChunk_data(result)) {
	fprintf(stdout, "%s", HTChunk_data(result));
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
    char * range_unit = NULL;
    HTChunk * ranges = NULL;
    char * uri = NULL;

    /* Create a new premptive client */
    HTProfile_newNoCacheClient("RangeApp", "1.0");

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Add our own filter to update the history list */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Set trace messages and alert messages*/
#if 0
    HTSetTraceMessageMask("sop");
#endif
    HTAlert_setInteractive(NO);

    /* Setup a timeout on the request for 15 secs */
    HTHost_setEventTimeout(15000);

    /* Handle command line args */
    if (argc >= 3) {
	int arg;
	uri = argv[1];
	range_unit = argv[2];
	for (arg=3; arg<argc; arg++) {
	    char * range = argv[arg];
	    if (range) {

		/* Create a list to hold the ranges */
		if (!ranges)
		    ranges = HTChunk_new(64);
		else
		    HTChunk_putc(ranges, ',');
		HTChunk_puts(ranges, range);

	    }
	}
    }

    if (uri && range_unit && ranges) {

	/* Create a request */
	request = HTRequest_new();

	/* Get an anchor object for the URI */
	anchor = HTAnchor_findAddress(uri);

	/* Get the source stream asis */
	HTRequest_setOutputFormat(request, WWW_SOURCE);

	/* Set the ranges */
	HTRequest_addRange(request, range_unit, HTChunk_data(ranges));

	/* Issue the GET and store the result in a chunk */
	result = HTLoadAnchorToChunk(anchor, request);

	HTChunk_delete(ranges);

	/* Go into the event loop... */
	HTEventList_loop(request);

    } else {
	HTPrint("Type the URI to get the range(s) from\n");
	HTPrint("\t%s <uri> <range-unit> 1*(range-set)\n", argv[0]);
	HTPrint("where\n");
	HTPrint("\trange-unit = bytes | token\n");
	HTPrint("\trange-set = (first-byte-pos - [last-byte-pos]) |  - suffix-length\n");
	HTPrint("For example:\n");
	HTPrint("\t%s http://www.w3.org bytes \"0-499\" \"1000-1499\" \"-10\"\n", argv[0]);
    }

    return 0;
}
