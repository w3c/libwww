# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=pics - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to pics - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "pics - Win32 Release" && "$(CFG)" != "pics - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "pics.mak" CFG="pics - Win32 Debug"
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
################################################################################
# Begin Project
# PROP Target_Last_Scanned "pics - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "pics - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Bin"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\..\..\..\Bin
INTDIR=.\Release

ALL : "$(OUTDIR)\Release\pics.dll"

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
	-@erase "$(OUTDIR)\pics.exp"
	-@erase "$(OUTDIR)\pics.lib"
	-@erase "$(OUTDIR)\Release\pics.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\Library\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\Library\src" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC"\
 /Fp"$(INTDIR)/pics.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pics.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/pics.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/pics.pdb" /machine:I386 /def:".\pics.def"\
 /out:"$(OUTDIR)/Release\pics.dll" /implib:"$(OUTDIR)/pics.lib" 
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

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Bin"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\..\..\..\Bin
INTDIR=.\Debug

ALL : "$(OUTDIR)\Debug\pics.dll"

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
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Debug\pics.dll"
	-@erase "$(OUTDIR)\Debug\pics.ilk"
	-@erase "$(OUTDIR)\pics.exp"
	-@erase "$(OUTDIR)\pics.lib"
	-@erase "$(OUTDIR)\pics.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /D "_DEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC"\
 /Fp"$(INTDIR)/pics.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pics.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/pics.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/pics.pdb" /debug /machine:I386 /def:".\pics.def"\
 /out:"$(OUTDIR)/Debug\pics.dll" /implib:"$(OUTDIR)/pics.lib" 
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

# Name "pics - Win32 Release"
# Name "pics - Win32 Debug"

!IF  "$(CFG)" == "pics - Win32 Release"

!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE="..\CSUsrLst.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSUSR=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLApp.h"\
	".\..\CSLUtils.h"\
	

"$(INTDIR)\CSUsrLst.obj" : $(SOURCE) $(DEP_CPP_CSUSR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSUSR=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLApp.h"\
	".\..\CSLUtils.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSUSR=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSUsrLst.obj" : $(SOURCE) $(DEP_CPP_CSUSR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSChkLab.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSCHK=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\CSLUtils.h"\
	".\..\CSUser.h"\
	".\..\CSUserSt.h"\
	

"$(INTDIR)\CSChkLab.obj" : $(SOURCE) $(DEP_CPP_CSCHK) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSCHK=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\CSLUtils.h"\
	".\..\CSUser.h"\
	".\..\CSUserSt.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSCHK=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSChkLab.obj" : $(SOURCE) $(DEP_CPP_CSCHK) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSKwik.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSKWI=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	

"$(INTDIR)\CSKwik.obj" : $(SOURCE) $(DEP_CPP_CSKWI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSKWI=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSKWI=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSKwik.obj" : $(SOURCE) $(DEP_CPP_CSKWI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSLabel.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSLAB=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\CSLUtils.h"\
	".\..\CSParse.h"\
	

"$(INTDIR)\CSLabel.obj" : $(SOURCE) $(DEP_CPP_CSLAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSLAB=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\CSLUtils.h"\
	".\..\CSParse.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSLAB=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLabel.obj" : $(SOURCE) $(DEP_CPP_CSLAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSLLOut.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSLLO=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\CSLUtils.h"\
	

"$(INTDIR)\CSLLOut.obj" : $(SOURCE) $(DEP_CPP_CSLLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSLLO=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\CSLUtils.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSLLO=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLLOut.obj" : $(SOURCE) $(DEP_CPP_CSLLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSLLURLs.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSLLU=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\CSLUtils.h"\
	

"$(INTDIR)\CSLLURLs.obj" : $(SOURCE) $(DEP_CPP_CSLLU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSLLU=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\CSLUtils.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSLLU=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLLURLs.obj" : $(SOURCE) $(DEP_CPP_CSLLU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSMacRed.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSMAC=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\CSLUtils.h"\
	".\..\CSMR.h"\
	".\..\CSMRSt.h"\
	".\..\CSParse.h"\
	

"$(INTDIR)\CSMacRed.obj" : $(SOURCE) $(DEP_CPP_CSMAC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSMAC=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\CSLUtils.h"\
	".\..\CSMR.h"\
	".\..\CSMRSt.h"\
	".\..\CSParse.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSMAC=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSMacRed.obj" : $(SOURCE) $(DEP_CPP_CSMAC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSMem.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSMEM=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLUtils.h"\
	

"$(INTDIR)\CSMem.obj" : $(SOURCE) $(DEP_CPP_CSMEM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSMEM=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLUtils.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSMEM=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSMem.obj" : $(SOURCE) $(DEP_CPP_CSMEM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSParse.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSPAR=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\CSLUtils.h"\
	".\..\CSParse.h"\
	

"$(INTDIR)\CSParse.obj" : $(SOURCE) $(DEP_CPP_CSPAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSPAR=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\CSLUtils.h"\
	".\..\CSParse.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSPAR=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSParse.obj" : $(SOURCE) $(DEP_CPP_CSPAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSStream.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSSTR=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTProxy.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLApp.h"\
	".\..\CSLL.h"\
	".\..\CSLUtils.h"\
	".\..\CSMR.h"\
	".\..\CSParse.h"\
	".\..\CSUser.h"\
	

"$(INTDIR)\CSStream.obj" : $(SOURCE) $(DEP_CPP_CSSTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSSTR=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTProxy.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLApp.h"\
	".\..\CSLL.h"\
	".\..\CSLUtils.h"\
	".\..\CSMR.h"\
	".\..\CSParse.h"\
	".\..\CSUser.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSSTR=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSStream.obj" : $(SOURCE) $(DEP_CPP_CSSTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSUser.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSUSE=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\CSLUtils.h"\
	".\..\CSParse.h"\
	".\..\CSUser.h"\
	".\..\CSUserSt.h"\
	

"$(INTDIR)\CSUser.obj" : $(SOURCE) $(DEP_CPP_CSUSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSUSE=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\CSLUtils.h"\
	".\..\CSParse.h"\
	".\..\CSUser.h"\
	".\..\CSUserSt.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSUSE=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSUser.obj" : $(SOURCE) $(DEP_CPP_CSUSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\CSApp.c"

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSAPP=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HTHeader.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTProxy.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTRules.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLApp.h"\
	".\..\CSLL.h"\
	".\..\CSLUtils.h"\
	".\..\CSParse.h"\
	".\..\CSUser.h"\
	".\..\CSUserSt.h"\
	".\..\CSUsrLst.h"\
	

"$(INTDIR)\CSApp.obj" : $(SOURCE) $(DEP_CPP_CSAPP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSAPP=\
	"..\..\..\Library\src\HTChunk.h"\
	"..\..\..\Library\src\HTEscape.h"\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HTHeader.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTParse.h"\
	"..\..\..\Library\src\HTProxy.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTRules.h"\
	"..\..\..\Library\src\HTString.h"\
	"..\..\..\Library\src\HTStruct.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\sysdep.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\CSLApp.h"\
	".\..\CSLL.h"\
	".\..\CSLUtils.h"\
	".\..\CSParse.h"\
	".\..\CSUser.h"\
	".\..\CSUserSt.h"\
	".\..\CSUsrLst.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CSAPP=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSApp.obj" : $(SOURCE) $(DEP_CPP_CSAPP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pics.def

!IF  "$(CFG)" == "pics - Win32 Release"

!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwutils.lib"

!IF  "$(CFG)" == "pics - Win32 Release"

!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwmime.lib"

!IF  "$(CFG)" == "pics - Win32 Release"

!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwdll.lib"

!IF  "$(CFG)" == "pics - Win32 Release"

!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwcore.lib"

!IF  "$(CFG)" == "pics - Win32 Release"

!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwapp.lib"

!IF  "$(CFG)" == "pics - Win32 Release"

!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
