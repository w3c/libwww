/*

  					W3C Sample Code Library Socket Reader Stream


!
  Demultiplexing Stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The MUX Reader Stream is an input stream
&nbsp;which knows how to demultiplex a MUX channel. It is part of the
Transport interface and may be registered as
part of a Transport Object. The application
can&nbsp;initialize this stream together with the
HTMuxTx stream, for example. In the
default initialization module, you can find the
HTTransportInit() function which sets up this stream as a default
transport for handling socket read operations.

This module is implemented by HTMuxRx.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTMUXRX_H
#define HTMUXRX_H

#ifdef __cplusplus
extern "C" { 
#endif 

#include "HTStream.h"
#include "HTMuxCh.h"

/*

*/

extern HTStream * HTDemux_new (HTHost * host, HTMuxChannel * muxch);

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTMUXRX_H */

/*

  

  @(#) $Id$

*/
