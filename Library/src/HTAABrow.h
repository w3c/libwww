/*

  					W3C Sample Code Library libwww Client Side Authentication Challenges
  and Credentials


!
  Client Side Authentication Challenges and Credentials
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

Contains code for parsing challenges and creating credentials for basic and
digest authentication schemes. See also the HTAAUtil module for how to handle
other authentication schemes. You don't have to use this code at all if you
better like to use your own functions for parsing challenges and generating
credentials. All functions are implemented as callback functions to the
Authentication Manager

This module is implemented by HTAABrow.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTAABROW_H
#define HTAABROW_H
#include "HTUTree.h"
#include "HTNet.h"

#ifdef __cplusplus
extern "C" { 
#endif 


/*
.
  Basic Authentication
.

This is the set of callback functions for handling basic authentication.
*/

extern HTNetBefore	HTBasic_generate;
extern HTNetAfter 	HTBasic_parse;
extern HTUTree_gc	HTBasic_delete;

/*
.
  Digest Authentication
.

This is the set of callback functions for handling digest authentication.
*/

extern HTNetBefore	HTDigest_generate;
extern HTNetAfter 	HTDigest_parse;
extern HTNetAfter       HTDigest_updateInfo;
extern HTUTree_gc	HTDigest_delete;


/*
*/

#ifdef __cplusplus
}
#endif

#endif	/* NOT HTAABROW_H */

/*

  

  @(#) $Id$

*/


