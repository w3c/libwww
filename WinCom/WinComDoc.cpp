// WinComDoc.cpp : implementation of the CWinComDoc class
//

#include "stdafx.h"
#include "WinCom.h"

#include "WinComDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinComDoc

IMPLEMENT_DYNCREATE(CWinComDoc, CDocument)

BEGIN_MESSAGE_MAP(CWinComDoc, CDocument)
	//{{AFX_MSG_MAP(CWinComDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinComDoc construction/destruction

CWinComDoc::CWinComDoc()
{
	// TODO: add one-time construction code here

}

CWinComDoc::~CWinComDoc()
{
}

BOOL CWinComDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWinComDoc serialization

void CWinComDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWinComDoc diagnostics

#ifdef _DEBUG
void CWinComDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWinComDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinComDoc commands
