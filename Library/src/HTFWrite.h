/*

  
  					W3C Sample Code Library libwww ANSI C File Streams


!
  Basic ANSI C File Writer Stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module contains a basic file writer stream that can be used to
dump data objects to local disk. See also the various subclasses of
this basic stream which can be used to do save-as, save-and-callback etc. 
in the HTFSave module

This module is implemented by HTFWrite.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTFWRITE_H
#define HTFWRITE_H

#include "HTStream.h"
#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Basic ANSI C File Writer Stream
.

This function puts up a new stream given an open file descripter.
If the file is not to be closed afterwards, then set
leave_open=NO.
*/

extern HTStream * HTFWriter_new	(
        HTRequest * request,
	FILE * fp,
	BOOL leave_open);

/*

*/

#ifdef __cplusplus
}
#endif

#endif  /* HTFWRITE_H */

/*

  

  @(#) $Id$

*/
