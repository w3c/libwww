#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
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

PUBLIC int TextToAWindow(HTView * pView, const char * fmt, va_list pArgs)
{
    char space[513];
    int len;

    len = vsprintf(space, fmt, pArgs);
    Scroll_WriteBlock(pView->pScrollInfo, pView->hWnd, (LPSTR)space, len);
    return (len);
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
    	    	    break;

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
    if (!(PMonitorView = HTView_create("WWW Console", 25, 80, 0)))
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
    pView->hWnd = CreateWindow(WWWClassName, pView->name, style, CW_USEDEFAULT, CW_USEDEFAULT, 
				CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, HInstance, (LPVOID)pView);
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
PRIVATE char *HTDialogs[HT_MSG_ELEMENTS] = {
    "Please enter username:",
    "Password:",
    "Please give name of file to save in:",
    "Plase enter account:",
    "It might not be allowed to use this method, continue?",
    "Location has moved, continue?",
    "A new set of rules is to be added to your setup - continue?"
};

/*
** All errors that are not strictly HTTP errors but originates from, e.g., 
** the FTP protocol all have element numbers > HTERR_HTTP_CODES_END, i.e.,
** they should be placed after the blank line
*/
typedef struct _HTErrorMessage {
    int  	code;           	/* Error number */
    char *	msg;          		/* Short explanation */
    char *	url;			/* Explaning URL */
} HTErrorMessage;

PRIVATE HTErrorMessage HTErrors[HTERR_ELEMENTS] = {

/*    CODE  ERROR MESSAGE				ERROR URL */

    { 200, "OK", 					"ok" },
    { 201, "Created", 					"created" },
    { 202, "Accepted", 					"accepted" },
    { 203, "Provisional Information", 			"partial" },
    { 204, "No Content",				"no_response" },
    { 301, "Moved Permanently",				"moved" },
    { 302, "Moved Temporarily", 			"moved" },
    { 303, "Method", 					"method" },
    { 304, "Not Modified",       			"not_modified" },
    { 400, "Bad Request", 				"bad_request" },
    { 401, "Unauthorized",				"unauthorized" },
    { 402, "Payment Required", 				"payment" },
    { 403, "Forbidden", 				"forbidden" },
    { 404, "Not Found",		       			"not_found" },
    { 405, "Method Not Allowed",	 		"method" },
    { 406, "None Acceptable",		 		"none" },
    { 407, "Proxy Authentication Required", 		"proxy" },
    { 408, "Request Timeout",		 		"timeout" },
    { 500, "Internal Server Error",			"internal" },
    { 501, "Not Implemented", 				"not_made" },
    { 502, "Bad Gateway", 				"bad_gate" },
    { 503, "Service Unavailable",			"unavailable" },
    { 504, "Gateway Timeout", 				"timeout" },
    { 0,   "-------------------------", "----------------------------------" },
    { 0,   "Can't locate remote host", 			"host" },
    { 0,   "No host name found", 			"host" },
    { 0,   "No file name found or file not accessible", "file" },
    { 0,   "FTP-server replies", 			"ftp" },
    { 0,   "FTP-server doesn't reply", 			"no_server" },
    { 0,   "Server timed out", 				"time_out" },
    { 0,   "Gopher-server replies", 			"gopher" },
    { 0,   "Data transfer interrupted", 		"interrupt" },
    { 0,   "Connection establishment interrupted", 	"interrupt" },
    { 0,   "CSO-server replies", 			"cso" },
    { 0,   "This is probably a HTTP server 0.9 or less","http_version" },
    { 0,   "Bad, Incomplete, or Unknown Response",	"bad_reply" },
    { 0,   "Unknown access authentication scheme",	"control" },
    { 0,   "News-server replies",			"news" },
    { 0,   "Trying `ftp://' instead of `file://'",	"ftpfile" },
    { 0,   "Too many redirections",			"redirections" },
    { 0,   "Premature End Of File",			"EOF" },
    { 0,   "Response from WAIS Server too Large - Extra lines ignored",
	                                                "wais" },
    { 0,   "WAIS-server doesn't return any data", 	"wais" },
    { 0,   "Can't connect to WAIS-server",		"wais" },
    { 0,   "System replies",				"system" },
    { 0,   "Wrong or unknown access scheme",		"class" },
    { 0,   "Access scheme not allowed in this context",	"class" },
    { 0,   "When you are connected, you can log in",	"telnet" }
};

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
		long b_read = HTRequest_bytesRead(request);
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

      case HT_PROG_WAIT:
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
    MessageBox(0, dfault, "HTError_print", MB_OK);
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

