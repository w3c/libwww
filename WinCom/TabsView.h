// TabsView.h : interface of the CTabsView class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////

class CTabsView : public CView
{
protected: // create from serialization only
	CTabsView();
	DECLARE_DYNCREATE(CTabsView)

public:
	//{{AFX_DATA(CTabsView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CWinComDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabsView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL
	virtual void OnActivateFrame(UINT nState, CFrameWnd* /*pFrameWnd*/);
	BOOL SaveFocusControl();
	HWND m_hWndFocus;

// Implementation
public:
	virtual ~CTabsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// tracks whether parent frame has already been sized.
	BOOL m_bSizedBefore;

	// property sheet is wired to MDI child frame and is not displayed
	CPropertySheet* m_pPropSheet;

	// one page for each menu so we can initialize controls
	// using OnInitDialog
	CPropertyPage* m_pLocation;
	CPropertyPage* m_pEntityInfo;
	CPropertyPage* m_pLinks;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDraw(CDC* pDC);

// Generated message map functions
protected:
	//{{AFX_MSG(CTabsView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TabsView.cpp
inline CWinComDoc* CTabsView::GetDocument()
   { return (CWinComDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
