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
*/

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

    int		DeleteDocument	(HTAnchor * address,
				 HTPreconditions conditions);

    int		PutDocument	(HTAnchor * source,
				 HTAnchor * destination,
				 HTPreconditions conditions);

    int		PutDocumentWithPrecheck (HTAnchor * source,
					 HTAnchor * destination);

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
