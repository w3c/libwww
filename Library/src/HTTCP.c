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
#include "WWWMux.h"

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
/* x seconds penalty on a multi-homed host if IP-address is down */
#define TCP_PENALTY		1200

/* x seconds penalty on a multi-homed host if IP-address is timed out */
#define TCP_DELAY		600

/* imperical study in socket call error codes
 */
#ifdef _WINSOCKAPI_					/* windows */
#define NETCALL_ERROR(ret)	(ret == SOCKET_ERROR)
#define NETCALL_DEADSOCKET(err)	(err == WSAEBADF)
#define NETCALL_WOULDBLOCK(err)	(err == WSAEWOULDBLOCK)
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
/* ------------------------------------------------------------------------- */
/*	       	      CONNECTION ESTABLISHMENT MANAGEMENT 		     */
/* ------------------------------------------------------------------------- */

/* _makeSocket - create a socket, if !preemptive, set FIONBIO
 * returns 1: blocking
 *	   0: non-blocking
 *	  -1: creation error
 */
PRIVATE int _makeSocket(HTHost * host, HTRequest * request, int preemptive, HTTransport * transport)
{
    int status = 1;
    SOCKET sockfd;
#ifdef DECNET
    if ((sockfd=socket(AF_DECnet, SOCK_STREAM, 0))==INVSOC)
#else
    if ((sockfd=socket(AF_INET, SOCK_STREAM,IPPROTO_TCP))==INVSOC)
#endif
	{
	HTRequest_addSystemError(request, ERR_FATAL, socerrno, 
				 NO, "socket");
	host->tcpstate = TCP_ERROR;
	return -1;
	}
    if (PROT_TRACE) HTTrace(HTHIDE("Socket...... Created %d\n"), sockfd);

    /* Increase the number of sockets by one */
    HTNet_increaseSocket();

    /*
    **  If we have compiled without Nagle's algorithm then try and turn
    **  it off now
    */
#ifdef HT_NO_NAGLE
    {
	int disable = 1;
	status = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
			    (char *) &disable, sizeof(int));
	if (status == -1) {
	    if (PROT_TRACE)
		HTTrace("Socket...... Could not disable Nagle's algorithm - error %d\n",
			sockfd);
	} else {
	    if (PROT_TRACE) HTTrace("Socket...... Turned off Nagle's algorithm\n");
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
	if (PROT_TRACE) HTTrace(HTHIDE("Socket...... %slocking socket\n"), status == -1 ? "B" : "Non-b");
    } else
	if (PROT_TRACE) HTTrace(HTHIDE("Socket...... Blocking socket\n"));

    /*
    **  Associate the channel with the host and create an input and and output stream
    **  for this host/channel
    */
    HTHost_setChannel(host, HTChannel_new(sockfd, NULL, YES));
    HTHost_getInput(host, transport, NULL, 0);
    HTHost_getOutput(host, transport, NULL, 1024);

    return status == -1 ? 1 : 0;
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
PUBLIC int HTDoConnect (HTNet * net, char * url, u_short default_port)
{
    HTHost * me = HTNet_host(net);
    HTRequest * request = HTNet_request(net);
    int preemptive = net->preemptive;
    int status = HT_OK;
    char * hostname = HTHost_name(me);

    /* Jump into the state machine */
    if (!me) {
	char * proxy = HTRequest_proxy(request);
	HTProtocol * protocol = HTNet_protocol(net);

	/* Check to see whether we connect directly or via a proxy */
	if ((me = HTHost_newWParse(request, proxy ? proxy : url, 
				   HTProtocol_id(protocol))) == NULL)
	    return NO;

	net->host = me;
    }
    while (1) {
	switch (me->tcpstate) {
	  case TCP_BEGIN:
	  {
	      /*
	      ** Add the net object to the host object found above. If the
	      ** host is idle then we can start the request right away,
	      ** otherwise we must wait until it is free. 
	      */
	      if ((status = HTHost_addNet(net->host, net)) == HT_PENDING)
		  if (PROT_TRACE) HTTrace("HTDoConnect. Pending...\n");

	      /*
	      ** If we are pending then return here, otherwise go to next state
	      ** which is setting up a channel
	      */
	      me->tcpstate = TCP_CHANNEL;
	      if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_CHANNEL.\n"), me);
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
	    if (HTHost_channel(me) == NULL) {
		me->tcpstate = TCP_DNS;
		if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_DNS.\n"), me);
	    } else {

		/*
		**  Get a new session on the MUX layer - we already have a
		**  connection.
		*/
#ifdef HT_MUX
		HTMuxChannel * muxch = HTMuxChannel_find(me);
		HTProtocol * protocol = HTNet_protocol(net);
		net->session = HTMuxSession_connect(muxch, net, HTProtocol_id(protocol));
#endif /* HT_MUX */

		/*
		**  There is now one more using the channel
		*/
		HTChannel_upSemaphore(me->channel);

		/*
		**  We are now all set and can jump to connected mode
		*/
		me->tcpstate = TCP_CONNECTED;
		if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_CONNECTED.\n"), me);
	    }
	    hostname = HTHost_name(me);
	    break;

	case TCP_DNS:
	    if ((status = HTParseInet(me, hostname, request)) < 0) {
		if (PROT_TRACE) HTTrace(HTHIDE("HTDoConnect. Can't locate `%s\'\n"), hostname);
		HTRequest_addError(request, ERR_FATAL, NO,
				   HTERR_NO_REMOTE_HOST,
				   (void *) hostname, strlen(hostname),
				   "HTDoConnect");
		me->tcpstate = TCP_ERROR;
		if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_ERROR.\n"), me);
		break;
	    }
	    if (!HTHost_retry(me) && status > 1)		/* If multiple homes */
		HTHost_setRetry(me, status);
	    me->tcpstate = TCP_NEED_SOCKET;
	    if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_NEED_SOCKET.\n"), me);
	    break;

	  case TCP_NEED_SOCKET:
	    if (_makeSocket(me, request, preemptive, net->transport) == -1)
		break;

#ifdef HT_MUX
	    /*
	    **  Create a MUX channel and do a connect on this channel with a
	    **  new session.
	    */
	    {
		HTProtocol * protocol = HTNet_protocol(net);
		HTMuxChannel * muxch = HTMuxChannel_new(me);
		net->session = HTMuxSession_connect(muxch, net, HTProtocol_id(protocol));
	    }
#endif /* HT_MUX */

	    /* If multi-homed host then start timer on connection */
	    if (HTHost_retry(me)) me->connecttime = HTGetTimeInMillis();

	    /* Progress */
	    {
		HTAlertCallback *cbf = HTAlert_find(HT_PROG_CONNECT);
		if (cbf) (*cbf)(request, HT_PROG_CONNECT, HT_MSG_NULL,
				NULL, hostname, NULL);
	    }
	    me->tcpstate = TCP_NEED_CONNECT;
	    if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_NEED_CONNECT.\n"), me);
	    break;
	  case TCP_NEED_CONNECT:
	    status = connect(HTChannel_socket(me->channel), (struct sockaddr *) &me->sock_addr,
			     sizeof(me->sock_addr));
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
		    if (PROT_TRACE) HTTrace(HTHIDE("HTDoConnect. WOULD BLOCK `%s'\n"), hostname);
		    HTHost_register(me, net, HTEvent_CONNECT);
		    return HT_WOULD_BLOCK;
		}
		if (socerrno == EISCONN) {
		    me->tcpstate = TCP_CONNECTED;
		    if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_CONNECTED.\n"), me);
		    break;
		}
		if (NETCALL_DEADSOCKET(socerrno))     /* We lost the socket */
		{
		    me->tcpstate = TCP_NEED_SOCKET;
		    if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_NEED_SOCKET.\n"), me);
		    break;
		}
		if (HTHost_retry(me)) {
		    me->connecttime -= HTGetTimeInMillis();
		    /* Added EINVAL `invalid argument' as this is what I 
		       get back from a non-blocking connect where I should 
		       get `connection refused' on BSD. SVR4 gives SIG_PIPE */
#if defined(__svr4__) || defined (_WINSOCKAPI_)
		    if (socerrno==ECONNREFUSED || socerrno==ETIMEDOUT ||
			socerrno==ENETUNREACH || socerrno==EHOSTUNREACH ||
			socerrno==EHOSTDOWN)
#else
		    if (socerrno==ECONNREFUSED || socerrno==ETIMEDOUT ||
			socerrno==ENETUNREACH || socerrno==EHOSTUNREACH ||
			socerrno==EHOSTDOWN || socerrno==EINVAL)
#endif
		        me->connecttime += TCP_DELAY;
		    else
		        me->connecttime += TCP_PENALTY;
		    HTDNS_updateWeigths(me->dns, HTHost_home(me), me->connecttime);
		}
		me->tcpstate = TCP_ERROR;		
		if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_ERROR.\n"), me);
	    } else {
		me->tcpstate = TCP_CONNECTED;
		if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_CONNECTED.\n"), me);
	    }
	    break;

	  case TCP_CONNECTED:
	    HTHost_unregister(me, net, HTEvent_CONNECT);
	    if (HTHost_retry(me)) {
		me->connecttime -= HTGetTimeInMillis();
		HTDNS_updateWeigths(me->dns, HTHost_home(me), me->connecttime);
	    }
	    HTHost_setRetry(me, 0);
	    me->tcpstate = TCP_IN_USE;
	    if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p connected.\n"), me);
	    return HT_OK;
	    break;

	  /* once a host is connected, subsequent connections are immediately OK */
	  case TCP_IN_USE:
	      if ((status = HTHost_addNet(net->host, net)) == HT_PENDING) {
		  if (PROT_TRACE) HTTrace("HTDoConnect. Pending...\n");
		  return HT_PENDING;
	      }

	      HTChannel_upSemaphore(me->channel);
	      return HT_OK;

	  case TCP_NEED_BIND:
	  case TCP_NEED_LISTEN:
	  case TCP_ERROR:
	    HTTrace(HTHIDE("HTDoConnect. Connect failed %d\n"), socerrno);
	    if (HTChannel_socket(me->channel) != INVSOC) {
	      /*	        HTEvent_unregister(HTChannel_socket(me->channel), (SockOps) FD_ALL); */
		NETCLOSE(HTChannel_socket(me->channel));
		/*		HTChannel_setSocket(me->channel, INVSOC); */
#if 1 /* @@@ */
		if (HTHost_isPersistent(me)) {	 /* Inherited socket */
		    HTHost_setPersistent(me, NO, HT_TP_SINGLE);
		    me->tcpstate = TCP_NEED_SOCKET;
		    if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_NEED_SOCKET.\n"), me);
		    break;
		}
#endif
	    }

	    /* Do we have more homes to try? */
	    HTHost_decreaseRetry(me);
	    if (HTHost_retry(me) > 0) {
	        HTRequest_addSystemError(request, ERR_NON_FATAL, socerrno, NO,
			      "connect");
		me->tcpstate = TCP_DNS;
		if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_DNS.\n"), me);
		break;
	    }
	    HTRequest_addSystemError(request, ERR_FATAL,socerrno,NO,"connect");
	    HTDNS_delete(hostname);
	    HTHost_setRetry(me, 0);
	    me->tcpstate = TCP_BEGIN;
	    if (PROT_TRACE) HTTrace(HTHIDE("HTHost %p going to state TCP_BEGIN.\n"), me);
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
    int size = sizeof(net->host->sock_addr);
    HTRequest * request = HTNet_request(net);
    if (!request || HTNet_socket(net)==INVSOC) {
	if (PROT_TRACE) HTTrace("HTDoAccept.. Invalid socket\n");
	return HT_ERROR;
    }

    /* Progress report */
    {
	HTAlertCallback *cbf = HTAlert_find(HT_PROG_ACCEPT);
	if (cbf) (*cbf)(request, HT_PROG_ACCEPT, HT_MSG_NULL,NULL, NULL, NULL);
    }
    status = accept(HTNet_socket(net), (struct sockaddr *) &net->host->sock_addr, &size);
    if (NETCALL_ERROR(status))
    {
	if (NETCALL_WOULDBLOCK(socerrno))
	{
	    if (PROT_TRACE)
		HTTrace("HTDoAccept.. WOULD BLOCK %d\n", HTNet_socket(net));
	    HTEvent_register(HTNet_socket(net), HTEvent_ACCEPT, &net->event);
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
    HTNet_setSocket(*accepted, status);	

    /* Create a channel for the new socket */
    {
	HTHost * host = (*accepted)->host;
	HTChannel * ch = HTChannel_new(HTNet_socket(*accepted), NULL, NO);
#ifdef HT_MUX
	HTMuxChannel_new(host);
#endif /* HT_MUX */
	HTHost_setChannel(host, ch);
    }
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
	switch (net->host->tcpstate) {
	  case TCP_BEGIN:
	    {
		SockA *sin = &net->host->sock_addr;
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
			net->host->tcpstate = TCP_ERROR;
			break;
		    }
		} else
		    sin->sin_addr.s_addr = INADDR_ANY;
		sin->sin_port = htons(port);
#endif
	    }
	    if (PROT_TRACE)
		HTTrace("Socket...... Listen on port %d\n", port);
	    net->host->tcpstate = TCP_NEED_SOCKET;
	    break;

	  case TCP_NEED_SOCKET:
	    if (_makeSocket(net->host, net->request, net->preemptive, net->transport) == -1)
	        break;
	    net->host->tcpstate = TCP_NEED_BIND;
	    break;

	  case TCP_NEED_BIND:
	    status = bind(HTNet_socket(net), (struct sockaddr *) &net->host->sock_addr,
			  sizeof(net->host->sock_addr));
	    if (NETCALL_ERROR(status))
	    {
		if (PROT_TRACE)
		    HTTrace("Socket...... Bind failed %d\n", socerrno);
		net->host->tcpstate = TCP_ERROR;		
	    } else
		net->host->tcpstate = TCP_NEED_LISTEN;
	    break;

	  case TCP_NEED_LISTEN:
	    status = listen(HTNet_socket(net), backlog);
	    if (NETCALL_ERROR(status))
		net->host->tcpstate = TCP_ERROR;		
	    else
		net->host->tcpstate = TCP_CONNECTED;
	    break;

	  case TCP_CONNECTED:
	    net->host->tcpstate = TCP_BEGIN;
	    if (PROT_TRACE)
		HTTrace("Socket...... Bind and listen on port %d %s\n",
			(int) ntohs(net->host->sock_addr.sin_port),
			HTInetString(&net->host->sock_addr));
	    return HT_OK;
	    break;

	  case TCP_CHANNEL:
	  case TCP_NEED_CONNECT:
	  case TCP_DNS:
	  case TCP_ERROR:
	    if (PROT_TRACE) HTTrace("Socket...... Listen failed\n");
	    HTRequest_addSystemError(net->request, ERR_FATAL, socerrno, NO, "HTDoListen");
	    net->host->tcpstate = TCP_BEGIN;
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
	if (PROT_TRACE) HTTrace("HTDoClose... Close %d\n", HTNet_socket(net));
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
	if (PROT_TRACE) HTTrace("HTDoClose... No pending requests\n");
    return status < 0 ? HT_ERROR : HT_OK;
}


