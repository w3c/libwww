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

#if !defined(AFX_WINCOMDOC_H__CF29CB59_B627_11D1_93DF_080009DCA30B__INCLUDED_)
#define AFX_WINCOMDOC_H__CF29CB59_B627_11D1_93DF_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Links.h"
#include "EntityInfo.h"
#include "Location.h"
#include "ProxySetup.h"

class CRequest;

class CWinComDoc : public CDocument
{
protected: // create from serialization only
	CWinComDoc();
	DECLARE_DYNCREATE(CWinComDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinComDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL		AddRequest (CRequest * pRequest);
	BOOL		DeleteRequest (CRequest * pRequest);
    
	BOOL            SaveDocument ();

	char *		m_cwd;

//	CList		m_ActiveRequests;
	CProxySetup     m_ProxySetup;

	CLocation	m_Location;
	CEntityInfo     m_EntityInfo;
	CLinks		m_Links;

	virtual ~CWinComDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL            m_detectVersionConflict;
	BOOL            m_showServerStatus;

// Generated message map functions
protected:
	//{{AFX_MSG(CWinComDoc)
	afx_msg void OnVersionConflict();
	afx_msg void OnUpdateVersionConflict(CCmdUI* pCmdUI);
	afx_msg void OnShowServerStatus();
	afx_msg void OnUpdateShowServerStatus(CCmdUI* pCmdUI);
	afx_msg void OnAppUpgrade();
	afx_msg void OnLoadAddress();
	afx_msg void OnDeleteAddress();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINCOMDOC_H__CF29CB59_B627_11D1_93DF_080009DCA30B__INCLUDED_)
