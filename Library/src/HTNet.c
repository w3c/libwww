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

typedef struct _CBFInfo {
    HTNetCallBack *	cbf;
    int 		status;	     /* Status associated with this callback */
} CBFInfo;

struct _HTStream {
    CONST HTStreamClass *	isa;
    /* ... */
};

PRIVATE int 	HTMaxActive = HT_MAX_SOCKETS;  	      /* Max active requests */
PRIVATE HTList *HTNetCBF = NULL;	      /* List of call back functions */

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
/*			  Termination Call Back Functions		     */
/* ------------------------------------------------------------------------- */

/*	HTNet_Register
**	--------------
**	Register a call back function that is to be called on every
**	termination of a request. Several call back functions can be registered
**	in which case all of them are called in the order of which they
**	were registered.
**
**	The status signifies which call back function to call depending of the 
**	result of the request. This can be
**
**		HT_ERROR	An error occured
**		HT_LOADED	The document was loaded
**		HT_NO_DATA	OK, but no data
**		HT_RETRY	Retry request after at a later time
**		HT_ALL		All of above
*/
PUBLIC BOOL HTNet_register (HTNetCallBack *cbf, int status)
{
    if (THD_TRACE) 
	fprintf(TDEST, "Net register HTNetCallBack %p\n", (void *) cbf);
    if (cbf) {
	CBFInfo *cbfinfo = (CBFInfo *) calloc(1, sizeof(CBFInfo));
	if (!cbfinfo) outofmem(__FILE__, "HTNet_register");
	cbfinfo->cbf = cbf;
	cbfinfo->status = status;
	if (!HTNetCBF) HTNetCBF = HTList_new();
	return HTList_addObject(HTNetCBF, (void *) cbfinfo);
    }
    return NO;
}

/*	HTNet_unregister
**	--------------
**	Unregister a call back function that is to be called on every
**	termination of a request.
*/
PUBLIC BOOL HTNet_unregister (HTNetCallBack *cbf)
{
    if (THD_TRACE) 
	fprintf(TDEST, "Net unreg.. HTNetCallBack %p\n", (void *) cbf);
    if (HTNetCBF && cbf) {
	HTList *cur = HTNetCBF;
	CBFInfo *pres;
	while ((pres = (CBFInfo *) HTList_nextObject(cur))) {
	    if (pres->cbf == cbf) {
		HTList_removeObject(HTNetCBF, (void *) pres);
		free(pres);
		return YES;
	    }
	}
    }
    return NO;
}

/*	HTNet_unregisterAll
**	-------------------
**	Unregisters all call back functions
*/
PUBLIC BOOL HTNet_unregisterAll (void)
{
    if (THD_TRACE) 
	fprintf(TDEST, "Net unreg.. All callback functions\n");
    if (HTNetCBF) {
	HTList *cur = HTNetCBF;
	CBFInfo *pres;
	while ((pres = (CBFInfo *) HTList_nextObject(cur))) {
	    HTList_removeObject(HTNetCBF, (void *) pres);
	    free(pres);
	}
	HTList_delete(HTNetCBF);
	HTNetCBF = NULL;
	return YES;
    }
    return NO;
}

/*	HTNet_callback
**	--------------
**	Call all the call back functions registered in the list IF not the 
**	status is HT_IGNORE.
**	The callback functions are called in the order of which they
**	were registered. At the moment an application callback function is
**	called, it can free the request object - it is no longer used by the
**	Library.
**	Returns YES if OK, else NO.
*/
PUBLIC BOOL HTNet_callback (HTRequest * request, int status)
{
    if (HTNetCBF && request && status != HT_IGNORE) {	
	int cnt = HTList_count(HTNetCBF);
	while (--cnt >= 0) {
	    CBFInfo *pres = HTList_objectAt(HTNetCBF, cnt);
	    if (pres && (pres->status == status || pres->status == HT_ALL)) {
		if (THD_TRACE)
		    fprintf(TDEST, "Net callback %p (request=%p, status=%d)\n",
			    (void *) pres->cbf, request, status);
		(*(pres->cbf))(request, status);
	    }
	}
	return YES;
    }
    return NO;
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

/*							     	   HTNet_new
**
**	Create a new HTNet object as a new request to be handled. If we have
**	more than HTMaxActive connections already then put this into the
**	pending queue, else start the request by calling the call back
**	function registered with this access method. 
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTNet_new (HTRequest * request, HTPriority priority)
{
    HTNet *me;
    HTProtocol *prot;
    if (!request) return HT_ERROR;
    if (!HTNetActive) HTNetActive = HTList_new();
    prot = HTAnchor_protocol(request->anchor);

    /* Create new net object and bind it to the request object */
    if ((me = (HTNet *) calloc(1, sizeof(HTNet))) == NULL)
	outofmem(__FILE__, "HTNet_new");
    me->request = request;
    request->net = me;
    me->preemtive = (HTProtocol_preemtive(prot) || request->preemtive);
    me->priority = priority;
    me->sockfd = INVSOC;
    if (!(me->cbf = HTProtocol_callback(prot))) {
	if (THD_TRACE)
	    fprintf(TDEST, "HTNet_new... NO CALL BACK FUNCTION!\n");
	free(me);
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
	if (THD_TRACE)
	    fprintf(TDEST, "HTNet_new... starting request %p (retry=%d)\n",
		    request, request->retrys);
	(*(me->cbf))(me->sockfd, request, FD_NONE);
    } else {
	if (!HTNetPending) HTNetPending = HTList_new();
	if (THD_TRACE)
	    fprintf(TDEST, "HTNet_new... request %p registered as pending\n",
		    request);
	HTProgress(request, HT_PROG_WAIT, NULL);
	HTList_addObject(HTNetPending, (void *) me);	
    }
    return YES;
}

/*	delete_object
**	-------------
**	Deletes an HTNet object
*/
PRIVATE BOOL delete_object (HTNet *net, int status)
{
    if (THD_TRACE)
	fprintf(TDEST, "HTNet_delete Remove net object %p\n", net);
    if (net) {
	int status = 0;

	/* Free stream with data FROM network to application */
	if (net->target) {
	    if (status == HT_INTERRUPTED)
		(*net->target->isa->abort)(net->target, NULL);
	    else
		(*net->target->isa->_free)(net->target);
	}

	/* Close socket */
	if (net->sockfd != INVSOC) {
	    if (HTDNS_socket(net->dns) == INVSOC) {
		if ((status = NETCLOSE(net->sockfd)) < 0)
		    HTErrorSysAdd(net->request, ERR_FATAL, socerrno, NO,
				  "NETCLOSE");
		if (THD_TRACE)
		    fprintf(TDEST, "HTNet_delete closing %d\n", net->sockfd);
		HTEvent_UnRegister(net->sockfd, (SockOps) FD_ALL);
	    } else {
		if (THD_TRACE)
		    fprintf(TDEST, "HTNet_delete keeping %d\n", net->sockfd);
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
	free(net);
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
*/
PUBLIC BOOL HTNet_delete (HTNet * net, int status)
{
    if (THD_TRACE) 
	fprintf(TDEST,"HTNetDelete. Net Object and call callback functions\n");
    if (HTNetActive && net) {
	SOCKFD cs = net->sockfd;			   /* Current sockfd */

	/* Remove object and call callback functions */
	HTRequest *request = net->request;
	HTList_removeObject(HTNetActive, (void *) net);
 	delete_object(net, status);
	HTNet_callback(request, status);

	/*
	** See first if we have a persistent request queued up for this socket
	** If not then see if there is a pending request
	*/
	if (HTNetPersistent) {
	    HTList *cur = HTNetPersistent;
	    HTNet *next;
	    while ((next = (HTNet *) HTList_nextObject(cur))) {
		if (next->sockfd == cs) {
		    BOOL e1, e2;
		    if (THD_TRACE)
			fprintf(TDEST, "HTNet delete launch WARM request %p\n",
				next->request);
		    e1 = HTList_addObject(HTNetActive, (void *) next);
		    e2 = HTList_removeObject(HTNetPersistent, (void *) next);
		    fprintf(TDEST, "TEST........ (%d) and (%d)\n",
			    e1+0x30, e2+0x30);
		    (*(next->cbf))(next->sockfd, next->request, FD_NONE);
		    break;
		}
	    }
	} else if (HTList_count(HTNetActive) < HTMaxActive &&
		   HTList_count(HTNetPending)) {
	    HTNet *next = (HTNet *) HTList_removeFirstObject(HTNetPending);
	    if (next) {
		HTList_addObject(HTNetActive, (void *) next);
		if (THD_TRACE)
		    fprintf(TDEST,"HTNet delete launch PENDING request %p\n",
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
    if (THD_TRACE) 
	fprintf(TDEST, "HTNetDelete. Remove all Net objects, NO callback\n"); 
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
*/
PUBLIC BOOL HTNet_wait (HTNet *net)
{
    if (net) {
	if (THD_TRACE)
	    fprintf(TDEST,"HTNet_wait.. request %p is waiting for socket %d\n",
		    net->request, net->sockfd);
	if (!HTNetPersistent) HTNetPersistent = HTList_new();
	HTList_addObject(HTNetPersistent, (void *) net);	
	return YES;
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
    if (THD_TRACE)
	fprintf(TDEST, "HTNet_kill.. object %p is not registered\n", me);
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
    if (THD_TRACE)
	fprintf(TDEST, "HTNet_kill.. ALL registered requests!!!\n");

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
