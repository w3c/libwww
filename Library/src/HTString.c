/*								     HTString.c
**	DYNAMIC STRING UTILITIES
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	Original version came with listserv implementation.
**	Version TBL Oct 91 replaces one which modified the strings.
**	02-Dec-91 (JFG) Added stralloccopy and stralloccat
**	23 Jan 92 (TBL) Changed strallocc* to 8 char HTSAC* for VM and suchlike
**	 6 Oct 92 (TBL) Moved WWW_TraceFlag in here to be in library
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTTCP.h"
#include "HTString.h"					 /* Implemented here */

#ifdef NO_STDIO
PUBLIC FILE *WWWTrace = NULL;
#endif

PUBLIC int WWW_TraceFlag = 0;	/* Global trace flag for ALL W3 code */

#ifndef VC
#define VC "unknown"
#endif

PUBLIC CONST char * HTLibraryVersion = VC; /* String for help screen etc */

PRIVATE long HTTimeZone = 0L;		       /* Offset from GMT in seconds */

/* ------------------------------------------------------------------------- */

#ifndef VM		/* VM has these already it seems */
	
/*	Strings of any length
**	---------------------
*/
PUBLIC int strcasecomp ARGS2 (CONST char*,a, CONST char *,b)
{
    int diff;
    for( ; *a && *b; a++, b++) {
	if ((diff = TOLOWER(*a) - TOLOWER(*b)))
	    return diff;
    }
    if (*a) return 1;			/* a was longer than b */
    if (*b) return -1;			/* a was shorter than b */
    return 0;				/* Exact match */
}


/*	With count limit
**	----------------
*/
PUBLIC int strncasecomp ARGS3(CONST char*,a, CONST char *,b, int,n)
{
	CONST char *p =a;
	CONST char *q =b;
	
	for(p=a, q=b;; p++, q++) {
	    int diff;
	    if (p == a+n) return 0;	/*   Match up to n characters */
	    if (!(*p && *q)) return *p - *q;
	    diff = TOLOWER(*p) - TOLOWER(*q);
	    if (diff) return diff;
	}
	/*NOTREACHED*/
}
#endif


/*
 * strcasestr(s1,s2) -- like strstr(s1,s2) but case-insensitive.
 */
PUBLIC char * strcasestr ARGS2(char *,	s1,
			       char *,	s2)
{
    char * ptr = s1;

    if (!s1 || !s2 || !*s2) return s1;

    while (*ptr) {
	if (TOUPPER(*ptr) == TOUPPER(*s2)) {
	    char * cur1 = ptr + 1;
	    char * cur2 = s2 + 1;
	    while (*cur1 && *cur2 && TOUPPER(*cur1) == TOUPPER(*cur2)) {
		cur1++;
		cur2++;
	    }
	    if (!*cur2) {
		if (TRACE)
		    fprintf(TDEST, "Debug....... strcasestr(s1 = \"%s\", s2 = \"%s\") => \"%s\"\n",
			    s1,s2,ptr);
		return ptr;
	    }
	}
	ptr++;
    }
    if (TRACE)
	fprintf(TDEST,
		"Debug....... strcasestr(s1=\"%s\", s2=\"%s\") => No match\n",
		s1,s2);
    return NULL;
}



/*	Allocate a new copy of a string, and returns it
*/
PUBLIC char * HTSACopy
  ARGS2 (char **,dest, CONST char *,src)
{
  if (*dest) free(*dest);
  if (! src)
    *dest = NULL;
  else {
    *dest = (char *) malloc (strlen(src) + 1);
    if (*dest == NULL) outofmem(__FILE__, "HTSACopy");
    strcpy (*dest, src);
  }
  return *dest;
}

/*	String Allocate and Concatenate
*/
PUBLIC char * HTSACat
  ARGS2 (char **,dest, CONST char *,src)
{
  if (src && *src) {
    if (*dest) {
      int length = strlen (*dest);
      *dest = (char *) realloc (*dest, length + strlen(src) + 1);
      if (*dest == NULL) outofmem(__FILE__, "HTSACat");
      strcpy (*dest + length, src);
    } else {
      *dest = (char *) malloc (strlen(src) + 1);
      if (*dest == NULL) outofmem(__FILE__, "HTSACat");
      strcpy (*dest, src);
    }
  }
  return *dest;
}


/*	Find next Field
**	---------------
**
** On entry,
**	*pstr	points to a string containing a word separated
**		by white white space "," ";" or "=". The word
**		can optionally be quoted using <"> or "<" ">"
**		Comments surrrounded by '(' ')' are filtered out
**
** On exit,
**	*pstr	has been moved to the first delimiter past the
**		field
**		THE STRING HAS BEEN MUTILATED by a 0 terminator
**
**	Returns	a pointer to the first word or NULL on error
*/
PUBLIC char * HTNextField ARGS1(char **, pstr)
{
    char * p = *pstr;
    char * start;

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
	} else if (*p == '<') {				     /* quoted field */
	    for(;*p && *p!='>'; p++)
		if (*p == '\\' && *(p+1)) p++;	       /* Skip escaped chars */
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
**	Returns a string pointer to a static area of the current calendar
**	time in RFC 1123 format, for example
**
**		Sun, 06 Nov 1994 08:49:37 GMT
**
**	The result can be given in both local and GMT dependent on the flag
*/
PUBLIC CONST char *HTDateTimeStr ARGS2(time_t *, calendar, BOOL, local)
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
		wkday[gmt->tm_wday],
		gmt->tm_mday,
		month[gmt->tm_mon],
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
		wkday[gmt->tm_wday],
		gmt->tm_mday,
		month[gmt->tm_mon],
		gmt->tm_year % 100,
		gmt->tm_hour,
		gmt->tm_min,
		gmt->tm_sec);
    }
#endif
    return buf;
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
PUBLIC CONST char *HTMessageIdStr NOARGS
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
	if (TRACE)
	    fprintf(TDEST, "MessageID...  Can't make a unique MessageID\n");
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

PRIVATE char * month_names[12] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

PRIVATE int make_num ARGS1(CONST char *, s)
{
    if (*s >= '0' && *s <= '9')
	return 10 * (*s - '0') + *(s+1) - '0';
    else
	return *(s+1) - '0';
}

PRIVATE int make_month ARGS1(CONST char *, s)
{
    int i;
    for (i=0; i<12; i++)
	if (!strncasecomp(month_names[i], s, 3))
	    return i;
    return 0;
}

/*	Timezone Offset
**	---------------
**	Calculates the offset from GMT in seconds
*/
PUBLIC long HTGetTimeZoneOffset NOARGS
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
	if (TRACE)
	    fprintf(TDEST,"TimeZone.... GMT + (%02d) hours (including DST)\n",
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
	if (TRACE)
	    fprintf(TDEST,"TimeZone.... GMT + (%02d) hours (including DST)\n",
		    (int)local->tm_gmtoff / 3600);
    }
#else
    if (TRACE) fprintf(TDEST,"TimeZone.... Not defined\n");
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
PUBLIC time_t HTParseTime ARGS1(CONST char *, str)
{
    CONST char * s;
    struct tm tm;
    time_t t;

    if (!str) return 0;

    if ((s = strchr(str, ','))) {	 /* Thursday, 10-Jun-93 01:29:59 GMT */
	s++;				/* or: Thu, 10 Jan 1993 01:29:59 GMT */
	while (*s && *s==' ') s++;
	if (strchr(s,'-')) {				     /* First format */
	    if (TRACE)
		fprintf(TDEST, "Format...... Weekday, 00-Mon-00 00:00:00 GMT\n");
	    if ((int)strlen(s) < 18) {
		if (TRACE)
		    fprintf(TDEST,
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
	    if (TRACE)
		fprintf(TDEST, "Format...... Wkd, 00 Mon 0000 00:00:00 GMT\n");
	    if ((int)strlen(s) < 20) {
		if (TRACE)
		    fprintf(TDEST,
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
	if (TRACE) {
#ifdef HT_REENTRANT
	    char buffer[CTIME_MAX];
	    fprintf(TDEST, "Time string. Delta-time %s parsed to %ld seconds, or in local time: %s", str, (long) t, (char *) ctime_r(&t, buffer, CTIME_MAX));
#else
	    fprintf(TDEST, "Time string. Delta-time %s parsed to %ld seconds, or in local time: %s", str, (long) t, ctime(&t));
#endif
	}
	return t;

    } else {	      /* Try the other format:  Wed Jun  9 01:29:59 1993 GMT */
	if (TRACE)
	    fprintf(TDEST, "Format...... Wkd Mon 00 00:00:00 0000 GMT\n");
	s = str;
	while (*s && *s==' ') s++;
	if (TRACE)
	    fprintf(TDEST, "Trying...... The Wrong time format: %s\n", s);
	if ((int)strlen(s) < 24) {
	    if (TRACE)
		fprintf(TDEST, "ERROR....... Not a valid time format \"%s\"\n",s);
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
	if (TRACE) fprintf(TDEST,
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
    if (TRACE) fprintf(TDEST,"Time String. Can not be parsed\n");
#endif /* !NO_TIMEGM */
#endif /* !NO_MKTIME */

    if (TRACE)
	fprintf(TDEST,
		"Time string. %s parsed to %ld seconds, or in local time: %s",
		str, (long) t, ctime(&t));
    return t;
}


/*	Strip white space off a string
**	------------------------------
**
** On exit,
**	Return value points to first non-white character, or to 0 if none.
**	All trailing white space is OVERWRITTEN with zero.
*/

PUBLIC char * HTStrip ARGS1(char *, s)
{
#define SPACE(c) ((c==' ')||(c=='\t')||(c=='\n')) 
    char * p=s;
    if (!s) return NULL;	/* Doesn't dump core if NULL */
    for(p=s;*p;p++);		/* Find end of string */
    for(p--;p>=s;p--) {
    	if(SPACE(*p)) *p=0;	/* Zap trailing blanks */
	else break;
    }
    while(SPACE(*s))s++;	/* Strip leading blanks */
    return s;
}
