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

ALL : $(OUTDIR)/www.exe $(OUTDIR)/www.bsc

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
	$(INTDIR)/HTDir.sbr \
	$(INTDIR)/HTFTPDir.sbr \
	$(INTDIR)/HTTelnet.sbr \
	$(INTDIR)/www.sbr \
	$(INTDIR)/HTWSRC.sbr \
	$(INTDIR)/HTLog.sbr \
	$(INTDIR)/HTHome.sbr \
	$(INTDIR)/HTInit.sbr \
	$(INTDIR)/HTIcons.sbr \
	$(INTDIR)/HTFile.sbr \
	$(INTDIR)/HTTeXGen.sbr \
	$(INTDIR)/lib.sbr \
	$(INTDIR)/HTML.sbr \
	$(INTDIR)/HTHist.sbr \
	$(INTDIR)/HTGopher.sbr \
	$(INTDIR)/HTFTP.sbr \
	$(INTDIR)/HTPlain.sbr \
	$(INTDIR)/HTGuess.sbr \
	$(INTDIR)/scroll.sbr \
	$(INTDIR)/HTMulti.sbr \
	$(INTDIR)/GridStyle.sbr \
	$(INTDIR)/GridText.sbr \
	$(INTDIR)/HTBrowse.sbr \
	$(INTDIR)/DefaultStyles.sbr \
	$(INTDIR)/HTMLPDTD.sbr \
	$(INTDIR)/SGML.sbr \
	$(INTDIR)/HTBInit.sbr

$(OUTDIR)/www.bsc : $(OUTDIR)  $(BSC32_SBRS)
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
	$(INTDIR)/HTDir.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwnews.lib \
	$(INTDIR)/HTFTPDir.obj \
	$(INTDIR)/HTTelnet.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwdll.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwutils.lib \
	$(INTDIR)/www.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwcore.lib \
	$(INTDIR)/HTWSRC.obj \
	$(INTDIR)/HTLog.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwcache.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwmime.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwhttp.lib \
	$(INTDIR)/HTHome.obj \
	$(INTDIR)/HTInit.obj \
	$(INTDIR)/HTIcons.obj \
	$(INTDIR)/HTFile.obj \
	$(INTDIR)/HTTeXGen.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwrules.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwmlgen.lib \
	$(INTDIR)/lib.obj \
	$(INTDIR)/HTML.obj \
	$(INTDIR)/HTHist.obj \
	$(INTDIR)/HTGopher.obj \
	$(INTDIR)/HTFTP.obj \
	$(INTDIR)/HTPlain.obj \
	$(INTDIR)/HTGuess.obj \
	$(INTDIR)/scroll.obj \
	$(INTDIR)/HTMulti.obj \
	$(INTDIR)/GridStyle.obj \
	$(INTDIR)/GridText.obj \
	$(INTDIR)/HTBrowse.obj \
	$(INTDIR)/DefaultStyles.obj \
	$(INTDIR)/HTMLPDTD.obj \
	$(INTDIR)/SGML.obj \
	$(INTDIR)/www.res \
	$(INTDIR)/HTBInit.obj

$(OUTDIR)/www.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/www.exe $(OUTDIR)/www.bsc

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
	$(INTDIR)/HTDir.sbr \
	$(INTDIR)/HTFTPDir.sbr \
	$(INTDIR)/HTTelnet.sbr \
	$(INTDIR)/www.sbr \
	$(INTDIR)/HTWSRC.sbr \
	$(INTDIR)/HTLog.sbr \
	$(INTDIR)/HTHome.sbr \
	$(INTDIR)/HTInit.sbr \
	$(INTDIR)/HTIcons.sbr \
	$(INTDIR)/HTFile.sbr \
	$(INTDIR)/HTTeXGen.sbr \
	$(INTDIR)/lib.sbr \
	$(INTDIR)/HTML.sbr \
	$(INTDIR)/HTHist.sbr \
	$(INTDIR)/HTGopher.sbr \
	$(INTDIR)/HTFTP.sbr \
	$(INTDIR)/HTPlain.sbr \
	$(INTDIR)/HTGuess.sbr \
	$(INTDIR)/scroll.sbr \
	$(INTDIR)/HTMulti.sbr \
	$(INTDIR)/GridStyle.sbr \
	$(INTDIR)/GridText.sbr \
	$(INTDIR)/HTBrowse.sbr \
	$(INTDIR)/DefaultStyles.sbr \
	$(INTDIR)/HTMLPDTD.sbr \
	$(INTDIR)/SGML.sbr \
	$(INTDIR)/HTBInit.sbr

$(OUTDIR)/www.bsc : $(OUTDIR)  $(BSC32_SBRS)
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
	$(INTDIR)/HTDir.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwnews.lib \
	$(INTDIR)/HTFTPDir.obj \
	$(INTDIR)/HTTelnet.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwdll.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwutils.lib \
	$(INTDIR)/www.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwcore.lib \
	$(INTDIR)/HTWSRC.obj \
	$(INTDIR)/HTLog.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwcache.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwmime.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwhttp.lib \
	$(INTDIR)/HTHome.obj \
	$(INTDIR)/HTInit.obj \
	$(INTDIR)/HTIcons.obj \
	$(INTDIR)/HTFile.obj \
	$(INTDIR)/HTTeXGen.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwrules.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwmlgen.lib \
	$(INTDIR)/lib.obj \
	$(INTDIR)/HTML.obj \
	$(INTDIR)/HTHist.obj \
	$(INTDIR)/HTGopher.obj \
	$(INTDIR)/HTFTP.obj \
	$(INTDIR)/HTPlain.obj \
	$(INTDIR)/HTGuess.obj \
	$(INTDIR)/scroll.obj \
	$(INTDIR)/HTMulti.obj \
	$(INTDIR)/GridStyle.obj \
	$(INTDIR)/GridText.obj \
	$(INTDIR)/HTBrowse.obj \
	$(INTDIR)/DefaultStyles.obj \
	$(INTDIR)/HTMLPDTD.obj \
	$(INTDIR)/SGML.obj \
	$(INTDIR)/www.res \
	$(INTDIR)/HTBInit.obj

$(OUTDIR)/www.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=..\..\..\Library\Implementation\HTDir.c

$(INTDIR)/HTDir.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwnews.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTFTPDir.c

$(INTDIR)/HTFTPDir.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTTelnet.c

$(INTDIR)/HTTelnet.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwdll.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwutils.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\www.c
DEP_WWW_C=\
	.\lib.h\
	.\scroll.h\
	..\HTBrowse.h

$(INTDIR)/www.obj :  $(SOURCE)  $(DEP_WWW_C) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwcore.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTWSRC.c

$(INTDIR)/HTWSRC.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTLog.c

$(INTDIR)/HTLog.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwcache.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwmime.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwhttp.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTHome.c

$(INTDIR)/HTHome.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTInit.c

$(INTDIR)/HTInit.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTIcons.c

$(INTDIR)/HTIcons.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTFile.c

$(INTDIR)/HTFile.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTTeXGen.c

$(INTDIR)/HTTeXGen.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwrules.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwmlgen.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=.\lib.c

$(INTDIR)/lib.obj :  $(SOURCE)  $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTML.c

$(INTDIR)/HTML.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTHist.c

$(INTDIR)/HTHist.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTGopher.c

$(INTDIR)/HTGopher.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTFTP.c

$(INTDIR)/HTFTP.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTPlain.c

$(INTDIR)/HTPlain.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTGuess.c

$(INTDIR)/HTGuess.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scroll.c

$(INTDIR)/scroll.obj :  $(SOURCE)  $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTMulti.c

$(INTDIR)/HTMulti.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\GridStyle.c

$(INTDIR)/GridStyle.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\GridText.c

$(INTDIR)/GridText.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBrowse.c

$(INTDIR)/HTBrowse.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\DefaultStyles.c

$(INTDIR)/DefaultStyles.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTMLPDTD.c

$(INTDIR)/HTMLPDTD.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\SGML.c

$(INTDIR)/SGML.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\www.rc
DEP_WWW_R=\
	.\www.ico

$(INTDIR)/www.res :  $(SOURCE)  $(DEP_WWW_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTBInit.c

$(INTDIR)/HTBInit.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
# End Group
# End Project
################################################################################
