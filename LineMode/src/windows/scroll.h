
typedef enum {Scroll_control_crBegetsLf = 0x01, Scroll_control_lfBegetsCr = 0x02, Scroll_control_autoWrap = 0x04, Scroll_control_literal = 0x08} Scroll_control_t;
typedef enum {Scroll_control_null, 
							Scroll_control_CR, Scroll_control_LF, Scroll_control_BS, Scroll_control_Tab, Scroll_control_Bell, 
							Scroll_control_clearEol, Scroll_control_goto, 
							Scroll_control_normal, Scroll_control_bold, Scroll_control_inverse
						 } Scroll_control2_t;

typedef struct
	{
	int     xSize, ySize, xScroll, yScroll, xOffset, yOffset, nColumn, nRow, xChar, yChar, maxRows, maxCols;
	Scroll_control_t control;
	BYTE* abScreen;
	POINT maxTrackSize;
	WORD    wCursorState;
	} ScrollInfo_t;

//Scroll_
int NEAR Scroll_SetupInfo(ScrollInfo_t* pScroll, int maxRows, int maxCols);
void NEAR Scroll_DestroyInfo(ScrollInfo_t* pScroll);
int NEAR Scroll_WriteControl(ScrollInfo_t* pScroll, HWND hWnd, Scroll_control2_t control, int x, int y);
int NEAR Scroll_WriteBlock(ScrollInfo_t* pScroll, HWND, LPSTR, int);
int NEAR Scroll_SetSize(ScrollInfo_t*, HWND, WORD, WORD);
int NEAR Scroll_SetVert(ScrollInfo_t*, HWND, WORD, WORD);
int NEAR Scroll_SetHorz(ScrollInfo_t*, HWND, WORD, WORD);
int NEAR Scroll_SetFocus(HWND, ScrollInfo_t*);
int NEAR Scroll_KillFocus(HWND, ScrollInfo_t*);
int NEAR Scroll_Paint(ScrollInfo_t* pScroll, FontInfo_t* pFont, HWND hWnd, int showCursor);
int NEAR Scroll_ResetScreen(HWND hWnd, ScrollInfo_t* pScroll, FontInfo_t* pFont);

