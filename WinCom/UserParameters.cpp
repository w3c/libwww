/*
**	@(#) $Id$
**	
**	W3C Web Commander can be found at "http://www.w3.org/WinCom/"
**	
**	Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**	UserParameters.cpp: implementation of the CUserParameters class.
*/

#include "stdafx.h"
#include "WinCom.h"

#include "Password.h"
#include "MainFrm.h"

/* Library include files */
#include "WWWLib.h"
#include "WWWApp.h"
#include "UserParameters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/* ------------------------------------------------------------------------- */

PUBLIC BOOL UserProgress (HTRequest * request, HTAlertOpcode op,
                          int msgnum, const char * dfault, void * input,
                          HTAlertPar * reply)
{
    char * msg = HTDialog_progressMessage(request, op, msgnum, dfault, input);
    CRequest * req = (CRequest *) HTRequest_context(request);
    ASSERT(request != NULL);
    ASSERT(req != NULL);
    CProgressCtrl * progress = req->GetProgressBar();

    switch (op) {
    case HT_PROG_READ:
        {
            long cl = HTAnchor_length(HTRequest_anchor(request));
            if (cl > 0) {
                long b_read = HTRequest_bodyRead(request);
                double pro = (double) b_read/cl*100;
		progress->SetPos((int) pro);
            }
        }
        break;
        
    case HT_PROG_WRITE:
        {
            long cl = HTAnchor_length(HTRequest_entityAnchor(request));
            if (cl > 0) {
                long b_written = HTRequest_bodyWritten(request);
                double pro = (double) b_written/cl*100;
		progress->SetPos((int) pro);
            }
        }
        break;
    }

    // Update pane 0 of the status bar
    if (msg) {
	CWinComDoc * doc = req->m_pDoc;
	if (doc) {
	    POSITION pos = doc->GetFirstViewPosition();
	    CView * view = doc->GetNextView( pos );
	    CMainFrame * mainframe = (CMainFrame *) view->GetParentFrame();
	    mainframe->m_wndStatusBar.SetPaneText(ID_SEPARATOR, msg);
	}
	HT_FREE(msg);
    }
    return YES;
}

PUBLIC BOOL UserPrint (HTRequest * request, HTAlertOpcode op,
                       int msgnum, const char * dfault, void * input,
                       HTAlertPar * reply)
{
    char * msg = HTDialog_errorMessage(request, op, msgnum, dfault, input);
    if (msg) {
        AfxMessageBox( msg, MB_OK | MB_ICONEXCLAMATION );
	HT_FREE(msg);
    }
    return YES;
}

PUBLIC BOOL UserConfirm (HTRequest * request, HTAlertOpcode op,
                         int msgnum, const char * dfault, void * input,
                         HTAlertPar * reply)
{
    WPARAM wParam = 0;
    int result = IDRETRY;
    BOOL status = YES;
    CUserParameters UserParams(request, op, msgnum, dfault, input, reply);
    switch (msgnum) {

    case HT_MSG_RETRY_PROXY_AUTH:
        result = AfxMessageBox(IDS_RETRY_PROXY_AUTH, MB_RETRYCANCEL | MB_ICONEXCLAMATION );
        status = result==IDRETRY ? YES : NO;
        break;

    case HT_MSG_RETRY_AUTHENTICATION:
        result = AfxMessageBox(IDS_RETRY_AUTH, MB_RETRYCANCEL | MB_ICONEXCLAMATION );
        status = result==IDRETRY ? YES : NO;
        break;
    
    case HT_MSG_CACHE_LOCK:
        result = AfxMessageBox(IDS_BREAK_CACHE_LOCK, MB_YESNO | MB_ICONSTOP | MB_DEFBUTTON2 );
        status = result==IDYES ? YES : NO;
        break;

    default:
        status = YES;
    }
    return status;
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
