# Microsoft Developer Studio Generated NMAKE File, Based on www.dsp
!IF "$(CFG)" == ""
CFG=www - Win32 Release
!MESSAGE No configuration specified. Defaulting to www - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "www - Win32 Release" && "$(CFG)" != "www - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "www.mak" CFG="www - Win32 Release"
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

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "www - Win32 Release"

OUTDIR=.\..\..\..\Bin\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\..\..\Bin\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\www.exe"

!ELSE 

ALL : "$(OUTDIR)\www.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\DefaultStyles.obj"
	-@erase "$(INTDIR)\GridStyle.obj"
	-@erase "$(INTDIR)\GridText.obj"
	-@erase "$(INTDIR)\HTBInit.obj"
	-@erase "$(INTDIR)\HTBrowse.obj"
	-@erase "$(INTDIR)\HTHInit.obj"
	-@erase "$(INTDIR)\HTInit.obj"
	-@erase "$(INTDIR)\HTML.obj"
	-@erase "$(INTDIR)\HTMLPDTD.obj"
	-@erase "$(INTDIR)\HTPlain.obj"
	-@erase "$(INTDIR)\HTProfil.obj"
	-@erase "$(INTDIR)\lib.obj"
	-@erase "$(INTDIR)\scroll.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\www.obj"
	-@erase "$(INTDIR)\www.res"
	-@erase "$(OUTDIR)\www.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\Library\src" /I\
 "..\..\..\PICS-client\src" /I "..\..\..\Library\External" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL"\
 /Fp"$(INTDIR)\www.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\www.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\www.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\www.pdb" /machine:I386 /out:"$(OUTDIR)\www.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DefaultStyles.obj" \
	"$(INTDIR)\GridStyle.obj" \
	"$(INTDIR)\GridText.obj" \
	"$(INTDIR)\HTBInit.obj" \
	"$(INTDIR)\HTBrowse.obj" \
	"$(INTDIR)\HTHInit.obj" \
	"$(INTDIR)\HTInit.obj" \
	"$(INTDIR)\HTML.obj" \
	"$(INTDIR)\HTMLPDTD.obj" \
	"$(INTDIR)\HTPlain.obj" \
	"$(INTDIR)\HTProfil.obj" \
	"$(INTDIR)\lib.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\www.obj" \
	"$(INTDIR)\www.res" \
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
	"..\..\..\Bin\wwwzip.lib"

"$(OUTDIR)\www.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "www - Win32 Debug"

OUTDIR=.\..\..\..\Bin\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\..\..\Bin\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\www.exe"

!ELSE 

ALL : "$(OUTDIR)\www.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\DefaultStyles.obj"
	-@erase "$(INTDIR)\GridStyle.obj"
	-@erase "$(INTDIR)\GridText.obj"
	-@erase "$(INTDIR)\HTBInit.obj"
	-@erase "$(INTDIR)\HTBrowse.obj"
	-@erase "$(INTDIR)\HTHInit.obj"
	-@erase "$(INTDIR)\HTInit.obj"
	-@erase "$(INTDIR)\HTML.obj"
	-@erase "$(INTDIR)\HTMLPDTD.obj"
	-@erase "$(INTDIR)\HTPlain.obj"
	-@erase "$(INTDIR)\HTProfil.obj"
	-@erase "$(INTDIR)\lib.obj"
	-@erase "$(INTDIR)\scroll.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\www.obj"
	-@erase "$(INTDIR)\www.res"
	-@erase "$(OUTDIR)\www.exe"
	-@erase "$(OUTDIR)\www.ilk"
	-@erase "$(OUTDIR)\www.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /I\
 "..\..\..\PICS-client\src" /I "..\..\..\Library\External" /D "_DEBUG" /D\
 "DEBUG" /D "WIN32" /D "_WINDOWS" /D "WWW_WIN_ASYNC" /D "WWW_WIN_DLL"\
 /Fp"$(INTDIR)\www.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\www.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\www.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\www.pdb" /debug /machine:I386 /out:"$(OUTDIR)\www.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DefaultStyles.obj" \
	"$(INTDIR)\GridStyle.obj" \
	"$(INTDIR)\GridText.obj" \
	"$(INTDIR)\HTBInit.obj" \
	"$(INTDIR)\HTBrowse.obj" \
	"$(INTDIR)\HTHInit.obj" \
	"$(INTDIR)\HTInit.obj" \
	"$(INTDIR)\HTML.obj" \
	"$(INTDIR)\HTMLPDTD.obj" \
	"$(INTDIR)\HTPlain.obj" \
	"$(INTDIR)\HTProfil.obj" \
	"$(INTDIR)\lib.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\www.obj" \
	"$(INTDIR)\www.res" \
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
	"..\..\..\Bin\wwwzip.lib"

"$(OUTDIR)\www.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "www - Win32 Release" || "$(CFG)" == "www - Win32 Debug"
SOURCE=..\DefaultStyles.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_DEFAU=\
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
	"..\..\..\library\src\htext.h"\
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
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\gridstyle.h"\
	"..\htfont.h"\
	

"$(INTDIR)\DefaultStyles.obj" : $(SOURCE) $(DEP_CPP_DEFAU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_DEFAU=\
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
	"..\..\..\library\src\htext.h"\
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
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\gridstyle.h"\
	"..\htfont.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DEFAU=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\DefaultStyles.obj" : $(SOURCE) $(DEP_CPP_DEFAU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\GridStyle.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_GRIDS=\
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
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	

"$(INTDIR)\GridStyle.obj" : $(SOURCE) $(DEP_CPP_GRIDS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_GRIDS=\
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
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_GRIDS=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\GridStyle.obj" : $(SOURCE) $(DEP_CPP_GRIDS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\GridText.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_GRIDT=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htcache.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdialog.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htevtlst.h"\
	"..\..\..\library\src\htext.h"\
	"..\..\..\library\src\htfilter.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
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
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlpdtd.h"\
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
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwapp.h"\
	"..\..\..\library\src\wwwcache.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\gridstyle.h"\
	"..\gridtext.h"\
	"..\htbrowse.h"\
	"..\htfont.h"\
	

"$(INTDIR)\GridText.obj" : $(SOURCE) $(DEP_CPP_GRIDT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_GRIDT=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htcache.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdialog.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htevtlst.h"\
	"..\..\..\library\src\htext.h"\
	"..\..\..\library\src\htfilter.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htfwrite.h"\
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
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlpdtd.h"\
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
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwapp.h"\
	"..\..\..\library\src\wwwcache.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\a_stdio.h"\
	"..\gridstyle.h"\
	"..\gridtext.h"\
	"..\htbrowse.h"\
	"..\htfont.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_GRIDT=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\GridText.obj" : $(SOURCE) $(DEP_CPP_GRIDT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTBInit.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTBIN=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htbinit.h"\
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
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTBIN=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htbinit.h"\
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
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBIN=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBInit.obj" : $(SOURCE) $(DEP_CPP_HTBIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\HTBrowse.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTBRO=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaabrow.h"\
	"..\..\..\library\src\htaautil.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htansi.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htbinit.h"\
	"..\..\..\library\src\htbound.h"\
	"..\..\..\library\src\htbufwrt.h"\
	"..\..\..\library\src\htcache.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htconlen.h"\
	"..\..\..\library\src\htdescpt.h"\
	"..\..\..\library\src\htdialog.h"\
	"..\..\..\library\src\htdir.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htevtlst.h"\
	"..\..\..\library\src\htext.h"\
	"..\..\..\library\src\htfile.h"\
	"..\..\..\library\src\htfilter.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htftp.h"\
	"..\..\..\library\src\htftpdir.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\htgopher.h"\
	"..\..\..\library\src\htguess.h"\
	"..\..\..\library\src\htheader.h"\
	"..\..\..\library\src\hthinit.h"\
	"..\..\..\library\src\hthist.h"\
	"..\..\..\library\src\hthome.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\hticons.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htinit.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htlocal.h"\
	"..\..\..\library\src\htlog.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmerge.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htmime.h"\
	"..\..\..\library\src\htmimerq.h"\
	"..\..\..\library\src\htmimimp.h"\
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlgen.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htmulpar.h"\
	"..\..\..\library\src\htmulti.h"\
	"..\..\..\library\src\htndir.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htnews.h"\
	"..\..\..\library\src\htnewsls.h"\
	"..\..\..\library\src\htnewsrq.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htpep.h"\
	"..\..\..\library\src\htplain.h"\
	"..\..\..\library\src\htprofil.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htproxy.h"\
	"..\..\..\library\src\htreader.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htrules.h"\
	"..\..\..\library\src\htschunk.h"\
	"..\..\..\library\src\htsocket.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httchunk.h"\
	"..\..\..\library\src\httcp.h"\
	"..\..\..\library\src\httee.h"\
	"..\..\..\library\src\httelnet.h"\
	"..\..\..\library\src\httexgen.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\http.h"\
	"..\..\..\library\src\httpgen.h"\
	"..\..\..\library\src\httpreq.h"\
	"..\..\..\library\src\httpres.h"\
	"..\..\..\library\src\httpserv.h"\
	"..\..\..\library\src\httputil.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwriter.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\htxparse.h"\
	"..\..\..\library\src\htzip.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwapp.h"\
	"..\..\..\library\src\wwwcache.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwdir.h"\
	"..\..\..\library\src\wwwfile.h"\
	"..\..\..\library\src\wwwftp.h"\
	"..\..\..\library\src\wwwgophe.h"\
	"..\..\..\library\src\wwwhtml.h"\
	"..\..\..\library\src\wwwhttp.h"\
	"..\..\..\library\src\wwwinit.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwmime.h"\
	"..\..\..\library\src\wwwnews.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\wwwzip.h"\
	"..\..\..\pics-client\src\cslapp.h"\
	"..\..\..\pics-client\src\csll.h"\
	"..\..\..\pics-client\src\cslutils.h"\
	"..\..\..\pics-client\src\csuser.h"\
	"..\..\..\pics-client\src\csusrlst.h"\
	"..\gridtext.h"\
	"..\htbrowse.h"\
	

"$(INTDIR)\HTBrowse.obj" : $(SOURCE) $(DEP_CPP_HTBRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTBRO=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaabrow.h"\
	"..\..\..\library\src\htaautil.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htansi.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htbinit.h"\
	"..\..\..\library\src\htbound.h"\
	"..\..\..\library\src\htbufwrt.h"\
	"..\..\..\library\src\htcache.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htconlen.h"\
	"..\..\..\library\src\htdescpt.h"\
	"..\..\..\library\src\htdialog.h"\
	"..\..\..\library\src\htdir.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htevtlst.h"\
	"..\..\..\library\src\htext.h"\
	"..\..\..\library\src\htfile.h"\
	"..\..\..\library\src\htfilter.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htftp.h"\
	"..\..\..\library\src\htftpdir.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\htgopher.h"\
	"..\..\..\library\src\htguess.h"\
	"..\..\..\library\src\htheader.h"\
	"..\..\..\library\src\hthinit.h"\
	"..\..\..\library\src\hthist.h"\
	"..\..\..\library\src\hthome.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\hticons.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htinit.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htlocal.h"\
	"..\..\..\library\src\htlog.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmerge.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htmime.h"\
	"..\..\..\library\src\htmimerq.h"\
	"..\..\..\library\src\htmimimp.h"\
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlgen.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htmulpar.h"\
	"..\..\..\library\src\htmulti.h"\
	"..\..\..\library\src\htndir.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htnews.h"\
	"..\..\..\library\src\htnewsls.h"\
	"..\..\..\library\src\htnewsrq.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htpep.h"\
	"..\..\..\library\src\htplain.h"\
	"..\..\..\library\src\htprofil.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htproxy.h"\
	"..\..\..\library\src\htreader.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htrules.h"\
	"..\..\..\library\src\htschunk.h"\
	"..\..\..\library\src\htsocket.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httchunk.h"\
	"..\..\..\library\src\httcp.h"\
	"..\..\..\library\src\httee.h"\
	"..\..\..\library\src\httelnet.h"\
	"..\..\..\library\src\httexgen.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\http.h"\
	"..\..\..\library\src\httpgen.h"\
	"..\..\..\library\src\httpreq.h"\
	"..\..\..\library\src\httpres.h"\
	"..\..\..\library\src\httpserv.h"\
	"..\..\..\library\src\httputil.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\HTWAIS.h"\
	"..\..\..\library\src\htwriter.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\htxparse.h"\
	"..\..\..\library\src\htzip.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwapp.h"\
	"..\..\..\library\src\wwwcache.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwdir.h"\
	"..\..\..\library\src\wwwfile.h"\
	"..\..\..\library\src\wwwftp.h"\
	"..\..\..\library\src\wwwgophe.h"\
	"..\..\..\library\src\wwwhtml.h"\
	"..\..\..\library\src\wwwhttp.h"\
	"..\..\..\library\src\wwwinit.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwmime.h"\
	"..\..\..\library\src\wwwnews.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\WWWWAIS.h"\
	"..\..\..\library\src\wwwzip.h"\
	"..\..\..\pics-client\src\cslapp.h"\
	"..\..\..\pics-client\src\csll.h"\
	"..\..\..\pics-client\src\cslutils.h"\
	"..\..\..\pics-client\src\csuser.h"\
	"..\..\..\pics-client\src\csusrlst.h"\
	"..\a_stdio.h"\
	"..\gridtext.h"\
	"..\htbrowse.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTBRO=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTBrowse.obj" : $(SOURCE) $(DEP_CPP_HTBRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTHInit.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTHIN=\
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
	"..\..\..\library\src\hthinit.h"\
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
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlgen.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htplain.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httexgen.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	

"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTHIN=\
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
	"..\..\..\library\src\hthinit.h"\
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
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlgen.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htplain.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httexgen.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTHIN=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTHInit.obj" : $(SOURCE) $(DEP_CPP_HTHIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTInit.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTINI=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaabrow.h"\
	"..\..\..\library\src\htaautil.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htansi.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htbound.h"\
	"..\..\..\library\src\htbufwrt.h"\
	"..\..\..\library\src\htcache.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htconlen.h"\
	"..\..\..\library\src\htdescpt.h"\
	"..\..\..\library\src\htdialog.h"\
	"..\..\..\library\src\htdir.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htevtlst.h"\
	"..\..\..\library\src\htfile.h"\
	"..\..\..\library\src\htfilter.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htftp.h"\
	"..\..\..\library\src\htftpdir.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\htgopher.h"\
	"..\..\..\library\src\htguess.h"\
	"..\..\..\library\src\htheader.h"\
	"..\..\..\library\src\hthist.h"\
	"..\..\..\library\src\hthome.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\hticons.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htinit.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htlocal.h"\
	"..\..\..\library\src\htlog.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmerge.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htmime.h"\
	"..\..\..\library\src\htmimerq.h"\
	"..\..\..\library\src\htmimimp.h"\
	"..\..\..\library\src\htmlgen.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htmulpar.h"\
	"..\..\..\library\src\htmulti.h"\
	"..\..\..\library\src\htndir.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htnews.h"\
	"..\..\..\library\src\htnewsls.h"\
	"..\..\..\library\src\htnewsrq.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htpep.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htproxy.h"\
	"..\..\..\library\src\htreader.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htrules.h"\
	"..\..\..\library\src\htschunk.h"\
	"..\..\..\library\src\htsocket.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httchunk.h"\
	"..\..\..\library\src\httcp.h"\
	"..\..\..\library\src\httee.h"\
	"..\..\..\library\src\httelnet.h"\
	"..\..\..\library\src\httexgen.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\http.h"\
	"..\..\..\library\src\httpgen.h"\
	"..\..\..\library\src\httpreq.h"\
	"..\..\..\library\src\httpres.h"\
	"..\..\..\library\src\httpserv.h"\
	"..\..\..\library\src\httputil.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwriter.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\htxparse.h"\
	"..\..\..\library\src\htzip.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwapp.h"\
	"..\..\..\library\src\wwwcache.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwdir.h"\
	"..\..\..\library\src\wwwfile.h"\
	"..\..\..\library\src\wwwftp.h"\
	"..\..\..\library\src\wwwgophe.h"\
	"..\..\..\library\src\wwwhtml.h"\
	"..\..\..\library\src\wwwhttp.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwmime.h"\
	"..\..\..\library\src\wwwnews.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\wwwzip.h"\
	

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTINI=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaabrow.h"\
	"..\..\..\library\src\htaautil.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htansi.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htbound.h"\
	"..\..\..\library\src\htbufwrt.h"\
	"..\..\..\library\src\htcache.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htconlen.h"\
	"..\..\..\Library\src\HTDemux.h"\
	"..\..\..\library\src\htdescpt.h"\
	"..\..\..\library\src\htdialog.h"\
	"..\..\..\library\src\htdir.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htevtlst.h"\
	"..\..\..\library\src\htfile.h"\
	"..\..\..\library\src\htfilter.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htftp.h"\
	"..\..\..\library\src\htftpdir.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\htgopher.h"\
	"..\..\..\library\src\htguess.h"\
	"..\..\..\library\src\htheader.h"\
	"..\..\..\library\src\hthist.h"\
	"..\..\..\library\src\hthome.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\hticons.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htinit.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htlocal.h"\
	"..\..\..\library\src\htlog.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmerge.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htmime.h"\
	"..\..\..\library\src\htmimerq.h"\
	"..\..\..\library\src\htmimimp.h"\
	"..\..\..\library\src\htmlgen.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htmulpar.h"\
	"..\..\..\library\src\htmulti.h"\
	"..\..\..\Library\src\HTMuxCh.h"\
	"..\..\..\Library\src\HTMuxHeader.h"\
	"..\..\..\Library\src\HTMuxTx.h"\
	"..\..\..\library\src\htndir.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htnews.h"\
	"..\..\..\library\src\htnewsls.h"\
	"..\..\..\library\src\htnewsrq.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htpep.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htproxy.h"\
	"..\..\..\library\src\htreader.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htrules.h"\
	"..\..\..\library\src\htschunk.h"\
	"..\..\..\library\src\htsocket.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httchunk.h"\
	"..\..\..\library\src\httcp.h"\
	"..\..\..\library\src\httee.h"\
	"..\..\..\library\src\httelnet.h"\
	"..\..\..\library\src\httexgen.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\http.h"\
	"..\..\..\library\src\httpgen.h"\
	"..\..\..\library\src\httpreq.h"\
	"..\..\..\library\src\httpres.h"\
	"..\..\..\library\src\httpserv.h"\
	"..\..\..\library\src\httputil.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\HTWAIS.h"\
	"..\..\..\library\src\htwriter.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\htxparse.h"\
	"..\..\..\library\src\htzip.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwapp.h"\
	"..\..\..\library\src\wwwcache.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwdir.h"\
	"..\..\..\library\src\wwwfile.h"\
	"..\..\..\library\src\wwwftp.h"\
	"..\..\..\library\src\wwwgophe.h"\
	"..\..\..\library\src\wwwhtml.h"\
	"..\..\..\library\src\wwwhttp.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwmime.h"\
	"..\..\..\Library\src\WWWMux.h"\
	"..\..\..\library\src\wwwnews.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\WWWWAIS.h"\
	"..\..\..\library\src\wwwzip.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTINI=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTInit.obj" : $(SOURCE) $(DEP_CPP_HTINI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTML.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTML_=\
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
	"..\..\..\library\src\htext.h"\
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
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlgen.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httexgen.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwhtml.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTML_=\
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
	"..\..\..\library\src\htext.h"\
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
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlgen.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httexgen.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwhtml.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTML_=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTML.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTMLPDTD.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTMLP=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTMLP=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTMLP=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTMLPDTD.obj" : $(SOURCE) $(DEP_CPP_HTMLP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTPlain.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTPLA=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htext.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htplain.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTPLA=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htext.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htplain.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htstyle.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwutil.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPLA=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTPlain.obj" : $(SOURCE) $(DEP_CPP_HTPLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTProfil.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_HTPRO=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaabrow.h"\
	"..\..\..\library\src\htaautil.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htansi.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htbinit.h"\
	"..\..\..\library\src\htbound.h"\
	"..\..\..\library\src\htbufwrt.h"\
	"..\..\..\library\src\htcache.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htconlen.h"\
	"..\..\..\library\src\htdescpt.h"\
	"..\..\..\library\src\htdialog.h"\
	"..\..\..\library\src\htdir.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htevtlst.h"\
	"..\..\..\library\src\htfile.h"\
	"..\..\..\library\src\htfilter.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htftp.h"\
	"..\..\..\library\src\htftpdir.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\htgopher.h"\
	"..\..\..\library\src\htguess.h"\
	"..\..\..\library\src\htheader.h"\
	"..\..\..\library\src\hthinit.h"\
	"..\..\..\library\src\hthist.h"\
	"..\..\..\library\src\hthome.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\hticons.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htinit.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htlocal.h"\
	"..\..\..\library\src\htlog.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmerge.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htmime.h"\
	"..\..\..\library\src\htmimerq.h"\
	"..\..\..\library\src\htmimimp.h"\
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlgen.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htmulpar.h"\
	"..\..\..\library\src\htmulti.h"\
	"..\..\..\library\src\htndir.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htnews.h"\
	"..\..\..\library\src\htnewsls.h"\
	"..\..\..\library\src\htnewsrq.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htpep.h"\
	"..\..\..\library\src\htplain.h"\
	"..\..\..\library\src\htprofil.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htproxy.h"\
	"..\..\..\library\src\htreader.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htrules.h"\
	"..\..\..\library\src\htschunk.h"\
	"..\..\..\library\src\htsocket.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httchunk.h"\
	"..\..\..\library\src\httcp.h"\
	"..\..\..\library\src\httee.h"\
	"..\..\..\library\src\httelnet.h"\
	"..\..\..\library\src\httexgen.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\http.h"\
	"..\..\..\library\src\httpgen.h"\
	"..\..\..\library\src\httpreq.h"\
	"..\..\..\library\src\httpres.h"\
	"..\..\..\library\src\httpserv.h"\
	"..\..\..\library\src\httputil.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\htwriter.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\htxparse.h"\
	"..\..\..\library\src\htzip.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwapp.h"\
	"..\..\..\library\src\wwwcache.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwdir.h"\
	"..\..\..\library\src\wwwfile.h"\
	"..\..\..\library\src\wwwftp.h"\
	"..\..\..\library\src\wwwgophe.h"\
	"..\..\..\library\src\wwwhtml.h"\
	"..\..\..\library\src\wwwhttp.h"\
	"..\..\..\library\src\wwwinit.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwmime.h"\
	"..\..\..\library\src\wwwnews.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\wwwzip.h"\
	

"$(INTDIR)\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_HTPRO=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaabrow.h"\
	"..\..\..\library\src\htaautil.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htansi.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htassoc.h"\
	"..\..\..\library\src\htatom.h"\
	"..\..\..\library\src\htbind.h"\
	"..\..\..\library\src\htbinit.h"\
	"..\..\..\library\src\htbound.h"\
	"..\..\..\library\src\htbufwrt.h"\
	"..\..\..\library\src\htcache.h"\
	"..\..\..\library\src\htchannl.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htconlen.h"\
	"..\..\..\library\src\htdescpt.h"\
	"..\..\..\library\src\htdialog.h"\
	"..\..\..\library\src\htdir.h"\
	"..\..\..\library\src\htdns.h"\
	"..\..\..\library\src\hterror.h"\
	"..\..\..\library\src\htescape.h"\
	"..\..\..\library\src\htevent.h"\
	"..\..\..\library\src\htevtlst.h"\
	"..\..\..\library\src\htfile.h"\
	"..\..\..\library\src\htfilter.h"\
	"..\..\..\library\src\htformat.h"\
	"..\..\..\library\src\htftp.h"\
	"..\..\..\library\src\htftpdir.h"\
	"..\..\..\library\src\htfwrite.h"\
	"..\..\..\library\src\htgopher.h"\
	"..\..\..\library\src\htguess.h"\
	"..\..\..\library\src\htheader.h"\
	"..\..\..\library\src\hthinit.h"\
	"..\..\..\library\src\hthist.h"\
	"..\..\..\library\src\hthome.h"\
	"..\..\..\library\src\hthost.h"\
	"..\..\..\library\src\hthstman.h"\
	"..\..\..\library\src\hticons.h"\
	"..\..\..\library\src\htinet.h"\
	"..\..\..\library\src\htinit.h"\
	"..\..\..\library\src\htiostream.h"\
	"..\..\..\library\src\htlib.h"\
	"..\..\..\library\src\htlink.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htlocal.h"\
	"..\..\..\library\src\htlog.h"\
	"..\..\..\library\src\htmemlog.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htmerge.h"\
	"..\..\..\library\src\htmethod.h"\
	"..\..\..\library\src\htmime.h"\
	"..\..\..\library\src\htmimerq.h"\
	"..\..\..\library\src\htmimimp.h"\
	"..\..\..\library\src\html.h"\
	"..\..\..\library\src\htmlgen.h"\
	"..\..\..\library\src\htmlpdtd.h"\
	"..\..\..\library\src\htmulpar.h"\
	"..\..\..\library\src\htmulti.h"\
	"..\..\..\library\src\htndir.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htnews.h"\
	"..\..\..\library\src\htnewsls.h"\
	"..\..\..\library\src\htnewsrq.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htpep.h"\
	"..\..\..\library\src\htplain.h"\
	"..\..\..\library\src\htprofil.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htproxy.h"\
	"..\..\..\library\src\htreader.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htresponse.h"\
	"..\..\..\library\src\htrules.h"\
	"..\..\..\library\src\htschunk.h"\
	"..\..\..\library\src\htsocket.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\httchunk.h"\
	"..\..\..\library\src\httcp.h"\
	"..\..\..\library\src\httee.h"\
	"..\..\..\library\src\httelnet.h"\
	"..\..\..\library\src\httexgen.h"\
	"..\..\..\library\src\httimer.h"\
	"..\..\..\library\src\http.h"\
	"..\..\..\library\src\httpgen.h"\
	"..\..\..\library\src\httpreq.h"\
	"..\..\..\library\src\httpres.h"\
	"..\..\..\library\src\httpserv.h"\
	"..\..\..\library\src\httputil.h"\
	"..\..\..\library\src\httrans.h"\
	"..\..\..\library\src\htuser.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\htutree.h"\
	"..\..\..\library\src\htuu.h"\
	"..\..\..\library\src\HTWAIS.h"\
	"..\..\..\library\src\htwriter.h"\
	"..\..\..\library\src\htwwwstr.h"\
	"..\..\..\library\src\htxparse.h"\
	"..\..\..\library\src\htzip.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwapp.h"\
	"..\..\..\library\src\wwwcache.h"\
	"..\..\..\library\src\wwwcore.h"\
	"..\..\..\library\src\wwwdir.h"\
	"..\..\..\library\src\wwwfile.h"\
	"..\..\..\library\src\wwwftp.h"\
	"..\..\..\library\src\wwwgophe.h"\
	"..\..\..\library\src\wwwhtml.h"\
	"..\..\..\library\src\wwwhttp.h"\
	"..\..\..\library\src\wwwinit.h"\
	"..\..\..\library\src\wwwlib.h"\
	"..\..\..\library\src\wwwmime.h"\
	"..\..\..\library\src\wwwnews.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\WWWWAIS.h"\
	"..\..\..\library\src\wwwzip.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTPRO=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTProfil.obj" : $(SOURCE) $(DEP_CPP_HTPRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\lib.c
DEP_CPP_LIB_C=\
	".\lib.h"\
	

"$(INTDIR)\lib.obj" : $(SOURCE) $(DEP_CPP_LIB_C) "$(INTDIR)"


SOURCE=.\scroll.c
DEP_CPP_SCROL=\
	".\lib.h"\
	".\scroll.h"\
	

"$(INTDIR)\scroll.obj" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"


SOURCE=.\www.c

!IF  "$(CFG)" == "www - Win32 Release"

DEP_CPP_WWW_C=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaabrow.h"\
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
	"..\..\..\library\src\htheader.h"\
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
	"..\..\..\library\src\htmimprs.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htreqman.h"\
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
	"..\htbrowse.h"\
	".\lib.h"\
	".\scroll.h"\
	

"$(INTDIR)\www.obj" : $(SOURCE) $(DEP_CPP_WWW_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "www - Win32 Debug"

DEP_CPP_WWW_C=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaabrow.h"\
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
	"..\..\..\library\src\htheader.h"\
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
	"..\..\..\library\src\htmimprs.h"\
	"..\..\..\library\src\htnet.h"\
	"..\..\..\library\src\htparse.h"\
	"..\..\..\library\src\htprot.h"\
	"..\..\..\library\src\htreq.h"\
	"..\..\..\library\src\htreqman.h"\
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
	"..\htbrowse.h"\
	".\lib.h"\
	".\scroll.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WWW_C=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\www.obj" : $(SOURCE) $(DEP_CPP_WWW_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\www.rc
DEP_RSC_WWW_R=\
	".\WWW.ICO"\
	

"$(INTDIR)\www.res" : $(SOURCE) $(DEP_RSC_WWW_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

