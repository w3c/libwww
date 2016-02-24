/*

  					W3C Sample Code Library libwww Generic Network Communication


!
  Generic Network Communication
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module has the common code for handling typical Internet functions like
getting the name of the local host, getting the domain name and email address
of user, parsing error codes returned in errno or equivalent
etc.

This module is implemented by HTInet.c, and it is
a part of the W3C Sample Code Library.
*/

#ifndef HTINET_H
#define HTINET_H
#include "HTReq.h"
#include "HTHstMan.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
(
  System Description of Error Message
)

Return error message corresponding to errno number given. We need to pass
the error number as a parameter as we on some platforms get different codes
from sockets and local file access.
*/

extern char * HTErrnoString	(int errnum);
extern int HTInetStatus		(int errnum, char * where);

/*
(
  Parse a Cardinal Value
)

Converts a string to a cardinal value. On entry: *pp points to first character
to be interpreted, terminated by non 0..9 character. *pstatus points to status
already valid, maxvalue gives the largest allowable value. On exit: *pp points
to first unread character, *pstatus points to status updated iff bad
*/

extern unsigned int HTCardinal (int *		pstatus,
				char **		pp,
				unsigned int	max_value);

/*
(
  Produce a string for an internet address
)

This function is equivalent to the BSD system call inet_ntoa in that
it converts a numeric 32-bit IP-address to a dotted-notation decimal string.
The pointer returned points to static memory which must be copied if it is
to be kept.
*/

extern const char * HTInetString (struct sockaddr_in * sin);

/*
(
  Parse a network node address and port
)

It is assumed that any portnumber and numeric host address is given in decimal
notation. Separation character is '.' Any port number given in host name
overrides all other values. 'host' might be modified.
*/

extern int HTParseInet (HTHost * host, char * hostname, HTRequest * request);

/*
(
  Timezone Offset
)

Calculates the offset from GMT in seconds.
*/

extern time_t HTGetTimeZoneOffset (void);

/*
(
  Get Time of day in Milli Seconds
)

Return the time of day in milli seconds.
*/
extern ms_t HTGetTimeInMillis (void);

/*
(
  FQDN of this Host
)

This function returns a the name of this host or NULL if not available. The
name is stored in a static variable.
*/

extern char * HTGetHostName (void);

/*
(
  User Email Address
)

This functions returns a char pointer to a static location containing the
mail address of the current user. The static location is different from the
one of the current host name so different values can be assigned. The default
value is <USER>@hostname where hostname is as returned
by HTGetHostName().
*/

#ifndef HT_DEFAULT_LOGIN
#define HT_DEFAULT_LOGIN	"libwww"
#endif

extern char * HTGetMailAddress (void);

/*
(
  News server
)

The default news host is "news" but you can get ans set the value here.
*/
extern char * HTGetNewsServer (void);

/*
(
  Get a Temporary File Name
)

HTGetTmpFileName() allows the user to control the choice of a directory.
The argument dir points to the name of the directory in which the file is
to be created. This is equivalent to tempnam() function.
*/

extern char * HTGetTmpFileName (const char * dir);

/*
(
  Signal Handling
)

This is only necessary to compile on a few platforms and only if the application
does not have its own signal handling. It is required on Solaris 2.3 (and
other SVR4 platforms?) due to a bug in the TCP kernel. When a
connect() is tried to a illegal port, solaris gives a SIGPIPE
signal instead of returning Connection refused.
*/

#ifdef WWWLIB_SIG
extern void HTSetSignal (void);
#endif

#ifdef __cplusplus
}
#endif

#endif   /* HTINET_H */

/*

  

  @(#) $Id$

*/
