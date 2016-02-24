/*

  					W3C Sample Code Library libwww HTNet Class


!
  The Net Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Net class manages information related to a "thread" in libwww. As libwww
threads are not really threads but a notion of using interleaved, non-blocking
I/O for accessing data objects from the network (or local file system), they
can be used on any platform with or without support for native threads. In
the case where you have an application using real threads the Net class is
simply a object maintaining links to all other objects involved in serving
the request. If you are using the libwww pseudo threads then the Net object
contains enough information to stop and start a request based on which BSD
sockets are ready. In practise this is of course transparent to the application
- this is just to explain the difference.

When a Request object is passed to the Library ,
the core creates a new HTNet object per channel
used by the request. In many cases a request only uses a single
channel object but, for example, FTP requests use
at least two - one for the control connection and one for the data connection.

You can find more information about the libwww pseudo thread model in the
 Multithread Specifications.

This module is implemented by HTNet.c, and it is a
part of the W3C Sample Code Library.
*/

#ifndef HTNET_H
#define HTNET_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*

The HTNet object is the core of the request queue management.
This object contains information about the socket descriptor, the input read
buffer etc. required to identify and service a request.
*/

typedef struct _HTNet HTNet;

#include "HTEvent.h"
#include "HTReq.h"
#include "HTResponse.h"
#include "HTTrans.h"
#include "HTHost.h"
#include "HTProt.h"
#include "HTChannl.h"
#include "HTDNS.h"

/*
.
  Generic BEFORE and AFTER Filter Management
.

Filter functions can be registered to be called before and
after a request has either been started or has terminated. The
conditions for BEFORE and AFTER filters are not the same, and so
we maintain them independently. Filters can be registered globally or locally.
The global filters are registered directly by the Net Object (this module)
and the local filters are registered by the
HTRequest Object. However, both &nbsp;local and
global filters use the same regisration mechanism which we provide here.
(
  Filter Ordering
)

Filters can be registered by anyone and as they are an often used mechanism
for introducing extensions in libwww, they are videly used to handle
authentication, redirection, etc. Many filters can be registered at once
and not all of the filters may know about the other filters. Therefore, it
is difficult to specify an absolute ordering by which the filters should
be called. Instead you can decide a relative order by which the filters should
be called. The order works pretty much like the Unix priority mechanism running
from HT_FILTER_FIRST to HT_FILTER_LAST having
HT_FILTER_MIDDLE being the "normal" case.
*/

typedef enum _HTFilterOrder {
    HT_FILTER_FIRST	= 0x0,		/*     0 */
    HT_FILTER_EARLY	= 0x3FFF,	/* 16383 */
    HT_FILTER_MIDDLE	= 0x7FFF,	/* 32767 */
    HT_FILTER_LATE	= 0xBFFE,	/* 49150 */
    HT_FILTER_LAST	= 0xFFFF	/* 65535 */
} HTFilterOrder;

/*

In case multiple filters are registered with the same order then they are
called in the inverse order they were registered.&nbsp;
(
  Filter URL Templates
)

Both BEFORE and AFTER filters can be registered with a URL
template in which case they are only called when the Request URL
matches the template. A template is simply a string which is matched against
the Request URL. The string can be terminated by a&nbsp;single
"*" in which case all strings matching the template up til the
"*" is considered a match. A template can be as short as the access scheme
which enmables a filter for a specific access method only, for example
"http//<star>".
(
  BEFORE Filters
)

A BEFORE filter is called whenever we issue a request and they have
been selected by the execution procedure. BEFORE filters are registered
with a context and a filter order by which they are to be called
and a URL template which may be NULL. In this case, the filter is
called on every request. The mode can be used by anybody to pass an extra
parameter to a filter. This is not really OO thinking - but hey - this is
C ;-)
*/
typedef int HTNetBefore (HTRequest * request, void * param, int mode);

/*

You can add a BEFORE filter in the list provided by the caller. Several
filters can be registered in which case they are called with the filter ordering
in mind.
*/

extern BOOL HTNetCall_addBefore (HTList * list, HTNetBefore * before,
				 const char * tmplate, void * param,
                                 HTFilterOrder order);

/*

You can also unregister all instances of a BEFORE filter from a list using
the following function
*/

extern BOOL HTNetCall_deleteBefore (HTList * list, HTNetBefore * before);

/*

You get rid of all BEFORE filters using this function
*/

extern BOOL HTNetCall_deleteBeforeAll (HTList * list);

/*

The BEFORE filters are expected and called if appropriate every time we issue
a new request. Libwww calls the BEFORE filters in the order specified at
registration time. If a filter returns other than HT_OK then stop and return
immediately. Otherwise return what the last filter returns.
*/

extern int HTNetCall_executeBefore (HTList * list, HTRequest * request);

/*
(
  AFTER Filters
)

An AFTER filter is called whenever we have terminated a request. That
is, on the way out from the protocol module and
back to the application. AFTER filters are registered with a
context, a status, a filter order by which they are
to be called and a URL template which may be NULL. The status of the
request may determine which filter to call. The set of possible values are
given below. An AFTER filter can be registered to handle one or more
of the codes.

  
    HT_ERROR
  
    An error occured
  
    HT_LOADED
  
    The document was loaded
  
    HT_NO_DATA
  
    OK, but no data
  
    HT_NO_ACCESS
  
    The request could not be succeeded due to lack of credentials
  
    HT_NO_PROXY_ACCESS
  
    The request could not be succeeded due to lack of credentials for accessing
    an intermediate proxy
  
    HT_RETRY
  
    Retry request after at a later time
  
    HT_PERM_REDIRECT
  
    The request has been permanently redirected and we send back the new URL
  
    HT_TEMP_REDIRECT
  
    The request has been temporarily redirected and we send back the new URL
  
    HT_ALL
  
    All of above


A Protocol module can also, in certain cases, return a HT_IGNORE
in which case no filters are called
*/

typedef int HTNetAfter (HTRequest * request, HTResponse * response,
                        void * param, int status);

/*

You can register a AFTER filter in the list provided by the caller. Several
filters can be registered in which case they are called with the filter ordering
in mind.
*/

extern BOOL HTNetCall_addAfter (HTList * list, HTNetAfter * after,
				const char * tmplate, void * param,
				int status, HTFilterOrder order);

/*

You can either unregister all filters registered for a given status using
this function or the filter for all status codes.
*/

extern BOOL HTNetCall_deleteAfter (HTList * list, HTNetAfter * after);
extern BOOL HTNetCall_deleteAfterStatus (HTList * list, int status);

/*

You can also delete all AFTER filters in list
*/

extern BOOL HTNetCall_deleteAfterAll (HTList * list);

/*

This function calls all the AFTER filters in the order specified at registration
time and if it has the right status code and it's not HT_IGNORE.
We also check for any template and whether it matches or not. If a filter
returns other than HT_OK then stop and return immediately. Otherwise return
what the last filter returns.
*/

extern int HTNetCall_executeAfter (HTList * list, HTRequest * request,
				   int status);

/*
.
  Global BEFORE and AFTER Filter Management
.

Global filters are inspected on every request (they do not have to be called
- only if the conditions match). You can also register filters locally in
the Request object.

  Global BEFORE Filters


These are the methods to handle global BEFORE Filters.
*/

extern BOOL HTNet_setBefore (HTList * list);

extern HTList * HTNet_before (void);

extern BOOL HTNet_addBefore (HTNetBefore * before, const char * tmplate,
			     void * param, HTFilterOrder order);

extern BOOL HTNet_deleteBefore (HTNetBefore * before);

/*

You can call both the local and the global BEFORE filters (if any)
*/

extern int HTNet_executeBeforeAll (HTRequest * request);

/*

  Global AFTER Filters


These are the methods to handle global AFTER Filters.
*/

extern BOOL HTNet_setAfter (HTList * list);

extern HTList * HTNet_after (void);

extern BOOL HTNet_addAfter (HTNetAfter * after, const char * tmplate,
			    void * param, int status,
                            HTFilterOrder order);

extern BOOL HTNet_deleteAfter (HTNetAfter * after);

extern BOOL HTNet_deleteAfterStatus (int status);

/*

You can call both the local and the global AFTER filters (if any)
*/

extern int HTNet_executeAfterAll (HTRequest * request, int status);

/*
.
  Socket Resource Management
.

The request queue ensures that no more than a fixed number of TCP connections
are open at the same time. If more requests are handed to the Library, they
are put into the pending queue and initiated when sockets become free.
(
  Number of Simultanous open TCP connections
)

Set the max number of simultanous sockets. The default value is HT_MAX_SOCKETS
which is 6. The number of persistent connections depend on this value as
a deadlock can occur if all available sockets a persistent (see the
DNS Manager for more information on setting the
number of persistent connections). The number of persistent connections can
never be more than the max number of sockets-2, so letting newmax=2 prevents
persistent sockets.
*/

extern BOOL HTNet_setMaxSocket (int newmax);
extern int  HTNet_maxSocket (void);

/*
(
  Socket Counters
)
*/

extern void HTNet_increaseSocket (void);
extern void HTNet_decreaseSocket (void);

extern int HTNet_availableSockets (void);

/*
(
  Persistent Socket Counters
)
*/

extern void HTNet_increasePersistentSocket (void);
extern void HTNet_decreasePersistentSocket (void);

extern int HTNet_availablePersistentSockets (void);

/*
(
  Any Ongoing Connections?
)

Returns whether there are active requests. Idle persistent sockets do not
count as active.
*/

extern BOOL HTNet_isIdle (void);

/*
(
  List Active Queue
)

Returns the list of active requests that are currently having an open connection.
Returns list of HTNet objects or NULL if error.
*/

extern HTList *HTNet_activeQueue (void);
extern BOOL HTNet_idle (void);

/*
(
  Are we Active?
)

We have some small functions that tell whether there are registered requests
in the Net manager. There are tree queues: The active, the
pending, and the persistent. The active queue
is the set of requests that are actively sending or receiving data. The
pending is the requests that we have registered but which are waiting
for a free socket. The Persistent queue are requets that are waiting
to use the same socket in order to save network resoures (if the server
understands persistent connections).

  Active Requests?


Returns whether there are requests in the active queue or not
*/

extern BOOL HTNet_idle (void);

/*

  Registered Requests?


Returns whether there are requests registered in any of the lists or not
*/

extern BOOL HTNet_isEmpty (void);
extern int HTNet_count (void);

/*
(
  List Pending Queue
)

Returns the list of pending requests that are waiting to become active. Returns
list of HTNet objects or NULL if error
*/

extern HTList *HTNet_pendingQueue (void);

/*
.
  Creation and Deletion Methods
.

The Net object is intended to live as long as the request is still active.
In that regard it is very similar to the Request Object
. However, the main difference is that a Net object represents a "thread"
in the Library and a request may have multiple "threads" - an example is
a FTP request which has a thread to handle the control connection and one
to handle the data connections.
(
  Create a new Object
)

If we have more than HTMaxActive connections already then put this into the
pending queue, else start the request by calling the call back function
registered with this access method. Returns YES if OK, else NO
*/

extern BOOL HTNet_newClient (HTRequest * request);

/*

You can create a new HTNet object as a new request to be handled. If we have
more than HTMaxActive connections already then return NO. Returns YES if
OK, else NO
*/

extern BOOL HTNet_newServer (HTRequest * request);

/*

And you can create a plain new HTNet object using the following method:
*/

extern HTNet * HTNet_new (HTHost * host);

/*
(
  Duplicate an existing Object
)

Creates a new HTNet object as a duplicate of the same request. Returns YES
if OK, else NO.
*/

extern HTNet * HTNet_dup (HTNet * src);
extern BOOL HTNet_deleteDup (HTNet * dup);

/*
(
  Launch a Net Object
)

Start a Net obejct by calling the protocol module.
*/
extern BOOL HTNet_start (HTNet * net);

/*
(
  Call a Net Event Handler
)

This functions lets the caller play event manager as it can calls any event
handler with the event type and context passed to the function
*/

extern BOOL HTNet_execute (HTNet * net, HTEventType type);

extern HTEvent * HTNet_event (HTNet * net);
extern BOOL HTNet_setEventParam (HTNet * net, void * eventParam);
extern void * HTNet_eventParam (HTNet * net);
extern BOOL HTNet_setEventCallback(HTNet * net, HTEventCallback * cbf);
extern HTEventCallback * HTNet_eventCallback(HTNet * net);

/*
(
  Delete an Object
)

Deletes the HTNet object from the list of active requests and calls any
registered call back functions IF not the status is HT_IGNORE. This is used
if we have internal requests that the app doesn't know about. We also see
if we have pending requests that can be started up now when we have a socket
free. The filters are called in the reverse order of which they were registered
(last one first);
*/

extern BOOL HTNet_delete (HTNet * me, int status);

/*
(
  Delete ALL HTNet Objects
)

Deletes all HTNet object that might either be active or pending We DO NOT
call the call back functions - A crude way of saying goodbye!
*/

extern BOOL HTNet_deleteAll (void);

/*
.
  Net Class Methods
.
(
  Make an Object Wait
)

Let a net object wait for a persistent socket. It will be launched from the
HTNet_delete() function when the socket gets free.
*/

extern BOOL HTNet_wait (HTNet *net);

/*
(
  Priority Management
)

Each HTNet object is created with a priority which it inherits from the
Request manager. However, in some stuations it is
useful to be to change the current priority after the request has been started.
These two functions allow you to do this. The effect will show up the first
time (which might be imidiately) the socket blocks and control returns to
the event loop. Also have a look at how you can do this before the request
is issued in the request manager.
*/

extern HTPriority HTNet_priority (HTNet * net);
extern BOOL HTNet_setPriority (HTNet * net, HTPriority priority);

/*
(
  Persistent Connections
)

You can set a Net object to handle persistent connections for example using
HTTP, NNTP, or FTP. You can control whether a Net object supports persistent
connections or not using this function.
*/

extern BOOL HTNet_persistent (HTNet * net);

/*

You can set or disable a Net object supporting persistent connections using
this function:
*/

extern BOOL HTNet_setPersistent (HTNet *           net,
                                 BOOL              persistent,
                                 HTTransportMode   mode);

/*
(
  Kill one or more Requests
)

  Kill this request and all requests in the Pipeline


When pipelining, it is not possible to kill a single request as we then loose
track of where we are in the pipe. It is therefore necessary to kill the
whole pipeline.
*/

extern BOOL HTNet_killPipe (HTNet * net);

/*

  Kill a single Request


This is not often used anymore, consider using the pipeline version above.
Kill the request by calling the call back function with a request for closing
the connection. Does not remove the object. This is done by HTNet_delete()
function which is called by the load routine. Returns OK if success, NO on
error.
*/

extern BOOL HTNet_kill (HTNet * me);

/*

  Kill ALL Requests


Kills all registered (active as well as pending) requests by calling
the call back function with a request for closing the connection. We do not
remove the HTNet object as it is done by HTNet_delete(). Returns OK if success,
NO on error
*/

extern BOOL HTNet_killAll (void);

/*
(
  Create Input and Output Streams
)

You create the input stream and bind it to the channel using the following
methods. Please read the description in the
HTIOStream module on the parameters
target, param, and mode. Both methods return YES
if OK, else NO.
*/

#if 0
extern HTInputStream * HTNet_getInput (HTNet * net, HTStream * target,
				       void * param, int mode);
#endif
extern HTOutputStream * HTNet_getOutput (HTNet * me, void * param, int mode);

/*
(
  Net Context Descriptor
)

Just like the request
object, a net object can be assigned a context which keeps track of context
dependent information. The Library does not use this information nor does
it depend on it but it allows the application to customize a net object to
specific uses.
*/
extern BOOL HTNet_setContext (HTNet * net, void * context);
extern void * HTNet_context (HTNet * net);

/*
(
  Socket Descriptor
)
*/

extern BOOL HTNet_setSocket (HTNet * net, SOCKET sockfd);
extern SOCKET HTNet_socket (HTNet * net);

/*
(
  Preemptive or Non-preemptive Access
)

A access scheme is defined with a default for using either preemptive (blocking
I/O) or non-premitve (non-blocking I/O). This is basically a result of the
implementation of the protocol module itself. However, if non-blocking I/O
is the default then some times it is nice to be able to set the mode to blocking
instead. For example when loading the first document (the home page) then
blocking can be used instead of non-blocking.
*/

extern BOOL HTNet_preemptive (HTNet * net);

/*
(
  The Request Object
)

The Request object is normally set up automatically
but can be changed at a later time.
*/

extern BOOL HTNet_setRequest (HTNet * net, HTRequest * request);
extern HTRequest * HTNet_request (HTNet * net);

/*
(
  The Protocol Object
)
*/

extern BOOL HTNet_setProtocol (HTNet * net, HTProtocol * protocol);
extern HTProtocol * HTNet_protocol (HTNet * net);

/*
(
  The Transport Object
)

The transport object is normally set up automatically
but can be changed at a later time.
*/

extern BOOL HTNet_setTransport (HTNet * net, HTTransport * tp);
extern HTTransport * HTNet_transport (HTNet * net);

/*
(
  The Channel Object
)
*/

extern BOOL HTNet_setChannel (HTNet * net, HTChannel * channel);
extern HTChannel * HTNet_channel (HTNet * net);

/*
(
  The Host Object
)
*/

extern BOOL HTNet_setHost (HTNet * net, HTHost * host);
extern HTHost * HTNet_host (HTNet * net);

/*
(
  The DNS Object
)

The DNS object keeps track of the DNS entries that we have already checked
out.
*/

extern BOOL HTNet_setDns (HTNet * net, HTdns * dns);
extern HTdns * HTNet_dns (HTNet * net);

/*
(
  Target for Input Read Stream
)
*/

extern HTStream * HTNet_readStream(HTNet * net);
extern BOOL HTNet_setReadStream (HTNet * net, HTStream * stream);

/*
(
  Should we count Raw bytes?
)

This functions can be used to determine whether bytes count should be managed
at the low level read stream or at a higher level. If the data transfer equals
the lifetime of a single document like for example in FTP or HTTP/1.0 then
this may be a reasonable thing to do.
*/
extern BOOL HTNet_setRawBytesCount (HTNet * net, BOOL mode);
extern BOOL HTNet_rawBytesCount (HTNet * net);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTNET_H */

/*

  

  @(#) $Id$

*/
