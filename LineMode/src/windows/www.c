#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "lib.h"
#include "scroll.h"
#include "wwwrc.h"
#include "WWWLib.h"
#include "HTReqMan.h"
#include "../HTBrowse.h"

#define VERSIONSTR "v0.1 beta"	//update this version for each release

// monitor size
#define MAXROWS         25
#define MAXCOLS         80

struct _HTView {
	HWND hWnd;
	char * name;
	ScrollInfo_t *  pScrollInfo;
	FontInfo_t *  pFontInfo;
	int rows;
	int cols;
	LineMode * pLm;
};

char WWWClassName[] = "WWWClass";
char VersionText[] = "WWW.EXE "VERSIONSTR".";

HINSTANCE HInstance;

HTView * PMonitorView = 0;
HTView * PDataView = 0;

extern HTRequest * LineMode_getConsole(LineMode * pLm);

typedef struct {	/* default */
    int zeroPad;	/* 0 */
    int leftJustify;	/* 0 */
    int showSign;	/* 0 */
    int width;		/* -1 */
    int precision;	/* -1 */
    char size;		/* 0 */
    int iArgs;		/* 0 */
} Crack_format_t;	/* = -2 */

#define Crack_format_INIT {0, 0, 0, -1, -1, 0, 0}

typedef struct {
    int freeMe;
    char * ptr;
    int ptrLen;
    int index;
    char terminate;
    Crack_format_t format;
} Crack_t;

#define Crack_INIT {0, 0, 0, 0, 0, Crack_format_INIT}

PRIVATE int _readInt(char * ptr, int * pDest)
{
    int i;
    int ret = 0;
    for (i = 0; ptr[i] >= '0' && ptr[i] <= '9'; i++) {
	ret *= 10;
	ret += ptr[i];
    }
    if (!i)
	return 0;
    *pDest = i;
    return i;
}

PRIVATE int Crack_next(Crack_t * pCrack, char * fmt, va_list pArgs)
{
    int innastring = 0;
    int scanIndex = pCrack->index;
    int used;
    static char shortRet[50];
    char sprintFmt[20];

    switch (fmt[scanIndex]) {
    case 0:
	return 0;
    case '%':
	scanIndex++;
	/* check for modifiers */
	if (fmt[scanIndex] == '0') {
	    pCrack->format.zeroPad = -1;
	    scanIndex++;
	}
	if (fmt[scanIndex] == '-') {
	    pCrack->format.leftJustify = -1;
	    scanIndex++;
	}
	if (fmt[scanIndex] == '+') {
	    pCrack->format.showSign = -1;
	    scanIndex++;
	}
	if ((used = _readInt(fmt+scanIndex, &pCrack->format.width)) > 0)
	    scanIndex += used;
	if (fmt[scanIndex] == '.') {
	    scanIndex++;
    	    if ((used = _readInt(fmt+scanIndex, &pCrack->format.precision)) > 0)
		scanIndex += used;
	    else
		pCrack->format.precision = 0;
	}
	if (fmt[scanIndex] == 'h' || fmt[scanIndex] == 'l' || fmt[scanIndex] == 'L') {
	    pCrack->format.size = fmt[scanIndex];
	    scanIndex++;
	}
	/* time for the format */
	switch (fmt[scanIndex]) {
	case 'd':
	case 'i':
	case 'o':
	case 'u':
	case 'x':
	case 'X':
	    strncpy(sprintFmt, fmt+pCrack->index, scanIndex+1-pCrack->index);
	    sprintFmt[scanIndex+1-pCrack->index] = 0;
	    if (!(pCrack->ptr = (char *)malloc(pCrack->format.width + pCrack->format.precision+20)))
		return 0;
	    pCrack->freeMe = 1;
	    switch (pCrack->format.size) {
	    case 0:
		pCrack->ptrLen = sprintf(pCrack->ptr, sprintFmt, *(int*)(pArgs+pCrack->format.iArgs));
		pCrack->format.iArgs += sizeof(int);
		break;
	    case 'h':
		pCrack->ptrLen = sprintf(pCrack->ptr, sprintFmt, *(short int*)(pArgs+pCrack->format.iArgs));
		pCrack->format.iArgs += sizeof(short int);
		break;
	    case 'l':
		pCrack->ptrLen = sprintf(pCrack->ptr, sprintFmt, *(long int*)(pArgs+pCrack->format.iArgs));
	        pCrack->format.iArgs += sizeof(long int);
		break;
	    default:
		free(pCrack->ptr);
		goto FMTSTRING; /* invalid arg combo */
	    }
	    pCrack->index = scanIndex+1;
	    return 1;
	case 'e':
	case 'E':
	case 'f':
	case 'g':
	case 'G':
	    strncpy(sprintFmt, fmt+pCrack->index, scanIndex+1-pCrack->index);
	    sprintFmt[scanIndex+1-pCrack->index] = 0;
	    if (!(pCrack->ptr = (char *)malloc(pCrack->format.width + pCrack->format.precision+20)))
		return 0;
	    pCrack->freeMe = 1;
	    switch (pCrack->format.size) {
	    case 0:
		pCrack->ptrLen = sprintf(pCrack->ptr, sprintFmt, *(double*)(pArgs+pCrack->format.iArgs));
		pCrack->format.iArgs += sizeof(double);
		break;
	    case 'L':
		pCrack->ptrLen = sprintf(pCrack->ptr, sprintFmt, *(long double*)(pArgs+pCrack->format.iArgs));
	        pCrack->format.iArgs += sizeof(long double);
		break;
	    default:
		free(pCrack->ptr);
		goto FMTSTRING; /* invalid arg combo */
	    }
	    pCrack->index = scanIndex+1;
	    return 1;
	case 'c':
	    shortRet[0] = *(char *)(pArgs+pCrack->format.iArgs);
	    pCrack->ptr = shortRet;
	    pCrack->freeMe = 0;
	    pCrack->ptrLen = 1;
	    pCrack->format.iArgs += sizeof(char);
	    pCrack->index = scanIndex+1;
	    return 1;
	case 's':
	    pCrack->ptr = *(char **)(pArgs+pCrack->format.iArgs);
	    pCrack->freeMe = 0;
	    pCrack->ptrLen = strlen(pCrack->ptr);
	    pCrack->format.iArgs += sizeof(char *);
	    pCrack->index = scanIndex+1;
	    return 1;
	case 'p':
	    pCrack->ptr = shortRet;
	    pCrack->freeMe = 0;
	    pCrack->ptrLen = sprintf(shortRet, "%p", *(void **)(pArgs+pCrack->format.iArgs));
	    pCrack->format.iArgs += sizeof(void *);
	    pCrack->index = scanIndex+1;
	    return 1;
	case 'n':
	    return 0;
	case '%':
	    shortRet[0] = '%';
	    pCrack->ptr = shortRet;
	    pCrack->freeMe = 0;
	    pCrack->ptrLen = 1;
	    pCrack->index = scanIndex+1;
	    return 1;
	}
    }
FMTSTRING:
    while (fmt[scanIndex] && fmt[scanIndex] != '%')
	scanIndex++;
    pCrack->ptr = fmt+pCrack->index;
    pCrack->freeMe = 0;
    pCrack->ptrLen = scanIndex - pCrack->index;
    pCrack->index = scanIndex;
    return 1;
}

PUBLIC int TextToAWindow(HTView * pView, const char * fmt, va_list pArgs)
{
    Crack_t crack = Crack_INIT;
    int totalLen = 0;

    while (Crack_next(&crack, fmt, pArgs)) {
	totalLen += crack.ptrLen;
        Scroll_WriteBlock(pView->pScrollInfo, pView->hWnd, (LPSTR)crack.ptr, crack.ptrLen);
	if (crack.freeMe)
	    free(crack.ptr);
    }
    return (totalLen);
}

PUBLIC int OutputConsole(const char* fmt, va_list pArgs)
{
    return (TextToAWindow(PMonitorView, fmt, pArgs));
}

PUBLIC int OutputData(HTView * pView, const char * fmt, ...)
{
    va_list pArgs;
    int len;

    va_start(pArgs, fmt);
    len = TextToAWindow(pView ? pView : PMonitorView, fmt, pArgs);
    va_end(pArgs);
    return (len);
}

#ifndef _WIN32
typedef MINMAXINFO FAR* LPMINMAXINFO;
#endif

#ifdef _WIN32
#define WINCALLBACK CALLBACK
#else
#define WINCALLBACK CALLBACK _export
#endif

LRESULT WINCALLBACK monitorWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
static int sized = 0;
    HTView * pView = (HTView *)GetWindowLong(hWnd, 0); /* valid for all events except for WM_CREATE */
    switch (uMsg)
    	{
    	case WM_USER+1:	//we just got a data set
    	    return (0);

    	case WM_CREATE:
    	    {
    	    HMENU hMenu = GetMenu(hWnd);
	    pView = (HTView *)((CREATESTRUCT *)lParam)->lpCreateParams;
	    SetWindowLong(hWnd, 0, (long)pView);
//	    	pView->pScrollInfo->control = Scroll_control_autoWrap | Scroll_control_crBegetsLf | Scroll_control_lfBegetsCr | Scroll_control_BSbegetsSpace;// | Scroll_control_literal;
    	    Scroll_ResetScreen(hWnd, pView->pScrollInfo, pView->pFontInfo);
    	    MoveWindow(hWnd, 0, 0, pView->pScrollInfo->maxTrackSize.x, pView->pScrollInfo->maxTrackSize.y, FALSE);
    	    return (0);
    	    }

#define USEMINMAX
#ifdef USEMINMAX
    	case WM_GETMINMAXINFO:
    	    {
    	    LPMINMAXINFO pMax = (LPMINMAXINFO)lParam;
			if (pView)
    			pMax->ptMaxTrackSize = pView->pScrollInfo->maxTrackSize;
    	    }
    	    break;
#endif

    	case WM_COMMAND:
    	    switch ((WORD) wParam)
    	    	{
//	    	    case IDM_CRBEGETSLF:
//	    	    	pView->pScrollInfo->control ^= Scroll_control_crBegetsLf;
//	    	    	CheckMenuItem(GetMenu(hWnd), (UINT)IDM_CRBEGETSLF, (pView->pScrollInfo->control & Scroll_control_crBegetsLf) ? MF_CHECKED : MF_UNCHECKED);
//    	    	    break;

    	    	case IDM_FONT:
    	    	    if (Lib_SelectWorkingFont(hWnd, pView->pFontInfo))
    	    	      Scroll_ResetScreen(hWnd, pView->pScrollInfo, pView->pFontInfo) ;
      	    	return (TRUE);
    	    	    break;

    	    	case IDM_ABOUT:
    	    	    while (MessageBox(hWnd, "It's a program, all right?", "About LineMode", MB_OKCANCEL) != IDOK);
    	    	    break;

    	    	case IDM_EXIT:
    	    	    PostMessage(hWnd, WM_CLOSE, 0, 0L);
    	    	    break ;
    	    	}
    	    break;

    	case WM_PAINT:
    	    Scroll_Paint(pView->pScrollInfo, pView->pFontInfo, hWnd, Scroll_cursorShow);
    	    break ;

    	case WM_SIZE:
    	    {
static int recursing = 0;
    	    RECT rect;
    	    LONG style;
    	    WORD xScroll = GetSystemMetrics(SM_CXVSCROLL);
    	    WORD yScroll = GetSystemMetrics(SM_CYHSCROLL);
    	    if (recursing == 1)
    	    	return (0);
    	    rect.left = 0;
    	    rect.top = 0;
    	    rect.right = LOWORD(lParam)+xScroll-1;
    	    rect.bottom = HIWORD(lParam)+yScroll-1;
    	    style = GetWindowLong(hWnd, GWL_STYLE);
    	    AdjustWindowRect(&rect, style, TRUE);
    	    if (style & (WS_VSCROLL | WS_HSCROLL) && rect.right-rect.left == pView->pScrollInfo->maxTrackSize.x && rect.bottom-rect.top == pView->pScrollInfo->maxTrackSize.y)
    	    	{
    	    	recursing = 1;
    	    	Scroll_SetSize(pView->pScrollInfo, hWnd, (WORD)(HIWORD(lParam)+yScroll), (WORD)(LOWORD(lParam)+xScroll));
    	    	recursing = 0;
    	    	}
    	    else
    	    	Scroll_SetSize(pView->pScrollInfo, hWnd, HIWORD(lParam), LOWORD(lParam));
    	    return (0);
    	    }
    	    break ;

    	case WM_HSCROLL:
#ifdef _WIN32
    	    Scroll_SetHorz(pView->pScrollInfo, hWnd, LOWORD(wParam), HIWORD(wParam));
#else
    	    Scroll_SetHorz(pView->pScrollInfo, hWnd, (WORD) wParam, LOWORD(lParam));
#endif
    	    break ;

    	case WM_VSCROLL:
#ifdef _WIN32
    	    Scroll_SetVert(pView->pScrollInfo, hWnd, LOWORD(wParam), HIWORD(wParam));
#else
    	    Scroll_SetVert(pView->pScrollInfo, hWnd, (WORD) wParam, LOWORD(lParam));
#endif
    	    break ;

    	case WM_CHAR:
    	    {
#ifdef _WIN32
    	    BYTE bOut = LOBYTE(LOWORD(wParam));
#else
    	    BYTE bOut = LOBYTE(wParam);
#endif

	    if (pView->pLm && 
		bufferInput(&bOut, 1, 0, LineMode_getConsole(pView->pLm), 0) == -1)
    	    	PostMessage(hWnd, WM_QUIT, 0, 0);
    	    Scroll_WriteBlock(pView->pScrollInfo, hWnd, &bOut, 1);
    	    }
    	    break ;

    	case WM_SETFOCUS:
    	    Scroll_SetFocus(hWnd, pView->pScrollInfo);
    	    break ;

    	case WM_KILLFOCUS:
    	    Scroll_KillFocus(hWnd, pView->pScrollInfo);
    	    break ;

    	case WM_DESTROY:
    	    {
    	    Scroll_DestroyInfo(pView->pScrollInfo);
    	    Font_DestroyInfo(pView->pFontInfo);
    	    PostMessage(hWnd, WM_QUIT, 0, 0);
    	    }
    	    break ;

    	default:
    	    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
    	}
    return 0L;
    } // end of monitorWndProc()

int makeArgcArgv(HINSTANCE hInstance, char*** pArgv, char* commandLine)
{
    int argc;
static char* argv[20];
static char argv0[256];
    char* ptr = commandLine;
    char lookFor = 0;
    enum {nowAt_start, nowAt_text} nowAt;
    *pArgv = argv;
    argc = 0;
    GetModuleFileName(hInstance, argv0, sizeof(argv0));
    argv[argc++] = argv0;
    for (nowAt = nowAt_start;;) {
        if (!*ptr)
            return (argc);
        if (lookFor) {
            if (*ptr == lookFor) {
                nowAt = nowAt_start;
                lookFor = 0;
                *ptr = 0;   /* remove the quote */
            } else if (nowAt == nowAt_start) {
                argv[argc++] = ptr;
                nowAt = nowAt_text;
            }
            ptr++;
            continue;
        }
        if (*ptr == ' ' || *ptr == '\t') {
            *ptr = 0;
            ptr++;
            nowAt = nowAt_start;
            continue;
        }
        if ((*ptr == '\'' || *ptr == '\"' || *ptr == '`') && nowAt == nowAt_start) {
            lookFor = *ptr;
            nowAt = nowAt_start;
            ptr++;
            continue;
        }
        if (nowAt == nowAt_start) {
            argv[argc++] = ptr;
            nowAt = nowAt_text;
        }
        ptr++;
    }
}

extern int main(int, char**);
extern HTView * HTView_create(char* name, int rows, int cols, LineMode * pLm);
extern BOOL HTView_destroy(HTView * pView);
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    int argc;
    char** argv;
    WNDCLASS wc = {
    0, //UINT    style
    monitorWndProc, //WNDPROC	lpfnWndProc
    0, //int         cbClsExtra
    sizeof(long), //int         cbWndExtra - HTRequest* and LineMode*
    hInstance, //HINSTANCE	hInstance - fill in later
    LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WWW)), //HICON	hIcon
    LoadCursor(NULL, IDC_ARROW), //HCURSOR	hCursor
    (HBRUSH)(COLOR_WINDOW+1), //HBRUSH	hbrBackground
    MAKEINTRESOURCE(IDM_SNP), //LPCSTR	lpszMenuName
    WWWClassName}; //LPCSTR	lpszClassName

    RegisterClass(&wc); // don't check error as class may be laying around from previously use
    if (!(PMonitorView = HTView_create("WWW Console", 50, 80, 0)))
    	return (0);
    HTTrace_setCallback(&OutputConsole);
    argc = makeArgcArgv(hInstance, &argv, lpszCmdLine);
    main(argc, argv);
    HTView_destroy(PMonitorView);
    PMonitorView = 0;
    return (0);
}

PUBLIC HTView * HTView_create(char* name, int rows, int cols, LineMode * pLm)
{
    HTView * pView;
    DWORD style;
static nextX = 0;
static nextY = 0;

    if ((pView = (HTView *)HT_CALLOC(1, sizeof(HTView))) == NULL || 
	(pView->pScrollInfo = (ScrollInfo_t *)HT_CALLOC(1, sizeof(ScrollInfo_t))) == NULL || 
	(pView->pFontInfo = (FontInfo_t *)HT_CALLOC(1, sizeof(FontInfo_t))) == NULL || 
	(pView->pScrollInfo->screenBuf = (char *)HT_CALLOC(rows * cols, sizeof(char))) == NULL)
	HT_OUTOFMEM("HTView");
    pView->rows = pView->pScrollInfo->maxRows = rows;
    pView->cols = pView->pScrollInfo->maxCols = cols;
    pView->pScrollInfo->control = Scroll_control_crBegetsLf|Scroll_control_lfBegetsCr|Scroll_control_autoWrap|Scroll_control_BSbegetsSpace;
    if (Font_SetupInfo(pView->pFontInfo) || Scroll_SetupInfo(pView->pScrollInfo, rows, cols))
	return (0);
    style = WS_CAPTION | WS_VISIBLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    style |= PMonitorView ? WS_POPUPWINDOW : WS_POPUPWINDOW;
    pView->pLm = pLm;
//    pView->hWnd = CreateWindow(WWWClassName, pView->name, style, CW_USEDEFAULT, CW_USEDEFAULT, 
    pView->hWnd = CreateWindow(WWWClassName, pView->name, style, nextX, nextY, 
				CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, HInstance, (LPVOID)pView);
    nextX += 400;
    nextY += 100;
    return pView;
}

PUBLIC BOOL HTView_destroy(HTView * pView)
{
    DestroyWindow(pView->hWnd);
    Scroll_DestroyInfo(pView->pScrollInfo);
    Font_DestroyInfo(pView->pFontInfo);
	return YES;
}

/* ------------------------------------------------------------------------- */
/*  This was copied from HTDialog.c, will be put into resource or something  */
/*
** Dialog Messages
*/
PRIVATE char *HTDialogs[HT_MSG_ELEMENTS] = {HT_MSG_ENGLISH_INITIALIZER};

/*
** All errors that are not strictly HTTP errors but originates from, e.g., 
** the FTP protocol all have element numbers > HTERR_HTTP_CODES_END, i.e.,
** they should be placed after the blank line
*/
PRIVATE HTErrorMessage HTErrors[HTERR_ELEMENTS] = {HTERR_ENGLISH_INITIALIZER};

/* ------------------------------------------------------------------------- */

#if 0
typedef enum {Dialog_PROMPT, Dialog_CONFIRM, Dialog_Passwd, Dialog_UserPasswd} Dialog_type;
#endif
typedef struct {
    HTRequest * request;
    HTAlertOpcode op;
    int msgnum;
    const char * dfault;
    void * input;
    HTAlertPar * reply;
    int type;
    char * buf;
    size_t bufLen;
    WPARAM * pWParam;
} DialogParms;

BOOL PASCAL AlertDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DialogParms * pParms = (DialogParms *)GetProp(hDlg, MAKEINTATOM(0x100));
    switch (uMsg) {
	case WM_INITDIALOG: {
	    pParms = (DialogParms *)lParam;
	    SetProp(hDlg, MAKEINTATOM(0x100), (HANDLE)pParms);
	    SetDlgItemText(hDlg, IDC_PROMPT1, HTDialogs[pParms->msgnum]);
	    if (pParms->type == IDD_CONFIRM || pParms->type == IDD_PROMPT) {
	    }
	    if (pParms->type == IDD_PROMPT) {
		if (pParms->dfault)
		    SetDlgItemText(hDlg, IDC_TEXT, pParms->dfault);
	    }
	    if (pParms->type == IDD_PASSWD || pParms->type == IDD_USERPASSWD) {
		if (pParms->dfault)
		    SetDlgItemText(hDlg, IDC_USER, pParms->dfault);
	    }
	    return TRUE;
	}

    case WM_COMMAND:
	*pParms->pWParam = LOWORD(wParam);
	switch (LOWORD(wParam)) {
	    case IDOK:
		if (pParms->type == IDD_PROMPT) {
		    char space[256];
		    GetDlgItemText(hDlg, IDC_TEXT, space, sizeof(space) - 1);
		    HTAlert_setReplyMessage(pParms->reply, space);
		}
		if (pParms->type == IDD_PASSWD || pParms->type == IDD_USERPASSWD) {
		    char space[256];
		    GetDlgItemText(hDlg, IDC_PASSWD, space, sizeof(space) - 1);
		    HTAlert_setReplySecret(pParms->reply, space);
		}
		if (pParms->type == IDD_USERPASSWD) {
		    char space[256];
		    GetDlgItemText(hDlg, IDC_USER, space, sizeof(space) - 1);
		    HTAlert_setReplyMessage(pParms->reply, space);
		}
		EndDialog(hDlg, TRUE);
		return (TRUE);

	    case IDCANCEL:
		EndDialog(hDlg, TRUE);
		return (TRUE);
	}
	break;

    case WM_DESTROY:
	RemoveProp(hDlg, MAKEINTATOM(0x100));
	break ;
    }
    return (FALSE);
} // AlertDialogProc()

PRIVATE int DialogParms_do(HTRequest * request, HTAlertOpcode op, 
				      int msgnum, const char * dfault, 
				      void * input, HTAlertPar * reply, 
				      int resource, 
				      WPARAM * pWParam, 
				      HTRequest * pRequest)
{
    int ret;
    HTView * pView;
    DialogParms * pParms;

    pView = LineMode_getView(Context_getLineMode(pRequest));
    if ((pParms = (DialogParms *)HT_CALLOC(1, sizeof(DialogParms))) == NULL)
	HT_OUTOFMEM("DialogParms");
    pParms->request = request;
    pParms->op = op;
    pParms->msgnum = msgnum;
    pParms->dfault = dfault;
    pParms->input = input;
    pParms->reply = reply;
    pParms->type = resource;
    pParms->pWParam = pWParam;
    ret = Lib_ModalDialogBoxParam(HInstance, MAKEINTRESOURCE(resource), pView->hWnd, 
				  (DLGPROC)AlertDialogProc, (LPARAM)pParms);
    HT_FREE(pParms);
    return ret;
}

PUBLIC BOOL HTProgress (HTRequest * request, HTAlertOpcode op,
                                int msgnum, const char * dfault, void * input,
                                HTAlertPar * reply)
{
    static int inside = 0;
    char space[256];
    HTView * pView;
    pView = LineMode_getView(Context_getLineMode(request));
    if (!request) {
	if (WWWTRACE) HTTrace("HTProgress.. Bad argument\n");
	return NO;
    }
    switch (op) {
      case HT_PROG_DNS:
	sprintf(space, "Looking up %s\n", (char *) input);
	break;

      case HT_PROG_CONNECT:
	sprintf(space, "Contacting host...\n");
	break;

      case HT_PROG_ACCEPT:
	sprintf(space, "Waiting for connection...\n");
	break;

      case HT_PROG_READ:
	{
	    long cl = HTAnchor_length(HTRequest_anchor(request));
	    if (cl > 0) {
		long b_read = HTRequest_bodyRead(request);
		double pro = (double) b_read/cl*100;
		char buf[10];
		HTNumToStr((unsigned long) cl, buf, 10);
		sprintf(space, "Read (%d%% of %s)\n", (int) pro, buf);
	    } else
		sprintf(space, "Reading...\n");
	}
	break;

      case HT_PROG_WRITE:
	if (HTRequest_isPostWeb(request)) {
	    HTParentAnchor *anchor=HTRequest_anchor(HTRequest_source(request));
	    long cl = HTAnchor_length(anchor);
	    if (cl > 0) {
		long b_write = HTRequest_bytesWritten(request);
		double pro = (double) b_write/cl*100;
		char buf[10];
		HTNumToStr((unsigned long) cl, buf, 10);
		sprintf(space, "Written (%d%% of %s)\n", (int) pro, buf);
	    } else
		sprintf(space, "Writing...\n");
	} else
	    return YES;
	break;

      case HT_PROG_DONE:
	sprintf(space, "Finished\n");
	break;

      case HT_PROG_OTHER:
	sprintf(space, "Waiting for HT_FREE socket...\n");
	break;

      default:
	sprintf(space, "UNKNOWN PROGRESS STATE\n");
	break;
    }
    if (inside) return YES;
    inside = 1;
    MessageBox(pView->hWnd, space, "HTProgress", MB_OK);
    inside = 0;
    return YES;
}

PUBLIC BOOL HTError_print (HTRequest * request, HTAlertOpcode op,
			   int msgnum, const char * dfault, void * input,
			   HTAlertPar * reply)
{
    HTList *cur = (HTList *) input;
    HTError *pres;
    HTErrorShow showmask = HTError_show();
    HTChunk *msg = NULL;
    int code;
    if (WWWTRACE) HTTrace("HTError..... Generating message\n");
    if (!request || !cur) return NO;
    while ((pres = (HTError *) HTList_nextObject(cur))) {
	int index = HTError_index(pres);
	if (HTError_doShow(pres)) {
	    if (!msg) {
		HTSeverity severity = HTError_severity(pres);
		msg = HTChunk_new(128);
		if (severity == ERR_WARN)
		    HTChunk_puts(msg, "Warning: ");
		else if (severity == ERR_NON_FATAL)
		    HTChunk_puts(msg, "Non Fatal Error: ");
		else if (severity == ERR_FATAL)
		    HTChunk_puts(msg, "Fatal Error: ");
		else if (severity == ERR_INFO)
		    HTChunk_puts(msg, "Information: ");
		else {
		    if (WWWTRACE)
			HTTrace("HTError..... Unknown Classification of Error (%d)...\n", severity);
		    HTChunk_delete(msg);
		    return NO;
		}

		/* Error number */
		if ((code = HTErrors[index].code) > 0) {
		    char buf[10];
		    sprintf(buf, "%d ", code);
		    HTChunk_puts(msg, buf);
		}
	    } else
		HTChunk_puts(msg, "\nReason: ");
	    HTChunk_puts(msg, HTErrors[index].msg);	    /* Error message */

	    if (showmask & HT_ERR_SHOW_PARS) {		 /* Error parameters */
		int length;
		int cnt;		
		char *pars = (char *) HTError_parameter(pres, &length);
		if (length && pars) {
		    HTChunk_puts(msg, " (");
		    for (cnt=0; cnt<length; cnt++) {
			char ch = *(pars+cnt);
			if (ch < 0x20 || ch >= 0x7F)
			    HTChunk_putc(msg, '#');
			else
			    HTChunk_putc(msg, ch);
		    }
		    HTChunk_puts(msg, ") ");
		}
	    }

	    if (showmask & HT_ERR_SHOW_LOCATION) {	   /* Error Location */
		HTChunk_puts(msg, "This occured in ");
		HTChunk_puts(msg, HTError_location(pres));
		HTChunk_putc(msg, '\n');
	    }

	    /*
	    ** Make sure that we don't get this error more than once even
	    ** if we are keeping the error stack from one request to another
	    */
	    HTError_setIgnore(pres);
	    
	    /* If we only are show the most recent entry then break here */
	    if (showmask & HT_ERR_SHOW_FIRST)
		break;
	}
    }
    if (msg) {
	HTChunk_putc(msg, '\n');
	HTTrace("WARNING: %s\n", HTChunk_data(msg));
	HTChunk_delete(msg);
    }
    return YES;
}

PUBLIC BOOL HTConfirm (HTRequest * request, HTAlertOpcode op,
                                int msgnum, const char * dfault, void * input,
                                HTAlertPar * reply)
{
    WPARAM wParam = 0;
    int ret;

    ret = DialogParms_do(request, op, msgnum, dfault, input, reply, 
			 IDD_CONFIRM, &wParam, request);
    return wParam == IDOK;
}

PUBLIC BOOL HTPrompt (HTRequest * request, HTAlertOpcode op,
                                int msgnum, const char * dfault, void * input,
                                HTAlertPar * reply)
{
    WPARAM wParam = 0;
    int ret;

    ret = DialogParms_do(request, op, msgnum, dfault, input, reply, 
			 IDD_PROMPT, &wParam, request);
    return wParam == IDOK;
}

PUBLIC BOOL HTPromptPassword (HTRequest * request, HTAlertOpcode op,
                                int msgnum, const char * dfault, void * input,
                                HTAlertPar * reply)
{
    WPARAM wParam = 0;
    int ret;

    ret = DialogParms_do(request, op, msgnum, dfault, input, reply, 
			 IDD_PASSWD, &wParam, request);
    return wParam == IDOK;
}

PUBLIC BOOL HTPromptUsernameAndPassword (HTRequest * request, HTAlertOpcode op,
                                int msgnum, const char * dfault, void * input,
                                HTAlertPar * reply)
{
    WPARAM wParam = 0;
    int ret;

    ret = DialogParms_do(request, op, msgnum, dfault, input, reply, 
			 IDD_USERPASSWD, &wParam, request);
    return wParam == IDOK;
}

