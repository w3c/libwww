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
#include "HTHost.h"					 /* Implemented here */

#define HOST_TIMEOUT		43200L	     /* Default host timeout is 12 h */
#define TCP_TIMEOUT		3600L		/* Default TCP timeout i 1 h */
#define HASH_SIZE		67

/* Type definitions and global variables etc. local to this module */
struct _HTHost {
    char *  		hostname;	     /* name of host + optional port */
    time_t		ntime;				    /* Creation time */
    char *		type;				        /* Peer type */
    int 		version;			     /* Peer version */
    HTMethod		methods;	       	/* Public methods (bit-flag) */
    char *		server;				      /* Server name */
    char *		user_agent;			       /* User Agent */
    HTTransportMode	mode;	      			   /* Supported mode */
    HTChannel *		channel;		       /* Persistent channel */
    HTList *		pipeline;		 /* Pipe line of net objects */
    HTList *		pending;	      /* List of pending Net objects */
    time_t		expires;	  /* Persistent channel expires time */
};

PRIVATE time_t	HostTimeout = HOST_TIMEOUT;	  /* Timeout on host entries */
PRIVATE time_t	TCPTimeout = TCP_TIMEOUT;  /* Timeout on persistent channels */

PRIVATE HTList	** HostTable = NULL;
PRIVATE HTList * PendHost = NULL;	    /* List of pending host elements */

/* ------------------------------------------------------------------------- */

PRIVATE void free_object (HTHost * me)
{
    if (me) {
	HT_FREE(me->hostname);
	HT_FREE(me->type);
	if (me->channel) {
	    HTChannel_delete(me->channel, HT_OK);
	    me->channel = NULL;
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
PUBLIC HTHost * HTHost_new (char * host)
{
    HTList * list = NULL;			    /* Current list in cache */
    HTHost * pres = NULL;
    if (!host) {
	if (CORE_TRACE) HTTrace("Host info... Bad argument\n");
	return NULL;
    }
    
    /* Find a hash for this host */
    {
	int hash = 0;
	char *ptr;
	for (ptr=host; *ptr; ptr++)
	    hash = (int) ((hash * 3 + (*(unsigned char *) ptr)) % HASH_SIZE);
	if (!HostTable) {
	    if ((HostTable = (HTList **) HT_CALLOC(HASH_SIZE,
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
	    if (!strcmp(pres->hostname, host)) {
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
	    if (pres->expires < time(NULL)) {	   /* Cached channel is cold */
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
	StrAllocCopy(pres->hostname, host);
	pres->ntime = time(NULL);
	pres->mode = HT_TP_SINGLE;
	if (CORE_TRACE) 
	    HTTrace("Host info... added `%s\' to list %p\n", host, list);
	HTList_addObject(list, (void *) pres);
    }
    return pres;
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
	    hash = (int) ((hash * 3 + (*(unsigned char *) ptr)) % HASH_SIZE);
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

/*	HTHost_catchClose
**	-----------------
**	This function is registered when the socket is idle so that we get
**	a notification if the socket closes at the other end. At this point
**	we can't use the request object as it might have been freed a long
**	time ago.
*/
PUBLIC int HTHost_catchClose (SOCKET soc, HTRequest * request, SockOps ops)
{
    if (CORE_TRACE)
	HTTrace("Catch Close. called with socket %d with ops %x\n",
		soc, (unsigned) ops);
    if (ops == FD_READ) {
	HTChannel * ch = HTChannel_find(soc);	  /* Find associated channel */
	HTHost * host = HTChannel_host(ch);	      /* and associated host */
	if (ch && host) {	    
	    if (CORE_TRACE) HTTrace("Catch Close. CLOSING socket %d\n", soc);
	    HTHost_clearChannel(host, HT_OK);
	} else {
	    if (CORE_TRACE) HTTrace("Catch Close. socket %d NOT FOUND!\n",soc);
	}
    }
    HTEvent_unregister(soc, (SockOps) FD_ALL);
    return HT_OK;
}

/*
**	As soon as we know that this host accepts persistent connections,
**	we associated the channel with the host. 
**	We don't want more than MaxSockets-2 connections to be persistent in
**	order to avoid deadlock.
*/
PUBLIC BOOL HTHost_setChannel (HTHost *		host,
			       HTChannel *	channel,
			       HTTransportMode	mode)
{
    if (!host || !channel) return NO;
    if (host->channel) {
	if (CORE_TRACE) HTTrace("Host info... %p already persistent\n", host);
	return YES;
    } else {
	SOCKET sockfd = HTChannel_socket(channel);
	if (sockfd != INVSOC && HTNet_availablePersistentSockets() > 0) {
	    host->channel = channel;
	    host->mode = mode;
	    host->expires = time(NULL) + TCPTimeout;	  /* Default timeout */
	    HTChannel_setHost(channel, host); 
	    HTNet_increasePersistentSocket();
	    if (CORE_TRACE)
		HTTrace("Host info... added host %p as persistent\n", host);
	    return YES;
	} else {
	    if (CORE_TRACE)
		HTTrace("Host info... no room for persistent socket %d\n",
			sockfd);
	}
    }
    return NO;
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
	HTChannel_delete(host->channel, status);
	host->expires = 0;
	host->channel = NULL;
	HTNet_decreasePersistentSocket();
	if (CORE_TRACE)
	    HTTrace("Host info... removed host %p as persistent\n", host);
	return YES;
    }
    return NO;
}

/*
**	Check whether we have a persistent channel or not
*/
PUBLIC BOOL HTHost_isPersistent (HTHost * host)
{
    return host && host->channel;
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
		    HTNet * net = HTList_removeFirstObject(host->pipeline);
		    HTList_appendObject(host->pending, net);
		}
	    }
	}
	host->mode = mode;
	return YES;
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

	/* Check to see if we can get a socket */
	if (HTNet_availableSockets() <= 0) {
	    if (!PendHost) PendHost = HTList_new();
	    if (CORE_TRACE)
		HTTrace("Host info... Add Host %p as pending\n", host);
	    HTList_addObject(PendHost, host);
	    status = HT_PENDING;
	}

	/* Add to either active or pending queue */
	if (HTHost_isIdle(host)) {
	    if (CORE_TRACE) HTTrace("Host info... Add Net %p to pipeline of host %p\n", net, host);
	    if (!host->pipeline) host->pipeline = HTList_new();
	    HTList_addObject(host->pipeline, net);
	    
	    /*
	    **  We have been idle and must hence unregister our catch close
	    **  event handler
	    */
	    if (host->channel) {
		SOCKET sockfd = HTChannel_socket(host->channel);
		HTEvent_unregister(sockfd, (SockOps) FD_CLOSE);
	    }
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
	if ((net = (HTNet *) HTList_removeFirstObject(host->pending)) != NULL)
	    if (PROT_TRACE)
		HTTrace("Host info... Popping %p from pending net queue\n",
			net);
	HTList_addObject(host->pipeline, net);
    }
    return net;
}

/*
**	Return the current list of pending host obejcts waiting for a socket
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

	/*
	**  Check the current Host obejct for pending Net objects
	*/
	if (host) {
	    HTNet * net = HTHost_nextPendingNet(host);
	    if (net) return HTNet_start(net);
	}

	/*
	**  Check for other pending Host objects
	*/
	{
	    HTHost * pending = HTHost_nextPendingHost();
	    if (pending) {
		HTNet * net = HTHost_nextPendingNet(pending);
		if (net) return HTNet_start(net);
	    }
	}

	/*
	**  If nothing pending then register our catch close event handler to
	**  have something catching the socket if the remote server closes the
	**  connection, for example due to timeout.
	*/
	if (PROT_TRACE) HTTrace("Host info... Nothing pending\n");
	if (host->channel) {
	    SOCKET sockfd = HTChannel_socket(host->channel);
	    HTEvent_register(sockfd, 0, (SockOps) FD_CLOSE,
			     HTHost_catchClose,  HT_PRIORITY_MAX);
	}
    } else
	if (PROT_TRACE) HTTrace("Host info... No available sockets\n");
    return NO;
}

