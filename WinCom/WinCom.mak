# Microsoft Developer Studio Generated NMAKE File, Based on WinCom.dsp
!IF "$(CFG)" == ""
CFG=WinCom - Win32 Debug
!MESSAGE No configuration specified. Defaulting to WinCom - Win32 Debug.
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
	-@erase "$(INTDIR)\CacheSetup.obj"
	-@erase "$(INTDIR)\Delete.obj"
	-@erase "$(INTDIR)\EntityInfo.obj"
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
	"$(INTDIR)\CacheSetup.obj" \
	"$(INTDIR)\Delete.obj" \
	"$(INTDIR)\EntityInfo.obj" \
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
	-@erase "$(INTDIR)\CacheSetup.obj"
	-@erase "$(INTDIR)\Delete.obj"
	-@erase "$(INTDIR)\EntityInfo.obj"
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
	"$(INTDIR)\CacheSetup.obj" \
	"$(INTDIR)\Delete.obj" \
	"$(INTDIR)\EntityInfo.obj" \
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
SOURCE=.\CacheSetup.cpp
DEP_CPP_CACHE=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htcache.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcache.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	
NODEP_CPP_CACHE=\
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CacheSetup.obj" : $(SOURCE) $(DEP_CPP_CACHE) "$(INTDIR)"


SOURCE=.\Delete.cpp
DEP_CPP_DELET=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Delete.obj" : $(SOURCE) $(DEP_CPP_DELET) "$(INTDIR)"


SOURCE=.\EntityInfo.cpp
DEP_CPP_ENTIT=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htfile.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htmulti.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwfile.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\EntityInfo.obj" : $(SOURCE) $(DEP_CPP_ENTIT) "$(INTDIR)"


SOURCE=.\Links.cpp
DEP_CPP_LINKS=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Links.obj" : $(SOURCE) $(DEP_CPP_LINKS) "$(INTDIR)"


SOURCE=.\LinkView.cpp
DEP_CPP_LINKV=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\LinkView.obj" : $(SOURCE) $(DEP_CPP_LINKV) "$(INTDIR)"


SOURCE=.\Listvwex.cpp
DEP_CPP_LISTV=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\Links.h"\
	".\LinkView.h"\
	".\Listvwex.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_LISTV=\
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Listvwex.obj" : $(SOURCE) $(DEP_CPP_LISTV) "$(INTDIR)"


SOURCE=.\Load.cpp
DEP_CPP_LOAD_=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Load.obj" : $(SOURCE) $(DEP_CPP_LOAD_) "$(INTDIR)"


SOURCE=.\Location.cpp
DEP_CPP_LOCAT=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Location.obj" : $(SOURCE) $(DEP_CPP_LOCAT) "$(INTDIR)"


SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


SOURCE=.\Password.cpp
DEP_CPP_PASSW=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Password.obj" : $(SOURCE) $(DEP_CPP_PASSW) "$(INTDIR)"


SOURCE=.\Progress.cpp
DEP_CPP_PROGR=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\Progress.obj" : $(SOURCE) $(DEP_CPP_PROGR) "$(INTDIR)"


SOURCE=.\ProxySetup.cpp
DEP_CPP_PROXY=\
	"..\library\external\regex.h"\
	"..\library\src\htaccess.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdialog.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htevtlst.h"\
	"..\library\src\htfilter.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthist.h"\
	"..\library\src\hthome.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htlog.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htproxy.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htrules.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwapp.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\ProxySetup.obj" : $(SOURCE) $(DEP_CPP_PROXY) "$(INTDIR)"


SOURCE=.\Request.cpp
DEP_CPP_REQUE=\
	"..\library\external\regex.h"\
	"..\library\src\htaccess.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdialog.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htevtlst.h"\
	"..\library\src\htfilter.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthist.h"\
	"..\library\src\hthome.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htlog.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htproxy.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htrules.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwapp.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

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
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\TabsView.obj" : $(SOURCE) $(DEP_CPP_TABSV) "$(INTDIR)"


SOURCE=.\UserParameters.cpp
DEP_CPP_USERP=\
	"..\library\external\regex.h"\
	"..\library\src\htaccess.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdialog.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htevtlst.h"\
	"..\library\src\htfilter.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthist.h"\
	"..\library\src\hthome.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htlog.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htproxy.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htrules.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwapp.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\UserParameters.obj" : $(SOURCE) $(DEP_CPP_USERP) "$(INTDIR)"


SOURCE=.\VersionConflict.cpp
DEP_CPP_VERSI=\
	"..\library\external\regex.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\VersionConflict.obj" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"


SOURCE=.\WinCom.cpp
DEP_CPP_WINCO=\
	"..\library\external\regex.h"\
	"..\library\src\htaabrow.h"\
	"..\library\src\htaautil.h"\
	"..\library\src\htaccess.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htansi.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htbinit.h"\
	"..\library\src\htbound.h"\
	"..\library\src\htbufwrt.h"\
	"..\library\src\htcache.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htconlen.h"\
	"..\library\src\htdescpt.h"\
	"..\library\src\htdialog.h"\
	"..\library\src\htdir.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htevtlst.h"\
	"..\library\src\htext.h"\
	"..\library\src\htfile.h"\
	"..\library\src\htfilter.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htftp.h"\
	"..\library\src\htftpdir.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\htgopher.h"\
	"..\library\src\htguess.h"\
	"..\library\src\htheader.h"\
	"..\library\src\hthinit.h"\
	"..\library\src\hthist.h"\
	"..\library\src\hthome.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\hticons.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htinit.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htlocal.h"\
	"..\library\src\htlog.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmerge.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htmime.h"\
	"..\library\src\htmimerq.h"\
	"..\library\src\htmimimp.h"\
	"..\library\src\html.h"\
	"..\library\src\htmlgen.h"\
	"..\library\src\htmlpdtd.h"\
	"..\library\src\htmulpar.h"\
	"..\library\src\htmulti.h"\
	"..\library\src\htndir.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htnews.h"\
	"..\library\src\htnewsls.h"\
	"..\library\src\htnewsrq.h"\
	"..\library\src\htnofree.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htpep.h"\
	"..\library\src\htplain.h"\
	"..\library\src\htprofil.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htproxy.h"\
	"..\library\src\htreader.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htrules.h"\
	"..\library\src\htschunk.h"\
	"..\library\src\htsocket.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httchunk.h"\
	"..\library\src\httcp.h"\
	"..\library\src\httee.h"\
	"..\library\src\httelnet.h"\
	"..\library\src\httexgen.h"\
	"..\library\src\httimer.h"\
	"..\library\src\http.h"\
	"..\library\src\httpgen.h"\
	"..\library\src\httpreq.h"\
	"..\library\src\httpres.h"\
	"..\library\src\httpserv.h"\
	"..\library\src\httputil.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\HTWAIS.h"\
	"..\library\src\htwriter.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\HTXML.h"\
	"..\library\src\htxparse.h"\
	"..\library\src\htzip.h"\
	"..\library\src\sgml.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwapp.h"\
	"..\library\src\wwwcache.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwdir.h"\
	"..\library\src\wwwfile.h"\
	"..\library\src\wwwftp.h"\
	"..\library\src\wwwgophe.h"\
	"..\library\src\wwwhtml.h"\
	"..\library\src\wwwhttp.h"\
	"..\library\src\wwwinit.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwmime.h"\
	"..\library\src\wwwnews.h"\
	"..\library\src\wwwstream.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwtelnt.h"\
	"..\library\src\wwwtrans.h"\
	"..\library\src\wwwutil.h"\
	"..\library\src\WWWWAIS.h"\
	"..\library\src\wwwzip.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

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
	"..\library\external\regex.h"\
	"..\library\src\htaccess.h"\
	"..\library\src\htalert.h"\
	"..\library\src\htanchor.h"\
	"..\library\src\htarray.h"\
	"..\library\src\htassoc.h"\
	"..\library\src\htatom.h"\
	"..\library\src\htbind.h"\
	"..\library\src\htchannl.h"\
	"..\library\src\htchunk.h"\
	"..\library\src\htdialog.h"\
	"..\library\src\htdns.h"\
	"..\library\src\hterror.h"\
	"..\library\src\htescape.h"\
	"..\library\src\htevent.h"\
	"..\library\src\htevtlst.h"\
	"..\library\src\htfilter.h"\
	"..\library\src\htformat.h"\
	"..\library\src\htfwrite.h"\
	"..\library\src\hthist.h"\
	"..\library\src\hthome.h"\
	"..\library\src\hthost.h"\
	"..\library\src\hthstman.h"\
	"..\library\src\htinet.h"\
	"..\library\src\htiostream.h"\
	"..\library\src\htlib.h"\
	"..\library\src\htlink.h"\
	"..\library\src\htlist.h"\
	"..\library\src\htlog.h"\
	"..\library\src\htmemlog.h"\
	"..\library\src\htmemory.h"\
	"..\library\src\htmethod.h"\
	"..\library\src\htnet.h"\
	"..\library\src\htparse.h"\
	"..\library\src\htprot.h"\
	"..\library\src\htproxy.h"\
	"..\library\src\htreq.h"\
	"..\library\src\htresponse.h"\
	"..\library\src\htrules.h"\
	"..\library\src\htstream.h"\
	"..\library\src\htstring.h"\
	"..\library\src\htstruct.h"\
	"..\library\src\httimer.h"\
	"..\library\src\httrans.h"\
	"..\library\src\htuser.h"\
	"..\library\src\htutils.h"\
	"..\library\src\htutree.h"\
	"..\library\src\htuu.h"\
	"..\library\src\htwwwstr.h"\
	"..\library\src\windows\config.h"\
	"..\library\src\wwwapp.h"\
	"..\library\src\wwwcore.h"\
	"..\library\src\wwwlib.h"\
	"..\library\src\wwwsys.h"\
	"..\library\src\wwwutil.h"\
	".\CacheSetup.h"\
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
	"..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\WinComDoc.obj" : $(SOURCE) $(DEP_CPP_WINCOMD) "$(INTDIR)"



!ENDIF 

