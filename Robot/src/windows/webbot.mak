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
	-@erase ".\Release\SGML.obj"
	-@erase ".\Release\HTInit.obj"
	-@erase ".\Release\HTML.obj"
	-@erase ".\Release\HTHInit.obj"
	-@erase ".\Release\HTPlain.obj"
	-@erase ".\Release\HTBInit.obj"
	-@erase ".\Release\HTMLPDTD.obj"
	-@erase ".\Release\HTRobot.obj"
	-@erase ".\Release\HTStyle.obj"

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
	"$(INTDIR)/SGML.obj" \
	"$(INTDIR)/HTInit.obj" \
	"$(INTDIR)/HTML.obj" \
	"$(INTDIR)/HTHInit.obj" \
	"$(INTDIR)/HTPlain.obj" \
	"$(INTDIR)/HTBInit.obj" \
	"$(INTDIR)/HTMLPDTD.obj" \
	"$(INTDIR)/HTRobot.obj" \
	"$(INTDIR)/HTStyle.obj" \
	"..\..\..\Library\src\windows\wwwnews\Debug\wwwnews.lib" \
	"..\..\..\Library\src\windows\wwwcore\Debug\wwwcore.lib" \
	"..\..\..\Library\src\windows\wwwwais\Debug\wwwwais.lib" \
	"..\..\..\Library\src\windows\wwwutils\Debug\wwwutils.lib" \
	"..\..\..\Library\src\windows\wwwhtml\Debug\wwwhtml.lib" \
	"..\..\..\Library\src\windows\wwwmime\Debug\wwwmime.lib" \
	"..\..\..\Library\src\windows\wwwcache\Debug\wwwcache.lib" \
	"..\..\..\Library\src\windows\wwwdll\Debug\wwwdll.lib" \
	"..\..\..\Library\src\windows\wwwrules\Debug\wwwrules.lib" \
	"..\..\..\PICS-client\src\windows\Debug\pics.lib" \
	"..\..\..\Library\src\windows\wwwftp\Debug\wwwftp.lib" \
	"..\..\..\Library\src\windows\wwwstream\Debug\wwwstream.lib" \
	"..\..\..\Library\src\windows\wwwfile\Debug\wwwfile.lib" \
	"..\..\..\Library\src\windows\wwwtrans\Debug\wwwtrans.lib" \
	"..\..\..\Library\src\windows\wwwdir\Debug\wwwdir.lib" \
	"..\..\..\Library\src\windows\wwwtelnt\Debug\wwwtelnt.lib" \
	"..\..\..\Library\src\windows\wwwhttp\Debug\wwwhttp.lib" \
	"..\..\..\Library\src\windows\wwwapp\Debug\wwwapp.lib" \
	"..\..\..\Library\src\windows\wwwgophe\Debug\wwwgophe.lib"

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
	-@erase ".\Debug\HTHInit.sbr"
	-@erase ".\Debug\HTStyle.sbr"
	-@erase ".\Debug\HTBInit.sbr"
	-@erase ".\Debug\HTPlain.sbr"
	-@erase ".\Debug\SGML.sbr"
	-@erase ".\Debug\HTInit.sbr"
	-@erase ".\Debug\HTRobot.sbr"
	-@erase ".\Debug\HTML.sbr"
	-@erase ".\Debug\HTMLPDTD.sbr"
	-@erase "..\..\..\Bin\Debug\webbot.exe"
	-@erase ".\Debug\SGML.obj"
	-@erase ".\Debug\HTInit.obj"
	-@erase ".\Debug\HTRobot.obj"
	-@erase ".\Debug\HTML.obj"
	-@erase ".\Debug\HTMLPDTD.obj"
	-@erase ".\Debug\HTHInit.obj"
	-@erase ".\Debug\HTStyle.obj"
	-@erase ".\Debug\HTBInit.obj"
	-@erase ".\Debug\HTPlain.obj"
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
	"$(INTDIR)/HTHInit.sbr" \
	"$(INTDIR)/HTStyle.sbr" \
	"$(INTDIR)/HTBInit.sbr" \
	"$(INTDIR)/HTPlain.sbr" \
	"$(INTDIR)/SGML.sbr" \
	"$(INTDIR)/HTInit.sbr" \
	"$(INTDIR)/HTRobot.sbr" \
	"$(INTDIR)/HTML.sbr" \
	"$(INTDIR)/HTMLPDTD.sbr"

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
	"$(INTDIR)/HTInit.obj" \
	"$(INTDIR)/HTRobot.obj" \
	"$(INTDIR)/HTML.obj" \
	"$(INTDIR)/HTMLPDTD.obj" \
	"$(INTDIR)/HTHInit.obj" \
	"$(INTDIR)/HTStyle.obj" \
	"$(INTDIR)/HTBInit.obj" \
	"$(INTDIR)/HTPlain.obj" \
	"..\..\..\Library\src\windows\wwwnews\Debug\wwwnews.lib" \
	"..\..\..\Library\src\windows\wwwcore\Debug\wwwcore.lib" \
	"..\..\..\Library\src\windows\wwwwais\Debug\wwwwais.lib" \
	"..\..\..\Library\src\windows\wwwutils\Debug\wwwutils.lib" \
	"..\..\..\Library\src\windows\wwwhtml\Debug\wwwhtml.lib" \
	"..\..\..\Library\src\windows\wwwmime\Debug\wwwmime.lib" \
	"..\..\..\Library\src\windows\wwwcache\Debug\wwwcache.lib" \
	"..\..\..\Library\src\windows\wwwdll\Debug\wwwdll.lib" \
	"..\..\..\Library\src\windows\wwwrules\Debug\wwwrules.lib" \
	"..\..\..\PICS-client\src\windows\Debug\pics.lib" \
	"..\..\..\Library\src\windows\wwwftp\Debug\wwwftp.lib" \
	"..\..\..\Library\src\windows\wwwstream\Debug\wwwstream.lib" \
	"..\..\..\Library\src\windows\wwwfile\Debug\wwwfile.lib" \
	"..\..\..\Library\src\windows\wwwtrans\Debug\wwwtrans.lib" \
	"..\..\..\Library\src\windows\wwwdir\Debug\wwwdir.lib" \
	"..\..\..\Library\src\windows\wwwtelnt\Debug\wwwtelnt.lib" \
	"..\..\..\Library\src\windows\wwwhttp\Debug\wwwhttp.lib" \
	"..\..\..\Library\src\windows\wwwapp\Debug\wwwapp.lib" \
	"..\..\..\Library\src\windows\wwwgophe\Debug\wwwgophe.lib"

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

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTROB=\
	{$(INCLUDE)}"\WWWLib.h"\
	{$(INCLUDE)}"\WWWApp.h"\
	{$(INCLUDE)}"\WWWRules.h"\
	{$(INCLUDE)}"\WWWTrans.h"\
	{$(INCLUDE)}"\WWWInit.h"\
	{$(INCLUDE)}"\HText.h"\
	".\..\HTRobot.h"\
	{$(INCLUDE)}"\sysdep.h"\
	{$(INCLUDE)}"\HTEvntrg.h"\
	{$(INCLUDE)}"\HTEvent.h"\
	{$(INCLUDE)}"\HTReq.h"\
	{$(INCLUDE)}"\HTList.h"\
	{$(INCLUDE)}"\HTAssoc.h"\
	{$(INCLUDE)}"\HTFormat.h"\
	{$(INCLUDE)}"\HTStream.h"\
	{$(INCLUDE)}"\HTError.h"\
	{$(INCLUDE)}"\HTNet.h"\
	{$(INCLUDE)}"\HTUtils.h"\
	{$(INCLUDE)}"\HTAtom.h"\
	{$(INCLUDE)}"\HTAnchor.h"\
	{$(INCLUDE)}"\HTMemory.h"\
	{$(INCLUDE)}"\HTMethod.h"\
	{$(INCLUDE)}"\HTTrans.h"\
	{$(INCLUDE)}"\HTRules.h"\
	{$(INCLUDE)}"\HTProxy.h"\
	{$(INCLUDE)}"\HTInit.h"\
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
	{$(INCLUDE)}"\HTArray.h"\
	{$(INCLUDE)}"\HTReader.h"\
	{$(INCLUDE)}"\HTIOStream.h"\
	{$(INCLUDE)}"\HTChannl.h"\
	{$(INCLUDE)}"\HTIcons.h"\
	{$(INCLUDE)}"\HTDescpt.h"\
	{$(INCLUDE)}"\HTDir.h"\
	{$(INCLUDE)}"\HTStyle.h"\
	
NODEP_CPP_HTROB=\
	"..\..\..\Library\src\HTWAIS.h"\
	

"$(INTDIR)\HTRobot.obj" : $(SOURCE) $(DEP_CPP_HTROB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

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
	"..\..\..\Library\src\HTVMSUtils.h"\
	"..\..\..\Library\src\HTWAIS.h"\
	

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

SOURCE="..\..\..\Library\src\windows\wwwcache\Debug\wwwcache.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwcore\Debug\wwwcore.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwdir\Debug\wwwdir.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwdll\Debug\wwwdll.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwfile\Debug\wwwfile.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwftp\Debug\wwwftp.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwgophe\Debug\wwwgophe.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwhtml\Debug\wwwhtml.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwhttp\Debug\wwwhttp.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwmime\Debug\wwwmime.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwnews\Debug\wwwnews.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwrules\Debug\wwwrules.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwstream\Debug\wwwstream.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwtelnt\Debug\wwwtelnt.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwtrans\Debug\wwwtrans.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwutils\Debug\wwwutils.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwwais\Debug\wwwwais.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\HTHInit.c"

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTHIN=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	

"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_HTHIN=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	
NODEP_CPP_HTHIN=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

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

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTINI=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\WWWApp.h"\
	".\..\..\..\Library\src\WWWTrans.h"\
	".\..\..\..\Library\src\WWWHTTP.h"\
	".\..\..\..\Library\src\WWWFile.h"\
	".\..\..\..\Library\src\WWWFTP.h"\
	".\..\..\..\Library\src\WWWGophe.h"\
	".\..\..\..\Library\src\WWWTelnt.h"\
	".\..\..\..\Library\src\WWWNews.h"\
	".\..\..\..\Library\src\WWWWAIS.h"\
	".\..\..\..\Library\src\WWWMIME.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWStream.h"\
	".\..\..\..\Library\src\WWWDir.h"\
	".\..\..\..\Library\src\WWWRules.h"\
	".\..\..\..\Library\src\HTEvntrg.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTANSI.h"\
	".\..\..\..\Library\src\HTLocal.h"\
	".\..\..\..\Library\src\HTTCP.h"\
	".\..\..\..\Library\src\HTSocket.h"\
	".\..\..\..\Library\src\HTReader.h"\
	".\..\..\..\Library\src\HTWriter.h"\
	".\..\..\..\Library\src\HTBufWrt.h"\
	".\..\..\..\Library\src\HTMux.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTTPUtil.h"\
	".\..\..\..\Library\src\HTTP.h"\
	".\..\..\..\Library\src\HTTPServ.h"\
	".\..\..\..\Library\src\HTTPGen.h"\
	".\..\..\..\Library\src\HTTPReq.h"\
	".\..\..\..\Library\src\HTTPRes.h"\
	".\..\..\..\Library\src\HTTChunk.h"\
	".\..\..\..\Library\src\HTAAUtil.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFile.h"\
	".\..\..\..\Library\src\HTMulti.h"\
	".\..\..\..\Library\src\HTFTP.h"\
	".\..\..\..\Library\src\HTFTPDir.h"\
	".\..\..\..\Library\src\HTGopher.h"\
	".\..\..\..\Library\src\HTTelnet.h"\
	".\..\..\..\Library\src\HTNews.h"\
	".\..\..\..\Library\src\HTNewsLs.h"\
	".\..\..\..\Library\src\HTNDir.h"\
	".\..\..\..\Library\src\HTMIME.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTBound.h"\
	".\..\..\..\Library\src\HTMulpar.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTConLen.h"\
	".\..\..\..\Library\src\HTGuess.h"\
	".\..\..\..\Library\src\HTTee.h"\
	".\..\..\..\Library\src\HTSChunk.h"\
	".\..\..\..\Library\src\HTXParse.h"\
	".\..\..\..\Library\src\HTDir.h"\
	".\..\..\..\Library\src\HTIcons.h"\
	".\..\..\..\Library\src\HTDescpt.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	
NODEP_CPP_HTINI=\
	".\..\..\..\Library\src\HTWAIS.h"\
	

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_HTINI=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTInit.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\WWWApp.h"\
	".\..\..\..\Library\src\WWWTrans.h"\
	".\..\..\..\Library\src\WWWHTTP.h"\
	".\..\..\..\Library\src\WWWFile.h"\
	".\..\..\..\Library\src\WWWFTP.h"\
	".\..\..\..\Library\src\WWWGophe.h"\
	".\..\..\..\Library\src\WWWTelnt.h"\
	".\..\..\..\Library\src\WWWNews.h"\
	".\..\..\..\Library\src\WWWWAIS.h"\
	".\..\..\..\Library\src\WWWMIME.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWStream.h"\
	".\..\..\..\Library\src\WWWDir.h"\
	".\..\..\..\Library\src\WWWRules.h"\
	".\..\..\..\Library\src\HTEvntrg.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTANSI.h"\
	".\..\..\..\Library\src\HTLocal.h"\
	".\..\..\..\Library\src\HTTCP.h"\
	".\..\..\..\Library\src\HTSocket.h"\
	".\..\..\..\Library\src\HTReader.h"\
	".\..\..\..\Library\src\HTWriter.h"\
	".\..\..\..\Library\src\HTBufWrt.h"\
	".\..\..\..\Library\src\HTMux.h"\
	".\..\..\..\Library\src\HTTPUtil.h"\
	".\..\..\..\Library\src\HTTP.h"\
	".\..\..\..\Library\src\HTTPServ.h"\
	".\..\..\..\Library\src\HTTPGen.h"\
	".\..\..\..\Library\src\HTTPReq.h"\
	".\..\..\..\Library\src\HTTPRes.h"\
	".\..\..\..\Library\src\HTTChunk.h"\
	".\..\..\..\Library\src\HTAAUtil.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTFile.h"\
	".\..\..\..\Library\src\HTMulti.h"\
	".\..\..\..\Library\src\HTFTP.h"\
	".\..\..\..\Library\src\HTFTPDir.h"\
	".\..\..\..\Library\src\HTGopher.h"\
	".\..\..\..\Library\src\HTTelnet.h"\
	".\..\..\..\Library\src\HTNews.h"\
	".\..\..\..\Library\src\HTNewsLs.h"\
	".\..\..\..\Library\src\HTNDir.h"\
	".\..\..\..\Library\src\HTMIME.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTBound.h"\
	".\..\..\..\Library\src\HTMulpar.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTConLen.h"\
	".\..\..\..\Library\src\HTGuess.h"\
	".\..\..\..\Library\src\HTTee.h"\
	".\..\..\..\Library\src\HTSChunk.h"\
	".\..\..\..\Library\src\HTXParse.h"\
	".\..\..\..\Library\src\HTDir.h"\
	".\..\..\..\Library\src\HTIcons.h"\
	".\..\..\..\Library\src\HTDescpt.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	
NODEP_CPP_HTINI=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

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

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTBIN=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTBInit.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTUser.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_HTBIN=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTBInit.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	
NODEP_CPP_HTBIN=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

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

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_SGML_=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTList.h"\
	

"$(INTDIR)\SGML.obj" : $(SOURCE) $(DEP_CPP_SGML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_SGML_=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\SGML.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTList.h"\
	
NODEP_CPP_SGML_=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

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

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTML_=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\HText.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_HTML_=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\HText.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\HTML.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	
NODEP_CPP_HTML_=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

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

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTMLP=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTList.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_HTMLP=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTList.h"\
	
NODEP_CPP_HTMLP=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

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

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTPLA=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HText.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_HTPLA=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HText.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	
NODEP_CPP_HTPLA=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

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

SOURCE="..\..\..\PICS-client\src\windows\Debug\pics.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\HTStyle.c"

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTSTY=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	

"$(INTDIR)\HTStyle.obj" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_HTSTY=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	
NODEP_CPP_HTSTY=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HTStyle.obj" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HTStyle.sbr" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Library\src\windows\wwwapp\Debug\wwwapp.lib"

!IF  "$(CFG)" == "webbot - Win32 Release"

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
