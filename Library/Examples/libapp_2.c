#include "WWWLib.h"
#include "WWWHTTP.h"

int main()
{
    HTList *converters = HTList_new();		     /* Create a list object */

    /* Initialize the Library */
    HTLibInit("TestApp", "1.0");

    /* Register the HTTP Module */
    HTProtocol_add("http", YES, HTLoadHTTP, NULL);

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
