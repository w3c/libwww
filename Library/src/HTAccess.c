/*								     HTAccess.c
**	ACCESS MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
** Authors
**	TBL	Tim Berners-Lee timbl@w3.org
**	JFG	Jean-Francois Groff jfg@dxcern.cern.ch
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
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
** Bugs
**	This module assumes that that the graphic object is hypertext, as it
**	needs to select it when it has been loaded.  A superclass needs to be
**	defined which accepts select and select_anchor.
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
#include "HTAABrow.h"				/* Should be HTAAUtil.html! */
#include "HTFWrite.h"	/* for cache stuff */
#include "HTLog.h"
#include "HTSocket.h"
#include "HTTCP.h"      /* HWL: for HTFindRelatedName */
#include "HTThread.h"
#include "HTEvent.h"
#include "HTBind.h"
#include "HTInit.h"
#include "HTProxy.h"
#include "HTML.h"		/* SCW */
#include "HText.h"	/* See bugs above */

#ifndef NO_RULES
#include "HTRules.h"
#endif

#include "HTAccess.h"					 /* Implemented here */

/* These flags may be set to modify the operation of this module */
PUBLIC int  HTMaxRedirections = 10;	       /* Max number of redirections */

PUBLIC char * HTClientHost = NULL;	 /* Name of remote login host if any */
PUBLIC BOOL HTSecure = NO;		 /* Disable access for telnet users? */

PUBLIC char * HTImServer = NULL;/* cern_httpd sets this to the translated URL*/
PUBLIC BOOL HTImProxy = NO;			   /* cern_httpd as a proxy? */

#ifdef _WINDOWS 
PUBLIC HWND HTsocketWin = 0 ;
unsigned long HTwinMsg = 0 ;
#endif 

/* Variables and typedefs local to this module */
PRIVATE HTList * protocols = NULL;           /* List of registered protocols */

/* Superclass defn */
struct _HTStream {
	HTStreamClass * isa;
	/* ... */
};

/* --------------------------------------------------------------------------*/
/*			Management of the HTRequest structure		     */
/* --------------------------------------------------------------------------*/

/*	Create  a request structure
**	---------------------------
*/
PUBLIC HTRequest * HTRequest_new NOARGS
{
    HTRequest * me = (HTRequest*) calloc(1, sizeof(*me));  /* zero fill */
    if (!me) outofmem(__FILE__, "HTRequest_new()");
    
    /* User preferences for this particular request. Only empty lists! */
    me->conversions = HTList_new();
    me->encodings = HTList_new();
    me->languages = HTList_new();
    me->charsets = HTList_new();

    /* Format of output */
    me->output_format	= WWW_PRESENT;	    /* default it to present to user */
    me->error_format	= WWW_HTML;	 /* default format of error messages */

    /* HTTP headers */
    me->GenMask		= DEFAULT_GENERAL_HEADERS;
    me->RequestMask	= DEFAULT_REQUEST_HEADERS;
    me->EntityMask	= DEFAULT_ENTITY_HEADERS;

    /* Content negotiation */
    me->ContentNegotiation = NO;		       /* Do this by default */

#ifdef _WINDOWS
    me->hwnd = HTsocketWin;
    me->winMsg = HTwinMsg;
#endif

    return me;
}


/*	Clear  a request structure
**	---------------------------
**	This function clears the reguest structure so that only the
**	conversions remain. Everything else is as if it was created from
**	scratch.
*/
PUBLIC void HTRequest_clear ARGS1(HTRequest *, req)
{
    HTList *conversions;
    if (!req) {
	if (TRACE)
	    fprintf(TDEST, "Clear....... request: Bad argument!\n");
	return;
    }
    conversions = req->conversions;		     /* Save the conversions */
    HTErrorFree(req);
    HTAACleanup(req);
    memset(req, '\0', sizeof(HTRequest));

    /* Now initialize as from scratch but with the old list of conversions */
    req->conversions = conversions;
    req->output_format = WWW_PRESENT;	    /* default it to present to user */

#ifdef _WINDOWS   
    req->hwnd = HTsocketWin ;
    req->winMsg = HTwinMsg ;
#endif 

}


/*	Delete a request structure
**	--------------------------
*/
PUBLIC void HTRequest_delete ARGS1(HTRequest *, req)
{
    if (req) {
	FREE(req->redirect);
	FREE(req->authenticate);
	HTFormatDelete(req);
	HTErrorFree(req);
	HTAACleanup(req);
	if (req->CopyRequest)
	    HTRequest_delete(req->CopyRequest);

	/* These are temporary until we get a MIME thingy */
	FREE(req->redirect);
	FREE(req->WWWAAScheme);
	FREE(req->WWWAARealm);
	FREE(req->WWWprotection);

	FREE(req);
    }
}

/* --------------------------------------------------------------------------*/
/*			Management of HTTP Methods			     */
/* --------------------------------------------------------------------------*/

static char *method_names[] =
{
    "INVALID-METHOD",
    "GET",
    "HEAD",
    "POST",
    "PUT",
    "DELETE",
    "LINK",
    "UNLINK",
    NULL
};

/*	Get method enum value
**	---------------------
*/
PUBLIC HTMethod HTMethod_enum ARGS1(CONST char *, name)
{
    if (name) {
	if (!strcmp(name, *(method_names+1)))
	    return METHOD_GET;
	else if (!strcmp(name, *(method_names+2)))
	    return METHOD_HEAD;
	else if (!strcmp(name, *(method_names+3)))
	    return METHOD_POST;
	else if (!strcmp(name, *(method_names+4)))
	    return METHOD_PUT;
	else if (!strcmp(name, *(method_names+5)))
	    return METHOD_DELETE;
	else if (!strcmp(name, *(method_names+6)))
	    return METHOD_LINK;
	else if (!strcmp(name, *(method_names+7)))
	    return METHOD_UNLINK;
    }
    return METHOD_INVALID;
}


/*	Get method name
**	---------------
**	Returns pointer to entry in static table in memory
*/
PUBLIC CONST char * HTMethod_name ARGS1(HTMethod, method)
{
    if (method & METHOD_GET)
	return *(method_names+1);
    else if (method == METHOD_HEAD)
	return *(method_names+2);
    else if (method == METHOD_POST)
	return *(method_names+3);
    else if (method == METHOD_PUT)
	return *(method_names+4);
    else if (method == METHOD_DELETE)
	return *(method_names+5);
    else if (method == METHOD_LINK)
	return *(method_names+6);
    else if (method == METHOD_UNLINK)
	return *(method_names+7);
    else
	return *method_names;
#if 0
    if ((int)METHOD_INVALID  && (int)method < (int)MAX_METHODS)
	return method_names[(int)method];
    else
	return method_names[(int)METHOD_INVALID];
#endif
}


#if 0
/* NOT NEEDED AS METHODS IS NOT A BIT-FLAG */
/*	Is method in a list of method names?
**	-----------------------------------
*/
PUBLIC BOOL HTMethod_inList ARGS2(HTMethod,	method,
				  HTList *,	list)
{
    char * method_name = HTMethod_name(method);
    HTList *cur = list;
    char *item;

    while (NULL != (item = (char*)HTList_nextObject(cur))) {
	if (PROT_TRACE)
	    fprintf(TDEST, " %s", item);
	if (0==strcasecomp(item, method_name))
	    return YES;
    }
    return NO;	/* Not found */
}
#endif

/* --------------------------------------------------------------------------*/
/*		      Management of the HTProtocol structure		     */
/* --------------------------------------------------------------------------*/

/*
**	Register a Protocol as an active access method
*/
PUBLIC BOOL HTRegisterProtocol ARGS1(HTProtocol *, protocol)
{
    if (!protocols) protocols = HTList_new();
    HTList_addObject(protocols, (void *) protocol);
    return YES;
}


/*
**	Delete the list of registered access methods. This is called from
**	within HTLibTerminate. Written by Eric Sink, eric@spyglass.com
*/
PUBLIC void HTDisposeProtocols NOARGS
{
    if (protocols) {
	HTList_delete(protocols);
	protocols = NULL;
    }
}


/*
**	Is a protocol registered as BLOCKING? The default behavior registered
**	when the protocol module was registered can be overridden by the
**	BlockingIO field in the HTRequest structure
*/
PUBLIC BOOL HTProtocolBlocking ARGS1(HTRequest *, me)
{
    if (me) {
	return (me->BlockingIO || (me->anchor && me->anchor->protocol &&
		((HTProtocol *) (me->anchor->protocol))->block == SOC_BLOCK));
    }
    return NO;
}

/* --------------------------------------------------------------------------*/
/*	           Initialization and Termination of the Library	     */
/* --------------------------------------------------------------------------*/

/*								     HTLibInit
**
**	This function initiates the Library and it MUST be called when
**	starting up an application. See also HTLibTerminate()
*/
PUBLIC BOOL HTLibInit NOARGS
{
#ifdef NO_STDIO						  /* Open trace file */
    if ((TDEST = fopen(TRACE_FILE, "a")) != NULL) {
	if (setvbuf(TDEST, NULL, _IOLBF, 0) < 0) {  /* Change to line buffer */
	    printf("WWWLibInit.. Can't initialize TRACE buffer - no TRACE\n");
	    fclose(TDEST);
	    TDEST = NULL;
	    WWW_TraceFlag = 0;
	}
    } else
	WWW_TraceFlag = 0;
#endif

    if (TRACE)
	fprintf(TDEST, "WWWLibInit.. INITIALIZING LIBRARY OF COMMON CODE\n");

    /* Set up User preferences, but leave initialization to the application */
    if (!HTConversions)
	HTConversions = HTList_new();
    if (!HTEncodings)
	HTEncodings = HTList_new();
    if (!HTLanguages)
	HTLanguages = HTList_new();
    if (!HTCharsets)
	HTCharsets = HTList_new();

    /* Set up bindings to the local file system */
    HTBind_init();

#ifndef HT_NO_INIT
    HTAccessInit();		 /* Bind access schemes and protocol modules */
    HTFileInit();		     /* Bind file extensions and media types */
#endif

#ifndef HT_DIRECT_WAIS
    HTProxy_setGateway("wais", HT_DEFAULT_WAIS_GATEWAY);
#endif

#ifdef WWWLIB_SIG
    /* On Solaris (and others?) we get a BROKEN PIPE signal when connecting
    ** to a port where we should get `connection refused'. We ignore this 
    ** using the following function call
    */
    HTSetSignal();				   /* Set signals in library */
#endif

#ifdef _WINDOWS
    /*
    ** Initialise WinSock DLL. This must also be shut down! PMH
    */
    {
        WSADATA            wsadata;
	if (WSAStartup(DESIRED_WINSOCK_VERSION, &wsadata)) {
	    if (TRACE)
		fprintf(TDEST, "WWWLibInit.. Can't initialize WinSoc\n");
            WSACleanup();
            return NO;
        }
        if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION) {
            if (TRACE)
		fprintf(TDEST, "WWWLibInit.. Bad version of WinSoc\n");
            WSACleanup();
            return NO;
        }
    }
#endif /* _WINDOWS */

#ifndef NO_TIMEGM
    HTGetTimeZoneOffset();	   /* Find offset from GMT if using mktime() */
#endif
    HTTmp_setRoot(NULL);		     /* Set up default tmp directory */
    HTThreadInit();			            /* Initialize bit arrays */
    return YES;
}


/*								 HTLibTerminate
**
**	This function frees memory kept by the Library and should be called
**	before exit of an application (if you are on a PC platform)
*/
PUBLIC BOOL HTLibTerminate NOARGS
{
    if (TRACE)
	fprintf(TDEST, "WWWLibTerm.. Cleaning up LIBRARY OF COMMON CODE\n");
    HTAtom_deleteAll();
    HTDisposeConversions();
    HTTCPCacheRemoveAll();

#ifndef HT_NO_INIT
    HTDisposeProtocols();    /* Remove bindings between access and protocols */
    HTBind_deleteAll();	    /* Remove bindings between suffixes, media types */
#endif

    HTProxy_deleteProxy();	   /* Clean up lists of proxies and gateways */
    HTProxy_deleteNoProxy();
    HTProxy_deleteGateway();

    HTFreeHostName();			    /* Free up some internal strings */
    HTFreeMailAddress();
    HTCache_freeRoot();
    HTTmp_freeRoot();

#ifdef _WINDOWS
    WSACleanup();
#endif

#ifdef NO_STDIO						 /* Close trace file */
    if (TDEST) {
	fclose(TDEST);
	TDEST = NULL;
	WWW_TraceFlag = 0;
    }
#endif
    return YES;
}

/* --------------------------------------------------------------------------*/
/*			Physical Anchor Address Manager			     */
/* --------------------------------------------------------------------------*/

#ifdef OLD_CODE
/*							override_proxy()
**
**	Check the no_proxy environment variable to get the list
**	of hosts for which proxy server is not consulted.
**
**	no_proxy is a comma- or space-separated list of machine
**	or domain names, with optional :port part.  If no :port
**	part is present, it applies to all ports on that domain.
**
**	Example:
**		no_proxy="cern.ch,some.domain:8001"
**
*/
PRIVATE BOOL override_proxy ARGS1(CONST char *, addr)
{
    CONST char * no_proxy = getenv("no_proxy");
    char * p = NULL;
    char * host = NULL;
    int port = 0;
    int h_len = 0;

    if (!no_proxy || !addr || !(host = HTParse(addr, "", PARSE_HOST)))
	return NO;
    if (!*host) { free(host); return NO; }

    if ((p = strchr(host, ':')) != NULL) {	/* Port specified */
	*p++ = 0;			/* Chop off port */
	port = atoi(p);
    }
    else {				/* Use default port */
	char * access = HTParse(addr, "", PARSE_ACCESS);
	if (access) {
	    if	    (!strcmp(access,"http"))	port = 80;
	    else if (!strcmp(access,"gopher"))	port = 70;
	    else if (!strcmp(access,"ftp"))	port = 21;
	    free(access);
	}
    }
    if (!port) port = 80;		/* Default */
    h_len = strlen(host);

    while (*no_proxy) {
	CONST char * end;
	CONST char * colon = NULL;
	int templ_port = 0;
	int t_len;

	while (*no_proxy && (WHITE(*no_proxy) || *no_proxy==','))
	    no_proxy++;			/* Skip whitespace and separators */

	end = no_proxy;
	while (*end && !WHITE(*end) && *end != ',') {	/* Find separator */
	    if (*end==':') colon = end;			/* Port number given */
	    end++;
	}

	if (colon) {
	    templ_port = atoi(colon+1);
	    t_len = colon - no_proxy;
	}
	else {
	    t_len = end - no_proxy;
	}

	if ((!templ_port || templ_port == port)  &&
	    (t_len > 0  &&  t_len <= h_len  &&
	     !strncmp(host + h_len - t_len, no_proxy, t_len))) {
	    free(host);
	    return YES;
	}
	if (*end) no_proxy = end+1;
	else break;
    }

    free(host);
    return NO;
}
#endif /* OLD_CODE */


/*		Find physical name and access protocol
**		--------------------------------------
**
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**	anchor		a pareent anchor with whose address is addr
**
** On exit,    
**	returns		HT_NO_ACCESS		no protocol module found
**			HT_FORBIDDEN		Error has occured.
**			HT_OK			Success
**
*/
PRIVATE int get_physical ARGS1(HTRequest *, req)
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

#ifdef OLDCODE
    access =  HTParse(HTAnchor_physical(req->anchor),
		      "file:", PARSE_ACCESS);

    if (!override_proxy(addr)) {
    /* make sure the using_proxy variable is false */
    req->using_proxy = NO;

	char * gateway_parameter, *gateway, *proxy;

	gateway_parameter = (char *)malloc(strlen(access)+20);
	if (gateway_parameter == NULL) outofmem(__FILE__, "HTLoad");

	/* search for proxy gateways */
	strcpy(gateway_parameter, "WWW_");
	strcat(gateway_parameter, access);
	strcat(gateway_parameter, "_GATEWAY");
	gateway = (char *)getenv(gateway_parameter); /* coerce for decstation */

	/* search for proxy servers */
	strcpy(gateway_parameter, access);
	strcat(gateway_parameter, "_proxy");
	proxy = (char *)getenv(gateway_parameter);

	free(gateway_parameter);

#ifndef HT_DIRECT_WAIS
	if (!gateway && 0==strcmp(access, "wais")) {
	    gateway = HT_DEFAULT_WAIS_GATEWAY;
	}
#endif

	if (TRACE && gateway)
	    fprintf(TDEST,"Gateway..... Found: `%s\'\n", gateway);
	if (TRACE && proxy)
	    fprintf(TDEST,"Proxy....... Found: `%s\'\n", proxy);
#endif /* OLD_CODE */

    /*
    **  Check whether gateway or proxy access has been set up for this url
    */
    {
	char *proxy = HTProxy_getProxy(addr);
	char *gateway = HTProxy_getGateway(addr);

	/* Proxy servers have precedence over gateway servers */
	if (proxy) {
	    StrAllocCat(proxy, addr);
	    req->using_proxy = YES;
	    HTAnchor_setPhysical(req->anchor, proxy);
	    free(proxy);
	} else if (gateway) {
	    char * path = HTParse(addr, "",
				  PARSE_HOST + PARSE_PATH + PARSE_PUNCTUATION);
		/* Chop leading / off to make host into part of path */
	    char * gatewayed = HTParse(path+1, gateway, PARSE_ALL);
            HTAnchor_setPhysical(req->anchor, gatewayed);
	    free(path);
	    free(gatewayed);
	    free(gateway);
	} else {
	    req->using_proxy = NO;     	    /* We don't use proxy or gateway */
	}
    }
    FREE(addr);

    /*
    ** Search registered protocols to find suitable one
    */
    {
	char *access = HTParse(HTAnchor_physical(req->anchor),"",PARSE_ACCESS);
	HTList *cur = protocols;
	HTProtocol *p;
	if (!cur) {
	    if (TRACE)
		fprintf(TDEST, "HTAccess.... NO PROTOCOL MODULES INITIATED\n");
	} else {
	    while ((p = (HTProtocol*)HTList_nextObject(cur))) {
		if (strcmp(p->name, access)==0) {	/* Case insensitive? */
		    HTAnchor_setProtocol(req->anchor, p);
		    free(access);
		    return (HT_OK);
		}
	    }
	}
	free(access);
    }
    return HT_NO_ACCESS;
}

/* --------------------------------------------------------------------------*/
/*				Document Loader 			     */
/* --------------------------------------------------------------------------*/

/*		Load a document
**		---------------
**
**	This is an internal routine, which has an address AND a matching
**	anchor.  (The public routines are called with one OR the other.)
**
** On entry,
**	request->
**	    anchor		a parent anchor with fully qualified
**				hypertext reference as its address set
**	    output_format	valid
**	    output_stream	valid on NULL
**
** On exit,
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PUBLIC int HTLoad ARGS2(HTRequest *, request, BOOL, keep_error_stack)
{
    char	*arg = NULL;
    HTProtocol	*p;
    int 	status;

    if (request->method == METHOD_INVALID)
	request->method = METHOD_GET;
    if (!keep_error_stack) {
	HTErrorFree(request);
	request->error_block = NO;
    }

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
	return HT_ERROR;		       /* Can't resolve or forbidden */
    }

    if(!(arg = HTAnchor_physical(request->anchor)) || !*arg) 
    	return HT_ERROR;

    p = (HTProtocol *) HTAnchor_protocol(request->anchor);
    return (*(p->load))(request);
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
PUBLIC BOOL HTLoadTerminate ARGS2(HTRequest *, request, int, status)
{
    char * uri = HTAnchor_address((HTAnchor*)request->anchor);

    HTLog_request(request);

    /* The error stack might contain general information to the client
       about what has been going on in the library (not only errors) */
    if (!HTImProxy && request->error_stack)
	HTErrorMsg(request);

    switch (status) {
      case HT_LOADED:
	if (PROT_TRACE) {
	    fprintf(TDEST, "HTAccess.... OK: `%s\' has been accessed.\n", uri);
	}
	break;

      case HT_OK:
	if (PROT_TRACE) {
	    fprintf(TDEST,"HTAccess.... SOURCE FINISHED LOADING: `%s\'\n",uri);
	}
	break;

      case HT_NO_DATA:
	if (PROT_TRACE) {
	    fprintf(TDEST, "HTAccess.... OK BUT NO DATA: `%s\'\n", uri);
	}
	break;

      case HT_WOULD_BLOCK:
	if (PROT_TRACE) {
	    fprintf(TDEST, "HTAccess.... WOULD BLOCK: `%s\'\n", uri);
	}
	break;

      case HT_RETRY:
	if (PROT_TRACE) {
	    fprintf(TDEST, "HTAccess.... NOT AVAILABLE, RETRY AT `%s\'\n",uri);
	}
	break;

      case HT_ERROR:
	if (HTImProxy)
	    HTErrorMsg(request);		     /* Only on a real error */
	if (PROT_TRACE) {
	    fprintf(TDEST, "HTAccess.... ERROR: Can't access `%s\'\n", uri);
	}
	break;

      default:
	if (PROT_TRACE) {
	    fprintf(TDEST, "HTAccess.... **** Internal software error in CERN WWWLib version %s ****\n", HTLibraryVersion);
	    fprintf(TDEST, "............ Please mail libwww@w3.org quoting what software\n");
	    fprintf(TDEST, "............ and version you are using including the URL:\n");
	    fprintf(TDEST, "............ `%s\'\n", uri);
	    fprintf(TDEST, "............ that caused the problem, thanks!\n");
	}
	break;
    }
    free(uri);
    return YES;
}


/*		Load a document - with logging etc
**		----------------------------------
**
**	- Checks or documents already loaded
**	- Logs the access
**	- Trace ouput and error messages
**
**    On Entry,
**        request->anchor	valid for of the document to be accessed.
**	 request->childAnchor   optional anchor within doc to be selected
**
**	  request->anchor   is the node_anchor for the document
**	  request->output_format is valid
**
** On exit,
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PRIVATE int HTLoadDocument ARGS2(HTRequest *,	request,
				 BOOL,		keep_error_stack)

{
    int	        status;
    HText *	text;
    char * full_address = HTAnchor_address((HTAnchor*)request->anchor);

    if (PROT_TRACE) fprintf (TDEST, "HTAccess.... Loading document %s\n",
			     full_address);

    request->using_cache = NULL;
    
    if (!request->output_format) request->output_format = WWW_PRESENT;

    /* Check if document is already loaded or in cache */
    if (!request->ForceReload) {
	if ((text=(HText *)HTAnchor_document(request->anchor))) {
	    if (PROT_TRACE)
		fprintf(TDEST, "HTAccess.... Document already in memory.\n");
	    if (request->childAnchor) {
		HText_selectAnchor(text, request->childAnchor);
	    } else {
		HText_select(text);	
	    }
	    free(full_address);
	    return HT_LOADED;
	}
	
	/* Check the Cache */
	/* Bug: for each format, we only check whether it is ok, we
	   don't check them all and chose the best */
	if (request->anchor->cacheItems) {
	    HTList * list = request->anchor->cacheItems;
	    HTList * cur = list;
	    HTCacheItem * item;
	    while ((item = (HTCacheItem*)HTList_nextObject(cur))) {
		HTStream * s;
		request->using_cache = item;
		s = HTStreamStack(item->format, request->output_format,
				  request->output_stream, request, NO);
		if (s) {	/* format was suitable */
		    FILE * fp = fopen(item->filename, "r");
		    if (PROT_TRACE) 
			fprintf(TDEST, "Cache....... HIT file %s for %s\n",
				item->filename, 
				full_address);
		    if (fp) {
			HTFileCopy(fp, s);
			(*s->isa->_free)(s); /* close up pipeline */
			fclose(fp);
			free(full_address);
			return HT_LOADED;
		    } else {
			fprintf(TDEST, "***** Can't read cache file %s !\n",
				item->filename);
		    } /* file open ok */
		} /* stream ok */
	    } /* next cache item */
	} /* if cache available for this anchor */
    } else {			  /* Make sure that we don't use old headers */
	HTAnchor_clearHeader(request->anchor);
	request->RequestMask += HT_PRAGMA;     /* Force reload through proxy */
    }
    if ((status = HTLoad(request, keep_error_stack)) != HT_WOULD_BLOCK)
	HTLoadTerminate(request, status);
    free(full_address);
    return status;
}


/*		Load a document from absolute name
**		---------------
**
** On Entry,
**        addr     The absolute address of the document to be accessed.
**
** On exit,
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PUBLIC int HTLoadAbsolute ARGS2(CONST char *,addr, HTRequest*, request)
{
   HTAnchor * anchor = HTAnchor_findAddress(addr);
   request->anchor = HTAnchor_parent(anchor);
   request->childAnchor = ((HTAnchor*)request->anchor == anchor) ?
   			NULL : (HTChildAnchor*) anchor;
   return HTLoadDocument(request, NO);
}


/*		Load a document from absolute name to stream
**		--------------------------------------------
**
** On Entry,
**        addr     The absolute address of the document to be accessed.
**        request->output_stream     if non-NULL, send data down this stream
**
** On exit,
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PUBLIC int HTLoadToStream ARGS3(CONST char *,	addr,
				BOOL, 		filter,
				HTRequest*,	request)
{
    HTAnchor * anchor = HTAnchor_findAddress(addr);
    request->anchor = HTAnchor_parent(anchor);
    request->childAnchor = ((HTAnchor*)request->anchor == anchor) ? NULL :
   	(HTChildAnchor*) anchor;
    request->output_stream = request->output_stream;
    return HTLoadDocument(request, NO);
}


/*		Load a document from relative name
**		---------------
**
** On Entry,
**        relative_name     The relative address of the document
**	  		    to be accessed.
**
** On exit,
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PUBLIC int HTLoadRelative ARGS3(CONST char *,		relative_name,
				HTParentAnchor *,	here,
				HTRequest *,		request)
{
    char * 		full_address = 0;
    int       		result;
    char * 		mycopy = 0;
    char * 		stripped = 0;
    char *		current_address =
    				HTAnchor_address((HTAnchor*)here);

    StrAllocCopy(mycopy, relative_name);

    stripped = HTStrip(mycopy);
    full_address = HTParse(stripped,
	           current_address,
		   PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
    result = HTLoadAbsolute(full_address, request);
    free(full_address);
    free(current_address);
    free(mycopy);  /* Memory leak fixed 10/7/92 -- JFG */
    return result;
}


/*		Load if necessary, and select an anchor
**		--------------------------------------
**
** On Entry,
**        destination      	    The child or parenet anchor to be loaded.
**
** On exit,
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PUBLIC int HTLoadAnchor ARGS2(HTAnchor*, anchor, HTRequest *, request)
{
    if (!anchor || !request)
	return HT_ERROR;
    request->anchor = HTAnchor_parent(anchor);
    request->childAnchor = ((HTAnchor *) request->anchor == anchor) ?
	NULL : (HTChildAnchor*) anchor;
    return HTLoadDocument(request, NO);
}


/*		Load if necessary, and select an anchor
**		--------------------------------------
**
**	This function is almost identical to HTLoadAnchor, but it doesn't
**	clear the error stack so that the information in there is kept.
**
** On Entry,
**        destination      	    The child or parenet anchor to be loaded.
**
** On exit,
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PUBLIC int HTLoadAnchorRecursive ARGS2(HTAnchor*,	anchor,
				       HTRequest *,	request)
{
    if (!anchor) return HT_ERROR;				  /* No link */
    
    request->anchor  = HTAnchor_parent(anchor);
    request->childAnchor = ((HTAnchor *) request->anchor == anchor) ?
	NULL : (HTChildAnchor*) anchor;
    
    return HTLoadDocument(request, YES);
}


/*		Search
**		------
**  Performs a keyword search on word given by the user. Adds the keyword to 
**  the end of the current address and attempts to open the new address.
**
**  On Entry,
**       *keywords  	space-separated keyword list or similar search list
**	here		is anchor search is to be done on.
**
** On exit,
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PRIVATE char hex ARGS1(int, i)
{
    char * hexchars = "0123456789ABCDEF";
    return hexchars[i];
}

PUBLIC int HTSearch ARGS3(CONST char *,        	keywords,
			  HTParentAnchor *, 	here,
			  HTRequest *,		request)
{

#define acceptable \
"1234567890abcdefghijlkmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-_"

    char *q, *u;
    CONST char * p, *s, *e;		/* Pointers into keywords */
    char * address = HTAnchor_address((HTAnchor*)here);
    int result;
    char * escaped = (char *) malloc(strlen(keywords)*3+1);

    /* static CONST BOOL isAcceptable[96] = */
    /* static AND const is not good for a gnu compiler! Frystyk 25/02-94 */
    static BOOL isAcceptable[96] =
    /*   0 1 2 3 4 5 6 7 8 9 A B C D E F */
    {    0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,	/* 2x   !"#$%&'()*+,-./	 */
         1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,	/* 3x  0123456789:;<=>?	 */
	 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/* 4x  @ABCDEFGHIJKLMNO  */
	 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,	/* 5X  PQRSTUVWXYZ[\]^_	 */
	 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/* 6x  `abcdefghijklmno	 */
	 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0 };	/* 7X  pqrstuvwxyz{\}~	DEL */

    if (escaped == NULL) outofmem(__FILE__, "HTSearch");
    
/* Convert spaces to + and hex escape unacceptable characters */

    for(s=keywords; *s && WHITE(*s); s++); /*scan */ 	 /* Skip white space */
    for(e = s + strlen(s); e>s && WHITE(*(e-1)) ; e--);     /* Skip trailers */
    for(q=escaped, p=s; p<e; p++) {		      /* scan stripped field */
        int c = (int)TOASCII(*p);
        if (WHITE(*p)) {
	    *q++ = '+';
	} else if (c>=32 && c<=127 && isAcceptable[c-32] != 0) {
	    *q++ = *p;			/* 930706 TBL for MVS bug */
	} else {
	    *q++ = '%';
	    *q++ = hex(c / 16);
	    *q++ = hex(c % 16);
	}
    } /* Loop over string */
    
    *q=0;
    				/* terminate escaped sctring */
    u=strchr(address, '?');		/* Find old search string */
    if (u) *u = 0;			        /* Chop old search off */

    StrAllocCat(address, "?");
    StrAllocCat(address, escaped);
    free(escaped);
    result = HTLoadRelative(address, here, request);
    free(address);
    
    return result;
}


/*		Search Given Indexname
**		------
**  Performs a keyword search on word given by the user. Adds the keyword to 
**  the end of the current address and attempts to open the new address.
**
** On Entry,
**       *keywords  	space-separated keyword list or similar search list
**	*addres		is name of object search is to be done on.
** On exit,
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PUBLIC int HTSearchAbsolute ARGS3(CONST char *, 	keywords,
				  CONST char *, 	indexname,
				  HTRequest *,		request)
{
    HTParentAnchor * anchor =
    	(HTParentAnchor*) HTAnchor_findAddress(indexname);
    return HTSearch(keywords, anchor, request);
}

/* --------------------------------------------------------------------------*/
/*				Document Poster 			     */
/* --------------------------------------------------------------------------*/

/*		Get a save stream for a document
**		--------------------------------
*/
PUBLIC HTStream *HTSaveStream ARGS1(HTRequest *, request)
{
    HTProtocol * p;
    int status;
    request->method = METHOD_PUT;
    status = get_physical(request);
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
	return NULL;	/* should return error status? */
    }
    if (status < 0) return NULL; /* @@ error. Can't resolve or forbidden */
    
    p = (HTProtocol *) HTAnchor_protocol(request->anchor);
    if (!p) return NULL;
    
    return (*p->saveStream)(request);
    
}

/*	COPY AN ANCHOR
**	--------------
**  Fetch the URL (possibly local file URL) and send it using either PUT
**  or POST to the remote destination using HTTP. The caller can decide the
**  exact method used and which HTTP header fields to transmit by setting the
**  user fields in the request structure.
**
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PUBLIC int HTCopyAnchor ARGS3(HTAnchor *,	src_anchor,
			      HTParentAnchor *,	dest_anchor,
			      HTRequest *,	dest_req)
{
    HTRequest *src_req;
    if (!(src_anchor && dest_anchor && dest_req))
	return HT_ERROR;

    if (!(dest_anchor->methods & dest_req->method)) {
	char buf[80];
	sprintf(buf, "It might not be allowed to %s to this destination, continue?", HTMethod_name(dest_req->method));
	if (!HTConfirm(buf))
	    return HT_ERROR;
    }

    /* Get an internal HTRequest structure to handle the source part */
    src_req = HTRequest_new();
    HTAnchor_clearHeader((HTParentAnchor *) src_anchor);
    src_req->ForceReload = YES;

    /* Mark the source request so that there only is one HTThreadTerminate()
       function call handling the outcome of the destination request */
    src_req->Source = YES;

    /* Use SOURCE but at some point we can introduce format conversion here! */
    src_req->output_format = WWW_SOURCE;

#ifdef NO_UNIX_IO
    {
	char * addr = HTAnchor_address((HTAnchor *) src_anchor);
	char *access = HTParse(addr, "", PARSE_ACCESS);
	if (*access && !strcmp(access, "file"))
	    dest_req->BlockingIO = YES;
	free(addr);
	free(access);
    }
#endif

    dest_req->GenMask += HT_DATE;			 /* Send date header */
    dest_req->CopyRequest = src_req;

    /* First open the destination then open the source */
    if (HTLoadAnchor((HTAnchor *) dest_anchor, dest_req) != HT_ERROR) {
	int status;
	/*
	** Now make the link between the two request structures. First setup
	** the output stream of the source so that data get redirected to
	** the destination. Then set up the call back function so that
	** the destination can call for more data
	*/
	src_req->output_stream = dest_req->input_stream;
	dest_req->PostCallBack = HTSocketRead;
	if ((status = HTLoadAnchor(src_anchor, src_req)) != HT_LOADED)
	    return status;
    }
    return HT_ERROR;
}


/*	UPLOAD AN ANCHOR
**	----------------
**  Send the contents (in hyperdoc) of the source anchor using either PUT
**  or POST to the remote destination using HTTP. The caller can decide the
**  exact method used and which HTTP header fields to transmit by setting the
**  user fields in the request structure.
**
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PUBLIC int HTUploadAnchor ARGS3(HTAnchor *,		src_anchor,
				HTParentAnchor *,	dest_anchor,
				HTRequest *,		dest_req)
{
    if (!(src_anchor && dest_anchor && dest_req))
	return HT_ERROR;

    if (!(dest_anchor->methods & dest_req->method)) {
	char buf[80];
	sprintf(buf, "It might not be allowed to %s to this destination, continue?", HTMethod_name(dest_req->method));
	if (!HTConfirm(buf))
	    return HT_ERROR;
    }

    /* @@@ NOT FINISHED @@@ */

    return HT_ERROR;
}

/* --------------------------------------------------------------------------*/
/*				Anchor help routines   			     */
/* --------------------------------------------------------------------------*/

/*
**             Find Related Name
**
**  Creates a string that can be used as a related name when 
**  calling HTParse initially. 
**  
**  The code for this routine originates from the Linemode 
**  browser and was moved here by howcome@w3.org
**  in order for all clients to take advantage.
**
**  The string returned must be freed by the caller
*/
PUBLIC char * HTFindRelatedName NOARGS
{
    char* default_default = NULL;	      /* Parse home relative to this */
    CONST char *host = HTGetHostName(); 
    StrAllocCopy(default_default, "file://");
    if (host)
	StrAllocCat(default_default, host);
    else
	StrAllocCat(default_default, "localhost");
    {
	char wd[HT_MAX_PATH+1];

#ifdef NO_GETWD
#ifdef HAS_GETCWD	      /* System V variant SIGN CHANGED TBL 921006 !! */
	char *result = (char *) getcwd(wd, sizeof(wd)); 
#else
	char *result = NULL;
	HTAlert("This platform does not support neither getwd nor getcwd\n");
#endif
#else
	char *result = (char *) getwd(wd);
#endif
	*(wd+HT_MAX_PATH) = '\0';
	if (result) {
#ifdef VMS 
            /* convert directory name to Unix-style syntax */
	    char * disk = strchr (wd, ':');
	    char * dir = strchr (wd, '[');
	    if (disk) {
	        *disk = '\0';
		StrAllocCat (default_default, "/");  /* needs delimiter */
		StrAllocCat (default_default, wd);
	    }
	    if (dir) {
		char *p;
		*dir = '/';  /* Convert leading '[' */
		for (p = dir ; *p != ']'; ++p)
			if (*p == '.') *p = '/';
		*p = '\0';  /* Cut on final ']' */
		StrAllocCat (default_default, dir);
	    }
#else  /* not VMS */
#ifdef WIN32
	    char * p = wd ;	/* a colon */
	    StrAllocCat(default_default, "/");
	    while( *p != 0 ) { 
		if (*p == '\\')		         /* change to one true slash */
		    *p = '/' ;
		p++;
	    }
	    StrAllocCat( default_default, wd);
#else /* not WIN32 */
	    StrAllocCat (default_default, wd);
#endif /* not WIN32 */
#endif /* not VMS */
	}
    }
    StrAllocCat(default_default, "/default.html");
    return default_default;
}


/*		Generate the anchor for the home page
**		-------------------------------------
**
**	As it involves file access, this should only be done once
**	when the program first runs.
**	This is a default algorithm -- browser don't HAVE to use this.
**	But consistency betwen browsers is STRONGLY recommended!
**
**	Priority order is:
**
**		1	WWW_HOME environment variable (logical name, etc)
**		2	~/WWW/default.html
**		3	/usr/local/bin/default.html
**		4	http://www.w3.org/default.html
**
*/
PUBLIC HTParentAnchor * HTHomeAnchor NOARGS
{
    char * my_home_document = NULL;
    char * home = (char *) getenv(LOGICAL_DEFAULT);
    char * ref;
    HTParentAnchor * anchor;
    
    /* Someone telnets in, they get a special home */
    if (home) {
        StrAllocCopy(my_home_document, home);
    } else  if (HTClientHost) {				    /* Telnet server */
    	FILE * fp = fopen(REMOTE_POINTER, "r");
	char * status;
	if (fp) {
	    my_home_document = (char*) malloc(HT_MAX_PATH);
	    status = fgets(my_home_document, HT_MAX_PATH, fp);
	    if (!status) {
	        free(my_home_document);
		my_home_document = NULL;
	    }
	    fclose(fp);
	}
	if (!my_home_document) StrAllocCopy(my_home_document, REMOTE_ADDRESS);
    }

#ifdef unix
    if (!my_home_document) {
	FILE * fp = NULL;
	char * home = (char *) getenv("HOME");
	if (home) { 
	    my_home_document = (char *)malloc(
		strlen(home)+1+ strlen(PERSONAL_DEFAULT)+1);
	    if (my_home_document == NULL) outofmem(__FILE__, "HTLocalName");
	    sprintf(my_home_document, "%s/%s", home, PERSONAL_DEFAULT);
	    fp = fopen(my_home_document, "r");
	}
	
	if (!fp) {
	    StrAllocCopy(my_home_document, LOCAL_DEFAULT_FILE);
	    fp = fopen(my_home_document, "r");
	}
	if (fp) {
	    fclose(fp);
	} else {
	    if (TRACE)
		fprintf(TDEST,
			"HTBrowse: No local home document ~/%s or %s\n",
			PERSONAL_DEFAULT, LOCAL_DEFAULT_FILE);
	    free(my_home_document);
	    my_home_document = NULL;
	}
    }
#endif
    ref = HTParse(my_home_document ? my_home_document :
		  HTClientHost ? REMOTE_ADDRESS : LAST_RESORT, "file:",
		  PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
    if (my_home_document) {
	if (TRACE)
	    fprintf(TDEST,
		   "HTAccess.... `%s\' used for custom home page as\n`%s\'\n",
		    my_home_document, ref);
	free(my_home_document);
    }
    anchor = (HTParentAnchor*) HTAnchor_findAddress(ref);
    free(ref);
    return anchor;
}


/*		Bind an Anchor to the request structure
**		---------------------------------------
**
**    On Entry,
**	anchor		The child or parenet anchor to be binded
**	request		The request sturcture
**    On Exit,
**        returns    YES     Success
**                   NO      Failure 
**
**  Note: Actually the same as HTLoadAnchor() but DOES NOT do the loading
**						Henrik Frystyk 17/02-94
*/

PUBLIC BOOL HTBindAnchor ARGS2(HTAnchor*, anchor, HTRequest *, request)
{
    if (!anchor) return NO;	/* No link */
    
    request->anchor  = HTAnchor_parent(anchor);
    request->childAnchor = ((HTAnchor*)request->anchor == anchor) ? NULL
    					: (HTChildAnchor*) anchor;
	
    return YES;
}


