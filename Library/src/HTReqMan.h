/*

  
  					W3C Sample Code Library libwww Private Request Definition


!
  Private Request Definition
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module is the private part of the request object. It has the functions
declarations that are private to the Library and that shouldn't be used by
applications. The module has been separated from the old HTAccess module.
See also the public part of the declarition in the HTReq
Module.

This module is implemented by HTReqMan.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTREQMAN_H
#define HTREQMAN_H

#include "HTReq.h"
#include "HTList.h"
#include "HTFormat.h"
#include "HTAnchor.h"
#include "HTMethod.h"
#include "HTAABrow.h"
#include "HTStream.h"
#include "HTNet.h"
#include "HTMIMPrs.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*

When a request is handled, all kinds of things about it need to be passed
along together with a request. It is intended to live as long as the request
is still active, but can be deleted as soon as it has terminated. Only the
anchor object stays around after the request itself is terminated.
*/

struct _HTRequest {

    BOOL		internal;      /* Does the app knows about this one? */

    time_t		date;      /* Time stamp when the request was issued */

    HTMethod		method;

    BOOL                flush;                /* Should we flush immediately */

    HTPriority		priority;		/* Priority for this request */

/*
(
Message body
)

A request message body indicated for XML bodies. It is used for extension
methods. See HTReq.html for details.
*/

#ifdef HT_EXT
    char *             messageBody;
    long int           messageBodyLength;
    HTFormat           messageBodyFormat;
#endif

/*
(
  User Profile
)

Each request can be assigned a user profile containing
information about this host and the user issuing the request.
*/

    HTUserProfile *	userprofile;

/*
(
  Net Object
)

Each request is assigned a Net object which contains pointers to other objects
handling the request.
*/

    HTNet *		net;		    /* Information about socket etc. */

/*
(
  Response Object
)

When we start getting MIME headers in as a response we keep it in this object
until we know what to do with it.
*/

    HTResponse *        response;

/*
(
  Error Manager
)
*/

    HTList *		error_stack;		           /* List of errors */

/*
(
  Have many times do We Want to Try?
)
*/

    int			retrys;       	      /* Number of automatic reloads */
    int                 max_forwards;
    int			AAretrys;      	      /* Number of authentication
                                                 retries */

/*
(
  Preemptive or Non-Preemptive load?
)

Each protocol module is registered with a default behavior but if you have
registered a protocol module for non-preemtive load you can override this
by using the following flag.
*/

    BOOL		preemptive;

/*
(
  Content Negotiation
)

Normally, when we access the local file system we do content negotiation
in order to find the most suited representation. However, you can turn this
off by using the following flag.
*/

    BOOL		ContentNegotiation;

/*
(
  Should we use preconditions?
)
*/

    HTPreconditions     preconditions;

/*
(
  Headers and header information
)

These are the masks that decides what headers to send.
*/

    HTGnHd		GenMask;
    HTRsHd		ResponseMask;
    HTRqHd		RequestMask;
    HTEnHd		EntityMask;

/*
(
  Local MIME Header Parsers
)

Each request can be assigned its own MIME header parsers.
*/

    HTMIMEParseSet *	parseSet;
    BOOL		pars_local;

/*
(
  Accept headers
)

These are the accept headers that we want to send out.
*/

    HTList *		conversions;
    BOOL		conv_local;

    HTList *		encodings;
    BOOL		enc_local;

    HTList *		tes;
    BOOL		te_local;

    HTList *		languages;
    BOOL		lang_local;

    HTList *		charsets;
    BOOL		char_local;

    HTList *		befores;
    BOOL		befores_local;

    HTList *		afters;
    BOOL		afters_local;

/*
(
  Are we using a Proxy?
)

If so then we keep the name in this variable
*/

    char * 		proxy;
    BOOL                full_uri;

/*
(
  Cache Control Directives
)

This association list is a list of the cache control directives that are
to be sent as part of the Cache-Control header.
*/

    HTReload		reload;
    HTAssocList *       cache_control;

/*
(
  Default PUT name
)

Stores the default name when publishing to a "/" URL.
*/

   char *               default_put_name;

/*
(
  Byte Ranges
)

This association list is a list of the cache control directives that are
to be sent as part of the Range header.
*/

    HTAssocList *       byte_ranges;

/*
(
  Connection Control Directives
)

This association list is a list of the connection control directives that
are to be sent as part of the Connection header.
*/

    HTAssocList *       connection;

/*
(
  Expect Directives
)

The Expect request-header field is used to indicate that particular server
behaviors are required by the client. A server that does not understand or
is unable to comply with any of the expectation values in the Expect field
of a request MUST respond with appropriate error status.
*/

    HTAssocList *       expect;

/*
(
  Access Authentication Information
)

The credentials list contains the information that we are to
send as part of the Authorization header. The realm is if we
already know that we need to generate credentials for a specific realm.
*/

    char *		realm;				    /* Current realm */
    HTAssocList *	credentials;	   /* Credentials received by server */

/*
(
  Request Header Extensibility
)

  

  1) Simple Association List


Add the (name, value) and it will be converted into MIME header format as
name: value. DO NOT ADD CRLF line termination - this is done
by the HTTP header generator stream
*/

    HTAssocList	*	extra_headers;

/*

  2) Stream Oriented Header Generators


A generator is a stream with direct access to the output stream
*/

    HTList *		generators;
    BOOL		gens_local;

/*

  4) HTTP Extension Framework


These association lists contain the information that we are to send as
HTTP Extension Framework.
*/

    HTAssocList *	mandatory;
    HTAssocList *	optional;

/*
(
  Anchors
)
*/

    HTParentAnchor *	anchor;	       /* The Client anchor for this request */

    HTChildAnchor *	childAnchor;	    /* For element within the object */
    HTParentAnchor *	parentAnchor;			/* For referer field */

/*
(
  Streams From Network to Application
)
*/

    HTStream *		output_stream; 
    HTStream *		orig_output_stream; 
    HTFormat		output_format;
    BOOL		connected;

    HTStream *		debug_stream;
    HTStream *		orig_debug_stream;
    HTFormat		debug_format;

/*
(
  Streams From Application to Network
)
*/

    HTStream *		input_stream; 
    HTFormat		input_format;

/*
(
  Callback Function for getting data down the Input Stream
)
*/

    HTPostCallback *	PostCallback;

/*
(
  Context Swapping
)
*/

    HTRequestCallback *	callback;
    void *		context;

/*
(
  PostWeb Information (Not used anymore - don't use!)
)
*/

    HTRequest *		source;		     /* Source for request or itself */
    HTParentAnchor *	source_anchor;		  /* Source anchor or itself */

    HTRequest *		mainDestination;	     /* For the typical case */
    HTList *		destinations;		 /* List of related requests */
    int			destRequests;	   /* Number of destination requests */
    int			destStreams;	    /* Number of destination streams */

/*
*/

};

/*
.
  Post Web Management
.

These functions are mainly used internally in the Library but there is no
reason for them not to be public.
*/

extern BOOL HTRequest_addDestination (HTRequest * src, HTRequest * dest);
extern BOOL HTRequest_removeDestination	(HTRequest * dest);
extern BOOL HTRequest_destinationsReady (HTRequest * me);

extern BOOL HTRequest_linkDestination (HTRequest * dest);
extern BOOL HTRequest_unlinkDestination (HTRequest * dest);

extern BOOL HTRequest_removePostWeb (HTRequest * me);
extern BOOL HTRequest_killPostWeb (HTRequest * me);

#define	HTRequest_mainDestination(me) \
	((me) && (me)->source ? (me)->source->mainDestination : NULL)
#define HTRequest_isDestination(me) \
	((me) && (me)->source && (me) != (me)->source)
#define HTRequest_isMainDestination(me) \
	((me) && (me)->source && \
	(me) == (me)->source->mainDestination)
#define HTRequest_isSource(me) \
	((me) && (me)->source && (me) == (me)->source)

/*

End of Declaration
*/

#ifdef __cplusplus
}
#endif

#endif /* HTREQMAN_H */

/*

  

  @(#) $Id$

*/
