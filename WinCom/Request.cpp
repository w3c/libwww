// Request.cpp: implementation of the CRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinCom.h"
#include "Request.h"

// From libwww
#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWApp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CRequest, CObject)

CRequest::CRequest() : CObject()
{
}

CRequest::CRequest( CWinComApp * pApp ) : CObject()
{
    m_pApp = pApp;
    pApp->m_pRequest = this;
    m_cwd = HTGetCurrentDirectoryURL();
    m_pHTRequest = NULL;
}

CRequest::~CRequest()
{

}

int CRequest::PutDocument()
{
    if (m_pHTAnchorDestination && m_pHTAnchorSource) {

        /* Create a new libwww request object */
        m_pHTRequest = HTRequest_new();
	
	/* Set the context so that we can find it again */
	HTRequest_setContext(m_pHTRequest, this);
	
	/* Start the PUT */    
	if (HTPutDocumentAnchor(HTAnchor_parent(m_pHTAnchorSource),
	    m_pHTAnchorDestination, m_pHTRequest) != YES) {
	    return -1;
	}
	return 0;
    }
    return -1;
}
