/*

  					W3C Sample Code Library libwww Persistent Cache Manager


!
  Declaration of W3C Sample Code Persistent Cache Manager
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

Caching is a required part of any efficient Internet access applications
as it saves bandwidth and improves access performance significantly in almost
all types of accesses. The Library supports two different types of cache:
The memory cache and the file cache. The two types differ in several ways
which reflects their two main purposes: The memory cache is for short term
storage of graphic objects whereas the file cache is for intermediate term
storage of data objects. Often it is desirable to have both a memory and
a file version of a cached document, so the two types do not exclude each
other.

The cache contains details of persietent files which contain the contents
of remote documents. The existing cache manager is somewhat naive - especially
in its garbage collection but it is just an example of how it can be done.
More advanced implementations are welcome!
*/

#ifndef WWWCACHE_H
#define WWWCACHE_H

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
(
  Caching Manager
)

The cache interface defines a persistent cache manager based on accessing
files.
*/
#include "HTCache.h"

/*
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
