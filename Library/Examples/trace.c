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
**	Sample showing how to use a TRACE request
*/

#include "WWWLib.h"
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
    /* We are done with this request */
    HTRequest_delete(request);

    /* Terminate libwww */
    HTProfile_delete();

    exit(0);
}

int main (int argc, char ** argv)
{
    HTRequest * request = NULL;
    HTAnchor * dst = NULL;
    char * dst_str = NULL;
    int max_forward = 1;
    BOOL status = NO;

    /* Create a new premptive client */
    HTProfile_newNoCacheClient("libwww-TRACE", "1.0");

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Add our own filter to handle termination */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Handle command line args */
    if (argc >= 3) {
	dst_str = argv[1];
	max_forward = atoi(argv[2]);
	if (max_forward<0 || max_forward>10) max_forward=1;
    } else {
	HTPrint("Type the URI of the destination you want to TRACE and the max number of hops you want to try\n");
	HTPrint("\t%s <destination> <hops>\n", argv[0]);
	HTPrint("For example, %s http://www.w3.org 2\n", argv[0]);
	return -1;
    }

    if (dst_str && *dst_str) {

	/* Make source relative to where we are */
	char * cwd = HTGetCurrentDirectoryURL();

	HTPrint("Tracing request to %s with %d number of hops\n",
		dst_str, max_forward);

	/* Create a request */
	request = HTRequest_new();

	/* We don't wany any progress notification or other user stuff */
	HTAlert_setInteractive(YES);

	/* Set the output format to source and put output to stdout */
	HTRequest_setOutputFormat(request, WWW_SOURCE);
	HTRequest_setOutputStream(request, HTFWriter_new(request, stdout, YES));

	/* Set the max forwards */
	HTRequest_setMaxForwards(request, max_forward);

	/* Get an anchor object for the destination URI */
	dst = HTAnchor_findAddress(dst_str);

	/* POST the source to the dest */
	status = HTTraceAnchor(dst, request);

	/* We don't need these anymore */
	HT_FREE(cwd);

	/* Go into the event loop... */
	if (status == YES) HTEventList_loop(request);

    }

    return 0;
}
