dnl Autoconfiguration input script.
dnl Process this file with autoconf to produce a configure script.
dnl Copyright CERN 1994; See Copyright.html for conditions.

AC_INIT(HTTP.c)
AC_CONFIG_HEADER(config.h)
AC_CONFIG_AUX_DIR($srcdir/../../Implementation)

dnl Version:
VC=3.0pre2

AC_SUBST(VC)
AC_DEFINE_UNQUOTED(VC, "$VC")

CFLAGS=
LDFLAGS=

dnl Basic compiler stuff

AC_PROG_CC
AC_ISC_POSIX

dnl Handle command-line arguments
AC_ARG_PROGRAM

AC_ARG_ENABLE(debug, [  --disable-debug         do not use this flag], ,
 AC_DEFINE(DEBUG) [CFLAGS="$CFLAGS -g"] [LDFLAGS="$LDFLAGS -g"])
AC_ARG_ENABLE(event-loop, [  --enable-event-loop     enable multithreading],
 AC_DEFINE(EVENT_LOOP),)

AC_ARG_WITH(wais, [  --with-wais             build in the WAIS searching code],
  AC_PATH_WAIS(
    AC_DEFINE(DIRECT_WAIS)
    HTWAIS="HTWAIS.o",
    AC_MSG_WARN(Cannot find WAIS; code will be built without it)
    HTWAIS=""),
  HTWAIS="")
AC_SUBST(HTWAIS)

AC_ARG_WITH(socks, [  --with-socks            build in the firewall-breaching SOCKS code],
  AC_PATH_SOCKS(
    AC_DEFINE(SOCKS),
    AC_MSG_WARN(Cannot find SOCKS; code will be built without it)
  ),
)

dnl Checks for programs:

AC_PROG_INSTALL
AC_PROG_RANLIB
AC_CHECK_PROGS(have_telnet, telnet)
if test -n $have_telnet ; then
  AC_DEFINE_UNQUOTED(TELNET_PROGRAM, "$have_telnet")
  AC_PROG_TELNET_L
fi
AC_CHECK_PROGS(have_rlogin, rlogin)
if test -n $have_rlogin ; then
  AC_DEFINE_UNQUOTED(RLOGIN_PROGRAM, "$have_rlogin")
fi
AC_CHECK_PROGS(have_tn3270, tn3270 x3270) dnl  VMS: telnet/tn3270...
if test -n $have_tn3270 ; then
  AC_DEFINE_UNQUOTED(TN3270_PROGRAM, "$have_tn3270")
fi

AC_PATH_PROG(RM, rm, rm)
AC_PATH_PROG(AR, ar, ar)
AC_PATH_PROG(MKDIR, mkdir, mkdir)
AC_PATH_PROG(CHMOD, chmod, chmod)
AC_PATH_PROG(CP, cp, cp)
AC_PATH_PROG(TAR, tar, tar)
AC_PATH_PROG(CVS, cvs, cvs)
AC_PATH_PROG(SED, sed, sed)
AC_PATH_PROGS(COMPRESS, gzip compress, compress)
AC_FILE_RESOLV_CONF
AC_PROG_LN_S

dnl One remaining platform-specific difference in the library is the
dnl default postscript and image viewers: On the NeXT they are "open,"
dnl on other platforms they are ghostview or xv.  We should decide
dnl how to handle these things; perhaps the following lines would be
dnl useful:
dnl AC_PATH_PROGS(postscript_viewer, open ghostview, ghostview)
dnl AC_DEFINE_UNQUOTED(PRESENT_POSTSCRIPT, "$postscript_viewer %s")
dnl AC_PATH_PROGS(image_viewer, open xv, xv)
dnl AC_DEFINE_UNQUOTED(PRESENT_IMAGE, "$image_viewer %s")

dnl Checks for libraries: (needed for AC_WEIRD_ACCEPT)

AC_CHECK_LIB(socket, main)
AC_CHECK_LIB(inet, main)
AC_CHECK_LIB(nsl, main)
AC_WEIRD_ACCEPT

dnl Checks for header files:

AC_CHECK_HEADERS(stdio.h)
AC_CHECK_HEADERS(sys/types.h types.h)
AC_CHECK_HEADERS(sys/unistd.h unistd.h)
AC_CHECK_HEADERS(sys/fcntl.h fcntl.h)
AC_CHECK_HEADERS(sys/limits.h limits.h)
AC_CHECK_HEADERS(sys/stat.h stat.h)
AC_CHECK_HEADERS(sys/file.h)
AC_CHECK_HEADERS(sys/ioctl.h)
AC_CHECK_HEADERS(sys/time.h time.h)
AC_CHECK_HEADERS(string.h strings.h)
AC_CHECK_HEADERS(syslog.h)
AC_CHECK_HEADERS(sys/socket.h socket.h)
AC_CHECK_HEADERS(MacSockDefs.h socket.ext.h)
AC_CHECK_HEADERS(appkit/appkit.h appkit.h)
AC_CHECK_HEADERS(netinet/in.h in.h)
AC_CHECK_HEADERS(dn.h)
AC_CHECK_HEADERS(sys/ipc.h)
AC_CHECK_HEADERS(net/errno.h sys/errno.h errno.h)
AC_CHECK_HEADERS(pwd.h)
AC_CHECK_HEADERS(grp.h)
AC_CHECK_HEADERS(arpa/inet.h inet.h)
AC_CHECK_HEADERS(netdb.h)
AC_CHECK_HEADERS(manifest.h)
AC_CHECK_HEADERS(bsdtypes.h)
AC_CHECK_HEADERS(stdefs.h)
AC_CHECK_HEADERS(bsdtime.h)
AC_CHECK_HEADERS(sys/select.h select.h)
AC_CHECK_HEADERS(dnetdb.h)
dnl AC_CHECK_HEADERS(ucx$inetdef.h) dnl Oooh, this'd be fun...
AC_CHECK_HEADERS(winsock.h)
AC_CHECK_HEADERS(libc.h)
AC_CHECK_HEADERS(stdlib.h)
AC_CHECK_HEADERS(malloc.h)
AC_CHECK_HEADERS(memory.h)
AC_CHECK_HEADERS(unixlib.h)
AC_CHECK_HEADERS(ctype.h)
AC_CHECK_HEADERS(cursesX.h curses.h)
AC_CHECK_HEADERS(sys/resource.h resource.h)
AC_HEADER_DIRENT
AC_CHECK_HEADERS(dir.h direct.h) dnl AC_HEADER_DIRENT only checks for some..
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_HEADER_STAT

dnl Checks for typedefs, structures, and compiler characteristics:

AC_TYPE_UID_T
AC_TYPE_GETGROUPS
AC_TYPE_MODE_T
AC_TYPE_FD_SET
AC_STRUCT_DIRENT_INO
AC_CHECK_TYPE(BOOLEAN, char)
AC_CHECK_TYPE(u_char, unsigned char)
AC_CHECK_TYPE(u_short, unsigned short)
AC_CHECK_TYPE(u_long, unsigned long)
AC_C_CONST
AC_C_VOLATILE
AC_C_NOSHARE
AC_C_EXTERN_TYPEDEF_FUNC
AC_C_PTR_UNDEF_STRUCT

dnl Other types that aren't checked for, but maybe should be:
dnl  AC_TYPE_SIGNAL AC_TYPE_OFF_T AC_TYPE_PID_T AC_STRUCT_ST_RDEV 
dnl  AC_STRUCT_TM AC_STRUCT_TIMEZONE

dnl Checks for library functions:

AC_FUNC_WAIT3
AC_CHECK_FUNCS(opendir getpid strchr memcpy getcwd getwd select strerror)
AC_FUNC_STRFTIME dnl This checks the intl library...
AC_CHECK_FUNCS(unlink, , AC_CHECK_FUNCS(remove, AC_DEFINE(unlink, remove)))
AC_CHECK_FUNCS(gmtime, ,
  AC_CHECK_FUNCS(localtime, AC_DEFINE(gmtime, localtime)))
AC_CHECK_FUNCS(NXRunAlertPanel NXGetDefaultValue strcasecomp strncasecomp)
AC_CHECK_FUNCS(socket_ioctl vms_errno_string cuserid getlogin getpass)
AC_CHECK_FUNCS(getdomainname)
dnl We need to know if the system supports symlinks -- 
AC_CHECK_FUNCS(symlink)

dnl Other functions not currently checked for, but maybe should be:
dnl  gethostname mkdir mktime rmdir socket strstr strtol system pipe putenv

AC_CHECK_EXTERNS(socket_errno sys_errlist sys_nerr)
AC_DECL_NEED_SYS_ERR
dnl AC_CHECK_EXTERNS(vaxc$errno) The dollar will probably screw things up.

dnl This is the standard UNIX value
AC_DEFINE(CACHE_FILE_PREFIX, "/usr/wsrc/")

AC_OUTPUT(Makefile, date > stamp-h)
