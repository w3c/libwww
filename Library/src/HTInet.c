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
#include "sysdep.h"
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
**	result is a static buffer.
*/
PUBLIC const char * HTErrnoString (int errornumber)
{
#ifdef HAVE_STRERROR
    return strerror(errornumber);
#else
#ifdef HAVE_SYS_ERRLIST
#ifdef HAVE_SYS_NERR
    return (errno < sys_nerr ? sys_errlist[errno] : "Unknown error");
#else
    return sys_errlist[errno];
#endif /* HAVE_SYS_NERR */
#else
#ifdef VMS
    static char buf[60];
    sprintf(buf, "Unix errno=%ld dec, VMS error=%lx hex", errornumber,
	    vaxc$errno);
    return buf;
#else
#ifdef _WINSOCKAPI_
    static char buf[60];
    sprintf(buf, "Unix errno=%ld dec, WinSock error=%ld", errornumber,
	    WSAGetLastError());
    return buf;
#else
    return "(Error number not translated)";
#endif /* _WINSOCKAPI_ */
#endif /* VMS */
#endif /* HAVE_SYS_ERRLIST */
#endif /* HAVE_STRERROR */
}


/*	Debug error message
*/
PUBLIC int HTInetStatus (int errnum, char * where)
{
#ifdef VMS
    if (PROT_TRACE) HTTrace("System Error Unix = %ld dec\n", errno);
    if (PROT_TRACE) HTTrace("System Error VMS  = %lx hex\n", vaxc$errno);
    return (-vaxc$errno);
#else
#ifdef _WINSOCKAPI_
    if (PROT_TRACE) HTTrace("System Error Unix = %ld dec\n", errno);
    if (PROT_TRACE) HTTrace("System Error WinSock error=%lx hex\n",
			    WSAGetLastError());
    return (-errnum);
#else
    if (PROT_TRACE)
	HTTrace("System Error %d after call to %s() failed\n............ %s\n",
		errno, where, HTErrnoString(errnum));
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
	if (PROT_TRACE) HTTrace("HTSignal.... Can't catch SIGPIPE\n");
    } else {
	if (PROT_TRACE) HTTrace("HTSignal.... Ignoring SIGPIPE\n");
    }
}
#else
#ifdef WWW_WIN_DLL
PUBLIC void HTSetSignal (void) {}
#endif /* WWW_WIN_DLL */
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

    if (PROT_TRACE)
	HTTrace("DECnet: Parsed address as object number %d on host %.6s...\n",
		sin->sdn_objnum, hostname);
#else /* Internet */
    {
	char *strptr = hostname;
	while (*strptr) {
	    if (*strptr == ':') {
		*strptr = '\0';	   /* Don't want port number in numeric host */
		break;
	    }
	    if (!isdigit(*strptr) && *strptr != '.')
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
	if (PROT_TRACE) {
	    if (status > 0)
		HTTrace("ParseInet... as port %d on %s with %d homes\n",
			(int) ntohs(sin->sin_port), HTInetString(sin), status);
	}
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
	if (PROT_TRACE) HTTrace("HostName.... sysinfo says `%s\'\n", name);
	StrAllocCopy(hostname, name);
	fqdn = dot ? 2 : 1;
    }
#endif /* HAVE_SYSINFO */

#ifdef HAVE_GETHOSTNAME
    if (!fqdn && gethostname(name, MAXHOSTNAMELEN) == 0) {
	char * dot = strchr(name, '.');
	if (PROT_TRACE) HTTrace("HostName.... gethostname says `%s\'\n", name);
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
		    while (*end && !isspace(*end))
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
	    if (PROT_TRACE)
		HTTrace("HostName.... Can't get domain name\n");
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
	if (PROT_TRACE) HTTrace("HostName.... FQDN is `%s\'\n", hostname);
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
  char name[LOGNAME_MAX+1];    /* For getlogin_r or getUserName */
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
        if (PROT_TRACE) HTTrace("MailAddress. GetUsername returns NO\n");
#endif /* WWW_MSWINDOWS */

#ifdef HAVE_CUSERID
    if (!login && (login = (char *) cuserid(NULL)) == NULL)
        if (PROT_TRACE) HTTrace("MailAddress. cuserid returns NULL\n");
#endif /* HAVE_CUSERID */

#ifdef HAVE_GETLOGIN
#ifdef HT_REENTRANT
    if (!login && (login = (char *) getlogin_r(name, LOGNAME_MAX)) == NULL)
#else
    if (!login && (login = (char *) getlogin()) == NULL)
#endif /* HT_REENTRANT */
	if (PROT_TRACE) HTTrace("MailAddress. getlogin returns NULL\n");
#endif /* HAVE_GETLOGIN */

#ifdef HAVE_PWD_H
    if (!login && (pw_info = getpwuid(getuid())) != NULL)
	login = pw_info->pw_name;
#endif /* HAVE_PWD_H */

    if (!login && (login = getenv("LOGNAME")) == NULL)
	if (PROT_TRACE) HTTrace("MailAddress. LOGNAME not found\n");

    if (!login && (login = getenv("USER")) == NULL)
	if (PROT_TRACE) HTTrace("MailAddress. USER not found\n");

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
		while (*end && !isspace(*end))
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
 	    HTTimeZone = timezone - 3600;
#endif /* HAVE_ALTZONE */
	} else {						       /* no */
	    HTTimeZone = timezone;
	}
	HTTimeZone = -HTTimeZone;
	if (CORE_TRACE)
	    HTTrace("TimeZone.... GMT + (%02d) hours (including DST)\n",
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
	if (CORE_TRACE)
	    HTTrace("TimeZone.... GMT + (%02d) hours (including DST)\n",
		    (int)local->tm_gmtoff / 3600);
    }
#else
    if (CORE_TRACE) HTTrace("TimeZone.... Not defined\n");
#endif /* HAVE_TM_GMTOFF */
#endif /* HAVE_TIMEZONE */
    return HTTimeZone;
}

/*
**	Finds a temporary name in in the directory given. If the directory
**	is NULL then don't prepend anything.
**	If success, the result must be freed by caller, else we return NULL
*/
PUBLIC char * HTGetTmpFileName (const char * dir)
{
    char * result = NULL;
    char * offset = NULL;
    if (!(result = (char *) HT_MALLOC(dir ? strlen(dir) : 0 +HT_MAX_TMPNAM+2)))
	HT_OUTOFMEM("HTGetTmpFileName");
    if (dir && *dir) {
	strcpy(result, dir);
	offset = result+strlen(result)-1;
	if (*offset != '/') *offset++ = '/';
    } else
	offset = result;
#ifdef HT_REENTRANT
    tmpnam_r(offset);
#else
    tmpnam(offset);
#endif

    /*
    **  If we don't have a dir then remove what the system call may have
    **  prepended the response with
    */
    if (offset == result) {
#ifdef WWW_MSWINDOWS
	char * orig = strrchr(offset, '\\');
#else
        char * orig = strrchr(offset, '/');
#endif /* WWW_MSWINDOWS */
        char * dest = result;
	if (orig++) while ((*dest++ = *orig++));
    }
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
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return(tp.tv_sec * 1000) + (tp.tv_usec / 1000);
#endif /* !WWW_MSWINDOWS */
}
