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
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTReqMan.h"
#include "HTNetMan.h"
#include "HTTCP.h"					 /* Implemented here */

#include "HTHstMan.h"

/* VMS stuff */
#ifdef VMS
#ifndef MULTINET
#define FD_SETSIZE 32
#else /* Multinet */
#define FD_SETSIZE 256
#endif /* Multinet */
#endif /* VMS */

/* Macros and other defines */
/* x ms penalty on a multi-homed host if IP-address is unreachable */
#define TCP_DELAY		30000

/* x ms penalty on a multi-homed host if IP-address is down for unknown reason */
#define TCP_PENALTY		60000

/* empirical study in socket call error codes
   yovavm@contact.com : added handling for WSAEINVAL error code (Windows)
   "When calling connect() in the second time, after the first call to 
   connect() returned WSAEWOULDBLOCK, an error of WSAEINVAL is returned. 
   It happens often on WinNT & Win95, and rarely on Win2K & Win98, where in 
   most cases the second call to connect() returns WSAEISCON (10056).
   jose@w3.org : didn't add that test for Unix, as the connect() doc (Linux
   and Solaris) says it's not needed.
 */
#ifdef _WINSOCKAPI_					/* windows */
#define NETCALL_ERROR(ret)	(ret == SOCKET_ERROR)
#define NETCALL_DEADSOCKET(err)	(err == WSAEBADF)
#define NETCALL_WOULDBLOCK(err)	(err == WSAEWOULDBLOCK)
#define NETCALL_INVAL(err)      (err == WSAEINVAL)
#else /* _WINSOCKAPI_ 					   unix    */
#define NETCALL_ERROR(ret)	(ret < 0)
#define NETCALL_DEADSOCKET(err)	(err == EBADF)
#if defined(EAGAIN) && defined(EALREADY)
#define NETCALL_WOULDBLOCK(err)	(err == EINPROGRESS || \
				 err == EALREADY || \
				 err == EAGAIN)
#else /* (EAGAIN && EALREADY) */
#ifdef EALREADY
#define NETCALL_WOULDBLOCK(err)	(err == EINPROGRESS || err == EALREADY)
#else /* EALREADY */
#ifdef EAGAIN
#define NETCALL_WOULDBLOCK(err)	(err == EINPROGRESS || err == EAGAIN)
#else /* EAGAIN */
#define NETCALL_WOULDBLOCK(err)	(err == EINPROGRESS)
#endif /* !EAGAIN */
#endif /* !EALREADY */
#endif /* !(EAGAIN && EALREADY) */
#endif /* !_WINSOCKAPI_ 				   done */

#if defined(__svr4__) || defined (_WINSOCKAPI_)
#define HT_HOSTUNREACHABLE(e)	((e)==ECONNREFUSED || (e)==ETIMEDOUT || \
				 (e)==ENETUNREACH || (e)==EHOSTUNREACH || \
				 (e)==EHOSTDOWN)
#else
#define HT_HOSTUNREACHABLE(e)	((e)==ECONNREFUSED || (e)==ETIMEDOUT || \
				 (e)==ENETUNREACH || (e)==EHOSTUNREACH || \
				 (e)==EHOSTDOWN || (e)==EINVAL)
#endif

/* ------------------------------------------------------------------------- */
/*	       	      CONNECTION ESTABLISHMENT MANAGEMENT 		     */
/* ------------------------------------------------------------------------- */

/* _makeSocket - create a socket, if !preemptive, set FIONBIO
** returns sockfd or INVSOC if error
*/
PRIVATE int _makeSocket (HTHost * host, HTRequest * request, int preemptive)
{
    int status = 1;
    SOCKET sockfd = INVSOC;
#ifdef DECNET
    if ((sockfd=socket(AF_DECnet, SOCK_STREAM, 0))==INVSOC)
#else
    if ((sockfd=socket(AF_INET, SOCK_STREAM,IPPROTO_TCP))==INVSOC)
#endif
    {
	HTRequest_addSystemError(request, ERR_FATAL, socerrno, NO, "socket");
	return INVSOC;
    }
    HTTRACE(PROT_TRACE, "Socket...... Created %d\n" _ sockfd);

    /* Increase the number of sockets by one */
    HTNet_increaseSocket();

    /*
    **  If we have compiled without Nagle's algorithm then try and turn
    **  it off now
    */
#if defined(HT_NO_NAGLE) && defined(HAVE_SETSOCKOPT) && defined(TCP_NODELAY)
    {
	int disable = 1;
	status = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
			    (char *) &disable, sizeof(int));
	if (status == -1) {
	    HTTRACE(PROT_TRACE, "Socket...... Could not disable Nagle's algorithm - error %d\n" _ 
			sockfd);
	} else {
	    HTTRACE(PROT_TRACE, "Socket...... Turned off Nagle's algorithm\n");
	}
    }
#endif

    /* If non-blocking protocol then change socket status
    ** I use fcntl() so that I can ask the status before I set it.
    ** See W. Richard Stevens (Advan. Prog. in UNIX environment, p.364)
    ** Be CAREFULL with the old `O_NDELAY' - it will not work as read()
    ** returns 0 when blocking and NOT -1. FNDELAY is ONLY for BSD and
    ** does NOT work on SVR4 systems. O_NONBLOCK is POSIX.
    */
    if (!preemptive) {
#ifdef _WINSOCKAPI_
	{		/* begin windows scope  */
	    long levents = FD_READ | FD_WRITE | FD_ACCEPT | 
			   FD_CONNECT | FD_CLOSE ;
	    int rv = 0 ;
	    u_long one = 1;

	    status = ioctlsocket(sockfd, FIONBIO, &one) == 
		     SOCKET_ERROR ? -1 : 0;
	} /* end scope */
#else /* _WINSOCKAPI_ */
#if defined(VMS)
	{
	    int enable = 1;
	    status = IOCTL(sockfd, FIONBIO, &enable);
	}
#else /* VMS */
	if((status = fcntl(sockfd, F_GETFL, 0)) != -1) {
#ifdef O_NONBLOCK
	    status |= O_NONBLOCK;			    /* POSIX */
#else /* O_NONBLOCK */
#ifdef F_NDELAY
		    status |= F_NDELAY;				      /* BSD */
#endif /* F_NDELAY */
#endif /* !O_NONBLOCK */
		    status = fcntl(sockfd, F_SETFL, status);
	}
#endif /* !VMS */
#endif /* !_WINSOCKAPI_ */
	HTTRACE(PROT_TRACE, "Socket...... %slocking socket\n" _ status == -1 ? "B" : "Non-b");
    } else
	HTTRACE(PROT_TRACE, "Socket...... Blocking socket\n");

    return sockfd;
}

/*
**  Associate the channel with the host and create an input and and output stream
**  for this host/channel
*/
PRIVATE BOOL createChannelAndTransportStreams (HTHost * host, SOCKET sockfd, HTTransport * trans)
{
    if (host && sockfd!=INVSOC && trans) {
	HTHost_setChannel(host, HTChannel_new(sockfd, NULL, YES));
	HTHost_getInput(host, trans, NULL, 0);
	HTHost_getOutput(host, trans, NULL, 0);
	return YES;
    }
    return NO;
}

/*								HTDoConnect()
**
**	Note: Any port indication in URL, e.g., as `host:port' overwrites
**	the default port value.
**
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_OK		if connected
**			HT_WOULD_BLOCK  if operation would have blocked
*/
PUBLIC int HTDoConnect (HTNet * net)
{
    HTHost * host = HTNet_host(net);
    HTRequest * request = HTNet_request(net);
    char * hostname = HTHost_name(host);
    int preemptive = net->preemptive;
    int status = HT_OK;

    /* Jump into the state machine */
    while (1) {
	switch (host->tcpstate) {
	  case TCP_BEGIN:
	  {
	      /*
	      ** Add the net object to the host object found above. If the
	      ** host is idle then we can start the request right away,
	      ** otherwise we must wait until it is free. 
	      */
	      if ((status = HTHost_addNet(host, net)) == HT_PENDING)
		  HTTRACE(PROT_TRACE, "HTDoConnect. Pending...\n");

	      /*
	      ** If we are pending then return here, otherwise go to next state
	      ** which is setting up a channel
	      */
	      host->tcpstate = TCP_CHANNEL;
	      HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_CHANNEL.\n" _ host);
	      if (status == HT_PENDING) return HT_PENDING;
	  }
	  break;

	case TCP_CHANNEL:
	    /*
	    **  The next state depends on whether we have a connection
	    **  or not - if so then we can jump directly to connect() to
	    **  test it - otherwise we must around DNS to get the name
	    **  Resolved
	    */
	    if (HTHost_channel(host) == NULL) {
		host->tcpstate = TCP_DNS;
		HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_DNS.\n" _ host);
	    } else {

		/*
		**  There is now one more using the channel
		*/
		HTChannel_upSemaphore(host->channel);

		/*
		**  We are now all set and can jump to connected mode
		*/
		host->tcpstate = TCP_CONNECTED;
		HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_CONNECTED.\n" _ host);
	    }
	    hostname = HTHost_name(host);
	    break;

	case TCP_DNS:
	    if ((status = HTParseInet(host, hostname, request)) < 0) {
		HTTRACE(PROT_TRACE, "HTDoConnect. Can't locate `%s\'\n" _ hostname);
		HTRequest_addError(request, ERR_FATAL, NO,
				   HTERR_NO_REMOTE_HOST,
				   (void *) hostname, strlen(hostname),
				   "HTDoConnect");
		host->tcpstate = TCP_DNS_ERROR;
		HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_ERROR.\n" _ host);
		break;
	    }
	    if (!HTHost_retry(host) && status > 1)		/* If multiple homes */
		HTHost_setRetry(host, status);
	    host->tcpstate = TCP_NEED_SOCKET;
	    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_NEED_SOCKET.\n" _ host);
	    break;

	case TCP_NEED_SOCKET:
	{
	    SOCKET sockfd;

	    /* Create a new socket */
	    if ((sockfd = _makeSocket(host, request, preemptive)) == INVSOC) {
		host->tcpstate = TCP_ERROR;
		break;
	    }

	    /* Create channnel and streams */
	    createChannelAndTransportStreams (host, sockfd, net->transport);

	    /* If multi-homed host then start timer on connection */
	    if (HTHost_retry(host)) host->connecttime = HTGetTimeInMillis();

	    /* Progress notification */
	    {
		HTAlertCallback *cbf = HTAlert_find(HT_PROG_CONNECT);
		if (cbf) (*cbf)(request, HT_PROG_CONNECT, HT_MSG_NULL,
				NULL, hostname, NULL);
	    }
	    host->tcpstate = TCP_NEED_CONNECT;
	    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_NEED_CONNECT.\n" _ host);
	    break;
	}

	case TCP_NEED_CONNECT:
#ifdef _WINSOCKAPI_
	    /* 2000/08/02 Jens Meggers (jens@meggers.com):
            ** In HTDoConnect(), the connect command is done before the
            ** WSAAsyncSelect() that is called when 
            ** HTHost_register(host, net, HTEvent_CONNECT); is executed. 
            ** Although that is in line with the WinSock2 and Microsoft 
            ** documentation, it does _not_ work all the time. I have done 
            ** extensive tests on Win2000 and Win 4.0 SP5. In very rare cases,
            ** the connect is finished between the connect() command itself and 
            ** the WSAAsyncSelect(). In this unlikely case, WinSock does not 
            ** (always) send the FD_CONNECT message. As a result, when using 
            ** the Async mode, the event loop hangs because there is no 
            ** timeout procedure registered for FD_CONNECT.
	    ** JK: what happens if status returns an error? Do we have to
	    ** unregister the HTEvent_CONNECT event then?                       
            */
	    HTHost_register(host, net, HTEvent_CONNECT);
#endif /* _WINSOCKAPI_ */
	    status = connect(HTChannel_socket(host->channel), (struct sockaddr *) &host->sock_addr,
			     sizeof(host->sock_addr));
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
	    if (NETCALL_ERROR(status))
	    {
		if (NETCALL_WOULDBLOCK(socerrno))
		{
		    HTTRACE(PROT_TRACE, "HTDoConnect. WOULD BLOCK `%s'\n" _ hostname);
#ifndef _WINSOCKAPI_

		    HTHost_register(host, net, HTEvent_CONNECT);
#endif /* _WINSOCKAPI_ */
		    return HT_WOULD_BLOCK;
		}
#ifdef _WINSOCKAPI_
		/*
		 * yovavm@contact.com
		 *
		 * According to Microsoft docs, the error code WSAEALREADY is 
		 * described as:
		 * "A nonblocking connect call is in progress on the specified
		 *  socket. Note In order to preserve backward compatibility, 
		 *  this error is reported as WSAEINVAL to Windows Sockets 1.1
		 *  applications that link to either Winsock.dll or
		 *  Wsock32.dll."
		 */
		if (NETCALL_INVAL(socerrno)) {
		    host->tcpstate = TCP_CONNECTED;
		    HTTRACE(PROT_TRACE, "Connection to HTHost %p is already in progress.\n" _ host);
		    break;
		}
#endif /* _WINSOCKAPI_ */

		if (socerrno == EISCONN) {
		    host->tcpstate = TCP_CONNECTED;
		    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_CONNECTED.\n" _ host);
		    break;
		}
		if (NETCALL_DEADSOCKET(socerrno))     /* We lost the socket */
		{
		    host->tcpstate = TCP_NEED_SOCKET;
		    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_NEED_SOCKET.\n" _ host);
		    break;
		}
		if (HTHost_retry(host)) {
		    host->connecttime = HTGetTimeInMillis() - host->connecttime;
		    /* Added EINVAL `invalid argument' as this is what I 
		       get back from a non-blocking connect where I should 
		       get `connection refused' on BSD. SVR4 gives SIG_PIPE */
		    if (HT_HOSTUNREACHABLE(socerrno))
		        host->connecttime += TCP_DELAY;
		    else
		        host->connecttime += TCP_PENALTY;
		    HTDNS_updateWeigths(host->dns, HTHost_home(host), host->connecttime);
		}
		host->tcpstate = TCP_ERROR;		
		HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_ERROR.\n" _ host);
	    } else {
		host->tcpstate = TCP_CONNECTED;
		HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_CONNECTED.\n" _ host);
	    }
	    break;

	  case TCP_CONNECTED:
	    HTHost_unregister(host, net, HTEvent_CONNECT);
	    if (HTHost_retry(host)) {
		host->connecttime = HTGetTimeInMillis() - host->connecttime;
		HTDNS_updateWeigths(host->dns, HTHost_home(host), host->connecttime);
	    }
	    HTHost_setRetry(host, 0);
	    host->tcpstate = TCP_IN_USE;
	    HTTRACE(PROT_TRACE, "HTHost %p connected.\n" _ host);
	    return HT_OK;
	    break;

	  /* Once a host is connected, subsequent connections are immediately OK */
	  case TCP_IN_USE:
	      if ((status = HTHost_addNet(host, net)) == HT_PENDING) {
		  HTTRACE(PROT_TRACE, "HTDoConnect. Pending...\n");
		  return HT_PENDING;
	      }

	      HTChannel_upSemaphore(host->channel);
	      return HT_OK;

	  case TCP_DNS_ERROR:
	    HTHost_setRetry(host, 0);
	    host->tcpstate = TCP_BEGIN;
	    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_BEGIN.\n" _ host);
	    return HT_NO_HOST;
	    break;

	  case TCP_NEED_BIND:
	  case TCP_NEED_LISTEN:
	  case TCP_ERROR:
	    if (HTChannel_socket(host->channel) != INVSOC) {
		NETCLOSE(HTChannel_socket(host->channel));
		if (HTHost_isPersistent(host)) {	 /* Inherited socket */
		    HTHost_setPersistent(host, NO, HT_TP_SINGLE);
		    host->tcpstate = TCP_NEED_SOCKET;
		    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_NEED_SOCKET.\n" _ host);
		    break;
		}
	    }

	    /* Do we have more homes to try? */
	    HTHost_decreaseRetry(host);
	    if (HT_HOSTUNREACHABLE(socerrno) && HTHost_retry(host) > 0) {
	        HTRequest_addSystemError(request, ERR_NON_FATAL, socerrno, NO,"connect");
		host->tcpstate = TCP_DNS;
		HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_DNS.\n" _ host);
		break;
	    }
	    HTRequest_addSystemError(request, ERR_FATAL, socerrno, NO, "connect");
	    HTDNS_delete(hostname);
	    HTHost_setRetry(host, 0);
	    host->tcpstate = TCP_BEGIN;
	    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_BEGIN.\n" _ host);
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
PUBLIC int HTDoAccept (HTNet * listening, HTNet * accepting)
{
    HTHost * host = HTNet_host(listening);
    HTRequest * request = HTNet_request(accepting);
    int size = sizeof(host->sock_addr);
    int status;
    if (!request || HTNet_socket(listening)==INVSOC) {
	HTTRACE(PROT_TRACE, "HTDoAccept.. Invalid socket\n");
	return HT_ERROR;
    }

    status = accept(HTNet_socket(listening), (struct sockaddr *) &host->sock_addr, &size);
    if (NETCALL_ERROR(status))
    {
	if (NETCALL_WOULDBLOCK(socerrno))
	{
	    HTTRACE(PROT_TRACE, "HTDoAccept.. WOULD BLOCK %d\n" _ HTNet_socket(listening));
	    HTHost_register(host, listening, HTEvent_ACCEPT);
	    return HT_WOULD_BLOCK;
	}
	HTRequest_addSystemError(request, ERR_WARN, socerrno, YES, "accept");
	HTTRACE(PROT_TRACE, "HTDoAccept.. Accept failed\n");
	return HT_ERROR;
    }

    HTTRACE(PROT_TRACE, "Accepted.... socket %d\n" _ status);

    /* Remember the new socket we got and close the old one */
    NETCLOSE(HTNet_socket(accepting));
    HTNet_setSocket(accepting, status);	

    return HT_OK;
}


/*	HTDoListen
**	----------
**	Listens on the specified port. 
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_OK		if connected
*/
PUBLIC int HTDoListen (HTNet * listening, HTNet * accepting, int backlog)
{
    HTHost * host = HTNet_host(listening);
    HTRequest * request = HTNet_request(listening);
    int preemptive = listening->preemptive;
    int status = HT_OK;
    char * hostname = HTHost_name(host);

    /* Jump into the state machine */
    while (1) {
	switch (host->tcpstate) {
	case TCP_BEGIN:
	{
	    /*
	    ** Add the net object to the host object found above. If the
	    ** host is idle then we can start the request right away,
	    ** otherwise we must wait until it is free. 
	    */
	    if ((status = HTHost_addNet(host, accepting)) == HT_PENDING)
		HTTRACE(PROT_TRACE, "HTDoListen.. Pending...\n");

	    /*
	    ** If we are pending then return here, otherwise go to next state
	    ** which is setting up a channel
	    */
	    host->tcpstate = TCP_CHANNEL;
	    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_CHANNEL.\n" _ host);
	    if (status == HT_PENDING) return HT_PENDING;
	}
	break;

	case TCP_CHANNEL:
	    /*
	    **  The next state depends on whether we have a connection or not.
	    */
	    if (HTHost_channel(host) == NULL) {
		host->tcpstate = TCP_NEED_SOCKET;
		HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_SOCKET.\n" _ host);
	    } else {

		/*
		**  There is now one more using the channel
		*/
		HTChannel_upSemaphore(host->channel);

		/*
		**  We are now all set and can jump to connected mode
		*/
		host->tcpstate = TCP_CONNECTED;
		HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_CONNECTED.\n" _ host);
	    }
	    hostname = HTHost_name(host);
	    break;

	case TCP_NEED_SOCKET:
	{
	    SOCKET sockfd;

	    /* Create a new socket */
	    if ((sockfd = _makeSocket(host, request, preemptive)) == INVSOC) {
		host->tcpstate = TCP_ERROR;
		break;
	    }

	    /* Create channnel and streams */
	    createChannelAndTransportStreams (host, sockfd, accepting->transport);

	    /* Progress nofitication */
	    {
		HTAlertCallback *cbf = HTAlert_find(HT_PROG_ACCEPT);
		if (cbf) (*cbf)(request, HT_PROG_ACCEPT, HT_MSG_NULL,
				NULL, hostname, NULL);
	    }
	    host->tcpstate = TCP_NEED_BIND;
	    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_NEED_BIND\n" _ host);
	    break;
	}

	case TCP_NEED_BIND:
	    HTTRACE(PROT_TRACE, "Socket...... Binding socket %d\n" _ HTNet_socket(listening));
	    status = bind(HTNet_socket(listening), (struct sockaddr *) &host->sock_addr,
			  sizeof(host->sock_addr));
	    if (NETCALL_ERROR(status)) {
		HTTRACE(PROT_TRACE, "Socket...... Bind failed %d\n" _ socerrno);
		host->tcpstate = TCP_ERROR;		
	    } else {
		HTTRACE(PROT_TRACE, "Socket...... Starting listening on socket %d\n" _ HTNet_socket(listening));
		host->tcpstate = TCP_NEED_LISTEN;
	    }
	    break;

	case TCP_NEED_LISTEN:
	    status = listen(HTNet_socket(listening), backlog);
	    if (NETCALL_ERROR(status))
		host->tcpstate = TCP_ERROR;		
	    else
		host->tcpstate = TCP_CONNECTED;
	    break;

	case TCP_CONNECTED:
	    HTHost_unregister(host, listening, HTEvent_ACCEPT);
	    HTHost_setRetry(host, 0);
	    host->tcpstate = TCP_IN_USE;
	    HTTRACE(PROT_TRACE, "HTHost %p listening.\n" _ host);
	    return HT_OK;
	    break;

	    /* once a host is connected, subsequent connections are immediately OK */
	case TCP_IN_USE:
	    if ((status = HTHost_addNet(host, accepting)) == HT_PENDING) {
		HTTRACE(PROT_TRACE, "HTDoListen.. Pending...\n");
		return HT_PENDING;
	    }

	    HTChannel_upSemaphore(host->channel);
	    return HT_OK;

	case TCP_NEED_CONNECT:
	case TCP_DNS:
	case TCP_DNS_ERROR:
	case TCP_ERROR:
	    if (HTChannel_socket(host->channel) != INVSOC) {
		NETCLOSE(HTChannel_socket(host->channel));
		if (HTHost_isPersistent(host)) {	 /* Inherited socket */
		    HTHost_setPersistent(host, NO, HT_TP_SINGLE);
		    host->tcpstate = TCP_NEED_SOCKET;
		    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_NEED_SOCKET.\n" _ host);
		    break;
		}
	    }

	    HTRequest_addSystemError(request, ERR_FATAL, socerrno, NO, "accept");
	    HTHost_setRetry(host, 0);
	    host->tcpstate = TCP_BEGIN;
	    HTTRACE(PROT_TRACE, "HTHost %p going to state TCP_BEGIN.\n" _ host);
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
    if (net && HTNet_socket(net) != INVSOC) {
	HTTRACE(PROT_TRACE, "HTDoClose... Close %d\n" _ HTNet_socket(net));
	status = NETCLOSE(HTNet_socket(net));
	/*	HTEvent_unregister(HTNet_socket(net), (SockOps) FD_ALL); */
	HTNet_decreaseSocket();
 	HTNet_setSocket(net, INVSOC);
	
	/*
	**  As we have a socket available we check for whether
	**  we can start any pending requests. We do this by asking for
	**  pending Host objects. If none then use the current object
	*/
	HTHost_launchPending(net->host);

    } else
	HTTRACE(PROT_TRACE, "HTDoClose... No pending requests\n");
    return status < 0 ? HT_ERROR : HT_OK;
}


