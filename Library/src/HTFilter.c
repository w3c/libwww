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
#include "WWWRules.h"
#include "WWWHTTP.h"
#include "HTLog.h"
#include "HTAccess.h"
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
PUBLIC int HTProxyFilter (HTRequest * request, void * param, int status)
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
PUBLIC int HTRuleFilter (HTRequest * request, void * param, int status)
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
PUBLIC int HTCacheFilter (HTRequest * request, void * param, int status)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTReload mode = HTRequest_reloadMode(request);
    HTMethod method = HTRequest_method(request);

    /*
    ** Check the method of the request
    */
    if (method != METHOD_GET) {
	if (CACHE_TRACE) HTTrace("Cachefilter. We only check GET methods\n");
	return HT_OK;
    }

    /*
    ** If the mode if "Force Reload" then don't even bother to check the
    ** cache - we flush everything we know about this document
    */
    if (mode == HT_FORCE_RELOAD) {
 	/*
	** Add the appropriate request headers. We use both the "pragma"
	** and the "cache-control" headers in order to be
	** backwards compatible with HTP/1.0
	*/
	HTRequest_addGnHd(request, HT_G_PRAGMA_NO_CACHE);

	/* @@@ CACHE CONTROL @@@ */

	/*
	** We also flush the information in the anchor
	*/
	HTAnchor_clearHeader(anchor);
	return HT_OK;
    }

    /*
    ** Check the application provided memory cache. This is equivalent to a
    ** history list and does not follow the same cache mechanisms as the 
    ** persistent cache
    */
    if (HTMemoryCache_check(request) == HT_LOADED)
	return HT_LOADED;
    
    /*
    ** Check the persistent cache manager. If we have a cache hit then
    ** continue to see if the reload mode requires us to do a validation check.
    ** This filter assumes that we can get the cached version through one of
    ** our protocol modules (for example the file module)
    */
    {
	char * addr = HTAnchor_address((HTAnchor *) anchor);
	char * cache = HTCache_getReference(addr);
	if (cache) {
	    if (mode != HT_CACHE_REFRESH) {
		HTAnchor_setPhysical(anchor, cache);
		HTAnchor_setCacheHit(anchor, YES);
	    } else {

		/* @@@ Do cache validation @@@ */

	    }
	}
	HT_FREE(addr);
    }
    return HT_OK;
}

/*
**	Error and Information AFTER filter
**	----------------------------------
**	It checks the status code from a request and generates an 
**	error/information message if required.
*/
PUBLIC int HTInfoFilter (HTRequest * request, void * param, int status)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * uri = HTAnchor_address((HTAnchor*) anchor);
    switch (status) {
    case HT_RETRY:
	if (PROT_TRACE)
	    HTTrace("Load End.... NOT AVAILABLE, RETRY AT %ld\n",
		    HTRequest_retryTime(request));
	break;

    case HT_ERROR:
    {
	/*
	** See if we have a function registered for outputting errors.
	** If so then call it and present the message to the user
	*/
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) (*cbf)(request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			HTRequest_error(request), NULL);
	if (PROT_TRACE)
	    HTTrace("Load End.... ERROR: Can't access `%s\'\n",
		    uri ? uri : "<UNKNOWN>");
	break;
    }    

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
    {
	/*
	** Even though we have received a loaded status the thing we have
	** loaded successfully may in fact be an error message. We therefore
	** look at the error stack to see what to do.
	*/
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) (*cbf)(request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			HTRequest_error(request), NULL);
	if (PROT_TRACE) HTTrace("Load End.... OK: `%s\'\n", uri);
	break;
    }

    default:
	if (PROT_TRACE)
	    HTTrace("Load End.... Request ended with code %d\n", status);
	break;
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
PUBLIC int HTRedirectFilter (HTRequest * request, void * param, int status)
{
    HTMethod method = HTRequest_method(request); 
    HTAnchor * new_anchor = HTRequest_redirection(request); 
    if (!new_anchor) {
	if (PROT_TRACE) HTTrace("Redirection. No destination\n");
	return HT_OK;
    }

    /*
    ** Only do redirect on GET and HEAD
    */
    if (!HTMethod_isSafe(method)) { 
	HTAlertCallback * prompt = HTAlert_find(HT_A_CONFIRM);
	if (prompt) {
	    if ((*prompt)(request, HT_A_CONFIRM, HT_MSG_REDIRECTION,
			  NULL, NULL, NULL) != YES)
		return HT_ERROR;
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
**	Client side authentication BEFORE filter
**	----------------------------------------
**	The filter generates the credentials required to access a document
**	Getting the credentials may involve asking the user
*/
PUBLIC int HTCredentialsFilter (HTRequest * request, void * param, int status)
{
    /*
    ** Ask the authentication module to call the right credentials generator
    ** that understands this scheme
    */
    if (HTAA_beforeFilter(request, param, status) == HT_OK) {
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
PUBLIC int HTAuthFilter (HTRequest * request, void * param, int status)
{
    /*
    ** Ask the authentication module to call the right challenge parser
    ** that understands this scheme
    */
    if (HTAA_afterFilter(request, param, status) == HT_OK) {

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
**	Request Loggin AFTER filter
**	---------------------------
**	Default Logging filter using the log manager provided by HTLog.c
*/
PUBLIC int HTLogFilter (HTRequest * request, void * param, int status)
{
    if (request) {
	if (HTLog_isOpen()) HTLog_add(request, status);
	return HT_OK;
    }
    return HT_ERROR;
}
