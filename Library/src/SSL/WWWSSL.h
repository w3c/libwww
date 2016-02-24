/*

  					W3C Sample Code Library libwww SSL Transport Wrapper


!
  W3C Sample Code Library libwww SSL Transport Wrapper
!
*/

/*
**	(c) COPYRIGHT MIT 1999.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module declares a SSL (using the OpenSSL
library, for example) transport wrapper so that libwww can use SSL as
a transport the same way it can use TCP and
local host as transports.

The SSL module is contributed by Olga Antropova
*/

#ifndef WWWSSL_H
#define WWWSSL_H

/*
*/

#ifdef __cplusplus
extern "C" { 
#endif

/*
(
  System dependencies
)

The wwwsys.h file includes system-specific include
files and flags for I/O to network and disk. The only reason for this file
is that the Internet world is more complicated than Posix and ANSI.
*/

#include "wwwsys.h"

/*
.
  The Main SSL Module
.

This module is the one that actually talks to the SSL library and handles
the internal SSL mechanisms for verifying certificates etc.
*/

#include "HTSSL.h"

/*
.
  Initializing the SSL Module for "https:"
.

This module registers the various parts in libwww required for handling the
https: URI scheme.
*/

#include "HTSSLhttps.h"

/*
.
  SSL Read Transport Stream
.

The SSL read transport stream is responsible for reading from an SSL socket
and pass the data to libwww
*/

#include "HTSSLReader.h"

/*
.
  SSL Write Transport Stream
.

The SSL write transport stream is responsible for writing data to an SSL
socket
*/

#include "HTSSLWriter.h"

/*
*/

#ifdef __cplusplus
} 
#endif

#endif  /* WWWSSL_H */

/*

  

  @(#) $Id$

*/
