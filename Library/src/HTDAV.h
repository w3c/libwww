/*

  					W3C Sample Code Library libwww WebDAV Access Methods



!
W3C Sample Code Library libwww WebDAV Access Methods
!
*/

/*
**      (c) COPYRIGHT MIT 1995.
**      Please first read the full copyright statement in the file COPYRIGH.
*/

/*


This module contains methods for accessing URIs using WebDAV methods. It
also contains functions for headers definition.


This module is implemented by HTDAV.c and it is a
part of the W3C Sample
CodeLibrary.
*/

#ifndef HTDAV_H
#define HTDAV_H

#ifdef __cplusplus
extern "C" { 
#endif 

#ifdef HT_DAV

/*

.
WebDAV HEADERS
.


WebDAV extension protocol defines new headers to be used in its requests.
See RFC2518 for details.
*/

typedef struct _HTDAVHeaders HTDAVHeaders;

extern HTDAVHeaders * HTDAVHeaders_new (void);
extern BOOL HTDAVHeaders_delete (HTDAVHeaders *me);

/*

(
If Header
)


Manipulates the "If" header, which describes a series of state lists. The
caller must assure that the parameter "If" is well-formed. Below, you can see
a small description of If header format. See section 9.4 of RFC2518 for details.


If = "If" ":" ( 1*NOTAGGED | 1*TAGGED )


NOTAGGED = LIST


TAGGED = CODED-URL 1*LIST


LIST = "(" 1*( ["Not"] (STATE-TOKEN | "[" ENTITY-TAG "]" ) ) ")"


STATE-TOKEN = CODED-URL


CODED-URL = "<" AbsoluteURI ">"


Note: The caller should free the strings returned by
HTDAV_ifHeader  method.
*/

extern BOOL HTDAV_setIfHeader (HTDAVHeaders *me, const char *If);
extern BOOL HTDAV_deleteIfHeader (HTDAVHeaders * me);
extern char * HTDAV_ifHeader (HTDAVHeaders *me);

/*

(
Depth Header
)


Manipulates the "Depth" header. Depth header is used with methods executed
on resource which could have internal members (Collections) to indicate
whether the method should be applied to the resource children.


The caller must assure that the parameter "Depth" is "0", "1" or
"infinity", and that its value can be applied in the used resquest method
(for example, LOCK method does not support Depth value 1).


Note: The caller should free the string returned by
HTDAV_DepthHeader method.
*/

extern BOOL HTDAV_setDepthHeader (HTDAVHeaders *me, const char *Depth);
extern BOOL HTDAV_deleteDepthHeader (HTDAVHeaders * me);
extern char * HTDAV_DepthHeader (HTDAVHeaders *me);

/*

(
Lock-Tocken header
)


Manipulates the "LockToken" header. It is used in UNLOCK method to
identify the lock to be removed. The caller must assure that the parameter is
a state token well-formed (
RFC2518 section 9.5).


Note: The caller should free the string returned by
HTDAV_LockTokenHeader method.
*/

extern BOOL HTDAV_setLockTokenHeader (HTDAVHeaders *me, const char *LockToken);
extern BOOL HTDAV_deleteLockTokenHeader (HTDAVHeaders * me);
extern char * HTDAV_LockTokenHeader (HTDAVHeaders *me);

/*

(
Destination Header
)


Manipulates the "Destination" header. It is used in COPY and MOVE methods
to identify a destination resource. The caller must assure that the parameter
is an absolute URI.


Note: The caller should free the string returned by
HTDAV_DestinationHeader method.
*/

extern BOOL HTDAV_setDestinationHeader (HTDAVHeaders *me, const char *Destination);
extern BOOL HTDAV_deleteDestinationHeader (HTDAVHeaders * me);
extern char * HTDAV_DestinationHeader (HTDAVHeaders *me);

/*

(
Timeout Header
)


Manipulates the "Timeout" header. It is used in LOCK requests to indicate
the desired timeout value for the requested lock. However, according to the
RFC2518, the server is not
required to honor this value.


The caller must assure that the parameter follows the specification in the
section 9.8 of RFC 2518:


Timeout = "Timeout" ":" 1#TIMETYPE


TIMETYPE = ( "Second-" VAL | "Infinite" | OTHER )


VAL = 1*digit


OTHER = "Extend" Field ; 
RFC2068 - section 4.2


Note: The caller should free the string returned by
HTDAV_TimeoutHeader  method.
*/

extern BOOL HTDAV_setTimeoutHeader (HTDAVHeaders *me, const char *Timeout);
extern BOOL HTDAV_deleteTimeoutHeader (HTDAVHeaders * me);
extern char * HTDAV_TimeoutHeader (HTDAVHeaders *me);

/*

(
Overwrite Header
)


Manipulates the "Overwrite" header. It is used in COPY and MOVE methods to
specify whether the server should overwrite a destination resource.
*/

extern BOOL HTDAV_setOverwriteHeader (HTDAVHeaders *me, BOOL Overwrite);
extern BOOL HTDAV_deleteOverwriteHeader (HTDAVHeaders * me); 
extern BOOL HTDAV_OverwriteHeader (HTDAVHeaders * me); 

/*

.
WebDAV REQUESTS
.


WebDAV extension protocol defines new methods: LOCK, UNLOCK, MOVE, COPY,
MKCOL, PROPFIND, PROPPATCH (See 
RFC2518 for details).

(
LOCK Requests
)


A LOCK request create or refresh a lock over the destiny URI. If it wants
to create a new lock, the request should have a XML body (parameter
"xmlbody"), but if it is a refresh request, this body may be NULL and the
header "If" should be set in HTDAVHeaders object.
*/

extern BOOL HTLOCKDocumentAnchor(HTRequest * request, HTAnchor * dst,
                                 HTParentAnchor *xmlbody, HTDAVHeaders *headers);
extern BOOL HTLOCKAnchor (HTRequest * request,HTAnchor * dst,
                                 char * xmlbody, HTDAVHeaders * headers); 
extern BOOL HTLOCKAbsolute (HTRequest * request, const char * uri,
                                 char * xmlbody, HTDAVHeaders * headers);
extern BOOL HTLOCKRelative (HTRequest * request, const char * relative,
                                 HTParentAnchor * base, char * xmlbody,
                                 HTDAVHeaders * headers);

/*

(
UNLOCK Requests
)


An UNLOCK request removes a lock from the destiny URI. The request must
contain the Lock-Token header set in HTDAVHeaders object (so, the
HTDAVHeaders * headers parameter can't be NULL).
*/

extern BOOL HTUNLOCKAnchor (HTRequest * request, HTAnchor * dst,
                            HTDAVHeaders * headers);
extern BOOL HTUNLOCKAbsolute (HTRequest * request, const char * uri,
                            HTDAVHeaders * headers);
extern BOOL HTUNLOCKRelative (HTRequest * request, const char * relative,
                            HTParentAnchor * base, HTDAVHeaders * headers);

/*

(
PROPFIND Requests
)


PROPFIND requests returns properties defined for the resource. The request
may contain xml entity body with a "propfind" element, which may include an
"allprop" element (to get all properties), a "propname" element (the name of
all properties defined), and a "prop" element containing the desired
properties.
*/

extern BOOL HTPROPFINDAnchor (HTRequest * request, HTAnchor * dst,
                          const char * xmlbody, HTDAVHeaders * headers);
extern BOOL HTPROPFINDDocumentAnchor (HTRequest * request, HTAnchor * dst,
                          HTParentAnchor * xmlbody, HTDAVHeaders * headers);
extern BOOL HTPROPFINDAbsolute (HTRequest * request, const char * uri,
                          const char * xmlbody, HTDAVHeaders * headers);
extern BOOL HTPROPFINDRelative (HTRequest * request, const char * relative,
                          HTParentAnchor * base, const char * xmlbody,
                          HTDAVHeaders * headers);

/*

(
PROPPATCH Requests
)


PROPPATCH requests sets or removes properties defined for the resource.
The request MUST contain xml message body (parameter xmlbody) with a
"propertyupdate" element, which may include a "set" element (to set the
properties) or a "remove" element (to remove the properties).
*/

extern BOOL HTPROPPATCHAnchor (HTRequest * request, HTAnchor * dst,
                         const char * xmlbody,HTDAVHeaders * headers);
extern BOOL HTPROPPATCHDocumentAnchor (HTRequest * request,HTAnchor * dst,
                         HTParentAnchor * xmlbody,HTDAVHeaders * headers);
extern BOOL HTPROPPATCHAbsolute (HTRequest * request, const char * uri,
                         const char * xmlbody, HTDAVHeaders * headers);
extern BOOL HTPROPPATCHRelative (HTRequest * request, const char * relative,
                         HTParentAnchor * base, const char * xmlbody,
                         HTDAVHeaders * headers);

/*

(
MKCOL Requests
)


MKCOL requests are used to create Collections. The resource indicated by
the Request-URI (parameters HTAnchor *dst or char *absolute/relative) MUST
not exist, but all the resource's ancestros MUST exist.
*/

extern BOOL HTMKCOLAnchor (HTRequest * request, HTAnchor * dst,
                         HTDAVHeaders * headers);
extern BOOL HTMKCOLAbsolute (HTRequest * request, const char * uri,
                         HTDAVHeaders * headers);
extern BOOL HTMKCOLRelative (HTRequest * request, const char * relative,
                         HTParentAnchor * base, HTDAVHeaders * headers);

/*

(
COPY Requests
)


COPY requests copies the Request-URI (parameters HTAnchor *src or char
*absolute/relative) to the resource indicated in Destinarion header. The
HTDAVHeaders *headers parameter MUST be a non-null object and it MUST have
the Destination header set. Other headers may also be used, like Depth (0 or
infinity), If and Overwrite headers.
*/

extern BOOL HTCOPYAnchor (HTRequest * request, HTAnchor * src,
                        const char * xmlbody, HTDAVHeaders * headers);
extern BOOL HTCOPYDocumentAnchor (HTRequest * request, HTAnchor * src,
                        HTParentAnchor * xmlbody, HTDAVHeaders * headers);
extern BOOL HTCOPYAbsolute (HTRequest * request, const char * uri,
                        const char * xmlbody, HTDAVHeaders * headers);
extern BOOL HTCOPYRelative (HTRequest * request, const char * relative,
                        HTParentAnchor * base, const char * xmlbody,
                        HTDAVHeaders * headers);

/*

(
MOVE Requests
)


MOVE requests moves the Request-URI (parameters HTAnchor *src or char
*absolute/relative) to the resource indicated in Destinarion header. The
HTDAVHeaders *headers parameter MUST be a non-null object and it MUST have
the Destination header set. Other headers may also be used, like Depth ("0"
or "infinity" - if the resource is a Collection, Depth must only be
"infinity"), If and Overwrite headers.
*/

extern BOOL HTMOVEAnchor (HTRequest * request, HTAnchor * src,
                        const char * xmlbody, HTDAVHeaders * headers);
extern BOOL HTMOVEDocumentAnchor (HTRequest * request, HTAnchor * src,
                        HTParentAnchor * xmlbody, HTDAVHeaders * headers);
extern BOOL HTMOVEAbsolute (HTRequest * request, const char * uri,
                        const char * xmlbody, HTDAVHeaders * headers);
extern BOOL HTMOVERelative (HTRequest * request, const char * relative,
                        HTParentAnchor * base, const char * xmlbody,
                        HTDAVHeaders * headers);

#endif /* HT_DAV */

#ifdef __cplusplus
}
#endif

#endif /* HTDAV_H */

/*




  $Id$




*/
