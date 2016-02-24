/*

  					W3C Sample Code Library libwww SSL Writer Stream


!
  SSL Writer Stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The SSL Stream is an output stream which
knows how to write to a SSL socket layer, for example provided by the
OpenSSL Library. It is a libwww
transport and may be registered using the
Transport Manager. The application
can&nbsp;initialize this stream together with the
HTSSLReader stream, for example.

This module is implemented by HTSSLWriter.c,
and it is a part of the W3C Sample Code
Library.

The module is contributed by Olga Antropova
*/

#ifndef HTSSLWRITE_H
#define HTSSLWRITE_H

#include "HTIOStream.h"

#ifdef __cplusplus
extern "C" { 
#endif 

extern HTOutput_new HTSSLWriter_new;

extern BOOL HTSSLWriter_set (HTOutputStream *      me,
                             HTNet *               net,
                             HTChannel *           ch,
                             void *                param,
                             int                   mode);


#ifdef __cplusplus
}
#endif

#endif /* HTSSLWRITE_H */

/*

  

  @(#) $Id$

*/
