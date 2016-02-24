/*

  					W3C Sample Code Library libwww Event Class


!
  The Event Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Event Class defines any event manager to be used by libwww for handling
events. An event is not strictly defined as it is highly platform
dependent and hence out of scope for the Library. If you are using the libwww
pseudo threads on Unix then an event is when the&nbsp;select() system
call returns a notification on&nbsp;a socket descriptor, but it may as well
be an asynchronous event from the windows manager etc. If your application
is not using anything but traditional blocking sockets then you do not need
an event manager at all. In that case, libwww will block on any socket or
system call until the process can proceed.

The libwww interface to an event manager is very simple as it consists of
registering a socket descriptor, the location in the
program, and the current state when an operation (for example
read) would block. When the event manager at a later point in
time gets a notification that the socket has become ready, it can then call
libwww with the state saved from the registration and libwww can continue.
Second, libwww must be able to unregister a socket when it is not
anymore in a state where it can block. Only in case the application
wishes to use non-blocking sockets it should register methods for
handling the registration process as described below.

Note: The library core does not define any event manager
- it is considered part of the application. The library comes with a
default event manager which can be initiated
using the function HTEventInit() in HTInit
module

This module is implemented by HTEvent.c, and it is
a part of the W3C Sample Code Library.
*/

#ifndef HTEVENT_H
#define HTEVENT_H
#include "wwwsys.h"

#ifdef __cplusplus
extern "C" { 
#endif 

#ifdef IN_EVENT
typedef struct _HTTimer HTTimer;
#endif

typedef enum _HTPriority {
    HT_PRIORITY_INV = -1,
    HT_PRIORITY_OFF = 0,
    HT_PRIORITY_MIN = 1,
    HT_PRIORITY_MAX = 20
} HTPriority; 

#define HTEVENT_INDEX 0x10
typedef enum {
#ifdef WWW_WIN_ASYNC
    HTEvent_READ    = (0x001 | 0 << HTEVENT_INDEX),
    HTEvent_WRITE   = (0x002 | 1 << HTEVENT_INDEX),
    HTEvent_OOB     = (0x004 | 2 << HTEVENT_INDEX),
    HTEvent_ACCEPT  = (0x008 | 3 << HTEVENT_INDEX),
    HTEvent_CONNECT = (0x010 | 4 << HTEVENT_INDEX),
    HTEvent_CLOSE   = (0x020 | 5 << HTEVENT_INDEX),
    HTEvent_TYPES   = 6,	/* winsock has seperate events for all of these */
#define HTEVENT_TYPES	6 /* use in constructing the fake event below */
#else /* WWW_WIN_ASYNC */
    HTEvent_READ    = (0x001 | 0 << HTEVENT_INDEX),
    HTEvent_ACCEPT  = (0x002 | 0 << HTEVENT_INDEX),
    HTEvent_CLOSE   = (0x004 | 0 << HTEVENT_INDEX),
    HTEvent_WRITE   = (0x008 | 1 << HTEVENT_INDEX),
    HTEvent_CONNECT = (0x010 | 1 << HTEVENT_INDEX),
    HTEvent_OOB     = (0x020 | 2 << HTEVENT_INDEX),
    HTEvent_TYPES   = 3,	/* only READ, WRITE, and OOB are real types */
#define HTEVENT_TYPES	3 /* use in constructing the fake event below */
#endif /* !WWW_WIN_ASYNC */
    /*
    **	fake events - these don't correspond to event manager events, but they
    **	are usefull for communicating with the protocol modules
    */
    HTEvent_TIMEOUT = (0x040 | HTEVENT_TYPES << HTEVENT_INDEX),
    HTEvent_BEGIN   = (0x000 | HTEVENT_TYPES << HTEVENT_INDEX),
    HTEvent_END     = (0x080 | HTEVENT_TYPES << HTEVENT_INDEX),
    HTEvent_FLUSH   = (0x100 | HTEVENT_TYPES << HTEVENT_INDEX),
    HTEvent_RESET   = (0x200 | HTEVENT_TYPES << HTEVENT_INDEX),
    HTEvent_ALL     = 0xFFFF
} HTEventType;

#define HTEvent_BITS(type) (type & 0xFFFF)
#define HTEvent_INDEX(type) (type >> HTEVENT_INDEX)

#define HT_EVENT_INITIALIZER \
    {HTEvent_READ, "HTEvent_READ"}, \
    {HTEvent_ACCEPT, "HTEvent_ACCEPT"}, \
    {HTEvent_CLOSE, "HTEvent_CLOSE"}, \
    {HTEvent_WRITE, "HTEvent_WRITE"}, \
    {HTEvent_CONNECT, "HTEvent_CONNECT"}, \
    {HTEvent_OOB, "HTEvent_OOB"}, \
    {HTEvent_TIMEOUT, "HTEvent_TIMEOUT"}, \
    {HTEvent_BEGIN, "HTEvent_BEGIN"}, \
    {HTEvent_END, "HTEvent_END"}, \
    {HTEvent_FLUSH, "HTEvent_FLUSH"}, \
    {HTEvent_RESET, "HTEvent_RESET"}

extern char * HTEvent_type2str(HTEventType type);

/*
.
  Event Handlers
.

A location is a function that can be registered by the event manager
and called at a later point in time in order to continue an operation. All
locations must be of type &nbsp;HTEventCallback as defined here:
*/

typedef int HTEventCallback (SOCKET, void *, HTEventType);
typedef struct _HTEvent HTEvent;

/* Avoid circular include for HTReq->HTNet->HTHost: HTEvent blah */
#include "HTReq.h"

/*

There are many default event handlers provided with the Library. For example,
all the protocol modules such as the HTTP client module
are implemented as event handlers. In stead of using blocking sockets, this
allows a protocol module to register itself when performing an operation
that would block. When the sockets becomes ready the handler is called with
th socket in question, the request object, and the socket operation &nbsp;
.
  Registering and Unregistering Events
.

As mentioned above, the only interface libwww requires from an event manager
is a method to register an event when an operation would block and
unregister it when the operation has completed The library registers
and unregisters events by calling the following two functions:
*/

extern int HTEvent_register	(SOCKET, HTEventType, HTEvent *);
extern int HTEvent_unregister	(SOCKET, HTEventType);

/*

The register function contains information about which socket we are waiting
on to get ready and which operation we are waiting for (read, write, etc.),
the request object containing the current request, the event handler that
we want to be called when the socket becomes reasy, and finally the priority
by which we want the thread to be processed by the event manager. Likewise,
libwww can unregister a operation on a socket which means that libwww is
no longer waiting for this actiion to become ready.
.
  Registering an Event Manager
.

Libwww core does not contain any event manager as it depends on whether you
want to use pseudo threads no threads, or real threads. Instead, libwww comes
with a default implementation that you may register,
but you may as well implement and register your own. The register and unregister
functions above actually does nothing than looking for a registered event
manager and then passes the call on to that. You register your own event
manager by using the methods below:
*/

typedef int HTEvent_registerCallback(SOCKET, HTEventType, HTEvent *);
typedef int HTEvent_unregisterCallback(SOCKET, HTEventType);

extern void HTEvent_setRegisterCallback(HTEvent_registerCallback *);
extern void HTEvent_setUnregisterCallback(HTEvent_unregisterCallback *);

/*
(
  Has Register and Unregister Callbacks been setup?
)

Replies YES if both an HTEvent_setRegisterCallback and
HTEvent_setUnregisterCallback have been called with non NULL callbacks.
*/

extern BOOL HTEvent_isCallbacksRegistered(void);

/*
.
  Create and Delete Events
.
*/

extern HTEvent * HTEvent_new (HTEventCallback * cbf, void * context,
			      HTPriority pritority, int timeoutInMillis);
extern BOOL HTEvent_delete (HTEvent * event);

/*
(
  Event Timeouts, Priorities, Callbacks, and Contexts
)

Normally, these are set when creating the event.
*/

extern BOOL HTEvent_setParam(HTEvent * event, void * param);
extern BOOL HTEvent_setPriority(HTEvent * event, HTPriority priority);
extern BOOL HTEvent_setTimeout(HTEvent * event, int timeoutInMillis);
extern BOOL HTEvent_setCallback(HTEvent * event, HTEventCallback * cbf);

/*
.
  The Raw Event Type
.

Don't use this directly, use the methods above instead.
*/

struct _HTEvent {
    HTPriority		priority;	 /* Priority of this request (event) */
    int                 millis;              /* Timeout in ms for this event */
#ifdef IN_EVENT
    HTTimer *		timer;
#endif
    HTEventCallback *	cbf;			   /* Protocol state machine */
    void *		param;		       /* HTEvent_register parameter */
    HTRequest *		request;
};

/*

You can register the event manager provided together with libwww by using
the HTEventInit() in the HTInit module
*/

#ifdef __cplusplus
}
#endif

#endif /* HTEVENT_H */

/*

  

  @(#) $Id$

*/
