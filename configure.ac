dnl Process this file with Autoconf to create configure
dnl (c) COPYRIGHT MIT 1995.
dnl Please first read the full copyright statement in the file COPYRIGH.
dnl @(#) $Id$

PACKAGE=w3c-libwww
VERSION=5.4.2
#dnl for CVS to be able to make a release diff
PREVIOUS_VERSION=5.4.1

AC_REVISION([$Id$])
AC_PREREQ([2.69])
AC_SUBST(PACKAGE)
AC_SUBST(VERSION)
AC_SUBST(CODE_URL)

dnl I don't know how to reuse the above variables, so I have to repeat it
AC_INIT([w3c-libwww], [5.4.2], [https://github.com/w3c/libwww])
AC_CONFIG_SRCDIR([Library])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([config/m4])

AC_CANONICAL_TARGET([])
AC_ARG_PROGRAM

dnl Initialize Automake.
AC_DEFINE_UNQUOTED([W3C_PACKAGE], "$PACKAGE", [Define to be the package name.])
AC_DEFINE_UNQUOTED([W3C_VERSION], "$VERSION", [Define to be the version.])
AC_SUBST(PREVIOUS_VERSION)

AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(wwwconf.h)

dnl our own functions

AC_DEFUN([AC_ADDLIB],[LIBS="$LIBS $1"])

dnl taken from the DDD configure.in
AC_DEFUN([AC_STRUCT_WINSIZE],
	 [AC_MSG_CHECKING( [for struct winsize in sys/ioctl.h and termios.h] )
	  AC_CACHE_VAL( [libwww_cv_struct_winsize_header],
			[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
			                  #include <sys/types.h>
					  #include <sys/ioctl.h>
					 ]],	  
					 [[struct winsize x;]])],
					 libwww_cv_struct_winsize_header=ioctl_h,
					 [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
					                   #include <sys/types.h> 
							   #include <termios.h>
							   ]], 
							   [[struct winsize x;]])],
							   libwww_cv_struct_winsize_header=termios_h, 
							   libwww_cv_struct_winsize_header=other)
					])
			])
	if test $libwww_cv_struct_winsize_header = ioctl_h; then
	  AC_MSG_RESULT(sys/ioctl.h)
	  AC_DEFINE(HAVE_WINSIZE)
	else
	  AC_MSG_RESULT(not found)
	fi
])

dnl Check if time_t is long

AC_DEFUN([AC_TIME_T_IS_LONG],
     [AC_RUN_IFELSE([AC_LANG_SOURCE([[ 
#include <stdio.h>
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
# include <sys/time.h>
# else
# include <time.h>
# endif
#endif

int main()
{
  return !(sizeof (time_t) == sizeof (long));
}
          ]])],
          [AC_DEFINE(HAVE_LONG_TIME_T)])])

AC_DEFUN([AC_SIZE_T_IS_LONG],
     [AC_RUN_IFELSE([AC_LANG_SOURCE([[ 
#include <stdio.h>

int main()
{
  return !(sizeof (size_t) == sizeof (long) && sizeof (int) != sizeof (long));
}
          ]])],
          [AC_DEFINE(HAVE_LONG_SIZE_T)])])

dnl
dnl Regular configure...
dnl
AC_PROG_CC
if test "$GCC" = "yes"; then CFLAGS="$CFLAGS -Wall"; fi
AC_SEARCH_LIBS([strerror],[cposix])
dnl AC_PROG_CXX		Caused problems and is not needed...
dnl You should probably use all three of these for maximum portability

case $host_os in
  *cygwin* ) CYGWIN=yes MINGW32=no ;;
  *mingw32* ) MINGW32=yes CYGWIN=no ;;
          * ) MINGW32=no CYGWIN=no;;
esac

AC_EXEEXT
if test "$MINGW32" = "yes"; then
    # With MinGW, need to explicitly link against winsock2
    LIBS="$LIBS -lws2_32"
    # LIBS="$LIBS -no-unused" for DLL building, but doesn't work ATM,
    # so completely disable shared libs (speeds up compilation because
    # the .c files are not compiled twice):
    AC_DISABLE_SHARED
fi

AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_INSTALL

dnl Remove next line and you get an error -- autoconf
dnl should deal with this automatically.
AC_PATH_X
dnl AC_PATH_XTRA 	Causes problems...

dnl Default location of icons
icondir=$datadir/$PACKAGE
AC_SUBST(icondir)

dnl Location for purify cache
PURIFY_CACHE=/tmp/purify.cache
AC_SUBST(PURIFY_CACHE)

dnl Various compiler directives
AC_MSG_CHECKING(whether to define additional compiler specific flags)
case "$target" in
    alpha-dec*)
	if test "$CC" = "cc"; then
	  CFLAGS="$CFLAGS -std1"
	  AC_MSG_RESULT(Digital Unix's cc)
	fi
	;;
    hp*)
	if test "$CC" = "cc"; then
	  CFLAGS="$CFLAGS -Aa -D_HPUX_SOURCE"
	  AC_MSG_RESULT(HT-UX's cc)
	fi
	;;
    i386-next-nextstep*)
	  CFLAGS="$CFLAGS -D_POSIX_SOURCE"
	  AC_MSG_RESULT(NextStep and OpenStep require _POSIX_SOURCE defined. This may cause some compilation warnings about pre-compiled headers but that should be OK)
	;;
    *)
	AC_MSG_RESULT(no)
	;;
esac

dnl Checks for programs.
AC_CHECK_PROGS(have_telnet, telnet)
if test -n "$have_telnet" ; then
  AC_DEFINE_UNQUOTED(TELNET_PROGRAM, "$have_telnet", [Define this to be the telnet program name.])
fi
AC_CHECK_PROGS(have_rlogin, rlogin)
if test -n "$have_rlogin" ; then
  AC_DEFINE_UNQUOTED(RLOGIN_PROGRAM, "$have_rlogin", [Define this to be the rlogin program name.])
fi
AC_CHECK_PROGS(have_tn3270, tn3270 x3270)
if test -n "$have_tn3270" ; then
  AC_DEFINE_UNQUOTED(TN3270_PROGRAM, "$have_tn3270", 1, [Define this to be the tn3270 program name.])
fi
AC_CHECK_PROGS(have_perl, perl)
if test ! -n "$have_perl" ; then
   AC_MSG_ERROR("Perl needs to be installed in order to compile libwww")
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
dnl JK 26/July/2000: desactivated the use of the www tool, as it's faster to
dnl use perl
dnl AC_PATH_PROGS(WWW, www, [""])
AC_PROG_LN_S

dnl Checks for misc files
dnl Resolv.conf
AC_PATH_PROG(have_resolv_conf, resolv.conf, , /etc, /usr/etc)
if test -n "$have_resolv_conf" ; then
  AC_DEFINE_UNQUOTED(RESOLV_CONF, "$have_resolv_conf", [Define this to be the location of the resolver configuration file.])
fi

dnl This is the standard UNIX value
AC_DEFINE(CACHE_FILE_PREFIX, "/usr/wsrc/", [Define this to be the prefix for cache files.])

dnl Checks for libraries:
dnl AC_CHECK_LIB(ICE, IceConnectionNumber)
AC_CHECK_LIB(socket, socket)
AC_CHECK_LIB(inet, connect)
AC_CHECK_LIB(nsl, t_accept)
AC_CHECK_LIB(dl, dlopen)

dnl Checks for header files:
AC_CHECK_HEADERS(arpa/inet.h inet.h)
AC_CHECK_HEADERS(net/errno.h sys/errno.h errno.h)
AC_CHECK_HEADERS(netinet/in.h in.h)
AC_CHECK_HEADERS(netinet/tcp.h tcp.h)
AC_CHECK_HEADERS(sys/fcntl.h fcntl.h)
AC_CHECK_HEADERS(sys/file.h)
AC_CHECK_HEADERS(sys/ioctl.h)
AC_CHECK_HEADERS(sys/ipc.h)
AC_CHECK_HEADERS(sys/limits.h limits.h)
AC_CHECK_HEADERS(sys/machine.h)
AC_CHECK_HEADERS(sys/resource.h resource.h)
AC_CHECK_HEADERS(sys/select.h select.h)
AC_CHECK_HEADERS(sys/socket.h socket.h)
AC_CHECK_HEADERS(sys/stat.h stat.h)
AC_CHECK_HEADERS(sys/syslog syslog.h)
AC_CHECK_HEADERS(sys/systeminfo.h)
AC_CHECK_HEADERS(sys/time.h time.h)
AC_CHECK_HEADERS(sys/types.h types.h)
AC_CHECK_HEADERS(sys/unistd.h unistd.h)
AC_CHECK_HEADERS(wais/wais.h wais.h)
AC_CHECK_HEADERS(bsdtime.h)
AC_CHECK_HEADERS(bsdtypes.h)
AC_CHECK_HEADERS(ctype.h)
AC_CHECK_HEADERS(cursesX.h curses.h)
AC_CHECK_HEADERS(dn.h)
AC_CHECK_HEADERS(dnetdb.h)
AC_CHECK_HEADERS(grp.h)
AC_CHECK_HEADERS(libc.h)
AC_CHECK_HEADERS(malloc.h)
AC_CHECK_HEADERS(manifest.h)
AC_CHECK_HEADERS(memory.h)
AC_CHECK_HEADERS(netdb.h)
AC_CHECK_HEADERS(pwd.h)
AC_CHECK_HEADERS(rxposix.h regex.h)
AC_CHECK_HEADERS(stdefs.h)
AC_CHECK_HEADERS(stdio.h)
AC_CHECK_HEADERS(stddef.h)
AC_CHECK_HEADERS(stdlib.h)
AC_CHECK_HEADERS(string.h strings.h)
AC_CHECK_HEADERS(termios.h)
AC_CHECK_HEADERS(unixlib.h)
AC_HEADER_DIRENT
AC_CHECK_HEADERS(dir.h direct.h) dnl AC_HEADER_DIRENT only checks for some..
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_HEADER_STAT

dnl Checks for typedefs, structures, and compiler characteristics:
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_TYPE_GETGROUPS
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
dnl See http://www.manpagez.com/info/autoconf/autoconf-2.69/autoconf_227.php#AC_005fTYPE_005fSIGNAL
AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],[AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([#include <sys/types.h>
#include <signal.h>
],
		 [return *(signal (0, 0)) (0) == 1;])],
		   [ac_cv_type_signal=int],
		   [ac_cv_type_signal=void])])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[$ac_cv_type_signal],[Define as the return type of signal handlers
		    (`int' or `void').])


if test "$MINGW32" != "yes"; then
    AC_CHECK_TYPE(BOOLEAN, char)
    AC_CHECK_TYPE(u_char, unsigned char)
    AC_CHECK_TYPE(u_short, unsigned short)
    AC_CHECK_TYPE(u_long, unsigned long)
fi

AC_CHECK_SIZEOF(char,1)
AC_CHECK_SIZEOF(char *)
AC_CHECK_SIZEOF(int,4)
AC_CHECK_SIZEOF(long,4)
AC_CHECK_SIZEOF(time_t,long)
AC_CHECK_SIZEOF(size_t,long)
AC_TIME_T_IS_LONG
AC_SIZE_T_IS_LONG


AC_C_CONST
AC_C_BIGENDIAN
AC_C_CHAR_UNSIGNED
AC_TYPE_LONG_DOUBLE

AC_STRUCT_TM
AC_CHECK_MEMBER(struct tm.tm_gmtoff, HAVE_TM_GMTOFF)
AC_STRUCT_TIMEZONE
AC_STRUCT_WINSIZE

dnl Checks for library functions:
AC_FUNC_VPRINTF
AC_FUNC_STRFTIME
AC_CHECK_FUNCS(getcwd gethostname getdomainname getwd  \
		select socket strerror strtol opendir getpid strchr memcpy \
		getlogin getpass fcntl readdir sysinfo ioctl chdir tempnam \
		getsockopt setsockopt \
		gettimeofday mktime timegm tzset \
		fpathconf dirfd )
# AC_CHECK_FUNC(unlink, , AC_CHECK_FUNC(remove, AC_DEFINE(unlink, remove)))
## Path submitted by thurog@gmx.de for autoconf 2.53
AC_CHECK_FUNC(unlink)
  if test "$ac_cv_func_unlink" != yes ; then
     AC_CHECK_FUNC(remove)
     if test "$ac_cv_func_remove" == yes ; then
        AC_DEFINE(unlink, remove, [What to use for unlink])
     fi
fi

dnl Checks for configuration arguments
AC_MSG_CHECKING(whether to support direct WAIS access.)
AC_ARG_WITH(wais,
[  --with-wais[=PATH]      Compile with support for direct WAIS access.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    HTWAIS=""
    WWWWAIS=""
    LWWWWAIS=""
    LIBWWWWAIS=""
    ;;
  *)
    AC_MSG_RESULT(yes)
    if test "x$withval" = "xyes"; then
      withval="/usr/local/lib/wais/wais.a /usr/local/lib/wais/client.a"
    fi
    LIBS="$withval -lm $LIBS"
    AC_DEFINE(HT_DIRECT_WAIS, 1, [Define to enable direct WAIS access.])
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[ makeDocID(); ]])],[],[ AC_MSG_ERROR(Could not find one or more of the $withval libraries.  You must first install WAIS.) ])
    HTWAIS="HTWAIS.lo"
    WWWWAIS="libwwwwais.la"
    LWWWWAIS="-lwwwwais.la"
    LIBWWWWAIS='${top_builddir}/Library/src/libwwwwais.la'
    ;;
  esac ],
  AC_MSG_RESULT(no)
  HTWAIS=""
  WWWWAIS=""
  LWWWWAIS=""
  LIBWWWWAIS=""
)
AC_SUBST(HTWAIS)
AC_SUBST(WWWWAIS)
AC_SUBST(LWWWWAIS)
AC_SUBST(LIBWWWWAIS)

AC_MSG_CHECKING(whether to support zlib compress/decompress)
AC_ARG_WITH(zlib,
[  --with-zlib[=PATH]      Compile with zlib compress/decompress support.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    HTZLIB=""
    WWWZIP=""
    LWWWZIP=""
    LIBWWWZIP=""
    ;;
  *)
    AC_MSG_RESULT(yes)
    if test "x$withval" = "xyes"; then
      withval="-lz"
      LIBS="$LIBS $withval"
    else
      AC_ADDLIB($withval)
    fi
    AC_DEFINE(HT_ZLIB, 1, [Define to enable Zlib compression / decompression support.])
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[ zlibVersion(); ]])],[],[ AC_MSG_ERROR(Could not find the $withval library.  You must first install zlib.) ])
    HTZLIB="HTZip.lo"
    WWWZIP="libwwwzip.la"
    LWWWZIP="-lwwwzip"
    LIBWWWZIP='${top_builddir}/Library/src/libwwwzip.la'
    ;;
  esac ],
  AC_MSG_RESULT(no)
  HTZLIB=""
  WWWZIP=""
  LWWWZIP=""
  LIBWWWZIP=""
)
AC_SUBST(HTZLIB)
AC_SUBST(WWWZIP)
AC_SUBST(LWWWZIP)
AC_SUBST(LIBWWWZIP)

AC_MSG_CHECKING(whether to support POSIX regex)
AC_ARG_WITH(regex,
[  --with-regex[=PATH]     Compile with POSIC regex library support.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    ;;
  *)
    AC_MSG_RESULT(yes)
    AC_DEFINE(HT_POSIX_REGEX, 1, [Define to enable POSIX RegEx support.])
    if test "x$withval" = "xyes"; then
      AC_CHECK_LIB(rx, regexec, [ LIBS="-lrx $LIBS" ] )
    else
      AC_ADDLIB($withval)
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[ regexec(); ]])],[],[ AC_MSG_ERROR(Could not find the $withval library.  You must first install regex.)  ])
    fi
    ;;
  esac ],
  AC_MSG_RESULT(no)
)

# borrowed from SSH - thanks!
AC_MSG_CHECKING(whether to support SOCKS)
AC_ARG_WITH(socks,
[  --with-socks            Compile with SOCKS firewall traversal support.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    ;;
  yes)
    AC_MSG_RESULT(yes)
    AC_CHECK_LIB(socks5, SOCKSconnect, [
	    socks=5
	    LIBS="-lsocks5 $LIBS"], [
	AC_CHECK_LIB(socks, Rconnect, [
	    socks=4
	    LIBS="-lsocks $LIBS"], [
		AC_MSG_ERROR(Could not find socks library.  You must first install socks.) ] ) ] )
    ;;
  esac ],
  AC_MSG_RESULT(no)
)

if test "x$socks" = "x"; then
	AC_MSG_CHECKING(whether to support SOCKS5)
	AC_ARG_WITH(socks5,
	[  --with-socks5[=PATH]    Compile with SOCKS5 firewall traversal support.],
	[ case "$withval" in
	  no)
	    AC_MSG_RESULT(no)
	    ;;
	  *)
	    AC_MSG_RESULT(yes)
	    socks=5
	    if test "x$withval" = "xyes"; then
	      withval="-lsocks5"
	      LIBS="$withval $LIBS"
	    else
      	      AC_ADDLIB($withval)
	    fi
	    AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[ SOCKSconnect(); ]])],[],[ AC_MSG_ERROR(Could not find the $withval library.  You must first install socks5.) ])
	    ;;
	  esac ],
	  AC_MSG_RESULT(no)
	)
fi

if test "x$socks" = "x"; then
	AC_MSG_CHECKING(whether to support SOCKS4)
	AC_ARG_WITH(socks4,
	[  --with-socks4[=PATH]    Compile with SOCKS4 firewall traversal support.],
	[ case "$withval" in
	  no)
	    AC_MSG_RESULT(no)
	    ;;
	  *)
	    AC_MSG_RESULT(yes)
	    socks=4
	    if test "x$withval" = "xyes"; then
	      withval="-lsocks"
	      LIBS="$withval $LIBS"
            else
      	      AC_ADDLIB($withval)
	    fi
	    AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[ Rconnect(); ]])],[],[ AC_MSG_ERROR(Could not find the $withval library.  You must first install socks.) ])
	    ;;
	  esac ],
	  AC_MSG_RESULT(no)
	)
fi

if test "x$socks" = "x4"; then
    AC_DEFINE(SOCKS, 1, [Define to enable SOCKS firewall-breaching code.])
    AC_DEFINE(SOCKS4, 1, [Define to enable SOCKS4 firewall-breaching code.])
fi

if test "x$socks" = "x5"; then
    AC_DEFINE(SOCKS, 1, [Define to enable SOCKS firewall-breaching code.])
    AC_DEFINE(SOCKS5, 1, [Define to enable SOCKS5 firewall-breaching code.])
fi

AC_MSG_CHECKING(whether to support mysql access.)
AC_ARG_WITH(mysql,
[  --with-mysql[=PATH]      Compile with support for mysql access.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    HTSQL=""
    HTSQLLOG=""
    WWWSQL=""
    LWWWSQL=""
    LIBWWWSQL=""
    CVS2SQL=""
    ;;
  *)
    AC_MSG_RESULT(yes)
    if test "x$withval" = "xyes"; then
      if test -f '/usr/local/lib/mysql/libmysqlclient.a'; then
#         AC_ADDLIB("/usr/local/lib/mysql/libmysqlclient.a")
          mysql_dir="/usr/local/bin"
          # Try to find the mysql_config program
          AC_PATH_PROG(MYSQL_CONFIG, mysql_config, no, $mysql_dir $PATH )
          if test "$MYSQL_CONFIG" = no ; then
             AC_MSG_ERROR([cannot find mysql_config])
          fi
          MYSQL_CFLAGS=`$MYSQL_CONFIG --cflags`
          AC_SUBST(MYSQL_CFLAGS)
          MYSQL_LIBS=$($MYSQL_CONFIG --libs_r)
          AC_SUBST(MYSQL_LIBS)
      elif test -f '/usr/lib/mysql/libmysqlclient.a'; then
#        AC_ADDLIB("/usr/lib/mysql/libmysqlclient.a")
         mysql_dir="/usr/bin"
         # Try to find the mysql_config program
         AC_PATH_PROG(MYSQL_CONFIG, mysql_config, no, $mysql_dir $PATH )
         if test "$MYSQL_CONFIG" = no ; then
            AC_MSG_ERROR([cannot find mysql_config])
         fi
         MYSQL_CFLAGS=`$MYSQL_CONFIG --cflags`
         AC_SUBST(MYSQL_CFLAGS)
         MYSQL_LIBS=$($MYSQL_CONFIG --libs_r)
         AC_SUBST(MYSQL_LIBS)

      else
         AC_MSG_ERROR(Could not find the mysql client library.)
      fi
    else
#     AC_ADDLIB($withval)
      AC_MSG_CHECKING("$withval")
      mysql_dir="$withval/bin"
      # Try to find the mysql_config program
      AC_PATH_PROG(MYSQL_CONFIG, mysql_config, no, $mysql_dir $PATH )
      if test "$MYSQL_CONFIG" = no ; then
         AC_MSG_ERROR([cannot find mysql_config])
      fi
      MYSQL_CFLAGS=`$MYSQL_CONFIG --cflags`
      AC_SUBST(MYSQL_CFLAGS)
      MYSQL_LIBS=$($MYSQL_CONFIG --libs_r)
      AC_SUBST(MYSQL_LIBS)
    fi
    AC_DEFINE(HT_MYSQL, 1, [Define to enable mysql access.])
    HTSQL="HTSQL.lo"
    HTSQLLOG="HTSQLLog.lo"
    WWWSQL="libwwwsql.la"
    LWWWSQL="-lwwwsql"
    LIBWWWSQL='${top_builddir}/Library/src/libwwwsql.la'
    CVS2SQL="cvs2sql"
    ;;
  esac ],
  AC_MSG_RESULT(no)
  HTSQL=""
  HTSQLLOG=""
  WWWSQL=""
  LWWWSQL=""
  LIBWWWSQL=""
  CVS2SQL=""
)
AC_SUBST(HTSQL)
AC_SUBST(HTSQLLOG)
AC_SUBST(WWWSQL)
AC_SUBST(LWWWSQL)
AC_SUBST(LIBWWWSQL)
AC_SUBST(CVS2SQL)

AC_MSG_CHECKING(whether we include the Expat XML parser.)
AC_ARG_WITH(expat,
[  --with-expat[=PATH]      Compile with support for expat.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    HTXML=""
    HTLIBXML=""
    WWWXML=""
    LWWWXML=""
    LIBWWWXML=""
    SHOWXML=""
    ;;
  *)
    AC_MSG_RESULT(yes)
    AC_CHECK_HEADER(expat.h, [], [AC_MSG_ERROR(missing expat.h file)])
    AC_CHECK_LIB(expat, XML_ParserCreate, [], [AC_MSG_ERROR(missing libexpat)])
    AC_DEFINE(HT_EXPAT, 1, [Define to enable expat XML parser.])
    HTXML="HTXML.lo"
    HTLIBXML=""
    WWWXML="libwwwxml.la"
    LWWWXML="-lwwwxml -lexpat"
    LIBWWWXML='${top_builddir}/Library/src/libwwwxml.la'
    SHOWXML='showxml$(EXEEXT) ptri$(EXEEXT) stri$(EXEEXT) rdf_parse_file$(EXEEXT) rdf_parse_buffer$(EXEEXT)'
    ;;
  esac ],
  AC_MSG_RESULT(yes)
  AC_CHECK_HEADER(expat.h, [], [AC_MSG_ERROR(missing expat.h file)])
  AC_CHECK_LIB(expat, XML_ParserCreate, [], [AC_MSG_ERROR(missing libexpat)])
  AC_DEFINE(HT_EXPAT)
  HTXML="HTXML.lo"
  HTLIBXML=""
  WWWXML="libwwwxml.la"
  LWWWXML="-lwwwxml -lexpat"
  LIBWWWXML='${top_builddir}/Library/src/libwwwxml.la'
  SHOWXML='showxml$(EXEEXT) ptri$(EXEEXT) stri$(EXEEXT) rdf_parse_file$(EXEEXT) rdf_parse_buffer$(EXEEXT)'
)

AC_SUBST(HTXML)		# for old style big library
AC_SUBST(HTLIBXML)	# for old style big library
AC_SUBST(WWWXML)
AC_SUBST(LWWWXML)
AC_SUBST(LIBWWWXML)
AC_SUBST(SHOWXML)

AC_MSG_CHECKING(whether we include MD5 support for HTTP Digest Authentication.)
AC_ARG_WITH(md5,
[  --with-md5[=PATH]      Compile with support for md5.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    WWWMD5=""
    LWWWMD5=""
    LIBWWWMD5=""
    ;;
  *)
    AC_MSG_RESULT(yes)
    AC_DEFINE(HT_MD5, 1, [Define to enable MD5 for HTTP access authentication.])
    WWWMD5="libmd5.la"
    LWWWMD5="-lmd5"
    LIBWWWMD5='${top_builddir}/modules/md5/libmd5.la'
    ;;
  esac ],
  AC_MSG_RESULT(yes)
  AC_DEFINE(HT_MD5)
  WWWMD5="libmd5.la"
  LWWWMD5="-lmd5"
  LIBWWWMD5='${top_builddir}/modules/md5/libmd5.la'
)
AC_SUBST(WWWMD5)
AC_SUBST(LWWWMD5)
AC_SUBST(LIBWWWMD5)

AC_MSG_CHECKING(whether we include WebDAV support.)
AC_ARG_WITH(dav,
[  --with-dav      Compile with support for WebDAV.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    HTDAV=""
    WWWDAV=""
    LWWWDAV=""
    LIBWWWDAV=""
    DAVSAMPLE=""
    ;;
  *)
    AC_MSG_RESULT(yes)
    AC_DEFINE(HT_DAV, 1, [Define to enable WebDAV protocol.])
    HTDAV="HTDAV.lo"
    WWWDAV="libwwwdav.la"
    LWWWDAV="-lwwwdav"
    LIBWWWDAV='${top_builddir}/Library/src/libwwwdav.la'
    DAVSAMPLE="davsample"
    ;;
  esac ],
  AC_MSG_RESULT(no)
  HTDAV=""
  WWWDAV=""
  LWWWDAV=""
  LIBWWWDAV=""
  DAVSAMPLE=""
)
AC_SUBST(WWWDAV)
AC_SUBST(LWWWDAV)
AC_SUBST(LIBWWWDAV)
AC_SUBST(DAVSAMPLE)

AC_MSG_CHECKING(whether we include extension methods.)
AC_ARG_WITH(extension,
[  --with-extension      Compile with support for extension methods.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    MYEXT=""
    ;;
  *)
    AC_MSG_RESULT(yes)
    AC_DEFINE(HT_EXT, 1, [Define to enable extension methods.])
    MYEXT="myext myext2"
    ;;
  esac ],
  AC_MSG_RESULT(no)
  MYEXT=""
)
AC_SUBST(MYEXT)



dnl Checks for libwww and OpenSSL:
AC_MSG_CHECKING(whether we can find OpenSSL)
dnl find the ssl library dir (empirical)
if test -d '/usr/local/ssl/lib'; then
  ssllib="-L/usr/local/ssl/lib -lssl -lcrypto"
else
  ssllib="-L/usr/lib -lssl -lcrypto"
fi
dnl find the ssl include dir (empirical)
if test -d '/usr/local/ssl/include'; then
  sslinc="-I/usr/local/ssl/include"
elif test -d '/usr/local/openssl/include'; then
  sslinc="-I/usr/local/openssl/include"
elif test -d '/usr/local/include/openssl'; then
  sslinc="-I/usr/local/include/openssl"
elif test -d '/usr/include/ssl'; then
  sslinc="-I/usr/include/ssl"
elif test -d '/usr/include/openssl'; then
  sslinc="-I/usr/include/openssl"
else
  sslinc=""
fi
WWWSSL=""
SSLINC=""
LIBSSL=""
LWWWSSL=""
LIBWWWSSL=""
WWWSSLEX=""
AC_ARG_WITH(ssl,
[  --with-ssl[=PATH]       Link with OpenSSL.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    ;;
  *)
    if test "x$withval" = "xyes"; then
      withval=$ssllib
      SSLINC=$sslinc
    fi
    LIBS="$LIBS $withval"
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[ SSL_library_init(); ]])],[],[ AC_MSG_ERROR(Could not find the $withval libraries.  You must first install openSSL.) ])
    AC_MSG_RESULT(yes)
    WWWSSL="libwwwssl.la"
    LWWWSSL="-lwwwssl" 
    LIBWWWSSL='${top_builddir}/Library/src/SSL/libwwwssl.la'
    WWWSSLEX='wwwssl$(EXEEXT)'
    AC_DEFINE(HT_SSL, 1, [Define to enable ssl transport.])
    ;;
  esac ],
[ AC_MSG_RESULT(no) ])
AC_SUBST(WWWSSL)
AC_SUBST(SSLINC)
AC_SUBST(LWWWSSL)
AC_SUBST(LIBWWWSSL)
AC_SUBST(WWWSSLEX)

AC_MSG_CHECKING(whether to use internal signal handlers)
AC_ARG_ENABLE(signals, [  --enable-signals        use internal libwww signal handler.],
[ case "${enableval}" in
  no)
    AC_MSG_RESULT(no)
    ;;
  *)
    AC_MSG_RESULT(yes)
    AC_DEFINE(WWWLIB_SIG, 1, [Define to use internal signal handler])
    ;;
  esac ],
  AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(whether to disable Nagle's algorithm)
AC_ARG_ENABLE(nagle,
[  --disable-nagle         Turning off Nagle's algorithm.],
[ case "${enableval}" in
  no)
    AC_MSG_RESULT(yes)
    AC_DEFINE(HT_NO_NAGLE, 1, [Define to disable Nagle's algorithm])
    ;;
  *) 
    AC_MSG_RESULT(no)
    ;;
  esac ],
  AC_MSG_RESULT(yes)
  AC_DEFINE(HT_NO_NAGLE, 1, [Define to disable Nagle's algorithm])
)

AC_MSG_CHECKING(whether to accept new rule files without asking an end user.)
AC_ARG_ENABLE(autorules,
[  --enable-autorules      Enabling acceptance of rule files without user interaction.],
[ case "${enableval}" in
  yes)
    AC_MSG_RESULT(yes)
    AC_DEFINE(HT_AUTOMATIC_RULES, 1, [Define to enable support for enabling a rules file w/o user interaction.])
    ;;
  *) 
    AC_MSG_RESULT(no)
    ;;
  esac ],
  AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(whether to disable HTTP/1.1 pipelining)
AC_ARG_ENABLE(pipelining,
[  --disable-pipelining    Turning off HTTP/1.1 Pipelining.],
[ case "${enableval}" in
  no)
    AC_MSG_RESULT(yes)
    AC_DEFINE(HT_NO_PIPELINING, 1, [Define to disable HTTP/1.1 pipelining])
    ;;
  *) 
    AC_MSG_RESULT(no)
    ;;
  esac ],
  AC_MSG_RESULT(no)
)


AC_MSG_CHECKING(whether to use MUX as transport for HTTP.)
AC_ARG_ENABLE(mux,
[  --enable-mux            Enabling MUX as transport for HTTP.],
[ case "${enableval}" in
  yes)
    AC_MSG_RESULT(yes)
    AC_DEFINE(HT_MUX, 1, [Define to enable MUX as HTTP transport])
    ;;
  *) 
    AC_MSG_RESULT(no)
    ;;
  esac ],
  AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(whether to define _REENTRANT)
AC_ARG_ENABLE(reentrant, [  --enable-reentrant      define _REENTRANT to enable reentrant system calls],
[ case "${enableval}" in
  no)
    AC_MSG_RESULT(no)
    ac_cv_reentrant=no
    ;;
  *) 
    AC_MSG_RESULT(yes)
    AC_DEFINE(HT_REENTRANT, 1, [Define to build using reentrant system calls.]) [CFLAGS="$CFLAGS -D_REENTRANT"]
    ac_cv_reentrant=yes
    ;;
  esac ],
  AC_MSG_RESULT(no)
)

if test "$ac_cv_reentrant" = "yes"; then

    AC_MSG_CHECKING(for ctime_r)
    if test -z "$ac_cv_ctime_args"; then
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <time.h>]], [[
            time_t clock;
            char buf[26];
            ctime_r(&clock, buf);
        ]])],[ac_cv_ctime_args=2],[])
    fi
    if test -z "$ac_cv_ctime_args" ; then
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <time.h>]], [[
            time_t clock;
            char buf[26];
            ctime_r(&clock, buf, 26);
        ]])],[ac_cv_ctime_args=3],[])
    fi
    if test -z "$ac_cv_ctime_args"; then
        AC_MSG_RESULT(no)
        have_missing_r_funcs="$have_missing_r_funcs ctime_r"
    else
        if test "$ac_cv_ctime_args" = 2; then
            AC_DEFINE(HAVE_CTIME_R_2, 1, [Define to use the two-argument variant of ctime_r])
        elif test "$ac_cv_ctime_args" = 3; then
            AC_DEFINE(HAVE_CTIME_R_3, 1, [Define to use the three-argument variant of ctime_r])
        fi
        AC_MSG_RESULT([yes, and it takes $ac_cv_ctime_args arguments])
    fi

    AC_MSG_CHECKING(for readdir_r)
    if test -z "$ac_cv_readdir_args"; then
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>

#if defined(HAVE_DIRENT_H)
#   include <dirent.h>
#   define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#   define dirent direct
#   define NAMLEN(dirent) (dirent)->d_namlen
#   if defined(HAVE_SYS_NDIR_H)
#       include <sys/ndir.h>
#   endif
#   if defined(HAVE_SYS_DIR_H)
#       include <sys/dir.h>
#   endif
#   if defined(HAVE_NDIR_H)
#       include <ndir.h>
#   endif
#endif  ]], [[
        struct dirent dir, *dirp;
        DIR *mydir;
        dirp = readdir_r(mydir, &dir);
	]])],[ac_cv_readdir_args=2],[])
    fi
    if test -z "$ac_cv_readdir_args"; then
	
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#if defined(HAVE_DIRENT_H)
#   include <dirent.h>
#   define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#   define dirent direct
#   define NAMLEN(dirent) (dirent)->d_namlen
#   if defined(HAVE_SYS_NDIR_H)
#       include <sys/ndir.h>
#   endif
#   if defined(HAVE_SYS_DIR_H)
#       include <sys/dir.h>
#   endif
#   if defined(HAVE_NDIR_H)
#       include <ndir.h>
#   endif
#endif  ]], [[
        struct dirent dir, *dirp;
        DIR *mydir;
        int rc;
        rc = readdir_r(mydir, &dir, &dirp);
        ]])],[ac_cv_readdir_args=3],[])
    fi

    if test -z "$ac_cv_readdir_args"; then
        AC_MSG_RESULT(no)
        have_missing_r_funcs="$have_missing_r_funcs readdir_r"
    else
        if test "$ac_cv_readdir_args" = 2; then
            AC_DEFINE(HAVE_READDIR_R_2, 1, [Define to use the two-argument variant of readdir_r])
        elif test "$ac_cv_readdir_args" = 3; then
            AC_DEFINE(HAVE_READDIR_R_3, 1, [Define to use the three-argument variant of gethostbyname_r])
        fi
        AC_MSG_RESULT([yes, and it takes $ac_cv_readdir_args arguments])
    fi

    AC_MSG_CHECKING(for gethostbyname_r)
    if test -z "$ac_cv_gethostbyname_args"; then
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <netdb.h>]], [[
struct hostent *hp;
struct hostent h;
char *name;
char buffer[10];
int  h_errno;
hp = gethostbyname_r(name, &h, buffer, 10, &h_errno);]])],[ac_cv_gethostbyname_args=5],[])
    fi
    if test -z "$ac_cv_gethostbyname_args"; then
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <netdb.h>]], [[
struct hostent h;
struct hostent_data hdata;
char *name;
int  rc;
rc = gethostbyname_r(name, &h, &hdata);]])],[ac_cv_gethostbyname_args=3],[])
    fi
    if test -z "$ac_cv_gethostbyname_args"; then
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <netdb.h>]], [[
struct hostent h;
struct hostent *hp;
char *name;
char buf[10];
int rc;
int h_errno;

rc = gethostbyname_r(name, &h, buf, 10, &hp, &h_errno);
	    ]])],[ac_cv_gethostbyname_args=6],[])
    fi
    if test -z "$ac_cv_gethostbyname_args"; then
        AC_MSG_RESULT(no)
        have_missing_r_funcs="$have_missing_r_funcs gethostbyname_r"
    else
        if test "$ac_cv_gethostbyname_args" = 3; then
            AC_DEFINE(HAVE_GETHOSTBYNAME_R_3, 1, [Define to use the three-argument variant of gethostbyname_r])
        elif test "$ac_cv_gethostbyname_args" = 5; then
            AC_DEFINE(HAVE_GETHOSTBYNAME_R_5, 1, [Define to use the five-argument variant of gethostbyname_r])
        elif test "$ac_cv_gethostbyname_args" = 6; then
            AC_DEFINE(HAVE_GETHOSTBYNAME_R_6, 1, [Define to use the six-argument variant of gethostbyname_r])
        fi
        AC_MSG_RESULT([yes, and it takes $ac_cv_gethostbyname_args arguments])
    fi

    AC_MSG_CHECKING(for gethostbyaddr_r)
    if test -z "$ac_cv_gethostbyaddr_args"; then
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <netdb.h>]], [[
                char * address;
                int length;
                int type;
                struct hostent h;
                struct hostent_data hdata;
                int rc;
                rc = gethostbyaddr_r(address, length, type, &h, &hdata);
	    ]])],[ac_cv_gethostbyaddr_args=5],[])
	
    fi
    if test -z "$ac_cv_gethostbyaddr_args"; then
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <netdb.h>]], [[
                char * address;
                int length;
                int type;
                struct hostent h;
		char buffer[10];
		int buflen;
		int h_errnop;
		struct hostent * hp;

		hp = gethostbyaddr_r(address, length, type, &h,
				     buffer, buflen, &h_errnop);
	    ]])],[ac_cv_gethostbyaddr_args=7],[])
    fi

    if test -z "$ac_cv_gethostbyaddr_args"; then
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <netdb.h>]], [[
                char * address;
                int length;
                int type;
                struct hostent h;
		char buffer[10];
		int buflen;
		int h_errnop;
		struct hostent * hp;
		int rc;

		rc = gethostbyaddr_r(address, length, type, &h,
				     buffer, buflen, &hp, &h_errnop);
	    ]])],[ac_cv_gethostbyaddr_args=8],[])
    fi
    if test -z "$ac_cv_gethostbyaddr_args"; then
        AC_MSG_RESULT(no)
        have_missing_r_funcs="$have_missing_r_funcs gethostbyaddr_r"
    else
        if test "$ac_cv_gethostbyaddr_args" = 5; then
            AC_DEFINE(HAVE_GETHOSTBYADDR_R_5, 1, [Define to use the five-argument variant of gethostbyaddr_r])
        elif test "$ac_cv_gethostbyaddr_args" = 7; then
            AC_DEFINE(HAVE_GETHOSTBYADDR_R_7, 1, [Define to use the seven-argument variant of gethostbyaddr_r])
        elif test "$ac_cv_gethostbyaddr_args" = 8; then
            AC_DEFINE(HAVE_GETHOSTBYADDR_R_8, 1, [Define to use the eight-argument variant of gethostbyaddr_r])
        fi
        AC_MSG_RESULT([yes, and it takes $ac_cv_gethostbyaddr_args arguments])
    fi

    AC_MSG_CHECKING(for getlogin_r)
    if test -z "$ac_cv_getlogin_r_style"; then
	AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <unistd.h>

int main()
{
    char name[1024];
    int len=1024;
    int rc;
    char *login;

    name[0]='\0';

    rc = getlogin_r(name, len);
    if(rc == 0 && name[0] != '\0')
    {
	return 0; /* returns zero on success */
    }
    else if(rc != 0 && name[0] == '\0')
    {
	return 0; /* returns errno on failure */
    }
    else if(rc == 0 && name[0] == '\0')
    {
	return 0; /* can't tell */
    }
    else if(rc != 0 && name[0] != '\0')
    {
	return 1; /* returns pointer on success */
    }
}
	    ]])],[
		ac_cv_getlogin_r_style=1
	    ],[
		ac_cv_getlogin_r_style=2
	    ],[
		ac_cv_getlogin_r_style=
	    ])
    fi
    if test -z "$ac_cv_getlogin_r_style"; then
	AC_MSG_RESULT(no)
        have_missing_r_funcs="$have_missing_r_funcs getlogin_r"
    else
	if test $ac_cv_getlogin_r_style = 1; then
	    AC_DEFINE(GETLOGIN_R_RETURNS_INT, 1, [Define if getlogin_r returns an integer])
	    AC_MSG_RESULT([yes, and it returns an int])
	elif test $ac_cv_getlogin_r_style = 2; then
	    AC_DEFINE(GETLOGIN_R_RETURNS_POINTER, 1, [Define if getlogin_r returns a pointer])
	    AC_MSG_RESULT([yes, and it returns a pointer])
	else
	    AC_MSG_RESULT(no: result is $ac_cv_getlogin_r_style)
            have_missing_r_funcs="$have_missing_r_funcs getlogin_r"
	fi
    fi
    if test -n "$have_missing_r_funcs"; then
        AC_MSG_WARN(missing reentrant functions: $have_missing_r_funcs)
    fi
fi

AC_MSG_CHECKING(whether to define _POSIX_SOURCE)
AC_ARG_ENABLE(posix, [  --enable-posix          define _POSIX_SOURCE],
[ case "${enableval}" in
  no)
    AC_MSG_RESULT(no)
    ;;
  *) 
    AC_MSG_RESULT(yes)
    AC_DEFINE(_POSIX_SOURCE,1,[Enable POSIX]) [CFLAGS="$CFLAGS -D_POSIX_SOURCE"]
    ;;
  esac ],
  AC_MSG_RESULT(no)
)

AC_ARG_ENABLE(cyrillic, [  --enable-cyrillic       include support for Cyrillic in line mode browser],
  AC_DEFINE(CYRILLIC, 1, [Define to build Cyrillic version.])
  A_STDIO="a_stdio.o", A_STDIO="")
AC_SUBST(A_STDIO)

dnl The following corresponds to the deprecated @BOTTOM@ section of acconfig.h

AH_TEMPLATE([HAVE_SOCKET_ERRNO],
	[Define this if you have the external variable 'socket_errno'.])
AH_TEMPLATE([HAVE_SYS_ERRLIST],
	[Define this if you have the external variable 'sys_errlist'.])
AH_TEMPLATE([HAVE_SYS_NERR],
	[Define this if you have the external variable 'sys_nerr'.])
AH_TEMPLATE([HAVE_TIMEZONE],
	[Define this if you have the external variable 'timezone'])
AH_TEMPLATE([HAVE_ALTZONE],
	[Define this if you have the external variable 'altzone'])
AH_TEMPLATE([HAVE_DAYLIGHT],
	[Define this if you have the external variable 'daylight'])
AH_TEMPLATE([Define this if you have the uxc$inetdef.h header file.],
	[HAVE_UXC_INETDEF_H])
AH_TEMPLATE([HAVE_DIRENT_INO],
	[Define this if directory entries have inodes.])
AH_TEMPLATE([HAVE_TM_GMTOFF],
	[Define this if tm structure includes a tm_gmtoff entry.])
AH_TEMPLATE([HAVE_WINSIZE],
	[Define this if your ioctl understands the winsize structure.])
AH_TEMPLATE([NEGATIVE_TIMEZONE],
	[Define this if the extern timezone is negative.])
AH_TEMPLATE([NEED_SYS_ERRLIST_DECLARED],
	[Define this if sys_errlist must be declared (if it exists).])
AH_TEMPLATE([NEED_SYS_NERR_DECLARED],
	[Define this if sys_nerr must be declared (if it exists).])
AH_TEMPLATE([HAVE_LONG_TIME_T],
	[Define if time_t is of type long])
AH_TEMPLATE([HAVE_LONG_SIZE_T],
	[Define if size_t is of type long])

dnl Create output files
AC_CONFIG_FILES([Makefile libwww-config w3c-libwww.spec
config/Makefile
modules/Makefile
modules/md5/Makefile
Library/Makefile Library/User/Makefile Library/User/Patch/Makefile
Library/User/Architecture/Makefile Library/User/Using/Makefile Library/User/Guide/Makefile
Library/User/Style/Makefile Library/User/Platform/Makefile
Library/src/Makefile Library/src/windows/Makefile Library/src/vms/Makefile 
Library/src/SSL/Makefile Library/src/SSL/windows/Makefile
Library/Examples/Makefile
Library/cvs2sql/Makefile
Library/External/Makefile
PICS-client/Makefile PICS-client/User/Makefile PICS-client/src/Makefile PICS-client/src/windows/Makefile
LineMode/Makefile LineMode/User/Makefile
LineMode/src/Makefile LineMode/src/windows/Makefile LineMode/src/vms/Makefile
Robot/Makefile Robot/User/Makefile Robot/src/Makefile Robot/tcl/Makefile Robot/src/windows/Makefile
ComLine/Makefile ComLine/User/Makefile ComLine/src/Makefile ComLine/src/windows/Makefile
WinCom/Makefile WinCom/hlp/Makefile WinCom/res/Makefile
Icons/Makefile Icons/WWW/Makefile Icons/32x32/Makefile Icons/internal/Makefile
])
AC_CONFIG_COMMANDS([default],[[date>stamp-h]],[[]])
AC_OUTPUT
