// Progress.cpp : implementation file
//

#include "stdafx.h"
#include "wincom.h"
#include "Progress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgress dialog

CProgress::CProgress( CRequest * pRequest ) : CDialog(CProgress::IDD)
{
    ASSERT(pRequest != NULL);
    m_pRequest = pRequest;

	//{{AFX_DATA_INIT(CProgress)
	//}}AFX_DATA_INIT
}


void CProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgress)
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgress, CDialog)
	//{{AFX_MSG_MAP(CProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgress message handlers

void CProgress::OnCancel() 
{
    CDialog::OnCancel();
    m_pRequest->Cancel();
}

