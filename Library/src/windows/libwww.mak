# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=msvc_40 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to msvc_40 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "msvc_40 - Win32 Release" && "$(CFG)" !=\
 "msvc_40 - Win32 Debug" && "$(CFG)" != "wwwdll - Win32 Release" && "$(CFG)" !=\
 "wwwdll - Win32 Debug" && "$(CFG)" != "wwwutils - Win32 Release" && "$(CFG)" !=\
 "wwwutils - Win32 Debug" && "$(CFG)" != "wwwcore - Win32 Release" && "$(CFG)"\
 != "wwwcore - Win32 Debug" && "$(CFG)" != "wwwcache - Win32 Release" &&\
 "$(CFG)" != "wwwcache - Win32 Debug" && "$(CFG)" != "wwwmime - Win32 Release"\
 && "$(CFG)" != "wwwmime - Win32 Debug" && "$(CFG)" != "wwwhttp - Win32 Release"\
 && "$(CFG)" != "wwwhttp - Win32 Debug" && "$(CFG)" !=\
 "wwwrules - Win32 Release" && "$(CFG)" != "wwwrules - Win32 Debug" && "$(CFG)"\
 != "wwwnews - Win32 Release" && "$(CFG)" != "wwwnews - Win32 Debug" && "$(CFG)"\
 != "wwwgophe - Win32 Release" && "$(CFG)" != "wwwgophe - Win32 Debug" &&\
 "$(CFG)" != "wwwftp - Win32 Release" && "$(CFG)" != "wwwftp - Win32 Debug" &&\
 "$(CFG)" != "wwwdir - Win32 Release" && "$(CFG)" != "wwwdir - Win32 Debug" &&\
 "$(CFG)" != "wwwfile - Win32 Release" && "$(CFG)" != "wwwfile - Win32 Debug" &&\
 "$(CFG)" != "wwwtelnt - Win32 Release" && "$(CFG)" != "wwwtelnt - Win32 Debug"\
 && "$(CFG)" != "wwwhtml - Win32 Release" && "$(CFG)" != "wwwhtml - Win32 Debug"\
 && "$(CFG)" != "wwwapp - Win32 Release" && "$(CFG)" != "wwwapp - Win32 Debug"\
 && "$(CFG)" != "wwwwais - Win32 Release" && "$(CFG)" != "wwwwais - Win32 Debug"\
 && "$(CFG)" != "wwwtrans - Win32 Release" && "$(CFG)" !=\
 "wwwtrans - Win32 Debug" && "$(CFG)" != "wwwstream - Win32 Release" && "$(CFG)"\
 != "wwwstream - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "libwww.mak" CFG="msvc_40 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msvc_40 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "msvc_40 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwdll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwdll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwutils - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwutils - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwcore - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwcore - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwcache - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwcache - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwmime - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwmime - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwhttp - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwhttp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwrules - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwrules - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwnews - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwnews - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwgophe - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwgophe - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwftp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwftp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwdir - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwdir - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwfile - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwfile - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwtelnt - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwtelnt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwhtml - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwhtml - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwapp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwapp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwwais - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwwais - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwtrans - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwtrans - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwstream - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwstream - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "wwwstream - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "wwwstream - Win32 Release" "wwwtrans - Win32 Release"\
 "wwwwais - Win32 Release" "wwwapp - Win32 Release" "wwwhtml - Win32 Release"\
 "wwwtelnt - Win32 Release" "wwwfile - Win32 Release" "wwwdir - Win32 Release"\
 "wwwftp - Win32 Release" "wwwgophe - Win32 Release" "wwwnews - Win32 Release"\
 "wwwrules - Win32 Release" "wwwhttp - Win32 Release" "wwwmime - Win32 Release"\
 "wwwcache - Win32 Release" "wwwcore - Win32 Release" "wwwutils - Win32 Release"\
 "wwwdll - Win32 Release" "$(OUTDIR)\msvc_40.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\msvc_40.dll"
	-@erase ".\Release\wwwdll.obj"
	-@erase ".\Release\windll.obj"
	-@erase ".\Release\msvc_40.lib"
	-@erase ".\Release\msvc_40.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/libwww.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/libwww.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/msvc_40.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/msvc_40.pdb" /machine:I386\
 /out:"../../../Bin/Release/msvc_40.dll" /implib:"$(OUTDIR)/msvc_40.lib" 
LINK32_OBJS= \
	"$(INTDIR)/wwwdll.obj" \
	"$(INTDIR)/windll.obj" \
	".\wwwdll\Release\wwwdll.lib" \
	".\wwwutils\Release\wwwutils.lib" \
	".\wwwcore\Release\wwwcore.lib" \
	".\wwwcache\Release\wwwcache.lib" \
	".\wwwmime\Release\wwwmime.lib" \
	".\wwwhttp\Release\wwwhttp.lib" \
	".\wwwrules\Release\wwwrules.lib" \
	".\wwwnews\Release\wwwnews.lib" \
	".\wwwgophe\Release\wwwgophe.lib" \
	".\wwwftp\Release\wwwftp.lib" \
	".\wwwdir\Release\wwwdir.lib" \
	".\wwwfile\Release\wwwfile.lib" \
	".\wwwtelnt\Release\wwwtelnt.lib" \
	".\wwwhtml\Release\wwwhtml.lib" \
	".\wwwapp\Release\wwwapp.lib" \
	".\wwwwais\Release\wwwwais.lib" \
	".\wwwtrans\Release\wwwtrans.lib" \
	".\wwwstream\Release\wwwstream.lib"

"$(OUTDIR)\msvc_40.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "wwwstream - Win32 Debug" "wwwtrans - Win32 Debug"\
 "wwwwais - Win32 Debug" "wwwapp - Win32 Debug" "wwwhtml - Win32 Debug"\
 "wwwtelnt - Win32 Debug" "wwwfile - Win32 Debug" "wwwdir - Win32 Debug"\
 "wwwftp - Win32 Debug" "wwwgophe - Win32 Debug" "wwwnews - Win32 Debug"\
 "wwwrules - Win32 Debug" "wwwhttp - Win32 Debug" "wwwmime - Win32 Debug"\
 "wwwcache - Win32 Debug" "wwwcore - Win32 Debug" "wwwutils - Win32 Debug"\
 "wwwdll - Win32 Debug" "$(OUTDIR)\msvc_40.dll" ".\Debug\libwww.bsc"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\libwww.bsc"
	-@erase ".\Debug\windll.sbr"
	-@erase ".\Debug\wwwdll.sbr"
	-@erase "..\..\..\Bin\Debug\msvc_40.dll"
	-@erase ".\Debug\windll.obj"
	-@erase ".\Debug\wwwdll.obj"
	-@erase "..\..\..\Bin\Debug\msvc_40.ilk"
	-@erase ".\Debug\msvc_40.lib"
	-@erase ".\Debug\msvc_40.exp"
	-@erase ".\Debug\msvc_40.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /FR /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /FR"$(INTDIR)/" /Fp"$(INTDIR)/libwww.pch"\
 /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/libwww.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/windll.sbr" \
	"$(INTDIR)/wwwdll.sbr"

".\Debug\libwww.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/msvc_40.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/msvc_40.pdb" /debug /machine:I386\
 /out:"../../../Bin/Debug/msvc_40.dll" /implib:"$(OUTDIR)/msvc_40.lib" 
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/wwwdll.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwcache\Debug\wwwcache.lib" \
	".\wwwmime\Debug\wwwmime.lib" \
	".\wwwhttp\Debug\wwwhttp.lib" \
	".\wwwrules\Debug\wwwrules.lib" \
	".\wwwnews\Debug\wwwnews.lib" \
	".\wwwgophe\Debug\wwwgophe.lib" \
	".\wwwftp\Debug\wwwftp.lib" \
	".\wwwdir\Debug\wwwdir.lib" \
	".\wwwfile\Debug\wwwfile.lib" \
	".\wwwtelnt\Debug\wwwtelnt.lib" \
	".\wwwhtml\Debug\wwwhtml.lib" \
	".\wwwapp\Debug\wwwapp.lib" \
	".\wwwwais\Debug\wwwwais.lib" \
	".\wwwtrans\Debug\wwwtrans.lib" \
	".\wwwstream\Debug\wwwstream.lib"

"$(OUTDIR)\msvc_40.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwdll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwdll\Release"
# PROP BASE Intermediate_Dir "wwwdll\Release"
# PROP BASE Target_Dir "wwwdll"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwdll\Release"
# PROP Intermediate_Dir "wwwdll\Release"
# PROP Target_Dir "wwwdll"
OUTDIR=.\wwwdll\Release
INTDIR=.\wwwdll\Release

ALL : "$(OUTDIR)\wwwdll.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwdll.dll"
	-@erase ".\wwwdll\Release\windll.obj"
	-@erase ".\wwwdll\Release\wwwdll.obj"
	-@erase ".\wwwdll\Release\wwwdll.lib"
	-@erase ".\wwwdll\Release\wwwdll.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwdll.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwdll\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwdll.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwdll.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwdll.pdb" /machine:I386 /def:".\wwwdll.def"\
 /out:"../../../Bin/Release/wwwdll.dll" /implib:"$(OUTDIR)/wwwdll.lib" 
DEF_FILE= \
	".\wwwdll.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/wwwdll.obj"

"$(OUTDIR)\wwwdll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwdll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwdll\Debug"
# PROP BASE Intermediate_Dir "wwwdll\Debug"
# PROP BASE Target_Dir "wwwdll"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwdll\Debug"
# PROP Intermediate_Dir "wwwdll\Debug"
# PROP Target_Dir "wwwdll"
OUTDIR=.\wwwdll\Debug
INTDIR=.\wwwdll\Debug

ALL : "$(OUTDIR)\wwwdll.dll"

CLEAN : 
	-@erase ".\wwwdll\Debug\vc40.pdb"
	-@erase ".\wwwdll\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwdll.dll"
	-@erase ".\wwwdll\Debug\wwwdll.obj"
	-@erase ".\wwwdll\Debug\windll.obj"
	-@erase "..\..\..\Bin\Debug\wwwdll.ilk"
	-@erase ".\wwwdll\Debug\wwwdll.lib"
	-@erase ".\wwwdll\Debug\wwwdll.exp"
	-@erase ".\wwwdll\Debug\wwwdll.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwdll.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwdll\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwdll.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwdll.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwdll.pdb" /debug /machine:I386 /def:".\wwwdll.def"\
 /out:"../../../Bin/Debug/wwwdll.dll" /implib:"$(OUTDIR)/wwwdll.lib" 
DEF_FILE= \
	".\wwwdll.def"
LINK32_OBJS= \
	"$(INTDIR)/wwwdll.obj" \
	"$(INTDIR)/windll.obj"

"$(OUTDIR)\wwwdll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwutils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwutils\Release"
# PROP BASE Intermediate_Dir "wwwutils\Release"
# PROP BASE Target_Dir "wwwutils"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwutils\Release"
# PROP Intermediate_Dir "wwwutils\Release"
# PROP Target_Dir "wwwutils"
OUTDIR=.\wwwutils\Release
INTDIR=.\wwwutils\Release

ALL : "$(OUTDIR)\wwwutils.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwutils.dll"
	-@erase ".\wwwutils\Release\HTAssoc.obj"
	-@erase ".\wwwutils\Release\HTArray.obj"
	-@erase ".\wwwutils\Release\HTUU.obj"
	-@erase ".\wwwutils\Release\HTChunk.obj"
	-@erase ".\wwwutils\Release\HTAtom.obj"
	-@erase ".\wwwutils\Release\HTMemory.obj"
	-@erase ".\wwwutils\Release\HTString.obj"
	-@erase ".\wwwutils\Release\HTList.obj"
	-@erase ".\wwwutils\Release\windll.obj"
	-@erase ".\wwwutils\Release\wwwutils.lib"
	-@erase ".\wwwutils\Release\wwwutils.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwutils.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwutils\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwutils.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwutils.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwutils.pdb" /machine:I386 /def:".\wwwutils.def"\
 /out:"../../../Bin/Release/wwwutils.dll" /implib:"$(OUTDIR)/wwwutils.lib" 
DEF_FILE= \
	".\wwwutils.def"
LINK32_OBJS= \
	"$(INTDIR)/HTAssoc.obj" \
	"$(INTDIR)/HTArray.obj" \
	"$(INTDIR)/HTUU.obj" \
	"$(INTDIR)/HTChunk.obj" \
	"$(INTDIR)/HTAtom.obj" \
	"$(INTDIR)/HTMemory.obj" \
	"$(INTDIR)/HTString.obj" \
	"$(INTDIR)/HTList.obj" \
	"$(INTDIR)/windll.obj" \
	".\wwwdll\Debug\wwwdll.lib"

"$(OUTDIR)\wwwutils.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwutils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwutils\Debug"
# PROP BASE Intermediate_Dir "wwwutils\Debug"
# PROP BASE Target_Dir "wwwutils"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwutils\Debug"
# PROP Intermediate_Dir "wwwutils\Debug"
# PROP Target_Dir "wwwutils"
OUTDIR=.\wwwutils\Debug
INTDIR=.\wwwutils\Debug

ALL : "$(OUTDIR)\wwwutils.dll"

CLEAN : 
	-@erase ".\wwwutils\Debug\vc40.pdb"
	-@erase ".\wwwutils\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwutils.dll"
	-@erase ".\wwwutils\Debug\HTList.obj"
	-@erase ".\wwwutils\Debug\HTMemory.obj"
	-@erase ".\wwwutils\Debug\windll.obj"
	-@erase ".\wwwutils\Debug\HTString.obj"
	-@erase ".\wwwutils\Debug\HTUU.obj"
	-@erase ".\wwwutils\Debug\HTArray.obj"
	-@erase ".\wwwutils\Debug\HTChunk.obj"
	-@erase ".\wwwutils\Debug\HTAssoc.obj"
	-@erase ".\wwwutils\Debug\HTAtom.obj"
	-@erase "..\..\..\Bin\Debug\wwwutils.ilk"
	-@erase ".\wwwutils\Debug\wwwutils.lib"
	-@erase ".\wwwutils\Debug\wwwutils.exp"
	-@erase ".\wwwutils\Debug\wwwutils.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwutils.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwutils\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwutils.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwutils.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwutils.pdb" /debug /machine:I386 /def:".\wwwutils.def"\
 /out:"../../../Bin/Debug/wwwutils.dll" /implib:"$(OUTDIR)/wwwutils.lib" 
DEF_FILE= \
	".\wwwutils.def"
LINK32_OBJS= \
	"$(INTDIR)/HTList.obj" \
	"$(INTDIR)/HTMemory.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTString.obj" \
	"$(INTDIR)/HTUU.obj" \
	"$(INTDIR)/HTArray.obj" \
	"$(INTDIR)/HTChunk.obj" \
	"$(INTDIR)/HTAssoc.obj" \
	"$(INTDIR)/HTAtom.obj" \
	".\wwwdll\Debug\wwwdll.lib"

"$(OUTDIR)\wwwutils.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwcore - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwcore\Release"
# PROP BASE Intermediate_Dir "wwwcore\Release"
# PROP BASE Target_Dir "wwwcore"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwcore\Release"
# PROP Intermediate_Dir "wwwcore\Release"
# PROP Target_Dir "wwwcore"
OUTDIR=.\wwwcore\Release
INTDIR=.\wwwcore\Release

ALL : "$(OUTDIR)\wwwcore.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwcore.dll"
	-@erase ".\wwwcore\Release\HTProt.obj"
	-@erase ".\wwwcore\Release\HTInet.obj"
	-@erase ".\wwwcore\Release\HTAlert.obj"
	-@erase ".\wwwcore\Release\HTEvent.obj"
	-@erase ".\wwwcore\Release\HTNet.obj"
	-@erase ".\wwwcore\Release\HTChannl.obj"
	-@erase ".\wwwcore\Release\HTHost.obj"
	-@erase ".\wwwcore\Release\HTFormat.obj"
	-@erase ".\wwwcore\Release\HTEscape.obj"
	-@erase ".\wwwcore\Release\HTLink.obj"
	-@erase ".\wwwcore\Release\HTStream.obj"
	-@erase ".\wwwcore\Release\HTUTree.obj"
	-@erase ".\wwwcore\Release\HTAnchor.obj"
	-@erase ".\wwwcore\Release\windll.obj"
	-@erase ".\wwwcore\Release\HTReqMan.obj"
	-@erase ".\wwwcore\Release\HTFWrite.obj"
	-@erase ".\wwwcore\Release\HTError.obj"
	-@erase ".\wwwcore\Release\HTWWWStr.obj"
	-@erase ".\wwwcore\Release\HTMethod.obj"
	-@erase ".\wwwcore\Release\HTUser.obj"
	-@erase ".\wwwcore\Release\HTDNS.obj"
	-@erase ".\wwwcore\Release\HTTrans.obj"
	-@erase ".\wwwcore\Release\HTBind.obj"
	-@erase ".\wwwcore\Release\HTParse.obj"
	-@erase ".\wwwcore\Release\HTLib.obj"
	-@erase ".\wwwcore\Release\wwwcore.lib"
	-@erase ".\wwwcore\Release\wwwcore.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwcore.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwcore\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwcore.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwcore.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwcore.pdb" /machine:I386 /def:".\wwwcore.def"\
 /out:"../../../Bin/Release/wwwcore.dll" /implib:"$(OUTDIR)/wwwcore.lib" 
DEF_FILE= \
	".\wwwcore.def"
LINK32_OBJS= \
	"$(INTDIR)/HTProt.obj" \
	"$(INTDIR)/HTInet.obj" \
	"$(INTDIR)/HTAlert.obj" \
	"$(INTDIR)/HTEvent.obj" \
	"$(INTDIR)/HTNet.obj" \
	"$(INTDIR)/HTChannl.obj" \
	"$(INTDIR)/HTHost.obj" \
	"$(INTDIR)/HTFormat.obj" \
	"$(INTDIR)/HTEscape.obj" \
	"$(INTDIR)/HTLink.obj" \
	"$(INTDIR)/HTStream.obj" \
	"$(INTDIR)/HTUTree.obj" \
	"$(INTDIR)/HTAnchor.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTReqMan.obj" \
	"$(INTDIR)/HTFWrite.obj" \
	"$(INTDIR)/HTError.obj" \
	"$(INTDIR)/HTWWWStr.obj" \
	"$(INTDIR)/HTMethod.obj" \
	"$(INTDIR)/HTUser.obj" \
	"$(INTDIR)/HTDNS.obj" \
	"$(INTDIR)/HTTrans.obj" \
	"$(INTDIR)/HTBind.obj" \
	"$(INTDIR)/HTParse.obj" \
	"$(INTDIR)/HTLib.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	"..\..\..\..\Msdev\Lib\Wsock32.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwcore.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwcore\Debug"
# PROP BASE Intermediate_Dir "wwwcore\Debug"
# PROP BASE Target_Dir "wwwcore"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwcore\Debug"
# PROP Intermediate_Dir "wwwcore\Debug"
# PROP Target_Dir "wwwcore"
OUTDIR=.\wwwcore\Debug
INTDIR=.\wwwcore\Debug

ALL : "$(OUTDIR)\wwwcore.dll"

CLEAN : 
	-@erase ".\wwwcore\Debug\vc40.pdb"
	-@erase ".\wwwcore\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwcore.dll"
	-@erase ".\wwwcore\Debug\HTInet.obj"
	-@erase ".\wwwcore\Debug\HTFormat.obj"
	-@erase ".\wwwcore\Debug\HTEscape.obj"
	-@erase ".\wwwcore\Debug\HTUTree.obj"
	-@erase ".\wwwcore\Debug\HTStream.obj"
	-@erase ".\wwwcore\Debug\HTParse.obj"
	-@erase ".\wwwcore\Debug\HTUser.obj"
	-@erase ".\wwwcore\Debug\HTAnchor.obj"
	-@erase ".\wwwcore\Debug\HTBind.obj"
	-@erase ".\wwwcore\Debug\HTError.obj"
	-@erase ".\wwwcore\Debug\HTWWWStr.obj"
	-@erase ".\wwwcore\Debug\HTProt.obj"
	-@erase ".\wwwcore\Debug\HTMethod.obj"
	-@erase ".\wwwcore\Debug\windll.obj"
	-@erase ".\wwwcore\Debug\HTDNS.obj"
	-@erase ".\wwwcore\Debug\HTTrans.obj"
	-@erase ".\wwwcore\Debug\HTLib.obj"
	-@erase ".\wwwcore\Debug\HTHost.obj"
	-@erase ".\wwwcore\Debug\HTAlert.obj"
	-@erase ".\wwwcore\Debug\HTEvent.obj"
	-@erase ".\wwwcore\Debug\HTNet.obj"
	-@erase ".\wwwcore\Debug\HTReqMan.obj"
	-@erase ".\wwwcore\Debug\HTFWrite.obj"
	-@erase ".\wwwcore\Debug\HTLink.obj"
	-@erase ".\wwwcore\Debug\HTChannl.obj"
	-@erase "..\..\..\Bin\Debug\wwwcore.ilk"
	-@erase ".\wwwcore\Debug\wwwcore.lib"
	-@erase ".\wwwcore\Debug\wwwcore.exp"
	-@erase ".\wwwcore\Debug\wwwcore.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwcore.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwcore\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwcore.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwcore.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwcore.pdb" /debug /machine:I386 /def:".\wwwcore.def"\
 /out:"../../../Bin/Debug/wwwcore.dll" /implib:"$(OUTDIR)/wwwcore.lib" 
DEF_FILE= \
	".\wwwcore.def"
LINK32_OBJS= \
	"$(INTDIR)/HTInet.obj" \
	"$(INTDIR)/HTFormat.obj" \
	"$(INTDIR)/HTEscape.obj" \
	"$(INTDIR)/HTUTree.obj" \
	"$(INTDIR)/HTStream.obj" \
	"$(INTDIR)/HTParse.obj" \
	"$(INTDIR)/HTUser.obj" \
	"$(INTDIR)/HTAnchor.obj" \
	"$(INTDIR)/HTBind.obj" \
	"$(INTDIR)/HTError.obj" \
	"$(INTDIR)/HTWWWStr.obj" \
	"$(INTDIR)/HTProt.obj" \
	"$(INTDIR)/HTMethod.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTDNS.obj" \
	"$(INTDIR)/HTTrans.obj" \
	"$(INTDIR)/HTLib.obj" \
	"$(INTDIR)/HTHost.obj" \
	"$(INTDIR)/HTAlert.obj" \
	"$(INTDIR)/HTEvent.obj" \
	"$(INTDIR)/HTNet.obj" \
	"$(INTDIR)/HTReqMan.obj" \
	"$(INTDIR)/HTFWrite.obj" \
	"$(INTDIR)/HTLink.obj" \
	"$(INTDIR)/HTChannl.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	"..\..\..\..\Msdev\Lib\Wsock32.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwcore.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwcache\Release"
# PROP BASE Intermediate_Dir "wwwcache\Release"
# PROP BASE Target_Dir "wwwcache"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwcache\Release"
# PROP Intermediate_Dir "wwwcache\Release"
# PROP Target_Dir "wwwcache"
OUTDIR=.\wwwcache\Release
INTDIR=.\wwwcache\Release

ALL : "$(OUTDIR)\wwwcache.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwcache.dll"
	-@erase ".\wwwcache\Release\windll.obj"
	-@erase ".\wwwcache\Release\HTCache.obj"
	-@erase ".\wwwcache\Release\wwwcache.lib"
	-@erase ".\wwwcache\Release\wwwcache.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwcache.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwcache\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwcache.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwcache.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwcache.pdb" /machine:I386 /def:".\wwwcache.def"\
 /out:"../../../Bin/Release/wwwcache.dll" /implib:"$(OUTDIR)/wwwcache.lib" 
DEF_FILE= \
	".\wwwcache.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTCache.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwcache.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwcache\Debug"
# PROP BASE Intermediate_Dir "wwwcache\Debug"
# PROP BASE Target_Dir "wwwcache"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwcache\Debug"
# PROP Intermediate_Dir "wwwcache\Debug"
# PROP Target_Dir "wwwcache"
OUTDIR=.\wwwcache\Debug
INTDIR=.\wwwcache\Debug

ALL : "$(OUTDIR)\wwwcache.dll"

CLEAN : 
	-@erase ".\wwwcache\Debug\vc40.pdb"
	-@erase ".\wwwcache\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwcache.dll"
	-@erase ".\wwwcache\Debug\HTCache.obj"
	-@erase ".\wwwcache\Debug\windll.obj"
	-@erase "..\..\..\Bin\Debug\wwwcache.ilk"
	-@erase ".\wwwcache\Debug\wwwcache.lib"
	-@erase ".\wwwcache\Debug\wwwcache.exp"
	-@erase ".\wwwcache\Debug\wwwcache.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwcache.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwcache\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwcache.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwcache.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwcache.pdb" /debug /machine:I386 /def:".\wwwcache.def"\
 /out:"../../../Bin/Debug/wwwcache.dll" /implib:"$(OUTDIR)/wwwcache.lib" 
DEF_FILE= \
	".\wwwcache.def"
LINK32_OBJS= \
	"$(INTDIR)/HTCache.obj" \
	"$(INTDIR)/windll.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwcache.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwmime\Release"
# PROP BASE Intermediate_Dir "wwwmime\Release"
# PROP BASE Target_Dir "wwwmime"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwmime\Release"
# PROP Intermediate_Dir "wwwmime\Release"
# PROP Target_Dir "wwwmime"
OUTDIR=.\wwwmime\Release
INTDIR=.\wwwmime\Release

ALL : "$(OUTDIR)\wwwmime.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwmime.dll"
	-@erase ".\wwwmime\Release\HTHeader.obj"
	-@erase ".\wwwmime\Release\HTMIMImp.obj"
	-@erase ".\wwwmime\Release\HTMIME.obj"
	-@erase ".\wwwmime\Release\windll.obj"
	-@erase ".\wwwmime\Release\HTBound.obj"
	-@erase ".\wwwmime\Release\HTMIMPrs.obj"
	-@erase ".\wwwmime\Release\HTMIMERq.obj"
	-@erase ".\wwwmime\Release\wwwmime.lib"
	-@erase ".\wwwmime\Release\wwwmime.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwmime.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwmime\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwmime.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwmime.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwmime.pdb" /machine:I386 /def:".\wwwmime.def"\
 /out:"../../../Bin/Release/wwwmime.dll" /implib:"$(OUTDIR)/wwwmime.lib" 
DEF_FILE= \
	".\wwwmime.def"
LINK32_OBJS= \
	"$(INTDIR)/HTHeader.obj" \
	"$(INTDIR)/HTMIMImp.obj" \
	"$(INTDIR)/HTMIME.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTBound.obj" \
	"$(INTDIR)/HTMIMPrs.obj" \
	"$(INTDIR)/HTMIMERq.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwmime.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwmime\Debug"
# PROP BASE Intermediate_Dir "wwwmime\Debug"
# PROP BASE Target_Dir "wwwmime"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwmime\Debug"
# PROP Intermediate_Dir "wwwmime\Debug"
# PROP Target_Dir "wwwmime"
OUTDIR=.\wwwmime\Debug
INTDIR=.\wwwmime\Debug

ALL : "$(OUTDIR)\wwwmime.dll"

CLEAN : 
	-@erase ".\wwwmime\Debug\vc40.pdb"
	-@erase ".\wwwmime\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwmime.dll"
	-@erase ".\wwwmime\Debug\HTBound.obj"
	-@erase ".\wwwmime\Debug\HTMIMERq.obj"
	-@erase ".\wwwmime\Debug\HTHeader.obj"
	-@erase ".\wwwmime\Debug\HTMIMImp.obj"
	-@erase ".\wwwmime\Debug\HTMIMPrs.obj"
	-@erase ".\wwwmime\Debug\HTMIME.obj"
	-@erase ".\wwwmime\Debug\windll.obj"
	-@erase "..\..\..\Bin\Debug\wwwmime.ilk"
	-@erase ".\wwwmime\Debug\wwwmime.lib"
	-@erase ".\wwwmime\Debug\wwwmime.exp"
	-@erase ".\wwwmime\Debug\wwwmime.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwmime.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwmime\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwmime.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwmime.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwmime.pdb" /debug /machine:I386 /def:".\wwwmime.def"\
 /out:"../../../Bin/Debug/wwwmime.dll" /implib:"$(OUTDIR)/wwwmime.lib" 
DEF_FILE= \
	".\wwwmime.def"
LINK32_OBJS= \
	"$(INTDIR)/HTBound.obj" \
	"$(INTDIR)/HTMIMERq.obj" \
	"$(INTDIR)/HTHeader.obj" \
	"$(INTDIR)/HTMIMImp.obj" \
	"$(INTDIR)/HTMIMPrs.obj" \
	"$(INTDIR)/HTMIME.obj" \
	"$(INTDIR)/windll.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwmime.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwhttp\Release"
# PROP BASE Intermediate_Dir "wwwhttp\Release"
# PROP BASE Target_Dir "wwwhttp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwhttp\Release"
# PROP Intermediate_Dir "wwwhttp\Release"
# PROP Target_Dir "wwwhttp"
OUTDIR=.\wwwhttp\Release
INTDIR=.\wwwhttp\Release

ALL : "wwwtrans - Win32 Release" "wwwstream - Win32 Release"\
 "$(OUTDIR)\wwwhttp.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwhttp.dll"
	-@erase ".\wwwhttp\Release\HTTP.obj"
	-@erase ".\wwwhttp\Release\HTAAUtil.obj"
	-@erase ".\wwwhttp\Release\HTTPGen.obj"
	-@erase ".\wwwhttp\Release\HTTPReq.obj"
	-@erase ".\wwwhttp\Release\windll.obj"
	-@erase ".\wwwhttp\Release\HTAABrow.obj"
	-@erase ".\wwwhttp\Release\HTTPServ.obj"
	-@erase ".\wwwhttp\Release\HTTChunk.obj"
	-@erase ".\wwwhttp\Release\HTTPRes.obj"
	-@erase ".\wwwhttp\Release\wwwhttp.lib"
	-@erase ".\wwwhttp\Release\wwwhttp.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwhttp.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwhttp\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwhttp.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwhttp.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwhttp.pdb" /machine:I386 /def:".\wwwhttp.def"\
 /out:"../../../Bin/Release/wwwhttp.dll" /implib:"$(OUTDIR)/wwwhttp.lib" 
DEF_FILE= \
	".\wwwhttp.def"
LINK32_OBJS= \
	"$(INTDIR)/HTTP.obj" \
	"$(INTDIR)/HTAAUtil.obj" \
	"$(INTDIR)/HTTPGen.obj" \
	"$(INTDIR)/HTTPReq.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTAABrow.obj" \
	"$(INTDIR)/HTTPServ.obj" \
	"$(INTDIR)/HTTChunk.obj" \
	"$(INTDIR)/HTTPRes.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwstream\Debug\wwwstream.lib" \
	".\wwwcache\Debug\wwwcache.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwmime\Debug\wwwmime.lib" \
	".\wwwtrans\Debug\wwwtrans.lib" \
	".\wwwstream\Release\wwwstream.lib" \
	".\wwwtrans\Release\wwwtrans.lib"

"$(OUTDIR)\wwwhttp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwhttp\Debug"
# PROP BASE Intermediate_Dir "wwwhttp\Debug"
# PROP BASE Target_Dir "wwwhttp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwhttp\Debug"
# PROP Intermediate_Dir "wwwhttp\Debug"
# PROP Target_Dir "wwwhttp"
OUTDIR=.\wwwhttp\Debug
INTDIR=.\wwwhttp\Debug

ALL : "wwwtrans - Win32 Debug" "wwwstream - Win32 Debug"\
 "$(OUTDIR)\wwwhttp.dll"

CLEAN : 
	-@erase ".\wwwhttp\Debug\vc40.pdb"
	-@erase ".\wwwhttp\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwhttp.dll"
	-@erase ".\wwwhttp\Debug\HTTPRes.obj"
	-@erase ".\wwwhttp\Debug\windll.obj"
	-@erase ".\wwwhttp\Debug\HTTPServ.obj"
	-@erase ".\wwwhttp\Debug\HTAAUtil.obj"
	-@erase ".\wwwhttp\Debug\HTTChunk.obj"
	-@erase ".\wwwhttp\Debug\HTTPGen.obj"
	-@erase ".\wwwhttp\Debug\HTTPReq.obj"
	-@erase ".\wwwhttp\Debug\HTAABrow.obj"
	-@erase ".\wwwhttp\Debug\HTTP.obj"
	-@erase "..\..\..\Bin\Debug\wwwhttp.ilk"
	-@erase ".\wwwhttp\Debug\wwwhttp.lib"
	-@erase ".\wwwhttp\Debug\wwwhttp.exp"
	-@erase ".\wwwhttp\Debug\wwwhttp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwhttp.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwhttp\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwhttp.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwhttp.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwhttp.pdb" /debug /machine:I386 /def:".\wwwhttp.def"\
 /out:"../../../Bin/Debug/wwwhttp.dll" /implib:"$(OUTDIR)/wwwhttp.lib" 
DEF_FILE= \
	".\wwwhttp.def"
LINK32_OBJS= \
	"$(INTDIR)/HTTPRes.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTTPServ.obj" \
	"$(INTDIR)/HTAAUtil.obj" \
	"$(INTDIR)/HTTChunk.obj" \
	"$(INTDIR)/HTTPGen.obj" \
	"$(INTDIR)/HTTPReq.obj" \
	"$(INTDIR)/HTAABrow.obj" \
	"$(INTDIR)/HTTP.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwstream\Debug\wwwstream.lib" \
	".\wwwcache\Debug\wwwcache.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwmime\Debug\wwwmime.lib" \
	".\wwwtrans\Debug\wwwtrans.lib"

"$(OUTDIR)\wwwhttp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwrules - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwrules\Release"
# PROP BASE Intermediate_Dir "wwwrules\Release"
# PROP BASE Target_Dir "wwwrules"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwrules\Release"
# PROP Intermediate_Dir "wwwrules\Release"
# PROP Target_Dir "wwwrules"
OUTDIR=.\wwwrules\Release
INTDIR=.\wwwrules\Release

ALL : "$(OUTDIR)\wwwrules.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwrules.dll"
	-@erase ".\wwwrules\Release\windll.obj"
	-@erase ".\wwwrules\Release\HTRules.obj"
	-@erase ".\wwwrules\Release\HTProxy.obj"
	-@erase ".\wwwrules\Release\wwwrules.lib"
	-@erase ".\wwwrules\Release\wwwrules.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwrules.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwrules\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwrules.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwrules.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwrules.pdb" /machine:I386 /def:".\wwwrules.def"\
 /out:"../../../Bin/Release/wwwrules.dll" /implib:"$(OUTDIR)/wwwrules.lib" 
DEF_FILE= \
	".\wwwrules.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTRules.obj" \
	"$(INTDIR)/HTProxy.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwrules.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwrules - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwrules\Debug"
# PROP BASE Intermediate_Dir "wwwrules\Debug"
# PROP BASE Target_Dir "wwwrules"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwrules\Debug"
# PROP Intermediate_Dir "wwwrules\Debug"
# PROP Target_Dir "wwwrules"
OUTDIR=.\wwwrules\Debug
INTDIR=.\wwwrules\Debug

ALL : "$(OUTDIR)\wwwrules.dll"

CLEAN : 
	-@erase ".\wwwrules\Debug\vc40.pdb"
	-@erase ".\wwwrules\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwrules.dll"
	-@erase ".\wwwrules\Debug\HTRules.obj"
	-@erase ".\wwwrules\Debug\windll.obj"
	-@erase ".\wwwrules\Debug\HTProxy.obj"
	-@erase "..\..\..\Bin\Debug\wwwrules.ilk"
	-@erase ".\wwwrules\Debug\wwwrules.lib"
	-@erase ".\wwwrules\Debug\wwwrules.exp"
	-@erase ".\wwwrules\Debug\wwwrules.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwrules.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwrules\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwrules.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwrules.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwrules.pdb" /debug /machine:I386 /def:".\wwwrules.def"\
 /out:"../../../Bin/Debug/wwwrules.dll" /implib:"$(OUTDIR)/wwwrules.lib" 
DEF_FILE= \
	".\wwwrules.def"
LINK32_OBJS= \
	"$(INTDIR)/HTRules.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTProxy.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwrules.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwnews\Release"
# PROP BASE Intermediate_Dir "wwwnews\Release"
# PROP BASE Target_Dir "wwwnews"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwnews\Release"
# PROP Intermediate_Dir "wwwnews\Release"
# PROP Target_Dir "wwwnews"
OUTDIR=.\wwwnews\Release
INTDIR=.\wwwnews\Release

ALL : "wwwhtml - Win32 Release" "$(OUTDIR)\wwwnews.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwnews.dll"
	-@erase ".\wwwnews\Release\HTNewsLs.obj"
	-@erase ".\wwwnews\Release\HTNDir.obj"
	-@erase ".\wwwnews\Release\HTNewsRq.obj"
	-@erase ".\wwwnews\Release\HTNews.obj"
	-@erase ".\wwwnews\Release\windll.obj"
	-@erase ".\wwwnews\Release\wwwnews.lib"
	-@erase ".\wwwnews\Release\wwwnews.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwnews.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwnews\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwnews.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwnews.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwnews.pdb" /machine:I386 /def:".\wwwnews.def"\
 /out:"../../../Bin/Release/wwwnews.dll" /implib:"$(OUTDIR)/wwwnews.lib" 
DEF_FILE= \
	".\wwwnews.def"
LINK32_OBJS= \
	"$(INTDIR)/HTNewsLs.obj" \
	"$(INTDIR)/HTNDir.obj" \
	"$(INTDIR)/HTNewsRq.obj" \
	"$(INTDIR)/HTNews.obj" \
	"$(INTDIR)/windll.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwstream\Debug\wwwstream.lib" \
	".\wwwhtml\Debug\wwwhtml.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwmime\Debug\wwwmime.lib" \
	".\wwwtrans\Debug\wwwtrans.lib" \
	".\wwwhtml\Release\wwwhtml.lib"

"$(OUTDIR)\wwwnews.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwnews\Debug"
# PROP BASE Intermediate_Dir "wwwnews\Debug"
# PROP BASE Target_Dir "wwwnews"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwnews\Debug"
# PROP Intermediate_Dir "wwwnews\Debug"
# PROP Target_Dir "wwwnews"
OUTDIR=.\wwwnews\Debug
INTDIR=.\wwwnews\Debug

ALL : "wwwhtml - Win32 Debug" "$(OUTDIR)\wwwnews.dll"

CLEAN : 
	-@erase ".\wwwnews\Debug\vc40.pdb"
	-@erase ".\wwwnews\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwnews.dll"
	-@erase ".\wwwnews\Debug\windll.obj"
	-@erase ".\wwwnews\Debug\HTNews.obj"
	-@erase ".\wwwnews\Debug\HTNewsRq.obj"
	-@erase ".\wwwnews\Debug\HTNewsLs.obj"
	-@erase ".\wwwnews\Debug\HTNDir.obj"
	-@erase "..\..\..\Bin\Debug\wwwnews.ilk"
	-@erase ".\wwwnews\Debug\wwwnews.lib"
	-@erase ".\wwwnews\Debug\wwwnews.exp"
	-@erase ".\wwwnews\Debug\wwwnews.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwnews.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwnews\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwnews.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwnews.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwnews.pdb" /debug /machine:I386 /def:".\wwwnews.def"\
 /out:"../../../Bin/Debug/wwwnews.dll" /implib:"$(OUTDIR)/wwwnews.lib" 
DEF_FILE= \
	".\wwwnews.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTNews.obj" \
	"$(INTDIR)/HTNewsRq.obj" \
	"$(INTDIR)/HTNewsLs.obj" \
	"$(INTDIR)/HTNDir.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwstream\Debug\wwwstream.lib" \
	".\wwwhtml\Debug\wwwhtml.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwmime\Debug\wwwmime.lib" \
	".\wwwtrans\Debug\wwwtrans.lib"

"$(OUTDIR)\wwwnews.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwgophe\Release"
# PROP BASE Intermediate_Dir "wwwgophe\Release"
# PROP BASE Target_Dir "wwwgophe"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwgophe\Release"
# PROP Intermediate_Dir "wwwgophe\Release"
# PROP Target_Dir "wwwgophe"
OUTDIR=.\wwwgophe\Release
INTDIR=.\wwwgophe\Release

ALL : "wwwdir - Win32 Release" "$(OUTDIR)\wwwgophe.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwgophe.dll"
	-@erase ".\wwwgophe\Release\windll.obj"
	-@erase ".\wwwgophe\Release\HTGopher.obj"
	-@erase ".\wwwgophe\Release\wwwgophe.lib"
	-@erase ".\wwwgophe\Release\wwwgophe.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwgophe.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwgophe\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwgophe.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwgophe.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwgophe.pdb" /machine:I386 /def:".\wwwgophe.def"\
 /out:"../../../Bin/Release/wwwgophe.dll" /implib:"$(OUTDIR)/wwwgophe.lib" 
DEF_FILE= \
	".\wwwgophe.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTGopher.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwhtml\Debug\wwwhtml.lib" \
	".\wwwdir\Debug\wwwdir.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwtrans\Debug\wwwtrans.lib" \
	".\wwwdir\Release\wwwdir.lib"

"$(OUTDIR)\wwwgophe.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwgophe\Debug"
# PROP BASE Intermediate_Dir "wwwgophe\Debug"
# PROP BASE Target_Dir "wwwgophe"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwgophe\Debug"
# PROP Intermediate_Dir "wwwgophe\Debug"
# PROP Target_Dir "wwwgophe"
OUTDIR=.\wwwgophe\Debug
INTDIR=.\wwwgophe\Debug

ALL : "wwwdir - Win32 Debug" "$(OUTDIR)\wwwgophe.dll"

CLEAN : 
	-@erase ".\wwwgophe\Debug\vc40.pdb"
	-@erase ".\wwwgophe\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwgophe.dll"
	-@erase ".\wwwgophe\Debug\windll.obj"
	-@erase ".\wwwgophe\Debug\HTGopher.obj"
	-@erase "..\..\..\Bin\Debug\wwwgophe.ilk"
	-@erase ".\wwwgophe\Debug\wwwgophe.lib"
	-@erase ".\wwwgophe\Debug\wwwgophe.exp"
	-@erase ".\wwwgophe\Debug\wwwgophe.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwgophe.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwgophe\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwgophe.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwgophe.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwgophe.pdb" /debug /machine:I386 /def:".\wwwgophe.def"\
 /out:"../../../Bin/Debug/wwwgophe.dll" /implib:"$(OUTDIR)/wwwgophe.lib" 
DEF_FILE= \
	".\wwwgophe.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTGopher.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwhtml\Debug\wwwhtml.lib" \
	".\wwwdir\Debug\wwwdir.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwtrans\Debug\wwwtrans.lib"

"$(OUTDIR)\wwwgophe.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwftp\Release"
# PROP BASE Intermediate_Dir "wwwftp\Release"
# PROP BASE Target_Dir "wwwftp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwftp\Release"
# PROP Intermediate_Dir "wwwftp\Release"
# PROP Target_Dir "wwwftp"
OUTDIR=.\wwwftp\Release
INTDIR=.\wwwftp\Release

ALL : "$(OUTDIR)\wwwftp.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwftp.dll"
	-@erase ".\wwwftp\Release\HTFTP.obj"
	-@erase ".\wwwftp\Release\windll.obj"
	-@erase ".\wwwftp\Release\HTFTPDir.obj"
	-@erase ".\wwwftp\Release\wwwftp.lib"
	-@erase ".\wwwftp\Release\wwwftp.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwftp.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwftp\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwftp.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwftp.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwftp.pdb" /machine:I386 /def:".\wwwftp.def"\
 /out:"../../../Bin/Release/wwwftp.dll" /implib:"$(OUTDIR)/wwwftp.lib" 
DEF_FILE= \
	".\wwwftp.def"
LINK32_OBJS= \
	"$(INTDIR)/HTFTP.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTFTPDir.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwdir\Debug\wwwdir.lib" \
	"..\..\..\..\Msdev\Lib\Wsock32.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwtrans\Debug\wwwtrans.lib"

"$(OUTDIR)\wwwftp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwftp\Debug"
# PROP BASE Intermediate_Dir "wwwftp\Debug"
# PROP BASE Target_Dir "wwwftp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwftp\Debug"
# PROP Intermediate_Dir "wwwftp\Debug"
# PROP Target_Dir "wwwftp"
OUTDIR=.\wwwftp\Debug
INTDIR=.\wwwftp\Debug

ALL : "$(OUTDIR)\wwwftp.dll"

CLEAN : 
	-@erase ".\wwwftp\Debug\vc40.pdb"
	-@erase ".\wwwftp\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwftp.dll"
	-@erase ".\wwwftp\Debug\windll.obj"
	-@erase ".\wwwftp\Debug\HTFTPDir.obj"
	-@erase ".\wwwftp\Debug\HTFTP.obj"
	-@erase "..\..\..\Bin\Debug\wwwftp.ilk"
	-@erase ".\wwwftp\Debug\wwwftp.lib"
	-@erase ".\wwwftp\Debug\wwwftp.exp"
	-@erase ".\wwwftp\Debug\wwwftp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwftp.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwftp\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwftp.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwftp.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwftp.pdb" /debug /machine:I386 /def:".\wwwftp.def"\
 /out:"../../../Bin/Debug/wwwftp.dll" /implib:"$(OUTDIR)/wwwftp.lib" 
DEF_FILE= \
	".\wwwftp.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTFTPDir.obj" \
	"$(INTDIR)/HTFTP.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwdir\Debug\wwwdir.lib" \
	"..\..\..\..\Msdev\Lib\Wsock32.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwtrans\Debug\wwwtrans.lib"

"$(OUTDIR)\wwwftp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwdir\Release"
# PROP BASE Intermediate_Dir "wwwdir\Release"
# PROP BASE Target_Dir "wwwdir"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwdir\Release"
# PROP Intermediate_Dir "wwwdir\Release"
# PROP Target_Dir "wwwdir"
OUTDIR=.\wwwdir\Release
INTDIR=.\wwwdir\Release

ALL : "$(OUTDIR)\wwwdir.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwdir.dll"
	-@erase ".\wwwdir\Release\HTDescpt.obj"
	-@erase ".\wwwdir\Release\HTIcons.obj"
	-@erase ".\wwwdir\Release\windll.obj"
	-@erase ".\wwwdir\Release\HTDir.obj"
	-@erase ".\wwwdir\Release\wwwdir.lib"
	-@erase ".\wwwdir\Release\wwwdir.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwdir.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwdir\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwdir.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwdir.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwdir.pdb" /machine:I386 /def:".\wwwdir.def"\
 /out:"../../../Bin/Release/wwwdir.dll" /implib:"$(OUTDIR)/wwwdir.lib" 
DEF_FILE= \
	".\wwwdir.def"
LINK32_OBJS= \
	"$(INTDIR)/HTDescpt.obj" \
	"$(INTDIR)/HTIcons.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTDir.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwhtml\Debug\wwwhtml.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwdir.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwdir\Debug"
# PROP BASE Intermediate_Dir "wwwdir\Debug"
# PROP BASE Target_Dir "wwwdir"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwdir\Debug"
# PROP Intermediate_Dir "wwwdir\Debug"
# PROP Target_Dir "wwwdir"
OUTDIR=.\wwwdir\Debug
INTDIR=.\wwwdir\Debug

ALL : "$(OUTDIR)\wwwdir.dll"

CLEAN : 
	-@erase ".\wwwdir\Debug\vc40.pdb"
	-@erase ".\wwwdir\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwdir.dll"
	-@erase ".\wwwdir\Debug\windll.obj"
	-@erase ".\wwwdir\Debug\HTDir.obj"
	-@erase ".\wwwdir\Debug\HTDescpt.obj"
	-@erase ".\wwwdir\Debug\HTIcons.obj"
	-@erase "..\..\..\Bin\Debug\wwwdir.ilk"
	-@erase ".\wwwdir\Debug\wwwdir.lib"
	-@erase ".\wwwdir\Debug\wwwdir.exp"
	-@erase ".\wwwdir\Debug\wwwdir.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwdir.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwdir\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwdir.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwdir.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwdir.pdb" /debug /machine:I386 /def:".\wwwdir.def"\
 /out:"../../../Bin/Debug/wwwdir.dll" /implib:"$(OUTDIR)/wwwdir.lib" 
DEF_FILE= \
	".\wwwdir.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTDir.obj" \
	"$(INTDIR)/HTDescpt.obj" \
	"$(INTDIR)/HTIcons.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwhtml\Debug\wwwhtml.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwdir.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwfile\Release"
# PROP BASE Intermediate_Dir "wwwfile\Release"
# PROP BASE Target_Dir "wwwfile"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwfile\Release"
# PROP Intermediate_Dir "wwwfile\Release"
# PROP Target_Dir "wwwfile"
OUTDIR=.\wwwfile\Release
INTDIR=.\wwwfile\Release

ALL : "$(OUTDIR)\wwwfile.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwfile.dll"
	-@erase ".\wwwfile\Release\HTFile.obj"
	-@erase ".\wwwfile\Release\windll.obj"
	-@erase ".\wwwfile\Release\HTMulti.obj"
	-@erase ".\wwwfile\Release\wwwfile.lib"
	-@erase ".\wwwfile\Release\wwwfile.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwfile.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwfile\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwfile.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwfile.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwfile.pdb" /machine:I386 /def:".\wwwfile.def"\
 /out:"../../../Bin/Release/wwwfile.dll" /implib:"$(OUTDIR)/wwwfile.lib" 
DEF_FILE= \
	".\wwwfile.def"
LINK32_OBJS= \
	"$(INTDIR)/HTFile.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTMulti.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwdir\Debug\wwwdir.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwtrans\Debug\wwwtrans.lib"

"$(OUTDIR)\wwwfile.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwfile\Debug"
# PROP BASE Intermediate_Dir "wwwfile\Debug"
# PROP BASE Target_Dir "wwwfile"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwfile\Debug"
# PROP Intermediate_Dir "wwwfile\Debug"
# PROP Target_Dir "wwwfile"
OUTDIR=.\wwwfile\Debug
INTDIR=.\wwwfile\Debug

ALL : "$(OUTDIR)\wwwfile.dll"

CLEAN : 
	-@erase ".\wwwfile\Debug\vc40.pdb"
	-@erase ".\wwwfile\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwfile.dll"
	-@erase ".\wwwfile\Debug\HTMulti.obj"
	-@erase ".\wwwfile\Debug\HTFile.obj"
	-@erase ".\wwwfile\Debug\windll.obj"
	-@erase "..\..\..\Bin\Debug\wwwfile.ilk"
	-@erase ".\wwwfile\Debug\wwwfile.lib"
	-@erase ".\wwwfile\Debug\wwwfile.exp"
	-@erase ".\wwwfile\Debug\wwwfile.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwfile.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwfile\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwfile.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwfile.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwfile.pdb" /debug /machine:I386 /def:".\wwwfile.def"\
 /out:"../../../Bin/Debug/wwwfile.dll" /implib:"$(OUTDIR)/wwwfile.lib" 
DEF_FILE= \
	".\wwwfile.def"
LINK32_OBJS= \
	"$(INTDIR)/HTMulti.obj" \
	"$(INTDIR)/HTFile.obj" \
	"$(INTDIR)/windll.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwdir\Debug\wwwdir.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwtrans\Debug\wwwtrans.lib"

"$(OUTDIR)\wwwfile.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwtelnt\Release"
# PROP BASE Intermediate_Dir "wwwtelnt\Release"
# PROP BASE Target_Dir "wwwtelnt"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwtelnt\Release"
# PROP Intermediate_Dir "wwwtelnt\Release"
# PROP Target_Dir "wwwtelnt"
OUTDIR=.\wwwtelnt\Release
INTDIR=.\wwwtelnt\Release

ALL : "$(OUTDIR)\wwwtelnt.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwtelnt.dll"
	-@erase ".\wwwtelnt\Release\windll.obj"
	-@erase ".\wwwtelnt\Release\HTTelnet.obj"
	-@erase ".\wwwtelnt\Release\wwwtelnt.lib"
	-@erase ".\wwwtelnt\Release\wwwtelnt.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwtelnt.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwtelnt\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwtelnt.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwtelnt.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwtelnt.pdb" /machine:I386 /def:".\wwwtelnt.def"\
 /out:"../../../Bin/Release/wwwtelnt.dll" /implib:"$(OUTDIR)/wwwtelnt.lib" 
DEF_FILE= \
	".\wwwtelnt.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTTelnet.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwdir\Debug\wwwdir.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwtelnt.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwtelnt\Debug"
# PROP BASE Intermediate_Dir "wwwtelnt\Debug"
# PROP BASE Target_Dir "wwwtelnt"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwtelnt\Debug"
# PROP Intermediate_Dir "wwwtelnt\Debug"
# PROP Target_Dir "wwwtelnt"
OUTDIR=.\wwwtelnt\Debug
INTDIR=.\wwwtelnt\Debug

ALL : "$(OUTDIR)\wwwtelnt.dll"

CLEAN : 
	-@erase ".\wwwtelnt\Debug\vc40.pdb"
	-@erase ".\wwwtelnt\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwtelnt.dll"
	-@erase ".\wwwtelnt\Debug\HTTelnet.obj"
	-@erase ".\wwwtelnt\Debug\windll.obj"
	-@erase "..\..\..\Bin\Debug\wwwtelnt.ilk"
	-@erase ".\wwwtelnt\Debug\wwwtelnt.lib"
	-@erase ".\wwwtelnt\Debug\wwwtelnt.exp"
	-@erase ".\wwwtelnt\Debug\wwwtelnt.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwtelnt.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwtelnt\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwtelnt.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwtelnt.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwtelnt.pdb" /debug /machine:I386 /def:".\wwwtelnt.def"\
 /out:"../../../Bin/Debug/wwwtelnt.dll" /implib:"$(OUTDIR)/wwwtelnt.lib" 
DEF_FILE= \
	".\wwwtelnt.def"
LINK32_OBJS= \
	"$(INTDIR)/HTTelnet.obj" \
	"$(INTDIR)/windll.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwdir\Debug\wwwdir.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwtelnt.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwhtml - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwhtml\Release"
# PROP BASE Intermediate_Dir "wwwhtml\Release"
# PROP BASE Target_Dir "wwwhtml"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwhtml\Release"
# PROP Intermediate_Dir "wwwhtml\Release"
# PROP Target_Dir "wwwhtml"
OUTDIR=.\wwwhtml\Release
INTDIR=.\wwwhtml\Release

ALL : "$(OUTDIR)\wwwhtml.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwhtml.dll"
	-@erase ".\wwwhtml\Release\HTTeXGen.obj"
	-@erase ".\wwwhtml\Release\windll.obj"
	-@erase ".\wwwhtml\Release\HTMLGen.obj"
	-@erase ".\wwwhtml\Release\HTMLPDTD.obj"
	-@erase ".\wwwhtml\Release\SGML.obj"
	-@erase ".\wwwhtml\Release\wwwhtml.lib"
	-@erase ".\wwwhtml\Release\wwwhtml.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwhtml.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwhtml\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwhtml.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwhtml.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwhtml.pdb" /machine:I386 /def:".\wwwhtml.def"\
 /out:"../../../Bin/Release/wwwhtml.dll" /implib:"$(OUTDIR)/wwwhtml.lib" 
DEF_FILE= \
	".\wwwhtml.def"
LINK32_OBJS= \
	"$(INTDIR)/HTTeXGen.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTMLGen.obj" \
	"$(INTDIR)/HTMLPDTD.obj" \
	"$(INTDIR)/SGML.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwhtml.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwhtml - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwhtml\Debug"
# PROP BASE Intermediate_Dir "wwwhtml\Debug"
# PROP BASE Target_Dir "wwwhtml"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwhtml\Debug"
# PROP Intermediate_Dir "wwwhtml\Debug"
# PROP Target_Dir "wwwhtml"
OUTDIR=.\wwwhtml\Debug
INTDIR=.\wwwhtml\Debug

ALL : "$(OUTDIR)\wwwhtml.dll"

CLEAN : 
	-@erase ".\wwwhtml\Debug\vc40.pdb"
	-@erase ".\wwwhtml\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwhtml.dll"
	-@erase ".\wwwhtml\Debug\HTTeXGen.obj"
	-@erase ".\wwwhtml\Debug\windll.obj"
	-@erase ".\wwwhtml\Debug\HTMLPDTD.obj"
	-@erase ".\wwwhtml\Debug\SGML.obj"
	-@erase ".\wwwhtml\Debug\HTMLGen.obj"
	-@erase "..\..\..\Bin\Debug\wwwhtml.ilk"
	-@erase ".\wwwhtml\Debug\wwwhtml.lib"
	-@erase ".\wwwhtml\Debug\wwwhtml.exp"
	-@erase ".\wwwhtml\Debug\wwwhtml.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwhtml.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwhtml\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwhtml.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwhtml.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwhtml.pdb" /debug /machine:I386 /def:".\wwwhtml.def"\
 /out:"../../../Bin/Debug/wwwhtml.dll" /implib:"$(OUTDIR)/wwwhtml.lib" 
DEF_FILE= \
	".\wwwhtml.def"
LINK32_OBJS= \
	"$(INTDIR)/HTTeXGen.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTMLPDTD.obj" \
	"$(INTDIR)/SGML.obj" \
	"$(INTDIR)/HTMLGen.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwhtml.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwapp\Release"
# PROP BASE Intermediate_Dir "wwwapp\Release"
# PROP BASE Target_Dir "wwwapp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwapp\Release"
# PROP Intermediate_Dir "wwwapp\Release"
# PROP Target_Dir "wwwapp"
OUTDIR=.\wwwapp\Release
INTDIR=.\wwwapp\Release

ALL : "$(OUTDIR)\wwwapp.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwapp.dll"
	-@erase ".\wwwapp\Release\HTHist.obj"
	-@erase ".\wwwapp\Release\HTLog.obj"
	-@erase ".\wwwapp\Release\HTAccess.obj"
	-@erase ".\wwwapp\Release\HTDialog.obj"
	-@erase ".\wwwapp\Release\HTHome.obj"
	-@erase ".\wwwapp\Release\HTFilter.obj"
	-@erase ".\wwwapp\Release\HTEvntrg.obj"
	-@erase ".\wwwapp\Release\windll.obj"
	-@erase ".\wwwapp\Release\wwwapp.lib"
	-@erase ".\wwwapp\Release\wwwapp.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwapp.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwapp\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwapp.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwapp.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwapp.pdb" /machine:I386 /def:".\wwwapp.def"\
 /out:"../../../Bin/Release/wwwapp.dll" /implib:"$(OUTDIR)/wwwapp.lib" 
DEF_FILE= \
	".\wwwapp.def"
LINK32_OBJS= \
	"$(INTDIR)/HTHist.obj" \
	"$(INTDIR)/HTLog.obj" \
	"$(INTDIR)/HTAccess.obj" \
	"$(INTDIR)/HTDialog.obj" \
	"$(INTDIR)/HTHome.obj" \
	"$(INTDIR)/HTFilter.obj" \
	"$(INTDIR)/HTEvntrg.obj" \
	"$(INTDIR)/windll.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwstream\Debug\wwwstream.lib" \
	".\wwwcache\Debug\wwwcache.lib" \
	".\wwwrules\Debug\wwwrules.lib" \
	"..\..\..\..\Msdev\Lib\Wsock32.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwhttp\Debug\wwwhttp.lib"

"$(OUTDIR)\wwwapp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwapp\Debug"
# PROP BASE Intermediate_Dir "wwwapp\Debug"
# PROP BASE Target_Dir "wwwapp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwapp\Debug"
# PROP Intermediate_Dir "wwwapp\Debug"
# PROP Target_Dir "wwwapp"
OUTDIR=.\wwwapp\Debug
INTDIR=.\wwwapp\Debug

ALL : "$(OUTDIR)\wwwapp.dll"

CLEAN : 
	-@erase ".\wwwapp\Debug\vc40.pdb"
	-@erase ".\wwwapp\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwapp.dll"
	-@erase ".\wwwapp\Debug\HTAccess.obj"
	-@erase ".\wwwapp\Debug\HTHome.obj"
	-@erase ".\wwwapp\Debug\HTHist.obj"
	-@erase ".\wwwapp\Debug\HTLog.obj"
	-@erase ".\wwwapp\Debug\HTFilter.obj"
	-@erase ".\wwwapp\Debug\HTDialog.obj"
	-@erase ".\wwwapp\Debug\windll.obj"
	-@erase ".\wwwapp\Debug\HTEvntrg.obj"
	-@erase "..\..\..\Bin\Debug\wwwapp.ilk"
	-@erase ".\wwwapp\Debug\wwwapp.lib"
	-@erase ".\wwwapp\Debug\wwwapp.exp"
	-@erase ".\wwwapp\Debug\wwwapp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwapp.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwapp\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwapp.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwapp.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwapp.pdb" /debug /machine:I386 /def:".\wwwapp.def"\
 /out:"../../../Bin/Debug/wwwapp.dll" /implib:"$(OUTDIR)/wwwapp.lib" 
DEF_FILE= \
	".\wwwapp.def"
LINK32_OBJS= \
	"$(INTDIR)/HTAccess.obj" \
	"$(INTDIR)/HTHome.obj" \
	"$(INTDIR)/HTHist.obj" \
	"$(INTDIR)/HTLog.obj" \
	"$(INTDIR)/HTFilter.obj" \
	"$(INTDIR)/HTDialog.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTEvntrg.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwstream\Debug\wwwstream.lib" \
	".\wwwcache\Debug\wwwcache.lib" \
	".\wwwrules\Debug\wwwrules.lib" \
	"..\..\..\..\Msdev\Lib\Wsock32.lib" \
	".\wwwutils\Debug\wwwutils.lib" \
	".\wwwhttp\Debug\wwwhttp.lib"

"$(OUTDIR)\wwwapp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwwais\Release"
# PROP BASE Intermediate_Dir "wwwwais\Release"
# PROP BASE Target_Dir "wwwwais"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwwais\Release"
# PROP Intermediate_Dir "wwwwais\Release"
# PROP Target_Dir "wwwwais"
OUTDIR=.\wwwwais\Release
INTDIR=.\wwwwais\Release

ALL : "$(OUTDIR)\wwwwais.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwwais.dll"
	-@erase ".\wwwwais\Release\windll.obj"
	-@erase ".\wwwwais\Release\HTWSRC.obj"
	-@erase ".\wwwwais\Release\wwwwais.lib"
	-@erase ".\wwwwais\Release\wwwwais.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwwais.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wwwwais\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwwais.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwwais.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwwais.pdb" /machine:I386 /def:".\wwwwais.def"\
 /out:"../../../Bin/Release/wwwwais.dll" /implib:"$(OUTDIR)/wwwwais.lib" 
DEF_FILE= \
	".\wwwwais.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTWSRC.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwrules\Debug\wwwrules.lib" \
	".\wwwhtml\Debug\wwwhtml.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwwais.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwwais\Debug"
# PROP BASE Intermediate_Dir "wwwwais\Debug"
# PROP BASE Target_Dir "wwwwais"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwwais\Debug"
# PROP Intermediate_Dir "wwwwais\Debug"
# PROP Target_Dir "wwwwais"
OUTDIR=.\wwwwais\Debug
INTDIR=.\wwwwais\Debug

ALL : "$(OUTDIR)\wwwwais.dll"

CLEAN : 
	-@erase ".\wwwwais\Debug\vc40.pdb"
	-@erase ".\wwwwais\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwwais.dll"
	-@erase ".\wwwwais\Debug\windll.obj"
	-@erase ".\wwwwais\Debug\HTWSRC.obj"
	-@erase "..\..\..\Bin\Debug\wwwwais.ilk"
	-@erase ".\wwwwais\Debug\wwwwais.lib"
	-@erase ".\wwwwais\Debug\wwwwais.exp"
	-@erase ".\wwwwais\Debug\wwwwais.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwwais.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwwais\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwwais.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwwais.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwwais.pdb" /debug /machine:I386 /def:".\wwwwais.def"\
 /out:"../../../Bin/Debug/wwwwais.dll" /implib:"$(OUTDIR)/wwwwais.lib" 
DEF_FILE= \
	".\wwwwais.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTWSRC.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwrules\Debug\wwwrules.lib" \
	".\wwwhtml\Debug\wwwhtml.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwwais.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwtrans\Release"
# PROP BASE Intermediate_Dir "wwwtrans\Release"
# PROP BASE Target_Dir "wwwtrans"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwtrans\Release"
# PROP Intermediate_Dir "wwwtrans\Release"
# PROP Target_Dir "wwwtrans"
OUTDIR=.\wwwtrans\Release
INTDIR=.\wwwtrans\Release

ALL : "$(OUTDIR)\wwwtrans.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwtrans.dll"
	-@erase ".\wwwtrans\Release\windll.obj"
	-@erase ".\wwwtrans\Release\HTANSI.obj"
	-@erase ".\wwwtrans\Release\HTWriter.obj"
	-@erase ".\wwwtrans\Release\HTSocket.obj"
	-@erase ".\wwwtrans\Release\HTLocal.obj"
	-@erase ".\wwwtrans\Release\HTReader.obj"
	-@erase ".\wwwtrans\Release\HTMux.obj"
	-@erase ".\wwwtrans\Release\HTTCP.obj"
	-@erase ".\wwwtrans\Release\HTBufWrt.obj"
	-@erase ".\wwwtrans\Release\wwwtrans.lib"
	-@erase ".\wwwtrans\Release\wwwtrans.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "_MBCS" /Fp"$(INTDIR)/wwwtrans.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwtrans\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwtrans.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwtrans.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwtrans.pdb" /machine:I386 /def:".\wwwtrans.def"\
 /out:"../../../Bin/Release/wwwtrans.dll" /implib:"$(OUTDIR)/wwwtrans.lib" 
DEF_FILE= \
	".\wwwtrans.def"
LINK32_OBJS= \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTANSI.obj" \
	"$(INTDIR)/HTWriter.obj" \
	"$(INTDIR)/HTSocket.obj" \
	"$(INTDIR)/HTLocal.obj" \
	"$(INTDIR)/HTReader.obj" \
	"$(INTDIR)/HTMux.obj" \
	"$(INTDIR)/HTTCP.obj" \
	"$(INTDIR)/HTBufWrt.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	"..\..\..\..\Msdev\Lib\Wsock32.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwtrans.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwtrans\Debug"
# PROP BASE Intermediate_Dir "wwwtrans\Debug"
# PROP BASE Target_Dir "wwwtrans"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwtrans\Debug"
# PROP Intermediate_Dir "wwwtrans\Debug"
# PROP Target_Dir "wwwtrans"
OUTDIR=.\wwwtrans\Debug
INTDIR=.\wwwtrans\Debug

ALL : "$(OUTDIR)\wwwtrans.dll"

CLEAN : 
	-@erase ".\wwwtrans\Debug\vc40.pdb"
	-@erase ".\wwwtrans\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwtrans.dll"
	-@erase ".\wwwtrans\Debug\HTMux.obj"
	-@erase ".\wwwtrans\Debug\HTTCP.obj"
	-@erase ".\wwwtrans\Debug\HTSocket.obj"
	-@erase ".\wwwtrans\Debug\HTLocal.obj"
	-@erase ".\wwwtrans\Debug\HTReader.obj"
	-@erase ".\wwwtrans\Debug\HTBufWrt.obj"
	-@erase ".\wwwtrans\Debug\HTWriter.obj"
	-@erase ".\wwwtrans\Debug\windll.obj"
	-@erase ".\wwwtrans\Debug\HTANSI.obj"
	-@erase "..\..\..\Bin\Debug\wwwtrans.ilk"
	-@erase ".\wwwtrans\Debug\wwwtrans.lib"
	-@erase ".\wwwtrans\Debug\wwwtrans.exp"
	-@erase ".\wwwtrans\Debug\wwwtrans.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwtrans.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwtrans\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwtrans.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwtrans.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwtrans.pdb" /debug /machine:I386 /def:".\wwwtrans.def"\
 /out:"../../../Bin/Debug/wwwtrans.dll" /implib:"$(OUTDIR)/wwwtrans.lib" 
DEF_FILE= \
	".\wwwtrans.def"
LINK32_OBJS= \
	"$(INTDIR)/HTMux.obj" \
	"$(INTDIR)/HTTCP.obj" \
	"$(INTDIR)/HTSocket.obj" \
	"$(INTDIR)/HTLocal.obj" \
	"$(INTDIR)/HTReader.obj" \
	"$(INTDIR)/HTBufWrt.obj" \
	"$(INTDIR)/HTWriter.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTANSI.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	"..\..\..\..\Msdev\Lib\Wsock32.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwtrans.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwstream - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwstream\Release"
# PROP BASE Intermediate_Dir "wwwstream\Release"
# PROP BASE Target_Dir "wwwstream"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wwwstream\Release"
# PROP Intermediate_Dir "wwwstream\Release"
# PROP Target_Dir "wwwstream"
OUTDIR=.\wwwstream\Release
INTDIR=.\wwwstream\Release

ALL : "$(OUTDIR)\wwwstream.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\wwwstream.dll"
	-@erase ".\wwwstream\Release\HTSChunk.obj"
	-@erase ".\wwwstream\Release\HTXParse.obj"
	-@erase ".\wwwstream\Release\HTGuess.obj"
	-@erase ".\wwwstream\Release\HTConLen.obj"
	-@erase ".\wwwstream\Release\windll.obj"
	-@erase ".\wwwstream\Release\HTTee.obj"
	-@erase ".\wwwstream\Release\HTEPtoCl.obj"
	-@erase ".\wwwstream\Release\wwwstream.lib"
	-@erase ".\wwwstream\Release\wwwstream.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwstream.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwstream\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwstream.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwstream.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwstream.pdb" /machine:I386 /def:".\wwwstream.def"\
 /out:"../../../Bin/Release/wwwstream.dll" /implib:"$(OUTDIR)/wwwstream.lib" 
DEF_FILE= \
	".\wwwstream.def"
LINK32_OBJS= \
	"$(INTDIR)/HTSChunk.obj" \
	"$(INTDIR)/HTXParse.obj" \
	"$(INTDIR)/HTGuess.obj" \
	"$(INTDIR)/HTConLen.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTTee.obj" \
	"$(INTDIR)/HTEPtoCl.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwstream.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwstream - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwstream\Debug"
# PROP BASE Intermediate_Dir "wwwstream\Debug"
# PROP BASE Target_Dir "wwwstream"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wwwstream\Debug"
# PROP Intermediate_Dir "wwwstream\Debug"
# PROP Target_Dir "wwwstream"
OUTDIR=.\wwwstream\Debug
INTDIR=.\wwwstream\Debug

ALL : "$(OUTDIR)\wwwstream.dll"

CLEAN : 
	-@erase ".\wwwstream\Debug\vc40.pdb"
	-@erase ".\wwwstream\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\wwwstream.dll"
	-@erase ".\wwwstream\Debug\HTEPtoCl.obj"
	-@erase ".\wwwstream\Debug\HTSChunk.obj"
	-@erase ".\wwwstream\Debug\HTTee.obj"
	-@erase ".\wwwstream\Debug\HTXParse.obj"
	-@erase ".\wwwstream\Debug\windll.obj"
	-@erase ".\wwwstream\Debug\HTConLen.obj"
	-@erase ".\wwwstream\Debug\HTGuess.obj"
	-@erase "..\..\..\Bin\Debug\wwwstream.ilk"
	-@erase ".\wwwstream\Debug\wwwstream.lib"
	-@erase ".\wwwstream\Debug\wwwstream.exp"
	-@erase ".\wwwstream\Debug\wwwstream.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/wwwstream.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwstream\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwstream.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwstream.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwstream.pdb" /debug /machine:I386 /def:".\wwwstream.def"\
 /out:"../../../Bin/Debug/wwwstream.dll" /implib:"$(OUTDIR)/wwwstream.lib" 
DEF_FILE= \
	".\wwwstream.def"
LINK32_OBJS= \
	"$(INTDIR)/HTEPtoCl.obj" \
	"$(INTDIR)/HTSChunk.obj" \
	"$(INTDIR)/HTTee.obj" \
	"$(INTDIR)/HTXParse.obj" \
	"$(INTDIR)/windll.obj" \
	"$(INTDIR)/HTConLen.obj" \
	"$(INTDIR)/HTGuess.obj" \
	".\wwwdll\Debug\wwwdll.lib" \
	".\wwwcore\Debug\wwwcore.lib" \
	".\wwwutils\Debug\wwwutils.lib"

"$(OUTDIR)\wwwstream.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "msvc_40 - Win32 Release"
# Name "msvc_40 - Win32 Debug"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcache"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwcache - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwcache - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwcache - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwcache - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwmime"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwmime - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwmime - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwmime - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwmime - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwhttp"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwhttp - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwhttp - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwhttp - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwhttp - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwrules"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwrules - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwrules - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwrules - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwrules - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwnews"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwnews - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwnews - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwnews - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwnews - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwgophe"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwgophe - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwgophe - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwgophe - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwgophe - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwftp"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwftp - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwftp - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwftp - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwftp - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdir"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwdir - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwdir - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwdir - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwdir - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwfile"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwfile - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwfile - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwfile - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwfile - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtelnt"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwtelnt - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwtelnt - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwtelnt - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwtelnt - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwhtml"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwhtml - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwhtml - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwhtml - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwhtml - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwapp"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwapp - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwapp - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwapp - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwapp - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwwais"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwwais - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwwais - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwwais - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwwais - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\windll.c

!IF  "$(CFG)" == "msvc_40 - Win32 Release"


"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"


"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\windll.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtrans"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwtrans - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwtrans - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwtrans - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwtrans - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwstream"

!IF  "$(CFG)" == "msvc_40 - Win32 Release"

"wwwstream - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwstream - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"

"wwwstream - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwstream - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\wwwdll.c

!IF  "$(CFG)" == "msvc_40 - Win32 Release"


"$(INTDIR)\wwwdll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msvc_40 - Win32 Debug"


"$(INTDIR)\wwwdll.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\wwwdll.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwdll - Win32 Release"
# Name "wwwdll - Win32 Debug"

!IF  "$(CFG)" == "wwwdll - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwdll - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll.c

"$(INTDIR)\wwwdll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll.def

!IF  "$(CFG)" == "wwwdll - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwdll - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwutils - Win32 Release"
# Name "wwwutils - Win32 Debug"

!IF  "$(CFG)" == "wwwutils - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwutils - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTUU.c
DEP_CPP_HTUU_=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	
NODEP_CPP_HTUU_=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTUU.obj" : $(SOURCE) $(DEP_CPP_HTUU_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAssoc.c
DEP_CPP_HTASS=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTAssoc.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTList.h"\
	
NODEP_CPP_HTASS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAssoc.obj" : $(SOURCE) $(DEP_CPP_HTASS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAtom.c
DEP_CPP_HTATO=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTList.h"\
	".\..\HTAtom.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	
NODEP_CPP_HTATO=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAtom.obj" : $(SOURCE) $(DEP_CPP_HTATO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTChunk.c
DEP_CPP_HTCHU=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTChunk.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	
NODEP_CPP_HTCHU=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTChunk.obj" : $(SOURCE) $(DEP_CPP_HTCHU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTList.c
DEP_CPP_HTLIS=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTList.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	
NODEP_CPP_HTLIS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTList.obj" : $(SOURCE) $(DEP_CPP_HTLIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTString.c
DEP_CPP_HTSTR=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	
NODEP_CPP_HTSTR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTString.obj" : $(SOURCE) $(DEP_CPP_HTSTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTArray.c
DEP_CPP_HTARR=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	
NODEP_CPP_HTARR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTArray.obj" : $(SOURCE) $(DEP_CPP_HTARR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwutils.def

!IF  "$(CFG)" == "wwwutils - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwutils - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwutils - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwutils - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMemory.c
DEP_CPP_HTMEM=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	
NODEP_CPP_HTMEM=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMemory.obj" : $(SOURCE) $(DEP_CPP_HTMEM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwcore - Win32 Release"
# Name "wwwcore - Win32 Debug"

!IF  "$(CFG)" == "wwwcore - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwcore - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwcore - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore.def

!IF  "$(CFG)" == "wwwcore - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTWWWStr.c
DEP_CPP_HTWWW=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTParse.h"\
	".\..\HTUser.h"\
	".\..\HTWWWStr.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTEscape.h"\
	
NODEP_CPP_HTWWW=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTWWWStr.obj" : $(SOURCE) $(DEP_CPP_HTWWW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTReqMan.c
DEP_CPP_HTREQ=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTAccess.h"\
	".\..\HTParse.h"\
	".\..\HTAlert.h"\
	".\..\HTError.h"\
	".\..\HTNetMan.h"\
	".\..\HTEvent.h"\
	".\..\HTProt.h"\
	".\..\HTHeader.h"\
	".\..\HTLib.h"\
	".\..\HTReqMan.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTReq.h"\
	".\..\HTAnchor.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTEscape.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTUTree.h"\
	
NODEP_CPP_HTREQ=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTReqMan.obj" : $(SOURCE) $(DEP_CPP_HTREQ) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTProt.c
DEP_CPP_HTPRO=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTParse.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTEscape.h"\
	".\..\HTIOStream.h"\
	".\..\HTChannl.h"\
	".\..\HTList.h"\
	".\..\HTStream.h"\
	".\..\HTNet.h"\
	".\..\HTEvent.h"\
	".\..\HTReq.h"\
	".\..\HTHost.h"\
	".\..\HTDNS.h"\
	".\..\HTAssoc.h"\
	".\..\HTFormat.h"\
	".\..\HTError.h"\
	".\..\HTUser.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	
NODEP_CPP_HTPRO=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTProt.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTParse.c
DEP_CPP_HTPAR=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTString.h"\
	".\..\HTParse.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTUU.h"\
	".\..\HTEscape.h"\
	
NODEP_CPP_HTPAR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTParse.obj" : $(SOURCE) $(DEP_CPP_HTPAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNet.c
DEP_CPP_HTNET=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTProt.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTParse.h"\
	".\..\HTTrans.h"\
	".\..\HTReqMan.h"\
	".\..\HTEvent.h"\
	".\..\HTStream.h"\
	".\..\HTNetMan.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTReq.h"\
	".\..\HTAnchor.h"\
	".\..\HTFormat.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTEscape.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTUTree.h"\
	".\..\HTHeader.h"\
	
NODEP_CPP_HTNET=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNet.obj" : $(SOURCE) $(DEP_CPP_HTNET) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMethod.c
DEP_CPP_HTMET=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTMethod.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	
NODEP_CPP_HTMET=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMethod.obj" : $(SOURCE) $(DEP_CPP_HTMET) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFWrite.c
DEP_CPP_HTFWR=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTFormat.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTAccess.h"\
	".\..\HTLib.h"\
	".\..\HTBind.h"\
	".\..\HTParse.h"\
	".\..\HTReq.h"\
	".\..\HTFWrite.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTStream.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTUser.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTNet.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTFWR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFWrite.obj" : $(SOURCE) $(DEP_CPP_HTFWR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFormat.c
DEP_CPP_HTFOR=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTError.h"\
	".\..\HTFormat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTReq.h"\
	".\..\HTEvent.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	
NODEP_CPP_HTFOR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFormat.obj" : $(SOURCE) $(DEP_CPP_HTFOR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEscape.c
DEP_CPP_HTESC=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTEscape.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	
NODEP_CPP_HTESC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTEscape.obj" : $(SOURCE) $(DEP_CPP_HTESC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTError.c
DEP_CPP_HTERR=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTInet.h"\
	".\..\HTError.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTReq.h"\
	".\..\HTNet.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTUser.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTERR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTError.obj" : $(SOURCE) $(DEP_CPP_HTERR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDNS.c
DEP_CPP_HTDNS=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTParse.h"\
	".\..\HTAlert.h"\
	".\..\HTError.h"\
	".\..\HTNetMan.h"\
	".\..\HTTrans.h"\
	".\..\HTDNS.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTEscape.h"\
	".\..\HTReq.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTIOStream.h"\
	".\..\HTProt.h"\
	
NODEP_CPP_HTDNS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTDNS.obj" : $(SOURCE) $(DEP_CPP_HTDNS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBind.c
DEP_CPP_HTBIN=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTAncMan.h"\
	".\..\HTParse.h"\
	".\..\HTBind.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTAnchor.h"\
	".\..\HTMethod.h"\
	".\..\HTLink.h"\
	".\..\HTEscape.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTReq.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTBIN=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTBind.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAnchor.c
DEP_CPP_HTANC=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTFormat.h"\
	".\..\HTParse.h"\
	".\..\HTMethod.h"\
	".\..\HTAncMan.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTStream.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\HTLink.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTEscape.h"\
	
NODEP_CPP_HTANC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAnchor.obj" : $(SOURCE) $(DEP_CPP_HTANC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAlert.c
DEP_CPP_HTALE=\
	".\..\WWWLib.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTALE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAlert.obj" : $(SOURCE) $(DEP_CPP_HTALE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Msdev\Lib\Wsock32.lib

!IF  "$(CFG)" == "wwwcore - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTInet.c
DEP_CPP_HTINE=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTParse.h"\
	".\..\HTAlert.h"\
	".\..\HTError.h"\
	".\..\HTNetMan.h"\
	".\..\HTDNS.h"\
	".\..\HTInet.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTEscape.h"\
	".\..\HTReq.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTIOStream.h"\
	".\..\HTProt.h"\
	
NODEP_CPP_HTINE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTInet.obj" : $(SOURCE) $(DEP_CPP_HTINE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTHost.c
DEP_CPP_HTHOS=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTParse.h"\
	".\..\HTAlert.h"\
	".\..\HTError.h"\
	".\..\HTNetMan.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTEscape.h"\
	".\..\HTReq.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTProt.h"\
	
NODEP_CPP_HTHOS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTHost.obj" : $(SOURCE) $(DEP_CPP_HTHOS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTrans.c
DEP_CPP_HTTRA=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTParse.h"\
	".\..\HTReq.h"\
	".\..\HTIOStream.h"\
	".\..\HTTrans.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTList.h"\
	".\..\HTAssoc.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	
NODEP_CPP_HTTRA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTrans.obj" : $(SOURCE) $(DEP_CPP_HTTRA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTChannl.c
DEP_CPP_HTCHA=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTAlert.h"\
	".\..\HTError.h"\
	".\..\HTChannl.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTReq.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTCHA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTChannl.obj" : $(SOURCE) $(DEP_CPP_HTCHA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTStream.c
DEP_CPP_HTSTRE=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTStream.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTList.h"\
	
NODEP_CPP_HTSTRE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTStream.obj" : $(SOURCE) $(DEP_CPP_HTSTRE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEvent.c
DEP_CPP_HTEVE=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTEvent.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTReq.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTEVE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTEvent.obj" : $(SOURCE) $(DEP_CPP_HTEVE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTUser.c
DEP_CPP_HTUSE=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTInet.h"\
	".\..\HTUser.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTReq.h"\
	".\..\HTNet.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTError.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTUSE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTUser.obj" : $(SOURCE) $(DEP_CPP_HTUSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\..\..\..\Library\src\HTLib.c
DEP_CPP_HTLIB=\
	".\..\WWWUtil.h"\
	".\..\HTBind.h"\
	".\..\HTAnchor.h"\
	".\..\HTProt.h"\
	".\..\HTDNS.h"\
	".\..\HTUTree.h"\
	".\..\HTLib.h"\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTReq.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	
NODEP_CPP_HTLIB=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTLib.obj" : $(SOURCE) $(DEP_CPP_HTLIB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\..\..\..\Library\src\HTLink.c
DEP_CPP_HTLIN=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTFormat.h"\
	".\..\HTParse.h"\
	".\..\HTMethod.h"\
	".\..\HTAncMan.h"\
	".\..\HTLink.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTStream.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTEscape.h"\
	
NODEP_CPP_HTLIN=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTLink.obj" : $(SOURCE) $(DEP_CPP_HTLIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\..\..\..\Library\src\HTUTree.c
DEP_CPP_HTUTR=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTUTree.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTReq.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTUTR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTUTree.obj" : $(SOURCE) $(DEP_CPP_HTUTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwcache - Win32 Release"
# Name "wwwcache - Win32 Debug"

!IF  "$(CFG)" == "wwwcache - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwcache - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwcache - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcache.def

!IF  "$(CFG)" == "wwwcache - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTCache.c
DEP_CPP_HTCAC=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTCache.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTCAC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTCache.obj" : $(SOURCE) $(DEP_CPP_HTCAC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwcache - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwmime - Win32 Release"
# Name "wwwmime - Win32 Debug"

!IF  "$(CFG)" == "wwwmime - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwmime - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwmime - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMIMERq.c

!IF  "$(CFG)" == "wwwmime - Win32 Release"

DEP_CPP_HTMIM=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTWWWStr.h"\
	".\..\HTParse.h"\
	".\..\HTFormat.h"\
	".\..\HTAncMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTDNS.h"\
	".\..\HTTCP.h"\
	".\..\HTHeader.h"\
	".\..\HTReqMan.h"\
	".\..\HTTPReq.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTUser.h"\
	".\..\HTEscape.h"\
	".\..\HTStream.h"\
	".\..\HTAtom.h"\
	".\..\HTList.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTIOStream.h"\
	".\..\HTProt.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTUTree.h"\
	
NODEP_CPP_HTMIM=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMERq.obj" : $(SOURCE) $(DEP_CPP_HTMIM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

DEP_CPP_HTMIM=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTWWWStr.h"\
	".\..\HTParse.h"\
	".\..\HTFormat.h"\
	".\..\HTAncMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTDNS.h"\
	".\..\HTTCP.h"\
	".\..\HTHeader.h"\
	".\..\HTReqMan.h"\
	".\..\HTTPReq.h"\
	".\..\HTMemory.h"\
	".\..\HTUser.h"\
	".\..\HTEscape.h"\
	".\..\HTStream.h"\
	".\..\HTAtom.h"\
	".\..\HTList.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTIOStream.h"\
	".\..\HTProt.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTUTree.h"\
	

"$(INTDIR)\HTMIMERq.obj" : $(SOURCE) $(DEP_CPP_HTMIM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTHeader.c

!IF  "$(CFG)" == "wwwmime - Win32 Release"

DEP_CPP_HTHEA=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTParse.h"\
	".\..\HTReq.h"\
	".\..\HTHeader.h"\
	".\..\HTMIMPrs.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTList.h"\
	".\..\HTAssoc.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTHEA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTHeader.obj" : $(SOURCE) $(DEP_CPP_HTHEA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

DEP_CPP_HTHEA=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTParse.h"\
	".\..\HTReq.h"\
	".\..\HTHeader.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTMemory.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTList.h"\
	".\..\HTAssoc.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	

"$(INTDIR)\HTHeader.obj" : $(SOURCE) $(DEP_CPP_HTHEA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMIME.c

!IF  "$(CFG)" == "wwwmime - Win32 Release"

DEP_CPP_HTMIME=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTReqMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTHeader.h"\
	".\..\HTWWWStr.h"\
	".\..\HTMIME.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	
NODEP_CPP_HTMIME=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIME.obj" : $(SOURCE) $(DEP_CPP_HTMIME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

DEP_CPP_HTMIME=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTReqMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTHeader.h"\
	".\..\HTWWWStr.h"\
	".\..\HTMIME.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	

"$(INTDIR)\HTMIME.obj" : $(SOURCE) $(DEP_CPP_HTMIME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBound.c

!IF  "$(CFG)" == "wwwmime - Win32 Release"

DEP_CPP_HTBOU=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTReqMan.h"\
	".\..\HTBound.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTHeader.h"\
	
NODEP_CPP_HTBOU=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTBound.obj" : $(SOURCE) $(DEP_CPP_HTBOU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

DEP_CPP_HTBOU=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTReqMan.h"\
	".\..\HTBound.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTHeader.h"\
	

"$(INTDIR)\HTBound.obj" : $(SOURCE) $(DEP_CPP_HTBOU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwmime.def

!IF  "$(CFG)" == "wwwmime - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwmime - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\..\..\..\Library\src\HTMIMPrs.c

!IF  "$(CFG)" == "wwwmime - Win32 Release"

DEP_CPP_HTMIMP=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTMIMPrs.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTHeader.h"\
	".\..\HTReq.h"\
	".\..\HTStream.h"\
	".\..\HTEvent.h"\
	".\..\HTList.h"\
	".\..\HTAssoc.h"\
	".\..\HTFormat.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTMIMP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMPrs.obj" : $(SOURCE) $(DEP_CPP_HTMIMP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

DEP_CPP_HTMIMP=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTMemory.h"\
	".\..\HTHeader.h"\
	".\..\HTReq.h"\
	".\..\HTStream.h"\
	".\..\HTEvent.h"\
	".\..\HTList.h"\
	".\..\HTAssoc.h"\
	".\..\HTFormat.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	

"$(INTDIR)\HTMIMPrs.obj" : $(SOURCE) $(DEP_CPP_HTMIMP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\..\..\..\Library\src\HTMIMImp.c

!IF  "$(CFG)" == "wwwmime - Win32 Release"

DEP_CPP_HTMIMI=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTReqMan.h"\
	".\..\HTHeader.h"\
	".\..\HTMIMImp.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	
NODEP_CPP_HTMIMI=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMImp.obj" : $(SOURCE) $(DEP_CPP_HTMIMI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

DEP_CPP_HTMIMI=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTReqMan.h"\
	".\..\HTHeader.h"\
	".\..\HTMIMImp.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	

"$(INTDIR)\HTMIMImp.obj" : $(SOURCE) $(DEP_CPP_HTMIMI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwhttp - Win32 Release"
# Name "wwwhttp - Win32 Debug"

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTPServ.c

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

DEP_CPP_HTTPS=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\WWWMIME.h"\
	".\..\WWWStream.h"\
	".\..\WWWTrans.h"\
	".\..\HTNetMan.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTPRes.h"\
	".\..\HTTPServ.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTMIME.h"\
	".\..\HTHeader.h"\
	".\..\HTMIMERq.h"\
	".\..\HTMIMImp.h"\
	".\..\HTBound.h"\
	".\..\HTMulpar.h"\
	".\..\HTConLen.h"\
	".\..\HTGuess.h"\
	".\..\HTTee.h"\
	".\..\HTSChunk.h"\
	".\..\HTXParse.h"\
	".\..\HTReader.h"\
	".\..\HTANSI.h"\
	".\..\HTLocal.h"\
	".\..\HTTCP.h"\
	".\..\HTSocket.h"\
	".\..\HTWriter.h"\
	".\..\HTBufWrt.h"\
	".\..\HTMux.h"\
	

"$(INTDIR)\HTTPServ.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

DEP_CPP_HTTPS=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\WWWMIME.h"\
	".\..\WWWStream.h"\
	".\..\WWWTrans.h"\
	".\..\HTNetMan.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTPRes.h"\
	".\..\HTTPServ.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTMIME.h"\
	".\..\HTHeader.h"\
	".\..\HTMIMERq.h"\
	".\..\HTMIMImp.h"\
	".\..\HTBound.h"\
	".\..\HTMulpar.h"\
	".\..\HTConLen.h"\
	".\..\HTGuess.h"\
	".\..\HTTee.h"\
	".\..\HTSChunk.h"\
	".\..\HTXParse.h"\
	".\..\HTReader.h"\
	".\..\HTANSI.h"\
	".\..\HTLocal.h"\
	".\..\HTTCP.h"\
	".\..\HTSocket.h"\
	".\..\HTWriter.h"\
	".\..\HTBufWrt.h"\
	".\..\HTMux.h"\
	
NODEP_CPP_HTTPS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPServ.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAAUtil.c

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

DEP_CPP_HTAAU=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTAAUtil.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	

"$(INTDIR)\HTAAUtil.obj" : $(SOURCE) $(DEP_CPP_HTAAU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

DEP_CPP_HTAAU=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTAAUtil.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTAAU=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAAUtil.obj" : $(SOURCE) $(DEP_CPP_HTAAU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTP.c

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

DEP_CPP_HTTP_=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\WWWCache.h"\
	".\..\WWWMIME.h"\
	".\..\WWWStream.h"\
	".\..\WWWTrans.h"\
	".\..\HTReqMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTPReq.h"\
	".\..\HTTP.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTCache.h"\
	".\..\HTMIME.h"\
	".\..\HTHeader.h"\
	".\..\HTMIMERq.h"\
	".\..\HTMIMImp.h"\
	".\..\HTBound.h"\
	".\..\HTMulpar.h"\
	".\..\HTConLen.h"\
	".\..\HTGuess.h"\
	".\..\HTTee.h"\
	".\..\HTSChunk.h"\
	".\..\HTXParse.h"\
	".\..\HTReader.h"\
	".\..\HTANSI.h"\
	".\..\HTLocal.h"\
	".\..\HTTCP.h"\
	".\..\HTSocket.h"\
	".\..\HTWriter.h"\
	".\..\HTBufWrt.h"\
	".\..\HTMux.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	

"$(INTDIR)\HTTP.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

DEP_CPP_HTTP_=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\WWWCache.h"\
	".\..\WWWMIME.h"\
	".\..\WWWStream.h"\
	".\..\WWWTrans.h"\
	".\..\HTReqMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTPReq.h"\
	".\..\HTTP.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTCache.h"\
	".\..\HTMIME.h"\
	".\..\HTHeader.h"\
	".\..\HTMIMERq.h"\
	".\..\HTMIMImp.h"\
	".\..\HTBound.h"\
	".\..\HTMulpar.h"\
	".\..\HTConLen.h"\
	".\..\HTGuess.h"\
	".\..\HTTee.h"\
	".\..\HTSChunk.h"\
	".\..\HTXParse.h"\
	".\..\HTReader.h"\
	".\..\HTANSI.h"\
	".\..\HTLocal.h"\
	".\..\HTTCP.h"\
	".\..\HTSocket.h"\
	".\..\HTWriter.h"\
	".\..\HTBufWrt.h"\
	".\..\HTMux.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	
NODEP_CPP_HTTP_=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTP.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTPReq.c

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

DEP_CPP_HTTPR=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTPReq.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	

"$(INTDIR)\HTTPReq.obj" : $(SOURCE) $(DEP_CPP_HTTPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

DEP_CPP_HTTPR=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTPReq.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTTPR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPReq.obj" : $(SOURCE) $(DEP_CPP_HTTPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTPRes.c

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

DEP_CPP_HTTPRE=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTNetMan.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTPRes.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	

"$(INTDIR)\HTTPRes.obj" : $(SOURCE) $(DEP_CPP_HTTPRE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

DEP_CPP_HTTPRE=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTNetMan.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTPRes.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTTPRE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPRes.obj" : $(SOURCE) $(DEP_CPP_HTTPRE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAABrow.c

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

DEP_CPP_HTAAB=\
	".\..\WWWLib.h"\
	".\..\HTAAUtil.h"\
	".\..\HTAABrow.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	

"$(INTDIR)\HTAABrow.obj" : $(SOURCE) $(DEP_CPP_HTAAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

DEP_CPP_HTAAB=\
	".\..\WWWLib.h"\
	".\..\HTAAUtil.h"\
	".\..\HTAABrow.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTAAB=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAABrow.obj" : $(SOURCE) $(DEP_CPP_HTAAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwhttp.def

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwmime\Debug\wwwmime.lib

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcache\Debug\wwwcache.lib

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTPGen.c

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

DEP_CPP_HTTPG=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTWWWStr.h"\
	".\..\HTParse.h"\
	".\..\HTFormat.h"\
	".\..\HTAncMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTTCP.h"\
	".\..\HTHeader.h"\
	".\..\HTReqMan.h"\
	".\..\HTTPReq.h"\
	".\..\HTMemory.h"\
	".\..\HTUser.h"\
	".\..\HTEscape.h"\
	".\..\HTStream.h"\
	".\..\HTAtom.h"\
	".\..\HTList.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTProt.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTUTree.h"\
	

"$(INTDIR)\HTTPGen.obj" : $(SOURCE) $(DEP_CPP_HTTPG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

DEP_CPP_HTTPG=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTWWWStr.h"\
	".\..\HTParse.h"\
	".\..\HTFormat.h"\
	".\..\HTAncMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTTCP.h"\
	".\..\HTHeader.h"\
	".\..\HTReqMan.h"\
	".\..\HTTPReq.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTUser.h"\
	".\..\HTEscape.h"\
	".\..\HTStream.h"\
	".\..\HTAtom.h"\
	".\..\HTList.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTProt.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTUTree.h"\
	
NODEP_CPP_HTTPG=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPGen.obj" : $(SOURCE) $(DEP_CPP_HTTPG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwtrans\Debug\wwwtrans.lib

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwstream\Debug\wwwstream.lib

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwstream"

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

"wwwstream - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwstream - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

"wwwstream - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwstream - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtrans"

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

"wwwtrans - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwtrans - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

"wwwtrans - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwtrans - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=..\HTTChunk.c

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

DEP_CPP_HTTCH=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTTChunk.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	

"$(INTDIR)\HTTChunk.obj" : $(SOURCE) $(DEP_CPP_HTTCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

DEP_CPP_HTTCH=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTTChunk.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTTCH=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTChunk.obj" : $(SOURCE) $(DEP_CPP_HTTCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwrules - Win32 Release"
# Name "wwwrules - Win32 Debug"

!IF  "$(CFG)" == "wwwrules - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwrules - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwrules - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwrules - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwrules - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwrules - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwrules.def

!IF  "$(CFG)" == "wwwrules - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwrules - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTProxy.c
DEP_CPP_HTPROX=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTList.h"\
	".\..\HTParse.h"\
	".\..\HTWWWStr.h"\
	".\..\HTProxy.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTEscape.h"\
	".\..\HTUser.h"\
	
NODEP_CPP_HTPROX=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTProxy.obj" : $(SOURCE) $(DEP_CPP_HTPROX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTRules.c
DEP_CPP_HTRUL=\
	".\..\WWWLib.h"\
	".\..\HTProxy.h"\
	".\..\HTRules.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTRUL=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTRules.obj" : $(SOURCE) $(DEP_CPP_HTRUL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwrules - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwrules - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwnews - Win32 Release"
# Name "wwwnews - Win32 Debug"

!IF  "$(CFG)" == "wwwnews - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwnews - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwnews - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwnews.def

!IF  "$(CFG)" == "wwwnews - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNewsRq.c
DEP_CPP_HTNEW=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTReqMan.h"\
	".\..\HTMIMERq.h"\
	".\..\HTNewsRq.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTHeader.h"\
	".\..\HTAccess.h"\
	
NODEP_CPP_HTNEW=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNewsRq.obj" : $(SOURCE) $(DEP_CPP_HTNEW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNews.c
DEP_CPP_HTNEWS=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\WWWStream.h"\
	".\..\HTTCP.h"\
	".\..\HTReqMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTNewsRq.h"\
	".\..\HTNews.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTConLen.h"\
	".\..\HTGuess.h"\
	".\..\HTTee.h"\
	".\..\HTSChunk.h"\
	".\..\HTXParse.h"\
	".\..\HTReader.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTHeader.h"\
	".\..\HTAccess.h"\
	
NODEP_CPP_HTNEWS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNews.obj" : $(SOURCE) $(DEP_CPP_HTNEWS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNewsLs.c
DEP_CPP_HTNEWSL=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTNDir.h"\
	".\..\HTNews.h"\
	".\..\HTNewsLs.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTNEWSL=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNewsLs.obj" : $(SOURCE) $(DEP_CPP_HTNEWSL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNDir.c
DEP_CPP_HTNDI=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTMLGen.h"\
	".\..\HTEscape.h"\
	".\..\HTParse.h"\
	".\..\HTFormat.h"\
	".\..\HTReq.h"\
	".\..\HTStruct.h"\
	".\..\HTArray.h"\
	".\..\HTError.h"\
	".\..\HTNews.h"\
	".\..\HTNDir.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\SGML.h"\
	".\..\HTStream.h"\
	".\..\HTList.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTNDI=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNDir.obj" : $(SOURCE) $(DEP_CPP_HTNDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwnews - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwmime\Debug\wwwmime.lib

!IF  "$(CFG)" == "wwwnews - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwhtml\Debug\wwwhtml.lib

!IF  "$(CFG)" == "wwwnews - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwhtml"

!IF  "$(CFG)" == "wwwnews - Win32 Release"

"wwwhtml - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwhtml - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

"wwwhtml - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwhtml - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\wwwtrans\Debug\wwwtrans.lib

!IF  "$(CFG)" == "wwwnews - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwstream\Debug\wwwstream.lib

!IF  "$(CFG)" == "wwwnews - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwgophe - Win32 Release"
# Name "wwwgophe - Win32 Debug"

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTGopher.c
DEP_CPP_HTGOP=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\WWWHTML.h"\
	".\..\WWWDir.h"\
	".\..\WWWTrans.h"\
	".\..\HTNetMan.h"\
	".\..\HTGopher.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTMLPDTD.h"\
	".\..\SGML.h"\
	".\..\HTMLGen.h"\
	".\..\HTTeXGen.h"\
	".\..\HTStruct.h"\
	".\..\HTDir.h"\
	".\..\HTIcons.h"\
	".\..\HTDescpt.h"\
	".\..\WWWLib.h"\
	".\..\HTANSI.h"\
	".\..\HTLocal.h"\
	".\..\HTTCP.h"\
	".\..\HTSocket.h"\
	".\..\HTReader.h"\
	".\..\HTWriter.h"\
	".\..\HTBufWrt.h"\
	".\..\HTMux.h"\
	
NODEP_CPP_HTGOP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTGopher.obj" : $(SOURCE) $(DEP_CPP_HTGOP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwgophe.def

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdir\Debug\wwwdir.lib

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwhtml\Debug\wwwhtml.lib

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdir"

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

"wwwdir - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwdir - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

"wwwdir - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\libwww.mak CFG="wwwdir - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\wwwtrans\Debug\wwwtrans.lib

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwftp - Win32 Release"
# Name "wwwftp - Win32 Debug"

!IF  "$(CFG)" == "wwwftp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwftp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwftp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwftp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwftp.def

!IF  "$(CFG)" == "wwwftp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdir\Debug\wwwdir.lib

!IF  "$(CFG)" == "wwwftp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFTPDir.c
DEP_CPP_HTFTP=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\WWWDir.h"\
	".\..\WWWTrans.h"\
	".\..\HTFTPDir.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTDir.h"\
	".\..\HTIcons.h"\
	".\..\HTDescpt.h"\
	".\..\WWWLib.h"\
	".\..\HTANSI.h"\
	".\..\HTLocal.h"\
	".\..\HTTCP.h"\
	".\..\HTSocket.h"\
	".\..\HTReader.h"\
	".\..\HTWriter.h"\
	".\..\HTBufWrt.h"\
	".\..\HTMux.h"\
	".\..\HTFTP.h"\
	
NODEP_CPP_HTFTP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFTPDir.obj" : $(SOURCE) $(DEP_CPP_HTFTP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFTP.c
DEP_CPP_HTFTP_=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTTCP.h"\
	".\..\HTReqMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTFTPDir.h"\
	".\..\HTFTP.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTHeader.h"\
	
NODEP_CPP_HTFTP_=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFTP.obj" : $(SOURCE) $(DEP_CPP_HTFTP_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Msdev\Lib\Wsock32.lib

!IF  "$(CFG)" == "wwwftp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwtrans\Debug\wwwtrans.lib

!IF  "$(CFG)" == "wwwftp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwdir - Win32 Release"
# Name "wwwdir - Win32 Debug"

!IF  "$(CFG)" == "wwwdir - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwdir - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwdir - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwdir - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdir.def

!IF  "$(CFG)" == "wwwdir - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTIcons.c
DEP_CPP_HTICO=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTIcons.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\WWWLib.h"\
	
NODEP_CPP_HTICO=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTIcons.obj" : $(SOURCE) $(DEP_CPP_HTICO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDir.c
DEP_CPP_HTDIR=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTMLGen.h"\
	".\..\HTBind.h"\
	".\..\HTEscape.h"\
	".\..\HTParse.h"\
	".\..\HTFormat.h"\
	".\..\HTReq.h"\
	".\..\HTIcons.h"\
	".\..\HTStruct.h"\
	".\..\HTDescpt.h"\
	".\..\HTArray.h"\
	".\..\HTError.h"\
	".\..\HTDir.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\SGML.h"\
	".\..\HTStream.h"\
	".\..\HTList.h"\
	".\..\HTAnchor.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\WWWLib.h"\
	".\..\WWWCore.h"\
	".\..\HTLib.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTAlert.h"\
	".\..\HTFWrite.h"\
	".\..\HTInet.h"\
	".\..\HTProt.h"\
	
NODEP_CPP_HTDIR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTDir.obj" : $(SOURCE) $(DEP_CPP_HTDIR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDescpt.c
DEP_CPP_HTDES=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTFormat.h"\
	".\..\HTList.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTStream.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTDES=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTDescpt.obj" : $(SOURCE) $(DEP_CPP_HTDES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwhtml\Debug\wwwhtml.lib

!IF  "$(CFG)" == "wwwdir - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwfile - Win32 Release"
# Name "wwwfile - Win32 Debug"

!IF  "$(CFG)" == "wwwfile - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwfile - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdir\Debug\wwwdir.lib

!IF  "$(CFG)" == "wwwfile - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwfile - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwfile.def

!IF  "$(CFG)" == "wwwfile - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMulti.c
DEP_CPP_HTMUL=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTMulti.h"\
	".\..\HTFile.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTMUL=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMulti.obj" : $(SOURCE) $(DEP_CPP_HTMUL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFile.c
DEP_CPP_HTFIL=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\WWWDir.h"\
	".\..\WWWTrans.h"\
	".\..\HTNetMan.h"\
	".\..\HTReqMan.h"\
	".\..\HTMulti.h"\
	".\..\HTFile.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTDir.h"\
	".\..\HTIcons.h"\
	".\..\HTDescpt.h"\
	".\..\WWWLib.h"\
	".\..\HTANSI.h"\
	".\..\HTLocal.h"\
	".\..\HTTCP.h"\
	".\..\HTSocket.h"\
	".\..\HTReader.h"\
	".\..\HTWriter.h"\
	".\..\HTBufWrt.h"\
	".\..\HTMux.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTHeader.h"\
	
NODEP_CPP_HTFIL=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFile.obj" : $(SOURCE) $(DEP_CPP_HTFIL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwfile - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwtrans\Debug\wwwtrans.lib

!IF  "$(CFG)" == "wwwfile - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwtelnt - Win32 Release"
# Name "wwwtelnt - Win32 Debug"

!IF  "$(CFG)" == "wwwtelnt - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwtelnt - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwtelnt - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdir\Debug\wwwdir.lib

!IF  "$(CFG)" == "wwwtelnt - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwtelnt - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwtelnt.def

!IF  "$(CFG)" == "wwwtelnt - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTelnet.c
DEP_CPP_HTTEL=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTTelnet.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTTEL=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTelnet.obj" : $(SOURCE) $(DEP_CPP_HTTEL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwhtml - Win32 Release"
# Name "wwwhtml - Win32 Debug"

!IF  "$(CFG)" == "wwwhtml - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhtml - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=..\HTMLGen.c
DEP_CPP_HTMLG=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTStruct.h"\
	".\..\HTFormat.h"\
	".\..\HTMLGen.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\SGML.h"\
	".\..\HTStream.h"\
	".\..\HTList.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTMLG=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMLGen.obj" : $(SOURCE) $(DEP_CPP_HTMLG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMLPDTD.c
DEP_CPP_HTMLP=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTMLPDTD.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTStruct.h"\
	".\..\SGML.h"\
	".\..\HTStream.h"\
	".\..\HTList.h"\
	
NODEP_CPP_HTMLP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwhtml - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhtml - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwhtml - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhtml - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwhtml - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhtml - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTeXGen.c
DEP_CPP_HTTEX=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTTeXGen.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTStruct.h"\
	".\..\HTFormat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\SGML.h"\
	".\..\HTStream.h"\
	".\..\HTList.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTTEX=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTeXGen.obj" : $(SOURCE) $(DEP_CPP_HTTEX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\SGML.c
DEP_CPP_SGML_=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTChunk.h"\
	".\..\SGML.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTStream.h"\
	".\..\HTStruct.h"\
	".\..\HTList.h"\
	
NODEP_CPP_SGML_=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\SGML.obj" : $(SOURCE) $(DEP_CPP_SGML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwhtml.def

!IF  "$(CFG)" == "wwwhtml - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwhtml - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwapp - Win32 Release"
# Name "wwwapp - Win32 Debug"

!IF  "$(CFG)" == "wwwapp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwapp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwapp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwapp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwapp.def

!IF  "$(CFG)" == "wwwapp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTLog.c
DEP_CPP_HTLOG=\
	".\..\WWWLib.h"\
	".\..\HTLog.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTLOG=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTLog.obj" : $(SOURCE) $(DEP_CPP_HTLOG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTHome.c
DEP_CPP_HTHOM=\
	".\..\WWWLib.h"\
	".\..\HTHome.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTHOM=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTHome.obj" : $(SOURCE) $(DEP_CPP_HTHOM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTHist.c
DEP_CPP_HTHIS=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTAnchor.h"\
	".\..\HTHist.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	
NODEP_CPP_HTHIS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTHist.obj" : $(SOURCE) $(DEP_CPP_HTHIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcache\Debug\wwwcache.lib

!IF  "$(CFG)" == "wwwapp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwrules\Debug\wwwrules.lib

!IF  "$(CFG)" == "wwwapp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDialog.c
DEP_CPP_HTDIA=\
	".\..\WWWLib.h"\
	".\..\WWWApp.h"\
	".\..\WWWHTTP.h"\
	".\..\HTDialog.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTEvntrg.h"\
	".\..\HTHome.h"\
	".\..\HTAccess.h"\
	".\..\HTFilter.h"\
	".\..\HTLog.h"\
	".\..\HTHist.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTP.h"\
	".\..\HTTPServ.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPReq.h"\
	".\..\HTTPRes.h"\
	".\..\HTTChunk.h"\
	".\..\HTAAUtil.h"\
	".\..\HTAABrow.h"\
	
NODEP_CPP_HTDIA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTDialog.obj" : $(SOURCE) $(DEP_CPP_HTDIA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEvntrg.c
DEP_CPP_HTEVN=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTReqMan.h"\
	".\..\HTEvntrg.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTHeader.h"\
	
NODEP_CPP_HTEVN=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTEvntrg.obj" : $(SOURCE) $(DEP_CPP_HTEVN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Msdev\Lib\Wsock32.lib

!IF  "$(CFG)" == "wwwapp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAccess.c
DEP_CPP_HTACC=\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\WWWStream.h"\
	".\..\HTReqMan.h"\
	".\..\HTAccess.h"\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTConLen.h"\
	".\..\HTGuess.h"\
	".\..\HTTee.h"\
	".\..\HTSChunk.h"\
	".\..\HTXParse.h"\
	".\..\HTReader.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTHeader.h"\
	
NODEP_CPP_HTACC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAccess.obj" : $(SOURCE) $(DEP_CPP_HTACC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwstream\Debug\wwwstream.lib

!IF  "$(CFG)" == "wwwapp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\..\..\..\Library\src\HTFilter.c
DEP_CPP_HTFILT=\
	".\..\WWWLib.h"\
	".\..\WWWCache.h"\
	".\..\WWWRules.h"\
	".\..\WWWHTTP.h"\
	".\..\HTLog.h"\
	".\..\HTAccess.h"\
	".\..\HTFilter.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTCache.h"\
	".\..\HTRules.h"\
	".\..\HTProxy.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTP.h"\
	".\..\HTTPServ.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPReq.h"\
	".\..\HTTPRes.h"\
	".\..\HTTChunk.h"\
	".\..\HTAAUtil.h"\
	".\..\HTAABrow.h"\
	
NODEP_CPP_HTFILT=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFilter.obj" : $(SOURCE) $(DEP_CPP_HTFILT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwhttp\Debug\wwwhttp.lib

!IF  "$(CFG)" == "wwwapp - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwwais - Win32 Release"
# Name "wwwwais - Win32 Debug"

!IF  "$(CFG)" == "wwwwais - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwwais - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwwais - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwwais - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwwais.def

!IF  "$(CFG)" == "wwwwais - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTWSRC.c

!IF  "$(CFG)" == "wwwwais - Win32 Release"

DEP_CPP_HTWSR=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTMLGen.h"\
	".\..\HTParse.h"\
	".\..\HTReqMan.h"\
	".\..\HTProxy.h"\
	".\..\HTWSRC.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTStruct.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTList.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTEscape.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTUTree.h"\
	".\..\HTHeader.h"\
	
NODEP_CPP_HTWSR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTWSRC.obj" : $(SOURCE) $(DEP_CPP_HTWSR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

DEP_CPP_HTWSR=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTMLGen.h"\
	".\..\HTParse.h"\
	".\..\HTReqMan.h"\
	".\..\HTProxy.h"\
	".\..\HTWSRC.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTStruct.h"\
	".\..\SGML.h"\
	".\..\HTStream.h"\
	".\..\HTList.h"\
	".\..\HTFormat.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTIOStream.h"\
	".\..\HTEscape.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTUTree.h"\
	".\..\HTHeader.h"\
	
NODEP_CPP_HTWSR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTWSRC.obj" : $(SOURCE) $(DEP_CPP_HTWSR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwhtml\Debug\wwwhtml.lib

!IF  "$(CFG)" == "wwwwais - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwrules\Debug\wwwrules.lib

!IF  "$(CFG)" == "wwwwais - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwtrans - Win32 Release"
# Name "wwwtrans - Win32 Debug"

!IF  "$(CFG)" == "wwwtrans - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwtrans.def

!IF  "$(CFG)" == "wwwtrans - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTANSI.c
DEP_CPP_HTANS=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTError.h"\
	".\..\HTBind.h"\
	".\..\HTAlert.h"\
	".\..\HTParse.h"\
	".\..\HTReq.h"\
	".\..\HTChannl.h"\
	".\..\HTIOStream.h"\
	".\..\HTNetMan.h"\
	".\..\HTANSI.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTFormat.h"\
	".\..\HTAnchor.h"\
	".\..\HTStream.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTDNS.h"\
	".\..\HTProt.h"\
	
NODEP_CPP_HTANS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTANSI.obj" : $(SOURCE) $(DEP_CPP_HTANS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTLocal.c
DEP_CPP_HTLOC=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\HTAlert.h"\
	".\..\HTParse.h"\
	".\..\HTReq.h"\
	".\..\HTNetMan.h"\
	".\..\HTError.h"\
	".\..\HTLocal.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTStream.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTProt.h"\
	
NODEP_CPP_HTLOC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTLocal.obj" : $(SOURCE) $(DEP_CPP_HTLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTReader.c
DEP_CPP_HTREA=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTNetMan.h"\
	".\..\HTReader.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTREA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTReader.obj" : $(SOURCE) $(DEP_CPP_HTREA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTSocket.c
DEP_CPP_HTSOC=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\WWWTrans.h"\
	".\..\HTNetMan.h"\
	".\..\HTSocket.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTANSI.h"\
	".\..\HTLocal.h"\
	".\..\HTTCP.h"\
	".\..\HTReader.h"\
	".\..\HTWriter.h"\
	".\..\HTBufWrt.h"\
	".\..\HTMux.h"\
	
NODEP_CPP_HTSOC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTSocket.obj" : $(SOURCE) $(DEP_CPP_HTSOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTCP.c
DEP_CPP_HTTCP=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTReqMan.h"\
	".\..\HTNetMan.h"\
	".\..\HTTCP.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTAABrow.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTHeader.h"\
	
NODEP_CPP_HTTCP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTCP.obj" : $(SOURCE) $(DEP_CPP_HTTCP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTWriter.c
DEP_CPP_HTWRI=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTNetMan.h"\
	".\..\HTWriter.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTWRI=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTWriter.obj" : $(SOURCE) $(DEP_CPP_HTWRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBufWrt.c
DEP_CPP_HTBUF=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTNetMan.h"\
	".\..\HTWriter.h"\
	".\..\HTBufWrt.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTBUF=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTBufWrt.obj" : $(SOURCE) $(DEP_CPP_HTBUF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Msdev\Lib\Wsock32.lib

!IF  "$(CFG)" == "wwwtrans - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwtrans - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwtrans - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwtrans - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMux.c
DEP_CPP_HTMUX=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTAlert.h"\
	".\..\HTNetMan.h"\
	".\..\HTStream.h"\
	".\..\HTMux.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTReq.h"\
	".\..\HTEvent.h"\
	".\..\HTList.h"\
	".\..\HTAssoc.h"\
	".\..\HTFormat.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	".\..\HTProt.h"\
	
NODEP_CPP_HTMUX=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMux.obj" : $(SOURCE) $(DEP_CPP_HTMUX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwstream - Win32 Release"
# Name "wwwstream - Win32 Debug"

!IF  "$(CFG)" == "wwwstream - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwstream - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wwwstream.def

!IF  "$(CFG)" == "wwwstream - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwstream - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTConLen.c
DEP_CPP_HTCON=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTConLen.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTCON=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTConLen.obj" : $(SOURCE) $(DEP_CPP_HTCON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTee.c
DEP_CPP_HTTEE=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTTee.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTStream.h"\
	".\..\HTList.h"\
	
NODEP_CPP_HTTEE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTee.obj" : $(SOURCE) $(DEP_CPP_HTTEE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTGuess.c
DEP_CPP_HTGUE=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTString.h"\
	".\..\HTFormat.h"\
	".\..\HTAlert.h"\
	".\..\HTAncMan.h"\
	".\..\HTList.h"\
	".\..\HTGuess.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTStream.h"\
	".\..\HTAtom.h"\
	".\..\HTAnchor.h"\
	".\..\HTReq.h"\
	".\..\WWWUtil.h"\
	".\..\HTLink.h"\
	".\..\HTMethod.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTChunk.h"\
	".\..\HTUU.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTNet.h"\
	".\..\HTUser.h"\
	".\..\HTTrans.h"\
	".\..\HTHost.h"\
	".\..\HTChannl.h"\
	".\..\HTDNS.h"\
	".\..\HTIOStream.h"\
	
NODEP_CPP_HTGUE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTGuess.obj" : $(SOURCE) $(DEP_CPP_HTGUE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEPtoCl.c
DEP_CPP_HTEPT=\
	".\..\sysdep.h"\
	".\..\HTUtils.h"\
	".\..\HTStream.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTMemory.h"\
	".\..\HTList.h"\
	
NODEP_CPP_HTEPT=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTEPtoCl.obj" : $(SOURCE) $(DEP_CPP_HTEPT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTXParse.c
DEP_CPP_HTXPA=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTXParse.h"\
	".\..\HTEPtoCl.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	".\..\HTReader.h"\
	
NODEP_CPP_HTXPA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTXParse.obj" : $(SOURCE) $(DEP_CPP_HTXPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "wwwstream - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwstream - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "wwwstream - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwstream - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "wwwstream - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwstream - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\..\..\..\Library\src\HTSChunk.c
DEP_CPP_HTSCH=\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\..\WWWCore.h"\
	".\..\HTSChunk.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\HTUtils.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUU.h"\
	".\..\HTLib.h"\
	".\..\HTReq.h"\
	".\..\HTMethod.h"\
	".\..\HTAnchor.h"\
	".\..\HTLink.h"\
	".\..\HTParse.h"\
	".\..\HTEscape.h"\
	".\..\HTUTree.h"\
	".\..\HTWWWStr.h"\
	".\..\HTUser.h"\
	".\..\HTEvent.h"\
	".\..\HTError.h"\
	".\..\HTAlert.h"\
	".\..\HTFormat.h"\
	".\..\HTBind.h"\
	".\..\HTStream.h"\
	".\..\HTIOStream.h"\
	".\..\HTFWrite.h"\
	".\..\HTDNS.h"\
	".\..\HTHost.h"\
	".\..\HTNet.h"\
	".\..\HTInet.h"\
	".\..\HTTrans.h"\
	".\..\HTProt.h"\
	".\..\HTChannl.h"\
	
NODEP_CPP_HTSCH=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTSChunk.obj" : $(SOURCE) $(DEP_CPP_HTSCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
