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
#include "HTBind.h"
#include "HTProt.h"
#include "HTProxy.h"

#ifndef NO_RULES
#include "HTRules.h"
#endif

#include "HTReqMan.h"					 /* Implemented here */

/* These flags may be set to modify the operation of this module */
PUBLIC char * HTClientHost = NULL;	 /* Name of remote login host if any */
PUBLIC BOOL HTSecure = NO;		 /* Disable access for telnet users? */

PUBLIC char * HTImServer = NULL;/* cern_httpd sets this to the translated URL*/
PUBLIC BOOL HTImProxy = NO;			   /* cern_httpd as a proxy? */

/* Private flags */
#ifndef HT_MAX_RELOADS
#define HT_MAX_RELOADS	6
#endif
PRIVATE int HTMaxRetry = HT_MAX_RELOADS;

#ifdef _WINDOWS 
PUBLIC HWND HTsocketWin = 0;
PUBLIC unsigned long HTwinMsg = 0;
#endif 

/* Variables and typedefs local to this module */
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
    HTRequest * me = (HTRequest*) calloc(1, sizeof(HTRequest));
    if (!me) outofmem(__FILE__, "HTRequest_new()");
    
    /* User preferences for this particular request. Only empty lists! */
    me->conversions = HTList_new();
    me->encodings = HTList_new();
    me->languages = HTList_new();
    me->charsets = HTList_new();

    /* Force Reload */
    me->reload = HT_ANY_VERSION;

    /* Format of output */
    me->output_format	= WWW_PRESENT;	    /* default it to present to user */
    me->debug_format	= WWW_HTML;	 /* default format of error messages */

    /* HTTP headers */
    me->GenMask		= DEFAULT_GENERAL_HEADERS;
    me->RequestMask	= DEFAULT_REQUEST_HEADERS;
    me->EntityMask	= DEFAULT_ENTITY_HEADERS;

    /* Default retry after value */
    me->retry_after = -1;

    /* Content negotiation */
    me->ContentNegotiation = NO;		       /* Do this by default */

#ifdef _WINDOWS
    me->hwnd = HTsocketWin;
    me->winMsg = HTwinMsg;
#endif

    return me;
}


/*  Delete a request structure
**  --------------------------
*/
PUBLIC void HTRequest_delete (HTRequest * request)
{
    if (request) {
	FREE(request->redirect);
	FREE(request->authenticate);
	HTFormatDelete(request);
	HTErrorFree(request);
	HTAACleanup(request);

	if (request->net)			/* Break connection to HTNet */
	    request->net->request = NULL;

	/* These are temporary until we get a MIME thingy */
	FREE(request->redirect);
	FREE(request->WWWAAScheme);
	FREE(request->WWWAARealm);
	FREE(request->WWWprotection);

	FREE(request);
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
PUBLIC void HTRequest_setFormat (HTRequest *request, HTList *type,
				 BOOL override)
{
    if (request) {
	request->conversions = type;
	request->conv_local = override;
    }
}

PUBLIC HTList * HTRequest_format (HTRequest *request)
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
**	Extra headers to be sent
*/
PUBLIC void HTRequest_setExtra (HTRequest *request, char *extra)
{
    if (request) request->ExtraHeaders = extra;
}

PUBLIC char *HTRequest_extra (HTRequest *request)
{
    return request ? request->ExtraHeaders : NULL;
}

/*
**	Output stream
*/
PUBLIC void HTRequest_setOutputStream (HTRequest *request, HTStream *output)
{
    if (request) request->output_stream = output;
}

PUBLIC HTStream *HTRequest_OutputStream (HTRequest *request)
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

PUBLIC HTFormat HTRequest_OutputFormat (HTRequest *request)
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

PUBLIC HTStream *HTRequest_DebugStream (HTRequest *request)
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

PUBLIC HTFormat HTRequest_DebugFormat (HTRequest *request)
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
**	Socket mode: preemtive or non-preemtive (blocking or non-blocking)
*/
PUBLIC void HTRequest_setPreemtive (HTRequest *request, BOOL mode)
{
    if (request) request->preemtive = mode;
}

PUBLIC BOOL HTRequest_preemtive (HTRequest *request)
{
    return request ? request->preemtive : NO;
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
**	Kill this request
*/
PUBLIC BOOL HTRequest_kill(HTRequest * request)
{
    return request ? HTNet_kill(request->net) : NO;
}

/*
**	Error stack
**	-----------
**	Returns the error stack if a stream is 
*/
PUBLIC HTList *HTRequest_errorStack (HTRequest *request)
{
    return request ? request->error_stack : NULL;
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

/* ------------------------------------------------------------------------- */
/*				POST WEB METHODS	      	 	     */
/* ------------------------------------------------------------------------- */

/*
**  Add a destination request to this source request structure so that we
**  build the internal request representation of the POST web
**  Returns YES if OK, else NO
*/
PUBLIC BOOL HTRequest_addDestination (HTRequest *src, HTRequest *dest)
{
    if (src && dest) {
	if (!src->mainDestination) {
	    src->mainDestination = dest;
	    src->destRequests = 1;
	    return YES;
	} else {
	    if (!src->destinations)
		src->destinations = HTList_new();
	    if (HTList_addObject(src->destinations, (void *) dest)==YES) {
		src->destRequests++;
		return YES;
	    }
	}
    }
    return NO;
}

/*
**  Remove a destination request from this source request structure
**  Remember not to delete the main destination as it comes from the
**  application!
**  Returns YES if OK, else NO
*/
PUBLIC BOOL HTRequest_removeDestination (HTRequest *dest)
{
    BOOL found=NO;
    if (dest && dest->source) {
	HTRequest *src = dest->source;
	if (src->mainDestination == dest) {
	    dest->source = NULL;
	    src->mainDestination = NULL;
	    src->destRequests--;
	    found = YES;
	} if (src->destinations) {
	    if (HTList_removeObject(src->destinations, (void *) dest)) {
		HTRequest_delete(dest);
		src->destRequests--;
		found = YES;
	    }
	}
	if (found) {
	    if (TRACE)
		fprintf(TDEST, "Destination. %p removed from %p\n",
			dest, src);
	}
	if (!src->destRequests) {
	    if (TRACE)
		fprintf(TDEST, "Destination. PostWeb terminated\n");
	    HTRequest_delete(src);
	}
    }
    return found;
}

/*
**  Find the source request structure and make the link between the 
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

	source->output_stream = pipe ? pipe : dest->input_stream;

	if (STREAM_TRACE)
	    fprintf(TDEST,"Destination. Linked %p to source %p\n",dest,source);
	if (++source->destStreams == source->destRequests) {
	    HTNet *net = source->net;
	    if (STREAM_TRACE)
		fprintf(TDEST, "Destination. All destinations ready!\n");
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
	    if (STREAM_TRACE)
		fprintf(TDEST, "Destination. Unlinked %p from source %p\n",
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
**  keep the request structures.
**  Some requests might be preemtive, for example a SMTP request (when
**  that has been implemented). However, this will be handled internally
**  in the load function.
*/
PUBLIC BOOL HTRequest_killPostWeb (HTRequest *me)
{
    if (me && me->source) {
	HTRequest *source = me->source;

	/* Kill main destination */
	if (source->mainDestination)
	    HTNet_kill(source->mainDestination->net);

	/* Kill all other destinations */
	if (source->destinations) {
	    HTList *cur = source->destinations;
	    HTRequest *pres;
	    while ((pres = (HTRequest *) HTList_nextObject(cur)) != NULL)
		HTNet_kill(pres->net);
	}
	/*
	** Kill source. The stream tree is now freed so we have to build
	** that again. This is done in HTRequest_linkDestination()
	*/
	HTNet_kill(source->net);
	source->output_stream = NULL;
	return YES;
    }
    return NO;
}

/* --------------------------------------------------------------------------*/
/*			Physical Anchor Address Manager			     */
/* --------------------------------------------------------------------------*/

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

#ifndef HT_NO_RULES
    if (HTImServer) {	/* cern_httpd has already done its own translations */
	HTAnchor_setPhysical(req->anchor, HTImServer);
	StrAllocCopy(addr, HTImServer);	/* Oops, queries thru many proxies */
					/* didn't work without this -- AL  */
    }
    else {
	char * physical = HTTranslate(addr);
	if (!physical) {
	    free(addr);
	    return HT_FORBIDDEN;
	}
	HTAnchor_setPhysical(req->anchor, physical);
	free(physical);			/* free our copy */
    }
#else
    HTAnchor_setPhysical(req->anchor, addr);
#endif /* HT_NO_RULES */

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
	} else if ((newaddr = HTProxy_getProxy(addr))) {
	    StrAllocCat(newaddr, addr);
	    req->using_proxy = YES;
	    HTAnchor_setPhysical(req->anchor, newaddr);
	} else if ((newaddr = HTProxy_getGateway(addr))) {
	    char * path = HTParse(addr, "",
				  PARSE_HOST + PARSE_PATH + PARSE_PUNCTUATION);
		/* Chop leading / off to make host into part of path */
	    char * gatewayed = HTParse(path+1, newaddr, PARSE_ALL);
            HTAnchor_setPhysical(req->anchor, gatewayed);
	    free(path);
	    free(gatewayed);
	} else {
	    req->using_proxy = NO;     	    /* We don't use proxy or gateway */
	}
	FREE(newaddr);
    }
    FREE(addr);

    /* Set the access scheme on our way out */
    return (HTProtocol_bind(req->anchor)==YES) ? HT_OK : HT_NO_ACCESS;
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
PUBLIC BOOL HTLoad (HTRequest * request, HTPriority priority, BOOL recursive)
{
    char *arg = NULL;
    int status;
    if (!request || !request->anchor) {
        if (PROT_TRACE) fprintf(TDEST, "Load Start.. Bad argument\n");
        return NO;
    }

    /*
    ** Check if document is already loaded. As the application handles the
    ** memory cache, we call the application to ask.
    */
    if (request->reload != HT_FORCE_RELOAD) {
	if (HTMemoryCache_check(request) == HT_LOADED)
	    return HTNet_callback(request, HT_LOADED);
    } else {
	request->RequestMask |= HT_NO_CACHE;		  /* no-cache pragma */
	HTAnchor_clearHeader(request->anchor);
    }

    if (request->method == METHOD_INVALID)
	request->method = METHOD_GET;

    if (!recursive)
	HTErrorFree(request);

    if ((status = get_physical(request)) < 0) {
	if (status == HT_FORBIDDEN) {
	    char *url = HTAnchor_address((HTAnchor *) request->anchor);
	    if (url) {
		HTUnEscape(url);
		HTErrorAdd(request, ERR_FATAL, NO, HTERR_FORBIDDEN,
			   (void *) url, (int) strlen(url), "HTLoad");
		free(url);
	    } else {
		HTErrorAdd(request, ERR_FATAL, NO, HTERR_FORBIDDEN,
			   NULL, 0, "HTLoad");
	    }
	} 
	return HTNet_callback(request, HT_ERROR);
	return YES;			       /* Can't resolve or forbidden */
    }
    if (!(arg = HTAnchor_physical(request->anchor)) || !*arg)
	return NO;
    return HTNet_new(request, priority);
}


/*		Terminate a LOAD
**		----------------
**
**	This function looks at the status code from the HTLoadDocument
**	function and updates logfiles, creates error messages etc.
**
**    On Entry,
**	Status code from load function
*/
PUBLIC int HTLoad_terminate (HTRequest *request, int status)
{
    char * uri = HTAnchor_address((HTAnchor*)request->anchor);

    /*
    ** The error stack might contain general information to the client
    ** about what has been going on in the library (not only errors)
    */
    if (!HTImProxy && request->error_stack)
	HTErrorMsg(request);

    switch (status) {
      case HT_LOADED:
	if (PROT_TRACE)
	    fprintf(TDEST, "Load End.... OK: `%s\' has been accessed.\n", uri);
	break;

      case HT_NO_DATA:
	if (PROT_TRACE)
	    fprintf(TDEST, "Load End.... OK BUT NO DATA: `%s\'\n", uri);
	break;

      case HT_INTERRUPTED:
	if (PROT_TRACE)
	    fprintf(TDEST, "Load End.... INTERRUPTED: `%s\'\n", uri);
	break;

      case HT_RETRY:
	if (PROT_TRACE)
	    fprintf(TDEST, "Load End.... NOT AVAILABLE, RETRY AT %ld\n",
		    HTRequest_retryTime(request));
	break;

      case HT_ERROR:
	if (HTImProxy)
	    HTErrorMsg(request);		     /* Only on a real error */
	if (PROT_TRACE)
	    fprintf(TDEST, "Load End.... ERROR: Can't access `%s\'\n", uri);
	break;

      default:
	if (PROT_TRACE)
	    fprintf(TDEST, "Load End.... UNKNOWN RETURN CODE %d\n", status);
	break;
    }
    free(uri);
    return YES;
}

