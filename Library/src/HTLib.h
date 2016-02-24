/*

  					W3C Sample Code Library libwww Initialization


!
  Libwww Initialization
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module contains some generic functions for getting the name and version
of libwww. It also contains some global configuration options like if you
can access the local file system, for example.

This module is implemented by HTLib.c, and it is a
part of the  W3C Sample Code Library.
*/

#ifndef HTLIB_H
#define HTLIB_H

#include "HTUser.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Initializing and Terminating libwww
.

These two functions initialize memory and settings for the libwww core and
cleans up memory kept by libwww core when exiting the application. They
&nbsp;MUST be used as libwww otherwise is in a undefined state.
*/
extern BOOL HTLibInit (const char * AppName, const char * AppVersion);
extern BOOL HTLibTerminate (void);

/*
.
  Libwww Name and Version
.

You can get the generic name of the Library and the version by using the
following functions:
*/
extern const char * HTLib_name (void);
extern const char * HTLib_version (void);

/*
.
  Is libwww Initalized?
.

Returns YES or NO
*/
extern BOOL HTLib_isInitialized (void);

/*
.
  Application Name and Version
.

Returns the name of the application name and the version number. The name
and version SHOULD be short and to the point. They MUST NOT be used for
advertising or other non-essential information. Although any token character
MAY appear in a product-version, this token SHOULD only be used for a version
identifier.
(
  Set or get the application name
)

The name can not contain any spaces as it is used as part of the user agent
field. For the same reason, the string MUST be in US-ASCII. For example,
"SmartApp"
*/

extern const char * HTLib_appName (void);
extern BOOL HTLib_setAppName (const char * name);

/*
(
  Set or get the application version
)

The version token can not contain any spaces as it is used as part of the
user agent field. For the same reason, the string MUST be in US-ASCII. For
example, "1.0". 
*/

extern const char * HTLib_appVersion (void);
extern BOOL HTLib_setAppVersion (const char * version);

/*
.
  Accessing the Local File System
.

Libwww does normally use the local file system for dumping unknown data objects,
file cache etc. In some situations this is not desired, and we can therefore
turn it off. This mode also prevents you from being able to access other
resources where you have to login telnet, for example.
*/
extern BOOL HTLib_secure (void);
extern void HTLib_setSecure (BOOL mode);

/*
.
  Default User Profile
.

The default user profile is automatically created
by the libwww in order to get information about the hostname, default
email -address etc. All request objects will
be created with this default user profile. The application may assign individual
user profiles to every request object or may set the default user profile.
*/
extern HTUserProfile * HTLib_userProfile (void);
extern BOOL HTLib_setUserProfile (HTUserProfile * up);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTLIB_H */

/*

  

  @(#) $Id$

*/
