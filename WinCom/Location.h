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

#if !defined(AFX_LOCATION_H__CF29CB7F_B627_11D1_93DF_080009DCA30B__INCLUDED_)
#define AFX_LOCATION_H__CF29CB7F_B627_11D1_93DF_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Location.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocation dialog

class CRequest;

class CLocation : public CPropertyPage
{
	DECLARE_DYNCREATE(CLocation)

// Construction
public:
        CLocation();
        ~CLocation();

	CString	    m_source;
	CString	    m_destination;

// Dialog Data
	//{{AFX_DATA(CLocation)
	enum { IDD = IDD_LOCATION };
	CComboBox	m_destinationList;
	CComboBox	m_sourceList;
	CButton	m_submit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLocation)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	protected:
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Generated message map functions
	//{{AFX_MSG(CLocation)
	afx_msg void OnBrowse();
	afx_msg void OnSubmit();
	afx_msg void OnEditchangeSourceUri();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCATION_H__CF29CB7F_B627_11D1_93DF_080009DCA30B__INCLUDED_)
