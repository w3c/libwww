#if !defined(AFX_LOAD_H__093300C6_43F1_11D2_93FE_080009DCA30B__INCLUDED_)
#define AFX_LOAD_H__093300C6_43F1_11D2_93FE_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Load.h : header file
//
#include "WinComDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CLoad dialog

class CLoad : public CDialog
{

// Construction
public:
        CLoad(CWinComDoc * pDoc);

// Dialog Data
	//{{AFX_DATA(CLoad)
	enum { IDD = IDD_LOAD };
	CComboBox	m_loadList;
	int		m_cacheValidate;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoad)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   	CString m_address;
	CWinComDoc * m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CLoad)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOAD_H__093300C6_43F1_11D2_93FE_080009DCA30B__INCLUDED_)
