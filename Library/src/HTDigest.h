/*

  					W3C Sample Code Library libwww Generic Message Digest
  Interface


!
  Generic Message Digest Interface
!
*/

/*
**	(c) COPYRIGHT W3C, INRIA 1998.
**	Please first read the full copyright statement in the file COPYRIGHT.
*/

/*


Contains a generic interface to the message digest algorithms, inspired
from the RSA-Euro toolkit. For the moment, it only covers MD5.
SHA and other algorithms could be added later on. This code is only
used during message digest authentication.

AUTHORS:
   JK	Jose Kahan       jose@w3.org

HISTORY:
        Dec 98 JK	Created the module from scratch


This module is implemented by HTDigest.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTDigest_H
#define HTDigest_H
/* Library include files */
#include "WWWLib.h"
/* add the MD algorithm header files here below */
#include "md5.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Interface to message digest algorithms
.

The list of known message digest algorithms
*/

#define HTDaMD5 1

/*

This is our general digest context structure. If you add a new
algorithm, include its context in the union.
*/

typedef struct _HTDigestContext {
  int algorithm;          
    union { 
           MD5_CTX md5;
    } context;
} HTDigestContext;

/*

This is a set of generic functions for generating a message digest
*/

extern BOOL HTDigest_init (HTDigestContext *context, int digesttype);
extern BOOL HTDigest_update (HTDigestContext *context, char *input, unsigned int inputLen);
extern BOOL HTDigest_final (unsigned char *digest, HTDigestContext *context);

/*
*/

#ifdef __cplusplus
}
#endif

#endif	/* NOT HTDigest_H */

/*

  

  @(#) $Id$

*/





