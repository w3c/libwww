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
	CPropertyPage* m_pLoad;
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
