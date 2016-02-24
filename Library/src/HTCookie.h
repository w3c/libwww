/*

  					W3C Sample Code Library libwww Simple Cookie Handler


!
  Simple Cookie Handler
!
*/

/*
**	(c) COPYRIGHT MIT 1999.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module provides a simple
HTTP Cookie
handling mechanism. It really also is an excersize in showing how libwww
can be extended with something like cookies in a modular manner. An important
thing to note about this implementation is that it does not provide
storage for cookies - this is left to the application as normally cookies
have to be kept under lock.

This module is implemented by HTCookie.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTCOOKIE_H
#define HTCOOKIE_H
#include "WWWLib.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Initiate and Terminate Cookie Handling
.

In order to start handling cookies, you need to initiate the module. Likewise,
when you are done, you must terminate the module in order to clean up memory
etc. Note that this cookie handler doesn't provide storage room for cookies
- the application MUST do that by registering cookie
callbacks. Initiation does three things:

  o 
	     Register a MIME header parser to parse the Set-Cookie
    header field
  o 
	     Register a BEFORE filter to see if a cookie should
    be added to the request (uses the "set" callback)
  o 
	     Register an AFTER filter to handle new cookies (uses
    the "find" callback)

*/

extern BOOL HTCookie_init (void);
extern BOOL HTCookie_terminate (void);

/*
.
  The Cookie Class
.

The cookie class is used to handle cookies in libwww and to hand them off
to the application. The creation and deletion of cookie object is handled
by this cookie module - the application is handed a cookie and can access
the elements using the following methods:
*/

typedef struct _HTCookie HTCookie;

/*
(
  Cookie Name
)
*/

extern BOOL HTCookie_setName (HTCookie * me, const char * name);
extern char * HTCookie_name (HTCookie * me);

/*
(
  Cookie Value
)
*/

extern BOOL HTCookie_setValue (HTCookie * me, const char * value);
extern char * HTCookie_value (HTCookie * me);

/*
(
  Cookie Domain
)
*/

extern BOOL HTCookie_setDomain (HTCookie * me, const char * domain);
extern char * HTCookie_domain (HTCookie * me);

/*
(
  Cookie Path
)
*/

extern BOOL HTCookie_setPath (HTCookie * me, const char * path);
extern char * HTCookie_path (HTCookie * me);

/*
(
  Cookie Expiration
)
*/

extern time_t HTCookie_setExpiration (HTCookie * me, time_t expiration);
extern time_t HTCookie_expiration (HTCookie * me);

/*
(
  Is Cookie Secure?
)
*/

extern time_t HTCookie_setSecure (HTCookie * me, BOOL secure);
extern BOOL HTCookie_isSecure (HTCookie * me);

/*
.
  Cookie Callbacks
.

The cookie callbacks are called before the request is shipped over the wire
to see if any cookies should be included and after the response has been
recieved if a new cookie is found in a response and before. Cookie callbacks
can be registered with a context that is sent along with the callback when
called.
*/

typedef BOOL HTCookieSetCallback (HTRequest * request, HTCookie * cookie, void * param);
typedef HTAssocList * HTCookieFindCallback (HTRequest * request, void * param);

extern BOOL HTCookie_setCallbacks (HTCookieSetCallback *	setCookie,
				   void * 			setCookieContext,
				   HTCookieFindCallback *	findCookie,
				   void * 			findCookieContext);
extern BOOL HTCookie_deleteCallbacks (void);

/*
.
  Cookie Handling Mode
.

The application can decide how cookies are to be handled - should they be
ignored, should the user be asked, etc.
*/

typedef enum _HTCookieMode {
    HT_COOKIE_ACCEPT          = 0x1,  /* Accept cookies */
    HT_COOKIE_SEND            = 0x2,  /* Send cookies when fit */
    HT_COOKIE_SAME_HOST       = 0x4,  /* Don't accept cookies for other hosts */
    HT_COOKIE_SAME_DOMAIN     = 0x8,  /* Don't accept cookies for other domains */
    HT_COOKIE_PROMPT          = 0x10  /* Prompt before accepting cookies */
} HTCookieMode;

extern BOOL HTCookie_setCookieMode (HTCookieMode mode);
extern HTCookieMode HTCookie_cookieMode (void);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTCOOKIE_H */

/*

  

  @(#) $Id$

*/
