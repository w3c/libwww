/*									HTTCP.c
**	TCP SPECIFIC CODE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This code is in common between client and server sides.
**
**	16 Jan 92  TBL	Fix strtol() undefined on CMU Mach.
**	25 Jun 92  JFG  Added DECNET option through TCP socket emulation.
**	13 Sep 93  MD   Added correct return of vmserrorno for HTInetStatus.
**			Added decoding of vms error message for MULTINET.
**	31 May 94  HF	Added cache on host id's; now use inet_ntoa() to
**			HTInetString and some other fixes. Added HTDoConnect
**			and HTDoAccept
*/

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"

#include "HTReqMan.h"
#include "HTNetMan.h"
#include "HTTCP.h"					 /* Implemented here */

/* VMS stuff */
#ifdef VMS
#ifndef MULTINET
#define FD_SETSIZE 32
#else /* Multinet */
#define FD_SETSIZE 256
#endif /* Multinet */
#endif /* VMS */

/* Macros and other defines */
/* x seconds penalty on a multi-homed host if IP-address is down */
#define TCP_PENALTY		1200

/* x seconds penalty on a multi-homed host if IP-address is timed out */
#define TCP_DELAY		600

/* ------------------------------------------------------------------------- */
/*	       	      CONNECTION ESTABLISHMENT MANAGEMENT 		     */
/* ------------------------------------------------------------------------- */

/*								HTDoConnect()
**
**	Note: Any port indication in URL, e.g., as `host:port' overwrites
**	the default port value.
**
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_OK		if connected
**			HT_WOULD_BLOCK  if operation would have blocked
*/
PUBLIC int HTDoConnect (HTNet * net, char * url, u_short default_port)
{
    int status;
    HTRequest * request = HTNet_request(net);
    char *fullhost = HTParse(url, "", PARSE_HOST);
    char *at_sign;
    char *host;

    /* if there's an @ then use the stuff after it as a hostname */
    if ((at_sign = strchr(fullhost, '@')) != NULL)
	host = at_sign+1;
    else
	host = fullhost;
    if (!*host) {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_NO_HOST,
		   NULL, 0, "HTDoConnect");
	HT_FREE(fullhost);
	return HT_ERROR;
    }

    /* Jump into the state machine */
    while (1) {
	switch (net->tcpstate) {
	  case TCP_BEGIN:
	    if (PROT_TRACE) HTTrace("HTDoConnect. Looking up `%s\'\n", host);
	    {
		char *port = strchr(host, ':');
		SockA *sin = &net->sock_addr;
		memset((void *) sin, '\0', sizeof(SockA));
		if (port) {
		    *port++ = '\0';
		    if (*port && isdigit(*port)) {
#ifdef DECNET
			sin->sdn_family = AF_DECnet;
			sin->sdn_objnum=(unsigned char)(strtol(port,(char**)0,10));
#else
			sin->sin_family = AF_INET;
			sin->sin_port = htons(atol(port));
#endif
		    }
		} else {
#ifdef DECNET
		    sin->sdn_family = AF_DECnet;
		    net->sock_addr.sdn_objnum = DNP_OBJ;
#else  /* Internet */
		    sin->sin_family = AF_INET;
		    sin->sin_port = htons(default_port);
#endif
		}
	    }

	    /* Find information about this host */
	    if ((net->host = HTHost_new(host)) == NULL) {
		if (PROT_TRACE) HTTrace("HTDoConnect. Can't get host info\n");
		net->tcpstate = TCP_ERROR;
		break;
	    }

	    /*
	    ** Check whether we have a cached channel for this host. If not
	    ** then go ahead and create a new socket and a new channel. If we
	    ** have to wait for a persistent connection the return. When we
	    ** get back, we check that the socket hasn't been closed in the
	    ** meantime
	    */
	    if ((net->channel = HTHost_channel(net->host)) != NULL) {
		net->sockfd = HTChannel_socket(net->channel);
		if (HTChannel_idle(net->channel)) {
		    HTChannel_upSemaphore(net->channel);
		    net->tcpstate = TCP_CONNECTED;
		    break;
		} else {
		    if (PROT_TRACE) HTTrace("HTDoConnect. Waiting...\n");
		    net->tcpstate = TCP_NEED_CONNECT;
		    HT_FREE(fullhost);
		    HTNet_wait(net);
		    return HT_PERSISTENT;
		}
	    }
	    net->tcpstate = TCP_DNS;
	    break;

	  case TCP_DNS:
	    if ((status = HTParseInet(net, host)) < 0) {
		if (PROT_TRACE)
		    HTTrace("HTDoConnect. Can't locate `%s\'\n", host);
		HTRequest_addError(request, ERR_FATAL, NO,HTERR_NO_REMOTE_HOST,
			   (void *) host, strlen(host), "HTDoConnect");
		net->tcpstate = TCP_ERROR;
		break;
	    }
	    if (!net->retry && status > 1)		/* If multiple homes */
		net->retry = status;
	    net->tcpstate = TCP_NEED_SOCKET;
	    break;

	  case TCP_NEED_SOCKET:
#ifdef DECNET
	    if ((net->sockfd=socket(AF_DECnet, SOCK_STREAM, 0))==INVSOC)
#else
	    if ((net->sockfd=socket(AF_INET, SOCK_STREAM,IPPROTO_TCP))==INVSOC)
#endif
	    {
		HTRequest_addSystemError(request, ERR_FATAL, socerrno, NO, "socket");
		net->tcpstate = TCP_ERROR;
		break;
	    }
	    if (PROT_TRACE)
		HTTrace("HTDoConnect. Created socket %d\n",net->sockfd);

	    /* If non-blocking protocol then change socket status
	    ** I use fcntl() so that I can ask the status before I set it.
	    ** See W. Richard Stevens (Advan. Prog. in UNIX environment, p.364)
	    ** Be CAREFULL with the old `O_NDELAY' - it will not work as read()
	    ** returns 0 when blocking and NOT -1. FNDELAY is ONLY for BSD and
	    ** does NOT work on SVR4 systems. O_NONBLOCK is POSIX.
	    */
	    if (!net->preemptive) {
#ifdef _WINSOCKAPI_
		{		/* begin windows scope  */
		    HTRequest * rq = request;
		    long levents = FD_READ | FD_WRITE | FD_ACCEPT | 
			FD_CONNECT | FD_CLOSE ;
		    int rv = 0 ;
				    
#ifdef WWW_WIN_ASYNC
		    /* N.B WSAAsyncSelect() turns on non-blocking I/O */
		    rv = WSAAsyncSelect( net->sockfd, rq->hwnd, 
					rq->winMsg, levents);
		    if (rv == SOCKET_ERROR) {
			status = -1 ;
			if (PROT_TRACE) 
			    HTTrace("HTDoConnect. WSAAsyncSelect() fails: %d\n", 
				     WSAGetLastError());
		    } /* error returns */
#else
		    int enable = 1;
		    status = IOCTL(net->sockfd, FIONBIO, &enable);
#endif
		} /* end scope */
#else 
#if defined(VMS)
		{
		    int enable = 1;
		    status = IOCTL(net->sockfd, FIONBIO, &enable);
		}
#else
		if((status = fcntl(net->sockfd, F_GETFL, 0)) != -1) {
#ifdef O_NONBLOCK
		    status |= O_NONBLOCK;			    /* POSIX */
#else
#ifdef F_NDELAY
		    status |= F_NDELAY;				      /* BSD */
#endif /* F_NDELAY */
#endif /* O_NONBLOCK */
		    status = fcntl(net->sockfd, F_SETFL, status);
		}
#endif /* VMS */
#endif /* WINDOW */
		if (PROT_TRACE) {
		    if (status == -1)
			HTTrace("HTDoConnect. Only blocking works\n");
		    else
			HTTrace("HTDoConnect. Non-blocking socket\n");
		}
	    } else if (PROT_TRACE)
		HTTrace("HTDoConnect. Blocking socket\n");

	    /* Create a channel for this socket */
	    net->channel = HTChannel_new(net, YES);

	    /* If multi-homed host then start timer on connection */
	    if (net->retry) net->connecttime = time(NULL);

	    /* Progress */
	    {
		HTAlertCallback *cbf = HTAlert_find(HT_PROG_CONNECT);
		if (cbf)
		    (*cbf)(request,HT_PROG_CONNECT,HT_MSG_NULL,NULL,host,NULL);
	    }
	    net->tcpstate = TCP_NEED_CONNECT;
	    break;

	  case TCP_NEED_CONNECT:
	    status = connect(net->sockfd, (struct sockaddr *) &net->sock_addr,
			     sizeof(net->sock_addr));
	    /*
	     * According to the Sun man page for connect:
	     *     EINPROGRESS         The socket is non-blocking and the  con-
	     *                         nection cannot be completed immediately.
	     *                         It is possible to select(2) for  comple-
	     *                         tion  by  selecting the socket for writ-
	     *                         ing.
	     * According to the Motorola SVR4 man page for connect:
	     *     EAGAIN              The socket is non-blocking and the  con-
	     *                         nection cannot be completed immediately.
	     *                         It is possible to select for  completion
	     *                         by  selecting  the  socket  for writing.
	     *                         However, this is only  possible  if  the
	     *                         socket  STREAMS  module  is  the topmost
	     *                         module on  the  protocol  stack  with  a
	     *                         write  service  procedure.  This will be
	     *                         the normal case.
	     */
#ifdef _WINSOCKAPI_
	    if (status == SOCKET_ERROR)
#else
	    if (status < 0) 
#endif
	    {
#ifdef EAGAIN
		if (socerrno==EINPROGRESS || socerrno==EAGAIN)
#else 
#ifdef _WINSOCKAPI_
		if (socerrno==WSAEWOULDBLOCK)
#else
		if (socerrno==EINPROGRESS)
#endif /* _WINSOCKAPI_ */
#endif /* EAGAIN */
		{
		    if (PROT_TRACE)
			HTTrace("HTDoConnect. WOULD BLOCK `%s'\n",host);
		    HTEvent_register(net->sockfd, request, (SockOps)FD_CONNECT,
				     net->cbf, net->priority);
		    HT_FREE(fullhost);
		    return HT_WOULD_BLOCK;
		}
		if (socerrno == EISCONN) {
		    net->tcpstate = TCP_CONNECTED;
		    break;
		}
#ifdef _WINSOCKAPI_
		if (socerrno == WSAEBADF)  	       /* We lost the socket */
#else
		if (socerrno == EBADF)  	       /* We lost the socket */
#endif
		{
		    net->tcpstate = TCP_NEED_SOCKET;
		    break;
		}
		if (net->retry) {
		    net->connecttime -= time(NULL);
		    /* Added EINVAL `invalid argument' as this is what I 
		       get back from a non-blocking connect where I should 
		       get `connection refused' on BSD. SVR4 gives SIG_PIPE */
#if defined(__srv4__) || defined (_WINSOCKAPI_)
		    if (socerrno==ECONNREFUSED || socerrno==ETIMEDOUT ||
			socerrno==ENETUNREACH || socerrno==EHOSTUNREACH ||
			socerrno==EHOSTDOWN)
#else
		    if (socerrno==ECONNREFUSED || socerrno==ETIMEDOUT ||
			socerrno==ENETUNREACH || socerrno==EHOSTUNREACH ||
			socerrno==EHOSTDOWN || socerrno==EINVAL)
#endif
		        net->connecttime += TCP_DELAY;
		    else
		        net->connecttime += TCP_PENALTY;
		    HTDNS_updateWeigths(net->dns, net->home, net->connecttime);
		}
		net->tcpstate = TCP_ERROR;		
	    } else
		net->tcpstate = TCP_CONNECTED;
	    break;

	  case TCP_CONNECTED:
	    HTEvent_unregister(net->sockfd, (SockOps) FD_CONNECT);
	    if (net->retry) {
		net->connecttime -= time(NULL);
		HTDNS_updateWeigths(net->dns, net->home, net->connecttime);
	    }
	    net->retry = 0;
	    HT_FREE(fullhost);	    
	    net->tcpstate = TCP_BEGIN;
	    return HT_OK;
	    break;

	  case TCP_NEED_BIND:
	  case TCP_NEED_LISTEN:
	  case TCP_ERROR:
	    if (PROT_TRACE) HTTrace("HTDoConnect. Connect failed\n");
	    if (net->sockfd != INVSOC) {
	        HTEvent_unregister(net->sockfd, (SockOps) FD_ALL);
		NETCLOSE(net->sockfd);
		net->sockfd = INVSOC;
		if (HTHost_isPersistent(net->host)) {	 /* Inherited socket */
		    HTHost_clearChannel(net->host);
		    net->tcpstate = TCP_NEED_SOCKET;
		    break;
		}
	    }

	    /* Do we have more homes to try? */
	    if (--net->retry > 0) {
	        HTRequest_addSystemError(request, ERR_NON_FATAL, socerrno, NO,
			      "connect");
		net->tcpstate = TCP_DNS;
		break;
	    }
	    HTRequest_addSystemError(request, ERR_FATAL,socerrno,NO,"connect");
	    HTDNS_delete(host);
	    net->retry = 0;
	    HT_FREE(fullhost);
	    net->tcpstate = TCP_BEGIN;
	    return HT_ERROR;
	    break;
	}
    }
}

/*	HTDoAccept()
**	------------
**	This function makes a non-blocking accept which will turn up as ready
**	read in the select.
**	Returns
**		HT_ERROR	Error has occured or interrupted
**		HT_OK		if connected
**		HT_WOULD_BLOCK  if operation would have blocked
*/
PUBLIC int HTDoAccept (HTNet * net, HTNet ** accepted)
{
    int status;
    int size = sizeof(net->sock_addr);
    HTRequest * request = HTNet_request(net);
    if (!request || net->sockfd==INVSOC) {
	if (PROT_TRACE) HTTrace("HTDoAccept.. Invalid socket\n");
	return HT_ERROR;
    }

    /* Progress report */
    {
	HTAlertCallback *cbf = HTAlert_find(HT_PROG_ACCEPT);
	if (cbf) (*cbf)(request, HT_PROG_ACCEPT, HT_MSG_NULL,NULL, NULL, NULL);
    }
    status = accept(net->sockfd, (struct sockaddr *) &net->sock_addr, &size);
#ifdef _WINSOCKAPI_
    if (status == SOCKET_ERROR)
#else
    if (status < 0) 
#endif
    {
#ifdef EAGAIN
	if (socerrno==EINPROGRESS || socerrno==EAGAIN)
#else 
#ifdef _WINSOCKAPI_
        if (socerrno==WSAEWOULDBLOCK)
#else
	if (socerrno==EINPROGRESS)
#endif /* _WINSOCKAPI_ */
#endif /* EAGAIN */
	{
	    if (PROT_TRACE)
		HTTrace("HTDoAccept.. WOULD BLOCK %d\n", net->sockfd);
	    HTEvent_register(net->sockfd, request, (SockOps) FD_ACCEPT,
			     net->cbf, net->priority);
	    return HT_WOULD_BLOCK;
	}
	HTRequest_addSystemError(request, ERR_WARN, socerrno, YES, "accept");
	if (PROT_TRACE) HTTrace("HTDoAccept.. Accept failed\n");
	return HT_ERROR;
    }

    if (PROT_TRACE) HTTrace("Accepted.... socket %d\n", status);

    /*
    ** If accepted is the same as the net obejct then reuse it, else create
    ** a new object and leave the original alone
    */
    if (*accepted == net)
	HTDoClose(net);
    else
	*accepted = HTNet_dup(net);
    (*accepted)->sockfd = status;	

    /* Create a channel for the new socket */
    (*accepted)->channel = HTChannel_new(*accepted, NO);

    return HT_OK;
}


/*	HTDoListen
**	----------
**	Listens on the specified port. 0 means that we chose it here
**	If master==INVSOC then we listen on all local interfaces (using a 
**	wildcard). If !INVSOC then use this as the local interface
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_OK		if connected
*/
PUBLIC int HTDoListen (HTNet * net, u_short port, SOCKET master, int backlog)
{
    int status;

    /* Jump into the state machine */
    while (1) {
	switch (net->tcpstate) {
	  case TCP_BEGIN:
	    {
		SockA *sin = &net->sock_addr;
		memset((void *) sin, '\0', sizeof(SockA));
#ifdef DECNET
		sin->sdn_family = AF_DECnet;
		sin->sdn_objnum = port;
#else
		sin->sin_family = AF_INET;
		if (master != INVSOC) {
		    int len = sizeof(SockA);
		    if (getsockname(master, (struct sockaddr *) sin, &len)<0) {
			HTRequest_addSystemError(net->request, ERR_FATAL,
						 socerrno, NO, "getsockname");
			net->tcpstate = TCP_ERROR;
			break;
		    }
		} else
		    sin->sin_addr.s_addr = INADDR_ANY;
		sin->sin_port = htons(port);
#endif
	    }
	    if (PROT_TRACE)
		HTTrace("Socket...... Listen on port %d\n", port);
	    net->tcpstate = TCP_NEED_SOCKET;
	    break;

	  case TCP_NEED_SOCKET:
#ifdef DECNET
	    if ((net->sockfd=socket(AF_DECnet, SOCK_STREAM, 0))==INVSOC)
#else
	    if ((net->sockfd=socket(AF_INET, SOCK_STREAM,IPPROTO_TCP))==INVSOC)
#endif
	    {
		HTRequest_addSystemError(net->request, ERR_FATAL, socerrno,
					 NO, "socket");
		net->tcpstate = TCP_ERROR;
		break;
	    }
	    if (PROT_TRACE)
		HTTrace("Socket...... Created %d\n", net->sockfd);

	    /* If non-blocking protocol then change socket status
	    ** I use fcntl() so that I can ask the status before I set it.
	    ** See W. Richard Stevens (Advan. Prog. in UNIX environment, p.364)
	    ** Be CAREFULL with the old `O_NDELAY' - it will not work as read()
	    ** returns 0 when blocking and NOT -1. FNDELAY is ONLY for BSD and
	    ** does NOT work on SVR4 systems. O_NONBLOCK is POSIX.
	    */
	    if (!net->preemptive) {
#ifdef _WINSOCKAPI_ 
		{		/* begin windows scope  */
		    long levents = FD_READ | FD_WRITE | FD_ACCEPT | 
			FD_CONNECT | FD_CLOSE ;
		    int rv = 0 ;
				    
#ifdef WWW_WIN_ASYNC
		    /* N.B WSAAsyncSelect() turns on non-blocking I/O */
		    rv = WSAAsyncSelect(net->sockfd, net->request->hwnd, 
					net->request->winMsg, levents);
		    if (rv == SOCKET_ERROR) {
			status = -1 ;
			if (PROT_TRACE) 
			    HTTrace("Socket...... WSAAsyncSelect() fails: %d\n", 
				     WSAGetLastError());
			} /* error returns */
#else
		    int enable = 1 ;
		    status = IOCTL(net->sockfd, FIONBIO, &enable);
#endif
		} /* end scope */
#else 
#if defined(VMS)
		{
		    int enable = 1;
		    status = IOCTL(net->sockfd, FIONBIO, &enable);
		}
#else
		if((status = fcntl(net->sockfd, F_GETFL, 0)) != -1) {
#ifdef O_NONBLOCK
		    status |= O_NONBLOCK;			    /* POSIX */
#else
#ifdef F_NDELAY
		    status |= F_NDELAY;				      /* BSD */
#endif /* F_NDELAY */
#endif /* O_NONBLOCK */
		    status = fcntl(net->sockfd, F_SETFL, status);
		}
#endif /* VMS */
#endif /* WINDOW */
		if (PROT_TRACE) {
		    if (status == -1)
			HTTrace("Sockrt...... Blocking socket\n");
		    else
			HTTrace("Socket...... Non-blocking socket\n");
		}
	    }
	    net->tcpstate = TCP_NEED_BIND;
	    break;

	  case TCP_NEED_BIND:
	    status = bind(net->sockfd, (struct sockaddr *) &net->sock_addr,
			  sizeof(net->sock_addr));
#ifdef _WINSOCKAPI_
	    if (status == SOCKET_ERROR)
#else
	    if (status < 0) 
#endif
	    {
		if (PROT_TRACE)
		    HTTrace("Socket...... Bind failed %d\n", socerrno);
		net->tcpstate = TCP_ERROR;		
	    } else
		net->tcpstate = TCP_NEED_LISTEN;
	    break;

	  case TCP_NEED_LISTEN:
	    status = listen(net->sockfd, backlog);
#ifdef _WINSOCKAPI_
	    if (status == SOCKET_ERROR)
#else
	    if (status < 0) 
#endif
		net->tcpstate = TCP_ERROR;		
	    else
		net->tcpstate = TCP_CONNECTED;
	    break;

	  case TCP_CONNECTED:
	    net->tcpstate = TCP_BEGIN;
	    if (PROT_TRACE)
		HTTrace("Socket...... Bind and listen on port %d %s\n",
			(int) ntohs(net->sock_addr.sin_port),
			HTInetString(&net->sock_addr));
	    return HT_OK;
	    break;

	  case TCP_NEED_CONNECT:
	  case TCP_DNS:
	  case TCP_ERROR:
	    if (PROT_TRACE) HTTrace("Socket...... Listen failed\n");
	    HTRequest_addSystemError(net->request, ERR_FATAL, socerrno, NO, "HTDoListen");
	    net->tcpstate = TCP_BEGIN;
	    return HT_ERROR;
	    break;
	}
    }
}

/*	HTDoClose
**	---------
**	Closes a file descriptor whatever means are available on the current
**	platform. If we have unix file descriptors then use this otherwise use
**	the ANSI C file descriptors
**
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_OK		if connected
**			HT_WOULD_BLOCK  if operation would have blocked
*/
PUBLIC int HTDoClose (HTNet * net)
{
    int status = -1;
    if (net && net->sockfd != INVSOC) {
	if (PROT_TRACE) HTTrace("Socket...... Close %d\n", net->sockfd);
	status = NETCLOSE(net->sockfd);
	HTEvent_unregister(net->sockfd, (SockOps) FD_ALL);
 	net->sockfd = INVSOC;
    }
    return status < 0 ? HT_ERROR : HT_OK;
}

