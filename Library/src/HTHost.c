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
    HTChannelMode	mode;	      			   /* Supported mode */
    HTChannel *		channel;		       /* Persistent channel */
    time_t		expires;	  /* Persistent channel expires time */
};

PRIVATE HTList	** HostTable = NULL;
PRIVATE time_t	HostTimeout = HOST_TIMEOUT;	  /* Timeout on host entries */
PRIVATE time_t	TCPTimeout = TCP_TIMEOUT;  /* Timeout on persistent channels */

PRIVATE HTList * Persistent = NULL;	       /* List of persistent sockets */

/* ------------------------------------------------------------------------- */

PRIVATE void free_object (HTHost * me)
{
    if (me) {
	HT_FREE(me->hostname);
	HT_FREE(me->type);
	if (me->channel) {
	    HTChannel_delete(me->channel);
	    me->channel = NULL;
	}
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
		if (time(NULL) > pres->ntime + HostTimeout) {
		    if (CORE_TRACE)
			HTTrace("Host info... Collecting host info %p\n",pres);
		    delete_object(list, pres);
		    pres = NULL;
		}
		break;
	    }
	}
    }

    /* If not found then create new Host object */
    if (pres) {
	if (pres->channel) {
	    if (pres->expires < time(NULL)) {	   /* Cached channel is cold */
		if (CORE_TRACE)
		    HTTrace("Host info... Persistent channel %p gotten cold\n",
			    pres->channel);
		HTChannel_delete(pres->channel);
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
	if (CORE_TRACE) 
	    HTTrace("Host info... added `%s\' to list %p\n", host, list);
	HTList_addObject(list, (void *) pres);
    }
    return pres;
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
**	Searches the list of persistent connections for a host object
**	associated with this channel
*/
PRIVATE HTHost * HTHost_findPersistent (HTChannel * ch)
{
    if (Persistent && ch) {
	HTList * cur = Persistent;
	HTHost * pres;
	while ((pres = (HTHost *) HTList_nextObject(cur)))
	    if (pres->channel == ch) return pres;
    }
    return NULL;
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
	HTHost * host = HTHost_findPersistent(ch);
	if (ch && host) {	    
	    if (CORE_TRACE) HTTrace("Catch Close. CLOSING socket %d\n", soc);
	    HTHost_clearChannel(host);
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
PUBLIC BOOL HTHost_setChannel (HTHost * host, HTChannel * channel)
{
    if (!host && !channel) return NO;
    if (host->channel) {
	if (CORE_TRACE) HTTrace("Host info... %p already persistent\n", host);
	return YES;
    } else {
	SOCKET sockfd = HTChannel_socket(channel);
	if (!Persistent) Persistent = HTList_new();
	if (sockfd != INVSOC && HTList_count(Persistent)<HTNet_maxSocket()-2) {
	    host->channel = channel;
	    host->expires = time(NULL) + TCPTimeout;	  /* Default timeout */
	    HTList_addObject(Persistent, host);
	    if (CORE_TRACE)
		HTTrace("Host info... added host %p as persistent\n", host);
	    return YES;
	} else {
	    if (CORE_TRACE)
		HTTrace("Host info... no room for persistent socket %d\n",
			socket);
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
PUBLIC BOOL HTHost_clearChannel (HTHost * host)
{
    if (host && host->channel) {
	HTChannel_delete(host->channel);
	host->expires = 0;
	host->channel = NULL;
	if (CORE_TRACE)
	    HTTrace("Host info... removed host %p as persistent\n", host);
	HTList_removeObject(Persistent, host);
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

