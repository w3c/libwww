// EntityInfo.cpp : implementation file

#include "stdafx.h"
#include "WinCom.h"
#include "EntityInfo.h"

// From libwww
#include "WWWLib.h"

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
}

CEntityInfo::CEntityInfo( CRequest * pRequest) : CPropertyPage(CEntityInfo::IDD)
{
    //{{AFX_DATA_INIT(CEntityInfo)
    m_charset = _T("");
    m_contentEncoding = _T("");
    m_contentLength = 0;
    m_language = _T("");
    m_mediaType = _T("");
    m_age = 0;
	//}}AFX_DATA_INIT
    m_pRequest = pRequest;
    m_statFile = FALSE;
    pRequest->m_pEntityInfo = this;
}

CEntityInfo::~CEntityInfo()
{
}

void CEntityInfo::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CEntityInfo)
    DDX_Text(pDX, IDC_MEDIA_CHARSET, m_charset);
    DDV_MaxChars(pDX, m_charset, 64);

    DDX_Text(pDX, IDC_CONTENT_ENCODING, m_contentEncoding);
    DDV_MaxChars(pDX, m_contentEncoding, 64);

    DDX_Text(pDX, IDC_MEDIA_LANGUAGE, m_language);
    DDV_MaxChars(pDX, m_language, 64);

    DDX_Text(pDX, IDC_MEDIA_TYPE, m_mediaType);
    DDV_MaxChars(pDX, m_mediaType, 64);

    DDX_Text(pDX, IDC_MAX_AGE, m_age);
    DDV_MinMaxInt(pDX, m_age, 0, 60000);

    DDX_Text(pDX, IDC_CONTENT_LENGTH, m_contentLength);

	//}}AFX_DATA_MAP
}

#if 0
    DDX_Text(pDX, IDC_LAST_MODIFIED, m_lastModified.Format("%a, %d %b %Y %H:%M:%S"));
#endif

Format( DWORD dwFlags = 0, LCID lcid = LANG_USER_DEFAULT );


BEGIN_MESSAGE_MAP(CEntityInfo, CPropertyPage)
    //{{AFX_MSG_MAP(CEntityInfo)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEntityInfo message handlers

BOOL CEntityInfo::OnSetActive()
{
    BOOL bOk = CPropertyPage::OnSetActive();
    if (bOk) {
	HTParentAnchor * src = HTAnchor_parent(m_pRequest->m_pHTAnchorSource);
	char * uri = HTAnchor_address((HTAnchor *) src);
	char * access = HTParse(uri, "", PARSE_ACCESS);

	/* Check what we can find out about the file */
	if (m_statFile == FALSE) {
	    if (!strcasecomp(access, "file")) {
		char * filename = HTParse(uri, "", PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
		if (filename && *filename) {
		    if (*filename == '/' && filename[2] == ':') {
			char *orig=filename, *dest=filename+3;
			while((*orig++ = *dest++));
		    }
		    HTUnEscape(filename);
		    
		    CFile src_file;
		    CString fname(filename);
		    if (src_file.Open(fname, CFile::modeRead) == FALSE) {
			CString strMessage;
			AfxFormatString1(strMessage, IDS_CANNOT_READ_FILE, filename);
			AfxMessageBox(strMessage);
			HT_FREE(uri);
			HT_FREE(access);
			HT_FREE(filename);
			return FALSE;
		    }
		    
		    /* Get the size and last modified date */
		    CFileStatus status;
		    if(src_file.GetStatus(status)) {
			m_contentLength = status.m_size;
			HTAnchor_setLength(src, m_contentLength);
			SetDlgItemInt(IDC_CONTENT_LENGTH, m_contentLength);
			m_lastModified = status.m_mtime;
			HTAnchor_setLastModified(src, m_lastModified.GetTime());
			SetDlgItemText(IDC_LAST_MODIFIED, m_lastModified.Format("%a, %d %b %Y %H:%M:%S"));
		    }
		    src_file.Close();
		    
		    /* Get file suffix bindings */
		    if (HTBind_getAnchorBindings(src)) {
			char * mt = HTAtom_name(HTAnchor_format(src));
			char * charset = HTAtom_name(HTAnchor_charset(src));
			long length = HTAnchor_length(src);
			SetDlgItemText(IDC_MEDIA_TYPE, mt);
			SetDlgItemText(IDC_MEDIA_CHARSET, charset);
		    }
		}
	    }
	    m_statFile = TRUE;
	    HT_FREE(uri);
	    HT_FREE(access);
	}
    }
    return bOk;
 }

BOOL CEntityInfo::OnKillActive()
{
    BOOL bOk = CPropertyPage::OnKillActive();
    if ( bOk ) {
	HTParentAnchor * src = HTAnchor_parent(m_pRequest->m_pHTAnchorSource);
	if (m_mediaType && !m_mediaType.IsEmpty()) {
	    char * mediaType = m_mediaType.GetBuffer(16);
	    HTAnchor_setFormat(src, HTAtom_for(mediaType));
	    m_mediaType.ReleaseBuffer();
	}
	if (m_contentEncoding && !m_contentEncoding.IsEmpty()) {
	    char * encoding = m_contentEncoding.GetBuffer(16);
	    HTAnchor_deleteEncodingAll(src);
	    HTAnchor_addEncoding(src, HTAtom_for(encoding));
	    m_contentEncoding.ReleaseBuffer();
	}
	if (m_charset && !m_charset.IsEmpty()) {
	    char * charset = m_charset.GetBuffer(16);
	    HTAnchor_setCharset(src, HTAtom_for(charset));
	    m_charset.ReleaseBuffer();
	}
	if (m_language && !m_language.IsEmpty()) {
	    char * language = m_language.GetBuffer(16);
	    HTAnchor_deleteLanguageAll(src);
	    HTAnchor_addLanguage(src, HTAtom_for(language));
	    m_language.ReleaseBuffer();
	}
	if (m_age > 0) {
	    char age[20];
	    HTRequest_addCacheControl(m_pRequest->m_pHTRequest,
	    "max_age", ltoa(m_age, age, 10));
	}
    }
    return bOk;
}
