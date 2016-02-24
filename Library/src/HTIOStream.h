/*

  
  					W3C Sample Code Library libwww I/O Stream Classes


!
  I/O Stream Classes
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The I/O Stream class defines objects which accepts a sequence of characters
to and from a transport &nbsp;The input and output
stream are mainly derived from the generic stream
class and contains much of the same functionality. The main difference
is that the I/O streams also contains methods for reading and writing to
a transport.

This module is a part of the W3C
Sample Code Library.
*/

#ifndef HTIOSTREAM_H
#define HTIOSTREAM_H

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTInputStream HTInputStream;
typedef struct _HTOutputStream HTOutputStream;

#include "HTList.h"
#include "HTStream.h"
#include "HTChannl.h"


/*
.
  Input Stream
.

An input stream is a stream that can read data from a transport and via a
channel putting the data down to the application.
*/

typedef struct _HTInputStreamClass {

    char * name;

/*

This field is for diagnostics only
*/

    int (*flush)	(HTInputStream * me);

/*

The flush method is introduced in order to allow the stream to put
any buffered data down the stream pipe but without taking the stream pipe
down. It is for the stream to decide whether it has buffered data or not.
In some situations, the stream might not want to send buffered data down
the target as the date might be relevant for this stream only.
*/

    int (*_free)	(HTInputStream * me);

/*

The free method is like the flush method
but it also frees the current stream object and all stream objects down stream.
When the free method has been called, the whole stream
pipe (not only this object) should not accept any more data. See also the
close method below
*/

    int (*abort)	(HTInputStream * me, HTList * errorlist);

/*

The abort method should only be used if a stream is interrupted, for
example by the user, or an error occurs.
*/

    int (*read)		(HTInputStream * me);

/*

The read method is the method by which we can read data from the
transport layer.
*/

    int (*close)	(HTInputStream * me);

/*

Pipelined transports need to know how many bytes were consumed by 
the net object.
*/

    int (*consumed)	(HTInputStream * me, size_t bytes);

/*

The close method closes the transport and deletes the input
stream object. Note that this is different than the free method which doesn't
have to delete the input stream object itself.
*/

} HTInputStreamClass;

/*
.
  Output Stream
.

The output stream is similar to the generic stream
definition in that it has a superset of methods. The param
parameter and the mode parameter can be used for whatever purpose
suited.
*/

typedef struct _HTOutputStreamClass {

    char * name;

    int (*flush)	(HTOutputStream * me);

    int (*_free)	(HTOutputStream * me);

    int (*abort)	(HTOutputStream * me, HTList * errorlist);

    int (*put_character)(HTOutputStream * me, char ch);

    int (*put_string)	(HTOutputStream * me, const char * str);

    int (*put_block)	(HTOutputStream * me, const char * str, int len);

/*

See the generic Stream Definition for an explanation
of these methods. Note that they all have a HTOutputStream object
a the parameter, not a generic stream. This is to avoid incompatible
pointer warnings
*/

    int (*close)	(HTOutputStream * me);

/*

The close method closes the transport and deletes the input
stream object. Note that this is different than the free method which doesn't
have to delete the input stream object itself.
*/

} HTOutputStreamClass;

/*
.
  Transport Streams
.

Transport streams are special streams with creation methods like defined
below. Transport streams can be registered in a
transport object as ways of communicating with
the a transport.
(
  Transport Input Stream
)

We have two modes of the input stream depending on model used for data reading
is PUSH or PULL. The PUSH model is suitable if we are using
pseudo threads based on a select() call or equivalent and the
PULL is suitable in a real thread environment. In the latter case
it doesn't matter if a read procedure blocks as this only concerns a single
thread.
*/

typedef HTInputStream * HTInput_new	(HTHost *	host,
					 HTChannel *	ch,
					 void *		param,
					 int		mode);

/*
(
  Transport Output Stream
)
*/

typedef HTOutputStream * HTOutput_new	(HTHost *	host,
					 HTChannel *	ch,
					 void *		param,
					 int		mode);

/*
(
  Transport Output Stream Converter
)
*/

typedef HTOutputStream * HTOutputConverter_new(
	HTHost *		host,
	HTChannel *		ch,
	void *			param,
	int			mode,
	HTOutputStream *	target);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTIOSTREAM_H */

/*

  

  @(#) $Id$

*/
