/*

  
  					W3C Sample Code Library libwww Generic String Management


!
  Memory Buffered Logging
!
*/

/*
**	(c) COPYRIGHT MIT 199G.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

These functions provide functionality for memory buffered
logging. This is usefull for tracking data read and written without
affecting the timing of the network traffic.

This module is implemented by HTMemLog.c, and it
is a part of the W3C
Sample Code Library.
*/

#ifndef HTMEMLOG_H
#define HTMEMLOG_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  The Whole Deal
.
Too much to do right now.

*/

extern int HTMemLog_open (char * logName, size_t size, BOOL keepOpen);
extern int HTMemLog_add (char * buf, size_t len);
extern int HTMemLog_flush (void);
extern void HTMemLog_close (void);
extern HTTraceDataCallback HTMemLog_callback;

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* !HTMEMLOG_H */

/*

  

  @(#) $Id$

*/
