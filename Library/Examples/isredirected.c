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
    /*
    **  If some sort of redirection then print out the address
    **  we are redirected to.
    */
    if (status == HT_PERM_REDIRECT || status == HT_FOUND || 
	status == HT_SEE_OTHER || status == HT_PROXY_REDIRECT ||
	status == HT_TEMP_REDIRECT) {
	char * addr = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
	char * re_addr = HTAnchor_address(HTResponse_redirection(response));
	time_t retry_after = HTResponse_retryTime(response);
	HTPrint("The address `%s\' was redirected to `%s\'\n",
		addr, re_addr ? re_addr : "<none>");
	HT_FREE (re_addr);
	if (retry_after >= 0)
	    HTPrint("A retry after period of %d secs was given\n", retry_after);
    } else {
	HTPrint("Request didn't result in a redirection\n");
    }

    /* We are done with this request */
    HTRequest_delete(request);

    /* Terminate libwww */
    HTProfile_delete();

    /* As we have nothing else todo, we may as well exit */
    exit(0);
}

int main (int argc, char ** argv)
{
    HTRequest * request = NULL;
    HTAnchor * dst = NULL;
    char * dst_str = NULL;
    BOOL status = NO;

    /* Create a new premptive client */
    HTProfile_newRobot("catch-redirect", "1.0");

    /* Gotta set up our own traces */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Turn on TRACE so we can see what is going on */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Set a timeout to 10 secs */
    HTHost_setEventTimeout(10000);

    /* Add our own filter to handle termination */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Handle command line args */
    if (argc >= 2) {
	dst_str = argv[1];
    } else {
	HTPrint("Type the URI of the destination you want to check whether it redirected or not\n");
	HTPrint("\t%s <address>\n", argv[0]);
	HTPrint("For example, %s http://www.w3.org\n", argv[0]);
	return -1;
    }

    if (dst_str && *dst_str) {

	HTPrint("Sending request to %s\n", dst_str);

	/* Create a request */
	request = HTRequest_new();

	/* We don't wany any progress notification or other user stuff */
	HTAlert_setInteractive(NO);

	/* Get an anchor object for the destination URI */
	dst = HTAnchor_findAddress(dst_str);

	/* POST the source to the dest */
	status = HTHeadAnchor(dst, request);

	/* Go into the event loop... */
	if (status == YES) HTEventList_loop(request);

    }

    return 0;
}
