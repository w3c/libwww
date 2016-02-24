/*

  					W3C Sample Code Library libwww News/NNTP Client


!
  Declaration of W3C Sample Code News/NNTP Client
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This application interface defines a News client which &nbsp;can be used
together with the Library core.The module itself
is not part of the Library core. It may
be registered by the application if wanted. The application may also use
the application profiles to initialize this module.
*/

#ifndef WWWNEWS_H
#define WWWNEWS_H

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
  NNTP Protocol State machine
)

The state machine handles requests and responses to an NNTP server according
to the NNTP specifications.
*/
#include "HTNews.h"

/*
(
  News Request Stream
)

The request stream generates a request which may or may not include a message
body to be posted to the server
*/
#include "HTNewsRq.h"

/*
(
  News Response Stream for Messages and group Listings
)

The News response streams parses the output from an NNTP server. The two
formats understood are News groups and News articles.
*/
#include "HTNewsLs.h"

/*
(
  Presentation of News Listings in HTML
)

When the output has been received then we convert it to HTML by parsing it
through the following stream.
*/
#include "HTNDir.h"

/*

End of News module
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
