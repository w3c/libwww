#include "WWWLib.h"
#include "CSLUtils.h"
#if 0
void * CSApp_calloc (size_t num, size_t size, const char * file, const char * func)
{
    void * ret;
    if ((ret = HT_CALLOC(num, size)) == NULL)
        HT_OUTOFMEM(func);
    return ret;
}

void HT_FREE (void *memblock)
{
   HT_FREE(memblock);
}
#endif
/* return whether to give up the ghost
 */
#if 0 /* obsolete */
PUBLIC BOOL warn(CSParse_t * pCSParse, Message_t message, ...)
{
    va_list list;
    char space[257];
    char ** messages = CSParse_getMessages();
    HTAlertCallback *cbf = HTAlert_find(HT_A_CONFIRM);

    va_start(list, message);
    if (message >= message_COUNT) {
        HTTrace("Yous got a error\n");
        return YES;
    }
    vsprintf(space, messages[message], list);
    va_end(list);
    HTTrace(space);
    HTTrace("\n");
    if (cbf && (*cbf)(0, HT_A_CONFIRM, HT_MSG_RULES, NULL, NULL, NULL))
        return NO;
    return YES;
}
#endif
