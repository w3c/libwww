/*				    				     HTEvtLst.c
**	EVENT MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Updated HTEvent module 
**	This new module combines the functions of the old HTEvent module and 
**	the HTThread module. We retain the old HTThread module, but it
**	consists of calls to the HTEvent interfaces
**
** Authors:
**	HFN	Henrik Frystyk <frystyk@w3.org>
**	CLB    	Charlie Brooks <cbrooks@osf.org>
** Bugs
**
*/

/*   WSAAsyncSelect and windows app stuff need the following definitions:
 *   WWW_WIN_ASYNC - enable WSAAsyncSelect instead of select
 *   _WIN23 - win32 libararies - may be window or console app
 *   _WINSOCKAPI_ - using WINSOCK.DLL - not necessarily the async routines.
 *   _CONSOLE - the console app for NT
 *
 * first pass: EGP - 10/26/95
 */

/* Implementation dependent include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTReqMan.h"
#include "HTTimer.h"
#include "HTEvent.h"
#include "HTEvtLst.h"					 /* Implemented here */

/* Type definitions and global variables etc. local to this module */
#define MILLI_PER_SECOND	1000
#define HASH(s)			((s) % HT_M_HASH_SIZE) 

#define EVENTS_TO_EXECUTE	10 /* how many to execute in one select loop */

#define HT_FS_BYTES(a)		((((a)/16)+1) * 4)

typedef struct {
    SOCKET 	s ;	 		/* our socket */
    HTEvent * 	events[HTEvent_TYPES];	/* event parameters for read, write, oob */
    HTTimer *	timeouts[HTEvent_TYPES];
} SockEvents;

typedef struct {
    HTEvent *	event;
    SOCKET	s;
    HTEventType	type;
    HTPriority	skipped;
} EventOrder;

typedef enum {
    SockEvents_mayCreate,
    SockEvents_find
} SockEvents_action;

PRIVATE HTList * HashTable [HT_M_HASH_SIZE]; 
PRIVATE HTList * EventOrderList = NULL;
PRIVATE int HTEndLoop = 0;		       /* If !0 then exit event loop */
PRIVATE BOOL HTInLoop = NO;

#ifdef WWW_WIN_ASYNC
#define TIMEOUT	1 /* WM_TIMER id */
PRIVATE HWND HTSocketWin;
PRIVATE ATOM HTclass;
PRIVATE HINSTANCE HTinstance;
PRIVATE unsigned long HTwinMsg;
#else /* WWW_WIN_ASYNC */
PRIVATE fd_set FdArray[HTEvent_TYPES];
PRIVATE SOCKET MaxSock = 0;			  /* max socket value in use */
#endif /* !WWW_WIN_ASYNC */

/* ------------------------------------------------------------------------- */
/* 				DEBUG FUNCTIONS	    		             */
/* ------------------------------------------------------------------------- */

#ifdef HTDEBUG
PRIVATE void Event_trace (HTEvent * event)
{
    if (event) {
	HTTRACE(ALL_TRACE, "%8p: %3d %6d %8p %8p %8p" _
		event _ event->priority _ event->millis _ event->cbf _
		event->param _ event->request);
    }
}

PRIVATE void Event_traceHead (void)
{
    HTTRACE(ALL_TRACE, "     event: pri millis  callback   param    request  ");
}

PRIVATE void Timer_trace (HTTimer * timer)
{
    if (timer) {
	HTTRACE(ALL_TRACE, "%8p: %6d %ld %c %8p" _
		timer _
		HTTimer_expiresAbsolute(timer) _
		HTTimer_expiresRelative(timer) _
		HTTimer_isRelative(timer) ? 'R' : 'A' _
		HTTimer_callback(timer));
    }
}

PRIVATE void Timer_traceHead (void)
{
    HTTRACE(ALL_TRACE, "     timer: millis expires ?   param   callback  ");
}

/*
**  A simple debug function that dumps all the socket arrays
**  as trace messages
*/
PRIVATE void EventList_dump (void)
{
    int v = 0;
    HTList* cur;
    SockEvents * pres;
    HTTRACE(ALL_TRACE, "Event....... Dumping socket events\n");
    HTTRACE(ALL_TRACE, "soc ");
    Event_traceHead();
    HTTRACE(ALL_TRACE, " ");
    Timer_traceHead();
    HTTRACE(ALL_TRACE, "\n");
    for (v = 0; v < HT_M_HASH_SIZE; v++) {
	cur = HashTable[v];
	while ((pres = (SockEvents *) HTList_nextObject(cur))) {
	    int i;
	    HTTRACE(ALL_TRACE, "%3d \n" _ pres->s);
	    for (i = 0; i < HTEvent_TYPES; i++)
		if (pres->events[i]) {
		    static char * names[HTEvent_TYPES] = {"read", "writ", "xcpt"};
		    HTTRACE(ALL_TRACE, "%s " _ names[i]);
		    Event_trace(pres->events[i]);
		    HTTRACE(ALL_TRACE, " ");
		    Timer_trace(pres->timeouts[i]);
		    HTTRACE(ALL_TRACE, " ");
		}
	    HTTRACE(ALL_TRACE, "\n");
	}
    }
}

PRIVATE void fd_dump (SOCKET maxfs, fd_set * rset, fd_set * wset, fd_set * oset, 
		      struct timeval * wt)
{
    SOCKET cnt;

    /* Check read set */
    HTTRACE(THD_TRACE, "............ READ :");
    for (cnt=0; cnt<=maxfs; cnt++)
	if (FD_ISSET(cnt, rset)) HTTRACE(THD_TRACE, " %d" _ cnt);
    HTTRACE(THD_TRACE, "\n");

    /* Check write set */
    HTTRACE(THD_TRACE, "............ WRITE:");
    for (cnt=0; cnt<=maxfs; cnt++)
	if (FD_ISSET(cnt, wset)) HTTRACE(THD_TRACE, " %d" _ cnt);
    HTTRACE(THD_TRACE, "\n");

    /* Check oob set */
    HTTRACE(THD_TRACE, "............ OOB  :");
    for (cnt=0; cnt<=maxfs; cnt++)
	if (FD_ISSET(cnt, oset)) HTTRACE(THD_TRACE, " %d" _ cnt);
    HTTRACE(THD_TRACE, "\n");

    if (wt)
	HTTRACE(THD_TRACE, "............ Timeout is %ld s, %ld microsecs\n" _
		wt->tv_sec _ wt->tv_usec);
}
#endif /* HTDEBUG */

/* ------------------------------------------------------------------------- */
/*		           EVENT TIMING FUNCTIONS			     */
/* ------------------------------------------------------------------------- */

#ifdef WWW_WIN_ASYNC
PRIVATE BOOL Timer_setWindowsTimer (HTTimer * timer)
{
    UINT id;
    HWND hwnd = HTEventList_getWinHandle(&id);
    BOOL status = (SetTimer(hwnd, (UINT)timer, (UINT)HTTimer_getTime(timer), NULL) == 0) ? NO : YES;
    return status;
}

PRIVATE BOOL Timer_deleteWindowsTimer (HTTimer * timer)
{
    UINT id;
    HWND hwnd = HTEventList_getWinHandle(&id);
    BOOL status = (KillTimer(hwnd, (UINT)timer) == 0) ? NO : YES;
    return status;
}
#endif /* WWW_WIN_ASYNC */

/*
**  Event timeout handler
**  If an event didn't occur before the timeout then call it explicitly
**  indicating that it timed out.
*/
PRIVATE int EventListTimerHandler (HTTimer * timer, void * param, HTEventType type)
{
    SockEvents * sockp = (SockEvents *) param;
    HTEvent * event = NULL;

    /* Check for read timeout */
    if (sockp->timeouts[HTEvent_INDEX(HTEvent_READ)] == timer) {
	event = sockp->events[HTEvent_INDEX(HTEvent_READ)];
	HTTRACE(THD_TRACE, "Event....... READ timed out on %d.\n" _ sockp->s);
	return (*event->cbf) (sockp->s, event->param, HTEvent_TIMEOUT);
    }

    /* Check for write timeout */
    if (sockp->timeouts[HTEvent_INDEX(HTEvent_WRITE)] == timer) {
	event = sockp->events[HTEvent_INDEX(HTEvent_WRITE)];
	HTTRACE(THD_TRACE, "Event....... WRITE timed out on %d.\n" _ sockp->s);
	return (*event->cbf) (sockp->s, event->param, HTEvent_TIMEOUT);
    }

    /* Check for out-of-band data timeout */
    if (sockp->timeouts[HTEvent_INDEX(HTEvent_OOB)] == timer) {
	event = sockp->events[HTEvent_INDEX(HTEvent_OOB)];
	HTTRACE(THD_TRACE, "Event....... OOB timed out on %d.\n" _ sockp->s);
	return (*event->cbf) (sockp->s, event->param, HTEvent_TIMEOUT);
    }
    HTTRACE(THD_TRACE, "Event....... No event for timer %p with context %p\n" _ timer _ param);
    return HT_ERROR;
}

PUBLIC void CheckSockEvent (HTTimer * timer, HTTimerCallback * cbf, void * param)
{
    SockEvents * sockp = (SockEvents *)param;
    if (cbf == EventListTimerHandler && 
	sockp->timeouts[0] != timer && 
	sockp->timeouts[1] != timer && 
	sockp->timeouts[2] != timer) {
	HTDEBUGBREAK("Bad timer %p\n" _ timer);
    }
}

/* ------------------------------------------------------------------------- */
/*		             EVENT ORDERING STUFF			     */
/* ------------------------------------------------------------------------- */

PRIVATE SockEvents * SockEvents_get (SOCKET s, SockEvents_action action)
{
    long v = HASH(s);
    HTList* cur;
    SockEvents * pres;

    /* if the socket doesn't exists, don't do anything */
    if (s == INVSOC)
      return NULL;

    if (HashTable[v] == NULL) HashTable[v] = HTList_new();
    cur = HashTable[v];
    while ((pres = (SockEvents *) HTList_nextObject(cur)))
	if (pres->s == s) return pres;

    if (action == SockEvents_mayCreate) {
        if ((pres = (SockEvents *) HT_CALLOC(1, sizeof(SockEvents))) == NULL)
	    HT_OUTOFMEM("HTEventList_register");
	pres->s = s;
	HTList_addObject(HashTable[v], (void *)pres);
	return pres;
    }
    return NULL;
}

PRIVATE int EventOrder_add (SOCKET s, HTEventType type, ms_t now)
{
    EventOrder * pres;
    HTList * cur = EventOrderList;
    HTList * insertAfter = cur;
    SockEvents * sockp = SockEvents_get(s, SockEvents_find);
    HTEvent * event;

    if (sockp == NULL || (event = sockp->events[HTEvent_INDEX(type)]) == NULL) {
	HTTRACE(THD_TRACE, "EventOrder.. no event found for socket %d, type %s.\n" _
		s _ HTEvent_type2str(type));
	return HT_ERROR;
    }

    /*	Fixup the timeout */
    if (sockp->timeouts[HTEvent_INDEX(type)])
	HTTimer_refresh(sockp->timeouts[HTEvent_INDEX(type)], now);

    /* Look to see if it's already here from before */
    while ((pres = (EventOrder *) HTList_nextObject(cur))) {
	if (pres->s == s && pres->event == event && pres->type == type) {
	    pres->skipped++;
	    return HT_OK;
	}
	if (pres->event->priority+pres->skipped > event->priority)
	    insertAfter = cur;
    }

    /* Create a new element */
    if ((pres = (EventOrder *) HT_CALLOC(1, sizeof(EventOrder))) == NULL)
	HT_OUTOFMEM("EventOrder_add");
    pres->event = event;
    pres->s = s;
    pres->type = type;
    HTList_addObject(insertAfter, (void *)pres);
    return HT_OK;
}

PUBLIC int EventOrder_executeAndDelete (void) 
{
    HTList * cur = EventOrderList;
    EventOrder * pres;
    int i = 0;
    HTTRACE(THD_TRACE, "EventOrder.. execute ordered events\n");
    if (cur == NULL) return NO;
    while ((pres=(EventOrder *) HTList_removeLastObject(cur)) && i<EVENTS_TO_EXECUTE) {
	HTEvent * event = pres->event;
	int ret;
	HTTRACE(THD_TRACE, "EventList... calling socket %d, request %p handler %p type %s\n" _ 
		    pres->s _ (void *) event->request _ 
		    (void *) event->cbf _ HTEvent_type2str(pres->type));
	ret = (*pres->event->cbf)(pres->s, pres->event->param, pres->type);
	HT_FREE(pres);
	if (ret != HT_OK) return ret;
	i++;
    }
    return HT_OK;
}

PRIVATE BOOL EventOrder_clearAll (void)
{
    HTList * cur = EventOrderList;
    EventOrder * pres;
    HTTRACE(THD_TRACE, "EventOrder.. Clearing all ordered events\n");
    /* PATCH INFOVISTA */
    if (!cur) return NO;
    while ((pres = (EventOrder *) HTList_nextObject(cur))) {
      HTList_quickRemoveElement(cur, EventOrderList);
      HT_FREE (pres);
      cur = EventOrderList;
    }
    return YES;
    /* PATCH INFOVISTA */
}

PUBLIC BOOL EventOrder_deleteAll (void) 
{
    EventOrder_clearAll();
    HTList_delete(EventOrderList);
    EventOrderList = NULL;
    return YES;
}

/* ------------------------------------------------------------------------- */
/*				EVENT REGISTRATION			     */
/* ------------------------------------------------------------------------- */

/*
** ResetMaxSock - reset the value of the maximum socket in use 
*/
#ifndef WWW_WIN_ASYNC
PRIVATE void __ResetMaxSock (void)
{
    SOCKET cnt;
    SOCKET t_max = 0;
    SOCKET old_max = MaxSock;
    for (cnt = 0 ; cnt <= MaxSock; cnt++) { 
	if (FD_ISSET(cnt, (FdArray + HTEvent_INDEX(HTEvent_READ))) ||
	    FD_ISSET(cnt, (FdArray + HTEvent_INDEX(HTEvent_WRITE))) ||
	    FD_ISSET(cnt, (FdArray + HTEvent_INDEX(HTEvent_OOB))))
  	    if (cnt > t_max) t_max = cnt;
    }
    MaxSock = t_max+1;
    HTTRACE(THD_TRACE, "Event....... Reset MaxSock from %u to %u\n" _ old_max _ MaxSock);
    return;
}  
#endif /* !WWW_WIN_ASYNC */

PRIVATE int EventList_remaining (SockEvents * pres)
{
    int ret = 0;
    int i;
    for (i = 0; i < HTEvent_TYPES; i++)
	if (pres->events[i] != NULL)
	    ret |= 1<<i;
    return ret;
}

/*
**  For a given socket, reqister a request structure, a set of operations, 
**  a HTEventCallback function, and a priority. For this implementation, 
**  we allow only a single HTEventCallback function for all operations.
**  and the priority field is ignored.
*/
PUBLIC int HTEventList_register (SOCKET s, HTEventType type, HTEvent * event)
{
    int newset = 0;
    SockEvents * sockp;
    HTTRACE(THD_TRACE, "Event....... Register socket %d, request %p handler %p type %s at priority %d\n" _ 
		s _ (void *) event->request _ 
		(void *) event->cbf _ HTEvent_type2str(type) _ 
		(unsigned) event->priority);
    if (s==INVSOC || HTEvent_INDEX(type) >= HTEvent_TYPES)
	return 0;

    /*
    ** Insert socket into appropriate file descriptor set. We also make sure
    ** that it is registered in the global set.
    */
    HTTRACE(THD_TRACE, "Event....... Registering socket for %s\n" _ HTEvent_type2str(type));
    sockp = SockEvents_get(s, SockEvents_mayCreate);
    sockp->s = s;
    sockp->events[HTEvent_INDEX(type)] = event;
    newset = EventList_remaining(sockp);
#ifdef WWW_WIN_ASYNC
    if (WSAAsyncSelect(s, HTSocketWin, HTwinMsg, HTEvent_BITS(newset)) < 0) {
        HTTRACE(THD_TRACE, "Event....... WSAAsyncSelect returned `%s'!" _ HTErrnoString(socerrno));
	return HT_ERROR;
    }
#else /* WWW_WIN_ASYNC */
    FD_SET(s, FdArray+HTEvent_INDEX(type));

    HTTRACEDATA((char *) FdArray+HTEvent_INDEX(type), 8, "HTEventList_register: (s:%d)" _ s);

    if (s > MaxSock) {
	MaxSock = s ;
	HTTRACE(THD_TRACE, "Event....... New value for MaxSock is %d\n" _ MaxSock);
    }
#endif /* !WWW_WIN_ASYNC */

    /*
    **  If the timeout has been set (relative in millis) then we register 
    **  a new timeout for this event unless we already have a timer.
    */
    if (event->millis >= 0) {
	sockp->timeouts[HTEvent_INDEX(type)] =
	    HTTimer_new(sockp->timeouts[HTEvent_INDEX(type)],
			EventListTimerHandler, sockp, event->millis, YES, YES);
    }

    return HT_OK;
}

/* PATCH INFOVISTA */
PRIVATE void EventOrder_clean (SOCKET s, HTEventType type) 
{
    HTList * cur = EventOrderList;
    HTList * last = EventOrderList;
    EventOrder * pres;
    HTTRACE(THD_TRACE, "EventOrder.. Clearing ordered events of type %s for socket %d\n" _ HTEvent_type2str(type) _ s);
    /* Look to see if it's already here from before */
    while ((pres = (EventOrder *) HTList_nextObject(cur))) {
	if (pres->s == s && pres->type == type) {
	  HTList_quickRemoveElement(cur, last);
	  HT_FREE (pres);
	  cur = last;
	} else {
	  last  = (HTList *) cur;
	}
    }
}
/* PATCH INFOVISTA */

/*
** Remove the registered information for the specified socket for the actions 
** specified in ops. if no actions remain after the unregister, the registered
** info is deleted, and, if the socket has been registered for notification, 
** the HTEventCallback will be invoked.
*/
PUBLIC int HTEventList_unregister (SOCKET s, HTEventType type) 
{
    long 		v;
    HTList * 		cur;
    HTList * 		last;
    SockEvents *	pres;
    int			ret = HT_ERROR;

    /* PATCH INFOVISTA */
    if (s == INVSOC || HTEvent_INDEX (type) >= HTEvent_TYPES)
       return HT_ERROR;

    last = cur = HashTable[v = HASH(s)];

    EventOrder_clean (s, type);
    /* PATCH INFOVISTA */

    while (cur && (pres = (SockEvents *) HTList_nextObject(cur))) {
        if (pres->s == s) {
	    int	remaining = 0;

	    /*
	    **  Unregister the event from this action
	    */
	    pres->events[HTEvent_INDEX(type)] = NULL;
            remaining = EventList_remaining(pres);

	    /*
	    **  Check to see of there was a timeout connected with the event.
	    **  If so then delete the timeout as well.
	    */
	    {
		HTTimer * timer = pres->timeouts[HTEvent_INDEX(type)];
                if (timer) HTTimer_delete(timer);
                pres->timeouts[HTEvent_INDEX(type)] = NULL;
	    }
	    
#ifdef WWW_WIN_ASYNC
	    if (WSAAsyncSelect(s, HTSocketWin, HTwinMsg, remaining) < 0)
		ret = HT_ERROR;
#else /* WWW_WIN_ASYNC */
	    FD_CLR(s, FdArray+HTEvent_INDEX(type));

	    HTTRACEDATA((char*)FdArray+HTEvent_INDEX(type), 8, "HTEventList_unregister: (s:%d)" _ s);

#endif /* !WWW_WIN_ASYNC */

	    /*
	    **  Check to see if we can delete the action completely. We do this
	    **  if there are no more events registered.
	    */
	    if (remaining == 0) {
		HTList * doomed = cur;
		HTTRACE(THD_TRACE, "Event....... No more events registered for socket %d\n" _ s);

#ifndef WWW_WIN_ASYNC
		/* Check to see if we have to update MaxSock */
		if (pres->s >= MaxSock) __ResetMaxSock();
#endif /* !WWW_WIN_ASYNC */

		HT_FREE(pres);
		pres = (SockEvents *) HTList_nextObject(cur);
		HTList_quickRemoveElement(doomed, last);
	    }
	    ret = HT_OK;

      	    HTTRACE(THD_TRACE, "Event....... Socket %d unregistered for %s\n" _ s _ 
				   HTEvent_type2str(type));

	    /* We found the socket and can break */
	    break;
	}
	last = cur;
    }
    if (THD_TRACE) {
	if (ret == HT_ERROR)
	    HTTRACE(THD_TRACE, "Event....... Couldn't find socket %d. Can't unregister type %s\n" _
		    s _ HTEvent_type2str(type));
    }
    return ret;
}

/*
** Unregister all sockets 
** N.B. we just remove them for our internal data structures: it is up to the 
** application to actually close the socket. 
*/
PUBLIC int HTEventList_unregisterAll (void) 
{
    int i;
    HTTRACE(THD_TRACE, "Unregister.. all sockets\n");
    for (i = 0 ; i < HT_M_HASH_SIZE; i++) {
	HTList * cur = HashTable[i];
	SockEvents * pres;
	while ((pres = (SockEvents *) HTList_nextObject(cur))) {
#ifdef WWW_WIN_ASYNC
	    WSAAsyncSelect(pres->s, HTSocketWin, HTwinMsg, 0);
#endif /* WWW_WIN_ASYNC */
	    HT_FREE(pres);
	}
	HTList_delete(HashTable[i]);
	HashTable[i] = NULL;
    }

#ifndef WWW_WIN_ASYNC
    MaxSock = 0 ;
    HTTRACE(THD_TRACE, "Event....... New value for MaxSock is %d\n" _ MaxSock);
    FD_ZERO(FdArray+HTEvent_INDEX(HTEvent_READ));
    FD_ZERO(FdArray+HTEvent_INDEX(HTEvent_WRITE));
    FD_ZERO(FdArray+HTEvent_INDEX(HTEvent_OOB));
#endif /* !WWW_WIN_ASYNC */

    EventOrder_deleteAll();
    return 0;
}

/*
**  Dispatch the event to the appropriate event handler.
**  If no event handler is found then just return.
*/
PUBLIC int HTEventList_dispatch (SOCKET s, HTEventType type, ms_t now)
{
    SockEvents * sockp = SockEvents_get(s, SockEvents_find);
    if (sockp) {
	HTEvent * event = sockp->events[HTEvent_INDEX(type)];

	/* Fixup the timeout */
	if (sockp->timeouts[HTEvent_INDEX(type)])
	    HTTimer_refresh(sockp->timeouts[HTEvent_INDEX(type)], now);

	/*
	**  If we have found an event object for this event then see
	**  is we should call it.
	*/
	if (event && event->priority!=HT_PRIORITY_OFF)
	    return (*event->cbf) (s, event->param, type);
	HTTRACE(THD_TRACE, "Dispatch.... Handler %p NOT called\n" _ sockp);
	return HT_OK;
    }
    HTTRACE(THD_TRACE, "Dispatch.... Bad socket %d\n" _ s);
    return NO;
}

PUBLIC HTEvent * HTEventList_lookup (SOCKET s, HTEventType type)
{
    SockEvents * sockp = NULL;
    if ((sockp = SockEvents_get(s, SockEvents_find)) == NULL)
	return NULL;
    return sockp->events[HTEvent_INDEX(type)];
}

/* ------------------------------------------------------------------------- */
/*		     		THE EVENT LOOP 		 		     */
/* ------------------------------------------------------------------------- */

/*
**  Start eventloop
*/
PUBLIC int HTEventList_newLoop (void)
{
    return HTEventList_loop (NULL);
}

/*
**  Stops the event loop. The function does not guarantee
**  that all requests have terminated. This is for the app to do
*/
PUBLIC void HTEventList_stopLoop (void)
{
    HTEndLoop = 1;
}

/*
**  There are now two versions of the event loop. The first is if you want
**  to use async I/O on windows, and the other is if you want to use normal
**  Unix setup with sockets
*/
PUBLIC int HTEventList_loop (HTRequest * theRequest)
{
#ifdef WWW_WIN_ASYNC

    MSG msg;
    int status;
    while (!HTEndLoop && GetMessage(&msg,0,0,0)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
    }

    status = HTEndLoop;
    
    /* Reset HTEndLoop in case we want to start again */
    HTEndLoop = 0;
    
    return (status == 1 ? HT_OK : HT_ERROR);

#else /* WWW_WIN_ASYNC */

    fd_set treadset, twriteset, texceptset;
    struct timeval waittime, * wt;
    int active_sockets;
    int maxfds;
    ms_t timeout;
    ms_t now;
    SOCKET s;
    int status = HT_OK;

    /* Check that we don't have multiple loops started at once */
    if (HTInLoop) {
	HTTRACE(THD_TRACE, "Event Loop.. Already one loop running - exiting\n");
	return HT_ERROR;
    }
    HTInLoop = YES;

    /* Set up list of events - is kept around until EventOrder_deleteAll */
    if (!EventOrderList)
	EventOrderList = HTList_new();
    else
	EventOrder_clearAll();

    /* Don't leave this loop until we leave the application */
    while (!HTEndLoop) {

        /*
	**  Timeval struct copy needed for linux, as it set the value to the
	**  remaining timeout while exiting the select. (and perhaps for
	**  other OS). Code borrowed from X server.
	*/
	wt = NULL;
	if ((status = HTTimer_next(&timeout)))
	    break;
	if (timeout != 0) {
	    waittime.tv_sec = timeout / MILLI_PER_SECOND;
	    waittime.tv_usec = (timeout % MILLI_PER_SECOND) *
		(1000000 / MILLI_PER_SECOND);
	    wt = &waittime;
	}

	/*
	** Check whether we still have to continue the event loop. It could
	** be that one of the timer handlers ended the loop.
	*/
	if (HTEndLoop) break;

	/*
	**  Now we copy the current active file descriptors to pass them to select.
	*/
        treadset = FdArray[HTEvent_INDEX(HTEvent_READ)];
        twriteset = FdArray[HTEvent_INDEX(HTEvent_WRITE)];
        texceptset = FdArray[HTEvent_INDEX(HTEvent_OOB)];

	/* And also get the max socket value */
        maxfds = MaxSock; 

	HTTRACE(THD_TRACE, "Event Loop.. calling select: maxfds is %d\n" _ maxfds);
#ifdef HTDEBUG
	fd_dump(maxfds, &treadset, &twriteset, &texceptset, wt);
#endif

#ifdef __hpux 
        active_sockets = select(maxfds+1, (int *)&treadset, (int *)&twriteset,
				(int *)&texceptset, wt);
#elif defined(_WINSOCKAPI_)
	/*
	 * yovavm@contact.com
	 *
	 * On some WINSOCK versions select() with 3 empty sets and NULL timeout
	 * returns 0 and in some it returns -1.
	 * If 0 is returned in such situation, we will go into an infinite loop
	 * (cause the sets will stay empty forever ...),
	 * so make sure to set the active_sockets = -1 which will take us out 
	 * of the loop.
	 */
	if ((treadset.fd_count || twriteset.fd_count || texceptset.fd_count) 
	    && wt)
	     active_sockets = select(maxfds+1, &treadset, &twriteset,
				     &texceptset, wt);
	else
	     active_sockets = -1;	
#else
        active_sockets = select(maxfds+1, &treadset, &twriteset, &texceptset, wt);
#endif

	now = HTGetTimeInMillis();

	HTTRACE(THD_TRACE, "Event Loop.. select returns %d\n" _ active_sockets);
#ifdef HTDEBUG
	fd_dump(maxfds, &treadset, &twriteset, &texceptset, wt);
#endif

        if (active_sockets == -1) {
#ifdef EINTR
	    if (socerrno == EINTR) {
		/*
		** EINTR     The select() function was interrupted  before  any
		**           of  the  selected  events  occurred and before the
		**           timeout interval expired.
		**
		**           If SA_RESTART has been set  for  the  interrupting
		**           signal,  it  is  implementation-dependent  whether
		**	     select() restarts or returns with EINTR.
		*/
		HTTRACE(THD_TRACE, "Event Loop.. select was interruted - try again\n");
		continue;
	    }
#endif /* EINTR */
#ifdef EBADF
	    if (socerrno == EBADF) {
	        /*
		** EBADF     One or more of the file descriptor sets  specified
		**           a  file  descriptor  that is not a valid open file
		**           descriptor.
		*/
		HTTRACE(THD_TRACE, "Event Loop.. One or more sockets were not through their connect phase - try again\n");
		continue;
	    }
#endif
	    HTTRACE(THD_TRACE, "Event Loop.. select returned error %d\n" _ socerrno);

#ifdef HTDEBUG
	    EventList_dump();
#endif /* HTDEBUG */

	    status = HT_ERROR;
	    break;
        }

	/*
	**  We had a timeout so now we check and see if we have a timeout
	**  handler to call. Let HTTimer_next get it.
	*/ 
	if (active_sockets == 0)
	    continue;

	/* There were active sockets. Determine which fd sets they were in */
	for (s = 0 ; s <= maxfds ; s++) { 
	    if (FD_ISSET(s, &texceptset))
		if ((status = EventOrder_add(s, HTEvent_OOB, now)) != HT_OK)
		    goto stop_loop;
	    if (FD_ISSET(s, &twriteset))
		if ((status = EventOrder_add(s, HTEvent_WRITE, now)) != HT_OK)
		    goto stop_loop;
	    if (FD_ISSET(s, &treadset))
		if ((status = EventOrder_add(s, HTEvent_READ, now)) != HT_OK)
		    goto stop_loop;
	}
	if ((status = EventOrder_executeAndDelete()) != HT_OK) break;
    };

    /* Reset HTEndLoop in case we want to start again */
 stop_loop:
    HTEndLoop = 0;
    HTInLoop = NO;
    return status;
#endif /* !WWW_WIN_ASYNC */
}

/* ------------------------------------------------------------------------- */
/*		     EVENT INITIALIZATION AND TERMINATION		     */
/* ------------------------------------------------------------------------- */

#ifdef WWW_WIN_ASYNC

/* Only responsible for WM_TIMER and WSA_AsyncSelect */    	
PRIVATE LRESULT CALLBACK AsyncWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD event;
    SOCKET sock;
    HTEventType type;
    ms_t now = HTGetTimeInMillis();

    /* timeout stuff */
    if (uMsg == WM_TIMER) {
	HTTimer_dispatch((HTTimer *)wParam);
	return (0);
    }

    if (uMsg != HTwinMsg)	/* not our async message */
    	return (DefWindowProc(hwnd, uMsg, wParam, lParam));

    event = LOWORD(lParam);
    sock = (SOCKET)wParam;
    switch (event) {
    case FD_READ: type = HTEvent_READ; break;
    case FD_WRITE: type = HTEvent_WRITE; break;
    case FD_ACCEPT: type = HTEvent_ACCEPT; break;
    case FD_CONNECT: type = HTEvent_CONNECT; break;
    case FD_OOB: type = HTEvent_OOB; break;
    /* JK: was returning HTEvent_CLOSE before, and this was a source of
       errors, as libwww detects the socket shutdown with a call to recv  */  
    case FD_CLOSE: type = HTEvent_READ; break;
    default: HTDEBUGBREAK("Unknown event %d\n" _ event);
    }
    if (HTEventList_dispatch((int)sock, type, now) != HT_OK)
	HTEndLoop = -1;
    return (0);
}

/*	HTEventList_get/setWinHandle
**	--------------------------
**	Managing the windows handle on Windows
*/
PUBLIC BOOL HTEventList_setWinHandle (HWND window, unsigned long message)
{
    HTSocketWin = window;
    HTwinMsg = message;
    return YES;
}

PUBLIC HWND HTEventList_getWinHandle (unsigned long * pMessage)
{
    if (pMessage)
        *pMessage = HTwinMsg;
    return (HTSocketWin);
}
#endif /* WWW_WIN_ASYNC */

PUBLIC BOOL HTEventInit (void)
{
#ifdef WWW_WIN_ASYNC
    /*
    **	We are here starting a hidden window to take care of events from
    **  the async select() call in the async version of the event loop in
    **	the Internal event manager (HTEvtLst.c)
    */
    static char className[] = "AsyncWindowClass";
    WNDCLASS wc;
    OSVERSIONINFO osInfo;
    
    wc.style=0;
    wc.lpfnWndProc=(WNDPROC)AsyncWindowProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hIcon=0;
    wc.hCursor=0;
    wc.hbrBackground=0;
    wc.lpszMenuName=(LPSTR)0;
    wc.lpszClassName=className;

    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    GetVersionEx(&osInfo);

    /* According to Gary Johnson, GetModuleHandle() works for NT as well */
#if 0
    if (osInfo.dwPlatformId == VER_PLATFORM_WIN32s || osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	wc.hInstance=GetModuleHandle(NULL); /* 95 and non threaded platforms */
    else
        wc.hInstance=GetCurrentProcess(); /* NT and hopefully everything following */
#else
    wc.hInstance=GetModuleHandle(NULL); /* Should work on all win32 stuff */
#endif

    HTinstance = wc.hInstance;
    HTclass = RegisterClass(&wc);
    if (!HTclass) {
	HTTRACE(THD_TRACE, "HTLibInit.. Can't RegisterClass \"%s\"\n" _ className);
	return NO;
    }
    if (!(HTSocketWin = CreateWindow(className, "WWW_WIN_ASYNC", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 
                                     CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wc.hInstance,0))) {
#ifdef HTDEBUG
	char space[50];
       	HTTRACE(THD_TRACE, "HTLibInit.. Can't Create Window \"WWW_WIN_ASYNC\" - error:");
	sprintf(space, "%ld\n", GetLastError());
	HTTRACE(THD_TRACE, space);
#endif /* HTDEBUG */
    	return NO;
    }
    HTwinMsg = WM_USER;  /* use first available message since app uses none */

    /*  Register platform specific timer handlers for windows */
    HTTimer_registerSetTimerCallback(Timer_setWindowsTimer);
    HTTimer_registerDeleteTimerCallback(Timer_deleteWindowsTimer);

#endif /* WWW_WIN_ASYNC */

#ifdef _WINSOCKAPI_
    /*
    ** Initialise WinSock DLL. This must also be shut down! PMH
    */
    {
        WSADATA            wsadata;
	if (WSAStartup(DESIRED_WINSOCK_VERSION, &wsadata)) {
	    HTTRACE(THD_TRACE, "HTEventInit. Can't initialize WinSoc\n");
            WSACleanup();
            return NO;
        }
        if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION) {
            HTTRACE(THD_TRACE, "HTEventInit. Bad version of WinSoc\n");
            WSACleanup();
            return NO;
        }
	HTTRACE(APP_TRACE, "HTEventInit. Using WinSoc version \"%s\".\n" _ 
		    wsadata.szDescription);
    }
#endif /* _WINSOCKAPI_ */

    HTEvent_setRegisterCallback(HTEventList_register);
    HTEvent_setUnregisterCallback(HTEventList_unregister);
    return YES;
}

PUBLIC BOOL HTEventTerminate (void)
{
#ifdef _WINSOCKAPI_
    WSACleanup();
#endif /* _WINSOCKAPI_ */

#ifdef WWW_WIN_ASYNC
    DestroyWindow(HTSocketWin);
    UnregisterClass((LPCTSTR)HTclass, HTinstance);
#endif /* WWW_WIN_ASYNC */

    return YES;
}
