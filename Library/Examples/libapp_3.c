#include "WWWLib.h"
#include "WWWStream.h"
#include "WWWInit.h"

/*
**	Loads a URL and count the content length
*/
int main (int argc, char ** argv)
{
    HTRequest * request;
    HTProfile_newPreemptiveRobot("TestApp", "1.0");
    request = HTRequest_new();
    HTRequest_setOutputFormat(request, WWW_SOURCE);
    if (argc == 2) {
	char * url = argv[1];
	if (url && *url) {
	    HTParentAnchor * anchor=(HTParentAnchor*)HTAnchor_findAddress(url);
	    BOOL status =
		HTLoadToStream(url,
			       HTContentCounter(HTBlackHole(), request,0x2000),
			       request);
	    if (status)
		printf("Content length found to be %ld bytes\n",
		       HTAnchor_length(anchor));
	    else
		printf("Content length could not be found\n");
	} else
	    printf("Bad parameters - please try again\n");
    } else {
	printf("Type the URL to fetch\n");
    }
    HTRequest_delete(request);			/* Delete the request object */
    HTProfile_delete();
    return 0;
}
