// Load.cpp : implementation file
//

#include "stdafx.h"
#include "wincom.h"
#include "Load.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoad dialog

CLoad::CLoad( CWinComDoc * pDoc ) : CDialog(CLoad::IDD)
{
    ASSERT(pDoc != NULL);
    m_pDoc = pDoc;

        //{{AFX_DATA_INIT(CVersionConflict)
	m_cacheValidate = 0;
	//}}AFX_DATA_INIT
}

void CLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoad)
	DDX_Control(pDX, IDC_LOAD_ADDRESS, m_loadList);
	DDX_Radio(pDX, IDC_CACHE_VALIDATION, m_cacheValidate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoad, CDialog)
	//{{AFX_MSG_MAP(CLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoad message handlers

void CLoad::OnOK() 
{
    CDialog::OnOK();
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 

    // Check address to load
    if (m_loadList.GetWindowTextLength()) {
	m_loadList.GetWindowText(m_address);
	if (m_address.IsEmpty()) return;
	
	// Get the base address (if any)
	CString base;
	m_loadList.GetLBText(0, base);
	char * address = HTParse(m_address, base.IsEmpty() ? "" : base, PARSE_ALL);
	address = HTSimplify(&address);
	m_address = address;
	m_loadList.SetWindowText(m_address);
	HT_FREE(address);
	HTAnchor * anchor = HTAnchor_findAddress(m_address);
	
	// Update the address combo box
	if (m_loadList.FindStringExact(-1, m_address) == CB_ERR) {
	    m_loadList.InsertString(0, m_address);
	    int srcLength = m_loadList.GetCount();
	    if (srcLength >= MAX_LIST_LENGTH) {
		int cnt;
		for (cnt=MAX_LIST_LENGTH; cnt < srcLength; cnt++)
		    m_loadList.DeleteString(cnt);
	    }
	    
	    // Write into INI file
	    pApp->AddLoadAddressToIniFile(m_address);
	}
	
	
	// Start the request
	CRequest * request = new CRequest(m_pDoc);
	request->GetDocument(anchor, m_cacheValidate);
    }
}

BOOL CLoad::OnInitDialog() 
{
    CDialog::OnInitDialog();
	
    // Setup the combo box
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 
    pApp->FillLoadComboBox(&m_loadList);
	
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
