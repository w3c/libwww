/*		Access Manager					HTAccess.c
**		==============
**
** Authors
**	TBL	Tim Berners-Lee timbl@info.cern.ch
**	JFG	Jean-Francois Groff jfg@dxcern.cern.ch
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
** History
**       8 Jun 92 Telnet hopping prohibited as telnet is not secure TBL
**	26 Jun 92 When over DECnet, suppressed FTP, Gopher and News. JFG
**	 6 Oct 92 Moved HTClientHost and logfile into here. TBL
**	17 Dec 92 Tn3270 added, bug fix. DD
**	 4 Feb 93 Access registration, Search escapes bad chars TBL
**		  PARAMETERS TO HTSEARCH AND HTLOADRELATIVE CHANGED
**	28 May 93 WAIS gateway explicit if no WAIS library linked in.
**	   Dec 93 Bug change around, more reentrant, etc
** Bugs
**	This module assumes that that the graphic object is hypertext, as it
**	needs to select it when it has been loaded.  A superclass needs to be
**	defined which accepts select and select_anchor.
*/

#ifndef DEFAULT_WAIS_GATEWAY
#define DEFAULT_WAIS_GATEWAY "http://info.cern.ch:8001/"
#endif

/* Implements:
*/
#include "HTAccess.h"

/* Uses:
*/

#include "HTParse.h"
#include "HTUtils.h"
#include "HTML.h"		/* SCW */

#ifndef NO_RULES
#include "HTRules.h"
#endif

#include <stdio.h>

#include "HTList.h"
#include "HText.h"	/* See bugs above */
#include "HTAlert.h"
#include "HTFWriter.h"	/* for cache stuff */
#include "HTTee.h"

/*	These flags may be set to modify the operation of this module
*/
PUBLIC char * HTClientHost = 0;	/* Name of remote login host if any */
PUBLIC FILE * logfile = 0;	/* File to which to output one-liners */
PUBLIC BOOL HTSecure = NO;	/* Disable access for telnet users? */

/*	To generate other things, play with these:
*/

/* PUBLIC HTFormat HTOutputFormat = NULL;	use request->output_format */
/* PUBLIC HTStream* HTOutputStream = NULL;	use request->output_stream */ 

PRIVATE HTList * protocols = NULL;   /* List of registered protocol descriptors */


/*	Create  a request structure
**	---------------------------
*/

PUBLIC HTRequest * HTRequest_new NOARGS
{
    HTRequest * me = (HTRequest*) calloc(sizeof(*me), 1);  /* zero fill */
    if (!me) outofmem(__FILE__, "HTRequest_new()");
    
    me->conversions	= HTList_new();	/* No conversions registerd yet */
    me->output_format	= WWW_PRESENT;	/* default it to present to user */
    me->scheme		= HTAA_NONE;

    return me;
}


/*	Delete a request structure
**	--------------------------
*/
PUBLIC void HTRequest_delete ARGS1(HTRequest *, req)
{
    if (req) {
	HTList *cur = req->conversions;
	HTPresentation *pres;

	while ((pres = (HTPresentation*)HTList_nextObject(cur)))
	    free(pres);

	free(req);
    }
}



/*	Register a Protocol				HTRegisterProtocol
**	-------------------
*/

PUBLIC BOOL HTRegisterProtocol(protocol)
	HTProtocol * protocol;
{
    if (!protocols) protocols = HTList_new();
    HTList_addObject(protocols, protocol);
    return YES;
}


/*	Register all known protocols
**	----------------------------
**
**	Add to or subtract from this list if you add or remove protocol modules.
**	This routine is called the first time the protocol list is needed,
**	unless any protocols are already registered, in which case it is not called.
**	Therefore the application can override this list.
**
**	Compiling with NO_INIT prevents all known protocols from being forced
**	in at link time.
*/
#ifndef NO_INIT
PRIVATE void HTAccessInit NOARGS			/* Call me once */
{
GLOBALREF HTProtocol HTTP, HTFile, HTTelnet, HTTn3270, HTRlogin;
#ifndef DECNET
GLOBALREF  HTProtocol HTFTP, HTNews, HTGopher;
#ifdef DIRECT_WAIS
GLOBALREF  HTProtocol HTWAIS;
#endif
    HTRegisterProtocol(&HTFTP);
    HTRegisterProtocol(&HTNews);
    HTRegisterProtocol(&HTGopher);
#ifdef DIRECT_WAIS
    HTRegisterProtocol(&HTWAIS);
#endif
#endif

    HTRegisterProtocol(&HTTP);
    HTRegisterProtocol(&HTFile);
    HTRegisterProtocol(&HTTelnet);
    HTRegisterProtocol(&HTTn3270);
    HTRegisterProtocol(&HTRlogin);
}
#endif


/*		Find physical name and access protocol
**		--------------------------------------
**
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**	anchor		a pareent anchor with whose address is addr
**
** On exit,
**	returns		HT_NO_ACCESS		Error has occured.
**			HT_OK			Success
**
*/
PRIVATE int get_physical ARGS1(HTParentAnchor *,	anchor)
{
    char * access=0;	/* Name of access method */
    char * physical = 0;
    char * addr = HTAnchor_address((HTAnchor*)anchor);	/* free me */
    
#ifndef NO_RULES
    physical = HTTranslate(addr);
    if (!physical) {
    	free(addr);
	return HT_FORBIDDEN;
    }
    HTAnchor_setPhysical(anchor, physical);
    free(physical);			/* free our copy */
#else
    HTAnchor_setPhysical(anchor, addr);
#endif

    access =  HTParse(HTAnchor_physical(anchor),
    		"file:", PARSE_ACCESS);

/*	Check whether gateway access has been set up for this
**
**	This function can be replaced by the rule system above.
*/
#define USE_GATEWAYS
#ifdef USE_GATEWAYS
    {
	char * gateway_parameter, *gateway;
	gateway_parameter = (char *)malloc(strlen(access)+20);
	if (gateway_parameter == NULL) outofmem(__FILE__, "HTLoad");
	strcpy(gateway_parameter, "WWW_");
	strcat(gateway_parameter, access);
	strcat(gateway_parameter, "_GATEWAY");
	gateway = (char *)getenv(gateway_parameter); /* coerce for decstation */
	free(gateway_parameter);
	
#ifndef DIRECT_WAIS
	if (!gateway && 0==strcmp(access, "wais")) {
	    gateway = DEFAULT_WAIS_GATEWAY;
	}
#endif
	if (gateway) {
	    char * path = HTParse(addr, "",
	    	PARSE_HOST + PARSE_PATH + PARSE_PUNCTUATION);
		/* Chop leading / off to make host into part of path */
	    char * gatewayed = HTParse(path+1, gateway, PARSE_ALL);
	    free(path);
            HTAnchor_setPhysical(anchor, gatewayed);
	    free(gatewayed);
	    free(access);
	    
    	    access =  HTParse(HTAnchor_physical(anchor),
    		"http:", PARSE_ACCESS);
	}
    }
#endif

    free(addr);


/*	Search registered protocols to find suitable one
*/
    {
	HTList *cur;
	HTProtocol *p;
#ifndef NO_INIT
        if (!protocols) HTAccessInit();
#endif
	cur = protocols;
	while ((p = (HTProtocol*)HTList_nextObject(cur))) {
	    if (strcmp(p->name, access)==0) {
		HTAnchor_setProtocol(anchor, p);
		free(access);
		return (HT_OK);
	    }
	}
    }

    free(access);
    return HT_NO_ACCESS;
}


/*		Load a document
**		---------------
**
**	This is an internal routine, which has an address AND a matching
**	anchor.  (The public routines are called with one OR the other.)
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**	request->
**	    anchor		a parent anchor with whose address is addr
**	    output_format	valid
**	    output_stream	valid on NULL
**
** On exit,
**	returns		<0		Error has occured.
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**
*/
PRIVATE int HTLoad ARGS2(
	CONST char *,		addr,	/* not used */
	HTRequest *,		request)
{
    HTProtocol* p;
    int status;
    request->method = HTAtom_for("GET");
    status = get_physical(request->anchor);
    if (status == HT_FORBIDDEN) {
        return HTLoadError(request->output_stream, 500,
		"Access forbidden by rule");
    }
    if (status < 0) return status;	/* Can't resolve or forbidden */
    
    p = HTAnchor_protocol(request->anchor);
    return (*(p->load))(request);
}


/*		Get a save stream for a document
**		--------------------------------
*/
PUBLIC HTStream *HTSaveStream ARGS1(HTRequest *, request)
{
    HTProtocol * p;
    int status;
    request->method = HTAtom_for("PUT");
    status = get_physical(request->anchor);
    if (status == HT_FORBIDDEN) {
        HTLoadError(request->output_stream, 500,
		"Access forbidden by rule");
	return NULL;	/* should return error status? */
    }
    if (status < 0) return NULL; /* @@ error. Can't resolve or forbidden */
    
    p = HTAnchor_protocol(request->anchor);
    if (!p) return NULL;
    
    return (*p->saveStream)(request);
    
}


/*		Load a document - with logging etc
**		----------------------------------
**
**	- Checks or documents already loaded
**	- Logs the access
**	- Allows stdin filter option
**	- Trace ouput and error messages
**
**    On Entry,
**        request->anchor	valid for of the document to be accessed.
**	 request->childAnchor   optional anchor within doc to be selected
**
**        filter            if YES, treat stdin as HTML
**
**	  request->anchor   is the node_anchor for the document
**	  request->output_format is valid
**
**    On Exit,
**        returns    YES     Success in opening document
**                   NO      Failure 
**
*/

PRIVATE BOOL HTLoadDocument ARGS1(HTRequest *,		request)

{
    int	        status;
    HText *	text;
    char * full_address = HTAnchor_address((HTAnchor*)request->anchor);
    
    if (TRACE) fprintf (stderr,
      "HTAccess: loading document %s\n", full_address);

    request->using_cache = NULL;
    
    if (!request->output_format) request->output_format = WWW_PRESENT;
    
    if ((text=(HText *)HTAnchor_document(request->anchor)))
    {	/* Already loaded */
        if (TRACE) fprintf(stderr, "HTAccess: Document already in memory.\n");
	if (request->childAnchor) {
	    HText_selectAnchor(text, request->childAnchor);
	} else {
	    HText_select(text);	
	}
	free(full_address);
	return YES;
    }
    
    /* Check the Cache
    */
    /* Bug: for each format, we only check whether it is ok, we
       don't check them all and chose the best */
    if (request->anchor->cacheItems) {
        HTList * list = request->anchor->cacheItems;
	HTList * cur = list;
	HTCacheItem * item;

	while ((item = (HTCacheItem*)HTList_nextObject(cur))) {
	    HTStream * s;
	    
	    request->using_cache = item;
	    
	    s = HTStreamStack(item->format, request);
	    if (s) {		/* format was suitable */
	        FILE * fp = fopen(item->filename, "r");
	    	if (TRACE) fprintf(stderr, "Cache: HIT file %s for %s\n",
				   item->filename, 
				   full_address);
		if (fp) {
		    HTFileCopy(fp, s);
		    fclose(fp);
		    free(full_address);
		    return YES;
		} else {
		    fprintf(stderr, "***** Can't read cache file %s !\n",
			    item->filename);
		} /* file open ok */
	    } /* stream ok */
	} /* next cache item */
    } /* if cache available for this anchor */
    
    status = HTLoad(full_address, request);

    
/*	Log the access if necessary
*/
    if (logfile) {
	time_t theTime;
	time(&theTime);
	fprintf(logfile, "%24.24s %s %s %s\n",
	    ctime(&theTime),
	    HTClientHost ? HTClientHost : "local",
	    status<0 ? "FAIL" : "GET",
	    full_address);
	fflush(logfile);	/* Actually update it on disk */
	if (TRACE) fprintf(stderr, "Log: %24.24s %s %s %s\n",
	    ctime(&theTime),
	    HTClientHost ? HTClientHost : "local",
	    status<0 ? "FAIL" : "GET",
	    full_address);
    }
    

    if (status == HT_LOADED) {
	if (TRACE) {
	    fprintf(stderr, "HTAccess: `%s' has been accessed.\n",
	    full_address);
	}
	free(full_address);
	return YES;
    }
    
    if (status == HT_NO_DATA) {
	if (TRACE) {
	    fprintf(stderr, 
	    "HTAccess: `%s' has been accessed, No data left.\n",
	    full_address);
	}
	free(full_address);
	return NO;
    }
    
    if (status<0) {		      /* Failure in accessing a document */
#ifdef CURSES
        user_message("Can't access `%s'", full_address);
#else
	if (TRACE) fprintf(stderr, 
		"HTAccess: Can't access `%s'\n", full_address);
#endif
	HTLoadError(request->output_stream, 500, "Unable to access document.");
	free(full_address);
	return NO;
    }
 
    /* If you get this, then please find which routine is returning
       a positive unrecognised error code! */
 
    fprintf(stderr,
    "**** HTAccess: socket or file number returned by obsolete load routine!\n");
    fprintf(stderr,
    "**** HTAccess: Internal software error. Please mail www-bug@info.cern.ch quoting the version number of this software and the URL: %s!\n",
    	full_address);
    free(full_address);
   
    exit(-6996);
    return NO;	/* For gcc :-( */
} /* HTLoadDocument */



/*		Load a document from absolute name
**		---------------
**
**    On Entry,
**        addr     The absolute address of the document to be accessed.
**        filter   if YES, treat document as HTML
**
**    On Exit,
**        returns    YES     Success in opening document
**                   NO      Failure 
**
**
*/

PUBLIC BOOL HTLoadAbsolute ARGS2(CONST char *,addr, HTRequest*, request)
{
   HTAnchor * anchor = HTAnchor_findAddress(addr);
   request->anchor = HTAnchor_parent(anchor);
   request->childAnchor = ((HTAnchor*)request->anchor == anchor) ?
   			NULL : (HTChildAnchor*) anchor;
   return HTLoadDocument(request);
}


/*		Load a document from absolute name to stream
**		--------------------------------------------
**
**    On Entry,
**        addr     The absolute address of the document to be accessed.
**        request->output_stream     if non-NULL, send data down this stream
**
**    On Exit,
**        returns    YES     Success in opening document
**                   NO      Failure 
**
**
*/

PUBLIC BOOL HTLoadToStream ARGS3(
		CONST char *,	addr,
		BOOL, 		filter,
		HTRequest*,	request)
{
   HTAnchor * anchor = HTAnchor_findAddress(addr);
   request->anchor = HTAnchor_parent(anchor);
   request->childAnchor = ((HTAnchor*)request->anchor == anchor) ? NULL :
   	(HTChildAnchor*) anchor;
    request->output_stream = request->output_stream;
    return HTLoadDocument(request);
}




/*		Load a document from relative name
**		---------------
**
**    On Entry,
**        relative_name     The relative address of the document
**	  		    to be accessed.
**
**    On Exit,
**        returns    YES     Success in opening document
**                   NO      Failure 
**
**
*/

PUBLIC BOOL HTLoadRelative ARGS3(
		CONST char *,		relative_name,
		HTParentAnchor *,	here,
		HTRequest *,		request)
{
    char * 		full_address = 0;
    BOOL       		result;
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
**    On Entry,
**        destination      	    The child or parenet anchor to be loaded.
**
**    On Exit,
**        returns    YES     Success
**                   NO      Failure 
**
*/

PUBLIC BOOL HTLoadAnchor ARGS2(HTAnchor*, anchor, HTRequest *, request)
{
    if (!anchor) return NO;	/* No link */
    
    request->anchor  = HTAnchor_parent(anchor);
    request->childAnchor = ((HTAnchor*)request->anchor == anchor) ? NULL
    					: (HTChildAnchor*) anchor;
    
    return HTLoadDocument(request) ? YES : NO;
	
} /* HTLoadAnchor */


/*		Search
**		------
**  Performs a keyword search on word given by the user. Adds the keyword to 
**  the end of the current address and attempts to open the new address.
**
**  On Entry,
**       *keywords  	space-separated keyword list or similar search list
**	here		is anchor search is to be done on.
*/

PRIVATE char hex(i)
    int i;
{
    char * hexchars = "0123456789ABCDEF";
    return hexchars[i];
}

PUBLIC BOOL HTSearch ARGS3(
	CONST char *, 		keywords,
	HTParentAnchor *, 	here,
	HTRequest *,		request)
{

#define acceptable \
"1234567890abcdefghijlkmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-_"

    char *q, *u;
    CONST char * p, *s, *e;		/* Pointers into keywords */
    char * address = HTAnchor_address((HTAnchor*)here);
    BOOL result;
    char * escaped = malloc(strlen(keywords)*3+1);

    static CONST BOOL isAcceptable[96] =

    /*   0 1 2 3 4 5 6 7 8 9 A B C D E F */
    {    0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,	/* 2x   !"#$%&'()*+,-./	 */
         1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,	/* 3x  0123456789:;<=>?	 */
	 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/* 4x  @ABCDEFGHIJKLMNO  */
	 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,	/* 5X  PQRSTUVWXYZ[\]^_	 */
	 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/* 6x  `abcdefghijklmno	 */
	 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0 };	/* 7X  pqrstuvwxyz{\}~	DEL */

    if (escaped == NULL) outofmem(__FILE__, "HTSearch");
    

/*	Convert spaces to + and hex escape unacceptable characters
*/
    for(s=keywords; *s && WHITE(*s); s++) /*scan */ ;	/* Skip white space */
    for(e = s + strlen(s); e>s && WHITE(*(e-1)) ; e--); /* Skip trailers */
    for(q=escaped, p=s; p<e; p++) {			/* scan stripped field */
        int c = (int)TOASCII(*p);
        if (WHITE(*p)) {
	    *q++ = '+';
	} else if (c>=32 && c<=(char)127 && isAcceptable[c-32]) {
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
**  On Entry,
**       *keywords  	space-separated keyword list or similar search list
**	*addres		is name of object search is to be done on.
*/

PUBLIC BOOL HTSearchAbsolute ARGS3(
	CONST char *, 	keywords,
	CONST char *, 	indexname,
	HTRequest *,	request)
{
    HTParentAnchor * anchor =
    	(HTParentAnchor*) HTAnchor_findAddress(indexname);
    return HTSearch(keywords, anchor, request);
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
**		4	http://info.cern.ch/default.html
**
*/
PUBLIC HTParentAnchor * HTHomeAnchor NOARGS
{
    char * my_home_document = NULL;
    char * home = (char *)getenv(LOGICAL_DEFAULT);
    char * ref;
    HTParentAnchor * anchor;
    
    if (home) {
        StrAllocCopy(my_home_document, home);
    
/* 	Someone telnets in, they get a special home.
*/
#define MAX_FILE_NAME 1024					/* @@@ */
    } else  if (HTClientHost) {			/* Telnet server */
    	FILE * fp = fopen(REMOTE_POINTER, "r");
	char * status;
	if (fp) {
	    my_home_document = (char*) malloc(MAX_FILE_NAME);
	    status = fgets(my_home_document, MAX_FILE_NAME, fp);
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
	CONST char * home =  (CONST char*)getenv("HOME");
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
	if (TRACE) fprintf(stderr,
	    "HTBrowse: No local home document ~/%s or %s\n",
	    PERSONAL_DEFAULT, LOCAL_DEFAULT_FILE);
	    free(my_home_document);
	    my_home_document = NULL;
	}
    }
#endif
    ref = HTParse( my_home_document ?	my_home_document :
				HTClientHost ? REMOTE_ADDRESS
				: LAST_RESORT,
		    "file:",
		    PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
    if (my_home_document) {
	if (TRACE) fprintf(stderr,
	    "HTAccess: Using custom home page %s i.e. address %s\n",
	    my_home_document, ref);
	free(my_home_document);
    }
    anchor = (HTParentAnchor*) HTAnchor_findAddress(ref);
    free(ref);
    return anchor;
}


