/*
**	RESPONSE MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** Authors
**	HFN	Henrik Frystyk, frystyk@w3.org
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTHeader.h"
#include "HTLib.h"
#include "HTWWWStr.h"
#include "HTResMan.h"					 /* Implemented here */

/* --------------------------------------------------------------------------*/
/*			Create and delete the HTResponse Object		     */
/* --------------------------------------------------------------------------*/

PUBLIC HTResponse * HTResponse_new (void)
{
    HTResponse * me;
    if ((me = (HTResponse *) HT_CALLOC(1, sizeof(HTResponse))) == NULL)
	HT_OUTOFMEM("HTResponse_new()");
    
    /* Default content-* values */
    me->content_type = WWW_UNKNOWN;
    me->content_length = -1;

    /* Default retry after value */
    me->retry_after = -1;

    /* By default a response is not cachable */
    me->cachable = NO;

    HTTRACE(CORE_TRACE, "Response.... Created %p\n" _ me);
    return me;
}

PUBLIC BOOL HTResponse_delete (HTResponse * me)
{
    if (me) {
	HTTRACE(CORE_TRACE, "Response.... Delete %p\n" _ me);

	/* Access Authentication */
	HT_FREE(me->realm);
	HT_FREE(me->scheme);
	if (me->challenge) HTAssocList_delete(me->challenge);

	/* Connection headers */
	if (me->connection) HTAssocList_delete(me->connection);

	/* PEP Information */
	if (me->protocol) HTAssocList_delete(me->protocol);
	if (me->protocol_request) HTAssocList_delete(me->protocol_request);
	if (me->protocol_info) HTAssocList_delete(me->protocol_info);

	/* Cache control headers */
	if (me->cache_control) HTAssocList_delete(me->cache_control);

	/* Byte ranges */
	if (me->byte_ranges) HTAssocList_delete(me->byte_ranges);

	/* Transfer Encodings */
	if (me->transfer_encoding) HTList_delete(me->transfer_encoding);

	/* Trailers */
	if (me->trailer) HTAssocList_delete(me->trailer);

	/* Variants */
	if (me->variants) HTAssocList_delete(me->variants);

	/*
	** Only delete Content Type parameters and original headers if the
	** information is not used elsewhere, for example by the anchor
	** object.
	*/
	if (!me->cached) {

	    /* Content type parameters */
	    if (me->type_parameters) HTAssocList_delete(me->type_parameters);

	    /* Content Encodings */
	    if (me->content_encoding) HTList_delete(me->content_encoding);

	    /* List of all headers */
	    if (me->headers) HTAssocList_delete(me->headers);
	}

	/* HTTP reason string */
	if (me->reason)  HT_FREE (me->reason);

 	HT_FREE(me);
	return YES;
    }
    return NO;
}

/* --------------------------------------------------------------------------*/
/*			Methods on the HTResponse Object		     */
/* --------------------------------------------------------------------------*/

/*
**    Redirection information
*/
PUBLIC HTAnchor * HTResponse_redirection (HTResponse * me)
{
    return (me ? me->redirectionAnchor : NULL);
}

PUBLIC BOOL HTResponse_setRedirection (HTResponse * me, HTAnchor * anchor)
{
    if (me && anchor) {
	me->redirectionAnchor = (HTAnchor *) HTAnchor_parent(anchor);
	return YES;
    }
    return NO;
}

/*
**	When to retry a response if HT_RETRY
**	Returns -1 if not available
*/
PUBLIC time_t HTResponse_retryTime (HTResponse * me)
{
    return me ? me->retry_after : -1;
}

PUBLIC BOOL HTResponse_setRetryTime (HTResponse * me, time_t retry)
{
    if (me) {
	me->retry_after = retry;
	return YES;
    }
    return NO;
}

/*
**  Access Authentication Challenges
*/
PUBLIC BOOL HTResponse_addChallenge (HTResponse * me,
				    char * token, char * value)
{
    if (me) {
	if (!me->challenge) me->challenge = HTAssocList_new();
	return HTAssocList_addObject(me->challenge, token, value);
    }
    return NO;
}

PUBLIC BOOL HTResponse_deleteChallengeAll (HTResponse * me)
{
    if (me && me->challenge) {
	HTAssocList_delete(me->challenge);
	me->challenge = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTResponse_challenge (HTResponse * me)
{
    return (me ? me->challenge : NULL);
}

/*
**  Access Authentication Realms
*/
PUBLIC BOOL HTResponse_setRealm (HTResponse * me, char * realm)
{
    if (me && realm) {
	StrAllocCopy(me->realm, realm);
	return YES;
    }
    return NO;
}

PUBLIC const char * HTResponse_realm (HTResponse * me)
{
    return (me ? me->realm : NULL);
}

/*
**  Access Authentication Schemes
*/
PUBLIC BOOL HTResponse_setScheme (HTResponse * me, char * scheme)
{
    if (me && scheme) {
	StrAllocCopy(me->scheme, scheme);
	return YES;
    }
    return NO;
}

PUBLIC const char * HTResponse_scheme (HTResponse * me)
{
    return (me ? me->scheme : NULL);
}

/*
**  Connection Directives
*/
PUBLIC BOOL HTResponse_addConnection (HTResponse * me,
				      char * token, char * value)
{
    if (me) {
	if (!me->connection) me->connection = HTAssocList_new();
	return HTAssocList_replaceObject(me->connection, token, value);
    }
    return NO;
}

PUBLIC BOOL HTResponse_deleteConnectionAll (HTResponse * me)
{
    if (me && me->connection) {
	HTAssocList_delete(me->connection);
	me->connection = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTResponse_connection (HTResponse * me)
{
    return (me ? me->connection : NULL);
}

/*
**  PEP Protocol header
*/
PUBLIC BOOL HTResponse_addProtocol (HTResponse * me,
				    char * token, char * value)
{
    if (me) {
	if (!me->protocol) me->protocol = HTAssocList_new();
	return HTAssocList_addObject(me->protocol, token,value);
    }
    return NO;
}

PUBLIC BOOL HTResponse_deleteProtocolAll (HTResponse * me)
{
    if (me && me->protocol) {
	HTAssocList_delete(me->protocol);
	me->protocol = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTResponse_protocol (HTResponse * me)
{
    return (me ? me->protocol : NULL);
}

/*
**  PEP Protocol Info header
*/
PUBLIC BOOL HTResponse_addProtocolInfo (HTResponse * me,
					char * token, char * value)
{
    if (me) {
	if (!me->protocol_info) me->protocol_info = HTAssocList_new();
	return HTAssocList_addObject(me->protocol_info, token,value);
    }
    return NO;
}

PUBLIC BOOL HTResponse_deleteProtocolInfoAll (HTResponse * me)
{
    if (me && me->protocol_info) {
	HTAssocList_delete(me->protocol_info);
	me->protocol_info = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTResponse_protocolInfo (HTResponse * me)
{
    return (me ? me->protocol_info : NULL);
}

/*
**  PEP Protocol request header
*/
PUBLIC BOOL HTResponse_addProtocolRequest (HTResponse * me,
					   char * token, char * value)
{
    if (me) {
	if (!me->protocol_request) me->protocol_request = HTAssocList_new();
	return HTAssocList_addObject(me->protocol_request, token,value);
    }
    return NO;
}

PUBLIC BOOL HTResponse_deleteProtocolRequestAll (HTResponse * me)
{
    if (me && me->protocol_request) {
	HTAssocList_delete(me->protocol_request);
	me->protocol_request = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTResponse_protocolRequest (HTResponse * me)
{
    return (me ? me->protocol_request : NULL);
}

/*
**  Cache control directives received in the response
*/
PUBLIC BOOL HTResponse_deleteCacheControlAll (HTResponse * me)
{
    if (me && me->cache_control) {
	HTAssocList_delete(me->cache_control);
	me->cache_control = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTResponse_cacheControl (HTResponse * me)
{
    return (me ? me->cache_control : NULL);
}

PUBLIC BOOL HTResponse_addCacheControl (HTResponse * me,
				       char * token, char * value)
{
    if (me) {
	if (!me->cache_control)
	    me->cache_control=HTAssocList_new();
	return HTAssocList_replaceObject(me->cache_control,
					 token, value);
    }
    return NO;
}

/*
**  Check whether we can cache this object or not.
*/
PUBLIC HTCachable HTResponse_isCachable (HTResponse * me)
{
    if (me) {

	/* We may already have decided that this object is not cachable */
	if (me->cachable == HT_NO_CACHE) return HT_NO_CACHE;

#if 0
	/*  We don't cache negotiated resources for the moment */
	if (me->variants) return HT_NO_CACHE;
#endif

	/*
	**  Check if we should cache this object or not. We are very liberale
	**  in that we cache everything except if we explicit are told not to
	**  cache (no-store, no-cache). In all other cases we can get around
	**  it by forcing revalidation
	*/
	if (me->cache_control) {
	    char * token;
	    if ((token=HTAssocList_findObject(me->cache_control, "no-store")))
		return HT_NO_CACHE;
	    if ((token=HTAssocList_findObject(me->cache_control, "no-cache")))
		if (!*token) return HT_NO_CACHE;
	}

	/* Cache everything else */
	return me->cachable;
    }
    return HT_NO_CACHE;
}

PUBLIC BOOL HTResponse_setCachable (HTResponse * me, HTCachable mode)
{
    if (me) {
	me->cachable = mode;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTResponse_isCached (HTResponse * me, BOOL mode)
{
    if (me) {
	me->cached = mode;
	return YES;
    }
    return NO;
}

PUBLIC time_t HTResponse_maxAge (HTResponse * me)
{
    if (me && me->cache_control) {
	char * token = HTAssocList_findObject(me->cache_control, "max-age");
	if (token) return atol(token);
    }
    return (time_t) -1;
}

PUBLIC BOOL HTResponse_mustRevalidate (HTResponse * me)
{
    return me && me->cache_control &&
	(HTAssocList_findObject(me->cache_control,
				"must-revalidate") != NULL);
}

PUBLIC char * HTResponse_noCache (HTResponse * me)
{
    return (me && me->cache_control) ?
	HTAssocList_findObject(me->cache_control,
			       "no-cache") : NULL;
}

PUBLIC char * HTResponse_etag (HTResponse * me)
{
    if (me && me->headers) {
	char * value = HTAssocList_findObject(me->headers, "etag");
	char * etag = HTNextField(&value);
	return etag;
    }
    return NULL;
}

/*
**  Byte ranges
*/
PUBLIC BOOL HTResponse_deleteRangeAll (HTResponse * me)
{
    if (me && me->byte_ranges) {
	HTAssocList_delete(me->byte_ranges);
	me->byte_ranges = NULL;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTResponse_addRange (HTResponse * me, char * unit, char * range)
{
    if (me) {
	if (!me->byte_ranges) me->byte_ranges = HTAssocList_new();
	return HTAssocList_addObject(me->byte_ranges, unit, range);
    }
    return NO;
}

PUBLIC HTAssocList * HTResponse_range (HTResponse * me)
{
    return (me ? me->byte_ranges : NULL);
}

/*
**  Content Length
*/
PUBLIC long int HTResponse_length (HTResponse * me)
{
    return me ? me->content_length : -1;
}

PUBLIC void HTResponse_setLength (HTResponse * me, long int length)
{
    if (me) me->content_length = length;
}

PUBLIC void HTResponse_addLength (HTResponse * me, long int delta_length)
{
    if (me) {
	if (me->content_length < 0)
	    me->content_length = delta_length;
	else
	    me->content_length += delta_length;
    }
}

/*
**  Content-Type
*/
PUBLIC HTFormat HTResponse_format (HTResponse * me)
{
    return me ? me->content_type : NULL;
}

PUBLIC void HTResponse_setFormat (HTResponse * me, HTFormat form)
{
    if (me) me->content_type = form;
}

PUBLIC HTAssocList * HTResponse_formatParam (HTResponse * me)
{
    return me ? me->type_parameters : NULL;
}

PUBLIC BOOL HTResponse_addFormatParam (HTResponse * me,
				     const char * name, const char * value)
{
    if (me) {
	if (!me->type_parameters) me->type_parameters = HTAssocList_new();
	return HTAssocList_replaceObject(me->type_parameters, name, value);
    }
    return NO;
}

/*
**  Charset parameter to Content-Type
*/
PUBLIC HTCharset HTResponse_charset (HTResponse * me)
{
    if (me && me->type_parameters) {
	char * charset = HTAssocList_findObject(me->type_parameters,"charset");
	return HTAtom_for(charset);
    }
    return NULL;
}

PUBLIC BOOL HTResponse_setCharset (HTResponse * me, HTCharset charset)
{
    return HTResponse_addFormatParam(me, "charset", HTAtom_name(charset));
}

/*
**	Content Encoding
*/
PUBLIC BOOL HTResponse_addEncoding (HTResponse * me, HTEncoding encoding)
{
    if (me && encoding) {
	if (!me->content_encoding) me->content_encoding = HTList_new();
	return HTList_addObject(me->content_encoding, encoding);
    }
    return NO;
}

PUBLIC HTList * HTResponse_encoding (HTResponse * me)
{
    return me ? me->content_encoding : NULL;
}

/*
**	Transfer Encoding
*/
PUBLIC BOOL HTResponse_addTransfer (HTResponse * me, HTEncoding transfer)
{
    if (me && transfer) {
	if (!me->transfer_encoding) me->transfer_encoding = HTList_new();
	return HTList_addObject(me->transfer_encoding, transfer);
    }
    return NO;
}

PUBLIC HTList * HTResponse_transfer (HTResponse * me)
{
    return me ? me->transfer_encoding : NULL;
}

/*
**	Content Transfer Encoding
*/
PUBLIC HTEncoding HTResponse_contentTransferEncoding (HTResponse * me)
{
    return me ? me->cte : NULL;
}

PUBLIC BOOL HTResponse_setContentTransferEncoding (HTResponse * me, HTEncoding transfer)
{
    if (me) {
	me->cte = transfer;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTResponse_addVariant (HTResponse * me, char * token, char * value)
{
    if (me) {
	if (!me->variants) me->variants =HTAssocList_new();
	return HTAssocList_replaceObject (me->variants, token, value);
    }
    return NO;
}

PUBLIC BOOL HTResponse_deleteVariantAll (HTResponse * me)
{
    if (me && me->variants) {
	HTAssocList_delete(me->variants);
	me->variants = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTResponse_variant (HTResponse * me)
{
    return (me ? me->variants : NULL);
}

/*
**  Trailers
*/
PUBLIC BOOL HTResponse_addTrailer (HTResponse * me,
				   char * token, char * value)
{
    if (me) {
	if (!me->trailer) me->trailer = HTAssocList_new();
	return HTAssocList_addObject(me->trailer, token, value);
    }
    return NO;
}

PUBLIC BOOL HTResponse_deleteTrailerAll (HTResponse * me)
{
    if (me && me->trailer) {
	HTAssocList_delete(me->trailer);
	me->trailer = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTResponse_trailer (HTResponse * me)
{
    return (me ? me->trailer : NULL);
}


/*
**  Original header information
*/
PUBLIC BOOL HTResponse_addHeader (HTResponse * me,
				  char * token, char * value)
{
    if (me) {
	if (!me->headers) me->headers = HTAssocList_new();
	return HTAssocList_addObject(me->headers, token, value);
    }
    return NO;
}

PUBLIC BOOL HTResponse_deleteHeaderAll (HTResponse * me)
{
    if (me && me->headers) {
	HTAssocList_delete(me->headers);
	me->headers = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTResponse_header (HTResponse * me)
{
    return (me ? me->headers : NULL);
}

PUBLIC HTAssocList * HTResponse_handOverHeader (HTResponse * me)
{
    HTAssocList * headers = NULL;
    if (me) {
	headers = me->headers;
	me->headers = NULL;
	me->type_parameters = NULL;	/* @@@ */
    }
    return headers;
}

/*
**  HTTP reason string
*/
PUBLIC char * HTResponse_reason (HTResponse * me)
{
    if (me) {
      return me->reason;
    }
    return NULL;
}

PUBLIC BOOL HTResponse_setReason (HTResponse * me, char * reason)
{
  if (me && reason && *reason) {
      StrAllocCopy(me->reason, reason);
      return YES;
    }
  return NO;
}

