/*

  
  					W3C Sample Code Library libwww Default Initialization


!
  Application Profiles
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

As mentioned in the Library Architecture,
libwww consists of a small core and a large set of hooks for adding
functionality. By itself, the core it not capable of performing any Web related
tasks like accessing a HTTP server or parsing a HTML document. All this
functionality must be registered by the application. This way, the core of
libwww is kept application independent and can be used as the basic building
block for any kind of Web application.

The Library comes with a large set of default functions, for example for
accessing HTTP and FTP servers, parsing
RFC
822 headers etc. but it must all be registered by the application before
the core knows about it. You can either register the parts directly through
the many initialization functions in the Initialization Interface or you
can use these "precompiled" profiles which are set up to contain the featuers
often used by the a specific type of application, for example a client, a
server, a robot etc.

This module helps the application programmer setting up all this functionality,
but it is important to note that none of it is required in order to
use the Library. All the profiles declared below superseed the Library core
initialization function HTLibInit() which is defined in the
HTLib module.

This module is implemented by HTProfil.c, and it
is a part of the W3C Sample Code
Library. You can also have a look at the other
Initialization modules.
*/

#ifndef HTPROF_H
#define HTPROF_H
#include "WWWLib.h"
#include "WWWInit.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Client Application Profiles
.

We have a couple of different client application profiles suited to different
types of clients.
(
  Traditional Client Application
)

The first uses a non-blocking socket interface which enables the application
to perform multiple requests simultanously - in everyday words called
pseudo-threads. Besides that this profile contains typical filters for handling
redirection, authentication, logging, and proxy servers.
*/

extern void HTProfile_newClient (
	const char * AppName,
	const char * AppVersion);

/*
(
  Simple HTML Client
)

As above but furthermore contains the default libwww
HTML parser
*/

extern void HTProfile_newHTMLClient (
	const char * AppName,
	const char * AppVersion);

/*
(
  Client without a Persistent Cache
)

This is the same as above except that it doesn't start the persistent cache.
*/

extern void HTProfile_newNoCacheClient (
	const char * AppName,
	const char * AppVersion);

/*
(
  Simple HTML Client without Persistent Cache
)

As above but furthermore contains the default libwww
HTML parser
*/

extern void HTProfile_newHTMLNoCacheClient (
	const char * AppName,
	const char * AppVersion);

/*
(
  Preemptive (Blocking) Client
)

Do not use preemptive profiles if you are doing PUT or
POST requests.

We also have a blocking (preemptive) version of the same client interface.
The difference is that this version uses traditional blocking sockets and
hence only one request can be performed at the same time.
*/

extern void HTProfile_newPreemptiveClient (
	const char * AppName,
	const char * AppVersion);

/*
.
  Robot Application Profile
.

The robot profile contains much of the same functionality as the client,
but it does contain less filters. For example, robots are normally not interested
in performing automatic redirections or access authentication, and hence
this is not part of the robot profile.
*/

extern void HTProfile_newRobot (
	const char * AppName,
	const char * AppVersion);

/*

Do not use preemptive profiles if you are doing PUT or
POST requests.

As for the client interfacem we also have a blocking (preemptive) version
of the same interface. The difference is that this version uses traditional
blocking sockets and hence only one reques can be performed at the same time.
*/

extern void HTProfile_newPreemptiveRobot (
	const char * AppName,
	const char * AppVersion);

/*
.
  Delete a Profile
.

This is a generic profile cleanup that should be used on all the profiles
above. This will clean up the memory allocated by the profile and by the
Library core itself. After calling this you can not call any Library function
again. This call also supersedes the termination function for the Library
core, HTLibTerminate() so that you don't have to call that after
calling this function.
*/
extern void HTProfile_delete (void);

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTPROF_H */

/*

  

  @(#) $Id$

*/
