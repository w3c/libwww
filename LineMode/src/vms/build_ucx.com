$ WRITE SYS$OUTPUT "Creating WWW LineMode for "UCX" on "VAX"." 
$ WRITE SYS$OUTPUT "=================================================" 
$ IF "''F$SEARCH("[--.VAX.UCX]*.*")'" .EQS. "" 	   THEN CREATE/DIR [--.VAX.UCX]
$ copy []setup.com [--.VAX.UCX]setup.com
$ cc /DEFINE=(VMS,DEBUG,RULES,ACCESS_AUTH,VL="""2.14vms2""" ,UCX)/INC=([---.Library.Implementation],[---.Library.Implementation.vms])/obj=[--.VAX.UCX]HTBROWSE.obj [-]HTBrowse.c
$ cc /DEFINE=(VMS,DEBUG,RULES,ACCESS_AUTH,VL="""2.14vms2""" ,UCX)/INC=([---.Library.Implementation],[---.Library.Implementation.vms])/obj=[--.VAX.UCX]GRIDTEXT.obj [-]GridText.c
$ cc /DEFINE=(VMS,DEBUG,RULES,ACCESS_AUTH,VL="""2.14vms2""" ,UCX)/INC=([---.Library.Implementation],[---.Library.Implementation.vms])/obj=[--.VAX.UCX]DEFAULTSTYLES.obj [-]DefaultStyles.c
$ cc /DEFINE=(VMS,DEBUG,RULES,ACCESS_AUTH,VL="""2.14vms2""" ,UCX)/INC=([---.Library.Implementation],[---.Library.Implementation.vms])/obj=[--.VAX.UCX]GRIDSTYLE.obj [-]GridStyle.c
$ link /exe=[--.VAX.UCX]www.exe [--.VAX.UCX]HTBrowse.obj, [--.VAX.UCX]GridText.obj, 		[--.VAX.UCX]DefaultStyles.obj, [--.VAX.UCX]GridStyle.obj, [---.LIBRARY.VAX.UCX]wwwlib/lib, [---.LIBRARY.VAX.UCX]wwwlib.opt/opt
$ continue
$ continue
