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
**	Sample showing how to PUT data from memory to an HTTP server
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
    HTParentAnchor * src = NULL;
    HTAnchor * dst = NULL;
    char * dst_str = NULL;
    char * data = NULL;
    BOOL status = NO;

    /* Create a new premptive client */
    HTProfile_newNoCacheClient("libwww-PUT", "1.0");

    /* Gotta set up our own traces */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Add our own filter to update the history list */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Set trace messages and alert messages*/
#if 0
    HTSetTraceMessageMask("p");
#endif

    /* Handle command line args */
    if (argc >= 3) {
	dst_str = argv[1];
	data = argv[2];
    } else {
	HTPrint("Type the URI of the destination you want to create/update and the contents that you want to save.\n");
	HTPrint("\t%s <destination> <data>\n", argv[0]);
	HTPrint("For example, %s http://myserver/destination.html \"This is some testdata\"\n",
	       argv[0]);
	return -1;
    }

    if (data && *data && dst_str && *dst_str) {

	HTPrint("Saving to %s\n", dst_str);

	/* Create a request */
	request = HTRequest_new();

	/* Get an anchor object for the destination URI */
	dst = HTAnchor_findAddress(dst_str);

	/*
	** Dream up a source anchor (an editor can for example use this).
	** After creation we associate the data that we want to save and
	** set some metadata about what the data is. More formats can be found
	** ../src/HTFormat.html
	*/
	src = HTTmpAnchor(NULL);
	HTAnchor_setDocument(src, data);
	HTAnchor_setFormat(src, WWW_PLAINTEXT);

	/*
	** If not saving to an HTTP/1.1 server then content length MUST be
	** there. If HTTP/1.1 then it doesn't matter as we just use chunked
	** encoding under the covers
	*/
	HTAnchor_setLength(src, strlen(data));

	/* PUT the source to the dest */
	status = HTPutAnchor(src, dst, request);

	/* Go into the event loop... */
	if (status == YES) HTEventList_loop(request);

    }

    return 0;
}
