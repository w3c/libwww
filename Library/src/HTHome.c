/*								     HTHome.c
**	ANCHOR TRANSLATIONS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
** Authors
**	TBL	Tim Berners-Lee timbl@w3.org
**	JFG	Jean-Francois Groff jfg@dxcern.cern.ch
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
**	HFN	Henrik Frystyk
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
**	   Nov 95 Spawned from HTAccess.c
*/

/* Library include files */
#include "WWWLib.h"
#include "WWWApp.h"
#include "WWWCache.h"
#include "WWWRules.h"
#include "HTReqMan.h"
#include "HTHome.h"					 /* Implemented here */

/* ------------------------------------------------------------------------- */

/*	Find Related Name
**	-----------------
**	Creates a string that can be used as a related name when 
**	calling HTParse initially. 
**  
**	The code for this routine originates from the Linemode 
**	browser and was moved here by howcome@w3.org
**	in order for all clients to take advantage.
**	The string returned must be freed by the caller
*/
PUBLIC char * HTFindRelatedName (void)
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


/*	Generate the anchor for the home page
**	-------------------------------------
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
PUBLIC HTParentAnchor * HTHomeAnchor (void)
{
    char * my_home_document = NULL;
    char * home = (char *) getenv(LOGICAL_DEFAULT);
    char * ref;
    HTParentAnchor * anchor;
    
    /* Someone telnets in, they get a special home */
    if (home) {
        StrAllocCopy(my_home_document, home);
    } else  if (HTLib_secure()) {				    /* Telnet server */
    	FILE * fp = fopen(REMOTE_POINTER, "r");
	char * status;
	if (fp) {
	    if ((my_home_document = (char *) HT_MALLOC(HT_MAX_PATH)) == NULL)
	        HT_OUTOFMEM("my_home_document ");
	    status = fgets(my_home_document, HT_MAX_PATH, fp);
	    if (!status) {
	        HT_FREE(my_home_document);
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
	    if ((my_home_document = (char  *) HT_MALLOC(strlen(home)+1+ strlen(PERSONAL_DEFAULT)+1)) == NULL)
	        HT_OUTOFMEM("HTLocalName");
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
		TTYPrint(TDEST,
			"HTBrowse: No local home document ~/%s or %s\n",
			PERSONAL_DEFAULT, LOCAL_DEFAULT_FILE);
	    HT_FREE(my_home_document);
	    my_home_document = NULL;
	}
    }
#endif
    ref = HTParse(my_home_document ? my_home_document :
		  HTLib_secure() ? REMOTE_ADDRESS : LAST_RESORT, "file:",
		  PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
    if (my_home_document) {
	if (WWWTRACE)
	    TTYPrint(TDEST,
		   "HTAccess.... `%s\' used for custom home page as\n`%s\'\n",
		    my_home_document, ref);
	HT_FREE(my_home_document);
    }
    anchor = (HTParentAnchor*) HTAnchor_findAddress(ref);
    HT_FREE(ref);
    return anchor;
}

/*	Application "BEFORE" Callback
**	-----------------------------
**	This function uses all the functionaly that the app part of the Library
**	gives for URL translations BEFORE a request is isseud.
**	It checks for Cache, proxy, and gateway (in that order)
**	returns		HT_LOADED		We already have this
**			HT_ERROR		We can't load this
**			HT_OK			Success
*/
PUBLIC int HTLoadStart (HTRequest * request, int status)
{    
    HTParentAnchor *anchor = HTRequest_anchor(request);
    char * addr = HTAnchor_address((HTAnchor *) anchor);
    HTReload mode = HTRequest_reloadMode(request);

    /*
    ** Check if document is already loaded. 
    */
    if (mode != HT_FORCE_RELOAD) {
	if (HTMemoryCache_check(request) == HT_LOADED) {
	    HT_FREE(addr);
	    return HT_LOADED;
	}
    } else {
	HTRequest_addGnHd(request, HT_G_NO_CACHE);	  /* No-cache pragma */
	HTAnchor_clearHeader(request->anchor);
    }

    /*
    ** Check if we have any rule translations to do
    */
    {
	HTList *list = HTRule_global();
	char * physical = HTRule_translate(list, addr, NO);
	if (!physical) {
	    char *url = HTAnchor_address((HTAnchor *) request->anchor);
	    if (url) {
		HTUnEscape(url);
		HTRequest_addError(request, ERR_FATAL, NO, HTERR_FORBIDDEN,
			   (void *) url, (int) strlen(url), "HTLoad");
	    } else {
		HTRequest_addError(request, ERR_FATAL, NO, HTERR_FORBIDDEN,
			   NULL, 0, "HTLoad");
	    }
	    HT_FREE(addr);
	    HT_FREE(url);
	    return HT_ERROR;
	}
	HTAnchor_setPhysical(anchor, physical);
	HT_FREE(physical);
    }

    /*
    ** Check local Disk Cache (if we are not forced to reload), then
    ** for proxy, and finally gateways
    */
    {
	char *newaddr=NULL;
	if (mode != HT_FORCE_RELOAD && (newaddr = HTCache_getReference(addr))){
	    if (mode != HT_CACHE_REFRESH) {
		HTAnchor_setPhysical(anchor, newaddr);
		HTAnchor_setCacheHit(anchor, YES);
	    } else {			 /* If refresh version in file cache */
		HTRequest_addGnHd(request, HT_G_NO_CACHE);
		HTRequest_addRqHd(request, HT_C_IMS);
	    }
	} else if ((newaddr = HTProxy_find(addr))) {
	    StrAllocCat(newaddr, addr);
	    request->using_proxy = YES;
	    HTAnchor_setPhysical(anchor, newaddr);
	} else if ((newaddr = HTGateway_find(addr))) {
	    char * path = HTParse(addr, "",
				  PARSE_HOST + PARSE_PATH + PARSE_PUNCTUATION);
		/* Chop leading / off to make host into part of path */
	    char * gatewayed = HTParse(path+1, newaddr, PARSE_ALL);
            HTAnchor_setPhysical(anchor, gatewayed);
	    HT_FREE(path);
	    HT_FREE(gatewayed);
	} else {
	    request->using_proxy = NO;      /* We don't use proxy or gateway */
	}
	HT_FREE(newaddr);
    }
    HT_FREE(addr);
    return HT_OK;
}

/*	Application "AFTER" Callback
**	-----------------------------
**	This function uses all the functionaly that the app part of the Library
**	gives for handling AFTER a request.
*/
PUBLIC int HTLoadTerminate (HTRequest * request, int status)
{
    char * uri = HTAnchor_address((HTAnchor*)request->anchor);
    switch (status) {
      case HT_LOADED:
	if (PROT_TRACE)
	    TTYPrint(TDEST, "Load End.... OK: `%s\' has been accessed\n", uri);
	break;

      case HT_NO_DATA:
	if (PROT_TRACE)
	    TTYPrint(TDEST, "Load End.... OK BUT NO DATA: `%s\'\n", uri);
	break;

      case HT_INTERRUPTED:
	if (PROT_TRACE)
	    TTYPrint(TDEST, "Load End.... INTERRUPTED: `%s\'\n", uri);
	break;

      case HT_RETRY:
	if (PROT_TRACE)
	    TTYPrint(TDEST, "Load End.... NOT AVAILABLE, RETRY AT %ld\n",
		     HTRequest_retryTime(request));
	break;

      case HT_ERROR:
	{
	    HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	    if (cbf) (*cbf)(request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			    request->error_stack, NULL);
	}
	if (PROT_TRACE)
	    TTYPrint(TDEST, "Load End.... ERROR: Can't access `%s\'\n",
		     uri ? uri : "<UNKNOWN>");
	break;

      default:
	if (PROT_TRACE)
	    TTYPrint(TDEST, "Load End.... UNKNOWN RETURN CODE %d\n", status);
	break;
    }

    /* Should we do logging? */
    if (HTLog_isOpen()) HTLog_add(request, status);
    HT_FREE(uri);
    return HT_OK;
}
