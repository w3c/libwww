
#include <windows.h>
#include <string.h>
#include "commdlg.h"
//#include "ctrl.h"
#include "lib.h"

#define MAXLEN_TEMPSTR 81

int NEAR Lib_FindStringInTable(HINSTANCE hInstance, char* lookFor, int startId, WORD wTableLen)
	{
	char  szBuffer[MAXLEN_TEMPSTR];
	WORD  wCount;

	for (wCount = 0; wCount < wTableLen; wCount++)
		{
		LoadString(hInstance, startId + wCount, szBuffer, sizeof(szBuffer));
		if (!strcmp(szBuffer, lookFor))
			return (wCount);
		}
	return (-1);		//not found
	} // end of Lib_FindStringInTable()

int NEAR Font_SetupInfo(FontInfo_t* pFont)
	{
	// setup default font information
	pFont->hFont		= 0;
	pFont->rgbFGColor		 = RGB(0, 0, 0);
	pFont->logFont.lfHeight =			12;
	pFont->logFont.lfWidth =			 0;
	pFont->logFont.lfEscapement =	  0;
	pFont->logFont.lfOrientation =	 0;
	pFont->logFont.lfWeight =			0;
	pFont->logFont.lfItalic =			0;
	pFont->logFont.lfUnderline =		0;
	pFont->logFont.lfStrikeOut =		0;
	pFont->logFont.lfCharSet =		  OEM_CHARSET;
	pFont->logFont.lfOutPrecision =	OUT_DEFAULT_PRECIS;
	pFont->logFont.lfClipPrecision =  CLIP_DEFAULT_PRECIS;
	pFont->logFont.lfQuality =		  DEFAULT_QUALITY;
	pFont->logFont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	lstrcpy(pFont->logFont.lfFaceName, "FixedSys");
	return (0);
	}

void NEAR Font_DestroyInfo(FontInfo_t* pFont)
	{
	DeleteObject(pFont->hFont);
	}

//---------------------------------------------------------------------------
//  VOID NEAR Lib_FillComboBox(HINSTANCE hInstance, HWND hCtrlWnd, int nIDString,
//								  WORD NEAR *npTable, WORD wTableLen,
//								  WORD wCurrentSetting)
//
//  Description:
//	  Fills the given combo box with strings from the resource
//	  table starting at nIDString.  Associated items are
//	  added from given table.  The combo box is notified of
//	  the current setting.
//
//  Parameters:
//	  HINSTANCE hInstance
//		  handle to application instance
//
//	  HWND hCtrlWnd
//		  handle to combo box control
//
//	  int nIDString
//		  first resource string id
//
//	  WORD NEAR *npTable
//		  near point to table of associated values
//
//	  WORD wTableLen
//		  length of table
//
//	  WORD wCurrentSetting
//		  current setting (for combo box selection)
//
//---------------------------------------------------------------------------

void NEAR Lib_FillComboBox(HINSTANCE hInstance, HWND hCtrlWnd, int nIDString, DWORD NEAR *npTable, WORD wTableLen, DWORD wCurrentSetting)
	{
	char  szBuffer[MAXLEN_TEMPSTR];
	WORD  wCount, wPosition;

	for (wCount = 0; wCount < wTableLen; wCount++)
		{
		// load the string from the string resources and
		// add it to the combo box
		LoadString(hInstance, nIDString + wCount, szBuffer, sizeof(szBuffer));
		wPosition = LOWORD(SendMessage(hCtrlWnd, CB_ADDSTRING, 0, (LPARAM) (LPSTR) szBuffer));

		// use item data to store the actual table value
		SendMessage(hCtrlWnd, CB_SETITEMDATA, (WPARAM) wPosition, (LPARAM) (LONG) *(npTable + wCount));

		// if this is our current setting, select it
		if (*(npTable + wCount) == wCurrentSetting)
			SendMessage(hCtrlWnd, CB_SETCURSEL, (WPARAM) wPosition, 0);
		}

	} // end of Lib_FillComboBox()

//---------------------------------------------------------------------------
//  VOID NEAR Lib_ModalDialogBoxParam( HINSTANCE hInstance,
//											  LPCSTR lpszTemplate, HWND hWnd,
//											  DLGPROC lpDlgProc, LPARAM lParam )
//
//  Description:
//	  It is a simple utility function that simply performs the
//	  MPI and invokes the dialog box with a DWORD paramter.
//
//  Parameters:
//	  similar to that of DialogBoxParam() with the exception
//	  that the lpDlgProc is not a procedure instance
//
//---------------------------------------------------------------------------

int NEAR Lib_ModalDialogBoxParam(HINSTANCE hInstance, LPCSTR lpszTemplate, HWND hWnd, DLGPROC lpDlgProc, LPARAM lParam)
	{
	DLGPROC  lpProcInstance;
	int ret;

	lpProcInstance = (DLGPROC)MakeProcInstance((FARPROC)lpDlgProc, hInstance);
	ret = DialogBoxParam(hInstance, lpszTemplate, hWnd, lpProcInstance, lParam);
	FreeProcInstance((FARPROC)lpProcInstance);
	return (ret);
	} // end of Lib_ModalDialogBoxParam()

int NEAR Lib_SelectWorkingFont(HWND hWnd, FontInfo_t NEAR* pFont)
	{
	CHOOSEFONT  cfTTYFont ;

	cfTTYFont.lStructSize	 = sizeof(CHOOSEFONT);
	cfTTYFont.hwndOwner		= hWnd;
	cfTTYFont.hDC				= 0;
	cfTTYFont.rgbColors		= pFont->rgbFGColor;
	cfTTYFont.lpLogFont		= &pFont->logFont;
	cfTTYFont.Flags			 = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
	cfTTYFont.lCustData		= 0;
	cfTTYFont.lpfnHook		 = 0;
	cfTTYFont.lpTemplateName = 0;
	cfTTYFont.hInstance		= GETHINST(hWnd);

	if (ChooseFont(&cfTTYFont))
		{
	  pFont->rgbFGColor = cfTTYFont.rgbColors ;
	  return (TRUE);
//	  ResetTTYScreen( GetParent( hWnd ), pXtra) ;
		}
	return (FALSE);
	} // end of Lib_SelectWorkingFont()

