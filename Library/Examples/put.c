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
**	Sample showing how to PUT a document to an HTTP server
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
    HTAnchor * src = NULL;
    HTAnchor * dst = NULL;
    char * src_str = NULL;
    char * dst_str = NULL;
    BOOL status = NO;

    /* Create a new premptive client */
    HTProfile_newNoCacheClient("libwww-PUT", "1.0");

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* And the traces... */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Add our own filter to update the history list */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Handle command line args */
    if (argc >= 3) {
	src_str = argv[1];
	dst_str = argv[2];
    } else {
	HTPrint("Type the URI of the source and the URI of the destination.\n");
	HTPrint("\t%s <src> <dst>\n", argv[0]);
	HTPrint("For example, %s http://www.w3.org http://myserver/destination.html\n",
	       argv[0]);
	return -1;
    }

    if (src_str && *src_str && dst_str && *dst_str) {

	/* Make source relative to where we are */
	char * cwd = HTGetCurrentDirectoryURL();
	char * full_src_str = HTParse(src_str, cwd, PARSE_ALL);

	HTPrint("Saving %s to %s\n", full_src_str, dst_str);

	/* Create a request */
	request = HTRequest_new();

	/* Get an anchor object for the src and dest URIs */
	src = HTAnchor_findAddress(full_src_str);
	dst = HTAnchor_findAddress(dst_str);

	/* PUT the source to the dest */
	status = HTPutDocumentAnchor(HTAnchor_parent(src), dst, request);

	/* We don't need these anymore */
	HT_FREE(cwd);
	HT_FREE(full_src_str);

	/* Go into the event loop... */
	if (status == YES) HTEventList_loop(request);

    }

    return 0;
}
