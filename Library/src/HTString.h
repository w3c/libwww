/*

  
  					W3C Sample Code Library libwww Generic String Management


!
  Generic String Management
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

These functions provide functionality for case-independent string comparison
and allocations with copies etc.

This module is implemented by HTString.c, and it
is a part of the W3C
Sample Code Library.
*/

#ifndef HTSTRING_H
#define HTSTRING_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Dynamic String Manipulation
.

These two functions are dynamic versions of strcpy and
strcat. They use malloc for allocating space for
the string. If StrAllocCopy is called with a non-NULL dest,
then this is freed before the new value is assigned so that only the
last string created has to be freed by the user. If
StrAllocCat is called with a NULL pointer as destination then
it is equivalent to StrAllocCopy. 
*/

#define StrAllocCopy(dest, src) HTSACopy (&(dest), src)
#define StrAllocCat(dest, src)  HTSACat  (&(dest), src)

extern char * HTSACopy (char **dest, const char *src);
extern char * HTSACat  (char **dest, const char *src);

/*

The next two functions take a variable number of strings and cats them together
using dynamic memory. This is basically like a simple form for sprintf where
the only argument is char *. One day we should turn this into
a real dynamic sprintf().
*/

extern char * StrAllocMCopy (char ** dest, ...);
extern char * StrAllocMCat (char ** dest, ...);

/*

The last argument MUST be NULL as we otherwise don't know when the argument
list stops.
.
  Case-insensitive String Comparison
.

The usual routines (comp instead of cmp) had some problem.
*/

extern int strcasecomp  (const char *a, const char *b);
extern int strncasecomp (const char *a, const char *b, int n);

/*
.
  Tail String Comparison
.

Like strcmp, but match the tail of s2 (used for cookie domain comparison)
*/

extern int tailcomp(const char * s1, const char * s2);
extern int tailcasecomp(const char * s1, const char * s2);

/*
.
  String Comparison with Wild Card Match
.

String comparison function for file names with one wildcard * in the template.
Arguments are:

  
    tmpl
  
    is a template string to match the name against. agaist, may contain a single
    wildcard character * which matches zero or more arbitrary characters.
  
    name
  
    is the name to be matched agaist the template.


Returns empty string ("") if perfect match, pointer to part matched by wildcard
if any, or NULL if no match. This is basically the same as YES if match,
else NO.
*/

extern char * HTStrMatch	(const char * tmpl, const char * name);
extern char * HTStrCaseMatch	(const char * tmpl, const char * name);

/*
.
  Case-insensitive strstr
.

This works like strstr() but is not case-sensitive.
*/

extern char * HTStrCaseStr (char * s1, char * s2);

/*
.
  Strip white space off a string
.

Return value points to first non-white character, or to '/0' if none. All
trailing white space is OVERWRITTEN with zero.
*/

extern char * HTStrip (char * s);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* !HTSTRING_H */

/*

  

  @(#) $Id$

*/
