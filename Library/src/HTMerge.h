/*

  
  					W3C Sample Code Library libwww Merge Stream


!
  Merge stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Merge stream can be used to merge multiple streams into a single
target stream. The Merge stream does not prevent any of the streams from
writing and no ordering is imposed. The main feature is basically that the
free and abort methods can be called n times where n equals the number of
feeds that put data to the stream.

This module is implemented by HTMerge.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef _HTMERGE_H
#define _HTMERGE_H

#include "HTStream.h"

#ifdef __cplusplus
extern "C" { 
#endif 

extern HTStream * HTMerge (HTStream * target, int feeds);

#ifdef __cplusplus
}
#endif

#endif /* HTMERGE_H */

/*

  

  @(#) $Id$

*/
