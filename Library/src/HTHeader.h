/*

  
  
  
  					W3C Sample Code Library libwww MIME Headers


!
  MIME Headers
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module handles lists of callback functions for generating and parsing
protocol headers. This works exactly like the lists in
HTFormat.

This module is implemented by HTHeader.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTHEADER_H
#define HTHEADER_H

#include "HTResponse.h"
#include "HTStream.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*

We have two call back functions: the first is for generating headers. This
needs a stream to put down the extra headers. This one is defined in the
Request Manager. The other one is for parsing. This
needs the string to parse.
*/

typedef int HTParserCallback (HTRequest * request, HTResponse * response,
                              char * token, char * value);

/*
.
  Header Parser Management
.

Header Parsers can be registered to handle any header. The standard set of
HTTP/1.1 MIME parsers is registered by HTMIMEInit in the
HTInit module. The HTParserCallbacks are called
by the HTMIME module.

HTParserCallbacks may be registered for known MIME headers (HTParser_*),
or for regular expressions (HTRegexParser_*). To data the regex support is
limited to the use of '*' for a wild card.
.
  Header Generator Management
.

Header Generators can be use to add additional information to aprotocol request.
They will all be called.
*/

extern BOOL HTGenerator_add (HTList * gens, HTPostCallback * callback);
extern BOOL HTGenerator_delete (HTList * gens, HTPostCallback * callback);
extern BOOL HTGenerator_deleteAll (HTList * gens);

/*
.
  Global List Of Parsers and Generators
.

As in HTFormat module you can register a list
globally or locally as you like. The local registrations is managed by
Request Manager
(
  Header Parsers
)
*/

#define MIME_HASH_SIZE  HT_L_HASH_SIZE

extern void HTHeader_setMIMEParseSet (HTMIMEParseSet * list);
extern HTMIMEParseSet * HTHeader_MIMEParseSet (void);
extern BOOL HTHeader_addParser (const char * token, BOOL case_sensitive,
				HTParserCallback * callback);
extern BOOL HTHeader_addRegexParser (const char * token, BOOL case_sensitive,
				HTParserCallback * callback);
extern BOOL HTHeader_deleteParser (const char * token);

/*
(
  Header Generation
)
*/

extern void HTHeader_setGenerator (HTList * list);
extern BOOL HTHeader_addGenerator (HTPostCallback * callback);
extern BOOL HTHeader_deleteGenerator (HTPostCallback * callback);
extern HTList * HTHeader_generator (void);

/*
(
  Delete all Global Lists
)
*/

extern void HTHeader_deleteAll (void);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTHEADER_H */

/*

  

  @(#) $Id$

*/
