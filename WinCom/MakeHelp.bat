@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by WINCOM.HPJ. >"hlp\WinCom.hm"
echo. >>"hlp\WinCom.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\WinCom.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\WinCom.hm"
echo. >>"hlp\WinCom.hm"
echo // Prompts (IDP_*) >>"hlp\WinCom.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\WinCom.hm"
echo. >>"hlp\WinCom.hm"
echo // Resources (IDR_*) >>"hlp\WinCom.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\WinCom.hm"
echo. >>"hlp\WinCom.hm"
echo // Dialogs (IDD_*) >>"hlp\WinCom.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\WinCom.hm"
echo. >>"hlp\WinCom.hm"
echo // Frame Controls (IDW_*) >>"hlp\WinCom.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\WinCom.hm"
REM -- Make help for Project WINCOM


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\WinCom.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\WinCom.hlp" goto :Error
if not exist "hlp\WinCom.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\WinCom.hlp" ..\Bin\Debug
if exist Debug\nul copy "hlp\WinCom.cnt" ..\Bin\Debug
if exist Release\nul copy "hlp\WinCom.hlp" ..\Bin\Release
if exist Release\nul copy "hlp\WinCom.cnt" ..\Bin\Release
echo.
goto :done

:Error
echo hlp\WinCom.hpj(1) : error: Problem encountered creating help file

:done
echo.

