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
*/

#if !defined(AFX_PROXYSETUP_H__AA2EFDC9_015B_11D2_93F2_080009DCA30B__INCLUDED_)
#define AFX_PROXYSETUP_H__AA2EFDC9_015B_11D2_93F2_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ProxySetup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProxySetup dialog

class CProxySetup : public CDialog
{
// Construction
public:
	BOOL GetProxyOptions();
	CProxySetup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProxySetup)
	enum { IDD = IDD_PROXIES };
	CString	m_proxy;
	CString	m_proxyPrefix;
	CString	m_proxyException;
	//}}AFX_DATA

// Operations
	void AddProxyOptionsToIni (void);
	BOOL RegisterProxies (void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProxySetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProxySetup)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROXYSETUP_H__AA2EFDC9_015B_11D2_93F2_080009DCA30B__INCLUDED_)
