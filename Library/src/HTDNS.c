/*									HTDNS.c
**	DOMAIN NAME SERVICE MANAGER
**
**	(c) COPYRIGHT MIT 1995-2003.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This object manages a cache of hosts we have looked up vis DNS.
**	The object contains the necessary parts from hostent. For Internet host
**	hostent->h_addr_list is not an array of char pointers but an array of 
**	pointers of type in_addr.
**
**	13 Sep 95  HFN	Spawned from HTTCP.c and rewritten
**      23 Feb 03  MJD  Started working on IDN implementation
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTParse.h"
#include "HTAlert.h"
#include "HTError.h"
#include "HTTrans.h"
#include "HTHstMan.h"
#include "HTDNS.h"					 /* Implemented here */

#ifdef LIBWWW_USEIDN
#include "idn/api.h"
#endif

#define DNS_TIMEOUT		1800L	     /* Default DNS timeout is 30 mn */

/* Type definitions and global variables etc. local to this module */
struct _HTdns {
    char *  		hostname;			     /* name of host */
    time_t		ntime;				    /* Creation time */
    int			addrlength;	       /* Length of address in bytes */
    int			homes;	       /* Number of IP addresses on the host */
    char **		addrlist;      /* List of addresses from name server */
    double *		weight;			   /* Weight on each address */
};

PRIVATE HTList	**CacheTable = NULL;
PRIVATE time_t	DNSTimeout = DNS_TIMEOUT;	   /* Timeout on DNS entries */

/* ------------------------------------------------------------------------- */

PRIVATE void free_object (HTdns * me)
{
    if (me) {
	HT_FREE(me->hostname);
	if (*me->addrlist)
	    HT_FREE(*me->addrlist);
	HT_FREE(me->addrlist);
	HT_FREE(me->weight);
	HT_FREE(me);
    }
}

PRIVATE BOOL delete_object (HTList * list, HTdns * me)
{
    HTTRACE(PROT_TRACE, "DNS Delete.. object %p from list %p\n" _ me _ list);
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

/*	HTDNS_add
**	---------
**	Add an element to the cache of visited hosts. Note that this function
**	requires the system implemented structure hostent and not our own
**	host_info. The homes variable indicates the number of IP addresses 
**	found. A host name must NOT contain a port number.
**	Returns address of new HTdns object
*/
PUBLIC HTdns * HTDNS_add (HTList * list, struct hostent * element,
			  char *host, int *homes)
{
    HTdns *me;
    char *addr = NULL;
    char **index = element->h_addr_list;
    int cnt = 1;

    while(*index++) cnt++;
    if ((me = (HTdns *) HT_CALLOC(1, sizeof(HTdns))) == NULL ||
	(me->addrlist = (char **) HT_CALLOC(1, cnt*sizeof(char*))) == NULL ||
	(addr = (char *) HT_CALLOC(1, cnt*element->h_length)) == NULL)
	HT_OUTOFMEM("HTDNS_add");
    StrAllocCopy(me->hostname, host);
    me->ntime = time(NULL);
    index = element->h_addr_list;
    cnt = 0;
    while (*index) {
	*(me->addrlist+cnt) = addr+cnt*element->h_length;
	memcpy((void *) *(me->addrlist+cnt++), *index++, element->h_length);
    }
    me->homes = cnt;
    *homes = cnt;
    if ((me->weight = (double *) HT_CALLOC(me->homes, sizeof(double))) == NULL)
        HT_OUTOFMEM("HTDNS_add");
    me->addrlength = element->h_length;
    HTTRACE(PROT_TRACE, "DNS Add..... `%s\' with %d home(s) to %p\n" _ 
		host _ *homes _ list);
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
**	"current" is the index returned by HTGetHostByName()
*/
PUBLIC BOOL HTDNS_updateWeigths(HTdns *dns, int current, ms_t deltatime)
{
    if (dns) {
	int cnt;
	const double passive = 0.9; 	  /* Factor for all passive IP_addrs */
#if 0
	const int Neff = 3;
	const double alpha = exp(-1.0/Neff);
#else
	const double alpha = 0.716531310574;	/* Doesn't need the math lib */
#endif
	for (cnt=0; cnt<dns->homes; cnt++) {
	    if (cnt == current) {
		*(dns->weight+current) = *(dns->weight+current)*alpha + (1.0-alpha)*deltatime;
		if (*(dns->weight+current) < 0.0) *(dns->weight+current) = 0.0;
	    } else {
		*(dns->weight+cnt) = *(dns->weight+cnt) * passive;
	    }
	    HTTRACE(PROT_TRACE, "DNS weight.. Home %d has weight %4.2f\n" _ cnt _ 
			*(dns->weight+cnt));
	}
	return YES;
    }
    HTTRACE(PROT_TRACE, "DNS weight.. Object %p not found'\n" _ dns);
    return NO;
}

/*	HTDNS_delete
**	------------
**	Remove an element from the cache
*/
PUBLIC BOOL HTDNS_delete (const char * host)
{
    HTList *list;
    int hash = 0;
    const char *ptr;
    if (!host || !CacheTable) return NO;
    for(ptr=host; *ptr; ptr++)
	hash = (int) ((hash * 3 + (*(unsigned char *) ptr)) % HT_M_HASH_SIZE);
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
    for (cnt=0; cnt<HT_M_HASH_SIZE; cnt++) {
	if ((cur = CacheTable[cnt])) { 
	    HTdns *pres;
	    while ((pres = (HTdns *) HTList_nextObject(cur)) != NULL)
		free_object(pres);
	}
	HTList_delete(CacheTable[cnt]);
	CacheTable[cnt] = NULL;
    }
    HT_FREE(CacheTable);
    return YES;
}

/*	Decode one hex character
*/
/* copied from HTSRC.c, where it is not used */
PRIVATE long from_hex (char c)
{
    return 		  (c>='0')&&(c<='9') ? c-'0'
			: (c>='A')&&(c<='F') ? c-'A'+10
			: (c>='a')&&(c<='f') ? c-'a'+10
			:		       0;
}

#ifdef LIBWWW_USEIDN
/*	HTDNSUTF8toACE
**	--------------
**	Convert Internationalized Domain Name in UTF-8 to
**	Ascii Compatible Encoding (ACE, i.e. punycode).
**      Parameters:
**          In:     hostUTF8: host name in (potentially %-escaped) UTF-8
**                  memory (allocated by caller) for punycode
**                  length of punycode in punyLength
**          Out:    result in punycode
**	Returns:
**	       	0	Success
**	        !=0	Error
*/
PUBLIC int HTACEfromUTF8 (char *hostUTF8, char *punycode, size_t punyLength)
{
    idn_result_t result;
    char *hostNoPercent = (char *) HT_MALLOC(strlen(hostUTF8)+1);
    char *ptr, *ptr2;

    /* unescape %-escapes from URI */
    /* this code already exists elsewhere, replace HTEscape.c:HTUnEscape() */
    for (ptr = hostUTF8, ptr2 = hostNoPercent; *ptr; ) {
	if (*ptr=='%' && isxdigit(*(ptr+1)) && isxdigit(*(ptr+2))) {
	    /* isxdigit() includes check for end of string */
	    unsigned char unesc = (unsigned char) (from_hex(*(ptr+1)) * 16 + from_hex(*(ptr+2)));
	    if (unesc=='.')  *ptr2++ = *ptr++;   /* just copy, because reserved, for security */
	    else {
		*ptr2++ = (char) unesc;
		ptr += 3;
	    }
	} else *ptr2++ = *ptr++;   /* just copy */
    }
    *ptr2 = *ptr;

    /* still to do? check for valid UTF-8 */
    /* maybe check for ASCII-only (efficiency?) */

    /* call ToASCII conversion (stringprep/nameprep) */
    result = idn_encodename(IDN_ENCODE_QUERY & (~IDN_LOCALCONV),
	                    hostNoPercent, punycode, punyLength);
    if (result!=idn_success)
        HTTRACE(PROT_TRACE, "IDN encoding not successful.\n");
    HT_FREE(hostNoPercent);
    return result;
}
#else
PUBLIC int HTACEfromUTF8 (char *hostUTF8, char *punycode, size_t punyLength)
{
	int l = strlen(hostUTF8);
	l = l<(punyLength-1) ? l : punyLength-1;
	strncpy (punycode, hostUTF8, l);
	punycode[l] = 0;
	return 0;
}
#endif

/*	HTGetHostByName
**	---------------
**	Resolve the host name using internal DNS cache. As we want to refer   
**	a specific host when timing the connection the weight function must
**	use the 'current' value as returned.
**      Returns:
**	       	>0	Number of homes
**		-1	Error
*/
PUBLIC int HTGetHostByName (HTHost * host, char *hostname, HTRequest* request)
{
    SockA *sin = HTHost_getSockAddr(host);
    int homes = -1;
    HTList *list;				    /* Current list in cache */
    HTdns *pres = NULL;
    char hostace[256]; /* check lengths!!! */

    if (!host || !hostname) {
	HTTRACE(PROT_TRACE, "HostByName.. Bad argument\n");
	return -1;
    }
    if (HTACEfromUTF8 (hostname, hostace, 255)) {
            /* HTRequest_addSystemError(request, ERR_FATAL, socerrno, NO,
   			             "gethostbyname"); */
	    return -1;
    }
    HTHost_setHome(host, 0); 

    /* Find a hash for this host */
    {
	int hash = 0;
	char *ptr;
	for(ptr=hostace; *ptr; ptr++)
	    hash = (int) ((hash * 3 + (*(unsigned char *) ptr)) % HT_M_HASH_SIZE);
	if (!CacheTable) {
	    if ((CacheTable = (HTList* *) HT_CALLOC(HT_M_HASH_SIZE, sizeof(HTList *))) == NULL)
	        HT_OUTOFMEM("HTDNS_init");
	}
	if (!CacheTable[hash]) CacheTable[hash] = HTList_new();
	list = CacheTable[hash];
    }

    /* Search the cache */
    {
	HTList *cur = list;
	while ((pres = (HTdns *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->hostname, hostace)) {
		if (time(NULL) > pres->ntime + DNSTimeout) {
		    HTTRACE(PROT_TRACE, "HostByName.. Refreshing cache\n");
		    delete_object(list, pres);
		    pres = NULL;
		}
		break;
	    }
	}
    }
    if (pres) {
	/*
	** Find the best home. We still want to do this as we use it as a
	** fall back for persistent connections
	*/
	homes = pres->homes;
	if (pres->homes > 1) {
	    int cnt = 0;
	    double best_weight = 1e30;			      /* Pretty bad */
	    while (cnt < pres->homes) {
		if (*(pres->weight+cnt) < best_weight) {
		    best_weight = *(pres->weight+cnt);
		    HTHost_setHome(host, cnt);
		}
		cnt++;
	    }
	}
	host->dns = pres;
	memcpy((void *) &sin->sin_addr, *(pres->addrlist+HTHost_home(host)),
	       pres->addrlength);
    } else {
	struct hostent *hostelement;			      /* see netdb.h */
	HTAlertCallback *cbf = HTAlert_find(HT_PROG_DNS);
#ifdef HT_REENTRANT
	int thd_errno;
	char buffer[HOSTENT_MAX];
	struct hostent result;			      /* For gethostbyname_r */
#endif
#ifdef HAVE_GETHOSTBYNAME_R_3
        struct hostent_data hdata;
#endif

	if (cbf) (*cbf)(request, HT_PROG_DNS, HT_MSG_NULL,NULL,hostace,NULL);
#ifdef HAVE_GETHOSTBYNAME_R_5
	hostelement = gethostbyname_r(hostace, &result, buffer,
				      HOSTENT_MAX, &thd_errno);
#elif defined(HAVE_GETHOSTBYNAME_R_6)
	gethostbyname_r(hostace, &result, buffer,
		        HOSTENT_MAX, &hostelement, &thd_errno);

#elif defined(HAVE_GETHOSTBYNAME_R_3)
        if (gethostbyname_r(hostace, &result, &hdata) == 0) {
	    hostelement = &result;
	}
	else {
	    hostelement = NULL;
	}
#else
	if (cbf) (*cbf)(request, HT_PROG_DNS, HT_MSG_NULL,NULL,hostace,NULL);
	hostelement = gethostbyname(hostace);
#endif
	if (!hostelement) {
            HTRequest_addSystemError(request, ERR_FATAL, socerrno, NO,
   			             "gethostbyname");
	    return -1;
	}	
	host->dns = HTDNS_add(list, hostelement, hostace, &homes);
	memcpy((void *) &sin->sin_addr, *hostelement->h_addr_list,
	       hostelement->h_length);
    }
    return homes;
}


/*
**	Get host name of the machine on the other end of a socket.
**
*/
PUBLIC char * HTGetHostBySock (int soc)
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
#ifdef HAVE_GETHOSTBYADDR_R_5
    struct hostent_data hdata;
#endif

#ifdef DECNET  /* Decnet ain't got no damn name server 8#OO */
    return NULL;
#else
    if (getpeername(soc, &addr, &len) < 0)
	return NULL;
    iaddr = &(((struct sockaddr_in *)&addr)->sin_addr);

#ifdef HAVE_GETHOSTBYADDR_R_7
    phost = gethostbyaddr_r((char *) iaddr, sizeof(struct in_addr), AF_INET,
			    &result, buffer, HOSTENT_MAX, &thd_errno);
#elif defined(HAVE_GETHOSTBYADDR_R_8)
    gethostbyaddr_r((char *) iaddr, sizeof(struct in_addr), AF_INET,
		    &result, buffer, HOSTENT_MAX, &phost, &thd_errno);
#elif defined(HAVE_GETHOSTBYADDR_R_5)
    if(gethostbyaddr_r((char *) iaddr, sizeof(struct in_addr), AF_INET,
		       &result, &hdata)==0) {
	phost=&result;
    }
    else {
	phost = NULL;
    }
#else
    phost = gethostbyaddr((char *) iaddr, sizeof(struct in_addr), AF_INET);
#endif
    if (!phost) {
	HTTRACE(PROT_TRACE, "TCP......... Can't find internet node name for peer!!\n");
	return NULL;
    }
    StrAllocCopy(name, phost->h_name);
    HTTRACE(PROT_TRACE, "TCP......... Peer name is `%s'\n" _ name);
    return name;

#endif	/* not DECNET */
}
