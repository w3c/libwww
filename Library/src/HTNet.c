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
**  	12 June 94	Written by Henrik Frystyk, frystyk@dxcern.cern.ch
*/

/* Implemention dependent include files */
#include "tcp.h"

/* Library include files */
#include "HTUtils.h"
#include "HTAccess.h"
#include "HTError.h"
#include "HTThread.h"					 /* Implemented here */

#ifdef WIN32
#include <io.h>
#endif

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
	FD_ZERO(&HTfd_read) ;

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
	    "CLOSE"
	    };
	fprintf(TDEST,
		"Thread...... Register socket number %d for action %s\n",
		sockfd, *(actionmsg+action));
    }
    switch (action) {
      case THD_SET_WRITE:
	FD_CLR(sockfd, &HTfd_read);
	if (! FD_ISSET(sockfd, &HTfd_write))
		FD_SET(sockfd, &HTfd_write);
	if (! FD_ISSET(sockfd, &HTfd_set))
		FD_SET(sockfd, &HTfd_set);
	break;

      case THD_CLR_WRITE:
	FD_CLR(sockfd, &HTfd_read);
	FD_CLR(sockfd, &HTfd_write);
	FD_CLR(sockfd, &HTfd_set);
	break;

      case THD_SET_READ:
	if (! FD_ISSET(sockfd, &HTfd_read))
		FD_SET(sockfd, &HTfd_read);
	FD_CLR(sockfd, &HTfd_write);
	if (! FD_ISSET(sockfd, &HTfd_set))
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
	if (! FD_ISSET(sockfd, &HTfd_intr))
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
#ifdef _WINSOCKAPI_
	HTMaxfdpl = HTfd_set.fd_count ;
#else 
	if (sockfd+1 >= HTMaxfdpl) {
	    while (HTMaxfdpl > 0 && !FD_ISSET(HTMaxfdpl-1, &HTfd_set))
		HTMaxfdpl--;
	}
#endif
    } else {
#ifdef _WINSOCKAPI_
	HTMaxfdpl = HTfd_set.fd_count;
#else
	if (sockfd+1 > HTMaxfdpl)
	    HTMaxfdpl = sockfd+1;
#endif 
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
#ifdef _WINSOCKAPI_
    int i;
#endif
    if (THD_TRACE)
	fprintf(TDEST, "Thread...... Mark ALL Library sockfd INTERRUPTED\n");
#ifdef _WINSOCKAPI_
    for (i = 0 ; i < HTfd_set.fd_count; i++) {
	cnt = HTfd_set.fd_array[i];
	if (!FD_ISSET(cnt, fd_user))
#else
    for (cnt=0; cnt<HTMaxfdpl; cnt++) {
	if (FD_ISSET(cnt, &HTfd_set) && !FD_ISSET(cnt, fd_user))
#endif
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
#ifdef _WINSOCKAPI_
    if (HTfd_set.fd_count > 0)
	return YES;
#else
    for (cnt=STDIN_FILENO+1; cnt<HTMaxfdpl; cnt++)
	if (FD_ISSET(cnt, &HTfd_set))
	    return YES;
#endif
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
    SocAction found = SOC_INVALID;

#ifdef _WINSOCKAPI_
    int cnt;
    int ic = 0;
#endif

#ifdef _WINSOCKAPI_
    for (ic = 0; ic < HTfd_set.fd_count; ic++) { cnt = HTfd_set.fd_array[ic];
#else
    for (cnt=STDIN_FILENO+1; cnt<HTMaxfdpl; cnt++) {		/* INTERRUPT */
#endif
	if (FD_ISSET(cnt, &HTfd_intr)) {
	    if (THD_TRACE)
		fprintf(TDEST, "GetSocket... Socket %d INTERRUPTED\n", cnt);
	    found = SOC_INTERRUPT;
	    break;
	}
    }

    if (found == SOC_INVALID) {
#ifdef _WINSOCKAPI_
	for (ic = 0; ic < HTfd_set.fd_count; ic++) {
	    cnt = HTfd_set.fd_array[ic]; 
#else
	for (cnt=STDIN_FILENO+1; cnt<HTMaxfdpl; cnt++) {	     /* READ */
#endif
	    if (FD_ISSET(cnt, fd_read)) {
		if (THD_TRACE)
		    fprintf(TDEST, "GetSocket... Socket %d for READ\n", cnt);
		found = SOC_READ;
		break;
	    }
	}
    }

    if (found == SOC_INVALID) {
#ifdef _WINSOCKAPI_
	for (ic = 0; ic < HTfd_set.fd_count; ic++) {
	    cnt = HTfd_set.fd_array[ic]; 
#else
        for (cnt=STDIN_FILENO+1; cnt<HTMaxfdpl; cnt++) {   	    /* WRITE */
#endif
	    if (FD_ISSET(cnt, fd_write)) {
		if (THD_TRACE)
		    fprintf(TDEST, "GetSocket... Socket %d for WRITE\n", cnt);
		found = SOC_WRITE;
		break;
	    }
	}
    }

    if (found == SOC_INVALID || cur == NULL)
	return NULL;

    /* Find the corresponding HTNetInfo and HTRequest structure */
    while ((pres = (HTNetInfo *) HTList_nextObject(cur)) != NULL) {
	if (pres->sockfd == cnt) {
	    pres->action = found;
	    return pres->request;
	}
    }
    return NULL;
}

