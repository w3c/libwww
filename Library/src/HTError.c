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
#include "HTMLPDTD.h"
#include "HTUtils.h"
#include "HTAccess.h"
#include "HTML.h"
#include "HTTCP.h"
#include "HTError.h"					 /* Implemented here */

/* Macros and other defines */
#define PUTC(c) 	(*target->isa->put_character)(target, c)
#define PUTS(s)		(*target->isa->put_string)(target, s)
#define START(e)	(*target->isa->start_element)(target, e, 0, 0)
#define END(e)		(*target->isa->end_element)(target, e)
#define FREE_TARGET	(*target->isa->free)(target)

/* Globals */
PUBLIC unsigned int HTErrorShowMask = HT_ERR_SHOW_WARNING+HT_ERR_SHOW_PARS;

/* Type definitions and global variables etc. local to this module */
typedef struct _HTErrorInfo {
    HTErrorElement 	element;        /* Index number into HTErrorMsgInfo */
    HTErrSeverity 	severity; 	/* A la VMS */
    BOOL               	ignore;         /* YES if msg should not go to user */
    void *  		par;          	/* Explanation, e.g. filename  */
    unsigned int 	par_length;   	/* For copying by generic routine */
    char *       	where;          /* Which function */
} HTErrorInfo;

typedef struct _HTErrorMsgInfo {
    int  	code;           	/* Error number */
    char *	msg;          		/* Short explanation */
    char *	url;			/* Explaning URL */
} HTErrorMsgInfo;

struct _HTStructured {
    CONST HTStructuredClass *	isa;
    /* ... */
};

PRIVATE char *HTErrorInfoPath  = NULL;
PRIVATE char *HTErrorSignature = NULL;
PRIVATE char *HTErrorSigLink   = NULL;

/* All errors that are not strictly HTTP errors but originates from, e.g., 
   the FTP protocol all have element numbers > HTERR_HTTP_CODES_END, i.e.,
   they should be placed after the blank line */
PRIVATE HTErrorMsgInfo error_info[HTERR_ELEMENTS] = {
    { 200, "OK", 			"ok.multi" },
    { 201, "Created", 			"created.multi" },
    { 202, "Accepted", 			"accepted.multi" },
    { 203, "Partial Information", 	"partial.multi" },
    { 204, "No Response", 		"no_response.multi" },
    { 301, "Moved", 			"moved.multi" },
    { 302, "Found", 			"found.multi" },
    { 303, "Method", 			"method.multi" },
    { 304, "Not Modified", 		"not_modified.multi" },
    { 400, "Bad Request", 		"bad_request.multi" },
    { 401, "Unauthorized", 		"unauthorized.multi" },
    { 402, "Payment Required", 		"payment.multi" },
    { 403, "Forbidden", 		"forbidden.multi" },
    { 404, "Not Found", 		"not_found.multi" },
    { 500, "Can't access document",	"internal.multi" },
    { 501, "Not Implemented", 		"not_implemented.multi" },
    { 0,   "-------------------------", "-----------------------" },
    { 0,   "Can't locate remote host", 	"locate_host.multi" },
    { 0,   "FTP-server replies", 	"ftp.multi" },
    { 0,   "FTP-server doesn't reply", 	"no_server.multi" },
    { 0,   "Server timed out", 		"time_out.multi" },
    { 0,   "Gopher-server replies", 	"gopher.multi" },
    { 0,   "Data transfer Interrupted", "interrupt.multi" },
    { 0,   "CSO-server replies", 	"cso.multi" },
    { 0,   "Message from System Call", 	"system.multi" }
};

/* ------------------------------------------------------------------------- */

/*								HTErrorAdd
**
**	Add an error message to the error list in HTRequest. `par' and `where'
**	might be set to NULL. If par is a string, it is sufficient to let
**	par_length be unspecified, i.e., 0.
**
**	NOTE: See also HTErrorSysAdd for system errors
*/
PUBLIC void HTErrorAdd ARGS7(HTRequest *, 	request,
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
	return;
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
    return;
}


/*								HTErrorSysAdd
**
**	Add a system error message to the error list in HTRequest. syscall
**	is the name of the system call, e.g. "close". The message put to the
**	list is that corresponds to the errno. This function also replaces
**	HTInetStatus, which is called from within.
**
**	See also HTErrorAdd.
*/
PUBLIC void HTErrorSysAdd ARGS4(HTRequest *, 	request,
				HTErrSeverity, 	severity,
				BOOL,		ignore,
				char *,		syscall)

{
    HTErrorInfo *newError;
    if (!request) {
	if (TRACE) fprintf(stderr, "HTErrorSys.. Bad argument!\n");
	return;
    }
    if ((newError = (HTErrorInfo *) calloc(1, sizeof(HTErrorInfo))) == NULL)
	outofmem(__FILE__, "HTErrorAdd");
    newError->element = HTERR_SYSTEM;
    newError->severity = severity;
    newError->ignore = ignore;
    if (syscall) {
	newError->where = syscall;
	HTInetStatus(syscall);
    } else
	HTInetStatus("Unspecified System Call");
    {
	char *errmsg = NULL;
	StrAllocCopy(errmsg, HTErrnoString());
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
    return;
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
    HTList *cur = request->error_stack;
    HTErrorInfo *pres;
    if (!request) {
	if (TRACE) fprintf(stderr, "HTErrorIgnore Bad argument!\n");
	return;
    }
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


/*								HTErrorInit
**
**	Initiates the external multi linguistic help files that appears 
**	as anchors in the message, a `signature', e.g., "HTTPD Server",
**	and a link to information on the signer.
*/
PUBLIC void HTErrorInit ARGS3(char *, pathname, char *, signature,
			      char *, siglink)
{
    if (pathname) {
	if (TRACE) fprintf(stderr, "HTErrorInit. Initializing path for info on error messages\n");
	StrAllocCopy(HTErrorInfoPath, pathname);
	if (*(HTErrorInfoPath+strlen(HTErrorInfoPath)-1) != '/')
	    StrAllocCat(HTErrorInfoPath, "/");
    }
    if (signature) {
	if (TRACE) fprintf(stderr, "HTErrorInit. Initializing signature\n");
	StrAllocCopy(HTErrorSignature, signature);
	if (siglink) {
	    if (TRACE) fprintf(stderr, "HTErrorInit. Initializing info on signer\n");
	    StrAllocCopy(HTErrorSigLink, siglink);
	}
    }
    return;
}


/*								HTErrorMsg
**
**	Creates a HTML error message from using the whole error_stack.
**	Only if the global variable HTErrorInfoPath != NULL, an anchor
**	will be created to an message help file.
*/
PUBLIC void HTErrorMsg ARGS1(HTRequest *, request)
{
    HTList *cur = request->error_stack;
    BOOL highest = YES;
    HTErrorInfo *pres;
    HTStructured *target = NULL;
    if (!request) {
	if (TRACE) fprintf(stderr, "HTErrorMsg.. Bad argument!\n");
	return;
    }

    /* Output messages */
    while ((pres = (HTErrorInfo *) HTList_nextObject(cur))) {

	/* Check if we are going to show the message */
	if ((!pres->ignore || HTErrorShowMask & HT_ERR_SHOW_IGNORE) && 
	    (HTErrorShowMask & pres->severity)) {

	    /* Output code number */
	    if (highest) {			    /* If first time through */
		if (TRACE)
		    fprintf(stderr,
			    "HTError..... Generating error message.\n");
		target = HTML_new(request, NULL, WWW_HTML,
				  request->output_format,
				  request->output_stream);
		
		/* Output title */
		START(HTML_TITLE);
		PUTS("Error Message");
		END(HTML_TITLE);
		START(HTML_H1);

		if (pres->severity == ERR_WARNING)
		    PUTS("Warning ");
		else if (pres->severity == ERR_NON_FATAL)
		    PUTS("Non Fatal Error ");
		else if (pres->severity == ERR_FATAL)
		    PUTS("Fatal Error ");
		else {
		    PUTS("Unknown Classification of Error");
		    FREE_TARGET;
		    return;
		}

		/* Only output error code if it is a real HTTP code */
		if (pres->element < HTERR_HTTP_CODES_END) {
		    char codestr[20];
		    sprintf(codestr, "%d", error_info[pres->element].code);
		    PUTS(codestr);
		}
		END(HTML_H1);
		highest = NO;
	    } else {
		START(HTML_B);
		PUTS("This occurred because ");
		END(HTML_B);
	    }

	    /* Output error message */
	    PUTS(error_info[pres->element].msg);

	    /* Output parameters */
	    if (pres->par && HTErrorShowMask & HT_ERR_SHOW_PARS) {
		int cnt;
		PUTS(" (");
		for (cnt=0; cnt<pres->par_length; cnt++) {
		    if (*((char *)(pres->par)+cnt) < 0x20 ||
			*((char *)(pres->par)+cnt) >= 0x7F)
			PUTC('-');
		    else
			PUTC(*((char *)(pres->par)+cnt));
		}
		PUTC(')');
	    }

	    /* Make an anchor for further information */
	    if (HTErrorInfoPath && error_info[pres->element].url) {
		char *url = NULL;
		START(HTML_BR);
		StrAllocCopy(url, HTErrorInfoPath);
		StrAllocCat(url, error_info[pres->element].url);
		HTStartAnchor(target, NULL, url);
		PUTS("Further information");
		END(HTML_A);
		free(url);
	    }

	    /* Output location */
	    if (pres->where && HTErrorShowMask & HT_ERR_SHOW_LOCATION) {
		START(HTML_BR);
		PUTS("This occured in ");
		PUTS(pres->where);
	    }
	    
	    /* If we only are going to show the higest entry */
	    if (HTErrorShowMask & HT_ERR_SHOW_FIRST)
		break;
	    START(HTML_P);
	}
    }

    /* Put out any signature */
    if (target) {
	if (HTErrorSignature) {
	    START(HTML_ADDRESS);
	    if (HTErrorSigLink)
		HTStartAnchor(target, NULL, HTErrorSigLink);
	    PUTS(HTErrorSignature);
	    if (HTErrorSigLink)
		END(HTML_A);
	    END(HTML_ADDRESS);
	}
	FREE_TARGET;
    }
    return;
}

/* END OF MODULE */





