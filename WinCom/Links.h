#if !defined(AFX_CLinks_H__4DEC1954_F40B_11D1_93EC_080009DCA30B__INCLUDED_)
#define AFX_CLinks_H__4DEC1954_F40B_11D1_93EC_080009DCA30B__INCLUDED_

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
	bool m_addLink;
	int m_linkTypeIndex;
	CLinks( CRequest *pRequest );
        CLinks();
	~CLinks();

// Dialog Data
	//{{AFX_DATA(CLinks)
	enum { IDD = IDD_LINKS };
	CString	m_link;
	CString	m_linkType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLinks)
	protected:
	    virtual BOOL OnKillActive();
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

       	CRequest * m_pRequest;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLinks)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLinks_H__4DEC1954_F40B_11D1_93EC_080009DCA30B__INCLUDED_)
