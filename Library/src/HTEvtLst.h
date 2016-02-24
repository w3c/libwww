/*

  
  					W3C Sample Code Library libwww Default Event Manager


!
  Default Event Manager
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module provides a default event registry and an eventloop which may
be used by an application if desired. See also the libwww timers defined
in the HTTimer module. An application may use
this module for:
	 
	   o 
	     Eventloop and registry - Application registers
    HTEvntrg_register and HTEvntrg_unregister and calls
    HTEventList_loop to dispatch events as they occur.
  o 
	     Event registry - Application just registers its own
    event handlers and chains them to
    HTEvntrg_register and HTEvntrg_unregister. When the
    application's eventloop gets activity on a socket, it calls
    HTEvent_dispatch to handle it.
  o 
	     Nothing - Application registers its own
    event handler uses its own eventloop
    to dispatch those events.

	 
This module is implemented by HTEvtLst.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTEVTLST_H
#define HTEVTLST_H

#include "wwwsys.h"
#include "HTEvent.h"
#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 


/*
(
  Windows Specific Handles
)
*/

#if defined(WWW_WIN_ASYNC) || defined(WWW_WIN_DLL)
extern BOOL HTEventList_winHandle (HTRequest * request);
extern BOOL HTEventList_setWinHandle (HWND window, unsigned long message);
extern HWND HTEventList_getWinHandle (unsigned long * pMessage);
extern LRESULT CALLBACK AsyncWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

/*
.
  Event Registry
.

The libwww event registry binds a socket and operation (FD_READ,
FD_WRITE, ...) to a callback function. Events are registered,
unregistered, and dispatched as they come in.
(
  Register an Event Handler
)

For a given socket, reqister a request structure, a set of operations, a
HTEventCallback function, and a priority. For this implementation, we allow
only a single HTEventCallback function for all operations. and the priority
field is ignored.
*/

extern HTEvent_registerCallback HTEventList_register;

/*
(
  Unregister an Event Handler
)

Remove the registered information for the specified socket for the actions
specified in ops. if no actions remain after the unregister, the registered
info is deleted, and, if the socket has been registered for notification,
the HTEventCallback will be invoked.
*/

extern HTEvent_unregisterCallback HTEventList_unregister;

/*
(
  Unregister ALL Event Handlers
)

Unregister all sockets. N.B. we just remove them for our internal data
structures: it is up to the application to actually close the socket.
*/

extern int HTEventList_unregisterAll (void);

/*
(
  Lookup and Dispatch Event Handlers
)

Callbacks can be looked up or dispatched based on the socket and operation
(read/write/oob)
*/

extern int HTEventList_dispatch (SOCKET s, HTEventType type, ms_t now);
extern HTEvent * HTEventList_lookup (SOCKET s, HTEventType type);

/*
.
  Libwww Default EventLoop
.

The libwww default eventloop dispatches events to the event
registry.
(
  Start and Stop the Event Manager
)
*/

extern BOOL HTEventInit (void);
extern BOOL HTEventTerminate (void);

/*
(
  Start the Eventloop
)

That is, we wait for activity from one of our registered channels, and dispatch
on that. Under Windows/NT, we must treat the console and sockets as distinct.
That means we can't avoid a busy wait, but we do our best.
*/

extern int HTEventList_newLoop (void);

/*

The next version is an old version of the eventloop start. The request is
not used for anything and can be NULL.
*/

extern int HTEventList_loop (HTRequest * request);

/*
(
  Stop the Eventloop
)

Stops the (select based) eventloop immediately. The function does not guarantee
that all requests have terminated so it is important that the application
does this before this function is called. This can be done using the
HTNet_isIdle() function in the HTNet Module
*/

extern void HTEventList_stopLoop (void);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTEVTLST_H */

/*

  

  @(#) $Id$

*/
