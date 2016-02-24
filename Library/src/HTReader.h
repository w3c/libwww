/*

  
  
  					W3C Sample Code Library Socket Reader Stream


!
  Socket Reader Stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Socket Reader Stream is an input stream
&nbsp;which knows how to read from&nbsp;a BSD type socket. It is part
of the Transport interface and may be registered
as part of a Transport Object. The application
can&nbsp;initialize this stream together with the
HTWriter stream, for example. In the
default initialization module, you can find the
HTTransportInit() function which sets up this stream as a default
transport for handling socket read operations.

This module is implemented by HTReader.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTREADER_H
#define HTREADER_H

#include "HTIOStream.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Input Buffering
.

In order to optimize reading a channel, we bind a buffer to each channel
object. The size of this buffer is a compromise between speed and memory.
Here it is chosen as the default TCP High Water Mark (sb_hiwat) for receiving
data. By default, we have chosen a value that equals the normal TCP High
Water Mark (sb_hiwat) for receiving data.
*/

#define INPUT_BUFFER_SIZE    32*1024

/*
.
  Read Stream
.
*/

extern HTInput_new HTReader_new;

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTREADER_H */

/*

  

  @(#) $Id$

*/
