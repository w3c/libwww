# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=msvc - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to msvc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "msvc - Win32 Release" && "$(CFG)" != "msvc - Win32 Debug" &&\
 "$(CFG)" != "wwwdll - Win32 Release" && "$(CFG)" != "wwwdll - Win32 Debug" &&\
 "$(CFG)" != "wwwutils - Win32 Release" && "$(CFG)" != "wwwutils - Win32 Debug"\
 && "$(CFG)" != "wwwcore - Win32 Release" && "$(CFG)" != "wwwcore - Win32 Debug"\
 && "$(CFG)" != "wwwcache - Win32 Release" && "$(CFG)" !=\
 "wwwcache - Win32 Debug" && "$(CFG)" != "wwwmime - Win32 Release" && "$(CFG)"\
 != "wwwmime - Win32 Debug" && "$(CFG)" != "wwwhttp - Win32 Release" && "$(CFG)"\
 != "wwwhttp - Win32 Debug" && "$(CFG)" != "wwwnews - Win32 Release" && "$(CFG)"\
 != "wwwnews - Win32 Debug" && "$(CFG)" != "wwwgophe - Win32 Release" &&\
 "$(CFG)" != "wwwgophe - Win32 Debug" && "$(CFG)" != "wwwftp - Win32 Release" &&\
 "$(CFG)" != "wwwftp - Win32 Debug" && "$(CFG)" != "wwwdir - Win32 Release" &&\
 "$(CFG)" != "wwwdir - Win32 Debug" && "$(CFG)" != "wwwfile - Win32 Release" &&\
 "$(CFG)" != "wwwfile - Win32 Debug" && "$(CFG)" != "wwwtelnt - Win32 Release"\
 && "$(CFG)" != "wwwtelnt - Win32 Debug" && "$(CFG)" !=\
 "wwwhtml - Win32 Release" && "$(CFG)" != "wwwhtml - Win32 Debug" && "$(CFG)" !=\
 "wwwapp - Win32 Release" && "$(CFG)" != "wwwapp - Win32 Debug" && "$(CFG)" !=\
 "wwwwais - Win32 Release" && "$(CFG)" != "wwwwais - Win32 Debug" && "$(CFG)" !=\
 "wwwtrans - Win32 Release" && "$(CFG)" != "wwwtrans - Win32 Debug" && "$(CFG)"\
 != "wwwstream - Win32 Release" && "$(CFG)" != "wwwstream - Win32 Debug" &&\
 "$(CFG)" != "wwwmux - Win32 Release" && "$(CFG)" != "wwwmux - Win32 Debug" &&\
 "$(CFG)" != "wwwzip - Win32 Release" && "$(CFG)" != "wwwzip - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "libwww.mak" CFG="msvc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msvc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "msvc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
!MESSAGE "wwwmux - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwmux - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwzip - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wwwzip - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "msvc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\../../../Bin
INTDIR=.\Release

ALL : "wwwzip - Win32 Release" "wwwmux - Win32 Release"\
 "wwwstream - Win32 Release" "wwwtrans - Win32 Release"\
 "wwwwais - Win32 Release" "wwwapp - Win32 Release" "wwwhtml - Win32 Release"\
 "wwwtelnt - Win32 Release" "wwwfile - Win32 Release" "wwwdir - Win32 Release"\
 "wwwftp - Win32 Release" "wwwgophe - Win32 Release" "wwwnews - Win32 Release"\
 "wwwhttp - Win32 Release" "wwwmime - Win32 Release" "wwwcache - Win32 Release"\
 "wwwcore - Win32 Release" "wwwutils - Win32 Release" "wwwdll - Win32 Release"\
 "$(OUTDIR)\Release\msvc.dll"

CLEAN : 
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\wwwdll.obj"
	-@erase "$(OUTDIR)\msvc.exp"
	-@erase "$(OUTDIR)\msvc.lib"
	-@erase "$(OUTDIR)\Release\msvc.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/libwww.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/libwww.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/msvc.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/msvc.pdb" /machine:I386 /out:"$(OUTDIR)/Release\msvc.dll"\
 /implib:"$(OUTDIR)/msvc.lib" 
LINK32_OBJS= \
	"$(INTDIR)\windll.obj" \
	"$(INTDIR)\wwwdll.obj" \
	"$(OUTDIR)\wwwapp.lib" \
	"$(OUTDIR)\wwwcache.lib" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdir.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwfile.lib" \
	"$(OUTDIR)\wwwftp.lib" \
	"$(OUTDIR)\wwwgophe.lib" \
	"$(OUTDIR)\wwwhtml.lib" \
	"$(OUTDIR)\wwwhttp.lib" \
	"$(OUTDIR)\wwwmime.lib" \
	"$(OUTDIR)\wwwmux.lib" \
	"$(OUTDIR)\wwwnews.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwtelnt.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib" \
	"$(OUTDIR)\wwwwais.lib" \
	"$(OUTDIR)\wwwzip.lib"

"$(OUTDIR)\Release\msvc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\../../../Bin
INTDIR=.\Debug

ALL : "wwwzip - Win32 Debug" "wwwmux - Win32 Debug" "wwwstream - Win32 Debug"\
 "wwwtrans - Win32 Debug" "wwwwais - Win32 Debug" "wwwapp - Win32 Debug"\
 "wwwhtml - Win32 Debug" "wwwtelnt - Win32 Debug" "wwwfile - Win32 Debug"\
 "wwwdir - Win32 Debug" "wwwftp - Win32 Debug" "wwwgophe - Win32 Debug"\
 "wwwnews - Win32 Debug" "wwwhttp - Win32 Debug" "wwwmime - Win32 Debug"\
 "wwwcache - Win32 Debug" "wwwcore - Win32 Debug" "wwwutils - Win32 Debug"\
 "wwwdll - Win32 Debug" "$(OUTDIR)\Debug\msvc.dll" "$(OUTDIR)\libwww.bsc"

CLEAN : 
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\windll.sbr"
	-@erase "$(INTDIR)\wwwdll.obj"
	-@erase "$(INTDIR)\wwwdll.sbr"
	-@erase "$(OUTDIR)\Debug\msvc.dll"
	-@erase "$(OUTDIR)\Debug\msvc.ilk"
	-@erase "$(OUTDIR)\libwww.bsc"
	-@erase "$(OUTDIR)\msvc.exp"
	-@erase "$(OUTDIR)\msvc.lib"
	-@erase "$(OUTDIR)\msvc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /FR /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/libwww.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
	"$(INTDIR)\windll.sbr" \
	"$(INTDIR)\wwwdll.sbr"

"$(OUTDIR)\libwww.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/msvc.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/msvc.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Debug\msvc.dll"\
 /implib:"$(OUTDIR)/msvc.lib" 
LINK32_OBJS= \
	"$(INTDIR)\windll.obj" \
	"$(INTDIR)\wwwdll.obj" \
	"$(OUTDIR)\wwwapp.lib" \
	"$(OUTDIR)\wwwcache.lib" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdir.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwfile.lib" \
	"$(OUTDIR)\wwwftp.lib" \
	"$(OUTDIR)\wwwgophe.lib" \
	"$(OUTDIR)\wwwhtml.lib" \
	"$(OUTDIR)\wwwhttp.lib" \
	"$(OUTDIR)\wwwmime.lib" \
	"$(OUTDIR)\wwwmux.lib" \
	"$(OUTDIR)\wwwnews.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwtelnt.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib" \
	"$(OUTDIR)\wwwwais.lib" \
	"$(OUTDIR)\wwwzip.lib"

"$(OUTDIR)\Debug\msvc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwdll\Release"
# PROP Target_Dir "wwwdll"
OUTDIR=.\../../../Bin
INTDIR=.\wwwdll\Release

ALL : "$(OUTDIR)\Release\wwwdll.dll"

CLEAN : 
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\wwwdll.obj"
	-@erase "$(OUTDIR)\Release\wwwdll.dll"
	-@erase "$(OUTDIR)\wwwdll.exp"
	-@erase "$(OUTDIR)\wwwdll.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwdll.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwdll\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwdll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwdll.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwdll.pdb" /machine:I386 /def:".\wwwdll.def"\
 /out:"$(OUTDIR)/Release\wwwdll.dll" /implib:"$(OUTDIR)/wwwdll.lib" 
DEF_FILE= \
	".\wwwdll.def"
LINK32_OBJS= \
	"$(INTDIR)\windll.obj" \
	"$(INTDIR)\wwwdll.obj"

"$(OUTDIR)\Release\wwwdll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwdll\Debug"
# PROP Target_Dir "wwwdll"
OUTDIR=.\../../../Bin
INTDIR=.\wwwdll\Debug

ALL : "$(OUTDIR)\Debug\wwwdll.dll"

CLEAN : 
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\wwwdll.obj"
	-@erase "$(OUTDIR)\Debug\wwwdll.dll"
	-@erase "$(OUTDIR)\Debug\wwwdll.ilk"
	-@erase "$(OUTDIR)\wwwdll.exp"
	-@erase "$(OUTDIR)\wwwdll.lib"
	-@erase "$(OUTDIR)\wwwdll.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwdll.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwdll\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwdll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwdll.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwdll.pdb" /debug /machine:I386 /def:".\wwwdll.def"\
 /out:"$(OUTDIR)/Debug\wwwdll.dll" /implib:"$(OUTDIR)/wwwdll.lib" 
DEF_FILE= \
	".\wwwdll.def"
LINK32_OBJS= \
	"$(INTDIR)\windll.obj" \
	"$(INTDIR)\wwwdll.obj"

"$(OUTDIR)\Debug\wwwdll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwutils\Release"
# PROP Target_Dir "wwwutils"
OUTDIR=.\../../../Bin
INTDIR=.\wwwutils\Release

ALL : "wwwdll - Win32 Release" "$(OUTDIR)\Release\wwwutils.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTArray.obj"
	-@erase "$(INTDIR)\HTAssoc.obj"
	-@erase "$(INTDIR)\HTAtom.obj"
	-@erase "$(INTDIR)\HTChunk.obj"
	-@erase "$(INTDIR)\HTList.obj"
	-@erase "$(INTDIR)\HTMemory.obj"
	-@erase "$(INTDIR)\HTString.obj"
	-@erase "$(INTDIR)\HTTrace.obj"
	-@erase "$(INTDIR)\HTUU.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwutils.dll"
	-@erase "$(OUTDIR)\wwwutils.exp"
	-@erase "$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwutils.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwutils\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwutils.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwutils.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwutils.pdb" /machine:I386 /def:".\wwwutils.def"\
 /out:"$(OUTDIR)/Release\wwwutils.dll" /implib:"$(OUTDIR)/wwwutils.lib" 
DEF_FILE= \
	".\wwwutils.def"
LINK32_OBJS= \
	"$(INTDIR)\HTArray.obj" \
	"$(INTDIR)\HTAssoc.obj" \
	"$(INTDIR)\HTAtom.obj" \
	"$(INTDIR)\HTChunk.obj" \
	"$(INTDIR)\HTList.obj" \
	"$(INTDIR)\HTMemory.obj" \
	"$(INTDIR)\HTString.obj" \
	"$(INTDIR)\HTTrace.obj" \
	"$(INTDIR)\HTUU.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwdll.lib"

"$(OUTDIR)\Release\wwwutils.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwutils\Debug"
# PROP Target_Dir "wwwutils"
OUTDIR=.\../../../Bin
INTDIR=.\wwwutils\Debug

ALL : "wwwdll - Win32 Debug" "$(OUTDIR)\Debug\wwwutils.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTArray.obj"
	-@erase "$(INTDIR)\HTAssoc.obj"
	-@erase "$(INTDIR)\HTAtom.obj"
	-@erase "$(INTDIR)\HTChunk.obj"
	-@erase "$(INTDIR)\HTList.obj"
	-@erase "$(INTDIR)\HTMemory.obj"
	-@erase "$(INTDIR)\HTString.obj"
	-@erase "$(INTDIR)\HTTrace.obj"
	-@erase "$(INTDIR)\HTUU.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwutils.dll"
	-@erase "$(OUTDIR)\Debug\wwwutils.ilk"
	-@erase "$(OUTDIR)\wwwutils.exp"
	-@erase "$(OUTDIR)\wwwutils.lib"
	-@erase "$(OUTDIR)\wwwutils.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwutils.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwutils\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwutils.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwutils.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwutils.pdb" /debug /machine:I386 /def:".\wwwutils.def"\
 /out:"$(OUTDIR)/Debug\wwwutils.dll" /implib:"$(OUTDIR)/wwwutils.lib" 
DEF_FILE= \
	".\wwwutils.def"
LINK32_OBJS= \
	"$(INTDIR)\HTArray.obj" \
	"$(INTDIR)\HTAssoc.obj" \
	"$(INTDIR)\HTAtom.obj" \
	"$(INTDIR)\HTChunk.obj" \
	"$(INTDIR)\HTList.obj" \
	"$(INTDIR)\HTMemory.obj" \
	"$(INTDIR)\HTString.obj" \
	"$(INTDIR)\HTTrace.obj" \
	"$(INTDIR)\HTUU.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwdll.lib"

"$(OUTDIR)\Debug\wwwutils.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwcore\Release"
# PROP Target_Dir "wwwcore"
OUTDIR=.\../../../Bin
INTDIR=.\wwwcore\Release

ALL : "wwwutils - Win32 Release" "wwwdll - Win32 Release"\
 "$(OUTDIR)\Release\wwwcore.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTAlert.obj"
	-@erase "$(INTDIR)\HTAnchor.obj"
	-@erase "$(INTDIR)\HTBind.obj"
	-@erase "$(INTDIR)\HTChannl.obj"
	-@erase "$(INTDIR)\HTDNS.obj"
	-@erase "$(INTDIR)\HTError.obj"
	-@erase "$(INTDIR)\HTEscape.obj"
	-@erase "$(INTDIR)\HTEvent.obj"
	-@erase "$(INTDIR)\HTFormat.obj"
	-@erase "$(INTDIR)\HTFWrite.obj"
	-@erase "$(INTDIR)\HTHost.obj"
	-@erase "$(INTDIR)\HTInet.obj"
	-@erase "$(INTDIR)\HTLib.obj"
	-@erase "$(INTDIR)\HTLink.obj"
	-@erase "$(INTDIR)\HTMemLog.obj"
	-@erase "$(INTDIR)\HTMethod.obj"
	-@erase "$(INTDIR)\HTNet.obj"
	-@erase "$(INTDIR)\HTParse.obj"
	-@erase "$(INTDIR)\HTProt.obj"
	-@erase "$(INTDIR)\HTReqMan.obj"
	-@erase "$(INTDIR)\HTResponse.obj"
	-@erase "$(INTDIR)\HTStream.obj"
	-@erase "$(INTDIR)\HTTCP.obj"
	-@erase "$(INTDIR)\HTTimer.obj"
	-@erase "$(INTDIR)\HTTrans.obj"
	-@erase "$(INTDIR)\HTUser.obj"
	-@erase "$(INTDIR)\HTUTree.obj"
	-@erase "$(INTDIR)\HTWWWStr.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwcore.dll"
	-@erase "$(OUTDIR)\wwwcore.exp"
	-@erase "$(OUTDIR)\wwwcore.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwcore.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwcore\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwcore.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwcore.dll"
LINK32_FLAGS=WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwcore.pdb" /machine:I386 /def:".\wwwcore.def"\
 /out:"$(OUTDIR)/Release\wwwcore.dll" /implib:"$(OUTDIR)/wwwcore.lib" 
DEF_FILE= \
	".\wwwcore.def"
LINK32_OBJS= \
	"$(INTDIR)\HTAlert.obj" \
	"$(INTDIR)\HTAnchor.obj" \
	"$(INTDIR)\HTBind.obj" \
	"$(INTDIR)\HTChannl.obj" \
	"$(INTDIR)\HTDNS.obj" \
	"$(INTDIR)\HTError.obj" \
	"$(INTDIR)\HTEscape.obj" \
	"$(INTDIR)\HTEvent.obj" \
	"$(INTDIR)\HTFormat.obj" \
	"$(INTDIR)\HTFWrite.obj" \
	"$(INTDIR)\HTHost.obj" \
	"$(INTDIR)\HTInet.obj" \
	"$(INTDIR)\HTLib.obj" \
	"$(INTDIR)\HTLink.obj" \
	"$(INTDIR)\HTMemLog.obj" \
	"$(INTDIR)\HTMethod.obj" \
	"$(INTDIR)\HTNet.obj" \
	"$(INTDIR)\HTParse.obj" \
	"$(INTDIR)\HTProt.obj" \
	"$(INTDIR)\HTReqMan.obj" \
	"$(INTDIR)\HTResponse.obj" \
	"$(INTDIR)\HTStream.obj" \
	"$(INTDIR)\HTTCP.obj" \
	"$(INTDIR)\HTTimer.obj" \
	"$(INTDIR)\HTTrans.obj" \
	"$(INTDIR)\HTUser.obj" \
	"$(INTDIR)\HTUTree.obj" \
	"$(INTDIR)\HTWWWStr.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwcore.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwcore\Debug"
# PROP Target_Dir "wwwcore"
OUTDIR=.\../../../Bin
INTDIR=.\wwwcore\Debug

ALL : "wwwutils - Win32 Debug" "wwwdll - Win32 Debug"\
 "$(OUTDIR)\Debug\wwwcore.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTAlert.obj"
	-@erase "$(INTDIR)\HTAnchor.obj"
	-@erase "$(INTDIR)\HTBind.obj"
	-@erase "$(INTDIR)\HTChannl.obj"
	-@erase "$(INTDIR)\HTDNS.obj"
	-@erase "$(INTDIR)\HTError.obj"
	-@erase "$(INTDIR)\HTEscape.obj"
	-@erase "$(INTDIR)\HTEvent.obj"
	-@erase "$(INTDIR)\HTFormat.obj"
	-@erase "$(INTDIR)\HTFWrite.obj"
	-@erase "$(INTDIR)\HTHost.obj"
	-@erase "$(INTDIR)\HTInet.obj"
	-@erase "$(INTDIR)\HTLib.obj"
	-@erase "$(INTDIR)\HTLink.obj"
	-@erase "$(INTDIR)\HTMemLog.obj"
	-@erase "$(INTDIR)\HTMethod.obj"
	-@erase "$(INTDIR)\HTNet.obj"
	-@erase "$(INTDIR)\HTParse.obj"
	-@erase "$(INTDIR)\HTProt.obj"
	-@erase "$(INTDIR)\HTReqMan.obj"
	-@erase "$(INTDIR)\HTResponse.obj"
	-@erase "$(INTDIR)\HTStream.obj"
	-@erase "$(INTDIR)\HTTCP.obj"
	-@erase "$(INTDIR)\HTTimer.obj"
	-@erase "$(INTDIR)\HTTrans.obj"
	-@erase "$(INTDIR)\HTUser.obj"
	-@erase "$(INTDIR)\HTUTree.obj"
	-@erase "$(INTDIR)\HTWWWStr.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwcore.dll"
	-@erase "$(OUTDIR)\Debug\wwwcore.ilk"
	-@erase "$(OUTDIR)\wwwcore.exp"
	-@erase "$(OUTDIR)\wwwcore.lib"
	-@erase "$(OUTDIR)\wwwcore.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwcore.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwcore\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwcore.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwcore.dll"
LINK32_FLAGS=WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwcore.pdb" /debug /machine:I386 /def:".\wwwcore.def"\
 /out:"$(OUTDIR)/Debug\wwwcore.dll" /implib:"$(OUTDIR)/wwwcore.lib" 
DEF_FILE= \
	".\wwwcore.def"
LINK32_OBJS= \
	"$(INTDIR)\HTAlert.obj" \
	"$(INTDIR)\HTAnchor.obj" \
	"$(INTDIR)\HTBind.obj" \
	"$(INTDIR)\HTChannl.obj" \
	"$(INTDIR)\HTDNS.obj" \
	"$(INTDIR)\HTError.obj" \
	"$(INTDIR)\HTEscape.obj" \
	"$(INTDIR)\HTEvent.obj" \
	"$(INTDIR)\HTFormat.obj" \
	"$(INTDIR)\HTFWrite.obj" \
	"$(INTDIR)\HTHost.obj" \
	"$(INTDIR)\HTInet.obj" \
	"$(INTDIR)\HTLib.obj" \
	"$(INTDIR)\HTLink.obj" \
	"$(INTDIR)\HTMemLog.obj" \
	"$(INTDIR)\HTMethod.obj" \
	"$(INTDIR)\HTNet.obj" \
	"$(INTDIR)\HTParse.obj" \
	"$(INTDIR)\HTProt.obj" \
	"$(INTDIR)\HTReqMan.obj" \
	"$(INTDIR)\HTResponse.obj" \
	"$(INTDIR)\HTStream.obj" \
	"$(INTDIR)\HTTCP.obj" \
	"$(INTDIR)\HTTimer.obj" \
	"$(INTDIR)\HTTrans.obj" \
	"$(INTDIR)\HTUser.obj" \
	"$(INTDIR)\HTUTree.obj" \
	"$(INTDIR)\HTWWWStr.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwcore.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwcache\Release"
# PROP Target_Dir "wwwcache"
OUTDIR=.\../../../Bin
INTDIR=.\wwwcache\Release

ALL : "wwwtrans - Win32 Release" "wwwutils - Win32 Release"\
 "wwwdll - Win32 Release" "wwwcore - Win32 Release"\
 "$(OUTDIR)\Release\wwwcache.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTCache.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwcache.dll"
	-@erase "$(OUTDIR)\wwwcache.exp"
	-@erase "$(OUTDIR)\wwwcache.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwcache.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwcache\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwcache.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwcache.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwcache.pdb" /machine:I386 /def:".\wwwcache.def"\
 /out:"$(OUTDIR)/Release\wwwcache.dll" /implib:"$(OUTDIR)/wwwcache.lib" 
DEF_FILE= \
	".\wwwcache.def"
LINK32_OBJS= \
	"$(INTDIR)\HTCache.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwcache.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwcache\Debug"
# PROP Target_Dir "wwwcache"
OUTDIR=.\../../../Bin
INTDIR=.\wwwcache\Debug

ALL : "wwwtrans - Win32 Debug" "wwwutils - Win32 Debug" "wwwdll - Win32 Debug"\
 "wwwcore - Win32 Debug" "$(OUTDIR)\Debug\wwwcache.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTCache.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwcache.dll"
	-@erase "$(OUTDIR)\Debug\wwwcache.ilk"
	-@erase "$(OUTDIR)\wwwcache.exp"
	-@erase "$(OUTDIR)\wwwcache.lib"
	-@erase "$(OUTDIR)\wwwcache.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwcache.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwcache\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwcache.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwcache.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwcache.pdb" /debug /machine:I386 /def:".\wwwcache.def"\
 /out:"$(OUTDIR)/Debug\wwwcache.dll" /implib:"$(OUTDIR)/wwwcache.lib" 
DEF_FILE= \
	".\wwwcache.def"
LINK32_OBJS= \
	"$(INTDIR)\HTCache.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwcache.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwmime\Release"
# PROP Target_Dir "wwwmime"
OUTDIR=.\../../../Bin
INTDIR=.\wwwmime\Release

ALL : "wwwcache - Win32 Release" "wwwutils - Win32 Release"\
 "wwwdll - Win32 Release" "wwwcore - Win32 Release" "wwwstream - Win32 Release"\
 "$(OUTDIR)\Release\wwwmime.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTBound.obj"
	-@erase "$(INTDIR)\HTHeader.obj"
	-@erase "$(INTDIR)\HTMIME.obj"
	-@erase "$(INTDIR)\HTMIMERq.obj"
	-@erase "$(INTDIR)\HTMIMImp.obj"
	-@erase "$(INTDIR)\HTMIMPrs.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwmime.dll"
	-@erase "$(OUTDIR)\wwwmime.exp"
	-@erase "$(OUTDIR)\wwwmime.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwmime.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwmime\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwmime.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwmime.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwmime.pdb" /machine:I386 /def:".\wwwmime.def"\
 /out:"$(OUTDIR)/Release\wwwmime.dll" /implib:"$(OUTDIR)/wwwmime.lib" 
DEF_FILE= \
	".\wwwmime.def"
LINK32_OBJS= \
	"$(INTDIR)\HTBound.obj" \
	"$(INTDIR)\HTHeader.obj" \
	"$(INTDIR)\HTMIME.obj" \
	"$(INTDIR)\HTMIMERq.obj" \
	"$(INTDIR)\HTMIMImp.obj" \
	"$(INTDIR)\HTMIMPrs.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcache.lib" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwmime.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwmime\Debug"
# PROP Target_Dir "wwwmime"
OUTDIR=.\../../../Bin
INTDIR=.\wwwmime\Debug

ALL : "wwwcache - Win32 Debug" "wwwutils - Win32 Debug" "wwwdll - Win32 Debug"\
 "wwwcore - Win32 Debug" "wwwstream - Win32 Debug" "$(OUTDIR)\Debug\wwwmime.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTBound.obj"
	-@erase "$(INTDIR)\HTHeader.obj"
	-@erase "$(INTDIR)\HTMIME.obj"
	-@erase "$(INTDIR)\HTMIMERq.obj"
	-@erase "$(INTDIR)\HTMIMImp.obj"
	-@erase "$(INTDIR)\HTMIMPrs.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwmime.dll"
	-@erase "$(OUTDIR)\Debug\wwwmime.ilk"
	-@erase "$(OUTDIR)\wwwmime.exp"
	-@erase "$(OUTDIR)\wwwmime.lib"
	-@erase "$(OUTDIR)\wwwmime.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwmime.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwmime\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwmime.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwmime.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwmime.pdb" /debug /machine:I386 /def:".\wwwmime.def"\
 /out:"$(OUTDIR)/Debug\wwwmime.dll" /implib:"$(OUTDIR)/wwwmime.lib" 
DEF_FILE= \
	".\wwwmime.def"
LINK32_OBJS= \
	"$(INTDIR)\HTBound.obj" \
	"$(INTDIR)\HTHeader.obj" \
	"$(INTDIR)\HTMIME.obj" \
	"$(INTDIR)\HTMIMERq.obj" \
	"$(INTDIR)\HTMIMImp.obj" \
	"$(INTDIR)\HTMIMPrs.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcache.lib" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwmime.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwhttp\Release"
# PROP Target_Dir "wwwhttp"
OUTDIR=.\../../../Bin
INTDIR=.\wwwhttp\Release

ALL : "wwwutils - Win32 Release" "wwwmime - Win32 Release"\
 "wwwdll - Win32 Release" "wwwcore - Win32 Release" "wwwcache - Win32 Release"\
 "wwwtrans - Win32 Release" "wwwstream - Win32 Release"\
 "$(OUTDIR)\Release\wwwhttp.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTAABrow.obj"
	-@erase "$(INTDIR)\HTAAUtil.obj"
	-@erase "$(INTDIR)\HTPEP.obj"
	-@erase "$(INTDIR)\HTTChunk.obj"
	-@erase "$(INTDIR)\HTTP.obj"
	-@erase "$(INTDIR)\HTTPGen.obj"
	-@erase "$(INTDIR)\HTTPReq.obj"
	-@erase "$(INTDIR)\HTTPRes.obj"
	-@erase "$(INTDIR)\HTTPServ.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwhttp.dll"
	-@erase "$(OUTDIR)\wwwhttp.exp"
	-@erase "$(OUTDIR)\wwwhttp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwhttp.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwhttp\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwhttp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwhttp.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwhttp.pdb" /machine:I386 /def:".\wwwhttp.def"\
 /out:"$(OUTDIR)/Release\wwwhttp.dll" /implib:"$(OUTDIR)/wwwhttp.lib" 
DEF_FILE= \
	".\wwwhttp.def"
LINK32_OBJS= \
	"$(INTDIR)\HTAABrow.obj" \
	"$(INTDIR)\HTAAUtil.obj" \
	"$(INTDIR)\HTPEP.obj" \
	"$(INTDIR)\HTTChunk.obj" \
	"$(INTDIR)\HTTP.obj" \
	"$(INTDIR)\HTTPGen.obj" \
	"$(INTDIR)\HTTPReq.obj" \
	"$(INTDIR)\HTTPRes.obj" \
	"$(INTDIR)\HTTPServ.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcache.lib" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwmime.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwhttp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwhttp\Debug"
# PROP Target_Dir "wwwhttp"
OUTDIR=.\../../../Bin
INTDIR=.\wwwhttp\Debug

ALL : "wwwutils - Win32 Debug" "wwwmime - Win32 Debug" "wwwdll - Win32 Debug"\
 "wwwcore - Win32 Debug" "wwwcache - Win32 Debug" "wwwtrans - Win32 Debug"\
 "wwwstream - Win32 Debug" "$(OUTDIR)\Debug\wwwhttp.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTAABrow.obj"
	-@erase "$(INTDIR)\HTAAUtil.obj"
	-@erase "$(INTDIR)\HTPEP.obj"
	-@erase "$(INTDIR)\HTTChunk.obj"
	-@erase "$(INTDIR)\HTTP.obj"
	-@erase "$(INTDIR)\HTTPGen.obj"
	-@erase "$(INTDIR)\HTTPReq.obj"
	-@erase "$(INTDIR)\HTTPRes.obj"
	-@erase "$(INTDIR)\HTTPServ.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwhttp.dll"
	-@erase "$(OUTDIR)\Debug\wwwhttp.ilk"
	-@erase "$(OUTDIR)\wwwhttp.exp"
	-@erase "$(OUTDIR)\wwwhttp.lib"
	-@erase "$(OUTDIR)\wwwhttp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwhttp.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwhttp\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwhttp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwhttp.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwhttp.pdb" /debug /machine:I386 /def:".\wwwhttp.def"\
 /out:"$(OUTDIR)/Debug\wwwhttp.dll" /implib:"$(OUTDIR)/wwwhttp.lib" 
DEF_FILE= \
	".\wwwhttp.def"
LINK32_OBJS= \
	"$(INTDIR)\HTAABrow.obj" \
	"$(INTDIR)\HTAAUtil.obj" \
	"$(INTDIR)\HTPEP.obj" \
	"$(INTDIR)\HTTChunk.obj" \
	"$(INTDIR)\HTTP.obj" \
	"$(INTDIR)\HTTPGen.obj" \
	"$(INTDIR)\HTTPReq.obj" \
	"$(INTDIR)\HTTPRes.obj" \
	"$(INTDIR)\HTTPServ.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcache.lib" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwmime.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwhttp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwnews\Release"
# PROP Target_Dir "wwwnews"
OUTDIR=.\../../../Bin
INTDIR=.\wwwnews\Release

ALL : "wwwutils - Win32 Release" "wwwtrans - Win32 Release"\
 "wwwstream - Win32 Release" "wwwmime - Win32 Release" "wwwdll - Win32 Release"\
 "wwwcore - Win32 Release" "wwwhtml - Win32 Release"\
 "$(OUTDIR)\Release\wwwnews.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTNDir.obj"
	-@erase "$(INTDIR)\HTNews.obj"
	-@erase "$(INTDIR)\HTNewsLs.obj"
	-@erase "$(INTDIR)\HTNewsRq.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwnews.dll"
	-@erase "$(OUTDIR)\wwwnews.exp"
	-@erase "$(OUTDIR)\wwwnews.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwnews.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwnews\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwnews.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwnews.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwnews.pdb" /machine:I386 /def:".\wwwnews.def"\
 /out:"$(OUTDIR)/Release\wwwnews.dll" /implib:"$(OUTDIR)/wwwnews.lib" 
DEF_FILE= \
	".\wwwnews.def"
LINK32_OBJS= \
	"$(INTDIR)\HTNDir.obj" \
	"$(INTDIR)\HTNews.obj" \
	"$(INTDIR)\HTNewsLs.obj" \
	"$(INTDIR)\HTNewsRq.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwhtml.lib" \
	"$(OUTDIR)\wwwmime.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwnews.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwnews\Debug"
# PROP Target_Dir "wwwnews"
OUTDIR=.\../../../Bin
INTDIR=.\wwwnews\Debug

ALL : "wwwutils - Win32 Debug" "wwwtrans - Win32 Debug"\
 "wwwstream - Win32 Debug" "wwwmime - Win32 Debug" "wwwdll - Win32 Debug"\
 "wwwcore - Win32 Debug" "wwwhtml - Win32 Debug" "$(OUTDIR)\Debug\wwwnews.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTNDir.obj"
	-@erase "$(INTDIR)\HTNews.obj"
	-@erase "$(INTDIR)\HTNewsLs.obj"
	-@erase "$(INTDIR)\HTNewsRq.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwnews.dll"
	-@erase "$(OUTDIR)\Debug\wwwnews.ilk"
	-@erase "$(OUTDIR)\wwwnews.exp"
	-@erase "$(OUTDIR)\wwwnews.lib"
	-@erase "$(OUTDIR)\wwwnews.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwnews.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwnews\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwnews.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwnews.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwnews.pdb" /debug /machine:I386 /def:".\wwwnews.def"\
 /out:"$(OUTDIR)/Debug\wwwnews.dll" /implib:"$(OUTDIR)/wwwnews.lib" 
DEF_FILE= \
	".\wwwnews.def"
LINK32_OBJS= \
	"$(INTDIR)\HTNDir.obj" \
	"$(INTDIR)\HTNews.obj" \
	"$(INTDIR)\HTNewsLs.obj" \
	"$(INTDIR)\HTNewsRq.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwhtml.lib" \
	"$(OUTDIR)\wwwmime.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwnews.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwgophe\Release"
# PROP Target_Dir "wwwgophe"
OUTDIR=.\../../../Bin
INTDIR=.\wwwgophe\Release

ALL : "wwwutils - Win32 Release" "wwwtrans - Win32 Release"\
 "wwwhtml - Win32 Release" "wwwdll - Win32 Release" "wwwcore - Win32 Release"\
 "wwwdir - Win32 Release" "$(OUTDIR)\Release\wwwgophe.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTGopher.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwgophe.dll"
	-@erase "$(OUTDIR)\wwwgophe.exp"
	-@erase "$(OUTDIR)\wwwgophe.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwgophe.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwgophe\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwgophe.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwgophe.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwgophe.pdb" /machine:I386 /def:".\wwwgophe.def"\
 /out:"$(OUTDIR)/Release\wwwgophe.dll" /implib:"$(OUTDIR)/wwwgophe.lib" 
DEF_FILE= \
	".\wwwgophe.def"
LINK32_OBJS= \
	"$(INTDIR)\HTGopher.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdir.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwhtml.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwgophe.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwgophe\Debug"
# PROP Target_Dir "wwwgophe"
OUTDIR=.\../../../Bin
INTDIR=.\wwwgophe\Debug

ALL : "wwwutils - Win32 Debug" "wwwtrans - Win32 Debug" "wwwhtml - Win32 Debug"\
 "wwwdll - Win32 Debug" "wwwcore - Win32 Debug" "wwwdir - Win32 Debug"\
 "$(OUTDIR)\Debug\wwwgophe.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTGopher.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwgophe.dll"
	-@erase "$(OUTDIR)\Debug\wwwgophe.ilk"
	-@erase "$(OUTDIR)\wwwgophe.exp"
	-@erase "$(OUTDIR)\wwwgophe.lib"
	-@erase "$(OUTDIR)\wwwgophe.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwgophe.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwgophe\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwgophe.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwgophe.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwgophe.pdb" /debug /machine:I386 /def:".\wwwgophe.def"\
 /out:"$(OUTDIR)/Debug\wwwgophe.dll" /implib:"$(OUTDIR)/wwwgophe.lib" 
DEF_FILE= \
	".\wwwgophe.def"
LINK32_OBJS= \
	"$(INTDIR)\HTGopher.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdir.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwhtml.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwgophe.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwftp\Release"
# PROP Target_Dir "wwwftp"
OUTDIR=.\../../../Bin
INTDIR=.\wwwftp\Release

ALL : "wwwutils - Win32 Release" "wwwtrans - Win32 Release"\
 "wwwdll - Win32 Release" "wwwdir - Win32 Release" "wwwcore - Win32 Release"\
 "$(OUTDIR)\Release\wwwftp.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTFTP.obj"
	-@erase "$(INTDIR)\HTFTPDir.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwftp.dll"
	-@erase "$(OUTDIR)\wwwftp.exp"
	-@erase "$(OUTDIR)\wwwftp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwftp.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwftp\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwftp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwftp.dll"
LINK32_FLAGS=WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwftp.pdb" /machine:I386 /def:".\wwwftp.def"\
 /out:"$(OUTDIR)/Release\wwwftp.dll" /implib:"$(OUTDIR)/wwwftp.lib" 
DEF_FILE= \
	".\wwwftp.def"
LINK32_OBJS= \
	"$(INTDIR)\HTFTP.obj" \
	"$(INTDIR)\HTFTPDir.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdir.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwftp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwftp\Debug"
# PROP Target_Dir "wwwftp"
OUTDIR=.\../../../Bin
INTDIR=.\wwwftp\Debug

ALL : "wwwutils - Win32 Debug" "wwwtrans - Win32 Debug" "wwwdll - Win32 Debug"\
 "wwwdir - Win32 Debug" "wwwcore - Win32 Debug" "$(OUTDIR)\Debug\wwwftp.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTFTP.obj"
	-@erase "$(INTDIR)\HTFTPDir.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwftp.dll"
	-@erase "$(OUTDIR)\Debug\wwwftp.ilk"
	-@erase "$(OUTDIR)\wwwftp.exp"
	-@erase "$(OUTDIR)\wwwftp.lib"
	-@erase "$(OUTDIR)\wwwftp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwftp.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwftp\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwftp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwftp.dll"
LINK32_FLAGS=WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwftp.pdb" /debug /machine:I386 /def:".\wwwftp.def"\
 /out:"$(OUTDIR)/Debug\wwwftp.dll" /implib:"$(OUTDIR)/wwwftp.lib" 
DEF_FILE= \
	".\wwwftp.def"
LINK32_OBJS= \
	"$(INTDIR)\HTFTP.obj" \
	"$(INTDIR)\HTFTPDir.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdir.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwftp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwdir\Release"
# PROP Target_Dir "wwwdir"
OUTDIR=.\../../../Bin
INTDIR=.\wwwdir\Release

ALL : "wwwutils - Win32 Release" "wwwhtml - Win32 Release"\
 "wwwdll - Win32 Release" "wwwcore - Win32 Release"\
 "$(OUTDIR)\Release\wwwdir.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTDescpt.obj"
	-@erase "$(INTDIR)\HTDir.obj"
	-@erase "$(INTDIR)\HTIcons.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwdir.dll"
	-@erase "$(OUTDIR)\wwwdir.exp"
	-@erase "$(OUTDIR)\wwwdir.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwdir.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwdir\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwdir.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwdir.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwdir.pdb" /machine:I386 /def:".\wwwdir.def"\
 /out:"$(OUTDIR)/Release\wwwdir.dll" /implib:"$(OUTDIR)/wwwdir.lib" 
DEF_FILE= \
	".\wwwdir.def"
LINK32_OBJS= \
	"$(INTDIR)\HTDescpt.obj" \
	"$(INTDIR)\HTDir.obj" \
	"$(INTDIR)\HTIcons.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwhtml.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwdir.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwdir\Debug"
# PROP Target_Dir "wwwdir"
OUTDIR=.\../../../Bin
INTDIR=.\wwwdir\Debug

ALL : "wwwutils - Win32 Debug" "wwwhtml - Win32 Debug" "wwwdll - Win32 Debug"\
 "wwwcore - Win32 Debug" "$(OUTDIR)\Debug\wwwdir.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTDescpt.obj"
	-@erase "$(INTDIR)\HTDir.obj"
	-@erase "$(INTDIR)\HTIcons.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwdir.dll"
	-@erase "$(OUTDIR)\Debug\wwwdir.ilk"
	-@erase "$(OUTDIR)\wwwdir.exp"
	-@erase "$(OUTDIR)\wwwdir.lib"
	-@erase "$(OUTDIR)\wwwdir.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwdir.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwdir\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwdir.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwdir.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwdir.pdb" /debug /machine:I386 /def:".\wwwdir.def"\
 /out:"$(OUTDIR)/Debug\wwwdir.dll" /implib:"$(OUTDIR)/wwwdir.lib" 
DEF_FILE= \
	".\wwwdir.def"
LINK32_OBJS= \
	"$(INTDIR)\HTDescpt.obj" \
	"$(INTDIR)\HTDir.obj" \
	"$(INTDIR)\HTIcons.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwhtml.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwdir.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwfile\Release"
# PROP Target_Dir "wwwfile"
OUTDIR=.\../../../Bin
INTDIR=.\wwwfile\Release

ALL : "wwwutils - Win32 Release" "wwwtrans - Win32 Release"\
 "wwwdll - Win32 Release" "wwwdir - Win32 Release" "wwwcore - Win32 Release"\
 "$(OUTDIR)\Release\wwwfile.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTFile.obj"
	-@erase "$(INTDIR)\HTMulti.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwfile.dll"
	-@erase "$(OUTDIR)\wwwfile.exp"
	-@erase "$(OUTDIR)\wwwfile.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwfile.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwfile\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwfile.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwfile.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwfile.pdb" /machine:I386 /def:".\wwwfile.def"\
 /out:"$(OUTDIR)/Release\wwwfile.dll" /implib:"$(OUTDIR)/wwwfile.lib" 
DEF_FILE= \
	".\wwwfile.def"
LINK32_OBJS= \
	"$(INTDIR)\HTFile.obj" \
	"$(INTDIR)\HTMulti.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdir.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwfile.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwfile\Debug"
# PROP Target_Dir "wwwfile"
OUTDIR=.\../../../Bin
INTDIR=.\wwwfile\Debug

ALL : "wwwutils - Win32 Debug" "wwwtrans - Win32 Debug" "wwwdll - Win32 Debug"\
 "wwwdir - Win32 Debug" "wwwcore - Win32 Debug" "$(OUTDIR)\Debug\wwwfile.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTFile.obj"
	-@erase "$(INTDIR)\HTMulti.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwfile.dll"
	-@erase "$(OUTDIR)\Debug\wwwfile.ilk"
	-@erase "$(OUTDIR)\wwwfile.exp"
	-@erase "$(OUTDIR)\wwwfile.lib"
	-@erase "$(OUTDIR)\wwwfile.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwfile.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwfile\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwfile.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwfile.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwfile.pdb" /debug /machine:I386 /def:".\wwwfile.def"\
 /out:"$(OUTDIR)/Debug\wwwfile.dll" /implib:"$(OUTDIR)/wwwfile.lib" 
DEF_FILE= \
	".\wwwfile.def"
LINK32_OBJS= \
	"$(INTDIR)\HTFile.obj" \
	"$(INTDIR)\HTMulti.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdir.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwfile.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwtelnt\Release"
# PROP Target_Dir "wwwtelnt"
OUTDIR=.\../../../Bin
INTDIR=.\wwwtelnt\Release

ALL : "wwwutils - Win32 Release" "wwwdll - Win32 Release"\
 "wwwdir - Win32 Release" "wwwcore - Win32 Release"\
 "$(OUTDIR)\Release\wwwtelnt.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTTelnet.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwtelnt.dll"
	-@erase "$(OUTDIR)\wwwtelnt.exp"
	-@erase "$(OUTDIR)\wwwtelnt.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwtelnt.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwtelnt\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwtelnt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwtelnt.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwtelnt.pdb" /machine:I386 /def:".\wwwtelnt.def"\
 /out:"$(OUTDIR)/Release\wwwtelnt.dll" /implib:"$(OUTDIR)/wwwtelnt.lib" 
DEF_FILE= \
	".\wwwtelnt.def"
LINK32_OBJS= \
	"$(INTDIR)\HTTelnet.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdir.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwtelnt.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwtelnt\Debug"
# PROP Target_Dir "wwwtelnt"
OUTDIR=.\../../../Bin
INTDIR=.\wwwtelnt\Debug

ALL : "wwwutils - Win32 Debug" "wwwdll - Win32 Debug" "wwwdir - Win32 Debug"\
 "wwwcore - Win32 Debug" "$(OUTDIR)\Debug\wwwtelnt.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTTelnet.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwtelnt.dll"
	-@erase "$(OUTDIR)\Debug\wwwtelnt.ilk"
	-@erase "$(OUTDIR)\wwwtelnt.exp"
	-@erase "$(OUTDIR)\wwwtelnt.lib"
	-@erase "$(OUTDIR)\wwwtelnt.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwtelnt.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwtelnt\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwtelnt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwtelnt.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwtelnt.pdb" /debug /machine:I386 /def:".\wwwtelnt.def"\
 /out:"$(OUTDIR)/Debug\wwwtelnt.dll" /implib:"$(OUTDIR)/wwwtelnt.lib" 
DEF_FILE= \
	".\wwwtelnt.def"
LINK32_OBJS= \
	"$(INTDIR)\HTTelnet.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdir.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwtelnt.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwhtml\Release"
# PROP Target_Dir "wwwhtml"
OUTDIR=.\../../../Bin
INTDIR=.\wwwhtml\Release

ALL : "wwwutils - Win32 Release" "wwwdll - Win32 Release"\
 "wwwcore - Win32 Release" "$(OUTDIR)\Release\wwwhtml.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTMLGen.obj"
	-@erase "$(INTDIR)\HTMLPDTD.obj"
	-@erase "$(INTDIR)\HTTeXGen.obj"
	-@erase "$(INTDIR)\SGML.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwhtml.dll"
	-@erase "$(OUTDIR)\wwwhtml.exp"
	-@erase "$(OUTDIR)\wwwhtml.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwhtml.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwhtml\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwhtml.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwhtml.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwhtml.pdb" /machine:I386 /def:".\wwwhtml.def"\
 /out:"$(OUTDIR)/Release\wwwhtml.dll" /implib:"$(OUTDIR)/wwwhtml.lib" 
DEF_FILE= \
	".\wwwhtml.def"
LINK32_OBJS= \
	"$(INTDIR)\HTMLGen.obj" \
	"$(INTDIR)\HTMLPDTD.obj" \
	"$(INTDIR)\HTTeXGen.obj" \
	"$(INTDIR)\SGML.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwhtml.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwhtml\Debug"
# PROP Target_Dir "wwwhtml"
OUTDIR=.\../../../Bin
INTDIR=.\wwwhtml\Debug

ALL : "wwwutils - Win32 Debug" "wwwdll - Win32 Debug" "wwwcore - Win32 Debug"\
 "$(OUTDIR)\Debug\wwwhtml.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTMLGen.obj"
	-@erase "$(INTDIR)\HTMLPDTD.obj"
	-@erase "$(INTDIR)\HTTeXGen.obj"
	-@erase "$(INTDIR)\SGML.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwhtml.dll"
	-@erase "$(OUTDIR)\Debug\wwwhtml.ilk"
	-@erase "$(OUTDIR)\wwwhtml.exp"
	-@erase "$(OUTDIR)\wwwhtml.lib"
	-@erase "$(OUTDIR)\wwwhtml.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwhtml.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwhtml\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwhtml.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwhtml.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwhtml.pdb" /debug /machine:I386 /def:".\wwwhtml.def"\
 /out:"$(OUTDIR)/Debug\wwwhtml.dll" /implib:"$(OUTDIR)/wwwhtml.lib" 
DEF_FILE= \
	".\wwwhtml.def"
LINK32_OBJS= \
	"$(INTDIR)\HTMLGen.obj" \
	"$(INTDIR)\HTMLPDTD.obj" \
	"$(INTDIR)\HTTeXGen.obj" \
	"$(INTDIR)\SGML.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwhtml.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwapp\Release"
# PROP Target_Dir "wwwapp"
OUTDIR=.\../../../Bin
INTDIR=.\wwwapp\Release

ALL : "wwwutils - Win32 Release" "wwwstream - Win32 Release"\
 "wwwhttp - Win32 Release" "wwwdll - Win32 Release" "wwwcore - Win32 Release"\
 "wwwcache - Win32 Release" "$(OUTDIR)\Release\wwwapp.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTAccess.obj"
	-@erase "$(INTDIR)\HTDialog.obj"
	-@erase "$(INTDIR)\HTEvtLst.obj"
	-@erase "$(INTDIR)\HTFilter.obj"
	-@erase "$(INTDIR)\HTHist.obj"
	-@erase "$(INTDIR)\HTHome.obj"
	-@erase "$(INTDIR)\HTLog.obj"
	-@erase "$(INTDIR)\HTProxy.obj"
	-@erase "$(INTDIR)\HTRules.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwapp.dll"
	-@erase "$(OUTDIR)\wwwapp.exp"
	-@erase "$(OUTDIR)\wwwapp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwapp.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwapp\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwapp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwapp.dll"
LINK32_FLAGS=WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwapp.pdb" /machine:I386 /def:".\wwwapp.def"\
 /out:"$(OUTDIR)/Release\wwwapp.dll" /implib:"$(OUTDIR)/wwwapp.lib" 
DEF_FILE= \
	".\wwwapp.def"
LINK32_OBJS= \
	"$(INTDIR)\HTAccess.obj" \
	"$(INTDIR)\HTDialog.obj" \
	"$(INTDIR)\HTEvtLst.obj" \
	"$(INTDIR)\HTFilter.obj" \
	"$(INTDIR)\HTHist.obj" \
	"$(INTDIR)\HTHome.obj" \
	"$(INTDIR)\HTLog.obj" \
	"$(INTDIR)\HTProxy.obj" \
	"$(INTDIR)\HTRules.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcache.lib" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwhttp.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwapp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwapp\Debug"
# PROP Target_Dir "wwwapp"
OUTDIR=.\../../../Bin
INTDIR=.\wwwapp\Debug

ALL : "wwwutils - Win32 Debug" "wwwstream - Win32 Debug"\
 "wwwhttp - Win32 Debug" "wwwdll - Win32 Debug" "wwwcore - Win32 Debug"\
 "wwwcache - Win32 Debug" "$(OUTDIR)\Debug\wwwapp.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTAccess.obj"
	-@erase "$(INTDIR)\HTDialog.obj"
	-@erase "$(INTDIR)\HTEvtLst.obj"
	-@erase "$(INTDIR)\HTFilter.obj"
	-@erase "$(INTDIR)\HTHist.obj"
	-@erase "$(INTDIR)\HTHome.obj"
	-@erase "$(INTDIR)\HTLog.obj"
	-@erase "$(INTDIR)\HTProxy.obj"
	-@erase "$(INTDIR)\HTRules.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwapp.dll"
	-@erase "$(OUTDIR)\Debug\wwwapp.ilk"
	-@erase "$(OUTDIR)\wwwapp.exp"
	-@erase "$(OUTDIR)\wwwapp.lib"
	-@erase "$(OUTDIR)\wwwapp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwapp.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwapp\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwapp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwapp.dll"
LINK32_FLAGS=WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwapp.pdb" /debug /machine:I386 /def:".\wwwapp.def"\
 /out:"$(OUTDIR)/Debug\wwwapp.dll" /implib:"$(OUTDIR)/wwwapp.lib" 
DEF_FILE= \
	".\wwwapp.def"
LINK32_OBJS= \
	"$(INTDIR)\HTAccess.obj" \
	"$(INTDIR)\HTDialog.obj" \
	"$(INTDIR)\HTEvtLst.obj" \
	"$(INTDIR)\HTFilter.obj" \
	"$(INTDIR)\HTHist.obj" \
	"$(INTDIR)\HTHome.obj" \
	"$(INTDIR)\HTLog.obj" \
	"$(INTDIR)\HTProxy.obj" \
	"$(INTDIR)\HTRules.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcache.lib" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwhttp.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwapp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwwais\Release"
# PROP Target_Dir "wwwwais"
OUTDIR=.\../../../Bin
INTDIR=.\wwwwais\Release

ALL : "wwwutils - Win32 Release" "wwwhtml - Win32 Release"\
 "wwwdll - Win32 Release" "wwwcore - Win32 Release" "wwwapp - Win32 Release"\
 "$(OUTDIR)\Release\wwwwais.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTWSRC.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwwais.dll"
	-@erase "$(OUTDIR)\wwwwais.exp"
	-@erase "$(OUTDIR)\wwwwais.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwwais.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwwais\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwwais.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwwais.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwwais.pdb" /machine:I386 /def:".\wwwwais.def"\
 /out:"$(OUTDIR)/Release\wwwwais.dll" /implib:"$(OUTDIR)/wwwwais.lib" 
DEF_FILE= \
	".\wwwwais.def"
LINK32_OBJS= \
	"$(INTDIR)\HTWSRC.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwapp.lib" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwhtml.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwwais.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwwais\Debug"
# PROP Target_Dir "wwwwais"
OUTDIR=.\../../../Bin
INTDIR=.\wwwwais\Debug

ALL : "wwwutils - Win32 Debug" "wwwhtml - Win32 Debug" "wwwdll - Win32 Debug"\
 "wwwcore - Win32 Debug" "wwwapp - Win32 Debug" "$(OUTDIR)\Debug\wwwwais.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTWSRC.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwwais.dll"
	-@erase "$(OUTDIR)\Debug\wwwwais.ilk"
	-@erase "$(OUTDIR)\wwwwais.exp"
	-@erase "$(OUTDIR)\wwwwais.lib"
	-@erase "$(OUTDIR)\wwwwais.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwwais.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwwais\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwwais.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwwais.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwwais.pdb" /debug /machine:I386 /def:".\wwwwais.def"\
 /out:"$(OUTDIR)/Debug\wwwwais.dll" /implib:"$(OUTDIR)/wwwwais.lib" 
DEF_FILE= \
	".\wwwwais.def"
LINK32_OBJS= \
	"$(INTDIR)\HTWSRC.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwapp.lib" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwhtml.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwwais.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwtrans\Release"
# PROP Target_Dir "wwwtrans"
OUTDIR=.\../../../Bin
INTDIR=.\wwwtrans\Release

ALL : "wwwutils - Win32 Release" "wwwdll - Win32 Release"\
 "wwwcore - Win32 Release" "$(OUTDIR)\Release\wwwtrans.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTANSI.obj"
	-@erase "$(INTDIR)\HTBufWrt.obj"
	-@erase "$(INTDIR)\HTLocal.obj"
	-@erase "$(INTDIR)\HTReader.obj"
	-@erase "$(INTDIR)\HTSocket.obj"
	-@erase "$(INTDIR)\HTWriter.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwtrans.dll"
	-@erase "$(OUTDIR)\wwwtrans.exp"
	-@erase "$(OUTDIR)\wwwtrans.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwtrans.pch"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwtrans\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwtrans.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwtrans.dll"
LINK32_FLAGS=WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwtrans.pdb" /machine:I386 /def:".\wwwtrans.def"\
 /out:"$(OUTDIR)/Release\wwwtrans.dll" /implib:"$(OUTDIR)/wwwtrans.lib" 
DEF_FILE= \
	".\wwwtrans.def"
LINK32_OBJS= \
	"$(INTDIR)\HTANSI.obj" \
	"$(INTDIR)\HTBufWrt.obj" \
	"$(INTDIR)\HTLocal.obj" \
	"$(INTDIR)\HTReader.obj" \
	"$(INTDIR)\HTSocket.obj" \
	"$(INTDIR)\HTWriter.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwtrans.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwtrans\Debug"
# PROP Target_Dir "wwwtrans"
OUTDIR=.\../../../Bin
INTDIR=.\wwwtrans\Debug

ALL : "wwwutils - Win32 Debug" "wwwdll - Win32 Debug" "wwwcore - Win32 Debug"\
 "$(OUTDIR)\Debug\wwwtrans.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTANSI.obj"
	-@erase "$(INTDIR)\HTBufWrt.obj"
	-@erase "$(INTDIR)\HTLocal.obj"
	-@erase "$(INTDIR)\HTReader.obj"
	-@erase "$(INTDIR)\HTSocket.obj"
	-@erase "$(INTDIR)\HTWriter.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwtrans.dll"
	-@erase "$(OUTDIR)\Debug\wwwtrans.ilk"
	-@erase "$(OUTDIR)\wwwtrans.exp"
	-@erase "$(OUTDIR)\wwwtrans.lib"
	-@erase "$(OUTDIR)\wwwtrans.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwtrans.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwtrans\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwtrans.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwtrans.dll"
LINK32_FLAGS=WSock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwtrans.pdb" /debug /machine:I386 /def:".\wwwtrans.def"\
 /out:"$(OUTDIR)/Debug\wwwtrans.dll" /implib:"$(OUTDIR)/wwwtrans.lib" 
DEF_FILE= \
	".\wwwtrans.def"
LINK32_OBJS= \
	"$(INTDIR)\HTANSI.obj" \
	"$(INTDIR)\HTBufWrt.obj" \
	"$(INTDIR)\HTLocal.obj" \
	"$(INTDIR)\HTReader.obj" \
	"$(INTDIR)\HTSocket.obj" \
	"$(INTDIR)\HTWriter.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwtrans.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwstream\Release"
# PROP Target_Dir "wwwstream"
OUTDIR=.\../../../Bin
INTDIR=.\wwwstream\Release

ALL : "wwwutils - Win32 Release" "wwwdll - Win32 Release"\
 "wwwcore - Win32 Release" "$(OUTDIR)\Release\wwwstream.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTConLen.obj"
	-@erase "$(INTDIR)\HTEPtoCl.obj"
	-@erase "$(INTDIR)\HTGuess.obj"
	-@erase "$(INTDIR)\HTMerge.obj"
	-@erase "$(INTDIR)\HTSChunk.obj"
	-@erase "$(INTDIR)\HTTee.obj"
	-@erase "$(INTDIR)\HTXParse.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Release\wwwstream.dll"
	-@erase "$(OUTDIR)\wwwstream.exp"
	-@erase "$(OUTDIR)\wwwstream.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwstream.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwstream\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwstream.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwstream.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwstream.pdb" /machine:I386 /def:".\wwwstream.def"\
 /out:"$(OUTDIR)/Release\wwwstream.dll" /implib:"$(OUTDIR)/wwwstream.lib" 
DEF_FILE= \
	".\wwwstream.def"
LINK32_OBJS= \
	"$(INTDIR)\HTConLen.obj" \
	"$(INTDIR)\HTEPtoCl.obj" \
	"$(INTDIR)\HTGuess.obj" \
	"$(INTDIR)\HTMerge.obj" \
	"$(INTDIR)\HTSChunk.obj" \
	"$(INTDIR)\HTTee.obj" \
	"$(INTDIR)\HTXParse.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Release\wwwstream.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwstream\Debug"
# PROP Target_Dir "wwwstream"
OUTDIR=.\../../../Bin
INTDIR=.\wwwstream\Debug

ALL : "wwwutils - Win32 Debug" "wwwdll - Win32 Debug" "wwwcore - Win32 Debug"\
 "$(OUTDIR)\Debug\wwwstream.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTConLen.obj"
	-@erase "$(INTDIR)\HTEPtoCl.obj"
	-@erase "$(INTDIR)\HTGuess.obj"
	-@erase "$(INTDIR)\HTMerge.obj"
	-@erase "$(INTDIR)\HTSChunk.obj"
	-@erase "$(INTDIR)\HTTee.obj"
	-@erase "$(INTDIR)\HTXParse.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\Debug\wwwstream.dll"
	-@erase "$(OUTDIR)\Debug\wwwstream.ilk"
	-@erase "$(OUTDIR)\wwwstream.exp"
	-@erase "$(OUTDIR)\wwwstream.lib"
	-@erase "$(OUTDIR)\wwwstream.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwstream.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwstream\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwstream.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwstream.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwstream.pdb" /debug /machine:I386 /def:".\wwwstream.def"\
 /out:"$(OUTDIR)/Debug\wwwstream.dll" /implib:"$(OUTDIR)/wwwstream.lib" 
DEF_FILE= \
	".\wwwstream.def"
LINK32_OBJS= \
	"$(INTDIR)\HTConLen.obj" \
	"$(INTDIR)\HTEPtoCl.obj" \
	"$(INTDIR)\HTGuess.obj" \
	"$(INTDIR)\HTMerge.obj" \
	"$(INTDIR)\HTSChunk.obj" \
	"$(INTDIR)\HTTee.obj" \
	"$(INTDIR)\HTXParse.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\Debug\wwwstream.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwmux - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwmux\Release"
# PROP BASE Intermediate_Dir "wwwmux\Release"
# PROP BASE Target_Dir "wwwmux"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwmux\Release"
# PROP Target_Dir "wwwmux"
OUTDIR=.\../../../Bin
INTDIR=.\wwwmux\Release

ALL : "wwwtrans - Win32 Release" "wwwstream - Win32 Release"\
 "wwwdll - Win32 Release" "wwwutils - Win32 Release" "wwwcore - Win32 Release"\
 "$(OUTDIR)\wwwmux.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTDemux.obj"
	-@erase "$(INTDIR)\HTMux.obj"
	-@erase "$(INTDIR)\HTMuxCh.obj"
	-@erase "$(INTDIR)\HTMuxTx.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\wwwmux.dll"
	-@erase "$(OUTDIR)\wwwmux.exp"
	-@erase "$(OUTDIR)\wwwmux.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwmux.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwmux\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwmux.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wwwmux.pdb" /machine:I386 /def:".\wwwmux.def"\
 /out:"$(OUTDIR)/wwwmux.dll" /implib:"$(OUTDIR)/wwwmux.lib" 
DEF_FILE= \
	".\wwwmux.def"
LINK32_OBJS= \
	"$(INTDIR)\HTDemux.obj" \
	"$(INTDIR)\HTMux.obj" \
	"$(INTDIR)\HTMuxCh.obj" \
	"$(INTDIR)\HTMuxTx.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\wwwmux.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwmux - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwmux\wwwmux__"
# PROP BASE Intermediate_Dir "wwwmux\wwwmux__"
# PROP BASE Target_Dir "wwwmux"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwmux\wwwmux__"
# PROP Target_Dir "wwwmux"
OUTDIR=.\../../../Bin
INTDIR=.\wwwmux\wwwmux__

ALL : "wwwtrans - Win32 Debug" "wwwstream - Win32 Debug" "wwwdll - Win32 Debug"\
 "wwwutils - Win32 Debug" "wwwcore - Win32 Debug" "$(OUTDIR)\wwwmux.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTDemux.obj"
	-@erase "$(INTDIR)\HTMux.obj"
	-@erase "$(INTDIR)\HTMuxCh.obj"
	-@erase "$(INTDIR)\HTMuxTx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(OUTDIR)\wwwmux.dll"
	-@erase "$(OUTDIR)\wwwmux.exp"
	-@erase "$(OUTDIR)\wwwmux.ilk"
	-@erase "$(OUTDIR)\wwwmux.lib"
	-@erase "$(OUTDIR)\wwwmux.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WWW_WIN_DLL" /D\
 "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/wwwmux.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwmux\wwwmux__/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwmux.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib WSock32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 WSock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/wwwmux.pdb" /debug /machine:I386 /def:".\wwwmux.def"\
 /out:"$(OUTDIR)/wwwmux.dll" /implib:"$(OUTDIR)/wwwmux.lib" 
DEF_FILE= \
	".\wwwmux.def"
LINK32_OBJS= \
	"$(INTDIR)\HTDemux.obj" \
	"$(INTDIR)\HTMux.obj" \
	"$(INTDIR)\HTMuxCh.obj" \
	"$(INTDIR)\HTMuxTx.obj" \
	"$(INTDIR)\windll.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwtrans.lib" \
	"$(OUTDIR)\wwwutils.lib"

"$(OUTDIR)\wwwmux.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwzip - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wwwzip\Release"
# PROP BASE Intermediate_Dir "wwwzip\Release"
# PROP BASE Target_Dir "wwwzip"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwzip\Release"
# PROP Target_Dir "wwwzip"
OUTDIR=.\../../../Bin
INTDIR=.\wwwzip\Release

ALL : "wwwutils - Win32 Release" "wwwstream - Win32 Release"\
 "wwwdll - Win32 Release" "wwwcore - Win32 Release"\
 "$(OUTDIR)\Release\wwwzip.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTZip.obj"
	-@erase "$(OUTDIR)\Release\wwwzip.dll"
	-@erase "$(OUTDIR)\wwwzip.exp"
	-@erase "$(OUTDIR)\wwwzip.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\External" /D "NDEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\External" /D "NDEBUG" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/wwwzip.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wwwzip\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwzip.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/wwwzip.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/wwwzip.pdb"\
 /machine:I386 /def:".\wwwzip.def" /out:"$(OUTDIR)/Release\wwwzip.dll"\
 /implib:"$(OUTDIR)/wwwzip.lib" 
DEF_FILE= \
	".\wwwzip.def"
LINK32_OBJS= \
	"$(INTDIR)\HTZip.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwutils.lib" \
	"..\..\External\zlib.lib"

"$(OUTDIR)\Release\wwwzip.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wwwzip - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wwwzip\Debug"
# PROP BASE Intermediate_Dir "wwwzip\Debug"
# PROP BASE Target_Dir "wwwzip"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Bin"
# PROP Intermediate_Dir "wwwzip\Debug"
# PROP Target_Dir "wwwzip"
OUTDIR=.\../../../Bin
INTDIR=.\wwwzip\Debug

ALL : "wwwutils - Win32 Debug" "wwwstream - Win32 Debug" "wwwdll - Win32 Debug"\
 "wwwcore - Win32 Debug" "$(OUTDIR)\Debug\wwwzip.dll"

CLEAN : 
	-@erase "$(INTDIR)\HTZip.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Debug\wwwzip.dll"
	-@erase "$(OUTDIR)\Debug\wwwzip.ilk"
	-@erase "$(OUTDIR)\wwwzip.exp"
	-@erase "$(OUTDIR)\wwwzip.lib"
	-@erase "$(OUTDIR)\wwwzip.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\External" /D "_DEBUG" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\External" /D "_DEBUG" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/wwwzip.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wwwzip\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wwwzip.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/wwwzip.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/wwwzip.pdb"\
 /debug /machine:I386 /def:".\wwwzip.def" /out:"$(OUTDIR)/Debug\wwwzip.dll"\
 /implib:"$(OUTDIR)/wwwzip.lib" 
DEF_FILE= \
	".\wwwzip.def"
LINK32_OBJS= \
	"$(INTDIR)\HTZip.obj" \
	"$(OUTDIR)\wwwcore.lib" \
	"$(OUTDIR)\wwwdll.lib" \
	"$(OUTDIR)\wwwstream.lib" \
	"$(OUTDIR)\wwwutils.lib" \
	"..\..\External\zlib.lib"

"$(OUTDIR)\Debug\wwwzip.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "msvc - Win32 Release"
# Name "msvc - Win32 Debug"

!IF  "$(CFG)" == "msvc - Win32 Release"

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcache"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwcache - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcache - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwcache - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcache - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwmime"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwmime - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwmime - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwmime - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwmime - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwhttp"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwhttp - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhttp - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwhttp - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhttp - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwnews"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwnews - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwnews - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwnews - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwnews - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwgophe"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwgophe - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwgophe - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwgophe - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwgophe - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwftp"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwftp - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwftp - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwftp - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwftp - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdir"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwdir - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdir - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwdir - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdir - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwfile"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwfile - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwfile - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwfile - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwfile - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtelnt"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwtelnt - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtelnt - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwtelnt - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtelnt - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwhtml"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwhtml - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhtml - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwhtml - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhtml - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwapp"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwapp - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwapp - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwapp - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwapp - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwwais"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwwais - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwwais - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwwais - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwwais - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\windll.c

!IF  "$(CFG)" == "msvc - Win32 Release"


"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"


"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\windll.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtrans"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwtrans - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwtrans - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwstream"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwstream - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwstream - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\wwwdll.c

!IF  "$(CFG)" == "msvc - Win32 Release"


"$(INTDIR)\wwwdll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"


"$(INTDIR)\wwwdll.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\wwwdll.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwmux"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwmux - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwmux - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwmux - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwmux - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwzip"

!IF  "$(CFG)" == "msvc - Win32 Release"

"wwwzip - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwzip - Win32 Release" 

!ELSEIF  "$(CFG)" == "msvc - Win32 Debug"

"wwwzip - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwzip - Win32 Debug" 

!ENDIF 

# End Project Dependency
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

!IF  "$(CFG)" == "wwwdll - Win32 Release"


"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "wwwdll - Win32 Debug"


"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwdll.c

!IF  "$(CFG)" == "wwwdll - Win32 Release"


"$(INTDIR)\wwwdll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "wwwdll - Win32 Debug"


"$(INTDIR)\wwwdll.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

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
	".\..\HTMemory.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTUU_=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTUU.obj" : $(SOURCE) $(DEP_CPP_HTUU_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAssoc.c
DEP_CPP_HTASS=\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTASS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAssoc.obj" : $(SOURCE) $(DEP_CPP_HTASS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAtom.c
DEP_CPP_HTATO=\
	".\..\HTArray.h"\
	".\..\HTAtom.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTATO=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAtom.obj" : $(SOURCE) $(DEP_CPP_HTATO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTChunk.c
DEP_CPP_HTCHU=\
	".\..\HTChunk.h"\
	".\..\HTMemory.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTCHU=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTChunk.obj" : $(SOURCE) $(DEP_CPP_HTCHU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTList.c
DEP_CPP_HTLIS=\
	".\..\HTArray.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTLIS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTList.obj" : $(SOURCE) $(DEP_CPP_HTLIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTString.c
DEP_CPP_HTSTR=\
	".\..\HTMemory.h"\
	".\..\HTString.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTSTR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTString.obj" : $(SOURCE) $(DEP_CPP_HTSTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTArray.c
DEP_CPP_HTARR=\
	".\..\HTArray.h"\
	".\..\HTMemory.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
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

SOURCE=..\HTMemory.c
DEP_CPP_HTMEM=\
	".\..\HTArray.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMEM=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMemory.obj" : $(SOURCE) $(DEP_CPP_HTMEM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwutils - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwutils - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=..\HTTrace.c
DEP_CPP_HTTRA=\
	".\..\HTMemory.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTRA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTrace.obj" : $(SOURCE) $(DEP_CPP_HTTRA) "$(INTDIR)"
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

SOURCE=.\windll.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"


"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"


"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


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

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTWWW=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTWWW=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTWWWStr.obj" : $(SOURCE) $(DEP_CPP_HTWWW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTWWW=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTWWW=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTWWWStr.obj" : $(SOURCE) $(DEP_CPP_HTWWW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTReqMan.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTREQ=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTREQ=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTReqMan.obj" : $(SOURCE) $(DEP_CPP_HTREQ) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTREQ=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTREQ=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTReqMan.obj" : $(SOURCE) $(DEP_CPP_HTREQ) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTProt.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTPRO=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTPRO=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTProt.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTPRO=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTPRO=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTProt.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTParse.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTPAR=\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTEscape.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTParse.h"\
	".\..\HTString.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTPAR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTParse.obj" : $(SOURCE) $(DEP_CPP_HTPAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTPAR=\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTEscape.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTParse.h"\
	".\..\HTString.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTPAR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTParse.obj" : $(SOURCE) $(DEP_CPP_HTPAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNet.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTNET=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTNET=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNet.obj" : $(SOURCE) $(DEP_CPP_HTNET) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTNET=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTNET=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNet.obj" : $(SOURCE) $(DEP_CPP_HTNET) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMethod.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTMET=\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTString.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMET=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMethod.obj" : $(SOURCE) $(DEP_CPP_HTMET) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTMET=\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTString.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMET=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMethod.obj" : $(SOURCE) $(DEP_CPP_HTMET) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFWrite.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTFWR=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTFWR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFWrite.obj" : $(SOURCE) $(DEP_CPP_HTFWR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTFWR=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTFWR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFWrite.obj" : $(SOURCE) $(DEP_CPP_HTFWR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFormat.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTFOR=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTFOR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFormat.obj" : $(SOURCE) $(DEP_CPP_HTFOR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTFOR=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTFOR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFormat.obj" : $(SOURCE) $(DEP_CPP_HTFOR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEscape.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTESC=\
	".\..\HTEscape.h"\
	".\..\HTMemory.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTESC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTEscape.obj" : $(SOURCE) $(DEP_CPP_HTESC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTESC=\
	".\..\HTEscape.h"\
	".\..\HTMemory.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTESC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTEscape.obj" : $(SOURCE) $(DEP_CPP_HTESC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTError.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTERR=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTERR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTError.obj" : $(SOURCE) $(DEP_CPP_HTERR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTERR=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTERR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTError.obj" : $(SOURCE) $(DEP_CPP_HTERR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDNS.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTDNS=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTDNS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTDNS.obj" : $(SOURCE) $(DEP_CPP_HTDNS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTDNS=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTDNS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTDNS.obj" : $(SOURCE) $(DEP_CPP_HTDNS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBind.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTBIN=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTBIN=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTBind.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTBIN=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTBIN=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTBind.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAnchor.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTANC=\
	".\..\HTAnchor.h"\
	".\..\HTAncMan.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTANC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAnchor.obj" : $(SOURCE) $(DEP_CPP_HTANC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTANC=\
	".\..\HTAnchor.h"\
	".\..\HTAncMan.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTANC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAnchor.obj" : $(SOURCE) $(DEP_CPP_HTANC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAlert.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTALE=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTALE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAlert.obj" : $(SOURCE) $(DEP_CPP_HTALE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTALE=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTALE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAlert.obj" : $(SOURCE) $(DEP_CPP_HTALE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTInet.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTINE=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTINE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTInet.obj" : $(SOURCE) $(DEP_CPP_HTINE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTINE=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTINE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTInet.obj" : $(SOURCE) $(DEP_CPP_HTINE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTHost.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTHOS=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTCP.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTHOS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTHost.obj" : $(SOURCE) $(DEP_CPP_HTHOS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTHOS=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTCP.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTHOS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTHost.obj" : $(SOURCE) $(DEP_CPP_HTHOS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTrans.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTTRAN=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTRAN=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTrans.obj" : $(SOURCE) $(DEP_CPP_HTTRAN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTTRAN=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTRAN=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTrans.obj" : $(SOURCE) $(DEP_CPP_HTTRAN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTChannl.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTCHA=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTCHA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTChannl.obj" : $(SOURCE) $(DEP_CPP_HTCHA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTCHA=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTCHA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTChannl.obj" : $(SOURCE) $(DEP_CPP_HTCHA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTStream.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTSTRE=\
	".\..\HTArray.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTStream.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTSTRE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTStream.obj" : $(SOURCE) $(DEP_CPP_HTSTRE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTSTRE=\
	".\..\HTArray.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTStream.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTSTRE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTStream.obj" : $(SOURCE) $(DEP_CPP_HTSTRE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEvent.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTEVE=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTEVE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTEvent.obj" : $(SOURCE) $(DEP_CPP_HTEVE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTEVE=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTEVE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTEvent.obj" : $(SOURCE) $(DEP_CPP_HTEVE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTUser.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTUSE=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTUSE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTUser.obj" : $(SOURCE) $(DEP_CPP_HTUSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTUSE=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTUSE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTUser.obj" : $(SOURCE) $(DEP_CPP_HTUSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTLib.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTLIB=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTLIB=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTLib.obj" : $(SOURCE) $(DEP_CPP_HTLIB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTLIB=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTLIB=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTLib.obj" : $(SOURCE) $(DEP_CPP_HTLIB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTLink.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTLIN=\
	".\..\HTAnchor.h"\
	".\..\HTAncMan.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTLIN=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTLink.obj" : $(SOURCE) $(DEP_CPP_HTLIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTLIN=\
	".\..\HTAnchor.h"\
	".\..\HTAncMan.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTLIN=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTLink.obj" : $(SOURCE) $(DEP_CPP_HTLIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTUTree.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTUTR=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTUTR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTUTree.obj" : $(SOURCE) $(DEP_CPP_HTUTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTUTR=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTUTR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTUTree.obj" : $(SOURCE) $(DEP_CPP_HTUTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwcore - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwcore - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=..\HTResponse.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTRES=\
	".\..\HTAABrow.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTRES=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTResponse.obj" : $(SOURCE) $(DEP_CPP_HTRES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTRES=\
	".\..\HTAABrow.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTRES=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTResponse.obj" : $(SOURCE) $(DEP_CPP_HTRES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTCP.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTTCP=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDemux.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTMux.h"\
	".\..\HTMuxCh.h"\
	".\..\HTMuxHeader.h"\
	".\..\HTMuxTx.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWMux.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTCP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTCP.obj" : $(SOURCE) $(DEP_CPP_HTTCP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTTCP=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDemux.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTMux.h"\
	".\..\HTMuxCh.h"\
	".\..\HTMuxHeader.h"\
	".\..\HTMuxTx.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWMux.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTCP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTCP.obj" : $(SOURCE) $(DEP_CPP_HTTCP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTimer.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTTIM=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTimer.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTIM=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTimer.obj" : $(SOURCE) $(DEP_CPP_HTTIM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTTIM=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTimer.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTIM=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTimer.obj" : $(SOURCE) $(DEP_CPP_HTTIM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMemLog.c

!IF  "$(CFG)" == "wwwcore - Win32 Release"

DEP_CPP_HTMEML=\
	"..\HTMemLog.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTimer.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMEML=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMemLog.obj" : $(SOURCE) $(DEP_CPP_HTMEML) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwcore - Win32 Debug"

DEP_CPP_HTMEML=\
	"..\HTMemLog.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTimer.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMEML=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMemLog.obj" : $(SOURCE) $(DEP_CPP_HTMEML) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

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

SOURCE=.\wwwcache.def

!IF  "$(CFG)" == "wwwcache - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTCache.c
DEP_CPP_HTCAC=\
	".\..\HTAccess.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBufWrt.h"\
	".\..\HTCache.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDialog.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTEvtLst.h"\
	".\..\HTFilter.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHist.h"\
	".\..\HTHome.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTLog.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTProxy.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTRules.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWApp.h"\
	".\..\WWWCore.h"\
	".\..\WWWLib.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTCAC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTCache.obj" : $(SOURCE) $(DEP_CPP_HTCAC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwcache - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwcache - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwcache - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtrans"

!IF  "$(CFG)" == "wwwcache - Win32 Release"

"wwwtrans - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwcache - Win32 Debug"

"wwwtrans - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Debug" 

!ENDIF 

# End Project Dependency
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

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMIMERq.c
DEP_CPP_HTMIM=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTAncMan.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMERq.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMIM=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMERq.obj" : $(SOURCE) $(DEP_CPP_HTMIM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTHeader.c
DEP_CPP_HTHEA=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTHEA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTHeader.obj" : $(SOURCE) $(DEP_CPP_HTHEA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMIME.c
DEP_CPP_HTMIME=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTCache.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTConLen.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTGuess.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMerge.h"\
	".\..\HTMethod.h"\
	".\..\HTMIME.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTSChunk.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTee.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\HTXParse.h"\
	".\..\sysdep.h"\
	".\..\WWWCache.h"\
	".\..\WWWCore.h"\
	".\..\WWWLib.h"\
	".\..\WWWStream.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMIME=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIME.obj" : $(SOURCE) $(DEP_CPP_HTMIME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBound.c
DEP_CPP_HTBOU=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBound.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTBOU=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTBound.obj" : $(SOURCE) $(DEP_CPP_HTBOU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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

SOURCE=..\HTMIMPrs.c
DEP_CPP_HTMIMP=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMIMP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMPrs.obj" : $(SOURCE) $(DEP_CPP_HTMIMP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMIMImp.c
DEP_CPP_HTMIMI=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMImp.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMIMI=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMIMImp.obj" : $(SOURCE) $(DEP_CPP_HTMIMI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwstream"

!IF  "$(CFG)" == "wwwmime - Win32 Release"

"wwwstream - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

"wwwstream - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwmime - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwmime - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwmime - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcache"

!IF  "$(CFG)" == "wwwmime - Win32 Release"

"wwwcache - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcache - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwmime - Win32 Debug"

"wwwcache - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcache - Win32 Debug" 

!ENDIF 

# End Project Dependency
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

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTPServ.c
DEP_CPP_HTTPS=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBound.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTConLen.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTGuess.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMerge.h"\
	".\..\HTMethod.h"\
	".\..\HTMIME.h"\
	".\..\HTMIMERq.h"\
	".\..\HTMIMImp.h"\
	".\..\HTMulpar.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTSChunk.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTee.h"\
	".\..\HTTPRes.h"\
	".\..\HTTPServ.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\HTXParse.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWMIME.h"\
	".\..\WWWStream.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTPS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPServ.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAAUtil.c
DEP_CPP_HTAAU=\
	".\..\HTAAUtil.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTAAU=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAAUtil.obj" : $(SOURCE) $(DEP_CPP_HTAAU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTP.c
DEP_CPP_HTTP_=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBound.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTConLen.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTGuess.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMerge.h"\
	".\..\HTMethod.h"\
	".\..\HTMIME.h"\
	".\..\HTMIMERq.h"\
	".\..\HTMIMImp.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTMulpar.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTSChunk.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTee.h"\
	".\..\HTTP.h"\
	".\..\HTTPReq.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\HTXParse.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWMIME.h"\
	".\..\WWWStream.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTP_=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTP.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTPReq.c
DEP_CPP_HTTPR=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPReq.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTPR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPReq.obj" : $(SOURCE) $(DEP_CPP_HTTPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTPRes.c
DEP_CPP_HTTPRE=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPRes.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTPRE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPRes.obj" : $(SOURCE) $(DEP_CPP_HTTPRE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAABrow.c
DEP_CPP_HTAAB=\
	".\..\HTAABrow.h"\
	".\..\HTAAUtil.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTAAB=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAABrow.obj" : $(SOURCE) $(DEP_CPP_HTAAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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

SOURCE=..\HTTPGen.c
DEP_CPP_HTTPG=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBound.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIME.h"\
	".\..\HTMIMERq.h"\
	".\..\HTMIMImp.h"\
	".\..\HTMulpar.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTPReq.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWMIME.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTPG=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTPGen.obj" : $(SOURCE) $(DEP_CPP_HTTPG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwstream"

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

"wwwstream - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

"wwwstream - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtrans"

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

"wwwtrans - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

"wwwtrans - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=..\HTTChunk.c
DEP_CPP_HTTCH=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTChunk.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTCH=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTChunk.obj" : $(SOURCE) $(DEP_CPP_HTTCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTPEP.c
DEP_CPP_HTPEP=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTPEP.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTPEP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTPEP.obj" : $(SOURCE) $(DEP_CPP_HTPEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcache"

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

"wwwcache - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcache - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

"wwwcache - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcache - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwmime"

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

"wwwmime - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwmime - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

"wwwmime - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwmime - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwhttp - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhttp - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
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

!IF  "$(CFG)" == "wwwnews - Win32 Release"

DEP_CPP_HTNEW=\
	".\..\HTAABrow.h"\
	".\..\HTAccess.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMERq.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTNewsRq.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	

"$(INTDIR)\HTNewsRq.obj" : $(SOURCE) $(DEP_CPP_HTNEW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

DEP_CPP_HTNEW=\
	".\..\HTAABrow.h"\
	".\..\HTAccess.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMERq.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTNewsRq.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTNEW=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNewsRq.obj" : $(SOURCE) $(DEP_CPP_HTNEW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNews.c

!IF  "$(CFG)" == "wwwnews - Win32 Release"

DEP_CPP_HTNEWS=\
	".\..\HTAABrow.h"\
	".\..\HTAccess.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTConLen.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTGuess.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMerge.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTNews.h"\
	".\..\HTNewsLs.h"\
	".\..\HTNewsRq.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTSChunk.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTee.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\HTXParse.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWStream.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	

"$(INTDIR)\HTNews.obj" : $(SOURCE) $(DEP_CPP_HTNEWS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

DEP_CPP_HTNEWS=\
	".\..\HTAABrow.h"\
	".\..\HTAccess.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTConLen.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTGuess.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMerge.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTNews.h"\
	".\..\HTNewsLs.h"\
	".\..\HTNewsRq.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTSChunk.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTee.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\HTXParse.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWStream.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTNEWS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNews.obj" : $(SOURCE) $(DEP_CPP_HTNEWS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNewsLs.c

!IF  "$(CFG)" == "wwwnews - Win32 Release"

DEP_CPP_HTNEWSL=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNDir.h"\
	".\..\HTNet.h"\
	".\..\HTNews.h"\
	".\..\HTNewsLs.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	

"$(INTDIR)\HTNewsLs.obj" : $(SOURCE) $(DEP_CPP_HTNEWSL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

DEP_CPP_HTNEWSL=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNDir.h"\
	".\..\HTNet.h"\
	".\..\HTNews.h"\
	".\..\HTNewsLs.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTNEWSL=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNewsLs.obj" : $(SOURCE) $(DEP_CPP_HTNEWSL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNDir.c

!IF  "$(CFG)" == "wwwnews - Win32 Release"

DEP_CPP_HTNDI=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMLGen.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTNDir.h"\
	".\..\HTNet.h"\
	".\..\HTNews.h"\
	".\..\HTNewsLs.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTeXGen.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\SGML.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWHTML.h"\
	".\..\WWWUtil.h"\
	

"$(INTDIR)\HTNDir.obj" : $(SOURCE) $(DEP_CPP_HTNDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

DEP_CPP_HTNDI=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMLGen.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTNDir.h"\
	".\..\HTNet.h"\
	".\..\HTNews.h"\
	".\..\HTNewsLs.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTeXGen.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\SGML.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWHTML.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTNDI=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTNDir.obj" : $(SOURCE) $(DEP_CPP_HTNDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwhtml"

!IF  "$(CFG)" == "wwwnews - Win32 Release"

"wwwhtml - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhtml - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

"wwwhtml - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhtml - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwnews - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwnews - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwmime"

!IF  "$(CFG)" == "wwwnews - Win32 Release"

"wwwmime - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwmime - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

"wwwmime - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwmime - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwstream"

!IF  "$(CFG)" == "wwwnews - Win32 Release"

"wwwstream - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

"wwwstream - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtrans"

!IF  "$(CFG)" == "wwwnews - Win32 Release"

"wwwtrans - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

"wwwtrans - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwnews - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwnews - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
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

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTGopher.c
DEP_CPP_HTGOP=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDescpt.h"\
	".\..\HTDir.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTGopher.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIcons.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMLGen.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTeXGen.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\SGML.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWDir.h"\
	".\..\WWWHTML.h"\
	".\..\WWWLib.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
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
# Begin Project Dependency

# Project_Dep_Name "wwwdir"

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

"wwwdir - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdir - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

"wwwdir - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdir - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwhtml"

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

"wwwhtml - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhtml - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

"wwwhtml - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhtml - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtrans"

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

"wwwtrans - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

"wwwtrans - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwgophe - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwgophe - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
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

SOURCE=..\HTFTPDir.c
DEP_CPP_HTFTP=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDescpt.h"\
	".\..\HTDir.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFTP.h"\
	".\..\HTFTPDir.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIcons.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWDir.h"\
	".\..\WWWLib.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTFTP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFTPDir.obj" : $(SOURCE) $(DEP_CPP_HTFTP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFTP.c
DEP_CPP_HTFTP_=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFTP.h"\
	".\..\HTFTPDir.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTFTP_=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFTP.obj" : $(SOURCE) $(DEP_CPP_HTFTP_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwftp - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdir"

!IF  "$(CFG)" == "wwwftp - Win32 Release"

"wwwdir - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdir - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

"wwwdir - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdir - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwftp - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtrans"

!IF  "$(CFG)" == "wwwftp - Win32 Release"

"wwwtrans - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

"wwwtrans - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwftp - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwftp - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
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
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIcons.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTICO=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTIcons.obj" : $(SOURCE) $(DEP_CPP_HTICO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDir.c
DEP_CPP_HTDIR=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDescpt.h"\
	".\..\HTDir.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIcons.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMLGen.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTeXGen.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\SGML.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWHTML.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTDIR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTDir.obj" : $(SOURCE) $(DEP_CPP_HTDIR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDescpt.c
DEP_CPP_HTDES=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTDES=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTDescpt.obj" : $(SOURCE) $(DEP_CPP_HTDES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwdir - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwdir - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwhtml"

!IF  "$(CFG)" == "wwwdir - Win32 Release"

"wwwhtml - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhtml - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

"wwwhtml - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhtml - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwdir - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwdir - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
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
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFile.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMulti.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMUL=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMulti.obj" : $(SOURCE) $(DEP_CPP_HTMUL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFile.c
DEP_CPP_HTFIL=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDescpt.h"\
	".\..\HTDir.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFile.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIcons.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTMulti.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWDir.h"\
	".\..\WWWLib.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTFIL=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFile.obj" : $(SOURCE) $(DEP_CPP_HTFIL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwfile - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdir"

!IF  "$(CFG)" == "wwwfile - Win32 Release"

"wwwdir - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdir - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

"wwwdir - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdir - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwfile - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtrans"

!IF  "$(CFG)" == "wwwfile - Win32 Release"

"wwwtrans - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

"wwwtrans - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwfile - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwfile - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
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
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTelnet.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTEL=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTelnet.obj" : $(SOURCE) $(DEP_CPP_HTTEL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwtelnt - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdir"

!IF  "$(CFG)" == "wwwtelnt - Win32 Release"

"wwwdir - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdir - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

"wwwdir - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdir - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwtelnt - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwtelnt - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwtelnt - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
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
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMLGen.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\SGML.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMLG=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMLGen.obj" : $(SOURCE) $(DEP_CPP_HTMLG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMLPDTD.c
DEP_CPP_HTMLP=\
	".\..\HTArray.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTStream.h"\
	".\..\HTStruct.h"\
	".\..\HTUtils.h"\
	".\..\SGML.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
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

SOURCE=..\HTTeXGen.c
DEP_CPP_HTTEX=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTeXGen.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\SGML.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTEX=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTeXGen.obj" : $(SOURCE) $(DEP_CPP_HTTEX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\SGML.c
DEP_CPP_SGML_=\
	".\..\HTArray.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTUtils.h"\
	".\..\SGML.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
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
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwhtml - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhtml - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwhtml - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhtml - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwhtml - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwhtml - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
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
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLog.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTLOG=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTLog.obj" : $(SOURCE) $(DEP_CPP_HTLOG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTHome.c
DEP_CPP_HTHOM=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHome.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTHOM=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTHome.obj" : $(SOURCE) $(DEP_CPP_HTHOM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTHist.c
DEP_CPP_HTHIS=\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHist.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTHIS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTHist.obj" : $(SOURCE) $(DEP_CPP_HTHIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDialog.c
DEP_CPP_HTDIA=\
	".\..\HTAABrow.h"\
	".\..\HTAAUtil.h"\
	".\..\HTAccess.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDialog.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTEvtLst.h"\
	".\..\HTFilter.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHist.h"\
	".\..\HTHome.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLog.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTPEP.h"\
	".\..\HTProt.h"\
	".\..\HTProxy.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTRules.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTChunk.h"\
	".\..\HTTP.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPReq.h"\
	".\..\HTTPRes.h"\
	".\..\HTTPServ.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWApp.h"\
	".\..\WWWCore.h"\
	".\..\WWWHTTP.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTDIA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTDialog.obj" : $(SOURCE) $(DEP_CPP_HTDIA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAccess.c
DEP_CPP_HTACC=\
	".\..\HTAABrow.h"\
	".\..\HTAccess.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTConLen.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTGuess.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMerge.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTProxy.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTRules.h"\
	".\..\HTSChunk.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTee.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\HTXParse.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWStream.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTACC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTAccess.obj" : $(SOURCE) $(DEP_CPP_HTACC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFilter.c
DEP_CPP_HTFILT=\
	".\..\HTAABrow.h"\
	".\..\HTAAUtil.h"\
	".\..\HTAccess.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTCache.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFilter.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLog.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTPEP.h"\
	".\..\HTProt.h"\
	".\..\HTProxy.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTRules.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTChunk.h"\
	".\..\HTTP.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPReq.h"\
	".\..\HTTPRes.h"\
	".\..\HTTPServ.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCache.h"\
	".\..\WWWCore.h"\
	".\..\WWWHTTP.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTFILT=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTFilter.obj" : $(SOURCE) $(DEP_CPP_HTFILT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTRules.c
DEP_CPP_HTRUL=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTProxy.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTRules.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTRUL=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTRules.obj" : $(SOURCE) $(DEP_CPP_HTRUL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTProxy.c
DEP_CPP_HTPROX=\
	".\..\HTAABrow.h"\
	".\..\HTAAUtil.h"\
	".\..\HTAccess.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDialog.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTEvtLst.h"\
	".\..\HTFilter.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHist.h"\
	".\..\HTHome.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLog.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTPEP.h"\
	".\..\HTProt.h"\
	".\..\HTProxy.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTRules.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTChunk.h"\
	".\..\HTTP.h"\
	".\..\HTTPGen.h"\
	".\..\HTTPReq.h"\
	".\..\HTTPRes.h"\
	".\..\HTTPServ.h"\
	".\..\HTTPUtil.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWApp.h"\
	".\..\WWWCore.h"\
	".\..\WWWHTTP.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTPROX=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTProxy.obj" : $(SOURCE) $(DEP_CPP_HTPROX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcache"

!IF  "$(CFG)" == "wwwapp - Win32 Release"

"wwwcache - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcache - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

"wwwcache - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcache - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwapp - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwapp - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwhttp"

!IF  "$(CFG)" == "wwwapp - Win32 Release"

"wwwhttp - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhttp - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

"wwwhttp - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhttp - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwstream"

!IF  "$(CFG)" == "wwwapp - Win32 Release"

"wwwstream - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

"wwwstream - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwapp - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwapp - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=..\HTEvtLst.c
DEP_CPP_HTEVT=\
	".\..\HTAABrow.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTEvtLst.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTimer.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTEVT=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTEvtLst.obj" : $(SOURCE) $(DEP_CPP_HTEVT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
DEP_CPP_HTWSR=\
	".\..\HTAABrow.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHeader.h"\
	".\..\HTHost.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMIMPrs.h"\
	".\..\HTMLGen.h"\
	".\..\HTMLPDTD.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTProxy.h"\
	".\..\HTReq.h"\
	".\..\HTReqMan.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWSRC.h"\
	".\..\SGML.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTWSR=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTWSRC.obj" : $(SOURCE) $(DEP_CPP_HTWSR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwapp"

!IF  "$(CFG)" == "wwwwais - Win32 Release"

"wwwapp - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwapp - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

"wwwapp - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwapp - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwwais - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwwais - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwhtml"

!IF  "$(CFG)" == "wwwwais - Win32 Release"

"wwwhtml - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhtml - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

"wwwhtml - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwhtml - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwwais - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwwais - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
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
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTANS=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTANSI.obj" : $(SOURCE) $(DEP_CPP_HTANS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTLocal.c
DEP_CPP_HTLOC=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTIOStream.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTLOC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTLocal.obj" : $(SOURCE) $(DEP_CPP_HTLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTReader.c
DEP_CPP_HTREA=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTREA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTReader.obj" : $(SOURCE) $(DEP_CPP_HTREA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTSocket.c
DEP_CPP_HTSOC=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTSOC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTSocket.obj" : $(SOURCE) $(DEP_CPP_HTSOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTWriter.c
DEP_CPP_HTWRI=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTWRI=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTWriter.obj" : $(SOURCE) $(DEP_CPP_HTWRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBufWrt.c
DEP_CPP_HTBUF=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTimer.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTBUF=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTBufWrt.obj" : $(SOURCE) $(DEP_CPP_HTBUF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwtrans - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwtrans - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwtrans - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwtrans - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
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
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTConLen.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTCON=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTConLen.obj" : $(SOURCE) $(DEP_CPP_HTCON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTee.c
DEP_CPP_HTTEE=\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTTee.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTTEE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTTee.obj" : $(SOURCE) $(DEP_CPP_HTTEE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTGuess.c
DEP_CPP_HTGUE=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTGuess.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWLib.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTGUE=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTGuess.obj" : $(SOURCE) $(DEP_CPP_HTGUE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEPtoCl.c
DEP_CPP_HTEPT=\
	".\..\HTArray.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTStream.h"\
	".\..\HTUtils.h"\
	".\..\sysdep.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTEPT=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTEPtoCl.obj" : $(SOURCE) $(DEP_CPP_HTEPT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTXParse.c
DEP_CPP_HTXPA=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTEPtoCl.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\HTXParse.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTXPA=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTXParse.obj" : $(SOURCE) $(DEP_CPP_HTXPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTSChunk.c
DEP_CPP_HTSCH=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTSChunk.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTSCH=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTSChunk.obj" : $(SOURCE) $(DEP_CPP_HTSCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwstream - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwstream - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwstream - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwstream - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwstream - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwstream - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=..\HTMerge.c
DEP_CPP_HTMER=\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTChunk.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMerge.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTUtils.h"\
	".\..\HTUU.h"\
	".\..\sysdep.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMER=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMerge.obj" : $(SOURCE) $(DEP_CPP_HTMER) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "wwwmux - Win32 Release"
# Name "wwwmux - Win32 Debug"

!IF  "$(CFG)" == "wwwmux - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwmux - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=..\HTDemux.c
DEP_CPP_HTDEM=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDemux.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMuxCh.h"\
	".\..\HTMuxHeader.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTDEM=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTDemux.obj" : $(SOURCE) $(DEP_CPP_HTDEM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMuxTx.c
DEP_CPP_HTMUX=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMuxCh.h"\
	".\..\HTMuxHeader.h"\
	".\..\HTMuxTx.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMUX=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMuxTx.obj" : $(SOURCE) $(DEP_CPP_HTMUX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMuxCh.c
DEP_CPP_HTMUXC=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTANSI.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTBufWrt.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTConLen.h"\
	".\..\HTDemux.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTGuess.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTLocal.h"\
	".\..\HTMemory.h"\
	".\..\HTMerge.h"\
	".\..\HTMethod.h"\
	".\..\HTMuxCh.h"\
	".\..\HTMuxHeader.h"\
	".\..\HTMuxTx.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReader.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTSChunk.h"\
	".\..\HTSocket.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTCP.h"\
	".\..\HTTee.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWriter.h"\
	".\..\HTWWWStr.h"\
	".\..\HTXParse.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWStream.h"\
	".\..\WWWTrans.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMUXC=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMuxCh.obj" : $(SOURCE) $(DEP_CPP_HTMUXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMux.c
DEP_CPP_HTMUX_=\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTMux.h"\
	".\..\HTMuxCh.h"\
	".\..\HTMuxHeader.h"\
	".\..\HTNet.h"\
	".\..\HTNetMan.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMUX_=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTMux.obj" : $(SOURCE) $(DEP_CPP_HTMUX_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwmux.def

!IF  "$(CFG)" == "wwwmux - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwmux - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwmux - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwmux - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwmux - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwmux - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwmux - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwmux - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwstream"

!IF  "$(CFG)" == "wwwmux - Win32 Release"

"wwwstream - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwmux - Win32 Debug"

"wwwstream - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwtrans"

!IF  "$(CFG)" == "wwwmux - Win32 Release"

"wwwtrans - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwmux - Win32 Debug"

"wwwtrans - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwtrans - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "wwwzip - Win32 Release"
# Name "wwwzip - Win32 Debug"

!IF  "$(CFG)" == "wwwzip - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwzip - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=..\HTZip.c

!IF  "$(CFG)" == "wwwzip - Win32 Release"

DEP_CPP_HTZIP=\
	"..\..\External\zconf.h"\
	"..\..\External\zlib.h"\
	"..\HTZip.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	".\config.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTZIP=\
	".\..\HTVMSUtils.h"\
	

"$(INTDIR)\HTZip.obj" : $(SOURCE) $(DEP_CPP_HTZIP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "wwwzip - Win32 Debug"

DEP_CPP_HTZIP=\
	"..\..\External\zconf.h"\
	"..\..\External\zlib.h"\
	"..\HTZip.h"\
	".\..\HTAlert.h"\
	".\..\HTAnchor.h"\
	".\..\HTArray.h"\
	".\..\HTAssoc.h"\
	".\..\HTAtom.h"\
	".\..\HTBind.h"\
	".\..\HTChannl.h"\
	".\..\HTChunk.h"\
	".\..\HTDNS.h"\
	".\..\HTError.h"\
	".\..\HTEscape.h"\
	".\..\HTEvent.h"\
	".\..\HTFormat.h"\
	".\..\HTFWrite.h"\
	".\..\HTHost.h"\
	".\..\HTHstMan.h"\
	".\..\HTInet.h"\
	".\..\HTIOStream.h"\
	".\..\HTLib.h"\
	".\..\HTLink.h"\
	".\..\HTList.h"\
	".\..\HTMemory.h"\
	".\..\HTMethod.h"\
	".\..\HTNet.h"\
	".\..\HTParse.h"\
	".\..\HTProt.h"\
	".\..\HTReq.h"\
	".\..\HTResponse.h"\
	".\..\HTStream.h"\
	".\..\HTString.h"\
	".\..\HTStruct.h"\
	".\..\HTTrans.h"\
	".\..\HTUser.h"\
	".\..\HTUtils.h"\
	".\..\HTUTree.h"\
	".\..\HTUU.h"\
	".\..\HTWWWStr.h"\
	".\..\sysdep.h"\
	".\..\WWWCore.h"\
	".\..\WWWUtil.h"\
	

"$(INTDIR)\HTZip.obj" : $(SOURCE) $(DEP_CPP_HTZIP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwzip.def

!IF  "$(CFG)" == "wwwzip - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwzip - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwcore"

!IF  "$(CFG)" == "wwwzip - Win32 Release"

"wwwcore - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwzip - Win32 Debug"

"wwwcore - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwcore - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwdll"

!IF  "$(CFG)" == "wwwzip - Win32 Release"

"wwwdll - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwzip - Win32 Debug"

"wwwdll - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwdll - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwstream"

!IF  "$(CFG)" == "wwwzip - Win32 Release"

"wwwstream - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwzip - Win32 Debug"

"wwwstream - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwstream - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "wwwutils"

!IF  "$(CFG)" == "wwwzip - Win32 Release"

"wwwutils - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Release" 

!ELSEIF  "$(CFG)" == "wwwzip - Win32 Debug"

"wwwutils - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\libwww.mak" CFG="wwwutils - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=..\..\External\zlib.lib

!IF  "$(CFG)" == "wwwzip - Win32 Release"

!ELSEIF  "$(CFG)" == "wwwzip - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
