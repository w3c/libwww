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
	.\WinRel\www.sbr \
	.\WinRel\HTInit.sbr \
	.\WinRel\lib.sbr \
	.\WinRel\HTML.sbr \
	.\WinRel\HTPlain.sbr \
	.\WinRel\scroll.sbr \
	.\WinRel\GridStyle.sbr \
	.\WinRel\GridText.sbr \
	.\WinRel\HTBrowse.sbr \
	.\WinRel\DefaultStyles.sbr \
	.\WinRel\HTMLPDTD.sbr \
	.\WinRel\HTBInit.sbr

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
	..\..\..\Library\Implementation\windows\WinDebug\wwwnews.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwdll.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwutils.lib \
	.\WinRel\www.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwcore.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwcache.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwmime.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwhttp.lib \
	.\WinRel\HTInit.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwrules.lib \
	.\WinRel\lib.obj \
	.\WinRel\HTML.obj \
	.\WinRel\HTPlain.obj \
	.\WinRel\scroll.obj \
	.\WinRel\GridStyle.obj \
	.\WinRel\GridText.obj \
	.\WinRel\HTBrowse.obj \
	.\WinRel\DefaultStyles.obj \
	.\WinRel\HTMLPDTD.obj \
	.\WinRel\www.res \
	.\WinRel\HTBInit.obj \
	......\Library\Implementation\windows\WinDebug\wwwfile.lib \
	......\Library\Implementation\windows\WinDebug\wwwapp.lib \
	......\Library\Implementation\windows\WinDebug\wwwguess.lib \
	......\Library\Implementation\windows\WinDebug\wwwtelnt.lib \
	......\Library\Implementation\windows\WinDebug\wwwdir.lib \
	......\Library\Implementation\windows\WinDebug\wwwwais.lib \
	......\Library\Implementation\windows\WinDebug\wwwgophe.lib \
	......\Library\Implementation\windows\WinDebug\wwwftp.lib \
	......\Library\Implementation\windows\WinDebug\wwwhtml.lib

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
	.\WinDebug\www.sbr \
	.\WinDebug\HTInit.sbr \
	.\WinDebug\lib.sbr \
	.\WinDebug\HTML.sbr \
	.\WinDebug\HTPlain.sbr \
	.\WinDebug\scroll.sbr \
	.\WinDebug\GridStyle.sbr \
	.\WinDebug\GridText.sbr \
	.\WinDebug\HTBrowse.sbr \
	.\WinDebug\DefaultStyles.sbr \
	.\WinDebug\HTMLPDTD.sbr \
	.\WinDebug\HTBInit.sbr

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
	..\..\..\Library\Implementation\windows\WinDebug\wwwnews.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwdll.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwutils.lib \
	.\WinDebug\www.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwcore.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwcache.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwmime.lib \
	..\..\..\Library\Implementation\windows\WinDebug\wwwhttp.lib \
	.\WinDebug\HTInit.obj \
	..\..\..\Library\Implementation\windows\WinDebug\wwwrules.lib \
	.\WinDebug\lib.obj \
	.\WinDebug\HTML.obj \
	.\WinDebug\HTPlain.obj \
	.\WinDebug\scroll.obj \
	.\WinDebug\GridStyle.obj \
	.\WinDebug\GridText.obj \
	.\WinDebug\HTBrowse.obj \
	.\WinDebug\DefaultStyles.obj \
	.\WinDebug\HTMLPDTD.obj \
	.\WinDebug\www.res \
	.\WinDebug\HTBInit.obj \
	......\Library\Implementation\windows\WinDebug\wwwfile.lib \
	......\Library\Implementation\windows\WinDebug\wwwapp.lib \
	......\Library\Implementation\windows\WinDebug\wwwguess.lib \
	......\Library\Implementation\windows\WinDebug\wwwtelnt.lib \
	......\Library\Implementation\windows\WinDebug\wwwdir.lib \
	......\Library\Implementation\windows\WinDebug\wwwwais.lib \
	......\Library\Implementation\windows\WinDebug\wwwgophe.lib \
	......\Library\Implementation\windows\WinDebug\wwwftp.lib \
	......\Library\Implementation\windows\WinDebug\wwwhtml.lib

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

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwnews.lib
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

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\www.obj :  $(SOURCE)  $(DEP_WWW_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\www.obj :  $(SOURCE)  $(DEP_WWW_C) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwcore.lib
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

SOURCE=..\..\..\Library\Implementation\HTInit.c

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

SOURCE=..\..\..\Library\Implementation\windows\WinDebug\wwwrules.lib
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

SOURCE=..\..\..\Library\Implementation\HTML.c

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

SOURCE=..\..\..\Library\Implementation\HTPlain.c

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

SOURCE=.\scroll.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\scroll.obj :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\scroll.obj :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\GridStyle.c

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

SOURCE=..\GridText.c

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

SOURCE=..\HTBrowse.c

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

SOURCE=..\DefaultStyles.c

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

SOURCE=..\..\..\Library\Implementation\HTMLPDTD.c

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

SOURCE=.\www.rc
DEP_WWW_R=\
	.\www.ico

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\www.res :  $(SOURCE)  $(DEP_WWW_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\www.res :  $(SOURCE)  $(DEP_WWW_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTBInit.c

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\HTBInit.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\HTBInit.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
......\Library\Implementation\windows\WinDebug\wwwfile.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\
......\Library\Implementation\windows\WinDebug\wwwapp.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\
......\Library\Implementation\windows\WinDebug\wwwguess.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\
......\Library\Implementation\windows\WinDebug\wwwtelnt.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\
......\Library\Implementation\windows\WinDebug\wwwdir.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\
......\Library\Implementation\windows\WinDebug\wwwwais.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\
......\Library\Implementation\windows\WinDebug\wwwgophe.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\
......\Library\Implementation\windows\WinDebug\wwwftp.lib
# End Source File
################################################################################
# Begin Source File

SOURCE=\
......\Library\Implementation\windows\WinDebug\wwwhtml.lib
# End Source File
# End Group
# End Project
################################################################################
