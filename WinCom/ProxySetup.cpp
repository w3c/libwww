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
**	ProxySetup.cpp : implementation file
*/

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
        m_proxyPrefix = _T("http://*");
	m_proxyException = _T("");
	//}}AFX_DATA_INIT
}

void CProxySetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProxySetup)
	DDX_CBString(pDX, IDC_PROXY_SERVER, m_proxy);
	DDX_CBString(pDX, IDC_USE_PROXY, m_proxyPrefix);
	DDX_CBString(pDX, IDC_USE_NO_PROXY, m_proxyException);
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
    CString strExceptionItem    = "Proxy-exception";
    
    CWinApp * pApp = AfxGetApp();
    ASSERT(pApp != NULL);
    pApp->WriteProfileString(strSection, strProxyPrefixItem, m_proxyPrefix);
    pApp->WriteProfileString(strSection, strProxyItem, m_proxy);
    pApp->WriteProfileString(strSection, strExceptionItem, m_proxyException);
}

BOOL CProxySetup::RegisterProxies (void)
{
    // Delete all Registered proxies in libwww
    HTProxy_deleteAll();
    HTNoProxy_deleteAll();

    // regex for what to proxy
    if (!m_proxy.IsEmpty() && !m_proxyPrefix.IsEmpty()) {
        HTProxy_addRegex(m_proxyPrefix, m_proxy, -1);
    }

    // regex for what not to proxy
    if (!m_proxyException.IsEmpty()) {
        HTNoProxy_addRegex(m_proxyException, -1);
    }
    
    return TRUE;
}

BOOL CProxySetup::GetProxyOptions(void)
{
    /* Read the INI file to find any persistent options */
    CString strSection		= "Options";
    CString strProxyPrefixItem	= "Proxy-prefix";
    CString strProxyItem	= "Proxy";
    CString strExceptionItem    = "Proxy-exception";
    
    CWinApp * pApp = AfxGetApp();
    ASSERT(pApp != NULL);

    CString proxyPrefix = pApp->GetProfileString(strSection, strProxyPrefixItem);
    if (!proxyPrefix.IsEmpty()) m_proxyPrefix = proxyPrefix;

    CString proxy = pApp->GetProfileString(strSection, strProxyItem);
    if (!proxy.IsEmpty()) m_proxy = proxy;

    CString proxyException = pApp->GetProfileString(strSection, strExceptionItem);
    if (!proxyException.IsEmpty()) m_proxyException = proxyException;

    return TRUE;
}

BOOL CProxySetup::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    UpdateData();
    
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
