/*								       HTHost.c
**	REMOTE HOST INFORMATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This object manages the information that we know about a remote host.
**	This can for example be what type of host it is, and what version
**	it is using. We also keep track of persistent connections
**
**	April 96  HFN	Written
*/

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "HTParse.h"
#include "HTAlert.h"
#include "HTError.h"
#include "HTNetMan.h"
#include "HTTrans.h"
#include "HTTPUtil.h"
#include "HTTCP.h"
#include "HTHost.h"					 /* Implemented here */
#include "HTHstMan.h"

#define HOST_TIMEOUT		43200L	     /* Default host timeout is 12 h */
#define TCP_TIMEOUT		3600L		/* Default TCP timeout i 1 h */
#define MAX_PIPES		50   /* maximum number of pipelined requests */
#define MAX_HOST_RECOVER	3	      /* Max number of auto recovery */
#define DEFAULT_DELAY		50	  /* Default write flush delay in ms */

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

PRIVATE int HostEvent(SOCKET soc, void * pVoid, HTEventType type);

/* Type definitions and global variables etc. local to this module */
PRIVATE time_t	HostTimeout = HOST_TIMEOUT;	  /* Timeout on host entries */
PRIVATE time_t	TCPTimeout = TCP_TIMEOUT;  /* Timeout on persistent channels */

PRIVATE HTList	** HostTable = NULL;
PRIVATE HTList * PendHost = NULL;	    /* List of pending host elements */

PRIVATE int EventTimeout = -1;		        /* Global Host event timeout */

PRIVATE WriteDelay = DEFAULT_DELAY;			      /* Delay in ms */

/* ------------------------------------------------------------------------- */

PRIVATE void free_object (HTHost * me)
{
    if (me) {
	HT_FREE(me->hostname);
	HT_FREE(me->type);
	HT_FREE(me->server);
	HT_FREE(me->user_agent);
	HT_FREE(me->range_units);
	if (me->channel) {
	    HTChannel_delete(me->channel, HT_OK);
	    me->channel = NULL;
	}
	{
	int i;
	for (i = 0; i < HTEvent_TYPES; i++)
	    HTEvent_delete(me->events[i]);
	}
	HTList_delete(me->pipeline);
	HTList_delete(me->pending);
	HT_FREE(me);
    }
}

PRIVATE BOOL delete_object (HTList * list, HTHost * me)
{
    if (CORE_TRACE) HTTrace("Host info... object %p from list %p\n", me, list);
    HTList_removeObject(list, (void *) me);
    free_object(me);
    return YES;
}

PRIVATE BOOL isLastInPipe (HTHost * host, HTNet * net)
{
    return HTList_lastObject(host->pipeline) == net;
}

/*
**	HostEvent - host event manager - recieves events from the event 
**	manager and dispatches them to the client net objects by calling the 
**	net object's cbf.
**
*/
PRIVATE int HostEvent (SOCKET soc, void * pVoid, HTEventType type)
{
    HTHost * host = (HTHost *)pVoid;

    if (type == HTEvent_READ || type == HTEvent_CLOSE) {
	HTNet * targetNet;

	/* call the first net object */
	do {
	    int ret;
	    targetNet = (HTNet *)HTList_firstObject(host->pipeline);
	    if (targetNet) {
		if (CORE_TRACE)
		    HTTrace("Host Event.. READ passed to `%s\'\n", 
			    HTAnchor_physical(HTRequest_anchor(HTNet_request(targetNet))));
		if ((ret = (*targetNet->event.cbf)(HTChannel_socket(host->channel), 
						  targetNet->event.param, type)) != HT_OK)
		    return ret;
	    }
	    if (targetNet == NULL && host->remainingRead > 0) {
		HTTrace("HostEvent... Error: %d bytes left to read and nowhere to put them\n", host->remainingRead);
		host->remainingRead = 0;
		/*
		**	Fall through to close the channel
		*/
	    }
	/* call pipelined net object to eat all the data in the channel */
	} while (host->remainingRead > 0);

	/* last target net should have set remainingRead to 0 */
	if (targetNet)
	    return HT_OK;

	/* If there was notargetNet, it should be a close */
	if (CORE_TRACE)
	    HTTrace("Host Event.. host %p `%s\' closed connection.\n", 
		    host, host->hostname);

	/* Is there garbage in the channel? Let's check: */
	{
	    char buf[256];
	    int ret;
	    memset(buf, '\0', sizeof(buf));
	    while ((ret = NETREAD(HTChannel_socket(host->channel), buf, sizeof(buf))) > 0) {
		if (CORE_TRACE)
		    HTTrace("Host Event.. Host %p `%s\' had %d extraneous bytes: `%s\'\n",
			    host, host->hostname, ret, buf);
		memset(buf, '\0', sizeof(buf));		
	    }	    
	}
	HTHost_clearChannel(host, HT_OK);
	return HT_OK; 	     /* extra garbage does not constitute an application error */
	
    } else if (type == HTEvent_WRITE || type == HTEvent_CONNECT) {
	HTNet * targetNet = (HTNet *)HTList_lastObject(host->pipeline);
	if (targetNet) {
	    if (CORE_TRACE)
		HTTrace("Host Event.. WRITE passed to `%s\'\n", 
			HTAnchor_physical(HTRequest_anchor(HTNet_request(targetNet))));
	    return (*targetNet->event.cbf)(HTChannel_socket(host->channel), targetNet->event.param, type);
	}
	HTTrace("Host Event.. Who wants to write to `%s\'?\n", host->hostname);
	return HT_ERROR;
    } else if (type == HTEvent_TIMEOUT) {

	if (CORE_TRACE)
	    HTTrace("Host Event.. WE SHOULD DELETE ALL REQUEST ON `%s\'?\n",
		    host->hostname);

    } else {
	HTTrace("Don't know how to handle OOB data from `%s\'?\n", 
		host->hostname);
    }
    return HT_OK;
}

/*
**	Search the host info cache for a host object or create a new one
**	and add it. Examples of host names are
**
**		www.w3.org
**		www.foo.com:8000
**		18.52.0.18
**
**	Returns Host object or NULL if error. You may get back an already
**	existing host object - you're not guaranteed a new one each time.
*/

PUBLIC HTHost * HTHost_new (char * host, u_short u_port)
{
    HTList * list = NULL;			    /* Current list in cache */
    HTHost * pres = NULL;
    int hash = 0;
    if (!host) {
	if (CORE_TRACE) HTTrace("Host info... Bad argument\n");
	return NULL;
    }
    
    /* Find a hash for this host */
    {
	char *ptr;
	for (ptr=host; *ptr; ptr++)
	    hash = (int) ((hash * 3 + (*(unsigned char *) ptr)) % HOST_HASH_SIZE);
	if (!HostTable) {
	    if ((HostTable = (HTList **) HT_CALLOC(HOST_HASH_SIZE,
						   sizeof(HTList *))) == NULL)
	        HT_OUTOFMEM("HTHost_find");
	}
	if (!HostTable[hash]) HostTable[hash] = HTList_new();
	list = HostTable[hash];
    }

    /* Search the cache */
    {
	HTList * cur = list;
	while ((pres = (HTHost *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->hostname, host) && u_port == pres->u_port) {
		if (HTHost_isIdle(pres) && time(NULL)>pres->ntime+HostTimeout){
		    if (CORE_TRACE)
			HTTrace("Host info... Collecting host info %p\n",pres);
		    delete_object(list, pres);
		    pres = NULL;
		}
		break;
	    }
	}
    }

    /* If not found then create new Host object, else use existing one */
    if (pres) {
	if (pres->channel) {
	    if (pres->expires && pres->expires < time(NULL)) {	   /* Cached channel is cold */
		if (CORE_TRACE)
		    HTTrace("Host info... Persistent channel %p gotten cold\n",
			    pres->channel);
		HTChannel_delete(pres->channel, HT_OK);
		pres->channel = NULL;
	    } else {
		if (CORE_TRACE)
		    HTTrace("Host info... REUSING CHANNEL %p\n",pres->channel);
	    }
	}
    } else {
	if ((pres = (HTHost *) HT_CALLOC(1, sizeof(HTHost))) == NULL)
	    HT_OUTOFMEM("HTHost_add");
	pres->hash = hash;
	StrAllocCopy(pres->hostname, host);
	pres->u_port = u_port;
	pres->ntime = time(NULL);
	pres->mode = HT_TP_SINGLE;
	pres->delay = WriteDelay;
	{
	    int i;
	    for (i = 0; i < HTEvent_TYPES; i++)
		pres->events[i]= HTEvent_new(HostEvent, pres, HT_PRIORITY_MAX, EventTimeout);
	}
	if (CORE_TRACE) 
	    HTTrace("Host info... added `%s\' with host %p to list %p\n",
		    host, pres, list);
	HTList_addObject(list, (void *) pres);
    }
    return pres;
}

PUBLIC HTHost * HTHost_newWParse (HTRequest * request, char * url, u_short u_port)
{
	      char * port;
	      char * fullhost = NULL;
	      char * parsedHost = NULL;
	      SockA * sin;
	      HTHost * me;
	      char * proxy = HTRequest_proxy(request);

	      fullhost = HTParse(proxy ? proxy : url, "", PARSE_HOST);

	      /* If there's an @ then use the stuff after it as a hostname */
	      if (fullhost) {
		  char * at_sign;
		  if ((at_sign = strchr(fullhost, '@')) != NULL)
		      parsedHost = at_sign+1;
		  else
		      parsedHost = fullhost;
	      }
	      if (!parsedHost || !*parsedHost) {
		  HTRequest_addError(request, ERR_FATAL, NO, HTERR_NO_HOST,
				     NULL, 0, "HTDoConnect");
		  HT_FREE(fullhost);
		  return NULL;
	      }
	      port = strchr(parsedHost, ':');
	      if (PROT_TRACE)
		  HTTrace("HTDoConnect. Looking up `%s\'\n", parsedHost);
	      if (port) {
		  *port++ = '\0';
		  if (!*port || !isdigit(*port))
		      port = 0;
		  u_port = (u_short) atol(port);
	      }
	      /* Find information about this host */
	      if ((me = HTHost_new(parsedHost, u_port)) == NULL) {
		  if (PROT_TRACE)HTTrace("HTDoConnect. Can't get host info\n");
		  me->tcpstate = TCP_ERROR;
		  return NULL;
	      }
	      sin = &me->sock_addr;
	      memset((void *) sin, '\0', sizeof(SockA));
#ifdef DECNET
	      sin->sdn_family = AF_DECnet;
	      net->sock_addr.sdn_objnum = port ? (unsigned char)(strtol(port, (char **) 0, 10)) : DNP_OBJ;
#else  /* Internet */
	      sin->sin_family = AF_INET;
	      sin->sin_port = htons(u_port);
#endif
	      HT_FREE(fullhost);	/* parsedHost points into fullhost */
	      return me;
}

/*
**	Search the host info cache for a host object. Examples of host names:
**
**		www.w3.org
**		www.foo.com:8000
**		18.52.0.18
**
**	Returns Host object or NULL if not found.
*/
PUBLIC HTHost * HTHost_find (char * host)
{
    HTList * list = NULL;			    /* Current list in cache */
    HTHost * pres = NULL;
    if (CORE_TRACE)
	HTTrace("Host info... Looking for `%s\'\n", host ? host : "<null>");

    /* Find a hash for this host */
    if (host && HostTable) {
	int hash = 0;
	char *ptr;
	for (ptr=host; *ptr; ptr++)
	    hash = (int) ((hash * 3 + (*(unsigned char *) ptr)) % HOST_HASH_SIZE);
	if (!HostTable[hash]) return NULL;
	list = HostTable[hash];

	/* Search the cache */
	{
	    HTList * cur = list;
	    while ((pres = (HTHost *) HTList_nextObject(cur))) {
		if (!strcmp(pres->hostname, host)) {
		    if (time(NULL) > pres->ntime + HostTimeout) {
			if (CORE_TRACE)
			    HTTrace("Host info... Collecting host %p\n", pres);
			delete_object(list, pres);
			pres = NULL;
  		    } else {
			if (CORE_TRACE)
			    HTTrace("Host info... Found `%s\'\n", host);
		    }
		    return pres;
		}
	    }
	}
    }
    return NULL;
}

/*
**	Get and set the hostname of the remote host
*/
PUBLIC char * HTHost_name (HTHost * host)
{
     return host ? host->hostname : NULL;
}

/*
**	Get and set the type class of the remote host
*/
PUBLIC char * HTHost_class (HTHost * host)
{
     return host ? host->type : NULL;
}

PUBLIC void HTHost_setClass (HTHost * host, char * s_class)
{
    if (host && s_class) StrAllocCopy(host->type, s_class);
}

/*
**	Get and set the version of the remote host
*/
PUBLIC int HTHost_version (HTHost *host)
{
     return host ? host->version : 0;
}

PUBLIC void HTHost_setVersion (HTHost * host, int version)
{
    if (host) host->version = version;
}

/*
**	Get and set the cache timeout for persistent entries.
**	The default value is TCP_TIMEOUT
*/
PUBLIC void HTHost_setPersistTimeout (time_t timeout)
{
    TCPTimeout = timeout;
}

PUBLIC time_t HTHost_persistTimeout (time_t timeout)
{
    return TCPTimeout;
}

/*	Persistent Connection Expiration
**	--------------------------------
**	Should normally not be used. If, then use calendar time.
*/
PUBLIC void HTHost_setPersistExpires (HTHost * host, time_t expires)
{
    if (host) host->expires = expires;
}

PUBLIC time_t HTHost_persistExpires (HTHost * host)
{
    return host ? host->expires : -1;
}

PUBLIC void HTHost_setReqsPerConnection (HTHost * host, int reqs)
{
    if (host) host->reqsPerConnection = reqs;
}

PUBLIC int HTHost_reqsPerConnection (HTHost * host)
{
    return host ? host->reqsPerConnection : -1;
}

PUBLIC void HTHost_setReqsMade (HTHost * host, int reqs)
{
    if (host) host->reqsMade = reqs;
}

PUBLIC int HTHost_reqsMade (HTHost * host)
{
    return host ? host->reqsMade : -1;
}


/*
**	Public methods for this host
*/
PUBLIC HTMethod HTHost_publicMethods (HTHost * me)
{
    return me ? me->methods : METHOD_INVALID;
}

PUBLIC void HTHost_setPublicMethods (HTHost * me, HTMethod methodset)
{
    if (me) me->methods = methodset;
}

PUBLIC void HTHost_appendPublicMethods (HTHost * me, HTMethod methodset)
{
    if (me) me->methods |= methodset;
}

/*
**	Get and set the server name of the remote host
*/
PUBLIC char * HTHost_server (HTHost * host)
{
     return host ? host->server : NULL;
}

PUBLIC BOOL HTHost_setServer (HTHost * host, const char * server)
{
    if (host && server) {
	StrAllocCopy(host->server, server);
	return YES;
    }
    return NO;
}

/*
**	Get and set the userAgent name of the remote host
*/
PUBLIC char * HTHost_userAgent (HTHost * host)
{
     return host ? host->user_agent : NULL;
}

PUBLIC BOOL HTHost_setUserAgent (HTHost * host, const char * userAgent)
{
    if (host && userAgent) {
	StrAllocCopy(host->user_agent, userAgent);
	return YES;
    }
    return NO;
}

/*
**	Get and set acceptable range units
*/
PUBLIC char * HTHost_rangeUnits (HTHost * host)
{
     return host ? host->range_units : NULL;
}

PUBLIC BOOL HTHost_setRangeUnits (HTHost * host, const char * units)
{
    if (host && units) {
	StrAllocCopy(host->range_units, units);
	return YES;
    }
    return NO;
}

/*
**	Checks whether a specific range unit is OK. We always say
**	YES except if we have a specific statement from the server that
**	it doesn't understand byte ranges - that is - it has sent "none"
**	in a "Accept-Range" response header
*/
PUBLIC BOOL HTHost_isRangeUnitAcceptable (HTHost * host, const char * unit)
{
    if (host && unit) {
#if 0
	if (host->range_units) {
	    char * start = strcasestr(host->range_units, "none");

	    /*
	    **  Check that "none" is infact a token. It could be part of some
	    **  other valid string, so we'd better check for it.
	    */
	    if (start) {
		
		
	    }
	    return NO;
	}
#endif
	return strcasecomp(unit, "bytes") ? NO : YES;
    }
    return NO;
}

/*	HTHost_catchClose
**	-----------------
**	This function is registered when the socket is idle so that we get
**	a notification if the socket closes at the other end. At this point
**	we can't use the request object as it might have been freed a long
**	time ago.
*/
PUBLIC int HTHost_catchClose (SOCKET soc, void * context, HTEventType type)
{
    HTNet * net = (HTNet *)context;
    HTHost * host = net->host;
    if (CORE_TRACE)
	HTTrace("Catch Close. called with socket %d with type %x\n",
		soc, type);
    if (type == HTEvent_READ) {
	HTChannel * ch = HTChannel_find(soc);	  /* Find associated channel */
	HTHost * host = HTChannel_host(ch);	      /* and associated host */
	if (ch && host) {	    
	    if (CORE_TRACE) HTTrace("Catch Close. CLOSING socket %d\n", soc);
	    HTHost_clearChannel(host, HT_OK);
	} else {
	    if (CORE_TRACE) HTTrace("Catch Close. socket %d NOT FOUND!\n",soc);
	}
    }
    HTHost_unregister(host, net, HTEvent_CLOSE);
    return HT_OK;
}

/*
**	As soon as we know that this host accepts persistent connections,
**	we associated the channel with the host. 
**	We don't want more than MaxSockets-2 connections to be persistent in
**	order to avoid deadlock.
*/
PUBLIC BOOL HTHost_setPersistent (HTHost *		host,
				  BOOL			persistent,
				  HTTransportMode	mode)
{
    if (!host) return NO;

    if (!persistent) {
	/*
	**  We use the HT_IGNORE status code as we don't want to free
	**  the stream at this point in time. The situation we want to
	**  avoid is that we free the channel from within the stream pipe.
	**  This will lead to an infinite look having the stream freing
	**  itself.
	*/
	return HTHost_clearChannel(host, HT_IGNORE);
    }

    /*
    ** Set the host persistent if not already. Also update the mode to
    ** the new one - it may have changed
    */
    HTHost_setMode(host, mode);
    if (!host->persistent) {
	SOCKET sockfd = HTChannel_socket(host->channel);
	if (sockfd != INVSOC && HTNet_availablePersistentSockets() > 0) {
	    host->persistent = YES;
	    host->expires = time(NULL) + TCPTimeout;	  /* Default timeout */
	    HTChannel_setHost(host->channel, host);
	    HTNet_increasePersistentSocket();
	    if (CORE_TRACE)
		HTTrace("Host info... added host %p as persistent\n", host);
	    return YES;
	} else {
	    if (CORE_TRACE)
		HTTrace("Host info... no room for persistent socket %d\n",
			sockfd);
	    return NO;
	}
    } else {
	if (CORE_TRACE) HTTrace("Host info... %p already persistent\n", host);
	return YES;
    }
    return NO;
}

/*
**	Check whether we have a persistent channel or not
*/
PUBLIC BOOL HTHost_isPersistent (HTHost * host)
{
    return host && host->persistent;
}

/*
**	Find persistent channel associated with this host.
*/
PUBLIC HTChannel * HTHost_channel (HTHost * host)
{
    return host ? host->channel : NULL;
}

/*
**	Clear the persistent entry by deleting the channel object. Note that
**	the channel object is only deleted if it's not used anymore.
*/
PUBLIC BOOL HTHost_clearChannel (HTHost * host, int status)
{
    if (host && host->channel) {
	HTChannel_setHost(host->channel, NULL);
	
	HTEvent_unregister(HTChannel_socket(host->channel), HTEvent_READ);
	HTEvent_unregister(HTChannel_socket(host->channel), HTEvent_WRITE);
	host->registeredFor = 0;

	/*
	**  We don't want to recursively delete ourselves so if we are
	**  called from within the stream pipe then don't delete the channel
	**  at this point
	*/
	HTChannel_delete(host->channel, status);
	host->expires = 0;	
	host->channel = NULL;
	host->tcpstate = TCP_BEGIN;
	host->reqsMade = 0;
	if (HTHost_isPersistent(host)) HTNet_decreasePersistentSocket();
	if (CORE_TRACE) HTTrace("Host info... removed host %p as persistent\n", host);
	return YES;
    }
    return NO;
}

/*
**	Move all entries in the pipeline and move the rest to the pending
**	queue. They will get launched at a later point in time.
*/
PUBLIC BOOL HTHost_recoverPipe (HTHost * host)
{
    if (host) {
	int piped = HTList_count(host->pipeline);
	if (piped > 0) {
	    int cnt;
	    host->recovered++;
	    if (CORE_TRACE)
		HTTrace("Host recovered %d times. Moving %d Net objects from pipe line to pending queue\n",
			host->recovered, piped);
	    
	    /*
	    **  Unregister this host for all events
	    */
	    HTEvent_unregister(HTChannel_socket(host->channel), HTEvent_READ);
	    HTEvent_unregister(HTChannel_socket(host->channel), HTEvent_WRITE);
	    host->registeredFor = 0;

	    /*
	    **  Set new mode to single until we know what is going on
	    */
	    host->mode = HT_TP_SINGLE;

	    /*
	    **  Move all net objects from the net object to the pending queue.
	    */
	    if (!host->pending) host->pending = HTList_new();
	    for (cnt=0; cnt<piped; cnt++) {
		HTNet * net = HTList_removeLastObject(host->pipeline);
		if (CORE_TRACE) HTTrace("Host recover Resetting net object %p\n", net);
		net->registeredFor = 0;
		(*net->event.cbf)(HTChannel_socket(host->channel), net->event.param, HTEvent_RESET);
		HTList_appendObject(host->pending, net);
	    }
	    HTChannel_setSemaphore(host->channel, 0);
	    HTHost_clearChannel(host, HT_INTERRUPTED);
	}
#if 0
	/*
	**  We don't wanna change state here
	*/
	return HTHost_launchPending(host);
#else
	return YES;
#endif
    }
    return NO;
}

/*
**	Handle the connection mode. The mode may change mode in the 
**	middle of a connection.
*/
PUBLIC HTTransportMode HTHost_mode (HTHost * host, BOOL * active)
{
    return host ? host->mode : HT_TP_SINGLE;
}

/*
**	If the new mode is lower than the old mode then adjust the pipeline
**	accordingly. That is, if we are going into single mode then move
**	all entries in the pipeline and move the rest to the pending
**	queue. They will get launched at a later point in time.
*/
PUBLIC BOOL HTHost_setMode (HTHost * host, HTTransportMode mode)
{
    if (host) {
	/*
	**  Check the new mode and see if we must adjust the queues.
	*/
	if (mode == HT_TP_SINGLE && host->mode > mode) {
	    int piped = HTList_count(host->pipeline);
	    if (piped > 0) {
		int cnt;
		if (CORE_TRACE)
		    HTTrace("Host info... Moving %d Net objects from pipe line to pending queue\n", piped);
		if (!host->pending) host->pending = HTList_new();
		for (cnt=0; cnt<piped; cnt++) {
		    HTNet * net = HTList_removeLastObject(host->pipeline);
		    if (CORE_TRACE) HTTrace("Host info... Resetting net object %p\n", net);
		    (*net->event.cbf)(HTChannel_socket(host->channel), net->event.param, HTEvent_RESET);
		    HTList_appendObject(host->pending, net);
		}
		HTChannel_setSemaphore(host->channel, 0);
		HTHost_clearChannel(host, HT_INTERRUPTED);
	    }
	}

	/*
	**  If we know that this host is bad then we don't allow anything than
	**  single mode. We can't recover connections for the rest of our life
	*/
	if (mode == HT_TP_PIPELINE && host->recovered > MAX_HOST_RECOVER) {
	    if (PROT_TRACE)
		HTTrace("Host info... %p is bad for pipelining so we won't do it!!!\n",
			host);
	} else {
	    host->mode = mode;
	    if (PROT_TRACE)
		HTTrace("Host info... New mode is %d for host %p\n", host->mode, host);
	}
    }
    return NO;
}

/*
**	Check whether a host is idle meaning if it is ready for a new
**	request which depends on the mode of the host. If the host is 
**	idle, i.e. ready for use then return YES else NO. If the host supports
**	persistent connections then still only return idle if no requests are
**	ongoing. 
*/
PUBLIC BOOL HTHost_isIdle (HTHost * host)
{
    return (host && HTList_count(host->pipeline) <= 0);
}

PRIVATE BOOL _roomInPipe (HTHost * host)
{
    int count;
    if (!host) return NO;
    if (host->reqsPerConnection && host->reqsMade >= host->reqsPerConnection)
	return 0;
    count = HTList_count(host->pipeline);
    switch (host->mode) {
    case HT_TP_SINGLE:
	return count <= 0;
    case HT_TP_PIPELINE:
	return count < MAX_PIPES;
    case HT_TP_INTERLEAVE:
	return YES;
    }
    return NO;
}

/*
**	Add a net object to the host object. If the host
**	is idle then add to active list (pipeline) else add
**	it to the pending list
**	Return HT_PENDING if we must pend, HT_OK, or HT_ERROR
*/
PUBLIC int HTHost_addNet (HTHost * host, HTNet * net)
{
    if (host && net) {
	int status = HT_OK;

	/*
	**  If we don't have a socket already then check to see if we can get
	**  one. Otherwise we put the host object into our pending queue.
	*/	
	if (!host->channel && HTNet_availableSockets() <= 0) {
	    if (!PendHost) PendHost = HTList_new();
	    if (CORE_TRACE)
		HTTrace("Host info... Add Host %p as pending\n", host);
	    HTList_addObject(PendHost, host);
	    status = HT_PENDING;
	}

	/*
	**  Add net object to either active or pending queue.
	*/
	if (_roomInPipe(host)) {
	    if (CORE_TRACE) HTTrace("Host info... Add Net %p to pipeline of host %p\n", net, host);
	    if (!host->pipeline) host->pipeline = HTList_new();
	    HTList_addObject(host->pipeline, net);

	    /*
	    ** Send out the request if we're not blocked on write
	    */
#if 0
	    if (!(host->registeredFor & HTEvent_BITS(HTEvent_WRITE)))
		status = HTHost_launchPending(host) == YES ? HT_OK : HT_ERROR;
#endif
	} else {
	    if (CORE_TRACE) HTTrace("Host info... Add Net %p as pending\n", net);
	    if (!host->pending) host->pending = HTList_new();
	    HTList_addObject(host->pending, net);
	    status = HT_PENDING;
	}
	return status;
    }
    return HT_ERROR;
}

PUBLIC BOOL HTHost_free (HTHost * host, int status)
{
    if (host->channel == NULL) return NO;
#if 0
    if (host->persistent && !(host->reqsMade >= host->reqsPerConnection && HTList_count(host->pipeline) <= 1))
#else
    /* Check this with FTP as well */
    if (host->persistent &&
	(!host->reqsPerConnection ||
	 (host->reqsPerConnection && host->reqsMade < host->reqsPerConnection)))
#endif
    {
	if (CORE_TRACE)
	    HTTrace("Host Object. keeping socket %d\n", HTChannel_socket(host->channel));
	HTChannel_delete(host->channel, status);
    } else {
	if (CORE_TRACE)
	    HTTrace("Host Object. closing socket %d\n", HTChannel_socket(host->channel));

	/* 
	**  By lowering the semaphore we make sure that the channel
	**  is gonna be deleted
	*/
#if 1
	HTChannel_downSemaphore(host->channel);
	HTHost_clearChannel(host, status);
#else
	HTEvent_unregister(HTChannel_socket(host->channel), HTEvent_READ);
	HTEvent_unregister(HTChannel_socket(host->channel), HTEvent_WRITE);
	host->registeredFor = 0;
	HTChannel_downSemaphore(host->channel);
	HTChannel_delete(host->channel, status);
	host->channel = NULL;
	host->tcpstate = TCP_BEGIN;
#endif
    }
    return YES;
}

PUBLIC BOOL HTHost_deleteNet (HTHost * host, HTNet * net)
{
    if (host && net) {
	if (CORE_TRACE)
	    HTTrace("Host info... Remove Net %p from pipe line\n", net);
	HTList_removeObject(host->pipeline, net);
	HTList_removeObject(host->pending, net);
	return YES;
    }
    return NO;
}

/*
**	Handle pending host objects.
**	There are two ways we can end up with pending reqyests:
**	 1) If we are out of sockets then register new host objects as pending.
**	 2) If we are pending on a connection then register new net objects as
**	    pending
**	This set of functions handles pending host objects and can start new
**	requests as resources get available
*/

/*
**	Check this host object for any pending requests and return the next
**	registered Net object.
*/
PUBLIC HTNet * HTHost_nextPendingNet (HTHost * host)
{
    HTNet * net = NULL;
    if (host && host->pending && host->pipeline) {
	/*JK 23/Sep/96 Bug correction. Associated the following lines to the
	**above if. There was a missing pair of brackets. 
	*/
	if ((net = (HTNet *) HTList_removeFirstObject(host->pending)) != NULL) {
	  if (PROT_TRACE)
	      HTTrace("Host info... Popping %p from pending net queue\n", net);
/*	  HTList_addObject(host->pipeline, net); */
	}
    }
    return net;
}

/*
**	Return the current list of pending host objects waiting for a socket
*/
PUBLIC HTHost * HTHost_nextPendingHost (void)
{
    HTHost * host = NULL;
    if (PendHost) {
	if ((host = (HTHost *) HTList_removeFirstObject(PendHost)) != NULL)
	    if (PROT_TRACE)
		HTTrace("Host info... Poping %p from pending host queue\n",
			host);
    }
    return host;
}

/*
**	Start the next pending request if any. First we look for pending
**	requests for the same host and then we check for any other pending
**	hosts
*/
PUBLIC BOOL HTHost_launchPending (HTHost * host)
{
    int available = HTNet_availableSockets();

    if (!host) {
	if (PROT_TRACE) HTTrace("Host info... Bad arguments\n");
	return NO;
    }

    /*
    **  Check if we do have resources available for a new request
    **  This can either be reusing an existing connection or opening a new one
    */
    if (available > 0 || host->mode >= HT_TP_PIPELINE) {
	HTNet * net;

	/*
	**  In pipeline we can only have one doing writing at a time.
	**  We therefore check that there are no other Net object
	**  registered for write
	*/
	if (host->mode == HT_TP_PIPELINE) {
	    net = (HTNet *) HTList_lastObject(host->pipeline);
	    if (net && net->registeredFor == HTEvent_WRITE)
		return NO;
	}

	/*
	**  Check the current Host object for pending Net objects
	**
	**  Send out as many as will fit in pipe.
	*/
	while (_roomInPipe(host) && (net = HTHost_nextPendingNet(host))) {
	    int status = HTNet_execute(net, HTEvent_WRITE);
	    if (status != HT_OK)
		return status;
	}

	/*
	**  Check for other pending Host objects
	*/
	{
	    HTHost * pending = HTHost_nextPendingHost();
	    if (pending) {
		HTNet * net = HTHost_nextPendingNet(pending);
		if (net) return HTNet_execute(net, HTEvent_WRITE);
	    }
	}
    } else
	if (PROT_TRACE) HTTrace("Host info... No more requests.\n");
    return NO;
}

PUBLIC HTNet * HTHost_firstNet (HTHost * host)
{
    return (HTNet *) HTList_firstObject(host->pipeline);
}

/*
**	The host event manager keeps track of the state of it's client engines
**	(typically HTTPEvent), accepting multiple blocks on read or write from
**	multiple pipelined engines. It then registers its own engine 
**	(HostEvent) with the event manager.
*/
PUBLIC int HTHost_connect (HTHost * host, HTNet * net, char * url, HTProtocolId port)
{
    int status;
/*    if (host && host->connecttime)
	return HT_OK;
*/
    status = HTDoConnect(net, url, port);
    if (status == HT_OK) {
	HTNet_host(net)->reqsMade++;	/* @@@ - what if there's a connect but no req sent? */
	return HT_OK;
    }
    if (status == HT_WOULD_BLOCK || status == HT_PENDING)
	return HT_WOULD_BLOCK;
    return HT_ERROR; /* @@@ - some more deletion and stuff here? */
}

/*
**	Rules: SINGLE: one element in pipe, either reading or writing
**		 PIPE: n element in pipe, n-1 reading, 1 writing
*/
PUBLIC int HTHost_register (HTHost * host, HTNet * net, HTEventType type)
{
    if (host && net) {

	if (type == HTEvent_CLOSE) {

	    /*
	    **  Unregister this host for all events
	    */
	    HTEvent_unregister(HTChannel_socket(host->channel), HTEvent_READ);
	    HTEvent_unregister(HTChannel_socket(host->channel), HTEvent_WRITE);
	    host->registeredFor = 0;
	    return YES;

	} else {

	    /* net object may already be registered */
	    if (HTEvent_BITS(type) & net->registeredFor)
		return NO;
	    net->registeredFor ^= HTEvent_BITS(type);

	    /* host object may already be registered */
	    if (host->registeredFor & HTEvent_BITS(type))
		return YES;
	    host->registeredFor ^= HTEvent_BITS(type);
	    return HTEvent_register(HTChannel_socket(host->channel),
				    type, *(host->events+HTEvent_INDEX(type)));
	}
    }
    return NO;
}

PUBLIC int HTHost_unregister (HTHost * host, HTNet * net, HTEventType type)
{
    if (host && net) {

	/* net object may not be registered */
	if (!(HTEvent_BITS(type) & net->registeredFor))
	    return NO;
	net->registeredFor ^= HTEvent_BITS(type);

	/* host object may not be registered */
	if (!(host->registeredFor & HTEvent_BITS(type)))
	    return YES;
	host->registeredFor ^= HTEvent_BITS(type);

	/* stay registered for READ to catch a socket close */
	/* WRITE and CONNECT can be unregistered, though */
	if ((type == HTEvent_WRITE && isLastInPipe(host, net)) || 
	    type == HTEvent_CONNECT)
	    /* if we are blocked downstream, shut down the whole pipe */
	    HTEvent_unregister(HTChannel_socket(host->channel), type);
	return YES;
    }
    return NO;
}

/*
**	The reader tells HostEvent that it's stream did not finish the data
*/
PUBLIC BOOL HTHost_setRemainingRead (HTHost * host, size_t remaining)
{
    if (host == NULL) return NO;
    host->remainingRead = remaining;
    if (PROT_TRACE) HTTrace("Host........ %d bytes remaining \n", remaining);
    return YES;
}

PUBLIC SockA * HTHost_getSockAddr (HTHost * host)
{
    if (!host) return NULL;
    return &host->sock_addr;
}

PUBLIC BOOL HTHost_setHome (HTHost * host, int home)
{
    if (!host) return NO;
    host->home = home;
    return YES;
}

PUBLIC int HTHost_home (HTHost * host)
{
    if (!host) return 0;
    return host->home;
}

PUBLIC BOOL HTHost_setRetry (HTHost * host, int retry)
{
    if (!host) return NO;
    host->retry = retry;
    return YES;
}

PUBLIC BOOL HTHost_decreaseRetry (HTHost * host)
{
    if (!host) {
	if (host->retry > 0) host->retry--;
	return YES;
    }
    return NO;
}

PUBLIC int HTHost_retry (HTHost * host)
{
    if (!host) return 0;
    return host->retry;
}

#if 0	/* Is a macro right now */
PRIVATE BOOL HTHost_setDNS5 (HTHost * host, HTdns * dns)
{
    if (!host) return NO;
    host->dns = dns;
    return YES;
}
#endif

PUBLIC BOOL HTHost_setChannel (HTHost * host, HTChannel * channel)
{
    if (!host) return NO;
    host->channel = channel;
    return YES;
}

PUBLIC HTNet * HTHost_getReadNet(HTHost * host)
{
    if (host) {
	if (host->mode == HT_TP_INTERLEAVE) {
#ifdef HT_MUX
	    HTMuxChannel * muxch = HTMuxChannel_find(host);
	    return HTMuxChannel_net(muxch);
#endif
	}
	return (HTNet *) HTList_firstObject(host->pipeline);
    }
    return NULL;
}

PUBLIC HTNet * HTHost_getWriteNet(HTHost * host)
{
    return host ? (HTNet *) HTList_lastObject(host->pipeline) : NULL;
}

/*
**	Create the input stream and bind it to the channel
**	Please read the description in the HTIOStream module for the parameters
*/
PUBLIC HTInputStream * HTHost_getInput (HTHost * host, HTTransport * tp,
					void * param, int mode)
{
    if (host && host->channel && tp) {
	HTChannel * ch = host->channel;
	HTInputStream * input = (*tp->input_new)(host, ch, param, mode);
	HTChannel_setInput(ch, input);
	return HTChannel_getChannelIStream(ch);
    }
    if (CORE_TRACE) HTTrace("Host Object. Can't create input stream\n");
    return NULL;
}

PUBLIC HTOutputStream * HTHost_getOutput (HTHost * host, HTTransport * tp,
					  void * param, int mode)
{
    if (host && host->channel && tp) {
	HTChannel * ch = host->channel;
	HTOutputStream * output = (*tp->output_new)(host, ch, param, mode);
	HTChannel_setOutput(ch, output);
	return output;
    }
    if (CORE_TRACE) HTTrace("Host Object. Can't create output stream\n");
    return NULL;
}

PUBLIC HTOutputStream * HTHost_output (HTHost * host, HTNet * net)
{
    if (host && host->channel && net) {
	HTOutputStream * output = HTChannel_output(host->channel);

	/*
	**  If we are in MUX mode then create new output stream on top
	**  of the already existing one. Otherwise just return what we
	**  have.
	*/
	if (host->mode == HT_TP_INTERLEAVE) {
#ifdef HT_MUX
	    HTStream * target = (HTStream *) HTChannel_output(host->channel);
	    output = HTMuxWriter_new(host, net, target);
#endif
	}
	return output;
    }
    return NULL;
}

PUBLIC int HTHost_read(HTHost * host, HTNet * net)
{
    HTInputStream * input = HTChannel_input(host->channel);
    if (net != HTHost_getReadNet(host)) {
	HTHost_register(host, net, HTEvent_READ);
	return HT_WOULD_BLOCK;
    }

    /*
    **  If there is no input channel then this can either mean that
    **  we have lost the channel or an error occurred. We return
    **  HT_CLOSED as this is a sign to the caller that we don't 
    **  have a channel
    */
    return input ? (*input->isa->read)(input) : HT_CLOSED;
}

PUBLIC BOOL HTHost_setConsumed(HTHost * host, size_t bytes)
{
    HTInputStream * input;
    if (!host || !host->channel) return NO;
    if ((input = HTChannel_input(host->channel)) == NULL)
	return NO;
    if (PROT_TRACE)
	HTTrace("Host........ passing %d bytes as consumed to %p\n", bytes, input);
    return (*input->isa->consumed)(input, bytes);
}

PUBLIC int HTHost_hash (HTHost * host)
{
    return host ? host->hash : -1;
}

PUBLIC BOOL HTHost_setWriteDelay (HTHost * host, ms_t delay)
{
    if (host && delay >= 0) {
	host->delay = delay;
	return YES;
    }
    return NO;
}

PUBLIC ms_t HTHost_writeDelay (HTHost * host)
{
    return host ? host->delay : 0;
}

PUBLIC int HTHost_findWriteDelay (HTHost * host, ms_t lastFlushTime, int buffSize)
{
    unsigned short mtu;
    int ret = -1;
    int socket = HTChannel_socket(host->channel);
#ifndef WWW_MSWINDOWS
    ret = ioctl(socket, 666, (unsigned long)&mtu);
#endif /* WWW_MSWINDOWS */
    if ((ret == 0 && buffSize >= mtu) || host->forceWriteFlush)
	return 0;
    return host->delay;
}

PUBLIC BOOL HTHost_setDefaultWriteDelay (ms_t delay)
{
    if (delay >= 0) {
	WriteDelay = delay;
	if (CORE_TRACE) HTTrace("Host........ Default write delay is %d ms\n", delay);
	return YES;
    }
    return NO;
}

PUBLIC ms_t HTHost_defaultWriteDelay (void)
{
    return WriteDelay;
}

PUBLIC int HTHost_forceFlush(HTHost * host)
{
    HTNet * targetNet = (HTNet *)HTList_lastObject(host->pipeline);
    int wasForced = host->forceWriteFlush;
    int ret;
    if (targetNet == NULL)
	return HT_ERROR;
    if (CORE_TRACE)
	HTTrace("Host Event.. FLUSH passed to `%s\'\n", 
		HTAnchor_physical(HTRequest_anchor(HTNet_request(targetNet))));
    host->forceWriteFlush = YES;
    ret = (*targetNet->event.cbf)(HTChannel_socket(host->channel), targetNet->event.param, HTEvent_FLUSH);
    host->forceWriteFlush = wasForced;
    return ret;
}

PUBLIC int HTHost_eventTimeout (void)
{
    return EventTimeout;
}

PUBLIC void HTHost_setEventTimeout (int millis)
{
    EventTimeout = millis;
    if (CORE_TRACE) HTTrace("Host........ Setting event timeout to %d ms\n", millis);
}
