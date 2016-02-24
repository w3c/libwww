/*

  
  
  
  					W3C Sample Code Library libwww Callback Stream Callbacks


!
  External Parser Callbacks
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

An interface between the XParse module and the
Application. This module contains the interface between the XParse module
and the client. The dummy function is only here so that clients that use
the XParse module can overwrite it. See also
HTXParse

This module is implemented by HTEPtoCl.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTEPTOCLIENT_H
#define HTEPTOCLIENT_H

#include "HTStream.h"
#include "HTXParse.h"

#ifdef __cplusplus
extern "C" { 
#endif 

extern CallClient HTCallClient;


#ifdef __cplusplus
}
#endif

#endif  /* HTEPTOCLIENT_H */

/*

  

  @(#) $Id$

*/
