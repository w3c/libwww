// LinkView.cpp : implementation file
//

#include "stdafx.h"
#include "wincom.h"
#include "LinkView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinkView contruction/destruction

IMPLEMENT_DYNCREATE(CLinkView, CListView)

CLinkView::CLinkView()
{
    m_pLinks = NULL;
}

CLinkView::~CLinkView()
{
}

BEGIN_MESSAGE_MAP(CLinkView, CListView)
	//{{AFX_MSG_MAP(CLinkView)
	ON_WM_SIZE()
	ON_WM_PAINT()
        ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkView diagnostics

#ifdef _DEBUG
void CLinkView::AssertValid() const
{
	CListView::AssertValid();
}

void CLinkView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLinkView message handlers

BOOL CLinkView::SetLink(CLinks * pLinks)
{
    // because I can't figure out how to initialize the stupid object
    m_pLinks = pLinks;
    ModifyStyle(WS_VSCROLL, 0);
    return TRUE;
}

void CLinkView::OnSetFocus(CWnd* pOldWnd) 
{
    CListViewEx::OnSetFocus(pOldWnd);

    // Enable the "remove" key in the Links view
    ASSERT(m_pLinks != NULL);
    m_pLinks->m_linkRemove.EnableWindow(TRUE);
}

void CLinkView::OnKillFocus(CWnd* pNewWnd) 
{
    CListView::OnKillFocus(pNewWnd);
    
    // Disable the "remove" key in the Links view
    // ASSERT(m_pLinks != NULL);
    // m_pLinks->m_linkRemove.EnableWindow(FALSE);
}
