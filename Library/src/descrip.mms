!	Make WorldWideWeb LIBRARY under VMS
!       =======================================================
!
! History:
!  14 Aug 91 (TBL)	Reconstituted
!  25 Jun 92 (JFG)	Added TCP socket emulation over DECnet
!  07 Sep 93 (MD)	Remade for version 2.09a
!
! Bugs:
!	The dependencies are anything but complete - they were
!	just enough to allow the files to be compiled.
!
! Instructions:
! 	Copy [WWW.LIBRARY.VMS]DESCRIP.MMS into [WWW.LIBRARY.IMPLEMENTATION]
!	Use the correct command line for your TCP/IP implementation,
!	inside the IMPLEMENTATION directory:
!
!	$ MMS/MACRO=(MULTINET=1)	for Multinet
!	$ MMS/MACRO=(WIN_TCP=1)		for Wollongong TCP/IP
!	$ MMS/MACRO=(UCX=1)		for DEC/UCX
!	$ MMS/MACRO=(DECNET=1)		for socket emulation over DECnet
!
! To compile with debug mode:
!	
!	$ MMS/MACRO=(MULTINET=1, DEBUG=1)	for Multinet
!
!
! If you are on HEP net and want to build using the really latest sources on
! PRIAM:: then define an extra macro U=PRIAM::, e.g.
!
!	$ MMS/MACRO=(MULTINET=1, U=PRIAM::)	for Multinet
!
! This will copy the sources from PRIAM as necessary. You can also try
!
!	$ MMS/MACRO=(U=PRIAM::) descrip.mms
!
! to update this file.
!
!


! debug flags
.IFDEF DEBUG
DEBUGFLAGS = /DEBUG/NOOPT
.ENDIF

! defines valid for all compilations
EXTRADEFINES = DEBUG,XMOSAIC_HACK_REMOVED_NOW

.IFDEF UCX
LIBS = sys$library:ucx$ipc/lib		! For UCX
OPTION_FILE = 
CFLAGS = $(DEBUGFLAGS)/DEFINE=($(EXTRADEFINES))
TCP=UCX
.ENDIF
.IFDEF MULTINET
LIBS = multinet.opt/opt			! For Multinet
OPTION_FILE = multinet.opt
CFLAGS = $(DEBUGFLAGS)/DEFINE=($(EXTRADEFINES),MULTINET)
TCP=MULTINET                   
.ENDIF
.IFDEF WIN_TCP
LIBS = win_tcp.opt/opt			! For Wollongong TCP
OPTION_FILE = win_tcp.opt
CFLAGS = $(DEBUGFLAGS)/DEFINE=($(EXTRADEFINES),WIN_TCP)
TCP=WIN_TCP
.ENDIF
.IFDEF DECNET
LIBS =  disk$c3:[hemmer.unix.usr.lib]libc.opt/opt	! TCP socket library over DECnet
OPTION_FILE = disk$c3:[hemmer.unix.usr.lib]libc.opt
CFLAGS = $(DEBUGFLAGS)/DEFINE=($(EXTRADEFINES),DECNET)
TCP=DECNET
.ENDIF

.IFDEF LIBS
.ELSE
LIBS = multinet.opt/opt			! (Default to multinet)
OPTION_FILE = multinet.opt
CFLAGS = $(DEBUGFLAGS)/DEFINE=($(EXTRADEFINES),MULTINET)
TCP=MULTINET
.ENDIF

.INCLUDE Version.make

.IFDEF DECNET  ! Strip FTP, Gopher, News, WAIS
HEADERS = HTUtils.h, HTStream.h, tcp.h, HText.h, -
        HTParse.h, HTAccess.h, HTTP.h, HTFile.h, - 
	HTBTree.h, HTTCP.h, SGML.h, -
	HTML.h, HTMLDTD.h, HTChunk.h, HTPlain.h, -
	HTWriter.h, HTFwriter.h,HTMLGen.h, -
	HTAtom.h, HTAnchor.h, HTStyle.h, -
	HTList.h, HTString.h, HTAlert.h, -
	HTRules.h, HTFormat.h, HTInit.h, -
	HTMIME.h, HTHistory.h, HTTelnet.h

MODULES = HTParse, HTAccess, HTTP, HTFile, - 
	HTBTree, HTTCP, SGML, -
	HTML, HTMLDTD, HTChunk, HTPlain, -
	HTWriter, HTFwriter,HTMLGen, -
	HTAtom, HTAnchor, HTStyle, -
	HTList, HTString, HTAlert, -
	HTRules, HTFormat, HTInit, -
	HTMIME, HTHistory, HTTelnet

.ELSE
HEADERS = HTUtils.h, HTStream.h, tcp.h, HText.h, -
        HTParse.h, HTAccess.h, HTTP.h, HTFile.h, - 
	HTBTree.h, HTTCP.h, SGML.h, -
	HTML.h, HTMLDTD.h, HTChunk.h, HTPlain.h, -
	HTWriter.h, HTFwriter.h,HTMLGen.h, -
	HTAtom.h, HTAnchor.h, HTStyle.h, -
	HTList.h, HTString.h, HTAlert.h, -
	HTRules.h, HTFormat.h, HTInit.h, -
	HTMIME.h, HTHistory.h, HTTelnet.h, -         
	HTFTP.h, HTGopher.h, HTNews.h, HTWSRC.h, HTWAIS.h

MODULES = HTParse, HTAccess, HTTP, HTFile, - 
	HTBTree, HTTCP, SGML, -
	HTML, HTMLDTD, HTChunk, HTPlain, -
	HTWriter, HTFwriter,HTMLGen, -
	HTAtom, HTAnchor, HTStyle, -
	HTList, HTString, HTAlert, -
	HTRules, HTFormat, HTInit, -
	HTMIME, HTHistory, HTTelnet, -         
	HTFTP, HTGopher, HTNews, HTWSRC

.ENDIF

!___________________________________________________________________
! WWW Library

lib : $(HEADERS)  wwwlib_$(TCP)($(MODULES))  build_$(TCP).com
 	@ continue

build_$(TCP).com : descrip.mms
	mms/noaction/from_sources/out=build_$(TCP).com/macro=($(tcp)=1) 
!___________________________________________________________________
! BASIC modules

!_____________________________	HTAtom

HTAtom.obj   : HTAtom.c HTAtom.h HTUtils.h HTString.h
        cc $(CFLAGS)/obj=$*.obj HTAtom.c
.IFDEF U
HTAtom.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAtom.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAtom.c" - 
             HTAtom.c
HTAtom.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAtom.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAtom.h" -
             HTAtom.h
.ENDIF
!_____________________________	HTChunk

HTChunk.obj   : HTChunk.c HTChunk.h HTUtils.h
        cc $(CFLAGS)/obj=$*.obj HTChunk.c
.IFDEF U
HTChunk.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTChunk.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTChunk.c" - 
             HTChunk.c
HTChunk.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTChunk.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTChunk.h" -
             HTChunk.h
.ENDIF
!_____________________________	HTList

HTList.obj   : HTList.c HTList.h HTUtils.h
        cc $(CFLAGS)/obj=$*.obj HTList.c
.IFDEF U
HTList.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTList.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTList.c" - 
             HTList.c
HTList.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTList.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTList.h" -
             HTList.h
.ENDIF
!_____________________________	HTString

HTString.obj   : HTString.c HTString.h tcp.h Version.make HTUtils.h
        cc $(CFLAGS)/obj=$*.obj /define=(VC="""$(VC)""") HTString.c
.IFDEF U
HTString.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTString.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTString.c" - 
             HTString.c
HTString.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTString.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTString.h" -
             HTString.h
.ENDIF

!    C O M M O N	M O D U L E S

!_____________________________	HTBTree

HTBTree.obj   : HTBTree.c HTBTree.h HTUtils.h
        cc $(CFLAGS)/obj=$*.obj HTBTree.c
.IFDEF U
HTBTree.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTBTree.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTBTree.c" - 
             HTBTree.c
HTBTree.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTBTree.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTBTree.h" -
             HTBTree.h
.ENDIF
!_____________________________	HTMLDTD

HTMLDTD.obj   : HTMLDTD.c HTMLDTD.h SGML.h
        cc $(CFLAGS)/obj=$*.obj HTMLDTD.c
.IFDEF U
HTMLDTD.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMLDTD.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMLDTD.c" - 
             HTMLDTD.c
HTMLDTD.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMLDTD.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMLDTD.h" -
             HTMLDTD.h
.ENDIF
!_____________________________	HTPlain

HTPlain.obj   : HTPlain.c HTPlain.h HTStream.h
        cc $(CFLAGS)/obj=$*.obj HTPlain.c
.IFDEF U
HTPlain.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTPlain.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTPlain.c" - 
             HTPlain.c
HTPlain.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTPlain.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTPlain.h" -
             HTPlain.h
.ENDIF
!_____________________________	HTWriter

HTWriter.obj   : HTWriter.c HTWriter.h HTStream.h
        cc $(CFLAGS)/obj=$*.obj HTWriter.c
.IFDEF U
HTWriter.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWriter.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWriter.c" - 
             HTWriter.c
HTWriter.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWriter.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWriter.h" -
             HTWriter.h
.ENDIF
!_____________________________	HTFWriter

HTFWriter.obj   : HTFWriter.c HTFWriter.h HTStream.h
        cc $(CFLAGS)/obj=$*.obj HTFWriter.c
.IFDEF U
HTFWriter.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFWriter.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFWriter.c" - 
             HTFWriter.c
HTFWriter.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFWriter.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFWriter.h" -
             HTFWriter.h
.ENDIF
!_____________________________	HTMLGen

HTMLGen.obj   : HTMLGen.c HTMLGen.h HTUtils.h HTMLDTD.h
        cc $(CFLAGS)/obj=$*.obj HTMLGen.c
.IFDEF U
HTMLGen.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMLGen.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMLGen.c" - 
             HTMLGen.c
HTMLGen.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMLGen.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMLGen.h" -
             HTMLGen.h
.ENDIF
!_____________________________	HTAlert

HTAlert.obj   : HTAlert.c HTAlert.h HTUtils.h Version.make
        cc $(CFLAGS)/obj=$*.obj HTAlert.c
.IFDEF U
HTAlert.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAlert.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAlert.c" - 
             HTAlert.c
HTAlert.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAlert.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAlert.h" -
             HTAlert.h
.ENDIF
!_____________________________	HTRules

HTRules.obj   : HTRules.c HTRules.h HTUtils.h Version.make
        cc $(CFLAGS)/obj=$*.obj HTRules.c
.IFDEF U
HTRules.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTRules.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTRules.c" - 
             HTRules.c
HTRules.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTRules.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTRules.h" -
             HTRules.h
.ENDIF
!_____________________________	HTInit

HTInit.obj   : HTInit.c HTInit.h HTUtils.h HTList.h
        cc $(CFLAGS)/obj=$*.obj HTInit.c
.IFDEF U
HTInit.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTInit.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTInit.c" - 
             HTInit.c
HTInit.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTInit.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTInit.h" -
             HTInit.h
.ENDIF
!_____________________________	HTMIME

HTMIME.obj   : HTMIME.c HTMIME.h HTUtils.h HTList.h
        cc $(CFLAGS)/obj=$*.obj HTMIME.c
.IFDEF U
HTMIME.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMIME.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMIME.c" - 
             HTMIME.c
HTMIME.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMIME.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTMIME.h" -
             HTMIME.h
.ENDIF
!_____________________________	HTTelnet

HTTelnet.obj   : HTTelnet.c HTTelnet.h HTUtils.h
        cc $(CFLAGS)/obj=$*.obj HTTelnet.c
.IFDEF U
HTTelnet.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTelnet.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTelnet.c" - 
             HTTelnet.c
HTTelnet.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTelnet.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTelnet.h" -
             HTTelnet.h
.ENDIF
!_____________________________	HTWAIS
.IFDEF DECNET
.ELSE
HTWAIS.obj   : HTWAIS.c HTWAIS.h HTUtils.h HTList.h
        cc $(CFLAGS)/obj=$*.obj HTWAIS.c
.IFDEF U
HTWAIS.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWAIS.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWAIS.c" - 
             HTWAIS.c
HTWAIS.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWAIS.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWAIS.h" -
             HTWAIS.h
.ENDIF
.ENDIF
!_____________________________	HTWSRC
.IFDEF DECNET
.ELSE
HTWSRC.obj   : HTWSRC.c HTWSRC.h HTUtils.h HTList.h
        cc $(CFLAGS)/obj=$*.obj HTWSRC.c
.IFDEF U
HTWSRC.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWSRC.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWSRC.c" - 
             HTWSRC.c
HTWSRC.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWSRC.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTWSRC.h" -
             HTWSRC.h
.ENDIF
.ENDIF
!_____________________________	HTAccess

HTAccess.obj   : HTAccess.c HTAccess.h HTUtils.h
        cc $(CFLAGS)/obj=$*.obj HTAccess.c
.IFDEF U
HTAccess.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAccess.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAccess.c" - 
             HTAccess.c
HTAccess.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAccess.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAccess.h" -
             HTAccess.h
.ENDIF
!_____________________________	HTAnchor

HTAnchor.obj   : HTAnchor.c HTAnchor.h HTUtils.h HTList.h
        cc $(CFLAGS)/obj=$*.obj HTAnchor.c
.IFDEF U
HTAnchor.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAnchor.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAnchor.c" - 
             HTAnchor.c
HTAnchor.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAnchor.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTAnchor.h" -
             HTAnchor.h
.ENDIF

!_________________________________ HTFile

HTFile.obj   : HTFile.c HTFile.h HTUtils.h 
         cc $(CFLAGS)/obj=$*.obj HTFile.c
.IFDEF U
HTFile.c   : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFile.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFile.c" - 
             HTFile.c
HTFile.h   : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFile.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFile.h" -
             HTFile.h
.ENDIF

!_____________________________	HTFormat

HTFormat.obj   : HTFormat.c HTFormat.h HTUtils.h HTML.h SGML.h HTPlain.h HTMLGen.h HTList.h
        cc $(CFLAGS)/obj=$*.obj HTFormat.c
.IFDEF U
HTFormat.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFormat.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFormat.c" - 
             HTFormat.c
HTFormat.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFormat.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFormat.h" -
             HTFormat.h
.ENDIF
!__________________________________ HTFTP
.IFDEF DECNET
.ELSE
HTFTP.obj   : HTFTP.c HTFTP.h HTUtils.h 
        cc $(CFLAGS)/obj=$*.obj HTFTP.c
.IFDEF U
HTFTP.c    : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFTP.c"
             copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFTP.c" -
             HTFTP.c
HTFTP.h    : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFTP.h"
             copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTFTP.h" - 
             HTFTP.h
.ENDIF
.ENDIF

!_____________________________	HTGopher
.IFDEF DECNET
.ELSE
HTGopher.obj   : HTGopher.c HTGopher.h HTUtils.h HTList.h
        cc $(CFLAGS)/obj=$*.obj HTGopher.c
.IFDEF U
HTGopher.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTGopher.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTGopher.c" - 
             HTGopher.c
HTGopher.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTGopher.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTGopher.h" -
             HTGopher.h
.ENDIF
.ENDIF
!_____________________________	HTHistory

HTHistory.obj   : HTHistory.c HTHistory.h HTUtils.h HTList.h
        cc $(CFLAGS)/obj=$*.obj HTHistory.c
.IFDEF U
HTHistory.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTHistory.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTHistory.c" - 
             HTHistory.c
HTHistory.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTHistory.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTHistory.h" -
             HTHistory.h
.ENDIF
!_____________________________	HTNews
.IFDEF DECNET
.ELSE
HTNews.obj   : HTNews.c HTNews.h HTUtils.h HTList.h
        cc $(CFLAGS)/obj=$*.obj HTNews.c
.IFDEF U
HTNews.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTNews.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTNews.c" - 
             HTNews.c
HTNews.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTNews.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTNews.h" -
             HTNews.h
.ENDIF
.ENDIF
!_____________________________	HTML

HTML.obj   : HTML.c HTML.h HTUtils.h HTMLDTD.h
        cc $(CFLAGS)/obj=$*.obj HTML.c
.IFDEF U
HTML.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTML.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTML.c" - 
             HTML.c
HTML.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTML.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTML.h" -
             HTML.h
.ENDIF
!________________________________ HTParse

HTParse.obj   : HTParse.c HTParse.h HTUtils.h 
        cc $(CFLAGS)/obj=$*.obj HTParse.c
.IFDEF U
HTParse.c  : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTParse.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTParse.c" - 
             HTParse.c
HTParse.h  : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTParse.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTParse.h" -
             HTParse.h
.ENDIF

!_____________________________	HTStyle

HTStyle.obj   : HTStyle.c HTStyle.h HTUtils.h 
        cc $(CFLAGS)/obj=$*.obj HTStyle.c
.IFDEF U
HTStyle.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTStyle.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTStyle.c" - 
             HTStyle.c
HTStyle.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTStyle.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTStyle.h" -
             HTStyle.h
.ENDIF
! _________________________________ HTTCP

HTTCP.obj : HTTCP.c HTTCP.h HTUtils.h tcp.h
         cc $(CFLAGS)/obj=$*.obj HTTCP.c
.IFDEF U
HTTCP.c    : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTCP.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTCP.c" - 
             HTTCP.c
HTTCP.h    : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTCP.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTCP.h" - 
             HTTCP.h
.ENDIF
!________________________________ HTTP

HTTP.obj   : HTTP.c HTTP.h HTUtils.h
        cc $(CFLAGS)/obj=$*.obj HTTP.c
.IFDEF U
HTTP.c     : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTP.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTP.c" -
             HTTP.c
HTTP.h     : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTP.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTTP.h" -
             HTTP.h 
.ENDIF
!_____________________________	SGML

SGML.obj   : SGML.c SGML.h HTUtils.h
        cc $(CFLAGS)/obj=$*.obj SGML.c
.IFDEF U
SGML.c : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/SGML.c"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/SGML.c" - 
             SGML.c
SGML.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/SGML.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/SGML.h" -
             SGML.h
.ENDIF
!_________________________________ include files only:

.IFDEF U
HTUtils.h  : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTUtils.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTUtils.h" -
             HTUtils.h
HTStream.h : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTStream.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HTStream.h" -
             HTStream.h
tcp.h      : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/tcp.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/tcp.h" - 
             tcp.h
HText.h      : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HText.h"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/HText.h" - 
             HText.h
README      : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/README"
	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/README" - 
             README
!WWW.h      : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/WWW.h"
!	     copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/WWW.h" -
!             WWW.h
.ENDIF
 
! ______________________________  The version file

.IFDEF U
Version.make :  $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/Version.make"
	copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/Version.make" - 
             Version.make
	write sys$output: "Please rebuild with new Version file"
	exit 2	! Error
.ENDIF

! _____________________________VMS SPECIAL FILES:
! latest version of this one:

.IFDEF U
descrip.mms : $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/descrip.mms"
	copy $(U)"/userd/tbl/hypertext/WWW/Library/Implementation/descrip.mms" -
	descrip.mms
	write sys$output: "Please rebuild with new MMS file"
	exit 2	! Error

.ENDIF


