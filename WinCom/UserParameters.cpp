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
PRIVATE HTErrorMessage HTErrors[HTERR_ELEMENTS] = {HTERR_ENGLISH_INITIALIZER};

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
                long b_written = HTRequest_bytesWritten(request);
                double pro = (double) b_written/cl*100;
                char buf[10];
                HTNumToStr((unsigned long) cl, buf, 10);
                sprintf(space, "Written (%d%% of %s)", (int) pro, buf);
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
