/*

					W3C Sample Code Library libwww HTML GENERATOR




!HTML generator!

*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module converts structed stream into stream.  That is, given a
stream to write to, it will give you a structured stream to. 

This module is implemented by HTMLGen.c, and
it is a part of the  W3C
Sample Code Library.

*/

#ifndef HTMLGEN_H
#define HTMLGEN_H

#include "HTStruct.h"
#include "HTFormat.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*

The HTML generator stream is almost a converter stream but it returns
a structured stream instead of a generic stream. The difference is
that a structured stream has methods for starting and ending mark up
elements and for HTML/SGML entities.

*/

extern HTStructured* HTMLGenerator (HTRequest *	request,
				   void *	param,
				   HTFormat	input_format,
				   HTFormat	output_format,
				   HTStream *	output_stream);


extern HTConverter HTPlainToHTML;

#ifdef __cplusplus
}
#endif

#endif  /* HTMLGEN_H */

/*



@(#) $Id$


*/
