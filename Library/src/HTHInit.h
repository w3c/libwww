/*

  
  					W3C Sample Code Library libwww Default HTML Parser Initialization


!
  Default HTML Parser Initialization Methods
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

As mentioned in the Library Architecture,
libwww consists of a small core and a large set of hooks for adding
functionality. By itself, the core it not capable of performing any Web related
tasks like accessing a HTTP server or parsing a HTML document. All this
functionality must be registered by the application. This way, the core of
libwww is kept application independent and can be used as the basic building
block for any kind of Web application. The Library comes with a large set
of default functions, for example for accessing HTTP and FTP servers, parsing
RFC
822 headers etc. This module helps the application programmer setting
up the HTML parser, but it is important to note that none of it is
required in order to use the Library.

This module is implemented by HTInit.c, and it is
a part of the W3C Sample Code
Library. You can also have a look at the other
Initialization modules.
*/

#ifndef HTHINIT_H
#define HTHINIT_H
#include "WWWLib.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Default HTML Parsers
.

The Converters are used to convert a media type to another media
type, or to present it on screen. This is a part of the stream stack algorithm.
The Presenters are also used in the stream stack, but are initialized separately.
*/

#include "HTML.h"			/* Uses HTML/HText interface */
#include "HTPlain.h"			/* Uses HTML/HText interface */

#include "HTTeXGen.h"
#include "HTMLGen.h"

extern void HTMLInit		(HTList * conversions);


/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTHINIT_H */

/*

  

  @(#) $Id$

*/
