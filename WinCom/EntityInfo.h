#if !defined(AFX_ENTITYINFO_H__CF29CB80_B627_11D1_93DF_080009DCA30B__INCLUDED_)
#define AFX_ENTITYINFO_H__CF29CB80_B627_11D1_93DF_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EntityInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EntityInfo dialog

class CWinComApp;

class CEntityInfo : public CPropertyPage
{
	DECLARE_DYNCREATE(CEntityInfo)

// Construction
public:
        CEntityInfo( CWinComApp *pApp );
        CWinComApp *m_pApp;
        CEntityInfo();
	~CEntityInfo();

// Dialog Data
	//{{AFX_DATA(CEntityInfo)
	enum { IDD = IDD_ENTITY_INFO };
	CString	m_contentEncoding;
	long	m_contentLength;
	CString	m_charset;
	CString	m_language;
	CString	m_mediaType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEntityInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEntityInfo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTITYINFO_H__CF29CB80_B627_11D1_93DF_080009DCA30B__INCLUDED_)
