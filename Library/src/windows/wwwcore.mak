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

ALL : .\WinRel\wwwcore.dll .\WinRel\wwwcore.bsc

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
	.\WinRel\windll.sbr \
	.\WinRel\HTError.sbr \
	.\WinRel\HTProt.sbr \
	.\WinRel\HTEvntrg.sbr \
	.\WinRel\HTParse.sbr \
	.\WinRel\HTReqMan.sbr \
	.\WinRel\HTNet.sbr \
	.\WinRel\HTTCP.sbr \
	.\WinRel\HTDialog.sbr \
	.\WinRel\HTEscape.sbr \
	.\WinRel\HTDNS.sbr \
	.\WinRel\HTFWrite.sbr \
	.\WinRel\HTAccess.sbr \
	.\WinRel\HTFormat.sbr \
	.\WinRel\HTAlert.sbr \
	.\WinRel\HTBind.sbr \
	.\WinRel\HTAnchor.sbr \
	.\WinRel\HTWriter.sbr \
	.\WinRel\HTTee.sbr \
	.\WinRel\HTSocket.sbr \
	.\WinRel\HTMethod.sbr \
	.\WinRel\HTConLen.sbr \
	.\WinRel\HTWWWStr.sbr

.\WinRel\wwwcore.bsc : $(OUTDIR)  $(BSC32_SBRS)
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
	.\WinRel\windll.obj \
	.\WinRel\HTError.obj \
	.\WinRel\HTProt.obj \
	.\WinRel\HTEvntrg.obj \
	.\WinDebug\wwwdll.lib \
	.\WinDebug\wwwutils.lib \
	.\WinRel\HTParse.obj \
	.\WinRel\HTReqMan.obj \
	.\WinRel\HTNet.obj \
	.\WinRel\HTTCP.obj \
	.\WinRel\HTDialog.obj \
	.\WinRel\HTEscape.obj \
	.\WinRel\HTDNS.obj \
	.\WinRel\HTFWrite.obj \
	.\WinRel\HTAccess.obj \
	.\WinRel\HTFormat.obj \
	.\WinRel\HTAlert.obj \
	.\WinRel\HTBind.obj \
	.\WinRel\HTAnchor.obj \
	.\WinRel\HTWriter.obj \
	.\WinRel\HTTee.obj \
	.\WinRel\HTSocket.obj \
	.\WinRel\HTMethod.obj \
	.\WinRel\HTConLen.obj \
	.\WinRel\HTWWWStr.obj \
	\MSVC20\LIB\WSOCK32.LIB

.\WinRel\wwwcore.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : .\WinDebug\wwwcore.dll .\WinDebug\wwwcore.bsc

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
	.\WinDebug\windll.sbr \
	.\WinDebug\HTError.sbr \
	.\WinDebug\HTProt.sbr \
	.\WinDebug\HTEvntrg.sbr \
	.\WinDebug\HTParse.sbr \
	.\WinDebug\HTReqMan.sbr \
	.\WinDebug\HTNet.sbr \
	.\WinDebug\HTTCP.sbr \
	.\WinDebug\HTDialog.sbr \
	.\WinDebug\HTEscape.sbr \
	.\WinDebug\HTDNS.sbr \
	.\WinDebug\HTFWrite.sbr \
	.\WinDebug\HTAccess.sbr \
	.\WinDebug\HTFormat.sbr \
	.\WinDebug\HTAlert.sbr \
	.\WinDebug\HTBind.sbr \
	.\WinDebug\HTAnchor.sbr \
	.\WinDebug\HTWriter.sbr \
	.\WinDebug\HTTee.sbr \
	.\WinDebug\HTSocket.sbr \
	.\WinDebug\HTMethod.sbr \
	.\WinDebug\HTConLen.sbr \
	.\WinDebug\HTWWWStr.sbr

.\WinDebug\wwwcore.bsc : $(OUTDIR)  $(BSC32_SBRS)
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
	.\WinDebug\windll.obj \
	.\WinDebug\HTError.obj \
	.\WinDebug\HTProt.obj \
	.\WinDebug\HTEvntrg.obj \
	.\WinDebug\wwwdll.lib \
	.\WinDebug\wwwutils.lib \
	.\WinDebug\HTParse.obj \
	.\WinDebug\HTReqMan.obj \
	.\WinDebug\HTNet.obj \
	.\WinDebug\HTTCP.obj \
	.\WinDebug\HTDialog.obj \
	.\WinDebug\HTEscape.obj \
	.\WinDebug\HTDNS.obj \
	.\WinDebug\HTFWrite.obj \
	.\WinDebug\HTAccess.obj \
	.\WinDebug\HTFormat.obj \
	.\WinDebug\HTAlert.obj \
	.\WinDebug\HTBind.obj \
	.\WinDebug\HTAnchor.obj \
	.\WinDebug\HTWriter.obj \
	.\WinDebug\HTTee.obj \
	.\WinDebug\HTSocket.obj \
	.\WinDebug\HTMethod.obj \
	.\WinDebug\HTConLen.obj \
	.\WinDebug\HTWWWStr.obj \
	\MSVC20\LIB\WSOCK32.LIB

.\WinDebug\wwwcore.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\windll.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\windll.obj :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTError.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTError.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTError.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTProt.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTProt.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTProt.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEvntrg.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTEvntrg.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTEvntrg.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

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

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTParse.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTParse.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wwwcore.def
# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTReqMan.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTReqMan.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTReqMan.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTNet.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTNet.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTNet.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTCP.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTTCP.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTTCP.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDialog.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTDialog.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTDialog.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTEscape.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTEscape.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTEscape.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTDNS.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTDNS.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTDNS.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFWrite.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTFWrite.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTFWrite.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAccess.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTAccess.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTAccess.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTFormat.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTFormat.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTFormat.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAlert.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTAlert.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTAlert.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBind.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTBind.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTBind.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTAnchor.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTAnchor.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTAnchor.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTWriter.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTWriter.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTWriter.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTTee.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTTee.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTTee.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTSocket.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTSocket.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTSocket.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTMethod.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTMethod.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTMethod.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTConLen.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTConLen.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTConLen.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTWWWStr.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTWWWStr.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTWWWStr.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\MSVC20\LIB\WSOCK32.LIB
# End Source File
# End Group
# End Project
################################################################################
