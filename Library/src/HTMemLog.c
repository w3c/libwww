/*								     HTMemLog.c
**	MEMORY BUFFERED DATA LOGGING UTILITIES
**
**	(c) COPYRIGHT MIT 1996.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**      26 Nov 96 (EGP)	started
*/

/* Library include files */
#include "sysdep.h"
#include "HTUtils.h"
#include "HTMemLog.h"
#include "HTTimer.h"

#define OPEN_FLAGS	O_WRONLY|O_CREAT|O_TRUNC|O_SYNC

PRIVATE size_t		LogBuffSize = 1024; /* default size is 1k */
PRIVATE int		LogFd = 2;
PRIVATE const char *	LogName = NULL;
PRIVATE char *		LogBuff  = NULL;
PRIVATE size_t		LogLen = 0;
PRIVATE BOOL		KeepOpen = YES;
PRIVATE HTTimer *	Timer = NULL;

PRIVATE int MemLogTimeout (HTTimer * timer, void * param, HTEventType type)
{
    int ret;
 
    if (WWWTRACE) HTTrace("MemLog...... flushing on timeout\n");
    ret = HTMemLog_flush();

#if 0
    /*	Force flush */
    if (close(LogFd) == -1 || (LogFd = open(LogName, OPEN_FLAGS, 0666)) == -1)
	return HT_ERROR;
#endif

    return ret;
}

PUBLIC int HTMemLog_open (char * logName, size_t size, BOOL keepOpen)
{
#ifdef USE_SYSLOG
    openlog(LogName, LOG_NDELAY, LOG_USER);
#else /* USE_SYSLOG */
    LogName = logName;
    KeepOpen = keepOpen;
    if ((LogFd = open(LogName, OPEN_FLAGS, 0666)) == -1)
	return HT_ERROR;
    if (!KeepOpen)
	close(LogFd);
    LogBuffSize = size;
    if ((LogBuff = (char *) HT_MALLOC(size)) == NULL)
	HT_OUTOFMEM("HTMemLog_open");
    LogLen = 0;
#endif /* !USE_SYSLOG */
    HTTraceData_setCallback(HTMemLog_callback);
    Timer = HTTimer_new(NULL, MemLogTimeout, NULL, 10000, YES);
    return HT_OK;
}

PUBLIC int HTMemLog_flush(void)
{
    if (LogLen) {
	if (!KeepOpen)
	    if ((LogFd = open(LogName, O_WRONLY|O_CREAT|O_APPEND, 0666)) == -1)
		return HT_ERROR;
	write(LogFd, LogBuff, LogLen);
	LogLen = 0;
    }
    if (!KeepOpen)
	close(LogFd);
    return HT_OK;
}

PUBLIC int HTMemLog_add(char * buf, size_t len)
{
    if (LogBuff) {
	/*
	**	Dump everything that won't fit in buffer
	*/
	while (len + LogLen > LogBuffSize) {
	    size_t toWrite = LogBuffSize-LogLen;
	    memcpy(LogBuff+LogLen, buf, toWrite);
	    LogLen = LogBuffSize;	/* same as += toWrite */
	    HTMemLog_flush();
	    buf += toWrite;
	    len -= toWrite;
	}
	memcpy(LogBuff+LogLen, buf, len);
	LogLen += len;
	return HT_OK;
    }
    return HT_ERROR;
}

PRIVATE int HTMemLog_adjustGMT(long theTime)
{
    static long adjustment = -1;
    if (adjustment == -1) {
        tzset();
        adjustment = timezone;
    }
    return theTime-adjustment;
}

PRIVATE int HTMemLog_addTime(void)
{
    char buff[20];
    int len;
    int ret;
#ifdef WWW_MSWINDOWS
    SYSTEMTIME systemTime;

    GetLocalTime(&systemTime);
    ret = systemTime.wSecond;
    len = sprintf(buff, "%02d:%02d:%02d.%d", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
#else /* WWW_MSWINDOWS */
    struct timeval tp;
    struct timezone tz = {300, DST_USA};

    gettimeofday(&tp, &tz);
    tp.tv_sec = HTMemLog_adjustGMT(tp.tv_sec)%(24*60*60);
    ret = tp.tv_sec;
    len = sprintf(buff, "%02d:%02d:%02d.%d", tp.tv_sec/3600, (tp.tv_sec%3600)/60, tp.tv_sec%60, tp.tv_usec);
#endif /* !WWW_MSWINDOWS */
    HTMemLog_add(buff, len);
    return ret;
}

PUBLIC void HTMemLog_close (void)
{
#ifdef USE_SYSLOG
    closelog();
#else /* USE_SYSLOG */
    HTMemLog_flush();
    if (LogFd > 2)
	close(LogFd);
    if (LogBuff != NULL)
	HT_FREE(LogBuff);
#endif /* !USE_SYSLOG */
}

#ifdef USE_SYSLOG
#define PRINT_BUFF_SIZE	8200
#else /* USE_SYSLOG */
#define PRINT_BUFF_SIZE	200
#endif /* !USE_SYSLOG */

#ifdef USE_EXCLUDES
typedef struct {char * str; int len;} StrIndexIndex;

PRIVATE int StrIndex (char * str, StrIndexIndex element[], int elements)
{
    int i;
    for (i = 0; i < elements; i++)
	if (!strncmp(element[i].str, str, element[i].len))
	    return i + 1;
    return 0;
}

PRIVATE StrIndexIndex Excludes[] = {{"HTReader_read", 13}, {"HTWriter_write", 14}, {"HTEventList_loop", 16}};

#endif /* USE_EXCLUDES */

PUBLIC int HTMemLog_callback (char * data, size_t len, char * fmt, va_list pArgs)
{
    char buff[PRINT_BUFF_SIZE];
    int ret;
#ifdef USE_EXCLUDES
    if (StrIndex(fmt, Excludes, sizeof(Excludes)/sizeof(Excludes[0])))
	return 0;
#endif /* USE_EXCLUDES */
#ifdef USE_SYSLOG
    ret = vsprintf(buff, fmt, pArgs);
    syslog(LOG_DEBUG, "%s\n", buff);
    if (len > 8192)
	len = 8192;
    strncpy(buff, data, len);
    buff[len] = 0;
    syslog(LOG_DEBUG, "%s\n", buff);
#else /* USE_SYSLOG */
    HTMemLog_addTime();

    ret = sprintf(buff, " %d ", len);
    HTMemLog_add(buff, ret);

    if (fmt) {
      ret = vsprintf(buff, fmt, pArgs);
      HTMemLog_add(buff, ret);
    }
    HTMemLog_add("\n", 1);

    HTMemLog_add(data, len);
    HTMemLog_add("\n", 1);
#endif /* !USE_SYSLOG */
    return ret;
}

