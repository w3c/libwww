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
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

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

ALL : "$(OUTDIR)\www.exe"

CLEAN : 
	-@erase "..\..\..\Bin\Release\www.exe"
	-@erase ".\WinRel\lib.obj"
	-@erase ".\WinRel\HTPlain.obj"
	-@erase ".\WinRel\www.obj"
	-@erase ".\WinRel\scroll.obj"
	-@erase ".\WinRel\HTBInit.obj"
	-@erase ".\WinRel\DefaultStyles.obj"
	-@erase ".\WinRel\HTInit.obj"
	-@erase ".\WinRel\HTMLPDTD.obj"
	-@erase ".\WinRel\HTBrowse.obj"
	-@erase ".\WinRel\GridText.obj"
	-@erase ".\WinRel\GridStyle.obj"
	-@erase ".\WinRel\HTML.obj"
	-@erase ".\WinRel\HTProfil.obj"
	-@erase ".\WinRel\HTHInit.obj"
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/www.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"../../../Bin/Release/www.exe"
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/www.pdb" /machine:I386 /out:"../../../Bin/Release/www.exe" 
LINK32_OBJS= \
	"$(INTDIR)/lib.obj" \
	"$(INTDIR)/HTPlain.obj" \
	"$(INTDIR)/www.obj" \
	"$(INTDIR)/scroll.obj" \
	"$(INTDIR)/HTBInit.obj" \
	"$(INTDIR)/DefaultStyles.obj" \
	"$(INTDIR)/HTInit.obj" \
	"$(INTDIR)/HTMLPDTD.obj" \
	"$(INTDIR)/HTBrowse.obj" \
	"$(INTDIR)/GridText.obj" \
	"$(INTDIR)/GridStyle.obj" \
	"$(INTDIR)/HTML.obj" \
	"$(INTDIR)/HTProfil.obj" \
	"$(INTDIR)/HTHInit.obj" \
	"$(INTDIR)/www.res" \
	"..\..\..\Bin\wwwapp.lib" \
	"..\..\..\Bin\wwwmime.lib" \
	"..\..\..\Bin\wwwhttp.lib" \
	"..\..\..\Bin\pics.lib" \
	"..\..\..\Bin\wwwwais.lib" \
	"..\..\..\Bin\wwwnews.lib" \
	"..\..\..\Bin\wwwdir.lib" \
	"..\..\..\Bin\wwwstream.lib" \
	"..\..\..\Bin\wwwgophe.lib" \
	"..\..\..\Bin\wwwfile.lib" \
	"..\..\..\Bin\wwwcore.lib" \
	"..\..\..\Bin\wwwcache.lib" \
	"..\..\..\Bin\wwwftp.lib" \
	"..\..\..\Bin\wwwtelnt.lib" \
	"..\..\..\Bin\wwwdll.lib" \
	"..\..\..\Bin\wwwutils.lib" \
	"..\..\..\Bin\wwwtrans.lib" \
	"..\..\..\Bin\wwwhtml.lib"

"$(OUTDIR)\www.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "$(OUTDIR)\www.exe"

CLEAN : 
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\vc40.idb"
	-@erase "..\..\..\Bin\Debug\www.exe"
	-@erase ".\WinDebug\HTBInit.obj"
	-@erase ".\WinDebug\GridStyle.obj"
	-@erase ".\WinDebug\HTMLPDTD.obj"
	-@erase ".\WinDebug\HTBrowse.obj"
	-@erase ".\WinDebug\HTPlain.obj"
	-@erase ".\WinDebug\HTML.obj"
	-@erase ".\WinDebug\HTProfil.obj"
	-@erase ".\WinDebug\scroll.obj"
	-@erase ".\WinDebug\GridText.obj"
	-@erase ".\WinDebug\www.obj"
	-@erase ".\WinDebug\HTInit.obj"
	-@erase ".\WinDebug\HTHInit.obj"
	-@erase ".\WinDebug\DefaultStyles.obj"
	-@erase ".\WinDebug\lib.obj"
	-@erase ".\WinDebug\www.res"
	-@erase "..\..\..\Bin\Debug\www.ilk"
	-@erase ".\WinDebug\www.pdb"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/www.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../../Bin/Debug/www.exe"
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/www.pdb" /debug /machine:I386 /out:"../../../Bin/Debug/www.exe"\
 
LINK32_OBJS= \
	"$(INTDIR)/HTBInit.obj" \
	"$(INTDIR)/GridStyle.obj" \
	"$(INTDIR)/HTMLPDTD.obj" \
	"$(INTDIR)/HTBrowse.obj" \
	"$(INTDIR)/HTPlain.obj" \
	"$(INTDIR)/HTML.obj" \
	"$(INTDIR)/HTProfil.obj" \
	"$(INTDIR)/scroll.obj" \
	"$(INTDIR)/GridText.obj" \
	"$(INTDIR)/www.obj" \
	"$(INTDIR)/HTInit.obj" \
	"$(INTDIR)/HTHInit.obj" \
	"$(INTDIR)/DefaultStyles.obj" \
	"$(INTDIR)/lib.obj" \
	"$(INTDIR)/www.res" \
	"..\..\..\Bin\wwwapp.lib" \
	"..\..\..\Bin\wwwmime.lib" \
	"..\..\..\Bin\wwwhttp.lib" \
	"..\..\..\Bin\pics.lib" \
	"..\..\..\Bin\wwwwais.lib" \
	"..\..\..\Bin\wwwnews.lib" \
	"..\..\..\Bin\wwwdir.lib" \
	"..\..\..\Bin\wwwstream.lib" \
	"..\..\..\Bin\wwwgophe.lib" \
	"..\..\..\Bin\wwwfile.lib" \
	"..\..\..\Bin\wwwcore.lib" \
	"..\..\..\Bin\wwwcache.lib" \
	"..\..\..\Bin\wwwftp.lib" \
	"..\..\..\Bin\wwwtelnt.lib" \
	"..\..\..\Bin\wwwdll.lib" \
	"..\..\..\Bin\wwwutils.lib" \
	"..\..\..\Bin\wwwtrans.lib" \
	"..\..\..\Bin\wwwhtml.lib"

"$(OUTDIR)\www.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTReqMan.h"\
	".\..\HTBrowse.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTNet.h"\
	".\..\..\..\Library\src\HTMIMPrs.h"\
	".\..\..\..\Library\src\HTEvent.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTError.h"\
	".\..\..\..\Library\src\HTUser.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTTrans.h"\
	".\..\..\..\Library\src\HTHost.h"\
	".\..\..\..\Library\src\HTChannl.h"\
	".\..\..\..\Library\src\HTDNS.h"\
	".\..\..\..\Library\src\HTIOStream.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	
NODEP_CPP_WWW_C=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\www.obj" : $(SOURCE) $(DEP_CPP_WWW_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_WWW_C=\
	".\..\windows\lib.h"\
	".\..\windows\scroll.h"\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTReqMan.h"\
	".\..\HTBrowse.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
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
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTMIMPrs.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	
NODEP_CPP_WWW_C=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

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
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\sysdep.h"\
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
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	
NODEP_CPP_GRIDS=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\GridStyle.obj" : $(SOURCE) $(DEP_CPP_GRIDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_GRIDS=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	
NODEP_CPP_GRIDS=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\GridStyle.obj" : $(SOURCE) $(DEP_CPP_GRIDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\GridText.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_GRIDT=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\WWWCache.h"\
	".\..\..\..\Library\src\WWWRules.h"\
	".\..\HTBrowse.h"\
	".\..\HTFont.h"\
	".\..\GridStyle.h"\
	".\..\GridText.h"\
	".\..\..\..\Library\src\HTReqMan.h"\
	".\..\a_stdio.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTMemory.h"\
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
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HText.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTMIMPrs.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	
NODEP_CPP_GRIDT=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\GridText.obj" : $(SOURCE) $(DEP_CPP_GRIDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_GRIDT=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\WWWCache.h"\
	".\..\..\..\Library\src\WWWRules.h"\
	".\..\HTBrowse.h"\
	".\..\HTFont.h"\
	".\..\GridStyle.h"\
	".\..\GridText.h"\
	".\..\..\..\Library\src\HTReqMan.h"\
	".\..\a_stdio.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
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
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HText.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTMIMPrs.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	
NODEP_CPP_GRIDT=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\GridText.obj" : $(SOURCE) $(DEP_CPP_GRIDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\HTBrowse.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTBRO=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\WWWMIME.h"\
	".\..\..\..\Library\src\WWWRules.h"\
	".\..\..\..\Library\src\WWWCache.h"\
	".\..\..\..\Library\src\WWWApp.h"\
	".\..\..\..\Library\src\WWWTrans.h"\
	".\..\..\..\Library\src\WWWInit.h"\
	".\..\GridText.h"\
	".\..\HTBrowse.h"\
	".\..\..\..\PICS-client\src\CSLApp.h"\
	".\..\..\..\PICS-client\src\CSLL.h"\
	".\..\..\..\PICS-client\src\CSUser.h"\
	".\..\..\..\PICS-client\src\CSUsrLst.h"\
	".\..\a_stdio.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTMIME.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
	".\..\..\..\Library\src\HTBound.h"\
	".\..\..\..\Library\src\HTMulpar.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTReq.h"\
	".\..\..\..\Library\src\HTMemory.h"\
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
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTEvntrg.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTANSI.h"\
	".\..\..\..\Library\src\HTLocal.h"\
	".\..\..\..\Library\src\HTTCP.h"\
	".\..\..\..\Library\src\HTSocket.h"\
	".\..\..\..\Library\src\HTReader.h"\
	".\..\..\..\Library\src\HTWriter.h"\
	".\..\..\..\Library\src\HTBufWrt.h"\
	".\..\..\..\Library\src\HTMux.h"\
	".\..\..\..\Library\src\HTProfil.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTBInit.h"\
	".\..\..\..\Library\src\WWWHTTP.h"\
	".\..\..\..\Library\src\WWWFile.h"\
	".\..\..\..\Library\src\WWWFTP.h"\
	".\..\..\..\Library\src\WWWGophe.h"\
	".\..\..\..\Library\src\WWWTelnt.h"\
	".\..\..\..\Library\src\WWWNews.h"\
	".\..\..\..\Library\src\WWWWAIS.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWStream.h"\
	".\..\..\..\Library\src\WWWDir.h"\
	".\..\..\..\Library\src\HTTPUtil.h"\
	".\..\..\..\Library\src\HTTP.h"\
	".\..\..\..\Library\src\HTTPServ.h"\
	".\..\..\..\Library\src\HTTPGen.h"\
	".\..\..\..\Library\src\HTTPReq.h"\
	".\..\..\..\Library\src\HTTPRes.h"\
	".\..\..\..\Library\src\HTTChunk.h"\
	".\..\..\..\Library\src\HTAAUtil.h"\
	".\..\..\..\Library\src\HTAABrow.h"\
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTFile.h"\
	".\..\..\..\Library\src\HTMulti.h"\
	".\..\..\..\Library\src\HTFTP.h"\
	".\..\..\..\Library\src\HTFTPDir.h"\
	".\..\..\..\Library\src\HTGopher.h"\
	".\..\..\..\Library\src\HTTelnet.h"\
	".\..\..\..\Library\src\HTNews.h"\
	".\..\..\..\Library\src\HTNewsLs.h"\
	".\..\..\..\Library\src\HTNDir.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTLib.h"\
	".\..\..\..\Library\src\HTParse.h"\
	".\..\..\..\Library\src\HTEscape.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTBind.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	".\..\..\..\Library\src\HText.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\PICS-client\src\CSLUtils.h"\
	
NODEP_CPP_HTBRO=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

"$(INTDIR)\HTBrowse.obj" : $(SOURCE) $(DEP_CPP_HTBRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTBRO=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\WWWMIME.h"\
	".\..\..\..\Library\src\WWWRules.h"\
	".\..\..\..\Library\src\WWWCache.h"\
	".\..\..\..\Library\src\WWWApp.h"\
	".\..\..\..\Library\src\WWWTrans.h"\
	".\..\..\..\Library\src\WWWInit.h"\
	".\..\GridText.h"\
	".\..\HTBrowse.h"\
	".\..\..\..\PICS-client\src\CSLApp.h"\
	".\..\..\..\PICS-client\src\CSLL.h"\
	".\..\..\..\PICS-client\src\CSUser.h"\
	".\..\..\..\PICS-client\src\CSUsrLst.h"\
	".\..\a_stdio.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
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
	".\..\..\..\Library\src\HTMIME.h"\
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
	".\..\..\..\Library\src\HTBound.h"\
	".\..\..\..\Library\src\HTMulpar.h"\
	".\..\..\..\Library\src\HTRules.h"\
	".\..\..\..\Library\src\HTProxy.h"\
	".\..\..\..\Library\src\HTCache.h"\
	".\..\..\..\Library\src\HTEvntrg.h"\
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTHist.h"\
	".\..\..\..\Library\src\HTANSI.h"\
	".\..\..\..\Library\src\HTLocal.h"\
	".\..\..\..\Library\src\HTTCP.h"\
	".\..\..\..\Library\src\HTSocket.h"\
	".\..\..\..\Library\src\HTReader.h"\
	".\..\..\..\Library\src\HTWriter.h"\
	".\..\..\..\Library\src\HTBufWrt.h"\
	".\..\..\..\Library\src\HTMux.h"\
	".\..\..\..\Library\src\HTProfil.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTBInit.h"\
	".\..\..\..\Library\src\WWWHTTP.h"\
	".\..\..\..\Library\src\WWWFile.h"\
	".\..\..\..\Library\src\WWWFTP.h"\
	".\..\..\..\Library\src\WWWGophe.h"\
	".\..\..\..\Library\src\WWWTelnt.h"\
	".\..\..\..\Library\src\WWWNews.h"\
	".\..\..\..\Library\src\WWWWAIS.h"\
	".\..\..\..\Library\src\WWWHTML.h"\
	".\..\..\..\Library\src\WWWStream.h"\
	".\..\..\..\Library\src\WWWDir.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	".\..\..\..\Library\src\HText.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\PICS-client\src\CSLUtils.h"\
	
NODEP_CPP_HTBRO=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

"$(INTDIR)\HTBrowse.obj" : $(SOURCE) $(DEP_CPP_HTBRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\DefaultStyles.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_DEFAU=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\GridStyle.h"\
	".\..\HTFont.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HText.h"\
	".\..\..\..\Library\src\HTAnchor.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\HTLink.h"\
	".\..\..\..\Library\src\HTMethod.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTArray.h"\
	".\..\..\..\Library\src\HTAssoc.h"\
	".\..\..\..\Library\src\HTAtom.h"\
	".\..\..\..\Library\src\HTChunk.h"\
	".\..\..\..\Library\src\HTList.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTString.h"\
	".\..\..\..\Library\src\HTUU.h"\
	".\..\..\..\Library\src\HTFormat.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTStream.h"\
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
	
NODEP_CPP_DEFAU=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\DefaultStyles.obj" : $(SOURCE) $(DEP_CPP_DEFAU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_DEFAU=\
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\GridStyle.h"\
	".\..\HTFont.h"\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
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
	".\..\..\..\Library\src\HText.h"\
	".\..\..\..\Library\src\HTStyle.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	
NODEP_CPP_DEFAU=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\DefaultStyles.obj" : $(SOURCE) $(DEP_CPP_DEFAU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\www.rc
DEP_RSC_WWW_R=\
	".\WWW.ICO"\
	

"$(INTDIR)\www.res" : $(SOURCE) $(DEP_RSC_WWW_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTPlain.c

!IF  "$(CFG)" == "www - Win32 Release"

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
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

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
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTML.c

!IF  "$(CFG)" == "www - Win32 Release"

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
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	
NODEP_CPP_HTML_=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

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
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTMLGen.h"\
	".\..\..\..\Library\src\HTTeXGen.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	
NODEP_CPP_HTML_=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTMLPDTD.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTMLP=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\HTUtils.h"\
	".\..\..\..\Library\src\HTMLPDTD.h"\
	".\..\..\..\Library\src\HTMemory.h"\
	".\..\..\..\Library\src\HTStruct.h"\
	".\..\..\..\Library\src\SGML.h"\
	".\..\..\..\Library\src\HTStream.h"\
	".\..\..\..\Library\src\HTList.h"\
	
NODEP_CPP_HTMLP=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

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
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTInit.c

!IF  "$(CFG)" == "www - Win32 Release"

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
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTHist.h"\
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
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
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
	

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

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
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTHist.h"\
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
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
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
	

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTBInit.c

!IF  "$(CFG)" == "www - Win32 Release"

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
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	
NODEP_CPP_HTBIN=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

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
	".\..\..\..\Library\src\HTUTree.h"\
	".\..\..\..\Library\src\HTWWWStr.h"\
	".\..\..\..\Library\src\HTAlert.h"\
	".\..\..\..\Library\src\HTFWrite.h"\
	".\..\..\..\Library\src\HTInet.h"\
	".\..\..\..\Library\src\HTProt.h"\
	
NODEP_CPP_HTBIN=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTHInit.c

!IF  "$(CFG)" == "www - Win32 Release"

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
	

"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

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
	

"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\Library\src\HTProfil.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTPRO=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTProfil.h"\
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
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\WWWInit.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTBInit.h"\
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
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTHist.h"\
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
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	
NODEP_CPP_HTPRO=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

"$(INTDIR)\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTPRO=\
	".\..\..\..\Library\src\sysdep.h"\
	".\..\..\..\Library\src\WWWUtil.h"\
	".\..\..\..\Library\src\WWWCore.h"\
	".\..\..\..\Library\src\HTProfil.h"\
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
	".\..\..\..\Library\src\WWWLib.h"\
	".\..\..\..\Library\src\WWWInit.h"\
	".\..\..\..\Library\src\HTInit.h"\
	".\..\..\..\Library\src\HTHInit.h"\
	".\..\..\..\Library\src\HTBInit.h"\
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
	".\..\..\..\Library\src\HTHome.h"\
	".\..\..\..\Library\src\HTDialog.h"\
	".\..\..\..\Library\src\HTAccess.h"\
	".\..\..\..\Library\src\HTFilter.h"\
	".\..\..\..\Library\src\HTLog.h"\
	".\..\..\..\Library\src\HTHist.h"\
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
	".\..\..\..\Library\src\HTHeader.h"\
	".\..\..\..\Library\src\HTMIMERq.h"\
	".\..\..\..\Library\src\HTMIMImp.h"\
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
	".\..\..\..\Library\src\HTML.h"\
	".\..\..\..\Library\src\HTPlain.h"\
	
NODEP_CPP_HTPRO=\
	".\..\..\..\Library\src\HTVMSUtils.h"\
	".\..\..\..\Library\src\HTWAIS.h"\
	

"$(INTDIR)\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
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
