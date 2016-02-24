/*

  
  					W3C Sample Code Library libwww Content Negotiation


!
  Content Negotation
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

As part of the HTTP content negotiation, a server must be able to match the
preferences sent by the client in an HTTP request with the possible set of
documents that it has avaiable for this URL. For example , it may have an
English and a Danish version in which case it looks at the
Accept-Language header and sees what the client prefers. The
Library has a simple "Match" algorithm for finding the best as specified
by the HTTP specification. As the content algorithm is part of the File Interface
then all file access regardless of whether it is from a server or a client
application will be able to content negotiotion.

This module is implemented by HTMulti.c, and it is
a part of the  W3C Sample Code
Library
*/

#ifndef HTMULTI_H
#define HTMULTI_H

#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*

This function is used when the best match among several possible documents
is to be found as a function of the accept headers sent in the client request.
.
  Set the default Welcome page
.

Set default file name for welcome page on each directory.
*/
extern void HTAddWelcome (char * welcome_name);

/*
.
  Content Negotiation Algorithm
.

This function looks for a set of bindings between
a set of possible objects to be served on a request on the local file system.
*/

extern char * HTMulti (HTRequest *	req,
		      char *		path,
		      struct stat *	stat_info);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTMULTI_H */

/*

  

  @(#) $Id$

*/
