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
!MESSAGE NMAKE /f "wwwapp.mak" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/wwwapp.dll $(OUTDIR)/wwwapp.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /FR$(INTDIR)/ /Fp$(OUTDIR)/"wwwapp.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"wwwapp.bsc" 
BSC32_SBRS= \
	$(INTDIR)/windll.sbr \
	$(INTDIR)/HTHist.sbr \
	$(INTDIR)/HTLog.sbr \
	$(INTDIR)/HTHome.sbr \
	$(INTDIR)/HTDialog.sbr

$(OUTDIR)/wwwapp.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"wwwapp.pdb" /MACHINE:I386 /DEF:".\wwwapp.def"\
 /OUT:$(OUTDIR)/"wwwapp.dll" /IMPLIB:$(OUTDIR)/"wwwapp.lib" 
DEF_FILE=.\wwwapp.def
LINK32_OBJS= \
	.\WinDebug\wwwutils.lib \
	.\WinDebug\wwwcore.lib \
	.\WinDebug\wwwdll.lib \
	$(INTDIR)/windll.obj \
	.\WinDebug\wwwdir.lib \
	$(INTDIR)/HTHist.obj \
	$(INTDIR)/HTLog.obj \
	$(INTDIR)/HTHome.obj \
	.\WinDebug\wwwcache.lib \
	.\WinDebug\wwwrules.lib \
	$(INTDIR)/HTDialog.obj

$(OUTDIR)/wwwapp.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/wwwapp.dll $(OUTDIR)/wwwapp.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_DLL" /FR$(INTDIR)/ /Fp$(OUTDIR)/"wwwapp.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"wwwapp.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"wwwapp.bsc" 
BSC32_SBRS= \
	$(INTDIR)/windll.sbr \
	$(INTDIR)/HTHist.sbr \
	$(INTDIR)/HTLog.sbr \
	$(INTDIR)/HTHome.sbr \
	$(INTDIR)/HTDialog.sbr

$(OUTDIR)/wwwapp.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"wwwapp.pdb" /DEBUG /MACHINE:I386 /DEF:".\wwwapp.def"\
 /OUT:$(OUTDIR)/"wwwapp.dll" /IMPLIB:$(OUTDIR)/"wwwapp.lib" 
DEF_FILE=.\wwwapp.def
LINK32_OBJS= \
	$(INTDIR)/wwwutils.lib \
	$(INTDIR)/wwwcore.lib \
	$(INTDIR)/wwwdll.lib \
	$(INTDIR)/windll.obj \
	$(INTDIR)/wwwdir.lib \
	$(INTDIR)/HTHist.obj \
	$(INTDIR)/HTLog.obj \
	$(INTDIR)/HTHome.obj \
	$(INTDIR)/wwwcache.lib \
	$(INTDIR)/wwwrules.lib \
	$(INTDIR)/HTDialog.obj

$(OUTDIR)/wwwapp.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

$(INTDIR)/windll.obj :  $(SOURCE)  $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WinDebug\wwwdir.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTHist.c

$(INTDIR)/HTHist.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwapp.def
# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTLog.c

$(INTDIR)/HTLog.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTHome.c

$(INTDIR)/HTHome.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WinDebug\wwwcache.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\WinDebug\wwwrules.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDialog.c

$(INTDIR)/HTDialog.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
# End Group
# End Project
################################################################################
