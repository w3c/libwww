# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=www - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to www - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "www - Win32 Release" && "$(CFG)" != "www - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "msvc_40.mak" CFG="www - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "www - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "www - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "www - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "www - Win32 Release"

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

ALL : "$(OUTDIR)\msvc_40.exe"

CLEAN : 
	-@erase ".\WinRel\msvc_40.exe"
	-@erase ".\WinRel\HTBInit.obj"
	-@erase ".\WinRel\HTML.obj"
	-@erase ".\WinRel\GridStyle.obj"
	-@erase ".\WinRel\HTPlain.obj"
	-@erase ".\WinRel\HTMLPDTD.obj"
	-@erase ".\WinRel\lib.obj"
	-@erase ".\WinRel\DefaultStyles.obj"
	-@erase ".\WinRel\www.obj"
	-@erase ".\WinRel\scroll.obj"
	-@erase ".\WinRel\HTInit.obj"
	-@erase ".\WinRel\HTBrowse.obj"
	-@erase ".\WinRel\GridText.obj"
	-@erase ".\WinRel\www.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\Library\Implementation" /I "..\..\..\Pics\Implementation" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\Library\Implementation" /I\
 "..\..\..\Pics\Implementation" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /Fp"$(INTDIR)/msvc_40.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/www.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msvc_40.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/msvc_40.pdb" /machine:I386 /out:"$(OUTDIR)/msvc_40.exe" 
LINK32_OBJS= \
	"$(INTDIR)/HTBInit.obj" \
	"$(INTDIR)/HTML.obj" \
	"$(INTDIR)/GridStyle.obj" \
	"$(INTDIR)/HTPlain.obj" \
	"$(INTDIR)/HTMLPDTD.obj" \
	"$(INTDIR)/lib.obj" \
	"$(INTDIR)/DefaultStyles.obj" \
	"$(INTDIR)/www.obj" \
	"$(INTDIR)/scroll.obj" \
	"$(INTDIR)/HTInit.obj" \
	"$(INTDIR)/HTBrowse.obj" \
	"$(INTDIR)/GridText.obj" \
	"$(INTDIR)/www.res" \
	"..\..\..\Library\Implementation\windows\wwwhttp\Debug\wwwhttp.lib" \
	"..\..\..\Library\Implementation\windows\wwwnews\Debug\wwwnews.lib" \
	"..\..\..\Library\Implementation\windows\wwwftp\Debug\wwwftp.lib" \
	"..\..\..\Library\Implementation\windows\wwwutils\Debug\wwwutils.lib" \
	"..\..\..\Library\Implementation\windows\wwwwais\Debug\wwwwais.lib" \
	"..\..\..\Library\Implementation\windows\wwwdir\Debug\wwwdir.lib" \
	"..\..\..\Library\Implementation\windows\wwwhtml\Debug\wwwhtml.lib" \
	"..\..\..\Library\Implementation\windows\wwwcache\Debug\wwwcache.lib" \
	"..\..\..\Library\Implementation\windows\wwwgophe\Debug\wwwgophe.lib" \
	"..\..\..\Library\Implementation\windows\wwwmime\Debug\wwwmime.lib" \
	"..\..\..\Library\Implementation\windows\wwwrules\Debug\wwwrules.lib" \
	"..\..\..\Library\Implementation\windows\wwwguess\Debug\wwwguess.lib" \
	"..\..\..\Library\Implementation\windows\wwwapp\Debug\wwwapp.lib" \
	"..\..\..\Library\Implementation\windows\wwwdll\Debug\wwwdll.lib" \
	"..\..\..\Library\Implementation\windows\wwwcore\Debug\wwwcore.lib" \
	"..\..\..\Library\Implementation\windows\wwwfile\Debug\wwwfile.lib" \
	"..\..\..\Library\Implementation\windows\wwwtelnt\Debug\wwwtelnt.lib"

"$(OUTDIR)\msvc_40.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

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

ALL : "$(OUTDIR)\msvc_40.exe"

CLEAN : 
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\vc40.idb"
	-@erase ".\WinDebug\msvc_40.exe"
	-@erase ".\WinDebug\HTInit.obj"
	-@erase ".\WinDebug\HTMLPDTD.obj"
	-@erase ".\WinDebug\HTBrowse.obj"
	-@erase ".\WinDebug\www.obj"
	-@erase ".\WinDebug\GridStyle.obj"
	-@erase ".\WinDebug\lib.obj"
	-@erase ".\WinDebug\GridText.obj"
	-@erase ".\WinDebug\DefaultStyles.obj"
	-@erase ".\WinDebug\HTPlain.obj"
	-@erase ".\WinDebug\scroll.obj"
	-@erase ".\WinDebug\HTML.obj"
	-@erase ".\WinDebug\HTBInit.obj"
	-@erase ".\WinDebug\www.res"
	-@erase ".\WinDebug\msvc_40.ilk"
	-@erase ".\WinDebug\msvc_40.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\Implementation" /I "..\..\..\Pics\Implementation" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\Implementation"\
 /I "..\..\..\Pics\Implementation" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /Fp"$(INTDIR)/msvc_40.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/www.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msvc_40.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/msvc_40.pdb" /debug /machine:I386 /out:"$(OUTDIR)/msvc_40.exe" 
LINK32_OBJS= \
	"$(INTDIR)/HTInit.obj" \
	"$(INTDIR)/HTMLPDTD.obj" \
	"$(INTDIR)/HTBrowse.obj" \
	"$(INTDIR)/www.obj" \
	"$(INTDIR)/GridStyle.obj" \
	"$(INTDIR)/lib.obj" \
	"$(INTDIR)/GridText.obj" \
	"$(INTDIR)/DefaultStyles.obj" \
	"$(INTDIR)/HTPlain.obj" \
	"$(INTDIR)/scroll.obj" \
	"$(INTDIR)/HTML.obj" \
	"$(INTDIR)/HTBInit.obj" \
	"$(INTDIR)/www.res" \
	"..\..\..\Library\Implementation\windows\wwwhttp\Debug\wwwhttp.lib" \
	"..\..\..\Library\Implementation\windows\wwwnews\Debug\wwwnews.lib" \
	"..\..\..\Library\Implementation\windows\wwwftp\Debug\wwwftp.lib" \
	"..\..\..\Library\Implementation\windows\wwwutils\Debug\wwwutils.lib" \
	"..\..\..\Library\Implementation\windows\wwwwais\Debug\wwwwais.lib" \
	"..\..\..\Library\Implementation\windows\wwwdir\Debug\wwwdir.lib" \
	"..\..\..\Library\Implementation\windows\wwwhtml\Debug\wwwhtml.lib" \
	"..\..\..\Library\Implementation\windows\wwwcache\Debug\wwwcache.lib" \
	"..\..\..\Library\Implementation\windows\wwwgophe\Debug\wwwgophe.lib" \
	"..\..\..\Library\Implementation\windows\wwwmime\Debug\wwwmime.lib" \
	"..\..\..\Library\Implementation\windows\wwwrules\Debug\wwwrules.lib" \
	"..\..\..\Library\Implementation\windows\wwwguess\Debug\wwwguess.lib" \
	"..\..\..\Library\Implementation\windows\wwwapp\Debug\wwwapp.lib" \
	"..\..\..\Library\Implementation\windows\wwwdll\Debug\wwwdll.lib" \
	"..\..\..\Library\Implementation\windows\wwwcore\Debug\wwwcore.lib" \
	"..\..\..\Library\Implementation\windows\wwwfile\Debug\wwwfile.lib" \
	"..\..\..\Library\Implementation\windows\wwwtelnt\Debug\wwwtelnt.lib"

"$(OUTDIR)\msvc_40.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "www - Win32 Release"
# Name "www - Win32 Debug"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\www.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_WWW_C=\
	".\..\windows\lib.h"\
	".\..\windows\scroll.h"\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\HTBrowse.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\WWWCore.h"\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTArray.h"\
	".\..\..\..\Library\Implementation\HTAssoc.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTChunk.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTString.h"\
	".\..\..\..\Library\Implementation\HTUU.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\Implementation\HTAccess.h"\
	".\..\..\..\Library\Implementation\HTAlert.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTBind.h"\
	".\..\..\..\Library\Implementation\HTConLen.h"\
	".\..\..\..\Library\Implementation\HTDNS.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTEscape.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTFWrite.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTParse.h"\
	".\..\..\..\Library\Implementation\HTProt.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTSocket.h"\
	".\..\..\..\Library\Implementation\HTTCP.h"\
	".\..\..\..\Library\Implementation\HTTee.h"\
	".\..\..\..\Library\Implementation\HTWWWStr.h"\
	".\..\..\..\Library\Implementation\HTWriter.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	
NODEP_CPP_WWW_C=\
	".\..\..\..\Library\Implementation\HTVMSUtils.h"\
	

"$(INTDIR)\www.obj" : $(SOURCE) $(DEP_CPP_WWW_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_WWW_C=\
	".\..\windows\lib.h"\
	".\..\windows\scroll.h"\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\HTBrowse.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\tcp.h"\
	

"$(INTDIR)\www.obj" : $(SOURCE) $(DEP_CPP_WWW_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lib.c
DEP_CPP_LIB_C=\
	".\..\windows\lib.h"\
	

"$(INTDIR)\lib.obj" : $(SOURCE) $(DEP_CPP_LIB_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\scroll.c
DEP_CPP_SCROL=\
	".\..\windows\lib.h"\
	".\..\windows\scroll.h"\
	

"$(INTDIR)\scroll.obj" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\GridStyle.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_GRIDS=\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\WWWCore.h"\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTArray.h"\
	".\..\..\..\Library\Implementation\HTAssoc.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTChunk.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTString.h"\
	".\..\..\..\Library\Implementation\HTUU.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\Implementation\HTAccess.h"\
	".\..\..\..\Library\Implementation\HTAlert.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTBind.h"\
	".\..\..\..\Library\Implementation\HTConLen.h"\
	".\..\..\..\Library\Implementation\HTDNS.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTEscape.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTFWrite.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTParse.h"\
	".\..\..\..\Library\Implementation\HTProt.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTSocket.h"\
	".\..\..\..\Library\Implementation\HTTCP.h"\
	".\..\..\..\Library\Implementation\HTTee.h"\
	".\..\..\..\Library\Implementation\HTWWWStr.h"\
	".\..\..\..\Library\Implementation\HTWriter.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	
NODEP_CPP_GRIDS=\
	".\..\..\..\Library\Implementation\HTVMSUtils.h"\
	

"$(INTDIR)\GridStyle.obj" : $(SOURCE) $(DEP_CPP_GRIDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_GRIDS=\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	

"$(INTDIR)\GridStyle.obj" : $(SOURCE) $(DEP_CPP_GRIDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\GridText.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_GRIDT=\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\..\..\Library\Implementation\WWWApp.h"\
	".\..\HTBrowse.h"\
	".\..\HTFont.h"\
	".\..\GridStyle.h"\
	".\..\GridText.h"\
	".\..\..\..\Library\Implementation\HTReqMan.h"\
	".\..\a_stdio.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\WWWCore.h"\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTArray.h"\
	".\..\..\..\Library\Implementation\HTAssoc.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTChunk.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTString.h"\
	".\..\..\..\Library\Implementation\HTUU.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\Implementation\HTAccess.h"\
	".\..\..\..\Library\Implementation\HTAlert.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTBind.h"\
	".\..\..\..\Library\Implementation\HTConLen.h"\
	".\..\..\..\Library\Implementation\HTDNS.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTEscape.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTFWrite.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTParse.h"\
	".\..\..\..\Library\Implementation\HTProt.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTSocket.h"\
	".\..\..\..\Library\Implementation\HTTCP.h"\
	".\..\..\..\Library\Implementation\HTTee.h"\
	".\..\..\..\Library\Implementation\HTWWWStr.h"\
	".\..\..\..\Library\Implementation\HTWriter.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTInit.h"\
	".\..\..\..\Library\Implementation\HTBInit.h"\
	".\..\..\..\Library\Implementation\HTHome.h"\
	".\..\..\..\Library\Implementation\HTRules.h"\
	".\..\..\..\Library\Implementation\HTProxy.h"\
	".\..\..\..\Library\Implementation\HTCache.h"\
	".\..\..\..\Library\Implementation\HTLog.h"\
	".\..\..\..\Library\Implementation\HTHist.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTPlain.h"\
	".\..\..\..\Library\Implementation\HTTeXGen.h"\
	".\..\..\..\Library\Implementation\HTMLGen.h"\
	".\..\..\..\Library\Implementation\HTMIME.h"\
	".\..\..\..\Library\Implementation\HTBound.h"\
	".\..\..\..\Library\Implementation\HTGuess.h"\
	".\..\..\..\Library\Implementation\HTWSRC.h"\
	".\..\..\..\Library\Implementation\HTNewsLs.h"\
	".\..\..\..\Library\Implementation\HTIcons.h"\
	".\..\..\..\Library\Implementation\HTTP.h"\
	".\..\..\..\Library\Implementation\HTFile.h"\
	".\..\..\..\Library\Implementation\HTFTP.h"\
	".\..\..\..\Library\Implementation\HTGopher.h"\
	".\..\..\..\Library\Implementation\HTTelnet.h"\
	".\..\..\..\Library\Implementation\HTNews.h"\
	".\..\..\..\Library\Implementation\HTWAIS.h"\
	".\..\..\..\Library\Implementation\HTDialog.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	".\..\..\..\Library\Implementation\HText.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	".\..\..\..\Library\Implementation\HTAABrow.h"\
	".\..\..\..\Library\Implementation\HTAAUtil.h"\
	
NODEP_CPP_GRIDT=\
	".\..\..\..\Library\Implementation\HTVMSUtils.h"\
	

"$(INTDIR)\GridText.obj" : $(SOURCE) $(DEP_CPP_GRIDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_GRIDT=\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\..\..\Library\Implementation\WWWApp.h"\
	".\..\HTBrowse.h"\
	".\..\HTFont.h"\
	".\..\GridStyle.h"\
	".\..\GridText.h"\
	".\..\..\..\Library\Implementation\HTReqMan.h"\
	".\..\a_stdio.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTInit.h"\
	".\..\..\..\Library\Implementation\HTBInit.h"\
	".\..\..\..\Library\Implementation\HTHome.h"\
	".\..\..\..\Library\Implementation\HTRules.h"\
	".\..\..\..\Library\Implementation\HTProxy.h"\
	".\..\..\..\Library\Implementation\HTCache.h"\
	".\..\..\..\Library\Implementation\HTLog.h"\
	".\..\..\..\Library\Implementation\HTHist.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTPlain.h"\
	".\..\..\..\Library\Implementation\HTTeXGen.h"\
	".\..\..\..\Library\Implementation\HTMLGen.h"\
	".\..\..\..\Library\Implementation\HTMIME.h"\
	".\..\..\..\Library\Implementation\HTBound.h"\
	".\..\..\..\Library\Implementation\HTGuess.h"\
	".\..\..\..\Library\Implementation\HTWSRC.h"\
	".\..\..\..\Library\Implementation\HTFWrite.h"\
	".\..\..\..\Library\Implementation\HTNewsLs.h"\
	".\..\..\..\Library\Implementation\HTIcons.h"\
	".\..\..\..\Library\Implementation\HTTP.h"\
	".\..\..\..\Library\Implementation\HTFile.h"\
	".\..\..\..\Library\Implementation\HTFTP.h"\
	".\..\..\..\Library\Implementation\HTGopher.h"\
	".\..\..\..\Library\Implementation\HTTelnet.h"\
	".\..\..\..\Library\Implementation\HTNews.h"\
	".\..\..\..\Library\Implementation\HTWAIS.h"\
	".\..\..\..\Library\Implementation\HTDialog.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	".\..\..\..\Library\Implementation\HText.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	".\..\..\..\Library\Implementation\HTAABrow.h"\
	".\..\..\..\Library\Implementation\HTSocket.h"\
	".\..\..\..\Library\Implementation\HTAAUtil.h"\
	".\..\..\..\Library\Implementation\HTAssoc.h"\
	

"$(INTDIR)\GridText.obj" : $(SOURCE) $(DEP_CPP_GRIDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBrowse.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTBRO=\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\..\..\Library\Implementation\WWWMIME.h"\
	".\..\..\..\Library\Implementation\WWWApp.h"\
	".\..\GridText.h"\
	".\..\HTBrowse.h"\
	".\..\a_stdio.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\WWWCore.h"\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTArray.h"\
	".\..\..\..\Library\Implementation\HTAssoc.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTChunk.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTString.h"\
	".\..\..\..\Library\Implementation\HTUU.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\Implementation\HTAccess.h"\
	".\..\..\..\Library\Implementation\HTAlert.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTBind.h"\
	".\..\..\..\Library\Implementation\HTConLen.h"\
	".\..\..\..\Library\Implementation\HTDNS.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTEscape.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTFWrite.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTParse.h"\
	".\..\..\..\Library\Implementation\HTProt.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTSocket.h"\
	".\..\..\..\Library\Implementation\HTTCP.h"\
	".\..\..\..\Library\Implementation\HTTee.h"\
	".\..\..\..\Library\Implementation\HTWWWStr.h"\
	".\..\..\..\Library\Implementation\HTWriter.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTMIME.h"\
	".\..\..\..\Library\Implementation\HTBound.h"\
	".\..\..\..\Library\Implementation\HTMulpar.h"\
	".\..\..\..\Library\Implementation\HTHeader.h"\
	".\..\..\..\Library\Implementation\HTInit.h"\
	".\..\..\..\Library\Implementation\HTBInit.h"\
	".\..\..\..\Library\Implementation\HTHome.h"\
	".\..\..\..\Library\Implementation\HTRules.h"\
	".\..\..\..\Library\Implementation\HTProxy.h"\
	".\..\..\..\Library\Implementation\HTCache.h"\
	".\..\..\..\Library\Implementation\HTLog.h"\
	".\..\..\..\Library\Implementation\HTHist.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTPlain.h"\
	".\..\..\..\Library\Implementation\HTTeXGen.h"\
	".\..\..\..\Library\Implementation\HTMLGen.h"\
	".\..\..\..\Library\Implementation\HTGuess.h"\
	".\..\..\..\Library\Implementation\HTWSRC.h"\
	".\..\..\..\Library\Implementation\HTNewsLs.h"\
	".\..\..\..\Library\Implementation\HTIcons.h"\
	".\..\..\..\Library\Implementation\HTTP.h"\
	".\..\..\..\Library\Implementation\HTFile.h"\
	".\..\..\..\Library\Implementation\HTFTP.h"\
	".\..\..\..\Library\Implementation\HTGopher.h"\
	".\..\..\..\Library\Implementation\HTTelnet.h"\
	".\..\..\..\Library\Implementation\HTNews.h"\
	".\..\..\..\Library\Implementation\HTWAIS.h"\
	".\..\..\..\Library\Implementation\HTDialog.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	".\..\..\..\Library\Implementation\HText.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	
NODEP_CPP_HTBRO=\
	".\..\..\..\Library\Implementation\HTVMSUtils.h"\
	

"$(INTDIR)\HTBrowse.obj" : $(SOURCE) $(DEP_CPP_HTBRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTBRO=\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\..\..\Library\Implementation\WWWMIME.h"\
	".\..\..\..\Library\Implementation\WWWApp.h"\
	".\..\GridText.h"\
	".\..\HTBrowse.h"\
	".\..\a_stdio.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTMIME.h"\
	".\..\..\..\Library\Implementation\HTBound.h"\
	".\..\..\..\Library\Implementation\HTMulpar.h"\
	".\..\..\..\Library\Implementation\HTHeader.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTInit.h"\
	".\..\..\..\Library\Implementation\HTBInit.h"\
	".\..\..\..\Library\Implementation\HTHome.h"\
	".\..\..\..\Library\Implementation\HTRules.h"\
	".\..\..\..\Library\Implementation\HTProxy.h"\
	".\..\..\..\Library\Implementation\HTCache.h"\
	".\..\..\..\Library\Implementation\HTLog.h"\
	".\..\..\..\Library\Implementation\HTHist.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTPlain.h"\
	".\..\..\..\Library\Implementation\HTTeXGen.h"\
	".\..\..\..\Library\Implementation\HTMLGen.h"\
	".\..\..\..\Library\Implementation\HTGuess.h"\
	".\..\..\..\Library\Implementation\HTWSRC.h"\
	".\..\..\..\Library\Implementation\HTFWrite.h"\
	".\..\..\..\Library\Implementation\HTNewsLs.h"\
	".\..\..\..\Library\Implementation\HTIcons.h"\
	".\..\..\..\Library\Implementation\HTTP.h"\
	".\..\..\..\Library\Implementation\HTFile.h"\
	".\..\..\..\Library\Implementation\HTFTP.h"\
	".\..\..\..\Library\Implementation\HTGopher.h"\
	".\..\..\..\Library\Implementation\HTTelnet.h"\
	".\..\..\..\Library\Implementation\HTNews.h"\
	".\..\..\..\Library\Implementation\HTWAIS.h"\
	".\..\..\..\Library\Implementation\HTDialog.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	".\..\..\..\Library\Implementation\HText.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	

"$(INTDIR)\HTBrowse.obj" : $(SOURCE) $(DEP_CPP_HTBRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\DefaultStyles.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_DEFAU=\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\GridStyle.h"\
	".\..\HTFont.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\WWWCore.h"\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTArray.h"\
	".\..\..\..\Library\Implementation\HTAssoc.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTChunk.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTString.h"\
	".\..\..\..\Library\Implementation\HTUU.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\Implementation\HTAccess.h"\
	".\..\..\..\Library\Implementation\HTAlert.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTBind.h"\
	".\..\..\..\Library\Implementation\HTConLen.h"\
	".\..\..\..\Library\Implementation\HTDNS.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTEscape.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTFWrite.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTParse.h"\
	".\..\..\..\Library\Implementation\HTProt.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTSocket.h"\
	".\..\..\..\Library\Implementation\HTTCP.h"\
	".\..\..\..\Library\Implementation\HTTee.h"\
	".\..\..\..\Library\Implementation\HTWWWStr.h"\
	".\..\..\..\Library\Implementation\HTWriter.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HText.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	
NODEP_CPP_DEFAU=\
	".\..\..\..\Library\Implementation\HTVMSUtils.h"\
	

"$(INTDIR)\DefaultStyles.obj" : $(SOURCE) $(DEP_CPP_DEFAU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_DEFAU=\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\GridStyle.h"\
	".\..\HTFont.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HText.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	

"$(INTDIR)\DefaultStyles.obj" : $(SOURCE) $(DEP_CPP_DEFAU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\www.rc
DEP_RSC_WWW_R=\
	".\..\windows\www.ico"\
	

"$(INTDIR)\www.res" : $(SOURCE) $(DEP_RSC_WWW_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwgophe\Debug\wwwgophe.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwdir\Debug\wwwdir.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwftp\Debug\wwwftp.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwtelnt\Debug\wwwtelnt.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwmime\Debug\wwwmime.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwhttp\Debug\wwwhttp.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwutils\Debug\wwwutils.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwnews\Debug\wwwnews.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwcore\Debug\wwwcore.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwdll\Debug\wwwdll.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwcache\Debug\wwwcache.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwrules\Debug\wwwrules.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwfile\Debug\wwwfile.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTPlain.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTPLA=\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HText.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	".\..\..\..\Library\Implementation\HTPlain.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	
NODEP_CPP_HTPLA=\
	".\..\..\..\Library\Implementation\HTVMSUtils.h"\
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTPLA=\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HText.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	".\..\..\..\Library\Implementation\HTPlain.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTML.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTML_=\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTString.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTChunk.h"\
	".\..\..\..\Library\Implementation\HText.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	".\..\..\..\Library\Implementation\HTAlert.h"\
	".\..\..\..\Library\Implementation\HTMLGen.h"\
	".\..\..\..\Library\Implementation\HTParse.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTEscape.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	
NODEP_CPP_HTML_=\
	".\..\..\..\Library\Implementation\HTVMSUtils.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTML_=\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTString.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTChunk.h"\
	".\..\..\..\Library\Implementation\HText.h"\
	".\..\..\..\Library\Implementation\HTStyle.h"\
	".\..\..\..\Library\Implementation\HTAlert.h"\
	".\..\..\..\Library\Implementation\HTMLGen.h"\
	".\..\..\..\Library\Implementation\HTParse.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\HTEscape.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwhtml\Debug\wwwhtml.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTMLPDTD.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTMLP=\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	
NODEP_CPP_HTMLP=\
	".\..\..\..\Library\Implementation\HTVMSUtils.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTMLP=\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTInit.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTINI=\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTProt.h"\
	".\..\..\..\Library\Implementation\HTInit.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTPlain.h"\
	".\..\..\..\Library\Implementation\HTTeXGen.h"\
	".\..\..\..\Library\Implementation\HTMLGen.h"\
	".\..\..\..\Library\Implementation\HTMIME.h"\
	".\..\..\..\Library\Implementation\HTBound.h"\
	".\..\..\..\Library\Implementation\HTGuess.h"\
	".\..\..\..\Library\Implementation\HTRules.h"\
	".\..\..\..\Library\Implementation\HTWSRC.h"\
	".\..\..\..\Library\Implementation\HTFWrite.h"\
	".\..\..\..\Library\Implementation\HTNewsLs.h"\
	".\..\..\..\Library\Implementation\HTIcons.h"\
	".\..\..\..\Library\Implementation\HTTP.h"\
	".\..\..\..\Library\Implementation\HTFile.h"\
	".\..\..\..\Library\Implementation\HTFTP.h"\
	".\..\..\..\Library\Implementation\HTGopher.h"\
	".\..\..\..\Library\Implementation\HTTelnet.h"\
	".\..\..\..\Library\Implementation\HTNews.h"\
	".\..\..\..\Library\Implementation\HTWAIS.h"\
	".\..\..\..\Library\Implementation\HTHome.h"\
	".\..\..\..\Library\Implementation\HTDialog.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\WWWCore.h"\
	".\..\..\..\Library\Implementation\HTArray.h"\
	".\..\..\..\Library\Implementation\HTAssoc.h"\
	".\..\..\..\Library\Implementation\HTChunk.h"\
	".\..\..\..\Library\Implementation\HTString.h"\
	".\..\..\..\Library\Implementation\HTUU.h"\
	".\..\..\..\Library\Implementation\HTAccess.h"\
	".\..\..\..\Library\Implementation\HTAlert.h"\
	".\..\..\..\Library\Implementation\HTBind.h"\
	".\..\..\..\Library\Implementation\HTConLen.h"\
	".\..\..\..\Library\Implementation\HTDNS.h"\
	".\..\..\..\Library\Implementation\HTEscape.h"\
	".\..\..\..\Library\Implementation\HTParse.h"\
	".\..\..\..\Library\Implementation\HTSocket.h"\
	".\..\..\..\Library\Implementation\HTTCP.h"\
	".\..\..\..\Library\Implementation\HTTee.h"\
	".\..\..\..\Library\Implementation\HTWWWStr.h"\
	".\..\..\..\Library\Implementation\HTWriter.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	
NODEP_CPP_HTINI=\
	".\..\..\..\Library\Implementation\HTVMSUtils.h"\
	

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTINI=\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTProt.h"\
	".\..\..\..\Library\Implementation\HTInit.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	".\..\..\..\Library\Implementation\WWWLib.h"\
	".\..\..\..\Library\Implementation\HTML.h"\
	".\..\..\..\Library\Implementation\HTPlain.h"\
	".\..\..\..\Library\Implementation\HTTeXGen.h"\
	".\..\..\..\Library\Implementation\HTMLGen.h"\
	".\..\..\..\Library\Implementation\HTMIME.h"\
	".\..\..\..\Library\Implementation\HTBound.h"\
	".\..\..\..\Library\Implementation\HTGuess.h"\
	".\..\..\..\Library\Implementation\HTRules.h"\
	".\..\..\..\Library\Implementation\HTWSRC.h"\
	".\..\..\..\Library\Implementation\HTFWrite.h"\
	".\..\..\..\Library\Implementation\HTNewsLs.h"\
	".\..\..\..\Library\Implementation\HTIcons.h"\
	".\..\..\..\Library\Implementation\HTTP.h"\
	".\..\..\..\Library\Implementation\HTFile.h"\
	".\..\..\..\Library\Implementation\HTFTP.h"\
	".\..\..\..\Library\Implementation\HTGopher.h"\
	".\..\..\..\Library\Implementation\HTTelnet.h"\
	".\..\..\..\Library\Implementation\HTNews.h"\
	".\..\..\..\Library\Implementation\HTWAIS.h"\
	".\..\..\..\Library\Implementation\HTHome.h"\
	".\..\..\..\Library\Implementation\HTDialog.h"\
	".\..\..\..\Library\Implementation\WWWUtil.h"\
	".\..\..\..\Library\Implementation\HTMLPDTD.h"\
	".\..\..\..\Library\Implementation\HTStruct.h"\
	".\..\..\..\Library\Implementation\SGML.h"\
	

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\HTBInit.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTBIN=\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTBind.h"\
	".\..\..\..\Library\Implementation\HTBInit.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	
NODEP_CPP_HTBIN=\
	".\..\..\..\Library\Implementation\HTVMSUtils.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTBIN=\
	".\..\..\..\Library\Implementation\tcp.h"\
	".\..\..\..\Library\Implementation\HTUtils.h"\
	".\..\..\..\Library\Implementation\HTFormat.h"\
	".\..\..\..\Library\Implementation\HTBind.h"\
	".\..\..\..\Library\Implementation\HTBInit.h"\
	".\..\..\..\Library\Implementation\HTStream.h"\
	".\..\..\..\Library\Implementation\HTAtom.h"\
	".\..\..\..\Library\Implementation\HTList.h"\
	".\..\..\..\Library\Implementation\HTAnchor.h"\
	".\..\..\..\Library\Implementation\HTReq.h"\
	".\..\..\..\Library\Implementation\HTMethod.h"\
	".\..\..\..\Library\Implementation\HTEvntrg.h"\
	".\..\..\..\Library\Implementation\HTError.h"\
	".\..\..\..\Library\Implementation\HTNet.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwapp\Debug\wwwapp.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwguess\Debug\wwwguess.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\Implementation\windows\wwwwais\Debug\wwwwais.lib

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
