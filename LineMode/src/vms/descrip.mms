!	Make WorldWideWeb LineMode Browser under VMS
!       =======================================================
!
! History:
!  07 Sep 93 	MD	Created for version 2.06 of linemode browser (www)
!  06 Nov 93    MD	Changed to run in [.vms] directory
!  28 Feb 94 (MD)	Changed for version 2.15 of library
!			- added DECC, LIST flags
!			- added ALPHA flag
!			- generates obj directory structure...
!
! Bugs:
!	The dependencies are anything but complete - they were
!	just enough to allow the files to be compiled.
!
! Instructions:
!	Use the correct command line for your TCP/IP implementation,
!	inside the [.VMS] directory:
!
!	$ MMS/MACRO=(MULTINET=1)	for Multinet
!	$ MMS/MACRO=(WIN_TCP=1)		for Wollongong TCP/IP
!	$ MMS/MACRO=(UCX=1)		for DEC/UCX
!	$ MMS/MACRO=(DECNET=1)		for socket emulation over DECnet
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
WWW_INCL = [---.Library.Implementation]
WWW_VMS  = [---.Library.Implementation.vms]


.INCLUDE $(SRC)Version.make

! debug flags
.IFDEF DEBUG
DEBUGFLAGS = /DEBUG/NOOPT
LFLAGS = /DEBUG
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

.IFDEF MAP
LMAP=/MAP=$(MMS$TARGET_NAME)/FULL
.ELSE
LMAP=
.ENDIF



! extra defines
.IFDEF NORULES
EXTRADEFINES = VMS,DEBUG,ACCESS_AUTH,VL="""$(VL)""" 
.ELSE
EXTRADEFINES = VMS,DEBUG,RULES,ACCESS_AUTH,VL="""$(VL)""" 
.ENDIF


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

! now exe points at [--.machine.tcp layer]
EXE=[--.$(MACH).$(TCP)]
WWW_LIB=[---.LIBRARY.$(MACH).$(TCP)]
CFLAGS = $(DEBUGFLAGS)/DEFINE=($(EXTRADEFINES),$(TCP))/INC=($(WWW_INCL),$(WWW_VMS))$(CQUALDECC)$(CLIST)

WWW_LIBS = $(WWW_LIB)wwwlib/lib


SETUP_FILES = $(LIB)setup.com

VMS_FILES = $(SRC)version.make $(VMS)setup.com $(VMS)descrip.mms

WWW_OBJECTS = 	$(EXE)HTBrowse.obj, $(EXE)GridText.obj, -
		$(EXE)DefaultStyles.obj, $(EXE)GridStyle.obj
          
WWW_HEADERS = 	$(SRC)HTBrowse.h, $(SRC)GridText.h, $(SRC)HTFont.h, -
		$(SRC)GridStyle.h

!___________________________________________________________________
.FIRST
	@ WRITE SYS$OUTPUT "Creating WWW LineMode for "$(TCP)" on "$(MACH)"." 
	@ WRITE SYS$OUTPUT "=================================================" 
	@ IF "''F$SEARCH("$(EXE)*.*")'" .EQS. "" -
	   THEN CREATE/DIR $(EXE)
!___________________________________________________________________
! ALL executables
exe : $(VMS_FILES) $(EXE)setup.com www $(VMS)build_$(TCP).com$(ALPHA_EXT)
	@ continue

exe_only : $(EXE)setup.com www
	@ continue

$(VMS)build_$(TCP).com$(ALPHA_EXT) : $(VMS)descrip.mms
	mms/noaction/from_sources/out=$(VMS)build_$(TCP).com$(ALPHA_EXT)/macro=($(TCP)=1,$(MACH)=1) exe_only
          
$(EXE)setup.com : $(VMS)setup.com
	copy $(VMS)setup.com $(EXE)setup.com

!___________________________________________________________________
! WWW EXE (LineMode Browser)

www : $(WWW_HEADERS) $(EXE)www.exe 
 	@ continue

$(EXE)www.exe :	$(WWW_OBJECTS)
	link $(LFLAGS)$(LMAP)/exe=$(EXE)www.exe $(WWW_OBJECTS), $(WWW_LIBS), $(WWW_LIB)wwwlib.opt/opt

!_____________________________	HTBrowse

$(EXE)HTBrowse.obj   : $(SRC)HTBrowse.c $(SRC)HTBrowse.h $(SRC)GridText.h $(SRC)Version.make -
		 $(WWW_INCL)HTUtils.h $(WWW_INCL)HTAccess.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)HTBrowse.c
.IFDEF U
$(SRC)HTBrowse.c : $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/HTBrowse.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/HTBrowse.c" - 
             $(SRC)HTBrowse.c
$(SRC)HTBrowse.h : $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/HTBrowse.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/HTBrowse.h" -
             $(SRC)HTBrowse.h
.ENDIF
!_____________________________	GridText

$(EXE)GridText.obj   : $(SRC)GridText.c $(SRC)GridText.h $(SRC)GridStyle.h -
		 $(WWW_INCL)HTAnchor.h $(WWW_INCL)HTStyle.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)GridText.c
.IFDEF U
$(SRC)GridText.c : $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/GridText.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/GridText.c" - 
             $(SRC)GridText.c
$(SRC)GridText.h : $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/GridText.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/GridText.h" - 
             $(SRC)GridText.h
.ENDIF
!_____________________________	GridStyle

$(EXE)GridStyle.obj   : $(SRC)GridStyle.c $(SRC)GridStyle.h -
		 $(WWW_INCL)HTAnchor.h $(WWW_INCL)HTStyle.h
        cc $(CFLAGS)/obj=$*.obj $(SRC)GridStyle.c
.IFDEF U
$(SRC)GridStyle.c : $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/GridStyle.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/GridStyle.c" - 
             $(SRC)GridStyle.c
$(SRC)GridStyle.h : $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/GridStyle.h"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/GridStyle.h" - 
             $(SRC)GridStyle.h
.ENDIF
!_____________________________	DefaultStyles

$(EXE)DefaultStyles.obj   : $(SRC)DefaultStyles.c -
		 $(WWW_INCL)HTStyle.h 
        cc $(CFLAGS)/obj=$*.obj $(SRC)DefaultStyles.c
.IFDEF U
$(SRC)DefaultStyles.c : $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/DefaultStyles.c"
	     copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/DefaultStyles.c" - 
             $(SRC)DefaultStyles.c
.ENDIF
! ______________________________ separate header files

.IFDEF U
$(SRC)HTFont.h :  $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/HTFont.h"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/HTFont.h" - 
             $(SRC)HTFont.h
.ENDIF

! ______________________________  The version file

.IFDEF U
$(SRC)Version.make :  $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/Version.make"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/Version.make" - 
             $(SRC)Version.make
	@- write sys$output "Please rebuild with new Version file"
	@- exit 2	! Error
.ENDIF

! _____________________________VMS SPECIAL FILES:
! latest version of this one:

.IFDEF U
$(VMS)descrip.mms : $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/vms/descrip.mms"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/vms/descrip.mms" -
	$(VMS)descrip.mms
	@- write sys$output "Please rebuild with new MMS file"
	@- exit 2	! Error

$(VMS)setup.com : $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/vms/setup.com"
	copy $(U)"/userd/tbl/hypertext/WWW-duns/LineMode/Implementation/vms/setup.com" -
	$(VMS)setup.com
                    
.ENDIF


