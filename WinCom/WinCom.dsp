# Microsoft Developer Studio Project File - Name="WinCom" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WinCom - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinCom.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinCom.mak" CFG="WinCom - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinCom - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WinCom - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinCom - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Library\src" /I "..\Library\External" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "WWW_WIN_DLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /fo"..\Bin\Release\WinCom.res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "WinCom - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Library\src" /I "..\Library\External" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "WWW_WIN_DLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /fo"..\Bin\Debug\WinCom.res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WinCom - Win32 Release"
# Name "WinCom - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Delete.cpp
# End Source File
# Begin Source File

SOURCE=.\EntityInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Library\src\HTBInit.c
# End Source File
# Begin Source File

SOURCE=..\Library\src\HTInit.c
# End Source File
# Begin Source File

SOURCE=..\Library\src\HTProfil.c
# End Source File
# Begin Source File

SOURCE=.\Links.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkView.cpp
# End Source File
# Begin Source File

SOURCE=.\Listvwex.cpp
# End Source File
# Begin Source File

SOURCE=.\Load.cpp
# End Source File
# Begin Source File

SOURCE=.\Location.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Password.cpp
# End Source File
# Begin Source File

SOURCE=.\Progress.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxySetup.cpp
# End Source File
# Begin Source File

SOURCE=.\Request.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabsView.cpp
# End Source File
# Begin Source File

SOURCE=.\UserParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\VersionConflict.cpp
# End Source File
# Begin Source File

SOURCE=.\WinCom.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\WinCom.hpj
USERDEP__WINCO="$(ProjDir)\hlp\AfxCore.rtf"	"$(ProjDir)\hlp\AfxPrint.rtf"	
# End Source File
# Begin Source File

SOURCE=.\WinCom.rc

!IF  "$(CFG)" == "WinCom - Win32 Release"

!ELSEIF  "$(CFG)" == "WinCom - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WinComDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwapp.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwcache.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwcore.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwdir.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwfile.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwftp.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwgophe.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwhtml.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwhttp.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwmime.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwnews.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwstream.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwtelnt.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwtrans.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwutils.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\wwwzip.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Delete.h
# End Source File
# Begin Source File

SOURCE=.\EntityInfo.h
# End Source File
# Begin Source File

SOURCE=.\Links.h
# End Source File
# Begin Source File

SOURCE=.\LinkView.h
# End Source File
# Begin Source File

SOURCE=.\Listvwex.h
# End Source File
# Begin Source File

SOURCE=.\Load.h
# End Source File
# Begin Source File

SOURCE=.\Location.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Password.h
# End Source File
# Begin Source File

SOURCE=.\Progress.h
# End Source File
# Begin Source File

SOURCE=.\ProxySetup.h
# End Source File
# Begin Source File

SOURCE=.\Request.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabsView.h
# End Source File
# Begin Source File

SOURCE=.\UserParameters.h
# End Source File
# Begin Source File

SOURCE=.\VersionConflict.h
# End Source File
# Begin Source File

SOURCE=.\WinCom.h
# End Source File
# Begin Source File

SOURCE=.\WinComDoc.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WinCom.ico
# End Source File
# Begin Source File

SOURCE=.\res\WinCom.rc2
# End Source File
# Begin Source File

SOURCE=.\res\WinComDoc.ico
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hlp\AfxCore.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AfxPrint.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AppExit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw2.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw4.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\MakeHelp.bat
# End Source File
# Begin Source File

SOURCE=.\hlp\RecFirst.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecLast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecNext.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecPrev.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\WinCom.cnt
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Filecopy.avi
# End Source File
# End Target
# End Project
