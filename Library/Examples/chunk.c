#include "WWWLib.h"
#include "WWWHTTP.h"
#include "WWWInit.h"

int main (int argc, char ** argv)
{
    HTRequest * request = HTRequest_new();
    HTChunk * chunk = NULL;
    char * url = argc==2 ? argv[1] : NULL;

    /* Initialize libwww core */
    HTProfile_newPreemptiveClient("TestApp", "1.0");

    /* Turn on TRACE so we can see what is going on */
#if 0
    WWWTRACE = SHOW_CORE_TRACE + SHOW_STREAM_TRACE + SHOW_PROTOCOL_TRACE;
#endif

    /* Set up the request and pass it to the Library */
    HTRequest_setOutputFormat(request, WWW_SOURCE);
    if (url) {
	char * cwd = HTGetCurrentDirectoryURL();
	char * absolute_url = HTParse(url, cwd, PARSE_ALL);
	chunk = HTLoadToChunk(absolute_url, request);
	HT_FREE(absolute_url);
	HT_FREE(cwd);

	/* If chunk != NULL then we have the data */
	if (chunk) {
	    char * string = HTChunk_toCString(chunk);
	    printf("%s", string ? string : "no text");
	    HT_FREE(string);
	}
    } else {
	printf("Type the URL you want to accces on the command line\n");
    }

    /* Clean up the request */
    HTRequest_delete(request);

    /* Terminate the Library */
    HTProfile_delete();
    return 0;
}
