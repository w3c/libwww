/*		Case-independent string comparison		HTString.c
**
**	Original version came with listserv implementation.
**	Version TBL Oct 91 replaces one which modified the strings.
**	02-Dec-91 (JFG) Added stralloccopy and stralloccat
**	23 Jan 92 (TBL) Changed strallocc* to 8 char HTSAC* for VM and suchlike
**	 6 Oct 92 (TBL) Moved WWW_TraceFlag in here to be in library
*/

#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"					 /* Implemented here */

PUBLIC int WWW_TraceFlag = 0;	/* Global trace flag for ALL W3 code */

#ifndef VC
#define VC "unknown"
#endif

PUBLIC CONST char * HTLibraryVersion = VC; /* String for help screen etc */

#ifndef VM		/* VM has these already it seems */
	
/*	Strings of any length
**	---------------------
*/
PUBLIC int strcasecomp ARGS2 (CONST char*,a, CONST char *,b)
{
	CONST char *p =a;
	CONST char *q =b;
	for(p=a, q=b; *p && *q; p++, q++) {
	    int diff = TOLOWER(*p) - TOLOWER(*q);
	    if (diff) return diff;
	}
	if (*p) return 1;	/* p was longer than q */
	if (*q) return -1;	/* p was shorter than q */
	return 0;		/* Exact match */
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
    char * try = s1;

    if (!s1 || !s2 || !*s2) return s1;

    while (*try) {
	if (TOUPPER(*try) == TOUPPER(*s2)) {
	    char * cur1 = try + 1;
	    char * cur2 = s2 + 1;
	    while (*cur1 && *cur2 && TOUPPER(*cur1) == TOUPPER(*cur2)) {
		cur1++;
		cur2++;
	    }
	    if (!*cur2) {
		CTRACE(stderr,
	      "Debug....... strcasestr(s1 = \"%s\", s2 = \"%s\") => \"%s\"\n",
		       s1,s2,try);
		return try;
	    }
	}
	try++;
    }
    CTRACE(stderr,
	   "Debug....... strcasestr(s1 = \"%s\", s2 = \"%s\") => No match\n",
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
**	*pstr	points to a string containig white space separated
**		field, optionlly quoted.
**
** On exit,
**	*pstr	has been moved to the first delimiter past the
**		field
**		THE STRING HAS BEEN MUTILATED by a 0 terminator
**
**	returns	a pointer to the first field
*/
PUBLIC char * HTNextField ARGS1(char **, pstr)
{
    char * p = *pstr;
    char * start;			/* start of field */
    
    while(*p && WHITE(*p)) p++;		/* Strip white space */
    if (!*p) {
	*pstr = p;
        return NULL;		/* No first field */
    }
    if (*p == '"') {			/* quoted field */
        p++;
	start = p;
	for(;*p && *p!='"'; p++) {
	    if (*p == '\\' && p[1]) p++;	/* Skip escaped chars */
	}
    } else {
	start = p;
	while(*p && !WHITE(*p)) p++;	/* Skip first field */
    }
    if (*p) *p++ = 0;
    *pstr = p;
    return start;
}
