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
#include "Progress.h"

class CRequest : public CObject
{
    DECLARE_DYNCREATE(CRequest)
	
// Construction
public:
    CRequest(CWinComDoc * doc = NULL);
    ~CRequest();
    
    int		GetDocument	(HTAnchor * address, int cacheValidation);

    int		HeadDocument	(HTAnchor * address, int cacheValidation);

    int		DeleteDocument	(HTAnchor * address, BOOL UsePreconditions);

    int		PutDocument	(HTAnchor * source,
				 HTAnchor * destination,
				 BOOL UsePreconditions);

    int		PutDocumentWithPrecheck (HTAnchor * source,
					 HTAnchor * destination,
					 BOOL UsePreconditions);

    int		Cancel();
    BOOL	Cleanup();

    CProgressCtrl * GetProgressBar();

    HTAnchor *	Source();
    HTAnchor *	Destination();

    CWinComDoc *m_pDoc;	

protected:
    BOOL SetCacheValidation     (int mode);

    HTRequest * m_pHTRequest;

    HTAnchor *	m_pSource;
    HTAnchor *	m_pDestination;

    CString	m_saveAs;
    FILE *	m_file;

    CProgress *	m_pProgress;

private:
	BOOL InitializeFilters();
};

#endif // !defined(AFX_REQUEST_H__E0D48280_C4BE_11D1_93E1_080009DCA30B__INCLUDED_)
