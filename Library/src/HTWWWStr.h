/*

  
  
  					W3C Sample Code Library libwww WWW String Utilities


!
  WWW Related String Management
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module is like the generic string utility
module but it contains more Web related string utility functions. Examples
are functions that return a date string, a Message ID string
etc.

This module is implemented by HTWWWStr.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTWWWSTR_H
#define HTWWWSTR_H

#include "HTUser.h"
#include "HTAtom.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  MIME Parsing and other String Based Utilities
.

A bunch of "finding the next whatever" functions.
(
  Next word or quoted string
)

This function returns a RFC822 word separated by space, comma, or semi-colons.
pstr points to a string containing a word separated by white
white space "," ";" or "=". The word can optionally be quoted using
 or "" Comments surrrounded by '(' ')' are filtered out. On exit,
pstr has been moved to the first delimiter past the field THE
STRING HAS BEEN MUTILATED by a 0 terminator. The function returns a pointer
to the first word or NULL on error
*/

extern char * HTNextField (char** pstr);

/*
(
  Next Name-value Pair
)

This is the same as HTNextField but it does not look for '='
as a separator so if there is a name-value pair then both parts are returned.
Returns a pointer to the first word or NULL on error
*/

extern char * HTNextPair (char ** pstr);

/*
(
  Next Name-value Parameter
)

This is the same as HTNextPair but it does not look for ','
as a separator so if there is a name-value pair then both parts are returned.
Returns a pointer to the first word or NULL on error
*/

extern char * HTNextParam (char ** pstr);

/*
(
  Next LWS Delimited Token
)

A simpler version of the above that only looks for linear white space as
the delimiter.
*/

extern char * HTNextLWSToken (char ** pstr);

/*
(
  Find next "/" Delimited Segment
)

This is the same as HTNextField but it includes "/" as a delimiter. Returns
a pointer to the first segment or NULL on error
*/

extern char * HTNextSegment (char ** pstr);

/*
(
  Next Comma Separated String (or Element)
)

This is the same as HTNextPair but it does not look for anything else than
',' as separator Returns a pointer to the first word or NULL on error
*/

extern char * HTNextElement (char ** pstr);

/*
(
  Next S-expression
)

Find the next s-expression token from a string of characters. We return the
name of this expression and the param points to
the parameters. Note, that the string has been mutilated by a 0 terminator!
*/

extern char * HTNextSExp (char ** exp, char ** param);

/*
.
  Reading CRLF
.

The Library provides a default set of read routines that can handle the most
common situations. However, before we start we make following definition
is to make life easier when having a state machine looking for a
<CRLF> sequence.
*/

typedef enum _HTEOLState {
    EOL_ERR = -1,
    EOL_BEGIN = 0,
    EOL_FCR,
    EOL_FLF,
    EOL_DOT,
    EOL_SCR,
    EOL_SLF,
    /* intermediate states */
    EOL_END,
    EOL_FOLD,
    EOL_LINE
} HTEOLState;

/*
.
  RFC1123 Date/Time Stamp String
.

Returns a string containing a date/time stamp string in RFC-1123 format.
The string is in static memory so be aware!
*/

extern const char * HTDateTimeStr (time_t *calendar, BOOL local);

/*
.
  Date used for directory listings
.

Generates a date/time stamp string used in directory listings. There is nothing
special about this format, it is just to make directory listings look alike.
*/

extern BOOL HTDateDirStr (time_t * time, char * str, int len);

/*
.
  Parse a Date/Time String
.

Converts a variety of different string representations of date time stamps
in GMT to a local representation of localtime time_t. The local
time zone is taken from the user profile
information or directly from the system if NULL is passed as
user profile . If the time is relative (for example in the Age
header) then you can indicate whether it should be expanded to local time
or not by using the expand argument.
*/

extern time_t HTParseTime (const char * str, HTUserProfile * up, BOOL expand);

/*
.
  Unique Message-ID String
.

The message ID string can for example be use as a RFC 822 header. The content
is based on the information taken from the user
profile which can be supplied by the application.
*/
extern const char * HTMessageIdStr (HTUserProfile * up);

/*
.
  Matching MIME Content-Types
.

Matches MIME constructions for content-types and others like them,
for example "text/html", "text/plain". It can also match wild cards like
"text/" and "/. We use  instead of * in order note
to make C like comments :-)
*/

extern BOOL HTMIMEMatch (HTAtom * tmplate, HTAtom * actual);

/*
.
  Converts an Integer to a String using Prefix
.

In computer-world 1K is 1024 bytes and 1M is 1024K -- however, sprintf()
still formats in base-10. Therefore I output only until 999, and then start
using the next unit. This doesn't work wrong, it's just a feature. The conversion
is done in "str" which must be large enough to contain the result.
*/

extern void HTNumToStr (unsigned long n, char *str, int len);

/*
.
  Conversion between URLs and Local File Names
.

These are two functions that separate the URL naming syntax from platform
dependent file naming schemes. If you are porting the code to a new platform,
you probably have to do some translation here.
(
  Convert file URLs into a local representation
)

The URL has already been translated through the rules in get_physical in
HTAccess.c and all we need to do now is to map the path to a local
representation, for example if must translate '/' to the ones that turn the
wrong way ;-) Returns local file (that must be freed by caller) if OK, else
NULL.
*/

extern char * HTWWWToLocal (const char * url, const char * base,
			    HTUserProfile * up);

/*
(
  Convert a local file name into a URL
)

Generates a WWW URL name from a local file name or NULL if error. Returns
URL (that must be freed by caller) if OK, else NULL. The access parameter
can be used to indicate any special scheme used for local file access. If
NULL then "file:" is used.
*/

extern char * HTLocalToWWW (const char * local, const char * access);

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTWWWSTR_H */

/*

  

  @(#) $Id$

*/
