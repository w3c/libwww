/*								     HTTrace.c
**	TRACE AND DATA LOGGING UTILITIES
**
**	(c) COPYRIGHT MIT 1996.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**      26 Nov 96 (EGP)	moved HTTrace stuff here from HTString.c and
**			added HTTrace_data
*/

/* Library include files */
#include "sysdep.h"
/*
**	All the tracing facilities are prototyped in
**	HTUtils.h because they are needed everywhere.
*/
#include "HTUtils.h"

#if WWWTRACE_MODE == WWWTRACE_FILE
PUBLIC FILE *WWWTrace = NULL;
#endif

#ifndef WWW_WIN_DLL
PUBLIC int WWW_TraceFlag = 0;		/* Global trace flag for ALL W3 code */
#endif

PRIVATE HTTraceCallback * PHTTraceCallback = NULL;

PUBLIC void HTTrace_setCallback (HTTraceCallback * pCall)
{
    PHTTraceCallback = pCall;
}

PUBLIC HTTraceCallback * HTTrace_getCallback (void)
{
    return PHTTraceCallback;
}

PUBLIC int HTTrace (const char * fmt, ...)
{
    va_list pArgs;
    va_start(pArgs, fmt);
    if (PHTTraceCallback)
	return (*PHTTraceCallback)(fmt, pArgs);
#ifdef WWW_WIN_WINDOWS
    return (0);
#else
    return (vfprintf(stderr, fmt, pArgs));
#endif
}

PRIVATE HTTraceDataCallback * PHTTraceDataCallback = NULL;

PUBLIC void HTTraceData_setCallback (HTTraceDataCallback * pCall)
{
    PHTTraceDataCallback = pCall;
}

PUBLIC HTTraceDataCallback * HTTraceData_getCallback (void)
{
    return PHTTraceDataCallback;
}

PUBLIC int HTTraceData (const char * data, const size_t len, const char * fmt, ...)
{
    va_list pArgs;
    va_start(pArgs, fmt);
    if (PHTTraceDataCallback)
	return (*PHTTraceDataCallback)(data, len, fmt, pArgs);
    return (0);
}

PUBLIC void HTDebugBreak (void)
{
#ifdef WWW_MSWINDOWS
    DebugBreak();
#else /* WWW_MSWINDOWS */
    int i;
    i = 1/0;
    i = 1/1;
#endif /* !WWW_MSWINDOWS */
    return;
}

