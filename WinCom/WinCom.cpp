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
**	WinCom.cpp : Defines the class behaviors for the application.
*/

#include "stdafx.h"
#include "WinCom.h"

#include "MainFrm.h"
#include "WinComDoc.h"
#include "TabsView.h"

#include "Location.h"
#include "EntityInfo.h"
#include "Links.h"
#include "ProxySetup.h"
#include "CacheSetup.h"
#include "Password.h"
#include "VersionConflict.h"

// From libwww
#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWApp.h"
#include "WWWInit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinComApp

BEGIN_MESSAGE_MAP(CWinComApp, CWinApp)
	//{{AFX_MSG_MAP(CWinComApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_OPTIONS_PROXIES, OnOptionsProxies)
	ON_COMMAND(ID_CACHE_SETUP, OnCacheSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinComApp construction

CWinComApp::CWinComApp()
{	
	// TODO: add construction code here,
	m_pSourceList = NULL;
	m_pDestinationList = NULL;
	m_pLinkList = NULL;
	m_pLoadList = NULL;
	m_pDeleteList = NULL;

        m_detectVersionConflict = FALSE;
        m_showServerStatus = FALSE;
    
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWinComApp object

CWinComApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWinComApp initialization

BOOL CWinComApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

        /* Initialize libwww */
        HTProfile_newNoCacheClient("WebCommander", "1.1");

        /* Setup our own dialog handlers */
        {
            //HTAlert_deleteAll();
            HTAlert_add(UserProgress, HT_A_PROGRESS);
            HTAlert_add(UserPrint, HT_A_MESSAGE);
            HTAlert_add(UserConfirm, HT_A_CONFIRM);
            HTAlert_add(UserPrompt, HT_A_PROMPT);
            HTAlert_add(UserPassword, HT_A_SECRET);
            HTAlert_add(UserNameAndPassword, HT_A_USER_PW);
        }

	/* Setup our persistent cache */
	CacheSetup.Init();

	/* Set up amount of warning info wanted */
        {
            m_showServerStatus = GetIniShowServerStatus();
            int show_flag = m_showServerStatus ? HT_ERR_SHOW_PARS : 0;
            HTError_setShow((HTErrorShow) (HT_ERR_SHOW_INFO | show_flag));
        }

        /* We don't care about case sensitivity when matching local files */
        HTBind_caseSensitive(NO);

        CSingleDocTemplate * pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWinComDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTabsView));
	AddDocTemplate(pDocTemplate);

	// Set up the MRU file lists
	static TCHAR BASED_CODE srcIniFileSection[] = _T("Most Recent Source Address");
	static TCHAR BASED_CODE dstIniFileSection[] = _T("Most Recent Destination Address");
	static TCHAR BASED_CODE lnkIniFileSection[] = _T("Most Recent Link Address");
	static TCHAR BASED_CODE lodIniFileSection[] = _T("Most Recent Load Address");
	static TCHAR BASED_CODE delIniFileSection[] = _T("Most Recent Delete Address");

	static TCHAR BASED_CODE szIniFileEntry[] = _T("URI%d");

	m_pSourceList = new CRecentFileList(1, srcIniFileSection, szIniFileEntry, MAX_LIST_LENGTH);
	m_pDestinationList = new CRecentFileList(1, dstIniFileSection, szIniFileEntry, MAX_LIST_LENGTH);
	m_pLinkList = new CRecentFileList(1, lnkIniFileSection, szIniFileEntry, MAX_LIST_LENGTH);
	m_pLoadList = new CRecentFileList(1, lodIniFileSection, szIniFileEntry, MAX_LIST_LENGTH);
	m_pDeleteList = new CRecentFileList(1, delIniFileSection, szIniFileEntry, MAX_LIST_LENGTH);

	ASSERT(m_pDestinationList != NULL);
        m_pDestinationList->ReadList();

	ASSERT(m_pSourceList != NULL);
	m_pSourceList->ReadList();

	ASSERT(m_pLinkList != NULL);
	m_pLinkList->ReadList();

	ASSERT(m_pLoadList != NULL);
	m_pLoadList->ReadList();

	ASSERT(m_pDeleteList != NULL);
	m_pDeleteList->ReadList();

        // Setup proxies
	if (ProxySetup.GetProxyOptions() == TRUE)
	    ProxySetup.RegisterProxies();
	
	/* Create a new (empty) document */
	OnFileNew();

        return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// INI implementation

void CWinComApp::AddSourceToIniFile (LPCTSTR lpszPathName)
{
    ASSERT(m_pSourceList != NULL);
    m_pSourceList->Add(lpszPathName);
}

int CWinComApp::GetSourceIniListSize (void)
{
    ASSERT(m_pSourceList != NULL);
    return m_pSourceList->GetSize();
}

void CWinComApp::AddDestinationToIniFile (LPCTSTR lpszPathName)
{
    ASSERT(m_pDestinationList != NULL);
    m_pDestinationList->Add(lpszPathName);
}

int CWinComApp::GetDestinationIniListSize (void)
{
    ASSERT(m_pDestinationList != NULL);
    return m_pDestinationList->GetSize();
}

void CWinComApp::AddLoadAddressToIniFile (LPCTSTR lpszPathName)
{
    ASSERT(m_pLoadList != NULL);
    m_pLoadList->Add(lpszPathName);
}

int CWinComApp::GetLoadAddressIniListSize (void)
{
    ASSERT(m_pLoadList != NULL);
    return m_pLoadList->GetSize();
}

void CWinComApp::AddDeleteAddressToIniFile (LPCTSTR lpszPathName)
{
    ASSERT(m_pDeleteList != NULL);
    m_pDeleteList->Add(lpszPathName);
}

int CWinComApp::GetDeleteAddressIniListSize (void)
{
    ASSERT(m_pDeleteList != NULL);
    return m_pDeleteList->GetSize();
}

void CWinComApp::AddLinkToIniFile (LPCTSTR lpszPathName)
{
    ASSERT(m_pLinkList != NULL);
    m_pLinkList->Add(lpszPathName);
}

int CWinComApp::GetLinkIniListSize(void)
{
    ASSERT(m_pLinkList != NULL);
    return m_pLinkList->GetSize();
}

static int FillComboBox (CRecentFileList * pList, CComboBox *pBox)
{
    ASSERT(pList != NULL);
    ASSERT(pBox != NULL);
    int size = pList->GetSize();
    int cnt;
    int index = 0;
    int status = 0;
    for (cnt=0; cnt<size; cnt++) {
	if ((* pList)[cnt]) {
	    status = pBox->InsertString(index++, (* pList)[cnt]);
	    if (status == CB_ERRSPACE || status == CB_ERR) break;
	}
    }
    return status;
}

int CWinComApp::FillLinkComboBox (CComboBox * pBox)
{
    return FillComboBox(m_pLinkList, pBox);
}

int CWinComApp::FillSourceComboBox (CComboBox * pBox)
{
    return FillComboBox(m_pSourceList, pBox);
}

int CWinComApp::FillDestinationComboBox (CComboBox * pBox)
{
    return FillComboBox(m_pDestinationList, pBox);
}

int CWinComApp::FillLoadComboBox (CComboBox * pBox)
{
    return FillComboBox(m_pLoadList, pBox);
}

int CWinComApp::FillDeleteComboBox (CComboBox * pBox)
{
    return FillComboBox(m_pDeleteList, pBox);
}

BOOL CWinComApp::SetIniCWD (CString cwd)
{
    CString strSection		= "Application";
    CString strCWD  		= "Current Directory";
    m_currentDir = cwd;
    return WriteProfileString(strSection, strCWD, m_currentDir);
}

CString CWinComApp::GetIniCWD (void)
{
    CString strSection		= "Application";
    CString strCWD		= "Current Directory";
    m_currentDir = GetProfileString(strSection, strCWD);
    return m_currentDir;
}

BOOL CWinComApp::SetIniDetectVersionConflict (BOOL detect)
{
    CString strSection		= "Application";
    CString strCWD  		= "Detect Conflicts";
    m_detectVersionConflict = detect;
    return WriteProfileInt(strSection, strCWD, m_detectVersionConflict);
}

BOOL CWinComApp::GetIniDetectVersionConflict (void)
{
    CString strSection		= "Application";
    CString strCWD		= "Detect Conflicts";
    m_detectVersionConflict = GetProfileInt(strSection, strCWD, TRUE);
    return m_detectVersionConflict;
}

BOOL CWinComApp::SetIniShowServerStatus (BOOL show)
{
    CString strSection		= "Application";
    CString strCWD  		= "Show Server Status";
    m_showServerStatus = show;
    return WriteProfileInt(strSection, strCWD, m_showServerStatus);
}

BOOL CWinComApp::GetIniShowServerStatus (void)
{
    CString strSection		= "Application";
    CString strCWD		= "Show Server Status";
    m_showServerStatus = GetProfileInt(strSection, strCWD, FALSE);
    return m_showServerStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CWinComApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CWinComApp commands

int CWinComApp::Run() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinApp::Run();
}

int CWinComApp::ExitInstance() 
{
    /* Terminate cache */
    CacheSetup.Terminate();
  
    /* Terminate libwww */
    HTProfile_delete();

    ASSERT(m_pDestinationList != NULL);
    m_pDestinationList->WriteList();
    delete m_pDestinationList;

    ASSERT(m_pSourceList != NULL);
    m_pSourceList->WriteList();
    delete m_pSourceList;
    
    ASSERT(m_pLinkList != NULL);
    m_pLinkList->WriteList();
    delete m_pLinkList;

    ASSERT(m_pLoadList != NULL);
    m_pLoadList->WriteList();
    delete m_pLoadList;

    ASSERT(m_pDeleteList != NULL);
    m_pDeleteList->WriteList();
    delete m_pDeleteList;

    return CWinApp::ExitInstance();
}

void CWinComApp::OnOptionsProxies() 
{
    if (ProxySetup.DoModal() == IDOK) {
	ProxySetup.AddProxyOptionsToIni();
	ProxySetup.RegisterProxies();
    }
}

void CWinComApp::OnCacheSetup() 
{
    if (CacheSetup.DoModal() == IDOK) {
	CacheSetup.Terminate();
	CacheSetup.Init();
    }
}
