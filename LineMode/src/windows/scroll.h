/* scroll.h - This header represents the interface to the Scroll library. The 
   Scroll library provides a simple tty interface to a Windows app. This app is
   similar to a console app except that it runs as a Windows program and has the
   full Windows API available to it.
 */

/* I, Eric Prud'hommeaux, grant unlimited use of this code to the WWW consortium.
   I retain the copyright ownership and expect to be contacted for licencing if 
   the Scroll library or the library interface are used in any commercial products 
   or ventures.

   Also, please contact me if for a CURSES version of this.
   eric@apocalypse.org
 */

typedef enum {Scroll_control_crBegetsLf = 0x01, Scroll_control_lfBegetsCr = 0x02, Scroll_control_BSbegetsSpace = 0x04, Scroll_control_autoWrap = 0x08, Scroll_control_literal = 0x10} Scroll_control_t;
typedef enum {Scroll_control_null, 
							Scroll_control_CR, Scroll_control_LF, Scroll_control_BS, Scroll_control_Tab, Scroll_control_Bell, 
							Scroll_control_clearEol, Scroll_control_goto, 
							Scroll_control_normal, Scroll_control_bold, Scroll_control_inverse
						 } Scroll_control2_t;
typedef enum {Scroll_cursorHide = 0, Scroll_cursorShow = 1, Scroll_cursorWait = 2, Scroll_cursorNormal = 3} CursorState_t;

typedef struct
	{
	int     xSize, ySize, xScroll, yScroll, xOffset, yOffset, nColumn, nRow, xChar, yChar, maxRows, maxCols;
	Scroll_control_t control;
	BYTE* screenBuf;
	POINT maxTrackSize;
    CursorState_t cursorState;
	} ScrollInfo_t;

//Scroll_
int Scroll_SetupInfo(ScrollInfo_t* pScroll, int maxRows, int maxCols);
void Scroll_DestroyInfo(ScrollInfo_t* pScroll);
int Scroll_WriteControl(ScrollInfo_t* pScroll, HWND hWnd, Scroll_control2_t control, int x, int y);
int Scroll_WriteBlock(ScrollInfo_t* pScroll, HWND, LPSTR, int);
int Scroll_SetSize(ScrollInfo_t*, HWND, WORD, WORD);
int Scroll_SetVert(ScrollInfo_t*, HWND, WORD, WORD);
int Scroll_SetHorz(ScrollInfo_t*, HWND, WORD, WORD);
int Scroll_SetFocus(HWND, ScrollInfo_t*);
int Scroll_KillFocus(HWND, ScrollInfo_t*);
int Scroll_Paint(ScrollInfo_t* pScroll, FontInfo_t* pFont, HWND hWnd, CursorState_t cursorState);
int Scroll_ResetScreen(HWND hWnd, ScrollInfo_t* pScroll, FontInfo_t* pFont);

