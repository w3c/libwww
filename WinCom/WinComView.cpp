// WinComView.cpp : implementation of the CWinComView class
//

#include "stdafx.h"
#include "WinCom.h"

#include "WinComDoc.h"
#include "WinComView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinComView

IMPLEMENT_DYNCREATE(CWinComView, CView)

BEGIN_MESSAGE_MAP(CWinComView, CView)
	//{{AFX_MSG_MAP(CWinComView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinComView construction/destruction

CWinComView::CWinComView()
{
	// TODO: add construction code here

}

CWinComView::~CWinComView()
{
}

BOOL CWinComView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CWinComView drawing

void CWinComView::OnDraw(CDC* pDC)
{
	CWinComDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
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
	CView::AssertValid();
}

void CWinComView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWinComDoc* CWinComView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinComDoc)));
	return (CWinComDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinComView message handlers
