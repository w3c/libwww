/*

  
  					W3C Sample Code Library libwww Core Interface


!
  Declaration of W3C Sample Code Library Core Interface
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is the basic include file for the core of the W3C Sample Code Library.
The core part of the Library is designed as a set of registration modules
with no real functionality in itself. Instead all the functionality comes
when the application registeres the modules that provides a desired functionaly,
for example accessing HTTP servers or the local file system. The Library
has a special include file called WWWApp.h which
contains all converters, protocol modules, and a lot of other "sugar" modules
that can make the core a very powerful Web interface. You can include this
one if the application is to use all the functionality of the Library.
*/

#ifndef WWWCORE_H
#define WWWCORE_H

/*
*/

#ifdef __cplusplus
extern "C" { 
#endif

/*
(
  System dependencies
)

The wwwsys.h file includes system-specific include
files and flags for I/O to network and disk. The only reason for this file
is that the Internet world is more complicated than Posix and ANSI.
*/
#include "wwwsys.h"

/*
(
  Generic Libwww Information
)

This module contains some generic functions for getting the name and version
of libwww. It also contains some global configuration options like if you
can access the local file system, for example.
*/
#include "HTLib.h"

/*
(
  The Request Class
)

Libwww is based on a request/response paradigm and the Request class defines
"an operation to be performed on a URL". The request object is the
main entry point for an application to issue a request to the Library - all
operations on a URL must use a Request object.
*/

#include "HTReq.h"

/*
(
  Request Methods
)

This module defines the set of methods that you can perform on a request,
for example GET, HEAD, PUT, POST, DELETE,
etc.
*/

#include "HTMethod.h"

/*
(
  The Anchor (URL) Class
)

An anchor represents a region of a hypertext
document which is linked to another anchor in the same or a different document.
Another name for anchors would be URLs as an anchor represents all we know
about a URL - including where it points to and who points to it.
*/
#include "HTAnchor.h"

/*
(
  The Link Class
)

A Link represents the link between anchor objects.
By keeping the link as a object and not as part of the anchor we are capable
of handling link semantics in a much more organized way. For example, we
can then search for link types among all the link objects that we have created.
*/
#include "HTLink.h"

/*
(
  Parsing URLs
)

This module contains code to parse URIs for the various components according
to the URI syntax
*/

#include "HTParse.h"

/*
(
  Escaping and Unescaping URLs
)

URLs are written only with the graphic printable characters of the US-ASCII
coded character set. All other characters must be escaped before they can
be used in URLs. This module defines the methods required for escaping and
unescaping the URLs.
*/

#include "HTEscape.h"

/*
(
  URL Trees and Hierarchies
)

A URL tree is a data class that can store all
the information we know about a URL hierarchy. Typically, a URL hierarchy
is what a client sees of a Web server but it can also be the view a server
has of itself. A URL tree has the advantage that it can be searched using
URLs or using realms. The letter is most useful to "guess" information
about a remote URL that we haven't seen before.
*/

#include "HTUTree.h"

/*
(
  Web Related String Functions
)

This module is like the generic string utility
module but it contains more Web related string utility functions. Examples
are functions that return a date string, a Message ID string
etc.
*/

#include "HTWWWStr.h"

/*
(
  The User Profile Class
)

The User profile&nbsp;class manages what we know about a user on this
host. This can for example be the FQDN of the host, the user's email
address, the time zone, the news server etc.
*/

#include "HTUser.h"

/*
(
  The Event Class
)

The Event Class defines any event manager to be used by libwww for handling
events.
*/

#include "HTEvent.h"

/*
(
  The Network Trace Handler
)

The Network Trace Handler provides an easy mechanism
for tracing memory problems. It gives a complete dump of all read and written
information to and from the network
*/

#include "HTMemLog.h"

/*
(
  The Error Class
)

The Error class provides an easy API for registering
errors ocurring while the Library serves a request. All errors are registered
in an "error stack"&nbsp;in the Request object which
allows for nested errors.
*/

#include "HTError.h"

/*
(
  The Alert Class
)

The Alert class defines a set of methods to be
used by libwww to be used for passing prompts and message to a user.
*/

#include "HTAlert.h"

/*
(
  The Format Manager
)

The Format Manager is responsible for setting up the stream pipe from the
Channel Object to the Request
Object when data is arriving, for example as a response to s
HTTP Get request. The Format Manager is also
responsible for keeping track of the "preferences" of the application
and/or user. It is an integral part of the Web and HTTP, that the client
application can express its preferences as a set of "accept" headers in a
HTTP request.
*/

#include "HTFormat.h"

/*
(
  The Generic Stream Class
)

The Stream class defines objects which accepts
a sequence of characters. Streams may also have an output in which case multiple
stream objects can be cascaded to build a stream pipe where the output of
a stream is directed into the input of the next stream object "down the line".
*/

#include "HTStream.h"

/*
(
  The Structured Stream Class
)

The Structured stream class defines objects which
accepts a structured sequence of characters for eaxmple a SGML document.
I'll rephrase that. A structured object is am ordered tree-structured arrangement
of data which is representable as text. An example is the
SGML parser which outputs to a Structured Object.
*/

#include "HTStruct.h"

/*
(
  No Free Stream
)

This stream is a throughline for all methods except FREE and
ABORT. This means that it can be use to put ahead of streams that you
don't want to be freed or aborted until you are redy to do it
yourself.
*/

#include "HTNoFree.h"

/*
(
  The Input/output Stream Classes
)

The I/O Stream class defines objects which
accepts a sequence of characters to and from a
transport
*/

#include "HTIOStream.h"

/*
(
  The DNS Class
)

The DNS Class defines generic access to &nbsp;the DNS system. It maintains
a cache of all visited hosts so that subsequent connects to the same host
doesn't imply a new request to the DNS every time.
*/

#include "HTDNS.h"

/*
(
  The Host Class
)

The Host class manages what we know about a remote
host. This can for example be what type of host it is, and what version it
is using.
*/

#include "HTHost.h"

/*
(
  The Net Class
)

The Net class manages information related to a "thread"
in libwww. As libwww threads are not really threads but a notion of using
interleaved, non-blocking I/O for accessing data objects from the network
(or local file system), they can be used on any platform with or without
support for native threads.
*/

#include "HTNet.h"

/*
(
  Internet Functions
)

This module has the common code for handling typical Internet functions like
getting the name of the local host, getting the domain name and email address
of user etc.
*/

#include "HTInet.h"

/*
(
  The Transport Class
)

The Transport Class defines a transport as used by the
HTChannel class to communicate with the network,
the local file system etc. New transport objects may be registered at any
time. This allows the application to easily hook in its own transport layers.
*/

#include "HTTrans.h"

/*
(
  The Protocol Class
)

The Protocol class defines an application level protocol (HTTP, FTP, Gopher,
etc.) to be used by libwww. Please note that access to the local file system
also is considered to be an appliaction level protocol treated identically
to for example the HTTP protocol.
*/

#include "HTProt.h"

/*

End of Core modules
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
