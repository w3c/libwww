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
**	CLinks.cpp : implementation file
*/

#include "stdafx.h"
#include "WinCom.h"
#include "Links.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinks property page

IMPLEMENT_DYNCREATE(CLinks, CPropertyPage)
CLinks::CLinks() : CPropertyPage(CLinks::IDD)
{
	//{{AFX_DATA_INIT(CLinks)
	m_linkType = _T("");
	m_direction = 0;
	//}}AFX_DATA_INIT
    m_pLinkList = new CLinkView();
    
}

CLinks::~CLinks()
{
}

void CLinks::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinks)
	DDX_Control(pDX, IDC_LINK, m_linkDestination);
	DDX_Control(pDX, ID_LINK_REMOVE, m_linkRemove);
        DDX_Control(pDX, ID_LINK_ADD, m_linkAdd);
        DDX_Control(pDX, IDC_LINK_RELS, * m_pLinkList);
        DDX_CBString(pDX, IDC_LINK_TYPE, m_linkType);
        DDX_Radio(pDX, IDC_LINK_REL, m_direction);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLinks, CPropertyPage)
	//{{AFX_MSG_MAP(CLinks)
	ON_BN_CLICKED(ID_LINK_ADD, OnLinkAdd)
	ON_BN_CLICKED(ID_LINK_REMOVE, OnLinkRemove)
	ON_NOTIFY(HDN_ITEMCLICK, IDC_LINK_RELS, OnItemclickLinkRels)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
    
/////////////////////////////////////////////////////////////////////////////
// CListView initialization

#define NUM_COLUMNS	3
#define NUM_ROWS	3

static _TCHAR *_gszColumnLabel[NUM_COLUMNS] =
{
	_T("Type"), _T("Direction"), _T("Destination")
};

static int _gnColumnFmt[NUM_COLUMNS] = 
{
	LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_LEFT
};

static int _gnColumnWidth[NUM_COLUMNS] = 
{
	0, 0, 0
};

BOOL CLinks::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();

    // Setup the combo box
    CWinComApp * pApp = (CWinComApp *) AfxGetApp();
    ASSERT(pApp != NULL); 
    pApp->FillLinkComboBox(&m_linkDestination);

    // Setup the link list
    m_pLinkList->SetLink(this);
    CListCtrl &ListCtrl = m_pLinkList->GetListCtrl();
    
    // set image lists
    m_LargeImageList.Create(IDB_LARGEICONS, 32, 1, RGB(255, 255, 255));
    m_SmallImageList.Create(IDB_SMALLICONS, 16, 1, RGB(255, 255, 255));
    m_StateImageList.Create(IDB_STATEICONS, 16, 1, RGB(255, 0, 0));
    
    // m_LargeImageList.SetOverlayImage(NUM_ITEMS, 1);
    // m_SmallImageList.SetOverlayImage(NUM_ITEMS, 1);
    
    ListCtrl.SetImageList(&m_LargeImageList, LVSIL_NORMAL);
    // ListCtrl.SetImageList(&m_SmallImageList, LVSIL_SMALL);
    ListCtrl.SetImageList(NULL, LVSIL_SMALL);
    ListCtrl.SetImageList(NULL, LVSIL_STATE);

    // Find column widths
    CRect size;
    m_pLinkList->GetWindowRect(size);
    int width = size.Width();
    _gnColumnWidth[0] = (int) (width*0.20-1);
    _gnColumnWidth[1] = (int) (width*0.15-1);
    _gnColumnWidth[2] = (int) (width*0.65-1);

    // insert columns
    int i;
    LV_COLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    for (i = 0; i<NUM_COLUMNS; i++) {
	lvc.iSubItem = i;
	lvc.pszText = _gszColumnLabel[i];
	lvc.cx = _gnColumnWidth[i];
	lvc.fmt = _gnColumnFmt[i];
	ListCtrl.InsertColumn(i,&lvc);
    }
    
    // Select full row mode
    m_pLinkList->SetFullRowSel(TRUE);
    return TRUE;
}

void CLinks::OnLinkAdd() 
{
    UpdateData();
    
    // Get the edit text
    if (!m_linkType.IsEmpty() && m_linkDestination.GetWindowTextLength()) {
        CString LinkCaption;
	m_linkDestination.GetWindowText(LinkCaption);

	// Get the base address (if any)
        CString base;
        m_linkDestination.GetLBText(0, base);
        char * link = HTParse(LinkCaption, base.IsEmpty() ? "" : base, PARSE_ALL);
        link = HTSimplify(&link);
        LinkCaption = link;
        m_linkDestination.SetWindowText(LinkCaption);
        HT_FREE(link);


	// Update the combo box
	if (m_linkDestination.FindStringExact(-1, LinkCaption) == CB_ERR) {
	    m_linkDestination.InsertString(0, LinkCaption);
	    int length = m_linkDestination.GetCount();
	    if (length >= MAX_LIST_LENGTH) {
		int cnt;
		for (cnt=MAX_LIST_LENGTH; cnt < length; cnt++)
		    m_linkDestination.DeleteString(cnt);
	    }
	}

	// Write into INI file
	CWinComApp * pApp = (CWinComApp *) AfxGetApp();
	ASSERT(pApp != NULL); 
	pApp->AddLinkToIniFile(LinkCaption);

	// Update the list control with three columns
        CListCtrl &ListCtrl = m_pLinkList->GetListCtrl();

	// Insert item
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.pszText = (char *) LPCTSTR(m_linkType);
	lvi.iImage = 0;
	lvi.stateMask = LVIS_STATEIMAGEMASK;
	lvi.state = INDEXTOSTATEIMAGEMASK(1);
	int iItem = ListCtrl.InsertItem(&lvi);
	ASSERT(iItem != -1);

	// Set item text for additional columns
	ListCtrl.SetItemText(iItem,1, m_direction==0 ? _T("forward") : _T("reverse"));
	ListCtrl.SetItemText(iItem,2, (char *) LPCTSTR(LinkCaption));

	// Update changes 
	m_pLinkList->UpdateWindow();    
    }
}

void CLinks::OnLinkRemove() 
{
	// Update the list control with three columns
        CListCtrl &ListCtrl = m_pLinkList->GetListCtrl();

	// Find the selected item
	int nItem = ListCtrl.GetNextItem(-1, LVNI_FOCUSED);
	if(nItem != -1) ListCtrl.DeleteItem(nItem);

	// Update changes 
	m_pLinkList->UpdateWindow();

	UpdateData();
}

// Operations
BOOL CLinks::AddLinkRelationships (HTAnchor * source)
{
    ASSERT(source != NULL);
    if (!m_hWnd) return FALSE;
    HTParentAnchor * src = HTAnchor_parent(source);

    // Update the list control with three columns
    CListCtrl &ListCtrl = m_pLinkList->GetListCtrl();
    int iItem = ListCtrl.GetItemCount();
    int cnt;
    for (cnt=0; cnt<iItem; cnt++) {
        CString l_type = ListCtrl.GetItemText(cnt, 0);
        CString l_dir = ListCtrl.GetItemText(cnt, 1);
        CString l_dest = ListCtrl.GetItemText(cnt, 2);
        
        if (!l_type.IsEmpty() && !l_dir.IsEmpty() && !l_dest.IsEmpty()) {
            char * link_addr = HTParse(l_dest, "", PARSE_ALL);
            HTParentAnchor * dest = HTAnchor_parent(HTAnchor_findAddress(link_addr));
            
            /* Now add the new relation */
            if (l_dir == "forward") {
                HTLink_add((HTAnchor *) src, (HTAnchor *) dest,
                    (HTLinkType) HTAtom_caseFor(l_type),
                    METHOD_INVALID);
            } else if (l_dir == "reverse") {
                HTLink_add((HTAnchor *) dest, (HTAnchor *) src,
                    (HTLinkType) HTAtom_caseFor(l_type),
                    METHOD_INVALID);
            } else
                ASSERT(1);
        }
    }
    return TRUE;
}

void CLinks::Clear()
{
    if (m_hWnd) {

	// Delete all items
        CListCtrl &ListCtrl = m_pLinkList->GetListCtrl();
	ListCtrl.DeleteAllItems();

	// Update changes
	UpdateData();
    }
}

void CLinks::OnItemclickLinkRels(NMHDR* pNMHDR, LRESULT* pResult) 
{
    HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
    
    m_linkRemove.EnableWindow(FALSE);
    
    *pResult = 0;
}
