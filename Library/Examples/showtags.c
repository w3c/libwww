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
**      Parses a HTML document and prints out all the embedded links to stdout
*/

#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWHTML.h"

PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    /* We are done with this request */
    HTRequest_delete(request);

    /* Terminate libwww */
    HTProfile_delete();

    exit(0);
}

PRIVATE void beginElement (HText * 		text,
			   int			element_number,
			   const BOOL *		present,
			   const char **	value)
{
    SGML_dtd * dtd = HTML_dtd();
    HTTag * tag = SGML_findTag(dtd, element_number);
    if (tag) {
	char * tagname = HTTag_name(tag);
	int maxcnt = HTTag_attributes(tag);
	BOOL found = NO;
	int cnt;
	fprintf(stdout, "Begin element: <%s> with attributes:",
		tagname ? tagname : "<unknown>");
	for (cnt=0; cnt<maxcnt; cnt++) {
	    if (present[cnt]) {
		char * attrname = HTTag_attributeName(tag, cnt);
		found = YES;
		fprintf(stdout, " %s", attrname ? attrname : "<unknown>");
		if (value[cnt]) fprintf(stdout, "=\"%s\"", value[cnt]);
	    }
	}
	if (!found) fprintf(stdout, " none");
    }
    fprintf(stdout, "\n");
}

PRIVATE void endElement (HText * text, int element_number)
{
    SGML_dtd * dtd = HTML_dtd();
    char * tagname = SGML_findTagName(dtd, element_number);
    fprintf(stdout, "End element:   </%s>\n", tagname ? tagname : "<unknown>");
}

PRIVATE void unparsedBeginElement (HText * text, const char * buf, int len)
{
    if (buf) fprintf(stdout, "Unknown begin: <%s>\n", buf);
}

PRIVATE void unparsedEndElement (HText * text, const char * buf, int len)
{
    if (buf) fprintf(stdout, "Unknown end:   </%s>\n", buf);
}

int main (int argc, char ** argv)
{
    char * uri = NULL;

    /* Create a new premptive client */
    HTProfile_newHTMLNoCacheClient ("ShowTags", "1.0");

    /* Add our own termination filter */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /*
    ** Register our HTML element handlers. We don't actually create a HText
    ** object as this is not needed. We only register the specific link
    ** callback.
    */
    HText_registerElementCallback(beginElement, endElement);
    HText_registerUnparsedElementCallback(unparsedBeginElement,
					  unparsedEndElement);

    /* Set trace messages and alert messages */
#if 0
    HTSetTraceMessageMask("sop");
#endif
    HTAlert_setInteractive(NO);

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
