/*

  
  					W3C Sample Code Library libwww Content-Type Guessing Stream


!
  Content-Type Guessing Stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This interface provides functionality for guessing unknown media types from
magic words. The stream is a one that reads first a chunk of stuff, tries
to figure out the format, and calls HTStreamStack(). This is
a kind of lazy-evaluation of HTStreamStack().

This could be extended arbitrarily to recognize all the possible file formats
in the world, if someone only had time to do it.

This module is implemented by HTGuess.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTGUESS_H
#define HTGUESS_H

#include "HTStream.h"
#include "HTFormat.h"

#ifdef __cplusplus
extern "C" { 
#endif 

extern HTConverter HTGuess_new;

#ifdef __cplusplus
}
#endif

#endif	/* !HTGUESS_H */

/*

  

  @(#) $Id$

*/
