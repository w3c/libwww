#include "WWWLib.h"
#include "HTBrowse.h"

PRIVATE int WeHaveAView = 0;

struct _HTView {
    FILE * outFile;
};

PRIVATE HTView TheOnlyView = {stdout};

PUBLIC HTView * HTView_create(char* name, int rows, int cols, LineMode * pLm)
{
    if (WeHaveAView) return 0;
    WeHaveAView = 1;
    return &TheOnlyView;
}

PUBLIC BOOL HTView_destroy(HTView * pView)
{
    if (!WeHaveAView || pView != &TheOnlyView)
	return NO;
    WeHaveAView = 0;
    return YES;
}

PUBLIC int OutputData(HTView * pView, const char  * fmt, ...)
{
    int ret;
    va_list pArgs;
    va_start(pArgs, fmt);
    ret = vfprintf(pView->outFile, fmt, pArgs);
    va_end(pArgs);
    return ret;
}

