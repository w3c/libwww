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
**	Sample showing how to load a document and save it to local file using
**	blocking I/O
*/

#include "WWWLib.h"
#include "WWWInit.h"

/*
**	Loads a URL to a local file
*/
int main (int argc, char ** argv)
{
    HTRequest * request;
    HTProfile_newPreemptiveClient("TestApp", "1.0");
    request = HTRequest_new();
    if (argc == 3) {
	char * url = argv[1];
	char * filename = argv[2];
	if (url && *url && filename && *filename)
	    HTLoadToFile(url, request, filename);
	else
	    printf("Bad parameters - please try again\n");
    } else {
	printf("Type the URL to fetch and the name of the local file to put it in\n");
	printf("\t%s <url> <filename>\n", argv[0]);
    }
    HTRequest_delete(request);			/* Delete the request object */
    HTProfile_delete();
    return 0;
}
