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

#if !defined(AFX_LOAD_H__093300C6_43F1_11D2_93FE_080009DCA30B__INCLUDED_)
#define AFX_LOAD_H__093300C6_43F1_11D2_93FE_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Load.h : header file
//
#include "WinComDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CLoad dialog

class CLoad : public CDialog
{

// Construction
public:
        CLoad(CWinComDoc * pDoc);

// Dialog Data
	//{{AFX_DATA(CLoad)
	enum { IDD = IDD_LOAD };
	CComboBox	m_loadList;
	int		m_cacheValidate;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoad)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   	CString m_address;
	CWinComDoc * m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CLoad)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOAD_H__093300C6_43F1_11D2_93FE_080009DCA30B__INCLUDED_)
