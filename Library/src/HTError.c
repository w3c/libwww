/*								      HTError.c
**	ERROR REPORT MODULE
**
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This is the implementaion of an error message reporting system that 
**	reports errors occured either in a stream module (structured streams
**	inclusive) or in a protocol module. A list of errors are put into the
**	request structure and freed together with this.
**
** History:
**  	05 May 94	Written by Henrik Frystyk, frystyk@dxcern.cern.ch
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTTCP.h"
#include "HTError.h"					 /* Implemented here */

/* Globals */
PUBLIC unsigned int HTErrorShowMask = HT_ERR_SHOW_DEFAULT;
PRIVATE char *HTErrorPrefix = NULL;

/* Type definitions and global variables etc. local to this module */

/* All errors that are not strictly HTTP errors but originates from, e.g., 
   the FTP protocol all have element numbers > HTERR_HTTP_CODES_END, i.e.,
   they should be placed after the blank line

   NOTE: codes marked `HTTP' should be future HTTP error codes.
*/
PUBLIC HTErrorMsgInfo error_info[HTERR_ELEMENTS] = {
    { 200, "OK", 					"ok.multi" },
    { 201, "Created", 					"created.multi" },
    { 202, "Accepted", 					"accepted.multi" },
    { 203, "Provisional Information", 			"partial.multi" },
    { 204, "No Response",				"no_response.multi" },
    { 205, "Deleted",					"deleted.multi" },
    { 206, "Modified",					"modified.multi" },
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
    { 0,   "Trying `ftp://' instead of `file://'. ANY OLD URL STILL USING WRONG ACCESS METHOD WILL BE OBSOLETE IN THE NEXT MAJOR RELEASE!",
	                                                "ftpfile.multi" },
    { 0,   "Too many redirections",			"redirections.multi" },
    { 0,   "Premature End Of File",			"EOF.multi" },
    { 0,   "Response from WAIS Server too Large - Extra lines ignored",
	                                                "wais.multi" },
    { 0,   "WAIS-server doesn't return any data", 	"wais.multi" },
    { 0,   "Can't connect to WAIS-server",		"wais.multi" },
    { 0,   "System call `%s' failed: ",			"system.multi" }
};

/* ------------------------------------------------------------------------- */

/*								HTErrorAdd
**
**	Add an error message to the error list in HTRequest. `par' and `where'
**	might be set to NULL. If par is a string, it is sufficient to let
**	par_length be unspecified, i.e., 0. If only a part of the string is
**	wanted then specify a par_length inferior to strlen((char *) par).
**	The string is '\0' terminated automaticly.
**
**	NOTE: See also HTErrorSysAdd for system errors
**
**	Returns always HT_ERROR
*/
PUBLIC int HTErrorAdd ARGS7(HTRequest *, 	request,
			    HTErrSeverity, 	severity,
			    BOOL,		ignore,
			    int,		element,
			    void *,		par,
			    unsigned int,	par_length,
			    char *,		where)
{
    HTErrorInfo *newError;
    if (!request) {
	if (TRACE) fprintf(TDEST, "HTErrorAdd.. Bad argument!\n");
	return HT_ERROR;
    }
    if ((newError = (HTErrorInfo *) calloc(1, sizeof(HTErrorInfo))) == NULL)
	outofmem(__FILE__, "HTErrorAdd");
    newError->element = (HTErrorElement) element;
    newError->severity = severity;
    newError->ignore = ignore;
    if (par) {
	if (!par_length)
	    par_length = (int) strlen((char *) par);
	if ((newError->par = malloc(par_length+1)) == NULL)
	    outofmem(__FILE__, "HTErrorError");
	memcpy(newError->par, par, par_length);
	*(((char *) newError->par)+par_length) = '\0';
	newError->par_length = par_length;
    }
    newError->where = where;

    /* Add to the stack in the request structure */
    if (!request->error_stack)
	request->error_stack = HTList_new();
    else {			/* Get last object in order to find a handle */
	HTList *cur = request->error_stack;
	HTErrorInfo *pres = (HTErrorInfo *) HTList_nextObject(cur);
	if (pres != NULL)
	    newError->handle = pres->handle+1;
    }
    if (TRACE) {
	fprintf(TDEST, "Message..... Handle: %d\tCode: %3d\tMessage: `%s\'\tSeverity: %d\tParameter: `%s\'\tWhere: `%s\'\n",
		newError->handle,
		error_info[newError->element].code,
		error_info[newError->element].msg,
		newError->severity,
		newError->par ? (char *) newError->par : "Unspecified",
		newError->where ? newError->where : "Unspecified");
    }
    HTList_addObject(request->error_stack, (void *) newError);
    return HT_ERROR;
}


/*								HTErrorSysAdd
**
**	Add a system error message to the error list in HTRequest. syscall
**	is the name of the system call, e.g. "close". The message put to the
**	list is that corresponds to the error number passed. 
**
**	See also HTErrorAdd.
**
**	Returns always HT_ERROR
*/
PUBLIC int HTErrorSysAdd ARGS5(HTRequest *, 	request,
			       HTErrSeverity, 	severity,
			       int,		errornumber,
			       BOOL,		ignore,
			       char *,		syscall)
{
    if (!request) {
	if (TRACE) fprintf(TDEST, "HTErrorSys.. Bad argument!\n");
	return HT_ERROR;
    }
    {
	char temp[100];
	char *errmsg = NULL;
	sprintf(temp, error_info[HTERR_SYSTEM].msg, syscall);
	StrAllocCopy(errmsg, temp);
	StrAllocCat(errmsg, HTErrnoString(errornumber));
	HTErrorAdd(request, severity, ignore, HTERR_SYSTEM, (void *) errmsg,
		   (int) strlen(errmsg), syscall);
	free(errmsg);
    }
    return HT_ERROR;
}


/*								HTErrorFree
**
**	Free the whole error stack from the HTRequest structure.
*/
PUBLIC void HTErrorFree ARGS1(HTRequest *, request)
{
    HTList *cur = request->error_stack;
    HTErrorInfo *pres;
    if (!request || !request->error_stack)
	return;
    while ((pres = (HTErrorInfo *) HTList_nextObject(cur))) {
	FREE(pres->par);
	free(pres);
    }
    HTList_delete(request->error_stack);
    request->error_stack = NULL;
    return;
}


/*								HTErrorIgnore
**
**	Turns on the `ignore' flag for the error with the current handle in 
**	the error list. If the list is empty, nothing is done.
*/
PUBLIC void HTErrorIgnore ARGS2(HTRequest *, request, int, handle)
{
    BOOL found = NO;
    HTList *cur;
    HTErrorInfo *pres;
    if (!request) {
	if (TRACE) fprintf(TDEST, "HTErrorIgnore Bad argument!\n");
	return;
    }
    cur = request->error_stack;
    while ((pres = (HTErrorInfo *) HTList_nextObject(cur))) {
	if (pres->handle == handle) {
	    pres->ignore = YES;
	    found = YES;
	    break;
	}
    }

    if (TRACE) {
	if (found) {
	    fprintf(TDEST, "Error Ignore Handle: %d\tCode: %3d\tMessage: `%s\tSeverity: %d\tParameter: `%s\'\tWhere: `%s\'\n",
		    pres->handle,
		    error_info[pres->element].code,
		    error_info[pres->element].msg,
		    pres->severity,
		    pres->par ? (char *) pres->par : "Unspecified",
		    pres->where ? pres->where : "Unspecified");
	} else {
	    fprintf(TDEST, "Error Ignore Bad handle\n");
	}
    }
    return;
}


/*							    HTErrorIgnoreLast
**
**	Turns on the `ignore' flag for the most recent error entered the
**	error list. If the list is empty, nothing is done.
*/
PUBLIC void HTErrorIgnoreLast ARGS1(HTRequest *, request)
{
    HTList *cur;
    HTErrorInfo *pres;
    if (!request) {
	if (TRACE) fprintf(TDEST, "HTErrorIgnore Bad argument!\n");
	return;
    }
    cur = request->error_stack;
    if (cur && (pres = (HTErrorInfo *) HTList_nextObject(cur)) != NULL) {
	if (TRACE)
	    fprintf(TDEST, "Error Ignore Code: %3d\tMessage: `%s\tSeverity: %d\tParameter: `%s\'\tWhere: `%s\'\n",
		    error_info[pres->element].code,
		    error_info[pres->element].msg,
		    pres->severity,
		    pres->par ? (char *) pres->par : "Unspecified",
		    pres->where ? pres->where : "Unspecified");
	pres->ignore = YES;
    }
    return;
}


/*							    HTErrorSetPrefix
**
**	Sets the prefix for error URLs in the error message
*/
PUBLIC void HTErrorSetPrefix ARGS1(char *, path)
{
    if (path && *path)
	StrAllocCopy(HTErrorPrefix, path);
}


/*							    HTErrorGetPrefix
**
**	Gets the prefix for error URLs in the error message
*/
PUBLIC CONST char *HTErrorGetPrefix NOARGS
{
    return HTErrorPrefix;
}


/*								HTErrorMsg
**
**	Creates an error message on standard error containing the 
**	error_stack messages.
*/

/* *** LOOK IN HTErrMsg.c FOR ACTUAL IMPLEMENTATION OF THIS FUNCTION *** */

/* END OF MODULE */





