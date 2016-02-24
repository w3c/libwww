/*

  					W3C Sample Code Library libwww Host Class


!
  The Host Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Host class manages what we know about a remote host. This can for example
be what type of host it is, and what version it is using. Notice that a host
object can be used to describe both a server or a client - all information
in the Host object can be shared regardless of whether it is to be used in
a server application or a client application.

This module is implemented by HTHost.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTHOST_H
#define HTHOST_H

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTHost HTHost;
#define HOST_HASH_SIZE		HT_M_HASH_SIZE

#include "HTChannl.h"
#include "HTReq.h"
#include "HTEvent.h"
#include "HTProt.h"
#include "HTTimer.h"

/*

The Host class contains information about the remote host, for example the
type (HTTP/1.0, HTTP/1.1, FTP etc.) along with information on how the connections
can be used (if it supports persistent connections, interleaved access etc.)
.
  Creation and Deletion Methods
.

We keep a cache of information that we know about a remote host. This allows
us to be much more detailed in generating requests. Search the host info
cache for a host object or create a new one and add it. Examples of host
names are
	 
	   o 
	     www.w3.org
	   o 
	     www.foo.com:8000
	 
	 (
  Add a Host Object
)
*/

extern HTHost * HTHost_new (char * host, u_short u_port);
extern HTHost * HTHost_newWParse(HTRequest * request, char * url, u_short u_port);
extern int HTHost_hash (HTHost * host);

/*
(
  Delete a Host Object
)

The Host Class contains an automatic garbage collection of Host objects so
that we don't keep information around that is stale.
(
  Find a Host Object
)

Searches the cache of known hosts to see if we already have information about
this host. If not then we return NULL.
*/

extern HTHost * HTHost_find (char * host);

/*
(
  Delete the Host table
)

Cleanup and delete the host table.
*/

extern void HTHost_deleteAll (void);

/*
(
  Is Host Idle?
)

You can use this function to see whether a host object is idle or in use.
We have several modes describing how and when a host is idle. This is a function
of the Transport Object
*/

extern BOOL HTHost_isIdle (HTHost * host);

/*
.
  Remote Host Information
.

We keep track of the capabilities of the host in the other end so thatwe
may adjust our queries to fit it better
(
  Remote Host Name
)

Get the name of the remote host. This is set automatically when a new Host
object and can be asked for at any point in time. You can not change the
host name but must create a new Host object instead.
*/

extern char * HTHost_name	(HTHost * host);

/*
(
  Remote Host Protocol Class and Version
)

Define the host class of the host at the other end. A class is a
generic description of the protocol which is exactly like the access method
in a URL, for example "http" etc. The host version is a finer
distinction (sub-class) between various versions of the host class, for example
HTTP/0.9, HTTP/1.1 etc. The host version is a bit flag that the protocol
module can define on its own. That way we don't have to change this module
when registering a new protocol module. The host type is a description
of whether we can keep the connection persistent or not.
*/

extern char * HTHost_class	(HTHost * host);
extern void HTHost_setClass	(HTHost * host, char * s_class);

extern int  HTHost_version	(HTHost * host);
extern void HTHost_setVersion	(HTHost * host, int version);

/*
(
  Public Methods accessible on This Host
)

A server can inform a client about the supported methods using the
Public header.
*/
extern HTMethod HTHost_publicMethods 	(HTHost * me);
extern void HTHost_setPublicMethods 	(HTHost * me, HTMethod methodset);
extern void HTHost_appendPublicMethods	(HTHost * me, HTMethod methodset);

/*
(
  Server Name of Remote Host
)

A server can send its server application name and version in a HTTP response.
We pick up this information and add it to the Host object
*/
extern char * HTHost_server	(HTHost * host);
extern BOOL HTHost_setServer	(HTHost * host, const char * server);

/*
(
  User Agent Name of Remote Host
)

A client can send the name of the client application in a HTTP request. We
pick up this information and add it to the Host Object
*/
extern char * HTHost_userAgent 	(HTHost * host);
extern BOOL HTHost_setUserAgent	(HTHost * host, const char * userAgent);

/*
(
  Range Units Accepted by this Host
)

Since all HTTP entities are represented in HTTP messages as sequences of
bytes, the concept of a byte range is meaningful for any HTTP entity. (However,
not all clients and servers need to support byte-range operations.) Byte
range specifications in HTTP apply to the sequence of bytes in the entity-body
(not necessarily the same as the message-body). A byte range operation may
specify a single range of bytes, or a set of ranges within a single entity.

You can also check whether a specific range unit is OK. We always say
YES except if we have a specific statement from the server that
it doesn't understand byte ranges - that is - it has sent "none" in a
"Accept-Range" response header
*/

extern char * HTHost_rangeUnits  (HTHost * host);
extern BOOL HTHost_setRangeUnits (HTHost * host, const char * units);
extern BOOL HTHost_isRangeUnitAcceptable (HTHost * host, const char * unit);

/*
(
  User Defined Contexts
)

This can be used for anything that the application would like to keep tabs
on.
*/

extern void HTHost_setContext (HTHost * me, void * context);
extern void * HTHost_context  (HTHost * me);

/*
.
  Register a Request on a Host Object
.

Requests are queued in the Host object until we have
resources to start them. The request is in the form
of a Net object as we may have multiple socket requests
per Request object. This is for example the case
with FTP which uses two connections.
*/

extern int  HTHost_addNet    (HTHost * host, HTNet * net);
extern BOOL HTHost_deleteNet (HTHost * host, HTNet * net, int status);

extern HTList * HTHost_net   (HTHost * host);

/*
.
  Channels
.

A Channel object is an abstraction for a transport,
like a TCP connection, for example. Each host object can have at most one
channel object associated with it.
(
  Create a Channel to a Host
)

As a Net Object doesn't necessarily know whether
there is a channel up and running and whether that channel can be reused,
it must do an explicit connect the the host.
*/

extern int HTHost_connect (HTHost * host, HTNet * net, char * url);

extern int HTHost_accept  (HTHost * host, HTNet * net, char * url);

extern int HTHost_listen  (HTHost * host, HTNet * net, char * url);

/*
(
  Is Channel About to Close?
)

As soon as we know that a channel is about to close (for example because
the server sends us a Connection: close header field) then we register
this informtation in the Host object:
*/

extern BOOL HTHost_setCloseNotification (HTHost * host, BOOL mode);
extern BOOL HTHost_closeNotification (HTHost * host);

/*
(
  Find Channel Associated with a Host Object
)

Here you can find an already associated channel with a host object or you
can explicitly associate a channel with a host object.
*/

extern BOOL HTHost_setChannel (HTHost * host, HTChannel * channel);
extern HTChannel * HTHost_channel (HTHost * host);

/*
(
  Delete a Channel
)

When a channel is deleted, it must be unregistered from the host object which
is done by this operation:
*/

extern BOOL HTHost_clearChannel (HTHost * host, int status);

/*
.
  Transport Mode
.

The way a channel can be used depends on the
transport and what mode the channel is in. The
mode (whether we can use persistent connections, pipeline, etc.) may change
mode in the middle of a connection If the new mode is lower than the old
mode then adjust the pipeline accordingly. That is, if we are going into
single mode then move all entries in the pipeline and move the rest to the
pending queue. They will get launched at a later point in time.
*/

extern HTTransportMode HTHost_mode (HTHost * host, BOOL * active);
extern BOOL HTHost_setMode (HTHost * host, HTTransportMode mode);

/*
.
  Handling Pending Requests
.

There are two ways we can end up with pending requests:

  o 
	     If we are out of sockets then register new host objects as pending.
	   o 
	     If we are pending on a connection then register new net objects as pending
	 

This set of functions handles pending host objects and can start new requests
as resources get available. The first function checks the host object for
any pending Net objects and return the first of
these Net objects.
*/

extern HTNet * HTHost_nextPendingNet (HTHost * host);

/*

The second checks the list of pending host objects waiting for a socket and
returns the first of these Host objects.
*/

extern HTHost * HTHost_nextPendingHost (void);

/*
(
  Start the Next Pending reqeust
)

Start the next pending request if any. First we look for pending requests
for the same host and then we check for any other pending hosts. If nothing
pending then register a close event handler to have something catching the
socket if the remote server closes the connection, for example due to timeout.
*/

extern BOOL HTHost_launchPending (HTHost * host);

/*
(
  Stop Launch of Pending Requests
)

Controls whether pending requests should be automatically activated. The
default is on, but if turned off then no pending requests are launched.
*/

extern void HTHost_enable_PendingReqLaunch (void);
extern void HTHost_disable_PendingReqLaunch (void);

/*
.
  Persistent Connections
.

We don't want more than (Max open sockets) - 2 connections to be persistent
in order to avoid deadlock. You can set the max number of simultaneous open
connection in the HTNet manager.
(
  Is this host Persistent?
)
*/

extern BOOL HTHost_setPersistent (HTHost * host, BOOL persistent,
                                  HTTransportMode mode);
extern BOOL HTHost_isPersistent (HTHost * host);

/*
(
  Persistent Connection Timeouts
)

If the server doesn't close the connection on us then we close it after a
while so that we don't unnecessarily take up resources (see also how the
timeouts of individual requests can be set).
Libwww provides two mechanisms: an active timeout and a passive timeout.
The former uses libwww timers and is the preferred
mechanism, the latter passively looks at the Host object when a new request
is issued in order to determine whether the existing channel can be reused.
This is primariliy for non-preemptive requests which in general is deprecated.

By default we have an active timeout of 60 secs and a passive timeout of
120 secs (the latter is longer as this is less reliable). Active timeout
s can be accessed using these functions:
*/

extern BOOL HTHost_setActiveTimeout (ms_t timeout);
extern ms_t HTHost_activeTimeout (void);

/*

and passive timeouts can be accessed using these functions
*/

extern time_t HTHost_persistTimeout (void);
extern BOOL HTHost_setPersistTimeout (time_t timeout);

/*

The following two functions are deprecated:
*/

extern void HTHost_setPersistExpires (HTHost * host, time_t expires);
extern time_t HTHost_persistExpires (HTHost * host);

/*
(
  Keeping Track of Number of Reqeusts
)

Another way to detect when a connection is about to close is to count the
number of requests made. For example, the (current) default bevaior by most
Apache servers is to close a TCP connection after 100 requests. I don't quite
think it makes sense to control the close of a connection like this but anyway,
there we go.
*/

extern void HTHost_setReqsPerConnection (HTHost * host, int reqs);
extern int HTHost_reqsPerConnection (HTHost * host);
extern void HTHost_setReqsMade (HTHost * host, int reqs);
extern int HTHost_reqsMade (HTHost * host);

/*
.
  Read and Write Management
.

Which Net object can read and/or write? When doing
pipelining, we essentially serialize requests and therefore we must keep
track of who can read and who can write.
(
  Get the Next Net object for Reading and Writing
)
*/

extern HTNet * HTHost_firstNet     (HTHost * host);
extern HTNet * HTHost_getReadNet  (HTHost * host);
extern HTNet * HTHost_getWriteNet (HTHost * host);

/*
(
  Get input and output Streams for this Host
)
*/

extern HTInputStream * HTHost_getInput (HTHost * host, HTTransport * transport,
				        void * param, int mode);

extern HTOutputStream * HTHost_getOutput (HTHost * host, HTTransport * tp,
					  void * param, int mode);

/*
(
  Reading Data and Keeping Track of how Much
)

Because of the push streams, the streams must keep track of how much data
actually was consumed by that stream.
*/

extern int HTHost_read(HTHost * host, HTNet * net);

extern BOOL HTHost_setConsumed(HTHost * host, size_t bytes);
extern BOOL HTHost_setRemainingRead(HTHost * host, size_t remainaing);
extern size_t HTHost_remainingRead (HTHost * host);

/*
.
  Pipelining Requests
.

When possible, we try to pipeline requests onto the same connection as this
saves a lot of time and leads to much higher throughput.
(
  How many Requests can we Pipeline onto the same Connection?
)

Use these functions to set the max number of requests that can be pipelined
at any one time on a single, persistent connection. The higher the number,
the more we have to recover if the server closes the connection prematurely.
The default is about 50 requests which is enough to fill most links.
*/

extern BOOL HTHost_setMaxPipelinedRequests (int max);
extern int HTHost_maxPipelinedRequests (void);

/*
(
  How many Pending and Outstanding Net objects are there on a Host?
)

You can query how many Het objects (essentially requests) are outstanding
or pending on a host object using these methods:
*/

extern int HTHost_numberOfOutstandingNetObjects (HTHost * host);
extern int HTHost_numberOfPendingNetObjects (HTHost * host);

/*
(
  Pipeline Recovery
)

Pipelines normally run by themselves (requests are issued and responses
recieved). However, it may be necessry to either prematurely abort a pipeline
or to recover a broken pipeline due to communication problems with the server.
In case a pipeline is broken then we have to recover it and start again.
This is handled automatically by the host object, so you do not have to call
this one explicitly.
*/

extern BOOL HTHost_recoverPipe (HTHost * host);
extern BOOL HTHost_doRecover (HTHost * host);

/*
(
  Kill a Pipeline
)

Call this function to terminate all requests (pending as well as active)
registered with a host object. This is typically the function that handles
timeout, abort (user hits the red button, etc). You can also use the
HTNet object kill method which in terms call this
function.
*/
extern BOOL HTHost_killPipe (HTHost * host);

/*
.
  Event Management
.

These functions are used to register and unregister events (read, write,
etc.) so that the host object knows about it.
*/

extern int HTHost_register(HTHost * host, HTNet * net, HTEventType type);
extern int HTHost_unregister(HTHost * host, HTNet * net, HTEventType type);
extern int HTHost_tickleFirstNet(HTHost * host, HTEventType type);

extern SockA * HTHost_getSockAddr(HTHost * host);

/*
(
  Request Timeouts
)

Events can be assigned a timeout which causes the event to be triggered if
the timeout happens before other action is available on the socket. You can
assign a global timeout for all host object using the following methods.
The default is no timeout.
*/

extern int HTHost_eventTimeout (void);
extern void HTHost_setEventTimeout (int millis);

/*
.
  Delayed Flush Timer
.

These methods can control how long we want to wait for a flush on a pipelined
channel. The default is 30ms which is OK in most situations.
*/

extern BOOL HTHost_setWriteDelay (HTHost * host, ms_t delay);
extern ms_t HTHost_writeDelay (HTHost * host);
extern int HTHost_findWriteDelay(HTHost * host, ms_t lastFlushTime, int buffSize);

/*

It is also possible to explicitly require a flush using the following method.
This can also be set directly in the request object
for a single request.
*/

extern int HTHost_forceFlush(HTHost * host);

/*

You can also set the global value so that all new host objects (and
therefore all new requests) will inherit this value instead of setting it
individually.
*/
extern BOOL HTHost_setDefaultWriteDelay (ms_t delay);
extern ms_t HTHost_defaultWriteDelay (void);

/*
.
  Multi homed Host Management
.

We keep track of hosts with multiple IP addresses - socalled multi-homed
hosts. This is used for two things: finding the fastest IP address of
that host and as a backup if one or more of the hosts are down. This is handled
in connection with the DNS manager
*/

extern BOOL HTHost_setHome (HTHost * host, int home);
extern int HTHost_home (HTHost * host);

extern BOOL HTHost_setRetry (HTHost * host, int retry);
extern int HTHost_retry (HTHost * host);
extern BOOL HTHost_decreaseRetry (HTHost * host);

/*
.
  Notify Request that it has become Active
.

A new callback plugged to the activation of a request which allows an application
to know when a request has become active.
*/

typedef int HTHost_ActivateRequestCallback (HTRequest * request);
extern void HTHost_setActivateRequestCallback
		(HTHost_ActivateRequestCallback * cbf);

/*

*/

#ifdef __cplusplus
}
#endif

#endif /* HTHOST_H */

/*

  

  @(#) $Id$

*/
