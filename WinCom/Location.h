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
	CString	    m_saveAs;

// Operations       
        void OnFinish (void);

// Dialog Data
	//{{AFX_DATA(CLocation)
	enum { IDD = IDD_LOCATION };
	CComboBox	m_destinationList;
	CComboBox	m_sourceList;
	CButton	m_submit;
	CButton	m_cancel;
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

// Implementation
protected:
	void CheckSubmit (void);

// Generated message map functions
	//{{AFX_MSG(CLocation)
	afx_msg void OnBrowse();
	afx_msg void OnEditSourceLocation();
	afx_msg void OnEditDestinationLocation();
	afx_msg void OnSubmit();
	afx_msg void OnCancel();
	afx_msg void OnKillfocusSourceUri();
	afx_msg void OnKillfocusDestinationUri();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCATION_H__CF29CB7F_B627_11D1_93DF_080009DCA30B__INCLUDED_)
