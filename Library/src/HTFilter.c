/*
**	BEFORE AND AFTER FILTERS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module implrments a set of default filters that can be registerd
**	as BEFORE and AFTER filters to the Net manager
** Authors
**	HFN	Henrik Frystyk, frystyk@w.org
** History
**	Jul 4, 96	Written
*/

/* Library include files */
#include "WWWLib.h"
#include "WWWCache.h"
#include "WWWHTTP.h"
#include "HTLog.h"
#include "HTAccess.h"
#include "HTProxy.h"
#include "HTRules.h"
#include "HTFilter.h"					 /* Implemented here */

/* ------------------------------------------------------------------------- */

/*
**	Proxy and Gateway BEFORE filter
**	-------------------------------
**	Checks for registerd proxy servers or gateways and sees whether this
**	request should be redirected to a proxy or a gateway. Proxies have
**	higher priority than gateways so we look for them first!
**	For HTTP/1.0 and HTTP/1.1 we may only send a full URL (including the
**	host portion) to proxy servers. Therefore, we tell the Library whether
**	to use the full URL or the traditional HTTP one without the host part.
*/
PUBLIC int HTProxyFilter (HTRequest * request, void * param, int mode)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * addr = HTAnchor_physical(anchor);
    char * physical = NULL;
    if ((physical = HTProxy_find(addr))) {
	HTRequest_setFullURI(request, YES);			  /* For now */
	HTRequest_setProxy(request, physical);
	HT_FREE(physical);
#if 0
	/* Don't paste the URLs together anymore */
	StrAllocCat(physical, addr);
	HTAnchor_setPhysical(anchor, physical);	
#endif
    } else if ((physical = HTGateway_find(addr))) {
	/* 
	** A gateway URL is crated by chopping off any leading "/" to make the
	** host into part of path
	*/
	char * path =
	    HTParse(addr, "", PARSE_HOST + PARSE_PATH + PARSE_PUNCTUATION);
	char * gatewayed = HTParse(path+1, physical, PARSE_ALL);
	HTAnchor_setPhysical(anchor, gatewayed);
	HT_FREE(path);
	HT_FREE(gatewayed);
	HTRequest_setFullURI(request, NO);
	HTRequest_deleteProxy(request);
    } else {
	HTRequest_setFullURI(request, NO);			  /* For now */
	HTRequest_deleteProxy(request);
    }
    return HT_OK;
}

/*
**	Rule Translation BEFORE Filter
**	------------------------------
**	If we have a set of rules loaded (see the Rule manager) then check
**	before each request whether how that should be translated. The trick
**	is that a parent anchor has a "address" which is the part from the URL
**	we used when we created the anchor. However, it also have a "physical
**	address" which is the place we are actually going to look for the
**	resource. Hence this filter translates the physical address
**	(if any translations are found)
*/
PUBLIC int HTRuleFilter (HTRequest * request, void * param, int mode)
{
    HTList * list = HTRule_global();
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * addr = HTAnchor_physical(anchor);
    char * physical = HTRule_translate(list, addr, NO);
    if (!physical) {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_FORBIDDEN,
			   NULL, 0, "HTRuleFilter");
	return HT_ERROR;
    }
    HTAnchor_setPhysical(anchor, physical);
    HT_FREE(physical);
    return HT_OK;
}

/*
**	Cache Validation BEFORE Filter
**	------------------------------
**	Check the cache mode to see if we can use an already loaded version
**	of this document. If so and our copy is valid then we don't have
**	to go out and get it unless we are forced to
**	We only check the cache in caseof a GET request. Otherwise, we go
**	directly to the source.
*/
PUBLIC int HTCacheFilter (HTRequest * request, void * param, int mode)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTCache * cache = NULL;
    HTReload reload = HTRequest_reloadMode(request);
    HTMethod method = HTRequest_method(request);
    HTDisconnectedMode disconnect = HTCacheMode_disconnected();
    BOOL validate = NO;

    /*
    **  If the cache is disabled all together then it won't help looking, huh?
    */
    if (!HTCacheMode_enabled()) return HT_OK;
    if (CACHE_TRACE) HTTrace("Cachefilter. Checking persistent cache\n");

    /*
    **  Now check the cache...
    */
    if (method != METHOD_GET) {
	if (CACHE_TRACE) HTTrace("Cachefilter. We only check GET methods\n");
    } else if (reload == HT_CACHE_FLUSH) {
	/*
	** If the mode if "Force Reload" then don't even bother to check the
	** cache - we flush everything we know abut this document anyway.
	** Add the appropriate request headers. We use both the "pragma"
	** and the "cache-control" headers in order to be
	** backwards compatible with HTTP/1.0
	*/
	validate = YES;
	HTRequest_addGnHd(request, HT_G_PRAGMA_NO_CACHE);
	HTRequest_addCacheControl(request, "no-cache", "");

	/*
	**  We also flush the information in the anchor as we don't want to
	**  inherit any "old" values
	*/
	HTAnchor_clearHeader(anchor);

    } else {
	/*
	** Check the persistent cache manager. If we have a cache hit then
	** continue to see if the reload mode requires us to do a validation
	** check. This filter assumes that we can get the cached version
	** through one of our protocol modules (for example the file module)
	*/
	cache = HTCache_find(anchor);
	if (cache) {
	    HTReload cache_mode = HTCache_isFresh(cache, request);
	    if (cache_mode == HT_CACHE_ERROR) cache = NULL;
	    reload = HTMAX(reload, cache_mode);
	    HTRequest_setReloadMode(request, reload);

	    /*
	    **  Now check the mode and add the right headers for the validation
	    **  If we are to validate a cache entry then we get a lock
	    **  on it so that not other requests can steal it.
	    */
	    if (reload == HT_CACHE_RANGE_VALIDATE) {
		/*
		**  If we were asked to range validate the cached object then
		**  use the etag or the last modified for cache validation
		*/
		validate = YES;
		HTCache_getLock(cache, request);
		HTRequest_addRqHd(request, HT_C_IF_RANGE);
	    } else if (reload == HT_CACHE_END_VALIDATE) {
		/*
		**  If we were asked to end-to-end validate the cached object
		**  then use a max-age=0 cache control directive
		*/
		validate = YES;
		HTCache_getLock(cache, request);
		HTRequest_addCacheControl(request, "max-age", "0");
	    } else if (reload == HT_CACHE_VALIDATE) {
		/*
		**  If we were asked to validate the cached object then
		**  use the etag or the last modified for cache validation
		**  We use both If-None-Match or If-Modified-Since.
		*/
		validate = YES;
		HTCache_getLock(cache, request);
		HTRequest_addRqHd(request, HT_C_IF_NONE_MATCH | HT_C_IMS);
	    } else if (cache) {
		/*
		**  The entity does not require any validation at all. We
		**  can just go ahead and get it from the cache. In case we
		**  have a fresh subpart of the entity, then we issue a 
		**  conditional GET request with the range set by the cache
		**  manager. Issuing the conditional range request is 
		**  equivalent to a validation as we have to go out on the
		**  net. This may have an effect if running in disconnected
		**  mode. We disable all BEFORE filters as they don't make
		**  sense while loading the cache entry.
		*/
		{
		    char * name = HTCache_name(cache);
		    HTAnchor_setPhysical(anchor, name);
		    HTCache_addHit(cache);
		    HT_FREE(name);
		}
	    }
	}
    }
    
    /*
    **  If we are in disconnected mode and we are to validate an entry
    **  then check whether what mode of disconnected mode we're in. If
    **  we are to use our own cache then return a "504 Gateway Timeout"
    */
    if ((!cache || validate) && disconnect != HT_DISCONNECT_NONE) {
	if (disconnect == HT_DISCONNECT_EXTERNAL)
	    HTRequest_addCacheControl(request, "only-if-cached", "");
	else {
	    HTRequest_addError(request, ERR_FATAL, NO,
			       HTERR_GATE_TIMEOUT, "Disconnected Cache Mode",
			       0, "HTCacheFilter");
	    return HT_ERROR;
	}
    }
    return HT_OK;
}

/*
**	A small BEFORE filter that just finds a cache entry unconditionally
**	and loads the entry. All freshness and any other constraints are 
**	ignored.
*/
PUBLIC int HTCacheLoadFilter (HTRequest * request, void * param, int mode)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTCache * cache = HTCache_find(anchor);
    if (cache) {
	char * name = HTCache_name(cache);
	HTAnchor_setPhysical(anchor, name);
	HTCache_addHit(cache);
	HT_FREE(name);

	/*
	**  Start request directly from the cache. As with the redirection
	**  filter we reuse the same request object which means that we must
	**  keep this around until the cache load request has terminated
	*/
	{
	    HTLoad(request, NO);
	    return HT_ERROR;
	}
    }
    return HT_OK;
}

/*
**	Check the Memory Cache (History list) BEFORE filter
**	---------------------------------------------------
**	Check if document is already loaded. The user can define whether
**	the history list should follow normal expiration or work as a
**	traditional history list where expired documents are not updated.
**	We don't check for anything but existence proof of a document
**	associated with the anchor as the definition is left to the application
*/
PUBLIC int HTMemoryCacheFilter (HTRequest * request, void * param, int mode)
{
    HTReload validation = HTRequest_reloadMode(request);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    void * document = HTAnchor_document(anchor);

    /*
    **  We only check the memory cache if it's a GET method
    */
    if (HTRequest_method(request) != METHOD_GET) {
	if (CACHE_TRACE) HTTrace("Mem Cache... We only check GET methods\n");
	return HT_OK;
    }

    /*
    **  If we are asked to flush the persistent cache then there is no reason
    **  to do anything here - we're flushing it anyway. Also if no document
    **  then just exit from this filter.
    */
    if (!document || validation > HT_CACHE_FLUSH_MEM) {
	if (CACHE_TRACE) HTTrace("Mem Cache... No fresh document...\n");
	return HT_OK;
    }

    /*
    **  If we have a document object associated with this anchor then we also
    **  have the object in the history list. Depending on what the user asked,
    **  we can add a cache validator
    */
    if (document && validation != HT_CACHE_FLUSH_MEM) {
	if (CACHE_TRACE) HTTrace("Mem Cache... Document already in memory\n");
	return HT_LOADED;
    }
    return HT_OK;
}

/*
**	Cache Update AFTER filter
**	-------------------------
**	On our way out we catch the metainformation and stores it in
**	our persistent store. If we have a cache validation (a 304
**	response then we use the new metainformation and merges it with
**	the existing information already captured in the cache.
*/
PUBLIC int HTCacheUpdateFilter (HTRequest * request, HTResponse * response,
				void * param, int status)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTCache * cache = HTCache_find(anchor);
    if (cache) {

	/*
	**  It may in fact be that the information in the 304 response
	**  told us that we can't cache the entity anymore. If this is the
	**  case then flush it now. Otherwise prepare for a cache read
	*/
	if (CACHE_TRACE) HTTrace("Cache....... Merging metainformation\n");
	if (HTResponse_isCachable(response) == NO) {
	    HTCache_remove(cache);
	} else {
	    char * name = HTCache_name(cache);
	    HTAnchor_setPhysical(anchor, name);
	    HTCache_addHit(cache);
	    HT_FREE(name);
	    HTCache_updateMeta(cache, request, response);
	}

	/*
	**  Start request directly from the cache. As with the redirection filter
	**  we reuse the same request object which means that we must
	**  keep this around until the cache load request has terminated
	**  In the case of a 
	*/
#define SINGLE_CACHE_LOAD
#ifdef SINGLE_CACHE_LOAD
	{
	    static BOOL done = NO;
	    if (!done) {	
		HTLoad(request, YES);
		done = YES;
		return HT_ERROR;
	    }
	}
#else
	HTLoad(request, YES);
	return HT_ERROR;
#endif /* SINGLE_CACHE_LOAD */
    }
    return HT_OK;
}

/*
**	Error and Information AFTER filter
**	----------------------------------
**	It checks the status code from a request and generates an 
**	error/information message if required.
*/
PUBLIC int HTInfoFilter (HTRequest * request, HTResponse * response,
			 void * param, int status)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * uri = HTAnchor_address((HTAnchor*) anchor);
    switch (status) {
    case HT_RETRY:
	if (PROT_TRACE)
	    HTTrace("Load End.... NOT AVAILABLE, RETRY AT %ld\n",
		    HTResponse_retryTime(response));
	break;

    case HT_NO_DATA:
    {
	/*
	** The document was empty
	*/
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) (*cbf)(request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			HTRequest_error(request), NULL);
	if (PROT_TRACE)
	    HTTrace("Load End.... EMPTY: No content `%s\'\n",
		    uri ? uri : "<UNKNOWN>");
	break;
    }    

    case HT_LOADED:
	if (PROT_TRACE) HTTrace("Load End.... OK: `%s\'\n", uri);
	break;

    default:
    {
	/*
	** See if we have a function registered for outputting errors.
	** If so then call it and present the message to the user
	*/
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) (*cbf)(request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			HTRequest_error(request), NULL);
	if (PROT_TRACE)
	    HTTrace("Load End.... Request ended with code %d\n", status);
	break;
    }
    }
    HT_FREE(uri);
    return HT_OK;
}

/*
**	Redirection AFTER filter
**	------------------------
**	The redirection handler only handles redirections
**	on the GET or HEAD method (or any other safe method)
*/
PUBLIC int HTRedirectFilter (HTRequest * request, HTResponse * response,
			     void * param, int status)
{
    HTMethod method = HTRequest_method(request); 
    HTAnchor * new_anchor = HTResponse_redirection(response); 
    if (!new_anchor) {
	if (PROT_TRACE) HTTrace("Redirection. No destination\n");
	return HT_OK;
    }

    /*
    ** Only do automatic redirect on GET and HEAD. Ask for all
    ** other methods
    */
    if (!HTMethod_isSafe(method)) {

	/*
	** If we got a 303 See Other then change the method to GET.
	** Otherwise ask the user whether we should continue.
	*/
	if (status == HT_SEE_OTHER) {
	    if (PROT_TRACE)
		HTTrace("Redirection. Changing method from %s to GET\n",
			HTMethod_name(method));
	    HTRequest_setMethod(request, METHOD_GET);
	} else {
	    HTAlertCallback * prompt = HTAlert_find(HT_A_CONFIRM);
	    if (prompt) {
		if ((*prompt)(request, HT_A_CONFIRM, HT_MSG_REDIRECTION,
			      NULL, NULL, NULL) != YES)
		    return HT_ERROR;
	    }
	}
    } 
 
    /*
    **  Start new request with the redirect anchor found in the headers.
    **	Note that we reuse the same request object which means that we must
    **  keep this around until the redirected request has terminated. It also
    **  allows us in an easy way to keep track of the number of redirections
    **	so that we can detect endless loops.
    */ 
    if (HTRequest_doRetry(request)) { 
	HTLoadAnchor(new_anchor, request);
    } else {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_MAX_REDIRECT,
			   NULL, 0, "HTRedirectFilter");
    }

    /*
    **  By returning HT_ERROR we make sure that this is the last handler to be
    **  called. We do this as we don't want any other filter to delete the 
    **  request object now when we have just started a new one ourselves
    */
    return HT_ERROR;
} 

/*
**	Retry through Proxy AFTER Filter
**	--------------------------------
**	This filter handles a 305 Use Proxy response and retries the request
**	through the proxy
*/
PUBLIC int HTUseProxyFilter (HTRequest * request, HTResponse * response,
			     void * param, int status)
{
    HTAlertCallback * cbf = HTAlert_find(HT_A_CONFIRM);
    HTAnchor * proxy_anchor = HTResponse_redirection(response); 
    if (!proxy_anchor) {
	if (PROT_TRACE) HTTrace("Use Proxy... No proxy location\n");
	return HT_OK;
    }

    /*
    **  Add the proxy to the list. Assume HTTP access method only!
    **  Because evil servers may rediret the client to an untrusted
    **  proxy, we can only accept redirects for this particular
    **  server. Also, we do not know whether this is for HTTP or all
    **  other requests as well
    */
    if ((cbf && (*cbf)(request, HT_A_CONFIRM, HT_MSG_PROXY, NULL,NULL,NULL))) {
	char * addr = HTAnchor_address(proxy_anchor);
	HTProxy_add("http", addr);
	HT_FREE(addr);
 
	/*
	**  Start new request through the proxy if we haven't reached the max
	**  number of redirections for this request
	*/ 
	if (HTRequest_doRetry(request)) { 
	    HTLoadAnchor(proxy_anchor, request);
	} else {
	    HTRequest_addError(request, ERR_FATAL, NO, HTERR_MAX_REDIRECT,
			       NULL, 0, "HTRedirectFilter");
	}

	/*
	**  By returning HT_ERROR we make sure that this is the last handler to be
	**  called. We do this as we don't want any other filter to delete the 
	**  request object now when we have just started a new one ourselves
	*/
	return HT_ERROR;

    } else {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_NO_AUTO_PROXY,
			   NULL, 0, "HTUseProxyFilter");
	return HT_OK;
    }
} 

/*
**	Client side authentication BEFORE filter
**	----------------------------------------
**	The filter generates the credentials required to access a document
**	Getting the credentials may involve asking the user
*/
PUBLIC int HTCredentialsFilter (HTRequest * request, void * param, int mode)
{
    /*
    ** Ask the authentication module to call the right credentials generator
    ** that understands this scheme
    */
    if (HTAA_beforeFilter(request, param, mode) == HT_OK) {
	if (PROT_TRACE) HTTrace("Credentials. verified\n");
	return HT_OK;
    } else {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_UNAUTHORIZED,
			   NULL, 0, "HTCredentialsFilter");
	return HT_ERROR;
    }
}

/*
**	Client side authentication AFTER filter
**	---------------------------------------
**	The client side authentication filter uses the 
**	user dialog messages registered in the HTAlert module.
**	By default these are the ones used by the line mode browser but you can
**	just register something else.
*/
PUBLIC int HTAuthFilter (HTRequest * request, HTResponse * response,
			 void * param, int status)
{
    /*
    ** Ask the authentication module to call the right challenge parser
    ** that understands this scheme
    */
    if (HTAA_afterFilter(request, response, param, status) == HT_OK) {

	/*
	** Start request with new credentials. As with the redirection filter
	** we reuse the same request object which means that we must
	** keep this around until the redirected request has terminated
	*/
	HTLoad(request, NO);

	/*
	**  We return HT_ERROR to make sure that this is the last handler to be
	**  called. We do this as we don't want any other filter to delete the 
	**  request object now when we have just started a new one ourselves
	*/
	return HT_ERROR;
    }
    return HT_OK;
}

/*
**	Request Logging AFTER filter
**	----------------------------
**	Default Logging filter using the log manager provided by HTLog.c
*/
PUBLIC int HTLogFilter (HTRequest * request, HTResponse * response,
			void * param, int status)
{
    if (request) {
	HTLog * log = (HTLog *) param;
	if (log) HTLog_addCLF(log, request, status);
	return HT_OK;
    }
    return HT_ERROR;
}

/*
**	Request Referer AFTER filter
**	----------------------------
**	Default Referer Log filter using the log manager provided by HTLog.c
*/
PUBLIC int HTRefererFilter (HTRequest * request, HTResponse * response,
			    void * param, int status)
{
    if (request) {
	HTLog * log = (HTLog *) param;
	if (log) HTLog_addReferer(log, request, status);
	return HT_OK;
    }
    return HT_ERROR;
}
