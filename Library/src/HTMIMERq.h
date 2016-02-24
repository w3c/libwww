/*

					W3C Sample Code Library libwww MIME REQUEST STREAM




!MIME Request Stream!

*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The MIME Request stream generates a MIME request header and writes it
to the target which is normally a HTWriter stream.

This module is implemented by HTMIMERq.c, and
it is a part of the  W3C
Sample Code Library.

*/

#ifndef HTMIMERQ_H
#define HTMIMERQ_H

#include "HTStream.h"
#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*

(Streams Definition)

This stream makes a MIME header before it goes into transparent
mode. If endHeader is YES then we send an empty
CRLF in order to end the header.

*/

extern HTStream * HTMIMERequest_new    (HTRequest * request, HTStream * target,
					BOOL endHeader);

#ifdef __cplusplus
}
#endif

#endif  /* HTMIMERQ_H */

/*



@(#) $Id$


*/
