$ WRITE SYS$OUTPUT "Creating WWW LineMode for "MULTINET" on "VAX"." 
$ WRITE SYS$OUTPUT "=================================================" 
$ IF "''F$SEARCH("[--.VAX.MULTINET]*.*")'" .EQS. "" 	   THEN CREATE/DIR [--.VAX.MULTINET]
$ copy []setup.com [--.VAX.MULTINET]setup.com
$ cc /DEFINE=(VMS,DEBUG,RULES,ACCESS_AUTH,VL="""2.14vms2""" ,MULTINET)/INC=([---.Library.Implementation],[---.Library.Implementation.vms])/obj=[--.VAX.MULTINET]HTBROWSE.obj [-]HTBrowse.c
$ cc /DEFINE=(VMS,DEBUG,RULES,ACCESS_AUTH,VL="""2.14vms2""" ,MULTINET)/INC=([---.Library.Implementation],[---.Library.Implementation.vms])/obj=[--.VAX.MULTINET]GRIDTEXT.obj [-]GridText.c
$ cc /DEFINE=(VMS,DEBUG,RULES,ACCESS_AUTH,VL="""2.14vms2""" ,MULTINET)/INC=([---.Library.Implementation],[---.Library.Implementation.vms])/obj=[--.VAX.MULTINET]DEFAULTSTYLES.obj [-]DefaultStyles.c
$ cc /DEFINE=(VMS,DEBUG,RULES,ACCESS_AUTH,VL="""2.14vms2""" ,MULTINET)/INC=([---.Library.Implementation],[---.Library.Implementation.vms])/obj=[--.VAX.MULTINET]GRIDSTYLE.obj [-]GridStyle.c
$ link /exe=[--.VAX.MULTINET]www.exe [--.VAX.MULTINET]HTBrowse.obj, [--.VAX.MULTINET]GridText.obj, 		[--.VAX.MULTINET]DefaultStyles.obj, [--.VAX.MULTINET]GridStyle.obj, [---.LIBRARY.VAX.MULTINET]wwwlib/lib, [---.LIBRARY.VAX.MULTINET]wwwlib.opt/opt
$ continue
$ continue
