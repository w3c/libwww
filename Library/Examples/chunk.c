#include "WWWLib.h"
#include "WWWHTTP.h"
#include "WWWInit.h"

int main (int argc, char ** argv)
{
    HTRequest * request = HTRequest_new();
    HTList * encodings = HTList_new();		/* List of encoders */
    HTChunk * chunk = NULL;
    char * url = argc==2 ? argv[1] : NULL;

    /* Initialize libwww core */
    HTLibInit("TestApp", "1.0");

    /* Turn on TRACE so we can see what is going on */
#if 0
    WWWTRACE = SHOW_CORE_TRACE + SHOW_STREAM_TRACE + SHOW_PROTOCOL_TRACE;
#endif

    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of protocol modules */
    HTProtocolInit();

    /* Register the default set of BEFORE and AFTER callback functions */
    HTNetInit();

    /* Register the default set of transfer encoders and decoders */
    HTEncoderInit(encodings);
    HTFormat_setTransferCoding(encodings);

    /* Register the default set of MIME header parsers */
    HTMIMEInit();

    /* Set up the request and pass it to the Library */
    HTRequest_setOutputFormat(request, WWW_SOURCE);
    HTRequest_setPreemptive(request, YES);
    if (url) {
	char * cwd = HTGetCurrentDirectoryURL();
	char * absolute_url = HTParse(url, cwd, PARSE_ALL);
	HTAnchor * anchor = HTAnchor_findAddress(absolute_url);
	chunk = HTLoadAnchorToChunk(anchor, request);
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
    HTFormat_deleteAll();

    /* Terminate the Library */
    HTLibTerminate();
    return 0;
}
