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
**		  PARAMETERS TO HTSEARCH AND HTLOAFRELATIVE CHANGED
**
** Bugs
**	This module assumes that that the graphic object is hypertext, as it
**	needs to select it when it has been loaded.  A supercalss needs to be
**	defined which accepts select and select_anchor.
*/

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


/*	These flags may be set to modify the operation of this module
*/
PUBLIC char * HTClientHost = 0;	/* Name of remote login host if any */
PUBLIC FILE * logfile = 0;	/* File to which to output one-liners */

/*	To generate other things, play with these:
*/

PUBLIC HTFormat HTOutputFormat = NULL;
PUBLIC HTStream* HTOutputStream = NULL;	/* For non-interactive, set this */ 

PRIVATE HTList * protocols = NULL;   /* List of registered protocol descriptors */


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
extern HTProtocol HTTP, HTFile, HTTelnet, HTTn3270, HTRlogin;
#ifndef DECNET
extern HTProtocol HTFTP, HTNews, HTGopher;
#ifdef DIRECT_WAIS
extern HTProtocol HTWAIS;
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
PRIVATE int get_physical ARGS2(
	CONST char *,		addr,
	HTParentAnchor *,	anchor)
{
    char * access=0;	/* Name of access method */
    char * physical = 0;
    
#ifndef NO_RULES
    physical = HTTranslate(addr);
    if (!physical) {
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
*/
#ifdef USE_GATEWAYS
    {
	char * gateway_parameter, gateway;
	gateway_parameter = (char *)malloc(strlen(access)+20);
	if (gateway_parameter == NULL) outofmem(__FILE__, "HTLoad");
	strcpy(gateway_parameter, "WWW_");
	strcat(gateway_parameter, access);
	strcat(gateway_parameter, "_GATEWAY");
	gateway = (char *)getenv(gateway_parameter); /* coerce for decstation */
	free(gateway_parameter);
	if (gateway) {
	    status = HTLoadHTTP(addr, gateway, anchor,
	    	HTOutputFormat ? HTOutputFormat : WWW_PRESENT, sink);
	    HTAlert("Cannot retrieve required information from gateway.");
	    free(access);
	    return status;
	}
    }
#endif



/*	Search registered protocols to find suitable one
*/
    {
	int i, n;
#ifndef NO_INIT
        if (!protocols) HTAccessInit();
#endif
	n = HTList_count(protocols);
	for (i=0; i<n; i++) {
	    HTProtocol *p = HTList_objectAt(protocols, i);
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
**	anchor		a pareent anchor with whose address is addr
**
** On exit,
**	returns		<0		Error has occured.
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**
*/
PRIVATE int HTLoad ARGS4(
	CONST char *,		addr,
	HTParentAnchor *,	anchor,
	HTFormat,		format_out,
	HTStream *,		sink)
{
    HTProtocol* p;
    int status = get_physical(addr, anchor);
    if (status == HT_FORBIDDEN) {
        return HTLoadError(sink, 500, "Access forbidden by rule");
    }
    if (status < 0) return status;	/* Can't resolve or forbidden */
    
    p = HTAnchor_protocol(anchor);
    return (*(p->load))(HTAnchor_physical(anchor),
    			anchor, format_out, sink);
}


/*		Get a save stream for a document
**		--------------------------------
*/
PUBLIC HTStream *HTSaveStream ARGS1(HTParentAnchor *, anchor)
{
    HTProtocol * p = HTAnchor_protocol(anchor);
    if (!p) return NULL;
    
    return (*p->saveStream)(anchor);
    
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
**	  anchor	    is the node_anchor for the document
**        full_address      The address of the document to be accessed.
**        filter            if YES, treat stdin as HTML
**
**    On Exit,
**        returns    YES     Success in opening document
**                   NO      Failure 
**
*/

PRIVATE BOOL HTLoadDocument ARGS4(
	CONST char *,		full_address,
	HTParentAnchor *,	anchor,
	HTFormat,		format_out,
	HTStream*,		sink)

{
    int	        status;
    HText *	text;

    if (TRACE) fprintf (stderr,
      "HTAccess: loading document %s\n", full_address);

    if (text=(HText *)HTAnchor_document(anchor)) {	/* Already loaded */
        if (TRACE) fprintf(stderr, "HTAccess: Document already in memory.\n");
        HText_select(text);
	return YES;
    }
    
    status = HTLoad(full_address, anchor, format_out, sink);

    
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
	return YES;
    }
    
    if (status == HT_NO_DATA) {
	if (TRACE) {
	    fprintf(stderr, 
	    "HTAccess: `%s' has been accessed, No data left.\n",
	    full_address);
	}
	return NO;
    }
    
    if (status<0) {		      /* Failure in accessing a document */
#ifdef CURSES
        user_message("Can't access `%s'", full_address);
#else
	if (TRACE) fprintf(stderr, 
		"HTAccess: Can't access `%s'\n", full_address);
#endif
	HTLoadError(sink, 500, "Unable to access document.")
	return NO;
    }
    
    fprintf(stderr,
    "**** HTAccess: socket or file number returned by obsolete load routine!\n");
    exit(-6996);

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

PUBLIC BOOL HTLoadAbsolute ARGS1(CONST char *,addr)
{
   return HTLoadDocument( addr,
       		HTAnchor_parent(HTAnchor_findAddress(addr)),
       			HTOutputFormat ? HTOutputFormat : WWW_PRESENT,
			HTOutputStream);
}


/*		Load a document from absolute name to stream
**		--------------------------------------------
**
**    On Entry,
**        addr     The absolute address of the document to be accessed.
**        sink     if non-NULL, send data down this stream
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
		HTStream *, 	sink)
{
   return HTLoadDocument(addr,
       		HTAnchor_parent(HTAnchor_findAddress(addr)),
       			HTOutputFormat ? HTOutputFormat : WWW_PRESENT,
			sink);
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

PUBLIC BOOL HTLoadRelative ARGS2(
		CONST char *,		relative_name,
		HTParentAnchor *,	here)
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
    result = HTLoadAbsolute(full_address);
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

PUBLIC BOOL HTLoadAnchor ARGS1(HTAnchor *,destination)
{
    HTParentAnchor * parent;
    BOOL loaded = NO;
    if (!destination) return NO;	/* No link */
    
    parent  = HTAnchor_parent(destination);
    
    if (HTAnchor_document(parent) == NULL) {	/* If not alread loaded */
    						/* TBL 921202 */

        BOOL result;
        char * address = HTAnchor_address((HTAnchor*) parent);
	result = HTLoadDocument(address, parent,
		HTOutputFormat ? HTOutputFormat : WWW_PRESENT,
		HTOutputStream);
	free(address);
	if (!result) return NO;
	loaded = YES;
    }
    
    {
	HText *text = (HText*)HTAnchor_document(parent);
	if (destination != (HTAnchor *)parent) {  /* If child anchor */
	    HText_selectAnchor(text, 
		    (HTChildAnchor*)destination); /* Double display? @@ */
	} else {
	    if (!loaded) HText_select(text);
	}
    }
    return YES;
	
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
    return i < 10 ? '0'+i : 'A'+ i - 10;
}

PUBLIC BOOL HTSearch ARGS2(
	CONST char *, 		keywords,
	HTParentAnchor *, 	here)
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
	    *q++ = (char)c;
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
    result = HTLoadRelative(address, here);
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

PUBLIC BOOL HTSearchAbsolute ARGS2(
	CONST char *, 	keywords,
	CONST char *, 	indexname)
{
    HTParentAnchor * anchor =
    	(HTParentAnchor*) HTAnchor_findAddress(indexname);
    return HTSearch(keywords, anchor);
}


/*		Generate the anchor for the home page
**		-------------------------------------
**
**	As it involves file access, this should only be done once
**	when the program first runs.
**	This is a default algorithm -- browesr don't HAVE to use this.
**
*/
PUBLIC HTParentAnchor * HTHomeAnchor NOARGS
{
    char * my_home = (char *)getenv(LOGICAL_DEFAULT);
    BOOL got_local_default = NO;
    char * ref;
    HTParentAnchor * anchor;
    
#ifdef unix
    {
	FILE * fp = fopen(LOCAL_DEFAULT_FILE, "r");
	if (fp) {
	    fclose(fp);
	    got_local_default = YES;
	} else {
	if (TRACE) fprintf(stderr,
	    "HTBrowse: No local default home %s\n",
	    LOCAL_DEFAULT_FILE);
	}
    }
#endif
    ref = HTParse( my_home ?	my_home :
				HTClientHost ? REMOTE_ADDRESS :
				    got_local_default ? LOCAL_DEFAULT
					    : LAST_RESORT,
		    LAST_RESORT,
		    PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
    if (my_home) {
	if (TRACE) fprintf(stderr,
	    "HTAccess: Using custom home page %s i.e. address %s\n",
	    my_home, ref);
    }
    anchor = (HTParentAnchor*) HTAnchor_findAddress(ref);
    free(ref);
    return anchor;
}


