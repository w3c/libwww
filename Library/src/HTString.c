/*								     HTString.c
**	DYNAMIC STRING UTILITIES
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Original version came with listserv implementation.
**	Version TBL Oct 91 replaces one which modified the strings.
**	02-Dec-91 (JFG) Added stralloccopy and stralloccat
**	23 Jan 92 (TBL) Changed strallocc* to 8 char HTSAC* for VM and suchlike
**	 6 Oct 92 (TBL) Moved WWW_TraceFlag in here to be in library
**       9 Oct 95 (KR)  fixed problem with double quotes in HTNextField
**	26 Nov 96 (EGP) moved HTTrace stuff to HTTrace.c
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"					 /* Implemented here */

/* ------------------------------------------------------------------------- */

/*	Strings of any length
**	---------------------
*/
PUBLIC int strcasecomp (const char * a, const char * b)
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
PUBLIC int strncasecomp (const char * a, const char * b, int n)
{
	const char *p =a;
	const char *q =b;
	
	for(p=a, q=b;; p++, q++) {
	    int diff;
	    if (p == a+n) return 0;	/*   Match up to n characters */
	    if (!(*p && *q)) return *p - *q;
	    diff = TOLOWER(*p) - TOLOWER(*q);
	    if (diff) return diff;
	}
	/*NOTREACHED*/
}


/*
** strcasestr(s1,s2) -- like strstr(s1,s2) but case-insensitive.
*/
PUBLIC char * HTStrCaseStr (char * s1, char * s2)
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
	    if (!*cur2)	return ptr;
	}
	ptr++;
    }
    return NULL;
}

/*
** tailcomp(s1,s2) -- like strcmp(s1,s2) but match s1 with the tail of s2
**                    (used for cookie domain comparison)
*/
PUBLIC int tailcomp(const char * s1, const char * s2)
{
    int l1 = strlen(s1);
    int l2 = strlen(s2);

    if (l1 < l2)
        s2 += (l2 - l1);

    return strcmp(s1, s2);
}

PUBLIC int tailcasecomp(const char * s1, const char * s2)
{
    int l1 = strlen(s1);
    int l2 = strlen(s2);

    if (l1 < l2)
        s2 += (l2 - l1);

    return strcasecomp(s1, s2);
}

/*	Allocate a new copy of a string, and returns it
*/
PUBLIC char * HTSACopy (char ** dest, const char * src)
{
  if (*dest) HT_FREE(*dest);
  if (! src)
    *dest = NULL;
  else {
    if ((*dest  = (char  *) HT_MALLOC(strlen(src) + 1)) == NULL)
        HT_OUTOFMEM("HTSACopy");
    strcpy (*dest, src);
  }
  return *dest;
}

/*	String Allocate and Concatenate
*/
PUBLIC char * HTSACat (char ** dest, const char * src)
{
  if (src && *src) {
    if (*dest) {
      int length = strlen (*dest);
      if ((*dest  = (char  *) HT_REALLOC(*dest, length + strlen(src) + 1)) == NULL)
          HT_OUTOFMEM("HTSACat");
      strcpy (*dest + length, src);
    } else {
      if ((*dest  = (char  *) HT_MALLOC(strlen(src) + 1)) == NULL)
          HT_OUTOFMEM("HTSACat");
      strcpy (*dest, src);
    }
  }
  return *dest;
}

PUBLIC char * StrAllocMCopy (char ** dest, ...)
{
    va_list pArgs;
    char * p, * argp;

    /* How much space do we need? */
    int needed = 0;
    va_start(pArgs, dest);
    while ((p = va_arg(pArgs, char *)) != NULL) 
	needed += strlen(p);
    va_end(pArgs);

    if (*dest) HT_FREE(*dest);
    if (needed) {

	/* Allocate that amount of memory */
	if ((*dest = (char *) HT_MALLOC(needed + 1)) == NULL)
	    HT_OUTOFMEM("HTStrCpy");
	p = *dest;

	/* Fill the string */
	va_start(pArgs, dest);
	while ((argp = va_arg (pArgs, char *)) != NULL) {
	    strcpy(p, argp);
	    p += strlen(argp);
	}
	va_end (pArgs);
    }
    return *dest;
}

PUBLIC char * StrAllocMCat (char ** dest, ...)
{
    va_list pArgs;
    char * p, * argp;

    /* How much space do we need? */
    int needed = 0;
    va_start(pArgs, dest);
    while ((p = va_arg(pArgs, char *)) != NULL) 
	needed += strlen(p);
    va_end(pArgs);

    if (needed) {

	/* (Re) Allocate the amount of memory needed */
	if (*dest) {
	    int dest_len = strlen(*dest);
	    if ((*dest = (char *) HT_REALLOC(*dest, dest_len + needed + 1)) == NULL)
		HT_OUTOFMEM("HTStrCat");
	    p = *dest + dest_len;
	} else {
	    if ((*dest = (char  *) HT_MALLOC(needed + 1)) == NULL)
		HT_OUTOFMEM("HTStrCat");
	    p = *dest;
	}

	/* Fill the string */
	va_start(pArgs, dest);
	while ((argp = va_arg (pArgs, char *)) != NULL) {
	    strcpy(p, argp);
	    p += strlen(argp);
	}
	va_end (pArgs);
    }
    return *dest;
}

/*	String Matching
**	---------------
**	String comparison function for file names with one wildcard * in the
**	template. Arguments are:
**
**	tmpl	is a template string to match the name against.
**		agaist, may contain a single wildcard character * which
**		matches zero or more arbitrary characters.
**	name	is the name to be matched agaist the template.
**
**	return:	- Empty string if perfect match
**		- pointer to part matched by wildcard if any
**		- NULL if no match
*/
PUBLIC char * HTStrMatch (const char * tmpl, const char * name)
{
    while (*tmpl && *name && *tmpl==*name) tmpl++, name++;
    return ((!*tmpl && !*name) || *tmpl=='*') ? (char *) name : (char *) NULL;
}    

PUBLIC char * HTStrCaseMatch (const char * tmpl, const char * name)
{
    while (*tmpl && *name && TOUPPER(*tmpl)==TOUPPER(*name)) tmpl++, name++;
    return ((!*tmpl && !*name) || *tmpl=='*') ? (char *) name : (char *) NULL;
}    

/*	Strip white space off a string
**	------------------------------
**	Return value points to first non-white character, or to 0 if none.
**	All trailing white space is OVERWRITTEN with zero.
*/
PUBLIC char * HTStrip (char * s)
{
    if (s) {
	char * p=s;
	for(p=s;*p;p++);		/* Find end of string */
	for(p--;p>=s;p--) {
	    if (isspace((int) *p))
		*p=0;			/* Zap trailing blanks */
	    else
		break;
	}
	while (isspace((int) *s)) s++;	/* Strip leading blanks */
    }
    return s;
}

