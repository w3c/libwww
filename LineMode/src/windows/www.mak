# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
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
!MESSAGE NMAKE /f "www.mak" CFG="www - Win32 Debug"
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
RSC=rc.exe
MTL=mktyplib.exe
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

ALL : "..\..\..\Bin\Release\www.exe"

CLEAN : 
	-@erase "..\..\..\Bin\Release\www.exe"
	-@erase ".\WinRel\DefaultStyles.obj"
	-@erase ".\WinRel\GridStyle.obj"
	-@erase ".\WinRel\GridText.obj"
	-@erase ".\WinRel\HTBInit.obj"
	-@erase ".\WinRel\HTBrowse.obj"
	-@erase ".\WinRel\HTHInit.obj"
	-@erase ".\WinRel\HTInit.obj"
	-@erase ".\WinRel\HTML.obj"
	-@erase ".\WinRel\HTMLPDTD.obj"
	-@erase ".\WinRel\HTPlain.obj"
	-@erase ".\WinRel\HTProfil.obj"
	-@erase ".\WinRel\lib.obj"
	-@erase ".\WinRel\scroll.obj"
	-@erase ".\WinRel\www.obj"
	-@erase ".\WinRel\www.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\Library\src" /I "..\..\..\PICS-client\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\Library\src" /I\
 "..\..\..\PICS-client\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /Fp"$(INTDIR)/www.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/www.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/www.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"../../../Bin/Release/www.exe"
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/www.pdb" /machine:I386 /out:"../../../Bin/Release/www.exe" 
LINK32_OBJS= \
	"..\..\..\Bin\pics.lib" \
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
	".\WinRel\DefaultStyles.obj" \
	".\WinRel\GridStyle.obj" \
	".\WinRel\GridText.obj" \
	".\WinRel\HTBInit.obj" \
	".\WinRel\HTBrowse.obj" \
	".\WinRel\HTHInit.obj" \
	".\WinRel\HTInit.obj" \
	".\WinRel\HTML.obj" \
	".\WinRel\HTMLPDTD.obj" \
	".\WinRel\HTPlain.obj" \
	".\WinRel\HTProfil.obj" \
	".\WinRel\lib.obj" \
	".\WinRel\scroll.obj" \
	".\WinRel\www.obj" \
	".\WinRel\www.res"

"..\..\..\Bin\Release\www.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\..\..\Bin\Debug\www.exe"

CLEAN : 
	-@erase "..\..\..\Bin\Debug\www.exe"
	-@erase "..\..\..\Bin\Debug\www.ilk"
	-@erase ".\WinDebug\DefaultStyles.obj"
	-@erase ".\WinDebug\GridStyle.obj"
	-@erase ".\WinDebug\GridText.obj"
	-@erase ".\WinDebug\HTBInit.obj"
	-@erase ".\WinDebug\HTBrowse.obj"
	-@erase ".\WinDebug\HTHInit.obj"
	-@erase ".\WinDebug\HTInit.obj"
	-@erase ".\WinDebug\HTML.obj"
	-@erase ".\WinDebug\HTMLPDTD.obj"
	-@erase ".\WinDebug\HTPlain.obj"
	-@erase ".\WinDebug\HTProfil.obj"
	-@erase ".\WinDebug\lib.obj"
	-@erase ".\WinDebug\scroll.obj"
	-@erase ".\WinDebug\vc40.idb"
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\www.obj"
	-@erase ".\WinDebug\www.pdb"
	-@erase ".\WinDebug\www.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /I "..\..\..\PICS-client\src" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /I\
 "..\..\..\PICS-client\src" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WWW_WIN_ASYNC" /D "WWW_WIN_DLL" /Fp"$(INTDIR)/www.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/www.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/www.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../../Bin/Debug/www.exe"
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/www.pdb" /debug /machine:I386 /out:"../../../Bin/Debug/www.exe"\
 
LINK32_OBJS= \
	"..\..\..\Bin\pics.lib" \
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
	".\WinDebug\DefaultStyles.obj" \
	".\WinDebug\GridStyle.obj" \
	".\WinDebug\GridText.obj" \
	".\WinDebug\HTBInit.obj" \
	".\WinDebug\HTBrowse.obj" \
	".\WinDebug\HTHInit.obj" \
	".\WinDebug\HTInit.obj" \
	".\WinDebug\HTML.obj" \
	".\WinDebug\HTMLPDTD.obj" \
	".\WinDebug\HTPlain.obj" \
	".\WinDebug\HTProfil.obj" \
	".\WinDebug\lib.obj" \
	".\WinDebug\scroll.obj" \
	".\WinDebug\www.obj" \
	".\WinDebug\www.res"

"..\..\..\Bin\Debug\www.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTReqMan.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTMIMPrs.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\HTBrowse.h"\
	".\..\windows\lib.h"\
	".\..\windows\scroll.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_WWW_C=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinRel\www.obj" : $(SOURCE) $(DEP_CPP_WWW_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_WWW_C=\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTReqMan.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTMIMPrs.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\HTBrowse.h"\
	".\..\windows\lib.h"\
	".\..\windows\scroll.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_WWW_C=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinDebug\www.obj" : $(SOURCE) $(DEP_CPP_WWW_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lib.c
DEP_CPP_LIB_C=\
	".\..\windows\lib.h"\
	

!IF  "$(CFG)" == "www - Win32 Release"


".\WinRel\lib.obj" : $(SOURCE) $(DEP_CPP_LIB_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "www - Win32 Debug"


".\WinDebug\lib.obj" : $(SOURCE) $(DEP_CPP_LIB_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scroll.c
DEP_CPP_SCROL=\
	".\..\windows\lib.h"\
	".\..\windows\scroll.h"\
	

!IF  "$(CFG)" == "www - Win32 Release"


".\WinRel\scroll.obj" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "www - Win32 Debug"


".\WinDebug\scroll.obj" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\GridStyle.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_GRIDS=\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_GRIDS=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinRel\GridStyle.obj" : $(SOURCE) $(DEP_CPP_GRIDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_GRIDS=\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_GRIDS=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinDebug\GridStyle.obj" : $(SOURCE) $(DEP_CPP_GRIDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\GridText.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_GRIDT=\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\a_stdio.h"\
	".\..\GridStyle.h"\
	".\..\GridText.h"\
	".\..\HTBrowse.h"\
	".\..\HTFont.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_GRIDT=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinRel\GridText.obj" : $(SOURCE) $(DEP_CPP_GRIDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_GRIDT=\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\a_stdio.h"\
	".\..\GridStyle.h"\
	".\..\GridText.h"\
	".\..\HTBrowse.h"\
	".\..\HTFont.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_GRIDT=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinDebug\GridText.obj" : $(SOURCE) $(DEP_CPP_GRIDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBrowse.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTBRO=\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemLog.h"\
	"..\..\..\Library\src\HTMerge.h"\
	"..\..\..\Library\src\HTNewsRq.h"\
	"..\..\..\Library\src\HTPEP.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWInit.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\WWWMIME.h"\
	"..\..\..\Library\src\WWWTrans.h"\
	"..\..\..\PICS-client\src\CSLApp.h"\
	"..\..\..\PICS-client\src\CSLL.h"\
	"..\..\..\PICS-client\src\CSUser.h"\
	"..\..\..\PICS-client\src\CSUsrLst.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTAAUtil.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTANSI.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTBInit.h"\
	".\..\..\..\Library\src\HTBound.h"\
	".\..\..\..\Library\src\HTBufWrt.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTConLen.h"\
	".\..\..\..\Library\src\HTDescpt.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTDir.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFile.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFTP.h"\
	".\..\..\..\Library\src\HTFTPDir.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTGopher.h"\
	".\..\..\..\Library\src\HTGuess.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTIcons.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTLocal.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTMIME.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTMulpar.h"\
	".\..\..\..\Library\src\HTMulti.h"\
	".\..\..\..\Library\src\HTNDir.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTNews.h"\
	".\..\..\..\Library\src\HTNewsLs.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTProfil.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HTReader.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTSChunk.h"\
	".\..\..\..\Library\src\HTSocket.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTChunk.h"\
	".\..\..\..\Library\src\HTTCP.h"\
	".\..\..\..\Library\src\HTTee.h"\
	".\..\..\..\Library\src\HTTelnet.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTTP.h"\
	".\..\..\..\Library\src\HTTPGen.h"\
	".\..\..\..\Library\src\HTTPReq.h"\
	".\..\..\..\Library\src\HTTPRes.h"\
	".\..\..\..\Library\src\HTTPServ.h"\
	".\..\..\..\Library\src\HTTPUtil.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWriter.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTXParse.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWDir.h"\
	".\..\..\..\Library\src\WWWFile.h"\
	".\..\..\..\Library\src\WWWFTP.h"\
	".\..\..\..\Library\src\WWWGophe.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWHTTP.h"\
	".\..\..\..\Library\src\WWWNews.h"\
	".\..\..\..\Library\src\WWWStream.h"\
	".\..\..\..\Library\src\WWWTelnt.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWWAIS.h"\
	".\..\..\..\PICS-client\src\CSLUtils.h"\
	".\..\a_stdio.h"\
	".\..\GridText.h"\
	".\..\HTBrowse.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTBRO=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

".\WinRel\HTBrowse.obj" : $(SOURCE) $(DEP_CPP_HTBRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTBRO=\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMemLog.h"\
	"..\..\..\Library\src\HTMerge.h"\
	"..\..\..\Library\src\HTNewsRq.h"\
	"..\..\..\Library\src\HTPEP.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWInit.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\WWWMIME.h"\
	"..\..\..\Library\src\WWWTrans.h"\
	"..\..\..\PICS-client\src\CSLApp.h"\
	"..\..\..\PICS-client\src\CSLL.h"\
	"..\..\..\PICS-client\src\CSUser.h"\
	"..\..\..\PICS-client\src\CSUsrLst.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTAAUtil.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTANSI.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTBInit.h"\
	".\..\..\..\Library\src\HTBound.h"\
	".\..\..\..\Library\src\HTBufWrt.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTConLen.h"\
	".\..\..\..\Library\src\HTDescpt.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTDir.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFile.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFTP.h"\
	".\..\..\..\Library\src\HTFTPDir.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTGopher.h"\
	".\..\..\..\Library\src\HTGuess.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTIcons.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTLocal.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTMIME.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTMulpar.h"\
	".\..\..\..\Library\src\HTMulti.h"\
	".\..\..\..\Library\src\HTNDir.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTNews.h"\
	".\..\..\..\Library\src\HTNewsLs.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTProfil.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HTReader.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTSChunk.h"\
	".\..\..\..\Library\src\HTSocket.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTChunk.h"\
	".\..\..\..\Library\src\HTTCP.h"\
	".\..\..\..\Library\src\HTTee.h"\
	".\..\..\..\Library\src\HTTelnet.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTTP.h"\
	".\..\..\..\Library\src\HTTPGen.h"\
	".\..\..\..\Library\src\HTTPReq.h"\
	".\..\..\..\Library\src\HTTPRes.h"\
	".\..\..\..\Library\src\HTTPServ.h"\
	".\..\..\..\Library\src\HTTPUtil.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWriter.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTXParse.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWDir.h"\
	".\..\..\..\Library\src\WWWFile.h"\
	".\..\..\..\Library\src\WWWFTP.h"\
	".\..\..\..\Library\src\WWWGophe.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWHTTP.h"\
	".\..\..\..\Library\src\WWWNews.h"\
	".\..\..\..\Library\src\WWWStream.h"\
	".\..\..\..\Library\src\WWWTelnt.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWWAIS.h"\
	".\..\..\..\PICS-client\src\CSLUtils.h"\
	".\..\a_stdio.h"\
	".\..\GridText.h"\
	".\..\HTBrowse.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTBRO=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

".\WinDebug\HTBrowse.obj" : $(SOURCE) $(DEP_CPP_HTBRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\DefaultStyles.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_DEFAU=\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\GridStyle.h"\
	".\..\HTFont.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_DEFAU=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinRel\DefaultStyles.obj" : $(SOURCE) $(DEP_CPP_DEFAU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_DEFAU=\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\GridStyle.h"\
	".\..\HTFont.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_DEFAU=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinDebug\DefaultStyles.obj" : $(SOURCE) $(DEP_CPP_DEFAU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\www.rc
DEP_RSC_WWW_R=\
	".\WWW.ICO"\
	

!IF  "$(CFG)" == "www - Win32 Release"


".\WinRel\www.res" : $(SOURCE) $(DEP_RSC_WWW_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"


".\WinDebug\www.res" : $(SOURCE) $(DEP_RSC_WWW_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTPlain.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTPLA=\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTPLA=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinRel\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTPLA=\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTPLA=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinDebug\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTML.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTML_=\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTML_=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinRel\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTML_=\
	"..\..\..\Library\src\HText.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTStyle.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTML_=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinDebug\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTMLPDTD.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTMLP=\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMLP=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinRel\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTMLP=\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTMLP=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinDebug\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTInit.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTINI=\
	"..\..\..\Library\src\HTDemux.h"\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMerge.h"\
	"..\..\..\Library\src\HTMuxCh.h"\
	"..\..\..\Library\src\HTMuxHeader.h"\
	"..\..\..\Library\src\HTMuxTx.h"\
	"..\..\..\Library\src\HTNewsRq.h"\
	"..\..\..\Library\src\HTPEP.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\WWWMIME.h"\
	"..\..\..\Library\src\WWWMux.h"\
	"..\..\..\Library\src\WWWTrans.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTAAUtil.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTANSI.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTBound.h"\
	".\..\..\..\Library\src\HTBufWrt.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTConLen.h"\
	".\..\..\..\Library\src\HTDescpt.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTDir.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFile.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFTP.h"\
	".\..\..\..\Library\src\HTFTPDir.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTGopher.h"\
	".\..\..\..\Library\src\HTGuess.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTIcons.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTLocal.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTMIME.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTMulpar.h"\
	".\..\..\..\Library\src\HTMulti.h"\
	".\..\..\..\Library\src\HTMux.h"\
	".\..\..\..\Library\src\HTNDir.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTNews.h"\
	".\..\..\..\Library\src\HTNewsLs.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HTReader.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTSChunk.h"\
	".\..\..\..\Library\src\HTSocket.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTChunk.h"\
	".\..\..\..\Library\src\HTTCP.h"\
	".\..\..\..\Library\src\HTTee.h"\
	".\..\..\..\Library\src\HTTelnet.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTTP.h"\
	".\..\..\..\Library\src\HTTPGen.h"\
	".\..\..\..\Library\src\HTTPReq.h"\
	".\..\..\..\Library\src\HTTPRes.h"\
	".\..\..\..\Library\src\HTTPServ.h"\
	".\..\..\..\Library\src\HTTPUtil.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWriter.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTXParse.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWDir.h"\
	".\..\..\..\Library\src\WWWFile.h"\
	".\..\..\..\Library\src\WWWFTP.h"\
	".\..\..\..\Library\src\WWWGophe.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWHTTP.h"\
	".\..\..\..\Library\src\WWWNews.h"\
	".\..\..\..\Library\src\WWWStream.h"\
	".\..\..\..\Library\src\WWWTelnt.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWWAIS.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTINI=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

".\WinRel\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTINI=\
	"..\..\..\Library\src\HTDemux.h"\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMerge.h"\
	"..\..\..\Library\src\HTMuxCh.h"\
	"..\..\..\Library\src\HTMuxHeader.h"\
	"..\..\..\Library\src\HTMuxTx.h"\
	"..\..\..\Library\src\HTNewsRq.h"\
	"..\..\..\Library\src\HTPEP.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\WWWMIME.h"\
	"..\..\..\Library\src\WWWMux.h"\
	"..\..\..\Library\src\WWWTrans.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTAAUtil.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTANSI.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTBound.h"\
	".\..\..\..\Library\src\HTBufWrt.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTConLen.h"\
	".\..\..\..\Library\src\HTDescpt.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTDir.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFile.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFTP.h"\
	".\..\..\..\Library\src\HTFTPDir.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTGopher.h"\
	".\..\..\..\Library\src\HTGuess.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTIcons.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTLocal.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTMIME.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTMulpar.h"\
	".\..\..\..\Library\src\HTMulti.h"\
	".\..\..\..\Library\src\HTMux.h"\
	".\..\..\..\Library\src\HTNDir.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTNews.h"\
	".\..\..\..\Library\src\HTNewsLs.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HTReader.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTSChunk.h"\
	".\..\..\..\Library\src\HTSocket.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTChunk.h"\
	".\..\..\..\Library\src\HTTCP.h"\
	".\..\..\..\Library\src\HTTee.h"\
	".\..\..\..\Library\src\HTTelnet.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTTP.h"\
	".\..\..\..\Library\src\HTTPGen.h"\
	".\..\..\..\Library\src\HTTPReq.h"\
	".\..\..\..\Library\src\HTTPRes.h"\
	".\..\..\..\Library\src\HTTPServ.h"\
	".\..\..\..\Library\src\HTTPUtil.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWriter.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTXParse.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWDir.h"\
	".\..\..\..\Library\src\WWWFile.h"\
	".\..\..\..\Library\src\WWWFTP.h"\
	".\..\..\..\Library\src\WWWGophe.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWHTTP.h"\
	".\..\..\..\Library\src\WWWNews.h"\
	".\..\..\..\Library\src\WWWStream.h"\
	".\..\..\..\Library\src\WWWTelnt.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWWAIS.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTINI=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

".\WinDebug\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTBInit.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTBIN=\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTBInit.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
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
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTBIN=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinRel\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTBIN=\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTBInit.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
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
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTBIN=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinDebug\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTHInit.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTHIN=\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTHIN=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinRel\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTHIN=\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTHIN=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

".\WinDebug\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTProfil.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTPRO=\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMerge.h"\
	"..\..\..\Library\src\HTNewsRq.h"\
	"..\..\..\Library\src\HTPEP.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWInit.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\WWWMIME.h"\
	"..\..\..\Library\src\WWWTrans.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTAAUtil.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTANSI.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTBInit.h"\
	".\..\..\..\Library\src\HTBound.h"\
	".\..\..\..\Library\src\HTBufWrt.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTConLen.h"\
	".\..\..\..\Library\src\HTDescpt.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTDir.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFile.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFTP.h"\
	".\..\..\..\Library\src\HTFTPDir.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTGopher.h"\
	".\..\..\..\Library\src\HTGuess.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTIcons.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTLocal.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTMIME.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTMulpar.h"\
	".\..\..\..\Library\src\HTMulti.h"\
	".\..\..\..\Library\src\HTNDir.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTNews.h"\
	".\..\..\..\Library\src\HTNewsLs.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTProfil.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HTReader.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTSChunk.h"\
	".\..\..\..\Library\src\HTSocket.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTChunk.h"\
	".\..\..\..\Library\src\HTTCP.h"\
	".\..\..\..\Library\src\HTTee.h"\
	".\..\..\..\Library\src\HTTelnet.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTTP.h"\
	".\..\..\..\Library\src\HTTPGen.h"\
	".\..\..\..\Library\src\HTTPReq.h"\
	".\..\..\..\Library\src\HTTPRes.h"\
	".\..\..\..\Library\src\HTTPServ.h"\
	".\..\..\..\Library\src\HTTPUtil.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWriter.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTXParse.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWDir.h"\
	".\..\..\..\Library\src\WWWFile.h"\
	".\..\..\..\Library\src\WWWFTP.h"\
	".\..\..\..\Library\src\WWWGophe.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWHTTP.h"\
	".\..\..\..\Library\src\WWWNews.h"\
	".\..\..\..\Library\src\WWWStream.h"\
	".\..\..\..\Library\src\WWWTelnt.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWWAIS.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTPRO=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

".\WinRel\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTPRO=\
	"..\..\..\Library\src\HTEvtLst.h"\
	"..\..\..\Library\src\HTHstMan.h"\
	"..\..\..\Library\src\HTList.h"\
	"..\..\..\Library\src\HTMerge.h"\
	"..\..\..\Library\src\HTNewsRq.h"\
	"..\..\..\Library\src\HTPEP.h"\
	"..\..\..\Library\src\HTResponse.h"\
	"..\..\..\Library\src\HTUtils.h"\
	"..\..\..\Library\src\WWWApp.h"\
	"..\..\..\Library\src\WWWCache.h"\
	"..\..\..\Library\src\WWWCore.h"\
	"..\..\..\Library\src\WWWInit.h"\
	"..\..\..\Library\src\WWWLib.h"\
	"..\..\..\Library\src\WWWMIME.h"\
	"..\..\..\Library\src\WWWTrans.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTAAUtil.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTANSI.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTBInit.h"\
	".\..\..\..\Library\src\HTBound.h"\
	".\..\..\..\Library\src\HTBufWrt.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTConLen.h"\
	".\..\..\..\Library\src\HTDescpt.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTDir.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTFile.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTFTP.h"\
	".\..\..\..\Library\src\HTFTPDir.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTGopher.h"\
	".\..\..\..\Library\src\HTGuess.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTIcons.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTLocal.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTMIME.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTMulpar.h"\
	".\..\..\..\Library\src\HTMulti.h"\
	".\..\..\..\Library\src\HTNDir.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTNews.h"\
	".\..\..\..\Library\src\HTNewsLs.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HTProfil.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HTReader.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTSChunk.h"\
	".\..\..\..\Library\src\HTSocket.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTTChunk.h"\
	".\..\..\..\Library\src\HTTCP.h"\
	".\..\..\..\Library\src\HTTee.h"\
	".\..\..\..\Library\src\HTTelnet.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTTP.h"\
	".\..\..\..\Library\src\HTTPGen.h"\
	".\..\..\..\Library\src\HTTPReq.h"\
	".\..\..\..\Library\src\HTTPRes.h"\
	".\..\..\..\Library\src\HTTPServ.h"\
	".\..\..\..\Library\src\HTTPUtil.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTWriter.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTXParse.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWDir.h"\
	".\..\..\..\Library\src\WWWFile.h"\
	".\..\..\..\Library\src\WWWFTP.h"\
	".\..\..\..\Library\src\WWWGophe.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWHTTP.h"\
	".\..\..\..\Library\src\WWWNews.h"\
	".\..\..\..\Library\src\WWWStream.h"\
	".\..\..\..\Library\src\WWWTelnt.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWWAIS.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_HTPRO=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

".\WinDebug\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwwais.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwutils.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwtrans.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwtelnt.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwstream.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwnews.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwmime.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwhttp.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwhtml.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwgophe.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwftp.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwfile.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwdll.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwdir.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwcore.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwcache.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\wwwapp.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="..\..\..\Bin\pics.lib"

!IF  "$(CFG)" == "www - Win32 Release"

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
