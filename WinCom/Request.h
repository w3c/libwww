// Request.h: interface for the CRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUEST_H__E0D48280_C4BE_11D1_93E1_080009DCA30B__INCLUDED_)
#define AFX_REQUEST_H__E0D48280_C4BE_11D1_93E1_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CRequest : public CObject  
{
public:
	CRequest();
	virtual ~CRequest();
	char * mp_cwd;
        CString * mp_source;
	CString * mp_destination;
	CString * mp_proxy_prefix;
	CString * mp_proxy;

        CString * mp_charset;
        CString * mp_contentEncoding;
        CString * mp_mediaType;
protected:
};

#endif // !defined(AFX_REQUEST_H__E0D48280_C4BE_11D1_93E1_080009DCA30B__INCLUDED_)
