/*								     HTDialog.c
**	MESSAGES AND DIALOGS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This module provides a default implementation of the application part
**	of the Message and Dialog part or the Library. You do not have to do
**	it like this - you can use exactly the model for mapping codes into
**	messages as you like
**
** Authors
**	HFN	Henrik Frystyk
*/

/* Library include files */
#include "WWWLib.h"
#include "WWWApp.h"
#include "WWWHTTP.h"
#include "HTReqMan.h"
#include "HTDialog.h"					 /* Implemented here */

/*
** Dialog Messages
*/
PRIVATE char *HTDialogs[HT_MSG_ELEMENTS] = {
    "Please enter username:",
    "Password:",
    "Please give name of file to save in:",
    "Plase enter account:",
    "It might not be allowed to use this method, continue?",
    "Location has moved, continue?",
    "A new set of rules is to be added to your setup - continue?"
};

/*
** All errors that are not strictly HTTP errors but originates from, e.g., 
** the FTP protocol all have element numbers > HTERR_HTTP_CODES_END, i.e.,
** they should be placed after the blank line
*/
typedef struct _HTErrorMessage {
    int  	code;           	/* Error number */
    char *	msg;          		/* Short explanation */
    char *	url;			/* Explaning URL */
} HTErrorMessage;

PRIVATE HTErrorMessage HTErrors[HTERR_ELEMENTS] = {

/*    CODE  ERROR MESSAGE				ERROR URL */

    { 200, "OK", 					"ok" },
    { 201, "Created", 					"created" },
    { 202, "Accepted", 					"accepted" },
    { 203, "Provisional Information", 			"partial" },
    { 204, "No Content",				"no_response" },
    { 301, "Moved Permanently",				"moved" },
    { 302, "Moved Temporarily", 			"moved" },
    { 303, "Method", 					"method" },
    { 304, "Not Modified",       			"not_modified" },
    { 400, "Bad Request", 				"bad_request" },
    { 401, "Unauthorized",				"unauthorized" },
    { 402, "Payment Required", 				"payment" },
    { 403, "Forbidden", 				"forbidden" },
    { 404, "Not Found",		       			"not_found" },
    { 405, "Method Not Allowed",	 		"method" },
    { 406, "None Acceptable",		 		"none" },
    { 407, "Proxy Authentication Required", 		"proxy" },
    { 408, "Request Timeout",		 		"timeout" },
    { 500, "Internal Server Error",			"internal" },
    { 501, "Not Implemented", 				"not_made" },
    { 502, "Bad Gateway", 				"bad_gate" },
    { 503, "Service Unavailable",			"unavailable" },
    { 504, "Gateway Timeout", 				"timeout" },
    { 0,   "-------------------------", "----------------------------------" },
    { 0,   "Can't locate remote host", 			"host" },
    { 0,   "No host name found", 			"host" },
    { 0,   "No file name found or file not accessible", "file" },
    { 0,   "FTP-server replies", 			"ftp" },
    { 0,   "FTP-server doesn't reply", 			"no_server" },
    { 0,   "Server timed out", 				"time_out" },
    { 0,   "Gopher-server replies", 			"gopher" },
    { 0,   "Data transfer interrupted", 		"interrupt" },
    { 0,   "Connection establishment interrupted", 	"interrupt" },
    { 0,   "CSO-server replies", 			"cso" },
    { 0,   "This is probably a HTTP server 0.9 or less","http_version" },
    { 0,   "Bad, Incomplete, or Unknown Response",	"bad_reply" },
    { 0,   "Unknown access authentication scheme",	"control" },
    { 0,   "News-server replies",			"news" },
    { 0,   "Trying `ftp://' instead of `file://'",	"ftpfile" },
    { 0,   "Too many redirections",			"redirections" },
    { 0,   "Premature End Of File",			"EOF" },
    { 0,   "Response from WAIS Server too Large - Extra lines ignored",
	                                                "wais" },
    { 0,   "WAIS-server doesn't return any data", 	"wais" },
    { 0,   "Can't connect to WAIS-server",		"wais" },
    { 0,   "System replies",				"system" },
    { 0,   "Wrong or unknown access scheme",		"class" },
    { 0,   "Access scheme not allowed in this context",	"class" },
    { 0,   "When you are connected, you can log in",	"telnet" }
};

/* ------------------------------------------------------------------------- */

PUBLIC BOOL HTProgress (HTRequest * request, HTAlertOpcode op,
			int msgnum, const char * dfault, void * input,
			HTAlertPar * reply)
{
    if (!request) {
	if (WWWTRACE) HTTrace("HTProgress.. Bad argument\n");
	return NO;
    }
    switch (op) {
      case HT_PROG_DNS:
	HTTrace("Looking up %s\n", input ? (char *) input : "");
	break;

      case HT_PROG_CONNECT:
	HTTrace("Contacting %s\n", input ? (char *) input : "");
	break;

      case HT_PROG_ACCEPT:
	HTTrace("Waiting for connection...\n");
	break;

      case HT_PROG_READ:
	{
	    long cl = HTAnchor_length(HTRequest_anchor(request));
	    if (cl > 0) {
		long b_read = HTRequest_bytesRead(request);
		double pro = (double) b_read/cl*100;
		char buf[10];
		HTNumToStr((unsigned long) cl, buf, 10);
		HTTrace("Read (%d%% of %s)\n", (int) pro, buf);
	    } else
		HTTrace("Reading...\n");
	}
	break;

      case HT_PROG_WRITE:
	if (HTRequest_isPostWeb(request)) {
	    HTParentAnchor *anchor=HTRequest_anchor(HTRequest_source(request));
	    long cl = HTAnchor_length(anchor);
	    if (cl > 0) {
		long b_write = HTRequest_bytesWritten(request);
		double pro = (double) b_write/cl*100;
		char buf[10];
		HTNumToStr((unsigned long) cl, buf, 10);
		HTTrace("Written (%d%% of %s)\n", (int) pro, buf);
	    } else
		HTTrace("Writing...\n");
	}
	break;

      case HT_PROG_DONE:
	HTTrace("Finished\n");
	break;

      case HT_PROG_WAIT:
	HTTrace("Waiting for HT_FREE socket...\n");
	break;

      default:
	HTTrace("UNKNOWN PROGRESS STATE\n");
	break;
    }
    return YES;
}

PUBLIC BOOL HTConfirm (HTRequest * request, HTAlertOpcode op,
		       int msgnum, const char * dfault, void * input,
		       HTAlertPar * reply)
{
    char response[4];	/* One more for terminating NULL -- AL */
    HTTrace("%s", HTDialogs[msgnum]);
    if (input) HTTrace(" (%s)", (char *) input);
    HTTrace(" (y/n) ");
#ifndef NO_STDIO
    if (fgets(response, 4, stdin)) 		   /* get reply, max 3 chars */
#endif
    {
	char *ptr = response;
	while (*ptr) {
	    if (*ptr == '\n') {
		*ptr = '\0';
		break;
	    }
	    *ptr = TOUPPER(*ptr);
	    ptr++;
	}
	return (!strcmp(response, "YES") || !strcmp(response, "Y")) ? YES : NO;
    }
    return NO;
}

/*	Prompt for answer and get text back. Reply text is either NULL on
**	error or a dynamic string which the caller must HT_FREE.
*/
PUBLIC BOOL HTPrompt (HTRequest * request, HTAlertOpcode op,
		      int msgnum, const char * dfault, void * input,
		      HTAlertPar * reply)
{
    HTTrace("%s ", HTDialogs[msgnum]);
    if (input) HTTrace(" (%s) ", (char *) input);
    if (dfault) HTTrace("(RETURN for [%s]) ", (char *) dfault);
    if (reply && msgnum>=0) {
#ifndef NO_STDIO
        char buffer[200];
	if (!fgets(buffer, 200, stdin)) return NO;
	buffer[strlen(buffer)-1] = '\0';	        /* Overwrite newline */
	if (*buffer)
	    HTAlert_setReplyMessage(reply, buffer);
	else if (dfault)
	    HTAlert_setReplyMessage(reply, (char *) dfault);
	else
	    return NO;
	return YES;
#endif
    }
    return NO;
}

/*	Prompt for password without echoing the reply. Reply text is
**	either NULL on error or a dynamic string which the caller must HT_FREE.
*/
PUBLIC BOOL HTPromptPassword (HTRequest * request, HTAlertOpcode op,
			      int msgnum, const char * dfault, void * input,
			      HTAlertPar * reply)
{
    if (reply && msgnum>=0) {
#ifdef HAVE_GETPASS
	char * pw = (char *) getpass(HTDialogs[msgnum]);
	if (pw) HTAlert_setReplySecret(reply, pw);
	return YES;
#else
	return NO;	/* needed for WWW_MSWINDOWS */
#endif /* HAVE_GETPASS */
    }
    return NO;
}

/*	Username and password
**	---------------------
**	Prompt Username and password as an atomic operation
*/
PUBLIC BOOL HTPromptUsernameAndPassword (HTRequest * request, HTAlertOpcode op,
					 int msgnum, const char * dfault,
					 void * input, HTAlertPar * reply)
{
    BOOL status = HTPrompt(request, op, HT_MSG_UID, dfault, input, reply);
    return status ?
	HTPromptPassword(request, op, HT_MSG_PW, dfault, input, reply) : NO;
}

/*	HTError_print
**	-------------
**	Default function that creates an error message using HTAlert() to
**	put out the contents of the error_stack messages. Furthermore, the
**	error_info structure contains a name of a help file that might be put
**	up as a link. This file can then be multi-linguistic.
*/
PUBLIC BOOL HTError_print (HTRequest * request, HTAlertOpcode op,
			   int msgnum, const char * dfault, void * input,
			   HTAlertPar * reply)
{
    HTList *cur = (HTList *) input;
    HTError *pres;
    HTErrorShow showmask = HTError_show();
    HTChunk *msg = NULL;
    int code;
    if (WWWTRACE) HTTrace("HTError..... Generating message\n");
    if (!request || !cur) return NO;
    while ((pres = (HTError *) HTList_nextObject(cur))) {
	int index = HTError_index(pres);
	if (HTError_doShow(pres)) {
	    if (!msg) {
		HTSeverity severity = HTError_severity(pres);
		msg = HTChunk_new(128);
		if (severity == ERR_WARN)
		    HTChunk_puts(msg, "Warning: ");
		else if (severity == ERR_NON_FATAL)
		    HTChunk_puts(msg, "Non Fatal Error: ");
		else if (severity == ERR_FATAL)
		    HTChunk_puts(msg, "Fatal Error: ");
		else if (severity == ERR_INFO)
		    HTChunk_puts(msg, "Information: ");
		else {
		    if (WWWTRACE)
			HTTrace("HTError..... Unknown Classification of Error (%d)...\n", severity);
		    HTChunk_delete(msg);
		    return NO;
		}

		/* Error number */
		if ((code = HTErrors[index].code) > 0) {
		    char buf[10];
		    sprintf(buf, "%d ", code);
		    HTChunk_puts(msg, buf);
		}
	    } else
		HTChunk_puts(msg, "\nReason: ");
	    HTChunk_puts(msg, HTErrors[index].msg);	    /* Error message */

	    if (showmask & HT_ERR_SHOW_PARS) {		 /* Error parameters */
		int length;
		int cnt;		
		char *pars = (char *) HTError_parameter(pres, &length);
		if (length && pars) {
		    HTChunk_puts(msg, " (");
		    for (cnt=0; cnt<length; cnt++) {
			char ch = *(pars+cnt);
			if (ch < 0x20 || ch >= 0x7F)
			    HTChunk_putc(msg, '#');
			else
			    HTChunk_putc(msg, ch);
		    }
		    HTChunk_puts(msg, ") ");
		}
	    }

	    if (showmask & HT_ERR_SHOW_LOCATION) {	   /* Error Location */
		HTChunk_puts(msg, "This occured in ");
		HTChunk_puts(msg, HTError_location(pres));
		HTChunk_putc(msg, '\n');
	    }

	    /*
	    ** Make sure that we don't get this error more than once even
	    ** if we are keeping the error stack from one request to another
	    */
	    HTError_setIgnore(pres);
	    
	    /* If we only are show the most recent entry then break here */
	    if (showmask & HT_ERR_SHOW_FIRST)
		break;
	}
    }
    if (msg) {
	HTChunk_putc(msg, '\n');
	HTTrace("WARNING: %s\n", HTChunk_data(msg));
	HTChunk_delete(msg);
    }
    return YES;
}

/*	HTError_response
**	----------------
**	Default function that creates an error message using HTAlert() to
**	put out the contents of the error_stack messages. Furthermore, the
**	error_info structure contains a name of a help file that might be put
**	up as a link. This file can then be multi-linguistic.
*/
PUBLIC BOOL HTError_response (HTRequest * request, HTAlertOpcode op,
			      int msgnum, const char * dfault, void * input,
			      HTAlertPar * reply)
{
    HTList * cur = (HTList *) input;
    HTError * pres;
    HTErrorShow showmask = HTError_show();
    HTChunk * msg = NULL;
    int code;
    if (WWWTRACE) HTTrace("HTError..... Generating HTTP response\n");
    if (!request || !cur || !reply) return NO;
    while ((pres = (HTError *) HTList_nextObject(cur))) {
	int index = HTError_index(pres);
	if (HTError_doShow(pres)) {
	    if (!msg) {
		msg = HTChunk_new(128);
		if ((code = HTErrors[index].code) > 0) {
		    char * reason = HTErrors[index].msg;
		    char * buf;
		    if ((buf = (char  *) HT_MALLOC(20 + strlen(reason))) == NULL)
		        HT_OUTOFMEM("HTError_response");
		    sprintf(buf,"%s %d %s%c%c",HTTP_VERSION,code,reason,CR,LF);
		    HTAlert_assignReplyMessage(reply, buf);
		}
	    } else {
		HTChunk_puts(msg, "\nReason: ");
		HTChunk_puts(msg, HTErrors[index].msg);	    /* Error message */
	    }

	    if (showmask & HT_ERR_SHOW_PARS) {		 /* Error parameters */
		int length;
		int cnt;		
		char *pars = (char *) HTError_parameter(pres, &length);
		if (length && pars) {
		    HTChunk_puts(msg, " (");
		    for (cnt=0; cnt<length; cnt++) {
			char ch = *(pars+cnt);
			if (ch < 0x20 || ch >= 0x7F)
			    HTChunk_putc(msg, '#');
			else
			    HTChunk_putc(msg, ch);
		    }
		    HTChunk_puts(msg, ") ");
		}
	    }

	    if (showmask & HT_ERR_SHOW_LOCATION) {	   /* Error Location */
		HTChunk_puts(msg, "This occured in ");
		HTChunk_puts(msg, HTError_location(pres));
		HTChunk_putc(msg, '\n');
	    }

	    /*
	    ** Make sure that we don't get this error more than once even
	    ** if we are keeping the error stack from one request to another
	    */
	    HTError_setIgnore(pres);
	    
	    /* If we only are show the most recent entry then break here */
	    if (showmask & HT_ERR_SHOW_FIRST)
		break;
	}
    }
    if (msg) {
	HTChunk_putc(msg, '\n');
#if 0
	HTTrace("WARNING: %s\n", HTChunk_data(msg));
#endif
	HTChunk_delete(msg);
    }
    return YES;
}
