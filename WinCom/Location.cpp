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
**	Location.cpp : implementation file
*/

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
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLocation, CPropertyPage)
    //{{AFX_MSG_MAP(CLocation)
	ON_BN_CLICKED(ID_BROWSE, OnBrowse)
	ON_BN_CLICKED(ID_SUBMIT, OnSubmit)
	ON_CBN_EDITCHANGE(IDC_SOURCE_URI, OnEditchangeSourceUri)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocation message handlers

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
	char * fileaddr = HTLocalToWWW(path, NULL);
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

        UpdateData();
    }
}

void CLocation::OnEditchangeSourceUri() 
{
    UpdateData();
    
    ASSERT(GetParentFrame()->GetActiveView()->IsKindOf(RUNTIME_CLASS(CTabsView)));
    CTabsView * view = (CTabsView *) GetParentFrame()->GetActiveView();

    // If change in source then update the entity and link info
    view->GetDocument()->m_EntityInfo.Clear();
    view->GetDocument()->m_Links.Clear();

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

    // Update our data
    UpdateData();

    // Call the document with a request request
    ASSERT(GetParentFrame()->GetActiveView()->IsKindOf(RUNTIME_CLASS(CTabsView)));
    CTabsView * view = (CTabsView *) GetParentFrame()->GetActiveView();
    view->GetDocument()->SaveDocument();
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

