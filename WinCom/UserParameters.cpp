// UserParameters.cpp: implementation of the CUserParameters class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinCom.h"

#include "Password.h"
#include "MainFrm.h"

/* Library include files */
#include "WWWLib.h"
#include "UserParameters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
** All errors that are not strictly HTTP errors but originates from, e.g., 
** the FTP protocol all have element numbers > HTERR_HTTP_CODES_END, i.e.,
** they should be placed after the blank line
*/
PRIVATE HTErrorMessage HTErrors[HTERR_ELEMENTS] = 
{
    { 100, "Continue", 					"information" },
    { 101, "Switching Protocols",			"information" },
    { 200, "OK", 					"success" },
    { 201, "Document Created",				"success" },
    { 202, "Accepted", 					"success" },
    { 203, "Non-authoritative Information",		"success" },
    { 204, "Document Updated",          		"success" },
    { 205, "Reset Content",				"success" },
    { 206, "Partial Content",				"success" },
    { 207, "Partial Update OK",				"success" },
    { 300, "Multiple Choices",				"redirection" },
    { 301, "Moved Permanently",				"redirection" },
    { 302, "Found",  			                "redirection" },
    { 303, "See Other",					"redirection" },
    { 304, "Not Modified",       			"redirection" },
    { 305, "Use Proxy",					"redirection" },
    { 306, "Proxy Redirect",				"redirection" },
    { 307, "Temporary Redirect",			"redirection" },
    { 400, "Bad Request", 				"client_error" },
    { 401, "Unauthorized",				"client_error" },
    { 402, "Payment Required", 				"client_error" },
    { 403, "Forbidden", 				"client_error" },
    { 404, "Not Found",		       			"client_error" },
    { 405, "Method Not Allowed",	 		"client_error" },
    { 406, "Not Acceptable",		 		"client_error" },
    { 407, "Proxy Authentication Required", 		"client_error" },
    { 408, "Request Timeout",		 		"client_error" },
    { 409, "Conflict",			 		"client_error" },
    { 410, "Gone",			 		"client_error" },
    { 411, "Length Required",		 		"client_error" },
    { 412, "Version Conflict",		 		"client_error" },
    { 413, "Request Entity Too Large",	 		"client_error" },
    { 414, "Request-URI Too Large",	 		"client_error" },
    { 415, "Unsupported Media Type",	 		"client_error" },
    { 416, "Range Not Satisfiable",	 		"client_error" },
    { 417, "Expectation Failed",	 		"client_error" },
    { 418, "Reauthentication Required",	 		"client_error" },
    { 419, "Proxy Reauthentication Reuired", 		"client_error" },
    { 500, "Internal Server Error",			"server_error" },
    { 501, "Not Implemented", 				"server_error" },
    { 502, "Bad Gateway", 				"server_error" },
    { 503, "Service Unavailable",			"server_error" },
    { 504, "Gateway Timeout", 				"server_error" },
    { 505, "HTTP Version not supported",		"server_error" },
    { 506, "Partial update Not Implemented",		"server_error" },
 
    /* Cache Warnings */
    { 10,  "Response is Stale",				"cache" },
    { 11,  "Revalidation Failed",			"cache" },
    { 12,  "Disconnected Opeartion",			"cache" },
    { 13,  "Heuristic Expiration",			"cache" },
    { 14,  "Transformation Applied",			"cache" },
    { 99,  "Cache warning", 				"cache" },

    /* Non-HTTP Error codes and warnings */
    { 0,   "Can't locate remote host", 			"internal" },
    { 0,   "No host name found", 			"internal" },
    { 0,   "No file name found or file not accessible", "internal" },
    { 0,   "FTP server replies", 			"internal" },
    { 0,   "FTP server doesn't reply", 			"internal" },
    { 0,   "FTP login failure", 			"internal" },
    { 0,   "Server timed out", 				"internal" },
    { 0,   "Gopher-server replies", 			"internal" },
    { 0,   "Data transfer interrupted", 		"internal" },
    { 0,   "Connection establishment interrupted", 	"internal" },
    { 0,   "CSO-server replies", 			"internal" },
    { 0,   "This is probably a HTTP server 0.9 or less","internal" },
    { 0,   "Bad, Incomplete, or Unknown Response",	"internal" },
    { 0,   "Unknown access authentication scheme",	"internal" },
    { 0,   "News-server replies",			"internal" },
    { 0,   "Trying `ftp://' instead of `file://'",	"internal" },
    { 0,   "Too many redirections",			"internal" },
    { 0,   "Method not suited for automatic redirection","internal" },
    { 0,   "Premature End Of File",			"internal" },
    { 0,   "Response from WAIS Server too Large - Extra lines \
ignored","internal"},
    { 0,   "WAIS-server doesn't return any data", 	"internal" },
    { 0,   "Can't connect to WAIS-server",		"internal" },
    { 0,   "System replies",				"internal" },
    { 0,   "Wrong or unknown access scheme",		"internal" },
    { 0,   "Access scheme not allowed in this context",	"internal" },
    { 0,   "When you are connected, you can log in",	"internal" },
    { 0,   "This version has expired and will be automatically reloaded", "internal" },
    { 0,   "Loading new rules must be explicitly acknowledged", "internal" },
    { 0,   "Automatic proxy redirection must be explicitly acknowledged", "internal" }
};

/* ------------------------------------------------------------------------- */

PUBLIC BOOL UserProgress (HTRequest * request, HTAlertOpcode op,
                          int msgnum, const char * dfault, void * input,
                          HTAlertPar * reply)
{
    static int inside = 0;
    char space[256];
    CWaitCursor wait;
    if (!request) return NO;

    switch (op) {
    case HT_PROG_DNS:
        sprintf(space, "Looking up %s", input ? (char *) input : "host");
        break;
        
    case HT_PROG_CONNECT:
        sprintf(space, "Contacting %s...", input ? (char *) input : "host");
        break;
        
    case HT_PROG_ACCEPT:
        sprintf(space, "Waiting for connection...");
        break;
        
    case HT_PROG_READ:
        {
            long cl = HTAnchor_length(HTRequest_anchor(request));
            if (cl > 0) {
                long b_read = HTRequest_bodyRead(request);
                double pro = (double) b_read/cl*100;
                char buf[10];
                HTNumToStr((unsigned long) cl, buf, 10);
                sprintf(space, "Read (%d%% of %s)", (int) pro, buf);
            } else
                sprintf(space, "Reading...");
        }
        break;
        
    case HT_PROG_WRITE:
        {
            long cl = HTAnchor_length(HTRequest_entityAnchor(request));
            if (cl > 0) {
                long b_written = HTRequest_bodyWritten(request);
                double pro = (double) b_written/cl*100;
                char buf[10];
                HTNumToStr((unsigned long) cl, buf, 10);
                sprintf(space, "Writing (%d%% of %s)", (int) pro, buf);
            } else
                sprintf(space, "Writing...");
        }
        break;
        
    case HT_PROG_DONE:
        sprintf(space, "Done!");
        break;
        
    case HT_PROG_INTERRUPT:
        sprintf(space, "Interrupted!\n");
        break;
        
    case HT_PROG_OTHER:
        sprintf(space, "Working - please wait...\n");
        break;

    default:
        sprintf(space, "UNKNOWN PROGRESS STATE\n");
        break;
    }

    // Update pane 0 of the status bar
    CRequest * req = (CRequest *) HTRequest_context(request);
    if (req) {
	CWinComDoc * doc = req->m_pDoc;
	if (doc) {
            POSITION pos = doc->GetFirstViewPosition();
            CView * view = doc->GetNextView( pos );
	    CMainFrame * mainframe = (CMainFrame *) view->GetParentFrame();
	    mainframe->m_wndStatusBar.SetPaneText(ID_SEPARATOR, space);
	}
    }
    return YES;
}

PUBLIC BOOL UserPrint (HTRequest * request, HTAlertOpcode op,
                       int msgnum, const char * dfault, void * input,
                       HTAlertPar * reply)
{
    HTList *cur = (HTList *) input;
    HTError *pres;
    HTErrorShow showmask = HTError_show();
    HTChunk *msg = NULL;
    int code;
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
                    HTChunk_puts(msg, "Success: ");
                else {
                    HTChunk_delete(msg);
                    return NO;
                }
                
            } else
                HTChunk_puts(msg, "\nReason: ");
            HTChunk_puts(msg, HTErrors[index].msg);	    /* Error message */
            
            if (showmask & HT_ERR_SHOW_PARS) {		 /* Error parameters */
                int length;
                int cnt;		
                char *pars = (char *) HTError_parameter(pres, &length);
                if (length && pars) {
                    HTChunk_puts(msg, " (Server replies: ");

		    /* Error number */
		    if ((code = HTErrors[index].code) > 0) {
			char buf[10];
	                sprintf(buf, "%d ", code);
		        HTChunk_puts(msg, buf);
	            }

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
        AfxMessageBox( HTChunk_data(msg), MB_OK | MB_ICONEXCLAMATION );
        HTChunk_delete(msg);
    }
    return YES;
}

PUBLIC BOOL UserConfirm (HTRequest * request, HTAlertOpcode op,
                         int msgnum, const char * dfault, void * input,
                         HTAlertPar * reply)
{
    WPARAM wParam = 0;
    int result = IDRETRY;
    CUserParameters UserParams(request, op, msgnum, dfault, input, reply);
    if (msgnum == HT_MSG_RETRY_PROXY_AUTH) {
        result = AfxMessageBox(IDS_RETRY_PROXY_AUTH, MB_RETRYCANCEL | MB_ICONEXCLAMATION );
    } else if (msgnum == HT_MSG_RETRY_AUTHENTICATION) {
        result = AfxMessageBox(IDS_RETRY_AUTH, MB_RETRYCANCEL | MB_ICONEXCLAMATION );
    }
    return result == IDRETRY ? YES : NO;
}

PUBLIC BOOL UserPrompt (HTRequest * request, HTAlertOpcode op,
                        int msgnum, const char * dfault, void * input,
                        HTAlertPar * reply)
{
    WPARAM wParam = 0;
    CUserParameters UserParams(request, op, msgnum, dfault, input, reply);
    return NO;
}

PUBLIC BOOL UserPassword (HTRequest * request, HTAlertOpcode op,
                          int msgnum, const char * dfault, void * input,
                          HTAlertPar * reply)
{
    WPARAM wParam = 0;
    CUserParameters UserParams(request, op, msgnum, dfault, input, reply);
    return NO;
}

PUBLIC BOOL UserNameAndPassword (HTRequest * request, HTAlertOpcode op,
                                 int msgnum, const char * dfault, void * input,
                                 HTAlertPar * reply)
{
    CUserParameters UserParams(request, op, msgnum, dfault, input, reply);
    CPassword AskUserNameAndPassword( &UserParams );
    if (AskUserNameAndPassword.DoModal() == IDOK) {
        HTAlert_setReplyMessage(reply, AskUserNameAndPassword.m_username);
        HTAlert_setReplySecret(reply, AskUserNameAndPassword.m_password);
        return YES;
    }
    return NO;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CUserParameters, CObject)

CUserParameters::CUserParameters() : CObject()
{
}

CUserParameters::CUserParameters(HTRequest * request, HTAlertOpcode op,
                                 int msgnum, const char * dfault, void * input,
                                 HTAlertPar * reply)
{
    m_request = request;
    m_op = op;
    m_msgnum = msgnum;
    m_default = dfault;
    m_input = input;
    m_reply = reply;
}

CUserParameters::~CUserParameters()
{

}
