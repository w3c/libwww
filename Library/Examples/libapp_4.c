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
