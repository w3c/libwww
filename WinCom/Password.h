#if !defined(AFX_PASSWORD_H__B7CDBAE1_C4DC_11D1_93E1_080009DCA30B__INCLUDED_)
#define AFX_PASSWORD_H__B7CDBAE1_C4DC_11D1_93E1_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Password.h : header file
//

#include "UserParameters.h"

/////////////////////////////////////////////////////////////////////////////
// CPassword dialog

class CPassword : public CDialog
{

// Construction
public:
	CPassword(CUserParameters * UserParameters);

// Dialog Data
	//{{AFX_DATA(CPassword)
	enum { IDD = IDD_PASSWORD };
	CString	m_password;
	CString	m_username;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassword)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPassword)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PASSWORD_H__B7CDBAE1_C4DC_11D1_93E1_080009DCA30B__INCLUDED_)

