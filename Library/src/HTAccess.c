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

PRIVATE char * HTAppName = NULL;	  /* Application name: please supply */
PRIVATE char * HTAppVersion = NULL;    /* Application version: please supply */

/* --------------------------------------------------------------------------*/
/*	           Initialization and Termination of the Library	     */
/* --------------------------------------------------------------------------*/

/*	HTLib_appName
**	-------------
*/
PUBLIC CONST char * HTLib_appName (void)
{
    return HTAppName;
}

/*	HTLib_appVersion
**	----------------
*/
PUBLIC CONST char * HTLib_appVersion (void)
{
    return HTAppVersion;
}

/*								     HTLibInit
**
**	This function initiates the Library and it MUST be called when
**	starting up an application. See also HTLibTerminate()
*/
PUBLIC BOOL HTLibInit (CONST char * AppName, CONST char * AppVersion)
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

    if (WWWTRACE)
	fprintf(TDEST, "WWWLibInit.. INITIALIZING LIBRARY OF COMMON CODE\n");

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
    HTProxy_setGateway("wais", HT_DEFAULT_WAIS_GATEWAY);
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
		fprintf(TDEST, "WWWLibInit.. Can't initialize WinSoc\n");
            WSACleanup();
            return NO;
        }
        if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION) {
            if (WWWTRACE)
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
	fprintf(TDEST, "WWWLibTerm.. Cleaning up LIBRARY OF COMMON CODE\n");
    HTAtom_deleteAll();
    HTDisposeConversions();
    HTDNS_deleteAll();

    HTProtocol_deleteAll();  /* Remove bindings between access and protocols */
    HTBind_deleteAll();	    /* Remove bindings between suffixes, media types */

    HTProxy_deleteProxy();	   /* Clean up lists of proxies and gateways */
    HTProxy_deleteNoProxy();
    HTProxy_deleteGateway();

    HTFreeHostName();			    /* Free up some internal strings */
    HTFreeMailAddress();
    HTCache_freeRoot();
    HTCache_clearMem();				  /* Keep the disk versions! */
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
	fprintf (TDEST, "HTAccess.... Accessing document %s\n", full_address);
	free(full_address);
    }
    return HTLoad(request, recursive, 0);	       /* @@@@ PRIORITY @@@@ */
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
	return HTLoadAbsolute(base_url, request);
    }
    return NO;
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
		    fprintf(TDEST, "Copy Anchor. No destination found or unspecified method");
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
			fprintf(TDEST, "Copy Anchor. Bad anchor setup %p\n",
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
	    if (WWWTRACE)
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
	if (WWWTRACE)
	    fprintf(TDEST,
		   "HTAccess.... `%s\' used for custom home page as\n`%s\'\n",
		    my_home_document, ref);
	free(my_home_document);
    }
    anchor = (HTParentAnchor*) HTAnchor_findAddress(ref);
    free(ref);
    return anchor;
}

