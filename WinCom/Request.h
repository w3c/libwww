// Request.h: interface for the CRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUEST_H__E0D48280_C4BE_11D1_93E1_080009DCA30B__INCLUDED_)
#define AFX_REQUEST_H__E0D48280_C4BE_11D1_93E1_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WWWLib.h"			      /* Global Library Include file */
#include "WinComDoc.h"

class CRequest : public CObject
{
    DECLARE_DYNCREATE(CRequest)
	
// Construction
public:
    CRequest(CWinComDoc * doc);
    CRequest();
    ~CRequest();
    
    char *	m_cwd;
    int		PutDocument();
    int		Cancel();
    HTAnchor *	m_pHTAnchorSource;
    HTAnchor *	m_pHTAnchorDestination;
    HTRequest * m_pHTRequest;

    CWinComDoc *m_pDoc;	

protected:
};

#endif // !defined(AFX_REQUEST_H__E0D48280_C4BE_11D1_93E1_080009DCA30B__INCLUDED_)
