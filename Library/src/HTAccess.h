/*		Access Manager					HTAccess.h
**		==============
**
**	This module keeps a list of valid protocol (naming scheme) specifiers
**	with associated access code.  It allows documents to be loaded given
**	various combinations of parameters.
**
**	New access protocols may be registered at any time.
**
*/

#ifndef HTACCESS_H
#define HTACCESS_H

/*	Definition uses:
*/
#include "HTUtils.h"
#include "tcp.h"
#include "HTAnchor.h"
#include "HTFormat.h"

#ifdef SHORT_NAMES
#define HTClientHost HTClHost
#endif

/*	Return codes from load routines:
**
**	These codes may be returned by the protocol modules,
**	and by the HTLoad routines.
**	In general, positive codes are OK and negative ones are bad.
*/

#define HT_NO_DATA -9999	/* return code: OK but no data was loaded */
				/* Typically, other app started or forked */

/* Default Addresses */
/* ================= */

#define LOGICAL_DEFAULT "WWW_HOME"  /* Defined to be the home page */


#ifndef LOCAL_DEFAULT
#define LOCAL_DEFAULT  "file:/usr/local/lib/WWW/default.html"
#define LOCAL_DEFAULT_FILE "/usr/local/lib/WWW/default.html"
#define REMOTE_ADDRESS  "http://info.cern.ch/remote.html"  /* can't be file */
#endif

/* If run from telnet daemon and no -l specified, use this file:
*/
#ifndef DEFAULT_LOGFILE
#define DEFAULT_LOGFILE	"/usr/adm/www-log/www-log"
#endif

/*	If the home page isn't found, use this file:
*/
#ifndef LAST_RESORT
#define LAST_RESORT	"http://info.cern.ch:80/default.html"
#endif


/*	Flags which may be set to control this module
*/
extern int HTDiag;			/* Flag: load source as plain text */
extern char * HTClientHost;		/* Name or number of telnetting host */
extern FILE * logfile;			/* File to output one-liners to */
extern HTStream* HTOutputStream;	/* For non-interactive, set this */ 
extern HTFormat HTOutputFormat;		/* To convert on load, set this */


/*		Load a document
**		---------------
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

#ifdef NOT_USED
extern BOOL HTLoadDocument PARAMS((HTParentAnchor * anchor,
	CONST char * full_address,
	BOOL		filter,
	HTStream*	sink));
#endif


/*		Load a document from relative name
**		---------------
**
**    On Entry,
**        relative_name     The relative address of the file to be accessed.
**	  here 		    The anchor of the object being searched
**
**    On Exit,
**        returns    YES     Success in opening file
**                   NO      Failure 
**
**
*/

extern  BOOL HTLoadRelative PARAMS((
		CONST char * 		relative_name,
		HTParentAnchor *	here));


/*		Load a document from absolute name
**		---------------
**
**    On Entry,
**        addr       The absolute address of the document to be accessed.
**        filter     if YES, treat document as HTML
**
**    On Exit,
**        returns    YES     Success in opening document
**                   NO      Failure 
**
**    Note: This is equivalent to HTLoadDocument
*/

extern BOOL HTLoadAbsolute PARAMS((CONST char * addr));


/*		Load a document from absolute name to a stream
**		---------------
**
**    On Entry,
**        addr       The absolute address of the document to be accessed.
**        filter     if YES, treat document as HTML
**
**    On Exit,
**        returns    YES     Success in opening document
**                   NO      Failure 
**
**    Note: This is equivalent to HTLoadDocument
*/

extern BOOL HTLoadToStream PARAMS((CONST char * addr, BOOL filter,
				HTStream * sink));


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

extern BOOL HTLoadAnchor PARAMS((HTAnchor * destination));


/*		Make a stream for Saving object back
**		------------------------------------
**
**	On Entry,
**		anchor	is valid anchor which has previously beeing loaded
**	On exit,
**		returnes 0 if error else a stream to save the object to.
**
*/
extern HTStream * HTSaveStream PARAMS((HTParentAnchor * anchor));


/*		Search
**		------
**  Performs a search on word given by the user. Adds the search words to 
**  the end of the current address and attempts to open the new address.
**
**  On Entry,
**       *keywords  	space-separated keyword list or similar search list
**	  here 		The anchor of the object being searched
*/

extern BOOL HTSearch PARAMS((CONST char * keywords, HTParentAnchor* here));


/*		Search Given Indexname
**		------
**  Performs a keyword search on word given by the user. Adds the keyword to 
**  the end of the current address and attempts to open the new address.
**
**  On Entry,
**       *keywords  	space-separated keyword list or similar search list
**	*addres		is name of object search is to be done on.
*/

extern BOOL HTSearchAbsolute PARAMS((
	CONST char * 	keywords,
	CONST char * 	indexname));


/*		Register an access method
**		-------------------------
*/

typedef struct _HTProtocol {
	char * name;
	
	int (*load)PARAMS((
		CONST char * 	full_address,
		HTParentAnchor * anchor,
		HTFormat	format_out,
		HTStream*	sink));
		
	HTStream* (*saveStream)PARAMS((HTParentAnchor * anchor));

} HTProtocol;

extern BOOL HTRegisterProtocol PARAMS((HTProtocol * protocol));


/*		Generate the anchor for the home page
**		-------------------------------------
**
**	As it involves file access, this should only be done once
**	when the program first runs.
**	This is a default algorithm -- browser don't HAVE to use this.
**
*/
extern HTParentAnchor * HTHomeAnchor NOPARAMS;

#endif /* HTACCESS_H */
