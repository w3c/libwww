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
**	   Sep 95 Rewritten, HFN
*/

#if !defined(HT_DIRECT_WAIS) && !defined(HT_DEFAULT_WAIS_GATEWAY)
#define HT_DEFAULT_WAIS_GATEWAY "http://www.w3.org:8001/"
#endif

/* Library include files */
#include "WWWLib.h"
#include "HTReqMan.h"
#include "HTAccess.h"					 /* Implemented here */

#ifndef VC
#define VC "unknown"
#endif

PRIVATE char * HTAppName = NULL;	  /* Application name: please supply */
PRIVATE char * HTAppVersion = NULL;    /* Application version: please supply */

PRIVATE char * HTLibName = "libwww";
PRIVATE char * HTLibVersion = VC;

PRIVATE BOOL   HTSecure = NO;		 /* Can we access local file system? */

/* --------------------------------------------------------------------------*/
/*	           Initialization and Termination of the Library	     */
/* --------------------------------------------------------------------------*/

/*	Information about the Application
**	---------------------------------
*/
PUBLIC CONST char * HTLib_appName (void)
{
    return HTAppName ? HTAppName : "UNKNOWN";
}

PUBLIC CONST char * HTLib_appVersion (void)
{
    return HTAppVersion ? HTAppVersion : "0.0";
}

/*	Information about libwww
**	------------------------
*/
PUBLIC CONST char * HTLib_name (void)
{
    return HTLibName ? HTLibName : "UNKNOWN";
}

PUBLIC CONST char * HTLib_version (void)
{
    return HTLibVersion ? HTLibVersion : "0.0";
}

/*	Access Local File System
**	------------------------
**	In this mode we do not tough the local file system at all
*/
PUBLIC BOOL HTLib_secure (void)
{
    return HTSecure;
}

PUBLIC void HTLib_setSecure (BOOL mode)
{
    HTSecure = mode;
}

/*								     HTLibInit
**
**	This function initiates the Library and it MUST be called when
**	starting up an application. See also HTLibTerminate()
*/
PUBLIC BOOL HTLibInit (CONST char * AppName, CONST char * AppVersion)
{
#if WWWTRACE_MODE == WWWTRACE_FILE			  /* Open trace file */
    if ((TDEST = fopen(TRACE_FILE, "a")) != NULL) {
	if (setvbuf(TDEST, NULL, _IOLBF, 0) < 0) {  /* Change to line buffer */
	    TTYPrint(TDEST, "WWWLibInit.. Can't initialize TRACE buffer - no TRACE\n");
	    fclose(TDEST);
	    TDEST = NULL;
	    WWW_TraceFlag = 0;
	}
    } else
	WWW_TraceFlag = 0;
#endif /* WWWTRACE_FILE */

    if (WWWTRACE)
	TTYPrint(TDEST, "WWWLibInit.. INITIALIZING LIBRARY OF COMMON CODE\n");

    /* Set the application name and version */
    if (AppName) {
	char *ptr;
	StrAllocCopy(HTAppName, AppName);
	ptr = HTAppName;
	while (*ptr) {
	    if (WHITE(*ptr)) *ptr = '_';
	    ptr++;
	}
    }
    if (AppVersion) {
	char *ptr;
	StrAllocCopy(HTAppVersion, AppVersion);
	ptr = HTAppVersion;
	while (*ptr) {
	    if (WHITE(*ptr)) *ptr = '_';
	    ptr++;
	}
    }

    HTBind_init();				      /* Initialize bindings */

#ifndef HT_DIRECT_WAIS
    HTGateway_add("wais", HT_DEFAULT_WAIS_GATEWAY);
#endif

    /* Register a call back function for the Net Manager */
    HTNet_register (HTLoad_terminate, HT_ALL);

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
	    if (WWWTRACE)
		TTYPrint(TDEST, "WWWLibInit.. Can't initialize WinSoc\n");
            WSACleanup();
            return NO;
        }
        if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION) {
            if (WWWTRACE)
		TTYPrint(TDEST, "WWWLibInit.. Bad version of WinSoc\n");
            WSACleanup();
            return NO;
        }
    }
#endif /* _WINDOWS */

#ifndef NO_TIMEGM
    HTGetTimeZoneOffset();	   /* Find offset from GMT if using mktime() */
#endif
    HTTmp_setRoot(NULL);		     /* Set up default tmp directory */
    return YES;
}


/*	HTLibTerminate
**	--------------
**	This function frees memory kept by the Library and should be called
**	before exit of an application (if you are on a PC platform)
*/
PUBLIC BOOL HTLibTerminate NOARGS
{
    if (WWWTRACE)
	TTYPrint(TDEST, "WWWLibTerm.. Cleaning up LIBRARY OF COMMON CODE\n");
    HTAtom_deleteAll();
    HTDNS_deleteAll();

    HTProtocol_deleteAll();  /* Remove bindings between access and protocols */
    HTBind_deleteAll();	    /* Remove bindings between suffixes, media types */

    HTProxy_deleteAll();	   /* Clean up lists of proxies and gateways */
    HTNoProxy_deleteAll();
    HTGateway_deleteAll();

    HTFreeHostName();			    /* Free up some internal strings */
    HTFreeMailAddress();
    HTCache_freeRoot();
    HTCache_clearMem();				  /* Keep the disk versions! */
    HTTmp_freeRoot();
    HTError_freePrefix();

#ifdef _WINDOWS
    WSACleanup();
#endif

#if WWWTRACE_MODE == WWWTRACE_FILE			 /* Close trace file */
    if (TDEST) {
	fclose(TDEST);
	TDEST = NULL;
	WWW_TraceFlag = 0;
    }
#endif /* WWWTRACE_FILE */
    return YES;
}

/* --------------------------------------------------------------------------*/
/*	           		Access functions			     */
/* --------------------------------------------------------------------------*/

/*	Request a document
**	-----------------
**	Private version that requests a document from the request manager
**	Returns YES if request accepted, else NO
*/
PRIVATE BOOL HTLoadDocument ARGS2(HTRequest *, request, BOOL, recursive)
{
    if (PROT_TRACE) {
	HTParentAnchor *anchor = HTRequest_anchor(request);
	char * full_address = HTAnchor_address((HTAnchor *) anchor);
	TTYPrint(TDEST, "HTAccess.... Accessing document %s\n", full_address);
	free(full_address);
    }
    return HTLoad(request, recursive);
}


/*	Request a document from absolute name
**	-------------------------------------
**	Request a document referencd by an absolute URL.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadAbsolute (CONST char * url, HTRequest* request)
{
    if (url && request) {
	HTAnchor * anchor = HTAnchor_findAddress(url);
	HTRequest_setAnchor(request, anchor);
	return HTLoadDocument(request, NO);
    }
    return NO;
}


/*	Request a document from absolute name to stream
**	-----------------------------------------------
**	Request a document referencd by an absolute URL and sending the data
**	down a stream. This is _excactly_ the same as HTLoadAbsolute as
**	the ourputstream is specified using the function
**	HTRequest_setOutputStream(). 'filter' is ignored!
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadToStream (CONST char * url, BOOL filter, HTRequest *request)
{
    return HTLoadAbsolute(url, request);
}


/*	Request a document from relative name
**	-------------------------------------
**	Request a document referenced by a relative URL. The relative URL is 
**	made absolute by resolving it relative to the address of the 'base' 
**	anchor.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadRelative (CONST char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request)
{
    BOOL status = NO;
    if (relative && base && request) {
	char * rel = NULL;
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) base);
	StrAllocCopy(rel, relative);
	full_url = HTParse(HTStrip(rel), base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTLoadAbsolute(full_url, request);
	free(rel);
	free(full_url);
	free(base_url);
    }
    return status;
}


/*	Request an anchor
**	-----------------
**	Request the document referenced by the anchor
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadAnchor (HTAnchor * anchor, HTRequest * request)
{
    if (anchor && request) {
	HTRequest_setAnchor(request, anchor);
	return HTLoadDocument(request, NO);
    }
    return NO;
}


/*	Request an anchor
**	-----------------
**	Same as HTLoadAnchor but any information in the Error Stack in the 
**	request object is kept, so that any error messages in one 
**	This function is almost identical to HTLoadAnchor, but it doesn't
**	clear the error stack so that the information in there is kept.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadAnchorRecursive (HTAnchor * anchor, HTRequest * request)
{
    if (anchor && request) {
	HTRequest_setAnchor(request, anchor);
        return HTLoadDocument(request, YES);
    }
    return NO;
}


/*	Search an Anchor
**	----------------
**	Performs a keyword search on word given by the user. Adds the keyword
**	to the end of the current address and attempts to open the new address.
**	The list of keywords must be a space-separated list and spaces will
**	be converted to '+' before the request is issued.
**	Search can also be performed by HTLoadAbsolute() etc.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTSearch (CONST char *	keywords,
		      HTParentAnchor *  base,
		      HTRequest * 	request)
{
    BOOL status = NO;
    if (keywords && base && request) {
	char *base_url = HTAnchor_address((HTAnchor *) base);
	if (*keywords) {
	    char *plus;
	    StrAllocCat(base_url, "?");
	    StrAllocCat(base_url, keywords);
	    plus = strchr(base_url, '?');
	    while (*plus) {
		if (*plus == ' ') *plus = '+';
		plus++;
	    }
	}
	status = HTLoadAbsolute(base_url, request);
	free(base_url);
    }
    return status;
}


/*	Search a document from absolute name
**	------------------------------------
**	Request a document referencd by an absolute URL appended with the
**	keywords given. The URL can NOT contain any fragment identifier!
**	The list of keywords must be a space-separated list and spaces will
**	be converted to '+' before the request is issued.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTSearchAbsolute (CONST char *	keywords,
			      CONST char *	url,
			      HTRequest *	request)
{
    if (url && request) {
	HTAnchor * anchor = HTAnchor_findAddress(url);
	return HTSearch(keywords, HTAnchor_parent(anchor), request);
    }
    return NO;
}

/* --------------------------------------------------------------------------*/
/*				Document Poster 			     */
/* --------------------------------------------------------------------------*/

/*	Copy an anchor
**	--------------
**	Fetch the URL (possibly local file URL) and send it using either PUT
**	or POST to the remote destination using HTTP. The caller can decide the
**	exact method used and which HTTP header fields to transmit by setting
**	the user fields in the request structure.
**	If posting to NNTP then we can't dispatch at this level but must pass
**	the source anchor to the news module that then takes all the refs
**	to NNTP and puts into the "newsgroups" header
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTCopyAnchor (HTAnchor * src_anchor, HTRequest * main_req)
{ 
    HTRequest *src_req;
    if (!src_anchor || !main_req)
	return NO;

    /* Build the POST web if not already there */
    if (!main_req->source) {
	src_req = HTRequest_new();		  /* First set up the source */
	HTAnchor_clearHeader((HTParentAnchor *) src_anchor);
	src_req->reload = HT_MEM_REFRESH;
	src_req->source = src_req;			  /* Point to myself */
	src_req->output_format = WWW_SOURCE;	 /* We want source (for now) */

	/* Set up the main link in the source anchor */
	{
	    HTLink *main_link = HTAnchor_findMainLink(src_anchor);
	    HTAnchor *main_anchor = HTAnchor_linkDest(main_link);
	    HTMethod method = HTAnchor_linkMethod(main_link);
	    if (!main_link || method==METHOD_INVALID) {
		if (WWWTRACE)
		    TTYPrint(TDEST, "Copy Anchor. No destination found or unspecified method");
		HTRequest_delete(src_req);
		return NO;
	    }
	    if (HTAnchor_linkResult(main_link) == HT_LINK_NONE) {
		main_req->GenMask |= HT_DATE;		 /* Send date header */
		main_req->source = src_req;
		main_req->reload = HT_CACHE_REFRESH;
		main_req->method = method;
		HTRequest_addDestination(src_req, main_req);
		main_req->input_format = WWW_SOURCE;	  /* for now :-( @@@ */
		if (HTLoadAnchor(main_anchor, main_req) == NO)
		    return NO;
	    }
	}

	/* For all other links in the source anchor */
	if (src_anchor->links) {
	    HTList *cur = src_anchor->links;
	    HTLink *pres;
	    while ((pres = (HTLink *) HTList_nextObject(cur)) &&
		   HTAnchor_linkResult(pres) == HT_LINK_NONE) {
		HTAnchor *dest = HTAnchor_linkDest(pres);
		HTMethod method = HTAnchor_linkMethod(pres);
		HTRequest *dest_req;
		if (!dest || method==METHOD_INVALID) {
		    if (WWWTRACE)
			TTYPrint(TDEST, "Copy Anchor. Bad anchor setup %p\n",
				dest);
		    return NO;
		}
		dest_req = HTRequest_new();
		dest_req->GenMask |= HT_DATE;		 /* Send date header */
		dest_req->source = src_req;
		dest_req->reload = HT_CACHE_REFRESH;
		dest_req->method = method;
		HTRequest_addDestination(src_req, dest_req);
		dest_req->input_format = WWW_SOURCE;	  /* for now :-( @@@ */
		if (HTLoadAnchor(dest, dest_req) == NO)
		    return NO;
	    }
	}
    } else {			 /* Use the existing Post Web and restart it */
	src_req = main_req->source;
	if (src_req->mainDestination)
	    if (HTLoadDocument(main_req, NO) == NO)
		return NO;
	if (src_req->destinations) {
	    HTList *cur = src_anchor->links;
	    HTRequest *pres;
	    while ((pres = (HTRequest *) HTList_nextObject(cur)) != NULL) {
		if (HTLoadDocument(pres, NO) == NO)
		    return NO;
	    }
	}
    }

    /* Now open the source */
    return HTLoadAnchor(src_anchor, src_req);
}


/*	Upload an Anchor
**	----------------
**	Send the contents (in hyperdoc) of the source anchor using either PUT
**	or POST to the remote destination using HTTP. The caller can decide the
**	exact method used and which HTTP header fields to transmit by setting
**	the user fields in the request structure.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTUploadAnchor (HTAnchor *		src_anchor,
			    HTParentAnchor *	dest_anchor,
			    HTRequest *		dest_req)
{
    if (!src_anchor || !dest_anchor || !dest_req)
	return NO;
    if (!(dest_anchor->methods & dest_req->method)) {
	char buf[80];
	sprintf(buf, "It might not be allowed to %s to this destination, continue?", HTMethod_name(dest_req->method));
	if (!HTConfirm(dest_req, buf))
	    return NO;
    }

    /* @@@ NOT FINISHED @@@ */

    return NO;
}

