#include "WWWLib.h"
#include "WWWHTTP.h"
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
    /* Check for status */
    HTPrint("Load resulted in status %d\n", status);
	
	/* we're not handling other requests */
	HTEventList_stopLoop ();
 
	/* stop here */
    return HT_ERROR;
}

int main (int argc, char ** argv)
{
    HTRequest * request = HTRequest_new();
    HTChunk * chunk = NULL;
    char * url = argc==2 ? argv[1] : NULL;

    /* Initialize libwww core */
    HTProfile_newPreemptiveClient("TestApp", "1.0");

    /* Gotta set up our own traces */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Turn on TRACE so we can see what is going on */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* We want raw output including headers */
    HTRequest_setOutputFormat(request, WWW_RAW);

    /* Close connection immediately */
    HTRequest_addConnection(request, "close", "");

    /* Add our own filter to handle termination */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    if (url) {
	char * cwd = HTGetCurrentDirectoryURL();
	char * absolute_url = HTParse(url, cwd, PARSE_ALL);
	chunk = HTLoadToChunk(absolute_url, request);
	HT_FREE(absolute_url);
	HT_FREE(cwd);

	/* If chunk != NULL then we have the data */
	if (chunk) {
	    char * string;
	    /* Go into the event loop... */
	    HTEventList_loop(request);
	    /* print the chunk result */
	    string = HTChunk_toCString(chunk);
	    HTPrint("%s", string ? string : "no text");
	    HT_FREE(string);
	}
    } else {
	HTPrint("Type the URL you want to accces on the command line\n");
    }
	
    /* Clean up the request */
    HTRequest_delete(request);

    /* Terminate the Library */
    HTProfile_delete();
    return 0;
}
