/*			Error Report Module
**			===================
**
**	This is the implementaion of an error message reporting system that 
**	reports errors occured either in a stream module (structured streams
**	inclusive) or in a protocol module. A list of errors are put into the
**	request structure and freed together with this.
**
** History:
**  	05 May 94	Written by Henrik Frystyk, frystyk@dxcern.cern.ch
*/

/* Implemention dependent include files */
#include "tcp.h"

/* Library include files */
#include "HTUtils.h"
#include "HTAccess.h"
#include "HTTCP.h"
#include "HTError.h"					 /* Implemented here */

/* Globals */
PUBLIC unsigned int HTErrorShowMask = HT_ERR_SHOW_WARNING+HT_ERR_SHOW_PARS;

/* Type definitions and global variables etc. local to this module */

/* All errors that are not strictly HTTP errors but originates from, e.g., 
   the FTP protocol all have element numbers > HTERR_HTTP_CODES_END, i.e.,
   they should be placed after the blank line

   NOTE: codes marked `HTTP' should be future HTTP error codes.
*/
PUBLIC HTErrorMsgInfo error_info[HTERR_ELEMENTS] = {
    { 200, "OK", 			"ok.multi" },
    { 201, "Created", 			"created.multi" },
    { 202, "Accepted", 			"accepted.multi" },
    { 203, "Partial Information", 	"partial.multi" },
    { 204, "No Response",		"no_response.multi" },
    { 301, "Moved", 			"moved.multi" },
    { 302, "Found", 			"found.multi" },
    { 303, "Method", 			"method.multi" },
    { 304, "Not Modified Since",       	"not_modified.multi" },
    { 400, "Invalid Request", 		"bad_request.multi" },
    { 401, "Unauthorized Access Denied","unauthorized.multi" },
    { 402, "Payment Required", 		"payment.multi" },
    { 403, "Access forbidden by rule", 	"forbidden.multi" },
    { 404, "No Match Found for",       	"not_found.multi" },
    { 500, "Can't Access Document",	"internal.multi" },
    { 501, "Command not Implemented", 	"not_implemented.multi" },
    { 0,   "-------------------------", "-----------------------" },
    { 0,   "Can't locate remote host", 	"locate_host.multi" },
    { 0,   "FTP-server replies", 	"ftp.multi" },
    { 0,   "FTP-server doesn't reply", 	"no_server.multi" },
    { 0,   "Server timed out", 		"time_out.multi" },
    { 0,   "Gopher-server replies", 	"gopher.multi" },
    { 0,   "Data transfer interrupted", "interrupt.multi" },
    { 0,   "CSO-server replies", 	"cso.multi" },
    { 0,   "Bad or Incomplete Response","bad_reply.multi" },	     /* HTTP */
    { 0,   "News-server replies",	"news.multi" },
    { 0,   "System call `%s' failed: ",	"system.multi" }
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
**	Returns always < 0
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
	if (TRACE) fprintf(stderr, "HTErrorAdd.. Bad argument!\n");
	return -1;
    }
    if ((newError = (HTErrorInfo *) calloc(1, sizeof(HTErrorInfo))) == NULL)
	outofmem(__FILE__, "HTErrorAdd");
    newError->element = element;
    newError->severity = severity;
    newError->ignore = ignore;
    if (par) {
	if (!par_length)
	    par_length = (int) strlen((char *) par);
	if ((newError->par = malloc(par_length+1)) == NULL)
	    outofmem(__FILE__, "HTAddError");
	memcpy(newError->par, par, par_length);
	*(((char *) newError->par)+par_length) = '\0';
	newError->par_length = par_length;
    }
    newError->where = where;
    if (TRACE) {
	fprintf(stderr, "Error....... Code: %3d\tMessage: `%s\tSeverity: %d\tParameter: `%s\'\tWhere: `%s\'\n",
		error_info[newError->element].code,
		error_info[newError->element].msg,
		newError->severity,
		newError->par ? (char *) newError->par : "Unspecified",
		newError->where ? newError->where : "Unspecified");
    }

    /* Add to the stack in the request structure */
    if (!request->error_stack)
	request->error_stack = HTList_new();
    HTList_addObject(request->error_stack, (void *) newError);
    return (-element);
}


/*								HTErrorSysAdd
**
**	Add a system error message to the error list in HTRequest. syscall
**	is the name of the system call, e.g. "close". The message put to the
**	list is that corresponds to the errno. This function also replaces
**	HTInetStatus, which is called from within.
**
**	See also HTErrorAdd.
**
**	Returns always < 0
*/
PUBLIC int HTErrorSysAdd ARGS4(HTRequest *, 	request,
			       HTErrSeverity, 	severity,
			       BOOL,		ignore,
			       char *,		syscall)

{
    HTErrorInfo *newError;
    if (!request) {
	if (TRACE) fprintf(stderr, "HTErrorSys.. Bad argument!\n");
	return -1;
    }
    if ((newError = (HTErrorInfo *) calloc(1, sizeof(HTErrorInfo))) == NULL)
	outofmem(__FILE__, "HTErrorSysAdd");
    newError->element = HTERR_SYSTEM;
    newError->severity = severity;
    newError->ignore = ignore;
    if (syscall) {
	newError->where = syscall;
	HTInetStatus(syscall);
    } else
	HTInetStatus("Unspecified System Call");
    {
	char temp[100];
	char *errmsg = NULL;
	sprintf(temp, error_info[HTERR_SYSTEM].msg, syscall);
	StrAllocCopy(errmsg, temp);
	StrAllocCat(errmsg, HTErrnoString());
	newError->par = (void *) errmsg;
    }
    newError->par_length = (int) strlen(newError->par);
    if (TRACE) {
	fprintf(stderr, "System Error Code: %3d\tMessage: `%s\tSeverity: %d\tParameter: `%s\'\tWhere: `%s\'\n",
		error_info[newError->element].code,
		error_info[newError->element].msg,
		newError->severity,
		(char *) newError->par,
		newError->where ? newError->where : "Unspecified");
    }

    /* Add to the stack in the request structure */
    if (!request->error_stack)
	request->error_stack = HTList_new();
    HTList_addObject(request->error_stack, (void *) newError);
    return (-HTERR_SYSTEM);
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
**	Turns on the `ignore' flag for the most recent error entered the
**	error list. If the list is empty, nothing is done.
*/
PUBLIC void HTErrorIgnore ARGS1(HTRequest *, request)
{
    HTList *cur;
    HTErrorInfo *pres;
    if (!request) {
	if (TRACE) fprintf(stderr, "HTErrorIgnore Bad argument!\n");
	return;
    }
    cur = request->error_stack;
    if (cur && (pres = (HTErrorInfo *) HTList_nextObject(cur)) != NULL) {
	if (TRACE)
	    fprintf(stderr, "Error Ignore Code: %3d\tMessage: `%s\tSeverity: %d\tParameter: `%s\'\tWhere: `%s\'\n",
		    error_info[pres->element].code,
		    error_info[pres->element].msg,
		    pres->severity,
		    pres->par ? (char *) pres->par : "Unspecified",
		    pres->where ? pres->where : "Unspecified");
	pres->ignore = YES;
    }
    return;
}


/*								HTErrorMsg
**
**	Creates an error message on standard output containing the 
**	error_stack messages. The HTErr 
**	Only if the global variable HTErrorInfoPath != NULL, an anchor
**	will be created to an message help file. It is garanteed that
**	NO STREAM has been put up or taken down in the library at this point.
**	This function might be overwritten by a smart server or client.
*/

/* *** LOOK IN HTML.C FOR ACTUAL IMPLEMENTATION OF THIS FUNCTION *** */

/* END OF MODULE */





