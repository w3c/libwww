#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWHTTP.h"
#include "WWWTrans.h"

int main()
{
    HTList *converters = HTList_new();		     /* Create a list object */

    /* Initialize the Library */
    HTLibInit("TestApp", "1.0");

    /* Register TCP as the transport */
    HTTransport_add("buffered_tcp", HT_TP_SINGLE, HTReader_new, HTBufferWriter_new);

    /* Register the HTTP Module */
    HTProtocol_add("http", "buffered_tcp", HTTP_PORT, YES, HTLoadHTTP, NULL);

    /* Add a conversion to our empty list */
    HTConversion_add(converters, "*/*", "www/present", HTSaveLocally, 1.0, 0.0, 0.0);

    /* Register our list with one conversion */
    HTFormat_setConversion(converters);

    /* Delete the list with one conversion */
    HTConversion_deleteAll(converters);

    /* Terminate the Library */
    HTLibTerminate();
    return 0;
}
