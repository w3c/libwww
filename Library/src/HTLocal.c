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
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
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
    HTRequest * request = HTNet_request(net);
    HTHost * host = HTNet_host(net);
#ifdef NO_UNIX_IO
    FILE * fp = NULL;
#else
    SOCKET sockfd = INVSOC;
#endif /* NO_UNIX_IO */

#ifndef NO_UNIX_IO
    int status = -1;    /* JTD:5/30/96 - must init status to -1 */
    if ((sockfd = open(local, mode)) == -1) {
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
    if (!HTNet_preemptive(net)) {
#ifdef HAVE_FCNTL
	if ((status = fcntl(sockfd, F_GETFL, 0)) != -1) {
#ifdef O_NONBLOCK
	    status |= O_NONBLOCK;/* POSIX */
#else
#ifdef F_NDELAY
	    status |= F_NDELAY; /* BSD */
#endif /* F_NDELAY */
#endif /* O_NONBLOCK */
	    status = fcntl(sockfd, F_SETFL, status);
	}
#endif /* HAVE_FCNTL */
	HTTRACE(PROT_TRACE, "HTFileOpen.. `%s\' opened %d as %sblocking socket\n" _ 
		local _ sockfd _ status == -1 ? "" : "NON-");
    }
    /* #endif - HAVE_FCNTL <- wrong location, moved up JTD:5/30/96 */
#else /* !NO_UNIX_IO */
#ifdef VMS
    if ((fp = fopen(local, mode,"shr=put","shr=upd")) == NULL) {
	HTRequest_addSystemError(request, ERR_FATAL, errno, NO, "fopen");
	return HT_ERROR;
    }
#else
    if ((fp = fopen(local, mode)) == NULL) {
        HTRequest_addSystemError(request, ERR_FATAL, errno, NO, "fopen");
        return HT_ERROR;
    }
#endif /* VMS */
    HTTRACE(PROT_TRACE, "HTDoOpen.... `%s\' opened using FILE %p\n" _ local _ fp);
#endif /* !NO_UNIX_IO */

    /*
    **  Associate the channel with the host and create
    **  an input and and output stream 
    */
#ifdef NO_UNIX_IO
    HTHost_setChannel(host, HTChannel_new(INVSOC, fp, YES));
#else
    HTHost_setChannel(host, HTChannel_new(sockfd, NULL, YES));
#endif /* NO_UNIX_IO */

    HTHost_getInput(host, HTNet_transport(net), NULL, 0);
    HTHost_getOutput(host, HTNet_transport(net), NULL, 0);
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
    HTHost * host = HTNet_host(net);
    HTChannel * ch = HTHost_channel(host);
    int status = -1;
    if (net && ch) {
#ifdef NO_UNIX_IO
	FILE * fp = HTChannel_file(ch);
	if (fp) {
	    HTTRACE(PROT_TRACE, "Closing..... ANSI file %p\n" _ fp);
	    status = fclose(fp);
	    HTChannel_setFile(ch, NULL);
	}
#else
	SOCKET sockfd = HTChannel_socket(ch);
	if (sockfd != INVSOC) {
	    HTTRACE(PROT_TRACE, "Closing..... fd %d\n" _ sockfd);
	    status = NETCLOSE(sockfd);
	    HTChannel_setSocket(ch, INVSOC);
	}
#endif /* NO_UNIX_IO */
    }
    return status < 0 ? HT_ERROR : HT_OK;
}

