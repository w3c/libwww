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
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdialog.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htevtlst.h"\
	"..\..\..\library\src\htfilter.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\htheader.h"\
	"..\..\..\library\src\hthist.h"\
	"..\..\..\library\src\hthome.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htlog.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htproxy.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htrules.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwapp.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\cslapp.h"\
	"..\csll.h"\
	"..\cslutils.h"\
	"..\csparse.h"\
	"..\csuser.h"\
	"..\csuserst.h"\
	"..\csusrlst.h"\
	

"$(INTDIR)\CSApp.obj" : $(SOURCE) $(DEP_CPP_CSAPP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSAPP=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdialog.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htevtlst.h"\
	"..\..\..\library\src\htfilter.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\htheader.h"\
	"..\..\..\library\src\hthist.h"\
	"..\..\..\library\src\hthome.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htlog.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htproxy.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htrules.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwapp.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\cslapp.h"\
	"..\csll.h"\
	"..\cslutils.h"\
	"..\csparse.h"\
	"..\csuser.h"\
	"..\csuserst.h"\
	"..\csusrlst.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSAPP=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSApp.obj" : $(SOURCE) $(DEP_CPP_CSAPP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSChkLab.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSCHK=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\csll.h"\
	"..\csllst.h"\
	"..\cslutils.h"\
	"..\csuser.h"\
	"..\csuserst.h"\
	

"$(INTDIR)\CSChkLab.obj" : $(SOURCE) $(DEP_CPP_CSCHK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSCHK=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\csll.h"\
	"..\csllst.h"\
	"..\cslutils.h"\
	"..\csuser.h"\
	"..\csuserst.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSCHK=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSChkLab.obj" : $(SOURCE) $(DEP_CPP_CSCHK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSKwik.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSKWI=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	

"$(INTDIR)\CSKwik.obj" : $(SOURCE) $(DEP_CPP_CSKWI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSKWI=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSKWI=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSKwik.obj" : $(SOURCE) $(DEP_CPP_CSKWI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSLabel.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSLAB=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\csll.h"\
	"..\csllst.h"\
	"..\cslutils.h"\
	"..\csparse.h"\
	

"$(INTDIR)\CSLabel.obj" : $(SOURCE) $(DEP_CPP_CSLAB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSLAB=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\csll.h"\
	"..\csllst.h"\
	"..\cslutils.h"\
	"..\csparse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSLAB=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLabel.obj" : $(SOURCE) $(DEP_CPP_CSLAB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSLLOut.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSLLO=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\csll.h"\
	"..\csllst.h"\
	"..\cslutils.h"\
	

"$(INTDIR)\CSLLOut.obj" : $(SOURCE) $(DEP_CPP_CSLLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSLLO=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\csll.h"\
	"..\csllst.h"\
	"..\cslutils.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSLLO=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLLOut.obj" : $(SOURCE) $(DEP_CPP_CSLLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSLLURLs.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSLLU=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\csll.h"\
	"..\csllst.h"\
	"..\cslutils.h"\
	

"$(INTDIR)\CSLLURLs.obj" : $(SOURCE) $(DEP_CPP_CSLLU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSLLU=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\csll.h"\
	"..\csllst.h"\
	"..\cslutils.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSLLU=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSLLURLs.obj" : $(SOURCE) $(DEP_CPP_CSLLU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSMacRed.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSMAC=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\cslutils.h"\
	"..\csmr.h"\
	"..\csmrst.h"\
	"..\csparse.h"\
	

"$(INTDIR)\CSMacRed.obj" : $(SOURCE) $(DEP_CPP_CSMAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSMAC=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\cslutils.h"\
	"..\csmr.h"\
	"..\csmrst.h"\
	"..\csparse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSMAC=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSMacRed.obj" : $(SOURCE) $(DEP_CPP_CSMAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSMem.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSMEM=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\cslutils.h"\
	

"$(INTDIR)\CSMem.obj" : $(SOURCE) $(DEP_CPP_CSMEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSMEM=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\cslutils.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSMEM=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSMem.obj" : $(SOURCE) $(DEP_CPP_CSMEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSParse.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSPAR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\cslutils.h"\
	"..\csparse.h"\
	

"$(INTDIR)\CSParse.obj" : $(SOURCE) $(DEP_CPP_CSPAR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSPAR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\cslutils.h"\
	"..\csparse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSPAR=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSParse.obj" : $(SOURCE) $(DEP_CPP_CSPAR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSStream.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSSTR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htproxy.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\cslapp.h"\
	"..\csll.h"\
	"..\cslutils.h"\
	"..\csmr.h"\
	"..\csparse.h"\
	"..\csuser.h"\
	

"$(INTDIR)\CSStream.obj" : $(SOURCE) $(DEP_CPP_CSSTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSSTR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htproxy.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\cslapp.h"\
	"..\csll.h"\
	"..\cslutils.h"\
	"..\csmr.h"\
	"..\csparse.h"\
	"..\csuser.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSSTR=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSStream.obj" : $(SOURCE) $(DEP_CPP_CSSTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSUser.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSUSE=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\cslutils.h"\
	"..\csparse.h"\
	"..\csuser.h"\
	"..\csuserst.h"\
	

"$(INTDIR)\CSUser.obj" : $(SOURCE) $(DEP_CPP_CSUSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSUSE=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\cslutils.h"\
	"..\csparse.h"\
	"..\csuser.h"\
	"..\csuserst.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSUSE=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSUser.obj" : $(SOURCE) $(DEP_CPP_CSUSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CSUsrLst.c

!IF  "$(CFG)" == "pics - Win32 Release"

DEP_CPP_CSUSR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\cslapp.h"\
	"..\cslutils.h"\
	

"$(INTDIR)\CSUsrLst.obj" : $(SOURCE) $(DEP_CPP_CSUSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pics - Win32 Debug"

DEP_CPP_CSUSR=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\cslapp.h"\
	"..\cslutils.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CSUSR=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\CSUsrLst.obj" : $(SOURCE) $(DEP_CPP_CSUSR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

