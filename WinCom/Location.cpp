// Location.cpp : implementation file
//

#include "stdafx.h"
#include "WinCom.h"
#include "Location.h"

// From libwww
#include "WWWLib.h"

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
}

CLocation::CLocation( CRequest * pRequest) : CPropertyPage(CLocation::IDD)
{
    //{{AFX_DATA_INIT(CLocation)
    m_destination = _T("http://");
    m_source = _T("");
    //}}AFX_DATA_INIT
    m_pRequest = pRequest;
    m_appendSource = TRUE;
    pRequest->m_pLocation = this;
}

CLocation::~CLocation()
{
}

void CLocation::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CLocation)
    DDX_CBString(pDX, IDC_DESTINATION_URI, m_destination);
    DDV_MaxChars(pDX, m_destination, 2048);
    DDX_CBString(pDX, IDC_SOURCE_URI, m_source);
    DDV_MaxChars(pDX, m_source, 2048);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLocation, CPropertyPage)
    //{{AFX_MSG_MAP(CLocation)
    ON_BN_CLICKED(ID_BROWSE, OnBrowse)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocation message handlers

BOOL CLocation::OnKillActive() 
{
    BOOL bOk = CPropertyPage::OnKillActive();
    if ( bOk ) {

	/* Find the source */
        LPTSTR source = m_source.GetBuffer(64);
	char * src = HTParse(source, m_pRequest->m_cwd, PARSE_ALL);
	m_pRequest->m_pHTAnchorSource = HTAnchor_findAddress(src);
	m_source.ReleaseBuffer( );
	
        /* If destination is a local file then error */
        if (m_destination && !m_destination.IsEmpty()) {
            CFile dst_file;
            if (dst_file.Open(m_destination, CFile::modeRead)) {
                CString strMessage;
                AfxFormatString1(strMessage, IDS_CANNOT_WRITE_TO_FILE, m_destination);
                AfxMessageBox(strMessage);
                dst_file.Close();
                return FALSE;
            } else {
		LPTSTR destination = m_destination.GetBuffer(64);
		char * dest = HTParse(destination, m_pRequest->m_cwd, PARSE_ALL);
		m_pRequest->m_pHTAnchorDestination = HTAnchor_findAddress(dest);
		m_destination.ReleaseBuffer( );
	    }
	}
    }
    return bOk;
}

void CLocation::OnBrowse() 
{
    CFileDialog fd(true);
    if (fd.DoModal() == IDOK) {
        char * local = HTLocalToWWW(fd.GetPathName());
        if (local) {
            SetDlgItemText(IDC_SOURCE_URI, local);
            HT_FREE(local);
        }

        if (m_appendSource) {
            CString file = fd.GetFileName();
        	if (file) {
	        m_destination += "edit/" + file; 
	        SetDlgItemText(IDC_DESTINATION_URI, m_destination);
	    }
            m_appendSource = FALSE;
        }
    }
}

