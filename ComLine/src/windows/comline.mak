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
	-@erase "$(INTDIR)\HTLine.obj"
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
	"$(INTDIR)\HTLine.obj" \
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
	-@erase "$(INTDIR)\HTLine.obj"
	-@erase "$(INTDIR)\HTLine.sbr"
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
	"$(INTDIR)\HTLine.sbr"

"$(OUTDIR)\comline.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\comline.pdb" /debug /machine:I386 /out:"$(OUTDIR)\comline.exe" 
LINK32_OBJS= \
	"$(INTDIR)\HTLine.obj" \
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
SOURCE=..\HTLine.c

!IF  "$(CFG)" == "comline - Win32 Release"

DEP_CPP_HTLIN=\
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
	"..\htline.h"\
	

"$(INTDIR)\HTLine.obj" : $(SOURCE) $(DEP_CPP_HTLIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "comline - Win32 Debug"

DEP_CPP_HTLIN=\
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
	"..\..\..\library\src\HTXML.h"\
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
	"..\htline.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HTLIN=\
	"..\..\..\library\src\HTVMSUtils.h"\
	

"$(INTDIR)\HTLine.obj"	"$(INTDIR)\HTLine.sbr" : $(SOURCE) $(DEP_CPP_HTLIN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

