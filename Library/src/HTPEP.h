/*


  					W3C Sample Code Library libwww PEP Engine


!
  PEP Engine
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The PEP Manager is a registry for PEP Protocols that follow
the generic syntax defined by the HTTP PEP
protocol headers. All PEP Protocols are registered at run-time
in form of a PEP Module. A PEP Module consists of the following:

  
    protocol
  
    The name which is used to identify the protocol.
  
    BEFORE Filter
  
    When a new request is issued, the PEP Manager looks in the URL tree
    to see if we have any PEP information for this particular request. The search
    is based on the realm (if known) in which the request belongs and the URL
    itself. If a record is found then the PEP Manager calls the PEP
    Module in order to generate the PEP protocol headers.
  
    AFTER Filter
  
    After a request has terminated and the result was lack of required PEP protocol
    headers, the request should normally be repeated with a new set of PEP protocol
    headers. The AFTER filter is responsible for extracting the challenge from
    the HTTP response and store it in the URL tree, so that we next time we request
    the same URL we know that it is protected and we can ask the user for the
    appropriate PRP protocol headers.
  
    garbage collection
  
    The PEP information is stored in a URL Tree but
    as it doesn't know the format of the protocol specific parts, you must register
    a garbage collector (gc). The gc is called when node is deleted in the tree.


Note: The PEP Manager itself consists of BEFORE and
an AFTER filter - just like the PEP
Modules. This means that any PEP Module also can be registered
directly as a BEFORE and AFTER
filter by the Net Manager.
The reason for having the two layer model is that the PEP Manager
maintains a single URL tree for storing PEP
information for all PEP Protocols.

A PEP Module has three resources, it can use when creating PEP protocol
headers:
	 
	   o 
	     Handle the PEP protocol headers send from the remote party (typically
    in the form of a HTTP response.
  o 
	     Handle the PEP protocol headers which typically are to part of the
    next request.
  o 
	     Add information to the URL Tree

	 
This module is implemented by HTPEP.c (get it?), and
it is a part of the  W3C Sample Code
Library.
*/

#ifndef HTPEP_H
#define HTPEP_H
#include "HTList.h"
#include "HTReq.h"
#include "HTUTree.h"

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTPEPModule HTPEPModule;

/*
.
  PEP Module Registration
.

A PEP Protocol is registered by registering an PEP Module to
in the PEP Manager.
(
  Add a PEP Module
)

You can add a PEP protocol by using the following method. Each of the callback
function must have the type as defined below.
*/

extern HTPEPModule * HTPEP_newModule(const char *	protocol,
				     HTNetBefore *	before,
			             HTNetAfter *	after,
				     HTUTree_gc *	gc);

/*
(
  Find a PEP Module
)
*/
extern HTPEPModule * HTPEP_findModule (const char * protocol);

/*
(
  Delete a PEP Module
)
*/
extern BOOL HTPEP_deleteModule (const char * protocol);

/*
(
  Delete ALL PEP modules
)
*/
extern BOOL HTPEP_deleteAllModules (void);

/*
.
  Handling the URL Tree
.

The PEP information is stored as URL Trees. &nbsp;The
root of a URL Tree is identified by a hostname and a port number.
Each URL Tree contains a set of templates and realms which can be used to
predict what information to use in a hierarchical tree.

The URL trees are automatically collected after some time so the application
does not have to worry about freeing the trees. When a node in a tree is
freed, the gc registered as part of the PEP Module is called.

Server applications can have different PEP setups for each hostname and port
number, they control. For example, a server with interfaces
"www.foo.com" and "internal.foo.com" can have different
protection setups for each interface.
(
  Add information to the Database
)

Add a PEP information node to the database. If the entry is already found
then it is replaced with the new one. The template must follow normal URI
syntax but can include a wildcard Return YES if added (or replaced), else
NO
*/
extern HTPEPModule * HTPEP_findModule (const char * name);

/*
.
  The PEP Manager Filters
.

As mentioned, the PEP Manager is itself a set of
filters that can be registered by the
Net manager.
*/

extern HTNetBefore HTPEP_beforeFilter;
extern HTNetAfter  HTPEP_afterFilter;

/*
*/

#ifdef __cplusplus
}
#endif

#endif	/* NOT HTPEP_H */

/*

  

  @(#) $Id$

*/
