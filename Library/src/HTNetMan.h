/*

  
  					W3C Sample Code Library libwww Private Net Definition


!
  Private Net Definition
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is the private definition of the Net Class. Please look in the public
Net Class for more documentation

This module is implemented by HTNet.c, and it is a
part of the W3C Sample Code Library.
*/

#ifndef HTNETMAN_H
#define HTNETMAN_H
#include "HTNet.h"
#include "HTDNS.h"
#include "HTEvent.h"
#include "HTProt.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*

The HTNet object is the core of the request queue management.
This object contains information about the socket descriptor, the input read
buffer etc. required to identify and service a request.
*/

struct _HTNet {
    int                 hash;                                  /* Hash value */

    /* Link to other objects */
    HTRequest *		request;		   /* Link to request object */
    HTHost *		host;	       /* What we know about the remote host */
    HTProtocol *	protocol;		 /* Protocol to this request */
    HTTransport *	transport;	       /* Transport for this request */
    int                 session;

    /* For progress notifications */
    BOOL		countRawBytes;	     /* If we should count raw bytes */
    long		bytesRead;			    /* Bytes in body */
    long		headerBytesRead;	     /* Bytes read in header */
    long		bytesWritten;		 /* Bytes written to network */
    long		headerBytesWritten;	  /* Bytes written in header */

#if 0
    int			retry;		     /* Counting attempts to connect */
    int 		home;			 /* Current home if multiple */
#endif

    time_t		connecttime;		 /* Used on multihomed hosts */
    BOOL		preemptive;  /* Eff result from Request and Protocol */

    HTEvent		event;
    HTStream *		readStream;    /* host's input stream puts data here */

    /* User specific stuff */
    void *		context;		/* Protocol Specific context */

    /* Eric's sleezoid cheat - should go to extra pipeline object */
    HTEventType		registeredFor;
};

extern SOCKET HTNet_socket(HTNet * me);


/*
.
  Bytes Read Stats
.
(
  Total Bytes Read
)
*/

#define HTNet_setBytesRead(me,l)	  ((me) ? (me->bytesRead=(l)) : -1)
#define HTNet_bytesRead(me)		  ((me) ? (me)->bytesRead : -1)
#define HTNet_addBytesRead(me,l)          ((me) ? (me->bytesRead+=(l)) : -1)

/*
(
  Header Bytes Read
)
*/

#define HTNet_setHeaderBytesRead(me,l)	  ((me) ? (me->headerBytesRead=(l)) :-1)
#define HTNet_headerBytesRead(me)	  ((me) ? (me)->headerBytesRead : -1)
#define HTNet_addHeaderBytesRead(me,l)    ((me) ? (me->headerBytesRead+=(l)) : -1)

/*
.
  Bytes Written Stats
.
(
  Total Bytes Written
)
*/

#define HTNet_setBytesWritten(me,l)	  ((me) ? (me->bytesWritten=(l)) :-1)
#define HTNet_bytesWritten(me)		  ((me) ? (me)->bytesWritten : -1)
#define HTNet_addBytesWritten(me,l)       ((me) ? (me->bytesWritten+=(l)) : -1)

/*
(
  Header Bytes Written
)
*/

#define HTNet_setHeaderBytesWritten(me,l) ((me) ? (me->headerBytesWritten=(l)) :-1)
#define HTNet_headerBytesWritten(me)  ((me) ? \
                                       ((me)->headerBytesWritten==0 ? \
                                        HTNet_bytesWritten(me) : \
                                        (me)->headerBytesWritten) : -1)
#define HTNet_addHeaderBytesWritten(me,l) ((me) ? (me->headerBytesWritten+=(l)) : -1)

/*
.
  Event Callbacks
.
*/

extern BOOL HTNet_setEventParam(HTNet * net, void * eventParam);
extern void* HTNet_eventParam(HTNet * net);
extern BOOL HTNet_setEventCallback(HTNet * net, HTEventCallback * cbf);
extern HTEventCallback * HTNet_eventCallback(HTNet * net);
extern BOOL HTNet_setEventPriority(HTNet * net, HTPriority priority);
extern HTPriority HTNet_eventPriority(HTNet * net);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTNETMAN_H */

/*

  

  @(#) $Id$

*/
