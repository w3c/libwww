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

int main (int argc, char ** argv)
{
    HTRequest * request = HTRequest_new();
    HTList * converters = HTList_new();		/* List of converters */
    HTList * encodings = HTList_new();		/* List of encoders */
    HTChunk * chunk = NULL;
    char * url = argc==2 ? argv[1] : NULL;

    /* Initialize libwww core */
    HTLibInit("TestApp", "1.0");

    /* Gotta set up our own traces */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Turn on TRACE so we can see what is going on */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* On windows we must always set up the eventloop */
#ifdef WWW_WIN_ASYNC
    HTEventInit();
#endif
    
    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of protocol modules */
    HTProtocolInit();

    /* Register the default set of BEFORE and AFTER callback functions */
    HTNetInit();

    /* Register the default set of converters */
    HTConverterInit(converters);
    HTFormat_setConversion(converters);

    /* Register the default set of transfer encoders and decoders */
    HTTransferEncoderInit(encodings);
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
	    HTPrint("%s", string ? string : "no text");
	    HT_FREE(string);
	}
    } else {
	HTPrint("Type the URL you want to accces on the command line\n");
    }

    /* Clean up the request */
    HTRequest_delete(request);
    HTFormat_deleteAll();

    /* On windows, shut down eventloop as well */
#ifdef WWW_WIN_ASYNC
    HTEventTerminate();
#endif
   
    /* Terminate the Library */
    HTLibTerminate();
    return 0;
}
