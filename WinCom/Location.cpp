// Location.cpp : implementation file
//

#include "stdafx.h"
#include "WinCom.h"
#include "Location.h"
#include "MainFrm.h"
#include "WinComDoc.h"
#include "TabsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocation property page

IMPLEMENT_DYNCREATE(CLocation, CPropertyPage)

CLocation::CLocation() : CPropertyPage(CLocation::IDD)
{
    //{{AFX_DATA_INIT(CLocation)
	//}}AFX_DATA_INIT
}

CLocation::~CLocation()
{
}

void CLocation::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CLocation)
	DDX_Control(pDX, IDC_SOURCE_URI, m_sourceList);
	DDX_Control(pDX, IDC_DESTINATION_URI, m_destinationList);
	DDX_Control(pDX, ID_SUBMIT, m_submit);
	DDX_Control(pDX, ID_CANCEL, m_cancel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLocation, CPropertyPage)
    //{{AFX_MSG_MAP(CLocation)
	ON_BN_CLICKED(ID_BROWSE, OnBrowse)
	ON_CBN_EDITCHANGE(IDC_SOURCE_URI, OnEditSourceLocation)
	ON_CBN_EDITCHANGE(IDC_DESTINATION_URI, OnEditDestinationLocation)
	ON_BN_CLICKED(ID_SUBMIT, OnSubmit)
	ON_CBN_KILLFOCUS(IDC_SOURCE_URI, OnKillfocusSourceUri)
	ON_BN_CLICKED(ID_CANCEL, OnCancel)
	ON_CBN_KILLFOCUS(IDC_DESTINATION_URI, OnKillfocusDestinationUri)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocation message handlers

void CLocation::CheckSubmit (void)
{
#if 0
    // If both source and destination are available then allow "submit"
    int slen = m_sourceList.GetWindowTextLength();
    int dlen = m_destinationList.GetWindowTextLength(); 
    if (slen > 0 && dlen > 0) {
        m_submit.EnableWindow(TRUE);
    } else
        m_submit.EnableWindow(FALSE);
#else
        m_submit.EnableWindow(TRUE);
#endif
}

void CLocation::OnBrowse()
{
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 

    CFileDialog fd(TRUE);
    fd.m_ofn.lpstrInitialDir = pApp->GetIniCWD();
    UpdateData();
    if (fd.DoModal() == IDOK) {
	
	// Make the source an absolute URI
	CString path = fd.GetPathName();
	char * fileaddr = HTLocalToWWW(path);
	if (fileaddr) {
	    m_source = fileaddr;
	    m_sourceList.SetWindowText(fileaddr);
	    
	    // Set the initial dir for next time
	    int idx = path.ReverseFind('\\');
	    if (idx != -1) path.GetBufferSetLength(idx+1);
	    pApp->SetIniCWD(path);

	    HT_FREE(fileaddr);
	}

	/* Update the destination */
	CString file = fd.GetFileName();
	if (!file.IsEmpty()) {
            CString base = "";
	    char * escaped = HTEscape(file, URL_XALPHAS);
	    char * destination = escaped;
	    int length = m_destinationList.GetCount();
	    if (length > 0) {
		m_destinationList.GetLBText(0, base);
		if (!base.IsEmpty()) {
		    destination = HTParse(escaped, base, PARSE_ALL);
		    HT_FREE(escaped);
		}
	    }
	    destination = HTSimplify(&destination);
	    m_destination = destination;
	    m_destinationList.SetWindowText(m_destination);
	    HT_FREE(destination);
	}

	// If change in source then update the entity and link info
        ASSERT(GetParentFrame()->GetActiveView()->IsKindOf(RUNTIME_CLASS(CTabsView)));
	CTabsView * view = (CTabsView *) GetParentFrame()->GetActiveView();
	view->GetDocument()->m_EntityInfo.Clear();
        view->GetDocument()->m_Links.Clear();

	CheckSubmit();
        UpdateData();
    }
}

void CLocation::OnEditSourceLocation() 
{
    UpdateData();
    
    ASSERT(GetParentFrame()->GetActiveView()->IsKindOf(RUNTIME_CLASS(CTabsView)));
    CTabsView * view = (CTabsView *) GetParentFrame()->GetActiveView();

    // If change in source then update the entity and link info
    view->GetDocument()->m_EntityInfo.Clear();
    view->GetDocument()->m_Links.Clear();

    CheckSubmit();
}

void CLocation::OnEditDestinationLocation() 
{
    UpdateData();
    CheckSubmit();
}

void CLocation::OnSubmit() 
{
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 

    // Check source and destination
    m_sourceList.GetWindowText(m_source);
    m_destinationList.GetWindowText(m_destination);
    if (m_source.IsEmpty() || m_destination.IsEmpty()) return;
    
    // Update the source combo box
    if (m_sourceList.FindStringExact(-1, m_source) == CB_ERR) {
	m_sourceList.InsertString(0, m_source);
	int srcLength = m_sourceList.GetCount();
	if (srcLength >= MAX_LIST_LENGTH) {
	    int cnt;
	    for (cnt=MAX_LIST_LENGTH; cnt < srcLength; cnt++)
		m_sourceList.DeleteString(cnt);
	}

        // Write into INI file
        pApp->AddSourceToIniFile(m_source);
    }
    
    // Update the destination combo box
    if (m_destinationList.FindStringExact(-1, m_destination) == CB_ERR) {
	m_destinationList.InsertString(0, m_destination);
	int dstLength = m_destinationList.GetCount();
	if (dstLength >= MAX_LIST_LENGTH) {
	    int cnt;
	    for (cnt=MAX_LIST_LENGTH; cnt < dstLength; cnt++)
		m_destinationList.DeleteString(cnt);
	}

        // Write into INI file
        pApp->AddDestinationToIniFile(m_destination);
    }

    // Turn off the submit button
    m_submit.EnableWindow(FALSE);

    // Turn on the cancel button
    m_cancel.EnableWindow(TRUE);

    // Turn off source and destination controls
    m_sourceList.EnableWindow(FALSE);
    m_destinationList.EnableWindow(FALSE);

    // Update our data
    UpdateData();

    // Call the document with a request request
    ASSERT(GetParentFrame()->GetActiveView()->IsKindOf(RUNTIME_CLASS(CTabsView)));
    CTabsView * view = (CTabsView *) GetParentFrame()->GetActiveView();
    view->GetDocument()->SubmitRequest();    
}

void CLocation::OnCancel() 
{
    CheckSubmit();

    // Turn off the cancel button
    m_cancel.EnableWindow(FALSE);

    // Turn on source and destination controls
    m_sourceList.EnableWindow(TRUE);
    m_destinationList.EnableWindow(TRUE);

    // Call the document with a request request
    ASSERT(GetParentFrame()->GetActiveView()->IsKindOf(RUNTIME_CLASS(CTabsView)));
    CTabsView * view = (CTabsView *) GetParentFrame()->GetActiveView();
    view->GetDocument()->CancelRequest();
}

void CLocation::OnFinish()
{
    // Turn on the submit button
    m_submit.EnableWindow(TRUE);

    // Turn off the cancel button
    m_cancel.EnableWindow(FALSE);

    // Turn on source and destination controls
    m_sourceList.EnableWindow(TRUE);
    m_destinationList.EnableWindow(TRUE);
}

BOOL CLocation::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 
    pApp->FillSourceComboBox(&m_sourceList);
    pApp->FillDestinationComboBox(&m_destinationList);
    return TRUE;
}

BOOL CLocation::OnKillActive() 
{
    m_sourceList.GetWindowText(m_source);
    m_destinationList.GetWindowText(m_destination);
    return CPropertyPage::OnKillActive();
}

void CLocation::OnKillfocusSourceUri() 
{
    CheckSubmit();
}

void CLocation::OnKillfocusDestinationUri() 
{
    CheckSubmit();
}
