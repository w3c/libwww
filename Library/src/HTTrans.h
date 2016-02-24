/*

  					W3C Sample Code Library libwww Transport Class


!
  The Transport Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Transport Class defines a transport as used by the
HTChannel class to communicate with the network,
the local file system etc. New transport objects may be registered at any
time. This allows the application to easily hook in its own transport layers.
The purpose of the HTTransport object is to contain transport dependent methods
for opening and closing a connection to the transport and also to get an
input stream and an output strean for reading and writing to the transport
respectively.

Note: The library core does not &nbsp;define any default transport
objects - they are all considered part of the application. The library comes
with a default set of transports which can be initiated using the function
HTTransportInit() in HTInit module

This module is implemented by HTTrans.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTTRANS_H
#define HTTRANS_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Creation and Deletion Methods
.

All transport interfaces are registered dynamically in libwww. This means
that libwww is independent of the transport being used (TCP, for example),
and you can therefore use libwww in any context you like. You have to specify
a set of parameters in order for libwww to be able to use it. The transport
class is defined as follows:
*/

typedef struct _HTTransport HTTransport;

typedef enum _HTTransportMode {
    HT_TP_SINGLE	= 0,		/* One single request at a time */
    HT_TP_PIPELINE	= 1,		/* Use pipelined requests */
    HT_TP_INTERLEAVE	= 2		/* Can we interleave requests? */
} HTTransportMode;

#include "HTIOStream.h"
#include "HTReq.h"

/*
(
  Add a Transport
)

A new transport can be registered at any time in the Library. You must specify
a name ad the supported channel mode that the transport supports. Then you
must also register two creation methods of an input and an output stream
respectively. You can find the definition of the I/O streams in the
HTIOStream module.
*/

extern BOOL HTTransport_add (const char *		name,
			     HTTransportMode		mode,
			     HTInput_new *		get_input,
			     HTOutput_new *		get_output);

/*
(
  Delete a Transport
)

This functions deletes a registered protocol module so that it can not be
used for accessing a resource anymore.
*/

extern BOOL HTTransport_delete (const char * name);

/*
(
  Remove ALL Registered Transports
)

This is the garbage collection function. It is called by
HTLibTerminate()
*/

extern BOOL HTTransport_deleteAll (void);

/*
.
  Transport Class Methods
.
(
  Find a Transport Protocol Object
)

You can search the list of registered protocol objects as a function of the
access acheme. If an access scheme is found then the protocol object is returned.
*/

extern HTTransport * HTTransport_find (HTRequest * request, const char * name);

/*
(
  Supported Transort Modes
)

A transport object is registered with the flow control
mode that it supports. This mode describes whether we can issue multiple
requests at the same time.
*/

extern HTTransportMode HTTransport_mode (HTTransport * tp);
extern BOOL HTTransport_setMode (HTTransport * tp, HTTransportMode mode);

/*
(
  Input and Output Stream Creation Methods
)
*/

struct _HTTransport {
    char *		name;
    HTTransportMode	mode;			      /* Flow mode supported */
    HTInput_new *	input_new; 	     /* Input stream creation method */
    HTOutput_new *	output_new;	    /* Output stream creation method */
};

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTTRANS_H */

/*

  

  @(#) $Id$

*/
