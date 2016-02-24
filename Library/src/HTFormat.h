/*

  
  					W3C Sample Code Library libwww Stream Pipe Manager


!
  The Stream Pipe Manager
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Stream Pipe Manager is responsible for setting up the stream pipe from
the Channel Object to the
Request Object when data is arriving, for example
as a response to s HTTP Get request. As data
arrives, we start to parse it and the more we know the more we can build
up our stream pipe. For example, in the case of HTTP, we first have a stream
that can parse the HTTP response line containing "200 OK". Then
we have a MIME parser for handling the MIME headers.
When the MIME headers have been parsed, we know the content type and any
encoding of the MIME body. If we need to decode a chunked encoding then we
set up a chunked decoder, and if we have to parse a HTML object then we set
up a HTML parser.

The Format Manager is also responsible for keeping track of the
"preferences" of the application and/or user. It is an integral part
of the Web and HTTP, that the client application can express its preferences
as a set of "accept" headers in a HTTP request. This task is highly related
to the task mentioned above as we there use the modules that are registered
and here tell the remote server what we are capable of doing and what we
would prefer.

Note: The library core does not define any default decoders
or parsers - they are all considered part of the application. The library
comes with a default set of parsers including the ones mentioned above which
can be initiated using the functions in HTInit
module. There are different initialization functions for content type
parsers and content encodings respectively.

	 
	   o 
	     Content Type Converters and Presenters
  o 
	     Content Encoders and Decoders
  o 
	     Content Charsets
  o 
	     Natural Languages

	 
The application can assign its preferences in two ways: either locally
to a single request or globally to all requests. The local assignment
can either add to or override the global settings depending
on how they are registered. All local registration is handled by the
Request Object and the global registration is handled
by the Format Manager.

This module is implemented by HTFormat.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTFORMAT_H
#define HTFORMAT_H

#include "HTUtils.h"
#include "HTStream.h"
#include "HTAtom.h"
#include "HTList.h"
#include "HTAnchor.h"
#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Converters and Presenters
.

All content type converters are subclassed from the Generic stream objetc.
That way, we allow the application to do very fast progressive display of
incoming data. In other words, the stream model of the Library provides data
as soon as it arrives from the network, the application does not have to
wait until the whole document has been down loaded before it starts parsing
it.
(
  Predefined Content Types
)

These macros (which used to be constants) define some basic internally referenced
representations. The www/xxx ones are of course not MIME standard.
They are internal representations used in the Library but they can't be exported
to other apps!
*/

#define WWW_INTERNAL	HTAtom_for("www/*")          /* All internal formats */

/*

WWW_INTERNAL represent all internal formats. This can for example
be used to match using the HTMIMEMatch(...).
*/

#define WWW_RAW		HTAtom_for("www/void")   /* Raw output from Protocol */

/*

WWW_RAW is an output format which leaves the input untouched
exactly as it is received by the protocol module. For example, in
the case of FTP, this format returns raw ASCII objects for directory listings;
for HTTP, everything including the header is returned, for Gopher, a raw
ASCII object is returned for a menu etc.
*/

#define WWW_SOURCE	HTAtom_for("*/*")

/*

WWW_SOURCE is an output format which leaves the input untouched
exactly as it is received by the protocol module IF not a
suitable converter has been registered with a quality factor higher than
1 (for example 2). In this case the SUPER CONVERTER is preferred
for the raw output. This can be used as a filter effect that allows conversion
from, for example raw FTPdirectory listings into HTML but passes a MIME body
untouched.
*/

#define WWW_PRESENT	HTAtom_for("www/present")

/*

WWW_PRESENT represents the user's perception of the document.
If you convert to WWW_PRESENT, you present the material to the
user.
*/

#define WWW_DEBUG	HTAtom_for("www/debug")

/*

WWW_DEBUG represents the user's perception of debug information,
for example sent as a HTML document in a HTTP redirection message.
*/

#define WWW_UNKNOWN     HTAtom_for("www/unknown")

/*

WWW_UNKNOWN is a really unknown type. It differs from the real
MIME type "application/octet-stream" in that we haven't even tried
to figure out the content type at this point.
*/

#define WWW_CACHE         HTAtom_for("www/cache")
#define WWW_CACHE_APPEND  HTAtom_for("www/cache-append")

/*

WWW_CACHE is the internal content-type designated for a persistent
cache module which can store the object to local storage. The cache append
format is special in that we append information to an already existing cache
entry. This can happen if we have issued a If-Range request
and got back a "206 Partial response".

These are regular MIME types defined. Others can be added!
*/

#define WWW_HTML 	HTAtom_for("text/html")
#define WWW_PLAINTEXT 	HTAtom_for("text/plain")
#define WWW_FORM	HTAtom_for("application/x-www-form-urlencoded")

#define WWW_MIME	HTAtom_for("message/rfc822")
#define WWW_MIME_HEAD	HTAtom_for("message/x-rfc822-head")
#define WWW_MIME_FOOT	HTAtom_for("message/x-rfc822-foot")
#define WWW_MIME_PART   HTAtom_for("message/x-rfc822-partial")
#define WWW_MIME_CONT   HTAtom_for("message/x-rfc822-cont")
#define WWW_MIME_UPGRADE	HTAtom_for("message/x-rfc822-upgrade")

#define WWW_MIME_COPYHEADERS HTAtom_for("www/x-rfc822-headers")

#define WWW_AUDIO       HTAtom_for("audio/basic")

#define WWW_VIDEO 	HTAtom_for("video/mpeg")

#define WWW_GIF 	HTAtom_for("image/gif")
#define WWW_JPEG 	HTAtom_for("image/jpeg")
#define WWW_TIFF 	HTAtom_for("image/tiff")
#define WWW_PNG 	HTAtom_for("image/png")

#define WWW_BINARY 	HTAtom_for("application/octet-stream")
#define WWW_POSTSCRIPT 	HTAtom_for("application/postscript")
#define WWW_RICHTEXT 	HTAtom_for("application/rtf")

/*

We also have some MIME types that come from the various protocols when we
convert from ASCII to HTML.
*/

#define WWW_GOPHER_MENU HTAtom_for("text/x-gopher")
#define WWW_CSO_SEARCH	HTAtom_for("text/x-cso")

#define WWW_FTP_LNST	HTAtom_for("text/x-ftp-lnst")
#define WWW_FTP_LIST	HTAtom_for("text/x-ftp-list")

#define WWW_NNTP_LIST   HTAtom_for("text/x-nntp-list")
#define WWW_NNTP_OVER	HTAtom_for("text/x-nntp-over")
#define WWW_NNTP_HEAD	HTAtom_for("text/x-nntp-head")

#define WWW_HTTP	HTAtom_for("text/x-http")

/*

Finally we have defined a special format for our RULE files as they can be
handled by a special converter.
*/
#define WWW_RULES	HTAtom_for("application/x-www-rules")

/*
(
  The Quality Factor
)

Characteristic for all preferences is that there is a quality factor associated
with each member. The quality factor is a real number between 0 and 1 with
0 meaning "very bad" and 1 means "perfect". By registering a natural language
or any or other preference in this group together with a quality factor you
can specify "how well the preference is handled" either by the application
or by the user. In the case of the user the quality factor of a natural language
is how well the user understands the language. In my case, the quality factors
for, for example Greek would be close to zero and 1 for Danish (nothing bad
said about Greek!).

It is a bit different for converters where it is often the application's
ability of handling the data format rather than the user's perception. As
an example it is often faster to use a converter than a presenter as it takes
time to launch the external application and libwww can not use progressive
display mechanisms which is often the case for converters. Therefore, as
an example, if we capable of handling an image in png format inline
but rely on an external viewer for presenting postscript, we might set up
the following list:

HTConversion_add (converters, "image", "www/present", GifPresenter,
1.0, 0.0, 0.0);
HTPresentation_add (presenters, "application/postscript", "ghostview %s",
NULL, 0.5, 0.0, 0.0);>

where the gif converter is registered with a quality factor of 1.0
and the postscript presenter with a quality factor of 0.5.Register
Presenters
(
  The Converter Class
)

A converter is a stream with a special set of parameters and
which is registered as capable of converting from a MIME type to something
else (maybe another MIME-type). A converter is defined to be a function returning
a stream and accepting the following parameters. The content type elements
are atoms for which we have defined a prototype.
*/

typedef HTStream * HTConverter	(HTRequest *	request,
				 void *		param,
				 HTFormat	input_format,
				 HTFormat	output_format,
				 HTStream *	output_stream);

extern void HTConversion_add   (HTList *	conversions,
				const char * 	rep_in,
				const char * 	rep_out,
				HTConverter *	converter,
				double		quality,
				double		secs, 
				double		secs_per_byte);

extern void HTConversion_deleteAll	(HTList * list);

/*
(
  The Presenter Class
)

A presenter is a module (possibly an external program) which
can present a graphic object of a certain MIME type to the user. That is,
presenters are normally used to present objects that the
converters are not able to handle. Data is transferred to the
external program using a special "presenter stream" which for example can
use the local disk to transfer the data from libwww to the external program.

Libwww provides a default HTSaveAndExecute
stream which you may want to use for this purpose. However, any stream
that is of type HTConverter will do. You can manage the
special presenter stream using the following methods:
*/

extern void HTPresentation_setConverter (HTConverter * pconv);
extern HTConverter * HTPresentation_converter (void);

/*
Both presenters and converters are of the type
HTConverter.
*/

extern void HTPresentation_add (HTList *	conversions,
				const char * 	representation,
				const char * 	command,
				const char * 	test_command,
				double		quality,
				double		secs, 
				double		secs_per_byte);

extern void HTPresentation_deleteAll	(HTList * list);

/*
(
  Basic Converters
)

We have a small set of basic converters that can be hooked in anywhere. They
don't "convert" anything but are nice to have.
*/

extern HTConverter HTThroughLine;
extern HTConverter HTBlackHoleConverter;
extern HTConverter HTSaveConverter;

/*
.
  Content and Transfer Encoders and Decoders
.

Content codins are transformations applied to an entity object after it was
created in its original form. The Library handles two types of codings:

  
    Content Codings
  
    Content codings values indicate an encoding transformation that has been
    applied to a resource. Content cosings are primarily used to allow a document
    to be compressed or encrypted without loosing the identity of its underlying
    media type.
  
    Content Transfer Codings
  
    Content transfer codings values are used to indicate an encoding transformation
    that has been, can be, or may be need to be applied to an enity body in order
    to ensure safe transport through the network. This differs from a content
    coding in that the transfer coding is a property of the message, not the
    original message.


Both types of encodings use the same registration mechanism in the Library
which we describe below:
(
  Encoders and Decoders
)

Encoders and decoders are subclassed from the
generic stream class. Encoders are capable
of adding a content coding to a data object and decoders can remove
a content coding.
*/

typedef HTStream * HTCoder	(HTRequest *	request,
				 void *		param,
				 HTEncoding	coding,
				 HTStream *	target);

/*

The encoding is the name of the encoding mechanism reporesented
as an atom, for example "zip", "chunked", etc.
Encodings are registered in lists and content encodings are separated from
transfer encodings by registering them in different lists.
(
  Basic Encoders
)

We have a small set of basic coders that can be hooked in anywhere.
*/

extern HTCoder HTIdentityCoding;

/*
(
  The HTCoding Object
)

The HTCoding object represents a registered encoding together with
a encoder and a decoder.
*/

typedef struct _HTCoding HTCoding;

/*

Predefined Coding Types We have a set of pre defined atoms for various types
of content encodings and transfer encodings. "chunked" is not exactly in
the same group as the other encodings such as "binary" but it really doesn't
make any difference as it is just a matter of how the words are chosen. The
first three transfer encodings are actually not encodings - they are just
left overs from brain dead mail systems.
*/

#define WWW_CODING_7BIT		HTAtom_for("7bit")
#define WWW_CODING_8BIT		HTAtom_for("8bit")
#define WWW_CODING_BINARY	HTAtom_for("binary")
#define WWW_CODING_IDENTITY     HTAtom_for("identity")

#define WWW_CODING_BASE64	HTAtom_for("base64")
#define WWW_CODING_MACBINHEX	HTAtom_for("macbinhex")
#define WWW_CODING_CHUNKED	HTAtom_for("chunked")

#define WWW_CODING_COMPRESS	HTAtom_for("compress")
#define WWW_CODING_GZIP	        HTAtom_for("gzip")
#define WWW_CODING_DEFLATE      HTAtom_for("deflate")

/*
(
  Register Content Coders
)

Some documents are not send in their original data obejct but is encoded
in some way. On the Web this is mostly some kind of compression but other
encodings for example base 64 can be encountered when talking to NNTP servers
etc. Just as for the other preferences, an application can register a supported
encoders or decodes as a list. Encoders and decoders are registered in the
same way with no differentiation whether it is a encoder or a decoder:
*/

extern BOOL HTCoding_add (HTList * 	list,
			 const char *	encoding,
			 HTCoder *	encoder,
			 HTCoder *	decoder,
			 double		quality);

extern void HTCoding_deleteAll (HTList * list);

extern const char * HTCoding_name (HTCoding * me);

extern double HTCoding_quality (HTCoding * me);

/*
.
  Content Charsets
.

As the Web reaches all parts of the Internet there are more and more documents
written in languages which contains characters not included in the ISO-8859-1
character set. A consequence of this the set of characters sets is often
tightly connected with the natural language. libwww does not directly support
other character sets but in case an application is capable of handling
alternative sets it can register these as preferred character sets along
with a quality factor just as all the other preferences in this section.
*/
extern void HTCharset_add (HTList * list, const char * charset, double quality);

/*
*/
typedef struct _HTAcceptNode {
    HTAtom *	atom;
    double	quality;
} HTAcceptNode;

/*
*/

extern void HTCharset_deleteAll	(HTList * list);

/*
.
  Content Languages
.

The preferred natural language or languages is in almost all situations dependent
on the individual user and an application should therefore give the user
the opportunity to change the setup. When specifying a natural language
preference, libwww will send this preference along with all HTTP requests.
The remote server will then (it if supports this feature) look for a version
in the language or languages mentioned. If it finds a matching document then
it returns this one, otherwise it uses the best alternative. If no language
is specified the remote server may whatever version it finds.
*/
extern void HTLanguage_add (HTList * list, const char * lang, double quality);
extern void HTLanguage_deleteAll (HTList * list);

/*
.
  Global Preferences
.

There are two places where these preferences can be registered: in a
global list valid for all requests and a local list
valid for a particular request only. These are valid for all requests.
See the Request Manager fro local sets.
(
  Global Converters and Presenters
)

The global list of specific conversions which the format manager
can do in order to fulfill the request. There is also a
local list of conversions which contains
a generic set of possible conversions.
*/
extern void HTFormat_setConversion	(HTList * list);
extern HTList * HTFormat_conversion	(void);

extern void HTFormat_addConversion (const char *	input_format,
				    const char *	output_format,
				    HTConverter *	converter,
				    double		quality,
				    double		secs, 
				    double		secs_per_byte);

/*
(
  Global Content Codings
)
*/
extern void HTFormat_setContentCoding	(HTList * list);
extern HTList * HTFormat_contentCoding	(void);

extern BOOL HTFormat_addCoding ( char *		encoding,
				 HTCoder *	encoder,
				 HTCoder *	decoder,
				 double		quality);

/*

We also define a macro to find out whether a content encoding is really an
encoding or whether it is a unity encoder.
*/

#define HTFormat_isUnityContent(me) \
	((me)==NULL || \
	(me)==WWW_CODING_BINARY || (me)==WWW_CODING_IDENTITY || \
        (me)==WWW_CODING_7BIT || (me)==WWW_CODING_8BIT)

/*
(
  Global Transfer Codings
)
*/
extern void HTFormat_setTransferCoding	(HTList * list);
extern HTList * HTFormat_transferCoding	(void);

extern BOOL HTFormat_addTransferCoding ( char *		encoding,
					 HTCoder *	encoder,
					 HTCoder *	decoder,
					 double		quality);

/*

We also define a macro to find out whether a transfer encoding is really
an encoding or whether it is just a "dummy" as for example 7bit, 8bit, and
binary.
*/

#define HTFormat_isUnityTransfer(me) \
	((me)==NULL || \
	 (me)==WWW_CODING_BINARY || (me)==WWW_CODING_IDENTITY || \
         (me)==WWW_CODING_7BIT || (me)==WWW_CODING_8BIT)

/*
(
  Global Content Languages
)
*/
extern void HTFormat_setLanguage	(HTList * list);
extern HTList * HTFormat_language	(void);

/*
(
  Global Content Charsets
)
*/
extern void HTFormat_setCharset		(HTList * list);
extern HTList * HTFormat_charset	(void);

/*
(
  Delete All Global Lists
)

This is a convenience function that might make life easier.
*/
extern void HTFormat_deleteAll (void);

/*
.
  The Content Type Stream Stack
.

This is the routine which actually sets up the content type conversion. It
currently checks only for direct conversions, but multi-stage conversions
are forseen. It takes a stream into which the output should be sent in the
final format, builds the conversion stack, and returns a stream into which
the data in the input format should be fed. If guess is true
and input format is www/unknown, try to guess the format by
looking at the first few bytes of the stream.
*/

extern HTStream * HTStreamStack (HTFormat	rep_in,
				 HTFormat	rep_out,
				 HTStream *	output_stream,
				 HTRequest *	request,
				 BOOL		guess);

/*
(
  Cost of a Stream Stack
)

Must return the cost of the same stack which HTStreamStack would set up.
*/

extern double HTStackValue	(HTList *	conversions,
				 HTFormat	format_in,
				 HTFormat	format_out,
				 double		initial_value,
				 long int	length);

/*
.
  Content Encoding Stream Stack
.

When creating a coding stream stack, it is important that we keep the right
order of encoders and decoders. As an example, the HTTP spec specifies that
the list in the Content-Encoding header follows the order in which
the encodings have been applied to the object. Internally, we represent the
content encodings as atoms in a linked
list object.

The creation of the content coding stack is not based on quality factors
as we don't have the freedom as with content types. When using content codings
we must apply the codings specified or fail.
*/

extern HTStream * HTContentCodingStack (HTEncoding	coding,
					HTStream *	target,
					HTRequest *	request,
					void *		param,
					BOOL		encoding);

/*

Here you can provide a complete list instead of a single token. The list
has to be filled up in the order the _encodings_ are to be applied
*/

extern HTStream * HTContentEncodingStack (HTList *	encodings,
					  HTStream *	target,
					  HTRequest *	request,
					  void *	param);

/*

Here you can provide a complete list instead of a single token. The list
has to be in the order the _encodings_ were applied - that is, the same way
that _encodings_ are to be applied. This is all consistent with the order
of the Content-Encoding header.
*/

extern HTStream * HTContentDecodingStack (HTList *	encodings,
					  HTStream *	target,
					  HTRequest *	request,
					  void *	param);

/*
.
  Transfer Encoding Stream Stack
.

When creating a coding stream stack, it is important that we keep the right
order of encoders and decoders. As an example, the HTTP spec specifies that
the list in the Transfer-Encoding header follows the order in which
the encodings have been applied to the object. Internally, we represent the
content encodings as atoms in a linked
list object.

The creation of the content coding stack is not based on quality factors
as we don't have the freedom as with content types. When using content codings
we must apply the codings specified or fail.
*/

extern HTStream * HTTransferCodingStack (HTEncoding	coding,
					 HTStream *	target,
					 HTRequest *	request,
					 void *		param,
					 BOOL		encoding);

/*

Here you can provide a complete list instead of a single token. The list
has to be filled up in the order the _encodings_ are to be applied
*/

extern HTStream * HTTransferEncodingStack (HTList *	encodings,
					   HTStream *	target,
					   HTRequest *	request,
					   void *	param);

/*

Here you can provide a complete list instead of a single token. The list
has to be in the order the _encodings_ were applied - that is, the same way
that _encodings_ are to be applied. This is all consistent with the order
of the Transfer-Encoding header.
*/

extern HTStream * HTTransferDecodingStack (HTList *	encodings,
					  HTStream *	target,
					  HTRequest *	request,
					  void *	param);

/*
.
  Transfer Encoding Stream Stack
.

Creating the transfer content encoding stream stack is not based on quality
factors as we don't have the freedom as with content types. Specify whether
you you want encoding or decoding using the BOOL "encode" flag.
*/

extern HTStream * HTContentTransferCodingStack (HTEncoding	encoding,
					        HTStream *	target,
					        HTRequest *	request,
					        void *		param,
					        BOOL		encode);

/*
(
  Presentation Object
)

This object is not to be used - it should have been hidden&nbsp;
*/
typedef struct _HTPresentation {
    HTFormat	rep;			     /* representation name atomized */
    HTFormat	rep_out;			 /* resulting representation */
    HTConverter *converter;	      /* The routine to gen the stream stack */
    char *	command;			       /* MIME-format string */
    char *	test_command;			       /* MIME-format string */
    double	quality;		     /* Between 0 (bad) and 1 (good) */
    double	secs;
    double	secs_per_byte;
} HTPresentation;

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTFORMAT */

/*

  

  @(#) $Id$

*/
