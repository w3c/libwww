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
#include "wwwsys.h"

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
#include "HTHstMan.h"
#include "HTIOStream.h"
#include "HTNetMan.h"					 /* Implemented here */

#ifndef HT_MAX_SOCKETS
#define HT_MAX_SOCKETS	25
#endif

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

struct _HTInputStream {
    const HTInputStreamClass *	isa;
    /* ... */
};

typedef struct _HTFilterEvent {
    HTRequest *		request;
    int			status;
    HTTimer *		timer;
} HTFilterEvent;

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
	HTTRACE(CORE_TRACE, "Net Before.. Add %p with order %d tmplate `%s\' context %p\n" _ 
		    before _ me->order _ tmplate ? tmplate : "<null>" _ param);
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
    HTTRACE(CORE_TRACE, "Net Before.. Delete %p\n" _ before);
    if (list && before) {
	HTList * cur = list;
	BeforeFilter * pres;
	while ((pres = (BeforeFilter *) HTList_nextObject(cur))) {
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
    HTTRACE(CORE_TRACE, "Net Before. Delete All filters\n");
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
		HTTRACE(CORE_TRACE, "Net Before.. calling %p (request %p, context %p)\n" _ 
					pres->before _ 
					request _ pres->param);
		ret = (*pres->before)(request, pres->param, mode);
		if (ret != HT_OK) break;

		/*
		**  Update the address to match against if the filter changed
		**  the physical address.
		*/
		if ((url = HTAnchor_physical(anchor))) addr = url;
	    }
	}
    }
    if (!url) HT_FREE(addr);
    return ret;
}

/*
**	Register an AFTER filter in the list provided by the caller.
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
	HTTRACE(CORE_TRACE, "Net After... Add %p with order %d tmplate `%s\' code %d context %p\n" _ 
		    after _ me->order _ tmplate ? tmplate : "<null>" _ status _ param);
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
    HTTRACE(CORE_TRACE, "Net After... Delete %p\n" _ after);
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
    HTTRACE(CORE_TRACE, "Net After... Delete all with status %d\n" _ status);
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
    HTTRACE(CORE_TRACE, "Net After. Delete All filters\n");
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
		    HTTRACE(CORE_TRACE, "Net After... calling %p (request %p, response %p, status %d, context %p)\n" _ 
				pres->after _ request _ response _ 
				status _ pres->param);
		    ret = (*pres->after)(request, response, pres->param, status);
		    if (ret != HT_OK) break;

		    /*
		    **  Update the address to match against if the filter changed
		    **  the physical address.
		    */
		    if ((url = HTAnchor_physical(anchor))) addr = url;
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
    else HTNet_deleteBefore(before); /* Ensure not listed twice */
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
    else HTNet_deleteAfter(after); /* Ensure not listed twice */
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
    HTTRACE(CORE_TRACE, "Net Manager. Increasing active sockets to %d, %d persistent sockets\n" _ 
		Active _ Persistent);
}

PUBLIC void HTNet_decreaseSocket (void)
{
    if (--Active < 0) Active = 0;
    HTTRACE(CORE_TRACE, "Net Manager. Decreasing active sockets to %d, %d persistent sockets\n" _ 
		Active _ Persistent);
}

PUBLIC int HTNet_availableSockets (void)
{
    int available = MaxActive - Active;
    return available > 0 ? available : 0;
}

PUBLIC void HTNet_increasePersistentSocket (void)
{
    Persistent++;
    HTTRACE(CORE_TRACE, "Net Manager. %d active sockets, increasing persistent sockets to %d\n" _ 
		Active _ Persistent);
}

PUBLIC void HTNet_decreasePersistentSocket (void)
{
    if (--Persistent < 0) Persistent = 0;
    HTTRACE(CORE_TRACE, "Net Manager. %d active sockets, decreasing persistent sockets to %d\n" _ 
		Active _ Persistent);
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

PUBLIC int HTNet_count (void)
{
    return HTNetCount;
}

/* ------------------------------------------------------------------------- */
/*			  Creation and deletion methods  		     */
/* ------------------------------------------------------------------------- */

PRIVATE int AfterFilterEvent (HTTimer * timer, void * param, HTEventType type)
{
    HTFilterEvent * fe = (HTFilterEvent *) param;
    if (fe) {
	HTRequest * request = fe->request;
	int status = fe->status;
	if (timer != fe->timer)
	    HTDEBUGBREAK("Net timer. %p not in sync\n" _ timer);
	HTTRACE(CORE_TRACE, "HTNet....... Continuing calling AFTER filters %p with timer %p\n" _ 
		    fe _ timer);

	/* Delete the event context */
	HT_FREE(fe);

	/* Now call the remaining AFTER filters */
	return HTNet_executeAfterAll(request, status);
    }
    return HT_ERROR;
}

PRIVATE BOOL createAfterFilterEvent (HTRequest * request, int status)
{
    HTFilterEvent * me = NULL;
    if ((me = (HTFilterEvent *) HT_CALLOC(1, sizeof(HTFilterEvent))) == NULL)
        HT_OUTOFMEM("createAfterFilterEvent");
    me->request = request;
    me->status = status;
    me->timer = HTTimer_new(NULL, AfterFilterEvent, me, 1, YES, NO);
    return YES;
}

PRIVATE HTNet * create_object (void)
{
    static int net_hash = 0;
    HTNet * me = NULL;

    /* Create new object */
    if ((me = (HTNet *) HT_CALLOC(1, sizeof(HTNet))) == NULL)
        HT_OUTOFMEM("HTNet_new");
    me->hash = net_hash++ % HT_XL_HASH_SIZE;

    /* Insert into hash table */
    if (!NetTable) {
	if ((NetTable = (HTList **) HT_CALLOC(HT_XL_HASH_SIZE, sizeof(HTList *))) == NULL)
	    HT_OUTOFMEM("create_object");
    }
    if (!NetTable[me->hash]) NetTable[me->hash] = HTList_new();
    HTList_addObject(NetTable[me->hash], (void *) me);
    HTNetCount++;
    HTTRACE(CORE_TRACE, "Net Object.. %p created with hash %d\n" _ me _ me->hash);
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
	HTTRACE(CORE_TRACE, "Net Object.. Duplicated %p\n" _ src);
        memcpy((void *) me, src, sizeof(HTNet));
	me->hash = hash;			/* Carry over hash entry */
	return me;
    }
    return NULL;
}

PUBLIC BOOL HTNet_execute (HTNet * net, HTEventType type)
{
    if (net && net->event.cbf && net->request) {
	HTTRACE(CORE_TRACE, "Net Object.. %p calling %p with event type %d and context %p\n" _ 
		    net _ net->event.cbf _ type _ net->event.param);
	(*(net->event.cbf))(HTNet_socket(net), net->event.param, type);
	return YES;
    }
    return NO;
}

/*
**	Start a Net obejct by calling the protocol module.
*/
PUBLIC BOOL HTNet_start (HTNet * net)
{
    if (net && net->event.cbf && net->request) {
	HTTRACE(CORE_TRACE, "Net Object.. Launching %p\n" _ net);
	(*(net->event.cbf))(HTNet_socket(net), net->event.param, HTEvent_BEGIN);
	return YES;
    }
    return NO;
}

/*	HTNet_new
**	---------
**	This function creates a new HTNet object and assigns the socket number
**	to it. This is intended to be used when you are going to listen on a 
**	socket using the HTDoListen() function in HTTCP.c. The function do NOT
**	call any of the before or after filter functions.
**	Returns new object or NULL on error
*/
PUBLIC HTNet * HTNet_new (HTHost * host)
{
    if (host) {
	HTNet * me;
	HTTRACE(CORE_TRACE, "Net Object.. Creating listen object for host %p\n" _ host);
	me = create_object();
	me->host = host;
	return me;
    }
    return NULL;
}

/*      HTNet_newServer
**      ---------------
**      Create a new HTNet object as a new request to be handled. If we have
**      more than MaxActive connections already then return NO.
**      Returns YES if OK, else NO
*/
PUBLIC BOOL HTNet_newServer (HTRequest * request)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTNet * me = NULL;
    HTProtocol * protocol;
    HTTransport * tp = NULL;    	/* added JTD:5/28/96 */
    char * physical = NULL;
    int status;
    HTProtCallback * cbf;

    if (!request) return NO;

    /*
    ** First we do all the "BEFORE" callbacks in order to see if we are to
    ** continue with this request or not. If we receive a callback status
    ** that is NOT HT_OK then jump directly to the after callbacks and return
    */
    if ((status = HTNet_executeBeforeAll(request)) != HT_OK) {

 	/*
	**  If in non-blocking mode then return here and call AFTER
	**  filters from a timer event handler. As Olga Antropova
	**  points out, otherwise, the stack can grow if new requests
	**  are started directly from the after filters 
	*/
	if (HTEvent_isCallbacksRegistered() && !HTRequest_preemptive(request))
	    createAfterFilterEvent(request, status);
	else
	    HTNet_executeAfterAll(request, status);
	return YES;
    }

    /*
    ** If no translation was provided by the filters then use the anchor
    ** address directly
    */
    if (!(physical = HTAnchor_physical(anchor))) {
	char * addr = HTAnchor_address((HTAnchor *) anchor);
	HTTRACE(CORE_TRACE, "Net Object.. Using default address\n");
	HTAnchor_setPhysical(anchor, addr);
	physical = HTAnchor_physical(anchor);
	HT_FREE(addr);
    }

    /* Find a protocol object for this access scheme */
    {
	char * access = HTParse(physical, "", PARSE_ACCESS);      
	if ((protocol = HTProtocol_find(request, access)) == NULL) {
	    HTTRACE(CORE_TRACE, "Net Object.. NO PROTOCOL Object found for URI scheme `%s\'\n" _ access);
	    HT_FREE(access);
	    return NO;
	}
	if (!(cbf = HTProtocol_server(protocol))) {
	    HTTRACE(CORE_TRACE, "Net Object.. NO SERVER HANDLER for URI scheme `%s\'\n" _ access);
	    HT_FREE(access);
	    HT_FREE(me);
	    return NO;
	}
	HT_FREE(access);
    }

    /* Find a transport object for this protocol */
    if ((tp = HTTransport_find(request, HTProtocol_transport(protocol))) == NULL) {
        HTTRACE(CORE_TRACE, "Net Object.. NO TRANSPORT found for protocol `%s\'\n" _ HTProtocol_name(protocol));
        return NO;
    }

    /* Create new net object and bind to request object */
    if ((me = create_object()) == NULL) return NO;
    me->preemptive = (HTProtocol_preemptive(protocol) || HTRequest_preemptive(request));
    HTNet_setEventPriority(me, HTRequest_priority(request));
    me->protocol = protocol;
    me->transport = tp; 		/* added - JTD:5/28/96 */
    me->request = request;
    HTRequest_setNet(request, me);

    /* Start the server request */
    HTTRACE(CORE_TRACE, "Net Object.. starting SERVER request %p and net object %p\n" _ request _ me);
    (*(cbf))(INVSOC, request);
    return YES;
}

/*	HTNet_newClient
**	---------------
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
    HTProtCallback * cbf;

    if (!request) return NO;

    /*
    ** First we do all the "BEFORE" callbacks in order to see if we are to
    ** continue with this request or not. If we receive a callback status
    ** that is NOT HT_OK then jump directly to the after callbacks and return
    */
    if ((status = HTNet_executeBeforeAll(request)) != HT_OK) {

 	/*
	**  If in non-blocking mode then return here and call AFTER
	**  filters from a timer event handler. As Olga Antropova
	**  points out, otherwise, the stack can grow if new requests
	**  are started directly from the after filters 
	*/
	if (HTEvent_isCallbacksRegistered() && !HTRequest_preemptive(request))
	    createAfterFilterEvent(request, status);
	else
	    HTNet_executeAfterAll(request, status);
	return YES;
    }

    /*
    ** If no translation was provided by the filters then use the anchor
    ** address directly
    */
    if (!(physical = HTAnchor_physical(anchor))) {
	char * addr = HTAnchor_address((HTAnchor *) anchor);
	HTTRACE(CORE_TRACE, "Net Object.. Using default address\n");
	HTAnchor_setPhysical(anchor, addr);
	physical = HTAnchor_physical(anchor);
	HT_FREE(addr);
    }

    /* Find a protocol object for this access scheme */
    {
	char * proxy = HTRequest_proxy(request);
	char * access = HTParse(proxy ? proxy : physical, "", PARSE_ACCESS);      
	if ((protocol = HTProtocol_find(request, access)) == NULL) {
	    HTTRACE(CORE_TRACE, "Net Object.. NO PROTOCOL Object found for URI scheme `%s\'\n" _ access);
	    HT_FREE(access);
	    return NO;
	}
	if (!(cbf = HTProtocol_client(protocol))) {
	    HTTRACE(CORE_TRACE, "Net Object.. NO CLIENT HANDLER for URI scheme `%s\'\n" _ access);
	    HT_FREE(access);
	    HT_FREE(me);
	    return NO;
	}
	HT_FREE(access);
    }

    /* Find a transport object for this protocol */
    tp = HTTransport_find(request, HTProtocol_transport(protocol));
    if (tp == NULL) {
	HTTRACE(CORE_TRACE, "Net Object.. NO TRANSPORT found for protocol `%s\'\n" _ HTProtocol_name(protocol));
	return NO;
    }

    /* Create new net object and bind it to the request object */
    if ((me = create_object()) == NULL) return NO;
    me->preemptive = (HTProtocol_preemptive(protocol) || HTRequest_preemptive(request));
#if 0
    me->priority = HTRequest_priority(request);
#endif
    HTNet_setEventPriority(me, HTRequest_priority(request));
    me->protocol = protocol;
    me->transport = tp;
    me->request = request;
    HTRequest_setNet(request, me);

    /* Increase the number of retrys for this download */
    HTRequest_addRetry(request);

    /*
    ** Check if we can start the request, else put it into pending queue
    ** If so then call the call back function associated with the anchor.
    ** We use the INVSOC as we don't have a valid socket yet!
    */
    HTTRACE(CORE_TRACE, "Net Object.. starting request %p (retry=%d) with net object %p\n" _ 
	        request _ HTRequest_retrys(request) _ me);
    (*(cbf))(INVSOC, request);
    return YES;
}

/*
**      Check whether we have any pending HTNet objects and if so
**	then start the next one.
**	Return YES if OK, else NO
*/
PRIVATE BOOL check_pending (HTNet * net)
{
    HTTRACE(CORE_TRACE, "Net Object.. Check for pending Net objects\n");
    if (net) {

	/*
	**  As we may have a socket available we check for whether
	**  we can start any pending requests. We do this by asking for
	**  pending Host objects. If none then use the current object
	*/
        HTHost_launchPending(net->host);

        return YES;
    }
    return NO;
}

PRIVATE BOOL free_net (HTNet * net)
{
    HTTRACE(CORE_TRACE, "Net Object.. Freeing object %p\n" _ net);
    if (net) {
        if (net == HTRequest_net(net->request)) HTRequest_setNet(net->request, NULL);
        HT_FREE(net);
	return YES;
    }
    return NO;
}

/*
**  Unregister the net object from the global list
**  and see if we can start a new pending request.
*/
PRIVATE BOOL unregister_net (HTNet * net)
{
    if (net && NetTable) {
	HTList * list = NetTable[net->hash];
	if (list) {
	    HTList_removeObject(list, (void *) net);
	    check_pending(net);
	    HTNetCount--;
	    return YES;
	}
    }
    return NO;
}

/*
**	Clears the contents of the Net object so that we can use it again.
*/
PUBLIC BOOL HTNet_clear (HTNet * net)
{
    if (net) {
	net->host->channel = NULL;
	net->readStream = NULL;
	net->bytesRead = 0;
	net->headerBytesRead = 0;
	net->bytesWritten = 0;
	net->headerBytesWritten = 0;
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
    HTTRACE(CORE_TRACE, "Net Object.. Delete %p and call AFTER filters\n" _ net);
    if (net) {
	HTRequest * request = net->request;

	/*
	** If we have a premature close then recover the request. Otherwise
	** break the link to the Host object and continue deleting the net
	** object
	*/
	if (net->host) {
	    HTHost_unregister (net->host, net, HTEvent_READ);
	    HTHost_unregister (net->host, net, HTEvent_WRITE);
	    if (status == HT_RECOVER_PIPE) {
		HTNet_clear(net);
		HTTRACE(CORE_TRACE, "Net Object.. Restarting request %p (retry=%d) with net object %p\n" _ 
			    request _ HTRequest_retrys(request) _ net);
		return YES;
	    }
            HTHost_deleteNet(net->host, net, status);
	    if (HTHost_doRecover(net->host)) HTHost_recoverPipe(net->host);
        }

        /* Remove object from the table of Net Objects */
	unregister_net(net);
        free_net(net);

    	/* Call AFTER filters */
	if (status != HT_IGNORE) HTNet_executeAfterAll(request, status);
        
	/*
	** Truely delete the HTNet object. Thanks to Mikhail Grouchinski
	** we now do this after having called the after filters so that
	** these filters can use the information in the Net object
	*/

	return YES;
    }
    return NO;
}

PUBLIC BOOL HTNet_deleteDup (HTNet * dup)
{
    return dup ? (unregister_net(dup) && free_net(dup)) : NO;
}

/*	HTNet_deleteAll
**	---------------
**	Deletes all HTNet object that might either be active or pending
**	We DO NOT call the AFTER filters - A crude way of saying goodbye!
*/
PUBLIC BOOL HTNet_deleteAll (void)
{
    HTTRACE(CORE_TRACE, "Net Object.. Remove all Net objects, NO filters\n"); 
    if (NetTable) {
	HTList * cur = NULL;
        HTNet * pres = NULL;
	int cnt;
	for (cnt=0; cnt<HT_XL_HASH_SIZE; cnt++) {
	    if ((cur = NetTable[cnt])) { 
		while ((pres = (HTNet *) HTList_nextObject(cur)) != NULL)  {
		    check_pending(pres);
                    free_net(pres);
		}
	    }
	    HTList_delete(NetTable[cnt]);
	}
	HT_FREE(NetTable);
	HTNetCount = 0;
	return YES;
    }
    return NO;
}

/*
**	When pipelining, it is not possible to kill a single request 
**	as we then loose track of where we are in the pipe. It is 
**	therefore necessary to kill the whole pipeline.
*/
PUBLIC BOOL HTNet_killPipe (HTNet * net)
{
    return (net && net->host) ? HTHost_killPipe(net->host) : NO;
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
    if (net) {
        HTAlertCallback * cbf = HTAlert_find(HT_PROG_INTERRUPT);
        if (cbf) (*cbf)(net->request, HT_PROG_INTERRUPT, HT_MSG_NULL, NULL, NULL, NULL);
	HTTRACE(CORE_TRACE, "Net Object.. Killing %p\n" _ net);
	if (net->event.cbf) {
	    (*(net->event.cbf))(HTNet_socket(net), net->event.param, HTEvent_CLOSE);
	    return YES;
	}
	return unregister_net(net) && free_net(net);
    }
    HTTRACE(CORE_TRACE, "Net Object.. No object to kill\n");
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
    HTTRACE(CORE_TRACE, "Net Object.. Kill ALL Net objects!!!\n"); 
    if (NetTable) {
	HTList * cur = NULL;
        HTNet * pres = NULL;
	int cnt;
	for (cnt=0; cnt<HT_XL_HASH_SIZE; cnt++) {
	    if ((cur = NetTable[cnt])) { 
		while ((pres = (HTNet *) HTList_lastObject(cur)) != NULL) {
		    HTNet_kill(pres);
		    cur = NetTable[cnt];
		}
	    }
	}
	return YES;
    }
    HTTRACE(CORE_TRACE, "Net Object.. No objects to kill\n");
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
    return (net ? net->event.priority : HT_PRIORITY_INV);
}

/*	HTNet_setPriority
**	-----------------
**	Set the current priority of the Net object
**	This will change the priority next time the thread is blocked
*/
PUBLIC BOOL HTNet_setPriority (HTNet * net, HTPriority priority)
{
    if (net) {
	net->event.priority = priority;
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
	BOOL result = HTHost_setPersistent(net->host, persistent, mode);
	HTTRACE(CORE_TRACE, "Net Object.. Persistent connection set %s %s\n" _ 
		    persistent ? "ON" : "OFF" _ 
		    result ? "succeeded" : "failed");
	return result;
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
    if (net && net->host && net->host->channel) {
	HTChannel_setSocket(net->host->channel, sockfd);
	return YES;
    }
    return NO;
}

PUBLIC SOCKET HTNet_socket (HTNet * net)
{
    return (net && net->host && net->host->channel ? HTChannel_socket(net->host->channel) : INVSOC);
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
**  Get and set the HTChannel object
*/
PUBLIC BOOL HTNet_setChannel (HTNet * net, HTChannel * channel)
{
    return (net && channel) ? HTHost_setChannel(net->host, channel) : NO;
}

PUBLIC HTChannel * HTNet_channel (HTNet * net)
{
    return net ? HTHost_channel(net->host) : NULL;
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
	net->host->dns = dns;
	return YES;
    }
    return NO;
}

PUBLIC HTdns * HTNet_dns (HTNet * net)
{
    return (net ? net->host->dns : NULL);
}

PUBLIC BOOL HTNet_setProtocol (HTNet * net, HTProtocol * protocol)
{
    if (net && protocol) {
	net->protocol = protocol;
	return YES;
    }
    return NO;
}

PUBLIC HTProtocol * HTNet_protocol (HTNet * net)
{
    return (net ? net->protocol : NULL);
}

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

PUBLIC BOOL HTNet_preemptive (HTNet * net)
{
    return (net ? net->preemptive : NO);
}

/*
**	Create the output stream and bind it to the channel
**	Please read the description in the HTIOStream module on the parameters
*/
PUBLIC HTOutputStream * HTNet_getOutput (HTNet * me, void * param, int mode)
{
    if (me && me->host && me->host->channel && me->transport) {
	HTTransport * tp = me->transport;
	HTChannel * ch = me->host->channel;
	HTOutputStream * output = (*tp->output_new)(me->host, ch, param, mode);
	HTChannel_setOutput(ch, output);
	return output;
    }
    HTTRACE(CORE_TRACE, "Host Object.. Can't create output stream\n");
    return NULL;
}

PUBLIC HTEvent * HTNet_event (HTNet * net)
{
    return net ? &net->event : NULL;
}

PUBLIC BOOL HTNet_setEventParam (HTNet * net, void * eventParam)
{
    if (net) return HTEvent_setParam(&net->event, eventParam);
    return NO;
}

PUBLIC void * HTNet_eventParam (HTNet * net)
{
    return net ? net->event.param : NULL;
}

PUBLIC BOOL HTNet_setEventCallback(HTNet * net, HTEventCallback * cbf)
{
    if (net) return HTEvent_setCallback(&net->event, cbf);
    return NO;
}

PUBLIC HTEventCallback * HTNet_eventCallback(HTNet * net)
{
    return net->event.cbf;
}

PUBLIC BOOL HTNet_setEventPriority(HTNet * net, HTPriority priority)
{
    if (net) return HTEvent_setPriority(&net->event, priority);
    return NO;
}

PUBLIC HTPriority HTNet_eventPriority(HTNet * net)
{
    return net->event.priority;
}

PUBLIC HTStream * HTNet_readStream(HTNet * net)
{
    if (!net) return NULL;
    return net->readStream;
}

PUBLIC BOOL HTNet_setReadStream (HTNet * net, HTStream * stream)
{
    if (net) {
	net->readStream = stream;
	return YES;
    }
    return NO;
}

/*
**	Should we do raw byte count at the network or later?
**	Normally it is later but in cases like FTP we need it
**	in the raw form
*/
PUBLIC BOOL HTNet_setRawBytesCount (HTNet * net, BOOL mode)
{
    if (net) {
	net->countRawBytes = mode;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTNet_rawBytesCount (HTNet * net)
{
    return (net && net->countRawBytes);
}
