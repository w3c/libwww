# Microsoft Developer Studio Project File - Name="www" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=www - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "www.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "www.mak" CFG="www - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "www - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "www - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "www - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Bin\Release"
# PROP Intermediate_Dir ".\Release"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\Library\src" /I "..\..\..\PICS-client\src" /I "..\..\..\Library\External" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Bin\Debug"
# PROP Intermediate_Dir ".\Debug"
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /I "..\..\..\PICS-client\src" /I "..\..\..\Library\External" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386

!ENDIF 

# Begin Target

# Name "www - Win32 Release"
# Name "www - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\DefaultStyles.c
# End Source File
# Begin Source File

SOURCE=..\GridStyle.c
# End Source File
# Begin Source File

SOURCE=..\GridText.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\src\HTBInit.c
# End Source File
# Begin Source File

SOURCE=..\HTBrowse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\src\HTHInit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\src\HTInit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\src\HTML.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\src\HTMLPDTD.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\src\HTPlain.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\src\HTProfil.c
# End Source File
# Begin Source File

SOURCE=.\lib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\pics.lib
# End Source File
# Begin Source File

SOURCE=.\scroll.c
# End Source File
# Begin Source File

SOURCE=.\www.c
# End Source File
# Begin Source File

SOURCE=.\www.rc

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwapp.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwcache.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwcore.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwdir.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwdll.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwfile.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwftp.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwgophe.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwhtml.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwhttp.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwmime.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwnews.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwstream.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwtelnt.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwtrans.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwutils.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwwais.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Bin\wwwzip.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
