# Microsoft Developer Studio Generated NMAKE File, Based on webbot.dsp
!IF "$(CFG)" == ""
CFG=webbot - Win32 Release
!MESSAGE No configuration specified. Defaulting to webbot - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "webbot - Win32 Release" && "$(CFG)" != "webbot - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "webbot.mak" CFG="webbot - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "webbot - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "webbot - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "webbot - Win32 Release"

OUTDIR=.\..\..\..\Bin\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\..\..\Bin\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\webbot.exe"

!ELSE 

ALL : "$(OUTDIR)\webbot.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\HTBInit.obj"
	-@erase "$(INTDIR)\HTHInit.obj"
	-@erase "$(INTDIR)\HTInit.obj"
	-@erase "$(INTDIR)\HTML.obj"
	-@erase "$(INTDIR)\HTMLPDTD.obj"
	-@erase "$(INTDIR)\HTPlain.obj"
	-@erase "$(INTDIR)\HTProfil.obj"
	-@erase "$(INTDIR)\HTQueue.obj"
	-@erase "$(INTDIR)\HTRobot.obj"
	-@erase "$(INTDIR)\HTStyle.obj"
	-@erase "$(INTDIR)\RobotMain.obj"
	-@erase "$(INTDIR)\RobotTxt.obj"
	-@erase "$(INTDIR)\SGML.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\webbot.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\Library\src" /I\
 "..\..\..\Library\External" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "WWW_WIN_DLL" /Fp"$(INTDIR)\webbot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\webbot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\webbot.pdb" /machine:I386 /out:"$(OUTDIR)\webbot.exe" 
LINK32_OBJS= \
	"$(INTDIR)\HTBInit.obj" \
	"$(INTDIR)\HTHInit.obj" \
	"$(INTDIR)\HTInit.obj" \
	"$(INTDIR)\HTML.obj" \
	"$(INTDIR)\HTMLPDTD.obj" \
	"$(INTDIR)\HTPlain.obj" \
	"$(INTDIR)\HTProfil.obj" \
	"$(INTDIR)\HTQueue.obj" \
	"$(INTDIR)\HTRobot.obj" \
	"$(INTDIR)\HTStyle.obj" \
	"$(INTDIR)\RobotMain.obj" \
	"$(INTDIR)\RobotTxt.obj" \
	"$(INTDIR)\SGML.obj" \
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
	"..\..\..\Bin\wwwzip.lib" \
	"..\..\..\Library\External\gnu_regex.lib"

"$(OUTDIR)\webbot.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

OUTDIR=.\..\..\..\Bin\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\..\..\Bin\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\webbot.exe" "$(OUTDIR)\webbot.bsc"

!ELSE 

ALL : "$(OUTDIR)\webbot.exe" "$(OUTDIR)\webbot.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\HTBInit.obj"
	-@erase "$(INTDIR)\HTBInit.sbr"
	-@erase "$(INTDIR)\HTHInit.obj"
	-@erase "$(INTDIR)\HTHInit.sbr"
	-@erase "$(INTDIR)\HTInit.obj"
	-@erase "$(INTDIR)\HTInit.sbr"
	-@erase "$(INTDIR)\HTML.obj"
	-@erase "$(INTDIR)\HTML.sbr"
	-@erase "$(INTDIR)\HTMLPDTD.obj"
	-@erase "$(INTDIR)\HTMLPDTD.sbr"
	-@erase "$(INTDIR)\HTPlain.obj"
	-@erase "$(INTDIR)\HTPlain.sbr"
	-@erase "$(INTDIR)\HTProfil.obj"
	-@erase "$(INTDIR)\HTProfil.sbr"
	-@erase "$(INTDIR)\HTQueue.obj"
	-@erase "$(INTDIR)\HTQueue.sbr"
	-@erase "$(INTDIR)\HTRobot.obj"
	-@erase "$(INTDIR)\HTRobot.sbr"
	-@erase "$(INTDIR)\HTStyle.obj"
	-@erase "$(INTDIR)\HTStyle.sbr"
	-@erase "$(INTDIR)\RobotMain.obj"
	-@erase "$(INTDIR)\RobotMain.sbr"
	-@erase "$(INTDIR)\RobotTxt.obj"
	-@erase "$(INTDIR)\RobotTxt.sbr"
	-@erase "$(INTDIR)\SGML.obj"
	-@erase "$(INTDIR)\SGML.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\webbot.bsc"
	-@erase "$(OUTDIR)\webbot.exe"
	-@erase "$(OUTDIR)\webbot.ilk"
	-@erase "$(OUTDIR)\webbot.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\Library\src" /I\
 "..\..\..\Library\External" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "WWW_WIN_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\webbot.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\webbot.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\HTBInit.sbr" \
	"$(INTDIR)\HTHInit.sbr" \
	"$(INTDIR)\HTInit.sbr" \
	"$(INTDIR)\HTML.sbr" \
	"$(INTDIR)\HTMLPDTD.sbr" \
	"$(INTDIR)\HTPlain.sbr" \
	"$(INTDIR)\HTProfil.sbr" \
	"$(INTDIR)\HTQueue.sbr" \
	"$(INTDIR)\HTRobot.sbr" \
	"$(INTDIR)\HTStyle.sbr" \
	"$(INTDIR)\RobotMain.sbr" \
	"$(INTDIR)\RobotTxt.sbr" \
	"$(INTDIR)\SGML.sbr"

"$(OUTDIR)\webbot.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\webbot.pdb" /debug /machine:I386 /out:"$(OUTDIR)\webbot.exe" 
LINK32_OBJS= \
	"$(INTDIR)\HTBInit.obj" \
	"$(INTDIR)\HTHInit.obj" \
	"$(INTDIR)\HTInit.obj" \
	"$(INTDIR)\HTML.obj" \
	"$(INTDIR)\HTMLPDTD.obj" \
	"$(INTDIR)\HTPlain.obj" \
	"$(INTDIR)\HTProfil.obj" \
	"$(INTDIR)\HTQueue.obj" \
	"$(INTDIR)\HTRobot.obj" \
	"$(INTDIR)\HTStyle.obj" \
	"$(INTDIR)\RobotMain.obj" \
	"$(INTDIR)\RobotTxt.obj" \
	"$(INTDIR)\SGML.obj" \
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
	"..\..\..\Bin\wwwzip.lib" \
	"..\..\..\Library\External\gnu_regex.lib"

"$(OUTDIR)\webbot.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "webbot - Win32 Release" || "$(CFG)" == "webbot - Win32 Debug"
SOURCE=..\..\..\Library\src\HTBInit.c

!IF  "$(CFG)" == "webbot - Win32 Release"

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


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

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
	

"$(INTDIR)\HTBInit.obj"	"$(INTDIR)\HTBInit.sbr" : $(SOURCE) $(DEP_CPP_HTBIN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTHInit.c

!IF  "$(CFG)" == "webbot - Win32 Release"

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


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

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
	

"$(INTDIR)\HTHInit.obj"	"$(INTDIR)\HTHInit.sbr" : $(SOURCE) $(DEP_CPP_HTHIN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTInit.c

!IF  "$(CFG)" == "webbot - Win32 Release"

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


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

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
	

"$(INTDIR)\HTInit.obj"	"$(INTDIR)\HTInit.sbr" : $(SOURCE) $(DEP_CPP_HTINI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTML.c

!IF  "$(CFG)" == "webbot - Win32 Release"

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


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

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
	

"$(INTDIR)\HTML.obj"	"$(INTDIR)\HTML.sbr" : $(SOURCE) $(DEP_CPP_HTML_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTMLPDTD.c

!IF  "$(CFG)" == "webbot - Win32 Release"

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


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

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
	

"$(INTDIR)\HTMLPDTD.obj"	"$(INTDIR)\HTMLPDTD.sbr" : $(SOURCE) $(DEP_CPP_HTMLP)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTPlain.c

!IF  "$(CFG)" == "webbot - Win32 Release"

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


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

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
	

"$(INTDIR)\HTPlain.obj"	"$(INTDIR)\HTPlain.sbr" : $(SOURCE) $(DEP_CPP_HTPLA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTProfil.c

!IF  "$(CFG)" == "webbot - Win32 Release"

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


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

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
	

"$(INTDIR)\HTProfil.obj"	"$(INTDIR)\HTProfil.sbr" : $(SOURCE) $(DEP_CPP_HTPRO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\HTQueue.c

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTQUE=\
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
	"..\HTQueue.h"\
	

"$(INTDIR)\HTQueue.obj" : $(SOURCE) $(DEP_CPP_HTQUE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_HTQUE=\
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
	"..\HTQueue.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTQUE=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTQueue.obj"	"$(INTDIR)\HTQueue.sbr" : $(SOURCE) $(DEP_CPP_HTQUE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\HTRobot.c

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTROB=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaabrow.h"\
	"..\..\..\library\src\htaautil.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htancman.h"\
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
	"..\..\..\library\src\wwwsql.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\wwwzip.h"\
	"..\HTQueue.h"\
	"..\HTRobMan.h"\
	"..\HTRobot.h"\
	

"$(INTDIR)\HTRobot.obj" : $(SOURCE) $(DEP_CPP_HTROB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_HTROB=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htaabrow.h"\
	"..\..\..\library\src\htaautil.h"\
	"..\..\..\library\src\htaccess.h"\
	"..\..\..\library\src\htalert.h"\
	"..\..\..\library\src\htanchor.h"\
	"..\..\..\library\src\htancman.h"\
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
	"..\..\..\library\src\HTSQL.h"\
	"..\..\..\library\src\HTSQLLog.h"\
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
	"..\..\..\library\src\wwwsql.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\WWWWAIS.h"\
	"..\..\..\library\src\wwwzip.h"\
	"..\HTQueue.h"\
	"..\HTRobMan.h"\
	"..\HTRobot.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTROB=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTRobot.obj"	"$(INTDIR)\HTRobot.sbr" : $(SOURCE) $(DEP_CPP_HTROB)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\HTStyle.c

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_HTSTY=\
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
	

"$(INTDIR)\HTStyle.obj" : $(SOURCE) $(DEP_CPP_HTSTY) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_HTSTY=\
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
	
NODEP_CPP_HTSTY=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTStyle.obj"	"$(INTDIR)\HTStyle.sbr" : $(SOURCE) $(DEP_CPP_HTSTY)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\RobotMain.c

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_ROBOT=\
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
	"..\..\..\library\src\wwwsql.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\wwwzip.h"\
	"..\HTRobMan.h"\
	"..\HTRobot.h"\
	"..\RobotTxt.h"\
	

"$(INTDIR)\RobotMain.obj" : $(SOURCE) $(DEP_CPP_ROBOT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_ROBOT=\
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
	"..\..\..\library\src\HTSQL.h"\
	"..\..\..\library\src\HTSQLLog.h"\
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
	"..\..\..\library\src\wwwsql.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\WWWWAIS.h"\
	"..\..\..\library\src\wwwzip.h"\
	"..\HTRobMan.h"\
	"..\HTRobot.h"\
	"..\RobotTxt.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ROBOT=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\RobotMain.obj"	"$(INTDIR)\RobotMain.sbr" : $(SOURCE)\
 $(DEP_CPP_ROBOT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\RobotTxt.c

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_ROBOTT=\
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
	"..\..\..\library\src\wwwsql.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\wwwzip.h"\
	"..\HTRobMan.h"\
	"..\HTRobot.h"\
	"..\RobotTxt.h"\
	

"$(INTDIR)\RobotTxt.obj" : $(SOURCE) $(DEP_CPP_ROBOTT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_ROBOTT=\
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
	"..\..\..\library\src\HTSQL.h"\
	"..\..\..\library\src\HTSQLLog.h"\
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
	"..\..\..\library\src\wwwsql.h"\
	"..\..\..\library\src\wwwstream.h"\
	"..\..\..\library\src\wwwsys.h"\
	"..\..\..\library\src\wwwtelnt.h"\
	"..\..\..\library\src\wwwtrans.h"\
	"..\..\..\library\src\wwwutil.h"\
	"..\..\..\library\src\WWWWAIS.h"\
	"..\..\..\library\src\wwwzip.h"\
	"..\HTRobMan.h"\
	"..\HTRobot.h"\
	"..\RobotTxt.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ROBOTT=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\RobotTxt.obj"	"$(INTDIR)\RobotTxt.sbr" : $(SOURCE) $(DEP_CPP_ROBOTT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\Library\src\SGML.c

!IF  "$(CFG)" == "webbot - Win32 Release"

DEP_CPP_SGML_=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	

"$(INTDIR)\SGML.obj" : $(SOURCE) $(DEP_CPP_SGML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "webbot - Win32 Debug"

DEP_CPP_SGML_=\
	"..\..\..\library\external\regex.h"\
	"..\..\..\library\src\htarray.h"\
	"..\..\..\library\src\htchunk.h"\
	"..\..\..\library\src\htlist.h"\
	"..\..\..\library\src\htmemory.h"\
	"..\..\..\library\src\htstream.h"\
	"..\..\..\library\src\htstring.h"\
	"..\..\..\library\src\htstruct.h"\
	"..\..\..\library\src\htutils.h"\
	"..\..\..\library\src\sgml.h"\
	"..\..\..\library\src\windows\config.h"\
	"..\..\..\library\src\wwwsys.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SGML_=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\SGML.obj"	"$(INTDIR)\SGML.sbr" : $(SOURCE) $(DEP_CPP_SGML_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

