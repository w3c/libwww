/*									HTTCP.c
**	GENERIC COMMUNICATION CODE
**
**	(c) COPYRIGHT CERN 1994.
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
#include "HTAccess.h"
#include "HTError.h"
#include "HTThread.h"
#include "HTTCP.h"					 /* Implemented here */

#ifdef VMS 
#include "HTVMSUtils.h"
#endif /* VMS */

#ifdef SHORT_NAMES
#define HTInetStatus		HTInStat
#define HTErrnoString		HTErrnoS
#define HTInetString 		HTInStri
#define HTParseInet		HTPaInet
#endif


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
#define FCNTL(r, s, t)		fcntl(r, s, t)

#ifndef RESOLV_CONF
#define RESOLV_CONF "/etc/resolv.conf"
#endif

/* Globals */
PUBLIC unsigned int	HTConCacheSize = 512;	 /* Number of cached servers */

/* Type definitions and global variables etc. local to this module */

/* This structure is a cache of hosts to whom we have connected over time.
   The structure contains the necessary parts from hostent. For Internet host
   hostent->h_addr_list is not an array of char pointers but an array of 
   pointers of type in_addr. */
typedef struct _host_info {
    HTAtom *		hostname;		    /* Official name of host */
    int			hits;		/* Total number of hits on this host */
    int			addrlength;	       /* Length of address in bytes */
    int			homes;	       /* Number of IP addresses on the host */
    int			offset;         /* Offset value of active IP address */
    char **		addrlist;      /* List of addresses from name server */
    double *		weight;			   /* Weight on each address */
} host_info;

PRIVATE char *hostname = NULL;			    /* The name of this host */
PRIVATE char *mailaddress = NULL;		     /* Current mail address */
PRIVATE HTList *hostcache = NULL;  /* List of servers that we have talked to */
PRIVATE unsigned int HTCacheSize = 0;		    /* Current size of cache */

/* ------------------------------------------------------------------------- */

/*
**	Returns the string equivalent to the errno passed in the argument.
**	We can't use errno directly as we have both errno and socerrno. The
**	result is a static buffer.
*/
PUBLIC CONST char * HTErrnoString ARGS1(int, errornumber)
{
#if defined(NeXT) || defined(THINK_C)
    return strerror(errornumber);
#else
#ifdef VMS
    static char buf[60];
    sprintf(buf,"Unix errno = %ld dec, VMS error = %lx hex", errornumber,
	    vaxc$errno);
    return buf;
#else
    return (errornumber < sys_nerr ? sys_errlist[errornumber]:"Unknown error");
#endif
#endif
}


#ifdef OLD_CODE
/*	Debug error message
*/
PUBLIC int HTInetStatus ARGS1(char *, where)
{
#ifndef VMS

    if (PROT_TRACE)
	fprintf(TDEST, "TCP errno... %d after call to %s() failed.\n............ %s\n", errno, where, HTErrnoString(errornumber));

#else /* VMS */

    if (PROT_TRACE) fprintf(TDEST, "         Unix error number          = %ld dec\n", errno);
    if (PROT_TRACE) fprintf(TDEST, "         VMS error                  = %lx hex\n", vaxc$errno);

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
#endif


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
/*	       		     HOST CACHE MANAGEMENT 			     */
/* ------------------------------------------------------------------------- */

/*                                                     	HTTCPCacheRemoveElement
**
**	Remove the element specified from the cache
*/
PRIVATE void HTTCPCacheRemoveElement ARGS1(host_info *, element)
{
    if (!hostcache) {
        if (PROT_TRACE)
            fprintf(TDEST, "HostCache... Remove not done, no cache\n");
        return;
    }
    if (PROT_TRACE) fprintf(TDEST, "HostCache... Remove `%s' from cache\n",
			    HTAtom_name(element->hostname));
    HTList_removeObject(hostcache, (void *) element);
    if (*element->addrlist)
	free(*element->addrlist);
    if (element->addrlist)
	free(element->addrlist);
    if (element->weight)
	free(element->weight);
    free(element);
}


/*                                                     	HTTCPCacheRemoveAll
**
**	Cleans up the memory. Called by HTLibTerminate
*/
PUBLIC void HTTCPCacheRemoveAll NOARGS
{
    if (hostcache) {
	HTList *cur = hostcache;
	host_info *pres;
	while ((pres = (host_info *) HTList_nextObject(cur))) {
	    if (*pres->addrlist)
		free(*pres->addrlist);
	    if (pres->addrlist)
		free(pres->addrlist);
	    if (pres->weight)
		free(pres->weight);
	    free(pres);
	}
	HTList_delete(hostcache);
	hostcache = NULL;
    }
}


/*                                                     HTTCPCacheRemoveHost
**
**	Removes the corresponding entrance in the cache
*/
PRIVATE void HTTCPCacheRemoveHost ARGS1(char *, host)
{
    HTAtom *hostatom = HTAtom_for(host);
    HTList *cur = hostcache;
    host_info *pres = NULL;
    if (!hostcache) {
	if (PROT_TRACE)
	    fprintf(TDEST, "HostCache... Remove host not done, no cache\n");
	return;
    }
    while ((pres = (host_info *) HTList_nextObject(cur)) != NULL) {
	if (pres->hostname == hostatom) {
	    break;
	}
    }
    if (pres)
	HTTCPCacheRemoveElement(pres);
}


/*                                                     HTTCPCacheGarbage
**
**	Remove the element with the lowest hit rate
*/
PRIVATE void HTTCPCacheGarbage NOARGS
{
    HTList *cur = hostcache;
    host_info *pres, *worst_match = NULL;
    unsigned int worst_hits = 30000;		  /* Should use UINT_MAX :-( */
    if (!hostcache) {
	if (PROT_TRACE)
	    fprintf(TDEST, "HostCache... Garbage collection not done, no cache\n");
	return;
    }

    /* Seek for worst element */
    while ((pres = (host_info *) HTList_nextObject(cur))) {
	if (!worst_match || pres->hits <= worst_hits) {
	    worst_match = pres;
	    worst_hits = pres->hits;
	}
    }
    if (worst_match)
	HTTCPCacheRemoveElement(worst_match);
}


/*                                                     HTTCPCacheAddElement
**
**	Add an element to the cache of visited hosts. Note that this function
**	requires the system implemented structure hostent and not our own
**	host_info. The homes variable indicates the number of
**	IP addresses found.
**
**      Returns new element if OK NULL if error
*/
PRIVATE host_info *HTTCPCacheAddElement ARGS2(HTAtom *, host,
					      struct hostent *, element)
{
    host_info *newhost;
    char *addr;
    char **index = element->h_addr_list;
    int cnt = 1;
    if (!host || !element) {
	if (PROT_TRACE)
	    fprintf(TDEST, "HostCache... Bad argument to add to cache\n");
	return NULL;
    }
    while(*index++)
	cnt++;
    if ((newhost = (host_info *) calloc(1, sizeof(host_info))) == NULL ||
	(newhost->addrlist = (char **) calloc(1, cnt*sizeof(char*))) == NULL ||
	(addr = (char *) calloc(1, cnt*element->h_length)) == NULL)
	outofmem(__FILE__, "HTTCPCacheAddElement");
    newhost->hostname = host;
    index = element->h_addr_list;
    cnt = 0;
    while (*index) {
	*(newhost->addrlist+cnt) = addr+cnt*element->h_length;
	memcpy((void *) *(newhost->addrlist+cnt++), *index++,
	       element->h_length);
    }
    newhost->homes = cnt;
    if ((newhost->weight = (double *) calloc(newhost->homes,
					    sizeof(double))) == NULL)
	outofmem(__FILE__, "HTTCPCacheAddElement");

    newhost->addrlength = element->h_length;
    if (!hostcache)
	hostcache = HTList_new();

    if (PROT_TRACE) {
	if (newhost->homes == 1)
	    fprintf(TDEST, "HostCache... Adding single-homed host `%s'\n",
		    HTAtom_name(host));
	else
	    fprintf(TDEST, "HostCache... Adding host `%s' with %d homes\n",
		    HTAtom_name(host), newhost->homes);
    }
    HTList_addObject(hostcache, (void *) newhost);
    HTCacheSize++;				/* Update number of elements */
    return newhost;
}


/*                                                     	      HTTCPAddrWeights
**
**	This function calculates the weights of the different IP addresses
**	on a multi homed host. Each weight is calculated as
**
**		w(n+1) = w(n)*a + (1-a) * deltatime
**		a = exp(-1/Neff)
**		Neff is the effective number of samples used
**		deltatime is time spend on making a connection
**
**	A short window (low Neff) gives a high sensibility, but this is
**	required as we can't expect a lot of data to test on.
**
*/
PUBLIC void HTTCPAddrWeights ARGS2(char *, host, time_t, deltatime)
{
    HTAtom *hostatom = HTAtom_for(host);
    HTList *cur = hostcache;
    host_info *pres = NULL;
    if (!hostcache) {
	fprintf(TDEST, "HostCache... Weights not calculated, no cache\n");
	return;
    }
    /* Skip any port number from host name */
    if (strchr(host, ':')) {
	char *newhost = NULL;
	char *strptr;
	StrAllocCopy(newhost, host);
	strptr = strchr(newhost, ':');
	*strptr = '\0';
	hostatom = HTAtom_for(newhost);
	free(newhost);
    } else
	hostatom = HTAtom_for(host);
    
    while ((pres = (host_info *) HTList_nextObject(cur)) != NULL) {
	if (pres->hostname == hostatom) {
	    break;
	}
    }
    if (pres) {
	int cnt;
	CONST double passive = 0.9; 	  /* Factor for all passive IP_addrs */
#if 0
	CONST int Neff = 3;
	CONST double alpha = exp(-1.0/Neff);
#else
	CONST double alpha = 0.716531310574;	/* Doesn't need the math lib */
#endif
	for (cnt=0; cnt<pres->homes; cnt++) {
	    if (cnt == pres->offset) {
		*(pres->weight+pres->offset) = *(pres->weight+pres->offset)*alpha + (1.0-alpha)*deltatime;
	    } else {
		*(pres->weight+cnt) = *(pres->weight+cnt) * passive;
	    }
	    if (PROT_TRACE)
		fprintf(TDEST, "AddrWeights. Home %d has weight %4.2f\n", cnt,
			*(pres->weight+cnt));
	}
    } else if (PROT_TRACE) {
	fprintf(TDEST, "HostCache... Weights not calculated, host not found in cache: `%s\'\n", host);
    }
}

/* ------------------------------------------------------------------------- */
/*	       		     HOST NAME FUNCTIONS 			     */
/* ------------------------------------------------------------------------- */

#ifndef DECNET  /* Function only used below for a trace message */

/*	Produce a string for an Internet address
**	----------------------------------------
**
** On exit,
**	returns	a pointer to a static string which must be copied if
**		it is to be kept.
*/

PUBLIC CONST char * HTInetString ARGS1(SockA *, sin)
{
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
}
#endif /* Decnet */


/*	                                                     HTGetHostByName
**
**	Searched first the local cache then asks the DNS for an address of
**	the host specified.
**
**      Returns:	>0 if OK the number of homes are returned
**		     	-1 if error
*/
PUBLIC int HTGetHostByName ARGS3(char *, host, SockA *, sin,
				 BOOL, use_cur)
{
    HTAtom *hostatom = HTAtom_for(host);
    host_info *pres = NULL;
    if (!hostcache)
	hostcache = HTList_new(); 	     	       /* First time through */
    else {
	HTList *cur = hostcache;       		 	     /* Search cache */
	while ((pres = (host_info *) HTList_nextObject(cur)) != NULL) {
	    if (pres->hostname == hostatom) {
		if (PROT_TRACE)
		    fprintf(TDEST, "HostByName.. Host `%s\' found in cache.\n", host);
		break;
	    }
	}
    }
    
    /* If the host was not found in the cache, then do gethostbyname.
       If we are talking to a multi homed host then take the IP address with
       the lowest weight. If `use_cur'=YES then use current IP-address */
    if (pres) {
	if (pres->homes > 1 && !use_cur) {
	    int cnt;
	    double best_weight = 1e30;		    /* Should be FLT_MAX :-( */
	    for (cnt=0; cnt<pres->homes; cnt++) {
		if (*(pres->weight+cnt) < best_weight) {
		    best_weight = *(pres->weight+cnt);
		    pres->offset = cnt;
		}
	    }
	}
	pres->hits++;	 	 /* Update total number of hits on this host */
    } else {						/* Go and ask for it */
	struct hostent *hostelement;			      /* see netdb.h */
#ifdef MVS	/* Outstanding problem with crash in MVS gethostbyname */
	if (PROT_TRACE)
	    fprintf(TDEST, "HTTCP on MVS gethostbyname(%s)\n", host);
#endif
	if ((hostelement = gethostbyname(host)) == NULL) {
	    if (PROT_TRACE)
		fprintf(TDEST, "HostByName.. Can't find internet node name `%s'.\n", host);
	    return -1;
	}
	
	/* Add element to the cache and maybe do garbage collection */
	if (HTCacheSize >= HTConCacheSize)
	    HTTCPCacheGarbage();
	if ((pres = HTTCPCacheAddElement(hostatom, hostelement)) == NULL) {
	    return -1;
	}
    }
    
    /* Update socket structure using the element with the lowest weight. On
       single homed hosts it means the first value */
    memcpy(&sin->sin_addr, *(pres->addrlist+pres->offset), pres->addrlength);
    return pres->homes;
}


/*
**	Get host name of the machine on the other end of a socket.
**
*/
PUBLIC char * HTGetHostBySock ARGS1(int, soc)
{
    struct sockaddr addr;
    int len = sizeof(struct sockaddr);
    struct in_addr *iaddr;
    struct hostent * phost;		/* Pointer to host -- See netdb.h */
    char *name = NULL;

#ifdef DECNET  /* Decnet ain't got no damn name server 8#OO */
    return NULL;
#else
    if (getpeername(soc, &addr, &len) < 0)
	return NULL;

    iaddr = &(((struct sockaddr_in *)&addr)->sin_addr);
    phost=gethostbyaddr((char*)iaddr,
			sizeof(struct in_addr),
			AF_INET);
    if (!phost) {
	if (PROT_TRACE)
	    fprintf(TDEST, "TCP......... Can't find internet node name for peer!!\n");
	return NULL;
    }
    StrAllocCopy(name, phost->h_name);
    if (PROT_TRACE) fprintf(TDEST, "TCP......... Peer name is `%s'\n", name);

    return name;

#endif	/* not DECNET */
}


/*	Parse a network node address and port
**	-------------------------------------
**
** On entry,
**	str	points to a string with a node name or number,
**		with optional trailing colon and port number.
**	sin	points to the binary internet or decnet address field.
**
** On exit,	-1	If error
**		>0	If OK the number of homes on the host
**	*sin	is filled in. If no port is specified in str, that
**		field is left unchanged in *sin.
**
** NOTE:	It is assumed that any portnumber and numeric host address
**		is given in decimal notation. Separation character is '.'
*/
PUBLIC int HTParseInet ARGS3(SockA *, sin, CONST char *, str,
			     BOOL, use_cur)
{
    char *host = NULL;
    int status = 0;
    StrAllocCopy(host, str);		      /* Take a copy we can mutilate */

    /* Parse port number if present. */    
    {
	char *port;
	if ((port=strchr(host, ':'))) {
	    *port++ = 0;			       	    /* Chop off port */
	    if (isdigit(*port)) {

#ifdef DECNET
		sin->sdn_objnum = (unsigned char)(strtol(port, (char**)0, 10));
#else /* Internet */
		sin->sin_port = htons(atol(port));
#endif
	    } else {
		if (PROT_TRACE)
		    fprintf(TDEST, "ParseInet... No port indicated\n");
		free(host);
		return -1;
	    }
	}
    }

    /* Parse Internet host */
#ifdef DECNET
    /* read Decnet node name. @@ Should know about DECnet addresses, but it's
       probably worth waiting until the Phase transition from IV to V. */

    sin->sdn_nam.n_len = min(DN_MAXNAML, strlen(host));  /* <=6 in phase 4 */
    strncpy (sin->sdn_nam.n_name, host, sin->sdn_nam.n_len + 1);

    if (PROT_TRACE) fprintf(TDEST,  
	"DECnet: Parsed address as object number %d on host %.6s...\n",
		      sin->sdn_objnum, host);

#else /* Internet */

    /*	Parse host number if present */
    {
	BOOL numeric = YES;
	char *strptr = host;
	while (*strptr) {
	    if (!isdigit(*strptr) && *strptr != '.') {
		numeric = NO;
		break;
	    }
	    ++strptr;
	}
	if (numeric) {
	    sin->sin_addr.s_addr = inet_addr(host);	  /* See arpa/inet.h */
	} else {
	    if ((status = HTGetHostByName(host, sin, use_cur)) < 0) {
		free(host);
		return -1;
	    }
	}
	if (PROT_TRACE) {
	    fprintf(TDEST, "ParseInet... Parsed address as port %d on %s\n",
		    (int) ntohs(sin->sin_port),
		    HTInetString(sin));
	}
    }
#endif  /* Internet vs. Decnet */
    free(host);
    return status;
}


#ifdef OLD_CODE
/*	Derive the name of the host on which we are
**	-------------------------------------------
**
*/
PRIVATE void get_host_details NOARGS

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64		/* Arbitrary limit */
#endif

{
    char name[MAXHOSTNAMELEN+1];	/* The name of this host */
    struct hostent * phost;		/* Pointer to host -- See netdb.h */
    int namelength = sizeof(name);
    
    if (hostname) return;		/* Already done */
    gethostname(name, namelength);	/* Without domain */
    if (PROT_TRACE) fprintf(TDEST, "TCP......... Local host name is %s\n", name);
    StrAllocCopy(hostname, name);

#ifndef DECNET  /* Decnet ain't got no damn name server 8#OO */
    phost=gethostbyname(name);		/* See netdb.h */
    if (!phost) {
	if (PROT_TRACE) fprintf(TDEST, 
		"TCP......... Can't find my own internet node address for `%s'!!\n",
		name);
	return;  /* Fail! */
    }
    StrAllocCopy(hostname, phost->h_name);
    if (PROT_TRACE)
	fprintf(TDEST, "TCP......... Full local host name is %s\n", hostname);

#ifdef NEED_HOST_ADDRESS		/* no -- needs name server! */
    memcpy(&HTHostAddress, &phost->h_addr, phost->h_length);
    if (PROT_TRACE) fprintf(TDEST, "     Name server says that I am `%s' = %s\n",
	    hostname, HTInetString(&HTHostAddress));
#endif /* NEED_HOST_ADDRESS */

#endif /* not Decnet */
}
#endif /* OLD_CODE */


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
**		If there is no domain line in this file, then it is derived
**		from the domain name set by the domainname(1) command, usually
**		by removing the first component. For example, if the domain-
**		name is set to ``foo.podunk.edu'' then the default domain name
**		used will be ``pudunk.edu''.
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

#ifndef VMS
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

    /* If everything else has failed then try getdomainname */
#ifndef NO_GETDOMAINNAME
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
	    if (domain)
		StrAllocCat(hostname, domain);
	}
    }
#endif /* NO_GETDOMAINNAME */
#endif /* not VMS */

    {
	char *strptr = hostname;
	while (*strptr) {	    
	    *strptr = TOLOWER(*strptr);
	    strptr++;
	}
	if (*(hostname+strlen(hostname)-1) == '.')    /* Remove trailing dot */
	    *(hostname+strlen(hostname)-1) = '\0';
    }
    if (PROT_TRACE)
	fprintf(TDEST, "HostName.... Full host name is `%s\'\n", hostname);
    return hostname;

#ifndef DECNET  /* Decnet ain't got no damn name server 8#OO */
#ifdef OLD_CODE
			      /* Now we try to get information on the domain */
    {
	struct hostent *hostelement;
	if ((hostelement = gethostbyname(hostname)) == NULL) {
	    if (PROT_TRACE)
		fprintf(TDEST, "HostName.... Can't find host name on DNS\n");
	    FREE(hostname);
	    return NULL;
	}
	StrAllocCopy(hostname, (char *) hostelement->h_name);
    }
#endif /* OLD_CODE */
#endif /* not Decnet */
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
#ifdef _WINDOWS
    login = "PCUSER";				  /* @@@ COULD BE BETTER @@@ */
#else /* Unix like... */
    if ((login = (char *) getlogin()) == NULL) {
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
#endif
#endif

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
**	the default_port value.
**
**	Returns 0 if OK, -1 on error
*/
PUBLIC int HTDoConnect ARGS5(HTNetInfo *, net, char *, url,
			     u_short, default_port, u_long *, addr,
			     BOOL, use_cur)
{
    int status;
    char *p1 = HTParse(url, "", PARSE_HOST);
    char *at_sign;
    char *host;

    /* if theres an @ then use the stuff after it as a hostname */
    if((at_sign = strchr(p1, '@')) != NULL)
	host = at_sign+1;
    else
	host = p1;
    if (!*host) {
	HTErrorAdd(net->request, ERR_FATAL, NO, HTERR_NO_HOST,
		   NULL, 0, "HTDoConnect");
	free(p1);
	return -1;
    } else {
	if (PROT_TRACE)
	    fprintf(TDEST, "HTDoConnect. Looking up `%s\'\n", host);
    }

   /* Set up defaults */
    if (net->sockfd==INVSOC) {
	memset((void *) &net->sock_addr, '\0', sizeof(net->sock_addr));
#ifdef DECNET
	net->sock_addr.sdn_family = AF_DECnet;/* Family = DECnet, host order */
	net->sock_addr.sdn_objnum = DNP_OBJ;  /* Default: http object number */
#else  /* Internet */
	net->sock_addr.sin_family = AF_INET;
	net->sock_addr.sin_port = htons(default_port);
#endif
    }

    /* If we are trying to connect to a multi-homed host then loop here until
       success or we have tried all IP-addresses */
    do {
	if (net->sockfd==INVSOC) {
	    int hosts;
	    if ((hosts = HTParseInet(&net->sock_addr, host, use_cur)) < 0) {
		if (PROT_TRACE)
		    fprintf(TDEST, "HTDoConnect. Can't locate remote host `%s\'\n", host);
		HTErrorAdd(net->request, ERR_FATAL, NO, HTERR_NO_REMOTE_HOST,
			   (void *) host, strlen(host), "HTDoConnect");
		break;
	    };
	    if (!net->addressCount && hosts > 1)
		net->addressCount = hosts;
#ifdef DECNET
	    if ((net->sockfd=socket(AF_DECnet, SOCK_STREAM, 0))==INVSOC)
#else
	    if ((net->sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVSOC)
#endif
	    {
		HTErrorSysAdd(net->request, ERR_FATAL, socerrno, NO, "socket");
		break;
	    }
	    if (addr)
		*addr = ntohl(net->sock_addr.sin_addr.s_addr);
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
	    if (!HTProtocolBlocking(net->request)) {
#if defined(_WINDOWS) || defined(VMS)
		{
		    int enable = 1;
		    status = IOCTL(net->sockfd, FIONBIO, &enable);
		}
#else
		if((status = FCNTL(net->sockfd, F_GETFL, 0)) != -1) {
		    status |= O_NONBLOCK;			    /* POSIX */
		    status = FCNTL(net->sockfd, F_SETFL, status);
		}
#endif
		if (status == -1) {
		    if (PROT_TRACE)
			fprintf(TDEST, "HTDoConnect. Can NOT make socket non-blocking\n");
		} else if (PROT_TRACE)
		    fprintf(TDEST, "HTDoConnect. Using NON_BLOCKING I/O\n");
	    }
	    
	    /* If multi-homed host then start timer on connection */
	    if (net->addressCount >= 1)
		net->connecttime = time(NULL);
	}

	/* Check for interrupt */
	if (HTThreadIntr(net->sockfd)) {
	    if (NETCLOSE(net->sockfd) < 0)
		HTErrorSysAdd(net->request, ERR_FATAL, socerrno,NO,"NETCLOSE");
	    HTThreadState(net->sockfd, THD_CLOSE);
	    net->sockfd = INVSOC;
	    free(p1);
	    return HT_INTERRUPTED;
	}

	/* Do a connect */
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
	if ((status < 0) && (socerrno == EINPROGRESS))
#endif /* EAGAIN */
	{
	    if (PROT_TRACE)
		fprintf(TDEST, "HTDoConnect. WOULD BLOCK `%s'\n", host);
	    HTThreadState(net->sockfd, THD_SET_WRITE);
	    free(p1);
	    return HT_WOULD_BLOCK;
	}
	if (net->addressCount >= 1) {
	    net->connecttime = time(NULL) - net->connecttime;
	    if (status < 0) {
		if (socerrno==EISCONN) {  /* connect multi after would block */
		    HTThreadState(net->sockfd, THD_CLR_WRITE);
		    HTTCPAddrWeights(host, net->connecttime);
		    free(p1);
		    net->addressCount = 0;
		    return 0;
		}
		HTErrorSysAdd(net->request, ERR_NON_FATAL, socerrno, NO,
			      "connect");

		/* I have added EINVAL `invalid argument' as this is what I 
		   get back from a non-blocking connect where I should 
		   get `connection refused' on SVR4 */
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
		if (NETCLOSE(net->sockfd) < 0)
		    HTErrorSysAdd(net->request, ERR_FATAL, socerrno, NO, 
				  "NETCLOSE");
		HTThreadState(net->sockfd, THD_CLOSE);
		net->sockfd = INVSOC;
		HTTCPAddrWeights(host, net->connecttime);
	    } else {				   /* Connect on multi-homed */
		HTTCPAddrWeights(host, net->connecttime);
		free(p1);
		net->addressCount = 0;
		return 0;
	    }
	} else if (status < 0) {
	    if (socerrno==EISCONN) {     /* Connect single after would block */
		HTThreadState(net->sockfd, THD_CLR_WRITE);
		net->addressCount = 0;
		free(p1);
		return 0;
	    } else {
		HTErrorSysAdd(net->request, ERR_FATAL, socerrno, NO,
			      "connect");
		HTTCPCacheRemoveHost(host);
		if (NETCLOSE(net->sockfd) < 0)
		    HTErrorSysAdd(net->request, ERR_FATAL, socerrno, NO,
				  "NETCLOSE");
		HTThreadState(net->sockfd, THD_CLOSE);
		break;
	    }
	} else {				  /* Connect on single homed */
	    free(p1);
	    net->addressCount = 0;
	    return 0;
	}
    } while (--net->addressCount);

    if (PROT_TRACE)
	fprintf(TDEST, "HTDoConnect. Connect failed\n");
    free (p1);
    net->addressCount = 0;
    net->sockfd = INVSOC;
    return -1;
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
PUBLIC int HTDoAccept ARGS1(HTNetInfo *, net)
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


