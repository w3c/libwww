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

CRequest::CRequest(CWinComDoc * doc)
{
    m_pHTRequest = NULL;
    m_pHTAnchorSource = NULL;
    m_pHTAnchorDestination = NULL;
    m_pDoc = doc;
    m_file = NULL;
}

CRequest::~CRequest()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWinComDoc commands

int CRequest::PutDocument (BOOL UsePreconditions)
{
    if (m_pHTAnchorDestination && m_pHTAnchorSource) {

        /* Create a new libwww request object */
        m_pHTRequest = HTRequest_new();
	
	/* Set the context so that we can find it again */
	HTRequest_setContext(m_pHTRequest, this);

	/* Should we use preconditions? */
	if (UsePreconditions)
	    HTRequest_setPreconditions(m_pHTRequest, YES);

	/* Start the PUT */    
	if (HTPutDocumentAnchor(HTAnchor_parent(m_pHTAnchorSource),
	    m_pHTAnchorDestination, m_pHTRequest) != YES) {
	    return -1;
	}
	return 0;
    }
    return -1;
}

int CRequest::GetDocument (BOOL UsePreconditions)
{
    if (!m_saveAs.IsEmpty() && m_pHTAnchorDestination) {

        /* Create a new libwww request object */
        m_pHTRequest = HTRequest_new();
	
	/* Set the context so that we can find it again */
	HTRequest_setContext(m_pHTRequest, this);

	/* Should we use preconditions? */
	if (UsePreconditions)
	    HTRequest_setPreconditions(m_pHTRequest, YES);

	/* Start the GET */
	char * addr = HTAnchor_address(m_pHTAnchorDestination);
        if (HTLoadToFile (addr, m_pHTRequest, m_saveAs) == NO) {
            HT_FREE(addr);
	    return -1;
        }
        HT_FREE(addr);
	return 0;
    }
    return -1;
}

int CRequest::Cancel()
{
    BOOL status = HTRequest_kill(m_pHTRequest);
    return status==HT_OK ? 0 : -1;
}