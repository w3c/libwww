// Request.h: interface for the CRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUEST_H__E0D48280_C4BE_11D1_93E1_080009DCA30B__INCLUDED_)
#define AFX_REQUEST_H__E0D48280_C4BE_11D1_93E1_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WWWLib.h"			      /* Global Library Include file */

class CWinComApp;
class CLocation;
class CEntityInfo;
class CLinks;

class CRequest : public CObject
{
   DECLARE_DYNCREATE(CRequest)
        
        // Construction
public:
	CLinks * m_pLinks;
	HTAnchor * m_pHTAnchorSource;
	HTAnchor * m_pHTAnchorDestination;
	char * m_cwd;
	int PutDocument();
        CRequest( CWinComApp *pApp );
        CRequest();
        ~CRequest();
	CWinComApp * m_pApp;
	CLocation * m_pLocation;
    	CEntityInfo * m_pEntityInfo;
	HTRequest * m_pHTRequest;
    
protected:
        
};

#endif // !defined(AFX_REQUEST_H__E0D48280_C4BE_11D1_93E1_080009DCA30B__INCLUDED_)
