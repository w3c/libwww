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
**      Issues a HEAD request and parses the header information returned in the
**	response
*/

#include "WWWLib.h"
#include "WWWApp.h"
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
    HTChunk * chunk = (HTChunk *) HTRequest_context(request);

    /* Check for status */
    HTPrint("Load resulted in status %d\n", status);

#if 0
    if (status == HT_LOADED && chunk && HTChunk_data(chunk))
	HTPrint("%s", HTChunk_data(chunk));
#endif

    /* Remember to delete our chunk of data */
    if (chunk) HTChunk_delete(chunk);
	
	/* we're not handling other requests */
	HTEventList_stopLoop ();
 
	/* stop here */
    return HT_ERROR;
}

int main (int argc, char ** argv)
{
    HTRequest * request;
    int status = 0; 

    HTProfile_newPreemptiveClient("HTTPHeadApplication", "1.0");
    request = HTRequest_new();
    if (argc >= 2) {
	char * url = argv[1];

	/* Turn on TRACE so we can see what is going on */
	if (argc == 3) HTSetTraceMessageMask(argv[2]);

	/* Need our own trace and print functions */
        HTPrint_setCallback(printer);
	HTTrace_setCallback(tracer);

	/* Set the output format to source and put output to stdout */
	HTRequest_setOutputStream(request, HTFWriter_new(request, stdout, YES));

	/* Add our own filter to handle termination */
	HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

	/* Start the load */
	if (url && *url)
	    status = HTHeadAbsolute(url, request);
	else
	  HTPrint("Bad parameters - please try again\n");
    } else {
	HTPrint("Type the URL to perform a HEAD request on.\n");
	HTPrint("\t%s <url> <trace>\n", argv[0]);
    }

	/* Go into the event loop... */
	if (status) HTEventList_loop(request);

	/* We are done with this request */
	HTRequest_delete(request);

 	/* Terminate libwww */
	HTProfile_delete();

    return 0;
}
