/*

  					W3C Sample Code Library libwww FTP CLIENT


!
  Declaration of W3C Sample Code FTP MODULE
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is the include file for the basic FTP module that can be used together
with the core of the W3C Sample Code Library.
It contains all FTP specific modules which are required to compile and build
the FTP DLL.
*/

#ifndef WWWFTP_H
#define WWWFTP_H

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
  Library Includes
.
(
  The main FTP state machine
)
*/

#include "HTFTP.h"			/* FTP client state machine */

/*
(
  The FTP Directory listing Management
)
*/

#include "HTFTPDir.h"			/* Streams for parsing FTP output */

/*

End of FTP module
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
