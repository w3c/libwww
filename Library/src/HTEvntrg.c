/*				    				     HTEvntrg.c
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

#include <assert.h>			/* @@@ Should be in sysdep.h @@@ */

/* Implementation dependent include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTReqMan.h"
#include "HTEvntrg.h"					 /* Implemented here */

#ifdef WWW_WIN_ASYNC
#define TIMEOUT	1 /* WM_TIMER id */
PRIVATE HWND HTSocketWin;
PRIVATE unsigned long HTwinMsg;
#endif

/* Type definitions and global variables etc. local to this module */
PRIVATE fd_set read_fds ;		   /* sockets registered for reading */
PRIVATE fd_set write_fds;				   /* for Writing... */
PRIVATE fd_set except_fds;			       /* for exceptions ... */
PRIVATE fd_set all_fds ;			    /* any descriptor at all */

PRIVATE int HTEndLoop = 0;		       /* If !0 then exit event loop */

PRIVATE const int SecondsToWait = 5 ;

PRIVATE void __ResetMaxSock( void ) ;
PRIVATE void __DumpFDSet( fd_set *, const char *);

typedef unsigned long DWORD;

/*
** our internal structure to hold a socket, it's request 
** and our action. For version 1, we allow one action per socket
*/

typedef struct rq_t RQ;

/*
** an action consists of a request, a set of requested operations 
** a HTEventCallback function, and a priority (priority is not used in this
** version)
*/

typedef struct action_t  { 
    void * param ;    				/* request structure */
    HTEventType type ;         			     /* requested operations */
    HTEventCallback *cbf;      		     /* callback function to execute */
    HTPriority p;         	     /* priority associated with this socket */
} ACTION ;

struct rq_t { 
    RQ * next ;	  
    SOCKET s ;		 	/* our socket */
    BOOL unregister;		/* notify app when completely unregistered */
    ACTION actions[1];
};

#define PRIME_TABLE_SIZE	67
#define MAX_ACTIONS 1 

#define HASH(s) ((s) % PRIME_TABLE_SIZE) 

/*
** our internal table of requests,
** indexed by socket 
*/
PRIVATE RQ * table[PRIME_TABLE_SIZE]; 
PRIVATE SOCKET max_sock = 0 ;		/* max socket value in use */
PRIVATE int socketsInUse = 0 ;		/* actual sockets in use */
PRIVATE int userSockets = 0;		/* Number of user sockets */
PRIVATE BOOL console_in_use = NO;

#ifndef WWW_MSWINDOWS 
typedef void * HANDLE ;
#endif

PRIVATE HANDLE console_handle = 0 ;

/* Select Timeout handling */
typedef struct _HTTimeout {
    HTEventTimeout *	tcbf;
    void *		param;
    struct timeval	tv;
    BOOL		always;
} HTTimeout;

PRIVATE HTTimeout 	seltime;
#ifndef WWW_WIN_ASYNC
PRIVATE struct timeval *tvptr = NULL;
#endif /* WWW_WIN_ASYNC */

/*
** this set of SockOps map our WinSock "socket event SockOps" into 
** our read and write sets. Note that under the canonical Unix model,
** a non-blocking socket passed to an accept() call will appear as readable, 
** whilst a non-blocking call to connect() will appear as writeable. In add.
** if the connection has been closed, the socket will appear readable under
** BSD Unix semantics 
*/
PRIVATE const SockOps ReadBits = FD_READ | FD_ACCEPT  | FD_CLOSE;
PRIVATE const SockOps WriteBits = FD_WRITE | FD_CONNECT ;
PRIVATE const SockOps ExceptBits = FD_OOB ;

/*
** Local functions 
*/
PRIVATE int __AddRequest( SOCKET, void *, SockOps, HTEventCallback *, HTPriority); 
PRIVATE void __RequestInit( RQ *, SOCKET, void *, SockOps, HTEventCallback *, HTPriority);
PRIVATE int __ProcessFds( fd_set *, SockOps, const char *);
PRIVATE void __RequestUpdate( RQ *, SOCKET, void *, SockOps, HTEventCallback *, HTPriority);
PRIVATE int __EventUnregister(RQ * , RQ **, SockOps );

/* ------------------------------------------------------------------------- */

#ifdef WWW_WIN_ASYNC
/*	HTEventrg_get/setWinHandle
**	--------------------------
**	Managing the windows handle on Windows
*/
PUBLIC BOOL HTEventrg_setWinHandle (HWND window, unsigned long message)
{
    HTSocketWin = window;
    HTwinMsg = message;
    return YES;
}

PUBLIC HWND HTEventrg_getWinHandle (unsigned long * pMessage)
{
    if (pMessage)
        *pMessage = HTwinMsg;
    return (HTSocketWin);
}
#else
#ifdef WWW_WIN_DLL
/*
**	By having these dummy definitions we can keep the same def file
*/
PUBLIC BOOL HTEventrg_setWinHandle (HWND window, unsigned long message)
{
    return YES;
}

PUBLIC HWND HTEventrg_getWinHandle (unsigned long * pMessage)
{
    return (HWND) 0;
}
#endif /* WWW_WIN_DLL */
#endif /* WWW_WIN_ASYNC */

/*	HTEventrg_registerTimeout 
**	-----------------------
**	Set the timeout for sockets. This does only works on NON windows
**	platforms as we need to poll for the console on windows
**	The default is no timeout. If the tp points to a zero'ed structure
**	then the select is basically polling. If always is YES then
**	the callback is called at all times, if NO then only when Library
**	sockets are active.
**	Returns YES if OK else NO
*/
PUBLIC BOOL HTEventrg_registerTimeout (struct timeval *tp, void * param,
				     HTEventTimeout *tcbf, BOOL always)
{
    if (tp) {
#ifdef WWW_WIN_ASYNC
	/* same window process WWW_WIN_ASYNC stuff and TIMEOUT */
	SetTimer(HTSocketWin, TIMEOUT, tp->tv_usec/1000 + tp->tv_sec*1000, 0);
#else
	tvptr = &seltime.tv;
	tvptr->tv_sec = tp->tv_sec;
	tvptr->tv_usec = tp->tv_usec;
#endif
	seltime.tcbf = tcbf;
	seltime.param = param;
	seltime.always = always;
	if (THD_TRACE)
	    HTTrace("Timeout cbf. %p %s (req=%p, sec=%d, usec=%d)\n",
		     tcbf, always ? "always" : "active",
		     param, (int) tp->tv_sec, (int) tp->tv_usec);
    }
    return YES;
}

/*	HTEventrg_unregisterTimeout 
**	-------------------------
**	Disables the callback function.
**	Returns YES if OK else NO
*/
PUBLIC BOOL HTEventrg_unregisterTimeout(void)
{
#ifdef WWW_WIN_ASYNC
    /* Same window process WWW_WIN_ASYNC stuff and TIMEOUT */
    KillTimer(HTSocketWin, TIMEOUT);
#else
    tvptr = NULL;
#endif
    return YES;
}

/*
** HTEventrg_registerTTY 
**
** register the tty (console) as having events.
** if the TTY is select()-able (as is true under Unix), then we treat
** it as just another socket. Otherwise, take steps depending on the platform
*/
PUBLIC int HTEventrg_registerTTY (SOCKET fd, void * param, HTEventType type, 
				  HTEventCallback *cbf, HTPriority p) 
{
    console_in_use = YES;
    console_handle = (HANDLE) fd;

    if (THD_TRACE) 
	HTTrace(
		"RegisterTTY. socket %d, request %p HTEventCallback %p SockOps %x at priority %d\n",
		fd, param,  (void *)cbf, (unsigned) ops, (unsigned) p);
	
#ifdef TTY_IS_SELECTABLE 

    /* HTEventrg_register adds the request _and_ inserts in fd table */
    userSockets++;
    return HTEventrg_register( fd, param, ops, cbf, p) ;

#else 

/* 
** Should be true for Windows3.x and NT and 95 - EGP changed _WINDOWS to
** WIN32
*/
#ifdef WIN32

    if (THD_TRACE) 
   	HTTrace("RegisterTTY. Windows, and no handle given\n");

    console_handle = GetStdHandle( STD_INPUT_HANDLE) ;
 
    return __AddRequest((SOCKET)console_handle, param, ops, cbf, p);
#else 
#ifdef WWW_MSWINDOWS /* EGP - added stub */
    return (0);
#else
#error "Don't know how to handle console tty for this system!"
#endif
#endif /* WIN32 */
#endif /* TTY_IS_SELECTABLE */
}

/*
** HTEventrg_unregisterTTY - unregisters TTY i/o channel
*/
PUBLIC int HTEventrg_unregisterTTY(SOCKET s, HTEventType type) 
{
    if (THD_TRACE)
	HTTrace("UnregisterTTY on channel %d\n", s) ;
    if (!console_in_use)
    	return 0;
    console_in_use = NO;

#ifdef WWW_WIN_CONSOLE
    s = (SOCKET)console_handle ;
#endif
#ifdef TTY_IS_SELECTABLE
    userSockets--;
#endif
    return HTEventrg_unregister(s, ops) ;	/* no harm in unregistering...*/
}


/*
**  HTEventrg_register
**  for a given socket, reqister a request structure, a set of operations, 
**  a HTEventCallback function, and a priority. For this implementation, 
**  we allow only a single HTEventCallback function for all operations.
**  and the priority field is ignored.
*/
PUBLIC int HTEventrg_register (SOCKET s, void * param, HTEventType type,
			     HTEventCallback *cbf, HTPriority p) 
{
    if (THD_TRACE) 
	HTTrace("Register.... socket %d, request %p HTEventCallback %p SockOps %x at priority %d\n",
		s, param,  (void *)cbf, (unsigned) ops, (unsigned) p) ;


    if (s==INVSOC) return 0;
    (void)__AddRequest( s, param, ops, cbf, p);
 
#ifdef WWW_WIN_ASYNC
#ifndef WIN32	/* EGP */
#define GetLastError WSAGetLastError
#endif
        if (WSAAsyncSelect( s, HTSocketWin, HTwinMsg, ops) < 0) {
	  /*	    HTRequest_addSystemError(rq, ERR_FATAL, GetLastError(), NO,
				     "WSAAsyncSelect"); */
	    return HTERROR;
	}
#endif
    /* insert socket into appropriate file descriptor set */

    if (ops & ReadBits)	{
	if (! FD_ISSET(s, &read_fds))
	    FD_SET(s, &read_fds) ;
	if (THD_TRACE)
	   HTTrace("Register.... Registering socket as readable\n");
    }

    if (ops & WriteBits) {
	if (! FD_ISSET(s, &write_fds))
	    FD_SET(s, &write_fds);
	if (THD_TRACE) 
	   HTTrace("Register.... Registering socket as writeable\n");
    }

    if (ops & ExceptBits) { 
	if (! FD_ISSET(s, &except_fds))
	    FD_SET(s, &except_fds);
	if (THD_TRACE) 
	    HTTrace("Register.... Registering socket for exceptions\n");
    }

    if (!FD_ISSET(s, &all_fds)) {
	socketsInUse++;
	FD_SET(s, &all_fds);
    }
    if (s > max_sock) 
        max_sock = s ;
    
    return 0 ;
}

/*
**  __AddRequst - add the given socket and its associated Request
**  when socket has activity, the registered HTEventCallback function will
**  invoked  
*/
PRIVATE int __AddRequest(SOCKET s, void * param, HTEventType type,
				 HTEventCallback *cbf, HTPriority p)
{
    RQ * rqp = 0 , **rqpp = 0 ;
    long v = HASH(s);
    BOOL found = NO;
    for (rqpp = &table[v]; (rqp = *rqpp) != 0 ; rqpp = &rqp->next) {
        if (rqp->s == s) {
            __RequestUpdate( rqp, s, param, ops, cbf, p) ;
            found = YES;
            break;
        }
    }  
    
    if (THD_TRACE)
    	HTTrace("AddRequest.. %s socket %d\n", found ? "found" : "Did NOT find" , s) ;          

    if (!found) { 
        /* error if memory not allocated */
        if ((*rqpp = rqp = (RQ *) HT_CALLOC(1, sizeof(RQ))) == NULL)
	    HT_OUTOFMEM("__AddRequest");
        __RequestInit( rqp, s, param, ops, cbf, p) ;
    }
    return 0;
}


/*
** RequestInit - 
** initialize the request structure given 
** a set of inputs. N.B. This initializes the entire registration structure 
*/
PRIVATE void __RequestInit(RQ * rqp, SOCKET s, void * param, 
			   HTEventType type, HTEventCallback *cbf, HTPriority p) 
{
    if( THD_TRACE)
    	HTTrace("RequestInit. initializing RQ entry for socket %d\n", s);
    rqp->s = s; 
    __RequestUpdate( rqp, s, param, ops, cbf, p) ;
    rqp->next = 0 ;
    return ;
}  

/*
** RequestUpdate 
**
** updates the actions fields and the unregister value, but doesn't modify 
** the socket _or_ the next pointer
*/
PRIVATE void __RequestUpdate( RQ * rqp, SOCKET s, void * param,
			     HTEventType type, HTEventCallback * cbf, HTPriority p)
{
    if (THD_TRACE) 
    	HTTrace("Req Update.. updating for socket %u\n", s) ;
    rqp->unregister = (ops & FD_UNREGISTER) ? YES : NO;
    rqp->actions[0].param = param ;
    rqp->actions[0].ops |= ops ;
    rqp->actions[0].cbf = cbf ;
    rqp->actions[0].p = p ;
    return;
}

/*
** HTEventrg_unregister
** remove the registered information for the specified socket for the actions 
** specified in ops. if no actions remain after the unregister, the registered
** info is deleted, and, if the socket has been registered for notification, 
** the HTEventCallback will be invoked.
*/
PUBLIC int HTEventrg_unregister( SOCKET s, HTEventType type) 
{
    long v = HASH(s) ;
    int rv = 0 ;
    RQ * rqp = 0, **rqpp = 0;
    int found = 0;

    for (rqpp = &table[v]; (rqp = *rqpp) != 0 ; rqpp = &rqp->next) {
        if (rqp -> s == s ) { 
            found = 1 ;
            break;
        }
    }

    if (THD_TRACE)
    	HTTrace("UnRegister.. %s entry for socket %d with ops %x\n",
		 (found) ? "Found" : "Didn't find", s, (unsigned) ops);
    if (! found) 
        return 0;

    rv = __EventUnregister( rqp, rqpp, ops) ;
    return rv ;
}

/*
** __RetrieveCBF
** given a socket, return the HTEventCallback function registered for it
** and return the HTRequest pointer associated with it.
** If the socket isn't found, the function returns NULL
*/
PRIVATE HTEventCallback *__RetrieveCBF(SOCKET s, HTEventType type, void ** pParam)
{
    long value = HASH(s);
    register RQ * rqp = 0, **rqpp = 0 ;
    BOOL found = NO;

    *pParam = 0 ;	  /* just too be sure */

    for (rqpp = &table[value]; (rqp = *rqpp) != 0 ; rqpp = &rqp->next) {
        if (rqp -> s == s ) { 
            found = YES;
            break;
        }
    }

    if (THD_TRACE) 
    	HTTrace("Retrieve.... %s socket %d\n", found ? "Found" : "Didn't find", s) ;

    if (!found) 
        return (HTEventCallback *) NULL;
    else {
    	*pParam = rqp->actions[0].param;
        return rqp->actions[0].cbf;
    }      
}

/*
** HTEventrg_unregisterAll
** unregister all sockets 
** N.B. we just remove them for our internal data structures: it is up to the 
** application to actually close the socket. 
*/
PUBLIC int HTEventrg_unregisterAll( void ) 
{
    int i ;
    register RQ * rqp, **rqpp ;

    /* begin */
    if (THD_TRACE)
	HTTrace("Unregister.. all sockets\n");

    for (i = 0 ; i < PRIME_TABLE_SIZE; i++) {
	if (table[i] != 0) { 
            for (rqpp = &table[i]; (rqp = *rqpp) != 0 ; ) {
		(void)__EventUnregister(rqp, rqpp, FD_ALL) ;
            }        /* for each element on list */
	    table[i] = 0 ;
	} /* if table != 0 */
    }     /* for each element in table */

    max_sock = 0 ;

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds) ;
    FD_ZERO(&all_fds);

    return 0;
}


/*	HTEventrg_stopLoop
**	----------------
**	Stops the (select based) event loop. The function does not guarantee
**	that all requests have terminated. This is for the app to do
*/
PUBLIC void HTEventrg_stopLoop (void)
{
    HTEndLoop = 1;
}

/*  HTEventrg_loop
**  ------------
**  event loop: that is, we wait for activity from one of our registered 
**  channels, and dispatch on that.
**
**  There are now two versions of the event loop. The first is if you want
**  to use async I/O on windows, and the other is if you want to use normal
**  Unix setup with sockets
*/
#ifdef WWW_WIN_ASYNC
/* only responsible for WM_TIMER and WSA_AsyncSelect */    	
PUBLIC LRESULT CALLBACK AsyncWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD event;
    SOCKET sock;

    /* timeout stuff */
    if (uMsg == WM_TIMER) {
	if (seltime.tcbf && (seltime.always || HTNet_isIdle())) {
	    if (THD_TRACE)
		HTTrace("Event Loop.. calling timeout cbf\n");
	    (*(seltime.tcbf))(seltime.request);
	}
	return (0);
    }

    if (uMsg != HTwinMsg)	/* not our async message */
    	return (DefWindowProc(hwnd, uMsg, wParam, lParam));

    event = LOWORD(lParam);
    sock = (SOCKET)wParam;
    if (event & (FD_READ | FD_ACCEPT | FD_CLOSE))
    	if (HTEventrg_dispatch((int)sock, FD_READ) != HT_OK) {
	    HTEndLoop = -1;
	    return 0;
	}
    if (event & (FD_WRITE | FD_CONNECT))
    	if (HTEventrg_dispatch((int)sock, FD_WRITE) != HT_OK) {
	    HTEndLoop = -1;
	    return 0;
	}
    if (event & FD_OOB)
    	if (HTEventrg_dispatch((int)sock, FD_OOB) != HT_OK) {
	    HTEndLoop = -1;
	    return 0;
	}
    return (0);
}

PUBLIC int HTEventrg_loop (HTRequest * theRequest )
{
    MSG msg;
#ifdef WWW_WIN_CONSOLE
    HTEndLoop = 0;
    while (!HTEndLoop) {
    	DWORD toRead;
    	if (console_in_use) {
            int status;
    	    /* Check keystrokes */
    	    GetNumberOfConsoleInputEvents(console_handle, &toRead);
    	    if (toRead) {
    		if (THD_TRACE) 
		    HTTrace("Event Loop.. console ready, invoke callback\n");
		status = HTEventrg_dispatch((SOCKET) console_handle, FD_READ);
		if (status != HT_OK)
		    return status;
	    }
	}
    	if (!PeekMessage(&msg, HTSocketWin, 0, 0, PM_REMOVE)) { /* PeekMessage won't work with a NULL hWnd */
	    if (msg.message == WM_QUIT) {
    		/* got a WM_QUIT from system */
		DestroyWindow(HTSocketWin);
		return HT_ERROR;
	    }
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}
    }
#else
    while (GetMessage(&msg,0,0,0)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
    }
#endif
    return (HTEndLoop == 1 ? HT_OK : HT_ERROR);
}

#else /* WWW_WIN_ASYNC  - Unix HTEventrg_loop */

#ifdef WWW_WIN_DLL
PUBLIC LRESULT CALLBACK AsyncWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
					LPARAM lParam)
{
    return (LRESULT) 0;
}
#endif

PUBLIC int HTEventrg_loop (HTRequest * theRequest ) 
{
    fd_set treadset, twriteset, texceptset ;    
    int active_sockets;
    int maxfds ;
    int readings, writings, exceptions ;
    SOCKET s ;
    BOOL consoleReady = NO;
    int status = 0 ;
#ifdef _WINSOCKAPI_
    unsigned ui ;
#endif
    struct timeval tvsave;


    HTEndLoop = 0;
    /* Don't leave this loop until we the application */
    do { 
        treadset = read_fds;
        twriteset = write_fds ;
        texceptset = except_fds ;  
        maxfds = max_sock ; 
        readings = writings = exceptions = 0; 
	consoleReady = NO;

	if (console_in_use) { 

#ifdef WWW_WIN_CONSOLE
            int result;
 	    DWORD time2wait;
	    tvptr = &seltime.tv;
	    tvptr->tv_sec = SecondsToWait;
	    tvptr->tv_usec = 0;
	    if (max_sock == 0)     /* no sockets, so wait for keyboard input */
		time2wait = INFINITE;
 	    else 
		time2wait = 1000;	      /* this is a poll - one second */

	    if (THD_TRACE)
		HTTrace("Event Loop. console in use: waiting %s\n",
			(time2wait == 1000) ? "1 second" : "forever" );

	    switch( result = WaitForSingleObject( console_handle, time2wait)) {
		case WAIT_TIMEOUT:
			break;

		case WAIT_FAILED :
		case WAIT_ABANDONED: 		      /* should never happen */
		    HTRequest_addSystemError(theRequest, ERR_FATAL, GetLastError(), NO,
				  "WaitForSingleObject");
		    return HT_ERROR;
		    break ; 

		case WAIT_OBJECT_0:
		    consoleReady = YES;
		    tvptr->tv_sec = 0 ;		    /* just poll the sockets */
		    break;
	     } /* switch */
	     if (THD_TRACE)
		HTTrace("Console..... %s ready for input\n",
			consoleReady ? "is" : "ISN'T" );
#endif /* WWW_WIN_CONSOLE */

	} /* if tty in use */
	 
	/* if the console isn't ready, then this will block for 5 seconds;
	   otherwise, it's a poll
	 */

	if (THD_TRACE)
	    HTTrace("Event Loop.. calling select: maxfds is %d\n",
		    maxfds);
/*
 *  timeval struct copy needed for linux, as it set the value to the remaining
 *  timeout while exiting the select. (and perhaps for other OS).
 */
	tvsave.tv_sec = tvptr->tv_sec;
	tvsave.tv_usec = tvptr->tv_usec;

#ifdef __hpux 
        active_sockets = select(maxfds+1, (int *)&treadset, (int *)&twriteset,
				(int *)&texceptset, (struct timeval *) &tvsave);
#else
        active_sockets = select(maxfds+1, &treadset, &twriteset, &texceptset,
				(struct timeval *) &tvsave);
#endif
	if (THD_TRACE)
	    HTTrace("Event Loop.. select returns %d\n",active_sockets);

        switch(active_sockets)  {
            case 0:         /* no activity - timeout - allowed */
		break;
            
            case -1:        /* error has occurred */
	    	HTRequest_addSystemError(theRequest, ERR_FATAL, socerrno, NO, "select");
		__DumpFDSet( &treadset, "Read");
		__DumpFDSet( &twriteset, "Write") ;
		__DumpFDSet( &texceptset, "Exceptions");
                return HT_ERROR;
                break;
            
            default:
                break;
        } /* switch */

#ifndef WWW_WIN_CONSOLE 
	if (console_in_use) {
	    consoleReady = active_sockets && 
		FD_ISSET((SOCKET) (console_handle), &treadset);
        }
#endif
	
	if (console_in_use && consoleReady) {
	    if (THD_TRACE) 
	    	HTTrace("Event Loop.. console ready, do callback\n");
	    status = HTEventrg_dispatch((SOCKET) console_handle, FD_READ);
	    if (status != HT_OK)
		return status;

#ifndef WWW_WIN_CONSOLE
	    /* Remember to take out the console handle from the read set */
	    FD_CLR((SOCKET) (console_handle), &treadset);
#endif
	    
	} /* if console in use and it's ready */

	/* If timeout then see if we should call callback function */
	if (active_sockets == 0) {
	    if (seltime.tcbf && (seltime.always || HTNet_isIdle())) {
#if 0
		/* This drives you crazy! */
		if (THD_TRACE) HTTrace("Event Loop.. select timeout\n");
#endif
		if ((status = (*(seltime.tcbf))(seltime.param)) != HT_OK)
		    return status;
	    } else
		continue;
	}

	/*
	 * there were active sockets. Determine which fd sets they were in
	 */

#ifdef _WINSOCKAPI_ 
	for (ui = 0 ; ui < all_fds.fd_count; ui++) { 
            s = all_fds.fd_array[ui]; 
#else 
	for (s = 0 ; s <= maxfds ; s++) { 
#endif
	    { 
	        if (FD_ISSET(s, &texceptset))
		    exceptions++;
		if (FD_ISSET(s, &treadset))
		    readings++;
		if (FD_ISSET(s, &twriteset))
		    writings++;
	    }
	} /* for */

        if (exceptions) {
            if ((status= __ProcessFds(&texceptset, FD_OOB, "Except")) != HT_OK)
	    	return status;
        }

        if (readings) {
            if ((status = __ProcessFds(&treadset, FD_READ, "Read")) != HT_OK)
	    	return status;
        }

        if (writings) { 
            if ((status= __ProcessFds(&twriteset, FD_WRITE, "Write")) != HT_OK)
	    	return status;
        }

    } while (!HTEndLoop);
    return HT_OK;
}

/*
** ProcessFds 
** preform the associated HTEventCallback function for each FD in a given set  
*/
PRIVATE int __ProcessFds( fd_set * fdsp, HTEventType type, const char * str) 
{
    SOCKET s ;
#ifdef _WINSOCKAPI_
    unsigned ui ;
#endif
    if (THD_TRACE)
	HTTrace("Processing.. %s socket set. max_sock is %d\n",
		str, max_sock);
    
#ifdef _WINSOCKAPI_ 
    for (ui = 0 ; ui < fdsp->fd_count; ui++) {
	s = fdsp->fd_array[ui] ;
#else
    for (s = 0 ; s <= max_sock; s++) {
        if (FD_ISSET( s, fdsp)) 
#endif 
	return HTEventrg_dispatch( s, ops);
    }
    return HT_OK;
}
#endif /* else WWW_WIN_ASYNC */

/*
** HTEventrg_dispatch( SOCKET, SockOps ) 
**
** a  little function that just invokes the HTEventCallback routine associated
** with the given socket. 
**
*/
PUBLIC int HTEventrg_dispatch (SOCKET s, HTEventType type)
{
    void * param = NULL;
    HTEventCallback *cbf = __RetrieveCBF(s, ops, &param);
    /* although it makes no sense, callbacks can be null */
    /* was if (!cbf || (rqp && rqp->priority == HT_PRIORITY_OFF)) - EGP */
    if (!cbf) {
	if (THD_TRACE) HTTrace("Callback.... No callback found\n");
        return (0);
    }
    return (*cbf)(s, param, ops);
}

/*
** ResetMaxSock - reset the value of the maximum socket in use 
** 
*/
PRIVATE void __ResetMaxSock( void )
{
    SOCKET s ;
    SOCKET t_max = 0;
    SOCKET old_max = max_sock;
    
#ifdef _WINSOCKAPI_
    unsigned ui ;
#endif

#ifdef _WINSOCKAPI_ 
    for (ui = 0 ; ui < all_fds.fd_count; ui++) { 
        s = all_fds.fd_array[ui] ;
#else 
    for (s = 0 ; s <= max_sock; s++) { 
        if (FD_ISSET(s, &all_fds))
#endif
        {
  	    if (s > t_max)
 	        t_max = s ;
	} /* scope */
    } /* for */

    max_sock = t_max ;
    if (THD_TRACE)
	HTTrace("Sockets..... Reset max socket from %u to %u\n",
		old_max, max_sock);
    return;
}  

PRIVATE int __EventUnregister(register RQ *rqp, register RQ ** rqpp,
			      HTEventType type) 
{
    register struct action_t * ap = &rqp->actions[0];
    SOCKET s = rqp->s ;
    int rv ;

    ap->ops &= ~ops ;      /* get rid of 'em */

    if (THD_TRACE)
    	HTTrace("Unregister.. operations set for socket is %x\n",
		(unsigned) ap->ops);

    /* do we need to delete the socket from it's set as well? */

    if ((ap->ops & ReadBits) == 0)    /* nothing for reading */
        FD_CLR( s, &read_fds);
    if ((ap->ops & WriteBits) == 0)    /* nothing for writing */
        FD_CLR( s, &write_fds);
    if ((ap->ops & ExceptBits) == 0)   /* nothing for 'rithmetic */
        FD_CLR(s, &except_fds);


    /* if all actions are clear we are free to delete our request structure */
    if (ap->ops == 0)  {
        if (rqp->unregister) /* requested HTEventCallback */
            rv = ap->cbf( s, ap->param, FD_UNREGISTER); 
	FD_CLR(rqp->s, &all_fds) ;
	if (rqp->s == max_sock) 
	    	__ResetMaxSock();

#ifdef WWW_WIN_ASYNC
	WSAAsyncSelect(rqp->s, HTSocketWin, HTwinMsg, 0);
#endif /* WWW_WIN_ASYNC */
        *rqpp = rqp->next ;

        /* JK: Made a slight correction so socketsInUse will be correctly
        ** decremented. This "bug" had no effect whatsoever on libwww,
        ** but it's useful for debugging
        */

        /* NB we only count network sockets, NOT the console/display socket */
        if (console_in_use) {
                if(rqp->s != (SOCKET)console_handle)
                        if (--socketsInUse <= 0)  /* why this comparition? */
                                socketsInUse = 0;
        } else
                socketsInUse--;
 
        HT_FREE(rqp);
    }  /* if all unregistered */
    else { 
#ifdef WWW_WIN_ASYNC
      WSAAsyncSelect(rqp->s, HTSocketWin, HTwinMsg, ap->ops);
#endif /* WWW_WIN_ASYNC */
    }
    return 0 ;
}

PRIVATE void __DumpFDSet( fd_set * fdp, const char * str) 
{
    SOCKET s ;
#ifdef _WINSOCKAPI_
    unsigned ui ;
#endif
    if (THD_TRACE) {
	HTTrace("Dumping..... %s file descriptor set\n", str );
#ifdef _WINSOCKAPI_ 
        for (ui = 0 ; ui < fdp->fd_count; ui++) { 
            s = all_fds.fd_array[ui] ;
#else 
        for (s = 0 ; s <= max_sock; s++) { 
            if (FD_ISSET(s, fdp))
#endif
	    {
	        HTTrace("%4d\n", s);
	    }
        }	/* for */
    }           /* if */
    return ;
}

/*	REGISTER DEFULT EVENT MANAGER
**	-----------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC BOOL HTEventInit (void)
{
#ifdef WWW_WIN_ASYNC
    /*
    **	We are here starting a hidden window to take care of events from
    **  the async select() call in the async version of the event loop in
    **	the Internal event manager (HTEvntrg.c)
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
    if (osInfo.dwPlatformId == VER_PLATFORM_WIN32s || osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	wc.hInstance=GetModuleHandle(NULL); /* 95 and non threaded platforms */
    else
	wc.hInstance=GetCurrentProcess(); /* NT and hopefully everything following */
    if (!RegisterClass(&wc)) {
    	HTTrace("HTLibInit.. Can't RegisterClass \"%s\"\n", className);
	    return NO;
    }
    if (!(HTSocketWin = CreateWindow(className, "WWW_WIN_ASYNC", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 
                                     CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wc.hInstance,0))) {
	char space[50];
       	HTTrace("HTLibInit.. Can't CreateWindow \"WWW_WIN_ASYNC\" - error:");
	sprintf(space, "%ld\n", GetLastError());
	HTTrace(space);
    	return NO;
    }
    HTwinMsg = WM_USER;  /* use first available message since app uses none */
#endif /* WWW_WIN_ASYNC */

#ifdef _WINSOCKAPI_
    /*
    ** Initialise WinSock DLL. This must also be shut down! PMH
    */
    {
        WSADATA            wsadata;
	if (WSAStartup(DESIRED_WINSOCK_VERSION, &wsadata)) {
	    if (WWWTRACE)
		HTTrace("HTEventInit. Can't initialize WinSoc\n");
            WSACleanup();
            return NO;
        }
        if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION) {
            if (WWWTRACE)
		HTTrace("HTEventInit. Bad version of WinSoc\n");
            WSACleanup();
            return NO;
        }
	if (APP_TRACE)
	    HTTrace("HTEventInit. Using WinSoc version \"%s\".\n", 
		    wsadata.szDescription);
    }
#endif /* _WINSOCKAPI_ */

    HTEvent_setRegisterCallback(HTEventrg_register);
    HTEvent_setUnregisterCallback(HTEventrg_unregister);
    return YES;
}

PUBLIC BOOL HTEventTerminate (void)
{
#ifdef _WINSOCKAPI_
    WSACleanup();
#endif

#ifdef WWW_WIN_ASYNC
    DestroyWindow(HTSocketWin);
#endif
    return YES;
}

