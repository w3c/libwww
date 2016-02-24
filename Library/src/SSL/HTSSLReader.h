/*

  					W3C Sample Code Library SSL Reader Stream


!
  SSL Reader Stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The SSL Reader Stream is an input
stream&nbsp;which knows how to read from&nbsp;a SSL socket layer, for
example provided by the OpenSSL Library.
It is a libwww transport and may be registered
using the Transport Manager. The application
can&nbsp;initialize this stream together with the
HTSSLWriter stream, for example. This module
requires a SSL library in order to link/compile.

This module is implemented by HTSSLReader.c,
and it is a part of the  W3C Sample
Code Library.

The module is contributed by Olga Antropova
*/

#ifndef HTSSLREADER_H
#define HTSSLREADER_H

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
  SSL Read Stream
.
*/

extern HTInput_new HTSSLReader_new;

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTSSLREADER_H */

/*

  

  @(#) $Id$

*/
