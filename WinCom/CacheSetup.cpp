// CacheSetup.cpp : implementation file
//

#include "stdafx.h"
#include "wincom.h"
#include "CacheSetup.h"

#include "WWWCache.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCacheSetup dialog


CCacheSetup::CCacheSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CCacheSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCacheSetup)
	m_cacheEnable = TRUE;
	m_cacheRoot = _T("");
	m_sizeT = 20;
	m_sizeS = 3;
	//}}AFX_DATA_INIT
}


void CCacheSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCacheSetup)
	DDX_Text(pDX, IDC_CACHE_ROOT, m_cacheRoot);
	DDX_Control(pDX, IDC_CACHE_SPIN_T, m_cacheSpinT);
	DDX_Control(pDX, IDC_CACHE_SPIN_S, m_cacheSpinS);
	DDX_Control(pDX, ID_CACHE_FLUSH, m_cacheFlush);
	DDX_Check(pDX, IDC_CACHE_ENABLE, m_cacheEnable);
	DDX_CBString(pDX, IDC_CACHE_ROOT, m_cacheRoot);
	DDX_Text(pDX, IDC_CACHE_SIZE_T, m_sizeT);
	DDV_MinMaxInt(pDX, m_sizeT, 10, 256);
	DDX_Text(pDX, IDC_CACHE_SIZE_S, m_sizeS);
	DDV_MinMaxInt(pDX, m_sizeS, 1, 9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCacheSetup, CDialog)
	//{{AFX_MSG_MAP(CCacheSetup)
	ON_BN_CLICKED(ID_CACHE_FLUSH, OnCacheFlush)
	ON_BN_CLICKED(ID_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCacheSetup::Terminate (void)
{
    CString strSection		= "Options";
    CString strCacheRootItem	= "Cache-Root";
    CString strCacheSizeTItem	= "Cache-Size";
    CString strCacheSizeSItem   = "Cache-Single-Size";
    CString strCacheEnableItem  = "Cache-Enable";
    
    CWinApp * pApp = AfxGetApp();
    ASSERT(pApp != NULL);

    pApp->WriteProfileString(strSection, strCacheRootItem, m_cacheRoot);
    pApp->WriteProfileInt(strSection, strCacheSizeTItem, m_sizeT);
    pApp->WriteProfileInt(strSection, strCacheSizeSItem, m_sizeS);
    pApp->WriteProfileInt(strSection, strCacheEnableItem, m_cacheEnable);

    /* terminate cache */
    HTCacheTerminate();

    return TRUE;
}

BOOL CCacheSetup::Init(void)
{
    CString strSection		= "Options";
    CString strCacheRootItem	= "Cache-Root";
    CString strCacheSizeTItem	= "Cache-Size";
    CString strCacheSizeSItem   = "Cache-Single-Size";
    CString strCacheEnableItem  = "Cache-Enable";
    
    CWinApp * pApp = AfxGetApp();
    ASSERT(pApp != NULL);

    CString root = pApp->GetProfileString(strSection, strCacheRootItem);
    m_sizeT = pApp->GetProfileInt(strSection, strCacheSizeTItem, 20);
    m_sizeS = pApp->GetProfileInt(strSection, strCacheSizeSItem, 3);
    m_cacheEnable = pApp->GetProfileInt(strSection, strCacheEnableItem, TRUE);

    /* Set up the persistent cache */
    const char * root_str = NULL;
    if (!root.IsEmpty()) root_str = (const char *) root;
    HTCacheInit(root_str, m_sizeT);

    m_cacheRoot = HTCacheMode_getRoot();

    HTCacheMode_setMaxCacheEntrySize(m_sizeS);

    HTCacheMode_setEnabled(m_cacheEnable);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCacheSetup message handlers

BOOL CCacheSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_cacheSpinT.SetRange(10, 256);
	m_cacheSpinS.SetRange(1, 9);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCacheSetup::OnCacheFlush() 
{
    HTCache_flushAll();
}

void CCacheSetup::OnBrowse() 
{
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 
}
