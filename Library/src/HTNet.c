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

    /*
    ** Check if we can start the request, else put it into pending queue
    ** If so then call the call back function associated with the anchor.
    ** We use the INVSOC as we don't have a valid socket yet!
    */
    if (HTList_count(HTNetActive) < HTMaxActive) {
	HTList_addObject(HTNetActive, (void *) me);
	me->request->retrys++;
	if (THD_TRACE)
	    fprintf(TDEST, "HTNet_new... starting request %p (retry=%d)\n",
		    me->request, me->request->retrys);
	(*(me->cbf))(me->sockfd, me->request, FD_NONE);
    } else {
	if (!HTNetPending) HTNetPending = HTList_new();
	if (THD_TRACE)
	    fprintf(TDEST, "HTNet_new... request %p registered as pending\n",
		    me->request);
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
	    } else {
		if (THD_TRACE)
		    fprintf(TDEST, "HTNet_delete keeping %d\n", net->sockfd);
	    }
	    HTEvent_UnRegister(net->sockfd, (SockOps) FD_ALL);
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

	/* Remove object and call callback functions */
	HTRequest *request = net->request;
	HTList_removeObject(HTNetActive, (void *) net);
 	delete_object(net, status);
	HTNet_callback(request, status);

	/* See if we can start a pending request */
	if (HTList_count(HTNetActive) < HTMaxActive &&
	    HTList_count(HTNetPending)) {
	    HTNet *next = (HTNet *) HTList_removeFirstObject(HTNetPending);
	    if (next) {
		HTList_addObject(HTNetActive, (void *) next);
		next->request->retrys++;
		if (THD_TRACE)
		    fprintf(TDEST,"HTNet_delete start request %p from queue\n",
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
**	We DO NOT call the call back functions and we don't care about open
**	socket descriptors. A crude way of saying goodbye!
*/
PUBLIC BOOL HTNet_deleteAll (void)
{
    if (THD_TRACE) 
	fprintf(TDEST, "HTNetDelete. Removing ALL Net - NO call backs\n"); 
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
	HTNet *pres;
	while ((pres = (HTNet *) HTList_lastObject(HTNetActive)) != NULL) {
	    if (pres == me) {
		(*(pres->cbf))(INVSOC, pres->request, FD_CLOSE);
		return YES;
	    }
	}
    }
    if (THD_TRACE)
	fprintf(TDEST, "HTNet_kill.. Request %p is not registered\n", me);
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

    /* We start off in pending queue so we avoid racing */
    if (HTNetPending) {
	while ((pres = (HTNet *) HTList_lastObject(HTNetPending)) != NULL) {
	    (*(pres->cbf))(INVSOC, pres->request, FD_CLOSE);
	    HTList_removeObject(HTNetPending, pres);
	}
    }
    if (HTNetActive) {
	while ((pres = (HTNet *) HTList_lastObject(HTNetActive)) != NULL)
	    (*(pres->cbf))(INVSOC, pres->request, FD_CLOSE);
    }
    return YES;
}

#if 0
/*
**  LibraryCallback - "glue" between 3.0 thread code and new callback functions
**  map return codes into a simple yes/no model. 
*/
PRIVATE int LibraryCallback ARGS3(SOCKET, s, HTRequest *, rq, SockOps, f)
{
    int status = 0 ;
    HTEventState state ;
    HTProtocol * proto = (HTProtocol *)
            HTAnchor_protocol( rq -> anchor) ;

    /* begin */    

    if (proto == 0)   	/* Whoa! No protocol! */
    	return -1;
    status = proto->callback( s, rq, f) ;
    if (status != HT_WOULD_BLOCK) {   /* completed - good or bad... */
        if (THD_TRACE) 
            fprintf(TDEST, "LibCallBack. Calling Terminate...\n");
	if (status != HT_OK) {
	    HTLoadTerminate(rq, status);
	    state = HTEventRequestTerminate( rq, status) ;
	    /* if the state isn't EVENT_QUIT */
	    if (! HTEventCheckState( rq, state ))
		return HT_OK;  /* treat as failure */
	}
    }  /* if status */
    return HT_WOULD_BLOCK;
}

/*								  HTNetState
**
**	This function registers a socket as waiting for the action given
**	(read or write etc.).
**
**  	Charlie Brooks - we handle the interrupt thread state internally to this module 
**      setting the interrupt on a socket disables it from read/write.
*/
PUBLIC void HTNetState ARGS2(SOCKFD, sockfd, HTNetAction, action)
{
    register HTNet * pres ;
    HTList * cur = HTNetActive ;
    int found = 0 ;
    HTRequest * reqst ;

#ifdef _WIN32
    if (sockfd <= 2) 
	sockfd = _get_osfhandle(sockfd);
#endif
  
    if (THD_TRACE) {
	static char *actionmsg[] = {
	    "SET WRITE",
	    "CLEAR WRITE",
	    "SET READ",
	    "CLEAR READ",
	    "SET INTERRUPT",
	    "CLEAR INTERRUPT",
	    "CLOSE",
	    "SET CONNECT",
	    "CLEAR CONNECT"
	    };
	fprintf(TDEST,
		"Net......... Registering socket number %d for action %s\n",
		sockfd, *(actionmsg+action));
    }	 /* if */

    FD_SET( sockfd, &HTfd_libs) ;
    if (libMaxSock < sockfd)
    	libMaxSock = sockfd ;


    while ((pres = (HTNet *)HTList_nextObject(cur) ) != 0) { 
        if (pres->sockfd == sockfd) {
            found = 1 ;
            break ;
        }   /* if */
    }       /* while */

    if (! found)    /* how'd you get here? */
        return ;

    reqst = pres->request ;
    switch (action) {
      case THD_SET_WRITE:
      case THD_SET_CONNECT: 
        HTEvent_Register( sockfd, reqst, action == THD_SET_WRITE ? (SockOps)FD_WRITE : (SockOps)FD_CONNECT , 
        	LibraryCallback, 0);
	break;

      case THD_CLR_WRITE:
      case THD_CLR_CONNECT: 
        HTEvent_UnRegister( sockfd, action == THD_CLR_WRITE ? (SockOps)FD_WRITE : (SockOps)FD_CONNECT) ;
	break;

      case THD_SET_READ:
        HTEvent_Register( sockfd, reqst, (SockOps)FD_READ, LibraryCallback, 0);  
	break;

      case THD_CLR_READ:
        HTEvent_UnRegister( sockfd, FD_WRITE) ;
	break;

      case THD_CLOSE:
        HTEvent_UnRegister( sockfd, FD_ALL) ;
	FD_CLR( sockfd, &HTfd_libs);
	FD_CLR( sockfd, &HTfd_intr);
	libMaxSock = 0 ;
	while ((pres = (HTNet *)HTList_nextObject(cur) ) != 0) { 
	    if (pres->sockfd > libMaxSock) {
		libMaxSock = sockfd ;
	    }   /* if */
	}       /* while */

	break;

/*
 * we handle interrupts locally ... only library sockets can 
 * be interrupted? 
 */

      case THD_SET_INTR:
        HTEvent_UnRegister( sockfd, (SockOps)(FD_READ | FD_WRITE) );
        FD_SET( sockfd, &HTfd_intr) ;
	break;

      case THD_CLR_INTR:
        FD_CLR( sockfd, &HTfd_intr) ;
	HTEvent_UnRegister(sockfd, FD_ALL) ;  /* no sin to unregister and unregistered socket */
	break;

      default:
	if (THD_TRACE)
	    fprintf(TDEST, "Net...... Illegal socket action (%d)\n", (int)action);
    }
    return ;
}
#endif


