/*                                                      System dependencies in the W3 library
                                   SYSTEM DEPENDENCIES
                                             
   System-system differences for TCP include files and macros. This file includes for each
   system the files necessary for network and file I/O.
   
  AUTHORS
  
  TBL                     Tim Berners-Lee, W3 project, CERN, <timbl@info.cern.ch>
                         
  EvA                     Eelco van Asperen <evas@cs.few.eur.nl>
                         
  MA                      Marc Andreesen NCSA
                         
  AT                      Aleksandar Totic <atotic@ncsa.uiuc.edu>
                         
  SCW                    Susan C. Weber <sweber@kyle.eitech.com>
                         
  HISTORY:
  
  22 Feb 91               Written (TBL) as part of the WWW library.
                         
  16 Jan 92               PC code from EvA
                         
  22 Apr 93               Merged diffs bits from xmosaic release
                         
  29 Apr 93              NT code from SCW
                         
 */

#ifndef TCP_H
#define TCP_H

/*

Default values

   These values may be reset and altered by system-specific sections later on.  there are
   also a bunch of defaults at the end .
   
 */
/* Default values of those: */
#define NETCLOSE close      /* Routine to close a TCP-IP socket         */
#define NETREAD  read       /* Routine to read from a TCP-IP socket     */
#define NETWRITE write      /* Routine to write to a TCP-IP socket      */

/* Unless stated otherwise, */
#define SELECT                  /* Can handle >1 channel.               */
#define GOT_SYSTEM              /* Can call shell with string           */

#ifdef unix
#define GOT_PIPE
#endif
#ifdef VM
#define GOT_PIPE                /* Of sorts */
#endif

#ifdef DECNET
typedef struct sockaddr_dn SockA;  /* See netdnet/dn.h or custom vms.h */
#else /* Internet */
typedef struct sockaddr_in SockA;  /* See netinet/in.h */
#endif


/*

Macintosh - Think-C

   Think-C is one development environment on the Mac.
   
   We recommend that you compile with 4-byte ints to be compatible with MPW C.  We used
   Tom Milligan's s_socket library which was written for 4 byte int, and the MacTCP
   library assumes 4-byte int.
   
 */
#ifdef THINK_C
#undef GOT_SYSTEM
#define DEBUG                   /* Can't put it on the CC command line  */
#define NO_UNIX_IO              /* getuid() missing                     */
#define NO_GETPID               /* getpid() does not exist              */
#define NO_GETWD                /* getwd() does not exist               */

#undef NETCLOSE             /* Routine to close a TCP-IP socket         */
#undef NETREAD              /* Routine to read from a TCP-IP socket     */
#undef NETWRITE             /* Routine to write to a TCP-IP socket      */
#define NETCLOSE s_close    /* Routine to close a TCP-IP socket         */
#define NETREAD  s_read     /* Routine to read from a TCP-IP socket     */
#define NETWRITE s_write    /* Routine to write to a TCP-IP socket      */

#define bind s_bind         /* Funny names presumably to prevent clashes */
#define connect s_connect
#define accept s_accept
#define listen s_listen
#define socket s_socket
#define getsockname s_getsockname

/* The function prototype checking is better than the include files
*/

extern s_close(int s);
extern s_read(int s, char *buffer, int buflen);
extern s_write(int s, const char *buffer, int buflen);

extern bind(int s, struct sockaddr *name, int namelen);
extern accept(int s, struct sockaddr *addr, int *addrlen);
extern listen(int s, int qlen);
extern connect(int s, struct sockaddr *addr, int addrlen);

extern s_socket(int domain, int type, int protocol);
extern s_getsockname(int s, struct sockaddr *name, int *namelen);
extern struct hostent *gethostent(const char * name);
extern unsigned long inet_addr(const char * name);

#endif /* THINK_C */


/*

Macintosh - MPW

   MPW is one development environment on the Mac.
   
   This entry was created by Aleksandar Totic (atotic@ncsa.uiuc.edu) this file is
   compatible with sockets package released by NCSA.  One major conflict is that this
   library redefines write/read/etc as macros.  In some of HTML code these macros get
   executed when they should not be. Such files should define NO_SOCKET_DEFS on top. This
   is a temporary hack.
   
 */
#ifdef applec                   /* MPW  */
#undef GOT_SYSTEM
#define DEBUG                   /* Can't put it on the CC command line */
#define NO_UNIX_IO              /* getuid() missing
*/
#define NO_GETPID               /* getpid() does not exist
*/
#define NO_GETWD                /* getwd() does not exist
*/

#undef NETCLOSE             /* Routine to close a TCP-IP socket */
#undef NETREAD              /* Routine to read from a TCP-IP socket */
#undef NETWRITE             /* Routine to write to a TCP-IP socket */
#define NETCLOSE s_close    /* Routine to close a TCP-IP socket */
#define NETREAD  s_read     /* Routine to read from a TCP-IP socket */
#define NETWRITE s_write    /* Routine to write to a TCP-IP socket */
#define _ANSI_SOURCE
#define GUI
#define LINEFEED 10
#define ANON_FTP_HOSTNAME
#ifndef NO_SOCKET_DEFS
#include <MacSockDefs.h>
#endif

#include <socket.ext.h>
#include <string.h>

#endif                 /* MPW */



#ifndef STDIO_H
#include <stdio.h>
#define STDIO_H
#endif

/*

IBM RS600

 */
/*      On the IBM RS-6000, AIX is almost Unix.
**      But AIX must be defined in the makefile.
*/
#ifdef _AIX
#define AIX
#endif
#ifdef AIX
#define unix
#endif

/*    AIX 3.2
**    -------
*/

#ifdef _IBMR2
#define USE_DIRENT
#endif


/*

IBM VM-CMS, VM-XA Mainframes

   MVS is compiled as for VM. MVS has no unix-style I/O.  The command line compile options
   seem to come across in lower case.
   
 */
#ifdef mvs
#define MVS
#endif

#ifdef MVS
#define VM
#endif

#ifdef NEWLIB
#pragma linkage(newlib,OS)      /* Enables recursive NEWLIB */
#endif

/*      VM doesn't have a built-in predefined token, so we cheat: */
#ifndef VM
#include <string.h>             /* For bzero etc - not  VM */
#endif

/*      Note:   All include file names must have 8 chars max (+".h")
**
**      Under VM, compile with "(DEF=VM,SHORT_NAMES,DEBUG)"
**
**      Under MVS, compile with "NOMAR DEF(MVS)" to get rid of 72 char margin
**        System include files TCPIP and COMMMAC neeed line number removal(!)
*/

#ifdef VM                       /* or MVS -- see above. */
#define NOT_ASCII               /* char type is not ASCII */
#define NO_UNIX_IO              /* Unix I/O routines are not supported */
#define NO_GETPID               /* getpid() does not exist */
#define NO_GETWD                /* getwd() does not exist */
#ifndef SHORT_NAMES
#define SHORT_NAMES             /* 8 character uniqueness for globals */
#endif
#include <manifest.h>
#include <bsdtypes.h>
#include <stdefs.h>
#include <socket.h>
#include <in.h>
#include <inet.h>
#include <netdb.h>
#include <errno.h>          /* independent */
extern char asciitoebcdic[], ebcdictoascii[];
#define TOASCII(c)   (c=='\n' ?  10  : ebcdictoascii[c])
#define FROMASCII(c) (c== 10  ? '\n' : asciitoebcdic[c])

#include <bsdtime.h>
#include <time.h>
#include <string.h>
#define INCLUDES_DONE
#define TCP_INCLUDES_DONE
#endif


/*

IBM-PC running MS-DOS with SunNFS for TCP/IP

   This code thanks to Eelco van Asperen &lt;evas@cs.few.eur.nl>
   
 */
#ifdef PCNFS
#include <sys/types.h>
#include <string.h>
#include <errno.h>          /* independent */
#include <sys/time.h>       /* independent */
#include <sys/stat.h>
#include <fcntl.h>          /* In place of sys/param and sys/file */
#define INCLUDES_DONE
#define FD_SET(fd,pmask) (*(unsigned*)(pmask)) |=  (1<<(fd))
#define FD_CLR(fd,pmask) (*(unsigned*)(pmask)) &= ~(1<<(fd))
#define FD_ZERO(pmask)   (*(unsigned*)(pmask))=0
#define FD_ISSET(fd,pmask) (*(unsigned*)(pmask) & (1<<(fd)))
#endif  /* PCNFS */

/*

IBM-PC running Windows NT

   These parameters providede by  Susan C. Weber &lt;sweber@kyle.eitech.com>.
   
 */
#ifdef _WINDOWS
#include "fcntl.h"                      /* For HTFile.c */
#include "sys\types.h"                  /* For HTFile.c */
#include "sys\stat.h"                   /* For HTFile.c */

#undef NETREAD
#undef NETWRITE
#undef NETCLOSE
#define NETREAD(s,b,l)  ((s)>10 ? recv((s),(b),(l),0) : read((s),(b),(l)))
#define NETWRITE(s,b,l) ((s)>10 ? send((s),(b),(l),0) : write((s),(b),(l)))
#define NETCLOSE(s)     ((s)>10 ? closesocket(s) : close(s))
#include <io.h>
#include <string.h>
#include <process.h>
#include <time.h>
#include <direct.h>
#include <stdio.h>
#include <winsock.h>
typedef struct sockaddr_in SockA;  /* See netinet/in.h */
#define INCLUDES_DONE
#define TCP_INCLUDES_DONE
#endif  /* WINDOWS */



/*

VAX/VMS

   Under VMS, there are many versions of TCP-IP. Define one if you do not use Digital's
   UCX product:
   
  UCX                    DEC's "Ultrix connection" (default)
                         
  WIN_TCP                From Wollongong, now GEC software.
                         
  MULTINET               From SRI, now from TGV Inv.
                         
  DECNET                 Cern's TCP socket emulation over DECnet
                         
   The last three do not interfere with the unix i/o library, and so they need special
   calls to read, write and close sockets. In these cases the socket number is a VMS
   channel number, so we make the @@@ HORRIBLE @@@ assumption that a channel number will
   be greater than 10 but a unix file descriptor less than 10.  It works.
   
 */
#ifdef vms
#ifdef WIN_TCP
#undef NETREAD
#undef NETWRITE
#undef NETCLOSE
#define NETREAD(s,b,l)  ((s)>10 ? netread((s),(b),(l)) : read((s),(b),(l)))
#define NETWRITE(s,b,l) ((s)>10 ? netwrite((s),(b),(l)) : write((s),(b),(l)))
#define NETCLOSE(s)     ((s)>10 ? netclose(s) : close(s))
#endif

#ifdef MULTINET
#undef NETCLOSE
#undef NETREAD
#undef NETWRITE
#define NETREAD(s,b,l)  ((s)>10 ? socket_read((s),(b),(l)) : read((s),(b),(l)))
#define NETWRITE(s,b,l) ((s)>10 ? socket_write((s),(b),(l)) : \
                                write((s),(b),(l)))
#define NETCLOSE(s)     ((s)>10 ? socket_close(s) : close(s))
#endif

#ifdef DECNET
#undef SELECT  /* not supported */
#undef NETREAD
#undef NETWRITE
#undef NETCLOSE
#define NETREAD(s,b,l)  ((s)>10 ? recv((s),(b),(l),0) : read((s),(b),(l)))
#define NETWRITE(s,b,l) ((s)>10 ? send((s),(b),(l),0) : write((s),(b),(l)))
#define NETCLOSE(s)     ((s)>10 ? socket_close(s) : close(s))
#endif /* Decnet */

/*      Certainly this works for UCX and Multinet; not tried for Wollongong
*/
#ifdef MULTINET
#include "multinet_root:[multinet.include.sys]types.h"
#include "multinet_root:[multinet.include]errno.h"
#include "multinet_root:[multinet.include.sys]time.h"
#else
#include types
#include errno
#include time
#endif /* multinet */

#include string

#ifndef STDIO_H
#include stdio
#define STDIO_H
#endif

#include file

#ifndef DECNET  /* Why is it used at all ? Types conflict with "types.h" */
#include unixio
#endif

#define INCLUDES_DONE

#ifdef MULTINET  /* Include from standard Multinet directories */
#include "multinet_root:[multinet.include.sys]socket.h"
#ifdef __TIME_LOADED  /* defined by sys$library:time.h */
#define __TIME  /* to avoid double definitions in next file */
#endif
#include "multinet_root:[multinet.include.netinet]in.h"
#include "multinet_root:[multinet.include.arpa]inet.h"
#include "multinet_root:[multinet.include]netdb.h"

#else  /* not multinet */
#ifdef DECNET
#include "types.h"  /* for socket.h */
#include "socket.h"
#include "dn"
#include "dnetdb"
/* #include "vms.h" */

#else /* UCX or WIN */
#include socket
#include in
#include inet
#include netdb

#endif  /* not DECNET */
#endif  /* of Multinet or other TCP includes */

#define TCP_INCLUDES_DONE

#endif  /* vms */



/*

SCO ODT unix version

 */

#ifdef sco
#include <sys/fcntl.h>
#define USE_DIRENT
#endif

/*

MIPS unix

 */
/* Mips hack (bsd4.3/sysV mixture...) */

#ifdef mips
extern int errno;
#endif


/*

Regular BSD unix versions

   These are a default unix where not already defined specifically.
   
 */
#ifndef INCLUDES_DONE
#include <sys/types.h>
/* #include <streams/streams.h>                 not ultrix */
#include <string.h>

#include <errno.h>          /* independent */
#include <sys/time.h>       /* independent */
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/file.h>       /* For open() etc */
#define INCLUDES_DONE
#endif  /* Normal includes */

/*                      Directory reading stuff - BSD or SYS V
*/
#ifdef unix                    /* if this is to compile on a UNIX machine */
#define GOT_READ_DIR 1    /* if directory reading functions are available */
#ifdef USE_DIRENT             /* sys v version */
#include <dirent.h>
#define direct dirent
#else
#include <sys/dir.h>
#endif
#if defined(sun) && defined(__svr4__)
#include <sys/fcntl.h>
#include <limits.h>
#endif
#endif

/*

Defaults

  INCLUDE FILES FOR TCP
  
 */
#ifndef TCP_INCLUDES_DONE
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef __hpux /* this may or may not be good -marc */
#include <arpa/inet.h>      /* Must be after netinet/in.h */
#endif
#include <netdb.h>
#endif  /* TCP includes */


/*

  MACROS FOR MANIPULATING MASKS FOR SELECT()
  
 */
#ifdef SELECT
#ifndef FD_SET
typedef unsigned int fd_set;
#define FD_SET(fd,pmask) (*(pmask)) |=  (1<<(fd))
#define FD_CLR(fd,pmask) (*(pmask)) &= ~(1<<(fd))
#define FD_ZERO(pmask)   (*(pmask))=0
#define FD_ISSET(fd,pmask) (*(pmask) & (1<<(fd)))
#endif  /* FD_SET */
#endif  /* SELECT */

/*

  MACROS FOR CONVERTING CHARACTERS
  
 */
#ifndef TOASCII
#define TOASCII(c) (c)
#define FROMASCII(c) (c)
#endif

#endif /* TCP_H */



/*

   end of system-specific file  */
