/*

  
  					W3C Sample Code Library libwww Accessing URIs


!
  Accessing URIs
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module is the application interface module to the
Request class. It contains a lot of methods for
loading URIs and also for uploading URIs using PUT or
POST, for example. You can use the Request class directly but
this module makes it easier to use by providing a lot of small request functions
using the Request class in different ways. It contains help functions for
accessing documents and for uploading documents to a remote server.

This module contains functions for handling all HTTP/1.1 methods:

  
    GET Requests
  
    	 
	       o 
	 	Various GET requests including specialized functions like
	loading a rule file, etc.
      o 
	 	Search requests based on the GET method
      o 
	 	Formdata requests based on the GET method
    
	   
    PUT Requests
  
    	 
	       o 
	 	Save a document from memory ASIS using PUT
      o 
	 	Save a structured document from memory using PUT
      o 
	 	Save any URI (FTP, HTTP, local disk) using PUT
    
	   
    POST Requests
  
    	 
	       o 
	 	Post Formdata to a remote HTTP server
      o 
	 	Post a document from memory ASIS to a remote HTTP
	server
    
	   
    HEAD, DELETE, OPTIONS, and TRACE requests
  
    	 
	       o 
	 	Get metainformation about a document using HEAD requests
      o 
	 	Delete documents based on the DELETE method
      o 
	 	Get information about the features supoprted by a resource
	using OPTIONS
      o 
	 	Trace a request using the TRACE method
    
	 

Furthermore, it contains a few access methods for handling
incoming requests - in orther words acting as a server. Although libwww
is primarily for clients, it is in fact symmetric in that it can handle both
client requests and server requests.

This module is implemented by HTAccess.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTACCESS_H
#define HTACCESS_H

#include "HTReq.h"
#include "HTAnchor.h"

#ifdef __cplusplus
extern "C" { 
#endif 


/*
.
  Load a Document (Method = GET)
.

URIs can be accesses using a character string, for example
"http://www.w3.org" or it can be accessed by using the libwww
representation of a URI called an Anchor object.
Note that we call all objects accessible through URIs for documents
- this is a notion we have inherited from the hypertext world.
(
  Load a Document from Absolute URI
)

Request a document referencd by an absolute URI. The output from the
request is passed to the Stream Pipe Manager
that figures out where to pump the data. This can for example be to the display
or to a local file depending on the set of
converters registered by the application.
*/
extern BOOL HTLoadAbsolute (const char * url, HTRequest * request);

/*
(
  Load a Document from Relative URI
)

Request a document referenced by a relative URI. The relative URI
is made absolute by resolving it relative to the address of the 'base'
anchor.
*/
extern BOOL HTLoadRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request);

/*
(
  Load a Document into Memory
)

Request a document referred to by the URI and load it into a
HTChunk object. A chunk
object is a dynamic string so in the end you will have a single memory
buffer containing the document. The chunk must be freed by the caller.
*/
extern HTChunk * HTLoadToChunk (const char * url, HTRequest * request);

/*
(
  Load a Document and Save as a Local File
)

This function loads a URI and saves the contents in the specifed file. The
file should not &nbsp;be open, as the load function both opens and closes
the file. If the file already exists then it asks the user whether the file
should be overwritten or not. the contents is saved ASIS - that is
- we do not touch the contents of the file!
*/

extern BOOL HTLoadToFile (const char * url, HTRequest * request,
			  const char * filename);

/*
(
  Load a Document and put the Contents into a Stream
)

Request a document referenced by an absolute URI and sending the data down
a stream. This stream can be anny stream you like, for eample one from the
Stream Interface.
*/

extern BOOL HTLoadToStream (const char * url, HTStream * output,
			    HTRequest * request);

/*
(
  Load a Document using an Anchor
)

Here the URI is represented by an Anchor object.
You can get an anchor object representing a URI by passing the URI to the
appropriate method in the Anchor class.
*/
extern BOOL HTLoadAnchor (HTAnchor * anchor, HTRequest * request);

/*
(
  Load a Document into Memory using an Anchor
)

This is the same as HTLoadToChunk but instead of passing a URI
string you pass an HTAnchor object. Internally,
all URIs are represented as anchors which contains all the information we
have about the resource. The chunk must be freed by the caller.
*/
extern HTChunk * HTLoadAnchorToChunk (HTAnchor * anchor, HTRequest * request);

/*
(
  Recursively Request a Document using Anchors
)

Same as HTLoadAnchor() but the information in the
error stack in the request
object is kept, so that any error messages in one. This function is almost
identical to HTLoadAnchor, but it doesn't clear the error stack
so that the information in there is kept.
*/
extern BOOL HTLoadAnchorRecursive (HTAnchor * anchor, HTRequest * request);

/*
.
  Load Special Documents
.

We also have a set of functions for loading special files like rules files
which also are referenced by a URI but which do have to be treated specially.
(
  Load a Rule File
)

Rule files can be loaded just like any other URI but you can also just use
these functions which do all the work for you: they load a rule find with
the URI specified and add the set of rules to the existing set.

They come in two flavours - one that asks the user whether it is OK to add
the rules and one that does it automatically without asking. As the app would
have to call this method explicitly, it may have other ways of protecting
the user.

Both functions use preemptive requests
so that everything else stops in the meantime.
*/

extern BOOL HTLoadRules (const char * url);
extern BOOL HTLoadRulesAutomatically (const char * url);

/*
.
  Search a Document (Method = GET)
.

The search methods all use GET as the method in the
HTTP request. The functions take
the keywords and encode them according to
RFC
1866 (Hypertext Markup language). That is, the query part is separated
from the rest of the URI by a "?".

The keywords are passed to the function as a Chunk
Object and each keyword must be separated by a space ' '. This
will then be converted into a '+' before added to the URI.
(
  Search a Document from Absolute URI
)
*/

extern BOOL HTSearchAbsolute (HTChunk *		keywords,
			      const char *	base,
			      HTRequest *	request);

/*
(
  Search a Document from Relative URI
)

Search a document referenced by a relative URI. The relative URI is
made absolute by resolving it relative to the address of the 'base'
anchor.
*/

extern BOOL HTSearchRelative (HTChunk *		keywords,
			      const char * 	relative,
			      HTParentAnchor *	base,
			      HTRequest *	request);

/*
(
  Search a Document using an Anchor
)
*/

extern BOOL HTSearchAnchor (HTChunk *		keywords,
			    HTAnchor *		anchor,
			    HTRequest * 	request);

/*
(
  Search a Document using an Anchor Using a String
)

This works exactly as the HTSearchAnchor() function but takes
a C string instead of a chunk object.
*/

extern BOOL HTSearchString (const char *	keywords,
			    HTAnchor *		anchor,
			    HTRequest * 	request);

/*
.
  Submit Forms Using GET Method
.

Formdata can be sent to an HTTP server in two ways - it can either use a
GET method or it can use a POST method. The difference
is whether the request "has side effects" or not. For example, if you are
ordering a pizza then the (hopefully positive) sideeffect is that you actually
get one delivered. However, if you are issuing search data - for example
to Alta Vista, then there is no sideeffect. In the former example you would
use the GET form and in the latter you would use the
POST form.
(
  Submit Form from Absolute URI using GET
)

Submit formdata using GET to the address indicated as the "base" which must
be an absolute URI. The list of form data must be given as an
association list where the name is the field name
and the value is the value of the field.
*/

extern BOOL HTGetFormAbsolute (HTAssocList *	formdata,
			       const char *	base,
			       HTRequest *	request);

/*
(
  Submit Form from Relative URI using GET
)

Submit formdata using GET to the address indicated relative to the address
of the base anchor. The list of form data must be given as an
association list where the name is the field name
and the value is the value of the field.
*/

extern BOOL HTGetFormRelative (HTAssocList * 	formdata,
			       const char * 	relative,
			       HTParentAnchor *	base,
			       HTRequest *	request);

/*
(
  Send a Form using an Anchor and the GET Method
)

Submit formdata using GET to the address indicated of the anchor. The list
of form data must be given as an association list
where the name is the field name and the value is the value of the field.
*/

extern BOOL HTGetFormAnchor (HTAssocList *	formdata,
			     HTAnchor *		anchor,
			     HTRequest * 	request);

/*
.
  Submit Forms Using POST Method
.

The data in a POST form is sent as the body part of the
HTTP message whereas a
GET form wraps it all up into the URI. In order to be able to
use the POST data object at a later point in time, we create
a new anchor on the fly. This anchor has a URI file location which points
into the temporary area given by the User Profile
Object. That is - you can actually save the anchor using a
PUT request and then be able to retrive the form data at a later
point in time. Even though this may seem "ambitious" for posting form data,
it is really just a special example of sending any kind of data to a remote
server. All POST form functions return the new anchor or
NULL if they fail.
(
  Submit Form from Absolute URI using POST
)

Submit formdata using POST to the address indicated as the "base" which must
be an absolute URI. The list of form data must be given as an
association list where the name is the field name
and the value is the value of the field.
*/

extern HTParentAnchor * HTPostFormAbsolute (HTAssocList *	formdata,
					    const char *	base,
					    HTRequest *	request);

/*
(
  Submit Form from a Relative URI using GET
)

Submit formdata using POST to the address indicated relative to the address
of the base anchor. The list of form data must be given as an association
list where the name is the field name and the value is the value of the field.
*/

extern HTParentAnchor * HTPostFormRelative (HTAssocList * 	formdata,
					    const char * 	relative,
					    HTParentAnchor *	base,
					    HTRequest *		request);

/*
(
  Submit Form using an Anchor and the POST Method
)

Submit formdata using POST to the address indicated of the anchor. The list
of form data must be given as an association list
where the name is the field name and the value is the value of the field.
*/

extern HTParentAnchor * HTPostFormAnchor (HTAssocList *	formdata,
					  HTAnchor *	anchor,
					  HTRequest * 	request);

/*
(
  Submit Form and Save the Result in a Memory Buffer
)

Submit formdata to the address referred to by the
HTAnchor object and load the result of the POST
into a HTChunk object. A
chunk object is a dynamic memory buffer so in
the end you will have a single memory buffer containing the document. The
chunk must be freed by the caller.
*/

extern HTChunk * HTPostFormAnchorToChunk (HTAssocList * formdata,
                                          HTAnchor *    anchor,
                                          HTRequest *   request);

/*
.
  Get Metainformation about a Document (Method = HEAD)
.

If you are not interested in the document itself but only in the
metainformation that describes the document then you should
use the HEAD method in your request.
(
  Get Metainformation about a Document from Absolute URI
)

Request metainfomration about a document referencd by an absolute
URI.
*/
extern BOOL HTHeadAbsolute (const char * url, HTRequest * request);

/*
(
  Get Metainformation about a Document from Relative URI
)

Request metainformation about a document referenced by a relative
URI.
*/
extern BOOL HTHeadRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request);

/*
(
  Get Metainformation about a Document using an Anchor
)

Here the URI is represented by an Anchor object.
You can get an anchor object representing a URI by passing the URI to the
approproiate method in the Anchor class.
*/
extern BOOL HTHeadAnchor (HTAnchor * anchor, HTRequest * request);

/*
.
  Delete a Document (Method = DELETE)
.

If you want to delete a document (or make the document inaccessible for future
references) then you can use the DELETE method in your request.
(
  Delete a Document from Absolute URI
)

Request metainfomration about a document referencd by an absolute
URI.
*/
extern BOOL HTDeleteAbsolute (const char * url, HTRequest * request);

/*
(
  Delete a Document from Relative URI
)

Request metainformation about a document referenced by a relative
URI.
*/
extern BOOL HTDeleteRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request);

/*
(
  Delete a Document using an Anchor
)

Here the URI is represented by an Anchor object.
You can get an anchor object representing a URI by passing the URI to the
approproiate method in the Anchor class.
*/
extern BOOL HTDeleteAnchor (HTAnchor * anchor, HTRequest * request);

/*
.
  Save a Document ASIS From Memory (Method = PUT)
.

If you already have a document in memory and it is associated with an
Anchor object then you can PUT this document
to a remote server using the following methods. Other information that you
can set in the anchor is metadata like the media type, the document length,
the language, or any other information that you want to associate with the
document to be uploaded.

This set of functions takes the document ASIS - that it the
exact content of the document associated with this anchor will be
sent to the remote server. If your anchor represents a structured content
and the document itself is a parse tree, for example, then you can use the
structured PUT functions.

If your application is an editor, then you may want to create a new anchor
on the fly for temporary backups on local disk before you save it to a remote
server. An easy way to get a new anchor with a local file URI is to use the
HTTmpAnchor function which is part of the
WWWApp interface.
(
  Save a Document ASIS from Memory Using Absolute URI (PUT)
)

The source is an anchor with the contents already in memory and the destination
is an absolute URI.
*/

extern BOOL HTPutAbsolute (HTParentAnchor *	source,
			   const char *		destination,
			   HTRequest *		request);

/*
(
  Save a Document ASIS from Memory Using Relative URI (PUT)
)

The source is an anchor with the contents already in memory and the destination
is a relative URI relative to the destination anchor
*/

extern BOOL HTPutRelative (HTParentAnchor *	source,
			   const char * 	relative,
			   HTParentAnchor *	destination_base,
			   HTRequest *		request);

/*
(
  Save a Document ASIS from Memory Using an Anchor (PUT)
)

Both the source and the anchor are anchor objects. The source anchor already
has the contents in memory
*/

extern BOOL HTPutAnchor (HTParentAnchor *	source,
			 HTAnchor *		dest,
			 HTRequest *	 	request);

/*
.
  Save a Structured Document (Using PUT)
.

If you want to save a document from memory but it contains structured information
(for example, it is in the form of a parse tree) then you can use this interface.
The only difference from above is that the caller must provide the function
that provides data while sending it accross the network - we can't just take
it as a block. You can for example have a look at the
HTEntity_callback function which is used in the
ASIS interface if you want to write your own
data feeding method.
(
  Save a Structured Document from Memory to Absolute URI (PUT)
)

Upload a document referenced by an absolute URI.
*/

extern BOOL HTPutStructuredAbsolute (HTParentAnchor *	source,
				     const char *	destination,
				     HTRequest *	request,
				     HTPostCallback *	input);

/*
(
  Save a Structured Document from Memory to Relative URI (PUT)
)

The source is an anchor with the contents already in memory and the destination
is a relative URI relative to the destination anchor
*/

extern BOOL HTPutStructuredRelative (HTParentAnchor *	source,
				     const char * 	relative,
				     HTParentAnchor *	destination_base,
				     HTRequest *	request,
				     HTPostCallback *	input);

/*
(
  Save a Structured Document Using an Anchor and the PUT Method
)

The source is an anchor with the contents already in memory and the destination
is a relative URI. The HTPostCallback function type is declared in the HTRequest
object.
*/

extern BOOL HTPutStructuredAnchor (HTParentAnchor *	source,
				   HTAnchor *		destination,
				   HTRequest *	 	request,
				   HTPostCallback *	input);

/*
.
  Save a Remote Document (Using PUT)
.

If the content of the document associated with the anchor is NOT in
memory then you can use this interface. These methods make two requests:
first they go out and get the source which can be on an FTP server, on local
disk, on another HTTP server etc. and then they PUT this document ASIS to
the destination HTTP server.
(
  Save a Document from Absolute URI using PUT
)

Get the source and PUT it to the destination which is an absolute URI
*/

extern BOOL HTPutDocumentAbsolute (HTParentAnchor *	source,
				   const char *	        destination,
				   HTRequest *	        request);

/*
(
  Save a Document from Relative URI using PUT
)

Get the source and PUT it to the destination which is a relative URI
*/

extern BOOL HTPutDocumentRelative (HTParentAnchor *	source,
				   const char * 	relative,
				   HTParentAnchor *	destination_base,
				   HTRequest *          request);

/*
(
  Save a Document Using an Anchor and the PUT Method
)

Get the source and PUT it to the destination which is an anchor object.
*/

extern BOOL HTPutDocumentAnchor (HTParentAnchor *	source,
				 HTAnchor *		destination,
				 HTRequest *	 	request);

/*
.
  Post a Document from Memory ASIS (Method = POST)
.

If you already have a document in memory and it is associated with an
Anchor object then you can POST this document
to a remote server using the following methods. Other information that you
can set in the anchor is metadata like the media type, the document length,
the language, or any other information that you want to associate with the
document to be uploaded.

This set of functions takes the document ASIS - that it the
exact content of the document associated with this anchor will be
sent to the remote server.

If your application is an editor, then you may want to create a new anchor
on the fly for temporary backups on local disk before you save it to a remote
server. An easy way to get a new anchor with a local file URI is to use the
HTTmpAnchor function which is part of the
WWWApp interface.
(
  Post a Document from Memory ASIS using Absolute URI (POST)
)

Upload a document using POST referenced by an absolute URI.
*/

extern BOOL HTPostAbsolute (HTParentAnchor *	source,
			   const char *		destination,
			   HTRequest *		request);

/*
(
  Post a Document from Memory ASIS using Relative URI (POST)
)

Upload a document using POST referenced by a relative URI.
*/

extern BOOL HTPostRelative (HTParentAnchor *	source,
			   const char * 	relative,
			   HTParentAnchor *	destination_base,
			   HTRequest *		request);

/*
(
  Post a Document from Memory ASIS using an Anchor (POST)
)

POST an anchor - here both the source and the anchor are anchor objects.
The source anchor already has the contents in memory.
*/

extern BOOL HTPostAnchor (HTParentAnchor *	source,
			 HTAnchor *		dest,
			 HTRequest *	 	request);

/*
.
  Get Available Options for a Document (Method = OPTIONS)
.

If you want to get information about a document then you can use the the
OPTIONS method in your request. The OPTIONS method
represents a request for information about the communication options available
on the request/response chain identified by the Request-URI.
This method allows the client to determine the options and/or requirements
associated with a resource, or the capabilities of a server, without implying
a resource action or initiating a resource retrieval.

A speciality about the OPTIONS method is that the client can
issue a request with no pathinfo at all but only with a "*".
That is, the request line can look like this "OPTIONS * HTTP/1.1".
This means that we request information about the server as a whole and not
only about a single URI. You can get this effect by using a URI containing
the hostname alone with NO extra slash at the end, for example
http://www.w3.org, http://www.cern.ch.
(
  Options Available for Document from Absolute URI
)

Request options about a document referencd by an absolute URI.
*/
extern BOOL HTOptionsAbsolute (const char * url, HTRequest * request);

/*
(
  Options Available for Document from Relative URI
)

Request options about a document referenced by a relative URI.
*/
extern BOOL HTOptionsRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request);

/*
(
  Options Available for Document using an Anchor
)

Here the URI is represented by an Anchor object.
You can get an anchor object representing a URI by passing the URI to the
appropriate method in the Anchor class.
*/
extern BOOL HTOptionsAnchor (HTAnchor * anchor, HTRequest * request);

/*
.
  Get Trace Loop back Information for a Document (Method =
  TRACE)
.

The TRACE method is used to invoke a remote, application-layer loop-back
of the request message. The final recipient of the request SHOULD reflect
the message received back to the client as the entity-body of a 200 (OK)
response. The final recipient is either the origin server or the first proxy
or gateway to receive a Max-Forwards value of zero (0) in the request (see
section 14.31). A TRACE request MUST NOT include an entity.

TRACE allows the client to see what is being received at the other end of
the request chain and use that data for testing or diagnostic information.
The value of the Via header field (section 14.44) is of particular interest,
since it acts as a trace of the request chain. Use of the Max-Forwards header
field allows the client to limit the length of the request chain, which is
useful for testing a chain of proxies forwarding messages in an infinite
loop.

If successful, the response SHOULD contain the entire request message in
the entity-body, with a Content-Type of "message/http". Responses
to this method MUST NOT be cached.
(
  Traces Available for Document from Absolute URI
)

Request traces about a document referencd by an absolute URI.
*/
extern BOOL HTTraceAbsolute (const char * url, HTRequest * request);

/*
(
  Traces Available for Document from Relative URI
)

Request traces about a document referenced by a relative URI.
*/
extern BOOL HTTraceRelative (const char * 	relative,
			     HTParentAnchor *	base,
			     HTRequest *	request);

/*
(
  Traces Available for Document using an Anchor
)

Here the URI is represented by an Anchor object.
You can get an anchor object representing a URI by passing the URI to the
appropriate method in the Anchor class.
*/
extern BOOL HTTraceAnchor (HTAnchor * anchor, HTRequest * request);

/*
.
  Serve a Request
.

Although libwww is primarily for clients, it is in fact symmetric in that
it can handle both client requests and server requests. The way this is handled
is that each protocol is registered with both a
client handler and a server handler - depending on which type of request
you use, one of them is called. Note that in order to be able to serve any
document, there actually have to be a server handler. However, libwww only
comes with a raw socket loader which isn't much
of a server. There is an attempt of an HTTP server
but it is not complete

The protocol handler used to serve the request is determined by the URI -
just as for client side requests. That is, libwww can in fact simultaneously
be the server for multiple protocols if you really want to. Examples of URIs
that you can use are noop://localhost:8888 which means that libwww
starts listening on port 8888 (see the listen
example for details). Other examples are http://localhost:7777
which means that it listens for HTTP on port 7777. Again, there is no HTTP
server in libwww - this is just an example.
*/

extern BOOL HTServeAbsolute (const char * address, HTRequest * request);

/*
.
  Save a URI To Multiple Destinations - Not supported!!!
.

Note: This is no longer supported

These are the generic versions of the PUT and POST
functions. They can be used to send documents to multiple destinations
simultanously using the PostWeb model.
(
  Copy an anchor - not supported
)

Fetch the URI from either local file store or from a remote HTTP server
and send it using either PUT or POST to the remote destination using HTTP.
The caller can decide the exact method used and which HTTP header fields
to transmit by setting the user fields in the request structure. If posting
to NNTP then we can't dispatch at this level but must pass the source anchor
to the news module that then takes all the refs to NNTP and puts into the
"newsgroups" header Returns YES if request accepted, else NO
*/
extern BOOL HTCopyAnchor (HTAnchor * src_anchor, HTRequest * main_req);

/*
(
  Upload an Anchor - not supported
)

This function can be used to send data along with a request to a remote server.
It can for example be used to POST form data to a remote HTTP server - or
it can be used to post a newsletter to a NNTP server. In either case, you
pass a callback function which the request calls when the remote destination
is ready to accept data. In this callback you get the current request object
and a stream into where you can write data. It is very important that you
return the value returned by this stream to the Library so that it knows
what to do next. The reason is that the outgoing stream might block or an
error may occur and in that case the Library must know about it. If you do
not want to handle the stream interface yourself then you can use the
HTUpload_callback which is declared below. The source anchor
represents the data object in memory and it points to the destination anchor
by using the POSTWeb method.
The source anchor contains metainformation about the data object in memory
and the destination anchor represents the reponse from the remote server.
Returns YES if request accepted, else NO
*/
extern BOOL HTUploadAnchor (HTAnchor *		source_anchor,
			    HTRequest * 	request,
			    HTPostCallback *	callback);

/*
(
  POST Callback Handler - not supported
)

Is you do not want to handle the stream interface on your own, you can use
this "middleman" function which does the actual writing to the target stream
for the anchor upload and also handles the return value from the stream.
Now, your application is called via the callback function that you may associate
with a request object. You indicate when you have sent all the data you want
by returning HT_LOADED from the callback.
*/

extern int HTUpload_callback (HTRequest * request, HTStream * target);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTACCESS_H */

/*

  

  @(#) $Id$

*/
