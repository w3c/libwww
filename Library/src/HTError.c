/*								      HTError.c
**	ERROR REPORT MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This is the implementaion of an error message reporting system that 
**	reports errors occured either in a stream module (structured streams
**	inclusive) or in a protocol module. A list of errors are put into the
**	a list which can be bound to a request object or a stream
**
** History:
**  	05 May 94	Written by Henrik Frystyk, frystyk@w3.org
**	   Nov 95	Made list as basic data structure
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTTCP.h"
#include "HTList.h"
#include "HTError.h"					 /* Implemented here */

PRIVATE unsigned int HTShowMask = HT_ERR_SHOW_DEFAULT;
PRIVATE char *HTErrorPrefix = NULL;

typedef struct _HTErrorMessage {
    int  	code;           	/* Error number */
    char *	msg;          		/* Short explanation */
    char *	url;			/* Explaning URL */
} HTErrorMessage;

struct _HTError {
    HTErrorElement 	element;        /* Index number into HTError */
    HTSeverity 		severity; 	/* A la VMS */
    BOOL               	ignore;         /* YES if msg should not go to user */
    void *  		par;          	/* Explanation, e.g. filename  */
    int 		length;   	/* For copying by generic routine */
    char *       	where;          /* Which function */
};

/* ------------------------------------------------------------------------- */

/*
** All errors that are not strictly HTTP errors but originates from, e.g., 
** the FTP protocol all have element numbers > HTERR_HTTP_CODES_END, i.e.,
** they should be placed after the blank line
*/
PRIVATE HTErrorMessage HTErrors[HTERR_ELEMENTS] = {
    { 200, "OK", 					"ok.multi" },
    { 201, "Created", 					"created.multi" },
    { 202, "Accepted", 					"accepted.multi" },
    { 203, "Provisional Information", 			"partial.multi" },
    { 204, "No Content",				"no_response.multi" },
    { 301, "Moved Permanently",				"moved.multi" },
    { 302, "Moved Temporarily", 			"moved.multi" },
    { 303, "Method", 					"method.multi" },
    { 304, "Not Modified",       			"not_modified.multi" },
    { 400, "Bad Request", 				"bad_request.multi" },
    { 401, "Unauthorized",				"unauthorized.multi" },
    { 402, "Payment Required", 				"payment.multi" },
    { 403, "Forbidden", 				"forbidden.multi" },
    { 404, "Not Found",		       			"not_found.multi" },
    { 405, "Method Not Allowed",	 		"method.multi" },
    { 406, "None Acceptable",		 		"none.multi" },
    { 407, "Proxy Authentication Required", 		"proxy.multi" },
    { 408, "Request Timeout",		 		"timeout.multi" },
    { 500, "Internal Server Error",			"internal.multi" },
    { 501, "Not Implemented", 				"not_made.multi" },
    { 502, "Bad Gateway", 				"bad_gate.multi" },
    { 503, "Service Unavailable",			"unavailable.multi" },
    { 504, "Gateway Timeout", 				"timeout.multi" },
    { 0,   "-------------------------", "----------------------------------" },
    { 0,   "Can't locate remote host", 			"host.multi" },
    { 0,   "No host name found", 			"host.multi" },
    { 0,   "FTP-server replies", 			"ftp.multi" },
    { 0,   "FTP-server doesn't reply", 			"no_server.multi" },
    { 0,   "Server timed out", 				"time_out.multi" },
    { 0,   "Gopher-server replies", 			"gopher.multi" },
    { 0,   "Data transfer interrupted", 		"interrupt.multi" },
    { 0,   "Connection establishment interrupted", 	"interrupt.multi" },
    { 0,   "CSO-server replies", 			"cso.multi" },
    { 0,   "This is probably a HTTP server 0.9 or less","http_version" },
    { 0,   "Bad, Incomplete, or Unknown Response",	"bad_reply.multi" },
    { 0,   "Unknown access authentication scheme",	"control.multi" },
    { 0,   "News-server replies",			"news.multi" },
    { 0,   "Trying `ftp://' instead of `file://'",	"ftpfile.multi" },
    { 0,   "Too many redirections",			"redirections.multi" },
    { 0,   "Premature End Of File",			"EOF.multi" },
    { 0,   "Response from WAIS Server too Large - Extra lines ignored",
	                                                "wais.multi" },
    { 0,   "WAIS-server doesn't return any data", 	"wais.multi" },
    { 0,   "Can't connect to WAIS-server",		"wais.multi" },
    { 0,   "System replies",				"system.multi" },
    { 0,   "Wrong Protocol Class",			"class.multi" }
};

/* ------------------------------------------------------------------------- */

/*	HTError_add
**	-----------
**	Add an error message to the error list. `par' and `where'
**	might be set to NULL. If par is a string, it is sufficient to let
**	length be unspecified, i.e., 0. If only a part of the string is
**	wanted then specify a length inferior to strlen((char *) par).
**	The string is '\0' terminated automaticly.
**	NOTE: See also HTError_addSystem for system errors
**	Return YES if OK, else NO
*/
PUBLIC BOOL HTError_add (HTList * 	list,
			 HTSeverity	severity,
			 BOOL		ignore,
			 int		element,
			 void *		par,
			 unsigned int	length,
			 char *		where)
{
    HTError *newError;
    if (!list) return NO;
    if ((newError = (HTError *) calloc(1, sizeof(HTError))) == NULL)
	outofmem(__FILE__, "HTError_add");
    newError->element = (HTErrorElement) element;
    newError->severity = severity;
    newError->ignore = ignore;
    if (par) {
	if (!length) length = (int) strlen((char *) par);
	if ((newError->par = malloc(length+1)) == NULL)
	    outofmem(__FILE__, "HTErrorError");
	memcpy(newError->par, par, length);
	*(((char *) newError->par)+length) = '\0';
	newError->length = length;
    }
    newError->where = where;
    if (WWWTRACE) {
	TTYPrint(TDEST, "Error Add... Code: %3d\tMessage: `%s\'\tSeverity: %d\tParameter: `%s\'\tWhere: `%s\'\n",
		HTErrors[newError->element].code,
		HTErrors[newError->element].msg,
		newError->severity,
		newError->par ? (char *) newError->par : "Unspecified",
		newError->where ? newError->where : "Unspecified");
    }
    return HTList_addObject(list, (void *) newError);
}


/*	HTError_addSystem
**	-----------------
**	Add a system error message to the error list. syscall
**	is the name of the system call, e.g. "close". The message put to the
**	list is that corresponds to the error number passed. 
**	Returns YES if OK, else NO
**	See also HTError_add.
*/
PUBLIC BOOL HTError_addSystem (HTList *		list,
			       HTSeverity	severity,
			       int		errornumber,
			       BOOL		ignore,
			       char *		syscall)
{
    BOOL status = NO;
    if (list && syscall) {
	char *errmsg = NULL;
	StrAllocCopy(errmsg, syscall);
	StrAllocCat(errmsg, ": ");
	StrAllocCat(errmsg, HTErrnoString(errornumber));
	status = HTError_add(list, severity, ignore, HTERR_SYSTEM,
			     errmsg, (int) strlen(errmsg), syscall);
	free(errmsg);
    }
    return status;
}

/*	HTError_deleteAll
**	-----------------
**	Remove all errors from the list
*/
PUBLIC BOOL HTError_deleteAll (HTList * list)
{
    if (list) {
	HTList *cur = list;
	HTError *pres;
	while ((pres = (HTError *) HTList_nextObject(cur))) {
	    FREE(pres->par);
	    free(pres);
	}
	HTList_delete(list);
	return YES;
    }
    return NO;
}

/*	HTError_deleteLast
**	------------------
**	Deletes the last error entry added to the list
**	Return YES if OK, else NO
*/
PUBLIC BOOL HTError_deleteLast (HTList * list)
{
    if (list) {
	HTError *old = HTList_removeLastObject(list);
	if (old) {
	    if (WWWTRACE) TTYPrint(TDEST, "Error.Delete %p\n", old);
	    FREE(old->par);
	    free(old);
	    return YES;
	}
    }
    return NO;
}

/*	HTError_ignoreLast
**	------------------
**	Turns on the `ignore' flag for the most recent error entered the
**	error list. Returns YES if OK else NO
*/
PUBLIC BOOL HTError_ignoreLast (HTList * list)
{
    if (list) {
	HTError *last = HTList_lastObject(list);
	if (last) {
	    if (WWWTRACE) TTYPrint(TDEST, "Error.Ignore %p\n", last);
	    last->ignore = YES;
	    return YES;
	}
    }
    return NO;
}

/*	HTError_setIgnore
**	-----------------
*/
PUBLIC BOOL HTError_setIgnore (HTError * info)
{
    if (info) {
	info->ignore = YES;
	return YES;
    }
    return NO;
}

/*
**	Get show preferences
*/
PUBLIC HTErrorShow HTError_show (void)
{
    return HTShowMask;
}

PUBLIC BOOL HTError_setShow (HTErrorShow mask)
{
    HTShowMask = mask;
    return YES;
}

/*
**	Should we show this error entry?
*/
PUBLIC BOOL HTError_doShow (HTError *info)
{
    return (info && ((HTShowMask & info->severity) &&
		     (!info->ignore || HTShowMask & HT_ERR_SHOW_IGNORE)));
}

/*
**	Various Fields in the HTError structure
*/
PUBLIC HTSeverity HTError_severity (HTError *info)
{
    return info ? info->severity : 0;
}

PUBLIC int HTError_code (HTError * info)
{
    return info ? HTErrors[info->element].code : -1;
}

PUBLIC CONST char * HTError_message (HTError * info)
{
    return info ? HTErrors[info->element].msg : NULL;
}

PUBLIC void * HTError_parameter (HTError * info, int * length)
{
    if (info) {
	*length = info->length;
	return info->par;
    }
    return NULL;
}

PUBLIC CONST char * HTError_location (HTError * info)
{
    return info ? info->where : NULL;
}

/*	Error URLs
**	----------
**	Each error message can point to a URL with extended description of
**	what went wrong. This can for example also be used by a server in
**	order to send back a error URL to the client. The file names of the
**	URLs are given in the error message table but the prefix where to find
**	them can be handled with the following functions
*/
PUBLIC void HTError_setPrefix (CONST char * path)
{
    if (path && *path) StrAllocCopy(HTErrorPrefix, path);
}

PUBLIC CONST char *HTError_prefix (void)
{
    return HTErrorPrefix;
}

PUBLIC void HTError_freePrefix (void)
{
    FREE(HTErrorPrefix);
}
