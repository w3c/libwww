/* 

  
  					W3C Sample Code Library libwww HTTP Client


!
  Multi Threaded HyperText Tranfer Protocol Client Module
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is the HTTP client module. This is actually a very small definition
file as almost everything is set up elsewhere.

This module is implemented by HTTP.c, and it is a part
of the W3C Sample Code Library.
*/

#ifndef HTTP_H
#define HTTP_H

#include "HTProt.h"
#include "HTStream.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
(
  HTTP Client Connection Mode
)

The HTTP client module supports various modes for communicating with HTTP
servers. The mode are defined by the enumeration below.
*/

typedef enum _HTTPConnectionMode { 
    HTTP_11_PIPELINING     = 0x1,
    HTTP_11_NO_PIPELINING  = 0x2, 
    HTTP_11_MUX            = 0x4,
    HTTP_FORCE_10          = 0x8
} HTTPConnectionMode; 

extern void HTTP_setConnectionMode (HTTPConnectionMode mode);
extern HTTPConnectionMode HTTP_connectionMode (void);

/*
(
  HTTP Write Delay of Content Bodies
)

Because of the differences between HTTP/1.0 and HTTP/1.1, HTTP PUT
and POST requests can not be dealt with in non-preemptive mode -
they have to use timers and hence must be in preemptive mode.

The default write mechanism used is first to write the headers and then wait
for a period of time before writing the body. This often allows the server
to respond before we start sending data accross the wire. If the write for
some reason fails in a bad way then we try again, waiting a little bit longer
this time as this may give the server enough time to think.

The default wait periods are 2000ms for the first wait and 3000ms if that
fails. These are rather conservative values but can be changed by using these
functions. The second try value must be larger (or equal) to the first try
value and the first try value must be larger than 20 ms.
*/
extern BOOL HTTP_setBodyWriteDelay (ms_t first_try, ms_t second_try);
extern void HTTP_bodyWriteDelay (ms_t * first_try, ms_t * second_try);

/*
(
  HTTP Event Handler
)

The event handler is the actual HTTP client state machine taking care of
the communication.
*/

extern HTProtCallback HTLoadHTTP;
extern HTConverter HTTPStatus_new;

#ifdef __cplusplus
}
#endif

#endif /* HTTP_H */

/*

  

  @(#) $Id$

*/
