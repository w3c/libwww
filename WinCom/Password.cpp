// Password.cpp : implementation file
//

#include "stdafx.h"
#include "WinCom.h"
#include "Password.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPassword dialog


CPassword::CPassword( CUserParameters * UserParameters ) : CDialog(CPassword::IDD)
{
    char * realm = UserParameters && UserParameters->m_input ?
        (char *) UserParameters->m_input : "";
    if (!realm) realm = "";
    char * uid = UserParameters && UserParameters->m_default ?
        (char *) UserParameters->m_default : "";
    if (!uid) uid = "";
    //{{AFX_DATA_INIT(CPassword)
	m_password = _T("");
	m_username = _T(uid);
	m_realm = _T(realm);
	//}}AFX_DATA_INIT
}


void CPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassword)
	DDX_Text(pDX, IDC_PASSWORD, m_password);
	DDX_Text(pDX, IDC_UID, m_username);
	DDX_Text(pDX, IDC_REALM, m_realm);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassword, CDialog)
	//{{AFX_MSG_MAP(CPassword)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassword message handlers
