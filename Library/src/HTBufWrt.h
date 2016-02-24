/*

  
  
  					W3C Sample Code Library libwww Buffered Socket Writer Stream


!
  Buffered Socket Writer Stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Buffered Socket Writer Stream is an output
stream &nbsp;which knows how to write to a BSD type socket. It is part
of the Transport interface and may be registered
as part of a Transport Object. The application
can&nbsp;initialize this stream together with the
HTReader stream, for example. In the
default initialization module, you can find the
HTTransportInit() function which sets up this stream as a default
transport for handling unbuffered socket write operations.

The difference from the unbuffered write stream
is that this stream lets you write characters without causing a
write system call every time. The data is first written into
a buffer. Data is written to the transport only when the buffer is full,
or when the stream is flushed.

This module is implemented by HTBufWrt.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTBUFWRT_H
#define HTBUFWRT_H

#include "HTIOStream.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Default Output Buffering
.

In order to optimize reading a channel, we bind a buffer to each channel
object. The size of this buffer is a compromise between speed and memory.
By default, we have chosen a value frequently used in TCP connections. In
the stream creation method you can pass any other buffer size. In the case
of 0, we take the default size.
*/

#define OUTPUT_BUFFER_SIZE 1024

/*
.
  Buffered Write Stream
.
*/

extern HTOutput_new HTBufferWriter_new;

/*
.
  Buffered Write Converter Stream
.
*/

extern HTOutputConverter_new HTBufferConverter_new;

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTBUFWRT_H */

/*

  

  @(#) $Id$

*/
