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
**	Request.cpp: implementation of the CRequest class.
*/

#include "stdafx.h"
#include "WinCom.h"
#include "Request.h"
#include "VersionConflict.h"

// From libwww
#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWApp.h"
#include "WWWFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DEF_LAST_SEGMENT	"index"

/////////////////////////////////////////////////////////////////////////////
// Libwww filters

/*
**  Request termination handler
*/
PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    CRequest * req = (CRequest *) HTRequest_context(request);

    req->Cleanup();
	
    delete req;

    return HT_OK;
}

/*
**  412 "Precondition failed" handler
*/
PRIVATE int precondition_handler (HTRequest * request, HTResponse * response,
			          void * param, int status) 
{
    CRequest * req = (CRequest *) HTRequest_context(request);
    CVersionConflict conflict;
    
    req->Cleanup();
    
    if (conflict.DoModal() == IDOK) {
	
	if (conflict.m_versionResolution == 0) {
	    HTAnchor * source = req->Source();
	    HTAnchor * dest = req->Destination();
	    CRequest * new_req = new CRequest(req->m_pDoc);
	    delete req;
	    
	    /* Start a new PUT request without preconditions */
	    new_req->PutDocument(source, dest, HT_NO_MATCH);

	    /* Stop here */
	    return HT_ERROR;

	} else if (conflict.m_versionResolution == 1) {
	    HTAnchor * address = req->Source();
	    CRequest * new_req = new CRequest(req->m_pDoc);
	    delete req;
	    
	    /* Start a new GET request */
	    new_req->GetDocument(address, 2);
	    
	    /* Stop here */
	    return HT_ERROR;

	} else
	    return HT_OK;   /* Just finish the request */
	
    }
    return HT_OK;
}

/*
**  Request HEAD checker filter
*/
PRIVATE int check_handler (HTRequest * request, HTResponse * response,
			   void * param, int status)
{
    CRequest * req = (CRequest *) HTRequest_context(request);
    CVersionConflict conflict;
    
    req->Cleanup();
    
    /*
    ** If head request showed that the document doesn't exist
    ** then just go ahead and PUT it. Otherwise ask for help
    */
    if (status==HT_ERROR || status==HT_INTERRUPTED || status==HT_TIMEOUT) {
	delete req;
	return HT_ERROR;
    } else if (status==HT_NO_ACCESS || status==HT_NO_PROXY_ACCESS) {
	delete req;
	return HT_ERROR;
    } else if (abs(status)/100!=2) {
	HTAnchor * source = req->Source();
	HTAnchor * dest = req->Destination();
	CRequest * new_req = new CRequest(req->m_pDoc);
	delete req;
	
	/* Start a new PUT request with a "if-none-match *" precondition */
	new_req->PutDocument(source, dest, HT_DONT_MATCH_ANY);
	
    } else if (conflict.DoModal() == IDOK) {
	
	if (conflict.m_versionResolution == 0) {
	    HTAnchor * source = req->Source();
	    HTAnchor * dest = req->Destination();
	    CRequest * new_req = new CRequest(req->m_pDoc);
	    delete req;
	    
	    /* Start a new PUT request without preconditions */
	    new_req->PutDocument(source, dest, HT_NO_MATCH);
	    
	} else if (conflict.m_versionResolution == 1) {
	    HTAnchor * address = req->Source();
	    CRequest * new_req = new CRequest(req->m_pDoc);
	    delete req;
	    
	    /* Start a new GET request  */
	    new_req->GetDocument(address, 2);
        }
    }
    return HT_ERROR;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CRequest, CObject)

CRequest::CRequest(CWinComDoc * doc)
{
    ASSERT(doc != NULL);
    m_pDoc = doc;

    m_pHTRequest = NULL;
    m_pSource = NULL;
    m_pDestination = NULL;
    m_file = NULL;

    // @@@ADD TO DOC LIST@@@
}

CRequest::~CRequest()
{
    // @@@REMOVE FROM DOC LIST@@@
}

/////////////////////////////////////////////////////////////////////////////
// CWinComDoc commands

BOOL CRequest::SetCacheValidation(int mode)
{
    ASSERT(m_pHTRequest != NULL);
    HTReload reload = HT_CACHE_OK;
    switch (mode) {
    case 0:
        reload = HT_CACHE_OK;
        break;
    case 1:
        reload = HT_CACHE_VALIDATE;
        break;
    case 2:
        reload = HT_CACHE_END_VALIDATE;
        break;
    case 3:
        reload = HT_CACHE_FLUSH;
        break;
    default:
        reload = HT_CACHE_OK;
    }
 
    // Set the cache validation mode for this request
    HTRequest_setReloadMode(m_pHTRequest, reload);

    return TRUE;
}

int CRequest::GetDocument (HTAnchor * address, int cacheValidation)
{
    char filebuf[1024];
    ASSERT(address != NULL); 
    char * uri = HTAnchor_address(address);

    m_pSource = address;
    
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 

    CFileDialog fd(FALSE);

    /* Find the start filename */
    {
	char * name = NULL;
	char * suffix = HTBind_getSuffix(HTAnchor_parent(address));
        char * uri_path = NULL;
	if (uri && (uri_path = HTParse(uri, "", PARSE_PATH|PARSE_PUNCTUATION))) {
	    char * last_segment = strrchr(uri_path, '/');
	    if (last_segment && *(last_segment+1)) {
		StrAllocMCopy(&name, ++last_segment, NULL);
	    } else {
		StrAllocMCopy(&name, DEF_LAST_SEGMENT,
			      suffix ? suffix : "", NULL);
	    }
	}

	strcpy(filebuf, name);
        fd.m_ofn.lpstrFile = filebuf;
	
	HT_FREE(name);
	HT_FREE(suffix);
    }

    /* Find the start folder */
    fd.m_ofn.lpstrInitialDir = pApp->GetIniCWD();

    if (fd.DoModal() == IDOK) {
	
	m_saveAs = fd.GetPathName();
	
	// Set the initial dir for next time
	CString path = m_saveAs;    
	int idx = path.ReverseFind('\\');
	if (idx != -1) path.GetBufferSetLength(idx+1);
	pApp->SetIniCWD(path);
	
        /* Create a new libwww request object */
        m_pHTRequest = HTRequest_new();
	
	/* Set the context so that we can find it again */
	HTRequest_setContext(m_pHTRequest, this);

        /* Terminating filters */
	InitializeFilters();

        /* Should we do cache validation? */
        SetCacheValidation(cacheValidation);
        
        /* Progress bar */
        m_pProgress = new CProgress(this);
	m_pProgress->Create(IDD_PROGRESS);
	m_pProgress->ShowWindow(SW_SHOW);

	/* Start the GET */
        if (HTLoadToFile (uri, m_pHTRequest, m_saveAs) == NO) {
            HT_FREE(uri);
	    return -1;
        }

        HT_FREE(uri);
        return 0;
    }

    HT_FREE(uri);
    return -1;
}

int CRequest::HeadDocument (HTAnchor * address, int cacheValidation)
{
    ASSERT(address != NULL); 

    m_pSource = address;
    
    /* Create a new libwww request object */
    m_pHTRequest = HTRequest_new();
        
    /* Set the context so that we can find it again */
    HTRequest_setContext(m_pHTRequest, this);
    
    /* Should we do cache validation? */
    SetCacheValidation(cacheValidation);

    /* Terminating filters */
    InitializeFilters();

    /* Progress bar */
    m_pProgress = new CProgress(this);
    m_pProgress->Create(IDD_PROGRESS);
    m_pProgress->ShowWindow(SW_SHOW);
    
    /* Start the HEAD */
    if (HTHeadAnchor(address, m_pHTRequest) == NO) {
	return -1;
    }
    return 0;
}

int CRequest::DeleteDocument (HTAnchor * address, HTPreconditions preconditions)
{
    ASSERT(address != NULL); 

    m_pSource = address;
    
    /* Create a new libwww request object */
    m_pHTRequest = HTRequest_new();
        
    /* Set the context so that we can find it again */
    HTRequest_setContext(m_pHTRequest, this);
    
    /* Should we use preconditions? */
    HTRequest_setPreconditions(m_pHTRequest, preconditions);

    /* Terminating filters */
    InitializeFilters();

    /* Progress bar */
    m_pProgress = new CProgress(this);
    m_pProgress->Create(IDD_PROGRESS);
    m_pProgress->ShowWindow(SW_SHOW);
    
    /* Start the HEAD */
    if (HTDeleteAnchor(address, m_pHTRequest) == NO) {
	return -1;
    }
    return 0;
}

int CRequest::PutDocument (HTAnchor * source,
			   HTAnchor * destination,
			   HTPreconditions preconditions)
{
    ASSERT(source != NULL); 
    ASSERT(destination != NULL); 

    m_pSource = source;
    m_pDestination = destination;
    
    /* Create a new libwww request object */
    m_pHTRequest = HTRequest_new();
    
    /* Set the context so that we can find it again */
    HTRequest_setContext(m_pHTRequest, this);
    
    /* Should we use preconditions? */
    HTRequest_setPreconditions(m_pHTRequest, preconditions);
	
    /* Terminating filters */
    InitializeFilters();

    /* Progress bar */
    m_pProgress = new CProgress(this);
    m_pProgress->Create(IDD_PROGRESS);
    m_pProgress->ShowWindow(SW_SHOW);

    /* Start the PUT */    
    if (HTPutDocumentAnchor(HTAnchor_parent(source), destination, m_pHTRequest) != YES) {
	return -1;
    }
    return 0;
}

int CRequest::PutDocumentWithPrecheck (HTAnchor * source,
				       HTAnchor * destination)
{
    ASSERT(source != NULL); 
    ASSERT(destination != NULL); 

    m_pSource = source;
    m_pDestination = destination;
    
    /* Create a new libwww request object */
    m_pHTRequest = HTRequest_new();
    
    /* Only use authentication and check AFTER filters */
    HTRequest_addAfter(m_pHTRequest, HTAuthFilter,	"http://*", NULL, HT_NO_ACCESS, HT_FILTER_MIDDLE, YES);
    HTRequest_addAfter(m_pHTRequest, HTAuthFilter, 	"http://*", NULL, HT_REAUTH,    HT_FILTER_MIDDLE, YES);
    HTRequest_addAfter(m_pHTRequest, HTUseProxyFilter,	"http://*", NULL, HT_USE_PROXY, HT_FILTER_MIDDLE, YES);
    HTRequest_addAfter(m_pHTRequest, check_handler,	NULL,	    NULL, HT_ALL,       HT_FILTER_MIDDLE, YES);
    
    /* Set the context so that we can find it again */
    HTRequest_setContext(m_pHTRequest, this);
    
    /* Set cache validation */
    SetCacheValidation(2);

    /* Progress bar */
    m_pProgress = new CProgress(this);
    m_pProgress->Create(IDD_PROGRESS);
    m_pProgress->ShowWindow(SW_SHOW);

    /* Start the HEAD */
    if (HTHeadAnchor(destination, m_pHTRequest) == NO) {
	return -1;
    }
    return 0;
}

int CRequest::Cancel()
{
    if (m_pHTRequest) HTRequest_kill(m_pHTRequest);
    if (m_pProgress) m_pProgress = NULL;
    return 0;
}

BOOL CRequest::Cleanup()
{
    if (m_file) {
	fclose(m_file);
	m_file = NULL;
    }
    if (m_pProgress) {
	m_pProgress->DestroyWindow();
	m_pProgress = NULL;
    }
    return TRUE;
}

HTAnchor * CRequest::Source()
{
    return m_pSource;
}

HTAnchor * CRequest::Destination()
{
    return m_pDestination;
}

BOOL CRequest::InitializeFilters()
{
    static BOOL initialized = NO;
    if (initialized == NO) {

	/* Add our own after filter to handle 412 precondition failed */
        HTNet_addAfter(precondition_handler, NULL, NULL, HT_PRECONDITION_FAILED, HT_FILTER_MIDDLE);

        /* Add our own after filter to clecn up */
        HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

	initialized = YES;

	return TRUE;
    }
    return FALSE;
}

CProgressCtrl * CRequest::GetProgressBar()
{
    return &(m_pProgress->m_progress);
}

