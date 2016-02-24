/*

  
  					W3C Sample Code Library libwww Chunk Class


!
  The Chunk Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Chunk Class defines a way to automatically handle dynamic strings and
other data types. You create a chunk with an initial size and it will then
automatically grow to accommodate added data to the chunk. It is a general
utility module. It is guaranteed that the array is '\0' terminated
at all times (and hence is a valid C type string). The method
HTChunkTerminate can be used to explicitly
add a terminating '\0' and then to include this character in
the chunk size. If left out, the terminating character is not considered
part of the chunk.

Note: The names without a "_" (made as a #define's) are
only provided for backwards compatibility and should not be used.

This module is implemented by HTChunk.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTCHUNK_H
#define HTCHUNK_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Create new chunk
.

Create a new chunk and specify the number of bytes to allocate at a time
when the chunk is later extended. Arbitrary but normally a trade-off time
vs. memory
*/

typedef struct _HTChunk HTChunk;

extern HTChunk * HTChunk_new (int growby);

/*
.
  Free a chunk
.

Free a chunk created by HTChunk_newfrom memory
*/

extern void HTChunk_delete (HTChunk * ch);

/*
.
  Clear a chunk
.

Keep the chunk in memory but clear all data kept inside. This can be used
if you know that you can reuse the allocated memory instead of allocating
new memory.  This zeros out all the allocated data (even data past the
indicated size) and sets the size of the chunk to 0.  If you have not used
any bytes past the indicated size, it is more efficient to truncate the
chunk to 0 instead.
*/

extern void HTChunk_clear (HTChunk * ch);

/*
.
  Ensure a Chunk has a Certain Amount of Free Space
.

Make sure that a chunk has enough memory allocated to grow by the
indicated extra size. If this is not the case, then the chunk is expanded
(in multiples of the chunk's "growby" size).  Nothing is done if the
current size plus the requested extra space fits within the chunk's
currently allocated memory.
*/

extern void HTChunk_ensure (HTChunk * ch, int extra_size);

/*
.
  Append a character to a chunk
.

Add the character and increment the size of the chunk by one character
*/

extern void HTChunk_putc (HTChunk * ch, char c);

/*
.
  Append a string to a chunk
.

Add the string and increment the size of the chunk by the length of the string
(without the trailing zero)
*/

extern void HTChunk_puts (HTChunk * ch, const char *str);

/*
.
  Append a block to a chunk
.

Add the block and increment the size of the chunk by the len
*/

extern void HTChunk_putb (HTChunk * ch, const char *block, int len);


/*
.
  Return Pointer to Data
.

This define converts a chunk to a normal char pointer so that it can be parsed
to any ANSI C string function.
*/

extern char * HTChunk_data (HTChunk * ch);

/*
.
  Return Current Size
.

Returns the current size of the chunk
*/

extern int HTChunk_size (HTChunk * ch);

/*
.
  Setting the Size of a Chunk
.

If you want to cut off a piece of a chunk or extend it to make room
for some direct buffer manipulation, then you can use one of these
functions.  Both of these calls set the size of the chunk to be
size, but the truncate call only allows you to make the
string shorter. If the string is made shorter, the formerly-used bytes
are cleared, so truncating a chunk to 0 is analogous to clearing it,
but slightly more efficient.
*/

extern BOOL HTChunk_truncate (HTChunk * ch, int size);
extern BOOL HTChunk_setSize (HTChunk * ch, int size);

/*
.
  Zero Terminate a chunk
.

As a chunk often is a dynamic string, it needs to be terminated by a zero
in order to be used in C. However, by default any chunk is
always zero terminated, so the only purpose of this function is to
increment the size counter with one corresponding to the zero.
*/

extern void HTChunk_terminate (HTChunk * ch);

/*
.
  CString Conversions
.

A Chunk may be built from an allocated string. The chunk assumes control
of the passed string, eliminating the need for additional allocations and
string copies.
When you take control of the CString from a chunk, the chunk is destroyed.
*/

extern HTChunk * HTChunk_fromCString	(char * str, int grow);
extern char * HTChunk_toCString		(HTChunk * ch);

/*
.
 Creating a Chunk from an allocated buffer
.

A Chunk may be built from an allocted buffer.  You must specify how much
memory is allocated in the buffer (buflen) and what the size the new
Chunk should be (size).  All memory between size and buflen is zeroed.
Note that is is legal to specify a size equal to the buflen if you don't
expect the Chunk to be null terminated.  The chunk takes control of the
memory, and will free it when the Chunk is destroyed. Note that in order
to avoid conflicts, the buffer's memory should be allocated using
libwww's dedicated functions.
*/

extern HTChunk * HTChunk_fromBuffer (char * buf, int buflen, int size, int grow);

/*
.
  Old Interface Names
.

Don't use these in new applications
*/

#define HTChunkCreate(growby) HTChunk_new(growby)
#define HTChunkFree(ch)       HTChunk_delete(ch)
#define HTChunkClear(ch)      HTChunk_clear(ch)
#define HTChunkEnsure(ch, s)  HTChunk_ensure((ch), (s))
#define HTChunkPutc(ch, c)    HTChunk_putc((ch), (c))
#define HTChunkPuts(ch, str)  HTChunk_puts((ch), (str))
#define HTChunkTerminate(ch)  HTChunk_terminate(ch)
#define HTChunkData(ch)       HTChunk_data(ch)
#define HTChunkSize(ch)       HTChunk_size(ch)

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTCHUNK_H */

/*

  

  @(#) $Id$

*/
