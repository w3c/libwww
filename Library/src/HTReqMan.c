/*								     HTReqMan.c
**	REQUEST MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
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
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTAlert.h"
#include "HTError.h"
#include "HTList.h"
#include "HTCache.h"
#include "HTNetMan.h"
#include "HTEvntrg.h"
#include "HTProt.h"
#include "HTProxy.h"
#include "HTReqMan.h"					 /* Implemented here */

#include "HTRules.h"

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
    me->reload = HT_ANY_VERSION;

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

    /* Content negotiation */
    me->ContentNegotiation = NO;		       /* Do this by default */

#ifdef WWW_WIN_ASYNC
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
	me->boundary = NULL;
	me->authenticate = NULL;
	me->error_stack = NULL;
	me->authorization = NULL;
	me->prot_template = NULL;
	me->dialog_msg = NULL;
	me->net = NULL;
	me->WWWAAScheme = NULL;
	me->WWWAARealm = NULL;
	me->WWWprotection = NULL;
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
    if (!src) return NO;
    if ((me = (HTRequest  *) HT_MALLOC(sizeof(HTRequest))) == NULL)
        HT_OUTOFMEM("HTRequest_dup");
    memcpy(me, src, sizeof(HTRequest));
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
    if (!src) return NO;
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
	HT_FREE(request->boundary);
	HT_FREE(request->authenticate);
	if (request->error_stack) HTError_deleteAll(request->error_stack);

	HT_FREE(request->authorization);
	HT_FREE(request->prot_template);
	HT_FREE(request->dialog_msg);

	if (request->net)			/* Break connection to HTNet */
	    request->net->request = NULL;

	/* These are temporary until we get a MIME thingy */
	HT_FREE(request->WWWAAScheme);
	HT_FREE(request->WWWAARealm);
	HT_FREE(request->WWWprotection);

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
    return request ? request->reload : HT_ANY_VERSION;
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
PUBLIC void HTRequest_setParser (HTRequest *request, HTList *parser,
				 BOOL override)
{
    if (request) {
	request->parsers = parser;
	request->pars_local = override;
    }
}

PUBLIC HTList * HTRequest_parser (HTRequest *request, BOOL *override)
{
    if (request) {
	*override = request->pars_local;
	return request->parsers;
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
    if (request && anchor) {
	request->anchor = HTAnchor_parent(anchor);
	request->childAnchor = ((HTAnchor *) request->anchor != anchor) ?
	    (HTChildAnchor *) anchor : NULL;
    }
}

PUBLIC HTParentAnchor * HTRequest_anchor (HTRequest *request)
{
    return request ? request->anchor : NULL;
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
**	Call back function for proxying
*/
PUBLIC void HTRequest_setProxying (HTRequest * request, BOOL proxying)
{
    if (request) request->using_proxy = proxying;
}

PUBLIC BOOL HTRequest_proxying (HTRequest * request)
{
    return request ? request->using_proxy : NO;
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

/*
**	Should we try again?
**	--------------------
**	Returns YES if we are to retry the load, NO otherwise. We check
**	this so that we don't go into an infinte loop
*/
PUBLIC BOOL HTRequest_retry (HTRequest *request)
{
    return (request && request->retrys < HTMaxRetry-1);
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
**  Get and set the NET manager
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
    return (request ? request->net : NULL);
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
	    if (WWWTRACE)
		TTYPrint(TDEST,"POSTWeb..... Adding dest %p to src %p\n",
			 dest, src);
	    return YES;
	} else {
	    if (!src->destinations) src->destinations = HTList_new();
	    if (HTList_addObject(src->destinations, (void *) dest)==YES) {
		src->destRequests++;
		if (WWWTRACE)
		    TTYPrint(TDEST,"POSTWeb..... Adding dest %p to src %p\n",
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
	    if (WWWTRACE)
	    	TTYPrint(TDEST, "POSTWeb..... Deleting dest %p from src %p\n",
			 dest, src);
	}
	if (src->destRequests <= 0) {
	    if (WWWTRACE)
		TTYPrint(TDEST, "POSTWeb..... terminated\n");
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
	    if (WWWTRACE)
		TTYPrint(TDEST, "POSTWeb..... All destinations are ready!\n");
	    if (net)			      /* Might already have finished */
		HTEvent_Register(net->sockfd, source, (SockOps) FD_READ,
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

	if (WWWTRACE)
	    TTYPrint(TDEST,"POSTWeb..... Linking dest %p to src %p\n",
		     dest, source);
	if (++source->destStreams == source->destRequests) {
	    HTNet *net = source->net;
	    if (WWWTRACE)
		TTYPrint(TDEST, "POSTWeb..... All destinations ready!\n");
	    if (net)			      /* Might already have finished */
		HTEvent_Register(net->sockfd, source, (SockOps) FD_READ,
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
	    if (WWWTRACE)
		TTYPrint(TDEST, "POSTWeb..... Unlinking dest %p from src %p\n",
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
	if (WWWTRACE) TTYPrint(TDEST, "POSTWeb..... Killing\n");

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
/*			Physical Anchor Address Manager			     */
/* --------------------------------------------------------------------------*/
#if 0
/*		Find physical name and access protocol
**		--------------------------------------
**
**	Checks for Cache, proxy, and gateway (in that order)
**
** On exit,    
**	returns		HT_NO_ACCESS		no protocol module found
**			HT_FORBIDDEN		Error has occured.
**			HT_OK			Success
**
*/
PRIVATE int get_physical (HTRequest *req)
{    
    char * addr = HTAnchor_address((HTAnchor*)req->anchor);	/* free me */
    HTList *list = HTRule_global();
    char * physical = HTRule_translate(list, addr, NO);
    if (!physical) {
	HT_FREE(addr);
	return HT_FORBIDDEN;
    }
    HTAnchor_setPhysical(req->anchor, physical);
    HT_FREE(physical);

    /*
    ** Check local Disk Cache (if we are not forced to reload), then
    ** for proxy, and finally gateways
    */
    {
	char *newaddr=NULL;
	if (req->reload != HT_FORCE_RELOAD &&
	    (newaddr = HTCache_getReference(addr))) {
	    if (req->reload != HT_CACHE_REFRESH) {
		HTAnchor_setPhysical(req->anchor, newaddr);
		HTAnchor_setCacheHit(req->anchor, YES);
	    } else {			 /* If refresh version in file cache */
		req->RequestMask |= (HT_IMS + HT_NO_CACHE);
	    }
	} else if ((newaddr = HTProxy_find(addr))) {
	    StrAllocCat(newaddr, addr);
	    req->using_proxy = YES;
	    HTAnchor_setPhysical(req->anchor, newaddr);
	} else if ((newaddr = HTGateway_find(addr))) {
	    char * path = HTParse(addr, "",
				  PARSE_HOST + PARSE_PATH + PARSE_PUNCTUATION);
		/* Chop leading / off to make host into part of path */
	    char * gatewayed = HTParse(path+1, newaddr, PARSE_ALL);
            HTAnchor_setPhysical(req->anchor, gatewayed);
	    HT_FREE(path);
	    HT_FREE(gatewayed);
	} else {
	    req->using_proxy = NO;     	    /* We don't use proxy or gateway */
	}
	HT_FREE(newaddr);
    }
    HT_FREE(addr);

    /* Set the access scheme on our way out */
    return (HTProtocol_find(req, req->anchor)==YES) ? HT_OK : HT_NO_ACCESS;
}
#endif

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
        if (PROT_TRACE) TTYPrint(TDEST, "Load Start.. Bad argument\n");
        return NO;
    }
    if (request->method == METHOD_INVALID) request->method = METHOD_GET;
    if (!recursive && request->error_stack) {
	HTError_deleteAll(request->error_stack);
	request->error_stack = NULL;
    }
    return HTNet_newClient(request);
}

