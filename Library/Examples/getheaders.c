/*
**	@(#) $Id$
**	
**	Other libwww samples can be found at "http://www.w3.org/Library/Examples"
**	
**	Copyright (c¨ 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**	AUTHOR: Vladimir Kozlov
**
**      Issues a HEAD request and parses the header information returned in the
**	response and prints out the information requested by the caller
*/

#include "WWWLib.h"
#include "WWWApp.h"
#include "WWWInit.h"

PRIVATE char * match = "*";

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
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTAssocList * headers = HTAnchor_header(anchor);

    /*
    **  Write out the remaining list of headers that we not already store
    **  in the index file.
    */
    if (headers) {
	HTAssocList * cur = headers;
	HTAssoc * pres;
	while ((pres = (HTAssoc *) HTAssocList_nextObject(cur))) {
	    char * name = HTAssoc_name(pres);
	    char * value = HTAssoc_value(pres);

	    /* Now see if we have a match for this header field */
	    if (match && HTStrCaseMatch(match, name))
		HTPrint("%s: %s\n", name, value);
	}
    }

	/* stop the event loop */
	HTEventList_stopLoop ();

	/* stop here */
	return HT_ERROR;
}

int main (int argc, char ** argv)
{
    HTRequest * request;
    HTProfile_newPreemptiveClient("HTTPHeadApplication", "1.0");
    request = HTRequest_new();
    
#if 0
    /* Turn on TRACE so we can see what is going on */
    HTSetTraceMessageMask("sop");
#endif

    /* We don't want any progress notifications */
    HTAlert_setInteractive(NO);

    /* Parse the command line options */
    if (argc >= 2) {
	char * url = argv[1];

	/* Set the header field match (if any) */
	if (argc >= 3) match = argv[2];

	/* Need our own trace and print functions */
        HTPrint_setCallback(printer);
	HTTrace_setCallback(tracer);

	/* Add our own filter to handle termination */
	HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

	/* Set the output format to source and put output to stdout */
	HTRequest_setOutputStream(request, HTFWriter_new(request, stdout, YES));

	/* Start the load */
	if (url && *url) {
	    HTHeadAbsolute(url, request);
		/* wait until the request has finished */
		HTEventList_loop (request);
	}
	else
	    HTPrint("Bad parameters - please try again\n");
    } else {
	HTPrint("Type the URL to perform a HEAD request on.\n");
	HTPrint("\t%s <url> <header-prefix>\n", argv[0]);
	HTPrint("where <header-prefix> is the prefix of the header fields\n");
	HTPrint("you want to see, for example 'content*. If no \'*\' is used'\n");
	HTPrint("then an exact match is needed. Default value is all header fields'\n");
    }

    HTRequest_delete(request);			/* Delete the request object */
    HTProfile_delete();
    return 0;
}
