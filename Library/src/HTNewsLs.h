/*

  
  					W3C Sample Code Library libwww News/NNTP Listings


!
  News/NNTP Article and Group Listings
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module converts a generic News group listing
to a HTML object

This module is implemented by HTFTPDir.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTNEWSLS_H
#define HTNEWSLS_H
#include "HTStream.h"
#include "HTFormat.h"
#include "HTNet.h"

#ifdef __cplusplus
extern "C" { 
#endif 

extern HTConverter HTNewsList, HTNewsGroup;

/*
(
  Newsgroup List Cache
)


The news list cache uses a URL tree to store the information
*/

HTNetBefore HTNewsCache_before;
HTNetAfter HTNewsCache_after;

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTNEWSLS_H */

/*

  

  @(#) $Id$

*/
