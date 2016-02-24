/*

  
  					W3C Sample Code Library libwww ZLib Streams


!
  W3C Sample Code Library libwww ZLib Streams
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The stream part of the Library is a set of streams that can be used to
encode/decode, or compress/decompress content. Many of the streams are based
on zlib which is a freely
available compression library.
*/

#ifndef WWWZIP_H
#define WWWZIP_H

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
  GZip Compression / Decompression
)

This stream can encode / decode gzipped content.
*/
#include "HTZip.h"

/*
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
