#if !defined(AFX_VERSIONCONFLICT_H__AAC0C504_18AB_11D2_BA98_00A024AA28DE__INCLUDED_)
#define AFX_VERSIONCONFLICT_H__AAC0C504_18AB_11D2_BA98_00A024AA28DE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VersionConflict.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVersionConflict dialog

class CVersionConflict : public CDialog
{
// Construction
public:
	CVersionConflict(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVersionConflict)
	enum { IDD = IDD_VERSION_CONFLICT };
	int		m_versionResolution;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVersionConflict)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVersionConflict)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VERSIONCONFLICT_H__AAC0C504_18AB_11D2_BA98_00A024AA28DE__INCLUDED_)
