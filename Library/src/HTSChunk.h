/*


  					W3C Sample Code Library libwww Stream to Chunk Converter


!
  Stream to Chunk Converter
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This stream converts a Stream obejct into a
Chunk object. Chunks are dynamic streams so this
is in other words a conversion from a stream based model to a dynamic data
buffer model for handling a downloaded object. It is for the caller of this
stream to free the chunk.

If max_size is 0 then we use a default size, if -1 then there is no limit.

This module is implemented by HTSChunk.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTSCHUNK_H
#define HTSCHUNK_H

#include "HTChunk.h"
#include "HTStream.h"

#ifdef __cplusplus
extern "C" { 
#endif 

extern HTStream * HTStreamToChunk (HTRequest * 	request,
				   HTChunk **	chunk,
				   int 		max_size);

/*

End of definition module
*/

#ifdef __cplusplus
}
#endif

#endif /* HTSCHUNK_H */

/*

  

  @(#) $Id$

*/
