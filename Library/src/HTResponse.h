/*

  					W3C Sample Code Library libwww Response Class


!
  The Response Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The response object is created as a placeholder for the response received
by the remote server. All filters can then use the information passed in
the response and act appropriately. The response object is deleted automatically
when the corresponding request object is deleted. We try and do some fancy
tricks in order to do lazy parsing and reusing parsed values so that we can
optimize the code.

The Response object is created automatically when we start to receive
metainformation (for example MIME headers) and is
linked to the Request object. The
Response object is also deleted automatically when the corresponding request
object is deleted but it can of course be deleted before if this is
desired.

Note that if you are using non-blocking sockets then libwww
behaves asynchronously as you may issue multiple requests and get back the
responses in the order they appear on the net interface.


This module is implemented by HTResponse.c, and
it is a part of the  W3C Sample Code
Library.
*/

#ifndef HTRESPONSE_H
#define HTRESPONSE_H

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTResponse HTResponse;

#include "HTEvent.h"
#include "HTList.h"
#include "HTAssoc.h"
#include "HTFormat.h"
#include "HTUser.h"

/*
.
  Creating and Deleting Response Objects
.

The response obejct gets created when we start receiving a response from
a remote server.
(
  Create Response Object
)

Creates a new response object with a corresponding User Profile object.
*/

extern HTResponse * HTResponse_new (void);

/*
(
  Delete Response Object
)

This function deletes the object and cleans up the memory.
*/

extern BOOL HTResponse_delete (HTResponse * response);

/*
.
  Redirections
.

When a redirection response is returned to the Library, for example from
a remote HTTP server, this code is passed back to the application. The
application can then decide whether a new request should be established or
not. These two methods return the redirection information required to issue
a new request, that is the new anchor and any list of keywords associated
with this anchor.
*/

extern HTAnchor * HTResponse_redirection (HTResponse * response);
extern BOOL HTResponse_setRedirection (HTResponse * response, HTAnchor * anchor);

/*
.
  Retry Request After
.

Some services, for example HTTP, can in case they are unavailable at the
time the request is issued send back a time and date stamp to the client
telling when they are expected to back online. In case a request results
in a HT_RETRY status, the application can use any time indicated in this
field to retry the request at a later time. The Library does not initiate
any request on its own - it's for the application to do. The time returned
by this function is in calendar time or -1 if not available.
*/

extern time_t HTResponse_retryTime (HTResponse * response);
extern BOOL HTResponse_setRetryTime (HTResponse * response, time_t retry);

/*
.
  HTTP Access Authentication Challanges
.

When a access denied response is returned to the Library, for example from
a remote HTTP server, this code is passed back to the application. The
application can then decide whether a new request should be established or
not. These two methods return the authentication information required to
issue a new request, that is the new anchor and any list of keywords associated
with this anchor.
*/

extern BOOL HTResponse_addChallenge (HTResponse * response, char * token, char * value);

extern BOOL HTResponse_deleteChallengeAll (HTResponse * response);

extern HTAssocList * HTResponse_challenge (HTResponse * response);

/*
(
  Realms
)
*/

extern BOOL HTResponse_setRealm (HTResponse * response, char * realm);
extern const char * HTResponse_realm (HTResponse * response);

/*
(
  Access Authentication Schemes
)
*/

extern BOOL HTResponse_setScheme (HTResponse * response, char * scheme);
extern const char * HTResponse_scheme (HTResponse * response);

/*
.
  HTTP Connection Control Directives
.

The connection control directives are all part of the connection header and
control the behavior of this connection. The respose object contains the
connection information that we have received in the response.
*/

extern BOOL HTResponse_addConnection       (HTResponse * response,
                                            char * token, char * value);
extern BOOL HTResponse_deleteConnectionAll (HTResponse * response);
extern HTAssocList * HTResponse_connection (HTResponse * response);

/*
.
  HTTP Extensions (PEP)
.

HTTP can be extended in several ways but traditionally it has been by using
new headers. Here we present a new idea which provides a framework for describing
extensions and their scope. This is only an idea an may be modified later!
The implementation of the extensions can be found in the
PEP module
(
  Protocol
)

This association list is a list of the extension directives that were received
as part of the response.
*/

extern BOOL HTResponse_addProtocol       (HTResponse * response,
                                          char * token, char * value);
extern BOOL HTResponse_deleteProtocolAll (HTResponse * response);
extern HTAssocList * HTResponse_protocol (HTResponse * response);

/*
(
  Protocol Info
)

This association list is a list of the extension directives that were received
as part of the response.
*/

extern BOOL HTResponse_addProtocolInfo       (HTResponse * response,
                                              char * token, char * value);
extern BOOL HTResponse_deleteProtocolInfoAll (HTResponse * response);
extern HTAssocList * HTResponse_protocolInfo (HTResponse * response);

/*
(
  Protocol Request
)

This association list is a list of the extension directives that were received
as part of the response.
*/

extern BOOL HTResponse_addProtocolRequest       (HTResponse * response,
                                                 char * token, char * value);
extern BOOL HTResponse_deleteProtocolRequestAll (HTResponse * response);
extern HTAssocList * HTResponse_protocolRequest (HTResponse * response);

/*
.
  HTTP Cache Control Directives
.

This association list is a list of the cache control directives that have
been received as part of the response. We also keep track of whether the
response is cachable or not.
*/

extern BOOL HTResponse_addCacheControl       (HTResponse * response,
                                              char * token, char *value);
extern BOOL HTResponse_deleteCacheControlAll (HTResponse * response);
extern HTAssocList * HTResponse_cacheControl (HTResponse * response);

/*
(
  Is the response Cachable?
)

The various cache-control headers and directives decides whether
an object is cachable or not. Check these methods before starting caching!
*/

typedef enum _HTCachable {
    HT_NO_CACHE            = 0,
    HT_CACHE_ALL           = 1,
    HT_CACHE_ETAG          = 2,
    HT_CACHE_NOT_MODIFIED  = 3
} HTCachable; 

extern HTCachable HTResponse_isCachable  (HTResponse * me);
extern BOOL HTResponse_setCachable (HTResponse * me, HTCachable mode);

/*
(
  The Response Enity Tag (etag)
)
Return the etag if any
*/

extern char * HTResponse_etag (HTResponse * me);

/*

(
  Has the Response been Cached?
)

When we gc the response object we need to know whether the header lists have
been inherited by other objects (the anchor object,
for example) and hence shouldn't be deleted. We can tell the response object
this by using the following method call
*/

extern BOOL HTResponse_isCached (HTResponse * me, BOOL mode);

/*
(
  Some Cache-Control helper Methods
)

Some useful helper functions for handling specific response cache directives
*/

extern time_t HTResponse_maxAge              (HTResponse * response);
extern BOOL   HTResponse_mustRevalidate      (HTResponse * response);
extern char * HTResponse_noCache             (HTResponse * response);

/*
(
  Partial responses and Range Retrievals
)

Libwww can issue range requests in case we have already obtained a part of
the entity body. Since all HTTP entities are represented in HTTP messages
as sequences of bytes, the concept of a byte range is meaningful for any
HTTP entity. (However, not all clients and servers need to support byte-range
operations.) Byte range specifications in HTTP apply to the sequence of bytes
in the entity-body (not necessarily the same as the message-body). A byte
range operation may specify a single range of bytes, or a set of ranges within
a single entity.
*/

extern BOOL HTResponse_addRange       (HTResponse * response,
                                       char * unit, char * range);
extern BOOL HTResponse_deleteRangeAll (HTResponse * response);
extern HTAssocList * HTResponse_range (HTResponse * response);

/*
(
  Content Length
)

We store the content length so that we have an idea of how many bytes to
expect.
*/

extern long int HTResponse_length (HTResponse * response);
extern void HTResponse_setLength  (HTResponse * response, long int length);
extern void HTResponse_addLength  (HTResponse * response, long int deltalength);

/*
(
  Content Type (Media type)
)

We store the content-type of the response along with the charset, level and
all other paramaters that may follow the content-type itself.
*/

extern HTFormat HTResponse_format (HTResponse * response);
extern void HTResponse_setFormat  (HTResponse * response, HTFormat format);

/*

The Response object stores all content type parameters (charset, level, etc.)
in an Association list so here you will always be able to find them. We also
have a few methods for the special cases: charset and
level as they are often needed.
*/

extern HTAssocList * HTResponse_formatParam (HTResponse * response);
extern BOOL HTResponse_addFormatParam  (HTResponse * response,
					const char * name, const char * value);

/*

  Charset parameter to Content-Type

*/

extern HTCharset HTResponse_charset (HTResponse * response);
extern BOOL HTResponse_setCharset   (HTResponse * response, HTCharset charset);

/*

  Level parameter to Content-Type

*/

extern HTLevel HTResponse_level (HTResponse * response);
extern BOOL HTResponse_setLevel (HTResponse * response, HTLevel level);

/*
(
  Content Encodings
)

The list of encodings that we have received in the response.
*/

extern HTList * HTResponse_encoding (HTResponse * response);
extern BOOL HTResponse_addEncoding  (HTResponse * response, HTEncoding enc);

/*
(
  Transfer Encodings
)

The list of encodings that we have received in the response.
*/

extern HTList * HTResponse_transfer (HTResponse * response);
extern BOOL HTResponse_addTransfer  (HTResponse * response, HTEncoding te);

/*
(
  Content Transfer Encodings
)

Any transfer encoding that we have received in the response.
*/

extern HTEncoding HTResponse_contentTransferEncoding (HTResponse * response);
extern BOOL HTResponse_setContentTransferEncoding (HTResponse * response,
                                                   HTEncoding cte);

/*
(
  Vary Headers
)

Any Vary header fields
*/

extern BOOL HTResponse_addVariant (HTResponse * me, char * token, char * value);
extern BOOL HTResponse_deleteVariantAll (HTResponse * me);
extern HTAssocList * HTResponse_variant (HTResponse * me);

/*
(
  Trailers
)

Any trailer header fields
*/

extern BOOL HTResponse_addTrailer (HTResponse * me, char * token, char * value);
extern BOOL HTResponse_deleteTrailerAll (HTResponse * me);
extern HTAssocList * HTResponse_trailer (HTResponse * me);

/*

(
  Original Response Headers
)

The MIME parser may add the original response headers
as (name,value) pairs. The information may be picked up by the
persistent cache manager.
*/

extern BOOL HTResponse_addHeader       (HTResponse * response,
                                        char * token, char * value);
extern BOOL HTResponse_deleteHeaderAll (HTResponse * response);
extern HTAssocList * HTResponse_header (HTResponse * response);

extern HTAssocList * HTResponse_handOverHeader (HTResponse * me);

/*

(
  The HTTP reason string
)
The string returned in the HTTP status line. Some servers send custom
info in this string and applications may want to show it.
*/

extern char * HTResponse_reason (HTResponse * me);
extern BOOL HTResponse_setReason (HTResponse * me, char * reason);

/*

*/

#ifdef __cplusplus
}
#endif

#endif /* HTRESPONSE_H */

/*

  

  @(#) $Id$

*/
