/*		Access Manager					HTAccess.c
**		==============
**
** Authors
**	TBL	Tim Berners-Lee timbl@info.cern.ch
**	JFG	Jean-Francois Groff jgh@next.com
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
** History
**       8 Jun 92 Telnet hopping prohibited as telnet is not secure TBL
**	26 Jun 92 When over DECnet, suppressed FTP, Gopher and News. JFG
**	 6 Oct 92 Moved HTClientHost and logfile into here. TBL
**	17 Dec 92 Tn3270 added, bug fix. DD
*/

/* Implements:
*/
#include "HTAccess.h"

/* Uses:
*/

#include "HTParse.h"
#include "HTUtils.h"
#include "WWW.h"
#include "HTAnchor.h"
#include "HTTP.h"
#include "HTFile.h"
#include <errno.h>
#include <stdio.h>

#include "tcp.h"
#include "HTList.h"
#include "HText.h"
#ifndef DECNET
#include "HTFTP.h"
#include "HTGopher.h"
#include "HTNews.h"
#endif


/*	These flags may be set to modify the operation of this module
*/
PUBLIC int HTDiag = 0;		/* Diagnostics: load source as text */
PUBLIC char * HTClientHost = 0;	/* Name of remote login host if any */
PUBLIC FILE * logfile = 0;	/* File to which to output one-liners */


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
extern HTProtocol HTTP, HTFile, HTTelnet, HTTn3270, HTRlogin
#ifndef DECNET
extern HTProtocol HTFTP, HTNews, HTGopher
    HTRegisterProtocol(HTFTP);
    HTRegisterProtocol(HTNews);
    HTRegisterProtocol(HTGopher);
#endif

    HTRegisterProtocol(HTTP);
    HTRegisterProtocol(HTFile);
    HTRegisterProtocol(HTTelnet);
    HTRegisterProtocol(HTTn3270);
    HTRegisterProtocol(HTRlogin);
}
#endif


/*		Load a document
**		---------------
**
**	This is an internal routine, which has an address and a matching
**	anchor.  The public routines are called with one or the other.
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**	anchor		a pareent anchor with whose address is addr
**
** On exit,
**	returns		<0		Error has occured.
**			>=0		Success
**			HT_NO_DATA	Success, but no document loaded.
**
*/
PRIVATE int HTLoad ARGS2(
	CONST char *,addr1,
	HTParentAnchor *,anchor)
{
    char * access=0;	/* Name of access method */
    int status;
    char * gateway;
    char * gateway_parameter;
    char * addr = (char *)malloc(strlen(addr1)+1);
    
    if (addr == NULL) outofmem(__FILE__, "HTLoad");
    strcpy(addr, addr1);			/* Copy to play with */
    
    access =  HTParse(addr, "file:", PARSE_ACCESS);

/*	Check whether gateway access has been set up for this
*/
#ifdef USE_GATEWAYS
    gateway_parameter = (char *)malloc(strlen(access)+20);
    if (gateway_parameter == NULL) outofmem(__FILE__, "HTLoad");
    strcpy(gateway_parameter, "WWW_");
    strcat(gateway_parameter, access);
    strcat(gateway_parameter, "_GATEWAY");
    gateway = (char *)getenv(gateway_parameter); /* coerce for decstation */
    free(gateway_parameter);
    if (gateway) {
	status = HTLoadHTTP(addr, gateway, anchor, HTDiag);
        goto done;
    } 
#endif


/*	Use rule file to vary actual access protocol
*/
#ifdef RULES
	/* @@@ TBD */
#endif


/*	Search registered protocols to find suitable one
*/
    {
	int i, n;
#ifndef NO_INIT
        if (!protocols) HTAccessInitialize();
#endif
	n = HTList_count(protocols);
	for (i=0; i<n; i++) {
	    HTProtocol *p = HTList_objectAt(i);
	    if (strcmp(p->name, access)==0) [
	        status = (*(p->load))(addr, anchor, HTDiag);
		break;
	    }
	}
    }

#ifdef OLD_CODE

    if (0==strcmp(access, "http")) {
        status = HTLoadHTTP(addr, 0, anchor, HTDiag);
#ifndef CURSES
	if (status<0) fprintf(stderr,	/* For simple users */
		"Cannot connect to information server.\n");
#endif
    } else if ((0==strcmp(access, "file"))
           ||  (0==strcmp(access, "ftp"))) {	/* TBL 921021 */
        status = HTLoadFile(addr, anchor, HTDiag);
#ifndef DECNET
    } else if (0==strcmp(access, "news")) {
        status = HTLoadNews(addr, anchor, HTDiag);
	if (status>0) status = HT_LOADED;

    } else if (0==strcmp(access, "gopher")) {
        status = HTLoadGopher(addr, anchor, HTDiag);
	if (status>0) status = HT_LOADED;
#endif
    } else if (!strcmp(access, "telnet") ||		/* TELNET */
    	!strcmp(access, "rlogin")||			/* etc */
    	!strcmp(access, "tn3270")) {
        char * host = HTParse(addr, "", PARSE_HOST);
	status = remote_session(access, host);
	free(host);
	
    } else if (0==strcmp(access, "wais")) {
        user_message(
"HTAccess: For WAIS access set WWW_wais_GATEWAY to gateway address.\n");
    } else {

        user_message(
	"HTAccess: name scheme `%s' unknown by this browser version.\n",
		 access);
        status = -1;
    }
#endif /* old code */

done:
#ifndef CURSES
	if (status<0) fprintf(stderr,	/* For simple users */
	    "Cannot retrieve required information.\n",);
#endif
    free(access);
    free(addr);
    return status;
}



/*		Load a document
**		---------------
**
**    On Entry,
**	  anchor	    is the node_anchor for the document
**        full_address      The address of the document to be accessed.
**        filter            if YES, treat document as HTML
**
**    On Exit,
**        returns    YES     Success in opening document
**                   NO      Failure 
**
*/

PUBLIC BOOL HTLoadDocument ARGS3(HTParentAnchor *,anchor,
	CONST char *,full_address,
	BOOL,	filter)

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
    
#ifdef CURSES
      prompt_set("Retrieving document...");
#endif
    if (filter) {
        status = 0;
    } else {
	status = HTLoad(full_address, anchor);
    }
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
    
    if (status<0) {		      /* Failure in accessing a file */

#ifdef CURSES
        user_message("Can't access `%s'", full_address);
#else
	if (TRACE)
	    fprintf(stderr, "HTAccess: Can't access `%s'\n", full_address);
#endif

	return NO;
    }
    
    fprintf(stderr,
    "**** HTAcess: socket or file number returned by obsolete load routine!\n");
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

PUBLIC BOOL HTLoadAbsolute ARGS2(CONST char *,addr, BOOL, filter)
{
   return HTLoadDocument(
       		HTAnchor_parent(HTAnchor_findAddress(addr)),
       		addr, filter);
}


/*		Load a document from relative name
**		---------------
**
**    On Entry,
**        relative_name     The relative address of the document to be accessed.
**
**    On Exit,
**        returns    YES     Success in opening document
**                   NO      Failure 
**
**
*/

PUBLIC BOOL HTLoadRelative ARGS1(CONST char *,relative_name)
{
    char * 		full_address = 0;
    BOOL       		result;
    char * 		mycopy = 0;
    char * 		stripped = 0;
    char *		current_address =
    				HTAnchor_address((HTAnchor*)HTMainAnchor);

    StrAllocCopy(mycopy, relative_name);

    stripped = HTStrip(mycopy);
    full_address = HTParse(stripped,
	           current_address,
		   PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
    result = HTLoadAbsolute(full_address, NO);
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
/*  if ( parent != HTMainAnchor) {    before If not already loaded */
        BOOL result;
        char * address = HTAnchor_address((HTAnchor*) parent);
	result = HTLoadDocument(parent, address, NO);
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
**	HTMainAnchor	global must be valid.
*/

PUBLIC BOOL HTSearch ARGS1(char *, keywords)

{
    char * p;	          /* pointer to first non-blank */
    char * q, *s;
    char * address = HTAnchor_address((HTAnchor*)HTMainAnchor);
    BOOL result;
    
    p = HTStrip(keywords);
    for (q=p; *q; q++)
        if (WHITE(*q)) {
	    *q = '+';
	}

    s=strchr(address, '?');		/* Find old search string */
    if (s) *s = 0;			        /* Chop old search off */

    StrAllocCat(address, "?");
    StrAllocCat(address, p);

    result = HTLoadRelative(address);
    free(address);
    return result;
    
}


