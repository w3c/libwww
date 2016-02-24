/*

  					W3C Sample Code Library libwww Directory Listings


!
  Declaration of W3C Sample Code Directory Listings
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is the include file for the directory handling modules for parsing and
presenting directory listings - often in the form of a HTML document.
*/

#ifndef WWWDIR_H
#define WWWDIR_H

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
  Directory listings
)

The directory manager generates directory listings for FTP and HTTP requests.
This module contains the protocol independent code and it produces the HTML
object. It is only included if either the FTP or
the File module is included.
*/

#include "HTDir.h"

/*
(
  Icons
)

No directory listings without icons! The WWWDir interface contains support
for including references (URLs and ALT text tags) to icons in directory listings.
The icons are selected as a function of the media type and the content encoding
of the file in question. That is - you can set up icons for compressed files,
postscript files etc. There is also a small set of specific icons representing
directories etc.

Note: Icons are not set up by default! You must enable them yourself.
The Library distribution contains a small
set of default icons which you can find at
$(datadir)/www-icons, and they can be set up using the
HTIconInit() initialization function in the
WWWInit startup interface
*/
#include "HTIcons.h"

/*
(
  File Descriptions
)

Descriptions appearing in directory listings are
produced by this module. This may be overridden by another module for those
who which descriptions to come from somewhere else. It's only HTTP directory
listings that contain a description field (if enabled by the
Directory browsing module.
*/

#include "HTDescpt.h"

/*

End of DIR module
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
