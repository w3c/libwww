# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=webbot - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to webbot - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "webbot - Win32 Release" && "$(CFG)" != "webbot - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "webbot.mak" CFG="webbot - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "webbot - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "webbot - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "webbot - Win32 Debug"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "webbot - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Bin\Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\..\..\..\Bin\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\webbot.exe"

CLEAN : 
	-@erase "..\..\..\Bin\Debug\webbot.exe"
	-@erase ".\Release\HTStyle.obj"
	-@erase ".\Release\HTInit.obj"
	-@erase ".\Release\HTHInit.obj"
	-@erase ".\Release\HTPlain.obj"
	-@erase ".\Release\HTBInit.obj"
	-@erase ".\Release\SGML.obj"
	-@erase ".\Release\HTRobot.obj"
	-@erase ".\Release\HTMLPDTD.obj"
	-@erase ".\Release\HTML.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "_WINDOWS" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)/webbot.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/webbot.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\..\..\Bin\Debug\webbot.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/webbot.pdb" /machine:I386 /out:"..\..\..\Bin\Debug\webbot.exe" 
LINK32_OBJS= \
	"$(INTDIR)/HTStyle.obj" \
	"$(INTDIR)/HTInit.obj" \
	"$(INTDIR)/HTHInit.obj" \
	"$(INTDIR)/HTPlain.obj" \
	"$(INTDIR)/HTBInit.obj" \
	"$(INTDIR)/SGML.obj" \
	"$(INTDIR)/HTRobot.obj" \
	"$(INTDIR)/HTMLPDTD.obj" \
	"$(INTDIR)/HTML.obj" \
	"..\..\..\Library\src\windows\wwwhtml\Debug\wwwhtml.lib" \
	"..\..\..\Library\src\windows\wwwdir\Debug\wwwdir.lib" \
	"..\..\..\Library\src\windows\wwwmime\Debug\wwwmime.lib" \
	"..\..\..\Library\src\windows\wwwhttp\Debug\wwwhttp.lib" \
	"..\..\..\Library\src\windows\wwwgophe\Debug\wwwgophe.lib" \
	"..\..\..\Library\src\windows\wwwapp\Debug\wwwapp.lib" \
	"..\..\..\Library\src\windows\wwwcore\Debug\wwwcore.lib" \
	"..\..\..\Library\src\windows\wwwwais\Debug\wwwwais.lib" \
	"..\..\..\Library\src\windows\wwwdll\Debug\wwwdll.lib" \
	"..\..\..\Library\src\windows\wwwtrans\Debug\wwwtrans.lib" \
	"..\..\..\Library\src\windows\wwwfile\Debug\wwwfile.lib" \
	"..\..\..\Library\src\windows\wwwcache\Debug\wwwcache.lib" \
	"..\..\..\Library\src\windows\wwwrules\Debug\wwwrules.lib" \
	"..\..\..\PICS-client\src\windows\Debug\pics.lib" \
	"..\..\..\Library\src\windows\wwwstream\Debug\wwwstream.lib" \
	"..\..\..\Library\src\windows\wwwnews\Debug\wwwnews.lib" \
	"..\..\..\Library\src\windows\wwwftp\Debug\wwwftp.lib" \
	"..\..\..\Library\src\windows\wwwutils\Debug\wwwutils.lib" \
	"..\..\..\Library\src\windows\wwwtelnt\Debug\wwwtelnt.lib"

"$(OUTDIR)\webbot.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Bin\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\..\..\..\Bin\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\webbot.exe" ".\Debug\webbot.bsc"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\webbot.bsc"
	-@erase ".\Debug\SGML.sbr"
	-@erase ".\Debug\HTHInit.sbr"
	-@erase ".\Debug\HTPlain.sbr"
	-@erase ".\Debug\HTBInit.sbr"
	-@erase ".\Debug\HTRobot.sbr"
	-@erase ".\Debug\HTInit.sbr"
	-@erase ".\Debug\HTML.sbr"
	-@erase ".\Debug\HTMLPDTD.sbr"
	-@erase ".\Debug\HTStyle.sbr"
	-@erase "..\..\..\Bin\Debug\webbot.exe"
	-@erase ".\Debug\SGML.obj"
	-@erase ".\Debug\HTHInit.obj"
	-@erase ".\Debug\HTPlain.obj"
	-@erase ".\Debug\HTBInit.obj"
	-@erase ".\Debug\HTRobot.obj"
	-@erase ".\Debug\HTInit.obj"
	-@erase ".\Debug\HTML.obj"
	-@erase ".\Debug\HTMLPDTD.obj"
	-@erase ".\Debug\HTStyle.obj"
	-@erase "..\..\..\Bin\Debug\webbot.ilk"
	-@erase "..\..\..\Bin\Debug\webbot.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /FR /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE"\
 /D "_WINDOWS" /D "WWW_WIN_ASYNC" /FR"$(INTDIR)/" /Fp"$(INTDIR)/webbot.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"./Debug/webbot.bsc"
BSC32_FLAGS=/nologo /o"./Debug/webbot.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/SGML.sbr" \
	"$(INTDIR)/HTHInit.sbr" \
	"$(INTDIR)/HTPlain.sbr" \
	"$(INTDIR)/HTBInit.sbr" \
	"$(INTDIR)/HTRobot.sbr" \
	"$(INTDIR)/HTInit.sbr" \
	"$(INTDIR)/HTML.sbr" \
	"$(INTDIR)/HTMLPDTD.sbr" \
	"$(INTDIR)/HTStyle.sbr"

".\Debug\webbot.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/webbot.pdb" /debug /machine:I386 /out:"$(OUTDIR)/webbot.exe" 
LINK32_OBJS= \
	"$(INTDIR)/SGML.obj" \
	"$(INTDIR)/HTHInit.obj" \
	"$(INTDIR)/HTPlain.obj" \
	"$(INTDIR)/HTBInit.obj" \
	"$(INTDIR)/HTRobot.obj" \
	"$(INTDIR)/HTInit.obj" \
	"$(INTDIR)/HTML.obj" \
	"$(INTDIR)/HTMLPDTD.obj" \
	"$(INTDIR)/HTStyle.obj" \
	"..\..\..\Library\src\windows\wwwhtml\Debug\wwwhtml.lib" \
	"..\..\..\Library\src\windows\wwwdir\Debug\wwwdir.lib" \
	"..\..\..\Library\src\windows\wwwmime\Debug\wwwmime.lib" \
	"..\..\..\Library\src\windows\wwwhttp\Debug\wwwhttp.lib" \
	"..\..\..\Library\src\windows\wwwgophe\Debug\wwwgophe.lib" \
	"..\..\..\Library\src\windows\wwwapp\Debug\wwwapp.lib" \
	"..\..\..\Library\src\windows\wwwcore\Debug\wwwcore.lib" \
	"..\..\..\Library\src\windows\wwwwais\Debug\wwwwais.lib" \
	"..\..\..\Library\src\windows\wwwdll\Debug\wwwdll.lib" \
	"..\..\..\Library\src\windows\wwwtrans\Debug\wwwtrans.lib" \
	"..\..\..\Library\src\windows\wwwfile\Debug\wwwfile.lib" \
	"..\..\..\Library\src\windows\wwwcache\Debug\wwwcache.lib" \
	"..\..\..\Library\src\windows\wwwrules\Debug\wwwrules.lib" \
	"..\..\..\PICS-client\src\windows\Debug\pics.lib" \
	"..\..\..\Library\src\windows\wwwstream\Debug\wwwstream.lib" \
	"..\..\..\Library\src\windows\wwwnews\Debug\wwwnews.lib" \
	"..\..\..\Library\src\windows\wwwftp\Debug\wwwftp.lib" \
	"..\..\..\Library\src\windows\wwwutils\Debug\wwwutils.lib" \
	"..\..\..\Library\src\windows\wwwtelnt\Debug\wwwtelnt.lib"

"$(OUTDIR)\webbot.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "webbot - Win32 Release"
# Name "webbot - Win32 Debug"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE="..\HTRobot.c"
DEP_CPP_HTROB=\
	{$(INCLUDE)}"\WWWLib.h"\
	{$(INCLUDE)}"\WWWApp.h"\
	{$(INCLUDE)}"\WWWRules.h"\
	{$(INCLUDE)}"\WWWTrans.h"\
	{$(INCLUDE)}"\WWWInit.h"\
	{$(INCLUDE)}"\HText.h"\
	".\..\HTRobot.h"\
	{$(INCLUDE)}"\sysdep.h"\
	{$(INCLUDE)}"\WWWUtil.h"\
	{$(INCLUDE)}"\WWWCore.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\HTUtils.h"\
	{$(INCLUDE)}"\HTArray.h"\
	{$(INCLUDE)}"\HTAssoc.h"\
	{$(INCLUDE)}"\HTAtom.h"\
	{$(INCLUDE)}"\HTChunk.h"\
	{$(INCLUDE)}"\HTList.h"\
	{$(INCLUDE)}"\HTMemory.h"\
	{$(INCLUDE)}"\HTString.h"\
	{$(INCLUDE)}"\HTUU.h"\
	{$(INCLUDE)}"\HTReq.h"\
	{$(INCLUDE)}"\HTAccess.h"\
	{$(INCLUDE)}"\HTMethod.h"\
	{$(INCLUDE)}"\HTAnchor.h"\
	{$(INCLUDE)}"\HTParse.h"\
	{$(INCLUDE)}"\HTEscape.h"\
	{$(INCLUDE)}"\HTWWWStr.h"\
	{$(INCLUDE)}"\HTUser.h"\
	{$(INCLUDE)}"\HTEvent.h"\
	{$(INCLUDE)}"\HTError.h"\
	{$(INCLUDE)}"\HTAlert.h"\
	{$(INCLUDE)}"\HTFormat.h"\
	{$(INCLUDE)}"\HTBind.h"\
	{$(INCLUDE)}"\HTStream.h"\
	{$(INCLUDE)}"\HTIOStream.h"\
	{$(INCLUDE)}"\HTFWrite.h"\
	{$(INCLUDE)}"\HTDNS.h"\
	{$(INCLUDE)}"\HTHost.h"\
	{$(INCLUDE)}"\HTNet.h"\
	{$(INCLUDE)}"\HTInet.h"\
	{$(INCLUDE)}"\HTTrans.h"\
	{$(INCLUDE)}"\HTProt.h"\
	{$(INCLUDE)}"\HTChannl.h"\
	{$(INCLUDE)}"\HTEvntrg.h"\
	{$(INCLUDE)}"\HTHome.h"\
	{$(INCLUDE)}"\HTDialog.h"\
	{$(INCLUDE)}"\HTLog.h"\
	{$(INCLUDE)}"\HTHist.h"\
	{$(INCLUDE)}"\HTRules.h"\
	{$(INCLUDE)}"\HTProxy.h"\
	{$(INCLUDE)}"\HTANSI.h"\
	{$(INCLUDE)}"\HTLocal.h"\
	{$(INCLUDE)}"\HTTCP.h"\
	{$(INCLUDE)}"\HTSocket.h"\
	{$(INCLUDE)}"\HTReader.h"\
	{$(INCLUDE)}"\HTWriter.h"\
	{$(INCLUDE)}"\HTBufWrt.h"\
	{$(INCLUDE)}"\HTMux.h"\
	{$(INCLUDE)}"\HTInit.h"\
	{$(INCLUDE)}"\HTHInit.h"\
	{$(INCLUDE)}"\HTBInit.h"\
	{$(INCLUDE)}"\WWWHTTP.h"\
	{$(INCLUDE)}"\WWWFile.h"\
	{$(INCLUDE)}"\WWWFTP.h"\
	{$(INCLUDE)}"\WWWGophe.h"\
	{$(INCLUDE)}"\WWWTelnt.h"\
	{$(INCLUDE)}"\WWWNews.h"\
	{$(INCLUDE)}"\WWWWAIS.h"\
	{$(INCLUDE)}"\WWWMIME.h"\
	{$(INCLUDE)}"\WWWHTML.h"\
	{$(INCLUDE)}"\WWWStream.h"\
	{$(INCLUDE)}"\WWWDir.h"\
	{$(INCLUDE)}"\HTTPUtil.h"\
	{$(INCLUDE)}"\HTTP.h"\
	{$(INCLUDE)}"\HTTPServ.h"\
	{$(INCLUDE)}"\HTTPGen.h"\
	{$(INCLUDE)}"\HTTPReq.h"\
	{$(INCLUDE)}"\HTTPRes.h"\
	{$(INCLUDE)}"\HTTChunk.h"\
	{$(INCLUDE)}"\HTAAUtil.h"\
	{$(INCLUDE)}"\HTAABrow.h"\
	{$(INCLUDE)}"\HTMulti.h"\
	{$(INCLUDE)}"\HTFile.h"\
	{$(INCLUDE)}"\HTFTP.h"\
	{$(INCLUDE)}"\HTFTPDir.h"\
	{$(INCLUDE)}"\HTGopher.h"\
	{$(INCLUDE)}"\HTTelnet.h"\
	{$(INCLUDE)}"\HTNews.h"\
	{$(INCLUDE)}"\HTNewsLs.h"\
	{$(INCLUDE)}"\HTNDir.h"\
	{$(INCLUDE)}"\HTMIME.h"\
	{$(INCLUDE)}"\HTMIMERq.h"\
	{$(INCLUDE)}"\HTBound.h"\
	{$(INCLUDE)}"\HTMulpar.h"\
	{$(INCLUDE)}"\HTHeader.h"\
	{$(INCLUDE)}"\HTMLPDTD.h"\
	{$(INCLUDE)}"\SGML.h"\
	{$(INCLUDE)}"\HTMLGen.h"\
	{$(INCLUDE)}"\HTTeXGen.h"\
	{$(INCLUDE)}"\HTStruct.h"\
	{$(INCLUDE)}"\HTConLen.h"\
	{$(INCLUDE)}"\HTGuess.h"\
	{$(INCLUDE)}"\HTTee.h"\
	{$(INCLUDE)}"\HTXParse.h"\
	{$(INCLUDE)}"\HTIcons.h"\
	{$(INCLUDE)}"\HTDescpt.h"\
	{$(INCLUDE)}"\HTDir.h"\
	{$(INCLUDE)}"\HTML.h"\
	{$(INCLUDE)}"\HTPlain.h"\
	{$(INCLUDE)}"\HTStyle.h"\
	
NODEP_CPP_HTROB=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

!IF  "$(CFG)" == "webbot - Win32 Release"


"$(INTDIR)\HTRobot.obj" : $(SOURCE) $(DEP_CPP_HTROB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HTRobot.obj" : $(SOURCE) $(DEP_CPP_HTROB) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HTRobot.sbr" : $(SOURCE) $(DEP_CPP_HTROB) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwapp\Debug\wwwapp.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwcache\Debug\wwwcache.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwcore\Debug\wwwcore.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwdir\Debug\wwwdir.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwdll\Debug\wwwdll.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwfile\Debug\wwwfile.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwftp\Debug\wwwftp.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwgophe\Debug\wwwgophe.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwhtml\Debug\wwwhtml.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwhttp\Debug\wwwhttp.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwmime\Debug\wwwmime.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwnews\Debug\wwwnews.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwrules\Debug\wwwrules.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwstream\Debug\wwwstream.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwtelnt\Debug\wwwtelnt.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwtrans\Debug\wwwtrans.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwutils\Debug\wwwutils.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\Library\src\windows\wwwwais\Debug\wwwwais.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\HTHInit.c"
DEP_CPP_HTHIN=\
	{$(INCLUDE)}"\sysdep.h"\
	{$(INCLUDE)}"\WWWUtil.h"\
	{$(INCLUDE)}"\WWWCore.h"\
	{$(INCLUDE)}"\HTHInit.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\HTUtils.h"\
	{$(INCLUDE)}"\HTArray.h"\
	{$(INCLUDE)}"\HTAssoc.h"\
	{$(INCLUDE)}"\HTAtom.h"\
	{$(INCLUDE)}"\HTChunk.h"\
	{$(INCLUDE)}"\HTList.h"\
	{$(INCLUDE)}"\HTMemory.h"\
	{$(INCLUDE)}"\HTString.h"\
	{$(INCLUDE)}"\HTUU.h"\
	{$(INCLUDE)}"\HTReq.h"\
	{$(INCLUDE)}"\HTAccess.h"\
	{$(INCLUDE)}"\HTMethod.h"\
	{$(INCLUDE)}"\HTAnchor.h"\
	{$(INCLUDE)}"\HTParse.h"\
	{$(INCLUDE)}"\HTEscape.h"\
	{$(INCLUDE)}"\HTWWWStr.h"\
	{$(INCLUDE)}"\HTUser.h"\
	{$(INCLUDE)}"\HTEvent.h"\
	{$(INCLUDE)}"\HTError.h"\
	{$(INCLUDE)}"\HTAlert.h"\
	{$(INCLUDE)}"\HTFormat.h"\
	{$(INCLUDE)}"\HTBind.h"\
	{$(INCLUDE)}"\HTStream.h"\
	{$(INCLUDE)}"\HTIOStream.h"\
	{$(INCLUDE)}"\HTFWrite.h"\
	{$(INCLUDE)}"\HTDNS.h"\
	{$(INCLUDE)}"\HTHost.h"\
	{$(INCLUDE)}"\HTNet.h"\
	{$(INCLUDE)}"\HTInet.h"\
	{$(INCLUDE)}"\HTTrans.h"\
	{$(INCLUDE)}"\HTProt.h"\
	{$(INCLUDE)}"\HTChannl.h"\
	{$(INCLUDE)}"\WWWLib.h"\
	{$(INCLUDE)}"\HTML.h"\
	{$(INCLUDE)}"\HTPlain.h"\
	{$(INCLUDE)}"\HTTeXGen.h"\
	{$(INCLUDE)}"\HTMLGen.h"\
	{$(INCLUDE)}"\HTMLPDTD.h"\
	{$(INCLUDE)}"\HTStruct.h"\
	{$(INCLUDE)}"\SGML.h"\
	
NODEP_CPP_HTHIN=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "webbot - Win32 Release"


"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HTHInit.sbr" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\HTInit.c"
DEP_CPP_HTINI=\
	{$(INCLUDE)}"\sysdep.h"\
	{$(INCLUDE)}"\WWWUtil.h"\
	{$(INCLUDE)}"\WWWCore.h"\
	".\..\..\..\Library\src\HTReqMan.h"\
	{$(INCLUDE)}"\HTInit.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\HTUtils.h"\
	{$(INCLUDE)}"\HTArray.h"\
	{$(INCLUDE)}"\HTAssoc.h"\
	{$(INCLUDE)}"\HTAtom.h"\
	{$(INCLUDE)}"\HTChunk.h"\
	{$(INCLUDE)}"\HTList.h"\
	{$(INCLUDE)}"\HTMemory.h"\
	{$(INCLUDE)}"\HTString.h"\
	{$(INCLUDE)}"\HTUU.h"\
	{$(INCLUDE)}"\HTReq.h"\
	{$(INCLUDE)}"\HTAccess.h"\
	{$(INCLUDE)}"\HTMethod.h"\
	{$(INCLUDE)}"\HTAnchor.h"\
	{$(INCLUDE)}"\HTParse.h"\
	{$(INCLUDE)}"\HTEscape.h"\
	{$(INCLUDE)}"\HTWWWStr.h"\
	{$(INCLUDE)}"\HTUser.h"\
	{$(INCLUDE)}"\HTEvent.h"\
	{$(INCLUDE)}"\HTError.h"\
	{$(INCLUDE)}"\HTAlert.h"\
	{$(INCLUDE)}"\HTFormat.h"\
	{$(INCLUDE)}"\HTBind.h"\
	{$(INCLUDE)}"\HTStream.h"\
	{$(INCLUDE)}"\HTIOStream.h"\
	{$(INCLUDE)}"\HTFWrite.h"\
	{$(INCLUDE)}"\HTDNS.h"\
	{$(INCLUDE)}"\HTHost.h"\
	{$(INCLUDE)}"\HTNet.h"\
	{$(INCLUDE)}"\HTInet.h"\
	{$(INCLUDE)}"\HTTrans.h"\
	{$(INCLUDE)}"\HTProt.h"\
	{$(INCLUDE)}"\HTChannl.h"\
	{$(INCLUDE)}"\HTAABrow.h"\
	".\..\..\..\Library\src\HTMIMPrs.h"\
	{$(INCLUDE)}"\HTAAUtil.h"\
	{$(INCLUDE)}"\HTHeader.h"\
	{$(INCLUDE)}"\WWWLib.h"\
	{$(INCLUDE)}"\WWWApp.h"\
	{$(INCLUDE)}"\WWWTrans.h"\
	{$(INCLUDE)}"\WWWHTTP.h"\
	{$(INCLUDE)}"\WWWFile.h"\
	{$(INCLUDE)}"\WWWFTP.h"\
	{$(INCLUDE)}"\WWWGophe.h"\
	{$(INCLUDE)}"\WWWTelnt.h"\
	{$(INCLUDE)}"\WWWNews.h"\
	{$(INCLUDE)}"\WWWWAIS.h"\
	{$(INCLUDE)}"\WWWMIME.h"\
	{$(INCLUDE)}"\WWWHTML.h"\
	{$(INCLUDE)}"\WWWStream.h"\
	{$(INCLUDE)}"\WWWDir.h"\
	{$(INCLUDE)}"\WWWRules.h"\
	{$(INCLUDE)}"\HTEvntrg.h"\
	{$(INCLUDE)}"\HTHome.h"\
	{$(INCLUDE)}"\HTDialog.h"\
	{$(INCLUDE)}"\HTLog.h"\
	{$(INCLUDE)}"\HTHist.h"\
	{$(INCLUDE)}"\HTANSI.h"\
	{$(INCLUDE)}"\HTLocal.h"\
	{$(INCLUDE)}"\HTTCP.h"\
	{$(INCLUDE)}"\HTSocket.h"\
	{$(INCLUDE)}"\HTReader.h"\
	{$(INCLUDE)}"\HTWriter.h"\
	{$(INCLUDE)}"\HTBufWrt.h"\
	{$(INCLUDE)}"\HTMux.h"\
	{$(INCLUDE)}"\HTTPUtil.h"\
	{$(INCLUDE)}"\HTTP.h"\
	{$(INCLUDE)}"\HTTPServ.h"\
	{$(INCLUDE)}"\HTTPGen.h"\
	{$(INCLUDE)}"\HTTPReq.h"\
	{$(INCLUDE)}"\HTTPRes.h"\
	{$(INCLUDE)}"\HTTChunk.h"\
	{$(INCLUDE)}"\HTMulti.h"\
	{$(INCLUDE)}"\HTFile.h"\
	{$(INCLUDE)}"\HTFTP.h"\
	{$(INCLUDE)}"\HTFTPDir.h"\
	{$(INCLUDE)}"\HTGopher.h"\
	{$(INCLUDE)}"\HTTelnet.h"\
	{$(INCLUDE)}"\HTNews.h"\
	{$(INCLUDE)}"\HTNewsLs.h"\
	{$(INCLUDE)}"\HTNDir.h"\
	{$(INCLUDE)}"\HTMIME.h"\
	{$(INCLUDE)}"\HTMIMERq.h"\
	{$(INCLUDE)}"\HTBound.h"\
	{$(INCLUDE)}"\HTMulpar.h"\
	{$(INCLUDE)}"\HTMLPDTD.h"\
	{$(INCLUDE)}"\SGML.h"\
	{$(INCLUDE)}"\HTMLGen.h"\
	{$(INCLUDE)}"\HTTeXGen.h"\
	{$(INCLUDE)}"\HTStruct.h"\
	{$(INCLUDE)}"\HTConLen.h"\
	{$(INCLUDE)}"\HTGuess.h"\
	{$(INCLUDE)}"\HTTee.h"\
	{$(INCLUDE)}"\HTXParse.h"\
	{$(INCLUDE)}"\HTIcons.h"\
	{$(INCLUDE)}"\HTDescpt.h"\
	{$(INCLUDE)}"\HTDir.h"\
	{$(INCLUDE)}"\HTRules.h"\
	{$(INCLUDE)}"\HTProxy.h"\
	
NODEP_CPP_HTINI=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

!IF  "$(CFG)" == "webbot - Win32 Release"


"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HTInit.sbr" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\HTBInit.c"
DEP_CPP_HTBIN=\
	{$(INCLUDE)}"\sysdep.h"\
	{$(INCLUDE)}"\HTUtils.h"\
	{$(INCLUDE)}"\HTFormat.h"\
	{$(INCLUDE)}"\HTBind.h"\
	{$(INCLUDE)}"\HTBInit.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\HTMemory.h"\
	{$(INCLUDE)}"\HTStream.h"\
	{$(INCLUDE)}"\HTAtom.h"\
	{$(INCLUDE)}"\HTList.h"\
	{$(INCLUDE)}"\HTAnchor.h"\
	{$(INCLUDE)}"\HTReq.h"\
	{$(INCLUDE)}"\HTMethod.h"\
	{$(INCLUDE)}"\HTEvent.h"\
	{$(INCLUDE)}"\HTAssoc.h"\
	{$(INCLUDE)}"\HTError.h"\
	{$(INCLUDE)}"\HTNet.h"\
	{$(INCLUDE)}"\HTUser.h"\
	{$(INCLUDE)}"\HTTrans.h"\
	{$(INCLUDE)}"\HTHost.h"\
	{$(INCLUDE)}"\HTChannl.h"\
	{$(INCLUDE)}"\HTDNS.h"\
	{$(INCLUDE)}"\HTIOStream.h"\
	{$(INCLUDE)}"\WWWCore.h"\
	{$(INCLUDE)}"\HTAccess.h"\
	{$(INCLUDE)}"\HTParse.h"\
	{$(INCLUDE)}"\HTEscape.h"\
	{$(INCLUDE)}"\HTWWWStr.h"\
	{$(INCLUDE)}"\HTAlert.h"\
	{$(INCLUDE)}"\HTFWrite.h"\
	{$(INCLUDE)}"\HTInet.h"\
	{$(INCLUDE)}"\HTProt.h"\
	
NODEP_CPP_HTBIN=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "webbot - Win32 Release"


"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HTBInit.sbr" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\SGML.c"
DEP_CPP_SGML_=\
	{$(INCLUDE)}"\sysdep.h"\
	{$(INCLUDE)}"\HTUtils.h"\
	{$(INCLUDE)}"\HTString.h"\
	{$(INCLUDE)}"\HTChunk.h"\
	{$(INCLUDE)}"\SGML.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\HTMemory.h"\
	{$(INCLUDE)}"\HTStream.h"\
	{$(INCLUDE)}"\HTStruct.h"\
	{$(INCLUDE)}"\HTList.h"\
	
NODEP_CPP_SGML_=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "webbot - Win32 Release"


"$(INTDIR)\SGML.obj" : $(SOURCE) $(DEP_CPP_SGML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\SGML.obj" : $(SOURCE) $(DEP_CPP_SGML_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SGML.sbr" : $(SOURCE) $(DEP_CPP_SGML_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\HTML.c"
DEP_CPP_HTML_=\
	{$(INCLUDE)}"\sysdep.h"\
	{$(INCLUDE)}"\HTUtils.h"\
	{$(INCLUDE)}"\HTString.h"\
	{$(INCLUDE)}"\HTAtom.h"\
	{$(INCLUDE)}"\HTChunk.h"\
	{$(INCLUDE)}"\HText.h"\
	{$(INCLUDE)}"\HTStyle.h"\
	{$(INCLUDE)}"\HTAlert.h"\
	{$(INCLUDE)}"\HTMLGen.h"\
	{$(INCLUDE)}"\HTParse.h"\
	{$(INCLUDE)}"\HTML.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\HTMemory.h"\
	{$(INCLUDE)}"\HTList.h"\
	{$(INCLUDE)}"\HTAnchor.h"\
	{$(INCLUDE)}"\HTStruct.h"\
	{$(INCLUDE)}"\HTMethod.h"\
	{$(INCLUDE)}"\HTStream.h"\
	{$(INCLUDE)}"\HTFormat.h"\
	{$(INCLUDE)}"\HTReq.h"\
	{$(INCLUDE)}"\HTEvent.h"\
	{$(INCLUDE)}"\HTAssoc.h"\
	{$(INCLUDE)}"\HTError.h"\
	{$(INCLUDE)}"\HTNet.h"\
	{$(INCLUDE)}"\HTUser.h"\
	{$(INCLUDE)}"\HTTrans.h"\
	{$(INCLUDE)}"\HTHost.h"\
	{$(INCLUDE)}"\HTChannl.h"\
	{$(INCLUDE)}"\HTDNS.h"\
	{$(INCLUDE)}"\HTIOStream.h"\
	{$(INCLUDE)}"\HTEscape.h"\
	{$(INCLUDE)}"\HTMLPDTD.h"\
	{$(INCLUDE)}"\SGML.h"\
	
NODEP_CPP_HTML_=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "webbot - Win32 Release"


"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HTML.sbr" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\HTMLPDTD.c"
DEP_CPP_HTMLP=\
	{$(INCLUDE)}"\sysdep.h"\
	{$(INCLUDE)}"\HTUtils.h"\
	{$(INCLUDE)}"\HTMLPDTD.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\HTMemory.h"\
	{$(INCLUDE)}"\HTStruct.h"\
	{$(INCLUDE)}"\SGML.h"\
	{$(INCLUDE)}"\HTStream.h"\
	{$(INCLUDE)}"\HTList.h"\
	
NODEP_CPP_HTMLP=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "webbot - Win32 Release"


"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HTMLPDTD.sbr" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\HTPlain.c"
DEP_CPP_HTPLA=\
	{$(INCLUDE)}"\sysdep.h"\
	{$(INCLUDE)}"\HTUtils.h"\
	{$(INCLUDE)}"\HText.h"\
	{$(INCLUDE)}"\HTStyle.h"\
	{$(INCLUDE)}"\HTPlain.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\HTMemory.h"\
	{$(INCLUDE)}"\HTAnchor.h"\
	{$(INCLUDE)}"\HTStruct.h"\
	{$(INCLUDE)}"\HTList.h"\
	{$(INCLUDE)}"\HTAtom.h"\
	{$(INCLUDE)}"\HTMethod.h"\
	{$(INCLUDE)}"\HTStream.h"\
	{$(INCLUDE)}"\HTFormat.h"\
	{$(INCLUDE)}"\HTML.h"\
	{$(INCLUDE)}"\HTReq.h"\
	{$(INCLUDE)}"\HTEvent.h"\
	{$(INCLUDE)}"\HTAssoc.h"\
	{$(INCLUDE)}"\HTError.h"\
	{$(INCLUDE)}"\HTNet.h"\
	{$(INCLUDE)}"\HTUser.h"\
	{$(INCLUDE)}"\HTTrans.h"\
	{$(INCLUDE)}"\HTHost.h"\
	{$(INCLUDE)}"\HTChannl.h"\
	{$(INCLUDE)}"\HTDNS.h"\
	{$(INCLUDE)}"\HTIOStream.h"\
	{$(INCLUDE)}"\HTMLPDTD.h"\
	{$(INCLUDE)}"\SGML.h"\
	
NODEP_CPP_HTPLA=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "webbot - Win32 Release"


"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HTPlain.sbr" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\..\..\PICS-client\src\windows\Debug\pics.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\HTStyle.c"
DEP_CPP_HTSTY=\
	{$(INCLUDE)}"\sysdep.h"\
	{$(INCLUDE)}"\HTUtils.h"\
	{$(INCLUDE)}"\HTString.h"\
	{$(INCLUDE)}"\HTStyle.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\HTMemory.h"\
	{$(INCLUDE)}"\HTFormat.h"\
	{$(INCLUDE)}"\HTAnchor.h"\
	{$(INCLUDE)}"\HTML.h"\
	{$(INCLUDE)}"\HTStream.h"\
	{$(INCLUDE)}"\HTAtom.h"\
	{$(INCLUDE)}"\HTList.h"\
	{$(INCLUDE)}"\HTReq.h"\
	{$(INCLUDE)}"\HTEvent.h"\
	{$(INCLUDE)}"\HTAssoc.h"\
	{$(INCLUDE)}"\HTError.h"\
	{$(INCLUDE)}"\HTNet.h"\
	{$(INCLUDE)}"\HTUser.h"\
	{$(INCLUDE)}"\HTTrans.h"\
	{$(INCLUDE)}"\HTHost.h"\
	{$(INCLUDE)}"\HTChannl.h"\
	{$(INCLUDE)}"\HTDNS.h"\
	{$(INCLUDE)}"\HTIOStream.h"\
	{$(INCLUDE)}"\HTMethod.h"\
	{$(INCLUDE)}"\HTMLPDTD.h"\
	{$(INCLUDE)}"\HTStruct.h"\
	{$(INCLUDE)}"\SGML.h"\
	
NODEP_CPP_HTSTY=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "webbot - Win32 Release"


"$(INTDIR)\HTStyle.obj" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HTStyle.obj" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HTStyle.sbr" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
