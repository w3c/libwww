/*
** Hand tailored config.h for windows. The options included
** here are only the ones affecting the libwww user. All other
** defines can be found in the sysdep.h file
*/

/* Define to be the package name. */
#define W3C_PACKAGE "w3c-libwww"

/* Define to be the version. */
#define W3C_VERSION "5.1"

/* Define to enable HT_ZLIB compression / decompression support. */
/* #undef HT_ZLIB */

/* Define to enable support for enabling a rules file w/o user interaction. */
/* #undef HT_AUTOMATIC_RULES */

/* Define to enable SOCKS firewall-breaching code. */
/* #undef SOCKS */
/* #undef SOCKS4 */
/* #undef SOCKS5 */

/* Define to be the location of the W3C Icons */
#define W3C_ICONS "/usr/local/share/w3c-icons"

/* Define to be the location of the TCL Library */
#define W3C_TCL "/usr/local/lib/libtcl7.5.a"

/* Define to disable Nagle's algorithm */
#define HT_NO_NAGLE 1

/* Define to disable HTTP/1.1 pipelining */
/* #undef HT_NO_PIPELINING */

/* Define to use internal signal handler */
/* #undef WWWLIB_SIG */

/* Define this to be the rlogin program name. */
#define RLOGIN_PROGRAM "rlogin"

/* Define this to be the telnet program name. */
#define TELNET_PROGRAM "telnet"

/* Define this to be the tn3270 program name. */
/* #undef TN3270_PROGRAM */

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */


