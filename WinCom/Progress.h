#if !defined(AFX_PROGRESS_H__EC9E0464_44D4_11D2_BA98_00A024AA28DE__INCLUDED_)
#define AFX_PROGRESS_H__EC9E0464_44D4_11D2_BA98_00A024AA28DE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Progress.h : header file
//

#include "Request.h"

/////////////////////////////////////////////////////////////////////////////
// CProgress dialog

class CProgress : public CDialog
{
// Construction
public:
	CProgress (CRequest * pRequest);

// Dialog Data
	//{{AFX_DATA(CProgress)
	enum { IDD = IDD_PROGRESS };
	CButton	m_cancel;
	CProgressCtrl	m_progress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CRequest *      m_pRequest;

	// Generated message map functions
	//{{AFX_MSG(CProgress)
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESS_H__EC9E0464_44D4_11D2_BA98_00A024AA28DE__INCLUDED_)
