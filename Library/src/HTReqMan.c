/*								     HTReqMan.c
**	REQUEST MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** Authors
**	TBL	Tim Berners-Lee timbl@w3.org
**	JFG	Jean-Francois Groff jfg@dxcern.cern.ch
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
**	HFN	Henrik Frystyk, frystyk@w3.org
** History
**       8 Jun 92 Telnet hopping prohibited as telnet is not secure TBL
**	26 Jun 92 When over DECnet, suppressed FTP, Gopher and News. JFG
**	 6 Oct 92 Moved HTClientHost and HTlogfile into here. TBL
**	17 Dec 92 Tn3270 added, bug fix. DD
**	 4 Feb 93 Access registration, Search escapes bad chars TBL
**		  PARAMETERS TO HTSEARCH AND HTLOADRELATIVE CHANGED
**	28 May 93 WAIS gateway explicit if no WAIS library linked in.
**	   Dec 93 Bug change around, more reentrant, etc
**	09 May 94 logfile renamed to HTlogfile to avoid clash with WAIS
**	 8 Jul 94 Insulate free() from _free structure element.
**	02 Sep 95 Rewritten and spawned from HTAccess.c, HFN
*/

#if !defined(HT_DIRECT_WAIS) && !defined(HT_DEFAULT_WAIS_GATEWAY)
#define HT_DEFAULT_WAIS_GATEWAY "http://www.w3.org:8001/"
#endif

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "HTParse.h"
#include "HTAlert.h"
#include "HTError.h"
#include "HTNetMan.h"
#include "HTEvent.h"
#include "HTProt.h"
#include "HTHeader.h"
#include "HTLib.h"
#include "HTReqMan.h"					 /* Implemented here */

#ifndef HT_MAX_RELOADS
#define HT_MAX_RELOADS	6
#endif

PRIVATE int HTMaxRetry = HT_MAX_RELOADS;

struct _HTStream {
	HTStreamClass * isa;
	/* ... */
};

/* --------------------------------------------------------------------------*/
/*			Management of the HTRequest structure		     */
/* --------------------------------------------------------------------------*/

/*  Create  a request structure
**  ---------------------------
*/
PUBLIC HTRequest * HTRequest_new (void)
{
    HTRequest * me;
    if ((me = (HTRequest *) HT_CALLOC(1, sizeof(HTRequest))) == NULL)
        HT_OUTOFMEM("HTRequest_new()");
    
   /* Force Reload */
    me->reload = HT_CACHE_OK;

    /* Set the default user profile */
    me->userprofile = HTLib_userProfile();

    /* Format of output */
    me->output_format	= WWW_PRESENT;	    /* default it to present to user */
    me->debug_format	= WWW_DEBUG;	 /* default format of error messages */

    /* HTTP headers */
    me->GenMask		= DEFAULT_GENERAL_HEADERS;
    me->RequestMask	= DEFAULT_REQUEST_HEADERS;
    me->ResponseMask	= DEFAULT_RESPONSE_HEADERS;
    me->EntityMask	= DEFAULT_ENTITY_HEADERS;

    /* Default retry after value */
    me->retry_after = -1;
    me->priority = HT_PRIORITY_MAX;

    /* Default max forward value */
    me->max_forwards = -1;

    /* Content negotiation */
    me->ContentNegotiation = YES;		       /* Do this by default */

    if (CORE_TRACE) HTTrace("Request..... Created %p\n", me);

#if 0 /* WWW_WIN_ASYNC */
    HTEvent_winHandle(me);
#endif
    return me;
}

/*	HTRequest_clear
**	---------------
**	Clears all protocol specific information so that the request object
**	can be used for another request.
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTRequest_clear (HTRequest * me)
{
    if (me) {
	me->error_stack = NULL;
	me->net = NULL;
	me->realm = NULL;
	me->scheme = NULL;
	me->challenge = NULL;
	me->credentials = NULL;
	me->connected = NO;
	return YES;
    }
    return NO;
}

/*	HTRequest_dup
**	-------------
**	Creates a new HTRequest object as a duplicate of the src request.
**	Returns YES if OK, else NO
*/
PUBLIC HTRequest * HTRequest_dup (HTRequest * src)
{
    HTRequest * me;
    if (!src) return NULL;
    if ((me = (HTRequest  *) HT_MALLOC(sizeof(HTRequest))) == NULL)
        HT_OUTOFMEM("HTRequest_dup");
    memcpy(me, src, sizeof(HTRequest));
    if (CORE_TRACE) HTTrace("Request..... Duplicated %p to %p\n", src, me);
    return me;
}

/*	HTRequest_dupInternal
**	---------------------
**	Creates a new HTRequest object as a duplicate of the src request.
**	The difference to the HTRequest_dup function is that we don't copy the
**	error_stack and other information that the application keeps in its
**	copy of the request object. Otherwise it will be freed multiple times
**	Returns YES if OK, else NO
*/
PUBLIC HTRequest * HTRequest_dupInternal (HTRequest * src)
{
    HTRequest * me;
    if (!src) return 0;
    if ((me = (HTRequest  *) HT_MALLOC(sizeof(HTRequest))) == NULL)
        HT_OUTOFMEM("HTRequest_dup");
    memcpy(me, src, sizeof(HTRequest));
    HTRequest_clear(me);
    return me;
}

/*  Delete a request structure
**  --------------------------
*/
PUBLIC void HTRequest_delete (HTRequest * request)
{
    if (request) {
	if (CORE_TRACE) HTTrace("Request..... Delete %p\n", request);
	if (request->net) HTNet_setRequest(request->net, NULL);

	/* Should we delete the output stream? */
	if (!request->connected && request->output_stream) {
	    if (CORE_TRACE)
		HTTrace("Request..... Deleting dangling output stream\n");
	    (*request->output_stream->isa->_free)(request->output_stream);
	    request->output_stream = NULL;
	}

	/* Clean up the error stack */
	if (request->error_stack) HTError_deleteAll(request->error_stack);

	/* Before and After Filters */
	if (request->afters) HTList_delete(request->afters);
	if (request->befores) HTList_delete(request->befores);

	/* Access Authentication */
	if (request->challenge) HTAssocList_delete(request->challenge);
	if (request->credentials) HTAssocList_delete(request->credentials);
	HT_FREE(request->realm);
	HT_FREE(request->scheme);

	/* Cache control headers */
	if (request->cache_control) HTAssocList_delete(request->cache_control);

	/* Connection headers */
	if (request->connection) HTAssocList_delete(request->connection);

	/* Proxy information */
	HT_FREE(request->proxy);

	/* PEP Information */

	/* more */

	/* Simple extension protocol */
	if (request->extension) HTAssocList_delete(request->extension);

	HT_FREE(request);
    }
}

/*
**	Method
*/
PUBLIC void HTRequest_setMethod (HTRequest *request, HTMethod method)
{
    if (request) request->method = method;
}

PUBLIC HTMethod HTRequest_method (HTRequest *request)
{
    return request ? request->method : METHOD_INVALID;
}

/*
**	Reload Mode
*/
PUBLIC void HTRequest_setReloadMode (HTRequest *request, HTReload mode)
{
    if (request) request->reload = mode;
}

PUBLIC HTReload HTRequest_reloadMode (HTRequest *request)
{
    return request ? request->reload : HT_CACHE_OK;
}

/*
**	Accept Format Types
**	list can be NULL
*/
PUBLIC void HTRequest_setConversion (HTRequest *request, HTList *type,
				     BOOL override)
{
    if (request) {
	request->conversions = type;
	request->conv_local = override;
    }
}

PUBLIC HTList * HTRequest_conversion (HTRequest *request)
{
    return request ? request->conversions : NULL;
}

/*
**	Accept Encoding 
**	list can be NULL
*/
PUBLIC void HTRequest_setEncoding (HTRequest *request, HTList *enc,
				   BOOL override)
{
    if (request) {
	request->encodings = enc;
	request->enc_local = override;
    }
}

PUBLIC HTList * HTRequest_encoding (HTRequest *request)
{
    return request ? request->encodings : NULL;
}

/*
**	Accept Transfer Encoding 
**	list can be NULL
*/
PUBLIC void HTRequest_setTransfer (HTRequest * request,
				   HTList * cte, BOOL override)
{
    if (request) {
	request->ctes = cte;
	request->cte_local = override;
    }
}

PUBLIC HTList * HTRequest_transfer (HTRequest * request)
{
    return request ? request->ctes : NULL;
}

/*
**	Accept Language
**	list can be NULL
*/
PUBLIC void HTRequest_setLanguage (HTRequest *request, HTList *lang,
				   BOOL override)
{
    if (request) {
	request->languages = lang;
	request->lang_local = override;
    }
}

PUBLIC HTList * HTRequest_language (HTRequest *request)
{
    return request ? request->languages : NULL;
}

/*
**	Accept Charset
**	list can be NULL
*/
PUBLIC void HTRequest_setCharset (HTRequest *request, HTList *charset,
				  BOOL override)
{
    if (request) {
	request->charsets = charset;
	request->char_local = override;
    }
}

PUBLIC HTList * HTRequest_charset (HTRequest *request)
{
    return request ? request->charsets : NULL;
}

/*
**	Extra Header Generators. list can be NULL
*/
PUBLIC void HTRequest_setGenerator (HTRequest *request, HTList *generator,
				    BOOL override)
{
    if (request) {
	request->generators = generator;
	request->gens_local = override;
    }
}

PUBLIC HTList * HTRequest_generator (HTRequest *request, BOOL *override)
{
    if (request) {
	*override = request->gens_local;
	return request->generators;
    }
    return NULL;
}

/*
**	Extra Header Parsers. list can be NULL
*/
PUBLIC void HTRequest_setMIMEParseSet (HTRequest * me, 
				       HTMIMEParseSet * parseSet, BOOL local)
{
    if (me) {
        me->parseSet = parseSet;
	me->pars_local = local;
    }
}

PUBLIC HTMIMEParseSet * HTRequest_MIMEParseSet (HTRequest * me, BOOL * pLocal)
{
    if (me) {
        if (pLocal) *pLocal = me->pars_local;
	return me->parseSet;
    }
    return NULL;
}

/*
**	Set General Headers
*/
PUBLIC void HTRequest_setGnHd (HTRequest *request, HTGnHd gnhd)
{
    if (request) request->GenMask = gnhd;
}

PUBLIC void HTRequest_addGnHd (HTRequest *request, HTGnHd gnhd)
{
    if (request) request->GenMask |= gnhd;
}

PUBLIC HTGnHd HTRequest_gnHd (HTRequest *request)
{
    return request ? request->GenMask : 0;
}

/*
**	Set Request Headers
*/
PUBLIC void HTRequest_setRqHd (HTRequest *request, HTRqHd rqhd)
{
    if (request) request->RequestMask = rqhd;
}

PUBLIC void HTRequest_addRqHd (HTRequest *request, HTRqHd rqhd)
{
    if (request) request->RequestMask |= rqhd;
}

PUBLIC HTRqHd HTRequest_rqHd (HTRequest *request)
{
    return request ? request->RequestMask : 0;
}

/*
**	Set Response Headers
*/
PUBLIC void HTRequest_setRsHd (HTRequest *request, HTRsHd rshd)
{
    if (request) request->ResponseMask = rshd;
}

PUBLIC void HTRequest_addRsHd (HTRequest *request, HTRsHd rshd)
{
    if (request) request->ResponseMask |= rshd;
}

PUBLIC HTRsHd HTRequest_rsHd (HTRequest *request)
{
    return request ? request->ResponseMask : 0;
}

/*
**	Set Entity Headers (for the object)
*/
PUBLIC void HTRequest_setEnHd (HTRequest *request, HTEnHd enhd)
{
    if (request) request->EntityMask = enhd;
}

PUBLIC void HTRequest_addEnHd (HTRequest *request, HTEnHd enhd)
{
    if (request) request->EntityMask |= enhd;
}

PUBLIC HTEnHd HTRequest_enHd (HTRequest *request)
{
    return request ? request->EntityMask : 0;
}

/*
**	Anchor
*/
PUBLIC void HTRequest_setAnchor (HTRequest *request, HTAnchor *anchor)
{
    if (request) {
	request->anchor = HTAnchor_parent(anchor);
	request->childAnchor = ((HTAnchor *) request->anchor != anchor) ?
	    (HTChildAnchor *) anchor : NULL;
    }
}

PUBLIC HTParentAnchor * HTRequest_anchor (HTRequest *request)
{
    return request ? request->anchor : NULL;
}

PUBLIC HTChildAnchor * HTRequest_childAnchor (HTRequest * request)
{
    return request ? request->childAnchor : NULL;
}

/*
**	Net
*/
PUBLIC BOOL HTRequest_setNet (HTRequest * request, HTNet * net)
{
    if (request) {
	request->net = net;
	return YES;
    }
    return NO;
}

PUBLIC HTNet * HTRequest_net (HTRequest * request)
{
    return request ? request->net : NULL;
}

/*
**	User Profile
*/
PUBLIC BOOL HTRequest_setUserProfile (HTRequest * request, HTUserProfile * up)
{
    if (request) {
	request->userprofile = up;
	return YES;
    }
    return NO;
}

PUBLIC HTUserProfile * HTRequest_userProfile (HTRequest * request)
{
    return request ? request->userprofile : NULL;
}

/*
**	Parent anchor for Referer field
*/
PUBLIC void HTRequest_setParent (HTRequest *request, HTParentAnchor *parent)
{
    if (request) request->parentAnchor = parent;
}

PUBLIC HTParentAnchor * HTRequest_parent (HTRequest *request)
{
    return request ? request->parentAnchor : NULL;
}

/*
**	Output stream
*/
PUBLIC void HTRequest_setOutputStream (HTRequest *request, HTStream *output)
{
    if (request) request->output_stream = output;
}

PUBLIC HTStream *HTRequest_outputStream (HTRequest *request)
{
    return request ? request->output_stream : NULL;
}

/*
**	Output format
*/
PUBLIC void HTRequest_setOutputFormat (HTRequest *request, HTFormat format)
{
    if (request) request->output_format = format;
}

PUBLIC HTFormat HTRequest_outputFormat (HTRequest *request)
{
    return request ? request->output_format : NULL;
}

/*
**	Debug stream
*/
PUBLIC void HTRequest_setDebugStream (HTRequest *request, HTStream *debug)
{
    if (request) request->debug_stream = debug;
}

PUBLIC HTStream *HTRequest_debugStream (HTRequest *request)
{
    return request ? request->debug_stream : NULL;
}

/*
**	Debug Format
*/
PUBLIC void HTRequest_setDebugFormat (HTRequest *request, HTFormat format)
{
    if (request) request->debug_format = format;
}

PUBLIC HTFormat HTRequest_debugFormat (HTRequest *request)
{
    return request ? request->debug_format : NULL;
}

/*
**	Input stream
*/
PUBLIC void HTRequest_setInputStream (HTRequest *request, HTStream *input)
{
    if (request) request->input_stream = input;
}

PUBLIC HTStream *HTRequest_inputStream (HTRequest *request)
{
    return request ? request->input_stream : NULL;
}

/*
**	Net before and after callbacks
*/
PUBLIC BOOL HTRequest_addBefore (HTRequest * request, HTNetCallback * filter,
				 void * param, int status,
				 BOOL override)
{
    if (request) {
	request->befores_local = override;
	if (!request->befores) request->befores = HTList_new();
	return HTNetCall_add(request->befores, filter, param, status);
    }
    return NO;
}

PUBLIC BOOL HTRequest_deleteBefore (HTRequest * request, HTNetCallback * filter)
{
    if (request && request->befores)
	return HTNetCall_delete(request->befores, filter);
    return NO;
}

PUBLIC BOOL HTRequest_deleteBeforeAll (HTRequest * request)
{
    if (request && request->befores) {
	HTNetCall_deleteAll(request->befores);
	request->befores = NULL;
	request->befores_local = NO;
	return YES;
    }
    return NO;
}

PUBLIC HTList * HTRequest_before (HTRequest *request, BOOL *override)
{
    if (request) {
	*override = request->befores_local;
	return request->befores;
    }
    return NULL;
}

PUBLIC BOOL HTRequest_addAfter (HTRequest * request, HTNetCallback * filter,
				void * param, int status,
				BOOL override)
{
    if (request) {
	request->afters_local = override;
	if (!request->afters) request->afters = HTList_new();
	return HTNetCall_add(request->afters, filter, param, status);
    }
    return NO;
}

PUBLIC BOOL HTRequest_deleteAfter (HTRequest * request, HTNetCallback * filter)
{
    if (request && request->afters)
	return HTNetCall_delete(request->afters, filter);
    return NO;
}

PUBLIC BOOL HTRequest_deleteAfterAll (HTRequest * request)
{
    if (request && request->afters) {
	HTNetCall_deleteAll(request->afters);
	request->afters = NULL;
	request->afters_local = NO;
	return YES;
    }
    return NO;
}

PUBLIC HTList * HTRequest_after (HTRequest *request, BOOL *override)
{
    if (request) {
	*override = request->afters_local;
	return request->afters;
    }
    return NULL;
}

/*
**	Call back function for context swapping
*/
PUBLIC void HTRequest_setCallback (HTRequest *request, HTRequestCallback *cbf)
{
    if (request) request->callback = cbf;
}

PUBLIC HTRequestCallback *HTRequest_callback (HTRequest *request)
{
    return request ? request->callback : NULL;
}

/*
**	Context pointer to be used in context call back function
*/
PUBLIC void HTRequest_setContext (HTRequest *request, void *context)
{
    if (request) request->context = context;
}

PUBLIC void *HTRequest_context (HTRequest *request)
{
    return request ? request->context : NULL;
}

/*
**	Socket mode: preemptive or non-preemptive (blocking or non-blocking)
*/
PUBLIC void HTRequest_setPreemptive (HTRequest *request, BOOL mode)
{
    if (request) request->preemptive = mode;
}

PUBLIC BOOL HTRequest_preemptive (HTRequest *request)
{
    return request ? request->preemptive : NO;
}

/*
**	Has output stream been connected to the channel? If not then we
**	must free it explicitly when deleting the request object
*/
PUBLIC void HTRequest_setOutputConnected (HTRequest * request, BOOL mode)
{
    if (request) request->connected = mode;
}

PUBLIC BOOL HTRequest_outputConnected (HTRequest * request)
{
    return request ? request->connected : NO;
}

/*
**	Should we use content negotiation?
*/
PUBLIC void HTRequest_setNegotiation (HTRequest *request, BOOL mode)
{
    if (request) request->ContentNegotiation = mode;
}

PUBLIC BOOL HTRequest_negotiation (HTRequest *request)
{
    return request ? request->ContentNegotiation : NO;
}

/*
**	Are we using the full URL in the request or not?
*/
PUBLIC void HTRequest_setFullURI (HTRequest *request, BOOL mode)
{
    if (request) request->full_uri = mode;
}

PUBLIC BOOL HTRequest_fullURI (HTRequest *request)
{
    return request ? request->full_uri : NO;
}

/*
**	Are we using a proxy or not and in that case, which one?
*/
PUBLIC BOOL HTRequest_setProxy (HTRequest * request, const char * proxy)
{
    if (request && proxy) {
	StrAllocCopy(request->proxy, proxy);
	return YES;
    }
    return NO;
}

PUBLIC char * HTRequest_proxy (HTRequest * request)
{
    return request ? request->proxy : NULL;
}

PUBLIC BOOL HTRequest_deleteProxy (HTRequest * request)
{
    if (request) {
	HT_FREE(request->proxy);
	return YES;
    }
    return NO;
}

/*
**	Bytes read in this request
*/
PUBLIC long HTRequest_bytesRead(HTRequest * request)
{
    return request ? HTNet_bytesRead(request->net) : -1;
}

/*
**	Bytes written in this request
*/
PUBLIC long HTRequest_bytesWritten (HTRequest * request)
{
    return request ? HTNet_bytesWritten(request->net) : -1;
}

/*
**	Kill this request
*/
PUBLIC BOOL HTRequest_kill(HTRequest * request)
{
    return request ? HTNet_kill(request->net) : NO;
}

/*	Error Management
**	----------------
**	Returns the error stack if a stream is 
*/
PUBLIC HTList * HTRequest_error (HTRequest * request)
{
    return request ? request->error_stack : NULL;
}

PUBLIC void HTRequest_setError (HTRequest * request, HTList * list)
{
    if (request) request->error_stack = list;
}

PUBLIC BOOL HTRequest_addError (HTRequest * 	request,
				HTSeverity	severity,
				BOOL		ignore,
				int		element,
				void *		par,
				unsigned int	length,
				char *		where)
{
    if (request) {
	if (!request->error_stack) request->error_stack = HTList_new();
	return HTError_add(request->error_stack, severity, ignore, element,
			   par, length, where);
    }
    return NO;
}

PUBLIC BOOL HTRequest_addSystemError (HTRequest * 	request,
				      HTSeverity 	severity,
				      int		errornumber,
				      BOOL		ignore,
				      char *		syscall)
{
    if (request) {
	if (!request->error_stack) request->error_stack = HTList_new();
	return HTError_addSystem(request->error_stack, severity, errornumber,
				 ignore, syscall);
    }
    return NO;
}

/*
**	When to retry a request if HT_RETRY
**	Returns -1 if not available
*/
PUBLIC time_t HTRequest_retryTime (HTRequest * request)
{
    return request ? request->retry_after : -1;
}

/*
**    Redirection informantion
*/
PUBLIC HTAnchor * HTRequest_redirection (HTRequest * request)
{
    return (request ? request->redirectionAnchor : NULL);
}

PUBLIC BOOL HTRequest_setRedirection (HTRequest * request, HTAnchor * anchor)
{
    if (request && anchor) {
	request->redirectionAnchor = (HTAnchor *) HTAnchor_parent(anchor);
	return YES;
    }
    return NO;
}

/*
**  Set max number of automatic reload. Default is HT_MAX_RELOADS
*/
PUBLIC BOOL HTRequest_setMaxRetry (int newmax)
{
    if (newmax > 0) {
	HTMaxRetry = newmax;
	return YES;
    }
    return NO;
}

PUBLIC int HTRequest_maxRetry (void)
{
    return HTMaxRetry;
}

PUBLIC int HTRequest_retrys (HTRequest * request)
{
    return request ? request->retrys : 0;
}

PUBLIC BOOL HTRequest_addRetry (HTRequest * request)
{
    return (request && request->retrys++);
}

/*
**	Should we try again?
**	--------------------
**	Returns YES if we are to retry the load, NO otherwise. We check
**	this so that we don't go into an infinte loop
*/
PUBLIC BOOL HTRequest_doRetry (HTRequest *request)
{
    return (request && request->retrys < HTMaxRetry-1);
}

/*
**	Handle the max forward header value
*/
PUBLIC BOOL HTRequest_setMaxForwards (HTRequest * request, int maxforwards)
{
    if (request && maxforwards >= 0) {
	request->max_forwards = maxforwards;
	HTRequest_addRqHd(request, HT_C_MAX_FORWARDS);	       /* Turn it on */
	return YES;
    }
    return NO;
}

PUBLIC int HTRequest_maxForwards (HTRequest * request)
{
    return request ? request->max_forwards : -1;
}

/*
**  Priority to be inherited by all HTNet object hanging off this request
**  The priority can later be chaned by calling the HTNet object directly
*/
PUBLIC BOOL HTRequest_setPriority (HTRequest * request, HTPriority priority)
{
    if (request) {
	request->priority = priority;
	return YES;
    }
    return NO;
}

PUBLIC HTPriority HTRequest_priority (HTRequest * request)
{
    return (request ? request->priority : HT_PRIORITY_INV);
}

/*
**  Access Authentication Credentials
*/
PUBLIC BOOL HTRequest_deleteCredentials (HTRequest * request)
{
    if (request && request->credentials) {
	HTAssocList_delete(request->credentials);
	request->credentials = NULL;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTRequest_addCredentials (HTRequest * request,
				    char * token, char * value)
{
    if (request) {
	if (!request->credentials) request->credentials = HTAssocList_new();
	return HTAssocList_addObject(request->credentials, token, value);
    }
    return NO;
}

PUBLIC HTAssocList * HTRequest_credentials (HTRequest * request)
{
    return (request ? request->credentials : NULL);
}

/*
**  Access Authentication Challenges
*/
PUBLIC BOOL HTRequest_addChallenge (HTRequest * request,
				    char * token, char * value)
{
    if (request) {
	if (!request->challenge) request->challenge = HTAssocList_new();
	return HTAssocList_addObject(request->challenge, token, value);
    }
    return NO;
}

PUBLIC BOOL HTRequest_deleteChallenge (HTRequest * request)
{
    if (request && request->challenge) {
	HTAssocList_delete(request->challenge);
	request->challenge = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTRequest_challenge (HTRequest * request)
{
    return (request ? request->challenge : NULL);
}

/*
**  Access Authentication Realms
*/
PUBLIC BOOL HTRequest_setRealm (HTRequest * request, char * realm)
{
    if (request && realm) {
	StrAllocCopy(request->realm, realm);
	return YES;
    }
    return NO;
}

PUBLIC const char * HTRequest_realm (HTRequest * request)
{
    return (request ? request->realm : NULL);
}

/*
**  Access Authentication Schemes
*/
PUBLIC BOOL HTRequest_setScheme (HTRequest * request, char * scheme)
{
    if (request && scheme) {
	StrAllocCopy(request->scheme, scheme);
	return YES;
    }
    return NO;
}

PUBLIC const char * HTRequest_scheme (HTRequest * request)
{
    return (request ? request->scheme : NULL);
}

/*
**  Source request
*/
PUBLIC BOOL HTRequest_setSource (HTRequest * request, HTRequest * source)
{
    if (request) {
	request->source = source;
	return YES;
    }
    return NO;
}

PUBLIC HTRequest * HTRequest_source (HTRequest * request)
{
    return (request ? request->source : NULL);
}

PUBLIC BOOL HTRequest_isPostWeb (HTRequest * request)
{
    return (request ? request->source != NULL: NO);
}

/*
**  Internal request object
*/
PUBLIC BOOL HTRequest_setInternal (HTRequest * request, BOOL mode)
{
    if (request) {
	request->internal = mode;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTRequest_internal (HTRequest * request)
{
    return (request ? request->internal : NO);
}

/*
**	POST Call back function for sending data to the destination
*/
PUBLIC void HTRequest_setPostCallback (HTRequest *request, HTPostCallback *cbf)
{
    if (request) request->PostCallback = cbf;
}

PUBLIC HTPostCallback * HTRequest_postCallback (HTRequest * request)
{
    return request ? request->PostCallback : NULL;
}

/*
**	Entity Anchor
*/
PUBLIC BOOL HTRequest_setEntityAnchor (HTRequest * request,
				       HTParentAnchor * anchor)
{
    if (request) {
	request->source_anchor = anchor;
	return YES;
    }
    return NO;
}

PUBLIC HTParentAnchor * HTRequest_entityAnchor (HTRequest * request)
{
    return request ? request->source_anchor ? request->source_anchor :
	request->anchor : NULL;
}

/*
**	Simple Extension Protocol
*/
PUBLIC BOOL HTRequest_addExtension (HTRequest * request,
				    char * token, char * value)
{
    if (request) {
	if (!request->extension) request->extension = HTAssocList_new();
	return HTAssocList_addObject(request->extension, token, value);
    }
    return NO;
}

PUBLIC BOOL HTRequest_deleteExtension (HTRequest * request)
{
    if (request && request->extension) {
	HTAssocList_delete(request->extension);
	request->extension = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTRequest_extension (HTRequest * request)
{
    return (request ? request->extension : NULL);
}

/*
**	Cache control directives
*/
PUBLIC BOOL HTRequest_addCacheControl (HTRequest * request,
				    char * token, char * value)
{
    if (request) {
	if (!request->cache_control) request->cache_control=HTAssocList_new();
	return HTAssocList_addObject(request->cache_control, token, value);
    }
    return NO;
}

PUBLIC BOOL HTRequest_deleteCacheControl (HTRequest * request)
{
    if (request && request->cache_control) {
	HTAssocList_delete(request->cache_control);
	request->cache_control = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTRequest_cacheControl (HTRequest * request)
{
    return (request ? request->cache_control : NULL);
}

/*
**	Connection directives
*/
PUBLIC BOOL HTRequest_addConnection (HTRequest * request,
				    char * token, char * value)
{
    if (request) {
	if (!request->connection) request->connection=HTAssocList_new();
	return HTAssocList_addObject(request->connection, token, value);
    }
    return NO;
}

PUBLIC BOOL HTRequest_deleteConnection (HTRequest * request)
{
    if (request && request->connection) {
	HTAssocList_delete(request->connection);
	request->connection = NULL;
	return YES;
    }
    return NO;
}

PUBLIC HTAssocList * HTRequest_connection (HTRequest * request)
{
    return (request ? request->connection : NULL);
}

/* ------------------------------------------------------------------------- */
/*				POST WEB METHODS	      	 	     */
/* ------------------------------------------------------------------------- */

/*
**  Add a destination request to this source request structure so that we
**  build the internal request representation of the POST web
**  Returns YES if OK, else NO
*/
PUBLIC BOOL HTRequest_addDestination (HTRequest * src, HTRequest * dest)
{
    if (src && dest) {
	dest->source = src->source = src;
	if (!src->mainDestination) {
	    src->mainDestination = dest;
	    src->destRequests = 1;
	    if (CORE_TRACE)
		HTTrace("POSTWeb..... Adding dest %p to src %p\n",
			 dest, src);
	    return YES;
	} else {
	    if (!src->destinations) src->destinations = HTList_new();
	    if (HTList_addObject(src->destinations, (void *) dest)==YES) {
		src->destRequests++;
		if (CORE_TRACE)
		    HTTrace("POSTWeb..... Adding dest %p to src %p\n",
			     dest, src);
		return YES;
	    }
	}
    }
    return NO;
}

/*
**  Remove a destination request from this source request structure
**  Remember only to delete the internal request objects as the other
**  comes from the application!
**  Returns YES if OK, else NO
*/
PUBLIC BOOL HTRequest_removeDestination (HTRequest * dest)
{
    BOOL found=NO;
    if (dest && dest->source) {
	HTRequest *src = dest->source;
	if (src->mainDestination == dest) {
	    dest->source = NULL;
	    src->mainDestination = NULL;
	    src->destRequests--;
	    found = YES;
	} else if (src->destinations) {
	    if (HTList_removeObject(src->destinations, (void *) dest)) {
		src->destRequests--;
		found = YES;
	    }
	}
	if (found) {
	    if (dest->internal) HTRequest_delete(dest);
	    if (CORE_TRACE)
	    	HTTrace("POSTWeb..... Deleting dest %p from src %p\n",
			 dest, src);
	}
	if (src->destRequests <= 0) {
	    if (CORE_TRACE)
		HTTrace("POSTWeb..... terminated\n");
	    if (src->internal) HTRequest_delete(src);
	}
    }
    return found;
}

/*
**  Check to see whether all destinations are ready. If so then enable the
**  source as ready for reading.
**  Returns YES if all dests are ready, NO otherwise
*/
PUBLIC BOOL HTRequest_destinationsReady (HTRequest * me)
{
    HTRequest * source = me ? me->source : NULL;
    if (source) {
	if (source->destStreams == source->destRequests) {
	    HTNet * net = source->net;
	    if (CORE_TRACE)
		HTTrace("POSTWeb..... All destinations are ready!\n");
	    if (net)			      /* Might already have finished */
		HTEvent_register(net->sockfd, source, (SockOps) FD_READ,
				 net->cbf, net->priority);
	    return YES;
	}
    }
    return NO;
}

/*
**  Find the source request object and make the link between the 
**  source output stream and the destination input stream. There can be
**  a conversion between the two streams!
**  Returns YES if link is made, NO otherwise
*/
PUBLIC BOOL HTRequest_linkDestination (HTRequest *dest)
{
    if (dest && dest->input_stream && dest->source && dest!=dest->source) {
	HTRequest *source = dest->source;
	HTStream *pipe = HTStreamStack(source->output_format,
				       dest->input_format,
				       dest->input_stream,
				       dest, YES);

	/* Check if we are the only one - else spawn off T streams */
	/* @@@ We don't do this yet @@@ */

	/* Now set up output stream of the source */
	if (source->output_stream)
	    (*source->output_stream->isa->_free)(source->output_stream);
	source->output_stream = pipe ? pipe : dest->input_stream;

	if (CORE_TRACE)
	    HTTrace("POSTWeb..... Linking dest %p to src %p\n",
		     dest, source);
	if (++source->destStreams == source->destRequests) {
	    HTNet *net = source->net;
	    if (CORE_TRACE)
		HTTrace("POSTWeb..... All destinations ready!\n");
	    if (net)			      /* Might already have finished */
		HTEvent_register(net->sockfd, source, (SockOps) FD_READ,
				 net->cbf, net->priority);
	    return YES;
	}
    }
    return NO;
}

/*
**  Remove a feed stream to a destination request from this source
**  request structure. When all feeds are removed the request tree is
**  ready to take down and the operation can be terminated.
**  Returns YES if removed, else NO
*/
PUBLIC BOOL HTRequest_unlinkDestination (HTRequest *dest)
{
    BOOL found = NO;
    if (dest && dest->source && dest != dest->source) {
	HTRequest *src = dest->source;
	if (src->mainDestination == dest) {
	    src->output_stream = NULL;
	    if (dest->input_stream)
		(*dest->input_stream->isa->_free)(dest->input_stream);
	    found = YES;
	} else if (src->destinations) {

	    /* LOOK THROUGH THE LIST AND FIND THE RIGHT ONE */

	}	
	if (found) {
	    src->destStreams--;
	    if (CORE_TRACE)
		HTTrace("POSTWeb..... Unlinking dest %p from src %p\n",
			 dest, src);
	    return YES;
	}
    }
    return NO;
}

/*
**  Removes all request structures in this PostWeb.
*/
PUBLIC BOOL HTRequest_removePostWeb (HTRequest *me)
{
    if (me && me->source) {
	HTRequest *source = me->source;

	/* Kill main destination */
	if (source->mainDestination)
	    HTRequest_removeDestination(source->mainDestination);

	/* Kill all other destinations */
	if (source->destinations) {
	    HTList *cur = source->destinations;
	    HTRequest *pres;
	    while ((pres = (HTRequest *) HTList_nextObject(cur)) != NULL)
		HTRequest_removeDestination(pres);
	}

	/* Remove source request */
	HTRequest_removeDestination(source);
	return YES;
    }
    return NO;
}

/*
**  Kills all threads in a POST WEB connected to this request but
**  NOT this request itself. We also keep the request structures.
**  Some requests might be preemptive, for example a SMTP request (when
**  that has been implemented). However, this will be handled internally
**  in the load function.
*/
PUBLIC BOOL HTRequest_killPostWeb (HTRequest *me)
{
    if (me && me->source) {
	HTRequest *source = me->source;
	if (CORE_TRACE) HTTrace("POSTWeb..... Killing\n");

	/*
	** Kill source. The stream tree is now freed so we have to build
	** that again. This is done in HTRequest_linkDestination()
	*/
	if (me != source) {
	    HTNet_kill(source->net);
	    source->output_stream = NULL;
	}

	/* Kill all other destinations */
	if (source->destinations) {
	    HTList *cur = source->destinations;
	    HTRequest *pres;
	    while ((pres = (HTRequest *) HTList_nextObject(cur)) != NULL)
		if (me != pres) HTNet_kill(pres->net);
	}

	/* Kill main destination */
	if (source->mainDestination && me != source->mainDestination)
	    HTNet_kill(source->mainDestination->net);
	return YES;
    }
    return NO;
}

/* --------------------------------------------------------------------------*/
/*				Document Loader 			     */
/* --------------------------------------------------------------------------*/

/*	Request a resource
**	------------------
**	This is an internal routine, which has an address AND a matching
**	anchor.  (The public routines are called with one OR the other.)
**	Returns:
**		YES	if request has been registered (success)
**		NO	an error occured
*/
PUBLIC BOOL HTLoad (HTRequest * request, BOOL recursive)
{
    if (!request || !request->anchor) {
        if (CORE_TRACE) HTTrace("Load Start.. Bad argument\n");
        return NO;
    }
    HTAnchor_clearPhysical(request->anchor);
    if (request->method == METHOD_INVALID) request->method = METHOD_GET;
    if (!recursive && request->error_stack) {
	HTError_deleteAll(request->error_stack);
	request->error_stack = NULL;
    }
    return HTNet_newClient(request);
}

