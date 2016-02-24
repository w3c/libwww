/*

  					W3C Sample Code Library libwww WAIS Gateway


!
  WAIS Gateway Interface
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module does not actually perform the WAIS protocol directly, but it
does using one or more libraries of the freeWAIS distribution. The ui.a library
came with the old free WAIS from TMC, the client.a and wais.a libraries are
needed from the freeWAIS from CNIDR.

If you include this module in the library, you must also
	 
	   o 
	     Compile the Library by including the --with-wais directive in
    the configure script.
  o 
	     Have the WAIS Library available
	 
	 
The wais source files are parsed by a separate and independent module,
HTWSRC. You can include HTWSRC without
including direct wais using this module, and your WWW code will be able to
read source files, and access WAIS indexes through a gateway.

A WAIS-WWW gateway is just a normal
Web server with a libwww compiled with this module.

This module is implemented by HTWAIS.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTWAIS_H
#define HTWAIS_H

#ifdef __cplusplus
extern "C" { 
#endif 

#include "HTEvent.h"

/*
.
  Control Flags
.

The number of lines handled from a WAIS search is determined by this variable.
The default value is 100 (this is defined in the module)
*/

extern int HTMaxWAISLines;

extern HTProtCallback HTLoadWAIS;

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTWAIS_H */

/*

  

  @(#) $Id$

*/
