$ WRITE SYS$OUTPUT "Creating WWW LineMode for "MULTINET" on "VAX"." 
$ WRITE SYS$OUTPUT "=================================================" 
$ IF "''F$SEARCH("[--.VAX.MULTINET]*.*")'" .EQS. "" 	   THEN CREATE/DIR [--.VAX.MULTINET]
$ copy []setup.com [--.VAX.MULTINET]setup.com
$ cc /DEFINE=(DEBUG,VL="""2.12""",RULES,ACCESS_AUTH,MULTINET)/INC=[---.Library.Implementation]/obj=[--.VAX.MULTINET]HTBROWSE.obj [-]HTBrowse.c
$ cc /DEFINE=(DEBUG,VL="""2.12""",RULES,ACCESS_AUTH,MULTINET)/INC=[---.Library.Implementation]/obj=[--.VAX.MULTINET]GRIDTEXT.obj [-]GridText.c
$ cc /DEFINE=(DEBUG,VL="""2.12""",RULES,ACCESS_AUTH,MULTINET)/INC=[---.Library.Implementation]/obj=[--.VAX.MULTINET]DEFAULTSTYLES.obj [-]DefaultStyles.c
$ link /exe=[--.VAX.MULTINET]www.exe [--.VAX.MULTINET]HTBrowse.obj, [--.VAX.MULTINET]GridText.obj, [--.VAX.MULTINET]DefaultStyles.obj, [---.LIBRARY.VAX.MULTINET]wwwlib/lib, [---.LIBRARY.VAX.MULTINET]www_lib.opt/opt
$ continue
$ continue
