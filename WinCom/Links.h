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

#if !defined(AFX_CLinks_H__4DEC1954_F40B_11D1_93EC_080009DCA30B__INCLUDED_)
#define AFX_CLinks_H__4DEC1954_F40B_11D1_93EC_080009DCA30B__INCLUDED_

#include "resource.h"       // main symbols
#include "LinkView.h"
#include "WWWCore.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CLinks.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLinks dialog

class CLinks : public CPropertyPage
{
	DECLARE_DYNCREATE(CLinks)

// Construction
public:
        CLinks();
	~CLinks();

// Operations
        BOOL AddLinkRelationships (HTAnchor * source);
	void Clear(void);

// Dialog Data
	//{{AFX_DATA(CLinks)
	enum { IDD = IDD_LINKS };
	CComboBox	m_linkDestination;
	CButton	m_linkRemove;
	CButton	m_linkAdd;
	CLinkView * 	m_pLinkList;
	CString	m_linkType;
	int		m_direction;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLinks)
	public:
	virtual BOOL OnInitDialog();
	protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_LargeImageList;
	CImageList m_SmallImageList;
	CImageList m_StateImageList;

// flags and states
protected:
	BOOL m_bStateIcons;
	int m_nCheckedItem;
	HICON m_hSmallIcon;
	HICON m_hLargeIcon;

	void CheckItem(int nNewCheckedItem);
	// Generated message map functions
	//{{AFX_MSG(CLinks)
	afx_msg void OnLinkAdd();
	afx_msg void OnLinkRemove();
	afx_msg void OnItemclickLinkRels(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLinks_H__4DEC1954_F40B_11D1_93EC_080009DCA30B__INCLUDED_)
