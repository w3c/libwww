#if !defined(AFX_OPTIONS_H__CF29CB7E_B627_11D1_93DF_080009DCA30B__INCLUDED_)
#define AFX_OPTIONS_H__CF29CB7E_B627_11D1_93DF_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Options.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptions dialog

class CWinComApp;

class COptions : public CPropertyPage
{
    DECLARE_DYNCREATE(COptions)
        
        // Construction
public:
    COptions( CWinComApp *pApp );
    CWinComApp *m_pApp;
    COptions();
    ~COptions();
    
    // Dialog Data
    //{{AFX_DATA(COptions)
    enum { IDD = IDD_OPTIONS };
    CString	m_proxy;
    CString	m_proxy_prefix;
    //}}AFX_DATA
    
    
    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(COptions)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
    
    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(COptions)
    // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
        
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONS_H__CF29CB7E_B627_11D1_93DF_080009DCA30B__INCLUDED_)
