#include "WWWLib.h"
#include "WWWHTTP.h"
#include "WWWMIME.h"

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
    if (argc == 3 && *argv[2]) {
	FILE * fp;
	if ((fp = fopen(argv[2], "wb")) != NULL) {
	    HTRequest_setOutputFormat(request, WWW_SOURCE);
	    HTRequest_setOutputStream(request, HTFWriter_new(request, fp,YES));
	    HTLoadAbsolute(argv[1], request);
	    fclose(fp);
	} else {
	    printf("Can't open file name `%s\'\n", argv[2]);	
	}
    } else {
	printf("Type the URL to fetch and the name of the local file to put it in\n");
	printf("\t%s URL FILENAME\n", argv[0]);
    }
    HTRequest_delete(request);			/* Delete the request object */
    HTConversion_deleteAll(converters);
    HTLibTerminate();
    return 0;
}
