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

#if !defined(AFX_ENTITYINFO_H__CF29CB80_B627_11D1_93DF_080009DCA30B__INCLUDED_)
#define AFX_ENTITYINFO_H__CF29CB80_B627_11D1_93DF_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EntityInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EntityInfo dialog

class CRequest;

class CEntityInfo : public CPropertyPage
{
	DECLARE_DYNCREATE(CEntityInfo)
        
// Construction
public:
	void Clear(void);
	CEntityInfo();
	~CEntityInfo();
	
// Dialog Data
	//{{AFX_DATA(CEntityInfo)
	enum { IDD = IDD_ENTITY_INFO };
	CButton	m_guess;
	CString	m_charset;
	CString m_contentEncoding;
	long    m_contentLength;
	CString m_language;
	CString m_mediaType;
	CTime   m_lastModified;
	long	m_expires;
	CString	m_etag;
	CString	m_lmDate;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEntityInfo)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEntityInfo)
	afx_msg void OnGetInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
        
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTITYINFO_H__CF29CB80_B627_11D1_93DF_080009DCA30B__INCLUDED_)
