# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
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
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

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

ALL : "$(OUTDIR)\pics.dll"

CLEAN : 
	-@erase "..\..\..\Bin\Release\pics.dll"
	-@erase ".\Release\CSKwik.obj"
	-@erase ".\Release\CSApp.obj"
	-@erase ".\Release\CSLLURLs.obj"
	-@erase ".\Release\CSUser.obj"
	-@erase ".\Release\CSMem.obj"
	-@erase ".\Release\CSStream.obj"
	-@erase ".\Release\CSChkLab.obj"
	-@erase ".\Release\CSParse.obj"
	-@erase ".\Release\CSMacRed.obj"
	-@erase ".\Release\CSLLOut.obj"
	-@erase ".\Release\CSLabel.obj"
	-@erase ".\Release\CSUsrLst.obj"
	-@erase "..\..\..\Bin\pics.lib"
	-@erase "..\..\..\Bin\pics.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\Library\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\..\Library\src" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC"\
 /Fp"$(INTDIR)/pics.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pics.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../Bin/Release/pics.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/pics.pdb" /machine:I386 /def:".\pics.def"\
 /out:"$(OUTDIR)/Release\pics.dll" /implib:"$(OUTDIR)/pics.lib" 
DEF_FILE= \
	".\pics.def"
LINK32_OBJS= \
	".\Release\CSKwik.obj" \
	".\Release\CSApp.obj" \
	".\Release\CSLLURLs.obj" \
	".\Release\CSUser.obj" \
	".\Release\CSMem.obj" \
	".\Release\CSStream.obj" \
	".\Release\CSChkLab.obj" \
	".\Release\CSParse.obj" \
	".\Release\CSMacRed.obj" \
	".\Release\CSLLOut.obj" \
	".\Release\CSLabel.obj" \
	".\Release\CSUsrLst.obj" \
	"..\..\..\Bin\wwwutils.lib" \
	"..\..\..\Bin\wwwmime.lib" \
	"..\..\..\Bin\wwwdll.lib" \
	"..\..\..\Bin\wwwcore.lib" \
	"..\..\..\Bin\wwwapp.lib"

"$(OUTDIR)\pics.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\..\..\Bin\Debug\pics.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\pics.dll"
	-@erase ".\Debug\CSMacRed.obj"
	-@erase ".\Debug\CSMem.obj"
	-@erase ".\Debug\CSUsrLst.obj"
	-@erase ".\Debug\CSApp.obj"
	-@erase ".\Debug\CSLLURLs.obj"
	-@erase ".\Debug\CSKwik.obj"
	-@erase ".\Debug\CSParse.obj"
	-@erase ".\Debug\CSLLOut.obj"
	-@erase ".\Debug\CSUser.obj"
	-@erase ".\Debug\CSStream.obj"
	-@erase ".\Debug\CSLabel.obj"
	-@erase ".\Debug\CSChkLab.obj"
	-@erase "..\..\..\Bin\Debug\pics.ilk"
	-@erase "..\..\..\Bin\pics.lib"
	-@erase "..\..\..\Bin\pics.exp"
	-@erase "..\..\..\Bin\pics.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /D "_DEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_DLL" /D "WWW_WIN_ASYNC"\
 /Fp"$(INTDIR)/pics.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pics.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../Bin/Debug/pics.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/pics.pdb" /debug /machine:I386 /def:".\pics.def"\
 /out:"$(OUTDIR)/Debug\pics.dll" /implib:"$(OUTDIR)/pics.lib" 
DEF_FILE= \
	".\pics.def"
LINK32_OBJS= \
	".\Debug\CSMacRed.obj" \
	".\Debug\CSMem.obj" \
	".\Debug\CSUsrLst.obj" \
	".\Debug\CSApp.obj" \
	".\Debug\CSLLURLs.obj" \
	".\Debug\CSKwik.obj" \
	".\Debug\CSParse.obj" \
	".\Debug\CSLLOut.obj" \
	".\Debug\CSUser.obj" \
	".\Debug\CSStream.obj" \
	".\Debug\CSLabel.obj" \
	".\Debug\CSChkLab.obj" \
	"..\..\..\Bin\wwwutils.lib" \
	"..\..\..\Bin\wwwmime.lib" \
	"..\..\..\Bin\wwwdll.lib" \
	"..\..\..\Bin\wwwcore.lib" \
	"..\..\..\Bin\wwwapp.lib"

"..\..\..\Bin\Debug\pics.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE="..\..\..\PICS-client\src\CSUsrLst.c"
DEP_CPP_CSUSR=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\CSLApp.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
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
	".\..\..\..\Library\src\HTUTree.h"\
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
	".\..\CSLUtils.h"\
	
NODEP_CPP_CSUSR=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSUsrLst.obj" : $(SOURCE) $(DEP_CPP_CSUSR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSChkLab.c"
DEP_CPP_CSCHK=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\CSLUtils.h"\
	".\..\CSUser.h"\
	".\..\CSUserSt.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
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
	".\..\..\..\Library\src\HTUTree.h"\
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
	
NODEP_CPP_CSCHK=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSChkLab.obj" : $(SOURCE) $(DEP_CPP_CSCHK) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSKwik.c"
DEP_CPP_CSKWI=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
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
	".\..\..\..\Library\src\HTUTree.h"\
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
	
NODEP_CPP_CSKWI=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSKwik.obj" : $(SOURCE) $(DEP_CPP_CSKWI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSLabel.c"
DEP_CPP_CSLAB=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\CSParse.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
	".\..\CSLUtils.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	
NODEP_CPP_CSLAB=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLabel.obj" : $(SOURCE) $(DEP_CPP_CSLAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSLLOut.c"
DEP_CPP_CSLLO=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\CSLUtils.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
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
	".\..\..\..\Library\src\HTUTree.h"\
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
	
NODEP_CPP_CSLLO=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLLOut.obj" : $(SOURCE) $(DEP_CPP_CSLLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSLLURLs.c"
DEP_CPP_CSLLU=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\CSLUtils.h"\
	".\..\CSLL.h"\
	".\..\CSLLSt.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
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
	".\..\..\..\Library\src\HTUTree.h"\
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
	
NODEP_CPP_CSLLU=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLLURLs.obj" : $(SOURCE) $(DEP_CPP_CSLLU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSMacRed.c"
DEP_CPP_CSMAC=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\CSParse.h"\
	".\..\CSMR.h"\
	".\..\CSMRSt.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\CSLUtils.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	
NODEP_CPP_CSMAC=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSMacRed.obj" : $(SOURCE) $(DEP_CPP_CSMAC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSMem.c"
DEP_CPP_CSMEM=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\CSLUtils.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
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
	".\..\..\..\Library\src\HTUTree.h"\
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
	
NODEP_CPP_CSMEM=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSMem.obj" : $(SOURCE) $(DEP_CPP_CSMEM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSParse.c"
DEP_CPP_CSPAR=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\CSLUtils.h"\
	".\..\CSParse.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	
NODEP_CPP_CSPAR=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSParse.obj" : $(SOURCE) $(DEP_CPP_CSPAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSStream.c"
DEP_CPP_CSSTR=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\CSLUtils.h"\
	".\..\CSMR.h"\
	".\..\CSUser.h"\
	".\..\CSLL.h"\
	".\..\CSLApp.h"\
	".\..\CSParse.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
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
	".\..\..\..\Library\src\HTUTree.h"\
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
	
NODEP_CPP_CSSTR=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSStream.obj" : $(SOURCE) $(DEP_CPP_CSSTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSUser.c"
DEP_CPP_CSUSE=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\CSParse.h"\
	".\..\CSUser.h"\
	".\..\CSUserSt.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\CSLUtils.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	
NODEP_CPP_CSUSE=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSUser.obj" : $(SOURCE) $(DEP_CPP_CSUSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\PICS-client\src\CSApp.c"
DEP_CPP_CSAPP=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\WWWApp.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\CSLApp.h"\
	".\..\CSParse.h"\
	".\..\CSLUtils.h"\
	".\..\CSLL.h"\
	".\..\CSUser.h"\
	".\..\CSUserSt.h"\
	".\..\CSUsrLst.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	{$(INCLUDE)}"\sys\STAT.H"\
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
	".\..\..\..\Library\src\HTUTree.h"\
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
	".\..\..\..\Library\src\HTEvntrg.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTHist.h"\
	
NODEP_CPP_CSAPP=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSApp.obj" : $(SOURCE) $(DEP_CPP_CSAPP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
