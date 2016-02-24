/*

  
  					W3C Sample Code Library libwww Default MIME/RFC822 Header Parsers


!
  Default MIME/RFC822 Header Parsers
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Library's MIME parser is divided into two parts: A generic MIME parser
that knows how to unwrap all RFC822 headers and specialized header parsers
that knows how to parse Content-Length, for example. This is
the default set of the specialized MIME header parsers that can be registered
as part of the generic MIME. Note that these functions are not registered
by default - they must be registered by the application. This can be done
using the HTMIMEInit() function in the
WWWInit interface. Of course this can also be
used to register new headers that are not represented below - or if you want
to replace a default parser then this is also very easy.

This module is implemented by HTMIMImp.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTMIMIMP_H
#define HTMIMIMP_H

#include "HTReq.h"
#include "HTHeader.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
*/

extern HTParserCallback 
	HTMIME_accept, 
	HTMIME_acceptCharset, 
	HTMIME_acceptEncoding, 
	HTMIME_acceptLanguage, 
	HTMIME_acceptRanges, 
	HTMIME_authenticate,
	HTMIME_authenticationInfo,
	HTMIME_authorization,
	HTMIME_cacheControl,
	HTMIME_connection, 
	HTMIME_contentEncoding, 
	HTMIME_contentLength,
	HTMIME_contentRange,
	HTMIME_contentTransferEncoding, 
	HTMIME_contentType,
	HTMIME_keepAlive,
	HTMIME_link,
	HTMIME_location,
	HTMIME_maxForwards, 
	HTMIME_messageDigest,
	HTMIME_pragma,
	HTMIME_protocol,
	HTMIME_protocolInfo,
	HTMIME_protocolRequest,
	HTMIME_proxyAuthorization,
	HTMIME_proxyAuthenticationInfo,
	HTMIME_public,
	HTMIME_range,
	HTMIME_referer,
	HTMIME_retryAfter, 
	HTMIME_server, 
        HTMIME_trailer,
	HTMIME_transferEncoding,
	HTMIME_upgrade,
	HTMIME_userAgent,
	HTMIME_vary,
	HTMIME_via,
	HTMIME_warning;

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTMIMIMP_H */

/*

  

  @(#) $Id$

*/
