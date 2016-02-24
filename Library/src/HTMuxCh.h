/*

  					W3C Sample Code Library libwww MUX Protocol


!
  The MUX Protocol
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module defines the read and write functions to and from the network.
As we are having reentrant function and a smarter network I/O this will get
very small :-)

This module is implemented by HTMux.c, and it is a
part of the  W3C Sample Code
Library.
*/

#ifndef HTMUXCHANNEL_H
#define HTMUXCHANNEL_H

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTMuxChannel     HTMuxChannel;
typedef struct _HTMuxSession     HTMuxSession;
typedef struct _HTMuxProtocol    HTMuxProtocol;

typedef unsigned char            HTMuxSessionId;

#ifdef __cplusplus
}
#endif

#include "HTMuxHeader.h"
#include "HTChannl.h"
#include "HTHost.h"

#ifdef __cplusplus
extern "C" { 
#endif 

#define INVSID		0		/* Invalid session id */
#define INVPID          0               /* Invalid protocol id */

/*
.
  Mux Channel
.
*/

extern HTMuxChannel * HTMuxChannel_new (HTHost * host);

extern HTMuxChannel * HTMuxChannel_find (HTHost * host);

extern BOOL HTMuxChannel_delete (HTMuxChannel * me);

extern BOOL HTMuxChannel_deleteAll (void);

extern HTNet * HTMuxChannel_net (HTMuxChannel * me);

extern HTMuxSession * HTMuxChannel_findSession (HTMuxChannel * me, HTMuxSessionId sid);

extern HTMuxSession * HTMuxChannel_findSessionFromNet (HTMuxChannel * me,
						       HTNet * net);

extern HTHost * HTMuxChannel_host (HTMuxChannel * muxch);

extern int HTMuxChannel_sendControl (HTMuxChannel * muxch, HTMuxSessionId sid,
				     HTMuxHeader opcode, int value,
				     const void * param, int param_size);


/*
.
  Mux Session
.
*/

typedef enum _HTMuxClose {
    MUX_S_END_READ     = 0x1,
    MUX_S_END_WRITE    = 0x2,
    MUX_S_END          = 0x3
} HTMuxClose;

extern HTMuxSessionId HTMuxSession_accept (HTMuxChannel * muxch, HTNet * net,
					   HTProtocolId pid);

extern HTMuxSessionId HTMuxSession_connect (HTMuxChannel * muxch, HTNet * net,
					    HTProtocolId pid);

extern int HTMuxSession_close (HTMuxChannel * muxch, HTMuxSessionId sid);

extern HTMuxSession * HTMuxSession_register (HTMuxChannel * muxch,
				             HTMuxSessionId sid,
                                             HTProtocolId pid);

extern HTMuxSessionId HTMuxSession_id (HTMuxSession * session);

extern HTProtocolId HTMuxSession_pid (HTMuxSession * session);

extern HTNet * HTMuxSession_net (HTMuxSession * session);

extern BOOL HTMuxSession_setClose (HTMuxChannel * muxch,
				   HTMuxSession * session, HTMuxClose close);

extern int  HTMuxSession_credit (HTMuxSession * session);

extern BOOL HTMuxSession_setCredit (HTMuxChannel * muxch,
				    HTMuxSessionId sid, int credit);

extern int  HTMuxSession_fragment (HTMuxSession * session);

extern BOOL HTMuxSession_setFragment (HTMuxChannel * muxch,
				      HTMuxSessionId sid, int fragment);

extern int HTMuxSession_disposeData (HTMuxSession * me,
				     const char * buf, int len);

/*
.
  Mux Protocol
.
*/

extern BOOL HTMuxProtocol_add (HTMuxChannel * muxch,
			       HTProtocolId pid, const char * protocol);

extern BOOL HTMuxProtocol_delete (HTMuxChannel * muxch, HTProtocolId pid);

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTMUXCHANNEL_H */

/*

  

  @(#) $Id$

*/
