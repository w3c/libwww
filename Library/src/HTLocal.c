/*							       	      HTLocal.c
**	LOCAL FILE HANDLING
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**      History:
**         HFN: Written
*/

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "HTAlert.h"
#include "HTParse.h"
#include "HTReq.h"
#include "HTNetMan.h"
#include "HTError.h"
#include "HTLocal.h"					 /* Implemented here */

/* ------------------------------------------------------------------------- */

/*
**      Opens a local file usien whatever means are available on the current
**      platform. If we have unix file descriptors then use that as we can use
**      select on them. Otherwise use ANSI C file descriptors.
**
**      returns         HT_ERROR        Error has occured or interrupted
**                      HT_OK           if connected
**                      HT_WOULD_BLOCK  if operation would have blocked
*/
PUBLIC int HTFileOpen (HTNet * net, char * local, HTLocalMode mode)
{
    HTRequest * request = net->request;
#ifndef NO_UNIX_IO
    int status = -1;    /* JTD:5/30/96 - must init status to -1 */
    if ((net->sockfd = open(local, mode)) == -1) {
	HTRequest_addSystemError(request, ERR_FATAL, errno, NO, "open");
	return HT_ERROR;
    }

    /* If non-blocking protocol then change socket status
    ** I use fcntl() so that I can ask the status before I set it.
    ** See W. Richard Stevens (Advan. Prog. in UNIX env, p.364)
    ** Be CAREFULL with the old `O_NDELAY' - it wont work as read()
    ** returns 0 when blocking and NOT -1. FNDELAY is ONLY for BSD
    ** and does NOT work on SVR4 systems. O_NONBLOCK is POSIX.
    */
    if (!net->preemptive) {
#ifdef HAVE_FCNTL
	if ((status = fcntl(net->sockfd, F_GETFL, 0)) != -1) {
#ifdef O_NONBLOCK
	    status |= O_NONBLOCK;/* POSIX */
#else
#ifdef F_NDELAY
	    status |= F_NDELAY; /* BSD */
#endif /* F_NDELAY */
#endif /* O_NONBLOCK */
	    status = fcntl(net->sockfd, F_SETFL, status);
	}
#endif /* HAVE_FCNTL */
	if (PROT_TRACE)
	    HTTrace("HTFileOpen.. `%s\' opened using %sblocking socket\n",
		    local, status == -1 ? "" : "NON-");
    }
    /* #endif - HAVE_FCNTL <- wrong location, moved up JTD:5/30/96 */
#else /* !NO_UNIX_IO */
#ifdef VMS
    if (!(net->fp = fopen(local, mode,"shr=put","shr=upd"))) {
	HTRequest_addSystemError(request, ERR_FATAL, errno, NO, "fopen");
	return HT_ERROR;
    }
#else
    if ((net->fp = fopen(local, mode)) == NULL) {
        HTRequest_addSystemError(request, ERR_FATAL, errno, NO, "fopen");
        return HT_ERROR;
    }
#endif /* VMS */
    if (PROT_TRACE)
        HTTrace("HTDoOpen.... `%s\' opened using FILE %p\n",local, net->fp);
#endif /* !NO_UNIX_IO */
    /* Create a channel for this socket or file descriptor */
    net->channel = HTChannel_new(net, YES);
    return HT_OK;
}

/*
**	Closes a file descriptor whatever means are available on the current
**	platform. If we have unix file descriptors then use this otherwise use
**	the ANSI C file descriptors
**
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_OK		if connected
**			HT_WOULD_BLOCK  if operation would have blocked
*/
PUBLIC int HTFileClose (HTNet * net)
{
    int status = -1;
    if (net) {
#ifndef NO_UNIX_IO
	if (net->fp) {
	    if (PROT_TRACE) HTTrace("Closing..... ANSI file %p\n", net->fp);
	    status = fclose(net->fp);
	    net->fp = NULL;
	}
#else
	if (net->sockfd != INVSOC) {
	    if (PROT_TRACE) HTTrace("Closing..... fd %d\n", net->sockfd);
	    status = NETCLOSE(net->sockfd);
	    net->sockfd = INVSOC;
	}
#endif /* NO_UNIX_IO */
    }
    return status < 0 ? HT_ERROR : HT_OK;
}

