/*

  
  					W3C Sample Code Library libwww MIME header parser managment


!
  MIMEParseSet
!
*/

/*
**	(c) COPYRIGHT MIT 1996.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

A MIMEParseSet associates simple and regex MIME header tokens
with parsers. Regex tokens are stored in a list,
while simple tokens are stored in a hashed list for quick access.

This module is implemented by HTMIMPrs.c, and it
is a part of the  W3C Sample Code
Library.

*/

#ifndef HTMIMPRS_H
#define HTMIMPRS_H

#include "HTHeader.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Data Types
.

The HTMIMEParseSet contains all registered MIME parses. There are regex parsers,
which are stored in a list of HTMIMEParseEl, and simple (no wildcards) parsers
stored in a hashed array of lists of HTMIMEParseEl.
*/

typedef struct _HTMIMEParseEl HTMIMEParseEl;

struct _HTMIMEParseSet {
    int size;
    HTMIMEParseEl ** parsers;
    HTMIMEParseEl * regexParsers;
};

#define MIMEParseSet_NULL {0, NULL, NULL}

/*
.
  Public Functions
.

The following methods are available for this HTMIMEParseSets:
(
  Create and destroyHashLists
)
*/

extern HTMIMEParseSet * HTMIMEParseSet_new(int hashSize);
extern int HTMIMEParseSet_deleteAll (HTMIMEParseSet * me);

/*
(
  Add and remove parsers
)

Register a Header parser to be called if we encounter the token in the protocol
response. Tokens can contain a wildcard '*' which will match zero or more
arbritary chars.

*/

extern HTMIMEParseEl * HTMIMEParseSet_add (HTMIMEParseSet * me, 
					   const char * token, 
					   BOOL caseSensitive, 
					   HTParserCallback * callback);
extern HTMIMEParseEl * HTMIMEParseSet_addRegex (HTMIMEParseSet * me, 
						const char * token, 
						BOOL caseSensitive, 
						HTParserCallback * callback);
extern int HTMIMEParseSet_delete (HTMIMEParseSet * me, const char * token);

/*
(
  Execute these parsers
)

Find HTParserCallback which matches the string.
*/

extern int HTMIMEParseSet_dispatch (HTMIMEParseSet * me, HTRequest * request, 
				    char * token, char * value, BOOL * pFound);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTMIMPRS_H */

/*

  

  @(#) $Id$

*/
