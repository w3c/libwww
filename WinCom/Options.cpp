// Options.cpp : implementation file
//

#include "stdafx.h"
#include "WinCom.h"
#include "Options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptions property page

IMPLEMENT_DYNCREATE(COptions, CPropertyPage)

COptions::COptions() : CPropertyPage(COptions::IDD)
{
}

COptions::COptions( CWinComApp * pApp) : CPropertyPage(COptions::IDD)
{
	//{{AFX_DATA_INIT(COptions)
	m_proxy = _T("");
	m_proxy_prefix = _T("http");
	//}}AFX_DATA_INIT
}

COptions::~COptions()
{
}

void COptions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptions)
	DDX_CBString(pDX, IDC_PROXY, m_proxy);
	DDV_MaxChars(pDX, m_proxy, 1024);
	DDX_CBString(pDX, IDC_PROXY_PREFIX, m_proxy_prefix);
	DDV_MaxChars(pDX, m_proxy_prefix, 128);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptions, CPropertyPage)
	//{{AFX_MSG_MAP(COptions)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptions message handlers
