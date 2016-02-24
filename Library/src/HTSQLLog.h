/*

  					W3C Sample Code Library libwww SQL Log Class


!
  SQL Log Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This SQL based log class generates a SQL database and a set of tables storing
the results of a request. The result is stored in different tables depending
on whether it is information about the request or the resource returned.
The module uses the simple libwww SQL interface

This requires that you have linked against a MySQL
library. See the installation instructions
for details.

This module is implemented by HTSQLLog.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTSQLLOG_H
#define HTSQLLOG_H

#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Open and Close the Logs
.

Create a new SQLLog object and connect to the SQL server.
*/

typedef struct _HTSQLLog HTSQLLog;

typedef enum _HTSQLLogFlags {
    HTSQLLOG_CLEAR_URIS_TABLE 		= 0x1,
    HTSQLLOG_CLEAR_LINKS_TABLE		= 0x2,
    HTSQLLOG_CLEAR_REQUESTS_TABLE	= 0x4,
    HTSQLLOG_CLEAR_RESOURCES_TABLE	= 0x8,
    HTSQLLOG_DROP_URIS_TABLE 		= 0x10,
    HTSQLLOG_DROP_LINKS_TABLE		= 0x20,
    HTSQLLOG_DROP_REQUESTS_TABLE	= 0x40,
    HTSQLLOG_DROP_RESOURCES_TABLE	= 0x80
} HTSQLLogFlags; 

extern HTSQLLog * HTSQLLog_open (const char * 	host,
				 const char * 	user,
				 const char * 	pw,
				 const char * 	db,
				 HTSQLLogFlags 	flags);

/*
.
  Close Connection to the SQL Server
.

Close the log file and delete the log object
*/

extern BOOL HTSQLLog_close (HTSQLLog * me);

/*
.
  Write Logdata to the Database
.
(
  Add a Log Entry
)
*/

extern BOOL HTSQLLog_addEntry (HTSQLLog * me, HTRequest * request, int status);

/*
(
  Add a Link Relationship Entry
)
*/

extern BOOL HTSQLLog_addLinkRelationship (HTSQLLog * me,
					  const char * src_uri,
					  const char * dst_uri,
					  const char * link_type,
                                          const char * comment);

/*
.
  Options and Flags
.
(
  Make URIs Relative to this Base
)

Instead of inserting the absolute URI then you can log relative URIs instead
which often saves a lot of space. Set the base URI using this function
*/

extern BOOL HTSQLLog_makeRelativeTo (HTSQLLog * me, const char * relative);

/*
(
  How many times has this Log Object Been Accessed?
)

This has nothing to do with the SQL database but merely returns the access
count number to the log or -1 if error.
*/

extern int HTSQLLog_accessCount (HTSQLLog * me);

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTSQLLOG_H */

/*

  

  @(#) $Id$

*/
