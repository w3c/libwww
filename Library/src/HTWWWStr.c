/*								     HTWWWStr.c
**	WWW RELATED STRING UTILITIES
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**      Now 13 95	Spwaned from HTString.c
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTTCP.h"
#include "HTParse.h"
#include "HTWWWStr.h"					 /* Implemented here */

PRIVATE long HTTimeZone = 0L;		       /* Offset from GMT in seconds */
PRIVATE char * months[12] = {
    "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

#ifdef NO_STRFTIME
PRIVATE char * wkdays[7] = {
    "Mon","Tue","Wed","Thu","Fri","Sat","Sun"
};
#endif

/* ------------------------------------------------------------------------- */

/*	Find next Field
**	---------------
**	Finds the next RFC822 token in a string
**	On entry,
**	*pstr	points to a string containing a word separated
**		by white white space "," ";" or "=". The word
**		can optionally be quoted using <"> or "<" ">"
**		Comments surrrounded by '(' ')' are filtered out
**
** 	On exit,
**	*pstr	has been moved to the first delimiter past the
**		field
**		THE STRING HAS BEEN MUTILATED by a 0 terminator
**
**	Returns	a pointer to the first word or NULL on error
*/
PUBLIC char * HTNextField (char ** pstr)
{
    char * p = *pstr;
    char * start = NULL;
    if (!*pstr) return NULL;
    while (1) {
	/* Strip white space and other delimiters */
	while (*p && (WHITE(*p) || *p==',' || *p==';' || *p=='=')) p++;
	if (!*p) {
	    *pstr = p;
	    return NULL;				   	 /* No field */
	}

	if (*p == '"') {				     /* quoted field */
	    start = ++p;
	    for(;*p && *p!='"'; p++)
		if (*p == '\\' && *(p+1)) p++;	       /* Skip escaped chars */
	    break;			    /* kr95-10-9: needs to stop here */
	} else if (*p == '<') {				     /* quoted field */
	    start = ++p;
	    for(;*p && *p!='>'; p++)
		if (*p == '\\' && *(p+1)) p++;	       /* Skip escaped chars */
	    break;			    /* kr95-10-9: needs to stop here */
	} else if (*p == '(') {					  /* Comment */
	    for(;*p && *p!=')'; p++)
		if (*p == '\\' && *(p+1)) p++;	       /* Skip escaped chars */
	    p++;
	} else {					      /* Spool field */
	    start = p;
	    while(*p && !WHITE(*p) && *p!=',' && *p!=';' && *p!='=')
		p++;
	    break;						   /* Got it */
	}
    }
    if (*p) *p++ = '\0';
    *pstr = p;
    return start;
}

/*
**	Returns a Message-ID string including the open '<' and the closing '>'.
**	The format of the string is:
**
**		"<" time-in-sec "Z" process-id "@" FQDN ">"
**
**	or on systems that doesn't have process-id:
**
**		"<" time-in-sec "Z" user "@" FQDN ">"
**
**	Returns a pointer to the MessageID
*/
PUBLIC CONST char *HTMessageIdStr (void)
{
    static char buf[80];
    time_t sectime = time(NULL);
#ifndef NO_GETPID
    CONST char *address = HTGetDomainName();
#else
    CONST char *address = HTGetMailAddress();
#endif /* NO_GETPID */
    if (!address) address = tmpnam(NULL);
    if ((!address || !*address) && sectime < 0) {
	if (WWWTRACE)
	    TTYPrint(TDEST, "MessageID...  Can't make a unique MessageID\n");
	return "";
    }
#ifndef NO_GETPID
    sprintf(buf, "<%ldZ%d@%s>", sectime, getpid(), address ? address : "@@@");
#else
    sprintf(buf, "<%ldZ%s>", sectime, address ? address : "@@@");
#endif /* NO_GETPID */

    *(buf+79) = '\0';
    return buf;
}

/*	Date and Time Parser
**	--------------------
**	These functions are taken from the server written by Ari Luotonen
*/

PRIVATE int make_num (CONST char *  s)
{
    if (*s >= '0' && *s <= '9')
	return 10 * (*s - '0') + *(s+1) - '0';
    else
	return *(s+1) - '0';
}

PRIVATE int make_month (CONST char *  s)
{
    int i;
    for (i=0; i<12; i++)
	if (!strncasecomp(months[i], s, 3))
	    return i;
    return 0;
}

/*	Timezone Offset
**	---------------
**	Calculates the offset from GMT in seconds
*/
PUBLIC long HTGetTimeZoneOffset (void)
{
#ifndef NO_TIMEZONE
    {
	time_t cur_t = time(NULL);
#ifdef HT_REENTRANT
	struct tm loctime;
	struct tm *local = (struct tm *) localtime_r(&cur_t, &loctime);
#else
	struct tm *local = localtime(&cur_t);
#endif /* HT_REENTRANT */
	if (daylight && local->tm_isdst>0) {		   /* daylight time? */
#ifndef NO_ALTZONE
	    HTTimeZone = altzone;
#else
 	    /* Assumes a fixed DST offset of 1 hour, which is probably wrong */
 	    HTTimeZone = timezone - 3600;
#endif
	} else {						       /* no */
	    HTTimeZone = timezone;
	}
	HTTimeZone = -HTTimeZone;
	if (WWWTRACE)
	    TTYPrint(TDEST,"TimeZone.... GMT + (%02d) hours (including DST)\n",
		    (int) HTTimeZone/3600);
    }
#else
#ifndef NO_GMTOFF
    {
	time_t cur_t = time(NULL);
#ifdef HT_REENTRANT
	struct tm loctime;
	localtime_r(&cur_t, &loctime);
#else
	struct tm * local = localtime(&cur_t);
#endif /* HT_REENTRANT */
	HTTimeZone = local->tm_gmtoff;
	if (WWWTRACE)
	    TTYPrint(TDEST,"TimeZone.... GMT + (%02d) hours (including DST)\n",
		    (int)local->tm_gmtoff / 3600);
    }
#else
    if (WWWTRACE) TTYPrint(TDEST,"TimeZone.... Not defined\n");
#endif /* !NO_TIMEZONE */
#endif /* !NO_GMTOFF */
    return HTTimeZone;
}

/*
**	Parse a str in GMT format to a local time time_t representation
**	Four formats are accepted:
**
**		Wkd, 00 Mon 0000 00:00:00 GMT		(rfc1123)
**		Weekday, 00-Mon-00 00:00:00 GMT		(rfc850)
**		Wkd Mon 00 00:00:00 0000 GMT		(ctime)
**		1*DIGIT					(delta-seconds)
*/
PUBLIC time_t HTParseTime (CONST char *  str)
{
    CONST char * s;
    struct tm tm;
    time_t t;

    if (!str) return 0;

    if ((s = strchr(str, ','))) {	 /* Thursday, 10-Jun-93 01:29:59 GMT */
	s++;				/* or: Thu, 10 Jan 1993 01:29:59 GMT */
	while (*s && *s==' ') s++;
	if (strchr(s,'-')) {				     /* First format */
	    if (WWWTRACE)
		TTYPrint(TDEST, "Format...... Weekday, 00-Mon-00 00:00:00 GMT\n");
	    if ((int)strlen(s) < 18) {
		if (WWWTRACE)
		    TTYPrint(TDEST,
			    "ERROR....... Not a valid time format \"%s\"\n",s);
		return 0;
	    }
	    tm.tm_mday = make_num(s);
	    tm.tm_mon = make_month(s+3);
	    tm.tm_year = make_num(s+7);
	    tm.tm_hour = make_num(s+10);
	    tm.tm_min = make_num(s+13);
	    tm.tm_sec = make_num(s+16);
	} else {					    /* Second format */
	    if (WWWTRACE)
		TTYPrint(TDEST, "Format...... Wkd, 00 Mon 0000 00:00:00 GMT\n");
	    if ((int)strlen(s) < 20) {
		if (WWWTRACE)
		    TTYPrint(TDEST,
			    "ERROR....... Not a valid time format \"%s\"\n",s);
		return 0;
	    }
	    tm.tm_mday = make_num(s);
	    tm.tm_mon = make_month(s+3);
	    tm.tm_year = (100*make_num(s+7) - 1900) + make_num(s+9);
	    tm.tm_hour = make_num(s+12);
	    tm.tm_min = make_num(s+15);
	    tm.tm_sec = make_num(s+18);

	}
    } else if (isdigit(*str)) {				    /* delta seconds */
	t = time(NULL) + atol(str);	      /* Current local calendar time */
	if (WWWTRACE) {
#ifdef HT_REENTRANT
	    char buffer[CTIME_MAX];
	    TTYPrint(TDEST, "Time string. Delta-time %s parsed to %ld seconds, or in local time: %s", str, (long) t, (char *) ctime_r(&t, buffer, CTIME_MAX));
#else
	    TTYPrint(TDEST, "Time string. Delta-time %s parsed to %ld seconds, or in local time: %s", str, (long) t, ctime(&t));
#endif
	}
	return t;

    } else {	      /* Try the other format:  Wed Jun  9 01:29:59 1993 GMT */
	if (WWWTRACE)
	    TTYPrint(TDEST, "Format...... Wkd Mon 00 00:00:00 0000 GMT\n");
	s = str;
	while (*s && *s==' ') s++;
	if (WWWTRACE)
	    TTYPrint(TDEST, "Trying...... The Wrong time format: %s\n", s);
	if ((int)strlen(s) < 24) {
	    if (WWWTRACE)
		TTYPrint(TDEST, "ERROR....... Not a valid time format \"%s\"\n",s);
	    return 0;
	}
	tm.tm_mday = make_num(s+8);
	tm.tm_mon = make_month(s+4);
	tm.tm_year = make_num(s+22);
	tm.tm_hour = make_num(s+11);
	tm.tm_min = make_num(s+14);
	tm.tm_sec = make_num(s+17);
    }
    if (tm.tm_sec  < 0  ||  tm.tm_sec  > 59  ||
	tm.tm_min  < 0  ||  tm.tm_min  > 59  ||
	tm.tm_hour < 0  ||  tm.tm_hour > 23  ||
	tm.tm_mday < 1  ||  tm.tm_mday > 31  ||
	tm.tm_mon  < 0  ||  tm.tm_mon  > 11  ||
	tm.tm_year <70  ||  tm.tm_year >120) {
	if (WWWTRACE) TTYPrint(TDEST,
	"ERROR....... Parsed illegal time: %02d.%02d.%02d %02d:%02d:%02d\n",
	       tm.tm_mday, tm.tm_mon+1, tm.tm_year,
	       tm.tm_hour, tm.tm_min, tm.tm_sec);
	return 0;
    }

#if !defined(NO_TIMEZONE) && !defined(NO_ALTZONE)
    tm.tm_isdst = daylight;		       /* Already taken into account */
#else
    tm.tm_isdst = -1;
#endif

#ifndef NO_MKTIME
    t = mktime(&tm);
    t += (HTTimeZone);
#else
#ifndef NO_TIMEGM
    t = timegm(&tm);
#else
    if (WWWTRACE) TTYPrint(TDEST,"Time String. Can not be parsed\n");
#endif /* !NO_TIMEGM */
#endif /* !NO_MKTIME */

    if (WWWTRACE)
	TTYPrint(TDEST,
		"Time string. %s parsed to %ld seconds, or in local time: %s",
		str, (long) t, ctime(&t));
    return t;
}

/*
**	Returns a string pointer to a static area of the current calendar
**	time in RFC 1123 format, for example
**
**		Sun, 06 Nov 1994 08:49:37 GMT
**
**	The result can be given in both local and GMT dependent on the flag
*/
PUBLIC CONST char *HTDateTimeStr (time_t * calendar, BOOL local)
{
    static char buf[40];

#ifndef NO_STRFTIME
    if (local) {
	/*
	** Solaris 2.3 has a bug so we _must_ use reentrant version
	** Thomas Maslen <tmaslen@verity.com>
	*/
#if defined(HT_REENTRANT) || defined(SOLARIS)
	struct tm loctime;
	localtime_r(calendar, &loctime);
	strftime(buf, 40, "%a, %d %b %Y %H:%M:%S", &loctime);
#else
	struct tm *loctime = localtime(calendar);
	strftime(buf, 40, "%a, %d %b %Y %H:%M:%S", loctime);
#endif /* SOLARIS || HT_REENTRANT */
    } else {
#if defined(HT_REENTRANT) || defined(SOLARIS)
	struct tm gmt;
	gmtime_r(calendar, &gmt);
    	strftime(buf, 40, "%a, %d %b %Y %H:%M:%S GMT", &gmt);
#else
	struct tm *gmt = gmtime(calendar);
    	strftime(buf, 40, "%a, %d %b %Y %H:%M:%S GMT", gmt);
#endif /* SOLARIS || HT_REENTRANT */
    }
#else
    if (local) {
#if defined(HT_REENTRANT)
	struct tm loctime;
	localtime_r(calendar, &loctime);
#else
	struct tm *loctime = localtime(calendar);
#endif /* HT_REENTRANT */
	sprintf(buf,"%s, %02d %s 19%02d %02d:%02d:%02d",
		wkdays[gmt->tm_wday],
		gmt->tm_mday,
		months[gmt->tm_mon],
		gmt->tm_year % 100,
		gmt->tm_hour,
		gmt->tm_min,
		gmt->tm_sec);
    } else {
#if defined(HT_REENTRANT) || defined(SOLARIS)
	struct tm gmt;
	gmtime_r(calendar, &gmt);
#else
	struct tm *gmt = gmtime(calendar);
#endif
	sprintf(buf,"%s, %02d %s 19%02d %02d:%02d:%02d GMT",
		wkdays[gmt->tm_wday],
		gmt->tm_mday,
		months[gmt->tm_mon],
		gmt->tm_year % 100,
		gmt->tm_hour,
		gmt->tm_min,
		gmt->tm_sec);
    }
#endif
    return buf;
}

/*	HTDateDirStr
**	------------
**	Generates a date string used in directory listings
*/
PUBLIC BOOL HTDateDirStr (time_t * time, char * str, int len)
{
#ifndef NO_STRFTIME
#if defined(HT_REENTRANT) || defined(SOLARIS)
    struct tm loctime;
    localtime_r(time, &loctime);
    strftime(str, len, "%d-%b-%y %H:%M", &loctime);
    return YES;
#else
    strftime(str, len, "%d-%b-%y %H:%M", localtime(time));
    return YES;
#endif /* HT_REENTRANT || SOLARIS */
#else
    if (len >= 16) {
	struct tm *loctime = localtime(time);
	sprintf(bodyptr,"%02d-%s-%02d %02d:%02d",
		loctime->tm_mday,
		months[loctime->tm_mon],
		loctime->tm_year % 100,
		loctime->tm_hour,
		loctime->tm_min);
	return YES;
    }
    return NO;
#endif /* NO_STRFTIME */		
}

/* 							     	HTNumToStr
**	Converts a long (byte count) to a string
**	----------------------------------------
**	This function was a PAIN!  In computer-world 1K is 1024 bytes
**	and 1M is 1024K -- however, sprintf() still formats in base-10.
**	Therefore I output only until 999, and then start using the
**	next unit.  This doesn't work wrong, it's just a feature.
**	The "str" must be large enough to contain the result.
*/
PUBLIC void HTNumToStr (unsigned long n, char * str, int len)
{
    double size = n/1024.0;
    if (len < 6) {
	*str = '\0';
	return;
    }
    if (n < 1000)
	sprintf(str, "%dK", n>0 ? 1 : 0);
    else if (size + 0.999 < 1000)
	sprintf(str, "%dK", (int)(size + 0.5));
    else if ((size /= 1024) < 9.9)
	sprintf(str, "%.1fM", (size + 0.05));
    else if (size < 1000)
	sprintf(str, "%dM", (int)(size + 0.5));
    else if ((size /= 1024) < 9.9)
	sprintf(str, "%.1fG", (size + 0.05));
    else
	sprintf(str, "%dG", (int)(size + 0.5));
}

/*	Convert file URLs into a local representation
**	---------------------------------------------
**	The URL has already been translated through the rules in get_physical
**	in HTAccess.c and all we need to do now is to map the path to a local
**	representation, for example if must translate '/' to the ones that
**	turn the wrong way ;-)
**	Returns:
**		OK:	local file (that must be freed by caller)
**		Error:	NULL
*/
PUBLIC char * HTWWWToLocal (CONST char * url, CONST char * base)
{
    if (url) {
	char * access = HTParse(url, base, PARSE_ACCESS);
	char * host = HTParse(url, base, PARSE_HOST);
	char * path = HTParse(url, base, PARSE_PATH+PARSE_PUNCTUATION);
	CONST char *myhost = HTGetHostName();

	/* Find out if this is a reference to the local file system */
	if ((*access && strcmp(access, "file")) ||
	    (*host && strcasecomp(host, "localhost") &&
	     myhost && strcmp(host, myhost))) {
	    if (PROT_TRACE)
		TTYPrint(TDEST, "LocalName... Not on local file system\n");
	    HT_FREE(access);
	    HT_FREE(host);
	    HT_FREE(path);
	    return NULL;
	} else {
	    char *ptr;
	    if ((ptr = strchr(path, ';')) || (ptr = strchr(path, '?')))
		*ptr = '\0';
	
	    /*
	    ** Do whatever translation is required here in order to fit your
	    ** platform _before_ the path is unescaped.
	    */
#ifdef VMS
	    HTVMS_checkDecnet(path);
#endif
#ifdef WWW_MSWINDOWS
	    /* an absolute pathname with logical drive */
	    if (*path == '/' && path[2] == ':')    
		/* NB. need memmove because overlaps */
		memmove( path, path+1, strlen(path) + 1);
#endif
	    
	    HTUnEscape(path);		  /* Take out the escaped characters */
	    if (PROT_TRACE)
		TTYPrint(TDEST, "Node........ `%s' means path `%s'\n",url,path);
	    HT_FREE(access);
	    HT_FREE(host);
	    return path;
	}
    }
    return NULL;
}

/*	Convert a local file name into a URL
**	------------------------------------
**	Generates a WWW URL name from a local file name or NULL if error.
**	Returns:
**		OK:	local file (that must be freed by caller)
**		Error:	NULL
*/
PUBLIC char * HTLocalToWWW (CONST char * local)
{
    char * result = NULL;
    if (local && *local) {
	StrAllocCopy(result, "file:");	     /* We get an absolute file name */
#ifdef VMS 
	/* convert directory name to Unix-style syntax */
	{
	    char * disk = strchr (local, ':');
	    char * dir = strchr (local, '[');
	    if (disk) {
		*disk = '\0';
		StrAllocCat(result, "/"); /* needs delimiter */
		StrAllocCat(result, local);
	    }
	    if (dir) {
		char *p;
		*dir = '/';	/* Convert leading '[' */
		for (p = dir ; *p != ']'; ++p)
		    if (*p == '.') *p = '/';
		*p = '\0';	/* Cut on final ']' */
		StrAllocCat(result, dir);
	    }
	}
#else  /* not VMS */
#ifdef WIN32
	{
	    char * p;
	    StrAllocCat(result, "/");
	    StrAllocCat(result, local);	    
	    p = result;
	    while (*p) { 
		if (*p == '\\')		         /* change to one true slash */
		    *p = '/';
		p++;
	    }
	}
#else /* not WIN32 */
	StrAllocCat (result, local);
#endif /* not WIN32 */
#endif /* not VMS */
    }
    return result;
}
