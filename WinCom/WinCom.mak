# Microsoft Developer Studio Generated NMAKE File, Based on WinCom.dsp
!IF "$(CFG)" == ""
CFG=WinCom - Win32 Release
!MESSAGE No configuration specified. Defaulting to WinCom - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "WinCom - Win32 Release" && "$(CFG)" != "WinCom - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinCom.mak" CFG="WinCom - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinCom - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WinCom - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinCom - Win32 Release"

OUTDIR=.\..\Bin\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\Bin\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WinCom.exe" ".\Release\WinCom.pch"

!ELSE 

ALL : "$(OUTDIR)\WinCom.exe" ".\Release\WinCom.pch"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Delete.obj"
	-@erase "$(INTDIR)\EntityInfo.obj"
	-@erase "$(INTDIR)\HTBInit.obj"
	-@erase "$(INTDIR)\HTInit.obj"
	-@erase "$(INTDIR)\HTProfil.obj"
	-@erase "$(INTDIR)\Links.obj"
	-@erase "$(INTDIR)\LinkView.obj"
	-@erase "$(INTDIR)\Listvwex.obj"
	-@erase "$(INTDIR)\Load.obj"
	-@erase "$(INTDIR)\Location.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\Password.obj"
	-@erase "$(INTDIR)\Progress.obj"
	-@erase "$(INTDIR)\ProxySetup.obj"
	-@erase "$(INTDIR)\Request.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TabsView.obj"
	-@erase "$(INTDIR)\UserParameters.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\VersionConflict.obj"
	-@erase "$(INTDIR)\WinCom.obj"
	-@erase "$(INTDIR)\WinCom.pch"
	-@erase "$(INTDIR)\WinComDoc.obj"
	-@erase "$(OUTDIR)\WinCom.exe"
	-@erase "..\Bin\Release\WinCom.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\Library\src" /I "..\Library\External"\
 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "WWW_WIN_DLL"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"..\Bin\Release\WinCom.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinCom.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\WinCom.pdb" /machine:I386 /out:"$(OUTDIR)\WinCom.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Delete.obj" \
	"$(INTDIR)\EntityInfo.obj" \
	"$(INTDIR)\HTBInit.obj" \
	"$(INTDIR)\HTInit.obj" \
	"$(INTDIR)\HTProfil.obj" \
	"$(INTDIR)\Links.obj" \
	"$(INTDIR)\LinkView.obj" \
	"$(INTDIR)\Listvwex.obj" \
	"$(INTDIR)\Load.obj" \
	"$(INTDIR)\Location.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\Password.obj" \
	"$(INTDIR)\Progress.obj" \
	"$(INTDIR)\ProxySetup.obj" \
	"$(INTDIR)\Request.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TabsView.obj" \
	"$(INTDIR)\UserParameters.obj" \
	"$(INTDIR)\VersionConflict.obj" \
	"$(INTDIR)\WinCom.obj" \
	"$(INTDIR)\WinComDoc.obj" \
	"..\Bin\Release\WinCom.res" \
	"..\Bin\wwwapp.lib" \
	"..\Bin\wwwcache.lib" \
	"..\Bin\wwwcore.lib" \
	"..\Bin\wwwdir.lib" \
	"..\Bin\wwwfile.lib" \
	"..\Bin\wwwftp.lib" \
	"..\Bin\wwwgophe.lib" \
	"..\Bin\wwwhtml.lib" \
	"..\Bin\wwwhttp.lib" \
	"..\Bin\wwwmime.lib" \
	"..\Bin\wwwnews.lib" \
	"..\Bin\wwwstream.lib" \
	"..\Bin\wwwtelnt.lib" \
	"..\Bin\wwwtrans.lib" \
	"..\Bin\wwwutils.lib" \
	"..\Bin\wwwzip.lib"

"$(OUTDIR)\WinCom.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WinCom - Win32 Debug"

OUTDIR=.\..\Bin\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\Bin\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WinCom.exe" ".\Debug\WinCom.pch"

!ELSE 

ALL : "$(OUTDIR)\WinCom.exe" ".\Debug\WinCom.pch"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Delete.obj"
	-@erase "$(INTDIR)\EntityInfo.obj"
	-@erase "$(INTDIR)\HTBInit.obj"
	-@erase "$(INTDIR)\HTInit.obj"
	-@erase "$(INTDIR)\HTProfil.obj"
	-@erase "$(INTDIR)\Links.obj"
	-@erase "$(INTDIR)\LinkView.obj"
	-@erase "$(INTDIR)\Listvwex.obj"
	-@erase "$(INTDIR)\Load.obj"
	-@erase "$(INTDIR)\Location.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\Password.obj"
	-@erase "$(INTDIR)\Progress.obj"
	-@erase "$(INTDIR)\ProxySetup.obj"
	-@erase "$(INTDIR)\Request.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TabsView.obj"
	-@erase "$(INTDIR)\UserParameters.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\VersionConflict.obj"
	-@erase "$(INTDIR)\WinCom.obj"
	-@erase "$(INTDIR)\WinCom.pch"
	-@erase "$(INTDIR)\WinComDoc.obj"
	-@erase "$(OUTDIR)\WinCom.exe"
	-@erase "$(OUTDIR)\WinCom.ilk"
	-@erase "$(OUTDIR)\WinCom.pdb"
	-@erase "..\Bin\Debug\WinCom.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Library\src" /I\
 "..\Library\External" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D\
 "WWW_WIN_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"..\Bin\Debug\WinCom.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinCom.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\WinCom.pdb" /debug /machine:I386 /out:"$(OUTDIR)\WinCom.exe"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Delete.obj" \
	"$(INTDIR)\EntityInfo.obj" \
	"$(INTDIR)\HTBInit.obj" \
	"$(INTDIR)\HTInit.obj" \
	"$(INTDIR)\HTProfil.obj" \
	"$(INTDIR)\Links.obj" \
	"$(INTDIR)\LinkView.obj" \
	"$(INTDIR)\Listvwex.obj" \
	"$(INTDIR)\Load.obj" \
	"$(INTDIR)\Location.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\Password.obj" \
	"$(INTDIR)\Progress.obj" \
	"$(INTDIR)\ProxySetup.obj" \
	"$(INTDIR)\Request.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TabsView.obj" \
	"$(INTDIR)\UserParameters.obj" \
	"$(INTDIR)\VersionConflict.obj" \
	"$(INTDIR)\WinCom.obj" \
	"$(INTDIR)\WinComDoc.obj" \
	"..\Bin\Debug\WinCom.res" \
	"..\Bin\wwwapp.lib" \
	"..\Bin\wwwcache.lib" \
	"..\Bin\wwwcore.lib" \
	"..\Bin\wwwdir.lib" \
	"..\Bin\wwwfile.lib" \
	"..\Bin\wwwftp.lib" \
	"..\Bin\wwwgophe.lib" \
	"..\Bin\wwwhtml.lib" \
	"..\Bin\wwwhttp.lib" \
	"..\Bin\wwwmime.lib" \
	"..\Bin\wwwnews.lib" \
	"..\Bin\wwwstream.lib" \
	"..\Bin\wwwtelnt.lib" \
	"..\Bin\wwwtrans.lib" \
	"..\Bin\wwwutils.lib" \
	"..\Bin\wwwzip.lib"

"$(OUTDIR)\WinCom.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "WinCom - Win32 Release" || "$(CFG)" == "WinCom - Win32 Debug"
SOURCE=.\Delete.cpp
DEP_CPP_DELET=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\Delete.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DELET=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Delete.obj" : $(SOURCE) $(DEP_CPP_DELET) "$(INTDIR)"


SOURCE=.\EntityInfo.cpp
DEP_CPP_ENTIT=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFile.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTMulti.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWFile.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\TabsView.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ENTIT=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\EntityInfo.obj" : $(SOURCE) $(DEP_CPP_ENTIT) "$(INTDIR)"


SOURCE=..\Library\src\HTBInit.c
DEP_CPP_HTBIN=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTBInit.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBIN=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Library\src\HTInit.c
DEP_CPP_HTINI=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAABrow.h"\
	"..\Library\src\HTAAUtil.h"\
	"..\Library\src\HTAccess.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTANSI.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTBound.h"\
	"..\Library\src\HTBufWrt.h"\
	"..\Library\src\HTCache.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTConLen.h"\
	"..\Library\src\HTDemux.h"\
	"..\Library\src\HTDescpt.h"\
	"..\Library\src\HTDialog.h"\
	"..\Library\src\HTDir.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTEvtLst.h"\
	"..\Library\src\HTFile.h"\
	"..\Library\src\HTFilter.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFTP.h"\
	"..\Library\src\HTFTPDir.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTGopher.h"\
	"..\Library\src\HTGuess.h"\
	"..\Library\src\HTHeader.h"\
	"..\Library\src\HTHist.h"\
	"..\Library\src\HTHome.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTIcons.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTInit.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTLocal.h"\
	"..\Library\src\HTLog.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMerge.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTMIME.h"\
	"..\Library\src\HTMIMERq.h"\
	"..\Library\src\HTMIMImp.h"\
	"..\Library\src\HTMLGen.h"\
	"..\Library\src\HTMLPDTD.h"\
	"..\Library\src\HTMulpar.h"\
	"..\Library\src\HTMulti.h"\
	"..\Library\src\HTMuxCh.h"\
	"..\Library\src\HTMuxHeader.h"\
	"..\Library\src\HTMuxTx.h"\
	"..\Library\src\HTNDir.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTNews.h"\
	"..\Library\src\HTNewsLs.h"\
	"..\Library\src\HTNewsRq.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTPEP.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTProxy.h"\
	"..\Library\src\HTReader.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTRules.h"\
	"..\Library\src\HTSChunk.h"\
	"..\Library\src\HTSocket.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTChunk.h"\
	"..\Library\src\HTTCP.h"\
	"..\Library\src\HTTee.h"\
	"..\Library\src\HTTelnet.h"\
	"..\Library\src\HTTeXGen.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTP.h"\
	"..\Library\src\HTTPGen.h"\
	"..\Library\src\HTTPReq.h"\
	"..\Library\src\HTTPRes.h"\
	"..\Library\src\HTTPServ.h"\
	"..\Library\src\HTTPUtil.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWAIS.h"\
	"..\Library\src\HTWriter.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\HTXParse.h"\
	"..\Library\src\HTZip.h"\
	"..\Library\src\SGML.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWApp.h"\
	"..\Library\src\WWWCache.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWDir.h"\
	"..\Library\src\WWWFile.h"\
	"..\Library\src\WWWFTP.h"\
	"..\Library\src\WWWGophe.h"\
	"..\Library\src\WWWHTML.h"\
	"..\Library\src\WWWHTTP.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\WWWMIME.h"\
	"..\Library\src\WWWMux.h"\
	"..\Library\src\WWWNews.h"\
	"..\Library\src\WWWStream.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWTelnt.h"\
	"..\Library\src\WWWTrans.h"\
	"..\Library\src\WWWUtil.h"\
	"..\Library\src\WWWWAIS.h"\
	"..\Library\src\WWWZip.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTINI=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Library\src\HTProfil.c
DEP_CPP_HTPRO=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAABrow.h"\
	"..\Library\src\HTAAUtil.h"\
	"..\Library\src\HTAccess.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTANSI.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTBInit.h"\
	"..\Library\src\HTBound.h"\
	"..\Library\src\HTBufWrt.h"\
	"..\Library\src\HTCache.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTConLen.h"\
	"..\Library\src\HTDescpt.h"\
	"..\Library\src\HTDialog.h"\
	"..\Library\src\HTDir.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTEvtLst.h"\
	"..\Library\src\HTFile.h"\
	"..\Library\src\HTFilter.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFTP.h"\
	"..\Library\src\HTFTPDir.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTGopher.h"\
	"..\Library\src\HTGuess.h"\
	"..\Library\src\HTHeader.h"\
	"..\Library\src\HTHInit.h"\
	"..\Library\src\HTHist.h"\
	"..\Library\src\HTHome.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTIcons.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTInit.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTLocal.h"\
	"..\Library\src\HTLog.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMerge.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTMIME.h"\
	"..\Library\src\HTMIMERq.h"\
	"..\Library\src\HTMIMImp.h"\
	"..\Library\src\HTML.h"\
	"..\Library\src\HTMLGen.h"\
	"..\Library\src\HTMLPDTD.h"\
	"..\Library\src\HTMulpar.h"\
	"..\Library\src\HTMulti.h"\
	"..\Library\src\HTNDir.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTNews.h"\
	"..\Library\src\HTNewsLs.h"\
	"..\Library\src\HTNewsRq.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTPEP.h"\
	"..\Library\src\HTPlain.h"\
	"..\Library\src\HTProfil.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTProxy.h"\
	"..\Library\src\HTReader.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTRules.h"\
	"..\Library\src\HTSChunk.h"\
	"..\Library\src\HTSocket.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTChunk.h"\
	"..\Library\src\HTTCP.h"\
	"..\Library\src\HTTee.h"\
	"..\Library\src\HTTelnet.h"\
	"..\Library\src\HTTeXGen.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTP.h"\
	"..\Library\src\HTTPGen.h"\
	"..\Library\src\HTTPReq.h"\
	"..\Library\src\HTTPRes.h"\
	"..\Library\src\HTTPServ.h"\
	"..\Library\src\HTTPUtil.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWAIS.h"\
	"..\Library\src\HTWriter.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\HTXParse.h"\
	"..\Library\src\HTZip.h"\
	"..\Library\src\SGML.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWApp.h"\
	"..\Library\src\WWWCache.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWDir.h"\
	"..\Library\src\WWWFile.h"\
	"..\Library\src\WWWFTP.h"\
	"..\Library\src\WWWGophe.h"\
	"..\Library\src\WWWHTML.h"\
	"..\Library\src\WWWHTTP.h"\
	"..\Library\src\WWWInit.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\WWWMIME.h"\
	"..\Library\src\WWWNews.h"\
	"..\Library\src\WWWStream.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWTelnt.h"\
	"..\Library\src\WWWTrans.h"\
	"..\Library\src\WWWUtil.h"\
	"..\Library\src\WWWWAIS.h"\
	"..\Library\src\WWWZip.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPRO=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Links.cpp
DEP_CPP_LINKS=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_LINKS=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Links.obj" : $(SOURCE) $(DEP_CPP_LINKS) "$(INTDIR)"


SOURCE=.\LinkView.cpp
DEP_CPP_LINKV=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_LINKV=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\LinkView.obj" : $(SOURCE) $(DEP_CPP_LINKV) "$(INTDIR)"


SOURCE=.\Listvwex.cpp
DEP_CPP_LISTV=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_LISTV=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Listvwex.obj" : $(SOURCE) $(DEP_CPP_LISTV) "$(INTDIR)"


SOURCE=.\Load.cpp
DEP_CPP_LOAD_=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Load.h"\
	".\Location.h"\
	".\MainFrm.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_LOAD_=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Load.obj" : $(SOURCE) $(DEP_CPP_LOAD_) "$(INTDIR)"


SOURCE=.\Location.cpp
DEP_CPP_LOCAT=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\MainFrm.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\TabsView.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_LOCAT=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Location.obj" : $(SOURCE) $(DEP_CPP_LOCAT) "$(INTDIR)"


SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\MainFrm.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MAINF=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


SOURCE=.\Password.cpp
DEP_CPP_PASSW=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\Password.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\UserParameters.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PASSW=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Password.obj" : $(SOURCE) $(DEP_CPP_PASSW) "$(INTDIR)"


SOURCE=.\Progress.cpp
DEP_CPP_PROGR=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PROGR=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Progress.obj" : $(SOURCE) $(DEP_CPP_PROGR) "$(INTDIR)"


SOURCE=.\ProxySetup.cpp
DEP_CPP_PROXY=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAccess.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDialog.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTEvtLst.h"\
	"..\Library\src\HTFilter.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHist.h"\
	"..\Library\src\HTHome.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTLog.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTProxy.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTRules.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWApp.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PROXY=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\ProxySetup.obj" : $(SOURCE) $(DEP_CPP_PROXY) "$(INTDIR)"


SOURCE=.\Request.cpp
DEP_CPP_REQUE=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAccess.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDialog.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTEvtLst.h"\
	"..\Library\src\HTFilter.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHist.h"\
	"..\Library\src\HTHome.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTLog.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTProxy.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTRules.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWApp.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\VersionConflict.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_REQUE=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Request.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "WinCom - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\Library\src" /I\
 "..\Library\External" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D\
 "WWW_WIN_DLL" /Fp"$(INTDIR)\WinCom.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\WinCom.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WinCom - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Library\src" /I\
 "..\Library\External" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D\
 "WWW_WIN_DLL" /Fp"$(INTDIR)\WinCom.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\WinCom.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TabsView.cpp
DEP_CPP_TABSV=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\TabsView.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TABSV=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\TabsView.obj" : $(SOURCE) $(DEP_CPP_TABSV) "$(INTDIR)"


SOURCE=.\UserParameters.cpp
DEP_CPP_USERP=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAccess.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDialog.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTEvtLst.h"\
	"..\Library\src\HTFilter.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHist.h"\
	"..\Library\src\HTHome.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTLog.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTProxy.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTRules.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWApp.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\MainFrm.h"\
	".\Password.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\UserParameters.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_USERP=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\UserParameters.obj" : $(SOURCE) $(DEP_CPP_USERP) "$(INTDIR)"


SOURCE=.\VersionConflict.cpp
DEP_CPP_VERSI=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\VersionConflict.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_VERSI=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\VersionConflict.obj" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"


SOURCE=.\WinCom.cpp
DEP_CPP_WINCO=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAABrow.h"\
	"..\Library\src\HTAAUtil.h"\
	"..\Library\src\HTAccess.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTANSI.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTBInit.h"\
	"..\Library\src\HTBound.h"\
	"..\Library\src\HTBufWrt.h"\
	"..\Library\src\HTCache.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTConLen.h"\
	"..\Library\src\HTDescpt.h"\
	"..\Library\src\HTDialog.h"\
	"..\Library\src\HTDir.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTEvtLst.h"\
	"..\Library\src\HTFile.h"\
	"..\Library\src\HTFilter.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFTP.h"\
	"..\Library\src\HTFTPDir.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTGopher.h"\
	"..\Library\src\HTGuess.h"\
	"..\Library\src\HTHeader.h"\
	"..\Library\src\HTHInit.h"\
	"..\Library\src\HTHist.h"\
	"..\Library\src\HTHome.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTIcons.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTInit.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTLocal.h"\
	"..\Library\src\HTLog.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMerge.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTMIME.h"\
	"..\Library\src\HTMIMERq.h"\
	"..\Library\src\HTMIMImp.h"\
	"..\Library\src\HTML.h"\
	"..\Library\src\HTMLGen.h"\
	"..\Library\src\HTMLPDTD.h"\
	"..\Library\src\HTMulpar.h"\
	"..\Library\src\HTMulti.h"\
	"..\Library\src\HTNDir.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTNews.h"\
	"..\Library\src\HTNewsLs.h"\
	"..\Library\src\HTNewsRq.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTPEP.h"\
	"..\Library\src\HTPlain.h"\
	"..\Library\src\HTProfil.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTProxy.h"\
	"..\Library\src\HTReader.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTRules.h"\
	"..\Library\src\HTSChunk.h"\
	"..\Library\src\HTSocket.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTChunk.h"\
	"..\Library\src\HTTCP.h"\
	"..\Library\src\HTTee.h"\
	"..\Library\src\HTTelnet.h"\
	"..\Library\src\HTTeXGen.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTP.h"\
	"..\Library\src\HTTPGen.h"\
	"..\Library\src\HTTPReq.h"\
	"..\Library\src\HTTPRes.h"\
	"..\Library\src\HTTPServ.h"\
	"..\Library\src\HTTPUtil.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWAIS.h"\
	"..\Library\src\HTWriter.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\HTXParse.h"\
	"..\Library\src\HTZip.h"\
	"..\Library\src\SGML.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWApp.h"\
	"..\Library\src\WWWCache.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWDir.h"\
	"..\Library\src\WWWFile.h"\
	"..\Library\src\WWWFTP.h"\
	"..\Library\src\WWWGophe.h"\
	"..\Library\src\WWWHTML.h"\
	"..\Library\src\WWWHTTP.h"\
	"..\Library\src\WWWInit.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\WWWMIME.h"\
	"..\Library\src\WWWNews.h"\
	"..\Library\src\WWWStream.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWTelnt.h"\
	"..\Library\src\WWWTrans.h"\
	"..\Library\src\WWWUtil.h"\
	"..\Library\src\WWWWAIS.h"\
	"..\Library\src\WWWZip.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Location.h"\
	".\MainFrm.h"\
	".\Password.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\TabsView.h"\
	".\UserParameters.h"\
	".\VersionConflict.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WINCO=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\WinCom.obj" : $(SOURCE) $(DEP_CPP_WINCO) "$(INTDIR)"


SOURCE=.\hlp\WinCom.hpj
USERDEP__WINCO="$(ProjDir)\hlp\AfxCore.rtf"	"$(ProjDir)\hlp\AfxPrint.rtf"	
SOURCE=.\WinCom.rc
DEP_RSC_WINCOM_=\
	".\res\Largeico.bmp"\
	".\res\Smallico.bmp"\
	".\res\Stateico.bmp"\
	".\res\Toolbar.bmp"\
	".\res\WinCom.ico"\
	".\res\WinCom.rc2"\
	".\res\WinComDoc.ico"\
	

!IF  "$(CFG)" == "WinCom - Win32 Release"


"..\Bin\Release\WinCom.res" : $(SOURCE) $(DEP_RSC_WINCOM_) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinCom - Win32 Debug"


"..\Bin\Debug\WinCom.res" : $(SOURCE) $(DEP_RSC_WINCOM_) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\WinComDoc.cpp
DEP_CPP_WINCOMD=\
	"..\Library\External\regex.h"\
	"..\Library\src\HTAccess.h"\
	"..\Library\src\HTAlert.h"\
	"..\Library\src\HTAnchor.h"\
	"..\Library\src\HTArray.h"\
	"..\Library\src\HTAssoc.h"\
	"..\Library\src\HTAtom.h"\
	"..\Library\src\HTBind.h"\
	"..\Library\src\HTChannl.h"\
	"..\Library\src\HTChunk.h"\
	"..\Library\src\HTDialog.h"\
	"..\Library\src\HTDNS.h"\
	"..\Library\src\HTError.h"\
	"..\Library\src\HTEscape.h"\
	"..\Library\src\HTEvent.h"\
	"..\Library\src\HTEvtLst.h"\
	"..\Library\src\HTFilter.h"\
	"..\Library\src\HTFormat.h"\
	"..\Library\src\HTFWrite.h"\
	"..\Library\src\HTHist.h"\
	"..\Library\src\HTHome.h"\
	"..\Library\src\HTHost.h"\
	"..\Library\src\HTHstMan.h"\
	"..\Library\src\HTInet.h"\
	"..\Library\src\HTIOStream.h"\
	"..\Library\src\HTLib.h"\
	"..\Library\src\HTLink.h"\
	"..\Library\src\HTList.h"\
	"..\Library\src\HTLog.h"\
	"..\Library\src\HTMemLog.h"\
	"..\Library\src\HTMemory.h"\
	"..\Library\src\HTMethod.h"\
	"..\Library\src\HTNet.h"\
	"..\Library\src\HTParse.h"\
	"..\Library\src\HTProt.h"\
	"..\Library\src\HTProxy.h"\
	"..\Library\src\HTReq.h"\
	"..\Library\src\HTResponse.h"\
	"..\Library\src\HTRules.h"\
	"..\Library\src\HTStream.h"\
	"..\Library\src\HTString.h"\
	"..\Library\src\HTStruct.h"\
	"..\Library\src\HTTimer.h"\
	"..\Library\src\HTTrans.h"\
	"..\Library\src\HTUser.h"\
	"..\Library\src\HTUtils.h"\
	"..\Library\src\HTUTree.h"\
	"..\Library\src\HTUU.h"\
	"..\Library\src\HTWWWStr.h"\
	"..\Library\src\windows\config.h"\
	"..\Library\src\WWWApp.h"\
	"..\Library\src\WWWCore.h"\
	"..\Library\src\WWWLib.h"\
	"..\Library\src\wwwsys.h"\
	"..\Library\src\WWWUtil.h"\
	".\Delete.h"\
	".\EntityInfo.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\Load.h"\
	".\Location.h"\
	".\Progress.h"\
	".\ProxySetup.h"\
	".\Request.h"\
	".\StdAfx.h"\
	".\WinCom.h"\
	".\WinComDoc.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WINCOMD=\
	"..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\WinComDoc.obj" : $(SOURCE) $(DEP_CPP_WINCOMD) "$(INTDIR)"



!ENDIF 

