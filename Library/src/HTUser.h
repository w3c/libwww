/*

  
  					W3C Sample Code Library libwww User Profile Class


!
  The User Profile Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The User profile&nbsp;class manages what we know about a user on this
host. This can for example be the FQDN of the host, the user's email
address, the time zone, the news server etc. Note that this information does
not correspond to the actual information for the host but instead represents
"the information that the user wants to show the world". The user may use
an arbitrary email address to be used in a HTTP
request, for example. The application may assign a context to each use
which gives the application to extend the use of this class.

This module is implemented by HTUser.c, and it is
a part of the W3C Sample Code
Library.
*/

#ifndef HTUSER_H
#define HTUSER_H

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTUserProfile HTUserProfile;

/*
.
  Creation and Deletion Methods
.

The application may create any number of user profile objects. By default
the Library creates a generic user which is the default value used to initialize
a Request object. This can be replaced by other
user profiles at any point in time.
(
  Create a User Profile
)
*/
extern HTUserProfile * HTUserProfile_new (const char * name, void * context);

/*
(
  Localize a User Profile
)

Localize a user profile by filling in all the information that we can figure
out automatically, for example the email address, news server etc.
*/
extern BOOL HTUserProfile_localize (HTUserProfile * up);

/*
(
  Delete a User Profile
)
*/
extern BOOL HTUserProfile_delete (HTUserProfile * up);

/*
.
  User Profile Class Methods
.
(
  Fully Qualified Domain Name (FQDN)
)

The FQDN is a fully qualified domain name in that it contains both a local
host name and the domain name. It turns out that this is in fact very difficult
to obtain a FQDN on a variety of platforms.
*/

extern char * HTUserProfile_fqdn (HTUserProfile * up);
extern BOOL HTUserProfile_setFqdn (HTUserProfile * up, const char * fqdn);

/*
(
  User Email Address
)

This is the email address that the user wants to send out for example as
a "password" when using anonymous FTP access and
as a "From" field in a HTTP request.
*/

extern char * HTUserProfile_email (HTUserProfile * up);
extern BOOL HTUserProfile_setEmail (HTUserProfile * up, const char * email);

/*
(
  News Server
)

Control the news server that this user wishes to use
*/

extern char * HTUserProfile_news (HTUserProfile * host);
extern BOOL HTUserProfile_setNews (HTUserProfile * host, const char * news);

/*
(
  Location of Temporary Files
)

Control the location for temporary files for this profile. The format
must be in URL format which is different from local file syntax as
URL syntaz always uses '/' as delimiters and also encoding of special
characters. See the
documentation on URLs
for more information about URL syntax.
*/

extern char * HTUserProfile_tmp (HTUserProfile * host);
extern BOOL HTUserProfile_setTmp (HTUserProfile * host, const char * tmp);

/*
(
  Local Time Zone (in seconds)
)

Another widely used piece information that is very hard toobtain is the local
time zone. As we often must convert to and from GMT (Universal Time) we must
have the correct time zone. If we for some reason guesses wrong then the
user must change it manually.
*/

extern time_t HTUserProfile_timezone (HTUserProfile * up);
extern BOOL HTUserProfile_setTimezone (HTUserProfile * up, time_t timezone);

/*
(
  User Profile Context
)

The applicatoin may have additional information that it wishes to assign
to a user profile. It can do this using the user context which is handled
as follows:
*/

extern void * HTUserProfile_context (HTUserProfile * up);
extern BOOL HTUserProfile_setContext (HTUserProfile * up, void * context);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTUser_H */

/*

  

  @(#) $Id$

*/
