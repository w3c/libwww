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
	char * gatewayed = HTParse((*path=='/') ? path+1 : path, physical, PARSE_ALL);
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
	HTTRACE(CACHE_TRACE, "Mem Cache... We only check GET methods\n");
	return HT_OK;
    }

    /*
    **  If we are asked to flush the persistent cache then there is no reason
    **  to do anything here - we're flushing it anyway. Also if no document
    **  then just exit from this filter.
    */
    if (!document || validation > HT_CACHE_FLUSH_MEM) {
	HTTRACE(CACHE_TRACE, "Mem Cache... No fresh document...\n");
	return HT_OK;
    }

    /*
    **  If we have a document object associated with this anchor then we also
    **  have the object in the history list. Depending on what the user asked,
    **  we can add a cache validator
    */
    if (document && validation != HT_CACHE_FLUSH_MEM) {
	HTTRACE(CACHE_TRACE, "Mem Cache... Document already in memory\n");
	return HT_LOADED;
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
    case HT_RETRY: {
        HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) (*cbf)(request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			HTRequest_error(request), NULL);
	HTTRACE(PROT_TRACE, "Load End.... NOT AVAILABLE, RETRY AT %ld\n" _ 
		    HTResponse_retryTime(response));
        }
        break;

    case HT_NO_DATA:
    {
	/*
	** The document was empty
	*/
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) (*cbf)(request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			HTRequest_error(request), NULL);
	HTTRACE(PROT_TRACE, "Load End.... EMPTY: No content `%s\'\n" _ 
		    uri ? uri : "<UNKNOWN>");
	break;
    }    

    case HT_LOADED:
	HTTRACE(PROT_TRACE, "Load End.... OK: `%s\'\n" _ uri);
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
	HTTRACE(PROT_TRACE, "Load End.... Request ended with code %d\n" _ status);
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

    /* Check for destination */
    if (!new_anchor) {
	HTTRACE(PROT_TRACE, "Redirection. No destination\n");
	return HT_OK;
    }

    /*
    ** Only do automatic redirect on GET and HEAD. Ask for all
    ** other methods.
    */
    if (!HTMethod_isSafe(method)) {

	/*
	** If we got a 303 See Other then change the method to GET.
	** Otherwise ask the user whether we should continue.
	*/
	if (status == HT_SEE_OTHER) {
	    HTTRACE(PROT_TRACE, "Redirection. Changing method from %s to GET\n" _ 
			HTMethod_name(method));
	    HTRequest_setMethod(request, METHOD_GET);
	} else {
	    HTAlertCallback * prompt = HTAlert_find(HT_A_CONFIRM);
	    if (prompt) {
		if ((*prompt)(request, HT_A_CONFIRM, HT_MSG_REDIRECTION,
			      NULL, NULL, NULL) != YES)
		    return HT_OK;
	    }
	}
    } 

    /* Register the redirection as a link relationship */
    {
	HTLinkType ltype = status==HT_PERM_REDIRECT ? HT_LR_PERM_REDIRECT :
	    (status==HT_TEMP_REDIRECT || status==HT_FOUND) ? HT_LR_TEMP_REDIRECT :
	    status==HT_SEE_OTHER ? HT_LR_SEE_OTHER : NULL;
	if (ltype) {
	    HTLink_add((HTAnchor *) HTRequest_anchor(request), new_anchor, 
		       ltype, method);
	}
    }

    /* Delete any auth credendials as they get regenerated */
    HTRequest_deleteCredentialsAll(request);

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
	return HT_OK;		/* Wanna fall through */
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
	HTTRACE(PROT_TRACE, "Use Proxy... No proxy location\n");
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
	HTTRACE(PROT_TRACE, "Credentials. verified\n");
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
**	Client side authentication info AFTER filter
**	---------------------------------------
*/
PUBLIC int HTAuthInfoFilter (HTRequest * request, HTResponse * response,
			     void * param, int status)
{
    /*
    ** Ask the authentication module to call the right authentication info
    ** parser
    */
    if (! HTResponse_challenge (response))
      return HT_OK;
    else if (HTAA_updateFilter(request, response, param, status) == HT_OK) 
      return HT_OK;
    else
      return HT_ERROR;
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
