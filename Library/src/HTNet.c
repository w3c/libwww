/*								     HTNet.c
**	ASYNCRONOUS SOCKET MANAGEMENT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
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
#include "tcp.h"

/* Library include files */
#include "HTUtils.h"
#include "HTProt.h"
#include "HTError.h"
#include "HTAlert.h"
#include "HTParse.h"
#include "HTReqMan.h"
#include "HTEvntrg.h"
#include "HTStream.h"
#include "HTNetMan.h"					 /* Implemented here */

#ifdef WIN32
#include <io.h>
#endif

#ifndef HT_MAX_SOCKETS
#define HT_MAX_SOCKETS	6
#endif

typedef struct _NetCall {
    HTNetCallback *	cbf;
    int 		status;	     /* Status associated with this callback */
} NetCall;

struct _HTStream {
    CONST HTStreamClass *	isa;
    /* ... */
};

PRIVATE int 	HTMaxActive = HT_MAX_SOCKETS;  	      /* Max active requests */
PRIVATE HTList *HTBefore = NULL;	      /* List of call back functions */
PRIVATE HTList *HTAfter = NULL;	      	      /* List of call back functions */

PRIVATE HTList *HTNetActive = NULL;               /* List of active requests */
PRIVATE HTList *HTNetPending = NULL;		 /* List of pending requests */
PRIVATE HTList *HTNetPersistent = NULL;	   /* List of persistent connections */

/* ------------------------------------------------------------------------- */

/*
**	Set the max number of simultanous sockets. Default is HT_MAX_SOCKETS
*/
PUBLIC BOOL HTNet_setMaxSocket (int newmax)
{
    if (newmax > 0) {
	HTMaxActive = newmax;
	return YES;
    }
    return NO;
}

PUBLIC int HTNet_maxSocket (void)
{
    return HTMaxActive;
}

/* ------------------------------------------------------------------------- */
/*			  	Call Back Functions			     */
/* ------------------------------------------------------------------------- */

/*	HTNetCall_add
**	-------------
**	Register a call back function that is to be called on every request.
**	Several call back functions can be registered
**	in which case all of them are called in the order of which they
**	were registered.
**
**	The status signifies which call back function to call depending of the 
**	result of the request. This can be
**
**		HT_ERROR	An error occured
**		HT_LOADED	The document was loaded
**		HT_NO_DATA	OK, but no data
**		HT_REDIRECT	If we received a redirection
**		HT_RETRY	Retry request after at a later time
**		HT_ALL		All of above
*/
PUBLIC BOOL HTNetCall_add (HTList * list, HTNetCallback *cbf, int status)
{
    if (WWWTRACE) 
	TTYPrint(TDEST, "Call Add.... HTNetCallback %p\n", (void *) cbf);
    if (list && cbf) {
	NetCall *me;
	if ((me = (NetCall  *) HT_CALLOC(1, sizeof(NetCall))) == NULL)
	    HT_OUTOFMEM("HTNetCall_add");
	me->cbf = cbf;
	me->status = status;
	return HTList_addObject(list, (void *) me);
    }
    return NO;
}

/*	HTNetCall_delete
**	----------------
**	Unregister a call back function from a list
*/
PUBLIC BOOL HTNetCall_delete (HTList * list, HTNetCallback *cbf)
{
    if (WWWTRACE) 
	TTYPrint(TDEST, "Call delete HTNetCallback %p\n", (void *) cbf);
    if (list && cbf) {
	HTList *cur = list;
	NetCall *pres;
	while ((pres = (NetCall *) HTList_nextObject(cur))) {
	    if (pres->cbf == cbf) {
		HTList_removeObject(list, (void *) pres);
		HT_FREE(pres);
		return YES;
	    }
	}
    }
    return NO;
}

/*	HTNetCall_deleteAll
**	-------------------
**	Unregisters all call back functions
*/
PUBLIC BOOL HTNetCall_deleteAll (HTList * list)
{
    if (WWWTRACE) 
	TTYPrint(TDEST, "Call delete All callback functions\n");
    if (list) {
	HTList *cur = list;
	NetCall *pres;
	while ((pres = (NetCall *) HTList_nextObject(cur))) {
	    HTList_removeObject(list, (void *) pres);
	    HT_FREE(pres);
	}
	HTList_delete(list);
	return YES;
    }
    return NO;
}

/*	HTNetCall_execute
**	-----------------
**	Call all the call back functions registered in the list IF not the 
**	status is HT_IGNORE.
**	The callback functions are called in the order of which they
**	were registered. At the moment an application callback function is
**	called, it can free the request object - it is no longer used by the
**	Library.
**	Returns what the last callback function returns
*/
PUBLIC int HTNetCall_execute (HTList * list, HTRequest * request, int status)
{
    int ret = HT_OK;
    if (list && request && status != HT_IGNORE) {	
	int cnt = HTList_count(list);
	while (--cnt >= 0) {
	    NetCall *pres = (NetCall *) HTList_objectAt(list, cnt);
	    if (pres && (pres->status == status || pres->status == HT_ALL)) {
		if (WWWTRACE)
		    TTYPrint(TDEST,"Net callback %p (request=%p, status=%d)\n",
			    (void *) pres->cbf, request, status);
		if ((ret = (*(pres->cbf))(request, status)) != HT_OK) break;
	    }
	}
    }
    return ret;
}

/*
**	Global set of callback functions BEFORE the request is issued
**	list can be NULL
*/
PUBLIC BOOL HTNet_setBefore (HTList *list)
{
    if (HTBefore) HTNetCall_deleteAll(HTBefore);
    HTBefore = list;
    return YES;
}

PUBLIC HTList * HTNet_before (void)
{
    return HTBefore;
}

PUBLIC int HTNet_callBefore (HTRequest *request, int status)
{
    return HTNetCall_execute(HTBefore, request, status);
}

PUBLIC BOOL HTNetCall_addBefore (HTNetCallback *cbf, int status)
{
    if (!HTBefore) HTBefore = HTList_new();
    return HTNetCall_add(HTBefore, cbf, status);
}

/*
**	Global set of callback functions AFTER the request is issued
**	list can be NULL
*/
PUBLIC BOOL HTNet_setAfter (HTList *list)
{
    if (HTAfter) HTNetCall_deleteAll(HTAfter);
    HTAfter = list;
    return YES;
}

PUBLIC HTList * HTNet_after (void)
{
    return HTAfter;
}

PUBLIC int HTNet_callAfter (HTRequest *request, int status)
{
    return HTNetCall_execute(HTAfter, request, status);
}

PUBLIC BOOL HTNetCall_addAfter (HTNetCallback *cbf, int status)
{
    if (!HTAfter) HTAfter = HTList_new();
    return HTNetCall_add(HTAfter, cbf, status);
}

/* ------------------------------------------------------------------------- */
/*			  	Request Queue				     */
/* ------------------------------------------------------------------------- */

/*	HTNet_activeQueue
**	-----------------
**	Returns the list of active requests that are currently having an open
**	connection.
**	Returns list of HTNet objects or NULL if error
*/
PUBLIC HTList *HTNet_activeQueue (void)
{
    return HTNetActive;
}

/*	HTNet_idle
**	----------
**	Returns whether there are active requests
*/
PUBLIC BOOL HTNet_idle (void)
{
    return HTList_isEmpty(HTNetActive);
}

/*	HTNet_empty
**	-----------
**	Returns whether there are requests registered or not
*/
PUBLIC BOOL HTNet_isEmpty (void)
{
    return (HTList_isEmpty(HTNetActive) && HTList_isEmpty(HTNetPersistent) &&
	    HTList_isEmpty(HTNetPending));
}

/*	HTNet_pendingQueue
**	------------------
**	Returns the list of pending requests that are waiting to become active
**	Returns list of HTNet objects or NULL if error
*/
PUBLIC HTList *HTNet_pendingQueue (void)
{
    return HTNetPending;
}

/* ------------------------------------------------------------------------- */
/*			  Creation and deletion methods  		     */
/* ------------------------------------------------------------------------- */

/*	HTNet_duplicate
**	---------------
**	Creates a new HTNet object as a duplicate of the same request.
**	Returns YES if OK, else NO
**	BUG: We do not check if we have a socket free!
*/
PUBLIC HTNet * HTNet_dup (HTNet * src)
{
    HTNet * me;
    if (!src) return NO;
    if ((me = (HTNet *) HT_MALLOC(sizeof(HTNet))) == NULL)
	HT_OUTOFMEM("HTNet_dup");
    memcpy((void *) me, src, sizeof(HTNet));
    return me;
}

/*	HTNet_priority
**	--------------
**	Get the current priority of the Net object
*/
PUBLIC HTPriority HTNet_priority (HTNet * net)
{
    return (net ? net->priority : -1);
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

/*	create_object
**	-------------
**	Creates an HTNet object
*/
PRIVATE HTNet * create_object (HTRequest * request)
{
    HTNet * me;
    if ((me = (HTNet *) HT_CALLOC(1, sizeof(HTNet))) == NULL)
        HT_OUTOFMEM("HTNet_new");
    me->request = request;
    request->net = me;
    me->tcpstate = TCP_BEGIN;
    if (!HTNetActive) HTNetActive = HTList_new();
    return me;
}

/*	HTNet_new
**	---------
**	This function creates a new HTNet object and assigns the socket number
**	to it. This is intended to be used when you are going to listen on a 
**	socket using the HTDoListen() function in HTTCP.c. The function do NOT
**	call any of the callback functions.
**	Returns new object or NULL on error
*/
PUBLIC HTNet * HTNet_new (HTRequest * request, SOCKET sockfd)
{
    HTNet * me;
    if (WWWTRACE) TTYPrint(TDEST, "HTNet_new... Create empty Net object\n");
    if (!request || sockfd==INVSOC) return NULL;
    if ((me = create_object(request)) == NULL) return NULL;
    me->preemptive = request->preemptive;
    me->priority = request->priority;
    me->sockfd = sockfd;
    return me;
}

/*	HTNet_newServer
**	---------------
**	Create a new HTNet object as a new request to be handled. If we have
**	more than HTMaxActive connections already then return NO.
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTNet_newServer (HTRequest * request, SOCKET sockfd, char * access)
{
    HTNet * me;
    HTProtocol * protocol;
    if (!request) return NO;

    /* Check if we can start the request, else return immediately */
    if (HTList_count(HTNetActive) > HTMaxActive) {
	if (PROT_TRACE) TTYPrint(TDEST, "HTNet new... NO SOCKET AVAILABLE\n");
	HTNetCall_execute(HTAfter, request, HT_RETRY);
	return YES;
    }

    /* Find a protocol object for this access scheme */
    protocol = HTProtocol_find(request, access);
	
    /* Create new net object and bind it to the request object */
    if ((me = create_object(request)) == NULL) return NO;
    me->preemptive = (HTProtocol_preemptive(protocol) || request->preemptive);
    me->priority = request->priority;
    me->sockfd = sockfd;
    if (!(me->cbf = HTProtocol_server(protocol))) {
	if (WWWTRACE) TTYPrint(TDEST, "HTNet_new... NO CALL BACK FUNCTION!\n");
	HT_FREE(me);
	return NO;
    }
    request->retrys++;

    /* Start the server request */
    HTList_addObject(HTNetActive, (void *) me);
    if (WWWTRACE)
	TTYPrint(TDEST, "HTNet_new... starting SERVER request %p with net object %p\n", request, me);
    (*(me->cbf))(me->sockfd, request, FD_NONE);
    return YES;
}

/*	HTNet_new
**	---------
**	Create a new HTNet object as a new request to be handled. If we have
**	more than HTMaxActive connections already then put this into the
**	pending queue, else start the request by calling the call back
**	function registered with this access method. 
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTNet_newClient (HTRequest * request)
{
    int status;
    HTNet * me;
    HTProtocol * protocol;
    char * physical = NULL;
    if (!request) return NO;
    /*
    ** First we do all the "BEFORE" callbacks in order to see if we are to
    ** continue with this request or not. If we receive a callback status
    ** that is NOT HT_OK then jump directly to the after callbacks and return
    */
    if ((status = HTNetCall_execute(HTBefore, request, HT_OK)) != HT_OK) {
	HTNetCall_execute(HTAfter, request, status);
	return YES;
    }

    /*
    ** If no translation was provided by the application then use the anchor
    ** address directly
    */
    if (!(physical = HTAnchor_physical(request->anchor)) || !*physical) {
	char * addr = HTAnchor_address((HTAnchor *) request->anchor);
	if (WWWTRACE) TTYPrint(TDEST, "HTNet New... Using default address\n");
	HTAnchor_setPhysical(request->anchor, addr);
	physical = HTAnchor_physical(request->anchor);
	HT_FREE(addr);
    }

    /* Find a protocol object for this access scheme */
    {
	char * access = HTParse(physical, "", PARSE_ACCESS);
	if ((protocol = HTProtocol_find(request, access)) == NULL) {
	    if (WWWTRACE) TTYPrint(TDEST, "HTNet_new... NO PROTOCOL OBJECT\n");
	    HT_FREE(access);
	    return NO;
	}
	HT_FREE(access);
    }
	
    /* Create new net object and bind it to the request object */
    if ((me = create_object(request)) == NULL) return NO;
    me->preemptive = (HTProtocol_preemptive(protocol) || request->preemptive);
    me->priority = request->priority;
    me->sockfd = INVSOC;
    if (!(me->cbf = HTProtocol_client(protocol))) {
	if (WWWTRACE) TTYPrint(TDEST, "HTNet_new... NO CALL BACK FUNCTION!\n");
	HT_FREE(me);
	return NO;
    }
    request->retrys++;

    /*
    ** Check if we can start the request, else put it into pending queue
    ** If so then call the call back function associated with the anchor.
    ** We use the INVSOC as we don't have a valid socket yet!
    */
    if (HTList_count(HTNetActive) < HTMaxActive) {
	HTList_addObject(HTNetActive, (void *) me);
	if (WWWTRACE)
	    TTYPrint(TDEST, "HTNet_new... starting request %p (retry=%d) with net object %p\n",
		    request, request->retrys, me);
	(*(me->cbf))(me->sockfd, request, FD_NONE);
    } else {
	HTAlertCallback *cbf = HTAlert_find(HT_PROG_WAIT);
	if (!HTNetPending) HTNetPending = HTList_new();
	if (WWWTRACE)
	    TTYPrint(TDEST, "HTNet_new... request %p registered as pending\n",
		    request);
	if (cbf) (*cbf)(request, HT_PROG_WAIT, HT_MSG_NULL, NULL, NULL, NULL);
	HTList_addObject(HTNetPending, (void *) me);	
    }
    return YES;
}

/*	delete_object
**	-------------
**	Deletes an HTNet object
**	Return YES if OK, else NO
*/
PRIVATE BOOL delete_object (HTNet *net, int status)
{
    if (WWWTRACE)
	TTYPrint(TDEST, "HTNet_delete Remove net object %p\n", net);
    if (net) {
	int status = 0;

	/* Free stream with data FROM network to application */
	if (net->target) {
	    if (status == HT_INTERRUPTED)
		(*net->target->isa->abort)(net->target, NULL);
	    else
		(*net->target->isa->_free)(net->target);
	    net->target = NULL;
	}

	/* Close socket */
	if (net->sockfd != INVSOC) {
	    HTEvent_UnRegister(net->sockfd, (SockOps) FD_ALL);
	    if (HTDNS_socket(net->dns) == INVSOC) {
		if ((status = NETCLOSE(net->sockfd)) < 0)
		    HTRequest_addSystemError(net->request, ERR_FATAL,
					     socerrno, NO, "NETCLOSE");
		if (WWWTRACE)
		    TTYPrint(TDEST, "HTNet_delete closing %d\n", net->sockfd);
	    } else {
		if (WWWTRACE)
		    TTYPrint(TDEST, "HTNet_delete keeping %d\n", net->sockfd);
		HTDNS_clearActive(net->dns);
		/* Here we should probably use a low priority */
		HTEvent_Register(net->sockfd, net->request, (SockOps) FD_READ,
				 HTDNS_closeSocket, net->priority);
	    }
	}
	if (net->isoc)
	    HTInputSocket_free(net->isoc);
	if (net->request)
	    net->request->net = NULL;		    /* Break link to request */
	HT_FREE(net);
	return status ? NO : YES;
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
    if (WWWTRACE) 
	TTYPrint(TDEST,"HTNetDelete. Object and call callback functions\n");
    if (HTNetActive && net) {
	SOCKET cs = net->sockfd;			   /* Current sockfd */

	/* Remove object and call callback functions */
	HTRequest *request = net->request;
	if (HTList_removeObject(HTNetActive, (void *) net) != YES)
	    if (WWWTRACE)
		TTYPrint(TDEST, "HTNetDelete. %p not registered!\n", net);
 	delete_object(net, status);
	HTNetCall_execute(HTAfter, request, status);

	/*
	** See first if we have a persistent request queued up for this socket
	** If not then see if there is a pending request
	*/
	if (HTNetPersistent) {
	    HTList *cur = HTNetPersistent;
	    HTNet *next;
	    while ((next = (HTNet *) HTList_nextObject(cur))) {
		if (next->sockfd == cs) {
		    if (WWWTRACE)
			TTYPrint(TDEST, "HTNet delete Launch request %p on WARM socket %d (net object %p)\n",
				 next->request, next->sockfd, next);
		    HTList_addObject(HTNetActive, (void *) next);
		    HTList_removeObject(HTNetPersistent, (void *) next);
		    (*(next->cbf))(next->sockfd, next->request, FD_WRITE);
		    break;
		}
	    }
	} else if (HTList_count(HTNetActive) < HTMaxActive &&
		   HTList_count(HTNetPending)) {
	    HTNet *next = (HTNet *) HTList_removeFirstObject(HTNetPending);
	    if (next) {
		HTList_addObject(HTNetActive, (void *) next);
		if (WWWTRACE)
		    TTYPrint(TDEST,"HTNet delete launch PENDING request %p\n",
			    next->request);
		(*(next->cbf))(INVSOC, next->request, FD_NONE);
	    }
	}
	return YES;
    }
    return NO;
}

/*	HTNet_deleteAll
**	---------------
**	Deletes all HTNet object that might either be active or pending
**	We DO NOT call the call back functions - A crude way of saying goodbye!
*/
PUBLIC BOOL HTNet_deleteAll (void)
{
    if (WWWTRACE) 
	TTYPrint(TDEST, "HTNetDelete. Remove all Net objects, NO callback\n"); 
    if (HTNetPersistent) {
	HTList *cur = HTNetPersistent;
	HTNet *pres;
	while ((pres = (HTNet *) HTList_nextObject(cur))) {
	    pres->sockfd = INVSOC;	    /* Don't close it more than once */
	    delete_object(pres, HT_INTERRUPTED);
	}
	HTList_delete(HTNetPersistent);
	HTNetPersistent = NULL;
    }
    if (HTNetPending) {
	HTList *cur = HTNetPending;
	HTNet *pres;
	while ((pres = (HTNet *) HTList_nextObject(cur)))
	    delete_object(pres, HT_INTERRUPTED);
	HTList_delete(HTNetPending);
	HTNetPending = NULL;
    }
    if (HTNetActive) {
	HTList *cur = HTNetActive;
	HTNet *pres;
	while ((pres = (HTNet *) HTList_nextObject(cur)))
	    delete_object(pres, HT_INTERRUPTED);
	HTList_delete(HTNetActive);
	HTNetActive = NULL;
    }
    return NO;
}

/*	HTNet_wait
**	----------
**	Let a net object wait for a persistent socket. It will be launched
**	from the HTNet_delete() function
**	Return YES if OK, else NO
*/
PUBLIC BOOL HTNet_wait (HTNet *net)
{
    if (net) {
	if (WWWTRACE)
	    TTYPrint(TDEST,"HTNet_wait.. request %p is waiting for presistent socket %d\n",
		     net->request, net->sockfd);

	/* Take it out of the active queue and add it to persistent queue */
	if (HTList_removeObject(HTNetActive, (void *) net) != YES) {
	    if (WWWTRACE) TTYPrint(TDEST, "HTNet_wait.. not registered!\n");
	    return NO;
	}
	if (!HTNetPersistent) HTNetPersistent = HTList_new();
	return HTList_addObject(HTNetPersistent, (void *) net);	
    }
    return NO;
}

/* ------------------------------------------------------------------------- */
/*			        Killing requests  			     */
/* ------------------------------------------------------------------------- */

/*	HTNet_kill
**	----------
**	Kill the request by calling the call back function with a request for 
**	closing the connection. Does not remove the object. This is done by
**	HTNet_delete() function which is called by the load routine.
**	Returns OK if success, NO on error
*/
PUBLIC BOOL HTNet_kill (HTNet * me)
{
    if (HTNetActive && me) {
	HTList *cur = HTNetActive;
	HTNet *pres;
	while ((pres = (HTNet *) HTList_nextObject(cur))) {
	    if (pres == me) {
		(*(pres->cbf))(pres->sockfd, pres->request, FD_CLOSE);
		return YES;
	    }
	}
    }
    if (WWWTRACE)
	TTYPrint(TDEST, "HTNet_kill.. object %p is not registered\n", me);
    return NO;
}

/*	HTNet_killAll
**	-------------
**	Kills all registered (active+pending) requests by calling the call
**	back function with a request for closing the connection. We do not
**	remove the HTNet object as it is done by HTNet_delete().
**	Returns OK if success, NO on error
*/
PUBLIC BOOL HTNet_killAll (void)
{
    HTNet *pres;
    if (WWWTRACE)
	TTYPrint(TDEST, "HTNet_kill.. ALL registered requests!!!\n");

    /* We start off in persistent queue so we avoid racing */
    if (HTNetPersistent) {
	while ((pres = (HTNet *) HTList_lastObject(HTNetPersistent)) != NULL) {
	    pres->sockfd = INVSOC;
	    (*(pres->cbf))(pres->sockfd, pres->request, FD_CLOSE);
	    HTList_removeObject(HTNetPersistent, pres);
	}
    }
    if (HTNetPending) {
	while ((pres = (HTNet *) HTList_lastObject(HTNetPending)) != NULL) {
	    (*(pres->cbf))(pres->sockfd, pres->request, FD_CLOSE);
	    HTList_removeObject(HTNetPending, pres);
	}
    }
    if (HTNetActive) {
	while ((pres = (HTNet *) HTList_lastObject(HTNetActive)) != NULL)
	    (*(pres->cbf))(pres->sockfd, pres->request, FD_CLOSE);
    }
    return YES;
}

/* ------------------------------------------------------------------------- */
/*			      Data Access Methods  			     */
/* ------------------------------------------------------------------------- */

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

