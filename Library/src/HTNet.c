/*								     HTNet.c
**	HTNet Class
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This is the implementation of the internal library multithreading
**	functions. This includes an interrupt handler and a event loop.
**	
** History:
**  	12 June 94	Written by Henrik Frystyk, frystyk@w3.org
**      31 May  95      Charlie Brooks cbrooks@osf.org
**
*/

/* Implemention dependent include files */
#include "sysdep.h"

/* Library include files */
#include "WWWUtil.h"
#include "HTProt.h"
#include "HTError.h"
#include "HTAlert.h"
#include "HTParse.h"
#include "HTTrans.h"
#include "HTHost.h"
#include "HTReq.h"
#include "HTEvent.h"
#include "HTStream.h"
#include "HTNetMan.h"					 /* Implemented here */

#ifndef HT_MAX_SOCKETS
#define HT_MAX_SOCKETS	6
#endif

#define HASH_SIZE	67

typedef struct _BeforeFilter {
    HTNetBefore *	before;				  /* Filter function */
    char *		tmplate;     /* URL template for when to call filter */
    int			order;			 /* Relative execution order */
    void *		param;				    /* Local context */
} BeforeFilter;

typedef struct _AfterFilter {
    HTNetAfter *	after;				  /* Filter function */
    char *		tmplate;     /* URL template for when to call filter */
    int			order;			 /* Relative execution order */
    void *		param;				    /* Local context */
    int			status;	   /* Status of load for when to call filter */
} AfterFilter;

struct _HTStream {
    const HTStreamClass *	isa;
    /* ... */
};

PRIVATE HTList * HTBefore = NULL;	    /* List of global BEFORE filters */
PRIVATE HTList * HTAfter = NULL;	     /* List of global AFTER filters */

PRIVATE int MaxActive = HT_MAX_SOCKETS;  	      /* Max active requests */
PRIVATE int Active = 0;				      /* Counts open sockets */
PRIVATE int Persistent = 0;		        /* Counts persistent sockets */

PRIVATE HTList ** NetTable = NULL;		      /* List of net objects */
PRIVATE int HTNetCount = 0;		       /* Counting elements in table */

/* ------------------------------------------------------------------------- */
/*		   GENERIC BEFORE and AFTER filter Management		     */
/* ------------------------------------------------------------------------- */

PRIVATE int HTBeforeOrder (const void * a, const void * b)
{
    return ((BeforeFilter *) b)->order - ((BeforeFilter *) a)->order;
}

PRIVATE int HTAfterOrder (const void * a, const void * b)
{
    return ((AfterFilter *) b)->order - ((AfterFilter *) a)->order;
}

PRIVATE int check_order (HTFilterOrder order)
{
    return (order<HT_FILTER_FIRST) ? HT_FILTER_FIRST :
	(order>HT_FILTER_LAST) ? HT_FILTER_LAST : order;
}

/*
**	Register a BEFORE filter in the list provided by the caller.
**	Several filters can be registered in which case they are called
**	with the filter ordering in mind.
*/
PUBLIC BOOL HTNetCall_addBefore (HTList * list, HTNetBefore * before,
				 const char * tmplate, void * param, 
				 HTFilterOrder order)
{
    if (list && before) {
	BeforeFilter * me;
	if ((me = (BeforeFilter *) HT_CALLOC(1, sizeof(BeforeFilter)))==NULL)
	    HT_OUTOFMEM("HTNetCall_addBefore");
	me->before = before;
	if (tmplate) StrAllocCopy(me->tmplate, tmplate);
	me->order = check_order(order);
	me->param = param;
	if (CORE_TRACE)
	    HTTrace("Net Before.. Add %p with order %d tmplate `%s\' context %p\n",
		    before, me->order, tmplate ? tmplate : "<null>", param);
	return (HTList_addObject(list, me) &&
		HTList_insertionSort(list, HTBeforeOrder));
    }
    return NO;
}

/*
**	Unregister all instances of a BEFORE filter from a list.
*/
PUBLIC BOOL HTNetCall_deleteBefore (HTList * list, HTNetBefore * before)
{
    if (CORE_TRACE) HTTrace("Net Before.. Delete %p\n", before);
    if (list && before) {
	HTList * cur = list;
	BeforeFilter * pres;
	while ((pres = (BeforeFilter *) HTList_nextObject(list))) {
	    if (pres->before == before) {
		HTList_removeObject(list, (void *) pres);
		HT_FREE(pres->tmplate);
		HT_FREE(pres);
		cur = list;
	    }
	}
    }
    return NO;
}

/*
**	Deletes all BEFORE filters in list
*/
PUBLIC BOOL HTNetCall_deleteBeforeAll (HTList * list)
{
    if (CORE_TRACE) HTTrace("Net Before. Delete All filters\n");
    if (list) {
	HTList * cur = list;
	BeforeFilter * pres;
	while ((pres = (BeforeFilter *) HTList_nextObject(cur))) {
	    HT_FREE(pres->tmplate);
	    HT_FREE(pres);
	}
	HTList_delete(list);
	return YES;
    }
    return NO;
}

/*
**	Call all the BEFORE filters in the order specified at registration
**	time. We also check for any template and whether it matches or not. 
**	If a filter returns other than HT_OK then stop and return immediately.
**	Otherwise return what the last filter returns.
*/
PUBLIC int HTNetCall_executeBefore (HTList * list, HTRequest * request)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * url = HTAnchor_physical(anchor);
    char * addr = url ? url : HTAnchor_address((HTAnchor *) anchor);
    int ret = HT_OK;
    int mode = 0;    
    if (list && request && addr) {
	BeforeFilter * pres;	
	while ((pres = (BeforeFilter *) HTList_nextObject(list))) {
	    if (!pres->tmplate ||
		(pres->tmplate && HTStrMatch(pres->tmplate, addr))) {
		if (CORE_TRACE) HTTrace("Net Before.. calling %p (request %p, context %p)\n",
					pres->before,
					request, pres->param);
		ret = (*pres->before)(request, pres->param, mode);
		if (ret != HT_OK) break;
	    }
	}
    }
    if (!url) HT_FREE(addr);
    return ret;
}

/*
**	Register a AFTER filter in the list provided by the caller.
**	Several filters can be registered in which case they are called
**	with the filter ordering in mind.
*/
PUBLIC BOOL HTNetCall_addAfter (HTList * list, HTNetAfter * after,
				const char * tmplate, void * param,
				int status, HTFilterOrder order)
{
    if (list && after) {
	AfterFilter * me;
	if ((me = (AfterFilter *) HT_CALLOC(1, sizeof(AfterFilter)))==NULL)
	    HT_OUTOFMEM("HTNetCall_addAfter");
	me->after = after;
	if (tmplate) StrAllocCopy(me->tmplate, tmplate);
	me->order = check_order(order);
	me->param = param;
	me->status = status;
	if (CORE_TRACE)
	    HTTrace("Net After... Add %p with order %d tmplate `%s\' code %d context %p\n",
		    after, me->order, tmplate ? tmplate : "<null>", status, param);
	return (HTList_addObject(list, me) &&
		HTList_insertionSort(list, HTAfterOrder));
    }
    return NO;
}

/*
**	Unregister all instances of an AFTER filter from a list.
*/
PUBLIC BOOL HTNetCall_deleteAfter (HTList * list, HTNetAfter * after)
{
    if (CORE_TRACE) HTTrace("Net After... Delete %p\n", after);
    if (list && after) {
	HTList * cur = list;
	AfterFilter * pres;
	while ((pres = (AfterFilter *) HTList_nextObject(cur))) {
	    if (pres->after == after) {
		HTList_removeObject(list, (void *) pres);
		HT_FREE(pres->tmplate);
		HT_FREE(pres);
		cur = list;
	    }
	}
    }
    return NO;
}

/*
**	Unregister all filters registered for a given status.
*/
PUBLIC BOOL HTNetCall_deleteAfterStatus (HTList * list, int status)
{
    if (CORE_TRACE) HTTrace("Net After... Delete all with status %d\n",status);
    if (list) {
	HTList * cur = list;
	AfterFilter * pres;
	while ((pres = (AfterFilter *) HTList_nextObject(cur))) {
	    if (pres->status == status) {
		HTList_removeObject(list, (void *) pres);
		HT_FREE(pres->tmplate);
		HT_FREE(pres);
		cur = list;
	    }
	}
	return YES;
    }
    return NO;
}

/*
**	Deletes all AFTER filters in list
*/
PUBLIC BOOL HTNetCall_deleteAfterAll (HTList * list)
{
    if (CORE_TRACE) HTTrace("Net After. Delete All filters\n");
    if (list) {
	HTList * cur = list;
	AfterFilter * pres;
	while ((pres = (AfterFilter *) HTList_nextObject(cur))) {
	    HT_FREE(pres->tmplate);
	    HT_FREE(pres);
	}
	HTList_delete(list);
	return YES;
    }
    return NO;
}

/*
**	Call all the AFTER filters in the order specified at registration
**	time and if it has the right status code and it's not HT_IGNORE.
**	We also check for any template and whether it matches or not.
**	If a filter returns other than HT_OK then stop and return immediately.
**	Otherwise return what the last filter returns.
*/
PUBLIC int HTNetCall_executeAfter (HTList * list, HTRequest * request,
				   int status)
{
    int ret = HT_OK;
    if (status != HT_IGNORE) {
	HTParentAnchor * anchor = HTRequest_anchor(request);
	char * url = HTAnchor_physical(anchor);
	char * addr = url ? url : HTAnchor_address((HTAnchor *) anchor);
	HTResponse * response = HTRequest_response(request);
	if (list && request && addr) {
	    AfterFilter * pres;
	    while ((pres = (AfterFilter *) HTList_nextObject(list))) {
		if ((pres->status == status || pres->status == HT_ALL) &&
		    (!pres->tmplate ||
		     (pres->tmplate && HTStrMatch(pres->tmplate, addr)))) {
		    if (CORE_TRACE)
			HTTrace("Net After... calling %p (request %p, response %p, status %d, context %p)\n",
				pres->after, request, response,
				status, pres->param);
		    ret = (*pres->after)(request, response, pres->param, status);
		    if (ret != HT_OK) break;
		}
	    }
	}
	if (!url) HT_FREE(addr);
    }
    return ret;
}

/* ------------------------------------------------------------------------- */
/*		   GLOBAL BEFORE and AFTER filter Management		     */
/* ------------------------------------------------------------------------- */

/*
**	Global set of callback functions BEFORE the request is issued
**	list can be NULL
*/
PUBLIC BOOL HTNet_setBefore (HTList *list)
{
    HTBefore = list;
    return YES;
}

PUBLIC HTList * HTNet_before (void)
{
    return HTBefore;
}

PUBLIC BOOL HTNet_addBefore (HTNetBefore * before, const char * tmplate,
			     void * param, HTFilterOrder order)
{
    if (!HTBefore) HTBefore = HTList_new();
    return HTNetCall_addBefore(HTBefore, before, tmplate, param, order);
}

PUBLIC BOOL HTNet_deleteBefore (HTNetBefore * cbf)
{
    return HTNetCall_deleteBefore(HTBefore, cbf);
}

/*
**  Call both the local and the global BEFORE filters (if any)
*/
PUBLIC int HTNet_executeBeforeAll (HTRequest * request)
{
    int ret;
    BOOL override = NO;
    HTList * befores;
    if ((befores = HTRequest_before(request, &override))) {
	if ((ret = HTNetCall_executeBefore(befores, request)) != HT_OK)
	    return ret;
    }
    return override ? HT_OK : HTNetCall_executeBefore(HTBefore, request);
}

/*
**	Global set of callback functions AFTER the request is issued
**	list can be NULL
*/
PUBLIC BOOL HTNet_setAfter (HTList *list)
{
    HTAfter = list;
    return YES;
}

PUBLIC HTList * HTNet_after (void)
{
    return HTAfter;
}

PUBLIC BOOL HTNet_addAfter (HTNetAfter * after, const char * tmplate,
			    void * param, int status, HTFilterOrder order)
{
    if (!HTAfter) HTAfter = HTList_new();
    return HTNetCall_addAfter(HTAfter, after, tmplate, param, status, order);
}

PUBLIC BOOL HTNet_deleteAfter (HTNetAfter * cbf)
{
    return HTNetCall_deleteAfter(HTAfter, cbf);
}

PUBLIC BOOL HTNet_deleteAfterStatus (int status)
{
    return HTNetCall_deleteAfterStatus(HTAfter, status);
}

/*
**  Call both the local and the global AFTER filters (if any)
*/
PUBLIC int HTNet_executeAfterAll (HTRequest * request, int status)
{
    int ret;
    BOOL override = NO;
    HTList * afters;
    if ((afters = HTRequest_after(request, &override))) {
	if ((ret = HTNetCall_executeAfter(afters, request, status)) != HT_OK)
	    return ret;
    }
    return override ? HT_OK : HTNetCall_executeAfter(HTAfter, request, status);
}

/* ------------------------------------------------------------------------- */
/*			      Socket Management				     */
/* ------------------------------------------------------------------------- */

PUBLIC int HTNet_maxSocket (void)
{
    return MaxActive;
}

PUBLIC BOOL HTNet_setMaxSocket (int newmax)
{
    if (newmax > 0) {
	MaxActive = newmax;
	return YES;
    }
    return NO;
}

PUBLIC void HTNet_increaseSocket (void)
{
    Active++;
    if (CORE_TRACE) HTTrace("Net Manager. %d open sockets\n", Active);
}

PUBLIC void HTNet_decreaseSocket (void)
{
    if (--Active < 0) Active = 0;
    if (CORE_TRACE) HTTrace("Net Manager. %d open sockets\n", Active);
}

PUBLIC int HTNet_availableSockets (void)
{
    int available = MaxActive - Active;
    return available > 0 ? available : 0;
}

PUBLIC void HTNet_increasePersistentSocket (void)
{
    Persistent++;
    if (CORE_TRACE) HTTrace("Net Manager. %d persistent sockets\n", Persistent);
}

PUBLIC void HTNet_decreasePersistentSocket (void)
{
    if (--Persistent < 0) Persistent = 0;
    if (CORE_TRACE) HTTrace("Net Manager. %d persistent sockets\n", Persistent);
}

PUBLIC int HTNet_availablePersistentSockets (void)
{
    int available = MaxActive - 2 - Persistent;
    return available > 0 ? available : 0;
}

/*
**	Returns whether there are any Net objects pending or active
*/
PUBLIC BOOL HTNet_isIdle (void)
{
    return (HTNetCount > 0);
}

PUBLIC BOOL HTNet_isEmpty (void)
{
    return (HTNetCount <= 0);
}

/* ------------------------------------------------------------------------- */
/*			  Creation and deletion methods  		     */
/* ------------------------------------------------------------------------- */

PRIVATE HTNet * create_object (void)
{
    static int net_hash = 0;
    HTNet * me = NULL;

    /* Create new object */
    if ((me = (HTNet *) HT_CALLOC(1, sizeof(HTNet))) == NULL)
        HT_OUTOFMEM("HTNet_new");
    me->hash = net_hash++ % HASH_SIZE;
    me->tcpstate = TCP_BEGIN;

    /* Insert into hash table */
    if (!NetTable) {
	if ((NetTable = (HTList **) HT_CALLOC(HASH_SIZE, sizeof(HTList *))) == NULL)
	    HT_OUTOFMEM("create_object");
    }
    if (!NetTable[me->hash]) NetTable[me->hash] = HTList_new();
    HTList_addObject(NetTable[me->hash], (void *) me);
    HTNetCount++;
    if (CORE_TRACE)
	HTTrace("Net Object.. %p created with hash %d\n",me, me->hash);
    return me;
}

/*	HTNet_duplicate
**	---------------
**	Creates a new HTNet object as a duplicate of the same request.
**	Returns YES if OK, else NO
**	BUG: We do not check if we have a socket free!
*/
PUBLIC HTNet * HTNet_dup (HTNet * src)
{
    if (src) {
        HTNet * me;
	int hash;
	if ((me = create_object()) == NULL) return NULL;
	hash = me->hash;
	if (CORE_TRACE) HTTrace("Net Object.. Duplicated %p\n", src);
        memcpy((void *) me, src, sizeof(HTNet));
	me->hash = hash;			/* Carry over hash entry */
	return me;
    }
    return NULL;
}

/*
**	Start a Net obejct by calling the protocol module.
*/
PUBLIC BOOL HTNet_start (HTNet * net)
{
    if (net && net->cbf && net->request) {
	if (CORE_TRACE) HTTrace("Net Object.. Launching %p\n", net);
	HTEvent_unregister(net->sockfd, (SockOps) FD_ALL);
	(*(net->cbf))(net->sockfd, net->request, FD_NONE);
	return YES;
    }
    return NO;
}

/*	HTNet_new
**	---------
**	This function creates a new HTNet object and assigns the socket number
**	to it. This is intended to be used when you are going to listen on a 
**	socket using the HTDoListen() function in HTTCP.c. The function do NOT
**	call any of the callback functions.
**	Returns new object or NULL on error
*/
PUBLIC HTNet * HTNet_new (SOCKET sockfd, HTRequest * request)
{
    if (sockfd != INVSOC) {
	HTNet * me;
	if ((me = create_object()) == NULL) return NULL;
	me->preemptive = HTRequest_preemptive(request);
	me->priority = HTRequest_priority(request);
	me->sockfd = sockfd;
	me->request = request;
	HTRequest_setNet(request, me);
	return me;
    }
    if (CORE_TRACE) HTTrace("Net Object.. Can't create empty Net object!\n");
    return NULL;
}

/*      HTNet_newServer
**      ---------------
**      Create a new HTNet object as a new request to be handled. If we have
**      more than MaxActive connections already then return NO.
**      Returns YES if OK, else NO
*/
PUBLIC BOOL HTNet_newServer (HTRequest * request, HTNet * net, char * access)
{
    HTProtocol * protocol;
    HTTransport * tp = NULL;    	/* added JTD:5/28/96 */
    if (!request) return NO;

    /* Find a protocol object for this access scheme */
    protocol = HTProtocol_find(request, access);

    /* added - JTD:5/28/96 */
    /* Find a transport object for this protocol */
    tp = HTTransport_find(request, HTProtocol_transport(protocol));
    if (tp == NULL) {
        if (CORE_TRACE) HTTrace("Net Object.. NO TRANSPORT OBJECT\n");
        return NO;
    }
    /* end of additions - JTD:5/28/96 */

    /* Fill out the net object and bind it to the request object */
    net->preemptive = (HTProtocol_preemptive(protocol) || HTRequest_preemptive(request));
    net->protocol = protocol;
    net->transport = tp; 		/* added - JTD:5/28/96 */
    net->priority = HTRequest_priority(request);
    net->request = request;
    HTRequest_setNet(request, net);
    if (!(net->cbf = HTProtocol_server(protocol))) {
        if (CORE_TRACE) HTTrace("Net Object.. NO CALL BACK FUNCTION!\n");
        return NO;
    }

    /* Increase the number of retrys for this download */
    HTRequest_addRetry(request);

    /* Start the server request */
    if (CORE_TRACE)
        HTTrace("Net Object.. starting SERVER request %p with net object %p\n", request, net);
    (*(net->cbf))(net->sockfd, request, FD_NONE);
    return YES;
}

/*	HTNet_new
**	---------
**	Create a new HTNet object as a new request to be handled. If we have
**	more than MaxActive connections already then put this into the
**	pending queue, else start the request by calling the call back
**	function registered with this access method. 
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTNet_newClient (HTRequest * request)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTNet * me = NULL;
    HTProtocol * protocol = NULL;
    HTTransport * tp = NULL;
    char * physical = NULL;
    int status;
    if (!request) return NO;
    /*
    ** First we do all the "BEFORE" callbacks in order to see if we are to
    ** continue with this request or not. If we receive a callback status
    ** that is NOT HT_OK then jump directly to the after callbacks and return
    */
    if ((status = HTNet_executeBeforeAll(request)) != HT_OK) {
	HTNet_executeAfterAll(request, status);
	return YES;
    }

    /*
    ** If no translation was provided by the filters then use the anchor
    ** address directly
    */
    if (!(physical = HTAnchor_physical(anchor))) {
	char * addr = HTAnchor_address((HTAnchor *) anchor);
	if (CORE_TRACE) HTTrace("Net Object.. Using default address\n");
	HTAnchor_setPhysical(anchor, addr);
	physical = HTAnchor_physical(anchor);
	HT_FREE(addr);
    }

    /* Find a protocol object for this access scheme */
    {
	char * access = HTParse(physical, "", PARSE_ACCESS);
	if ((protocol = HTProtocol_find(request, access)) == NULL) {
	    if (CORE_TRACE) HTTrace("Net Object.. NO PROTOCOL OBJECT\n");
	    HT_FREE(access);
	    return NO;
	}
	HT_FREE(access);
    }

    /* Find a transport object for this protocol */
    tp = HTTransport_find(request, HTProtocol_transport(protocol));
    if (tp == NULL) {
	if (CORE_TRACE) HTTrace("Net Object.. NO TRANSPORT OBJECT\n");
	return NO;
    }

    /* Create new net object and bind it to the request object */
    if ((me = create_object()) == NULL) return NO;
    me->preemptive = (HTProtocol_preemptive(protocol) || HTRequest_preemptive(request));
    me->priority = HTRequest_priority(request);
    me->sockfd = INVSOC;
    me->protocol = protocol;
    me->transport = tp;
    me->request = request;
    HTRequest_setNet(request, me);
    if (!(me->cbf = HTProtocol_client(protocol))) {
	if (CORE_TRACE) HTTrace("Net Object.. NO CALL BACK FUNCTION!\n");
	HT_FREE(me);
	return NO;
    }

    /* Increase the number of retrys for this download */
    HTRequest_addRetry(request);

    /*
    ** Check if we can start the request, else put it into pending queue
    ** If so then call the call back function associated with the anchor.
    ** We use the INVSOC as we don't have a valid socket yet!
    */
    if (CORE_TRACE)
        HTTrace("Net Object.. starting request %p (retry=%d) with net object %p\n",
	        request, HTRequest_retrys(request), me);
    (*(me->cbf))(me->sockfd, request, FD_NONE);
    return YES;
}

/*	delete_object
**	-------------
**	Deletes an HTNet object
**	Return YES if OK, else NO
*/
PRIVATE BOOL delete_object (HTNet *net, int status)
{
    if (CORE_TRACE) HTTrace("Net Object.. Remove object %p\n", net);
    if (net) {

	/* Close socket */
	if (net->channel) {
	    HTEvent_unregister(net->sockfd, (SockOps) FD_ALL);
	    if (HTHost_isPersistent(net->host)) {
		if (CORE_TRACE)
		    HTTrace("Net Object.. keeping socket %d\n", net->sockfd);
		HTChannel_delete(net->channel, status);
	    } else {
		if (CORE_TRACE)
		    HTTrace("Net Object.. closing socket %d\n", net->sockfd);

		/* 
		**  By lowering the semaphore we make sure that the channel
		**  is gonna be deleted
		*/
		HTChannel_downSemaphore(net->channel);
		HTChannel_delete(net->channel, status);
	    }
	    net->channel = NULL;
	}

	/*
	**  As we may have a socket available we check for whether
	**  we can start any pending requests. We do this by asking for
	**  pending Host objects. If none then use the current object
	*/
	HTHost_launchPending(net->host);

	/*
	**  Break the link to the request and free the Net object
	*/
	HTRequest_setNet(net->request, NULL);
	HT_FREE(net);
	return status ? NO : YES;
    }
    return NO;
}

/*
**	Clears the contents of the Net object so that we can use it again.
*/
PUBLIC BOOL HTNet_clear (HTNet * net)
{
    if (net) {
	net->sockfd = INVSOC;
	net->channel = NULL;
	net->input = NULL;
	net->bytes_read = 0;
	net->bytes_written = 0;
	net->tcpstate = TCP_CHANNEL;
	return YES;
    }
    return NO;
}

/*	HTNet_delete
**	------------
**	Deletes the HTNet object from the list of active requests and calls
**	any registered call back functions IF not the status is HT_IGNORE.
**	This is used if we have internal requests that the app doesn't know
**	about. We also see if we have pending requests that can be started
**	up now when we have a socket free.
**	The callback functions are called in the reverse order of which they
**	were registered (last one first)
**	Return YES if OK, else NO
*/
PUBLIC BOOL HTNet_delete (HTNet * net, int status)
{
    if (CORE_TRACE) 
	HTTrace("Net Object.. Delete %p and call AFTER filters\n", net);
    if (net) {
	HTRequest * request = net->request;

	/*
	** If we have a premature close then recover the request. Otherwise
	** break the link to the Host object and continue deleting the net
	** object
	*/
	if (net->host) {
	    if (status == HT_RECOVER_PIPE) {
		if (CORE_TRACE) HTTrace("Net Object.. Recovering %p\n", net);
		HTChannel_setSemaphore(net->channel, 0);
		HTHost_clearChannel(net->host, HT_INTERRUPTED);
		HTHost_setMode(net->host, HT_TP_SINGLE);
		HTNet_clear(net);
		if (CORE_TRACE)
		    HTTrace("Net Object.. Restarting request %p (retry=%d) with net object %p\n",
			    request, HTRequest_retrys(request), net);
#if 0
		HTHost_launchPending(net->host);
		return YES;
#else
		return HTNet_start(net);
#endif
	    } else
		HTHost_deleteNet(net->host, net);
	}

        /* Remove object from the table of Net Objects */
	if (NetTable) {
	    HTList * list = NetTable[net->hash];
	    if (list) {
		HTList_removeObject(list, (void *) net);
		delete_object(net, status);
		HTNetCount--;
	    }
	}

    	/* Call AFTER filters */
	HTNet_executeAfterAll(request, status);
        return YES;
    }
    return NO;
}

/*	HTNet_deleteAll
**	---------------
**	Deletes all HTNet object that might either be active or pending
**	We DO NOT call the AFTER filters - A crude way of saying goodbye!
*/
PUBLIC BOOL HTNet_deleteAll (void)
{
    if (CORE_TRACE) 
	HTTrace("Net Object.. Remove all Net objects, NO filters\n"); 
    if (NetTable) {
	HTList * cur = NULL;
        HTNet * pres = NULL;
	int cnt;
	for (cnt=0; cnt<HASH_SIZE; cnt++) {
	    if ((cur = NetTable[cnt])) { 
		while ((pres = (HTNet *) HTList_nextObject(cur)) != NULL)
		    delete_object(pres, HT_INTERRUPTED);
	    }
	    HTList_delete(NetTable[cnt]);
	}
	HT_FREE(NetTable);
	HTNetCount = 0;
	return YES;
    }
    return NO;
}

/*	HTNet_kill
**	----------
**	Kill the request by calling the call back function with a request for 
**	closing the connection. Does not remove the object. This is done by
**	HTNet_delete() function which is called by the load routine.
**	Returns OK if success, NO on error
*/
PUBLIC BOOL HTNet_kill (HTNet * net)
{
    if (net && net->cbf) {
        if (CORE_TRACE) HTTrace("Net Object.. Killing %p\n", net);
        (*(net->cbf))(net->sockfd, net->request, FD_CLOSE);
	return YES;
    }
    if (CORE_TRACE) HTTrace("Net Object.. No object to kill\n", net);
    return NO;
}

/*	HTNet_killAll
**	-------------
**	Kills all registered net objects by calling the call
**	back function with a request for closing the connection. We do not
**	remove the HTNet object as it is done by HTNet_delete().
**	Returns OK if success, NO on error
*/
PUBLIC BOOL HTNet_killAll (void)
{
    if (CORE_TRACE) HTTrace("Net Object.. Kill ALL Net objects!!!\n"); 
    if (NetTable) {
	HTList * cur = NULL;
        HTNet * pres = NULL;
	int cnt;
	for (cnt=0; cnt<HASH_SIZE; cnt++) {
	    if ((cur = NetTable[cnt])) { 
		while ((pres = (HTNet *) HTList_lastObject(cur)) != NULL)
    		    HTNet_kill(pres);
	    }
	}
	return YES;
    }
    if (CORE_TRACE) HTTrace("Net Object.. No objects to kill\n");
    return NO;
}

/* ------------------------------------------------------------------------- */
/*			    Connection Specifics 			     */
/* ------------------------------------------------------------------------- */

/*	HTNet_priority
**	--------------
**	Get the current priority of the Net object
*/
PUBLIC HTPriority HTNet_priority (HTNet * net)
{
    return (net ? net->priority : HT_PRIORITY_INV);
}

/*	HTNet_setPriority
**	-----------------
**	Set the current priority of the Net object
**	This will change the priority next time the thread is blocked
*/
PUBLIC BOOL HTNet_setPriority (HTNet * net, HTPriority priority)
{
    if (net) {
	net->priority = priority;
	return YES;
    }
    return NO;
}

/*	HTNet_Persistent
**	----------------
**	Check whether the net object handles persistent connections
**	If we have a DNS entry then check that as well.
*/
PUBLIC BOOL HTNet_persistent (HTNet * net)
{
    return (net && HTHost_isPersistent(net->host));
}

/*	HTNet_persistent
**	----------------
**	Set the net object to handle persistent connections
**	If we also have a DNS entry then update that as well
*/
PUBLIC BOOL HTNet_setPersistent (HTNet *		net,
				 BOOL			persistent,
				 HTTransportMode	mode)
{
    if (net) {
	BOOL result;			/* Bill Rizzi */
	if (persistent)
	    result = HTHost_setChannel(net->host, net->channel, mode);
	else {

	    /*
	    **  We use the HT_IGNORE status code as we don't want to free
	    **  the stream at this point in time. The situation we want to
	    **  avoid is that we free the channel from within the stream pipe.
	    **  This will lead to an infinite look having the stream freing
	    **  itself.
	    */
	    result = HTHost_clearChannel(net->host, HT_IGNORE);
	}
	if (CORE_TRACE)
	    HTTrace("Net Object.. Persistent connection set %s %s\n",
		    persistent ? "ON" : "OFF",
		    result ? "succeeded" : "failed");
    }
    return NO;
}

/*
**	Context pointer to be used in context call back function
*/
PUBLIC BOOL HTNet_setContext (HTNet * net, void * context)
{
    if (net) {
	net->context = context;
	return YES;
    }
    return NO;
}

PUBLIC void * HTNet_context (HTNet * net)
{
    return net ? net->context : NULL;
}

/*
**  Get and set the socket number
*/
PUBLIC BOOL HTNet_setSocket (HTNet * net, SOCKET sockfd)
{
    if (net) {
	net->sockfd = sockfd;
	return YES;
    }
    return NO;
}

PUBLIC SOCKET HTNet_socket (HTNet * net)
{
    return (net ? net->sockfd : INVSOC);
}

/*
**  Get and set the HTRequest object
*/
PUBLIC BOOL HTNet_setRequest (HTNet * net, HTRequest * request)
{
    if (net && request) {
	net->request = request;
	return YES;
    }
    return NO;
}

PUBLIC HTRequest * HTNet_request (HTNet * net)
{
    return (net ? net->request : NULL);
}

/*
**  Get and set the HTTransport object
*/
PUBLIC BOOL HTNet_setTransport (HTNet * net, HTTransport * tp)
{
    if (net && tp) {
	net->transport = tp;
	return YES;
    }
    return NO;
}

PUBLIC HTTransport * HTNet_transport (HTNet * net)
{
    return (net ? net->transport : NULL);
}

/*
**  Get and set the HTChannel object
*/
PUBLIC BOOL HTNet_setChannel (HTNet * net, HTChannel * channel)
{
    if (net && channel) {
	net->channel = channel;
	return YES;
    }
    return NO;
}

PUBLIC HTChannel * HTNet_channel (HTNet * net)
{
    return (net ? net->channel : NULL);
}

/*
**  Get and set the HTHost object
*/
PUBLIC BOOL HTNet_setHost (HTNet * net, HTHost * host)
{
    if (net && host) {
	net->host = host;
	return YES;
    }
    return NO;
}

PUBLIC HTHost * HTNet_host (HTNet * net)
{
    return (net ? net->host : NULL);
}

/*
**  Get and set the HTdns object
*/
PUBLIC BOOL HTNet_setDns (HTNet * net, HTdns * dns)
{
    if (net && dns) {
	net->dns = dns;
	return YES;
    }
    return NO;
}

PUBLIC HTdns * HTNet_dns (HTNet * net)
{
    return (net ? net->dns : NULL);
}

PUBLIC int HTNet_home (HTNet * net)
{
    return (net ? net->home : 0);
}

/*
**	Create the input stream and bind it to the channel
**	Please read the description in the HTIOStream module for the parameters
*/
PUBLIC HTInputStream * HTNet_getInput (HTNet * net, HTStream * target,
				       void * param, int mode)
{
    if (net && net->channel && net->transport) {
	HTTransport * tp = net->transport;
	HTChannel * ch = net->channel;
	net->input = (*tp->input_new)(net, ch, target, param, mode);
	HTChannel_setInput(ch, net->input);
	return net->input;
    }
    if (CORE_TRACE) HTTrace("Net Object.. Can't create input stream\n");
    return NULL;
}

/*
**	Create the output stream and bind it to the channel
**	Please read the description in the HTIOStream module on the parameters
*/
PUBLIC HTOutputStream * HTNet_getOutput (HTNet * net, void * param, int mode)
{
    if (net && net->request && net->channel && net->transport) {
	HTTransport * tp = net->transport;
	HTChannel * ch = net->channel;
	HTOutputStream * output = (*tp->output_new)(net, ch, param, mode);
	HTChannel_setOutput(ch, output);
	return output;
    }
    if (CORE_TRACE) HTTrace("Net Object.. Can't create output stream\n");
    return NULL;
}
