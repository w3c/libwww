/*

					W3C Sample Code Library libwww Log Class



!Log Manager!

*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This is a generic log object which can be used to log events to a file.

This module is implemented by HTLog.c, and it is
a part of the  W3C
Sample Code Library.

*/

#ifndef HTLIBLOG_H
#define HTLIBLOG_H

#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*

.Create a new Log Object.

Create a new object and open the log file. The time used in the log file
is either GMT or local dependent on local.

*/

typedef struct _HTLog HTLog;

extern HTLog * HTLog_open (const char * filename, BOOL local, BOOL append);

/*

.Delete a Log Object.

Close the log file and delete the object

*/

extern BOOL HTLog_close (HTLog * log);

/*

.How many times has log object been accessed?.

Returns access count number or -1

*/

extern int HTLog_accessCount (HTLog * log);

/*

.Log a Client Request in CLF.

This functions logs the result of a request in what's close to CLF. It can 
be used on client side to track user behavior.

*/

extern BOOL HTLog_addCLF (HTLog * log, HTRequest * request, int status);

/*

.Log Referer Fields.

This functions logs the referer logs of where the user has been.

*/

extern BOOL HTLog_addReferer (HTLog * log, HTRequest * request, int status);

/*

.Log the following line.

A generic logger - logs whatever you put in as the line. The caller
is responsible for adding a line feed if desired.

*/

extern BOOL HTLog_addLine (HTLog * log, const char * line);

/*

.Log the Following Variable Arguments.

A generic logger with variable arguments

*/

extern BOOL HTLog_addText (HTLog * log, const char * fmt, ...);

/*

*/

#ifdef __cplusplus
}
#endif

#endif  /* HTLIBLOG_H */

/*



@(#) $Id$


*/
