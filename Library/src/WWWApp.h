/*

  
  					W3C Sample Code Library libwww Application Interface


!
  Application Interface
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

In addition top the basic W3C Sample Code Library WWWLib
interface you may include the other
interfaces&nbsp;depending on the needs of your
application. However, it is not required and none of the files included below
are ever used in the core part of the Library itself. Only if this file is
included, the extra modules will get included in the linked object code.
It is also possible to include only a subset of the files below if the
functionality you are after is covered by them. This interface contains many
application specific features including a set of default
BEFORE and AFTER filters.
*/

#ifndef WWWAPP_H
#define WWWAPP_H

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
  Event Manager
)

The core part of libwww only provides the hooks for the event manager. There
is no event loop internal to the core part. Instead the application must
provide the event loop in order to use either pseudo threads or real threads.
If the application only uses blocking sockets without threads then it is
not required to register any event loop at all. We provide a default
implementation of an event loop which you can either take or get some ideas
from.
*/
#include "HTEvtLst.h"

/*
(
  Managing the Home Page
)

This module provides some "make life easier" functions in order to get the
application going. They help you generate the first anchor, also called the
home anchor. It also contains a nice set of default WWW addresses.
*/
#include "HTHome.h"

/*
(
  User Dialogs and Messages
)

You can register a set of callback functions to handle user prompting, error
messages, confimations etc. Here we give a set of functions that can be used
on almost anu thinkable platform. If you want to provide your own platform
dependent implementation then fine :-)
*/
#include "HTDialog.h"

/*
(
  Load, Upload, and Search URLs
)

Even though you may use the API for the HTRequest object directly in order
to issue a request, you will probably find that in real life it is easier
to use a higher level abstraction API. This API is provided by the
HTAccess module where you will find all kind
of functions for down loading a URL etc.
*/
#include "HTAccess.h"

/*
(
  Rule File Management
)

Another way to initialize applications is to use a rule file, also known
as a configuration file. This is for
example the case with the W3C httpd and the W3C Line Mode Browser. This module
provides basic support for configuration file management and the application
can use this is desired. The module is not referred to by the Library. Reading
a rule file is implemented as a stream converter so that a rule file can
come from anywhere, even across the network!
*/
#include "HTRules.h"

/*
(
  Proxies and Gateways
)

Applications do not have to provide native support for all protocols, they
can in many situations rely on the support of proxies and gateways to help
doing the job. Proxy servers are often used to carry client requests through
a firewall where they can provide services like corporate caching and other
network optimizations. Both Proxy servers and gateways can serve as "protocol
translators" which can convert a request in the main Web protocol, HTTP,
to an equivalent request in another protocol, for example NNTP, FTP, or Gopher.
In case a proxy server or a gateway is available to the application, it can
therefore by use of HTTP forward all requests to for example a proxy server
which then handle the communications with the remote server, for example
using FTP about the document and return it to the application (proxy client)
using HTTP.
*/
#include "HTProxy.h"

/*
(
  BEFORE and AFTER Filters
)

Before a request has been issued and after it has terminated
the application often has to do some action as a result of the request (and
of the result of the request). The Client Profile Interface Library provides
a set of standard BEFORE and AFTER filters to handle caching,
redirection, authentication, logging etc.
*/
#include "HTFilter.h"

/*
(
  Logging
)

Often it is required to log the requests issued to the Library. This can
either be the case if the application is a server or it can also be useful
in a client application. This module provides a simple logging mechanism
which can be enabled if needed. See also the SQL
based logging module.
*/
#include "HTLog.h"

/*
(
  History Management
)

Another type of logging is keeping track of which documents a user has visited
when browsing along on the Web. The Library history manager provides a basic
set of functionality to keep track of a linear history list.
*/
#include "HTHist.h"

/*

End of application specific modules
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
