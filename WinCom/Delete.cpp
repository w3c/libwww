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
**	Delete.cpp : implementation file
*/

#include "stdafx.h"
#include "wincom.h"
#include "Delete.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDelete dialog

CDelete::CDelete( CWinComDoc * pDoc ) : CDialog(CDelete::IDD)
{
    ASSERT(pDoc != NULL);
    m_pDoc = pDoc;
}

void CDelete::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelete)
	DDX_Control(pDX, IDC_DELETE_ADDRESS, m_deleteList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelete, CDialog)
	//{{AFX_MSG_MAP(CDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelete message handlers

void CDelete::OnOK() 
{
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 

    // Check address to delete
    if (m_deleteList.GetWindowTextLength()) {
	m_deleteList.GetWindowText(m_address);
	if (m_address.IsEmpty()) return;
	
	// Get the base address (if any)
	CString base;
	m_deleteList.GetLBText(0, base);
	char * address = HTParse(m_address, base.IsEmpty() ? "" : base, PARSE_ALL);
	address = HTSimplify(&address);
	m_address = address;
	m_deleteList.SetWindowText(m_address);
	HT_FREE(address);
	HTAnchor * anchor = HTAnchor_findAddress(m_address);
	
	// Update the address combo box
	if (m_deleteList.FindStringExact(-1, m_address) == CB_ERR) {
	    m_deleteList.InsertString(0, m_address);
	    int srcLength = m_deleteList.GetCount();
	    if (srcLength >= MAX_LIST_LENGTH) {
		int cnt;
		for (cnt=MAX_LIST_LENGTH; cnt < srcLength; cnt++)
		    m_deleteList.DeleteString(cnt);
	    }
	    
	    // Write into INI file
	    pApp->AddDeleteAddressToIniFile(m_address);
	}
	
	
	// Start the request
	CRequest * request = new CRequest(m_pDoc);
	request->DeleteDocument(anchor, HT_NO_MATCH);
    }
    CDialog::OnOK();
}

BOOL CDelete::OnInitDialog() 
{
    CDialog::OnInitDialog();
	
    // Setup the combo box
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 
    pApp->FillDeleteComboBox(&m_deleteList);
	
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
