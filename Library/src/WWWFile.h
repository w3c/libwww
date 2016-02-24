/*

  					W3C Sample Code Library libwww Local File Access


!
  Declaration of W3C Sample Code Local File Access
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is the module for accessing local files and directories. The module
contans
*/

#ifndef WWWFILE_H
#define WWWFILE_H

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
  Access the Local File System
)

The WWWFile interface provides a platform independent access scheme for local
files. The local file access works exactly like any other access scheme,
for example HTTP, in that the "file protocol" is independent of the underlying
transport. This can be used to for example slide in a CVS transport layser
underneath the file module without making any modifications to the file module
itself. You can read more about the transport managament in the
Transport Interface.
*/
#include "HTFile.h"

/*
(
  Content Negotiation
)

When accessing the local file system, you can enable content negotiation
as described in the HTTP
specification. &nbsp;The content negotiation algorithm is based on file
suffixes as defined by the Bind manager. When looking
for a file you do not have to specify a suffix. Instead this module
looks for all alternatives with the same main name. For example, looking
for the file Overview can result in any of the files (or directories)
Overview.txt, Overview.html, Overview.ps etc. The selection
algorithm is based on the values of the preferences for language, media type,
encoding, etc. - exactly like a server would do with the accept
headers.
*/

#include "HTMulti.h"

/*
(
  File Suffix Binding
)

This module sets up the binding between a file suffix and a media type, language,
encoding etc. In a client application the suffixes are used in protocols
that does not directly support media types etc., like FTP, and in server
applications they are used to make the bindings between the server and the
local file store that the server can serve to the rest of the world (well
almost)
*/

#include "HTBind.h"

/*
.
  Default File Suffix Bindings
.

Register the default set of bindings between file suffixes and media types.
This is used for example to guess the media type of a FTP URL of a local
file URL.
*/

#include "HTBInit.h"

/*

End of FILE module
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
