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

CLocation::CLocation( CWinComApp * pApp) : CPropertyPage(CLocation::IDD)
{
	//{{AFX_DATA_INIT(CLocation)
	m_destination = _T("http://");
	m_source = _T("");
	//}}AFX_DATA_INIT
        m_pApp = pApp;
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

BOOL CLocation::OnKillActive() 
{
    BOOL bOk = CPropertyPage::OnKillActive();
    if ( bOk ) {

        /* If the soruce is a local file then check it out */
        if (m_source && !m_source.IsEmpty()) {
            CFile src_file;
            if (src_file.Open(m_source, CFile::modeRead)) {
                LPTSTR src_str = m_source.GetBuffer( 64 );
                char * src = HTParse(src_str, m_pApp->Request.mp_cwd, PARSE_ALL);
                HTAnchor * src_anchor = HTAnchor_findAddress(src);
                HTParentAnchor * src_parent = HTAnchor_parent(src_anchor);
                HTBind_getAnchorBindings(src_parent);
                m_source.ReleaseBuffer();

#if 0
                /* Update the bindings in the Entity Information Window */
                {
                    char * mt = HTAtom_name(HTAnchor_format(src_parent));
                    char * charset = HTAtom_name(HTAnchor_charset(src_parent));
                    long length = HTAnchor_length(src_parent);

                    SetDlgItemText(IDC_MEDIA_TYPE, mt);
                    m_mediaType = mt;

                    SetDlgItemText(IDC_MEDIA_CHARSET, charset);
                    m_charset = charset;

                    SetDlgItemInt(IDC_CONTENT_LENGTH, length);
                    m_length = length

                    SetDlgItemText(IDC_CONTENT_ENCODING, HTAtom_name(HTAnchor_encoding(src_parent)));
                }
#endif
                src_file.Close();
            }
 
        }

        /* If destination is a local file then error */
        if (m_destination && !m_destination.IsEmpty()) {
            CFile dst_file;
            if (dst_file.Open(m_destination, CFile::modeRead)) {
                CString strMessage;
                AfxFormatString1(strMessage, IDS_CANNOT_WRITE_TO_FILE, m_destination);
                AfxMessageBox(strMessage);
                dst_file.Close();
                return FALSE;
            }
        }
    }
    return bOk;
}

BEGIN_MESSAGE_MAP(CLocation, CPropertyPage)
	//{{AFX_MSG_MAP(CLocation)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocation message handlers
