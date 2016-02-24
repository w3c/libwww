/*

					W3C Sample Code Library libwww GOPHER CLIENT





!Declaration of W3C Sample Code GOPHER MODULE!

*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is the include file for the basic GOPHER module that can be used
together with the core of the W3C Sample Code Library. It contains all
GOPHER specific modules which are required to compile and build the
GOPHER DLL.

*/

#ifndef WWWGOPHER_H
#define WWWGOPHER_H

/*

*/

#ifdef __cplusplus
extern "C" { 
#endif

/*

.System dependencies.

The wwwsys.h file includes system-specific include
files and flags for I/O to network and disk. The only reason for this
file is that the Internet world is more complicated than Posix and
ANSI.

*/

#include "wwwsys.h"

/*

.Library Includes.


*/

#include "HTGopher.h"			      /* GOPHER client state machine */

/*

End of GOPHER module

*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*



@(#) $Id$


*/
