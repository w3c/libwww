/*

  					W3C Sample Code Library libwww Transports


!
  Declaration of W3C Sample Code Library Transports
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*
*/

#ifndef WWWTRANS_H
#define WWWTRANS_H

/*
*/

#ifdef __cplusplus
extern "C" { 
#endif

/*
.
  System dependencies
.

The wwwsys.h file includes system-specific include
files and flags for I/O to network and disk. The only reason for this file
is that the Internet world is more complicated than Posix and ANSI.
*/

#include "wwwsys.h"

/*
.
  Transport Modules
.

These are the transports describing this interface
(
  Local File Transport
)
*/

#include "HTANSI.h"
#include "HTLocal.h"

/*
(
  BSD Socket Transport
)
*/

#include "HTTCP.h"
#include "HTSocket.h"
#include "HTReader.h"
#include "HTWriter.h"
#include "HTBufWrt.h"

/*
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
