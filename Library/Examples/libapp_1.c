#include "WWWLib.h"

#ifdef LIBWWW_SHARED
#include "HTextImp.h"
#endif

int main()
{
    HTLibInit("TestApp", "1.0");
    HTLibTerminate();
    return 0;
}
