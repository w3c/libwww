// WinComDoc.h : interface of the CWinComDoc class
//
/////////////////////////////////////////////////////////////////////////////

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
	BOOL            SubmitRequest();
	BOOL            LoadRequest();
	BOOL            CancelRequest();

	char *		m_cwd;
	CRequest *	m_pRequest;
	CEntityInfo     m_EntityInfo;
	CLinks		m_Links;
	CLocation	m_Location;
	CProxySetup     m_ProxySetup;

	virtual ~CWinComDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL            m_detectVersionConflict;

// Generated message map functions
protected:
	//{{AFX_MSG(CWinComDoc)
	afx_msg void OnVersionConflict();
	afx_msg void OnUpdateVersionConflict(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINCOMDOC_H__CF29CB59_B627_11D1_93DF_080009DCA30B__INCLUDED_)
