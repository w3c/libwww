// CLinks.cpp : implementation file
//

#include "stdafx.h"
#include "WinCom.h"
#include "Links.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinks property page

IMPLEMENT_DYNCREATE(CLinks, CPropertyPage)
CLinks::CLinks() : CPropertyPage(CLinks::IDD)
{
}

CLinks::CLinks( CRequest * pRequest) : CPropertyPage(CLinks::IDD)
{
	//{{AFX_DATA_INIT(CLinks)
	m_link = _T("");
	m_linkType = _T("");
	//}}AFX_DATA_INIT
    m_pRequest = pRequest;
    m_addLink = TRUE;
    pRequest->m_pLinks = this;
}

CLinks::~CLinks()
{
}

void CLinks::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinks)
	DDX_CBString(pDX, IDC_LINK, m_link);
	DDV_MaxChars(pDX, m_link, 2048);
	DDX_CBString(pDX, IDC_LINK_TYPE, m_linkType);
	DDV_MaxChars(pDX, m_linkType, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinks, CPropertyPage)
	//{{AFX_MSG_MAP(CLinks)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinks message handlers

BOOL CLinks::OnKillActive()
{
    BOOL bOk = CPropertyPage::OnKillActive();
    if ( bOk && m_addLink == TRUE) {
	HTParentAnchor * src = HTAnchor_parent(m_pRequest->m_pHTAnchorSource);
        if (m_linkType && !m_linkType.IsEmpty() && m_link && !m_link.IsEmpty()) {
            char * linktype = m_linkType.GetBuffer(16);
            char * link = m_link.GetBuffer(64);
            if (link && linktype) {
                char * link_addr = HTParse(link, "", PARSE_ALL);
                HTParentAnchor * dest = HTAnchor_parent(HTAnchor_findAddress(link_addr));

                /* Now add the new relation */
                HTLink_add((HTAnchor *) src, (HTAnchor *) dest,
                    (HTLinkType) HTAtom_caseFor(linktype),
                    METHOD_INVALID);
                
                /* Clean up */
                m_linkType.ReleaseBuffer();
                m_link.ReleaseBuffer();
                m_addLink = FALSE;
            }
        }
    }
    return bOk;
}
