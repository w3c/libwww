
// data structures
typedef struct
	{
	HFONT   hFont ;
	LOGFONT logFont ;
	DWORD   rgbFGColor ;
	} FontInfo_t;

// macros ( for easier readability )
#ifdef _WIN32
#define GETHINST(x)  ((HINSTANCE)GetWindowLong(x, GWL_HINSTANCE))
#else
#define GETHINST(x)  ((HINSTANCE) GetWindowWord(x, GWW_HINSTANCE))
#endif
#define SET_PROP( x, y, z )  SetProp( x, MAKEINTATOM( y ), z )
#define GET_PROP( x, y )     GetProp( x, MAKEINTATOM( y ) )
#define REMOVE_PROP( x, y )  RemoveProp( x, MAKEINTATOM( y ) )

// CRT mappings to NT API
#define _fmemset   memset
#define _fmemmove  memmove

//Lib_
int NEAR Lib_FindStringInTable(HINSTANCE hInstance, char* lookFor, int startId, WORD wTableLen);
int NEAR Font_SetupInfo(FontInfo_t* pFont);
void NEAR Font_DestroyInfo(FontInfo_t* pFont);
void NEAR Lib_FillComboBox(HINSTANCE, HWND, int, DWORD NEAR *, WORD, DWORD);
int NEAR Lib_ModalDialogBoxParam(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM);
int NEAR Lib_SelectWorkingFont(HWND hWnd, FontInfo_t NEAR* pFont);

