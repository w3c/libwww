// ProxySetup.cpp : implementation file
//

#include "stdafx.h"
#include "WinCom.h"
#include "ProxySetup.h"

// libwww code
#include "WWWApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProxySetup dialog

CProxySetup::CProxySetup(CWnd* pParent /*=NULL*/)
	: CDialog(CProxySetup::IDD, pParent)
{
    //{{AFX_DATA_INIT(CProxySetup)
    m_proxyPrefix = _T("http");
    //}}AFX_DATA_INIT
}

void CProxySetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProxySetup)
	DDX_CBString(pDX, IDC_PROXY_SERVER, m_proxy);
	DDX_CBString(pDX, IDC_USE_PROXY, m_proxyPrefix);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProxySetup, CDialog)
	//{{AFX_MSG_MAP(CProxySetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProxySetup message handlers

void CProxySetup::AddProxyOptionsToIni (void)
{
    CString strSection		= "Options";
    CString strProxyPrefixItem	= "Proxy-prefix";
    CString strProxyItem	= "Proxy";
    
    CWinApp * pApp = AfxGetApp();
    ASSERT(pApp != NULL);
    pApp->WriteProfileString(strSection, strProxyPrefixItem, m_proxyPrefix);
    pApp->WriteProfileString(strSection, strProxyItem, m_proxy);
}

BOOL CProxySetup::RegisterProxies (void)
{
    // Register the proxies in libwww
    HTProxy_deleteAll();
    if (!m_proxy.IsEmpty() && !m_proxyPrefix.IsEmpty()) {
        HTProxy_add(m_proxyPrefix, m_proxy);
        return TRUE;
    }
    return FALSE;
}

BOOL CProxySetup::GetProxyOptions(void)
{
    /* Read the INI file to find any persistent options */
    CString strSection		= "Options";
    CString strProxyPrefixItem	= "Proxy-prefix";
    CString strProxyItem	= "Proxy";
    
    CWinApp * pApp = AfxGetApp();
    ASSERT(pApp != NULL);
    CString proxyPrefix = pApp->GetProfileString(strSection, strProxyPrefixItem);
    if (!proxyPrefix.IsEmpty()) m_proxyPrefix = proxyPrefix;
    CString proxy = pApp->GetProfileString(strSection, strProxyItem);
    if (!proxy.IsEmpty()) m_proxy = proxy;
    return TRUE;
}

BOOL CProxySetup::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    UpdateData();
    
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
