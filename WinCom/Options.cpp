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
**	Options.cpp : implementation file
*/

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
    m_pApp = pApp;
    pApp->m_pOptions = this;
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
