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
#include "HTReqMan.h"	     /* @@@@@@@@@@@@@@@@@@@@@@@@ */

#include "HTInit.h"
#include "HTAccess.h"					 /* Implemented here */

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
#if 0
    HTNet_deleteAll();			    /* Remove all remaining requests */
#endif
    HTAtom_deleteAll();
    HTDisposeConversions();
    HTTCPCacheRemoveAll();

#ifndef HT_NO_INIT
    HTProtocol_deleteAll();  /* Remove bindings between access and protocols */
    HTBind_deleteAll();	    /* Remove bindings between suffixes, media types */
#endif

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

/*	Load a document
**	---------------
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
**	returns
**		YES	if request has been registered (success)
**		NO	an error occured
*/
PRIVATE int HTLoadDocument ARGS2(HTRequest *, request, BOOL, recursive)
{
    if (PROT_TRACE) {
	char * full_address = HTAnchor_address((HTAnchor *) request->anchor);
	fprintf (TDEST, "HTAccess.... Accessing document %s\n", full_address);
	free(full_address);
    }
    if (!request->output_format)
	request->output_format = WWW_PRESENT;
    return HTLoad(request, recursive, 0);	       /* @@@@ PRIORITY @@@@ */
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

#if 0
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
		       (void *) url, (int) strlen(url), "HTSaveStream");
	    free(url);
	} else {
	    HTErrorAdd(request, ERR_FATAL, NO, HTERR_FORBIDDEN,
		       NULL, 0, "HTSaveStream");
	}
	return NULL;	/* should return error status? */
    }
    if (status < 0) return NULL; /* @@ error. Can't resolve or forbidden */
    
    p = (HTProtocol *) HTAnchor_protocol(request->anchor);
    if (!p) return NULL;
    
    return (*p->saveStream)(request);
    
}
#endif

/*	COPY AN ANCHOR
**	--------------
**  Fetch the URL (possibly local file URL) and send it using either PUT
**  or POST to the remote destination using HTTP. The caller can decide the
**  exact method used and which HTTP header fields to transmit by setting the
**  user fields in the request structure.
**
**  BUGS: Should take ALL links in the destination anchor and PUT/POST to
**  all of them!
**
**	returns		HT_WOULD_BLOCK	An I/O operation would block
**			HT_ERROR	Error has occured
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**			HT_RETRY	if service isn't available before
**					request->retry_after
*/
PUBLIC int HTCopyAnchor ARGS2(HTAnchor *,	src_anchor,
			      HTRequest *,	main_req)
{ 
    HTRequest *src_req;
    if (!src_anchor || !main_req)
	return HT_ERROR;

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
		if (TRACE)
		    fprintf(TDEST, "Copy Anchor. No destination found or unspecified method");
		HTRequest_delete(src_req);
		return HT_ERROR;
	    }
	    if (HTAnchor_linkResult(main_link) == HT_LINK_NONE) {
		main_req->GenMask |= HT_DATE;		 /* Send date header */
		main_req->source = src_req;
		main_req->reload = HT_CACHE_REFRESH;
		main_req->method = method;
		HTRequest_addDestination(src_req, main_req);
		main_req->input_format = WWW_SOURCE;	  /* for now :-( @@@ */
		if (HTLoadAnchor(main_anchor, main_req) == HT_ERROR)
		    return HT_ERROR;
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
		    if (TRACE)
			fprintf(TDEST, "Copy Anchor. Bad anchor setup %p\n",
				dest);
		    return HT_ERROR;
		}
		dest_req = HTRequest_new();
		dest_req->GenMask |= HT_DATE;		 /* Send date header */
		dest_req->source = src_req;
		dest_req->reload = HT_CACHE_REFRESH;
		dest_req->method = method;
		HTRequest_addDestination(src_req, dest_req);
		dest_req->input_format = WWW_SOURCE;	  /* for now :-( @@@ */
		if (HTLoadAnchor(dest, dest_req) == HT_ERROR)
		    return HT_ERROR;
	    }
	}
    } else {			 /* Use the existing Post Web and restart it */
	src_req = main_req->source;
	if (src_req->mainDestination)
	    if (HTLoadDocument(main_req, NO) == HT_ERROR)
		return HT_ERROR;
	if (src_req->destinations) {
	    HTList *cur = src_anchor->links;
	    HTRequest *pres;
	    while ((pres = (HTRequest *) HTList_nextObject(cur)) != NULL) {
		if (HTLoadDocument(pres, NO) == HT_ERROR)
		    return HT_ERROR;
	    }
	}
    }

    /* Now open the source */
    return HTLoadAnchor(src_anchor, src_req);
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
	if (!HTConfirm(dest_req, buf))
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

