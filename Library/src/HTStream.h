/*

  
  					W3C Sample Code Library libwww Generic Stream Class


!
  Generic Stream Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Stream class defines objects which accepts a sequence of characters.
Streams may also have an output in which case multiple stream objects can
be cascaded to build a stream pipe where the output of a stream is directed
into the input of the next stream object "down the line". Of course, one
of the main features of streams is that they can perform a data conversion
on the data before piping it to the output. As multiple streams may be
cascaded, the complete data conversion is then the sum of each individual
data conversion performed by the stream objects being part of the stream
pipe.

It is not required that a stream has a target, it might as well be
a black hole that just accepts data without ever giving it out again. The
generic stream class is subclassed multiple places in the Library and a good
example is the structured stream definition which
creates a SGML object.

All stream class methods return an integer status code telling whether the
operation succeeded or not.. This is the way for a stream to pass control
information upstream to the caller which may also be a stream. The general
return codes from the methods are:

	 
	   o 
	     HT_WOULD_BLOCK
  o 
	     HT_ERROR
  o 
	     HT_OK
  o 
	     >0 - any return greater than 0 will result in that the return code
    will be parsed through all stream objects. This can be used to pass back
    information to the protocol modules, for example

	 
It is in general not relevant to return how much data has been written in
the stream, as there often will be a relationship other than 1:1 between
indata and outdata. However, it is important that a stream keeps state (either
on the incoming data or the outgoing data stream) so that it can accept a
HT_WOULD_BLOCK and continue at a later time when the blocking
situation has stopped.

This module is implemented by HTStream.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTSTREAM_H
#define HTSTREAM_H

#include "HTList.h"

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTStream HTStream;

typedef struct _HTStreamClass {

    char * name;

/*

This field is for diagnostics only
*/

    int (*flush)	(HTStream * me);

/*

The flush method is introduced in order to allow the stream to put
any buffered data down the stream pipe but without taking the stream pipe
down. It is for the stream to decide whether it has buffered data or not.
In some situations, the stream might not want to send buffered data down
the target as the date might be relevant for this stream only.
*/

    int (*_free)	(HTStream * me);

/*

The free method is like the flush method but it also
frees the current stream object and all stream objects down stream. When
the free method has been called, the whole stream pipe
(not only this obejct) should not accept any more data.
*/

    int (*abort)	(HTStream * me, HTList * errorlist);

/*

The abort method should only be used if a stream is interrupted, for
example by the user, or an error occurs.
*/

    int (*put_character)(HTStream * me, char ch);
				
    int (*put_string)	(HTStream * me, const char * str);

    int (*put_block)	(HTStream * me, const char * str, int len);

/*

These methods are for actually putting data down the stream. It is important
that the most efficient method is chosen (often put_block). There is no guarantee
that a stream won't change method, for example from
put_character to put_block
*/

} HTStreamClass;

/*


.
  Basic Utility Streams
.

These streams can be plugged in everywhere in a stream pipe.
(
  Black Hole Stream
)

This stream simply absorbs data without doing anything what so ever.
*/

extern HTStream * HTBlackHole (void);

/*
(
  Generic Error Stream
)

The Error stream simply returns HT_ERROR on all methods. This
can be used to stop a stream as soon as data arrives, for example from the
network.
*/

extern HTStream * HTErrorStream (void);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTSTREAM_H */

/*

  

  @(#) $Id$

*/
