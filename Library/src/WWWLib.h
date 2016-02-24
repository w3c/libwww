/*

  
  
  					W3C Sample Code Library libwww Basic libwww Interface


!
  Basic Libwww Interface
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is the basic include files and the core include files necessary in order
to use the W3C Sample Code Library. It contains all core specific modules which
are required to compile and build the Library. No converter streams or protocol
modules are included in this file as they are for the application to set
up. The Library has a special include file called
WWWApp.h which contains all converters and protocol
modules known to the Library. You can include this one if the application
is to use all the functionality of the Library.
*/

#ifndef WWWLIB_H
#define WWWLIB_H

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
  General Utilities
.

The Basic Utility interface is mostly container
modules, dynamic strings and other stuff that you can't live without when
building applications. The modules are used by the core part of the W3C Sample Code
Library but can also be used by the application. 
*/
#include "WWWUtil.h"

/*
.
  Core Modules
.

This is the basic include file for the core of the W3C Sample Code Library.
The core part of the Library is designed as a set of registration modules
with no real functionality in itself. Instead all the functionality comes
when the application registeres the modules that provides a desired functionaly,
for example accessing HTTP servers or the local file system. 
*/
#include "WWWCore.h"

/*
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
