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
#include "wwwsys.h"
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

#define HOST_OBJECT_TTL	    43200L	     /* Default host timeout is 12 h */

#define TCP_IDLE_PASSIVE      120L /* Passive TTL in s on an idle connection */
#define TCP_IDLE_ACTIVE     60000L /* Active TTL in ms on an idle connection */

#define MAX_PIPES		50   /* maximum number of pipelined requests */
#define MAX_HOST_RECOVER	1	      /* Max number of auto recovery */
#define DEFAULT_DELAY		30	  /* Default write flush delay in ms */

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

PRIVATE int HostEvent(SOCKET soc, void * pVoid, HTEventType type);

/* Type definitions and global variables etc. local to this module */
PRIVATE time_t	HostTimeout = HOST_OBJECT_TTL;	 /* Timeout for host objects */
PRIVATE time_t	HTPassiveTimeout = TCP_IDLE_PASSIVE; /* Passive timeout in s */
PRIVATE ms_t	HTActiveTimeout = TCP_IDLE_ACTIVE;   /* Active timeout in ms */

PRIVATE HTList	** HostTable = NULL;
PRIVATE HTList * PendHost = NULL;	    /* List of pending host elements */

/* JK: New functions for interruption the automatic pending request 
   activation */
PRIVATE HTHost_ActivateRequestCallback * ActivateReqCBF = NULL;
PRIVATE int HTHost_ActivateRequest (HTNet *net);
PRIVATE BOOL DoPendingReqLaunch = YES; /* controls automatic activation
                                              of pending requests */

PRIVATE int EventTimeout = -1;		        /* Global Host event timeout */

PRIVATE ms_t WriteDelay = DEFAULT_DELAY;		      /* Delay in ms */

PRIVATE int MaxPipelinedRequests = MAX_PIPES;

/* ------------------------------------------------------------------------- */

PRIVATE void free_object (HTHost * me)
{
    if (me) {
	int i;
	HT_FREE(me->hostname);
	HT_FREE(me->type);
	HT_FREE(me->server);
	HT_FREE(me->user_agent);
	HT_FREE(me->range_units);

	/* Delete the channel (if any) */
	if (me->channel) {
	    HTChannel_delete(me->channel, HT_OK);
	    me->channel = NULL;
	}

	/* Unregister the events */
	for (i = 0; i < HTEvent_TYPES; i++)
	    HTEvent_delete(me->events[i]);

	/* Delete the timer (if any) */
	if (me->timer) HTTimer_delete(me->timer);

	/* Delete the queues */
	HTList_delete(me->pipeline);
	HTList_delete(me->pending);
	HT_FREE(me);
    }
}

PRIVATE BOOL delete_object (HTList * list, HTHost * me)
{
    HTTRACE(CORE_TRACE, "Host info... object %p from list %p\n" _ me _ list);
    HTList_removeObject(list, (void *) me);
    free_object(me);
    return YES;
}

PRIVATE BOOL isLastInPipe (HTHost * host, HTNet * net)
{
    return HTList_lastObject(host->pipeline) == net;
}

PRIVATE BOOL killPipeline (HTHost * host, HTEventType type)
{
    if (host) {
	int piped = HTList_count(host->pipeline);
	int pending = HTList_count(host->pending);
	int cnt;

	HTTRACE(CORE_TRACE, "Host kill... Pipeline due to %s event\n" _ HTEvent_type2str(type));

	/* Terminate all net objects in pending queue */
	for (cnt=0; cnt<pending; cnt++) {
	    HTNet * net = HTList_removeLastObject(host->pending);
	    if (net) {
		HTTRACE(CORE_TRACE, "Host kill... Terminating net object %p from pending queue\n" _ net);
		net->registeredFor = 0;
		(*net->event.cbf)(HTChannel_socket(host->channel), net->event.param, type);
		if (host->lock == net) host->lock = NULL;
	    }
	}

	/* Terminate all net objects in pipeline */
	if (piped >= 1) {
	    
	    /*
	    **  Terminte all net objects in the pipeline
	    */
	    for (cnt=0; cnt<piped; cnt++) {
		HTNet * net = HTList_firstObject(host->pipeline);
		if (net) {
		    HTTRACE(CORE_TRACE, "Host kill... Terminating net object %p from pipe line\n" _ net);
		    net->registeredFor = 0;
		    (*net->event.cbf)(HTChannel_socket(host->channel), net->event.param, type);
		}
	    }
	    /* Richard Atterer (18/04/03): Close the connection, 
	     * to prevent the server from sending us the remainder
	     * of the data for the currently active request. */
	    HTChannel_setSemaphore(host->channel, 0);
	    HTHost_clearChannel(host, HT_INTERRUPTED);    
	}
	return YES;
    }
    return NO;
}

/*
**  Silently close an idle persistent connection after 
**  HTActiveTimeout secs
*/
PRIVATE int IdleTimeoutEvent (HTTimer * timer, void * param, HTEventType type)
{
    HTHost * host = (HTHost *) param;
    SOCKET sockfd = HTChannel_socket(host->channel);

    HTTimer_delete(timer);
    host->timer = NULL;

    return HostEvent (sockfd, host, HTEvent_CLOSE);
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

    if (type == HTEvent_READ || type == HTEvent_CLOSE || type == HTEvent_ACCEPT) {
	HTNet * targetNet;

	/* call the first net object */
	do {
	    int ret;

            /* netscape and apache servers can do a lazy close well after usage
             * of previous socket has been dispensed by the library,
             * the section below makes sure the event does not get miss attributed
             */
	    if (HTChannel_socket(host->channel) != soc && type != HTEvent_ACCEPT && !host->listening) {
		HTTRACE(CORE_TRACE, "Host Event.. wild socket %d type = %s real socket is %d\n" _ soc _ 
			type == HTEvent_CLOSE ? "Event_Close" : "Event_Read" _ 
			HTChannel_socket(host->channel));
		return HT_OK;
	    }

	    targetNet = (HTNet *)HTList_firstObject(host->pipeline);
	    if (targetNet) {
		HTTRACE(CORE_TRACE, "Host Event.. READ passed to `%s\'\n" _ 
			    HTAnchor_physical(HTRequest_anchor(HTNet_request(targetNet))));
		if ((ret = (*targetNet->event.cbf)(HTChannel_socket(host->channel), 
						  targetNet->event.param, type)) != HT_OK)
		    return ret;
	    }
	    if (targetNet == NULL && host->remainingRead > 0) {
		HTTRACE(CORE_TRACE, "HostEvent... Error: %d bytes left to read and nowhere to put them\n" _ 
			    host->remainingRead);
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
	HTTRACE(CORE_TRACE, "Host Event.. host %p `%s\' closed connection.\n" _ 
		    host _ host->hostname);

	/* Is there garbage in the channel? Let's check: */
	{
	    char buf[256];
	    int ret;
	    memset(buf, '\0', sizeof(buf));
	    if (HTChannel_socket(host->channel) != INVSOC) {
	      while ((ret = NETREAD(HTChannel_socket(host->channel), buf, sizeof(buf)-1)) > 0) {
		HTTRACE(CORE_TRACE, "Host Event.. Host %p `%s\' had %d extraneous bytes: `%s\'\n" _ 
			host _ host->hostname _ ret _ buf);
		memset(buf, '\0', sizeof(buf));		
	      }	    
	    }
	}
	HTHost_clearChannel(host, HT_OK);
	return HT_OK; 	     /* extra garbage does not constitute an application error */
	
    } else if (type == HTEvent_WRITE || type == HTEvent_CONNECT) {
	HTNet * targetNet = (HTNet *)HTList_lastObject(host->pipeline);
	if (targetNet) {
	    HTTRACE(CORE_TRACE, "Host Event.. WRITE passed to `%s\'\n" _ 
			HTAnchor_physical(HTRequest_anchor(HTNet_request(targetNet))));
	    return (*targetNet->event.cbf)(HTChannel_socket(host->channel), targetNet->event.param, type);
	}
	HTTRACE(CORE_TRACE, "Host Event Host %p (`%s\') dispatched with event %s but doesn't have a target - %d requests made, %d requests in pipe, %d pending\n" _ 
		    host _ 
		    host ? host->hostname : "<null>" _ 
		    HTEvent_type2str(type) _ 
		    host ? host->reqsMade : -1 _ 
		    HTList_count(host->pipeline) _ 
		    HTList_count(host->pending));
#if 0
	HTDEBUGBREAK("Host Event.. Host %p (`%s\') dispatched with event %d\n" _ 
		     host _ host ? host->hostname : "<null>" _ type);
	return HT_ERROR;
#else
	return HT_OK;
#endif
    } else if (type == HTEvent_TIMEOUT) {
	killPipeline(host, HTEvent_TIMEOUT);
    } else {
	HTTRACE(CORE_TRACE, "Don't know how to handle OOB data from `%s\'?\n" _
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
	HTTRACE(CORE_TRACE, "Host info... Bad argument\n");
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
		    HTTRACE(CORE_TRACE, "Host info... Collecting host info %p\n" _ pres);
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

            /*
            **  If we have a TTL for this TCP connection then
            **  check that we haven't passed it.
            */
            if (pres->expires > 0) {
                time_t t = time(NULL);
                if (HTHost_isIdle(pres) && pres->expires < t) {
                    HTTRACE(CORE_TRACE, "Host info... Persistent channel %p gotten cold\n" _ 
                        pres->channel);
		    HTHost_clearChannel(pres, HT_OK);
                } else {
                    pres->expires = t + HTPassiveTimeout;
                    HTTRACE(CORE_TRACE, "Host info... REUSING CHANNEL %p\n" _ pres->channel);
                }
            }
	} else {
	    HTTRACE(CORE_TRACE, "Host info... Found Host %p with no active channel\n" _ pres);
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
	pres->inFlush = NO;
	{
	    int i;
	    for (i = 0; i < HTEvent_TYPES; i++)
		pres->events[i]= HTEvent_new(HostEvent, pres, HT_PRIORITY_MAX, EventTimeout);
	}
	HTTRACE(CORE_TRACE, "Host info... added `%s\' with host %p to list %p\n" _ 
		    host _ pres _ list);
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
			   NULL, 0, "HTHost_newWParse");
	HT_FREE(fullhost);
	return NULL;
    }

    /* See if the default port should be overridden */
    if ((port = strchr(parsedHost, ':')) != NULL) {
	*port++ = '\0';
	if (*port && isdigit((int) *port)) u_port = (u_short) atol(port);
    }
    HTTRACE(PROT_TRACE, "HTHost parse Looking up `%s\' on port %u\n" _ parsedHost _ u_port);

    /* Find information about this host */
    if ((me = HTHost_new(parsedHost, u_port)) == NULL) {
	HTTRACE(PROT_TRACE, "HTHost parse Can't get host info\n");
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
    HTTRACE(CORE_TRACE, "Host info... Looking for `%s\'\n" _ host ? host : "<null>");

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
			HTTRACE(CORE_TRACE, "Host info... Collecting host %p\n" _ pres);
			delete_object(list, pres);
			pres = NULL;
  		    } else {
			HTTRACE(CORE_TRACE, "Host info... Found `%s\'\n" _ host);
		    }
		    return pres;
		}
	    }
	}
    }
    return NULL;
}

/*
**	Cleanup the host list
*/
PUBLIC void HTHost_deleteAll (void)
{
    HTList * list;
    HTHost * host;
    int i;

    if (!HostTable)
	return;

    for (i=0; i < HOST_HASH_SIZE; i++) {
	list = HostTable[i];
	if (!list) continue;

	while ((host = (HTHost *) HTList_removeFirstObject(list)) != NULL)
	    free_object(host);

	HTList_delete(list);
    }

    HT_FREE(HostTable);
    HostTable = NULL;
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
**  Get and set the passive timeout for persistent entries.
*/
PUBLIC BOOL HTHost_setPersistTimeout (time_t timeout)
{
    if (timeout > 0) {
	HTPassiveTimeout = timeout;
	return YES;
    }
    return NO;
}

PUBLIC time_t HTHost_persistTimeout (void)
{
    return HTPassiveTimeout;
}

/*
**  Get and set the active timeout for persistent entries.
*/
PUBLIC BOOL HTHost_setActiveTimeout (ms_t timeout)
{
    if (timeout > 1000) {
	HTActiveTimeout = timeout;
	return YES;
    }
    return NO;
}

PUBLIC ms_t HTHost_activeTimeout (void)
{
    return HTActiveTimeout;
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
	    char * start = HTStrCaseStr(host->range_units, "none");

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
	host->persistent = NO;
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
	    host->expires = time(NULL) + HTPassiveTimeout;     /* Default timeout */
	    HTChannel_setHost(host->channel, host);
	    HTNet_increasePersistentSocket();
	    HTTRACE(CORE_TRACE, "Host info... added host %p as persistent\n" _ host);
	    return YES;
	} else {
	    HTTRACE(CORE_TRACE, "Host info... no room for persistent socket %d\n" _ 
			sockfd);
	    return NO;
	}
    } else {
	HTTRACE(CORE_TRACE, "Host info... %p already persistent\n" _ host);
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
**  Check whether we have got a "close" notification, for example in the
**  connection header
*/
PUBLIC BOOL HTHost_setCloseNotification (HTHost * host, BOOL mode)
{
    if (host) {
	host->close_notification = mode;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTHost_closeNotification (HTHost * host)
{
    return host && host->close_notification;
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
	if (HTHost_isPersistent(host)) {
	    HTNet_decreasePersistentSocket();
	    host->persistent = NO;
	}
	host->close_notification = NO;
	host->broken_pipe = NO;
       	host->mode = HT_TP_SINGLE;

	host->recovered = 0;

	HTTRACE(CORE_TRACE, "Host info... removed host %p as persistent\n" _ host);

	if (!HTList_isEmpty(host->pending)) {
	    HTTRACE(CORE_TRACE, "Host has %d object(s) pending - attempting launch\n" _ HTList_count(host->pending));
	    HTHost_launchPending(host);
	}
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTHost_doRecover (HTHost * host)
{
    return host ? host->do_recover : NO;
}

/*
**	Move all entries in the pipeline and move the rest to the pending
**	queue. They will get launched at a later point in time.
*/
PUBLIC BOOL HTHost_recoverPipe (HTHost * host)
{
    if (host) {
	int piped = HTList_count(host->pipeline);

	/*
	**  First check that we haven't already recovered more than we want
	*/
	if (host->recovered > MAX_HOST_RECOVER) {
	    HTTRACE(CORE_TRACE, "Host recover %p already %d times - not doing it anymore\n" _ host _ host->recovered);
	    return NO;
	}

	/*
	**  If we decided to recover and actually have something in the pipe
	**  then go ahead.
	*/
	if (piped > 0) {
	    int cnt;
	    host->recovered++;
	    HTTRACE(CORE_TRACE, "Host recover %p recovered %d times. Moving %d Net objects from pipe line to pending queue\n" _ host _ host->recovered _ piped);
	    
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
		HTTRACE(CORE_TRACE, "Host recover Resetting net object %p\n" _ net);
		net->registeredFor = 0;
		(*net->event.cbf)(HTChannel_socket(host->channel), net->event.param, HTEvent_RESET);
		HTList_appendObject(host->pending, net);
		host->lock = net;
	    }

	    HTChannel_setSemaphore(host->channel, 0);
	    HTHost_clearChannel(host, HT_INTERRUPTED);
	    host->do_recover = NO;
	}
	return YES;
    }
    return NO;
}

/*
**	Terminate a pipeline prematurely, for example because of timeout,
**	interruption, etc.
*/
PUBLIC BOOL HTHost_killPipe (HTHost * host)
{
    return killPipeline(host, HTEvent_CLOSE);
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
		HTTRACE(CORE_TRACE, "Host info... Moving %d Net objects from pipe line to pending queue\n" _ piped);
		if (!host->pending) host->pending = HTList_new();
		for (cnt=0; cnt<piped; cnt++) {
		    HTNet * net = HTList_removeLastObject(host->pipeline);
		    HTTRACE(CORE_TRACE, "Host info... Resetting net object %p\n" _ net);
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
	    HTTRACE(PROT_TRACE, "Host info... %p is bad for pipelining so we won't do it!!!\n" _ 
			host);
	} else {
	    host->mode = mode;
	    HTTRACE(PROT_TRACE, "Host info... New mode is %d for host %p\n" _ host->mode _ host);
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
    return (host && HTList_isEmpty(host->pipeline));
}

PRIVATE BOOL _roomInPipe (HTHost * host)
{
    int count;
    if (!host ||
	(host->reqsPerConnection && host->reqsMade >= host->reqsPerConnection) ||
	HTHost_closeNotification(host) || host->broken_pipe)
	return NO;
    count = HTList_count(host->pipeline);
    switch (host->mode) {
    case HT_TP_SINGLE:
	return count <= 0;
    case HT_TP_PIPELINE:
	return (host->recovered < MAX_HOST_RECOVER) ?
	    (count < MaxPipelinedRequests) : (count <= 0);
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
	BOOL doit = (host->doit==net);

	/*
	**  If we don't have a socket already then check to see if we can get
	**  one. Otherwise we put the host object into our pending queue.
	*/
	if (!host->channel && HTNet_availableSockets() <= 0) {

	    /* Create list for pending Host objects */
	    if (!PendHost) PendHost = HTList_new();

	    /* Add the host object ad pending if not already */
	    if (HTList_indexOf(PendHost, host) < 0) HTList_addObject(PendHost, host);

	    /* 
	    ** Add the Net object to the Host object. If it is the current Net
	    ** obejct holding the lock then add it to the beginning of the list.
	    ** Otherwise add it to the end
	    */
	    if (!host->pending) host->pending = HTList_new();
	    if (host->lock == net)
		HTList_appendObject(host->pending, net);
	    else
		HTList_addObject(host->pending, net);

 	    HTTRACE(CORE_TRACE, "Host info... Added Net %p (request %p) as pending on pending Host %p, %d requests made, %d requests in pipe, %d pending\n" _ 
			net _ net->request _ host _ host->reqsMade _ 
			HTList_count(host->pipeline) _ HTList_count(host->pending));
	    return HT_PENDING;
	}

#if 0
	/*
	** First check whether the net object is already on either queue.
	** Do NOT add extra copies of the HTNet object to
	** the pipeline or pending list (if it's already on the list).
	*/
	if (HTList_indexOf(host->pipeline, net) >= 0) {
	    HTTRACE(CORE_TRACE, "Host info... The Net %p (request %p) is already in pipe,"
			" %d requests made, %d requests in pipe, %d pending\n" _ 
			net _ net->request _ host->reqsMade _ 
			HTList_count(host->pipeline) _ 
			HTList_count(host->pending));
	    HTDEBUGBREAK("Net object %p registered multiple times in pipeline\n" _ 
			 net);
	    return HT_OK;
	}

	if (HTList_indexOf(host->pending,  net) >= 0) {
	    HTTRACE(CORE_TRACE, "Host info... The Net %p (request %p) already pending,"
			" %d requests made, %d requests in pipe, %d pending\n" _ 
			net _ net->request _ host->reqsMade _ 
			HTList_count(host->pipeline) _ 
			HTList_count(host->pending));
	    HTDEBUGBREAK("Net object %p registered multiple times in pending queue\n" _ 
			 net);

	    return HT_PENDING;
	}
#endif

	/*
	**  Add net object to either active or pending queue.
	*/
	if (_roomInPipe(host) && (HTList_isEmpty(host->pending) || doit)) {
	    if (doit) host->doit = NULL;
	    if (!host->pipeline) host->pipeline = HTList_new();
	    HTList_addObject(host->pipeline, net);
	    host->reqsMade++;
            HTTRACE(CORE_TRACE, "Host info... Added Net %p (request %p) to pipe on Host %p, %d requests made, %d requests in pipe, %d pending\n" _ 
			net _ net->request _ host _ host->reqsMade _ 
			HTList_count(host->pipeline) _ HTList_count(host->pending));

	    /*
	    **  If we have been idle then make sure we delete the timer
	    */
	    if (host->timer) {
		HTTimer_delete(host->timer);
		host->timer = NULL;
	    }
           
            /*JK: New CBF function
	    ** Call any user-defined callback to say the request will
            ** be processed.
            */
            HTHost_ActivateRequest (net);

	} else {
	    if (!host->pending) host->pending = HTList_new();
	    HTList_addObject(host->pending, net);	    
	    HTTRACE(CORE_TRACE, "Host info... Added Net %p (request %p) as pending on Host %p, %d requests made, %d requests in pipe, %d pending\n" _ 
			net _ net->request _ 
			host _ host->reqsMade _ 
			HTList_count(host->pipeline) _ HTList_count(host->pending));
	    status = HT_PENDING;
	}
	return status;
    }
    return HT_ERROR;
}

PRIVATE BOOL HTHost_free (HTHost * host, int status)
{
    if (host->channel) {

	/* Check if we should keep the socket open */
        if (HTHost_isPersistent(host)) {
	    int piped = HTList_count(host->pipeline);
            if (HTHost_closeNotification(host)) {
		HTTRACE(CORE_TRACE, "Host Object. got close notifiation on socket %d\n" _ 
			    HTChannel_socket(host->channel));
                
		/*
		**  If more than a single element (this one) in the pipe
		**  then we have to recover gracefully
		*/
		if (piped > 1) {
		    host->reqsPerConnection = host->reqsMade - piped;
		    HTTRACE(CORE_TRACE, "%d requests made, %d in pipe, max %d requests pr connection\n" _ 
				host->reqsMade _ piped _ host->reqsPerConnection);
		    host->do_recover = YES;
		    /* @@ JK: not clear yet if I need or not to set the channel to NULL. I think not */
		    /* HTChannel_delete(host->channel, status);  */
		    if (HTChannel_delete(host->channel, status)) {
			HTTRACE(CORE_TRACE, "Host Event.. clearing channel on host %p (%s)\n" _ host _ host->hostname);
			host->channel = NULL;
		    }
		} else {
		    HTChannel_setSemaphore(host->channel, 0);
		    HTHost_clearChannel(host, status);
		}
	    } else if (piped<=1 && host->reqsMade==host->reqsPerConnection) {
                HTTRACE(CORE_TRACE, "Host Object. closing persistent socket %d\n" _ 
			HTChannel_socket(host->channel));
                
                /* 
                **  By lowering the semaphore we make sure that the channel
                **  is gonna be deleted
                */
                HTChannel_setSemaphore(host->channel, 0);
                HTHost_clearChannel(host, status);

            } else {
                HTTRACE(CORE_TRACE, "Host Object. keeping persistent socket %d\n" _ 
			HTChannel_socket(host->channel));
                if (HTChannel_delete(host->channel, status)) {
		    HTDEBUGBREAK("Host Event.. Channel unexpected deleted from host %p (%s)\n" _ host _ host->hostname);
		    host->channel = NULL;
                }
                /*
                **  If connection is idle then set a timer so that we close the 
                **  connection if idle too long
                */
                if (piped<=1 && HTList_isEmpty(host->pending) && !host->timer) {
                    host->timer = HTTimer_new(NULL, IdleTimeoutEvent,
					      host, HTActiveTimeout, YES, NO);
                    HTTRACE(PROT_TRACE, "Host........ Object %p going idle...\n" _ host);
                }
            }
            return YES;
        } else {
            HTTRACE(CORE_TRACE, "Host Object. closing socket %d\n" _ HTChannel_socket(host->channel));
	    HTChannel_setSemaphore(host->channel, 0);
	    HTHost_clearChannel(host, status);
        }
    }
    return NO;
}

PUBLIC BOOL HTHost_deleteNet (HTHost * host, HTNet * net, int status)
{
    if (host && net) {
        HTTRACE(CORE_TRACE, "Host info... Remove %p from pipe\n" _ net);

	/* If the Net object is in the pipeline then also update the channel */
	if (host->pipeline && HTList_indexOf(host->pipeline, net) >= 0) {
	    HTHost_free(host, status);
	    HTList_removeObjectAll(host->pipeline, net);
	}

	HTList_removeObjectAll(host->pending, net); /* just to make sure */
	host->lock = HTList_firstObject(host->pending);
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
    if (host && host->pending) {
	/*JK 23/Sep/96 Bug correction. Associated the following lines to the
	**above if. There was a missing pair of brackets. 
	*/
	if ((net = (HTNet *) HTList_removeFirstObject(host->pending)) != NULL) {
	    HTTRACE(CORE_TRACE, "Host info... Popping %p from pending net queue on host %p\n" _ 
			net _ host);
#if 0
	    {
		HTRequest * request = HTNet_request(net);
		char * uri = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
		fprintf(stderr, "Popping '%s'\n", uri);
	    }
#endif
	    host->doit = net;
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
	    HTTRACE(PROT_TRACE, "Host info... Popping %p from pending host queue\n" _ 
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
    HTNet * net = NULL;
    if (!host) {
	HTTRACE(PROT_TRACE, "Host info... Bad arguments\n");
	return NO;
    }

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
    */
    if (_roomInPipe(host) && DoPendingReqLaunch &&
	   (net = HTHost_nextPendingNet(host))) {
	HTHost_ActivateRequest(net);
	HTTRACE(CORE_TRACE, "Launch pending net object %p with %d reqs in pipe (%d reqs made)\n" _ 
		    net _ HTList_count(host->pipeline) _ host->reqsMade);
	return HTNet_execute(net, HTEvent_WRITE);
    }

    /*
    **  Check for other pending Host objects
    */
    if (DoPendingReqLaunch && HTNet_availableSockets() > 0) {
	HTHost * pending = HTHost_nextPendingHost();
	if (pending && (net = HTHost_nextPendingNet(pending))) {
	    if (!pending->pipeline) pending->pipeline = HTList_new();
	    HTList_addObject(pending->pipeline, net);
	    host->reqsMade++;
	    HTTRACE(CORE_TRACE, "Launch pending host object %p, net %p with %d reqs in pipe (%d reqs made)\n" _ 
			pending _ net _ HTList_count(pending->pipeline) _ pending->reqsMade);
	    HTHost_ActivateRequest(net);
	    return HTNet_execute(net, HTEvent_WRITE);
	}
    }
    return YES;
}

PUBLIC HTNet * HTHost_firstNet (HTHost * host)
{
    return (HTNet *) HTList_firstObject(host->pipeline);
}

PUBLIC int HTHost_numberOfOutstandingNetObjects (HTHost * host)
{
    return host ? HTList_count(host->pipeline) : -1;
}

PUBLIC int HTHost_numberOfPendingNetObjects (HTHost * host)
{
    return host ? HTList_count(host->pending) : -1;
}

/*
**	The host event manager keeps track of the state of it's client engines
**	(typically HTTPEvent), accepting multiple blocks on read or write from
**	multiple pipelined engines. It then registers its own engine 
**	(HostEvent) with the event manager.
*/
PUBLIC int HTHost_connect (HTHost * host, HTNet * net, char * url)
{
    HTRequest * request = HTNet_request(net);
    int status = HT_OK;
    if (!host) {
	HTProtocol * protocol = HTNet_protocol(net);
	if ((host = HTHost_newWParse(request, url, HTProtocol_id(protocol))) == NULL)
	    return HT_ERROR;

	/*
	** If not already locked and without a channel
	** then lock the darn thing with the first Net object
	** pending.
	*/
	if (!host->lock && !host->channel) {
	    HTNet * next_pending = NULL;
	    host->forceWriteFlush = YES;
	    host->lock = (next_pending = HTList_firstObject(host->pending)) ?
		next_pending : net;
	    HTTRACE(CORE_TRACE, "Host connect Grabbing lock on Host %p with %p\n" _ host _ host->lock);
	}
	HTNet_setHost(net, host);
    }

    if (!host->lock || (host->lock && host->lock == net)) {
	status = HTDoConnect(net);
	if (status == HT_PENDING)
	    return HT_WOULD_BLOCK;
	else if (status == HT_WOULD_BLOCK) {
	    host->lock = net;
	    return status;
	} else {

	    /*
	    **  See if there is already a new pending request that should
	    **  take over the current lock
	    */
	    HTNet * next_pending = NULL;
	    if ((next_pending = HTList_firstObject(host->pending))) {
		HTTRACE(CORE_TRACE, "Host connect Changing lock on Host %p to %p\n" _ 
			host _ next_pending);
		host->lock = next_pending;	    
	    } else {
		HTTRACE(CORE_TRACE, "Host connect Unlocking Host %p\n" _ host);
		host->lock = NULL;	    
	    }
	    return status;
	}
    } else {
	HTTRACE(CORE_TRACE, "Host connect Host %p already locked with %p\n" _ host _ host->lock);
	if ((status = HTHost_addNet(host, net)) == HT_PENDING) {
	    return HT_PENDING;
	}
    }
    return HT_ERROR; /* @@@ - some more deletion and stuff here? */
}

PUBLIC int HTHost_listen (HTHost * host, HTNet * net, char * url)
{
    HTRequest * request = HTNet_request(net);
    int status = HT_OK;
    if (!host) {
	HTProtocol * protocol = HTNet_protocol(net);
	if ((host = HTHost_newWParse(request, url, HTProtocol_id(protocol))) == NULL)
	    return HT_ERROR;

	/*
	** If not already locked and without a channel
	** then lock the darn thing with the first Net object
	** pending.
	*/
	if (!host->lock && !host->channel) {
	    host->forceWriteFlush = YES;
	    host->lock = net;
	}
	HTNet_setHost(net, host);
    }

    /*
    ** See if we already have a dedicated listen Net object. If not 
    ** then create one.
    */
    if (!host->listening) host->listening = HTNet_new(host);

    /*
    ** Start listening on the Net object
    */
    status = HTDoListen(host->listening, net, HT_BACKLOG);
    if (status != HT_OK) {
	HTTRACE(CORE_TRACE, "Host listen. On Host %p resulted in %d\n" _ host _ status);
	return status;
    }
    return HT_OK;
}

PUBLIC int HTHost_accept (HTHost * host, HTNet * net, char * url)
{
    int status = HT_OK;
    if (!host || !host->listening) {
	HTTRACE(CORE_TRACE, "Host accept. No host object or not listening on anything\n");
	return HT_ERROR;
    }

    if (!host->lock || (host->lock && host->lock == net)) {
	status = HTDoAccept(host->listening, net);
	if (status == HT_PENDING)
	    return HT_WOULD_BLOCK;
	else if (status == HT_WOULD_BLOCK) {
	    host->lock = net;
	    return status;
	} else {

	    /*
	    **  See if there is already a new pending request that should
	    **  take over the current lock
	    */
	    HTNet * next_pending = NULL;
	    if ((next_pending = HTList_firstObject(host->pending))) {
		HTTRACE(CORE_TRACE, "Host connect Changing lock on Host %p to %p\n" _ 
			host _ next_pending);
		host->lock = next_pending;	    
	    } else {
		HTTRACE(CORE_TRACE, "Host connect Unlocking Host %p\n" _ host);
		host->lock = NULL;	    
	    }
	    return status;
	}
    } else {
	HTTRACE(CORE_TRACE, "Host connect Host %p already locked with %p\n" _ host _ host->lock);
	if ((status = HTHost_addNet(host, net)) == HT_PENDING) {
	    return HT_PENDING;
	}
    }
    return HT_ERROR;
}

/*
**	Rules: SINGLE: one element in pipe, either reading or writing
**		 PIPE: n element in pipe, n-1 reading, 1 writing
*/
PUBLIC int HTHost_register (HTHost * host, HTNet * net, HTEventType type)
{
  HTEvent *event;

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

#ifdef WWW_WIN_ASYNC
            /* Make sure we are registered for CLOSE on windows */
	    event =  *(host->events+HTEvent_INDEX(HTEvent_CLOSE));
	    HTEvent_register(HTChannel_socket(host->channel), HTEvent_CLOSE, event);
#endif /* WWW_WIN_ASYNC */
            
            /* JK:  register a request in the event structure */
	    event =  *(host->events+HTEvent_INDEX(type));
	    event->request = HTNet_request (net);
	    return HTEvent_register(HTChannel_socket(host->channel),
				    type, event);
	}

	return YES;
    }
    HTTRACE(CORE_TRACE, "HTHost...... Don't register event with bad arguments\n");
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
    HTTRACE(PROT_TRACE, "Host........ %d bytes remaining \n" _ remaining);
    if (host->broken_pipe && remaining == 0) {
	HTTRACE(PROT_TRACE, "Host........ Emtied out connection\n");
    }
    return YES;
}

PUBLIC size_t HTHost_remainingRead (HTHost * host)
{
    return host ? host->remainingRead : -1;
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
    if (!host) return NO;

    if (host->retry > 0) host->retry--;
    return YES;

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
    return host ? (HTNet *) HTList_firstObject(host->pipeline) : NULL;
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
    HTTRACE(CORE_TRACE, "Host Object. Can't create input stream\n");
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
    HTTRACE(CORE_TRACE, "Host Object. Can't create output stream\n");
    return NULL;
}

PUBLIC HTOutputStream * HTHost_output (HTHost * host, HTNet * net)
{
    if (host && host->channel && net) {
	HTOutputStream * output = HTChannel_output(host->channel);
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
    HTTRACE(CORE_TRACE, "Host........ passing %d bytes as consumed to %p\n" _ bytes _ input);
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
#if 0
    unsigned short mtu;
    int ret = -1;
    int socket = HTChannel_socket(host->channel);
#ifndef WWW_MSWINDOWS
    ret = ioctl(socket, 666, (unsigned long)&mtu);
#endif /* WWW_MSWINDOWS */
    if ((ret == 0 && buffSize >= mtu) || host->forceWriteFlush)
	return 0;
    return host->delay;
#else
    return host->forceWriteFlush ? 0 : host->delay;
#endif
}

PUBLIC BOOL HTHost_setDefaultWriteDelay (ms_t delay)
{
    if (delay >= 0) {
	WriteDelay = delay;
	HTTRACE(CORE_TRACE, "Host........ Default write delay is %d ms\n" _ delay);
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
    HTNet * targetNet = (HTNet *) HTList_lastObject(host->pipeline);
    int ret;
    if (targetNet == NULL) return HT_ERROR;
    /* 2000/28/07 JK: The following test was proposed by Heiner Kallweit, as there's a problem
    ** while using SSL because of a recursive call to this function. We tested the
    ** fix and it doesn't seem to introduce any side effects... but one never knows,
    ** thus this comment. This seems more like a bug in the SSL code than here.
    */
    if (host->inFlush) {
	HTTRACE(CORE_TRACE, "Host Event.. FLUSH requested for  `%s\'\n, but ignoring it as we're already processing a flush in this host" _ 
		HTAnchor_physical(HTRequest_anchor(HTNet_request(targetNet))));
	return HT_OK;
    }
    HTTRACE(CORE_TRACE, "Host Event.. FLUSH passed to `%s\'\n" _ 
		HTAnchor_physical(HTRequest_anchor(HTNet_request(targetNet))));
    host->forceWriteFlush = YES;
    host->inFlush = YES;
    ret = (*targetNet->event.cbf)(HTChannel_socket(host->channel), targetNet->event.param, HTEvent_FLUSH);
    host->forceWriteFlush = NO;
    host->inFlush = NO;
    return ret;
}

/*
** Context pointer to be used as a user defined context 
*/
PUBLIC void HTHost_setContext (HTHost * me, void * context)
{
  if (me) me->context = context;
}

PUBLIC void * HTHost_context (HTHost * me)
{
  return me ? me->context : NULL;
}

PUBLIC int HTHost_eventTimeout (void)
{
    return EventTimeout;
}

PUBLIC void HTHost_setEventTimeout (int millis)
{
    EventTimeout = millis;
    HTTRACE(CORE_TRACE, "Host........ Setting event timeout to %d ms\n" _ millis);
}

PUBLIC BOOL HTHost_setMaxPipelinedRequests (int max)
{
    if (max > 1) {
	MaxPipelinedRequests = max;
	return YES;
    }
    return NO;
}

PUBLIC int HTHost_maxPipelinedRequests (void)
{
    return MaxPipelinedRequests;
}

PUBLIC void HTHost_setActivateRequestCallback (HTHost_ActivateRequestCallback * cbf)
{
    HTTRACE(CORE_TRACE, "HTHost...... Registering %p\n" _ cbf);
    ActivateReqCBF = cbf;
}

PRIVATE int HTHost_ActivateRequest (HTNet * net)
{
    HTRequest * request = NULL;
    if (!ActivateReqCBF) {
	HTTRACE(CORE_TRACE, "HTHost...... No ActivateRequest callback handler registered\n");
	return HT_ERROR;
    }
    request = HTNet_request(net);
    return (*ActivateReqCBF)(request);
}

PUBLIC void HTHost_disable_PendingReqLaunch (void)
{
    DoPendingReqLaunch = NO;
}

PUBLIC void HTHost_enable_PendingReqLaunch (void)
{
    DoPendingReqLaunch = YES;
}

