$ WRITE SYS$OUTPUT "Creating WWW LineMode for "UCX" on "VAX"." 
$ WRITE SYS$OUTPUT "=================================================" 
$ IF "''F$SEARCH("[--.VAX.UCX]*.*")'" .EQS. "" 	   THEN CREATE/DIR [--.VAX.UCX]
$ copy []setup.com [--.VAX.UCX]setup.com
$ cc /DEFINE=(DEBUG,VL="""2.12""",RULES,ACCESS_AUTH,UCX)/INC=[---.Library.Implementation]/obj=[--.VAX.UCX]HTBROWSE.obj [-]HTBrowse.c
$ cc /DEFINE=(DEBUG,VL="""2.12""",RULES,ACCESS_AUTH,UCX)/INC=[---.Library.Implementation]/obj=[--.VAX.UCX]GRIDTEXT.obj [-]GridText.c
$ cc /DEFINE=(DEBUG,VL="""2.12""",RULES,ACCESS_AUTH,UCX)/INC=[---.Library.Implementation]/obj=[--.VAX.UCX]DEFAULTSTYLES.obj [-]DefaultStyles.c
$ link /exe=[--.VAX.UCX]www.exe [--.VAX.UCX]HTBrowse.obj, [--.VAX.UCX]GridText.obj, [--.VAX.UCX]DefaultStyles.obj, [---.LIBRARY.VAX.UCX]wwwlib/lib, [---.LIBRARY.VAX.UCX]wwwlib.opt/opt
$ continue
$ continue
