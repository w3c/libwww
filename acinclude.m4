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

AC_DEFUN(fp_PROG_INSTALL,
[AC_PROG_INSTALL
test -z "$INSTALL_SCRIPT" && INSTALL_SCRIPT='${INSTALL} -m 755'
AC_SUBST(INSTALL_SCRIPT)dnl
])

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
# define dirent direct
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_DIR_H
#  include <ndir.h>
# endif
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
  [ AC_TRY_COMPILE([#include <sys/ioctl.h>], [struct winsize foo;], 
    ac_cv_struct_winsize=yes, ac_cv_struct_winsize=no)])
  if test $ac_cv_struct_winsize = yes ; then
    AC_DEFINE(HAVE_WINSIZE)
  fi
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


