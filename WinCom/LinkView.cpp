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
**	LinkView.cpp : implementation file
*/

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
