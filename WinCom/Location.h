#if !defined(AFX_LOCATION_H__CF29CB7F_B627_11D1_93DF_080009DCA30B__INCLUDED_)
#define AFX_LOCATION_H__CF29CB7F_B627_11D1_93DF_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Location.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocation dialog

class CWinComApp;

class CLocation : public CPropertyPage
{
	DECLARE_DYNCREATE(CLocation)

// Construction
public:
	CLocation( CWinComApp *pApp );
        CWinComApp *m_pApp;
        CLocation();
        ~CLocation();

// Dialog Data
	//{{AFX_DATA(CLocation)
	enum { IDD = IDD_LOCATION };
	CString	m_destination;
	CString m_source;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLocation)
	public:
	protected:
            virtual BOOL OnKillActive();
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLocation)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCATION_H__CF29CB7F_B627_11D1_93DF_080009DCA30B__INCLUDED_)
