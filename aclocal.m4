dnl aclocal.m4 generated automatically by aclocal 1.3

dnl Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
dnl This Makefile.in is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

AC_DEFUN(AC_SYS_AFS_SYSNAME, [
AC_CHECK_PROG(ac_afsws_fs_found, fs, yes)
AC_MSG_CHECKING(AFS system name)
AC_CACHE_VAL(ac_cv_sys_afs_sysname, [ 
if test -n $ac_afsws_fs_found ; then
  ac_cv_sys_afs_sysname=`fs sysname|tr "'" " "|sed -e 's/Current sysname is //' -e 's/ //'`
  if test -z $ac_cv_sys_afs_sysname ; then
    ac_cv_sys_afs_sysname=bad_sysname
  fi
else
  ac_cv_sys_afs_sysname=unknown_sysname
fi
])
ac_sys_afs_sysname=$ac_cv_sys_afs_sysname
AC_MSG_RESULT($ac_sys_afs_sysname)
AC_SUBST(ac_sys_afs_sysname)dnl
])dnl

dnl --------------------------------------------------------- ##
dnl Use AC_PROG_INSTALL, supplementing it with INSTALL_SCRIPT ##
dnl substitution.                                             ##
dnl --------------------------------------------------------- ##

dnl AC_DEFUN(fp_PROG_INSTALL,
dnl [AC_PROG_INSTALL
dnl test -z "$INSTALL_SCRIPT" && INSTALL_SCRIPT='${INSTALL} -m 755'
dnl AC_SUBST(INSTALL_SCRIPT)dnl
dnl ])

dnl AC_PATH_WAIS(ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_PATH_WAIS, [
AC_MSG_CHECKING(for WAIS)
AC_CACHE_VAL(ac_cv_path_wais,
[
WAISPATH="$WAISPATH ${srcdir}/*WAIS* ${srcdir}/*wais* ${srcdir}/../*WAIS* ${srcdir}/../../*WAIS* ${srcdir}/../../../*WAIS* ${srcdir}/../*wais* ${srcdir}/../../*wais* ${srcdir}/../../../*wais*"
for ac_dir in $WAISPATH ; do
  if test -f ${ac_dir}/include/wais.h; then
    ac_cv_path_wais=$ac_dir
    break;
  fi
done
])
ac_path_wais=$ac_cv_path_wais
test -z "$ac_path_wais" && ac_path_wais=missing
AC_MSG_RESULT($ac_path_wais)
if test -n "$ac_cv_path_wais"; then
  LIBS="$LIBS $ac_path_wais/bin/client.a $ac_path_wais/bin/wais.a"
  ifelse([$1], , :, [$1])
ifelse([$2], , , [else
  $2
])dnl
fi
])

dnl AC_PATH_TCL(ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_PATH_TCL, [
AC_MSG_CHECKING(for TCL)
AC_CACHE_VAL(ac_cv_path_tcl,
[
TCLLIB="$TCLLIB /usr/local/lib/libtcl*.a"
for ac_dir in $TCLLIB ; do
  if test -f $ac_dir; then
    ac_cv_path_tcl=$ac_dir
    break;
  fi
done
])
LIBTCL=""
ac_path_tcl=$ac_cv_path_tcl
test -z "$ac_path_tcl" && ac_path_tcl=missing
AC_MSG_RESULT($ac_path_tcl)
if test -n "$ac_cv_path_tcl"; then
  LIBTCL="$ac_path_tcl"
  ifelse([$1], , :, [$1])
ifelse([$2], , , [else
  $2
])dnl
fi
])

dnl AC_PATH_SOCKS(ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_PATH_SOCKS, [
AC_MSG_CHECKING(for SOCKS)
AC_CACHE_VAL(ac_cv_path_socks,
[
  AC_CHECK_LIB(socks, main, ac_cv_path_socks=installed, 
  [
    SOCKSPATH="$SOCKSPATH ${srcdir}/../*SOCKS* ${srcdir}/../../*SOCKS* ${srcdir}/../../../*SOCKS* ${srcdir}/../*socks* ${srcdir}/../../*socks* ${srcdir}/../../../*socks*"
    for ac_dir in $SOCKSPATH; do
      if test -f $ac_dir/lib/libsocks.a ; then
        ac_cv_path_socks=$ac_dir/lib
        break;
      fi
    done
  ])
])
ac_path_socks=$ac_cv_path_socks
test -z "$ac_path_socks" && ac_path_socks=missing
AC_MSG_RESULT($ac_path_socks)
if test -n "$ac_cv_path_socks" && test "$ac_path_socks" != installed ; then
  LIBS="$LIBS $ac_path_socks/libsocks.a"
fi;
if test -n $ac_cv_path_socks; then
  ifelse([$1], , :, [$1])
ifelse([$2], , , [else
  $2
])dnl
fi
])

dnl AC_PROG_CC_STATIC
AC_DEFUN(AC_PROG_CC_STATIC,
[AC_REQUIRE([AC_PROG_CC])dnl
AC_MSG_CHECKING(if the C compiler accepts -static)
AC_CACHE_VAL(ac_cv_prog_cc_static,
[ ac_save_cflags="$CFLAGS"
  CFLAGS="-static"
  AC_TRY_LINK(,[int main(){return(0);}],
    ac_cv_prog_cc_static=yes,ac_cv_prog_cc_static=no)
  CFLAGS="$ac_save_cflags"
])dnl
AC_MSG_RESULT($ac_cv_prog_cc_static)
if test $ac_cv_prog_cc_static = yes; then
  CFLAGS="$CFLAGS -static"
fi
])

dnl AC_PROG_CC_DEBUG
AC_DEFUN(AC_PROG_CC_DEBUG,
[AC_REQUIRE([AC_PROG_CC])dnl
AC_MSG_CHECKING(if the C compiler accepts -g)
AC_CACHE_VAL(ac_cv_prog_cc_debug,
[ ac_save_cflags="$CFLAGS"
  CFLAGS="-g"
  AC_TRY_LINK(,[int main(){return(0);}],
    ac_cv_prog_cc_debug=yes,ac_cv_prog_cc_debug=no)
  CFLAGS="$ac_save_cflags"
])dnl
AC_MSG_RESULT($ac_cv_prog_cc_debug)
if test $ac_cv_prog_cc_debug = yes; then
  CFLAGS="$CFLAGS -g"
fi
])

dnl AC_TYPE_FD_SET
AC_DEFUN(AC_TYPE_FD_SET,
[AC_MSG_CHECKING(for fd_set)
 AC_CACHE_VAL(ac_cv_type_fd_set,
 [ AC_HEADER_INCLUDE(tmp_inc_select, sys/select.h select.h)
AC_EGREP_CPP(fd_set, [$tmp_inc_select
#include <sys/types.h>
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
], ac_cv_type_fd_set=yes, ac_cv_type_fd_set=no)])dnl
 AC_MSG_RESULT($ac_cv_type_fd_set)
 if test $ac_cv_type_fd_set = no; then
  AC_DEFINE(fd_set, unsigned int)
fi
])

dnl AC_C_EXTERN_TYPEDEF_FUNC
AC_DEFUN(AC_C_EXTERN_TYPEDEF_FUNC,
[AC_MSG_CHECKING(if an extern declaration of a typedefed function may precede its definition)
 AC_CACHE_VAL(ac_cv_c_extern_typedef_func,
 [AC_TRY_COMPILE([
typedef int foo_t();
extern foo_t foo;
int foo() { return 0; }
], , ac_cv_c_extern_typedef_func=yes, ac_cv_c_extern_typedef_func=no)])dnl
AC_MSG_RESULT($ac_cv_c_extern_typedef_func)
if test $ac_cv_c_extern_typedef_func = no ; then
  AC_DEFINE(NO_EXTERN_TYPEDEF_FUNC)
fi
])

dnl AC_C_PTR_UNDEF_STRUCT
AC_DEFUN(AC_C_PTR_UNDEF_STRUCT,
[AC_MSG_CHECKING(if pointers to undefined structures are allowed)
 AC_CACHE_VAL(ac_cv_c_ptr_undef_struct,
 [AC_TRY_COMPILE([typedef struct _foo foo; extern foo *bar();], bar();,
  ac_cv_c_ptr_undef_struct=yes, ac_cv_c_ptr_undef_struct=no)])dnl
AC_MSG_RESULT($ac_cv_c_ptr_undef_struct)
if test $ac_cv_c_ptr_undef_struct = no; then
  AC_DEFINE(NO_PTR_UNDEF_STRUCT)
fi
])

dnl AC_PROG_TELNET_L
dnl Ok, it's a hack of a method.  Got anything better?
AC_DEFUN(AC_PROG_TELNET_L,
[ AC_MSG_CHECKING(if telnet accepts the -l flag)
  AC_CACHE_VAL(ac_cv_prog_telnet_l,
  [ if test -r `which telnet` \
       && strings `which telnet` | egrep 'usage.*-l' > /dev/null ; then
      ac_cv_prog_telnet_l=yes
    else
      ac_cv_prog_telnet_l=no
    fi])dnl
  AC_MSG_RESULT($ac_cv_prog_telnet_l)
  if test $ac_cv_prog_telnet_l = yes; then
    AC_DEFINE(TELNET_MINUS_L)
  fi
])

dnl AC_FILE_RESOLV_CONF 
dnl Yeah, it should be very general.  So sue me.
AC_DEFUN(AC_FILE_RESOLV_CONF,
[ AC_MSG_CHECKING(for resolver configuration file)
  AC_CACHE_VAL(ac_cv_file_resolv_conf,
  [ if test -f /etc/resolv.conf; then
      ac_cv_file_resolv_conf=/etc/resolv.conf
    fi])dnl
  AC_MSG_RESULT($ac_cv_file_resolv_conf)
  if test -n $ac_cv_file_resolv_conf; then
    AC_DEFINE_UNQUOTED(RESOLV_CONF, "$ac_cv_file_resolv_conf")
  fi
])

dnl AC_CHECK_EXTERN(EXTERN, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_EXTERN,
[ AC_MSG_CHECKING([for $1])
  AC_CACHE_VAL(ac_cv_extern_$1,
  [ AC_TRY_LINK(ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
extern "C"
#endif
])dnl
[extern char * $1;
], [
* $1 = '\0';
], eval "ac_cv_extern_$1=yes", eval "ac_cv_extern_$1=no")])dnl
if eval "test \"`echo '$ac_cv_extern_'$1`\" = yes"; then
  AC_MSG_RESULT(yes)
  ifelse([$2], , :, [$2])
else
  AC_MSG_RESULT(no)
ifelse([$3], , , [$3
])dnl
fi
])

dnl AC_CHECK_EXTERNS(EXTERN... [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
AC_DEFUN(AC_CHECK_EXTERNS,
[for ac_exter in $1
do
AC_CHECK_EXTERN($ac_exter,
[changequote(, )dnl
  ac_tr_exter=HAVE_`echo $ac_exter | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_exter) $2], $3)dnl
done
])

dnl AC_C_VOLATILE
AC_DEFUN(AC_C_VOLATILE,
[ AC_MSG_CHECKING([for working volatile])
  AC_CACHE_VAL(ac_cv_c_volatile,
  [ AC_TRY_COMPILE(,[
extern volatile int errno;
], ac_cv_c_volatile=yes, ac_cv_c_volatile=no)])dnl
  AC_MSG_RESULT($ac_cv_c_volatile)
if test $ac_cv_c_volatile = no; then
  AC_DEFINE(volatile, )
fi
])

dnl AC_C_NOSHARE
AC_DEFUN(AC_C_NOSHARE,
[ AC_MSG_CHECKING([for working noshare])
  AC_CACHE_VAL(ac_cv_c_noshare,
  [ AC_TRY_COMPILE(,[
extern noshare int errno;
], ac_cv_c_noshare=yes, ac_cv_c_noshare=no)])dnl
  AC_MSG_RESULT($ac_cv_c_noshare)
if test $ac_cv_c_noshare = no; then
  AC_DEFINE(noshare, )
fi
])

dnl AC_HEADER_INCLUDE(VARIABLE, INCLUDES)
AC_DEFUN(AC_HEADER_INCLUDE,
[ $1=""
  for ac_hdr in $2; do
    AC_TRY_CPP([#include <$ac_hdr>], [ $1="[#]include <$ac_hdr>"
break])
  done
])

dnl AC_WEIRD_ACCEPT
dnl I couldn't even think of an appropriate name for this macro.
dnl On some platforms (e.g., Sequent/Dynix), the accept(2) call
dnl must be called on nonblocking sockets for the remote 
dnl connect(2) call to complete.  Their man page says:
dnl
dnl NOTES
dnl      In DYNIX/ptx, the accept call actually completes the network
dnl      connect.  BSD systems typically complete the connection
dnl      independently of the accept call.  This can have the effect
dnl      of blocking the remote connect call until the accept call is
dnl      executed.
dnl
dnl The below program tests for this behaivour.  If it exits zero,
dnl then it would appear we're on one of these screwed machines; 
dnl a negative code means we're on a normal machine and a positive
dnl means the test failed but we'll assume normal behaviour.
dnl
dnl I would like to thank Bill Rushka <wcr@aps.org> for his kind help
dnl in working this out.  Any problems with this macro or program are
dnl strictly my (<roeber@cern.ch>) fault.
dnl

AC_DEFUN(AC_WEIRD_ACCEPT, 
[ AC_MSG_CHECKING(for unusual accept call)
  AC_CACHE_VAL(ac_cv_weird_accept,
  [ AC_HEADER_INCLUDE(tmp_inc_types,  sys/types.h types.h)
    AC_HEADER_INCLUDE(tmp_inc_socket, sys/socket.h socket.h)
    AC_HEADER_INCLUDE(tmp_inc_in,     netinet/in.h in.h)
    AC_HEADER_INCLUDE(tmp_inc_inet,   arpa/inet.h inet.h)
    AC_HEADER_INCLUDE(tmp_inc_fcntl,  sys/fcntl.h fcntl.h)
    AC_HEADER_INCLUDE(tmp_inc_file,   sys/file.h file.h)
    AC_HEADER_INCLUDE(tmp_inc_time,   sys/time.h time.h)
    AC_HEADER_INCLUDE(tmp_inc_wait,   sys/wait.h wait.h)

    AC_TRY_RUN([
#include <stdio.h>
$tmp_inc_types
$tmp_inc_socket
$tmp_inc_in
$tmp_inc_inet
$tmp_inc_fcntl
$tmp_inc_file
$tmp_inc_time
$tmp_inc_wait

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7F000001
#endif

int main()
{
    int s, t, f, a;
    struct sockaddr_in sin, tin;
    int sin_len, tin_len;
    struct timeval v;
    fd_set d;

    sin_len = tin_len = sizeof(struct sockaddr_in);

    if( (s = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) exit(1);

    if( (f = fcntl(s, F_GETFL, 0)) == -1 ) exit(2);
    f |= FNDELAY;
    if( fcntl(s, F_SETFL, f) == -1 ) exit(3);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(0);
    sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if( bind(s, (struct sockaddr *)&sin, sin_len) == -1 ) exit(4);
    if( getsockname(s, (struct sockaddr *)&sin, &sin_len) == -1 ) exit(5);
    if( listen(s, 2) == -1 ) exit(6);

    switch( fork() )
    {
        case -1: exit(7);
        case 0: break;
        default:
            (void)wait(&a);
            exit(((int)a) >> 8);
    }

    (void)close(s);

    if( (t = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) exit(8);

    if( (f = fcntl(t, F_GETFL, 0)) == -1 ) exit(9);
    f |= FNDELAY;
    if( fcntl(t, F_SETFL, f) == -1 ) exit(10);

    tin.sin_family = AF_INET;
    tin.sin_port = sin.sin_port;
    tin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if( connect(t, (struct sockaddr *)&tin, tin_len) != -1 ) exit(-1);

    v.tv_sec = 2;
    v.tv_usec = 0;
    FD_SET(t, &d);

    switch( select(sizeof(int), 0, &d, 0, &v) )
    {
        case -1: exit(11);
        case  0: exit(0);
        case  1: exit(-2);
    }

    return -1;
}
], ac_cv_weird_accept=yes, ac_cv_weird_accept=no, ac_cv_weird_accept=xcc)])
  AC_MSG_RESULT($ac_cv_weird_accept)
  if test $ac_cv_weird_accept = yes; then
    AC_DEFINE(WEIRD_ACCEPT)
  fi
])

dnl AC_HEADER_TIOCGWINSZ
AC_DEFUN(AC_HEADER_TIOCGWINSZ,
[ AC_MSG_CHECKING([for TIOCGWINSZ get-window-size capability])
  AC_CACHE_VAL(ac_cv_header_tiocgwinsz,
  [ AC_HEADER_INCLUDE(tmp_inc_termios, sys/termios.h termios.h)
    AC_EGREP_HEADER(yes, [
$tmp_inc_termios
#ifdef TIOCGWINSZ
 yes
#endif
], ac_cv_header_tiocgwinsz=yes, ac_cv_header_tiocgwinsz=no)])dnl
  AC_MSG_RESULT($ac_cv_header_tiocgwinsz)
if test $ac_cv_header_tiocgwinsz = yes; then
  AC_DEFINE(GET_SCREEN_SIZE)
fi
])

dnl AC_SYS_NEED_SIGNAL_RESET
dnl If signal handlers need to be reset, define NEED_SIGNAL_RESET
AC_DEFUN(AC_SYS_NEED_SIGNAL_RESET, 
[ AC_MSG_CHECKING(if signal handlers need to be reset)
  AC_CACHE_VAL(ac_cv_sys_need_signal_reset,
  [ AC_TRY_RUN(
    [ handle_hup(){return;}
      main(){signal(1, handle_hup);kill(getpid(),1);
        exit(((int *)signal(1, handle_hup))==((int *)handle_hup));}],
      ac_cv_sys_need_signal_reset=yes, ac_cv_sys_need_signal_reset=no,
      ac_cv_sys_need_signal_reset=cross-compiling)
  ])
  AC_MSG_RESULT($ac_cv_sys_need_signal_reset)
if test $ac_cv_sys_need_signal_reset = yes; then
  AC_DEFINE(NEED_SIGNAL_RESET)
fi
])dnl

dnl AC_TYPE_STATUS_T
dnl Define status_t to be whatever wait/wait3/waitpid want.
AC_DEFUN(AC_TYPE_STATUS_T,
[ AC_MSG_CHECKING(for what the wait calls want)
  AC_CACHE_VAL(ac_cv_type_status_t,
  [ AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/wait.h>
], [union wait x; wait3(&x, 0, NULL);], ac_cv_type_status_t="union wait",
ac_cv_type_status_t=int)])
  AC_MSG_RESULT($ac_cv_type_status_t)
  AC_DEFINE_UNQUOTED(status_t, $ac_cv_type_status_t)
])dnl

dnl AC_STRUCT_GMTOFF
dnl If struct tm contains tm_gmtoff, define HAVE_TM_GMTOFF. Otherwise,
dnl  if timezone and/or altzone exist, define HAVE_TIMEZONE and/or
dnl  HAVE_ALTZONE respectively.  Finally, if the timezone value is
dnl  backwards, define NEGATIVE_TIMEZONE.
AC_DEFUN(AC_STRUCT_GMTOFF,
[ AC_REQUIRE([AC_STRUCT_TM])dnl
  AC_MSG_CHECKING([for tm_gmtoff in struct tm])
  AC_CACHE_VAL(ac_cv_struct_gmtoff,
  [ AC_TRY_COMPILE([#include <sys/types.h>
#include <$ac_cv_struct_tm>], [struct tm tm; tm.tm_gmtoff;],
  ac_cv_struct_gmtoff=yes, ac_cv_struct_gmtoff=no)])
  AC_MSG_RESULT($ac_cv_struct_gmtoff)
if test $ac_cv_struct_gmtoff = yes; then
  AC_DEFINE(HAVE_TM_GMTOFF)
fi

AC_MSG_CHECKING(for time.h that defines timezone)
AC_CACHE_VAL(ac_cv_extern_timezone,
[AC_TRY_COMPILE([#include <time.h>], [return timezone;],
  ac_cv_extern_timezone=yes,
  ac_cv_extern_timezone=no)])
AC_MSG_RESULT($ac_cv_extern_timezone)
if test $ac_cv_extern_timezone = yes; then
  AC_DEFINE(HAVE_TIMEZONE)
fi

dnl Borrowed from the Python distribution...
AC_MSG_CHECKING(for time.h that defines altzone)
AC_CACHE_VAL(ac_cv_extern_altzone,
[AC_TRY_COMPILE([#include <time.h>], [return altzone;],
  ac_cv_extern_altzone=yes,
  ac_cv_extern_altzone=no)])
AC_MSG_RESULT($ac_cv_extern_altzone)
if test $ac_cv_extern_altzone = yes; then
  AC_DEFINE(HAVE_ALTZONE)
fi

AC_MSG_CHECKING(for time.h that defines daylight)
AC_CACHE_VAL(ac_cv_extern_daylight,
[AC_TRY_COMPILE([#include <time.h>], [return daylight;],
  ac_cv_extern_daylight=yes,
  ac_cv_extern_daylight=no)])
AC_MSG_RESULT($ac_cv_extern_daylight)
if test $ac_cv_extern_daylight = yes; then
  AC_DEFINE(HAVE_DAYLIGHT)
fi

if test $ac_cv_extern_timezone = yes; then
  AC_REQUIRE([AC_ISC_POSIX])dnl
  AC_MSG_CHECKING(if timezone is backwards)
  AC_CACHE_VAL(ac_cv_backwards_timezone,
  dnl Bletcherosity alert!  This will work for the USA and Europe, and 
  dnl countries sharing their timezones.  It will be exactly wrong for
  dnl the Atlantic ocean, bits of Russia, and minor third-world
  dnl countries.  The far East will default to "no," regardless of
  dnl platform.  If you can come up with something better, great --
  dnl Please let me know!
  [ AC_TRY_RUN([main(){
extern long timezone;
switch( timezone/3600 )
{
    /* OK: */
    case 5: case 6: case 7: case 8:	/* USA */
    case 0: case -1: case -2:		/* Europe */
        exit(0);
    /* Backwards: */
    case -5: case -6: case -7: case -8:	/* USA */
    case 1: case 2:			/* Europe */
        exit(1);
    /* Other country -- default to ok: */
    default:
        exit(0);
} return 0;}], ac_cv_backwards_timezone=no, ac_cv_backwards_timezone=yes,
  ac_cv_backwards_timezone=cross-compiling)])
  AC_MSG_RESULT($ac_cv_backwards_timezone)
if test $ac_cv_backwards_timezone = yes; then
  AC_DEFINE(NEGATIVE_TIMEZONE)
fi
fi
])dnl

dnl AC_OUTPUT_PARALLEL(DIRECTORY...)
define(AC_OUTPUT_PARALLEL,
[
define([AC_LIST_PARALLEL_DIRS], [$1])dnl
subdirs="AC_LIST_PARALLEL_DIRS"
AC_SUBST(subdirs)dnl

if test "$no_recursion" != yes; then

  # Remove --cache-file and --srcdir arguments so they do not pile up.
  ac_sub_configure_args=
  ac_prev=
  for ac_arg in $ac_configure_args; do
    if test -n "$ac_prev"; then
      ac_prev=
      continue
    fi
    case "$ac_arg" in
    -srcdir | --srcdir | --srcdi | --srcd | --src | --sr)
      ac_prev=srcdir ;;
    -srcdir=* | --srcdir=* | --srcdi=* | --srcd=* | --src=* | --sr=*)
      ;;
    *) ac_sub_configure_args="$ac_sub_configure_args $ac_arg" ;;
    esac
  done

  for ac_config_dir in $1; do

    # Do not complain, so a configure script can configure whichever
    # parts of a large source tree are present.
    if test ! -d ${srcdir}/../$ac_config_dir; then
      continue
    fi

    echo configuring in $ac_config_dir

    ac_pd=`pwd`
    ac_bn=`basename $ac_pd`
    AC_SUBST(ac_bn)

    case "${srcdir}" in
    .) ;;
    *)
      if test -d ${srcdir}/../$ac_config_dir/$ac_bn || mkdir ${srcdir}/../$ac_config_dir/$ac_bn; then :;
      else
        AC_MSG_ERROR(can not create `pwd`/${srcdir}/../$ac_config_dir/$bn)
      fi
      ;;
    esac

    ac_popdir=`pwd`
    cd ${srcdir}/../$ac_config_dir/$ac_bn

    ac_sub_srcdir=${srcdir}

    # Check for guested configure; otherwise get Cygnus style configure.
    if test -f $ac_sub_srcdir/configure; then
      ac_sub_configure=$ac_sub_srcdir/configure
    elif test -f $ac_sub_srcdir/configure.in; then
      ac_sub_configure=$ac_configure
    else
      AC_MSG_WARN(no configuration information is in $ac_sub_srcdir)
dnl      AC_MSG_WARN(no configuration information is in $ac_config_dir)
      ac_sub_configure=
    fi

    # The recursion is here.
    if test -n "$ac_sub_configure"; then

      echo "[running ${CONFIG_SHELL-/bin/sh} $ac_sub_configure $ac_sub_configure_args ] --srcdir=$ac_sub_srcdir"
      # The eval makes quoting arguments work.
      if eval ${CONFIG_SHELL-/bin/sh} $ac_sub_configure $ac_sub_configure_args --srcdir=$ac_sub_srcdir
      then :
      else
        AC_MSG_ERROR($ac_sub_configure failed for $ac_config_dir)
      fi
    fi

    cd $ac_popdir
  done
fi
])

dnl AC_STRUCT_DIRENT_INO
AC_DEFUN(AC_STRUCT_DIRENT_INO, 
[ AC_REQUIRE([AC_HEADER_DIRENT])
  AC_MSG_CHECKING(if directory entries have inodes)
  AC_CACHE_VAL(ac_cv_struct_dirent_ino,
  [ AC_TRY_COMPILE([#if HAVE_DIRENT_H
#include <dirent.h>
#else
#define dirent direct
#if HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#if HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#if HAVE_DIR_H
#include <ndir.h>
#endif
#endif
],  [struct dirent foo; foo.d_ino;], 
    ac_cv_struct_dirent_ino=yes, ac_cv_struct_dirent_ino=no)])
  AC_MSG_RESULT($ac_cv_struct_dirent_ino)
  if test $ac_cv_struct_dirent_ino = yes ; then
    AC_DEFINE(HAVE_DIRENT_INO)
  fi
])

dnl AC_STRUCT_WINSIZE
AC_DEFUN(AC_STRUCT_WINSIZE, 
[ AC_CACHE_CHECK(for winsize structure, ac_cv_struct_winsize,
  [ AC_TRY_COMPILE([
#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
], [struct winsize foo;], 
    ac_cv_struct_winsize=yes, ac_cv_struct_winsize=no)])
  if test $ac_cv_struct_winsize = yes ; then
    AC_DEFINE(HAVE_WINSIZE)
  fi
])

dnl AC_CHECK_SIZEOF_TYPE_T
AC_DEFUN(AC_CHECK_SIZEOF_TIME_T,
[AC_CACHE_CHECK(for whether time_t is long, ac_cv_c_long_time_t,
[AC_TRY_RUN([
#include <time.h>
int main() { exit(sizeof(time_t) == sizeof(long)); }],
ac_cv_c_long_time_t=no, ac_cv_c_long_time_t=yes)
])
if test $ac_cv_c_long_time_t = yes; then
  AC_DEFINE(HAVE_LONG_TIME_T)
fi
])

dnl AC_CHECK_SIZEOF_SIZE_T
AC_DEFUN(AC_CHECK_SIZEOF_SIZE_T,
[AC_CACHE_CHECK(for whether size_t is long, ac_cv_c_long_size_t,
[AC_TRY_RUN([
#include <stddef.h>
int main() { exit(sizeof(size_t) == sizeof(int)); }],
ac_cv_c_long_size_t=yes, ac_cv_c_long_size_t=no)
])
if test $ac_cv_c_long_size_t = yes; then
  AC_DEFINE(HAVE_LONG_SIZE_T)
fi
])

dnl AC_ADDLIB
AC_DEFUN(AC_ADDLIB,
[ AC_MSG_CHECKING([for $1])
  LP=`dirname $1`
  LN=`basename $1 | sed 's/lib\(.*\)\.a/\1/g'`
  AC_MSG_RESULT([-L$LP -l$LN])
  LIBS="$LIBS -L$LP -l$LN"
])

dnl AC_DECL_NEED_SYS_ERR (Let me know if you have a better name.)
dnl Some systems don't have strerror; *do* have sys_errlist and sys_nerr,
dnl but don't declare them.  SunOS, for instance.
dnl This isn't a perfect test, but we can combine it with HAVE_STRERROR,
dnl HAVE_SYS_ERRLIST and HAVE_SYS_NERR.
AC_DEFUN(AC_DECL_NEED_SYS_ERR,
[ AC_MSG_CHECKING(if sys_errlist must be declared)
  AC_CACHE_VAL(ac_cv_decl_need_sys_errlist, 
  [ AC_HEADER_INCLUDE(tmp_inc_errno, errno.h sys/errno.h)
    AC_TRY_COMPILE([$tmp_inc_errno], [*sys_errlist;], 
      ac_cv_decl_need_sys_errlist=no, ac_cv_decl_need_sys_errlist=yes)])
  AC_MSG_RESULT($ac_cv_decl_need_sys_errlist)
  if test $ac_cv_decl_need_sys_errlist = yes ; then
    AC_DEFINE(NEED_SYS_ERRLIST_DECLARED)
  fi
  AC_MSG_CHECKING(if sys_nerr must be declared)
  AC_CACHE_VAL(ac_cv_decl_need_sys_nerr,
  [ AC_HEADER_INCLUDE(tmp_inc_errno, errno.h sys/errno.h)
    AC_TRY_COMPILE([$tmp_inc_errno], [sys_nerr;],
      ac_cv_decl_need_sys_nerr=no, ac_cv_decl_need_sys_nerr=yes)])
  AC_MSG_RESULT($ac_cv_decl_need_sys_nerr)
  if test $ac_cv_decl_need_sys_nerr = yes ; then
    AC_DEFINE(NEED_SYS_NERR_DECLARED)
  fi
])dnl



# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AM_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
AC_DEFINE_UNQUOTED(VERSION, "$VERSION"))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])


# serial 1

AC_DEFUN(AM_PROG_INSTALL,
[AC_REQUIRE([AC_PROG_INSTALL])
test -z "$INSTALL_SCRIPT" && INSTALL_SCRIPT='${INSTALL_PROGRAM}'
AC_SUBST(INSTALL_SCRIPT)dnl
])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

# Like AC_CONFIG_HEADER, but automatically create stamp file.

AC_DEFUN(AM_CONFIG_HEADER,
[AC_PREREQ([2.12])
AC_CONFIG_HEADER([$1])
dnl When config.status generates a header, we must update the stamp-h file.
dnl This file resides in the same directory as the config header
dnl that is generated.  We must strip everything past the first ":",
dnl and everything past the last "/".
AC_OUTPUT_COMMANDS(changequote(<<,>>)dnl
ifelse(patsubst(<<$1>>, <<[^ ]>>, <<>>), <<>>,
<<test -z "<<$>>CONFIG_HEADERS" || echo timestamp > patsubst(<<$1>>, <<^\([^:]*/\)?.*>>, <<\1>>)stamp-h<<>>dnl>>,
<<am_indx=1
for am_file in <<$1>>; do
  case " <<$>>CONFIG_HEADERS " in
  *" <<$>>am_file "*<<)>>
    echo timestamp > `echo <<$>>am_file | sed -e 's%:.*%%' -e 's%[^/]*$%%'`stamp-h$am_indx
    ;;
  esac
  am_indx=`expr "<<$>>am_indx" + 1`
done<<>>dnl>>)
changequote([,]))])

# Check to see if we're running under Cygwin32, without using
# AC_CANONICAL_*.  If so, set output variable CYGWIN32 to "yes".
# Otherwise set it to "no".

dnl AM_CYGWIN32()
AC_DEFUN(AM_CYGWIN32,
[AC_CACHE_CHECK(for Cygwin32 environment, am_cv_cygwin32,
[AC_TRY_COMPILE(,[return __CYGWIN32__;],
am_cv_cygwin32=yes, am_cv_cygwin32=no)
rm -f conftest*])
CYGWIN32=
test "$am_cv_cygwin32" = yes && CYGWIN32=yes])


# serial 24 AM_PROG_LIBTOOL
AC_DEFUN(AM_PROG_LIBTOOL,
[AC_REQUIRE([AM_ENABLE_SHARED])dnl
AC_REQUIRE([AM_ENABLE_STATIC])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_PROG_RANLIB])dnl
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AM_PROG_LD])dnl
AC_REQUIRE([AM_PROG_NM])dnl
AC_REQUIRE([AC_PROG_LN_S])dnl
dnl
# Always use our own libtool.
LIBTOOL='$(SHELL) $(top_builddir)/libtool'
AC_SUBST(LIBTOOL)dnl

# Check for any special flags to pass to ltconfig.
libtool_flags=
test "$enable_shared" = no && libtool_flags="$libtool_flags --disable-shared"
test "$enable_static" = no && libtool_flags="$libtool_flags --disable-static"
test "$silent" = yes && libtool_flags="$libtool_flags --silent"
test "$ac_cv_prog_gcc" = yes && libtool_flags="$libtool_flags --with-gcc"
test "$ac_cv_prog_gnu_ld" = yes && libtool_flags="$libtool_flags --with-gnu-ld"

# Some flags need to be propagated to the compiler or linker for good
# libtool support.
case "$host" in
*-*-irix6*)
  # Find out which ABI we are using.
  echo '[#]line __oline__ "configure"' > conftest.$ac_ext
  if AC_TRY_EVAL(ac_compile); then
    case "`/usr/bin/file conftest.o`" in
    *32-bit*)
      LD="${LD-ld} -32"
      ;;
    *N32*)
      LD="${LD-ld} -n32"
      ;;
    *64-bit*)
      LD="${LD-ld} -64"
      ;;
    esac
  fi
  rm -rf conftest*
  ;;

*-*-sco3.2v5*)
  # On SCO OpenServer 5, we need -belf to get full-featured binaries.
  CFLAGS="$CFLAGS -belf"
  ;;
esac

# Actually configure libtool.  ac_aux_dir is where install-sh is found.
CC="$CC" CFLAGS="$CFLAGS" CPPFLAGS="$CPPFLAGS" \
LD="$LD" NM="$NM" RANLIB="$RANLIB" LN_S="$LN_S" \
${CONFIG_SHELL-/bin/sh} $ac_aux_dir/ltconfig \
$libtool_flags --no-verify $ac_aux_dir/ltmain.sh $host \
|| AC_MSG_ERROR([libtool configure failed])
])

# AM_ENABLE_SHARED - implement the --enable-shared flag
# Usage: AM_ENABLE_SHARED[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AM_ENABLE_SHARED,
[define([AM_ENABLE_SHARED_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(shared,
changequote(<<, >>)dnl
<<  --enable-shared         build shared libraries [default=>>AM_ENABLE_SHARED_DEFAULT]
changequote([, ])dnl
[  --enable-shared=PKGS    only build shared libraries if the current package
                          appears as an element in the PKGS list],
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_shared=yes ;;
no) enable_shared=no ;;
*)
  enable_shared=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_shared=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_shared=AM_ENABLE_SHARED_DEFAULT)dnl
])

# AM_DISABLE_SHARED - set the default shared flag to --disable-shared
AC_DEFUN(AM_DISABLE_SHARED,
[AM_ENABLE_SHARED(no)])

# AM_DISABLE_STATIC - set the default static flag to --disable-static
AC_DEFUN(AM_DISABLE_STATIC,
[AM_ENABLE_STATIC(no)])

# AM_ENABLE_STATIC - implement the --enable-static flag
# Usage: AM_ENABLE_STATIC[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AM_ENABLE_STATIC,
[define([AM_ENABLE_STATIC_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(static,
changequote(<<, >>)dnl
<<  --enable-static         build static libraries [default=>>AM_ENABLE_STATIC_DEFAULT]
changequote([, ])dnl
[  --enable-static=PKGS    only build shared libraries if the current package
                          appears as an element in the PKGS list],
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_static=yes ;;
no) enable_static=no ;;
*)
  enable_static=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_static=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_static=AM_ENABLE_STATIC_DEFAULT)dnl
])


# AM_PROG_LD - find the path to the GNU or non-GNU linker
AC_DEFUN(AM_PROG_LD,
[AC_ARG_WITH(gnu-ld,
[  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]],
test "$withval" = no || with_gnu_ld=yes, with_gnu_ld=no)
AC_REQUIRE([AC_PROG_CC])
ac_prog=ld
if test "$ac_cv_prog_gcc" = yes; then
  # Check if gcc -print-prog-name=ld gives a path.
  AC_MSG_CHECKING([for ld used by GCC])
  ac_prog=`($CC -print-prog-name=ld) 2>&5`
  case "$ac_prog" in
  # Accept absolute paths.
  /* | [A-Za-z]:\\*)
    test -z "$LD" && LD="$ac_prog"
    ;;
  "")
    # If it fails, then pretend we aren't using GCC.
    ac_prog=ld
    ;;
  *)
    # If it is relative, then search for the first ld in PATH.
    with_gnu_ld=unknown
    ;;
  esac
elif test "$with_gnu_ld" = yes; then
  AC_MSG_CHECKING([for GNU ld])
else
  AC_MSG_CHECKING([for non-GNU ld])
fi
AC_CACHE_VAL(ac_cv_path_LD,
[if test -z "$LD"; then
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:"
  for ac_dir in $PATH; do
    test -z "$ac_dir" && ac_dir=.
    if test -f "$ac_dir/$ac_prog"; then
      ac_cv_path_LD="$ac_dir/$ac_prog"
      # Check to see if the program is GNU ld.  I'd rather use --version,
      # but apparently some GNU ld's only accept -v.
      # Break only if it was the GNU/non-GNU ld that we prefer.
      if "$ac_cv_path_LD" -v 2>&1 < /dev/null | egrep '(GNU|with BFD)' > /dev/null; then
	test "$with_gnu_ld" != no && break
      else
        test "$with_gnu_ld" != yes && break
      fi
    fi
  done
  IFS="$ac_save_ifs"
else
  ac_cv_path_LD="$LD" # Let the user override the test with a path.
fi])
LD="$ac_cv_path_LD"
if test -n "$LD"; then
  AC_MSG_RESULT($LD)
else
  AC_MSG_RESULT(no)
fi
test -z "$LD" && AC_MSG_ERROR([no acceptable ld found in \$PATH])
AC_SUBST(LD)
AM_PROG_LD_GNU
])

AC_DEFUN(AM_PROG_LD_GNU,
[AC_CACHE_CHECK([if the linker ($LD) is GNU ld], ac_cv_prog_gnu_ld,
[# I'd rather use --version here, but apparently some GNU ld's only accept -v.
if $LD -v 2>&1 </dev/null | egrep '(GNU|with BFD)' 1>&5; then
  ac_cv_prog_gnu_ld=yes
else
  ac_cv_prog_gnu_ld=no
fi])
])

# AM_PROG_NM - find the path to a BSD-compatible name lister
AC_DEFUN(AM_PROG_NM,
[AC_MSG_CHECKING([for BSD-compatible nm])
AC_CACHE_VAL(ac_cv_path_NM,
[case "$NM" in
/* | [A-Za-z]:\\*)
  ac_cv_path_NM="$NM" # Let the user override the test with a path.
  ;;
*)
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:"
  for ac_dir in /usr/ucb /usr/ccs/bin $PATH /bin; do
    test -z "$ac_dir" && ac_dir=.
    if test -f $ac_dir/nm; then
      # Check to see if the nm accepts a BSD-compat flag.
      # Adding the `sed 1q' prevents false positives on HP-UX, which says:
      #   nm: unknown option "B" ignored
      if ($ac_dir/nm -B /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
        ac_cv_path_NM="$ac_dir/nm -B"
      elif ($ac_dir/nm -p /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
        ac_cv_path_NM="$ac_dir/nm -p"
      else
        ac_cv_path_NM="$ac_dir/nm"
      fi
      break
    fi
  done
  IFS="$ac_save_ifs"
  test -z "$ac_cv_path_NM" && ac_cv_path_NM=nm
  ;;
esac])
NM="$ac_cv_path_NM"
AC_MSG_RESULT([$NM])
AC_SUBST(NM)
])

