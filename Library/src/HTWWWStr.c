/*								     HTWWWStr.c
**	WWW RELATED STRING UTILITIES
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**      Now 13 95	Spwaned from HTString.c
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTParse.h"
#include "HTInet.h"
#include "HTUser.h"
#include "HTWWWStr.h"					 /* Implemented here */

PRIVATE char * months[12] = {
    "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

#ifndef HAVE_STRFTIME
PRIVATE char * wkdays[7] = {
    "Sun","Mon","Tue","Wed","Thu","Fri","Sat"
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
    if (!pstr || !*pstr) return NULL;
    while (1) {
	/* Strip white space and other delimiters */
	while (*p && (isspace((int) *p) || *p==',' || *p==';' || *p=='=')) p++;
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
	    while(*p && !isspace((int) *p) && *p!=',' && *p!=';' && *p!='=')
		p++;
	    break;						   /* Got it */
	}
    }
    if (*p) *p++ = '\0';
    *pstr = p;
    return start;
}

/*	Find next LWS delimited token
**	-----------------------------
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
PUBLIC char * HTNextLWSToken (char ** pstr)
{
    char * p = *pstr;
    char * start = NULL;
    if (!pstr || !*pstr) return NULL;

    /* Strip initial white space  */
    while (*p && (isspace((int) *p))) p++;
    if (!*p) {
	*pstr = p;
	return NULL;				   	 /* No field */
    }

    /* Now search for the next white space */
    start = p;
    while(*p && !isspace((int) *p)) p++;

    if (*p) *p++ = '\0';
    *pstr = p;
    return start;
}

/*	Find next Name-value pair
**	-------------------------
**	This is the same as HTNextField but it does not look for '=' as a
**	separator so if there is a name-value pair then both parts are
**	returned.
**	Returns	a pointer to the first word or NULL on error
*/
PUBLIC char * HTNextPair (char ** pstr)
{
    char * p = *pstr;
    char * start = NULL;
    if (!pstr || !*pstr) return NULL;
    while (1) {
	/* Strip white space and other delimiters */
	while (*p && (*p==',' || *p==';')) p++;
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
	    while(*p && *p!=',' && *p!=';')
		p++;
	    break;						   /* Got it */
	}
    }
    if (*p) *p++ = '\0';
    *pstr = p;
    return start;
}

/*	Find next Name-value param
**	--------------------------
**	This is the same as HTNextPair but doesn't look for ','
**	Returns	a pointer to the first word or NULL on error
*/
PUBLIC char * HTNextParam (char ** pstr)
{
    char * p = *pstr;
    char * start = NULL;
    if (!pstr || !*pstr) return NULL;
    while (1) {
	/* Strip white space and other delimiters */
	while (*p && *p==';') p++;
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
	    while (*p && *p!=';')
		p++;
	    break;						   /* Got it */
	}
    }
    if (*p) *p++ = '\0';
    *pstr = p;
    return start;
}

/*	Find next element in a comma separated string
**	---------------------------------------------
**	This is the same as HTNextPair but it does not look for anything
**	else than ',' as separator
**	Returns	a pointer to the first word or NULL on error
*/
PUBLIC char * HTNextElement (char ** pstr)
{
    char * p = *pstr;
    char * start = NULL;
    if (!pstr || !*pstr) return NULL;

    /* Strip white space and other delimiters */
    while (*p && ((isspace((int) *p)) || *p==',')) p++;
    if (!*p) {
	*pstr = p;
	return NULL;					   	 /* No field */
    }
    start = p;
    while (1) {
	if (*p == '"') {				     /* quoted field */
	    for(;*p && *p!='"'; p++)
		if (*p == '\\' && *(p+1)) p++;	       /* Skip escaped chars */
	    p++;
	} else if (*p == '<') {				     /* quoted field */
	    for(;*p && *p!='>'; p++)
		if (*p == '\\' && *(p+1)) p++;	       /* Skip escaped chars */
	    p++;
	} else if (*p == '(') {					  /* Comment */
	    for(;*p && *p!=')'; p++)
		if (*p == '\\' && *(p+1)) p++;	       /* Skip escaped chars */
	    p++;
	} else {					      /* Spool field */
	    while (*p && *p!=',') p++;
	    break;						   /* Got it */
	}
    }
    if (*p) *p++ = '\0';
    *pstr = p;
    return start;
}

/*	Find next "/" delimied segment
**	------------------------------
**	This is the same as HTNextField but it includes "/" as a delimiter.
**	Returns	a pointer to the first segment or NULL on error
*/
PUBLIC char * HTNextSegment (char ** pstr)
{
    char * p = *pstr;
    char * start = NULL;
    if (!pstr || !*pstr) return NULL;
    while (1) {
	/* Strip white space and other delimiters */
	while (*p && (isspace((int) *p) || *p==',' || *p==';' || *p=='=' || *p=='/')) p++;
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
	    while(*p && !isspace((int) *p) && *p!=',' && *p!=';' && *p!='=' && *p!='/')
		p++;
	    break;						   /* Got it */
	}
    }
    if (*p) *p++ = '\0';
    *pstr = p;
    return start;
}

/*
**	Find the next s-expression token from a string of characters.
**	We return the name of this expression and the param points to the
**	parameters. 
**
**	NOTE: The string has been mutilated by '/0's
*/
PUBLIC char * HTNextSExp (char ** exp, char ** param)
{
    char * p = *exp;
    char * name = NULL;
    if (!exp || !*exp) return NULL;
    while (*p && isspace((int) *p)) p++;		/* Strip leading white space */
    if (!*p) {
	*exp = p;
	return NULL;					   	 /* No field */
    }
    if (*p == '{') {					     /* Open bracket */
	int cnt = 1;
	/*
	**  Look for name of this expression. If we find a token then search
	**  for the rest of the expression and remove the end '}'
	*/
	p++;
	if ((name = HTNextField(&p)) == NULL) return NULL;
	while (*p && isspace((int) *p)) p++;
	*param = p;
	while (*p) {
	    if (*p == '{') cnt++;
	    if (*p == '}') cnt--;
	    if (!cnt) {
		*p = '\0';
		break;
	    }
	    p++;
	}
    }
    return name;
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
PUBLIC const char * HTMessageIdStr (HTUserProfile * up)
{
    static char buf[80];
    time_t sectime = time(NULL);
#ifdef HAVE_GETPID
    const char * address = HTUserProfile_fqdn(up);
#else
    const char * address = HTUserProfile_email(up);
#endif /* HAVE_GETPID */
    if (!address) address = tmpnam(NULL);
    if ((!address || !*address) && sectime < 0) {
	HTTRACE(CORE_TRACE, "MessageID...  Can't make a unique MessageID\n");
	return "";
    }
#ifdef HAVE_GETPID
    sprintf(buf, "<%ldZ%ld@%s>", (long)sectime, (long)getpid(), address ? address : "@@@");
#else
    sprintf(buf, "<%ldZ%s>", sectime, address ? address : "@@@");
#endif /* HAVE_GETPID */

    *(buf+79) = '\0';
    return buf;
}

/*	Date and Time Parser
**	--------------------
**	These functions are taken from the server written by Ari Luotonen
*/
#if 0
PRIVATE int make_num (const char *  s)
{
    if (*s >= '0' && *s <= '9')
	return 10 * (*s - '0') + *(s+1) - '0';
    else
	return *(s+1) - '0';
}
#endif
PRIVATE int make_month (char * s, char ** ends)
{
    char * ptr = s;
    while (!isalpha((int) *ptr)) ptr++;
    if (*ptr) {
	int i;
	*ends = ptr+3;		
	for (i=0; i<12; i++)
	    if (!strncasecomp(months[i], ptr, 3)) return i;
    }
    return 0;
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
PUBLIC time_t HTParseTime (const char * str, HTUserProfile * up, BOOL expand)
{
    char * s;
    struct tm tm;
    time_t t;

    if (!str) return 0;

    if ((s = strchr(str, ','))) {	 /* Thursday, 10-Jun-93 01:29:59 GMT */
	s++;				/* or: Thu, 10 Jan 1993 01:29:59 GMT */
	while (*s && *s==' ') s++;
	if (strchr(s,'-')) {				     /* First format */
	    HTTRACE(CORE_TRACE, "Format...... Weekday, 00-Mon-00 00:00:00 GMT\n");
	    if ((int)strlen(s) < 18) {
		HTTRACE(CORE_TRACE, "ERROR....... Not a valid time format \"%s\"\n" _ s);
		return 0;
	    }
	    tm.tm_mday = strtol(s, &s, 10);
	    tm.tm_mon = make_month(s, &s);
	    tm.tm_year = strtol(++s, &s, 10);
	    tm.tm_hour = strtol(s, &s, 10);
	    tm.tm_min = strtol(++s, &s, 10);
	    tm.tm_sec = strtol(++s, &s, 10);

	} else {					    /* Second format */
	    HTTRACE(CORE_TRACE, "Format...... Wkd, 00 Mon 0000 00:00:00 GMT\n");
	    if ((int)strlen(s) < 20) {
		HTTRACE(CORE_TRACE, "ERROR....... Not a valid time format \"%s\"\n" _ s);
		return 0;
	    }
	    tm.tm_mday = strtol(s, &s, 10);
	    tm.tm_mon = make_month(s, &s);
	    tm.tm_year = strtol(s, &s, 10) - 1900;
	    tm.tm_hour = strtol(s, &s, 10);
	    tm.tm_min = strtol(++s, &s, 10);
	    tm.tm_sec = strtol(++s, &s, 10);
	}
    } else if (isdigit((int) *str)) {

	if (strchr(str, 'T')) {		        /* ISO (limited format) date string */
	    HTTRACE(CORE_TRACE, "Format...... YYYY.MM.DDThh:mmStzWkd\n");
	    s = (char *) str;
	    while (*s && *s==' ') s++;
	    if ((int)strlen(s) < 21) {
		HTTRACE(CORE_TRACE, "ERROR....... Not a valid time format `%s\'\n" _ s);
		return 0;
	    }
	    tm.tm_year = strtol(s, &s, 10) - 1900;
	    tm.tm_mon  = strtol(++s, &s, 10);
	    tm.tm_mday = strtol(++s, &s, 10);
	    tm.tm_hour = strtol(++s, &s, 10);
	    tm.tm_min  = strtol(++s, &s, 10);
	    tm.tm_sec  = strtol(++s, &s, 10);

	} else {					    /* delta seconds */
	    t = expand ? time(NULL) + atol(str) : atol(str);

#ifdef HTDEBUG
	    if (CORE_TRACE) {
		if (expand) {
#if defined (HAVE_CTIME_R_2)
		    char buffer[CTIME_MAX];
		    HTTRACE(CORE_TRACE, "Time string. Delta-time %s parsed to %ld seconds, or in local time: %s" _
			    str _ (long) t _ (char *) ctime_r(&t, buffer));
#elif defined(HAVE_CTIME_R_3)
		    char buffer[CTIME_MAX];
		    HTTRACE(CORE_TRACE, "Time string. Delta-time %s parsed to %ld seconds, or in local time: %s" _
			    str _ (long) t _ (char *) ctime_r(&t, buffer, CTIME_MAX));
#else
		    HTTRACE(CORE_TRACE, "Time string. Delta-time %s parsed to %ld seconds, or in local time: %s" _
			    str _ (long) t _ ctime(&t));
#endif /* HT_REENTRANT */
		} else {
		    HTTRACE(CORE_TRACE, "Time string. Delta-time %s parsed to %ld seconds\n" _ str _ (long) t);
		}
	    }
#endif /* HT_DEBUG */
	    return t;
	}

    } else {	      /* Try the other format:  Wed Jun  9 01:29:59 1993 GMT */
	HTTRACE(CORE_TRACE, "Format...... Wkd Mon 00 00:00:00 0000 GMT\n");
	s = (char *) str;
	while (*s && *s==' ') s++;
	HTTRACE(CORE_TRACE, "Trying...... The Wrong time format: %s\n" _ s);
	if ((int)strlen(s) < 24) {
	    HTTRACE(CORE_TRACE, "ERROR....... Not a valid time format \"%s\"\n" _ s);
	    return 0;
	}
	tm.tm_mon = make_month(s, &s);
	tm.tm_mday = strtol(s, &s, 10);
	tm.tm_hour = strtol(s, &s, 10);
	tm.tm_min = strtol(++s, &s, 10);
	tm.tm_sec = strtol(++s, &s, 10);
	tm.tm_year = strtol(s, &s, 10) - 1900;
    }
    if (tm.tm_sec  < 0  ||  tm.tm_sec  > 59  ||
	tm.tm_min  < 0  ||  tm.tm_min  > 59  ||
	tm.tm_hour < 0  ||  tm.tm_hour > 23  ||
	tm.tm_mday < 1  ||  tm.tm_mday > 31  ||
	tm.tm_mon  < 0  ||  tm.tm_mon  > 11  ||
	tm.tm_year <70  ||  tm.tm_year >120) {
	HTTRACE(CORE_TRACE, "ERROR....... Parsed illegal time: %02d.%02d.%02d %02d:%02d:%02d\n" _ 
	       tm.tm_mday _ tm.tm_mon+1 _ tm.tm_year _ 
	       tm.tm_hour _ tm.tm_min _ tm.tm_sec);
	return 0;
    }

#if 0
#if defined(HAVE_TIMEZONE) && defined(HAVE_ALTZONE)
    tm.tm_isdst = daylight;		       /* Already taken into account */
    HTTRACE(CORE_TRACE, "Time string. Daylight is %s\n" _
	    daylight>0 ? "on" : daylight==0 ? "off" : "unknown");
#endif
#else
    /* Let mktime decide whether we have DST or not */
    tm.tm_isdst = -1;
#endif

#ifdef HAVE_MKTIME
    t = mktime(&tm);
    t += (up ? HTUserProfile_timezone(up) : HTGetTimeZoneOffset());
#else
#ifdef HAVE_TIMEGM
    t = timegm(&tm);
#else
#error "Neither mktime nor timegm defined"
#endif /* HAVE_TIMEGM */
#endif /* HAVE_MKTIME */

    HTTRACE(CORE_TRACE, "Time string. %s parsed to %ld calendar time or `%s' in local time\n" _ 
		str _ (long) t _ ctime(&t));
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
PUBLIC const char *HTDateTimeStr (time_t * calendar, BOOL local)
{
    static char buf[40];

#ifdef HAVE_STRFTIME
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
	sprintf(buf,"%s, %02d %s %04d %02d:%02d:%02d",
		wkdays[loctime->tm_wday],
		loctime->tm_mday,
		months[loctime->tm_mon],
		loctime->tm_year + 1900,
		loctime->tm_hour,
		loctime->tm_min,
		loctime->tm_sec);
    } else {
#if defined(HT_REENTRANT) || defined(SOLARIS)
	struct tm gmt;
	gmtime_r(calendar, &gmt);
#else
	struct tm *gmt = gmtime(calendar);
#endif
	sprintf(buf,"%s, %02d %s %04d %02d:%02d:%02d GMT",
		wkdays[gmt->tm_wday],
		gmt->tm_mday,
		months[gmt->tm_mon],
		gmt->tm_year + 1900,
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
#ifdef HAVE_STRFTIME
#if defined(HT_REENTRANT) || defined(SOLARIS)
    struct tm loctime;
    localtime_r(time, &loctime);
    strftime(str, len, "%d-%b-%Y %H:%M", &loctime);
    return YES;
#else
    strftime(str, len, "%d-%b-%Y %H:%M", localtime(time));
    return YES;
#endif /* HT_REENTRANT || SOLARIS */
#else
    if (len >= 16) {
	struct tm *loctime = localtime(time);
	sprintf(str,"%02d-%s-%02d %02d:%02d",
		loctime->tm_mday,
		months[loctime->tm_mon],
		loctime->tm_year % 100,
		loctime->tm_hour,
		loctime->tm_min);
	return YES;
    }
    return NO;
#endif /* HAVE_STRFTIME */		
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

/*
**	Matches MIME constructions for content-types and others like
**	them, for example "text/html", "text/plain". It can also match
**	wild cards like "text/<star>" and "<star>/<star>. We use <star>
**	instead of * in order note to make C like comments :-)
*/
PUBLIC BOOL HTMIMEMatch (HTAtom * tmplate, HTAtom * actual)
{
    const char *t, *a;
    char *st, *sa;
    BOOL match = NO;

    if (tmplate && actual && (t = HTAtom_name(tmplate))) {
	if (!strcmp(t, "*"))
	    return YES;

	if (strchr(t, '*') &&
	    (a = HTAtom_name(actual)) &&
	    (st = strchr(t, '/')) && (sa = strchr(a,'/'))) {

	    *sa = 0;
	    *st = 0;

	    if ((*(st-1)=='*' &&
		 (*(st+1)=='*' || !strcasecomp(st+1, sa+1))) ||
		(*(st+1)=='*' && !strcasecomp(t,a)))
		match = YES;

	    *sa = '/';
	    *st = '/';
	}    
    }
    return match;
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
PUBLIC char * HTWWWToLocal (const char * url, const char * base,
			    HTUserProfile * up)
{
    if (url) {
	char * access = HTParse(url, base, PARSE_ACCESS);
	char * host = HTParse(url, base, PARSE_HOST);
	char * path = HTParse(url, base, PARSE_PATH+PARSE_PUNCTUATION);
	const char * myhost = HTUserProfile_fqdn(up);

	/* Find out if this is a reference to the local file system */
	if ((*access && strcmp(access, "file") && strcmp(access, "cache")) ||
	     (*host && strcasecomp(host, "localhost") &&
	      myhost && strcmp(host, myhost))) {
	    HTTRACE(CORE_TRACE, "LocalName... Not on local file system\n");
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
	    /* An absolute pathname with logical drive */
            if (*path == '/' && path[2] == ':') {
		char *orig=path, *dest=path+1;
		while((*orig++ = *dest++));

	    /* A network host */
            } else if (*host && strcasecomp(host, "localhost")) {
		char * newpath = NULL;
		StrAllocMCopy(&newpath, "//", host, path, NULL);
		HT_FREE(path);
		path = newpath;
	    }

	    /* Convert '/' to '\' */
	    {
		char *p = path;
		while (*p) {
		    if (*p=='/') *p='\\';
		    p++;
		}
	    }
#endif
	    
	    HTUnEscape(path);		  /* Take out the escaped characters */
	    HTTRACE(CORE_TRACE, "Node........ `%s' means path `%s'\n" _ url _ path);
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
PUBLIC char * HTLocalToWWW (const char * local, const char * access)
{
    char * escaped = NULL;
    const char * scheme = (access && *access) ? access : "file:"; 
    if (local && *local) {
#ifdef VMS 
        char * unescaped = NULL;
        if ((unescaped = (char *) HT_MALLOC(strlen(local) + 10)) == NULL)
            HT_OUTOFMEM("HTLocalToWWW");
        strcpy(unescaped, scheme);	     /* We get an absolute file name */

	/* convert directory name to Unix-style syntax */
	{
	    char * disk = strchr (local, ':');
	    char * dir = strchr (local, '[');
	    if (disk) {
		*disk = '\0';
		strcat(unescaped, "/");
		strcat(unescaped, local);
	    }
	    if (dir) {
		char *p;
		*dir = '/';	/* Convert leading '[' */
		for (p = dir ; *p != ']'; ++p)
		    if (*p == '.') *p = '/';
		*p = '\0';	/* Cut on final ']' */
		strcat(unescaped, dir);
	    }
	}
        escaped = HTEscape(unescaped, URL_DOSFILE);
        HT_FREE(unescaped);

#else  /* not VMS */
#ifdef WIN32
        char * unescaped = NULL;
        if ((unescaped = (char *) HT_MALLOC(strlen(local) + 10)) == NULL)
            HT_OUTOFMEM("HTLocalToWWW");
        strcpy(unescaped, scheme);	     /* We get an absolute file name */
        if (strchr(local, ':')) strcat(unescaped, "/");
        {
            const char *p = local;
            char *q = unescaped+strlen(unescaped);
            while (*p) {
                if (*p=='\\') {
                    *q++='/';
                } else
                    *q++=*p;
                p++;
            }
            *q = '\0';
	}
        escaped = HTEscape(unescaped, URL_DOSFILE);
        HT_FREE(unescaped);

#else  /* Unix */
        char * escaped_path = HTEscape(local, URL_PATH);
	escaped = StrAllocMCopy(&escaped, scheme, escaped_path, NULL);
        HT_FREE(escaped_path);

#endif /* not WIN32 */
#endif /* not VMS */
    }
    return escaped;
}
