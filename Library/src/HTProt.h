
/*



					W3C Sample Code Library libwww Protocol Class



!The Protocol Class!
*/
/*
**        (c) COPYRIGHT MIT 1995.
**        Please first read the full copyright statement in the file COPYRIGH.
*/
/*

The Protocol class defines an application level protocol (HTTP, FTP,
Gopher, etc.) to be used by libwww. Note that access to the local file system
also is considered to be an appliaction level protocol treated identically to
for example the HTTP protocol.

The Protocol class does only know about the application layer protocol and
it relies on the Transport Class to do the actualt
communication with the network, the local file system etc. The protocol class
defines an access method for both a client and a server. A typical client
application would probably only want to use the client method and a server
only the server method. However, any application can use both which allows it
to seemlessly to change between server and client profile as needed.

All protocol modules are dynamically bound to an access scheme. Take for
example the address http://www.w3.org which has the access scheme
http and if we have a protocol module capable of handling HTTP then we
can make the binding between http and this module. As mentioned in the
introduction to this chapter, the Library already comes with a large set of
protocol module, including HTTP so all we have to do in this case is to
register the HTTP module to the Library as being capable of handling
http URLs.

Libwww comes with a default set of protocols including the ones mentioned
above which can be initiated using the function HTProtocolInit()
in HTInit module

One special case is the support for access to WAIS
databases. WAIS has its own code Library
called freeWAIS which is required in order
to directly access wais URLs. We shall not describe in describe in detail here
how this can be enabled as it is described in the WWW-WAIS gateway.

This module is implemented by HTProt.c, and it is a
part of the  W3C Sample Code
Library.
*/

#ifndef HTPROT_H
#define HTPROT_H

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTProtocol HTProtocol;
typedef u_short HTProtocolId;

#include "HTReq.h"
#include "HTAnchor.h"
#include "HTEvent.h"
#include "HTTrans.h"
/*

An access scheme module takes as a parameter a socket (which is an invalid
socket the first time the function is called), a request structure containing details of the request,
and the action by which the (valid) socket was selected in the event loop.
When the protocol class routine is called, the anchor element in the request
is already valid (made valid by HTAccess).

.Creation and Deletion Methods.

(Add an Protocol)

This functions registers a protocol module and binds it to a specific
access acheme (the part before the first colon in a URL). For example, the
HTTP &nbsp;client module is bound to http URLs. The callback function is the
function to be called for loading.
*/
typedef int HTProtCallback (SOCKET, HTRequest *);

extern BOOL HTProtocol_add (const char *               name,
                            const char *        transport,
                            HTProtocolId        port,
                            BOOL                preemptive,
                            HTProtCallback *        client,
                            HTProtCallback *        server);
/*

(Delete a Protocol)

This functions deletes a registered protocol module so that it can not be
used for accessing a resource anymore.
*/
extern BOOL HTProtocol_delete (const char * name);
/*

(Remove ALL Registered Protocols)

This is the garbage collection function. It is called by HTLibTerminate()
*/
extern BOOL HTProtocol_deleteAll (void);
/*

.Protocol Class Methods.

(Find a Protocol Object)

You can search the list of registered protocol objects as a function of the
access acheme. If an access scheme is found then the protocol object is
returned.
*/
extern HTProtocol * HTProtocol_find (HTRequest * request, const char * access);
/*

(Get the callback functions)

You can get the callback functions registered together with a protocol
object using the following methods.
*/
extern HTProtCallback * HTProtocol_client (HTProtocol * protocol);
extern HTProtCallback * HTProtocol_server (HTProtocol * protocol);
/*

(Get the default Protocol ID)

Each protocol is registered with a default protocol ID which is the default
port number that this protocol is using. In the case of FTP it is 21, for
HTTP, it is 80 and for NNTP it is 119.
*/
extern HTProtocolId HTProtocol_id (HTProtocol * protocol);
/*

(Get the Protocol Name)

Get the protocol name that was registered when the protocol object was
created
*/
extern const char * HTProtocol_name (HTProtocol * protocol);
/*

(Is Access Scheme Preemptive)

Returns YES if the implementation of the access scheme supports preemptive
access only.
*/
extern BOOL HTProtocol_preemptive (HTProtocol * protocol);
/*

(Binding to the Transport Class)

An application protocol is registered together with a transport protocol in order to communicate with the
thansport layer.
*/
extern BOOL HTProtocol_setTransport (HTProtocol * protoccol,
                                     const char * transport);
extern const char * HTProtocol_transport (HTProtocol * protocol);
/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTPROT_H */

/*




@(#) $Id$ 

*/
