/*									HTDNS.c
**	DOMAIN NAME SERVICE MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This object manages a cache of hosts we have looked up vis DNS.
**	The object contains the necessary parts from hostent. For Internet host
**	hostent->h_addr_list is not an array of char pointers but an array of 
**	pointers of type in_addr.
**
**	13 Sep 95  HFN	Spawned from HTTCP.c and rewritten
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
#include "HTDNS.h"					 /* Implemented here */

#define TCP_TIMEOUT		3600L		/* Default TCP timeout i 1 h */
#define DNS_TIMEOUT		172800L	      /* Default DNS timeout is 48 h */
#define HASH_SIZE		67

/* Type definitions and global variables etc. local to this module */
struct _HTdns {
    char *  		hostname;		/* name of host + maybe port */
    time_t		ntime;				    /* Creation time */

    char *		server;				     /* Server class */
    int 		version;			   /* Server version */
    HTTCPType		type;				  /* Connection type */

    BOOL		active;				    /* Socket in use */
    time_t		expires;		      /* Socket expires time */
    SOCKFD		sockfd;			    /* Persistent connection */

    int			addrlength;	       /* Length of address in bytes */
    int			homes;	       /* Number of IP addresses on the host */
    char **		addrlist;      /* List of addresses from name server */
    double *		weight;			   /* Weight on each address */
};

PRIVATE HTList	**CacheTable = NULL;
PRIVATE HTList	*PersSock = NULL;	       /* List of persistent sockets */
PRIVATE time_t	DNSTimeout = DNS_TIMEOUT;	   /* Timeout on DNS entries */
PRIVATE time_t	TCPTimeout = TCP_TIMEOUT;	   /* Timeout on TCP sockets */

/* ------------------------------------------------------------------------- */

PRIVATE void free_object (HTdns * me)
{
    if (me) {
	FREE(me->hostname);
	FREE(me->server);
 	if (me->sockfd != INVSOC && !me->active) {
	    NETCLOSE(me->sockfd);
	    HTEvent_UnRegister(me->sockfd, (SockOps) FD_ALL);
	    HTList_removeObject(PersSock, me);
	}
	if (*me->addrlist)
	    free(*me->addrlist);
	FREE(me->addrlist);
	FREE(me->weight);
	free(me);
    }
}

PRIVATE BOOL delete_object (HTList * list, HTdns *me)
{
    if (PROT_TRACE)
	fprintf(TDEST,"DNS Delete.. object %p from list %p\n", me, list);
    HTList_removeObject(list, (void *) me);
    free_object(me);
    return YES;
}

/*	HTDNS_setTimeout
**	----------------
**	Set the cache timeout for DNS entries. Default is DNS_TIMEOUT
*/
PUBLIC void HTDNS_setTimeout (time_t timeout)
{
    DNSTimeout = timeout;
}

/*	HTDNS_timeout
**	-------------
**	Get the cache timeout 
*/
PUBLIC time_t HTDNS_timeout (time_t timeout)
{
    return DNSTimeout;
}

/*	HTDNS_setSockTimeout
**	--------------------
**	Set the cache timeout for DNS entries. Default is DNS_TIMEOUT
*/
PUBLIC void HTDNS_setSockTimeout (time_t timeout)
{
    TCPTimeout = timeout;
}

/*	HTDNS_sockTimeout
**	-----------------
**	Get the cache timeout 
*/
PUBLIC time_t HTDNS_sockTimeout (time_t timeout)
{
    return TCPTimeout;
}

/*	HTDNS_serverClass
**	-----------------
*/
PUBLIC char *HTDNS_serverClass (HTdns *dns)
{
     return dns ? dns->server : NULL;
}

PUBLIC void HTDNS_setServerClass (HTdns * dns, char *s_class)
{
    if (dns && s_class) StrAllocCopy(dns->server, s_class);
}

/*	HTDNS_serverVersion
**	-------------------
*/
PUBLIC int HTDNS_serverVersion (HTdns *dns)
{
     return dns ? dns->version : 0;
}

PUBLIC void HTDNS_setServerVersion (HTdns * dns, int version)
{
    if (dns) dns->version = version;
}

/*	HTDNS_connection
**	----------------
*/
PUBLIC HTTCPType HTDNS_connection (HTdns *dns)
{
     return dns ? dns->type : 0;
}

PUBLIC void HTDNS_setConnection (HTdns * dns, HTTCPType type)
{
    if (dns) dns->type = type;
}

/*	Persistent Connections
**	----------------------
*/
PUBLIC SOCKFD HTDNS_socket(HTdns *dns)
{
    return dns ? dns->sockfd : INVSOC;
}

/*
**	We don't want more than MaxSockets-2 connections to be persistent in
**	order to avoid deadlock.
*/
PUBLIC BOOL HTDNS_setSocket(HTdns *dns, SOCKFD socket)
{
    if (!dns) return NO;
    if (!PersSock) PersSock = HTList_new();
    if (socket == INVSOC) {
	dns->sockfd = socket;
	dns->active = NO;
	dns->expires = 0;
	HTList_removeObject(PersSock, dns);
	return YES;
    } else if (HTList_count(PersSock) < HTNet_maxSocket()-2) {
	dns->sockfd = socket;
	dns->active = YES;
	dns->expires = time(NULL) + TCPTimeout;		  /* Default timeout */
	HTList_addObject(PersSock, dns);
	return YES;
    }
    return NO;
}

PUBLIC int HTDNS_socketCount (void)
{
    return PersSock ? HTList_count(PersSock) : 0;
}

PUBLIC void HTDNS_clearActive (HTdns *dns)
{
    if (dns) {
	if (PROT_TRACE)
	    fprintf(TDEST, "DNS Clear... Active bit for socket %d\n",
		    dns->sockfd);
	dns->active = NO;
    }
}

/*	Persistent Connection Expiration
**	--------------------------------
**	Should normally not be used. If, then use calendar time.
*/
PUBLIC void HTDNS_setSockExpires (HTdns * dns, time_t expires)
{
    if (dns) dns->expires = expires;
}

PUBLIC time_t HTDNS_sockExpires (HTdns * dns)
{
    return dns ? dns->expires : -1;
}

/*	HTDNS_add
**	---------
**	Add an element to the cache of visited hosts. Note that this function
**	requires the system implemented structure hostent and not our own
**	host_info. The homes variable indicates the number of
**	IP addresses found. host is a host name possibly with a port number
**	returns address of new HTdns object
*/
PRIVATE HTdns * HTDNS_add (HTList * list, struct hostent * element,
			   char *host, int *homes)
{
    HTdns *me;
    char *addr;
    char **index = element->h_addr_list;
    int cnt = 1;

    while(*index++) cnt++;
    if ((me = (HTdns *) calloc(1, sizeof(HTdns))) == NULL ||
	(me->addrlist = (char **) calloc(1, cnt*sizeof(char*))) == NULL ||
	(addr = (char *) calloc(1, cnt*element->h_length)) == NULL)
	outofmem(__FILE__, "HTDNS_add");
    StrAllocCopy(me->hostname, host);
    me->ntime = time(NULL);
    me->sockfd = INVSOC;
    index = element->h_addr_list;
    cnt = 0;
    while (*index) {
	*(me->addrlist+cnt) = addr+cnt*element->h_length;
	memcpy((void *) *(me->addrlist+cnt++), *index++, element->h_length);
    }
    me->homes = cnt;
    *homes = cnt;
    if ((me->weight = (double *) calloc(me->homes, sizeof(double))) == NULL)
	outofmem(__FILE__, "HTDNS_add");
    me->addrlength = element->h_length;
    if (PROT_TRACE)
	fprintf(TDEST, "DNS Add..... `%s\' with %d home(s) to %p\n",
		host, *homes, list);
    HTList_addObject(list, (void *) me);
    return me;
}


/*	HTDNS_updateWeights
**	-------------------
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
**	host is a host name possibly with port number. current is the index
**	returned by HTGetHostByName()
*/
PUBLIC BOOL HTDNS_updateWeigths(HTdns *dns, int current, time_t deltatime)
{
    if (dns) {
	int cnt;
	CONST double passive = 0.9; 	  /* Factor for all passive IP_addrs */
#if 0
	CONST int Neff = 3;
	CONST double alpha = exp(-1.0/Neff);
#else
	CONST double alpha = 0.716531310574;	/* Doesn't need the math lib */
#endif
	for (cnt=0; cnt<dns->homes; cnt++) {
	    if (cnt == current) {
		*(dns->weight+current) = *(dns->weight+current)*alpha + (1.0-alpha)*deltatime;
	    } else {
		*(dns->weight+cnt) = *(dns->weight+cnt) * passive;
	    }
	    if (PROT_TRACE)
		fprintf(TDEST, "DNS Weigths. Home %d has weight %4.2f\n", cnt,
			*(dns->weight+cnt));
	}
	return YES;
    }
    if (PROT_TRACE)
	fprintf(TDEST, "DNS Weigths. Object %p not found'\n", dns);
    return NO;
}

/*	HTDNS_delete
**	------------
**	Remove an element from the cache
**	Host must be a host name possibly with a port number
*/
PUBLIC BOOL HTDNS_delete (CONST char * host)
{
    HTList *list;
    int hash = 0;
    CONST char *ptr;
    if (!host || !CacheTable) return NO;
    for(ptr=host; *ptr; ptr++)
	hash = (int) ((hash * 3 + (*(unsigned char *) ptr)) % HASH_SIZE);
    if ((list = CacheTable[hash])) {	 /* We have the list, find the entry */
	HTdns *pres;
	while ((pres = (HTdns *) HTList_nextObject(list))) {
	    if (!strcmp(pres->hostname, host)) {
		delete_object(CacheTable[hash], pres);
		break;
	    }
	}
    }
    return YES;
}

/*	HTDNS_deleteAll
**	---------------
**	Destroys the cache completely
*/
PUBLIC BOOL HTDNS_deleteAll (void)
{
    int cnt;
    HTList *cur;
    if (!CacheTable) return NO;
    for (cnt=0; cnt<HASH_SIZE; cnt++) {
	if ((cur = CacheTable[cnt])) { 
	    HTdns *pres;
	    while ((pres = (HTdns *) HTList_nextObject(cur)) != NULL)
		free_object(pres);
	}
	HTList_delete(CacheTable[cnt]);
	CacheTable[cnt] = NULL;
    }
    if (PersSock) {		     /* Remove list of persistent connection */
	HTList_delete(PersSock);
	PersSock = NULL;
    }
    return YES;
}

/*	HTDNS_closeSocket
**	-----------------
**	This function is registered when the socket is idle so that we get
**	a notification if the socket closes at the other end. At this point
**	we can't use the request object as it might have been freed a long
**	time ago.
*/
PUBLIC int HTDNS_closeSocket(SOCKET soc, HTRequest * request, SockOps ops)
{
    if (PROT_TRACE)
	fprintf(TDEST, "DNS Close... called with socket %d with ops %x\n",
		soc, (unsigned) ops);
    if (ops == FD_READ && PersSock) {
	HTList *cur = PersSock;
	HTdns *pres;
	while ((pres = (HTdns *) HTList_nextObject(cur))) {
	    if (pres->sockfd == soc) {
		if (PROT_TRACE) fprintf(TDEST, "DNS Close... socket %d\n",soc);
		NETCLOSE(soc);
		HTDNS_setSocket(pres, INVSOC);
		break;
	    }
	}
	if (!pres) fprintf(TDEST, "DNS Close... socket NOT FOUND!\n");
    }
    HTEvent_UnRegister(soc, (SockOps) FD_ALL);
    return HT_OK;
}

/*	HTGetHostByName
**	---------------
**	Resolve the host name using internal DNS cache. As we want to refer   
**	a specific host when timing the connection the weight function must
**	use the 'current' value as returned.
**	Host must be a host name possibly with a port number
**      Returns:
**	       	>0	Number of homes
**		 0	Wait for persistent socket
**		-1	Error
*/
PUBLIC int HTGetHostByName (HTNet *net, char *host)
{
    SockA *sin = &net->sock_addr;
    int homes = -1;
    HTList *list;				    /* Current list in cache */
    HTdns *pres = NULL;
    if (!net || !host) {
	if (PROT_TRACE)
	    fprintf(TDEST, "HostByName.. Bad argument\n");
	return -1;
    }
    net->home = 0;
    
    /* Find a hash for this host */
    {
	int hash = 0;
	char *ptr;
	for(ptr=host; *ptr; ptr++)
	    hash = (int) ((hash * 3 + (*(unsigned char *) ptr)) % HASH_SIZE);
	if (!CacheTable) {
	    CacheTable = (HTList**) calloc(HASH_SIZE, sizeof(HTList *));
	    if (!CacheTable) outofmem(__FILE__, "HTDNS_init");
	}
	if (!CacheTable[hash]) CacheTable[hash] = HTList_new();
	list = CacheTable[hash];
    }

    /* Search the cache */
    {
	HTList *cur = list;
	while ((pres = (HTdns *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->hostname, host)) {
		if (time(NULL) > pres->ntime + DNSTimeout) {
		    if (PROT_TRACE)
			fprintf(TDEST, "HostByName.. Refreshing cache\n");
		    delete_object(list, pres);
		    pres = NULL;
		}
		break;
	    }
	}
    }
    if (pres) {
	if (PROT_TRACE)
	    fprintf(TDEST, "HostByName.. '%s\' found in cache\n", host);

	/* See if we have an open connection already */
	if (pres->sockfd != INVSOC) {
	    if (pres->active) {			   /* Warm connection in use */
		net->sockfd = pres->sockfd;		    /* Assign always */
		if (pres->type == HT_TCP_INTERLEAVE) {
		    if (PROT_TRACE)
			fprintf(TDEST, "HostByName.. waiting for socket\n");
		    return 0;			/* Wait for clear connection */
		}
	    } else if (pres->expires < time(NULL)) {	      /* Gotton cold */
		if (PROT_TRACE)
		    fprintf(TDEST, "HostByName.. Closing %d\n", pres->sockfd);
		NETCLOSE(pres->sockfd);
		HTEvent_UnRegister(pres->sockfd, (SockOps) FD_ALL);
		HTDNS_setSocket(pres, INVSOC);
	    } else {		    /* Warm connection is idle and ready :-) */
		HTEvent_UnRegister(pres->sockfd, (SockOps) FD_ALL);
		pres->active = YES;		
		net->sockfd = pres->sockfd;
	    }
 	}

	/*
	** Find the best home. We still want to do this as we use it as a
	** fall back for persistent connections
	*/
	homes = pres->homes;
	if (pres->homes > 1) {
	    int cnt = 0;
	    double best_weight = 1e30;			      /* Pretty good */
	    while (cnt < pres->homes) {
		if (*(pres->weight+cnt) < best_weight) {
		    best_weight = *(pres->weight+cnt);
		    net->home = cnt;
		}
		cnt++;
	    }
	}
	net->dns = pres;
	memcpy(&sin->sin_addr, *(pres->addrlist+net->home), pres->addrlength);
    } else {
	struct hostent *hostelement;			      /* see netdb.h */
	char *port = strchr(host, ':');
#ifdef HT_REENTRANT
	int thd_errno;
	char buffer[HOSTENT_MAX];
	struct hostent result;			      /* For gethostbyname_r */
	if (port) *port='\0';
	HTProgress(net->request, HT_PROG_DNS, host);
	hostelement = gethostbyname_r(host, &result, buffer,
				      HOSTENT_MAX, &thd_errno);
#else
	if (port) *port='\0';
	HTProgress(net->request, HT_PROG_DNS, host);
	hostelement = gethostbyname(host);
#endif
	if (!hostelement) {
	    if (PROT_TRACE)
		fprintf(TDEST, "HostByName.. Can't find node `%s'.\n", host);
	    return -1;
	}	
	if (port) *port=':';			       	  /* Put ':' back in */
	net->dns = HTDNS_add(list, hostelement, host, &homes);
	memcpy(&sin->sin_addr,*hostelement->h_addr_list,hostelement->h_length);
    }
    return homes;
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
    char *name = NULL;
    struct hostent * phost;		/* Pointer to host -- See netdb.h */
#ifdef HT_REENTRANT
    int thd_errno;
    char buffer[HOSTENT_MAX];
    struct hostent result;		      	      /* For gethostbyaddr_r */
#endif

#ifdef DECNET  /* Decnet ain't got no damn name server 8#OO */
    return NULL;
#else
    if (getpeername(soc, &addr, &len) < 0)
	return NULL;
    iaddr = &(((struct sockaddr_in *)&addr)->sin_addr);

#ifdef HT_REENTRANT
    phost = gethostbyaddr_r((char *) iaddr, sizeof(struct in_addr), AF_INET,
			    &result, buffer, HOSTENT_MAX, &thd_errno);
#else
    phost = gethostbyaddr((char *) iaddr, sizeof(struct in_addr), AF_INET);
#endif
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
