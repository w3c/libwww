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
**      Shows how the HTTP 101 Switching Protocol status code can be used in libwww
**	If this program receives a 101 status code then it stops parsing HTTP and
**	writes everything ASIS to stdout.
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
    HTAnchor * anchor = NULL;
    char * uri = NULL;

    /* Create a new premptive client */
    HTProfile_newNoCacheClient("UpgradeApp", "1.0");

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Set trace messages and alert messages*/
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Add our own AFTER filter to terminate the app */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Don't ask - don't tell */
    HTAlert_setInteractive(NO);

    /* Setup a timeout on the request for 15 secs */
    HTHost_setEventTimeout(15000);

    /* Handle command line args */
    if (argc >= 2) uri = argv[1];

    if (uri && *uri) {

	/* Create a request */
	request = HTRequest_new();

	/* Get an anchor object for the URI */
	anchor = HTAnchor_findAddress(uri);

	/* Get the source stream asis */
	HTRequest_setOutputFormat(request, WWW_SOURCE);

	/* Get the source stream asis */
	HTRequest_setOutputStream(request, HTFWriter_new(request, stdout, YES));

	/* Issue the GET */
	if (HTLoadAnchor(anchor, request)) {

	    /* Go into the event loop... */
	    HTEventList_newLoop();
	}

    } else {
	HTPrint("Type the URI to get (to test for upgrade).\n");
	HTPrint("If an 101 Switching Protocols status is recieved\n");
        HTPrint("then print everything following to stdout.\n\n");
	HTPrint("\t%s <uri>\n", argv[0]);
    }

    return 0;
}
