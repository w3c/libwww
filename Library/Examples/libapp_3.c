#include "WWWLib.h"
#include "WWWHTTP.h"
#include "WWWMIME.h"
#include "WWWApp.h"

int main (int argc, char ** argv)
{
    HTList * converters = HTList_new();
    HTRequest * request = HTRequest_new();	  /* Create a request object */
    HTLibInit("TestApp", "1.0");
    HTProtocol_add("http", YES, HTLoadHTTP, NULL);

    HTConversion_add(converters, "text/x-http","*/*", HTTPStatus_new,
		     1.0, 0.0, 0.0);
    HTConversion_add(converters, "message/rfc822", "*/*", HTMIMEConvert,
		     1.0, 0.0, 0.0);
    HTFormat_setConversion(converters);
    HTAlert_add(HTPrompt, HT_A_PROMPT);
    if (argc == 2) {
	HTLoadAbsolute(argv[1], request);
    } else
	printf("Type the URL to fetch\n");
    HTRequest_delete(request);			/* Delete the request object */
    HTConversion_deleteAll(converters);
    HTLibTerminate();
    return 0;
}
