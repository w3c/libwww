// WinComDoc.cpp : implementation of the CWinComDoc class
//

#include "stdafx.h"
#include "WinCom.h"

#include "WinComDoc.h"

// From libwww
#include "WWWLib.h"
#include "WWWApp.h"


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
	m_pRequest = new CRequest(this);

}

CWinComDoc::~CWinComDoc()
{
        delete m_pRequest;
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

BOOL CWinComDoc::SubmitRequest()
{
    ASSERT(m_pRequest != NULL);

    // Create the anchors
    char * src = HTParse(m_Location.m_source, m_pRequest->m_cwd, PARSE_ALL);
    m_pRequest->m_pHTAnchorSource = HTAnchor_findAddress(src);
    HT_FREE(src);
    
    /* If destination is not http then error */
    if (!m_Location.m_destination.IsEmpty()) {
	char * access = HTParse(m_Location.m_destination, "", PARSE_ACCESS);
	if (strcasecomp(access, "http")) {
	    CString strMessage;
	    AfxFormatString1(strMessage, IDS_CANNOT_WRITE_TO_DESTINATION, m_Location.m_destination);
	    AfxMessageBox(strMessage);
	    HT_FREE(access);
	    return FALSE;
	} else {
	    char * dest = HTParse(m_Location.m_destination, m_pRequest->m_cwd, PARSE_ALL);
	    m_pRequest->m_pHTAnchorDestination = HTAnchor_findAddress(dest);
            HT_FREE(dest);
	    HT_FREE(access);
	}
    }

    
    // Do we have any metadata to set up?
    {
        HTParentAnchor * anchor = HTAnchor_parent(m_pRequest->m_pHTAnchorSource);
        
        if (!m_EntityInfo.m_mediaType.IsEmpty()) {
            HTAnchor_setFormat(anchor, HTAtom_for(m_EntityInfo.m_mediaType));
        }
        if (!m_EntityInfo.m_contentEncoding.IsEmpty()) {
            HTAnchor_deleteEncodingAll(anchor);
            HTAnchor_addEncoding(anchor, HTAtom_for(m_EntityInfo.m_contentEncoding));
        }
        if (!m_EntityInfo.m_charset.IsEmpty()) {
            HTAnchor_setCharset(anchor, HTAtom_for(m_EntityInfo.m_charset));
        }
        if (!m_EntityInfo.m_language.IsEmpty()) {
            HTAnchor_deleteLanguageAll(anchor);
            HTAnchor_addLanguage(anchor, HTAtom_for(m_EntityInfo.m_language));
        }
    }
    
    // Do we have any link relationships to set up?
    m_Links.AddLinkRelationships(m_pRequest->m_pHTAnchorSource);

    /* Set the cursor */
    BeginWaitCursor();
    
    /* Start the request */
    if (m_pRequest->PutDocument())
	EndWaitCursor();

    return TRUE;
}

BOOL CWinComDoc::CancelRequest()
{
    ASSERT(m_pRequest != NULL);

    /* Abort the request */
    m_pRequest->Cancel();

    /* End the wait cursor */
    EndWaitCursor();

    return TRUE;
}
