#if !defined(AFX_CACHESETUP_H__D30CB672_8A5B_11D2_A224_0060B01B1BFD__INCLUDED_)
#define AFX_CACHESETUP_H__D30CB672_8A5B_11D2_A224_0060B01B1BFD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CacheSetup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCacheSetup dialog

class CCacheSetup : public CDialog
{
// Construction
public:
	CCacheSetup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCacheSetup)
	enum { IDD = IDD_CACHE_SETUP };
	CSpinButtonCtrl	m_cacheSpinT;
	CSpinButtonCtrl	m_cacheSpinS;
	CButton	m_cacheFlush;
	BOOL	m_cacheEnable;
	CString	m_cacheRoot;
	int	m_sizeT;
	int	m_sizeS;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCacheSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Operations
public:
	BOOL Init (void);
	BOOL Terminate (void);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCacheSetup)
	virtual BOOL OnInitDialog();
	afx_msg void OnCacheFlush();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CACHESETUP_H__D30CB672_8A5B_11D2_A224_0060B01B1BFD__INCLUDED_)
