/*								       HTInet.c
**	GENERIC INTERNET UTILITIES
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This code is in common between client and server sides.
**
**	16 Mar 96  HFN	Spawned off from HTTCP.c
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTParse.h"
#include "HTAlert.h"
#include "HTError.h"
#include "HTNetMan.h"
#include "HTDNS.h"
#include "HTInet.h"					 /* Implemented here */

#ifndef DEFAULT_NEWS_HOST
#define DEFAULT_NEWS_HOST	"news"
#endif

#ifndef SERVER_FILE
#define SERVER_FILE		"/usr/local/lib/rn/server"
#endif

/* ------------------------------------------------------------------------- */

/*
**	Returns the string equivalent to the errno passed in the argument.
**	We can't use errno directly as we have both errno and socerrno. The
**	result is a dynamic string that must be freed by the caller.
*/
PUBLIC char * HTErrnoString (int errornumber)
{
    char * msg = NULL;
#ifdef _WINSOCKAPI_
    if ((msg = (char *) HT_MALLOC(64)) == NULL)
	HT_OUTOFMEM("HTErrnoString");
    *msg = '\0';
    sprintf(msg, "WinSock reported error=%ld", WSAGetLastError());
#else
#ifdef HAVE_STRERROR
    StrAllocCopy(msg, strerror(errornumber));
#else
#ifdef HAVE_SYS_ERRLIST
#ifdef HAVE_SYS_NERR
    if (errno < sys_nerr)
	StrAllocCopy(msg, sys_errlist[errno]);
    else 
        StrAllocCopy(msg, "Unknown error");
#else
    StrAllocCopy(msg, sys_errlist[errno]);
#endif /* HAVE_SYS_NERR */
#else
#ifdef VMS
    if ((msg = (char *) HT_MALLOC(64)) == NULL)
	HT_OUTOFMEM("HTErrnoString");
    *msg = '\0';
    sprintf(msg, "Unix errno=%ld dec, VMS error=%lx hex", errornumber,
	    vaxc$errno);
#else
    StrAllocCopy(msg, "Error number not translated!");
#endif /* _WINSOCKAPI_ */
#endif /* VMS */
#endif /* HAVE_SYS_ERRLIST */
#endif /* HAVE_STRERROR */
    return msg;
}


/*	Debug error message
*/
PUBLIC int HTInetStatus (int errnum, char * where)
{
#ifdef VMS
    HTTRACE(CORE_TRACE, "System Error Unix = %ld dec\n" _ errno);
    HTTRACE(CORE_TRACE, "System Error VMS  = %lx hex\n" _ vaxc$errno);
    return (-vaxc$errno);
#else
#ifdef _WINSOCKAPI_
    HTTRACE(CORE_TRACE, "System Error Unix = %ld dec\n" _ errno);
    HTTRACE(CORE_TRACE, "System Error WinSock error=%lx hex\n" _ 
			    WSAGetLastError());
    return (-errnum);
#else
#ifdef HTDEBUG
    if (CORE_TRACE) {
	char * errmsg = HTErrnoString(errnum);
	HTTRACE(CORE_TRACE, "System Error %d after call to %s() failed\n............ %s\n" _
		errno _ where _ errmsg);
	HT_FREE(errmsg);
    }
#endif /* HTDEBUG */
    return (-errnum);
#endif /* _WINSOCKAPI_ */
#endif /* VMS */
}


/*	Parse a cardinal value				       parse_cardinal()
**	----------------------
**
** On entry,
**	*pp	    points to first character to be interpreted, terminated by
**		    non 0:9 character.
**	*pstatus    points to status already valid
**	maxvalue    gives the largest allowable value.
**
** On exit,
**	*pp	    points to first unread character
**	*pstatus    points to status updated iff bad
*/

PUBLIC unsigned int HTCardinal (int *		pstatus,
				char **		pp,
				unsigned int	max_value)
{
    unsigned int n=0;
    if ( (**pp<'0') || (**pp>'9')) {	    /* Null string is error */
	*pstatus = -3;  /* No number where one expeceted */
	return 0;
    }
    while ((**pp>='0') && (**pp<='9')) n = n*10 + *((*pp)++) - '0';

    if (n>max_value) {
	*pstatus = -4;  /* Cardinal outside range */
	return 0;
    }

    return n;
}

/* ------------------------------------------------------------------------- */
/*	       		        SIGNAL HANDLING 			     */
/* ------------------------------------------------------------------------- */

#ifdef WWWLIB_SIG
/*								    HTSetSignal
**  This function sets up signal handlers. This might not be necessary to
**  call if the application has its own handlers.
*/
#include <signal.h>

PUBLIC void HTSetSignal (void)
{
    /* On some systems (SYSV) it is necessary to catch the SIGPIPE signal
    ** when attemting to connect to a remote host where you normally should
    ** get `connection refused' back
    */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
	HTTRACE(CORE_TRACE, "HTSignal.... Can't catch SIGPIPE\n");
    } else {
	HTTRACE(CORE_TRACE, "HTSignal.... Ignoring SIGPIPE\n");
    }
}
#else /* WWWLIB_SIG */

PUBLIC void HTSetSignal (void) { }

#endif /* WWWLIB_SIG */

/* ------------------------------------------------------------------------- */
/*	       		     HOST NAME FUNCTIONS 			     */
/* ------------------------------------------------------------------------- */

/*	Produce a string for an Internet address
**	----------------------------------------
**
** On exit,
**	returns	a pointer to a static string which must be copied if
**		it is to be kept.
*/
PUBLIC const char * HTInetString (SockA * sin)
{
#ifndef DECNET  /* Function only used below for a trace message */
#if 0
    /* This dumps core on some Sun systems :-(. The problem is now, that 
       the current implememtation only works for IP-addresses and not in
       other address spaces. */
    return inet_ntoa(sin->sin_addr);
#endif
    static char string[16];
    sprintf(string, "%d.%d.%d.%d",
	    (int)*((unsigned char *)(&sin->sin_addr)+0),
	    (int)*((unsigned char *)(&sin->sin_addr)+1),
	    (int)*((unsigned char *)(&sin->sin_addr)+2),
	    (int)*((unsigned char *)(&sin->sin_addr)+3));
    return string;
#else
    return "";
#endif /* Decnet */
}

/*	Parse a network node address and port
**	-------------------------------------
** 	It is assumed that any portnumber and numeric host address
**	is given in decimal notation. Separation character is '.'
**	Any port number gets chopped off
**      Returns:
**	       	>0	Number of homes
**		 0	Wait for persistent socket
**		-1	Error
*/
PUBLIC int HTParseInet (HTHost * host, char * hostname, HTRequest * request)
{
    int status = 1;
    SockA *sin = &host->sock_addr;

#ifdef DECNET
    /* read Decnet node name. @@ Should know about DECnet addresses, but it's
       probably worth waiting until the Phase transition from IV to V. */

    sin->sdn_nam.n_len = min(DN_MAXNAML, strlen(hostname));  /* <=6 in phase 4 */
    strncpy (sin->sdn_nam.n_name, hostname, sin->sdn_nam.n_len + 1);

    HTTRACE(CORE_TRACE, "DECnet: Parsed address as object number %d on host %.6s...\n" _ 
		sin->sdn_objnum _ hostname);
#else /* Internet */
    {
	char *strptr = hostname;
	while (*strptr) {
	    if (*strptr == ':') {
		*strptr = '\0';	   /* Don't want port number in numeric host */
		break;
	    }
	    if (!isdigit((int) *strptr) && *strptr != '.')
		break;
	    strptr++;
	}
	if (!*strptr) {
#ifdef GUSI
	    sin->sin_addr = inet_addr(hostname); 		 /* See netinet/in.h */
#else
	    sin->sin_addr.s_addr = inet_addr(hostname);	  /* See arpa/inet.h */
#endif
	} else {
	    char * port = strchr(hostname, ':');			/* Chop port */
	    if (port) *port = '\0';
	    status = HTGetHostByName(host, hostname, request);
	}
#ifdef HTDEBUG
	if (status > 0)
	    HTTRACE(CORE_TRACE, "ParseInet... as port %d on %s with %d homes\n" _
		    (int) ntohs(sin->sin_port) _ HTInetString(sin) _ status);
#endif /* HTDEBUG */
    }
#endif /* Internet vs. Decnet */
    return status;
}


#if 0
/*								HTGetDomainName
**	Returns the current domain name without the local host name.
**	The response is pointing to a static area that might be changed
**	using HTSetHostName().
**
**	Returns NULL on error, "" if domain name is not found
*/
PRIVATE char * HTGetDomainName (void)
{
    char * host = HTGetHostName();
    char * domain;
    if (host && *host) {
	if ((domain = strchr(host, '.')) != NULL)
	    return ++domain;
	else
	    return "";
    } else
	return NULL;
}
#endif

/*								HTGetHostName
**	Returns the name of this host. It uses the following algoritm:
**
**	1) gethostname()
**	2) if the hostname doesn't contain any '.' try to read
**	   /etc/resolv.conf. If there is no domain line in this file then
**	3) Try getdomainname and do as the man pages say for resolv.conf (sun)
**	   If there is no domain line in this file, then it is derived
**	   from the domain name set by the domainname(1) command, usually
**	   by removing the first component. For example, if the domain-
**	   name is set to ``foo.podunk.edu'' then the default domain name
**	   used will be ``pudunk.edu''.
**
**	This is the same procedure as used by res_init() and sendmail.
**
**	Return: hostname on success else NULL
*/
PUBLIC char * HTGetHostName (void)
{
    char * hostname = NULL;
    int fqdn = 0;				     /* 0=no, 1=host, 2=fqdn */
    char name[MAXHOSTNAMELEN+1];
    *(name+MAXHOSTNAMELEN) = '\0';

#if defined(HAVE_SYSINFO) && defined(SI_HOSTNAME)
    if (!fqdn && sysinfo(SI_HOSTNAME, name, MAXHOSTNAMELEN) > 0) {
	char * dot = strchr(name, '.');
	HTTRACE(CORE_TRACE, "HostName.... sysinfo says `%s\'\n" _ name);
	StrAllocCopy(hostname, name);
	fqdn = dot ? 2 : 1;
    }
#endif /* HAVE_SYSINFO */

#ifdef HAVE_GETHOSTNAME
    if (!fqdn && gethostname(name, MAXHOSTNAMELEN) == 0) {
	char * dot = strchr(name, '.');
	HTTRACE(CORE_TRACE, "HostName.... gethostname says `%s\'\n" _ name);
	StrAllocCopy(hostname, name);
	fqdn = dot ? 2 : 1;
    }
#endif /* HAVE_GETHOSTNAME */

#ifdef RESOLV_CONF
    /* Now try the resolver config file */
    {
	FILE *fp;
	if (fqdn==1 && (fp = fopen(RESOLV_CONF, "r")) != NULL) {
	    char buffer[80];
	    *(buffer+79) = '\0';
	    while (fgets(buffer, 79, fp)) {
		if (!strncasecomp(buffer, "domain", 6) ||
		    !strncasecomp(buffer, "search", 6)) {
		    char *domainstr = buffer+6;
		    char *end;
		    while (*domainstr == ' ' || *domainstr == '\t')
			domainstr++;
		    end = domainstr;
		    while (*end && !isspace((int) *end))
			end++;
		    *end = '\0';
		    if (*domainstr) {
			StrAllocCat(hostname, ".");
			StrAllocCat(hostname, domainstr);
			fqdn = 2;
			break;
		    }
		}
	    }
	    fclose(fp);
	}
    }
#endif /* RESOLV_CONF */

#ifdef HAVE_GETDOMAINNAME
    /* If everything else has failed then try getdomainname */
    if (fqdn==1) {
	if (getdomainname(name, MAXHOSTNAMELEN)) {
	    HTTRACE(CORE_TRACE, "HostName.... Can't get domain name\n");
	    StrAllocCopy(hostname, "");
	    return NULL;
	}

	/* If the host name and the first part of the domain name are different
	   then use the former as it is more exact (I guess) */
	if (strncmp(name, hostname, (int) strlen(hostname))) {
	    char *domain = strchr(name, '.');
	    if (!domain)
		domain = name;
	    StrAllocCat(hostname, domain);
	}
    }
#endif /* HAVE_GETDOMAINNAME */

    if (hostname) {
	char *strptr = hostname;
	while (*strptr) {	    
	    *strptr = TOLOWER(*strptr);
	    strptr++;
	}
	if (*(hostname+strlen(hostname)-1) == '.')    /* Remove trailing dot */
	    *(hostname+strlen(hostname)-1) = '\0';
	HTTRACE(CORE_TRACE, "HostName.... FQDN is `%s\'\n" _ hostname);
    }
    return hostname;
}

/*							       HTGetMailAddress
**
**	Get the mail address of the current user on the current host. The
**	domain name used is the one initialized in HTSetHostName or
**	HTGetHostName. The login name is determined using (ordered):
**
**		getlogin
**		getpwuid(getuid())
**
**	The weakness about the last attempt is if the user has multiple
**	login names each with the same user ID. If this fails as well then:
**
**		LOGNAME environment variable
**		USER environment variable
**
**	Returns NULL or string to be freed by caller
*/
PUBLIC char * HTGetMailAddress (void)
{
#ifdef HT_REENTRANT
  char name[HT_LOGNAME_MAX];    /* For getlogin_r or getUserName */
  int result;
#endif
#ifdef WWW_MSWINDOWS/* what was the plan for this under windows? - EGP */
  char name[256];    /* For getlogin_r or getUserName */
  unsigned int bufSize = sizeof(name);
#endif
#ifdef HAVE_PWD_H
    struct passwd * pw_info = NULL;
#endif
    char * login = NULL;

#ifdef WWW_MSWINDOWS
    if (!login && GetUserName(name, &bufSize) != TRUE)
        HTTRACE(CORE_TRACE, "MailAddress. GetUsername returns NO\n");
#endif /* WWW_MSWINDOWS */

#ifdef HAVE_CUSERID
    if (!login && (login = (char *) cuserid(NULL)) == NULL)
        HTTRACE(CORE_TRACE, "MailAddress. cuserid returns NULL\n");
#endif /* HAVE_CUSERID */

#ifdef HAVE_GETLOGIN
#ifdef GETLOGIN_R_RETURNS_POINTER
    if (!login && (login = (char *) getlogin_r(name, HT_LOGNAME_MAX)) == NULL)
#elif defined(GETLOGIN_R_RETURNS_INT)
    if (!login && (result = getlogin_r(name, HT_LOGNAME_MAX)) == 0)
    {
	login = &name[0];
    }
    else
#else
    if (!login && (login = (char *) getlogin()) == NULL)
#endif /* HT_REENTRANT */
	HTTRACE(CORE_TRACE, "MailAddress. getlogin returns NULL\n");
#endif /* HAVE_GETLOGIN */

#ifdef HAVE_PWD_H
    if (!login && (pw_info = getpwuid(getuid())) != NULL)
	login = pw_info->pw_name;
#endif /* HAVE_PWD_H */

    if (!login && (login = getenv("LOGNAME")) == NULL)
	HTTRACE(CORE_TRACE, "MailAddress. LOGNAME not found\n");

    if (!login && (login = getenv("USER")) == NULL)
	HTTRACE(CORE_TRACE, "MailAddress. USER not found\n");

    if (!login) login = HT_DEFAULT_LOGIN;

    if (login) {
	char * domain = NULL;
	char * mailaddress = NULL;
	StrAllocCopy(mailaddress, login);
	StrAllocCat(mailaddress, "@");
	if ((domain = HTGetHostName()) != NULL) {
	    StrAllocCat(mailaddress, domain);
	    HT_FREE(domain);
	}
	return mailaddress;
    }
    return NULL;
}

/*
**	Except on the NeXT, we pick up the NewsHost name from
**
**	1.	Environment variable NNTPSERVER
**	2.	File SERVER_FILE
**	3.	Compilation time macro DEFAULT_NEWS_HOST
**
**	On the NeXT, we pick up the NewsHost name from, in order:
**
**	1.	WorldWideWeb default "NewsHost"
**	2.	News default "NewsHost"
**	3.	Compilation time macro DEFAULT_NEWS_HOST
**
**	Returns NULL or string to be freed by caller
*/
PUBLIC char * HTGetNewsServer (void)
{
    char * newshost = NULL;
    char buffer[80];

#ifdef NeXTStep
    if ((newshost = NXGetDefaultValue("WorldWideWeb","NewsHost")) == 0)
	if ((newshost = NXGetDefaultValue("News","NewsHost")) == 0)
	    newshost = DEFAULT_NEWS_HOST;
#else
    if ((newshost = (char *) getenv("NNTPSERVER")) == NULL) {
	FILE *fp = fopen(SERVER_FILE, "r");
	*(buffer+79) = '\0';
	if (fp) {
	    if (fgets(buffer, 79, fp)) {
		char *end;
		newshost = buffer;
		while (*newshost == ' ' || *newshost == '\t')
		    newshost++;
		end = newshost;
		while (*end && !isspace((int) *end))
		    end++;
		*end = '\0';
	    }
	    fclose(fp);
	}
    }
#endif /* NestStep */

    /* Last resort */
    if (!newshost || !*newshost) newshost = DEFAULT_NEWS_HOST;

    /* Canonicalize host name */
    {
	char * result = NULL;
	StrAllocCopy(result, newshost);
	{
	    char * strptr = result;
	    while (*strptr) {
		*strptr = TOLOWER(*strptr);
		strptr++;
	    }
	}
	return result;
    }
}

/*	Timezone Offset
**	---------------
**	Calculates the offset from GMT in seconds
*/
PUBLIC time_t HTGetTimeZoneOffset (void)
{
    static time_t HTTimeZone = -1;		  /* Invalid timezone offset */
    if (HTTimeZone != -1) return HTTimeZone;		     /* Already done */
#ifdef HAVE_TIMEZONE
    {
	time_t cur_t = time(NULL);
#ifdef HT_REENTRANT
	struct tm loctime;
	struct tm *local = (struct tm *) localtime_r(&cur_t, &loctime);
#else
	struct tm *local = localtime(&cur_t);
#endif /* HT_REENTRANT */
#ifdef HAVE_DAYLIGHT
	if (daylight && local->tm_isdst>0) {		   /* daylight time? */
#else
	if (local->tm_isdst>0) {			   /* daylight time? */
#endif /* HAVE_DAYLIGHT */
#ifdef HAVE_ALTZONE
	    HTTimeZone = altzone;
#else
 	    /* Assumes a fixed DST offset of 1 hour, which is probably wrong */
#ifdef __CYGWIN__
 	    HTTimeZone = _timezone - 3600;
#else
 	    HTTimeZone = timezone - 3600;
#endif
#endif /* HAVE_ALTZONE */
	} else {						       /* no */
#ifdef __CYGWIN__
	    HTTimeZone = _timezone;
#else
	    HTTimeZone = timezone;
#endif
	}
	HTTimeZone = -HTTimeZone;
	HTTRACE(CORE_TRACE, "TimeZone.... GMT + (%02d) hours (including DST)\n" _ 
		    (int) HTTimeZone/3600);
    }
#else
#ifdef HAVE_TM_GMTOFF
    {
	time_t cur_t = time(NULL);
#ifdef HT_REENTRANT
	struct tm loctime;
	localtime_r(&cur_t, &loctime);
#else
	struct tm * local = localtime(&cur_t);
#endif /* HT_REENTRANT */
	HTTimeZone = local->tm_gmtoff;
	HTTRACE(CORE_TRACE, "TimeZone.... GMT + (%02d) hours (including DST)\n" _ 
		    (int)local->tm_gmtoff / 3600);
    }
#else
    HTTRACE(CORE_TRACE, "TimeZone.... Not defined\n");
#endif /* HAVE_TM_GMTOFF */
#endif /* HAVE_TIMEZONE */
    return HTTimeZone;
}

/*
**	Finds a temporary name in in the directory given. If the directory
**	is NULL then don't prepend anything.
**	If success, the result must be freed by caller, else we return NULL
*/
PUBLIC char * HTGetTmpFileName (const char * abs_dir)
{
    char * result = NULL;
#ifdef HAVE_TEMPNAM
    static char * envtmpdir = NULL;
    size_t len = 0;
    if (abs_dir && *abs_dir) {
      char * tmpdir = getenv("TMPDIR");
      if (tmpdir)
          len = strlen(tmpdir);
      if (len) {
          if (!(envtmpdir = (char *) HT_REALLOC(envtmpdir, len + 8)))
              HT_OUTOFMEM("HTGetTmpFileName");
          strcpy(envtmpdir, "TMPDIR=");
          strcpy(envtmpdir + 7, tmpdir);
          putenv("TMPDIR=");
      }
    }
#ifdef __CYGWIN__
    result = tempnam(abs_dir, "");
#else
    result = tempnam(abs_dir, NULL);
#endif /* __CYGWIN__ */
    if (len)
      putenv(envtmpdir);
#else
    /*
    **  This is only approx. as we don't know if this file exists or not.
    **  Hopefully, tempnam() exists on enough platforms so that this is not
    **  a problem.
    */
    char * offset = NULL;
    if (!(result = (char *) HT_MALLOC((abs_dir ? strlen(abs_dir) : 0) +
				      HT_MAX_TMPNAM + 2)))
	HT_OUTOFMEM("HTGetTmpFileName");

#ifdef WWW_MSWINDOWS
    if (abs_dir) {
#else
    if (abs_dir && *abs_dir==DIR_SEPARATOR_CHAR) {
#endif /* WWW_MSWINDOWS */
	strcpy(result, abs_dir);
	offset = result+strlen(result);
	if (*(offset-1) != DIR_SEPARATOR_CHAR) *offset++ = DIR_SEPARATOR_CHAR;

#ifdef HT_REENTRANT
	tmpnam_r(offset);
#else
	tmpnam(offset);
#endif

	{
	    char * orig = strrchr(offset, DIR_SEPARATOR_CHAR);
	    char * dest = offset;
	    if (orig++) while ((*dest++ = *orig++));
	}
    } else {
	offset = result;
#ifdef HT_REENTRANT
	tmpnam_r(offset);
#else
	tmpnam(offset);
#endif
	offset = result;
    }
#endif /* HAVE_TEMPNAM */
    return result;
}

/*
**  Copied from X utilities
*/
PUBLIC ms_t HTGetTimeInMillis (void)
{
#ifdef WWW_MSWINDOWS
    return GetTickCount();
#else /* WWW_MSWINDOWS */
#ifdef HAVE_GETTIMEOFDAY
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return(tp.tv_sec * 1000) + (tp.tv_usec / 1000);
#else
    return((ms_t) 0);
#endif
#endif /* !WWW_MSWINDOWS */
}
