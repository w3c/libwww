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
**      Gets the same document n times where n is specified as an option. The
**	program uses blocking sockets
*/

#include "WWWLib.h"
#include "WWWHTTP.h"
#include "WWWApp.h"
#include "WWWInit.h"

#define MAX_COUNT	1024

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
    HTRequest * request = NULL;
    HTChunk * chunk = NULL;
    char * uri = NULL;
    int arg = 0;
    int maxcount = 1;

    for (arg=1; arg<argc; arg++) {
	if (*argv[arg] == '-') {
	    if (!strcmp(argv[arg], "-n")) {
		maxcount  = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : 1;
		
	    } else {
		fprintf(stderr, "Bad Argument (%s)\n", argv[arg]);
	    }
	} else {
	    uri = argv[arg];
	}
    }

    /* Initialize libwww core */
    HTProfile_newPreemptiveClient("TestApp", "1.0");

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);
    
    /* Don't say anything */
    HTAlert_setInteractive(NO);

    /* Turn on TRACE so we can see what is going on */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    if (uri && maxcount<MAX_COUNT) {
	char * cwd = HTGetCurrentDirectoryURL();
	char * absolute_uri = HTParse(uri, cwd, PARSE_ALL);
	time_t local = time(NULL);
	ms_t start = HTGetTimeInMillis();
	ms_t end = -1;
	int cnt;
	HTPrint("Starting downloading %s %d time(s) at %s\n",
		uri, maxcount, HTDateTimeStr(&local, YES));
	for (cnt=0; cnt<maxcount; cnt++) {
	    request = HTRequest_new();

	    /* Set up the request and pass it to the Library */
	    HTRequest_setOutputFormat(request, WWW_SOURCE);

		/* Add our own filter to handle termination */
	    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

	    if (uri) {
		chunk = HTLoadToChunk(absolute_uri, request);

		/* If chunk != NULL then we have the data */
		if (chunk) {
		    char * string;
		    /* wait until the request has finished */
		    HTEventList_loop (request);
		    /* get the data */
		    string = HTChunk_toCString(chunk);
		    HT_FREE(string);
		}
	    }

	    /* Clean up the request */
	    HTRequest_delete(request);

	}

	local = time(NULL);
	end = HTGetTimeInMillis();
	HTPrint("Ending at %s - spent %ld ms\n",
		HTDateTimeStr(&local, YES), end-start);

	HT_FREE(absolute_uri);
	HT_FREE(cwd);
    } else {
	HTPrint("Downloads the same URI n times\n");
	HTPrint("Syntax: %s -n <count> <URI>\n", argv[0]);
    }

    /* Terminate the Library */
    HTProfile_delete();
    HTPrint("\n");
    return 0;
}
