# Microsoft Developer Studio Generated NMAKE File, Based on comline.dsp
!IF "$(CFG)" == ""
CFG=comline - Win32 Release
!MESSAGE No configuration specified. Defaulting to comline - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "comline - Win32 Release" && "$(CFG)" !=\
 "comline - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "comline.mak" CFG="comline - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "comline - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "comline - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "comline - Win32 Release"

OUTDIR=.\..\..\..\Bin\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\..\..\Bin\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\comline.exe"

!ELSE 

ALL : "$(OUTDIR)\comline.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\HTBInit.obj"
	-@erase "$(INTDIR)\HTInit.obj"
	-@erase "$(INTDIR)\HTLine.obj"
	-@erase "$(INTDIR)\HTMLPDTD.obj"
	-@erase "$(INTDIR)\HTProfil.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\comline.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\Library\src" /I\
 "..\..\..\Library\External" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "WWW_WIN_DLL" /Fp"$(INTDIR)\comline.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\comline.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\comline.pdb" /machine:I386 /out:"$(OUTDIR)\comline.exe" 
LINK32_OBJS= \
	"$(INTDIR)\HTBInit.obj" \
	"$(INTDIR)\HTInit.obj" \
	"$(INTDIR)\HTLine.obj" \
	"$(INTDIR)\HTMLPDTD.obj" \
	"$(INTDIR)\HTProfil.obj" \
	"..\..\..\Bin\wwwapp.lib" \
	"..\..\..\Bin\wwwcache.lib" \
	"..\..\..\Bin\wwwcore.lib" \
	"..\..\..\Bin\wwwdir.lib" \
	"..\..\..\Bin\wwwdll.lib" \
	"..\..\..\Bin\wwwfile.lib" \
	"..\..\..\Bin\wwwftp.lib" \
	"..\..\..\Bin\wwwgophe.lib" \
	"..\..\..\Bin\wwwhtml.lib" \
	"..\..\..\Bin\wwwhttp.lib" \
	"..\..\..\Bin\wwwmime.lib" \
	"..\..\..\Bin\wwwnews.lib" \
	"..\..\..\Bin\wwwstream.lib" \
	"..\..\..\Bin\wwwtelnt.lib" \
	"..\..\..\Bin\wwwtrans.lib" \
	"..\..\..\Bin\wwwutils.lib" \
	"..\..\..\Bin\wwwwais.lib" \
	"..\..\..\Bin\wwwzip.lib"

"$(OUTDIR)\comline.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "comline - Win32 Debug"

OUTDIR=.\..\..\..\Bin\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\..\..\Bin\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\comline.exe" "$(OUTDIR)\comline.bsc"

!ELSE 

ALL : "$(OUTDIR)\comline.exe" "$(OUTDIR)\comline.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\HTBInit.obj"
	-@erase "$(INTDIR)\HTBInit.sbr"
	-@erase "$(INTDIR)\HTInit.obj"
	-@erase "$(INTDIR)\HTInit.sbr"
	-@erase "$(INTDIR)\HTLine.obj"
	-@erase "$(INTDIR)\HTLine.sbr"
	-@erase "$(INTDIR)\HTMLPDTD.obj"
	-@erase "$(INTDIR)\HTMLPDTD.sbr"
	-@erase "$(INTDIR)\HTProfil.obj"
	-@erase "$(INTDIR)\HTProfil.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\comline.bsc"
	-@erase "$(OUTDIR)\comline.exe"
	-@erase "$(OUTDIR)\comline.ilk"
	-@erase "$(OUTDIR)\comline.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /I\
 "..\..\..\Library\External" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "WWW_WIN_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\comline.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\comline.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\HTBInit.sbr" \
	"$(INTDIR)\HTInit.sbr" \
	"$(INTDIR)\HTLine.sbr" \
	"$(INTDIR)\HTMLPDTD.sbr" \
	"$(INTDIR)\HTProfil.sbr"

"$(OUTDIR)\comline.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\comline.pdb" /debug /machine:I386 /out:"$(OUTDIR)\comline.exe" 
LINK32_OBJS= \
	"$(INTDIR)\HTBInit.obj" \
	"$(INTDIR)\HTInit.obj" \
	"$(INTDIR)\HTLine.obj" \
	"$(INTDIR)\HTMLPDTD.obj" \
	"$(INTDIR)\HTProfil.obj" \
	"..\..\..\Bin\wwwapp.lib" \
	"..\..\..\Bin\wwwcache.lib" \
	"..\..\..\Bin\wwwcore.lib" \
	"..\..\..\Bin\wwwdir.lib" \
	"..\..\..\Bin\wwwdll.lib" \
	"..\..\..\Bin\wwwfile.lib" \
	"..\..\..\Bin\wwwftp.lib" \
	"..\..\..\Bin\wwwgophe.lib" \
	"..\..\..\Bin\wwwhtml.lib" \
	"..\..\..\Bin\wwwhttp.lib" \
	"..\..\..\Bin\wwwmime.lib" \
	"..\..\..\Bin\wwwnews.lib" \
	"..\..\..\Bin\wwwstream.lib" \
	"..\..\..\Bin\wwwtelnt.lib" \
	"..\..\..\Bin\wwwtrans.lib" \
	"..\..\..\Bin\wwwutils.lib" \
	"..\..\..\Bin\wwwwais.lib" \
	"..\..\..\Bin\wwwzip.lib"

"$(OUTDIR)\comline.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "comline - Win32 Release" || "$(CFG)" ==\
 "comline - Win32 Debug"
SOURCE=..\..\..\Library\src\HTBInit.c
DEP_CPP_HTBIN=\
	"..\..\..\Library\External\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
	"..\..\..\Library\src\HTBInit.h"\
	"..\..\..\Library\src\HTChannl.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTDNS.h"\
	"..\..\..\Library\src\HTError.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTEvent.h"\
	"..\..\..\Library\src\HTFormat.h"\
	"..\..\..\Library\src\HTFWrite.h"\
	"..\..\..\Library\src\HTHost.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTInet.h"\
	"..\..\..\Library\src\HTIOStream.h"\
	"..\..\..\Library\src\HTLib.h"\
	"..\..\..\Library\src\HTLink.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemLog.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTMethod.h"\
	"..\..\..\Library\src\HTNet.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTProt.h"\
	"..\..\..\Library\src\HTReq.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStream.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTTimer.h"\
	"..\..\..\Library\src\HTTrans.h"\
	"..\..\..\Library\src\HTUser.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\HTUTree.h"\
	"..\..\..\Library\src\HTUU.h"\
	"..\..\..\Library\src\HTWWWStr.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBIN=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "comline - Win32 Release"


"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "comline - Win32 Debug"


"$(INTDIR)\HTBInit.obj"	"$(INTDIR)\HTBInit.sbr" : $(SOURCE) $(DEP_CPP_HTBIN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTInit.c
DEP_CPP_HTINI=\
	"..\..\..\Library\External\regex.h"\
	"..\..\..\Library\src\HTAABrow.h"\
	"..\..\..\Library\src\HTAAUtil.h"\
	"..\..\..\Library\src\HTAccess.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTANSI.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
	"..\..\..\Library\src\HTBound.h"\
	"..\..\..\Library\src\HTBufWrt.h"\
	"..\..\..\Library\src\HTCache.h"\
	"..\..\..\Library\src\HTChannl.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTConLen.h"\
	"..\..\..\Library\src\HTDemux.h"\
	"..\..\..\Library\src\HTDescpt.h"\
	"..\..\..\Library\src\HTDialog.h"\
	"..\..\..\Library\src\HTDir.h"\
	"..\..\..\Library\src\HTDNS.h"\
	"..\..\..\Library\src\HTError.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTEvent.h"\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HTFile.h"\
	"..\..\..\Library\src\HTFilter.h"\
	"..\..\..\Library\src\HTFormat.h"\
	"..\..\..\Library\src\HTFTP.h"\
	"..\..\..\Library\src\HTFTPDir.h"\
	"..\..\..\Library\src\HTFWrite.h"\
	"..\..\..\Library\src\HTGopher.h"\
	"..\..\..\Library\src\HTGuess.h"\
	"..\..\..\Library\src\HTHeader.h"\
	"..\..\..\Library\src\HTHist.h"\
	"..\..\..\Library\src\HTHome.h"\
	"..\..\..\Library\src\HTHost.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTIcons.h"\
	"..\..\..\Library\src\HTInet.h"\
	"..\..\..\Library\src\HTInit.h"\
	"..\..\..\Library\src\HTIOStream.h"\
	"..\..\..\Library\src\HTLib.h"\
	"..\..\..\Library\src\HTLink.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTLocal.h"\
	"..\..\..\Library\src\HTLog.h"\
	"..\..\..\Library\src\HTMemLog.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTMerge.h"\
	"..\..\..\Library\src\HTMethod.h"\
	"..\..\..\Library\src\HTMIME.h"\
	"..\..\..\Library\src\HTMIMERq.h"\
	"..\..\..\Library\src\HTMIMImp.h"\
	"..\..\..\Library\src\HTMLGen.h"\
	"..\..\..\Library\src\HTMLPDTD.h"\
	"..\..\..\Library\src\HTMulpar.h"\
	"..\..\..\Library\src\HTMulti.h"\
	"..\..\..\Library\src\HTMuxCh.h"\
	"..\..\..\Library\src\HTMuxHeader.h"\
	"..\..\..\Library\src\HTMuxTx.h"\
	"..\..\..\Library\src\HTNDir.h"\
	"..\..\..\Library\src\HTNet.h"\
	"..\..\..\Library\src\HTNews.h"\
	"..\..\..\Library\src\HTNewsLs.h"\
	"..\..\..\Library\src\HTNewsRq.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTPEP.h"\
	"..\..\..\Library\src\HTProt.h"\
	"..\..\..\Library\src\HTProxy.h"\
	"..\..\..\Library\src\HTReader.h"\
	"..\..\..\Library\src\HTReq.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTRules.h"\
	"..\..\..\Library\src\HTSChunk.h"\
	"..\..\..\Library\src\HTSocket.h"\
	"..\..\..\Library\src\HTStream.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTTChunk.h"\
	"..\..\..\Library\src\HTTCP.h"\
	"..\..\..\Library\src\HTTee.h"\
	"..\..\..\Library\src\HTTelnet.h"\
	"..\..\..\Library\src\HTTeXGen.h"\
	"..\..\..\Library\src\HTTimer.h"\
	"..\..\..\Library\src\HTTP.h"\
	"..\..\..\Library\src\HTTPGen.h"\
	"..\..\..\Library\src\HTTPReq.h"\
	"..\..\..\Library\src\HTTPRes.h"\
	"..\..\..\Library\src\HTTPServ.h"\
	"..\..\..\Library\src\HTTPUtil.h"\
	"..\..\..\Library\src\HTTrans.h"\
	"..\..\..\Library\src\HTUser.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\HTUTree.h"\
	"..\..\..\Library\src\HTUU.h"\
	"..\..\..\Library\src\HTWAIS.h"\
	"..\..\..\Library\src\HTWriter.h"\
	"..\..\..\Library\src\HTWWWStr.h"\
	"..\..\..\Library\src\HTXParse.h"\
	"..\..\..\Library\src\HTZip.h"\
	"..\..\..\Library\src\SGML.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWDir.h"\
	"..\..\..\Library\src\WWWFile.h"\
	"..\..\..\Library\src\WWWFTP.h"\
	"..\..\..\Library\src\WWWGophe.h"\
	"..\..\..\Library\src\WWWHTML.h"\
	"..\..\..\Library\src\WWWHTTP.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\WWWMIME.h"\
	"..\..\..\Library\src\WWWMux.h"\
	"..\..\..\Library\src\WWWNews.h"\
	"..\..\..\Library\src\WWWStream.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWTelnt.h"\
	"..\..\..\Library\src\WWWTrans.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\..\..\Library\src\WWWWAIS.h"\
	"..\..\..\Library\src\WWWZip.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTINI=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "comline - Win32 Release"


"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "comline - Win32 Debug"


"$(INTDIR)\HTInit.obj"	"$(INTDIR)\HTInit.sbr" : $(SOURCE) $(DEP_CPP_HTINI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\HTLine.c
DEP_CPP_HTLIN=\
	"..\..\..\Library\External\regex.h"\
	"..\..\..\Library\src\HTAABrow.h"\
	"..\..\..\Library\src\HTAAUtil.h"\
	"..\..\..\Library\src\HTAccess.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTANSI.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
	"..\..\..\Library\src\HTBInit.h"\
	"..\..\..\Library\src\HTBound.h"\
	"..\..\..\Library\src\HTBufWrt.h"\
	"..\..\..\Library\src\HTCache.h"\
	"..\..\..\Library\src\HTChannl.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTConLen.h"\
	"..\..\..\Library\src\HTDescpt.h"\
	"..\..\..\Library\src\HTDialog.h"\
	"..\..\..\Library\src\HTDir.h"\
	"..\..\..\Library\src\HTDNS.h"\
	"..\..\..\Library\src\HTError.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTEvent.h"\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTextImp.h"\
	"..\..\..\Library\src\HTFile.h"\
	"..\..\..\Library\src\HTFilter.h"\
	"..\..\..\Library\src\HTFormat.h"\
	"..\..\..\Library\src\HTFTP.h"\
	"..\..\..\Library\src\HTFTPDir.h"\
	"..\..\..\Library\src\HTFWrite.h"\
	"..\..\..\Library\src\HTGopher.h"\
	"..\..\..\Library\src\HTGuess.h"\
	"..\..\..\Library\src\HTHeader.h"\
	"..\..\..\Library\src\HTHInit.h"\
	"..\..\..\Library\src\HTHist.h"\
	"..\..\..\Library\src\HTHome.h"\
	"..\..\..\Library\src\HTHost.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTIcons.h"\
	"..\..\..\Library\src\HTInet.h"\
	"..\..\..\Library\src\HTInit.h"\
	"..\..\..\Library\src\HTIOStream.h"\
	"..\..\..\Library\src\HTLib.h"\
	"..\..\..\Library\src\HTLink.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTLocal.h"\
	"..\..\..\Library\src\HTLog.h"\
	"..\..\..\Library\src\HTMemLog.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTMerge.h"\
	"..\..\..\Library\src\HTMethod.h"\
	"..\..\..\Library\src\HTMIME.h"\
	"..\..\..\Library\src\HTMIMERq.h"\
	"..\..\..\Library\src\HTMIMImp.h"\
	"..\..\..\Library\src\HTML.h"\
	"..\..\..\Library\src\HTMLGen.h"\
	"..\..\..\Library\src\HTMLPDTD.h"\
	"..\..\..\Library\src\HTMulpar.h"\
	"..\..\..\Library\src\HTMulti.h"\
	"..\..\..\Library\src\HTNDir.h"\
	"..\..\..\Library\src\HTNet.h"\
	"..\..\..\Library\src\HTNews.h"\
	"..\..\..\Library\src\HTNewsLs.h"\
	"..\..\..\Library\src\HTNewsRq.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTPEP.h"\
	"..\..\..\Library\src\HTPlain.h"\
	"..\..\..\Library\src\HTProfil.h"\
	"..\..\..\Library\src\HTProt.h"\
	"..\..\..\Library\src\HTProxy.h"\
	"..\..\..\Library\src\HTReader.h"\
	"..\..\..\Library\src\HTReq.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTRules.h"\
	"..\..\..\Library\src\HTSChunk.h"\
	"..\..\..\Library\src\HTSocket.h"\
	"..\..\..\Library\src\HTStream.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTTChunk.h"\
	"..\..\..\Library\src\HTTCP.h"\
	"..\..\..\Library\src\HTTee.h"\
	"..\..\..\Library\src\HTTelnet.h"\
	"..\..\..\Library\src\HTTeXGen.h"\
	"..\..\..\Library\src\HTTimer.h"\
	"..\..\..\Library\src\HTTP.h"\
	"..\..\..\Library\src\HTTPGen.h"\
	"..\..\..\Library\src\HTTPReq.h"\
	"..\..\..\Library\src\HTTPRes.h"\
	"..\..\..\Library\src\HTTPServ.h"\
	"..\..\..\Library\src\HTTPUtil.h"\
	"..\..\..\Library\src\HTTrans.h"\
	"..\..\..\Library\src\HTUser.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\HTUTree.h"\
	"..\..\..\Library\src\HTUU.h"\
	"..\..\..\Library\src\HTWAIS.h"\
	"..\..\..\Library\src\HTWriter.h"\
	"..\..\..\Library\src\HTWWWStr.h"\
	"..\..\..\Library\src\HTXParse.h"\
	"..\..\..\Library\src\HTZip.h"\
	"..\..\..\Library\src\SGML.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWDir.h"\
	"..\..\..\Library\src\WWWFile.h"\
	"..\..\..\Library\src\WWWFTP.h"\
	"..\..\..\Library\src\WWWGophe.h"\
	"..\..\..\Library\src\WWWHTML.h"\
	"..\..\..\Library\src\WWWHTTP.h"\
	"..\..\..\Library\src\WWWInit.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\WWWMIME.h"\
	"..\..\..\Library\src\WWWNews.h"\
	"..\..\..\Library\src\WWWStream.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWTelnt.h"\
	"..\..\..\Library\src\WWWTrans.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\..\..\Library\src\WWWWAIS.h"\
	"..\..\..\Library\src\WWWZip.h"\
	"..\HTLine.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLIN=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "comline - Win32 Release"


"$(INTDIR)\HTLine.obj" : $(SOURCE) $(DEP_CPP_HTLIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "comline - Win32 Debug"


"$(INTDIR)\HTLine.obj"	"$(INTDIR)\HTLine.sbr" : $(SOURCE) $(DEP_CPP_HTLIN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTMLPDTD.c
DEP_CPP_HTMLP=\
	"..\..\..\Library\External\regex.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTMLPDTD.h"\
	"..\..\..\Library\src\HTStream.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\SGML.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMLP=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "comline - Win32 Release"


"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "comline - Win32 Debug"


"$(INTDIR)\HTMLPDTD.obj"	"$(INTDIR)\HTMLPDTD.sbr" : $(SOURCE) $(DEP_CPP_HTMLP)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTProfil.c
DEP_CPP_HTPRO=\
	"..\..\..\Library\External\regex.h"\
	"..\..\..\Library\src\HTAABrow.h"\
	"..\..\..\Library\src\HTAAUtil.h"\
	"..\..\..\Library\src\HTAccess.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTANSI.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
	"..\..\..\Library\src\HTBInit.h"\
	"..\..\..\Library\src\HTBound.h"\
	"..\..\..\Library\src\HTBufWrt.h"\
	"..\..\..\Library\src\HTCache.h"\
	"..\..\..\Library\src\HTChannl.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTConLen.h"\
	"..\..\..\Library\src\HTDescpt.h"\
	"..\..\..\Library\src\HTDialog.h"\
	"..\..\..\Library\src\HTDir.h"\
	"..\..\..\Library\src\HTDNS.h"\
	"..\..\..\Library\src\HTError.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTEvent.h"\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HTFile.h"\
	"..\..\..\Library\src\HTFilter.h"\
	"..\..\..\Library\src\HTFormat.h"\
	"..\..\..\Library\src\HTFTP.h"\
	"..\..\..\Library\src\HTFTPDir.h"\
	"..\..\..\Library\src\HTFWrite.h"\
	"..\..\..\Library\src\HTGopher.h"\
	"..\..\..\Library\src\HTGuess.h"\
	"..\..\..\Library\src\HTHeader.h"\
	"..\..\..\Library\src\HTHInit.h"\
	"..\..\..\Library\src\HTHist.h"\
	"..\..\..\Library\src\HTHome.h"\
	"..\..\..\Library\src\HTHost.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTIcons.h"\
	"..\..\..\Library\src\HTInet.h"\
	"..\..\..\Library\src\HTInit.h"\
	"..\..\..\Library\src\HTIOStream.h"\
	"..\..\..\Library\src\HTLib.h"\
	"..\..\..\Library\src\HTLink.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTLocal.h"\
	"..\..\..\Library\src\HTLog.h"\
	"..\..\..\Library\src\HTMemLog.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTMerge.h"\
	"..\..\..\Library\src\HTMethod.h"\
	"..\..\..\Library\src\HTMIME.h"\
	"..\..\..\Library\src\HTMIMERq.h"\
	"..\..\..\Library\src\HTMIMImp.h"\
	"..\..\..\Library\src\HTML.h"\
	"..\..\..\Library\src\HTMLGen.h"\
	"..\..\..\Library\src\HTMLPDTD.h"\
	"..\..\..\Library\src\HTMulpar.h"\
	"..\..\..\Library\src\HTMulti.h"\
	"..\..\..\Library\src\HTNDir.h"\
	"..\..\..\Library\src\HTNet.h"\
	"..\..\..\Library\src\HTNews.h"\
	"..\..\..\Library\src\HTNewsLs.h"\
	"..\..\..\Library\src\HTNewsRq.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTPEP.h"\
	"..\..\..\Library\src\HTPlain.h"\
	"..\..\..\Library\src\HTProfil.h"\
	"..\..\..\Library\src\HTProt.h"\
	"..\..\..\Library\src\HTProxy.h"\
	"..\..\..\Library\src\HTReader.h"\
	"..\..\..\Library\src\HTReq.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTRules.h"\
	"..\..\..\Library\src\HTSChunk.h"\
	"..\..\..\Library\src\HTSocket.h"\
	"..\..\..\Library\src\HTStream.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTTChunk.h"\
	"..\..\..\Library\src\HTTCP.h"\
	"..\..\..\Library\src\HTTee.h"\
	"..\..\..\Library\src\HTTelnet.h"\
	"..\..\..\Library\src\HTTeXGen.h"\
	"..\..\..\Library\src\HTTimer.h"\
	"..\..\..\Library\src\HTTP.h"\
	"..\..\..\Library\src\HTTPGen.h"\
	"..\..\..\Library\src\HTTPReq.h"\
	"..\..\..\Library\src\HTTPRes.h"\
	"..\..\..\Library\src\HTTPServ.h"\
	"..\..\..\Library\src\HTTPUtil.h"\
	"..\..\..\Library\src\HTTrans.h"\
	"..\..\..\Library\src\HTUser.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\HTUTree.h"\
	"..\..\..\Library\src\HTUU.h"\
	"..\..\..\Library\src\HTWAIS.h"\
	"..\..\..\Library\src\HTWriter.h"\
	"..\..\..\Library\src\HTWWWStr.h"\
	"..\..\..\Library\src\HTXParse.h"\
	"..\..\..\Library\src\HTZip.h"\
	"..\..\..\Library\src\SGML.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWDir.h"\
	"..\..\..\Library\src\WWWFile.h"\
	"..\..\..\Library\src\WWWFTP.h"\
	"..\..\..\Library\src\WWWGophe.h"\
	"..\..\..\Library\src\WWWHTML.h"\
	"..\..\..\Library\src\WWWHTTP.h"\
	"..\..\..\Library\src\WWWInit.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\WWWMIME.h"\
	"..\..\..\Library\src\WWWNews.h"\
	"..\..\..\Library\src\WWWStream.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWTelnt.h"\
	"..\..\..\Library\src\WWWTrans.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\..\..\Library\src\WWWWAIS.h"\
	"..\..\..\Library\src\WWWZip.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPRO=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

!IF  "$(CFG)" == "comline - Win32 Release"


"$(INTDIR)\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "comline - Win32 Debug"


"$(INTDIR)\HTProfil.obj"	"$(INTDIR)\HTProfil.sbr" : $(SOURCE) $(DEP_CPP_HTPRO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

