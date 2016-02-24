/*

  					W3C Sample Code Library libwww HTML Parser With Text Object Converter


!
  Simple HTML Parser With Text Object Converter
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This interprets the HTML
semantics passed to it by the SGML parser. It then
creates a HText object which the application can
use to render an HTML object as text comes in over the wire. The
stylesheet manager can be used to ensure that
the rendering is done according to whatever style information is present.

This module is implemented by HTML.c, and it is a part
of the  W3C Sample Code Library.
*/

#ifndef HTML_H
#define HTML_H

#include "HTFormat.h"
#include "HTMLPDTD.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Converters
.

These are the converters implemented in this module. The first converts from
HTML to presentation or plain text.
*/

extern HTConverter HTMLToPlain;

/*

The next converts from HTML to a simple C like representation. It puts everything
not in PRE within C style comments. This is the way that the line mode browser
is used to convert the libwww HTML files to C style .h files
*/

extern HTConverter HTMLToC;

/*

This one converts the HTML stream to a rendered object using the
HText interface.
*/

extern HTConverter HTMLPresent;

/*
.
  Selecting Internal Character Set Representation
.

Only ISO_LATIN1 is currently supported.
*/

typedef enum _HTMLCharacterSet {
	HTML_ISO_LATIN1,
	HTML_NEXT_CHARS,
	HTML_PC_CP950
} HTMLCharacterSet;

extern BOOL HTMLUseCharacterSet (HTMLCharacterSet charset);

/*
*/

#ifdef __cplusplus
}
#endif

#endif		/* end HTML_H */

/*

  

  @(#) $Id$

*/
