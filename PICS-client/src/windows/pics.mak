# Microsoft Developer Studio Generated NMAKE File, Based on pics.dsp
!IF "$(CFG)" == ""
CFG=pics - Win32 Release
!MESSAGE No configuration specified. Defaulting to pics - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "pics - Win32 Release" && "$(CFG)" != "pics - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pics.mak" CFG="pics - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pics - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pics - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "pics - Win32 Release"

OUTDIR=.\..\..\..\Bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\..\..\Bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Release\pics.dll"

!ELSE 

ALL : "$(OUTDIR)\Release\pics.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\CSApp.obj"
	-@erase "$(INTDIR)\CSChkLab.obj"
	-@erase "$(INTDIR)\CSKwik.obj"
	-@erase "$(INTDIR)\CSLabel.obj"
	-@erase "$(INTDIR)\CSLLOut.obj"
	-@erase "$(INTDIR)\CSLLURLs.obj"
	-@erase "$(INTDIR)\CSMacRed.obj"
	-@erase "$(INTDIR)\CSMem.obj"
	-@erase "$(INTDIR)\CSParse.obj"
	-@erase "$(INTDIR)\CSStream.obj"
	-@erase "$(INTDIR)\CSUser.obj"
	-@erase "$(INTDIR)\CSUsrLst.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\pics.exp"
	-@erase "$(OUTDIR)\pics.lib"
	-@erase "$(OUTDIR)\Release\pics.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\Library\src" /I\
 "..\..\..\Library\External" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)\pics.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pics.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\pics.pdb" /machine:I386 /def:".\pics.def"\
 /out:"$(OUTDIR)\Release\pics.dll" /implib:"$(OUTDIR)\pics.lib" 
DEF_FILE= \
	".\pics.def"
LINK32_OBJS= \
	"$(INTDIR)\CSApp.obj" \
	"$(INTDIR)\CSChkLab.obj" \
	"$(INTDIR)\CSKwik.obj" \
	"$(INTDIR)\CSLabel.obj" \
	"$(INTDIR)\CSLLOut.obj" \
	"$(INTDIR)\CSLLURLs.obj" \
	"$(INTDIR)\CSMacRed.obj" \
	"$(INTDIR)\CSMem.obj" \
	"$(INTDIR)\CSParse.obj" \
	"$(INTDIR)\CSStream.obj" \
	"$(INTDIR)\CSUser.obj" \
	"$(INTDIR)\CSUsrLst.obj" \
	"..\..\..\Bin\wwwapp.lib" \
	"..\..\..\Bin\wwwcore.lib" \
	"..\..\..\Bin\wwwdll.lib" \
	"..\..\..\Bin\wwwmime.lib" \
	"..\..\..\Bin\wwwutils.lib"

"$(OUTDIR)\Release\pics.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

OUTDIR=.\..\..\..\Bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\..\..\Bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Debug\pics.dll"

!ELSE 

ALL : "$(OUTDIR)\Debug\pics.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\CSApp.obj"
	-@erase "$(INTDIR)\CSChkLab.obj"
	-@erase "$(INTDIR)\CSKwik.obj"
	-@erase "$(INTDIR)\CSLabel.obj"
	-@erase "$(INTDIR)\CSLLOut.obj"
	-@erase "$(INTDIR)\CSLLURLs.obj"
	-@erase "$(INTDIR)\CSMacRed.obj"
	-@erase "$(INTDIR)\CSMem.obj"
	-@erase "$(INTDIR)\CSParse.obj"
	-@erase "$(INTDIR)\CSStream.obj"
	-@erase "$(INTDIR)\CSUser.obj"
	-@erase "$(INTDIR)\CSUsrLst.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\Debug\pics.dll"
	-@erase "$(OUTDIR)\Debug\pics.ilk"
	-@erase "$(OUTDIR)\pics.exp"
	-@erase "$(OUTDIR)\pics.lib"
	-@erase "$(OUTDIR)\pics.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /I\
 "..\..\..\Library\External" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /Fp"$(INTDIR)\pics.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pics.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\pics.pdb" /debug /machine:I386 /def:".\pics.def"\
 /out:"$(OUTDIR)\Debug\pics.dll" /implib:"$(OUTDIR)\pics.lib" 
DEF_FILE= \
	".\pics.def"
LINK32_OBJS= \
	"$(INTDIR)\CSApp.obj" \
	"$(INTDIR)\CSChkLab.obj" \
	"$(INTDIR)\CSKwik.obj" \
	"$(INTDIR)\CSLabel.obj" \
	"$(INTDIR)\CSLLOut.obj" \
	"$(INTDIR)\CSLLURLs.obj" \
	"$(INTDIR)\CSMacRed.obj" \
	"$(INTDIR)\CSMem.obj" \
	"$(INTDIR)\CSParse.obj" \
	"$(INTDIR)\CSStream.obj" \
	"$(INTDIR)\CSUser.obj" \
	"$(INTDIR)\CSUsrLst.obj" \
	"..\..\..\Bin\wwwapp.lib" \
	"..\..\..\Bin\wwwcore.lib" \
	"..\..\..\Bin\wwwdll.lib" \
	"..\..\..\Bin\wwwmime.lib" \
	"..\..\..\Bin\wwwutils.lib"

"$(OUTDIR)\Debug\pics.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "pics - Win32 Release" || "$(CFG)" == "pics - Win32 Debug"
SOURCE=..\CSApp.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSAPP=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAccess.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
	"..\..\..\Library\src\HTChannl.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTDialog.h"\
	"..\..\..\Library\src\HTDNS.h"\
	"..\..\..\Library\src\HTError.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTEvent.h"\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HTFilter.h"\
	"..\..\..\Library\src\HTFormat.h"\
	"..\..\..\Library\src\HTFWrite.h"\
	"..\..\..\Library\src\HTHeader.h"\
	"..\..\..\Library\src\HTHist.h"\
	"..\..\..\Library\src\HTHome.h"\
	"..\..\..\Library\src\HTHost.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTInet.h"\
	"..\..\..\Library\src\HTIOStream.h"\
	"..\..\..\Library\src\HTLib.h"\
	"..\..\..\Library\src\HTLink.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTLog.h"\
	"..\..\..\Library\src\HTMemLog.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTMethod.h"\
	"..\..\..\Library\src\HTNet.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTProt.h"\
	"..\..\..\Library\src\HTProxy.h"\
	"..\..\..\Library\src\HTReq.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTRules.h"\
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
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLApp.h"\
	"..\CSLL.h"\
	"..\CSLUtils.h"\
	"..\CSParse.h"\
	"..\CSUser.h"\
	"..\CSUserSt.h"\
	"..\CSUsrLst.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSAPP=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSApp.obj" : $(SOURCE) $(DEP_CPP_CSAPP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSAPP=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAccess.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
	"..\..\..\Library\src\HTChannl.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTDialog.h"\
	"..\..\..\Library\src\HTDNS.h"\
	"..\..\..\Library\src\HTError.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTEvent.h"\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HTFilter.h"\
	"..\..\..\Library\src\HTFormat.h"\
	"..\..\..\Library\src\HTFWrite.h"\
	"..\..\..\Library\src\HTHeader.h"\
	"..\..\..\Library\src\HTHist.h"\
	"..\..\..\Library\src\HTHome.h"\
	"..\..\..\Library\src\HTHost.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTInet.h"\
	"..\..\..\Library\src\HTIOStream.h"\
	"..\..\..\Library\src\HTLib.h"\
	"..\..\..\Library\src\HTLink.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTLog.h"\
	"..\..\..\Library\src\HTMemLog.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTMethod.h"\
	"..\..\..\Library\src\HTNet.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTProt.h"\
	"..\..\..\Library\src\HTProxy.h"\
	"..\..\..\Library\src\HTReq.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTRules.h"\
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
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLApp.h"\
	"..\CSLL.h"\
	"..\CSLUtils.h"\
	"..\CSParse.h"\
	"..\CSUser.h"\
	"..\CSUserSt.h"\
	"..\CSUsrLst.h"\
	

"$(INTDIR)\CSApp.obj" : $(SOURCE) $(DEP_CPP_CSAPP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSChkLab.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSCHK=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLL.h"\
	"..\CSLLSt.h"\
	"..\CSLUtils.h"\
	"..\CSUser.h"\
	"..\CSUserSt.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSCHK=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSChkLab.obj" : $(SOURCE) $(DEP_CPP_CSCHK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSCHK=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLL.h"\
	"..\CSLLSt.h"\
	"..\CSLUtils.h"\
	"..\CSUser.h"\
	"..\CSUserSt.h"\
	

"$(INTDIR)\CSChkLab.obj" : $(SOURCE) $(DEP_CPP_CSCHK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSKwik.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSKWI=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSKWI=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSKwik.obj" : $(SOURCE) $(DEP_CPP_CSKWI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSKWI=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	

"$(INTDIR)\CSKwik.obj" : $(SOURCE) $(DEP_CPP_CSKWI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSLabel.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSLAB=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\CSLL.h"\
	"..\CSLLSt.h"\
	"..\CSLUtils.h"\
	"..\CSParse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSLAB=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLabel.obj" : $(SOURCE) $(DEP_CPP_CSLAB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSLAB=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\CSLL.h"\
	"..\CSLLSt.h"\
	"..\CSLUtils.h"\
	"..\CSParse.h"\
	

"$(INTDIR)\CSLabel.obj" : $(SOURCE) $(DEP_CPP_CSLAB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSLLOut.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSLLO=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLL.h"\
	"..\CSLLSt.h"\
	"..\CSLUtils.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSLLO=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLLOut.obj" : $(SOURCE) $(DEP_CPP_CSLLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSLLO=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLL.h"\
	"..\CSLLSt.h"\
	"..\CSLUtils.h"\
	

"$(INTDIR)\CSLLOut.obj" : $(SOURCE) $(DEP_CPP_CSLLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSLLURLs.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSLLU=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLL.h"\
	"..\CSLLSt.h"\
	"..\CSLUtils.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSLLU=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLLURLs.obj" : $(SOURCE) $(DEP_CPP_CSLLU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSLLU=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLL.h"\
	"..\CSLLSt.h"\
	"..\CSLUtils.h"\
	

"$(INTDIR)\CSLLURLs.obj" : $(SOURCE) $(DEP_CPP_CSLLU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSMacRed.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSMAC=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\CSLUtils.h"\
	"..\CSMR.h"\
	"..\CSMRSt.h"\
	"..\CSParse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSMAC=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSMacRed.obj" : $(SOURCE) $(DEP_CPP_CSMAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSMAC=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\CSLUtils.h"\
	"..\CSMR.h"\
	"..\CSMRSt.h"\
	"..\CSParse.h"\
	

"$(INTDIR)\CSMacRed.obj" : $(SOURCE) $(DEP_CPP_CSMAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSMem.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSMEM=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLUtils.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSMEM=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSMem.obj" : $(SOURCE) $(DEP_CPP_CSMEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSMEM=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLUtils.h"\
	

"$(INTDIR)\CSMem.obj" : $(SOURCE) $(DEP_CPP_CSMEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSParse.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSPAR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\CSLUtils.h"\
	"..\CSParse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSPAR=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSParse.obj" : $(SOURCE) $(DEP_CPP_CSPAR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSPAR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\CSLUtils.h"\
	"..\CSParse.h"\
	

"$(INTDIR)\CSParse.obj" : $(SOURCE) $(DEP_CPP_CSPAR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSStream.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSSTR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\HTProxy.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLApp.h"\
	"..\CSLL.h"\
	"..\CSLUtils.h"\
	"..\CSMR.h"\
	"..\CSParse.h"\
	"..\CSUser.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSSTR=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSStream.obj" : $(SOURCE) $(DEP_CPP_CSSTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSSTR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\HTProxy.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLApp.h"\
	"..\CSLL.h"\
	"..\CSLUtils.h"\
	"..\CSMR.h"\
	"..\CSParse.h"\
	"..\CSUser.h"\
	

"$(INTDIR)\CSStream.obj" : $(SOURCE) $(DEP_CPP_CSSTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSUser.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSUSE=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\CSLUtils.h"\
	"..\CSParse.h"\
	"..\CSUser.h"\
	"..\CSUserSt.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSUSE=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSUser.obj" : $(SOURCE) $(DEP_CPP_CSUSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSUSE=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemory.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\windows\config.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\CSLUtils.h"\
	"..\CSParse.h"\
	"..\CSUser.h"\
	"..\CSUserSt.h"\
	

"$(INTDIR)\CSUser.obj" : $(SOURCE) $(DEP_CPP_CSUSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSUsrLst.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSUSR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLApp.h"\
	"..\CSLUtils.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSUSR=\
	"..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSUsrLst.obj" : $(SOURCE) $(DEP_CPP_CSUSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSUSR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\Library\src\HTAlert.h"\
	"..\..\..\Library\src\HTAnchor.h"\
	"..\..\..\Library\src\HTArray.h"\
	"..\..\..\Library\src\HTAssoc.h"\
	"..\..\..\Library\src\HTAtom.h"\
	"..\..\..\Library\src\HTBind.h"\
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
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\wwwsys.h"\
	"..\..\..\Library\src\WWWUtil.h"\
	"..\CSLApp.h"\
	"..\CSLUtils.h"\
	

"$(INTDIR)\CSUsrLst.obj" : $(SOURCE) $(DEP_CPP_CSUSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

