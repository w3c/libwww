# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "wwwtelnt.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\wwwtelnt.dll .\WinRel\wwwtelnt.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /FR$(INTDIR)/ /Fp$(OUTDIR)/"wwwtelnt.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"wwwtelnt.bsc" 
BSC32_SBRS= \
	.\WinRel\windll.sbr \
	.\WinRel\HTTelnet.sbr

.\WinRel\wwwtelnt.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"wwwtelnt.pdb" /MACHINE:I386 /DEF:".\wwwtelnt.def"\
 /OUT:$(OUTDIR)/"wwwtelnt.dll" /IMPLIB:$(OUTDIR)/"wwwtelnt.lib" 
DEF_FILE=.\wwwtelnt.def
LINK32_OBJS= \
	.\WinDebug\wwwutils.lib \
	.\WinDebug\wwwcore.lib \
	.\WinDebug\wwwdll.lib \
	.\WinRel\windll.obj \
	.\WinDebug\wwwhtml.lib \
	.\WinDebug\wwwdir.lib \
	.\WinRel\HTTelnet.obj

.\WinRel\wwwtelnt.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\wwwtelnt.dll .\WinDebug\wwwtelnt.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /FR$(INTDIR)/ /Fp$(OUTDIR)/"wwwtelnt.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"wwwtelnt.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"wwwtelnt.bsc" 
BSC32_SBRS= \
	.\WinDebug\windll.sbr \
	.\WinDebug\HTTelnet.sbr

.\WinDebug\wwwtelnt.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"wwwtelnt.pdb" /DEBUG /MACHINE:I386 /DEF:".\wwwtelnt.def"\
 /OUT:$(OUTDIR)/"wwwtelnt.dll" /IMPLIB:$(OUTDIR)/"wwwtelnt.lib" 
DEF_FILE=.\wwwtelnt.def
LINK32_OBJS= \
	.\WinDebug\wwwutils.lib \
	.\WinDebug\wwwcore.lib \
	.\WinDebug\wwwdll.lib \
	.\WinDebug\windll.obj \
	.\WinDebug\wwwhtml.lib \
	.\WinDebug\wwwdir.lib \
	.\WinDebug\HTTelnet.obj

.\WinDebug\wwwtelnt.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\WinDebug\wwwutils.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\WinDebug\wwwcore.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\WinDebug\wwwdll.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\windll.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\windll.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\windll.obj :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WinDebug\wwwhtml.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\WinDebug\wwwdir.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTelnet.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTTelnet.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTTelnet.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwtelnt.def
# End Source File
# End Group
# End Project
################################################################################
