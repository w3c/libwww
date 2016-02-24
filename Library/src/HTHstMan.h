/*

  					W3C Sample Code Library libwww Private Hst Definition


!
  Private Host Definition
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is the private definition of the Host Class. Please look in the public
Host Class for more documentation

This module is implemented by HTHost.c, and it is
a part of the W3C Sample Code Library.
*/

#ifndef HTHSTMAN_H
#define HTHSTMAN_H
#include "HTHost.h"
#include "HTDNS.h"
#include "HTEvent.h"
#include "HTProt.h"

#ifdef __cplusplus
extern "C" { 
#endif 

#define PIPE_BUFFER_SIZE	8192

/*

The HTHost object is the core of the request queue management.
This object contains information about the socket descriptor, the input read
buffer etc. required to identify and service a request.
*/

typedef enum _TCPState {
    TCP_DNS_ERROR	= -3,
    TCP_ERROR		= -2,
    TCP_CONNECTED	= -1,
    TCP_BEGIN		= 0,
    TCP_CHANNEL,
    TCP_DNS,
    TCP_NEED_SOCKET,
    TCP_NEED_BIND,
    TCP_NEED_LISTEN,
    TCP_NEED_CONNECT,
    TCP_IN_USE
} TCPState;

struct _HTHost {
    int                 hash;

    /* Information about the otherend */
    char *  		hostname;	     /* name of host + optional port */
    u_short		u_port;
    time_t		ntime;				    /* Creation time */
    char *		type;				        /* Peer type */
    int 		version;			     /* Peer version */
    HTMethod		methods;	       	/* Public methods (bit-flag) */
    char *		server;				      /* Server name */
    char *		user_agent;			       /* User Agent */
    char *		range_units;			       	      /* ??? */

    /* When does this entry expire? */
    time_t		expires;	  /* Persistent channel expires time */
    int			reqsPerConnection;	  /* from Keep-Alive: header */
    int			reqsMade;		 /* updated as they are sent */

    /* Queuing and connection modes */
    HTList *		pipeline;		 /* Pipe line of net objects */
    HTList *		pending;	      /* List of pending Net objects */
    HTNet *             doit;               /* Transfer from pending to pipe */ 
    HTNet *             lock;             /* This is a kludge! */
    HTNet *		listening;	 /* Master for accepting connections */
    BOOL		persistent;
    HTTransportMode	mode;	      			   /* Supported mode */
    HTTimer *           timer;         /* Timer for handling idle connection */
    BOOL                do_recover;         /* If we are supposed to recover */
    int                 recovered;        /* How many times had we recovered */
    BOOL                close_notification;        /* Got a hint about close */
    BOOL                broken_pipe;

    /* Support for transports */
    HTChannel *		channel;			     /* data channel */

    /* Connection dependent stuff */
    HTdns *		dns;			       /* Link to DNS object */
    TCPState		tcpstate;		      /* State in connection */
    SockA 		sock_addr;	     /* SockA is defined in wwwsys.h */
    int			retry;		     /* Counting attempts to connect */
    int 		home;			 /* Current home if multiple */
    ms_t		connecttime;	   /* Time in ms on multihomed hosts */

    /* Event Management */
    HTEvent *		events[HTEvent_TYPES];/* reading and writing may differ */
    HTEventType	        registeredFor;	  /* Which actions are we blocked on */
    size_t		remainingRead;	 /* Tells HostEvent to call next net */

    /* User specific stuff */
    ms_t                delay;                          /* Write delay in ms */
    void *		context;		/* Protocol Specific context */
    int			forceWriteFlush;
    int                 inFlush;         /* Tells if we're currently processing
                                            a file flush */
};

#define HTHost_bytesRead(me)		((me) ? (me)->bytes_read : -1)
#define HTHost_bytesWritten(me)		((me) ? (me)->bytes_written : -1)

#define HTHost_setBytesRead(me,l)	((me) ? (me->bytes_read=(l)) : -1)
#define HTHost_setBytesWritten(me,l)	((me) ? (me->bytes_written=(l)) :-1)
#define HTHost_setDNS (host, dns)	((me) ? (me->dns=(dns)) :-1)

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTHSTMAN_H */

/*

  

  @(#) $Id$

*/
