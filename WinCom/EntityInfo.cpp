// EntityInfo.cpp : implementation file
//

#include "stdafx.h"
#include "WinCom.h"
#include "EntityInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEntityInfo property page

IMPLEMENT_DYNCREATE(CEntityInfo, CPropertyPage)

CEntityInfo::CEntityInfo() : CPropertyPage(CEntityInfo::IDD)
{
}

CEntityInfo::CEntityInfo( CWinComApp * pApp) : CPropertyPage(CEntityInfo::IDD)
{
	//{{AFX_DATA_INIT(CEntityInfo)
	m_contentEncoding = _T("");
	m_contentLength = 0;
	m_charset = _T("");
	m_language = _T("");
	m_mediaType = _T("");
	//}}AFX_DATA_INIT
}

CEntityInfo::~CEntityInfo()
{
}

void CEntityInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEntityInfo)
	DDX_Text(pDX, IDC_CONTENT_ENCODING, m_contentEncoding);
	DDX_Text(pDX, IDC_CONTENT_LENGTH, m_contentLength);
	DDX_Text(pDX, IDC_MEDIA_CHARSET, m_charset);
	DDX_Text(pDX, IDC_MEDIA_LANGUAGE, m_language);
	DDX_Text(pDX, IDC_MEDIA_TYPE, m_mediaType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEntityInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CEntityInfo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEntityInfo message handlers
