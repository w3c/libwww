/*

  
  					W3C Sample Code Library libwww Unbuffered Socket Writer Stream


!
  Unbuffered Socket Writer Stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Socket Writer Stream is an output stream
&nbsp;which knows how to write to a BSD type socket. It is part of the
Transport interface and may be registered as
part of a Transport Object. The application
can&nbsp;initialize this stream together with the
HTReader stream, for example. In the
default initialization module, you can find the
HTTransportInit() function which sets up this stream as a default
transport for handling unbuffered socket write operations. See also the
buffered writer stream.

This module is implemented by HTWriter.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTWRITE_H
#define HTWRITE_H

#include "HTIOStream.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
*/
extern HTOutput_new HTWriter_new;

extern BOOL HTWriter_set (HTOutputStream *	me,
			  HTNet *		net,
			  HTChannel *		ch,
			  void *		param,
			  int			mode);


/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTWRITE_H */

/*

  

  @(#) $Id$

*/
