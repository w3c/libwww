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

#if !defined(AFX_LINKVIEW_H__0AC36CC1_12F6_11D2_BA98_00A024AA28DE__INCLUDED_)
#define AFX_LINKVIEW_H__0AC36CC1_12F6_11D2_BA98_00A024AA28DE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LinkView.h : header file
//

#include "ListVwex.h"
class CLinks;

/////////////////////////////////////////////////////////////////////////////
// CLinkView view

class CLinkView : public CListViewEx
{
	DECLARE_DYNCREATE(CLinkView)

public:
	CLinkView();

protected:
	CLinks * m_pLinks;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkView)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetLink(CLinks * pLinks);
	virtual ~CLinkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CLinkView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKVIEW_H__0AC36CC1_12F6_11D2_BA98_00A024AA28DE__INCLUDED_)
