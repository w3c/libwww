#include "WWWLib.h"
#include "HTTP.h"

int main (int argc, char ** argv)
{
    HTList *converters = HTList_new();
    HTRequest *request = HTRequest_new();	  /* Create a request object */
    HTLibInit("TestApp", "1.0");
    HTProtocol_add("http", YES, HTLoadHTTP);     
    HTConversion_add(converters, "*/*", "www/present", HTSaveLocally, 1.0, 0.0, 0.0);
    HTFormat_setConversion(converters);
    if (argc == 2) {
	HTLoadAbsolute(argv[1], request);
    } else
	printf("Type the URL to fetch\n");
    HTRequest_delete(request);			/* Delete the request object */
    HTConversion_deleteAll(converters);
    HTLibTerminate();
    return 0;
}
