/*								     HTThread.c
**	MULTIPLE THREAD SOCKET MANAGEMENT
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
#include "HTAccess.h"
#include "HTProt.h"
#include "HTError.h"
#include "HTThread.h"					 /* Implemented here */
#include "HTEvntrg.h"

#ifdef WIN32
#include <io.h>
#endif


PRIVATE HTList *HTThreads = NULL;      	 /* List of the HTNetInfo structures */
PRIVATE fd_set nullSet ;
PRIVATE fd_set HTfd_intr;
PRIVATE fd_set HTfd_libs ;
PRIVATE SOCKET libMaxSock = 0 ;


int LibraryCallback( SOCKET, HTRequest *, SockOps) ;

/* ------------------------------------------------------------------------- */

/*								   HTThreadInit
**
**	Initiates the thread socket registers. It is very important that
**	this function is called. It is currently done inside HTAccess in the
**	HTAccessInit function.
*/
PUBLIC BOOL HTThreadInit NOARGS
{
    static BOOL done=NO;

    if (done) {
	if (THD_TRACE)
	    fprintf(TDEST, "ThreadInit.. Already done\n");
	return NO;
    }
    done = YES;
    FD_ZERO( &HTfd_intr) ;
    return YES;
}


/*							      HTThreadGetFDInfo
**
**	Returns the maximum bit width and the read and write bit array.
** 	N.B. Only used by old HTEvent.c module
*/

PUBLIC int HTThreadGetFDInfo ARGS2(fd_set *, read, fd_set *, write)
{
    *read = nullSet;
    *write = nullSet;
    return libMaxSock;
}

/*							 HTThreadStateByRequest
**
**	This function registers a socket as waiting for the action given
**	(read or write etc.). It uses the request structure to find the socket
*/
PUBLIC void HTThreadStateByRequest ARGS2(HTRequest *, request,
					 HTThreadAction, action)
{
    if (request && request->net_info && request->net_info->sockfd != INVSOC)
	HTThreadState(request->net_info->sockfd, action);
}



/*								  HTThreadState
**
**	This function registers a socket as waiting for the action given
**	(read or write etc.).
**
**  	Charlie Brooks - we handle the interrupt thread state internally to this module 
**      setting the interrupt on a socket disables it from read/write.
*/
PUBLIC void HTThreadState ARGS2(SOCKFD, sockfd, HTThreadAction, action)
{
    register HTNetInfo * pres ;
    HTList * cur = HTThreads ;
    int found = 0 ;
    HTRequest * reqst ;

#ifdef _WIN32
	if (sockfd <= 2) 
		sockfd = _get_osfhandle(sockfd) ;
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
		"Thread...... Registering socket number %d for action %s\n",
		sockfd, *(actionmsg+action));
    }	 /* if */

    FD_SET( sockfd, &HTfd_libs) ;
    if (libMaxSock < sockfd)
    	libMaxSock = sockfd ;


    while ((pres = (HTNetInfo *)HTList_nextObject(cur) ) != 0) { 
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
	while ((pres = (HTNetInfo *)HTList_nextObject(cur) ) != 0) { 
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
	    fprintf(TDEST, "Thread...... Illegal socket action (%d)\n", (int)action);
    }
    return ;
}


/*								   HTThreadIntr
**
**	This function returns YES or NO to the question
*/
PUBLIC BOOL HTThreadIntr ARGS1(SOCKFD, sockfd)
{
    return FD_ISSET(sockfd, &HTfd_intr) ? YES : NO;
}


/*							    HTThreadMarkIntrAll
**
**	Marks all Library sockets as interrupted. User sockets can not be
**	interrupted
*/
PUBLIC BOOL HTThreadMarkIntrAll( CONST fd_set * fd_user )
{
    HTNetInfo *pres;
    if (HTThreads) {
	while ((pres = (HTNetInfo *) HTList_lastObject(HTThreads)) != NULL)
	    HTThread_kill(pres);
	return YES;
    }
    return NO;

}


/*								 HTThreadActive
**
**	Returns yes as long as a socket other than stdin is registered in 
**	the total set of sockets.
*/
PUBLIC BOOL HTThreadActive NOARGS
{
    if (libMaxSock > 0)
       return YES; 
    else 
       return NO;
}


/*							     	   HTThread_new
**
**	Register the HTNetInfo structure in a list so that we can find the 
**	request which corresponds to a socket descriptor
*/
PUBLIC BOOL HTThread_new ARGS1(HTNetInfo *, new_net)
{
    if (!HTThreads)
		HTThreads = HTList_new();
    return HTList_addObject(HTThreads, (void *) new_net);
}


/*							         HTThread_clear
**
**	Remove the HTNetInfo from the list of acrive threads.
*/
PUBLIC BOOL HTThread_clear ARGS1(HTNetInfo *, old_net)
{
    if (HTThreads)
	return HTList_removeObject(HTThreads, (void *) old_net);
    return NO;
}

/*							      HTThread_isAlive
**
**	Checks whether a thread is still registered and if so returns the 
**	corresponding HTRequest structure, else return NULL.
*/
PUBLIC HTRequest *HTThread_isAlive ARGS1(HTNetInfo *, net)
{
    HTList *cur = HTThreads;
    HTNetInfo *pres;
    if (cur && net) {
	while ((pres = (HTNetInfo *) HTList_nextObject(cur)) != NULL) {
	    if (pres == net) return pres->request;
	}
    }
    if (THD_TRACE)
	fprintf(TDEST, "Thread...... Thread is not alive\n");
    return NULL;
}


/*
 *   LibraryCallback - "glue" between 3.0 thread code and new callback functions
 *   map return codes into a simple yes/no model. 
 */

int LibraryCallback( SOCKET s, HTRequest * rq, SockOps f)
{
    int status = 0 ;
    HTEventState state ;
    HTProtocol * proto = (HTProtocol *)
            HTAnchor_protocol( rq -> anchor) ;

    /* begin */    

    if (proto == 0)   	/* Whoa! No protocol! */
    	return -1;
    status = proto->load( rq ) ;
    if (status != HT_WOULD_BLOCK) {   /* completed - good or bad... */
        if (THD_TRACE) 
            fprintf(TDEST, "LibCallBack. Calling Terminate...\n");

        HTLoadTerminate( rq, status ) ;
        state = HTEventRequestTerminate( rq, status) ;
	/* if the state isn't EVENT_QUIT */
        if (! HTEventCheckState( rq, state ))
		return HT_OK;  /* treat as failure */
    }  /* if status */
    return HT_WOULD_BLOCK;
}

/*							         HTThread_kill
**
**	Kill the thread and remove the HTNetInfo from the list of active
**	threads.
**	Returns YES on success, NO on error
**
**	BUG: We do not take ANSI C file descriptors into account
*/

PUBLIC BOOL HTThread_kill ARGS1(HTNetInfo *, kill_net)
{
    if (HTThreads) {
	HTList *cur = HTThreads;
	HTNetInfo *pres;

	/* Find the corresponding HTRequest structure */
	while ((pres = (HTNetInfo *) HTList_nextObject(cur)) != NULL) {
	    if (pres == kill_net) break;
	}

	/*
	**  Now see if a socket is active (or an ANSI C file descriptor).
	**  If so then mark the thread as interrupted and call the load
	**  function.
	*/
	if (pres) {
	    if (kill_net->sockfd != INVSOC) {	  /* @@@ ANSI C FIlE DES @@@ */
		HTProtocol *prot = (HTProtocol *)
		    HTAnchor_protocol(kill_net->request->anchor);
		HTThreadState(kill_net->sockfd, THD_SET_INTR);
		(*(prot->load))(kill_net->request);
	    }
	    return HTThread_clear(kill_net);
	} else {
	    if (THD_TRACE)
		fprintf(TDEST, "Kill Thread. Thread is not registered\n");
	    return NO;
	}
    }
    return NO;
}
