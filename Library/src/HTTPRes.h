/*

					W3C Sample Code Library libwww HTTP RESPONSE STREAM




!HTTP Response Stream!

*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The HTTP response stream generates a HTTP response header and writes
it to the target which is normally a HTWriter stream.

This module is implemented by HTTPRes.c, and
it is a part of the  W3C
Sample Code Library.

*/

#ifndef HTTPRES_H
#define HTTPRES_H

#include "HTStream.h"
#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*

(Streams Definition)

This stream makes a server specific HTTP header before it goes into
transparent mode. If endHeader is YES then we send an
empty CRLF in order to end the header.

*/

extern HTStream * HTTPResponse_new (HTRequest *	request, HTStream * target,
				    BOOL endHeader, int version);

/*

*/

#ifdef __cplusplus
}
#endif

#endif  /* HTTPRES_H */

/*



@(#) $Id$


*/
