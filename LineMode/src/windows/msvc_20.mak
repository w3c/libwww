# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "www.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
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

ALL : .\WinRel\www.exe .\WinRel\www.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /I "..\..\..\Library\Implementation" /I "..\..\..\Pics\Implementation" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /I "..\..\..\Library\Implementation" /I\
 "..\..\..\Pics\Implementation" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /FR$(INTDIR)/ /Fp$(OUTDIR)/"www.pch"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"www.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"www.bsc" 
BSC32_SBRS= \
	.\WinRel\HTDir.sbr \
	.\WinRel\HTFTPDir.sbr \
	.\WinRel\HTTelnet.sbr \
	.\WinRel\www.sbr \
	.\WinRel\HTWSRC.sbr \
	.\WinRel\HTLog.sbr \
	.\WinRel\HTHome.sbr \
	.\WinRel\HTInit.sbr \
	.\WinRel\HTIcons.sbr \
	.\WinRel\HTFile.sbr \
	.\WinRel\HTTeXGen.sbr \
	.\WinRel\lib.sbr \
	.\WinRel\HTML.sbr \
	.\WinRel\HTHist.sbr \
	.\WinRel\HTGopher.sbr \
	.\WinRel\HTFTP.sbr \
	.\WinRel\HTPlain.sbr \
	.\WinRel\HTGuess.sbr \
	.\WinRel\scroll.sbr \
	.\WinRel\HTMulti.sbr \
	.\WinRel\GridStyle.sbr \
	.\WinRel\GridText.sbr \
	.\WinRel\HTBrowse.sbr \
	.\WinRel\DefaultStyles.sbr \
	.\WinRel\HTMLPDTD.sbr \
	.\WinRel\SGML.sbr

.\WinRel\www.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib comdlg32.lib gdi32.lib wsock32.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=user32.lib comdlg32.lib gdi32.lib wsock32.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"www.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"www.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\HTDir.obj \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwnews.lib \
	.\WinRel\HTFTPDir.obj \
	.\WinRel\HTTelnet.obj \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwdll.lib \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwutils.lib \
	.\WinRel\www.obj \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwcore.lib \
	.\WinRel\HTWSRC.obj \
	.\WinRel\HTLog.obj \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwcache.lib \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwmime.lib \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwhttp.lib \
	.\WinRel\HTHome.obj \
	.\WinRel\HTInit.obj \
	.\WinRel\HTIcons.obj \
	.\WinRel\HTFile.obj \
	.\WinRel\HTTeXGen.obj \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwrules.lib \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwmlgen.lib \
	.\WinRel\lib.obj \
	.\WinRel\HTML.obj \
	.\WinRel\HTHist.obj \
	.\WinRel\HTGopher.obj \
	.\WinRel\HTFTP.obj \
	.\WinRel\HTPlain.obj \
	.\WinRel\HTGuess.obj \
	.\WinRel\scroll.obj \
	.\WinRel\HTMulti.obj \
	.\WinRel\GridStyle.obj \
	.\WinRel\GridText.obj \
	.\WinRel\HTBrowse.obj \
	.\WinRel\DefaultStyles.obj \
	.\WinRel\HTMLPDTD.obj \
	.\WinRel\SGML.obj \
	.\WinRel\www.res

.\WinRel\www.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : .\WinDebug\www.exe .\WinDebug\www.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /I "..\..\..\Library\Implementation" /I "..\..\..\Pics\Implementation" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /I "..\..\..\Library\Implementation" /I\
 "..\..\..\Pics\Implementation" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /FR$(INTDIR)/ /Fp$(OUTDIR)/"www.pch"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"www.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"www.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"www.bsc" 
BSC32_SBRS= \
	.\WinDebug\HTDir.sbr \
	.\WinDebug\HTFTPDir.sbr \
	.\WinDebug\HTTelnet.sbr \
	.\WinDebug\www.sbr \
	.\WinDebug\HTWSRC.sbr \
	.\WinDebug\HTLog.sbr \
	.\WinDebug\HTHome.sbr \
	.\WinDebug\HTInit.sbr \
	.\WinDebug\HTIcons.sbr \
	.\WinDebug\HTFile.sbr \
	.\WinDebug\HTTeXGen.sbr \
	.\WinDebug\lib.sbr \
	.\WinDebug\HTML.sbr \
	.\WinDebug\HTHist.sbr \
	.\WinDebug\HTGopher.sbr \
	.\WinDebug\HTFTP.sbr \
	.\WinDebug\HTPlain.sbr \
	.\WinDebug\HTGuess.sbr \
	.\WinDebug\scroll.sbr \
	.\WinDebug\HTMulti.sbr \
	.\WinDebug\GridStyle.sbr \
	.\WinDebug\GridText.sbr \
	.\WinDebug\HTBrowse.sbr \
	.\WinDebug\DefaultStyles.sbr \
	.\WinDebug\HTMLPDTD.sbr \
	.\WinDebug\SGML.sbr

.\WinDebug\www.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib comdlg32.lib gdi32.lib wsock32.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib comdlg32.lib gdi32.lib wsock32.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"www.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"www.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\HTDir.obj \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwnews.lib \
	.\WinDebug\HTFTPDir.obj \
	.\WinDebug\HTTelnet.obj \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwdll.lib \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwutils.lib \
	.\WinDebug\www.obj \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwcore.lib \
	.\WinDebug\HTWSRC.obj \
	.\WinDebug\HTLog.obj \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwcache.lib \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwmime.lib \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwhttp.lib \
	.\WinDebug\HTHome.obj \
	.\WinDebug\HTInit.obj \
	.\WinDebug\HTIcons.obj \
	.\WinDebug\HTFile.obj \
	.\WinDebug\HTTeXGen.obj \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwrules.lib \
	\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwmlgen.lib \
	.\WinDebug\lib.obj \
	.\WinDebug\HTML.obj \
	.\WinDebug\HTHist.obj \
	.\WinDebug\HTGopher.obj \
	.\WinDebug\HTFTP.obj \
	.\WinDebug\HTPlain.obj \
	.\WinDebug\HTGuess.obj \
	.\WinDebug\scroll.obj \
	.\WinDebug\HTMulti.obj \
	.\WinDebug\GridStyle.obj \
	.\WinDebug\GridText.obj \
	.\WinDebug\HTBrowse.obj \
	.\WinDebug\DefaultStyles.obj \
	.\WinDebug\HTMLPDTD.obj \
	.\WinDebug\SGML.obj \
	.\WinDebug\www.res

.\WinDebug\www.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTDir.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTDir.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTDir.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwnews.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTFTPDir.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTFTPDir.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTFTPDir.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTTelnet.c

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

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwdll.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\
\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwutils.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\www.c
DEP_WWW_C=\
	.\lib.h\
	.\scroll.h\
	..\HTBrowse.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\www.obj :  $(SOURCE)  $(DEP_WWW_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\www.obj :  $(SOURCE)  $(DEP_WWW_C) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwcore.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTWSRC.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTWSRC.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTWSRC.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTLog.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTLog.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTLog.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwcache.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwmime.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwhttp.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTHome.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTHome.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTHome.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTInit.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTInit.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTInit.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTIcons.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTIcons.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTIcons.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTFile.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTFile.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTFile.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTTeXGen.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTTeXGen.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTTeXGen.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwrules.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\
\PROJECTS\LIBWWW\WWW\Library\Implementation\windows\WinDebug\wwwmlgen.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\lib.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\lib.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\lib.obj :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTML.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTML.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTML.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTHist.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTHist.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTHist.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTGopher.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTGopher.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTGopher.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTFTP.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTFTP.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTFTP.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTPlain.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTPlain.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTPlain.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTGuess.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTGuess.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTGuess.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scroll.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\scroll.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\scroll.obj :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTMulti.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTMulti.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTMulti.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\LineMode\Implementation\GridStyle.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\GridStyle.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\GridStyle.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\LineMode\Implementation\GridText.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\GridText.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\GridText.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\LineMode\Implementation\HTBrowse.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTBrowse.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTBrowse.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\LineMode\Implementation\DefaultStyles.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\DefaultStyles.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\DefaultStyles.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\HTMLPDTD.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTMLPDTD.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTMLPDTD.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\PROJECTS\LIBWWW\WWW\Library\Implementation\SGML.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\SGML.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\SGML.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\www.rc
DEP_WWW_R=\
	.\www.ico\
	E:\u\eric\PROJECTS\WINLIB\COMM.RC

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\www.res :  $(SOURCE)  $(DEP_WWW_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\www.res :  $(SOURCE)  $(DEP_WWW_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
