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
**       9 Oct 95 (KR)  fixed problem with double quotes in HTNextField
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"					 /* Implemented here */

#if WWWTRACE_MODE == WWWTRACE_FILE
PUBLIC FILE *WWWTrace = NULL;
#endif

#ifndef WWW_WIN_DLL
PUBLIC int WWW_TraceFlag = 0;		/* Global trace flag for ALL W3 code */
#endif

/* ------------------------------------------------------------------------- */

/*	Strings of any length
**	---------------------
*/
PUBLIC int strcasecomp (CONST char * a, CONST char * b)
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
PUBLIC int strncasecomp (CONST char * a, CONST char * b, int n)
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


/*
** strcasestr(s1,s2) -- like strstr(s1,s2) but case-insensitive.
*/
PUBLIC char * strcasestr (char * s1, char * s2)
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



/*	Allocate a new copy of a string, and returns it
*/
PUBLIC char * HTSACopy (char ** dest, CONST char * src)
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
PUBLIC char * HTSACat (char ** dest, CONST char * src)
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
PUBLIC char * HTStrMatch (CONST char * tmpl, CONST char * name)
{
    while (*tmpl && *name && *tmpl==*name) tmpl++, name++;
    return ((!*tmpl && !*name) || *tmpl=='*') ? (char *) name : NULL;
}    

PUBLIC char * HTStrCaseMatch (CONST char * tmpl, CONST char * name)
{
    while (*tmpl && *name && TOUPPER(*tmpl)==TOUPPER(*name)) tmpl++, name++;
    return ((!*tmpl && !*name) || *tmpl=='*') ? (char *) name : NULL;
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
	    if (WHITE(*p))
		*p=0;			/* Zap trailing blanks */
	    else
		break;
	}
	while (WHITE(*s)) s++;		/* Strip leading blanks */
    }
    return s;
}
