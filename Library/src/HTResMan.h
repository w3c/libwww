/*

  
  					W3C Sample Code Library libwww Private Request Definition


!
  Private Response Definition
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module is the private part of the response object. It has the functions
declarations that are private to the Library and that shouldn't be used by
applications. See also the public part of the declarition in the
HTResponse Module.

This module is implemented by HTResMan.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTRESMAN_H
#define HTRESMAN_H

#include "HTResponse.h"
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

struct _HTResponse {

    int                 hash;

/*
(
  Redirection
)

If we get a redirection back then we return the new destination for this
request to the application using this anchor.
*/

    HTAnchor *		redirectionAnchor;		  /* Redirection URL */

/*
(
  Retry Request After
)

If we get a response back saying that we have to retry the request after
a certain amount of time then add this information here. Libwww does not
retry the request automatically - this is for the application to do.
*/

    time_t		retry_after;		 /* Absolut time for a retry */

/*
(
  Access Authentication Information
)

The challenge and the credentials entries are use
by the authentication parsers and generators respectively.
*/

    char *		realm;				    /* Current realm */
    char *		scheme;				   /* Current scheme */

    HTAssocList *	challenge;         /* Challenge received in response */

/*
(
  Connection Control Directives
)

This association list is a list of the connection control directives that
have been received in the response.
*/

    HTAssocList *       connection;

/*
(
  Protocol Extension Protocol (PEP) Information
)

These association lists contain the information that we have received in
PEP headers in the response.
*/

    HTAssocList *	protocol;
    HTAssocList *	protocol_info;
    HTAssocList *	protocol_request;

/*

(
  Cache Control Directives
)

This association list is a list of the cache control directives that have
been received as part of the response. We also keep track of whether the
response si cachable or not.
*/

    HTCachable          cachable;
    BOOL                cached;             /* If anchor has inherited lists */
    HTAssocList *       cache_control;

/*
(
  Range Requests
)

We may get a partial response in which case we register the received ranges
of the resource.
*/

    HTAssocList *       byte_ranges;

/*
(
  Variants
)

The response may be a negotiated response in which case we wanna know. This
has significance for whether the object is cachable or not.
*/

    HTAssocList *       variants;

/*
(
  Content Length
)

We register the content length as thjis is of importance to all responses.
The content length is&nbsp;a common way to figure out how many bytes we can
expect.
*/

    long int	       content_length;

/*
(
  Content Type
)

The Content type is important to know as we set of the stream pipe and do
the format conversion.
*/

    HTFormat           content_type;
    HTAssocList *      type_parameters;    /* Type parameters (charset etc.) */

/*
(
  Content Encoding
)

We register the content-encoding as it may affect the stream pipe as we set
it up.
*/

    HTList *	       content_encoding;

/*
(
  Transfer Encoding
)

The transfer encoding is likewise important when we set up the stream pipe.
*/

    HTList *          transfer_encoding;

/*
(
  Content Transfer Encoding
)

The content transfer encoding is likewise important when we set up the stream pipe.
*/

    HTEncoding         cte;

/*
(
  Trailer
)

The tailers are headers that come at the end
*/

    HTAssocList *      trailer;

/*
(
  Original Reponse Header Values
)

We store the&nbsp;original headers as they may become useful in many ways
- for example in lazy parsing.
*/

    HTAssocList *      headers;

/*

The reason string furnished by the server, as some servers may send
useful custom information in it
*/

    char *             reason;             /* JK: HTTP reason string */

/*
*/

}; /* End of definition of HTResponse */

/*

End of Declaration
*/

#ifdef __cplusplus
}
#endif

#endif /* HTRESMAN_H */

/*

  

  @(#) $Id$

*/
