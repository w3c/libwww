/*								     HTThread.c
**	MULTIPLE THREAD SOCKET MANAGEMENT
**
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This is the implementation of the internal library multithreading
**	functions. This includes an interrupt handler and a event loop.
**	
** History:
**  	12 June 94	Written by Henrik Frystyk, frystyk@dxcern.cern.ch
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTAccess.h"
#include "HTError.h"
#include "HTThread.h"					 /* Implemented here */

/* Type definitions and global variables etc. local to this module */
PRIVATE fd_set	HTfd_read;	      /* Bit array of sockets ready for read */
PRIVATE fd_set	HTfd_write;	     /* Bit array of sockets ready for write */
PRIVATE int 	HTMaxfdpl = 0;			/* Max number of sockets + 1 */

PRIVATE fd_set	HTfd_intr;	        /* All sockets currently interrupted */
PRIVATE fd_set	HTfd_set;	         /* All sockets currently registered */

PRIVATE HTList *HTThreads = NULL;      	 /* List of the HTNetInfo structures */

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

    /* Initialize the other internal bit arrays */
    FD_ZERO(&HTfd_write);
    FD_ZERO(&HTfd_set);
    FD_ZERO(&HTfd_intr);
    return YES;
}


/*							      HTThreadGetFDInfo
**
**	Returns the maximum bit width and the read and write bit array.
*/
PUBLIC int HTThreadGetFDInfo ARGS2(fd_set *, read, fd_set *, write)
{
    *read = HTfd_read;
    *write = HTfd_write;
    return HTMaxfdpl;
}


/*								  HTThreadState
**
**	This function registers a socket as waiting for the action given
**	(read or write etc.).
*/
PUBLIC void HTThreadState ARGS2(SOCKFD, sockfd, HTThreadAction, action)
{
    if (THD_TRACE) {
	static char *actionmsg[] = {
	    "SET WRITE",
	    "CLEAR WRITE",
	    "SET READ",
	    "CLEAR READ",
	    "SET INTERRUPT",
	    "CLEAR INTERRUPT",
	    "CLOSE"
	    };
	fprintf(TDEST,
		"Thread...... Register socket number %d for action %s\n",
		sockfd, *(actionmsg+action));
    }
    switch (action) {
      case THD_SET_WRITE:
	FD_CLR(sockfd, &HTfd_read);
	FD_SET(sockfd, &HTfd_write);
	FD_SET(sockfd, &HTfd_set);
	break;

      case THD_CLR_WRITE:
	FD_CLR(sockfd, &HTfd_read);
	FD_CLR(sockfd, &HTfd_write);
	FD_CLR(sockfd, &HTfd_set);
	break;

      case THD_SET_READ:
	FD_SET(sockfd, &HTfd_read);
	FD_CLR(sockfd, &HTfd_write);
	FD_SET(sockfd, &HTfd_set);
	break;

      case THD_CLR_READ:
	FD_CLR(sockfd, &HTfd_read);
	FD_CLR(sockfd, &HTfd_write);
	FD_CLR(sockfd, &HTfd_set);
	break;

      case THD_CLOSE:
	FD_CLR(sockfd, &HTfd_read);
	FD_CLR(sockfd, &HTfd_write);
	FD_CLR(sockfd, &HTfd_intr);
	FD_CLR(sockfd, &HTfd_set);
	break;

      case THD_SET_INTR:
	FD_CLR(sockfd, &HTfd_read);
	FD_CLR(sockfd, &HTfd_write);
	FD_SET(sockfd, &HTfd_intr);
	break;

      case THD_CLR_INTR:
	FD_CLR(sockfd, &HTfd_intr);
	break;

      default:
	if (THD_TRACE)
	    fprintf(TDEST, "Thread...... Illigal socket action\n");
    }

    /* Update max bit width. The method used ignores any other default
       opened file descriptors between 0 and the actual set of file
       descriptors used. However, they are not registered anyway */
    if (action == THD_CLOSE) {
	if (sockfd+1 >= HTMaxfdpl) {
	    while (HTMaxfdpl > 0 && !FD_ISSET(HTMaxfdpl-1, &HTfd_set))
		HTMaxfdpl--;
	}
    } else {
	if (sockfd+1 > HTMaxfdpl)
	    HTMaxfdpl = sockfd+1;
    }
    if (THD_TRACE)
	fprintf(TDEST, "Thread...... Max bitwidth is %d\n", HTMaxfdpl);
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
PUBLIC void HTThreadMarkIntrAll ARGS1(CONST fd_set *,	fd_user)
{
    int cnt;
    if (THD_TRACE)
	fprintf(TDEST, "Thread...... Mark ALL Library sockfd INTERRUPTED\n");
    for (cnt=0; cnt<HTMaxfdpl; cnt++) {
	if (FD_ISSET(cnt, &HTfd_set) && !FD_ISSET(cnt, fd_user))
	    FD_SET(cnt, &HTfd_intr);
    }
}


/*								 HTThreadActive
**
**	Returns yes as long as a socket other than stdin is registered in 
**	the total set of sockets.
*/
PUBLIC BOOL HTThreadActive NOARGS
{
    int cnt;
    for (cnt=STDIN_FILENO+1; cnt<HTMaxfdpl; cnt++)
	if (FD_ISSET(cnt, &HTfd_set))
	    return YES;
    return NO;
}


/*							     	   HTThread_new
**
**	Register the HTNetInfo structure in a list so that we can find the 
**	request which corresponds to a socket descriptor
*/
PUBLIC void HTThread_new ARGS1(HTNetInfo *, new_net)
{
    if (!HTThreads)
	HTThreads = HTList_new();
    HTList_addObject(HTThreads, (void *) new_net);
}


/*							         HTThread_clear
**
**	Remove the HTNetInfo from the list of acrive threads.
*/
PUBLIC int HTThread_clear ARGS1(HTNetInfo *, old_net)
{
    if (HTThreads)
	return HTList_removeObject(HTThreads, (void *) old_net);
    return NO;
}


/*							   HTThread_getCallBack
**
**	Finds a socket and makes it the active thread. READ has a higher
**	priority than WRITE as it might influence (stop) the WRITE process.
**	Returns the request structure of the active thread, NULL on error.
**	A interrupted socket has the highest priority
*/
PUBLIC HTRequest *HTThread_getRequest ARGS2(CONST fd_set *,	fd_read,
					    CONST fd_set *, 	fd_write)
{
    HTList *cur = HTThreads;
    HTNetInfo *pres;
    SOCKFD cnt;
    BOOL found = NO;
    for (cnt=STDIN_FILENO+1; cnt<HTMaxfdpl; cnt++) {		/* INTERRUPT */
	if (FD_ISSET(cnt, &HTfd_intr)) {
	    if (THD_TRACE)
		fprintf(TDEST, "GetSocket... Socket %d INTERRUPTED\n", cnt);
	    found = YES;
	    break;
	}
    }
    if (!found) {
	for (cnt=STDIN_FILENO+1; cnt<HTMaxfdpl; cnt++) {	     /* READ */
	    if (FD_ISSET(cnt, fd_read)) {
		if (THD_TRACE)
		    fprintf(TDEST, "GetSocket... Socket %d for READ\n", cnt);
		found = YES;
		break;
	    }
	}
    }
    if (!found) {
	for (cnt=STDIN_FILENO+1; cnt<HTMaxfdpl; cnt++) {   	    /* WRITE */
	    if (FD_ISSET(cnt, fd_write)) {
		if (THD_TRACE)
		    fprintf(TDEST, "GetSocket... Socket %d for WRITE\n", cnt);
		found = YES;
		break;
	    }
	}
    }
    if (!found || cur == NULL)
	return NULL;

    /* Find the corresponding HTNetInfo structure */
    while ((pres = (HTNetInfo *) HTList_nextObject(cur)) != NULL) {
	if (pres->sockfd == cnt) {
	    return pres->request;
	}
    }
    return NULL;
}
