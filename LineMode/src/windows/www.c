#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lib.h"
#include "scroll.h"
#include "wwwrc.h"
#include "WWWLib.h"
#include "../HTBrowse.h"

#define VERSIONSTR "v0.1 beta"	//update this version for each release

// monitor size
#define MAXROWS         25
#define MAXCOLS         80

char WWWWindowName[] = "WWWWindow";
char WWWClassName[] = "WWWClass";
char VersionText[] = "WWW.EXE "VERSIONSTR".";

HINSTANCE HInstance;
HWND MonitorWindow = 0;

char ScreenBuffer[MAXROWS * MAXCOLS] = "";
ScrollInfo_t ScrollInfo = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MAXROWS, MAXCOLS, //int     xSize, ySize, xScroll, yScroll, xOffset, yOffset, nColumn, nRow, xChar, yChar, maxRows, maxCols;
	Scroll_control_crBegetsLf|Scroll_control_autoWrap, //Scroll_control_t control;
	ScreenBuffer, //BYTE* abScreen;
	0, //POINT maxTrackSize;
	0 //WORD    wCursorState;
	};
FontInfo_t FontInfo = {
	0, //HFONT   hFont ;
	0, //LOGFONT logFont ;
	0 //DWORD   rgbFGColor ;
	};

#ifdef WWW_WIN_DLL
int AppTTYPrint(unsigned int target, const char* fmt, ...)
#else
int TTYPrint(unsigned int target, const char* fmt, ...)
#endif
	{
	int len;
	char space[513];
	char* pArgs;

	pArgs = (char*)(&fmt + 1);
	len = vsprintf(space, (char*)fmt, (char*)pArgs);
	Scroll_WriteBlock(&ScrollInfo, MonitorWindow, (LPSTR)space, len);
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
	switch (uMsg)
		{
		case WM_USER+1:	//we just got a data set
			return (0);

		case WM_CREATE:
			{
			HMENU hMenu = GetMenu(hWnd);
			ScrollInfo.control = Scroll_control_autoWrap | Scroll_control_crBegetsLf | Scroll_control_lfBegetsCr;// | Scroll_control_literal;
			if (Font_SetupInfo(&FontInfo) || Scroll_SetupInfo(&ScrollInfo, MAXROWS, MAXCOLS))
				return (-1);
			Scroll_ResetScreen(hWnd, &ScrollInfo, &FontInfo);
			MoveWindow(hWnd, 0, 0, ScrollInfo.maxTrackSize.x, ScrollInfo.maxTrackSize.y, FALSE);
			return (0);
			}

#define USEMINMAX
#ifdef USEMINMAX
		case WM_GETMINMAXINFO:
			{
			LPMINMAXINFO pMax = (LPMINMAXINFO)lParam;
			pMax->ptMaxTrackSize = ScrollInfo.maxTrackSize;
			}
			break;
#endif

		case WM_COMMAND:
			switch ((WORD) wParam)
				{
//				case IDM_CRBEGETSLF:
//					ScrollInfo.control ^= Scroll_control_crBegetsLf;
//					CheckMenuItem(GetMenu(hWnd), (UINT)IDM_CRBEGETSLF, (ScrollInfo.control & Scroll_control_crBegetsLf) ? MF_CHECKED : MF_UNCHECKED);
					break;

				case IDM_FONT:
					if (Lib_SelectWorkingFont(hWnd, &FontInfo))
					  Scroll_ResetScreen(hWnd, &ScrollInfo, &FontInfo) ;
	  			return (TRUE);
					break;

				case IDM_ABOUT:
					while (MessageBox(hWnd, "It's a program, all right?", "About ExpoSrv.DLL", MB_OKCANCEL) != IDOK);
					break;

				case IDM_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0L);
					break ;
				}
			break;

		case WM_PAINT:
			Scroll_Paint(&ScrollInfo, &FontInfo, hWnd, 1);
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
			if (style & (WS_VSCROLL | WS_HSCROLL) && rect.right-rect.left == ScrollInfo.maxTrackSize.x && rect.bottom-rect.top == ScrollInfo.maxTrackSize.y)
				{
				recursing = 1;
				Scroll_SetSize(&ScrollInfo, hWnd, (WORD)(HIWORD(lParam)+yScroll), (WORD)(LOWORD(lParam)+xScroll));
				recursing = 0;
				}
			else
				Scroll_SetSize(&ScrollInfo, hWnd, HIWORD(lParam), LOWORD(lParam));
			return (0);
			}
			break ;

		case WM_HSCROLL:
#ifdef _WIN32
			Scroll_SetHorz(&ScrollInfo, hWnd, LOWORD(wParam), HIWORD(wParam));
#else
			Scroll_SetHorz(&ScrollInfo, hWnd, (WORD) wParam, LOWORD(lParam));
#endif
			break ;

		case WM_VSCROLL:
#ifdef _WIN32
			Scroll_SetVert(&ScrollInfo, hWnd, LOWORD(wParam), HIWORD(wParam));
#else
			Scroll_SetVert(&ScrollInfo, hWnd, (WORD) wParam, LOWORD(lParam));
#endif
			break ;

		case WM_CHAR:
			{
#ifdef _WIN32
			BYTE bOut = LOBYTE(LOWORD(wParam));
#else
			BYTE bOut = LOBYTE(wParam);
#endif
			if (bufferInput(&bOut, 1, 0, 0, 0) == -1)
				PostMessage(hWnd, WM_QUIT, 0, 0);
			Scroll_WriteBlock(&ScrollInfo, hWnd, &bOut, 1);
			}
			break ;

		case WM_SETFOCUS:
			Scroll_SetFocus(hWnd, &ScrollInfo);
			break ;

		case WM_KILLFOCUS:
			Scroll_KillFocus(hWnd, &ScrollInfo);
			break ;

		case WM_DESTROY:
			{
			Scroll_DestroyInfo(&ScrollInfo);
			Font_DestroyInfo(&FontInfo);
			PostMessage(hWnd, WM_QUIT, 0, 0);
			}
			break ;

		default:
			return (DefWindowProc(hWnd, uMsg, wParam, lParam));
		}
	return 0L;
	} // end of monitorWndProc()

extern int main(int, char**);
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
	{
	WNDCLASS wc = {
	0, //UINT        style
	monitorWndProc, //WNDPROC	lpfnWndProc
	0, //int         cbClsExtra
	0, //int         cbWndExtra
	hInstance, //HINSTANCE	hInstance - fill in later
	0, //HICON	hIcon
	0, //HCURSOR	hCursor
	0, //HBRUSH	hbrBackground
	MAKEINTRESOURCE(IDM_SNP), //LPCSTR	lpszMenuName
	WWWClassName}; //LPCSTR	lpszClassName
	HInstance = hInstance;

	RegisterClass(&wc); // don't check error as class may be laying around from previously use
	MonitorWindow = CreateWindow(WWWClassName, WWWWindowName, 
			WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, HInstance, 0);
	if (!MonitorWindow)
		return (0);
#ifdef WWW_WIN_DLL
	*PTTYPrint = &AppTTYPrint;
#endif
	main(0, 0);
	if (MonitorWindow)	//if we don't kill monitor window, Windows will cause app ended
		if (DestroyWindow(MonitorWindow))
			MonitorWindow = 0;
	Scroll_DestroyInfo(&ScrollInfo);
	Font_DestroyInfo(&FontInfo);
	return (0);
	}

