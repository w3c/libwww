/*

  					W3C Sample Code Library libwww Media Type Initialization


!
  Default Media Type Initialization Module
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
up all this functionality, but it is important to note that none of it is
required in order to use the Library.

This module is implemented by HTBInit.c, and it is
a part of the W3C Sample Code
Library. You can also have a look at the other
Initialization modules.
*/

#ifndef HTBINIT_H
#define HTBINIT_H

#ifdef __cplusplus
extern "C" { 
#endif 


/*
.
  Default File Suffix Binding
.

Register the default set of bindings between file suffixes and media types.
This is used for example to guess the media type of a FTP URL of a local
file URL.
*/

#include "WWWCore.h"

extern void HTFileInit (void);

/*
*/

#ifdef __cplusplus
}
#endif

#endif

/*

  

  @(#) $Id$

*/
