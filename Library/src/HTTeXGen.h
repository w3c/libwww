/*                 /Net/dxcern/userd/timbl/hypertext/WWW/Library/Implementation/HTTeXGen.html
                                      TEX GENERATOR
                                             
   This module converts structed stream into stream.  That is, given a stream to write to,
   it will give you a structured stream to
   
 */
#ifndef HTTEXGEN_H
#define HTTEXGEN_H

#include "HTML.h"
#include "HTStream.h"

/* Subclass:
*/
/* extern CONST HTStructuredClass HTTeXGeneration; */

/* Special Creation:
*/
extern HTStructured * HTTeXGenerator PARAMS((HTStream * output));

#endif

/*

    */
