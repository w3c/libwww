#if !defined(AFX_PASSWORD_H__B7CDBAE0_C4DC_11D1_93E1_080009DCA30B__INCLUDED_)
#define AFX_PASSWORD_H__B7CDBAE0_C4DC_11D1_93E1_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Password.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Password dialog

class Password : public CDialog
{
// Construction
public:
	Password(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Password)
	enum { IDD = IDD_PASSWORD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Password)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Password)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORD_H__B7CDBAE0_C4DC_11D1_93E1_080009DCA30B__INCLUDED_)
