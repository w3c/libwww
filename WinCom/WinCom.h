// WinCom.h : main header file for the WINCOM application
//

#if !defined(AFX_WINCOM_H__CF29CB53_B627_11D1_93DF_080009DCA30B__INCLUDED_)
#define AFX_WINCOM_H__CF29CB53_B627_11D1_93DF_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Request.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CWinComApp:
// See WinCom.cpp for the implementation of this class
//

class CWinComApp : public CWinApp
{
public:
	CWinComApp();
	CRequest Request;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinComApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWinComApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINCOM_H__CF29CB53_B627_11D1_93DF_080009DCA30B__INCLUDED_)
