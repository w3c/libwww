// WinCom.h : main header file for the WINCOM application
//

#if !defined(AFX_WINCOM_H__CF29CB53_B627_11D1_93DF_080009DCA30B__INCLUDED_)
#define AFX_WINCOM_H__CF29CB53_B627_11D1_93DF_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxadv.h>

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Request.h"	// Added by ClassView
#include "ProxySetup.h"	// Added by ClassView
#include "EntityInfo.h"	// Added by ClassView
#include "Location.h"	// Added by ClassView

#define MAX_LIST_LENGTH	8

/////////////////////////////////////////////////////////////////////////////
// CWinComApp:
// See WinCom.cpp for the implementation of this class
//

class CWinComApp : public CWinApp
{
public:
	CWinComApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinComApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Operations
	void AddSourceToIniFile(LPCTSTR lpszPathName);
	int  GetSourceIniListSize(void);
	int  FillSourceComboBox (CComboBox * pBox);

	void AddDestinationToIniFile(LPCTSTR lpszPathName);
	int  GetDestinationIniListSize(void);
	int  FillDestinationComboBox (CComboBox * pBox);

	void AddLinkToIniFile(LPCTSTR lpszPathName);
	int  GetLinkIniListSize(void);
	int  FillLinkComboBox (CComboBox * pBox);

	BOOL SetIniCWD (CString cwd);
	CString GetIniCWD (void);

	BOOL SetIniDetectVersionConflict (BOOL detect);
	BOOL GetIniDetectVersionConflict (void);

	BOOL SetIniShowServerStatus (BOOL show);
	BOOL GetIniShowServerStatus (void);

// Implementation

	//{{AFX_MSG(CWinComApp)
	afx_msg void OnAppAbout();
	afx_msg void OnOptionsProxies();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CString		m_currentDir;
	CProxySetup	ProxySetup;
	CRecentFileList	* m_pSourceList;
	CRecentFileList * m_pDestinationList;
	CRecentFileList * m_pLinkList;
	BOOL            m_detectVersionConflict;
	BOOL            m_showServerStatus;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINCOM_H__CF29CB53_B627_11D1_93DF_080009DCA30B__INCLUDED_)
