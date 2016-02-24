/*

  					W3C Sample Code Library libwww Timer Class


!
  The Timer Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Timer class handles timers for libwww and the application. This works
exactly as in X where you create a timer object with a callback function
and a timeout. The callback will be called every time the timer expires.
There are several timeouts that can be set in libwww:

  o 
	     The time we keep an idle persistent connection open. Here there are in fact
	     two mechanisms depending on whether you use blocking or non-blocking sockets.
	     The default is 60 secs. The timers can be accessed using the functions defined
	     in the HTHost object
  o 
	     The idle time we want to wait when receiving a response from a server, that
	     is, if it doesn't send anything in a number of secs. The default here is
	     no timeout. It can be accessed in the
	     HTHost object as well.
  o 
	     The timeout before we flush pending requests in a pipeline, the default here
	     is 30 ms. It is also accessed in the HTHost
    object
  o 
	     The timeout before we start sending the body of a PUT or
    POST request. Normally we send the Expect: 100-continue
    header field but if the server doesn't send back a 100 Continue
    code then we upload the body anyway. The default is 2 secs and can be accessed
    in the HTTP module.

*/

#ifndef HTTIMER_H
#define HTTIMER_H

#include "wwwsys.h"
#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTTimer HTTimer;

typedef int HTTimerCallback (HTTimer *, void *, HTEventType type);

/*
.
  Create and Delete Timers
.

The callback function is the function that is to be called when timer expires.
*/

extern HTTimer * HTTimer_new (HTTimer *, HTTimerCallback *, 
			      void *, ms_t millis,
                              BOOL relative, BOOL repetitive);
extern BOOL HTTimer_delete (HTTimer * timer);
extern BOOL HTTimer_deleteAll (void);
extern BOOL HTTimer_expireAll (void);

/*
.
  Dispatch Timer
.

Just do it
*/

extern int HTTimer_dispatch (HTTimer * timer);

/*
(
  Get the next timer in line
)

Dispatches all expired timers and optionally returns the time till the next
one.
*/

extern int HTTimer_next (ms_t * pSoonest);

/*
(
  Reset an already existing Repetitive Timer
)
*/

extern BOOL HTTimer_refresh(HTTimer * timer, ms_t now);

/*
.
  Get Information about a Timer Object
.
(
  Absolute Time when This Timer Expires
)

Absolute time in millies when this timer will expire
*/

extern ms_t HTTimer_expiresAbsolute (HTTimer * timer);

/*
(
  Relative Time this Timer is running
)

Gived the relative time in millies that this timer was registered with. For
example, a relative timer set to expire in 20ms will return 20.
*/

#define HTTimer_getTime(t)	HTTimer_expiresRelative(t)
extern ms_t HTTimer_expiresRelative (HTTimer * timer);

/*
(
  Has this Timer Expired?
)

If so then it's time to call the dispatcher!
*/

extern BOOL HTTimer_hasTimerExpired (HTTimer * timer);

/*
(
  What callback is this Timer Registered with?
)
*/

extern HTTimerCallback * HTTimer_callback (HTTimer * timer);

/*
(
  Is this Time relative or Absolute?
)
*/

extern BOOL HTTimer_isRelative (HTTimer * timer);

/*
.
  Platform Specific Timers
.

On some platform, timers are supported via events or other OS specific
mechanisms. You can make libwww can support these by registering a platform
specific timer add and timer delete method.
*/

typedef BOOL HTTimerSetCallback (HTTimer * timer);

extern BOOL HTTimer_registerSetTimerCallback (HTTimerSetCallback * cbf);
extern BOOL HTTimer_registerDeleteTimerCallback (HTTimerSetCallback * cbf);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTTIMER_H */

/*

  

  @(#) $Id$

*/
