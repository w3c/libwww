/* 							            HTDigest.c
**	GENERIC INTERFACE TO MESSAGE DIGEST ALGORITHMS
**
**	(c) COPYRIGHT W3C/INRIA 1998.
**	Please first read the full copyright statement in the file COPYRIGHT.
**	@(#) $Id$
**
**	Contains a generic interface to the message digest algorithms, 
**      inspired from the RSA-Euro toolkit. For the moment, it only
**      covers MD5.
**      SHA and other algorithms could be added later on. This code
**      is only  used during message digest authentication.
**
** AUTHORS:
**	JKO	Jose Kahan       jose@w3.org
**
** HISTORY:
**	Dec 98  JKO	Created the module from scratch
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWLib.h"
#include "HTDigest.h"					 /* Implemented here */

PUBLIC BOOL HTDigest_init (HTDigestContext *context, int digesttype)
{
    context->algorithm = digesttype;

    switch (digesttype) {
      case HTDaMD5:
	  MD5Init (&context->context.md5);
	  break;
      default:
          return NO;
	  break;
    }
    return YES;
}

PUBLIC BOOL HTDigest_update (HTDigestContext *context, char *input, unsigned int inputLen)
{
    if (context) {
	switch (context->algorithm) {
	case HTDaMD5:
	    MD5Update (&context->context.md5, (unsigned char *) input,
		       inputLen);
	    break;
	default:
	    return NO;
	    break;
	}
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTDigest_final (unsigned char *digest, HTDigestContext *context)
{
    if (context) {
	switch (context->algorithm) {
	case HTDaMD5:
	    MD5Final (digest, &context->context.md5);
	    break;
	default:
	    return NO;
	    break;
	}
	return YES;
    }
    return NO;
}





