/*

  
  					W3C Sample Code Library libwww Request Class


!
  The Request Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

Libwww is based on a request/response paradigm and the Request class defines
"an operation to be performed on a URL". The request object is the
main entry point for an application to issue a request to the Library - all
operations on a URL must use a Request object. The request object
is application independent in that both servers and clients use the same
Request class. Examples of requests passed to the Library are a client
application issuing a GET request on a HTTP URL, or a server issuing
a load on a local file URL. The only difference is that the client gets the
input from a user whereas the server gets the input via the network.

A request object is created with a default set of parameters which are applicable
for many URL requests but the class defines a huge set of methods that an
be used to customize a request for a particular purpose. Example of things
that you can define is natural language, media types, what RFC 822 headers
to use, whether the request should be refreshed from cache etc. Scroll down
and see the set of parameters you can tune.

A request object is registered in the library by issuing an operation on
a URL - for example PUT, POST, or DELETE. You can find
many higher level "request issuing functions" in the
Access module - the methods defined by the Request
class itself are very low level but can of course be used directly if needed.

Whereas the lifetime of the URL (in form of an anchor) often is very long
(for example as long as the application is running), the lifetime of a request
is limited to the time it takes to service the request. The core does not
automatically delete any request object created by the application - it is
for the application to do. In many cases a request object can be deleted
when any of the termination callback functions
are called but the application may keep request objects around longer than
that

The Library can accept an unlimited number of simultaneous requests passed
by the application. One of the main functions of the Library core is to handle
any number of ongoing requests in an intelligent manner by limiting the number
of active request to the fit the available resources as defined by the
application. This is described in more detail in the HTNet
module.

This module is implemented by HTReqMan.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTREQ_H
#define HTREQ_H

#ifdef __cplusplus
extern "C" { 
#endif 

typedef long HTRequestID;
typedef struct _HTRequest HTRequest;

#include "HTEvent.h"
#include "HTList.h"
#include "HTAssoc.h"
#include "HTFormat.h"
#include "HTStream.h"
#include "HTError.h"
#include "HTNet.h"
#include "HTUser.h"
#include "HTResponse.h"

/*
.
  Creation and Deletion Methods
.

The request object is intended to live as long as the request is still active,
but can be deleted as soon as it has terminated, for example in one of the
request termination callback functions as described in the
Net Manager. Only the anchor object stays around
after the request itself is terminated.
(
  Create new Object
)

Creates a new request object with a default set of options -- in most cases
it will need some information added which can be done using the methods in
this module, but it will work as is for a simple request.
*/

extern HTRequest * HTRequest_new (void);

/*
(
  Clear a Request Object
)

Clears all protocol specific information so that the request object can be
used for another request. It should be used with care as application specific
information is not re-initialized. Returns YES if OK, else NO.
*/

extern BOOL HTRequest_clear (HTRequest * me);

/*
(
  Create a duplicate
)

Creates a new HTRequest object as a duplicate of the src request. Returns
YES if OK, else NO
*/

extern HTRequest * HTRequest_dup (HTRequest * src);

/*

  Create a duplicate for Internal use


Creates a new HTRequest object as a duplicate of the src request. The difference
to the HTRequest_dup function is that we don't copy the error_stack and other
information that the application keeps in its copy of the request object.
Otherwise it will be freed multiple times. Returns YES if OK, else NO
*/

extern HTRequest * HTRequest_dupInternal (HTRequest * src);

extern BOOL HTRequest_setInternal (HTRequest * request, BOOL mode);
extern BOOL HTRequest_internal (HTRequest * request);

/*
(
  Delete Object
)

This function deletes the object and cleans up the memory.
*/

extern void HTRequest_delete (HTRequest * request);

/*
.
  Issuing a Request
.

These are the "basic request methods" provided directly by the Request
class. This is a very low level API as the caller must have set up the request
object before passing it to libwww. There are two versions: one for issuing
client requests and one for issuing server requests. You will probably most
often use the client version, but, in fact, libwww can also deal with incoming
connections. You can find many higher level issuing functions in the
HTAccess module. If you like, you can of course
use this directly!
*/

extern BOOL HTLoad (HTRequest * request, BOOL recursive);
extern BOOL HTServe(HTRequest * request, BOOL recursive);

/*
.
  Killing a Request
.

This function kills this particular request, see HTNet
module for a function that kills them all. If you know that you are
pipelining requests (typically the case for GUI browsers, robots etc.) then
it is often not enough to just kill a single request as the whole pipeline
gets affected. Therefore, in that case you MUST call the
HTHost_killPipe function instead,
*/

extern BOOL HTRequest_kill(HTRequest * request);

/*

Note that you can get to the HTHost object via the HTNet
object which you can get by calling
HTRequest_net(...).
.
  Relations to Other Libwww Objects
.

The Request object is linked to a set of other libwww objects - here's how
to get to these objects...
(
  Binding to an Anchor Object
)

Every request object has an anchor associated
with it. The anchor normally lives until the application terminates but a
request object only lives as long as the request is being serviced. If the
anchor that we have requested is a child anchor then we always load
the parent anchor; after the load jump to the location. A child anchor
is a an anchor which points to a subpart of the document (has a "#" in the
URL).
*/

extern void HTRequest_setAnchor (HTRequest *request, HTAnchor *anchor);
extern HTParentAnchor * HTRequest_anchor (HTRequest *request);

extern HTChildAnchor * HTRequest_childAnchor (HTRequest * request);

/*
(
  Binding to a User Profile
)

Each request is associated with a User profile
which contains information about the local host name, email address of the
user, news server etc. A request object is created with a default "generic
user" but can be assigned a specific user at any time.
*/

extern BOOL HTRequest_setUserProfile (HTRequest * request, HTUserProfile * up);
extern HTUserProfile * HTRequest_userProfile (HTRequest * request);

/*
(
  Binding to a Net Object
)

If a request is actually going on the net then the Net
Manager is contacted to handle the request. The Net manager creates a
HTNEt object and links it to the Request object. You can get to the HTNet
object using the following functions.
*/

extern HTNet * HTRequest_net (HTRequest * request);
extern BOOL HTRequest_setNet (HTRequest * request, HTNet * net);

/*

Note that you can go from the HTNet object to the
HTHost object by calling HTNet_host(...).
(
  Binding to a Response Object
)

If a request is actually going on the net and we are getting a response back
then we also create a HTResponse object and
bind it to the request object. Once we know what to do with the response,
we may transfer the information to the anchor object.
*/

extern HTResponse * HTRequest_response (HTRequest * request);
extern BOOL HTRequest_setResponse (HTRequest * request, HTResponse * response);

/*
.
  Set the Method for the Request
.

The Method is the operation to be executed on the requested object. The default
set if the set of operations defined by the HTTP protocol, that is "GET",
"HEAD", "PUT", "POST", "LINK", "UNLINK", and "DELETE" but many of these can
be used in other protocols as well. The important thing is to think of the
requested element as an object on which you want to perform an operation.
Then it is for the specific protocol implementation to try and carry this
operation out. However, not all operations can be implemented (or make sense)
in all protocols.

Methods are handled by the Method Module, and
the default value is "GET".
*/

extern void HTRequest_setMethod (HTRequest *request, HTMethod method);
extern HTMethod HTRequest_method (HTRequest *request);

/*
.
  Priority Management
.

The request can be assigned an initial priority which then gets inherited
by all HTNet objects and other requests objects
created as a result of this one. You can also assign a separate priority
to an indicidual HTNet object by using the methods in the
Net manager.
*/

extern HTPriority HTRequest_priority (HTRequest * request);
extern BOOL HTRequest_setPriority (HTRequest * request, HTPriority priority);

/*
.
  Pipelining Managament
.

Libwww supports HTTP/1.1 pipelining which greatly optimizes HTTP's behavior
over TCP. libwww also tries very hard to minimize the number of TCP packets
sent over the network. This is done by buffering outgoing requests until
either a minimum amount of data has been collected or a timeout causes a
flush to happen. The application can override the output buffering by explicit
request a request object to be flushed.
*/

extern BOOL HTRequest_setFlush (HTRequest * me, BOOL mode);
extern BOOL HTRequest_flush (HTRequest * me);

/*
(
  Force the Pipeline to be Flushed Immediately
)

Forcing a fluch immediatly is slightly different as this can be done in
"retrospect". That is, if suddenly the application decides on performing
a flush after the request was initiated then it can use this function to
flush at a later time.
*/

extern int HTRequest_forceFlush (HTRequest * request);

/*
.
  Dealing with Request Error Messages
.

Errors are, like almost anything,  kept in lists. An error list can be
associated with a request using the following functions. In order to make 
life easier, there are also some easy mapping functions to the
HTError object, so that you can add an error directly
to a request object.
*/

extern HTList * HTRequest_error (HTRequest * request);
extern void HTRequest_setError	(HTRequest * request, HTList * list);
extern void HTRequest_deleteAllErrors (HTRequest * request);

/*

These are the cover functions that go directly to the
Error Object
*/

extern BOOL HTRequest_addError (HTRequest * 	request,
				HTSeverity	severity,
				BOOL		ignore,
				int		element,
				void *		par,
				unsigned int	length,
				char *		where);

extern BOOL HTRequest_addSystemError (HTRequest * 	request,
				      HTSeverity 	severity,
				      int		errornumber,
				      BOOL		ignore,
				      char *		syscall);

/*
.
  Max number of Retrys for a Down Load
.

Automatic reload can happen in two situations:
	 
	   o 
	     The server sends a redirection response
	   o 
	     The document has expired
	 
	 
In order to avoid the Library going into an infinite loop, it is necessary
to keep track of the number of automatic reloads. Loops can occur if the
server has a reload to the same document or if the server sends back a Expires
header which has already expired. The default maximum number of automatic
reloads is 6.
*/

extern BOOL HTRequest_setMaxRetry (int newmax);
extern int  HTRequest_maxRetry (void);

extern int HTRequest_retrys (HTRequest * request);
extern BOOL HTRequest_doRetry (HTRequest *request);
extern BOOL HTRequest_addRetry (HTRequest * request);

extern int HTRequest_AAretrys (HTRequest * request);
extern BOOL HTRequest_addAARetry (HTRequest * request);

/*
.
  Set Max Forwards for TRACE methods
.

The TRACE method is used to invoke a remote, application-layer
loop-back of the request message. The final recipient of the request SHOULD
reflect the message received back to the client as the entity-body of a 200
(OK) response. The final recipient is either the origin server or the first
proxy or gateway to receive a Max-Forwards value of zero (0) in the request.
A TRACE request MUST NOT include an entity.
*/
extern BOOL HTRequest_setMaxForwards (HTRequest * request, int maxforwards);
extern int HTRequest_maxForwards (HTRequest * request);

/*
.
  Preemptive or Non-preemptive Access
.

An access scheme is defined with a default for using either preemptive (blocking
I/O) or non-preemptive (non-blocking I/O). This is basically a result of the
implementation of the protocol module itself. However, if non-blocking I/O
is the default then some times it is nice to be able to set the mode to blocking
instead. For example, when loading the first document (the home page),
blocking mode can be used instead of non-blocking.
*/

extern void HTRequest_setPreemptive (HTRequest *request, BOOL mode);
extern BOOL HTRequest_preemptive (HTRequest *request);

/*
.
  Content Negotiation
.

When accessing the local file system, the Library is capable of performing
content negotioation as described by the HTTP protocol. This is mainly for
server applications, but some client applications might also want to use
content negotiation when accessing the local file system. This method enables
or disables content negotiation - the default value is ON.
*/

extern void HTRequest_setNegotiation (HTRequest *request, BOOL mode);
extern BOOL HTRequest_negotiation (HTRequest *request);

/*
.
  Request Preconditions (HTTP If-* Headers)
.

Should this request use preconditions when doing a PUT or a
POST? These are the "if-*" header fields that can be used
to avoid version conflicts etc. The default is not to use any preconsitions
(HT_NO_MATCH). The _THIS versions use etags and/or time
stamps and the _ANY versions use the "*" header field value
of the if-match and if-non-match header fields.
*/
typedef enum _HTPreconditions {
    HT_NO_MATCH = 0,
    HT_MATCH_THIS,
    HT_MATCH_ANY,
    HT_DONT_MATCH_THIS,
    HT_DONT_MATCH_ANY
} HTPreconditions;

extern void HTRequest_setPreconditions (HTRequest * me, HTPreconditions mode);
extern HTPreconditions HTRequest_preconditions (HTRequest * me);

/*
.
  Local MIME header Parsers
.

MIMEParsers get their own type which is optimized for static and regex parser
strings.
*/

typedef struct _HTMIMEParseSet HTMIMEParseSet;
extern void HTRequest_setMIMEParseSet (HTRequest *request,
				       HTMIMEParseSet * parseSet, BOOL local);
extern HTMIMEParseSet * HTRequest_MIMEParseSet (HTRequest *request,
					      BOOL * pLocal);

/*
.
  Which Default Protocol Header Fields To Use?
.

Libwww supports a large set of headers that can be sent along with a request
(or a response for that matter). All headers can be either disabled or enabled
using bit flags that are defined in the following. See also the
section on how to extend the default set of supported header
fields.
(
  General HTTP Header Mask
)

There are a few header fields which have general applicability for both request
and response mesages, but which do not apply to the communication parties
or theentity being transferred. This mask enables and disables these headers.
If the bit is not turned on they are not sent.
*/

typedef enum _HTGnHd {
    HT_G_CC             = 0x1,
    HT_G_CONNECTION	= 0x2,
    HT_G_DATE		= 0x4,
    HT_G_PRAGMA_NO_CACHE= 0x8,
    HT_G_FORWARDED	= 0x10,
    HT_G_MESSAGE_ID	= 0x20,
    HT_G_MIME		= 0x40,
    HT_G_TRAILER        = 0x80,
    HT_G_TRANSFER       = 0x100,
    HT_G_EXTRA_HEADERS  = 0x200
} HTGnHd;

#define DEFAULT_GENERAL_HEADERS	\
        HT_G_CONNECTION + HT_G_CC + HT_G_TRANSFER + HT_G_TRAILER + \
        HT_G_EXTRA_HEADERS

extern void HTRequest_setGnHd (HTRequest *request, HTGnHd gnhd);
extern void HTRequest_addGnHd (HTRequest *request, HTGnHd gnhd);
extern HTGnHd HTRequest_gnHd (HTRequest *request);

/*
(
  Request Headers
)

The request header fields allow the client to pass additional information
about the request (and about the client itself) to the server. All headers
are optional but the default value is all request headers if present
except From and Pragma.
*/

typedef enum _HTRqHd {
    HT_C_ACCEPT_TYPE	= 0x1,
    HT_C_ACCEPT_CHAR	= 0x2,
    HT_C_ACCEPT_ENC	= 0x4,
    HT_C_ACCEPT_TE	= 0x8,
    HT_C_ACCEPT_LAN	= 0x10,
    HT_C_AUTH		= 0x20,             /* Includes proxy authentication */
    HT_C_EXPECT         = 0x40,
    HT_C_FROM		= 0x80,
    HT_C_HOST		= 0x100,
    HT_C_IMS		= 0x200,
    HT_C_IF_MATCH	= 0x400,
    HT_C_IF_MATCH_ANY	= 0x800,
    HT_C_IF_NONE_MATCH	= 0x1000,
    HT_C_IF_NONE_MATCH_ANY=0x2000,
    HT_C_IF_RANGE	= 0x4000,
    HT_C_IF_UNMOD_SINCE	= 0x8000,
    HT_C_MAX_FORWARDS	= 0x10000,
    HT_C_RANGE		= 0x20000,
    HT_C_REFERER	= 0x40000,
    HT_C_USER_AGENT	= 0x80000
} HTRqHd;

#define DEFAULT_REQUEST_HEADERS	\
	HT_C_ACCEPT_TYPE + HT_C_ACCEPT_CHAR + \
	HT_C_ACCEPT_ENC + HT_C_ACCEPT_TE + HT_C_ACCEPT_LAN + HT_C_AUTH + \
        HT_C_EXPECT + HT_C_HOST + HT_C_REFERER + HT_C_USER_AGENT

extern void HTRequest_setRqHd (HTRequest *request, HTRqHd rqhd);
extern void HTRequest_addRqHd (HTRequest *request, HTRqHd rqhd);
extern HTRqHd HTRequest_rqHd (HTRequest *request);

/*
(
  Response Headers
)

The response header fields allow the server to pass additional information
about the response (and about the server itself) to the client. All headers
are optional.
*/

typedef enum _HTRsHd {
    HT_S_AGE		= 0x1,
    HT_S_LOCATION   	= 0x2,
    HT_S_PROXY_AUTH 	= 0x4,
    HT_S_PUBLIC    	= 0x8,
    HT_S_RETRY_AFTER	= 0x10,
    HT_S_SERVER		= 0x20,
    HT_S_VARY		= 0x40,
    HT_S_WARNING	= 0x80,
    HT_S_WWW_AUTH	= 0x100,
    HT_S_TRAILER        = 0x200
} HTRsHd;

#define DEFAULT_RESPONSE_HEADERS HT_S_SERVER

extern void HTRequest_setRsHd (HTRequest * request, HTRsHd rshd);
extern void HTRequest_addRsHd (HTRequest * request, HTRsHd rshd);
extern HTRsHd HTRequest_rsHd (HTRequest * request);

/*
(
  Entity Header Mask
)

The entity headers contain information about the object sent in the HTTP
transaction. See the Anchor module, for the storage
of entity headers. This flag defines which headers are to be sent in a request
together with an entity body. All headers are optional but the default value
is ALL ENTITY HEADERS IF PRESENT
*/

typedef enum _HTEnHd {
    HT_E_ALLOW			= 0x1,
    HT_E_CONTENT_BASE		= 0x2,
    HT_E_CONTENT_ENCODING	= 0x4,
    HT_E_CONTENT_LANGUAGE	= 0x8,
    HT_E_CONTENT_LENGTH		= 0x10,
    HT_E_CONTENT_LOCATION	= 0x20,
    HT_E_CONTENT_MD5		= 0x40,
    HT_E_CONTENT_RANGE		= 0x80,
    HT_E_CTE			= 0x100,	/* Content-Transfer-Encoding */
    HT_E_CONTENT_TYPE		= 0x200,
    HT_E_DERIVED_FROM		= 0x400,
    HT_E_ETAG			= 0x800,
    HT_E_EXPIRES		= 0x1000,
    HT_E_LAST_MODIFIED		= 0x2000,
    HT_E_LINK			= 0x4000,
    HT_E_TITLE			= 0x8000,
    HT_E_URI			= 0x10000,
    HT_E_VERSION		= 0x20000
} HTEnHd;

#define DEFAULT_ENTITY_HEADERS		0xFFFFFFFF		      /* all */

extern void HTRequest_setEnHd (HTRequest *request, HTEnHd enhd);
extern void HTRequest_addEnHd (HTRequest *request, HTEnHd enhd);
extern HTEnHd HTRequest_enHd (HTRequest *request);

/*
.
  Extending The Default Set Of Header Fields
.

See also how to set up default header fields. There
are three ways to extend the set of headers that are sent in a request:

  o 
	     A simple association list
  o 
	     A stream oriented approach where the stream (called
    a generator) has direct access to the outgoing stream. That is, it can
    add any header it likes.
  o 
	     HTTP extension mechanism which
    is a much better way for handling extensions.

(
  1) Simple Association List
)

Add the (name, value) and it will be converted into MIME header format as
name: value. Do NOT add CRLF line termination - this is done by
the HTTP header generator stream
*/

extern BOOL HTRequest_addExtraHeader       (HTRequest * request,
                                            char * token, char * value);
extern HTAssocList * HTRequest_extraHeader (HTRequest * request);
extern BOOL HTRequest_deleteExtraHeaderAll (HTRequest * request);

/*
(
  2) Stream Oriented Header Generators
)

Extra header information can be send along with a request using
header generators. The text is sent as is so
it must be preformatted with CRLF line terminators. You can also
register MIME header parsers using the HTHeader
module.
*/

extern void HTRequest_setGenerator (HTRequest *request, HTList *gens,
                                    BOOL override);
extern HTList * HTRequest_generator (HTRequest *request, BOOL *override);

/*
(
  3) HTTP Extension Framework
)

These association lists contain the information that we are to send as HTTP
Extension Framework. This is not done yet but you can find some hints in
the PEP module
*/

/* TBD */

/*
.
  User And Application Preferences Using Accept Headers
.

The Accept family of headers is an important part of HTTP handling the format
negotiation. The Library supports both a global set of accept headers that
are used in all HTTP requests and a local set of accept headers
that are used in specific requests only. The global ones are defined in the
Format Manager.

Each request can have its local set of accept headers that either are added
to the global set or replaces the global set of accept headers. Non of the
headers have to be set. If the global set is sufficient for all
requests then this us perfectly fine. If the parameter "override" is set
then only local accept headers are used, else both local and global
headers are used.
(
  Content Types
)

The local list of specific conversions which the format manager
can do in order to fulfill the request. It typically points to a list set
up on initialisation time for example by HTInit().
There is also a global list of
conversions which contains a generic set of possible conversions.
*/

extern void HTRequest_setConversion (HTRequest *request, HTList *type, BOOL override);
extern HTList * HTRequest_conversion (HTRequest *request);

/*
(
  Content Encodings
)

The list of encodings acceptable in the output stream.
*/

extern void HTRequest_setEncoding (HTRequest *request, HTList *enc, BOOL override);
extern HTList * HTRequest_encoding (HTRequest *request);

/*
(
  Transfer Encodings
)

The list of transfer encodings acceptable in the output stream.
*/

extern void HTRequest_setTransfer (HTRequest *request, HTList *te, BOOL override);
extern HTList * HTRequest_transfer (HTRequest *request);

/*
(
  Content Languages
)

The list of (human) language values acceptable in the response. The default
is all languages.
*/

extern void HTRequest_setLanguage (HTRequest *request, HTList *lang, BOOL override);
extern HTList * HTRequest_language (HTRequest *request);

/*
(
  Content Charsets
)

The list of charsets accepted by the application
*/

extern void HTRequest_setCharset (HTRequest *request, HTList *charset, BOOL override);
extern HTList * HTRequest_charset (HTRequest *request);

/*
.
  HTTP Cache Validation and Cache Control
.

The Library has two concepts of caching: in memory and on file. When loading
a document, this flag can be set in order to define who can give a response
to the request. The memory buffer is considered to be equivalent to a history
buffer. That is, it doesn't not follow the same expiration mechanism that
is characteristic for a persistent file cache.

You can also set the cache to run in disconnected mode - see the
Cache manager for more details on how to do this.
*/

typedef enum _HTReload {
    HT_CACHE_OK	            = 0x0,	        /* Use any version available */
    HT_CACHE_FLUSH_MEM	    = 0x1,	/* Reload from file cache or network */
    HT_CACHE_VALIDATE	    = 0x2,	             /* Validate cache entry */
    HT_CACHE_END_VALIDATE   = 0x4,                  /* End to end validation */
    HT_CACHE_RANGE_VALIDATE = 0x8,
    HT_CACHE_FLUSH	    = 0x10,                     /* Force full reload */
    HT_CACHE_ERROR          = 0x20         /* An error occurred in the cache */
} HTReload;

extern void HTRequest_setReloadMode (HTRequest *request, HTReload mode);
extern HTReload HTRequest_reloadMode (HTRequest *request);

/*
.
  Default PUT name
.

When publishing to a server which doesn't accept a URL ending in "/", e.g,
the default Overview, index page, you can use
HTRequest_setAltPutName to setup the intended URL. If this
variable is defined, it'll be used during the cache lookup and update
operationsm, so that cache-wise, it will look as if we had published
only to "/".
*/

extern char * HTRequest_defaultPutName (HTRequest * me);
extern BOOL HTRequest_setDefaultPutName (HTRequest * me, char * name);
extern BOOL HTRequest_deleteDefaultPutName (HTRequest * me);

/*
(
  HTTP Cache Control Directives
)

The cache control directives are all part of the cache control header and
control the behavior of any intermediate cache between the user agent and
the origin server. This association list is a list of the connection control
directives that are to be sent as part of the Cache-Control
header.
*/

extern BOOL HTRequest_addCacheControl        (HTRequest * request,
                                              char * token, char *value);
extern BOOL HTRequest_deleteCacheControlAll  (HTRequest * request);
extern HTAssocList * HTRequest_cacheControl  (HTRequest * request);

/*
.
  Date and Time Stamp when Request was Issued
.

The start time when the request was issued may be of value to the cache
validation mechanism as described by the HTTP/1.1 specification. The value
is automatically set when creating the request headers and sending off the
request. The time is a local time.
*/

extern time_t HTRequest_date  (HTRequest * request);
extern BOOL HTRequest_setDate (HTRequest * request, time_t date);

/*
.
  HTTP Expect Directives
.

The Expect request-header field is used to indicate that particular server
behaviors are required by the client. A server that does not understand or
is unable to comply with any of the expectation values in the Expect field
of a request MUST respond with appropriate error status.
*/

extern BOOL HTRequest_addExpect (HTRequest * me,
				 char * token, char * value);
extern BOOL HTRequest_deleteExpect (HTRequest * me);
extern HTAssocList * HTRequest_expect (HTRequest * me);

/*
.
  Partial Requests and Range Retrievals
.

Libwww can issue range requests in case we have already obtained a part of
the entity body. Since all HTTP entities are represented in HTTP messages
as sequences of bytes, the concept of a byte range is meaningful for any
HTTP entity. (However, not all clients and servers need to support byte-range
operations.) Byte range specifications in HTTP apply to the sequence of bytes
in the entity-body (not necessarily the same as the message-body). A byte
range operation may specify a single range of bytes, or a set of ranges within
a single entity.
*/

extern BOOL HTRequest_addRange       (HTRequest * request,
                                      char * unit, char * range);
extern BOOL HTRequest_deleteRangeAll (HTRequest * request);
extern HTAssocList * HTRequest_range (HTRequest * request);

/*
.
  HTTP Connection Control Request Directives
.

The connection control directives are all part of the connection header and
control the behavior of this connection. This association list is a list
of the connection control directives that are to be sent as part of the
Connection header.
*/

extern BOOL HTRequest_addConnection        (HTRequest * request,
                                            char * token, char * value);
extern BOOL HTRequest_deleteConnection     (HTRequest * request);
extern HTAssocList * HTRequest_connection  (HTRequest * request);

/*
.
  HTTP Access Authentication Credentials
.

When a access denied response is returned to the Library, for example from
a remote HTTP server, this code is passed back to the application. The
application can then decide whether a new request should be established or
not. These two methods return the authentication information required to
issue a new request, that is the new anchor and any list of keywords associated
with this anchor.
*/

extern BOOL HTRequest_addCredentials       (HTRequest * request,
                                            char * token, char * value);
extern BOOL HTRequest_deleteCredentialsAll (HTRequest * request);
extern HTAssocList * HTRequest_credentials (HTRequest * request);

/*
(
  Realms
)

The realm is normally set and used by the authentication filters.
*/

extern BOOL HTRequest_setRealm (HTRequest * request, char * realm);
extern const char * HTRequest_realm (HTRequest * request);
extern BOOL HTRequest_deleteRealm (HTRequest * me);

/*
.
  HTTP Referer Field
.

If this parameter is set then a `Referer: <parent address> can be generated
in the request to the server, see
Referer field in a HTTP Request
*/

extern void HTRequest_setParent (HTRequest *request, HTParentAnchor *parent);
extern HTParentAnchor * HTRequest_parent (HTRequest *request);

/*
.
  Local BEFORE and AFTER Filters
.

The request object may have it's own before and after
filters. These may override or suplement the
global set in HTNet. The request object itself handles
the list element, that is this should not be freed bu the caller.
(
  BEFORE Filters
)

The BEFORE filters are called just after the
request has been passed to the Library but before any request is issued over
the network. A BEFORE can infact stop a request completely from being processed.

  Add a local BEFORE Filter


You can add a local BEFORE filter for a single request so that the
both the local and global BEFORE filters are called or you can replace
the global filters with a local set. Note that the local set can be NULL.
This can be used to effectively disable all BEFORE filters without
unregistering the global ones.
*/

extern BOOL HTRequest_addBefore (HTRequest * request, HTNetBefore * filter,
				 const char * tmplate, void * param,
                                 HTFilterOrder order, BOOL override);
extern HTList * HTRequest_before (HTRequest * request, BOOL * override);

/*

  Delete a Local BEFORE Filter


You can delete a local BEFORE filter explicitly by passing the filter itself
or you can delete all filters which are registered for a certain status code.
*/
extern BOOL HTRequest_deleteBefore (HTRequest * request, HTNetBefore * filter);
extern BOOL HTRequest_deleteBeforeAll (HTRequest * request);

/*
(
  AFTER Filters
)

You can add a local AFTER filter for a single request so that the both the
local and global AFTER filters are called or you can replace the global filters
with a local set. Note that the local set can be NULL. This can be used to
effectively disable all AFTER filters without unregistering the global ones.

AFTER filters can be registered to handle a certain set of return values
from the protocol modules, for example explicitly to handle redirection,
authentication, etc. You can find all the available codes in the HTNet object
description.

  Add a local AFTER Filter

*/

extern BOOL HTRequest_addAfter (HTRequest * request, HTNetAfter * filter,
				const char * tmplate, void * param,
                                int status, HTFilterOrder order,
                                BOOL override);
extern HTList * HTRequest_after (HTRequest * request, BOOL * override);

/*

  Delete an AFTER Filter


You can delete a local AFTER filter explicitly by passing the filter itself
or you can delete all filters which are registered for a certain status code.
*/

extern BOOL HTRequest_deleteAfter (HTRequest * request, HTNetAfter * filter);
extern BOOL HTRequest_deleteAfterStatus (HTRequest * request, int status);
extern BOOL HTRequest_deleteAfterAll (HTRequest * request);

/*
.
  Sending data from App to Network
.

Multiple Request objects can be connected in order to create a
PostWeb for sending data
from one location (source) to another (destination). Request objects are
bound together by connecting the output stream of the source with the input
stream of the destination requst. The connection can be done directly so
that the output from the source is exactly what is sent to the destination
or there can be a conversion between the two streams so that we can do
conversions on the fly while copying data. This is in fact the way we use
for building a proxy server.

The Library supports two ways of posting a data object to a remote destination:
Input comes from a socket descriptor or from memory. In the case where you
want to copy a URL, for example from local file system or
from a remote HTTP server then you must use the
PostWeb design. This model
operates by using at least two request objects which gets linked to eachother
as part of the PostWeb model. However, if you are posting from memory, we
only use one request object to perform the operation. In order to
do this, the application must register a callback function that can be called
when the HTTP client module is ready for accepting data.
be included as part of the body and/or as extra metainformation. In the latter
case you need to register a callback function of the following type using
the methods provided in the next section.
*/

typedef int HTPostCallback (HTRequest * request, HTStream * target);

extern void HTRequest_setPostCallback (HTRequest * request, HTPostCallback * cbf);
extern HTPostCallback * HTRequest_postCallback (HTRequest * request);

/*

The Entity Anchor is either the anchor directly associated with the Request
object or the post anchor associated with the object. The purpose of the
entity anchor is if we are to send data to a remote server then we get the
metainformation using the entity anchor.
*/

extern BOOL HTRequest_setEntityAnchor (HTRequest * request, HTParentAnchor * anchor);
extern HTParentAnchor * HTRequest_entityAnchor (HTRequest * request);

/*
(
  Input Stream
)

The input stream is to be used to put data to the network. Normally
each protocol sets the input stream in order to generate the protocol headers
while making a request.
*/

extern void HTRequest_setInputStream (HTRequest * request, HTStream * input);
extern HTStream *HTRequest_inputStream (HTRequest * request);

/*
(
  Is This Request part of a Post Web? (Deprecated)
)

Check to see if this request object is part of a Post Web.
*/

extern BOOL HTRequest_isPostWeb (HTRequest * request);

/*
(
  Source of a Request
)

A request may have a source in which is another request object that as output
stream has the input stream of this request object.
*/

extern BOOL HTRequest_setSource (HTRequest * request, HTRequest * source);
extern HTRequest * HTRequest_source (HTRequest * request);

/*
.
  Streams From Network to Application
.
(
  Default Output Stream
)

The output stream is to be used to put data down to as they come in
from the network and back to the application. The default value is
NULL which means that the stream goes to the user (display).
*/

extern void HTRequest_setOutputStream (HTRequest *request, HTStream *output);
extern HTStream *HTRequest_outputStream (HTRequest *request);

/*
(
  Default Output Stream Format
)

The desired format of the output stream. This is used in the
stream stack builder to determine which stream
to plug in to deal with the data. If NULL, then
WWW_PRESENT is default value.
*/

extern void HTRequest_setOutputFormat (HTRequest *request, HTFormat format);
extern HTFormat HTRequest_outputFormat (HTRequest *request);

/*
(
  Has Output Stream been Connected to Channel? (Deprecated)
)

Has output stream been connected to the channel? If not then we must free
it explicitly when deleting the request object
*/
extern void HTRequest_setOutputConnected (HTRequest * request, BOOL mode);
extern BOOL HTRequest_outputConnected	 (HTRequest * request);

/*
(
  Default Debug Stream
)

All object bodies sent from the server with status codes different from
200 OK will be put down this stream. This can be used for
redirecting body information in status codes different from "200 OK" to for
example a debug window. If the value is NULL (default) then the stream is
not set up.
*/

extern void HTRequest_setDebugStream (HTRequest *request, HTStream *debug);
extern HTStream *HTRequest_debugStream (HTRequest *request);

/*
(
  Default Debug Stream Format
)

The desired format of the error stream. This can be used to get unconverted
data etc. from the library. The default value if WWW_HTML as
a character based only has one WWW_PRESENT.
*/

extern void HTRequest_setDebugFormat (HTRequest *request, HTFormat format);
extern HTFormat HTRequest_debugFormat (HTRequest *request);

/*
.
  Context Swapping
.

In multi threaded applications it is often required to keep track of the
context of a request so that when the Library returns a result of a request,
it can be put into the context it was in before the request was first passed
to the Library. This call back function allows the application to do this.
*/

typedef int HTRequestCallback (HTRequest * request, void *param);

extern void HTRequest_setCallback (HTRequest *request, HTRequestCallback *cb);
extern HTRequestCallback *HTRequest_callback (HTRequest *request);

/*

The callback function can be passed an arbitrary pointer (the void part)
which can describe the context of the current request structure. If such
context information is required then it can be set using the following methods:
*/

extern void HTRequest_setContext (HTRequest *request, void *context);
extern void *HTRequest_context (HTRequest *request);

/*
.
  Should we Issue a full HTTP Request-URI?
.

In early versions of HTTP, the request sent to the remote server varies whether
we use a proxy or go directly to the origin server. The default value is
OFF but we use a full request if we are talking to a proxy server.
*/

extern void HTRequest_setFullURI (HTRequest *request, BOOL mode);
extern BOOL HTRequest_fullURI (HTRequest *request);

/*
.
  Handling Proxies
.

In case we are using a proxy for this requst then we can register it together
with the request object. That way we can find the proxy and look for
authentication information, for example in the
Authentication filter. The string is freed by
the Request object on deletion. This is normally handled automatically by
the proxy and gateway module
*/

extern BOOL HTRequest_setProxy    (HTRequest * request, const char * proxy);
extern char * HTRequest_proxy     (HTRequest * request);
extern BOOL HTRequest_deleteProxy (HTRequest * request);

/*
.
Message Body Manipulation
.

An application may use these functions to set and manipulate the request
message body.  This message body is specially indicate for methods that use
small XML message bodies. Once the application defines it, this message body
will be send just after the headers. It  does not use
"Expect: 100-continue" header, and the application
should not try to use both.  It's important to remark that
"Expect: 100-continue" header is a very importante feature defined in HTTP.
It's prevents that, for example, the server must read many unnecessary bytes
from request body. Using "Expect: 100-continue" header, your application
safes time and network (see 
RFC2616, section 8.2.3).
Please, if possible, use always HTRequest Entity and entity callback, leave
this only for small XML bodies in extension methods (see 
HTMethod), and when using it, be very
careful!

When using a message body, the application may define its length and
format. If the message body is set and its length is also set and it greater
than 0, a Content-Lenght header will be added to the request. Similarly, if 
the message body and its type are set, a Content-Type header will be added 
to the request too. Otherwise, those headers will not be included.

Note: The caller should free the string returned by
HTRequest_messageBody function!
*/

extern BOOL HTRequest_setMessageBody (HTRequest * request, const char * body);
extern BOOL HTRequest_deleteMessageBody (HTRequest * request);
extern char * HTRequest_messageBody (HTRequest * request);

extern BOOL HTRequest_setMessageBodyLength (HTRequest * request, long int length);
extern long int HTRequest_messageBodyLength (HTRequest * request);

extern BOOL HTRequest_setMessageBodyFormat (HTRequest * request, HTFormat format);
extern HTFormat HTRequest_messageBodyFormat (HTRequest * request);

/*

.
  Bytes Read or Written in a Request
.

This function returns the bytes read in the current request. For a deeper
description of what the current request is, please read the user's guide.
This function can be used in for example the HTAlert
module to give the number of bytes read or written in a progress message.
*/

extern long HTRequest_bodyRead (HTRequest * request);
extern long HTRequest_bodyWritten (HTRequest * request);

/*

You can also get the total number of bytes read or written including the
headers
*/

extern long HTRequest_bytesRead (HTRequest * request);
extern long HTRequest_bytesWritten (HTRequest * request);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTREQ_H */

/*

  

  @(#) $Id$

*/
