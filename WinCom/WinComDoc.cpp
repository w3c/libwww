/*
**	@(#) $Id$
**	
**	W3C Web Commander can be found at "http://www.w3.org/WinCom/"
**	
**	Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**	WinComDoc.cpp : implementation of the CWinComDoc class
*/

#include "stdafx.h"
#include "WinCom.h"

#include "WinComDoc.h"
#include "Load.h"
#include "Delete.h"

// From libwww
#include "WWWLib.h"
#include "WWWApp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WEBCOMMANDER_LATEST     "http://www.w3.org/WinCom/WinCom.zip"
#define WEBCOMMANDER_THIS       "http://www.w3.org/WinCom/WinCom-0.984.zip"

/////////////////////////////////////////////////////////////////////////////
// CWinComDoc

IMPLEMENT_DYNCREATE(CWinComDoc, CDocument)

BEGIN_MESSAGE_MAP(CWinComDoc, CDocument)
	//{{AFX_MSG_MAP(CWinComDoc)
	ON_COMMAND(ID_VERSION_CONFLICT, OnVersionConflict)
	ON_UPDATE_COMMAND_UI(ID_VERSION_CONFLICT, OnUpdateVersionConflict)
	ON_COMMAND(ID_SHOW_SERVER_STATUS, OnShowServerStatus)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SERVER_STATUS, OnUpdateShowServerStatus)
	ON_COMMAND(ID_APP_UPGRADE, OnAppUpgrade)
	ON_COMMAND(ID_LOAD_ADDRESS, OnLoadAddress)
	ON_COMMAND(ID_DELETE_ADDRESS, OnDeleteAddress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinComDoc construction/destruction

CWinComDoc::CWinComDoc()
{
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 
    
    m_cwd = HTGetCurrentDirectoryURL();
    m_detectVersionConflict = pApp->GetIniDetectVersionConflict();
    m_showServerStatus = pApp->GetIniShowServerStatus();
}

CWinComDoc::~CWinComDoc()
{
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 

    HT_FREE(m_cwd);
    pApp->SetIniDetectVersionConflict(m_detectVersionConflict);
    pApp->SetIniShowServerStatus(m_showServerStatus);
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
// CWinComDoc handling of created request objects

BOOL CWinComDoc::AddRequest (CRequest * pRequest)
{
    return TRUE;
}

BOOL CWinComDoc::DeleteRequest (CRequest * pRequest)
{
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWinComDoc commands

BOOL CWinComDoc::SaveDocument()
{
    CRequest * request = new CRequest(this);
    HTAnchor * source = NULL;
    HTAnchor * destination = NULL;

    // Create the source anchor
    char * src = HTParse(m_Location.m_source, m_cwd, PARSE_ALL);
    source = HTAnchor_findAddress(src);
    HT_FREE(src);
    
    /* If destination is not http://... then error */
    if (!m_Location.m_destination.IsEmpty()) {
	char * access = HTParse(m_Location.m_destination, "", PARSE_ACCESS);
	if (strcasecomp(access, "http")) {
	    CString strMessage;
	    AfxFormatString1(strMessage, IDS_CANNOT_WRITE_TO_DESTINATION, m_Location.m_destination);
	    AfxMessageBox(strMessage);
	    HT_FREE(access);
	    return FALSE;
	} else {
	    char * dest = HTParse(m_Location.m_destination, m_cwd, PARSE_ALL);
	    destination = HTAnchor_findAddress(dest);
            HT_FREE(dest);
	    HT_FREE(access);
	}
    }

    // Do we have any metadata to set up?
    {
        HTParentAnchor * source_parent = HTAnchor_parent(source);
        
        if (!m_EntityInfo.m_mediaType.IsEmpty()) {
            HTAnchor_setFormat(source_parent, HTAtom_for(m_EntityInfo.m_mediaType));
        }
        if (!m_EntityInfo.m_contentEncoding.IsEmpty()) {
            HTAnchor_deleteEncodingAll(source_parent);
            HTAnchor_addEncoding(source_parent, HTAtom_for(m_EntityInfo.m_contentEncoding));
        }
        if (!m_EntityInfo.m_charset.IsEmpty()) {
            HTAnchor_setCharset(source_parent, HTAtom_for(m_EntityInfo.m_charset));
        }
        if (!m_EntityInfo.m_language.IsEmpty()) {
            HTAnchor_deleteLanguageAll(source_parent);
            HTAnchor_addLanguage(source_parent, HTAtom_for(m_EntityInfo.m_language));
        }
    }
    
    // Do we have any link relationships to set up?
    m_Links.AddLinkRelationships(source);

    /* Start the request */
    if (m_detectVersionConflict) {
        char * etag = HTAnchor_etag(HTAnchor_parent(destination));
        if (etag) {
	    request->PutDocument(source, destination, HT_MATCH_THIS);
        } else {
	    request->PutDocumentWithPrecheck(source, destination);
        }
    } else {
	request->PutDocument(source, destination, HT_NO_MATCH);
    }

    return TRUE;
}

void CWinComDoc::OnVersionConflict() 
{
    m_detectVersionConflict = !m_detectVersionConflict;
}

void CWinComDoc::OnUpdateVersionConflict(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_detectVersionConflict);
}

void CWinComDoc::OnShowServerStatus() 
{
    m_showServerStatus = !m_showServerStatus;
    int show_flag = m_showServerStatus ? HT_ERR_SHOW_PARS : 0;
    HTError_setShow((HTErrorShow) (HT_ERR_SHOW_INFO | show_flag));
}

void CWinComDoc::OnUpdateShowServerStatus(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_showServerStatus);
}

void CWinComDoc::OnAppUpgrade() 
{
    if (AfxMessageBox(IDS_APP_UPGRADE, MB_YESNO | MB_ICONEXCLAMATION )) {

#if 0
        CRequest * request = new CRequest(this);
        
        request->m_pHTAnchor = HTAnchor_findAddress(WEBCOMMANDER_THIS);
        
        request->CheckDocument(FALSE);
#endif
    }
}

void CWinComDoc::OnLoadAddress() 
{
    CLoad * load = new CLoad(this);
    load->DoModal();
}

void CWinComDoc::OnDeleteAddress() 
{
    CDelete * del = new CDelete(this);
    del->DoModal();
}
