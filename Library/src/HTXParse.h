/*

  
  					W3C Sample Code Library libwww Callback Stream


!
  XParse: Module to get Unparsed Stream from libwww
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This version of the stream object is a hook for clients that want an unparsed
stream from libwww. The HTXParse_put_* and HTXParse_write routines copy the
content of the incoming buffer into a buffer that is realloced whenever
necessary. This buffer is handed over to the client in HTXParse_free. See
also HTFWriter for writing to C files.
(
  Bugs:
)
	 
	   o 
	     strings written must be less than buffer size.
	 
	 
This module is implemented by HTXParse.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTXPARSE_H
#define HTXPARSE_H

#include "HTStream.h"
#include "HTReader.h"
#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTXParseStruct HTXParseStruct;

typedef void CallClient (HTXParseStruct * me);

struct _HTXParseStruct {
	CallClient	*call_client;
	int             used;         /* how much of the buffer is being used*/
	BOOL            finished;     /* document loaded? */
        int             length;       /* how long the buffer is */
	char *          buffer;       /* storage in until client takes over */
	char *          content_type;
	HTRequest *	request;      /* the request structure */
};

extern HTConverter HTXParse;

#ifdef __cplusplus
}
#endif

#endif  /* HTXPARSE_H */

/*

End of declaration
*/
