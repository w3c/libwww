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
**	EntityInfo.cpp : implementation file
*/

#include "stdafx.h"
#include "WinCom.h"
#include "EntityInfo.h"
#include "WinComDoc.h"
#include "TabsView.h"

// From libwww
#include "WWWLib.h"
#include "WWWFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEntityInfo property page

IMPLEMENT_DYNCREATE(CEntityInfo, CPropertyPage)
CEntityInfo::CEntityInfo() : CPropertyPage(CEntityInfo::IDD)
{
    //{{AFX_DATA_INIT(CEntityInfo)
	m_charset = _T("");
	m_contentEncoding = _T("");
	m_contentLength = 0;
	m_language = _T("");
	m_mediaType = _T("");
	m_expires = 0;
	m_etag = _T("");
	m_lmDate = _T("");
	//}}AFX_DATA_INIT
}

CEntityInfo::~CEntityInfo()
{
}

void CEntityInfo::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CEntityInfo)
	DDX_Control(pDX, ID_GET_INFO, m_guess);
        DDX_Text(pDX, IDC_CONTENT_CHARSET, m_charset);
        DDX_Text(pDX, IDC_CONTENT_ENCODING, m_contentEncoding);
        DDX_Text(pDX, IDC_CONTENT_LENGTH, m_contentLength);
	DDV_MinMaxLong(pDX, m_contentLength, 0, 2147483647);
        DDX_Text(pDX, IDC_CONTENT_LANGUAGE, m_language);
        DDX_Text(pDX, IDC_CONTENT_TYPE, m_mediaType);
        DDX_Text(pDX, IDC_EXPIRES, m_expires);
        DDV_MinMaxInt(pDX, m_expires, 0, 60000);
	DDX_Text(pDX, IDC_ETAG, m_etag);
	DDX_Text(pDX, IDC_LAST_MODIFIED, m_lmDate);
	//}}AFX_DATA_MAP
}

Format( DWORD dwFlags = 0, LCID lcid = LANG_USER_DEFAULT );

BEGIN_MESSAGE_MAP(CEntityInfo, CPropertyPage)
    //{{AFX_MSG_MAP(CEntityInfo)
	ON_BN_CLICKED(ID_GET_INFO, OnGetInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEntityInfo message handlers

BOOL CEntityInfo::OnKillActive() 
{
    UpdateData();

    if (!m_mediaType.IsEmpty() ||
	!m_contentEncoding.IsEmpty() ||
	!m_charset.IsEmpty() ||
	!m_language.IsEmpty()) {
	HTFile_doFileSuffixBinding(NO);
    } else
	HTFile_doFileSuffixBinding(YES);

    return CPropertyPage::OnKillActive();
}

void CEntityInfo::OnGetInfo() 
{
    ASSERT(GetParentFrame()->GetActiveView()->IsKindOf(RUNTIME_CLASS(CTabsView)));
    CTabsView * view = (CTabsView *) GetParentFrame()->GetActiveView();
    CWinComDoc * doc = view->GetDocument();
    
    // Create the source anchor
    char * src_str = HTParse(doc->m_Location.m_source, doc->m_cwd, PARSE_ALL);
    HTParentAnchor * src_parent = HTAnchor_parent(HTAnchor_findAddress(src_str));
    
    if (src_str) {
	char * local = HTWWWToLocal (src_str, "", NULL);
	
	/* Check what we can find out about the file */
	if (local) {
	    CWaitCursor wait;
	    if (local && *local) {
		CFile src_file;
		if (src_file.Open(local, CFile::modeRead) == FALSE) {
		    CString strMessage;
		    AfxFormatString1(strMessage, IDS_CANNOT_READ_FILE, local);
		    AfxMessageBox(strMessage);
		    HT_FREE(src_str);
		    HT_FREE(local);
		    return;
		}
		
		/* Get the size and last modified date */
		CFileStatus status;
		if(src_file.GetStatus(status)) {
		    m_contentLength = status.m_size;
		    HTAnchor_setLength(src_parent, m_contentLength);
		    SetDlgItemInt(IDC_CONTENT_LENGTH, m_contentLength);
		    m_lastModified = status.m_mtime;
		    HTAnchor_setLastModified(src_parent, m_lastModified.GetTime());
		    m_lmDate = m_lastModified.Format("%a, %d %b %Y %H:%M:%S");
		    SetDlgItemText(IDC_LAST_MODIFIED, m_lmDate);
		}
		src_file.Close();
		
		/* Get file suffix bindings */
		if (HTBind_getAnchorBindings(src_parent)) {
		    char * mt = HTAtom_name(HTAnchor_format(src_parent));
		    char * charset = HTAtom_name(HTAnchor_charset(src_parent));
                    HTAtom_name(HTAnchor_charset(src_parent));
		    HTList * encodings = HTAnchor_encoding(src_parent);
                    HTList * languages = HTAnchor_language(src_parent);
                    long length = HTAnchor_length(src_parent);
		    if (mt) {
			m_mediaType = mt;
			SetDlgItemText(IDC_MEDIA_TYPE, mt);
		    }
		    if (charset) {
			m_charset = charset;
			SetDlgItemText(IDC_MEDIA_CHARSET, charset);
		    }

                    /* Just take the first language */
                    if (languages) {
                        HTLanguage lang = (HTLanguage) HTList_firstObject(languages);
                        if (lang) m_language = HTAtom_name(lang);
                    }

                    /* Just take the first encoding */
                    if (encodings) {
                        HTEncoding enc = (HTEncoding) HTList_firstObject(encodings);
                        if (enc) m_contentEncoding = HTAtom_name(enc);
                    }
		}
	    }
	}
	
	/* Now don't use the automatic file suffix bindings */
	HTFile_doFileSuffixBinding(NO);
	
	/* Update the data */
	UpdateData(FALSE);
	
	HT_FREE(local);
        HT_FREE(src_str);
    }
}

void CEntityInfo::Clear()
{
    if (m_hWnd) {
	m_charset = _T("");
	m_contentEncoding = _T("");
	m_contentLength = 0;
	m_language = _T("");
	m_mediaType = _T("");
	m_expires = 0;
	m_etag = _T("");
	m_lmDate = _T("");
	UpdateData(FALSE);
	
	/* Do use the automatic file suffix bindings */
	HTFile_doFileSuffixBinding(YES);
    }
}

BOOL CEntityInfo::OnSetActive() 
{
    ASSERT(GetParentFrame()->GetActiveView()->IsKindOf(RUNTIME_CLASS(CTabsView)));
    CTabsView * view = (CTabsView *) GetParentFrame()->GetActiveView();
    CWinComDoc * pDoc = view->GetDocument();
    ASSERT(pDoc != NULL); 
    
    // See if Guess button should be active or not
    if (!pDoc->m_Location.m_source.IsEmpty())
	m_guess.EnableWindow(TRUE);
    else
        m_guess.EnableWindow(FALSE);

    return CPropertyPage::OnSetActive();
}
