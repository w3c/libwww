/*									HTTCP.c
**	GENERIC COMMUNICATION CODE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This code is in common between client and server sides.
**
**	16 Jan 92  TBL	Fix strtol() undefined on CMU Mach.
**	25 Jun 92  JFG  Added DECNET option through TCP socket emulation.
**	13 Sep 93  MD   Added correct return of vmserrorno for HTInetStatus.
**			Added decoding of vms error message for MULTINET.
**	31 May 94  HF	Added cache on host id's; now use inet_ntoa() to
**			HTInetString and some other fixes. Added HTDoConnect
**			and HTDoAccept
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTAtom.h"
#include "HTList.h"
#include "HTParse.h"
#include "HTAlert.h"
#include "HTError.h"
#include "HTNetMan.h"
#include "HTDNS.h"
#include "HTTCP.h"					 /* Implemented here */

#ifdef VMS 
#include "HTVMSUtils.h"
#endif /* VMS */

/* VMS stuff */
#ifdef VMS
#ifndef MULTINET
#define FD_SETSIZE 32
#else /* Multinet */
#define FD_SETSIZE 256
#endif /* Multinet */
#endif /* VMS */

/* Macros and other defines */
/* x seconds penalty on a multi-homed host if IP-address is down */
#define TCP_PENALTY		1200

/* x seconds penalty on a multi-homed host if IP-address is timed out */
#define TCP_DELAY		600

/* Max number of non-blocking accepts */
#define MAX_ACCEPT_POLL		30

#ifndef RESOLV_CONF
#define RESOLV_CONF "/etc/resolv.conf"
#endif

PRIVATE char *hostname = NULL;			    /* The name of this host */

PRIVATE char *mailaddress = NULL;		     /* Current mail address */

/* ------------------------------------------------------------------------- */

/*
**	Returns the string equivalent to the errno passed in the argument.
**	We can't use errno directly as we have both errno and socerrno. The
**	result is a static buffer.
*/
PUBLIC CONST char * HTErrnoString ARGS1(int, errornumber)
{

#ifdef HAVE_STRERROR
    return strerror(errornumber);
#else
#ifdef VMS
    static char buf[60];
    sprintf(buf,"Unix errno = %ld dec, VMS error = %lx hex", errornumber,
	    vaxc$errno);
    return buf;
#else 
#ifdef _WINDOWS 
	static char buf[60];
	sprintf(buf, "Unix errno = %ld dec, WinSock erro = %ld", errornumber, WSAGetLastError());
	return buf;
#else
    return (errornumber < sys_nerr ? sys_errlist[errornumber]:"Unknown error");
#endif  /* WINDOWS */
#endif /* VMS */
#endif /* HAVE_STRERROR */
}


/*	Debug error message
*/
PUBLIC int HTInetStatus ARGS2(int, errnum, char *, where)
{
#if ! (defined(VMS) || defined(WINDOWS))

    if (PROT_TRACE)
	fprintf(TDEST, "TCP errno... %d after call to %s() failed.\n............ %s\n", errno, where, HTErrnoString(errnum));

#else /* VMS */
#ifdef VMS 
    if (PROT_TRACE) fprintf(TDEST, "         Unix error number          = %ld dec\n", errno);
    if (PROT_TRACE) fprintf(TDEST, "         VMS error                  = %lx hex\n", vaxc$errno);
#endif
#ifdef WINDOWS 
    if (PROT_TRACE) fprintf(TDEST, "         Unix error number          = %ld dec\n", errno);
    if (PROT_TRACE) fprintf(TDEST, "         NT error                  = %lx hex\n", WSAGetLastError());
#endif 

#ifdef MULTINET
    if (PROT_TRACE) fprintf(TDEST, "         Multinet error             = %lx hex\n", socket_errno); 
    if (PROT_TRACE) fprintf(TDEST, "         Error String               = %s\n", vms_errno_string());
#endif /* MULTINET */

#endif /* VMS */

#ifdef VMS
    /* errno happen to be zero if vaxc$errno <> 0 */
    return -vaxc$errno;
#else
    return -errno;
#endif
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

PUBLIC unsigned int HTCardinal ARGS3
	(int *,		pstatus,
	char **,	pp,
	unsigned int,	max_value)
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
PUBLIC void HTSetSignal NOARGS
{
    /* On some systems (SYSV) it is necessary to catch the SIGPIPE signal
    ** when attemting to connect to a remote host where you normally should
    ** get `connection refused' back
    */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
	if (PROT_TRACE) fprintf(TDEST, "HTSignal.... Can't catch SIGPIPE\n");
    } else {
	if (PROT_TRACE) fprintf(TDEST, "HTSignal.... Ignoring SIGPIPE\n");
    }
}
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
PUBLIC CONST char * HTInetString ARGS1(SockA *, sin)
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
**	Any port number given in host name overrides all other values.
**	'host' might be modified.
**      Returns:
**	       	>0	Number of homes
**		 0	Wait for persistent socket
**		-1	Error
*/
PRIVATE int HTParseInet (HTNet * net, char * host)
{
    int status = 1;
    SockA *sin = &net->sock_addr;

#ifdef DECNET
    /* read Decnet node name. @@ Should know about DECnet addresses, but it's
       probably worth waiting until the Phase transition from IV to V. */

    sin->sdn_nam.n_len = min(DN_MAXNAML, strlen(host));  /* <=6 in phase 4 */
    strncpy (sin->sdn_nam.n_name, host, sin->sdn_nam.n_len + 1);

    if (PROT_TRACE) fprintf(TDEST,  
	"DECnet: Parsed address as object number %d on host %.6s...\n",
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
	} else
	    status = HTGetHostByName(net, host);

	if (PROT_TRACE) {
	    if (status > 0)
		fprintf(TDEST, "ParseInet... as port %d on %s with %d homes\n",
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
PUBLIC CONST char *HTGetDomainName NOARGS
{
    CONST char *host = HTGetHostName();
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
PUBLIC void HTSetHostName ARGS1(char *, host)
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
	if (PROT_TRACE) fprintf(TDEST, "SetHostName. Bad argument ignored\n");
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
PUBLIC CONST char * HTGetHostName NOARGS
{
    BOOL got_it = NO;
    FILE *fp;
    char name[MAXHOSTNAMELEN+1];
    if (hostname) {		       			  /* If already done */
	if (*hostname)
	    return hostname;
	else
	    return NULL;		    /* We couldn't get the last time */
    }
    *(name+MAXHOSTNAMELEN) = '\0';

#ifndef NO_GETHOSTNAME
    if (gethostname(name, MAXHOSTNAMELEN)) { 	     /* Maybe without domain */
	if (PROT_TRACE)
	    fprintf(TDEST, "HostName.... Can't get host name\n");
	return NULL;
    }
    if (PROT_TRACE)
	fprintf(TDEST, "HostName.... Local host name is  `%s\'\n", name);
    StrAllocCopy(hostname, name);
    {
	char *strptr = strchr(hostname, '.');
	if (strptr != NULL)				   /* We have it all */
	    got_it = YES;
    }

#ifndef NO_RESOLV_CONF
    /* Now try the resolver config file */
    if (!got_it && (fp = fopen(RESOLV_CONF, "r")) != NULL) {
	char buffer[80];
	*(buffer+79) = '\0';
	while (fgets(buffer, 79, fp)) {
	    if (!strncasecomp(buffer, "domain", 6)) {	
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
		    got_it = YES;
		    break;
		}
	    }
	}
	fclose(fp);
    }
#endif /* NO_RESOLV_CONF */

#ifndef NO_GETDOMAINNAME
    /* If everything else has failed then try getdomainname */
    if (!got_it) {
	if (getdomainname(name, MAXHOSTNAMELEN)) {
	    if (PROT_TRACE)
		fprintf(TDEST, "HostName.... Can't get domain name\n");
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
#endif /* NO_GETDOMAINNAME */

    {
	char *strptr = hostname;
	while (*strptr) {	    
	    *strptr = TOLOWER(*strptr);
	    strptr++;
	}
	if (*(hostname+strlen(hostname)-1) == '.')    /* Remove trailing dot */
	    *(hostname+strlen(hostname)-1) = '\0';
    }
#endif /* NO_GETHOSTNAME */

    if (PROT_TRACE)
	fprintf(TDEST, "HostName.... Full host name is `%s\'\n", hostname);
    return hostname;
}


/*
**	Free the host name. Called from HTLibTerminate
*/
PUBLIC void HTFreeHostName NOARGS
{
    FREE(hostname);
}


/*							       HTSetMailAddress
**	Sets the current mail address plus host name and domain name.
**	If this is not set then the default approach is used using
**	HTGetMailAddress(). If the argument is NULL or "" then HTGetMailAddress
**	returns NULL on a succeding request.
*/
PUBLIC void HTSetMailAddress ARGS1(char *, address)
{
    if (!address || !*address)
	StrAllocCopy(mailaddress, "");
    else
	StrAllocCopy(mailaddress, address);
    if (TRACE)
	fprintf(TDEST, "SetMailAdr.. Set mail address to `%s\'\n",
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
PUBLIC CONST char * HTGetMailAddress NOARGS
{
#ifdef HT_REENTRANT
    char name[LOGNAME_MAX+1];				   /* For getlogin_r */
#endif
    char *login;
    CONST char *domain;
    struct passwd *pw_info;
    if (mailaddress) {
	if (*mailaddress)
	    return mailaddress;
	else
	    return NULL;       /* No luck the last time so we wont try again */
    }

#ifdef VMS
    if ((login = (char *) cuserid(NULL)) == NULL) {
        if (PROT_TRACE) fprintf(TDEST, "MailAddress. cuserid returns NULL\n");
    }
#else
#ifdef WIN32 
    login = getenv("USERNAME") ;
#else 
#ifdef _WINDOWS
    login = "PCUSER";				  /* @@@ COULD BE BETTER @@@ */
#else
#ifdef GUSI
    if ((login = getenv("LOGNAME")) == NULL) 
	login = "MACUSER";
#else /* Unix like... */
#ifdef HT_REENTRANT
    if ((login = (char *) getlogin_r(name, LOGNAME_MAX)) == NULL) {
#else
    if ((login = (char *) getlogin()) == NULL) {
#endif
	if (PROT_TRACE)
	    fprintf(TDEST, "MailAddress. getlogin returns NULL\n");
	if ((pw_info = getpwuid(getuid())) == NULL) {
	    if (PROT_TRACE)
		fprintf(TDEST, "MailAddress. getpwid returns NULL\n");
	    if ((login = getenv("LOGNAME")) == NULL) {
		if (PROT_TRACE)
		    fprintf(TDEST, "MailAddress. LOGNAME not found\n");
		if ((login = getenv("USER")) == NULL) {
		    if (PROT_TRACE)
			fprintf(TDEST,"MailAddress. USER not found\n");
		    return NULL;		/* I GIVE UP */
		}
	    }
	} else
	    login = pw_info->pw_name;
    }
#endif /* GUSI */
#endif /* _WINDOWS */
#endif /* WIN32 */
#endif /* VMS */

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
PUBLIC void HTFreeMailAddress NOARGS
{
    FREE(mailaddress);
}


/* ------------------------------------------------------------------------- */
/*	       	      CONNECTION ESTABLISHMENT MANAGEMENT 		     */
/* ------------------------------------------------------------------------- */

/*								HTDoConnect()
**
**	Note: Any port indication in URL, e.g., as `host:port' overwrites
**	the default port value.
**
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_OK		if connected
**			HT_WOULD_BLOCK  if operation would have blocked
*/
PUBLIC int HTDoConnect (HTNet * net, char * url, u_short default_port)
{
    int status;
    char *p1 = HTParse(url, "", PARSE_HOST);
    char *at_sign;
    char *host;

    /* if there's an @ then use the stuff after it as a hostname */
    if ((at_sign = strchr(p1, '@')) != NULL)
	host = at_sign+1;
    else
	host = p1;
    if (!*host) {
	HTErrorAdd(net->request, ERR_FATAL, NO, HTERR_NO_HOST,
		   NULL, 0, "HTDoConnect");
	free(p1);
	return HT_ERROR;
    }

    /* Look for a port number, else use default port */
    if (net->sockfd == INVSOC) {
	char *port = strchr(host, ':');
	SockA *sin = &net->sock_addr;
	memset((void *) sin, '\0', sizeof(SockA));
	if (port++ && isdigit(*port)) {
#ifdef DECNET
	    sin->sdn_family = AF_DECnet;      /* Family = DECnet, host order */
	    sin->sdn_objnum = (unsigned char)(strtol(port, (char**)0, 10));
#else /* Internet */
	    sin->sin_family = AF_INET;
	    sin->sin_port = htons(atol(port));
#endif
	} else {
#ifdef DECNET
	    sin->sdn_family = AF_DECnet;      /* Family = DECnet, host order */
	    net->sock_addr.sdn_objnum = DNP_OBJ; /* Default: http object num */
#else  /* Internet */
	    sin->sin_family = AF_INET;
	    sin->sin_port = htons(default_port);
#endif
	}
    }

    /* If we are trying to connect to a multi-homed host then loop here until
       success or we have tried all IP-addresses */
    do {
	BOOL reuse = NO;
	if (net->sockfd==INVSOC) {
	    int homes;
	    if (PROT_TRACE)
		fprintf(TDEST, "HTDoConnect. Looking up `%s\'\n", host);
	    if ((homes = HTParseInet(net, host)) < 0) {
		if (PROT_TRACE)
		    fprintf(TDEST, "HTDoConnect. Can't locate remote host `%s\'\n", host);
		HTErrorAdd(net->request, ERR_FATAL, NO, HTERR_NO_REMOTE_HOST,
			   (void *) host, strlen(host), "HTDoConnect");
		break;
	    } else if (!homes)
		return HT_PERSISTENT;

	    if (net->sockfd != INVSOC) {
		reuse = YES;
		if (PROT_TRACE)
		    fprintf(TDEST, "HTDoConnect. REUSING SOCKET %d\n", net->sockfd);
		goto connect;
	    }
	    if (!net->retry && homes > 1)
		net->retry = homes;
#ifdef DECNET
	    if ((net->sockfd=socket(AF_DECnet, SOCK_STREAM, 0))==INVSOC)
#else
	    if ((net->sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVSOC)
#endif
	    {
		HTErrorSysAdd(net->request, ERR_FATAL, socerrno, NO, "socket");
		break;
	    }
	    if (PROT_TRACE)
		fprintf(TDEST, "HTDoConnect. Created socket number %d\n",
			net->sockfd);

	    /* If non-blocking protocol then change socket status
	    ** I use FCNTL so that I can ask the status before I set it.
	    ** See W. Richard Stevens (Advan. Prog. in UNIX environment, p.364)
	    ** Be CAREFULL with the old `O_NDELAY' - it will not work as read()
	    ** returns 0 when blocking and NOT -1. FNDELAY is ONLY for BSD and
	    ** does NOT work on SVR4 systems. O_NONBLOCK is POSIX.
	    */
	    if (!net->preemtive) {
#ifdef _WINDOWS 
		{		/* begin windows scope  */
		    HTRequest * rq = net->request;
		    long levents = FD_READ | FD_WRITE | FD_ACCEPT | 
			FD_CONNECT | FD_CLOSE ;
		    int rv = 0 ;
				    
#ifndef _WIN32 		
		    if (net->request->hwnd == 0) {
					
		    }
#endif 
		    /* N.B WSAAsyncSelect() turns on non-blocking I/O */

		    if (net->request->hwnd != 0) {
			rv = WSAAsyncSelect( net->sockfd, rq->hwnd, 
					    rq->winMsg, levents);
			if (rv == SOCKET_ERROR) {
			    status = -1 ;
			    if (PROT_TRACE) 
				fprintf(TDEST, 
					"HTDoConnect: WSAAsyncSelect() fails: %d\n", 
					WSAGetLastError());
			} /* error returns */
		    } else {
			int enable = 1 ;
			status = IOCTL(net->sockfd, FIONBIO, &enable);
		    }
		} /* end scope */
#else 
#if defined(VMS)
		{
		    int enable = 1;
		    status = IOCTL(net->sockfd, FIONBIO, &enable);
		}
#else
		if((status = FCNTL(net->sockfd, F_GETFL, 0)) != -1) {
		    status |= O_NONBLOCK; /* POSIX */
		    status = FCNTL(net->sockfd, F_SETFL, status);
		}
#endif /* VMS */
#endif /* WINDOW */
		if (PROT_TRACE) {
		    if (status == -1)
			fprintf(TDEST, "HTDoConnect. Can't make socket non-blocking\n");
		    else
			fprintf(TDEST, "HTDoConnect. Using NON_BLOCKING I/O\n");
		}
	    }
	    
	    /* If multi-homed host then start timer on connection */
	    if (net->retry)
		net->connecttime = time(NULL);

	    /* Update progress state */
	    HTProgress(net->request, HT_PROG_CONNECT, NULL);
	}

	/* Do a connect */
      connect:
	status = connect(net->sockfd, (struct sockaddr *) &net->sock_addr,
			 sizeof(net->sock_addr));
	/*
	 * According to the Sun man page for connect:
	 *     EINPROGRESS         The socket is non-blocking and the  con-
	 *                         nection cannot be completed immediately.
	 *                         It is possible to select(2) for  comple-
	 *                         tion  by  selecting the socket for writ-
	 *                         ing.
	 * According to the Motorola SVR4 man page for connect:
	 *     EAGAIN              The socket is non-blocking and the  con-
	 *                         nection cannot be completed immediately.
	 *                         It is possible to select for  completion
	 *                         by  selecting  the  socket  for writing.
	 *                         However, this is only  possible  if  the
	 *                         socket  STREAMS  module  is  the topmost
	 *                         module on  the  protocol  stack  with  a
	 *                         write  service  procedure.  This will be
	 *                         the normal case.
	 */
	
#ifdef EAGAIN
	if ((status < 0) && ((socerrno==EINPROGRESS) || (socerrno==EAGAIN)))
#else 
#ifdef WSAEWOULDBLOCK 	   /* WinSock API */
	if ((status == SOCKET_ERROR) && (socerrno == WSAEWOULDBLOCK))
#else
	if ((status < 0) && (socerrno == EINPROGRESS))
#endif /* WSAEWOULDBLOCK */
#endif /* EAGAIN */
	{
	    if (PROT_TRACE)
		fprintf(TDEST, "HTDoConnect. WOULD BLOCK `%s'\n", host);
	    HTEvent_Register(net->sockfd, net->request, (SockOps) FD_CONNECT,
			     net->cbf, net->priority);
	    free(p1);
	    return HT_WOULD_BLOCK;
	}
	
	/* We have 4 situations: single OK, Pb and multi OK, pb */
	if (net->retry) {
	    net->connecttime = time(NULL) - net->connecttime;
	    if (status < 0) {					 /* multi PB */
		if (socerrno == EISCONN) { /* connect multi after would block*/
		    HTEvent_UnRegister(net->sockfd, (SockOps) FD_CONNECT);
		    HTDNS_updateWeigths(net->dns, net->home, net->connecttime);
		    net->retry = 0;
		    free(p1);
		    return HT_OK;
		}
		if (reuse)
		    HTDNS_setSocket(net->dns, INVSOC);
		else  {
		    HTErrorSysAdd(net->request, ERR_NON_FATAL, socerrno, NO,
				  "connect");
		    
		    /* Added EINVAL `invalid argument' as this is what I 
		       get back from a non-blocking connect where I should 
		       get `connection refused' on BSD. SVR4 gives SIG_PIPE */
		    if (socerrno==ECONNREFUSED || socerrno==ETIMEDOUT ||
			socerrno==ENETUNREACH || socerrno==EHOSTUNREACH ||
#ifdef __srv4__
			socerrno==EHOSTDOWN || socerrno==EINVAL)
#else
			socerrno==EHOSTDOWN)
#endif
		        net->connecttime += TCP_DELAY;
		    else
		        net->connecttime += TCP_PENALTY;
	        }
	        if (NETCLOSE(net->sockfd) < 0)
		    HTErrorSysAdd(net->request, ERR_FATAL, socerrno, NO, 
				  "NETCLOSE");
	        HTEvent_UnRegister(net->sockfd, (SockOps) FD_ALL);
	        net->sockfd = INVSOC;
	        HTDNS_updateWeigths(net->dns, net->home, net->connecttime);
	    } else {						 /* multi OK */
		HTDNS_updateWeigths(net->dns, net->home, net->connecttime);
		net->retry = 0;
		free(p1);
		return HT_OK;
	    }
        } else if (status < 0) {				/* single PB */
	    if (socerrno==EISCONN) { 	 /* Connect single after would block */
		HTEvent_UnRegister(net->sockfd, (SockOps) FD_CONNECT);
		net->retry = 0;
		free(p1);
		return HT_OK;
	    } else {
		HTErrorSysAdd(net->request, ERR_FATAL, socerrno, NO,
			  "connect");
		HTDNS_delete(host);
		if (NETCLOSE(net->sockfd) < 0)
		    HTErrorSysAdd(net->request, ERR_FATAL, socerrno, NO,
				  "NETCLOSE");
	        HTEvent_UnRegister(net->sockfd, (SockOps) FD_ALL);
		break;
	    }
	} else {				  		/* single OK */
	    free(p1);
	    net->retry = 0;
	    return HT_OK;
	}
    } while (--net->retry);				 /* End of mega loop */

    if (PROT_TRACE)
        fprintf(TDEST, "HTDoConnect. Connect failed\n");
    free (p1);
    net->retry = 0;
    net->sockfd = INVSOC;
    return HT_ERROR;
}


/*								HTDoAccept()
**
**	This function makes a non-blocking accept on a port and polls every
**	second until MAX_ACCEPT_POLL or interrupted by user.
**
**	BUGS Interrupted is not yet implemented!!!
**
**	Returns  HT_WOULD_BLOCK 	if waiting
**		 0		if OK,
**		 -1		on error
*/
PUBLIC int HTDoAccept ARGS1(HTNet *, net)
{
    SockA soc_address;				/* SockA is defined in tcp.h */
    int status;
    int cnt;
    int soc_addrlen = sizeof(soc_address);
    if (net->sockfd==INVSOC) {
	if (PROT_TRACE) fprintf(TDEST, "HTDoAccept.. Bad socket number\n");
	return -1;
    }

    /* First make the socket non-blocking */
#if defined(_WINDOWS) || defined(VMS)
    {
	int enable = 1;		/* Need the variable! */
	status = IOCTL(net->sockfd, FIONBIO, enable);
    }
#else
    if((status = FCNTL(net->sockfd, F_GETFL, 0)) != -1) {
	status |= O_NONBLOCK;	/* POSIX */
	status = FCNTL(net->sockfd, F_SETFL, status);
    }
#endif
    if (status == -1) {
	HTErrorSysAdd(net->request, ERR_FATAL, socerrno, NO, "IOCTL");
	return -1;
    }

    /* Now poll every sekund */
    for(cnt=0; cnt<MAX_ACCEPT_POLL; cnt++) {
	if ((status = accept(net->sockfd, (struct sockaddr*) &soc_address,
			     &soc_addrlen)) != INVSOC) {
	    if (PROT_TRACE) fprintf(TDEST,
			       "HTDoAccept.. Accepted new socket %d\n",	
			       status);
	    return status;
	} else
	    HTErrorSysAdd(net->request, ERR_WARN, socerrno, YES, "accept");
	SLEEP(1);
    }
    
    /* If nothing has happened */    
    if (PROT_TRACE)
	fprintf(TDEST, "HTDoAccept.. Timed out, no connection!\n");
    HTErrorAdd(net->request, ERR_FATAL, NO, HTERR_TIME_OUT, NULL, 0,
	       "HTDoAccept");
    return -1;
}


