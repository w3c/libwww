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
#include "HTReqMan.h"
#include "HTNetMan.h"
#include "HTDNS.h"
#include "HTInet.h"					 /* Implemented here */

/* VMS stuff */
#ifdef VMS
#ifndef MULTINET
#define FD_SETSIZE 32
#else /* Multinet */
#define FD_SETSIZE 256
#endif /* Multinet */
#endif /* VMS */

PRIVATE char *hostname = NULL;			    /* The name of this host */
PRIVATE char *mailaddress = NULL;		     /* Current mail address */

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
PUBLIC int HTParseInet (HTNet * net, char * host)
{
    int status = 1;
    SockA *sin = &net->sock_addr;

#ifdef DECNET
    /* read Decnet node name. @@ Should know about DECnet addresses, but it's
       probably worth waiting until the Phase transition from IV to V. */

    sin->sdn_nam.n_len = min(DN_MAXNAML, strlen(host));  /* <=6 in phase 4 */
    strncpy (sin->sdn_nam.n_name, host, sin->sdn_nam.n_len + 1);

    if (PROT_TRACE)
	HTTrace("DECnet: Parsed address as object number %d on host %.6s...\n",
		sin->sdn_objnum, host);
#else /* Internet */
    {
	char *strptr = host;
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
	    sin->sin_addr = inet_addr(host); 		 /* See netinet/in.h */
#else
	    sin->sin_addr.s_addr = inet_addr(host);	  /* See arpa/inet.h */
#endif
	} else {
	    char * port = strchr(host, ':');			/* Chop port */
	    if (port) *port = '\0';
	    status = HTGetHostByName(net, host);
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


/*								HTGetDomainName
**	Returns the current domain name without the local host name.
**	The response is pointing to a static area that might be changed
**	using HTSetHostName().
**
**	Returns NULL on error, "" if domain name is not found
*/
PUBLIC const char *HTGetDomainName (void)
{
    const char *host = HTGetHostName();
    char *domain;
    if (host && *host) {
	if ((domain = strchr(host, '.')) != NULL)
	    return ++domain;
	else
	    return "";
    } else
	return NULL;
}


/*								HTSetHostName
**	Sets the current hostname inclusive domain name.
**	If this is not set then the default approach is used using
**	HTGetHostname().
*/
PUBLIC void HTSetHostName (char * host)
{
    if (host && *host) {
	char *strptr;
	StrAllocCopy(hostname, host);
	strptr = hostname;
	while (*strptr) {
	    *strptr = TOLOWER(*strptr);
	    strptr++;
	}
	if (*(hostname+strlen(hostname)-1) == '.')    /* Remove trailing dot */
	    *(hostname+strlen(hostname)-1) = '\0';
    } else {
	if (PROT_TRACE) HTTrace("SetHostName. Bad argument ignored\n");
    }
}


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
PUBLIC const char * HTGetHostName (void)
{
    int fqdn = 0;				     /* 0=no, 1=host, 2=fqdn */
    FILE *fp;
    char name[MAXHOSTNAMELEN+1];
    if (hostname) {		       			  /* If already done */
	if (*hostname)
	    return hostname;
	else
	    return NULL;		    /* We couldn't get the last time */
    }
    *(name+MAXHOSTNAMELEN) = '\0';

#ifdef HAVE_SYSINFO
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
		    fqdn = YES;
		    break;
		}
	    }
	}
	fclose(fp);
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


/*
**	Free the host name. Called from HTLibTerminate
*/
PUBLIC void HTFreeHostName (void)
{
    HT_FREE(hostname);
}


/*							       HTSetMailAddress
**	Sets the current mail address plus host name and domain name.
**	If this is not set then the default approach is used using
**	HTGetMailAddress(). If the argument is NULL or "" then HTGetMailAddress
**	returns NULL on a succeding request.
*/
PUBLIC void HTSetMailAddress (char * address)
{
    if (!address || !*address)
	StrAllocCopy(mailaddress, "");
    else
	StrAllocCopy(mailaddress, address);
    if (WWWTRACE)
	HTTrace("SetMailAdr.. Set mail address to `%s\'\n",
		mailaddress);
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
**	Returns NULL if error else pointer to static string
*/
PUBLIC const char * HTGetMailAddress (void)
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
    const char * domain;
    if (mailaddress) {
	if (*mailaddress)
	    return mailaddress;
	else
	    return NULL;       /* No luck the last time so we wont try again */
    }

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
	StrAllocCopy(mailaddress, login);
	StrAllocCat(mailaddress, "@");
	if ((domain = HTGetHostName()) != NULL)
	    StrAllocCat(mailaddress, domain);
	else {
	    *mailaddress = '\0';
	    return NULL;			/* Domain name not available */
	}
	return mailaddress;
    }
    return NULL;
}


/*
**	Free the mail address. Called from HTLibTerminate
*/
PUBLIC void HTFreeMailAddress (void)
{
    HT_FREE(mailaddress);
}
