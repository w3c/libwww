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
**	WinComView.cpp : implementation of the CWinComView class
*/

#include "stdafx.h"
#include "WinCom.h"

#include "WinComDoc.h"
#include "WinComView.h"

#include "Location.h"
#include "EntityInfo.h"
#include "Links.h"
#include "Password.h"

// From libwww
#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinComView

IMPLEMENT_DYNCREATE(CWinComView, CFormView)

BEGIN_MESSAGE_MAP(CWinComView, CFormView)
	//{{AFX_MSG_MAP(CWinComView)
	ON_BN_CLICKED(IDOK, OnSubmit)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinComView construction/destruction

CWinComView::CWinComView()
    : CFormView(CWinComView::IDD)
{
	//{{AFX_DATA_INIT(CWinComView)
	//}}AFX_DATA_INIT
}

CWinComView::~CWinComView()
{
}

BOOL CWinComView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CWinComView::OnInitialUpdate()
{
        CFormView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CWinComView drawing

void CWinComView::OnDraw(CDC* pDC)
{
	CWinComDoc * doc = GetDocument();
	ASSERT_VALID(doc);
	
    GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	
	// Create the tab pages
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
    TabCtrlItem.pszText = "Location";
	m_Tabs.InsertItem( 0, &TabCtrlItem );
#if 0
    TabCtrlItem.pszText = "Natural Gas";
	m_Tabs.InsertItem( 1, &TabCtrlItem );
    TabCtrlItem.pszText = "Kryptonite";
	m_Tabs.InsertItem( 2, &TabCtrlItem );
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CWinComView printing

BOOL CWinComView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWinComView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWinComView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CWinComView diagnostics

#ifdef _DEBUG
void CWinComView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWinComView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CWinComDoc* CWinComView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinComDoc)));
	return (CWinComDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinComView message handlers

