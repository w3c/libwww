#include "WWWLib.h"
#include "WWWHTTP.h"
#include "WWWInit.h"


int main()
{
    HTList * converters = HTList_new();		/* List of converters */
    HTList * encodings = HTList_new();		/* List of encoders */

    /* Set up the application's event loop. We use the 
       example event loop that comes with the Library.  */
    HTEventInit();

    /* Initialize libwww core */
    HTLibInit("TestApp", "1.0");

    /* Turn on TRACE so we can see what is going on */
    WWWTRACE = SHOW_CORE_TRACE;

    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of protocol modules */
    HTProtocolInit();

    /* Register the default set of BEFORE and AFTER filters */
    HTNetInit();

    /* Register the default set of converters */
    HTConverterInit(converters);
    HTFormat_setConversion(converters);

    /* Register the default HTML Parser */
    /* We don't do this here as we then must provide the HTExt interface as
       described in the libwww Users's Guide */
    /* HTMLInit(); */

    /* Register the default set of transfer encoders and decoders */
    HTEncoderInit(encodings);
    HTFormat_setTransferCoding(encodings);

    /* Register the default set of file suffix bindings */
    HTFileInit();

    /* Register the default set of MIME header parsers */
    HTMIMEInit();

    /* Register the default set of Icons for directory listings */
    HTIconInit(NULL);

    /* Register the default set of messages and dialog functions */
    HTAlertInit();

    /* Terminate the Library */
    HTLibTerminate();
    return 0;
}
