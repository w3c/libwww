!
!	Make WorldWideWeb LIBRARY under VMS
!       =======================================================
!
! History:
!  14 Aug 91 (TBL)	Reconstituted
!  25 Jun 92 (JFG)	Added TCP socket emulation over DECnet
!  07 Sep 93 (MD)	Remade for version 2.09a
!  02 Nov 93 (MD)	Added new modules for AA 2.12 including VMS specific
!  14 Nov 93 (MD)	Added HTVMSUtils.c and .h
!  22 Feb 94 (MD)	Changed for version 2.15
!			- took out modules
!			- (WG) added DECC, LIST flags
!			- added ALPHA flag
!			- generates obj directory structure...
!
! Bugs:
!	The dependencies are anything but complete - they were
!	just enough to allow the files to be compiled.
!
! Instructions:
!	Use the correct command line for your TCP/IP implementation,
!	inside the VMS directory:
!                                                                       target dir
!	$ MMS/MACRO=(MULTINET=1)	for Multinet,Vax 		[.VAX.MULTINET]
!	$ MMS/MACRO=(WIN_TCP=1)		for Wollongong TCP/IP           [.VAX.WIN_TCP]
!	$ MMS/MACRO=(UCX=1)		for DEC/UCX                     [.VAX.UCX]
!	$ MMS/MACRO=(DECNET=1)		for socket emul over DECnet	[.VAX.DECNET]
!
! To compile with debug mode:
!	
!	$ MMS/MACRO=(DEBUG=1)       	Debug Flag on
!	$ MMS/MACRO=(LIST=1)       	Produce Listing Files
!
! To compile for DECC use:
!
!	$ MMS/MACRO=(DECC=1)		for DECC only. Generates .OLB
!
! But to compile for ALPHA use:
!                                                                       target dir
!	$ MMS/MACRO=(ALPHA=1)		on ALPHA (implies DECC) 	[.ALPHA.MULTINET]
!
! If you are on HEP net and want to build using the really latest sources on
! DXCERN:: then define an extra macro U=DXCERN::, e.g.
!
!	$ MMS/MACRO=(MULTINET=1, U=DXCERN::)	for Multinet
!
! This will copy the sources from DXCERN as necessary. You can also try
!
!	$ MMS/MACRO=(U=DXCERN::) descrip.mms
!
! to update this file.
!
!


SRC = [-]
VMS = []

.INCLUDE $(SRC)Version.make

! debug flags
.IFDEF DEBUG
DEBUGFLAGS = /DEBUG/NOOPT
.ENDIF

.IFDEF ALPHA
ALPHA_EXT=_ALPHA
MACH=ALPHA
DECC=1
.ELSE
ALPHA_EXT= 
MACH=VAX
.ENDIF

.IFDEF DECC
.IFDEF UCX
CQUALDECC=/Standard=VAXC/Prefix=ALL
.ELSE
CQUALDECC=/Standard=VAXC/Prefix=ANSI
.ENDIF
.ELSE
CQUALDECC=
.ENDIF

.IFDEF LIST
CLIST=/LIST/SHOW=ALL
.ELSE
CLIST=
.ENDIF

! defines valid for all compilations
EXTRADEFINES = VMS,DEBUG,ACCESS_AUTH,VC="""$(VC)"""

.IFDEF UCX
TCP=UCX
.ENDIF
.IFDEF MULTINET
TCP=MULTINET
.ENDIF
.IFDEF WIN_TCP
TCP=WIN_TCP
.ENDIF
.IFDEF DECNET
TCP=DECNET
.ENDIF

.IFDEF TCP
.ELSE
TCP=MULTINET
.ENDIF

! now lib points at [--.machine.tcp layer]
LIB=[--.$(MACH).$(TCP)]
CFLAGS = $(DEBUGFLAGS)$(CQUALDECC)$(CLIST)/DEFINE=($(EXTRADEFINES),$(TCP))/INC=($(VMS),$(SRC))



SETUP_FILES = $(LIB)wwwlib.opt

VMS_FILES = $(VMS)COPYING.LIB $(VMS)descrip.mms -
	    $(VMS)multinet.opt $(VMS)multinet.opt_alpha -
	    $(VMS)ucx.opt $(VMS)ucx.opt_alpha

HEADERS = $(SRC)HTParse.h, $(SRC)HTAccess.h, $(SRC)HTTP.h, $(SRC)HTFile.h, -
	$(SRC)HTMulti.h, - 
	$(SRC)HTBTree.h, $(SRC)HTFTP.h, $(SRC)HTTCP.h, -
	$(SRC)SGML.h, $(SRC)HTML.h, $(SRC)HTMLPDTD.h, $(SRC)HTChunk.h, -
	$(SRC)HTPlain.h, $(SRC)HTWriter.h, -
	$(SRC)HTFwriter.h, $(SRC)HTMLGen.h, -
	$(SRC)HTStream.h, $(SRC)HTTee.h, -
	$(SRC)HTAtom.h, $(SRC)HTAnchor.h, $(SRC)HTStyle.h, -
	$(SRC)HTList.h, -
 	$(SRC)HTString.h, $(SRC)HTAlert.h, $(SRC)HTRules.h, -
	$(SRC)HTFormat.h, $(SRC)HTInit.h, -
	$(SRC)HTMIME.h, $(SRC)HTHistory.h, $(SRC)HTNews.h, -
	$(SRC)HTGopher.h, -
        $(SRC)HTUtils.h, $(SRC)tcp.h, $(SRC)HText.h, -
	$(SRC)HTTelnet.h, -
	$(SRC)HTWAIS.h, $(SRC)HTWSRC.h, -
        $(SRC)HTAAUtil.h, $(SRC)HTAABrow.h, $(SRC)HTAssoc.h, -
	$(SRC)HTUU.h, $(SRC)HTTeXGen.h, $(SRC)HTDirBrw.h, -
	$(SRC)HTDescript.h, $(SRC)HTGuess.h, $(SRC)HTIcons.h -
        $(SRC)HTError.h, -
	$(VMS)HTVMSUtils.h, -
	$(VMS)ufc-crypt.h, $(VMS)patchlevel.h -
	$(VMS)dirent.h, $(VMS)sys_dirent.h

.IFDEF DECNET  ! Strip FTP, Gopher, News, (WAIS)
MODULES = HTParse=$(LIB)HTParse.obj, HTAccess=$(LIB)HTAccess.obj, HTTP=$(LIB)HTTP.obj, -
	HTFile=$(LIB)HTFile.obj, HTMulti=$(LIB)HTMulti.obj, HTBTree=$(LIB)HTBTree.obj, HTTCP=$(LIB)HTTCP.obj, -
	SGML=$(LIB)SGML.obj, HTML=$(LIB)HTML.obj, HTMLPDTD=$(LIB)HTMLPDTD.obj, HTChunk=$(LIB)HTChunk.obj, -
	HTPlain=$(LIB)HTPlain.obj, HTWriter=$(LIB)HTWriter.obj, HTFwriter=$(LIB)HTFwriter.obj, -
	HTMLGen=$(LIB)HTMLGen.obj, HTTee=$(LIB)HTTee.obj, -
	HTAtom=$(LIB)HTAtom.obj, HTAnchor=$(LIB)HTAnchor.obj, HTStyle=$(LIB)HTStyle.obj, -
	HTList=$(LIB)HTList.obj, HTString=$(LIB)HTString.obj, HTAlert=$(LIB)HTAlert.obj, -
	HTRules=$(LIB)HTRules.obj, HTFormat=$(LIB)HTFormat.obj, HTInit=$(LIB)HTInit.obj, HTMIME=$(LIB)HTMIME.obj, -
	HTHistory=$(LIB)HTHistory.obj, -         
	HTTelnet=$(LIB)HTTelnet.obj, HTWSRC=$(LIB)HTWSRC.obj, -
        HTAAUtil=$(LIB)HTAAUtil.obj, HTAABrow=$(LIB)HTAABrow.obj, HTAssoc=$(LIB)HTAssoc.obj, - 
	HTUU=$(LIB)HTUU.obj, HTTeXGen=$(LIB)HTTeXGen.obj, -
	HTDirBrw=$(LIB)HTDirBrw.obj, HTDescript=$(LIB)HTDescript.obj, HTGuess=$(LIB)HTGuess.obj, -
        HTIcons=$(LIB)HTIcons.obj, HTError=$(LIB)HTError.obj, HTErrorMsg=$(LIB)HTErrorMsg.obj, -
	HTVMSUtils=$(LIB)HTVMSUtils.obj, -
 	getpass=$(LIB)getpass.obj, getline=$(LIB)getline.obj, -
	crypt=$(LIB)crypt.obj, crypt_util=$(LIB)crypt_util.obj -
        dirent=$(LIB)dirent.obj
.ELSE                   
MODULES = HTParse=$(LIB)HTParse.obj, HTAccess=$(LIB)HTAccess.obj, HTTP=$(LIB)HTTP.obj, -
	HTFile=$(LIB)HTFile.obj, HTMulti=$(LIB)HTMulti.obj, HTBTree=$(LIB)HTBTree.obj, HTFTP=$(LIB)HTFTP.obj, HTTCP=$(LIB)HTTCP.obj, -
	SGML=$(LIB)SGML.obj, HTML=$(LIB)HTML.obj, HTMLPDTD=$(LIB)HTMLPDTD.obj, HTChunk=$(LIB)HTChunk.obj, -
	HTPlain=$(LIB)HTPlain.obj, HTWriter=$(LIB)HTWriter.obj, HTFwriter=$(LIB)HTFwriter.obj, -
	HTMLGen=$(LIB)HTMLGen.obj, HTTee=$(LIB)HTTee.obj, -
	HTAtom=$(LIB)HTAtom.obj, HTAnchor=$(LIB)HTAnchor.obj, HTStyle=$(LIB)HTStyle.obj, -
	HTList=$(LIB)HTList.obj, HTString=$(LIB)HTString.obj, HTAlert=$(LIB)HTAlert.obj, -
	HTRules=$(LIB)HTRules.obj, HTFormat=$(LIB)HTFormat.obj, HTInit=$(LIB)HTInit.obj, HTMIME=$(LIB)HTMIME.obj, -
	HTHistory=$(LIB)HTHistory.obj, HTNews=$(LIB)HTNews.obj, HTGopher=$(LIB)HTGopher.obj, -         
	HTTelnet=$(LIB)HTTelnet.obj, HTWSRC=$(LIB)HTWSRC.obj, -
        HTAAUtil=$(LIB)HTAAUtil.obj, HTAABrow=$(LIB)HTAABrow.obj, HTAssoc=$(LIB)HTAssoc.obj, - 
	HTUU=$(LIB)HTUU.obj, HTTeXGen=$(LIB)HTTeXGen.obj, -
	HTDirBrw=$(LIB)HTDirBrw.obj, HTDescript=$(LIB)HTDescript.obj, HTGuess=$(LIB)HTGuess.obj, -
        HTIcons=$(LIB)HTIcons.obj, HTError=$(LIB)HTError.obj, HTErrorMsg=$(LIB)HTErrorMsg.obj, -
	HTVMSUtils=$(LIB)HTVMSUtils.obj, -
 	getpass=$(LIB)getpass.obj, getline=$(LIB)getline.obj, -
	crypt=$(LIB)crypt.obj, crypt_util=$(LIB)crypt_util.obj -
        dirent=$(LIB)dirent.obj
.ENDIF

!___________________________________________________________________
!.obj.olb
!	IF "''F$SEARCH("$(MMS$TARGET)")'" .EQS. "" -
!	   THEN $(LIBR)/CREATE $(MMS$TARGET)
!	$(LIBR) $(LIBRFLAGS) $(MMS$TARGET) $(LIB)$(MMS$SOURCE)
!
!___________________________________________________________________
.FIRST
	@ WRITE SYS$OUTPUT "Creating WWWLIB for "$(TCP)" on "$(MACH)"." 
	@ WRITE SYS$OUTPUT "=================================================" 
	@ IF "''F$SEARCH("$(LIB)*.*")'" .EQS. "" -
	   THEN CREATE/DIR $(LIB)
!___________________________________________________________________
! WWW Library

lib : $(SRC)Version.make $(VMS_FILES) $(HEADERS) $(LIB)wwwlib.opt $(LIB)wwwlib.olb($(MODULES)) $(VMS)build_$(TCP).com$(ALPHA_EXT)
 	@ continue

lib_only : $(SRC)Version.make $(LIB)wwwlib.opt $(LIB)wwwlib.olb($(MODULES))
 	@ continue

$(VMS)build_$(TCP).com$(ALPHA_EXT) : $(VMS)descrip.mms
	mms/noaction/from_sources/out=$(VMS)build_$(TCP).com$(ALPHA_EXT)/macro=($(TCP)=1,$(MACH)=1) lib_only

$(LIB)wwwlib.opt : $(VMS)$(TCP).opt$(ALPHA_EXT)
	copy $(VMS)$(TCP).opt$(ALPHA_EXT) $(LIB)wwwlib.opt
!___________________________________________________________________
! BASIC modules


!    C O M M O N	M O D U L E S

!_____________________________	HTList

$(LIB)HTList.obj   : $(SRC)HTList.c $(SRC)HTList.h $(SRC)HTUtils.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTList.c
.IFDEF U
$(SRC)HTList.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTList.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTList.c" - 
             $(SRC)HTList.c
$(SRC)HTList.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTList.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTList.h" -
             $(SRC)HTList.h
.ENDIF
!_____________________________	HTAnchor

$(LIB)HTAnchor.obj   : $(SRC)HTAnchor.c $(SRC)HTAnchor.h $(SRC)HTUtils.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTAnchor.c
.IFDEF U
$(SRC)HTAnchor.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAnchor.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAnchor.c" - 
             $(SRC)HTAnchor.c
$(SRC)HTAnchor.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAnchor.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAnchor.h" -
             $(SRC)HTAnchor.h
.ENDIF

!_____________________________	HTFormat

$(LIB)HTFormat.obj   : $(SRC)HTFormat.c $(SRC)HTFormat.h $(SRC)HTUtils.h $(SRC)HTML.h $(SRC)SGML.h $(SRC)HTPlain.h $(SRC)HTMLGen.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTFormat.c
.IFDEF U
$(SRC)HTFormat.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFormat.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFormat.c" - 
             $(SRC)HTFormat.c
$(SRC)HTFormat.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFormat.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFormat.h" -
             $(SRC)HTFormat.h
.ENDIF
!_____________________________	HTGuess

$(LIB)HTGuess.obj   : $(SRC)HTGuess.c $(SRC)HTGuess.h $(SRC)HTUtils.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTGuess.c
.IFDEF U
$(SRC)HTGuess.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTGuess.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTGuess.c" - 
             $(SRC)HTGuess.c
$(SRC)HTGuess.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTGuess.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTGuess.h" -
             $(SRC)HTGuess.h
.ENDIF
!_____________________________	HTIcons

$(LIB)HTIcons.obj   : $(SRC)HTIcons.c $(SRC)HTIcons.h $(SRC)HTInit.h $(SRC)HTUtils.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTIcons.c
.IFDEF U
$(SRC)HTIcons.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTIcons.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTIcons.c" - 
             $(SRC)HTIcons.c
$(SRC)HTIcons.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTIcons.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTIcons.h" -
             $(SRC)HTIcons.h
.ENDIF
!_____________________________	HTInit

$(LIB)HTInit.obj   : $(SRC)HTInit.c $(SRC)HTInit.h $(SRC)HTUtils.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTInit.c
.IFDEF U
$(SRC)HTInit.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTInit.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTInit.c" - 
             $(SRC)HTInit.c
$(SRC)HTInit.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTInit.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTInit.h" -
             $(SRC)HTInit.h
.ENDIF
!_____________________________	HTMIME

$(LIB)HTMIME.obj   : $(SRC)HTMIME.c $(SRC)HTMIME.h $(SRC)HTUtils.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTMIME.c
.IFDEF U
$(SRC)HTMIME.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMIME.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMIME.c" - 
             $(SRC)HTMIME.c
$(SRC)HTMIME.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMIME.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMIME.h" -
             $(SRC)HTMIME.h
.ENDIF
!_____________________________	HTHistory

$(LIB)HTHistory.obj   : $(SRC)HTHistory.c $(SRC)HTHistory.h $(SRC)HTUtils.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTHistory.c
.IFDEF U
$(SRC)HTHistory.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTHistory.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTHistory.c" - 
             $(SRC)HTHistory.c
$(SRC)HTHistory.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTHistory.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTHistory.h" -
             $(SRC)HTHistory.h
.ENDIF
!_____________________________	HTNews
.IFDEF DECNET
.ELSE
$(LIB)HTNews.obj   : $(SRC)HTNews.c $(SRC)HTNews.h $(SRC)HTUtils.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTNews.c
.IFDEF U
$(SRC)HTNews.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTNews.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTNews.c" - 
             $(SRC)HTNews.c
$(SRC)HTNews.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTNews.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTNews.h" -
             $(SRC)HTNews.h
.ENDIF
.ENDIF
!_____________________________	HTGopher
.IFDEF DECNET
.ELSE
$(LIB)HTGopher.obj   : $(SRC)HTGopher.c $(SRC)HTGopher.h $(SRC)HTUtils.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTGopher.c
.IFDEF U
$(SRC)HTGopher.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTGopher.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTGopher.c" - 
             $(SRC)HTGopher.c
$(SRC)HTGopher.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTGopher.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTGopher.h" -
             $(SRC)HTGopher.h
.ENDIF
.ENDIF
!_____________________________	HTTelnet

$(LIB)HTTelnet.obj   : $(SRC)HTTelnet.c $(SRC)HTTelnet.h $(SRC)HTUtils.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTTelnet.c
.IFDEF U
$(SRC)HTTelnet.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTelnet.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTelnet.c" - 
             $(SRC)HTTelnet.c
$(SRC)HTTelnet.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTelnet.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTelnet.h" -
             $(SRC)HTTelnet.h
.ENDIF
!_____________________________	HTStyle

$(LIB)HTStyle.obj   : $(SRC)HTStyle.c $(SRC)HTStyle.h $(SRC)HTUtils.h 
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTStyle.c
.IFDEF U
$(SRC)HTStyle.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTStyle.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTStyle.c" - 
             $(SRC)HTStyle.c
$(SRC)HTStyle.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTStyle.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTStyle.h" -
             $(SRC)HTStyle.h
.ENDIF
!_____________________________	HTAtom

$(LIB)HTAtom.obj   : $(SRC)HTAtom.c $(SRC)HTAtom.h $(SRC)HTUtils.h $(SRC)HTString.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTAtom.c
.IFDEF U
$(SRC)HTAtom.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAtom.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAtom.c" - 
             $(SRC)HTAtom.c
$(SRC)HTAtom.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAtom.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAtom.h" -
             $(SRC)HTAtom.h
.ENDIF
!_____________________________	HTChunk

$(LIB)HTChunk.obj   : $(SRC)HTChunk.c $(SRC)HTChunk.h $(SRC)HTUtils.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTChunk.c
.IFDEF U
$(SRC)HTChunk.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTChunk.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTChunk.c" - 
             $(SRC)HTChunk.c
$(SRC)HTChunk.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTChunk.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTChunk.h" -
             $(SRC)HTChunk.h
.ENDIF
!_____________________________	HTString

$(LIB)HTString.obj   : $(SRC)HTString.c $(SRC)HTString.h $(SRC)tcp.h $(SRC)Version.make $(SRC)HTUtils.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTString.c
.IFDEF U
$(SRC)HTString.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTString.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTString.c" - 
             $(SRC)HTString.c
$(SRC)HTString.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTString.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTString.h" -
             $(SRC)HTString.h
.ENDIF
!_____________________________	HTAlert

$(LIB)HTAlert.obj   : $(SRC)HTAlert.c $(SRC)HTAlert.h $(SRC)HTUtils.h 
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTAlert.c
.IFDEF U
$(SRC)HTAlert.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAlert.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAlert.c" - 
             $(SRC)HTAlert.c
$(SRC)HTAlert.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAlert.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAlert.h" -
             $(SRC)HTAlert.h
.ENDIF
!_____________________________	HTRules

$(LIB)HTRules.obj   : $(SRC)HTRules.c $(SRC)HTRules.h $(SRC)HTUtils.h 
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTRules.c
.IFDEF U
$(SRC)HTRules.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTRules.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTRules.c" - 
             $(SRC)HTRules.c
$(SRC)HTRules.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTRules.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTRules.h" -
             $(SRC)HTRules.h
.ENDIF
!_____________________________	SGML

$(LIB)SGML.obj   : $(SRC)SGML.c $(SRC)SGML.h $(SRC)HTUtils.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)SGML.c
.IFDEF U
$(SRC)SGML.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/SGML.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/SGML.c" - 
             $(SRC)SGML.c
$(SRC)SGML.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/SGML.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/SGML.h" -
             $(SRC)SGML.h
.ENDIF
!_____________________________	HTML

$(LIB)HTML.obj   : $(SRC)HTML.c $(SRC)HTML.h $(SRC)HTUtils.h $(SRC)HTMLPDTD.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTML.c
.IFDEF U
$(SRC)HTML.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTML.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTML.c" - 
             $(SRC)HTML.c
$(SRC)HTML.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTML.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTML.h" -
             $(SRC)HTML.h
.ENDIF
!_____________________________	HTMLGen

$(LIB)HTMLGen.obj   : $(SRC)HTMLGen.c $(SRC)HTMLGen.h $(SRC)HTUtils.h $(SRC)HTMLPDTD.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTMLGen.c
.IFDEF U
$(SRC)HTMLGen.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMLGen.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMLGen.c" - 
             $(SRC)HTMLGen.c
$(SRC)HTMLGen.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMLGen.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMLGen.h" -
             $(SRC)HTMLGen.h
.ENDIF
!_____________________________	HTTeXGen

$(LIB)HTTeXGen.obj   : $(SRC)HTTeXGen.c $(SRC)HTTeXGen.h $(SRC)HTUtils.h $(SRC)HTMLPDTD.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTTeXGen.c
.IFDEF U
$(SRC)HTTeXGen.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTeXGen.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTeXGen.c" - 
             $(SRC)HTTeXGen.c
$(SRC)HTTeXGen.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTeXGen.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTeXGen.h" -
             $(SRC)HTTeXGen.h
.ENDIF
!_____________________________	HTMLPDTD

$(LIB)HTMLPDTD.obj   : $(SRC)HTMLPDTD.c $(SRC)HTMLPDTD.h $(SRC)SGML.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTMLPDTD.c
.IFDEF U
$(SRC)HTMLPDTD.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMLPDTD.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMLPDTD.c" - 
             $(SRC)HTMLPDTD.c
$(SRC)HTMLPDTD.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMLPDTD.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMLPDTD.h" -
             $(SRC)HTMLPDTD.h
.ENDIF
!_____________________________	HTPlain

$(LIB)HTPlain.obj   : $(SRC)HTPlain.c $(SRC)HTPlain.h $(SRC)HTStream.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTPlain.c
.IFDEF U
$(SRC)HTPlain.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTPlain.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTPlain.c" - 
             $(SRC)HTPlain.c
$(SRC)HTPlain.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTPlain.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTPlain.h" -
             $(SRC)HTPlain.h
.ENDIF
!_____________________________	HTTee

$(LIB)HTTee.obj   : $(SRC)HTTee.c $(SRC)HTTee.h $(SRC)HTStream.h 
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTTee.c
.IFDEF U
$(SRC)HTTee.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTee.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTee.c" - 
             $(SRC)HTTee.c
$(SRC)HTTee.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTee.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTee.h" -
             $(SRC)HTTee.h
.ENDIF
!_____________________________	HTWAIS
.IFDEF DECNET
.ELSE
$(LIB)HTWAIS.obj   : $(SRC)HTWAIS.c $(SRC)HTWAIS.h $(SRC)HTUtils.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTWAIS.c
.IFDEF U
$(SRC)HTWAIS.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWAIS.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWAIS.c" - 
             $(SRC)HTWAIS.c
$(SRC)HTWAIS.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWAIS.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWAIS.h" -
             $(SRC)HTWAIS.h
.ENDIF
.ENDIF
!_____________________________	HTWSRC
.IFDEF DECNET
.ELSE
$(LIB)HTWSRC.obj   : $(SRC)HTWSRC.c $(SRC)HTWSRC.h $(SRC)HTUtils.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTWSRC.c
.IFDEF U
$(SRC)HTWSRC.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWSRC.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWSRC.c" - 
             $(SRC)HTWSRC.c
$(SRC)HTWSRC.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWSRC.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWSRC.h" -
             $(SRC)HTWSRC.h
.ENDIF
.ENDIF
!_____________________________	HTWriter

$(LIB)HTWriter.obj   : $(SRC)HTWriter.c $(SRC)HTWriter.h $(SRC)HTStream.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTWriter.c
.IFDEF U
$(SRC)HTWriter.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWriter.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWriter.c" - 
             $(SRC)HTWriter.c
$(SRC)HTWriter.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWriter.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTWriter.h" -
             $(SRC)HTWriter.h
.ENDIF
!_____________________________	HTFWriter

$(LIB)HTFWriter.obj   : $(SRC)HTFWriter.c $(SRC)HTFWriter.h $(SRC)HTStream.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTFWriter.c
.IFDEF U
$(SRC)HTFWriter.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFWriter.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFWriter.c" - 
             $(SRC)HTFWriter.c
$(SRC)HTFWriter.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFWriter.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFWriter.h" -
             $(SRC)HTFWriter.h
.ENDIF

! Access Authorisation Code


!_____________________________	HTAAUtil

$(LIB)HTAAUtil.obj   : $(SRC)HTAAUtil.c $(SRC)HTAAUtil.h $(SRC)HTUtils.h $(SRC)HTString.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTAAUtil.c
.IFDEF U
$(SRC)HTAAUtil.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAAUtil.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAAUtil.c" - 
             $(SRC)HTAAUtil.c
$(SRC)HTAAUtil.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAAUtil.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAAUtil.h" -
             $(SRC)HTAAUtil.h
.ENDIF
!_____________________________	HTAABrow

$(LIB)HTAABrow.obj   : $(SRC)HTAABrow.c $(SRC)HTAABrow.h $(SRC)HTAAUtil.h $(SRC)HTUU.h $(SRC)HTUtils.h $(SRC)HTString.h $(SRC)HTParse.h $(SRC)HTList.h $(SRC)HTAlert.h $(SRC)HTAssoc.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTAABrow.c
.IFDEF U
$(SRC)HTAABrow.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAABrow.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAABrow.c" - 
             $(SRC)HTAABrow.c
$(SRC)HTAABrow.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAABrow.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAABrow.h" -
             $(SRC)HTAABrow.h
.ENDIF
!_____________________________	HTAssoc

$(LIB)HTAssoc.obj   : $(SRC)HTAssoc.c $(SRC)HTAssoc.h $(SRC)HTUtils.h $(SRC)HTString.h $(SRC)HTList.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTAssoc.c
.IFDEF U
$(SRC)HTAssoc.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAssoc.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAssoc.c" - 
             $(SRC)HTAssoc.c
$(SRC)HTAssoc.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAssoc.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAssoc.h" -
             $(SRC)HTAssoc.h
.ENDIF
!_____________________________	HTUU

$(LIB)HTUU.obj   : $(SRC)HTUU.c $(SRC)HTUU.h $(SRC)HTUtils.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTUU.c
.IFDEF U
$(SRC)HTUU.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTUU.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTUU.c" - 
             $(SRC)HTUU.c
$(SRC)HTUU.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTUU.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTUU.h" -
             $(SRC)HTUU.h
.ENDIF

! Communication and Files

!________________________________ HTTP

$(LIB)HTTP.obj   : $(SRC)HTTP.c $(SRC)HTTP.h $(SRC)HTUtils.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTTP.c
.IFDEF U
$(SRC)HTTP.c     : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTP.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTP.c" -
             $(SRC)HTTP.c
$(SRC)HTTP.h     : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTP.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTP.h" -
             $(SRC)HTTP.h 
.ENDIF

! _________________________________ HTTCP

$(LIB)HTTCP.obj : $(SRC)HTTCP.c $(SRC)HTTCP.h $(SRC)HTUtils.h $(SRC)tcp.h
         cc $(CFLAGS)/obj=$*.obj $(SRC)HTTCP.c
.IFDEF U
$(SRC)HTTCP.c    : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTCP.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTCP.c" - 
             $(SRC)HTTCP.c
$(SRC)HTTCP.h    : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTCP.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTTCP.h" - 
             $(SRC)HTTCP.h
.ENDIF
!_________________________________ HTFile

$(LIB)HTFile.obj : $(SRC)HTFile.c $(SRC)HTFile.h $(SRC)HTUtils.h 
         cc $(CFLAGS)/obj=$*.obj $(SRC)HTFile.c
.IFDEF U
$(SRC)HTFile.c   : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFile.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFile.c" - 
             $(SRC)HTFile.c
$(SRC)HTFile.h   : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFile.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFile.h" -
             $(SRC)HTFile.h
.ENDIF
!_________________________________ HTDirBrw

$(LIB)HTDirBrw.obj : $(SRC)HTDirBrw.c $(SRC)HTDirBrw.h $(SRC)HTUtils.h 
         cc $(CFLAGS)/obj=$*.obj $(SRC)HTDirBrw.c
.IFDEF U
$(SRC)HTDirBrw.c   : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTDirBrw.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTDirBrw.c" - 
             $(SRC)HTDirBrw.c
$(SRC)HTDirBrw.h   : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTDirBrw.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTDirBrw.h" -
             $(SRC)HTDirBrw.h
.ENDIF
!_________________________________ HTDescript

$(LIB)HTDescript.obj : $(SRC)HTDescript.c $(SRC)HTDescript.h $(SRC)HTUtils.h 
         cc $(CFLAGS)/obj=$*.obj $(SRC)HTDescript.c
.IFDEF U
$(SRC)HTDescript.c   : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTDescript.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTDescript.c" - 
             $(SRC)HTDescript.c
$(SRC)HTDescript.h   : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTDescript.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTDescript.h" -
             $(SRC)HTDescript.h
.ENDIF
!_________________________________ HTMulti

$(LIB)HTMulti.obj : $(SRC)HTMulti.c $(SRC)HTMulti.h $(SRC)HTUtils.h 
         cc $(CFLAGS)/obj=$*.obj $(SRC)HTMulti.c
.IFDEF U
$(SRC)HTMulti.c   : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMulti.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMulti.c" - 
             $(SRC)HTMulti.c
$(SRC)HTMulti.h   : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMulti.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTMulti.h" -
             $(SRC)HTMulti.h
.ENDIF
!_____________________________	HTBTree

$(LIB)HTBTree.obj   : $(SRC)HTBTree.c $(SRC)HTBTree.h $(SRC)HTUtils.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTBTree.c
.IFDEF U
$(SRC)HTBTree.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTBTree.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTBTree.c" - 
             $(SRC)HTBTree.c
$(SRC)HTBTree.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTBTree.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTBTree.h" -
             $(SRC)HTBTree.h
.ENDIF

!__________________________________ HTFTP
.IFDEF DECNET
.ELSE
$(LIB)HTFTP.obj   : $(SRC)HTFTP.c $(SRC)HTFTP.h $(SRC)HTUtils.h 
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTFTP.c
.IFDEF U
$(SRC)HTFTP.c    : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFTP.c"
             copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFTP.c" -
             $(SRC)HTFTP.c
$(SRC)HTFTP.h    : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFTP.h"
             copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTFTP.h" - 
             $(SRC)HTFTP.h
.ENDIF
.ENDIF

!_____________________________	HTAccess

$(LIB)HTAccess.obj   : $(SRC)HTAccess.c $(SRC)HTAccess.h $(SRC)HTUtils.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTAccess.c
.IFDEF U
$(SRC)HTAccess.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAccess.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAccess.c" - 
             $(SRC)HTAccess.c
$(SRC)HTAccess.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAccess.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTAccess.h" -
             $(SRC)HTAccess.h
.ENDIF

!________________________________ HTParse

$(LIB)HTParse.obj   : $(SRC)HTParse.c $(SRC)HTParse.h $(SRC)HTUtils.h 
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTParse.c
.IFDEF U
$(SRC)HTParse.c  : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTParse.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTParse.c" - 
             $(SRC)HTParse.c
$(SRC)HTParse.h  : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTParse.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTParse.h" -
             $(SRC)HTParse.h
.ENDIF

!________________________________ HTError

$(LIB)HTError.obj   : $(SRC)HTError.c $(SRC)HTError.h 
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTError.c
.IFDEF U
$(SRC)HTError.c  : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTError.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTError.c" - 
             $(SRC)HTError.c
$(SRC)HTError.h  : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTError.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTError.h" -
             $(SRC)HTError.h
.ENDIF

!________________________________ HTErrorMsg

$(LIB)HTErrorMsg.obj   : $(SRC)HTErrorMsg.c $(SRC)HTError.h $(SRC)HTUtils.h 
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTErrorMsg.c
.IFDEF U
$(SRC)HTErrorMsg.c  : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTErrorMsg.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTErrorMsg.c" - 
             $(SRC)HTErrorMsg.c
.ENDIF

!_________________________________ include files only:

.IFDEF U
$(SRC)HTUtils.h  : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTUtils.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTUtils.h" -
             $(SRC)HTUtils.h
$(SRC)HTStream.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTStream.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HTStream.h" -
             $(SRC)HTStream.h
$(SRC)tcp.h      : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/tcp.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/tcp.h" - 
             $(SRC)tcp.h
$(SRC)HText.h      : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HText.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/HText.h" - 
             $(SRC)HText.h
$(SRC)README      : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/README"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/README" - 
             $(SRC)README
.ENDIF
 
! ______________________________  The version file

.IFDEF U
$(SRC)Version.make :  $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/Version.make"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/Version.make" - 
             $(SRC)Version.make
	@- write sys$output "Please rebuild with new Version file"
	@- exit 2	! Error
.ENDIF

! _____________________________VMS SPECIFIC FILES:
! latest version of this one:

.IFDEF U
$(VMS)descrip.mms : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/descrip.mms"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/descrip.mms" -
	$(VMS)descrip.mms
	@- write sys$output "Please rebuild with new MMS file"
	@- exit 2	! Error

$(VMS)COPYING.LIB : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/COPYING.LIB"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/COPYING.LIB" -
	$(VMS)COPYING.LIB
                    
$(VMS)multinet.opt : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/multinet.opt"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/multinet.opt" -
	$(VMS)multinet.opt
                    
$(VMS)multinet.opt_alpha : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/multinet.opt_alpha"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/multinet.opt_alpha" -
	$(VMS)multinet.opt_alpha
                    
$(VMS)ucx.opt : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/ucx.opt"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/ucx.opt" -
	$(VMS)ucx.opt
                    
$(VMS)ucx.opt_alpha : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/ucx.opt_alpha"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/ucx.opt_alpha" -
	$(VMS)ucx.opt_alpha
                    
.ENDIF
!_____________________________	VMS/HTVMSUTILS

$(LIB)HTVMSUtils.obj   : $(VMS)HTVMSUtils.c $(VMS)HTVMSUtils.h $(SRC)HTUtils.h $(SRC)HTAccess.h
        cc $(CFLAGS)/obj=$*.obj $(VMS)HTVMSUtils.c
.IFDEF U
$(VMS)HTVMSUtils.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/HTVMSUtils.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/HTVMSUtils.c" - 
             $(VMS)HTVMSUtils.c
$(VMS)HTVMSUtils.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/HTVMSUtils.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/HTVMSUtils.h" -
             $(VMS)HTVMSUtils.h
.ENDIF

!_____________________________	VMS/GETPASS

$(LIB)GETPASS.obj   : $(VMS)GETPASS.c
        cc $(CFLAGS)/obj=$*.obj $(VMS)GETPASS.c
.IFDEF U
$(VMS)GETPASS.c : 	$(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/getpass.c"
	            	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/getpass.c" - 
             		$(VMS)GETPASS.c
.ENDIF
!_____________________________	VMS/GETLINE

$(LIB)GETLINE.obj   : $(VMS)GETLINE.c
        cc $(CFLAGS)/obj=$*.obj $(VMS)GETLINE.c
.IFDEF U
$(VMS)GETLINE.c : 	$(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/getline.c"
	            	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/getline.c" - 
             		$(VMS)GETLINE.c
.ENDIF
!_____________________________	VMS/CRYPT

$(LIB)CRYPT.obj   : $(VMS)CRYPT.c $(VMS)UFC-CRYPT.h
        cc $(CFLAGS)/obj=$*.obj $(VMS)CRYPT.c
.IFDEF U
$(VMS)CRYPT.c : 	$(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/crypt.c"
	            	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/crypt.c" - 
             		$(VMS)CRYPT.c
.ENDIF
!_____________________________	VMS/CRYPT_UTIL

$(LIB)CRYPT_UTIL.obj   : $(VMS)CRYPT_UTIL.c $(VMS)UFC-CRYPT.h $(VMS)PATCHLEVEL.h
        cc $(CFLAGS)/obj=$*.obj $(VMS)CRYPT_UTIL.c
.IFDEF U
$(VMS)CRYPT_UTIL.c : 	$(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/crypt_util.c"
	            	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/crypt_util.c" - 
             		$(VMS)CRYPT_UTIL.c
.ENDIF
!_____________________________	VMS/DIRENT

$(LIB)dirent.obj   : $(VMS)dirent.c $(VMS)dirent.h $(VMS)sys_dirent.h 
        cc $(CFLAGS)/obj=$*.obj $(VMS)dirent.c
.IFDEF U
$(VMS)dirent.c : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/dirent.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/dirent.c" - 
             $(VMS)dirent.c
$(VMS)dirent.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/dirent.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/dirent.h" -
             $(VMS)dirent.h
$(VMS)sys_dirent.h : $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/sys_dirent.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/sys_dirent.h" -
             $(VMS)sys_dirent.h
.ENDIF

!_________________________________ VMS include files only:

.IFDEF U
$(VMS)UFC-CRYPT.h : 	$(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/ufc-crypt.h"
	            	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/ufc-crypt.h" - 
             		$(VMS)UFC-CRYPT.h
$(VMS)PATCHLEVEL.h : 	$(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/patchlevel.h"
	            	copy $(U)"/userd/tbl/hypertext/WWW-duns/Library/Implementation/vms/patchlevel.h" - 
             		$(VMS)PATCHLEVEL.h
.ENDIF



