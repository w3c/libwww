/*

  					W3C Sample Code Library libwww MIME Parsers


!
  Libwww MIME Parsers
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The MIME parser stream presents a MIME document with a header and possibly
a footer. It recursively invokes the format manager to handle embedded formats
like MIME multipart. As well as stripping off and parsing the headers, the
MIME parser has to parse any weird MIME encodings it may meet within the
body parts of messages, and must deal with multipart
messages (see HTBound.h).

This module is implemented to the level necessary for operation with WWW,
but is not currently complete for any arbitrary MIME message.

This module is implemented by HTMIME.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTMIME_H
#define HTMIME_H

#include "HTStream.h"
#include "HTFormat.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  How to Deal with Unknown Data
.

When the MIME parser can't find a target stream, for example because the
media type is unknown, or it has a content encoding or transfer encoding
that it doesn't know about then it has to get rid of the data in some other
fashion, for example by dumping it to local disk (but it could also be dumping
it to a black hole). The following two functions allow you to set and get
the stream to use in this situation. By default, libwww provides an
implementation of a save stream as HTSaveLocally
which you may want to use - this is for example used by the
current profiles.
*/

extern void HTMIME_setSaveStream (HTConverter * save_stream);
extern HTConverter * HTMIME_saveStream (void);

/*
.
  MIME Parse Stream
.

This stream parses a complete MIME message and if a media type header is
found then the stream stack is called to create the nest stream instance
in the stream pipe. Any piece of the MIME body is pumped right through the
stream.
*/

extern HTConverter HTMIMEConvert;

/*
.
  MIME Header ONLY Parse Stream
.

This stream parses a complete MIME header and then returnes
HT_LOADED. It does not set up any streams and resting data stays
in the buffer. This can be used if you only want to parse the headers before
you decide what to do next. This is for example the case with HTTP HEAD requests.
*/

extern HTConverter HTMIMEHeader;

/*
.
  MIME Footer ONLY Parse Stream
.

Parse only a footer, for example after a chunked encoding.
*/

extern HTConverter HTMIMEFooter;

/*
.
  HTTP 100 Continue Parse Stream
.

The 100 continue status codes can come at any time - we take them and put
the data down a temporary stream. When the 100 continue message has been
parsed, the stream returns HT_CONTINUE
*/

extern HTConverter HTMIMEContinue;

/*
.
  HTTP 101 Switching Protocol Parse Stream
.

The 101 Switching Protocol status code indicates that the rest of the stream
is using another format, protocol, what ever. The result is the same - we
are done parsing here and must leave the rest to the next stream which hopefully
knows more about how to parse the rest of the stream. The stream stack is
called to look for a stream registered for handling
WWW_MIME_UPGRADE. This steam should
return HT_LOADED when it is done, HT_ERROR if an
error occurred and HT_OK as long as it just reads more data.
*/

extern HTConverter HTMIMEUpgrade;

/*
.
  HTTP 206 Partial Data MIME Parse Stream
.

In case we sent a Range conditional GET we may get back a 206 Partial
Response. This response must be appended to the already existing cache entry
before presented to the user. That is, first we load the
cached object and pump it down the pipe and then
the new data follows. Only the latter part gets appended to the cache, of
course.
*/

extern HTConverter HTMIMEPartial;

/*
.
 HTMIME_anchor2response
.

Copies the anchor HTTP headers into a response object by means
of the generic _dispatchParsers function. Written so that we can
copy the HTTP headers stored in the cache to the response object.
*/

#ifndef NO_CACHE
extern HTConverter HTCacheCopyHeaders;
#endif

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTMIME_H */

/*

  

  @(#) $Id$

*/
