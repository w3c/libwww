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
**      Parses a HTML document and prints out all the text ASIS stdout
*/

#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWHTML.h"

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

PRIVATE void addText (HText * text, const char * buf, int len)
{
    if (buf) fwrite(buf, 1, len, stdout);
}

int main (int argc, char ** argv)
{
    char * uri = NULL;

    /* Create a new premptive client */
    HTProfile_newHTMLNoCacheClient ("ShowTags", "1.0");

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Set trace messages and alert messages */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Add our own termination filter */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /*
    ** Register our HTML element handler. We don't actually create a HText
    ** object as this is not needed. We only register the specific link
    ** callback.
    */
    HText_registerTextCallback(addText);

    /* Setup a timeout on the request for 15 secs */
    HTHost_setEventTimeout(15000);

    /* Handle command line args */
    if (argc >= 2)
	uri = HTParse(argv[1], NULL, PARSE_ALL);

    if (uri) {
	HTRequest * request = NULL;
	HTAnchor * anchor = NULL;
	BOOL status = NO;

	/* Create a request */
	request = HTRequest_new();

	/* Get an anchor object for the URI */
	anchor = HTAnchor_findAddress(uri);

	/* Issue the GET and store the result in a chunk */
	status = HTLoadAnchor(anchor, request);

	/* Go into the event loop... */
	if (status == YES) HTEventList_loop(request);

    } else {
	printf("Type the URI to print out a list of embedded links\n");
	printf("\t%s <uri>\n", argv[0]);
	printf("For example:\n");
	printf("\t%s http://www.w3.org\n", argv[0]);
    }

    return 0;
}
