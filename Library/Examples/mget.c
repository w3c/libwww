/*
**	@(#) $Id$
**	
**	More libwww samples can be found at "http://www.w3.org/Library/Examples/"
**	
**	Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**	Sample showing how fire off many GET requests and then enter the event loop
**	This does only work on GET and HEAD methods as other methods can't be
**	pipelined this way. If issuing lots of POST requests, for example, then
**	use the HTTP_setConnectionMode(HTTP_11_NO_PIPELINING) function call
*/

#include "WWWLib.h"
#include "WWWInit.h"

#define MAX_COUNT	1024

PRIVATE int remaining = 0;

/* ----------------------------------------------------------------- */

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
    HTPrint("Load %d resulted in status %d\n", remaining, status);

#if 0
    if (status == HT_LOADED && chunk && HTChunk_data(chunk))
	HTPrint("%s", HTChunk_data(chunk));
#endif

    /* Remember to delete our chunk of data */
    if (chunk) HTChunk_delete(chunk);
	
    /* We are done with this request */
    HTRequest_delete(request);

    if (--remaining <= 0) {

	/* Terminate libwww */
	HTProfile_delete();

	exit(0);
    }

    return HT_OK;
}

int main (int argc, char ** argv)
{
    HTRequest * request = NULL;
    char * addr = NULL;
    int max_cnt = 0;
    BOOL status = YES;

    /* Create a new premptive client */
    HTProfile_newNoCacheClient("libwww-MGET", "1.0");

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Add our own filter to handle termination */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Turn on tracing */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Handle command line args */
    if (argc >= 3) {
	addr = argv[1];
	max_cnt = atoi(argv[2]);
	if (max_cnt<0 || max_cnt>MAX_COUNT) max_cnt=1;
	remaining = max_cnt;
    } else {
	HTPrint("Type the URI of the destination you want to GET and the number of times you want to get it\n");
	HTPrint("\t%s <destination> <count>\n", argv[0]);
	HTPrint("For example, %s http://www.w3.org 2\n", argv[0]);
	return -1;
    }

    if (addr && *addr) {
	HTChunk * chunk;
	int cnt = 0;

	/* We don't wany any progress notification or other user stuff */
	HTAlert_setInteractive(NO);

	/* Now issue the requests */
	HTPrint("Issuing %d GET request(s) on `%s\'\n", max_cnt, addr);

	for (cnt=0; cnt<max_cnt; cnt++) {

	    /* Create a request */
	    request = HTRequest_new();

	    /* Set the output format to source */
	    HTRequest_setOutputFormat(request, WWW_SOURCE);

	    /* Now start the load */
	    if ((chunk = HTLoadToChunk(addr, request)) == NULL) {
		status = NO;
		break;
	    } else
		HTRequest_setContext(request, chunk);
	}

	/* Go into the event loop... */
	if (status) HTEventList_loop(request);

    }

    return 0;
}
