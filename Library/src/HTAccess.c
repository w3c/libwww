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
**	 8 Jul 94 Insulate HT_FREE();
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

#define PUTBLOCK(b, l)	(*target->isa->put_block)(target, b, l)

struct _HTStream {
    HTStreamClass * isa;
};

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
#ifdef WWW_WIN_ASYNC
    /*
    **	We are here starting a hidden window to take care of events from
    **  the async select() call in the async version of the event loop in
    **	the Internal event manager (HTEvntrg.c)
    */
    HWND htSocketWin;
    static char className[] = "AsyncWindowClass";
    WNDCLASS wc;
    OSVERSIONINFO osInfo;
    
    wc.style=0;
    wc.lpfnWndProc=(WNDPROC)AsyncWindowProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hIcon=0;
    wc.hCursor=0;
    wc.hbrBackground=0;
    wc.lpszMenuName=(LPSTR)0;
    wc.lpszClassName=className;

    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    GetVersionEx(&osInfo);
    if (osInfo.dwPlatformId == VER_PLATFORM_WIN32s || osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	wc.hInstance=GetModuleHandle(NULL); /* 95 and non threaded platforms */
    else
	wc.hInstance=GetCurrentProcess(); /* NT and hopefully everything following */
#endif /* WWW_WIN_ASYNC */

#if WWWTRACE_MODE == WWWTRACE_FILE			  /* Open trace file */
    if ((TDEST = fopen(HT_TRACE_FILE, "a")) != NULL) {
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

#ifdef WWWLIB_SIG
    /* On Solaris (and others?) we get a BROKEN PIPE signal when connecting
    ** to a port where we should get `connection refused'. We ignore this 
    ** using the following function call
    */
    HTSetSignal();				   /* Set signals in library */
#endif

#ifdef WWW_WIN_ASYNC
    if (!RegisterClass(&wc)) {
    	TTYPrint(TDEST, "HTEvent_Loop.. Can't RegisterClass \"%s\"\n", className);
	    return NO;
    }
    if (!(htSocketWin = CreateWindow(className, "WWW_WIN_ASYNC", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 
                                     CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wc.hInstance,0))) {
	char space[50];
       	TTYPrint(TDEST, "HTEvent_Loop.. Can't CreateWindow \"WWW_WIN_ASYNC\" - error:");
	sprintf(space, "%ld\n", GetLastError());
	TTYPrint(TDEST, space);
    	return NO;
    }
    HTEvent_setWinHandle (htSocketWin, WM_USER);     /* use first available message since app uses none */
#endif /* WWW_WIN_ASYNC */

#ifdef _WINSOCKAPI_
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
#endif /* _WINSOCKAPI_ */

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
PUBLIC BOOL HTLibTerminate (void)
{
    if (WWWTRACE)
	TTYPrint(TDEST, "WWWLibTerm.. Cleaning up LIBRARY OF COMMON CODE\n");
    HTAtom_deleteAll();					 /* Remove the atoms */
    HTDNS_deleteAll();				/* Remove the DNS host cache */
    HTAnchor_deleteAll(NULL);		/* Delete anchors and drop hyperdocs */

    HTProtocol_deleteAll();  /* Remove bindings between access and protocols */
    HTBind_deleteAll();	    /* Remove bindings between suffixes, media types */

    HTFreeHostName();			    /* Free up some internal strings */
    HTFreeMailAddress();
    HTTmp_freeRoot();

#ifdef _WINSOCKAPI_
    WSACleanup();
#endif

#ifdef WWW_WIN_ASYNC
    DestroyWindow(HTEvent_getWinHandle(0));
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
/*	           		Load Access functions			     */
/* --------------------------------------------------------------------------*/

/*	Request a document
**	-----------------
**	Private version that requests a document from the request manager
**	Returns YES if request accepted, else NO
*/
PRIVATE BOOL HTLoadDocument (HTRequest * request, BOOL recursive)
{
    if (PROT_TRACE) {
	HTParentAnchor *anchor = HTRequest_anchor(request);
	char * full_address = HTAnchor_address((HTAnchor *) anchor);
	TTYPrint(TDEST, "HTAccess.... Accessing document %s\n", full_address);
	HT_FREE(full_address);
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
	HT_FREE(rel);
	HT_FREE(full_url);
	HT_FREE(base_url);
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
	HT_FREE(base_url);
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
/*				Post Access Functions 			     */
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
PUBLIC BOOL HTCopyAnchor (HTAnchor * src_anchor, HTRequest * main_dest)
{ 
    HTRequest * src_req;
    HTList * cur;
    if (!src_anchor || !main_dest) {
	if (WWWTRACE) TTYPrint(TDEST, "Copy........ BAD ARGUMENT\n");
	return NO;
    }

    /* Set the source anchor */
    main_dest->source_anchor = HTAnchor_parent(src_anchor);

    /* Build the POST web if not already there */
    if (!main_dest->source) {
	src_req = HTRequest_dupInternal(main_dest);	  /* Get a duplicate */
	HTAnchor_clearHeader((HTParentAnchor *) src_anchor);
	src_req->method = METHOD_GET;
	src_req->reload = HT_MEM_REFRESH;
	src_req->output_stream = NULL;
	src_req->output_format = WWW_SOURCE;	 /* We want source (for now) */

	/* Set up the main link in the source anchor */
	{
	    HTLink * main_link = HTAnchor_mainLink((HTAnchor *) src_anchor);
	    HTAnchor *main_anchor = HTLink_destination(main_link);
	    HTMethod method = HTLink_method(main_link);
	    if (!main_link || method==METHOD_INVALID) {
		if (WWWTRACE)
		    TTYPrint(TDEST, "Copy Anchor. No destination found or unspecified method\n");
		HTRequest_delete(src_req);
		return NO;
	    }
	    main_dest->GenMask |= HT_G_DATE;		 /* Send date header */
	    main_dest->reload = HT_CACHE_REFRESH;
	    main_dest->method = method;
	    main_dest->input_format = WWW_SOURCE;
	    HTRequest_addDestination(src_req, main_dest);
	    if (HTLoadAnchor(main_anchor, main_dest) == NO)
		return NO;
	}

	/* For all other links in the source anchor */
	if ((cur = HTAnchor_subLinks(src_anchor))) {
	    HTLink * pres;
	    while ((pres = (HTLink *) HTList_nextObject(cur))) {
		HTAnchor *dest = HTLink_destination(pres);
		HTMethod method = HTLink_method(pres);
		HTRequest *dest_req;
		if (!dest || method==METHOD_INVALID) {
		    if (WWWTRACE)
			TTYPrint(TDEST, "Copy Anchor. Bad anchor setup %p\n",
				dest);
		    return NO;
		}
		dest_req = HTRequest_dupInternal(main_dest);
		dest_req->GenMask |= HT_G_DATE;		 /* Send date header */
		dest_req->reload = HT_CACHE_REFRESH;
		dest_req->method = method;
		dest_req->output_stream = NULL;
		dest_req->output_format = WWW_SOURCE;
		HTRequest_addDestination(src_req, dest_req);

		if (HTLoadAnchor(dest, dest_req) == NO)
		    return NO;
	    }
	}
    } else {			 /* Use the existing Post Web and restart it */
	src_req = main_dest->source;
	if (src_req->mainDestination)
	    if (HTLoadDocument(main_dest, NO) == NO)
		return NO;
	if (src_req->destinations) {
	    HTRequest * pres;
	    cur = HTAnchor_subLinks(src_anchor);
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
**	This function can be used to send data along with a request to a remote
**	server. It can for example be used to POST form data to a remote HTTP
**	server - or it can be used to post a newsletter to a NNTP server. In
**	either case, you pass a callback function which the request calls when
**	the remote destination is ready to accept data. In this callback
**	you get the current request object and a stream into where you can 
**	write data. It is very important that you return the value returned
**	by this stream to the Library so that it knows what to do next. The
**	reason is that the outgoing stream might block or an error may
**	occur and in that case the Library must know about it. The source
**	anchor represents the data object in memory and it points to 
**	the destination anchor by using the POSTWeb method. The source anchor
**	contains metainformation about the data object in memory and the 
**	destination anchor represents the reponse from the remote server.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTUploadAnchor (HTAnchor *		source_anchor,
			    HTRequest * 	request,
			    HTPostCallback *	callback)
{
    HTLink * link = HTAnchor_mainLink((HTAnchor *) source_anchor);
    HTAnchor * dest_anchor = HTLink_destination(link);
    HTMethod method = HTLink_method(link);
    if (!link || method==METHOD_INVALID || !callback) {
	if (WWWTRACE)
	    TTYPrint(TDEST, "Upload...... No destination found or unspecified method\n");
	return NO;
    }
    request->GenMask |= HT_G_DATE;			 /* Send date header */
    request->reload = HT_CACHE_REFRESH;
    request->method = method;
    request->source_anchor = HTAnchor_parent(source_anchor);
    request->PostCallback = callback;
    return HTLoadAnchor(dest_anchor, request);
}

/*	POST Callback Handler
**	---------------------
**	If you do not want to handle the stream interface on your own, you
**	can use this function which writes the source anchor hyperdoc to the
**	target stream for the anchor upload and also handles the return value
**	from the stream. If you don't want to write the source anchor hyperdoc
**	then you can register your own callback function that can get the data
**	you want.
*/
PUBLIC int HTUpload_callback (HTRequest * request, HTStream * target)
{
    if (WWWTRACE) TTYPrint(TDEST, "Uploading... from callback function\n");
    if (!request || !request->source_anchor || !target) return HT_ERROR;
    {
	int status;
	HTParentAnchor * source = request->source_anchor;
	char * document = (char *) HTAnchor_document(request->source_anchor);
	int len = HTAnchor_length(source);
	if (len < 0) {
	    len = strlen(document);
	    HTAnchor_setLength(source, len);
	}
	status = (*target->isa->put_block)(target, document, len);
	if (status == HT_OK)
	    return (*target->isa->flush)(target);
	if (status == HT_WOULD_BLOCK) {
	    if (PROT_TRACE)TTYPrint(TDEST,"POST Anchor. Target WOULD BLOCK\n");
	    return HT_WOULD_BLOCK;
	} else if (status == HT_PAUSE) {
	    if (PROT_TRACE) TTYPrint(TDEST,"POST Anchor. Target PAUSED\n");
	    return HT_PAUSE;
	} else if (status > 0) {	      /* Stream specific return code */
	    if (PROT_TRACE)
		TTYPrint(TDEST, "POST Anchor. Target returns %d\n", status);
	    return status;
	} else {				     /* We have a real error */
	    if (PROT_TRACE) TTYPrint(TDEST, "POST Anchor. Target ERROR\n");
	    return status;
	}
    }
}
