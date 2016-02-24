/*

  
  					W3C Sample Code Library libwww Content Length Counter and Other Buffer
  Streams


!
  Content Length Counter and Other Buffer Streams
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module provides various forms of buffering streams. You can also check
out the chunk buffer stream which converts a
stream to a memory buffer. These streams preseerve the streaming model.

The content length counter &nbsp;stream buffers the result&nbsp;to find out
the content length&nbsp;before the data is forwarded to the target stream.
You can use this stream when generating a HTTP response in order to find
the content length before writing the object to the socket, for example.

This module is implemented by HTConLen.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTCONLEN_H
#define HTCONLEN_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Buffer Stream
.

This stream does almost the same as the content length counter stream except
that it doesn't count the length! In other words - it's a completely normal
memory buffer for the stream. If the buffer fills up, this stream flushes
the buffer and goes transparent so that all new data will be pumped through
without any buffering.
*/

extern HTStream * HTBuffer_new		(HTStream *	target,
					 HTRequest *	request,
					int		max_size);

/*
.
  Content Length Counter Stream
.

This stream can be inserted anywhere to count the content length of the body.
The result is passed to the Anchor object of
this request so that future requests for the content length will get the
right size.
*/

extern HTStream * HTContentCounter	(HTStream *	target,
					 HTRequest *	request,
					 int		max_size);

/*
.
  Delay Buffer Stream
.

This stream is much like the buffer stream above but instead of goin transparent
when the buffer fills up, it returns HT_PAUSE which indicates
that the stream doesn't accept more data and so the caller should not put
any more data into it. When the buffer is flushed (the flush method invoked)
then the stream goes into transparent mode, just like the buffer stream.
*/

extern HTStream * HTDelayBuffer    	(HTStream *	target,
					 int		max_size);

/*
.
  Pipe Buffer Stream
.

This stream is much like the buffer stream above but instead of goin transparent
when the buffer fills up, it returns HT_PAUSE which indicates
that the stream doesn't accept more data and so the caller should not put
any more data into it.
*/

extern HTStream * HTPipeBuffer    	(HTStream *	target,
					 int		max_size);

/*

End of definition module
*/

#ifdef __cplusplus
}
#endif

#endif /* HTCONLEN_H */

/*

  

  @(#) $Id$

*/
