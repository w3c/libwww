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
**
**	TabsView.cpp : implementation of the CTabsView class
*/

#include "stdafx.h"
#include "WinCom.h"

#include "WinComDoc.h"
#include "TabsView.h"
#include "Location.h"
#include "EntityInfo.h"
#include "Links.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabsView

IMPLEMENT_DYNCREATE(CTabsView, CView)

BEGIN_MESSAGE_MAP(CTabsView, CView)
	//{{AFX_MSG_MAP(CTabsView)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP

	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabsView construction/destruction

CTabsView::CTabsView()
{
	//{{AFX_DATA_INIT(CTabsView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_bSizedBefore = FALSE;
	m_pPropSheet = NULL;
	m_pLocation = NULL;
	m_pEntityInfo = NULL;
	m_pLinks = NULL;

	m_hWndFocus = NULL;
}

CTabsView::~CTabsView()
{
	// explicitly delete sheet
	delete m_pPropSheet;
}

void CTabsView::DoDataExchange(CDataExchange* pDX)
{
	CView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabsView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTabsView printing


/////////////////////////////////////////////////////////////////////////////
// CTabsView diagnostics

#ifdef _DEBUG
void CTabsView::AssertValid() const
{
	CView::AssertValid();
}

void CTabsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWinComDoc* CTabsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinComDoc)));
	return (CWinComDoc*) m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTabsView message handlers

void CTabsView::OnSize(UINT nType, int cx, int cy)
{
	if (nType != SIZE_MINIMIZED && cx != 0 && cy != 0 && m_pPropSheet != NULL)
	{
		if (m_bSizedBefore == FALSE)
		{
			m_bSizedBefore = TRUE;

			// get the size of the property sheet
			CRect rectSized;
			m_pPropSheet->GetWindowRect(rectSized);
			rectSized.InflateRect(0,0,0,40);

			// calculate the size of the frame
			CFrameWnd* pFrame = GetParentFrame();
			if (pFrame != NULL)
			{
				pFrame->CalcWindowRect(rectSized,1);
				CWnd* pParent = pFrame->GetParent();

				if (pParent != NULL)
					pParent->ScreenToClient(rectSized);

				// resize and reposition the frame
				pFrame->MoveWindow(rectSized);
			}
		}
	}
}

BOOL CTabsView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
	DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	ASSERT(pParentWnd != NULL);
	ASSERT_KINDOF(CFrameWnd, pParentWnd);

	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle | WS_CLIPCHILDREN,
		rect, pParentWnd, nID, pContext))
	{
		return FALSE;
	}

	// add your pages here!

	m_pLocation = &GetDocument()->m_Location;
	m_pEntityInfo = &GetDocument()->m_EntityInfo;
	m_pLinks = &GetDocument()->m_Links;

	// create the window object

	m_pPropSheet = new CPropertySheet;
	m_pPropSheet->AddPage(m_pLocation);
	m_pPropSheet->AddPage(m_pEntityInfo);
	m_pPropSheet->AddPage(m_pLinks);

	// create a modeless property page
	if (!m_pPropSheet->Create(this,
			DS_CONTEXTHELP | DS_SETFONT | WS_CHILD | WS_VISIBLE))
	{
		DestroyWindow();
		return FALSE;
	}

	m_pPropSheet->SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);

	// we use the style from the template - but make sure that
	// the WS_BORDER bit is correct.
	// the WS_BORDER bit will be whatever is in dwRequestedStyle
	m_pPropSheet->ModifyStyle(WS_BORDER|WS_CAPTION,
		dwStyle & (WS_BORDER|WS_CAPTION));

	// Force the size requested.
	// Fake a call to OnSize()--it would have been called automatically
	// if this were using the base class implementation of Create().
	CFrameWnd* pParentFrame = GetParentFrame();
	CRect rectSize;
	m_pPropSheet->GetWindowRect(rectSize);
	pParentFrame->CalcWindowRect(rectSize);
	OnSize(SIZE_RESTORED, rectSize.Width(), rectSize.Height());
	return TRUE;
}

BOOL CTabsView::OnEraseBkgnd(CDC* pDC)
{
	CBrush* pOldBrush;

	CBrush backBrush;
	backBrush.CreateSolidBrush(::GetSysColor(COLOR_3DLIGHT));
	backBrush.UnrealizeObject();

	CRect rectClient;
	GetClientRect(rectClient);

	pOldBrush = pDC->SelectObject(&backBrush);
	pDC->Rectangle(rectClient);

	pDC->SelectObject(pOldBrush);
	return TRUE;
}

void CTabsView::OnDraw(CDC* pDC)
{
	// we don't draw because the child window will do it all
}

void CTabsView::OnActivateView(
	BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if (SaveFocusControl())
		return;     // don't call base class when focus is already set

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CTabsView::OnActivateFrame(UINT nState, CFrameWnd* /*pFrameWnd*/)
{
	if (nState == WA_INACTIVE)
		SaveFocusControl();     // save focus when frame loses activation
}

BOOL CTabsView::SaveFocusControl()
{
	// save focus window if focus is on this window's controls
	HWND hWndFocus = ::GetFocus();
	if (hWndFocus != NULL && ::IsChild(m_hWnd, hWndFocus))
	{
		m_hWndFocus = hWndFocus;
		return TRUE;
	}
	return FALSE;
}

void CTabsView::OnSetFocus(CWnd*)
{
	if (!::IsWindow(m_hWndFocus))
	{
		// invalid or unknown focus window... let windows handle it
		m_hWndFocus = NULL;
		Default();
		return;
	}

	// otherwise, set focus to the last known focus window
	::SetFocus(m_hWndFocus);
}
