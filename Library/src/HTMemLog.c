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

PRIVATE size_t LogBuffSize = 1024; /* default size is 1k */
PRIVATE int LogFile = 2;
PRIVATE const char * LogFileName = NULL;
PRIVATE char * LogBuff  = NULL;
PRIVATE size_t LogLen = 0;

PUBLIC int HTMemLog_open (const char *ident, const size_t size)
{
#ifdef USE_SYSLOG
    openlog(ident, LOG_NDELAY, LOG_USER);
#else /* USE_SYSLOG */
    LogFileName = ident;
#if 0
    if ((LogFile = open(ident, O_CREAT|O_TRUNC)) == -1)
	return HT_ERROR;
    close(LogFile);
#endif
    LogBuffSize = size;
    if ((LogBuff = (char *) HT_MALLOC(size)) == NULL)
	HT_OUTOFMEM("HTMemLog_open");
    LogLen = 0;
#endif /* !USE_SYSLOG */
    return HT_OK;
}

PRIVATE int HTMemLog_flush(void)
{
    if ((LogFile = open(LogFileName, O_CREAT|O_APPEND)) == -1)
	return HT_ERROR;
    if (write(LogFile, LogBuff, LogLen) == -1) {
	switch (errno) {
	case EBADF: printf("EBADF\n"); break;
	case EINVAL: printf("EINVAL\n"); break;
	case EFAULT: printf("EFAULT\n"); break;
	case EPIPE: printf("EPIPE\n"); break;
	case EAGAIN: printf("EAGAIN\n"); break;
	case EINTR: printf("EINTR\n"); break;
	case ENOSPC: printf("ENOSPC\n"); break;
	default: printf("default\n"); break;
	}
    }
    LogLen = 0;
    close(LogFile);
    return HT_OK;
}

PUBLIC int HTMemLog_add(const char * buf, const size_t len)
{
    /*
    **	Dump everything that won't fit in buffer
    */
    while (len + LogLen > LogBuffSize) {
	size_t toWrite = LogBuffSize-LogLen;
	memcpy(LogBuff+LogLen, buf, toWrite);
	HTMemLog_flush();
	buf += toWrite;
	len -= toWrite;
    }
    memcpy(LogBuff+LogLen, buf, len);
    LogLen += len;
    return HT_OK;
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
    struct timeval tp;
    struct timezone tz = {300, DST_USA};

    gettimeofday(&tp, &tz);
    tp.tv_sec = HTMemLog_adjustGMT(tp.tv_sec)%(24*60*60);
    len = sprintf(buff, "%02d:%02d:%02d.%d ", tp.tv_sec/3600, (tp.tv_sec%3600)/60, tp.tv_sec%60, tp.tv_usec);
    HTMemLog_add(buff, len);
    return tp.tv_sec;
}

PUBLIC void HTMemLog_close (void)
{
#ifdef USE_SYSLOG
    closelog();
#else /* USE_SYSLOG */
    if (LogLen)
	HTMemLog_flush();
    if (LogFile > 2)
	close(LogFile);
    if (LogBuff != NULL)
	HT_FREE(LogBuff);
#endif /* !USE_SYSLOG */
}

PUBLIC int HTMemLog_callback (const char * data, const size_t len, const char * fmt, va_list pArgs)
{
    char buff[8200];
    int ret;
    ret = vsprintf(buff, fmt, pArgs);
#ifdef USE_SYSLOG
    syslog(LOG_DEBUG, "%s\n", buff);
    if (len > 8192)
	len = 8192;
    strncpy(buff, data, len);
    buff[len] = 0;
    syslog(LOG_DEBUG, "%s\n", buff);
#else /* USE_SYSLOG */
    /*
    time(&now);
    tptr = ctime(&now);
    HTMemLog_add(tptr, strlen(tptr));
    */
    HTMemLog_addTime();
    HTMemLog_add(buff, ret);
    HTMemLog_add("\n", 1);
    HTMemLog_add(data, len);
#endif /* !USE_SYSLOG */
    return ret;
}

