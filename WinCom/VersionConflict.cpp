// VersionConflict.cpp : implementation file
//

#include "stdafx.h"
#include "wincom.h"
#include "VersionConflict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVersionConflict dialog


CVersionConflict::CVersionConflict(CWnd* pParent /*=NULL*/)
	: CDialog(CVersionConflict::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVersionConflict)
	m_versionResolution = 0;
	//}}AFX_DATA_INIT
}


void CVersionConflict::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVersionConflict)
	DDX_Radio(pDX, IDC_RESOLVE_CONFLICT1, m_versionResolution);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVersionConflict, CDialog)
	//{{AFX_MSG_MAP(CVersionConflict)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVersionConflict message handlers
