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

#if !defined(AFX_DELETE_H__363EE1A4_4595_11D2_BA98_00A024AA28DE__INCLUDED_)
#define AFX_DELETE_H__363EE1A4_4595_11D2_BA98_00A024AA28DE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Delete.h : header file
//
#include "WinComDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDelete dialog

class CDelete : public CDialog
{

// Construction
public:
        CDelete(CWinComDoc * pDoc);

// Dialog Data
	//{{AFX_DATA(CDelete)
	enum { IDD = IDD_DELETE };
	CComboBox	m_deleteList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelete)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   	CString m_address;
	CWinComDoc * m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CDelete)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETE_H__363EE1A4_4595_11D2_BA98_00A024AA28DE__INCLUDED_)
