/*			Generic Communication Code		HTTCP.c
**			==========================
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

#include "tcp.h"		/* Defines SHORT_NAMES if necessary */
#include "HTUtils.h"
#include "HTAtom.h"
#include "HTList.h"
#include "HTParse.h"
#include "HTAccess.h"
#include "HTError.h"

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
#define MAX_ACCEPT_POLL		30
#define FCNTL(r, s, t)		fcntl(r, s, t)

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
    int			multihomed;    /* Number of IP addresses on the host */
    int			offset;         /* Offset value of active IP address */
    char **		addrlist;      /* List of addresses from name server */
    float *		weight;			   /* Weight on each address */
} host_info;

PRIVATE char *hostname = NULL;			    /* The name of this host */
PRIVATE HTList *hostcache = NULL;  /* List of servers that we have talked to */
PRIVATE unsigned int HTCacheSize = 0;		    /* Current size of cache */

/* ------------------------------------------------------------------------- */

/*	Encode INET status (as in sys/errno.h)			  inet_status()
**	------------------
**
** On entry,
**	where		gives a description of what caused the error
**	global errno	gives the error number in the unix way.
**
** On return,
**	returns		a negative status in the unix way.
*/
#ifndef PCNFS
#ifdef VMS
#ifndef __DECC
extern int uerrno;	/* Deposit of error info (as per errno.h) */
extern volatile noshare int socket_errno; /* socket VMS error info 
                                             (used for translation of vmserrno) */
extern volatile noshare int vmserrno;	/* Deposit of VMS error info */
extern volatile noshare int errno;  /* noshare to avoid PSECT conflict */
#endif /* not DECC */
#endif /* VMS */

#ifndef errno
extern int errno;
#endif /* errno */

#ifndef VM
#ifndef VMS
#ifndef NeXT
#ifndef THINK_C
extern char *sys_errlist[];		/* see man perror on cernvax */
extern int sys_nerr;
#endif  /* think c */
#endif	/* NeXT */
#endif  /* VMS */
#endif	/* VM */

#endif	/* PCNFS */


/*
 *	Returns the string equivalent of the current errno.
 */
PUBLIC CONST char * HTErrnoString NOARGS
{
#ifndef VMS

#ifdef VM
    return "(Error number not translated)";	/* What Is the VM equiv? */
#define ER_NO_TRANS_DONE
#endif

#if defined(NeXT) || defined(THINK_C)
    return strerror(errno);
#define ER_NO_TRANS_DONE
#endif

#ifndef ER_NO_TRANS_DONE
    return (errno < sys_nerr ? sys_errlist[errno] : "Unknown error");
#endif

#else /* VMS */

    static char buf[60];
    sprintf(buf,"Unix errno = %ld dec, VMS error = %lx hex",errno,vaxc$errno);
    return buf;

#endif
}


/*	Report Internet Error
**	---------------------
*/
PUBLIC int HTInetStatus ARGS1(char *, where)
{
#ifndef VMS

    CTRACE(tfp,
	   "TCP errno... %d after call to %s() failed.\n............ %s\n",
	   errno, where, HTErrnoString());

#else /* VMS */

    CTRACE(tfp, "         Unix error number          = %ld dec\n", errno);
    CTRACE(tfp, "         VMS error                  = %lx hex\n", vaxc$errno);

#ifdef MULTINET
    CTRACE(tfp, "         Multinet error             = %lx hex\n", socket_errno); 
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
    int   n;
    if ( (**pp<'0') || (**pp>'9')) {	    /* Null string is error */
	*pstatus = -3;  /* No number where one expeceted */
	return 0;
    }

    n=0;
    while ((**pp>='0') && (**pp<='9')) n = n*10 + *((*pp)++) - '0';

    if (n>max_value) {
	*pstatus = -4;  /* Cardinal outside range */
	return 0;
    }

    return n;
}


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
    return inet_ntoa(sin->sin_addr);
#ifdef OLD_CODE
    static char string[16];
    sprintf(string, "%d.%d.%d.%d",
	    (int)*((unsigned char *)(&sin->sin_addr)+0),
	    (int)*((unsigned char *)(&sin->sin_addr)+1),
	    (int)*((unsigned char *)(&sin->sin_addr)+2),
	    (int)*((unsigned char *)(&sin->sin_addr)+3));
#endif
}
#endif /* Decnet */


/*                                                     HTTCPCacheRemoveElement
**
**	Remove the element specified from the cache
*/
PRIVATE void HTTCPCacheRemoveElement ARGS1(host_info *, element)
{
    if (!hostcache) {
        if (TRACE)
            fprintf(stderr, "HostCache... Remove not done, no cache\n");
        return;
    }
    if (TRACE) fprintf(stderr, "HostCache... Remove `%s' from cache\n",
		       HTAtom_name(element->hostname));
    HTList_removeObject(hostcache, element);
    if (element->addrlist)
	free(element->addrlist);
    if (element->weight)
	free(element->weight);
    free(element);
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
	fprintf(stderr, "HostCache... Remove host not done, no cache\n");
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
	if (TRACE)
	    fprintf(stderr, "HostCache... Garbage collection not done, no cache\n");
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
**	host_info. If multihomed host then multihome indicates the number of
**	IP addresses found. If not then multihome=0
**
**      Returns new element if OK NULL if error
*/
PRIVATE host_info *HTTCPCacheAddElement ARGS2(HTAtom *, host,
					      struct hostent *, element)
{
    int homes;				      /* Number of homes on the host */
    host_info *new;
    if (!host || !element) {
	if (TRACE)
	    fprintf(stderr, "HostCache... Bad argument to add to cache\n");
	return NULL;
    }
    if ((new = (host_info *) calloc(1, sizeof(host_info))) == NULL ||
	(new->addrlist = (char **)
	 calloc(1, sizeof element->h_addr_list)) == NULL)
	outofmem(__FILE__, "HTParseInet");
    new->hostname = host;

    if ((homes = (sizeof element->h_addr_list) / element->h_length) > 1) {
	new->multihomed = homes;
	if ((new->weight = (float *) calloc(homes, sizeof(float))) == NULL)
	    outofmem(__FILE__, "HTTCPCacheAddElement");
    }
    new->addrlength = element->h_length;
    memcpy((void *) new->addrlist, element->h_addr_list,
	   sizeof element->h_addr_list);
    if (!hostcache)
	hostcache = HTList_new();

    if (TRACE) {
	if (new->multihomed)
	    fprintf(stderr, "HostCache... Adding multihomed host `%s' having %d homes\n", HTAtom_name(host), new->multihomed);
	else
	    fprintf(stderr, "HostCache... Adding `%s'\n", HTAtom_name(host));
    }
    HTList_addObject(hostcache, (void *) new);
    HTCacheSize++;				/* Update number of elements */
    return new;
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
	fprintf(stderr, "HostCache... Weights not calculated, no cache\n");
	return;
    }
    while ((pres = (host_info *) HTList_nextObject(cur)) != NULL) {
	if (pres->hostname == hostatom) {
	    break;
	}
    }
    if (pres && pres->multihomed) {
	int cnt;
	CONST float passive = 0.9; 	  /* Factor for all passive IP_addrs */
#if 0
	CONST int Neff = 3;
	CONST float alpha = exp(-1.0/Neff);
#else
	CONST float alpha = 0.716531310574;
#endif
	for (cnt=0; cnt<pres->multihomed; cnt++) {
	    if (cnt == pres->offset) {
		*(pres->weight+pres->offset) = *(pres->weight+pres->offset)*alpha + (1.0-alpha)*deltatime;
	    } else {
		*(pres->weight+cnt) = *(pres->weight+cnt) * passive;
	    }
	}
    } else if (TRACE) {
	fprintf(stderr, "HostCache... Weights not calculated, host not found in cache or is not multihomed: `%s\'\n", host);
    }
}


/*	                                                     HTGetHostByName
**
**	Searched first the local cache then asks the DNS for an address of
**	the host specified.
**
**      Returns 0 if OK else -1
*/
PUBLIC int HTGetHostByName ARGS3(char *, host, SockA *, sin, BOOL *, multi)
{
    HTAtom *hostatom = HTAtom_for(host);
    host_info *pres = NULL;
    if (!hostcache)
	hostcache = HTList_new(); 	     	       /* First time through */
    else {
	HTList *cur = hostcache;       		 	     /* Search cache */
	while ((pres = (host_info *) HTList_nextObject(cur)) != NULL) {
	    if (pres->hostname == hostatom) {
		if (TRACE)
		    fprintf(stderr, "ParseInet... Host `%s\' found in cache.\n", host);
		break;
	    }
	}
    }
    
    /* If the host was not found in the cache, then do gethostbyname.
       If cache found then find the best address if more than one. If we
       are talking to a multi homed host then take the IP address with the
       lowest weight */
    if (pres) {
	if (pres->multihomed) {
	    int cnt;
	    float best_weight = 1e30;		    /* Should be FLT_MAX :-( */
	    for (cnt=0; cnt<pres->multihomed; cnt++) {
		if (*(pres->weight+cnt) < best_weight) {
		    best_weight = *(pres->weight+cnt);
		    pres->offset = cnt;
		}
	    }
	    *multi = YES;
	} else
	    *multi = NO;
	pres->hits++;				    /* Update number of hits */
    } else {						/* Go and ask for it */
	struct hostent *hostelement;			      /* see netdb.h */
#ifdef MVS	/* Outstanding problem with crash in MVS gethostbyname */
	if (TRACE)
	    fprintf(stderr, "HTTCP: gethostbyname(%s)\n", host);
#endif
	if ((hostelement = gethostbyname(host)) == NULL) {
	    if (TRACE) fprintf(stderr, "ParseInet... Can't find internet node name `%s'.\n", host);
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
    return 0;
}


/*	Parse a network node address and port
**	-------------------------------------
**
** On entry,
**	str	points to a string with a node name or number,
**		with optional trailing colon and port number.
**	sin	points to the binary internet or decnet address field.
**
** On exit,
**	*sin	is filled in. If no port is specified in str, that
**		field is left unchanged in *sin.
**
** NOTE:	It is assumed that any portnumber and numeric host address
**		is given in decimal notation. Separation character is '.'
*/
PUBLIC int HTParseInet ARGS3(SockA *,sin, CONST char *,str, BOOL *, multihome)
{
    char *host = NULL;
    StrAllocCopy(host, str);		      /* Take a copy we can mutilate */

    /* Parse port number if present. */    
    *multihome = NO;
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
		fprintf(stderr, "ParseInet... No port indicated\n");
	    }
	}
    }

    /* Parse Internet host */
#ifdef DECNET
    /* read Decnet node name. @@ Should know about DECnet addresses, but it's
       probably worth waiting until the Phase transition from IV to V. */

    sin->sdn_nam.n_len = min(DN_MAXNAML, strlen(host));  /* <=6 in phase 4 */
    strncpy (sin->sdn_nam.n_name, host, sin->sdn_nam.n_len + 1);

    if (TRACE) fprintf(stderr,  
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
	    if (HTGetHostByName(host, sin, multihome)) {
		free(host);
		return -1;
	    }
	}
	if (TRACE) {
	    fprintf(stderr, "ParseInet... Parsed address as port %d on %s\n",
		    (int) ntohs(sin->sin_port),
		    HTInetString(sin));
	}
    }
#endif  /* Internet vs. Decnet */
    free(host);
    return 0;	/* OK */
}


/*
**	Get host name of the machine on the other end of a socket.
**	THIS SHOULD BE CALLED HTGetHostByAddr :-(
*/
PUBLIC char * HTGetHostName ARGS1(int, soc)
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
	if (TRACE) fprintf(stderr,
			   "TCP......... Can't find internet node name for peer!!\n");
	return NULL;
    }
    StrAllocCopy(name, phost->h_name);
    if (TRACE) fprintf(stderr, "TCP......... Peer name is `%s'\n", name);

    return name;

#endif	/* not DECNET */
}


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
    CTRACE(tfp, "TCP......... Local host name is %s\n", name);
    StrAllocCopy(hostname, name);

#ifndef DECNET  /* Decnet ain't got no damn name server 8#OO */
    phost=gethostbyname(name);		/* See netdb.h */
    if (!phost) {
	if (TRACE) fprintf(stderr, 
		"TCP......... Can't find my own internet node address for `%s'!!\n",
		name);
	return;  /* Fail! */
    }
    StrAllocCopy(hostname, phost->h_name);
    CTRACE(tfp, "TCP......... Full local host name is %s\n", hostname);

#ifdef NEED_HOST_ADDRESS		/* no -- needs name server! */
    memcpy(&HTHostAddress, &phost->h_addr, phost->h_length);
    if (TRACE) fprintf(stderr, "     Name server says that I am `%s' = %s\n",
	    hostname, HTInetString(&HTHostAddress));
#endif /* NEED_HOST_ADDRESS */

#endif /* not Decnet */
}


PUBLIC CONST char * HTHostName NOARGS
{
    get_host_details();
    return hostname;
}


/*								HTDoConnect()
**
**	TEMPORARY FUNCTION.
**	Note: Any port indication in URL, e.g., as `host:port' overwrites
**	the default_port value.
**
**	Returns 0 if OK, -1 on error
*/
PUBLIC int HTDoConnect ARGS5(HTRequest *, request, char *, url,
			     u_short, default_port, int *, sockfd,
			     u_long *, addr)
{
    BOOL multihomed = NO;
    time_t deltatime;
    int status;
    SockA sock_addr;				/* SockA is defined in tcp.h */
    char *p1 = HTParse(url, "", PARSE_HOST);
    char *at_sign;
    char *host;

    /* if theres an @ then use the stuff after it as a hostname */
    if((at_sign = strchr(p1,'@')) != NULL)
	host = at_sign+1;
    else
	host = p1;
    if (TRACE) fprintf(stderr, "HTDoConnect. Looking up `%s\'\n", host);

   /* Set up defaults */
    memset((void *) &sock_addr, '\0', sizeof(sock_addr));
#ifdef DECNET
    sock_addr.sdn_family = AF_DECnet;	      /* Family = DECnet, host order */
    sock_addr.sdn_objnum = DNP_OBJ;	      /* Default: http object number */
#else  /* Internet */
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(default_port);
#endif

    /* Get node name */
    if (HTParseInet(&sock_addr, host, &multihomed)) {
	if (TRACE) fprintf(stderr, "HTDoConnect. Can't locate remote host `%s\'\n", host);
	HTErrorAdd(request, ERR_FATAL, NO, HTERR_NO_REMOTE_HOST,
		   (void *) host, strlen(host), "HTDoConnect");
	free (p1);
	*sockfd = -1;
	return -1;
    }

#ifdef DECNET
    if ((*sockfd = socket(AF_DECnet, SOCK_STREAM, 0)) < 0) {
#else
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
#endif
	HTErrorSysAdd(request, ERR_FATAL, NO, "socket");
	free (p1);
	return -1;
    }
    if (addr)
	*addr = ntohl(sock_addr.sin_addr.s_addr);

    if (TRACE)
	fprintf(stderr, "HTDoConnect. Created socket number %d\n", *sockfd);

    if (multihomed)				/* Start timer on connection */
	deltatime = time(NULL);
    if ((status = connect(*sockfd, (struct sockaddr *) &sock_addr,
			  sizeof(sock_addr))) < 0) {
	HTErrorSysAdd(request, ERR_FATAL, NO, "connect");
	HTTCPCacheRemoveHost(host);		   /* Remove host from cache */
	if (NETCLOSE(*sockfd) < 0)
	    HTErrorSysAdd(request, ERR_FATAL, NO, "close");
	free(p1);
	*sockfd = -1;
	return -1;
    }

    /* Measure time to make a connection and recalculate weights */
    if (multihomed) {
	deltatime = time(NULL) - deltatime;
	HTTCPAddrWeights(host, deltatime);
    }
    free(p1);
    return status;
}


/*								HTDoAccept()
**
**	This function makes a non-blocking accept on a port and polls every
**	second until MAX_ACCEPT_POLL or interrupted by user.
**
**	BUGS Interrupted is not yet implemented!!!
**
**	Returns 0 if OK, -1 on error
*/
PUBLIC int HTDoAccept ARGS2(HTRequest *, request, int, sockfd)
{
    SockA soc_address;				/* SockA is defined in tcp.h */
    int status;
    int cnt;
    int soc_addrlen = sizeof(soc_address);
    if (sockfd < 0) {
	if (TRACE) fprintf(stderr, "HTDoAccept.. Bad socket number\n");
	return -1;
    }
	
    /* First make the socket non-blocking */
    if((status = FCNTL(sockfd, F_GETFL, 0)) != -1) {
	status |= FNDELAY;
	status = FCNTL(sockfd, F_SETFL, status);
    }
    if (status == -1) {
	HTErrorSysAdd(request, ERR_FATAL, NO, "fcntl");
	return -1;
    }

    /* Now poll every sekund */
    for(cnt=0; cnt<MAX_ACCEPT_POLL; cnt++) {
	if ((status = accept(sockfd, (struct sockaddr*) &soc_address,
			     &soc_addrlen)) >= 0) {
	    if (TRACE) fprintf(stderr,
			       "HTDoAccept.. Accepted new socket %d\n",	
			       status);
	    return status;
	} else
	    HTErrorSysAdd(request, ERR_WARNING, YES, "accept");
	sleep(1);
    }	
    
    /* If nothing has happened */    
    if (TRACE)
	fprintf(stderr, "HTDoAccept.. Timed out, no connection!\n");
    HTErrorAdd(request, ERR_FATAL, NO, HTERR_TIME_OUT, NULL, 0, "HTDoAccept");
    return -1;
}

