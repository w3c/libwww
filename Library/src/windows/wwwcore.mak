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
!MESSAGE NMAKE /f "wwwcore.mak" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/wwwcore.dll $(OUTDIR)/wwwcore.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /FR$(INTDIR)/ /Fp$(OUTDIR)/"wwwcore.pch"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"wwwcore.bsc" 
BSC32_SBRS= \
	$(INTDIR)/windll.sbr \
	$(INTDIR)/HTError.sbr \
	$(INTDIR)/HTProt.sbr \
	$(INTDIR)/HTEvntrg.sbr \
	$(INTDIR)/HTParse.sbr \
	$(INTDIR)/HTReqMan.sbr \
	$(INTDIR)/HTNet.sbr \
	$(INTDIR)/HTTCP.sbr \
	$(INTDIR)/HTEscape.sbr \
	$(INTDIR)/HTDNS.sbr \
	$(INTDIR)/HTFWrite.sbr \
	$(INTDIR)/HTAccess.sbr \
	$(INTDIR)/HTFormat.sbr \
	$(INTDIR)/HTAlert.sbr \
	$(INTDIR)/HTBind.sbr \
	$(INTDIR)/HTAnchor.sbr \
	$(INTDIR)/HTWriter.sbr \
	$(INTDIR)/HTTee.sbr \
	$(INTDIR)/HTSocket.sbr \
	$(INTDIR)/HTMethod.sbr \
	$(INTDIR)/HTConLen.sbr \
	$(INTDIR)/HTWWWStr.sbr

$(OUTDIR)/wwwcore.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"wwwcore.pdb" /MACHINE:I386 /DEF:".\wwwcore.def"\
 /OUT:$(OUTDIR)/"wwwcore.dll" /IMPLIB:$(OUTDIR)/"wwwcore.lib" 
DEF_FILE=.\wwwcore.def
LINK32_OBJS= \
	$(INTDIR)/windll.obj \
	$(INTDIR)/HTError.obj \
	$(INTDIR)/HTProt.obj \
	$(INTDIR)/HTEvntrg.obj \
	.\WinDebug\wwwdll.lib \
	.\WinDebug\wwwutils.lib \
	$(INTDIR)/HTParse.obj \
	$(INTDIR)/HTReqMan.obj \
	$(INTDIR)/HTNet.obj \
	$(INTDIR)/HTTCP.obj \
	$(INTDIR)/HTEscape.obj \
	$(INTDIR)/HTDNS.obj \
	$(INTDIR)/HTFWrite.obj \
	$(INTDIR)/HTAccess.obj \
	$(INTDIR)/HTFormat.obj \
	$(INTDIR)/HTAlert.obj \
	$(INTDIR)/HTBind.obj \
	$(INTDIR)/HTAnchor.obj \
	$(INTDIR)/HTWriter.obj \
	$(INTDIR)/HTTee.obj \
	$(INTDIR)/HTSocket.obj \
	$(INTDIR)/HTMethod.obj \
	$(INTDIR)/HTConLen.obj \
	$(INTDIR)/HTWWWStr.obj \
	\MSVC20\LIB\WSOCK32.LIB

$(OUTDIR)/wwwcore.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/wwwcore.dll $(OUTDIR)/wwwcore.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"wwwcore.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"wwwcore.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"wwwcore.bsc" 
BSC32_SBRS= \
	$(INTDIR)/windll.sbr \
	$(INTDIR)/HTError.sbr \
	$(INTDIR)/HTProt.sbr \
	$(INTDIR)/HTEvntrg.sbr \
	$(INTDIR)/HTParse.sbr \
	$(INTDIR)/HTReqMan.sbr \
	$(INTDIR)/HTNet.sbr \
	$(INTDIR)/HTTCP.sbr \
	$(INTDIR)/HTEscape.sbr \
	$(INTDIR)/HTDNS.sbr \
	$(INTDIR)/HTFWrite.sbr \
	$(INTDIR)/HTAccess.sbr \
	$(INTDIR)/HTFormat.sbr \
	$(INTDIR)/HTAlert.sbr \
	$(INTDIR)/HTBind.sbr \
	$(INTDIR)/HTAnchor.sbr \
	$(INTDIR)/HTWriter.sbr \
	$(INTDIR)/HTTee.sbr \
	$(INTDIR)/HTSocket.sbr \
	$(INTDIR)/HTMethod.sbr \
	$(INTDIR)/HTConLen.sbr \
	$(INTDIR)/HTWWWStr.sbr

$(OUTDIR)/wwwcore.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DLL /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"wwwcore.pdb" /DEBUG /MACHINE:I386 /DEF:".\wwwcore.def"\
 /OUT:$(OUTDIR)/"wwwcore.dll" /IMPLIB:$(OUTDIR)/"wwwcore.lib" 
DEF_FILE=.\wwwcore.def
LINK32_OBJS= \
	$(INTDIR)/windll.obj \
	$(INTDIR)/HTError.obj \
	$(INTDIR)/HTProt.obj \
	$(INTDIR)/HTEvntrg.obj \
	$(INTDIR)/wwwdll.lib \
	$(INTDIR)/wwwutils.lib \
	$(INTDIR)/HTParse.obj \
	$(INTDIR)/HTReqMan.obj \
	$(INTDIR)/HTNet.obj \
	$(INTDIR)/HTTCP.obj \
	$(INTDIR)/HTEscape.obj \
	$(INTDIR)/HTDNS.obj \
	$(INTDIR)/HTFWrite.obj \
	$(INTDIR)/HTAccess.obj \
	$(INTDIR)/HTFormat.obj \
	$(INTDIR)/HTAlert.obj \
	$(INTDIR)/HTBind.obj \
	$(INTDIR)/HTAnchor.obj \
	$(INTDIR)/HTWriter.obj \
	$(INTDIR)/HTTee.obj \
	$(INTDIR)/HTSocket.obj \
	$(INTDIR)/HTMethod.obj \
	$(INTDIR)/HTConLen.obj \
	$(INTDIR)/HTWWWStr.obj \
	\MSVC20\LIB\WSOCK32.LIB

$(OUTDIR)/wwwcore.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\windll.c

$(INTDIR)/windll.obj :  $(SOURCE)  $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTError.c

$(INTDIR)/HTError.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTProt.c

$(INTDIR)/HTProt.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEvntrg.c

$(INTDIR)/HTEvntrg.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WinDebug\wwwdll.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\WinDebug\wwwutils.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTParse.c

$(INTDIR)/HTParse.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore.def
# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTReqMan.c

$(INTDIR)/HTReqMan.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNet.c

$(INTDIR)/HTNet.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTCP.c

$(INTDIR)/HTTCP.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEscape.c

$(INTDIR)/HTEscape.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDNS.c

$(INTDIR)/HTDNS.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFWrite.c

$(INTDIR)/HTFWrite.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAccess.c

$(INTDIR)/HTAccess.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFormat.c

$(INTDIR)/HTFormat.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAlert.c

$(INTDIR)/HTAlert.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBind.c

$(INTDIR)/HTBind.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAnchor.c

$(INTDIR)/HTAnchor.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTWriter.c

$(INTDIR)/HTWriter.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTee.c

$(INTDIR)/HTTee.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTSocket.c

$(INTDIR)/HTSocket.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMethod.c

$(INTDIR)/HTMethod.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTConLen.c

$(INTDIR)/HTConLen.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTWWWStr.c

$(INTDIR)/HTWWWStr.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\MSVC20\LIB\WSOCK32.LIB
# End Source File
# End Group
# End Project
################################################################################
