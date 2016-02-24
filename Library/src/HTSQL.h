/*

  					W3C Sample Code Library libwww SQL API


!
  Simple SQL API
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module interacts with the MYSQL C client library to perform SQL operations.
It is not intended as a complete SQL API but handles most of the typical
error situations talking to an SQL server so that the caller doesn't have
to think about it.

This requires that you have linked against a MySQL
library. See the installation instructions
for details.

This module is implemented by HTSQL.c, and it is a
part of the  W3C Sample Code Library.
*/

#ifndef HTSQL_H
#define HTSQL_H

#include <mysql.h>

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  The HTSQL Object
.
*/

typedef struct _HTSQL HTSQL;

extern HTSQL * HTSQL_new (const char * host,
			  const char * user, const char * pw,
			  int flags);
extern BOOL HTSQL_delete (HTSQL * me);

/*
.
  Handling the link to the Server
.
(
  Open a connection to the SQL server
)
*/

extern BOOL HTSQL_connect (HTSQL * me);

/*
(
  Close the link to the database
)
*/

extern BOOL HTSQL_close (HTSQL * me);

/*
(
  You can get information about whom we are talking to by calling this function:
)
*/

extern BOOL HTSQL_version (HTSQL *		me,
			   char **		server,
			   unsigned int * 	protocol_version,
			   char **		server_version,
			   char **		client_version);

/*
(
  Selecting the current database
)
*/

extern BOOL HTSQL_selectDB (HTSQL * me, const char * db);

/*
(
  Getting the raw MYSQL object
)

After you have connected you can get the raw MYSQL object by
calling this function
*/

extern MYSQL * HTSQL_getMysql (HTSQL * me);

/*
.
  SQL printf
.

When creating queries to send to the SQL server you need to generate a lot
of SQL specific strings. This function knows most of the commonly used SQL
ways of handling date and time string, escaping and quoting strings and how
to write integers as strings. The function works pretty much as fprintf with
the following possible format arguments:

  
    %s
  
    Writes a string as is. No quotation or escaping is performed. NULL is written
    as "null".
  
    %S
  
    Writes a string in quotes and escapes any special SQL characters. NULL is
    written as "null".
  
    %T
  
    Creates an SQL datetime stamp from a time_t variable looking
    like this YYYY-MM-DD HH:MM:SS. -1 is written as
    "null"
  
    %u
  
    Unsigned integer
  
    %l
  
    Unsigned long integer

*/

extern char * HTSQL_printf (char * buf, int length, char * fmt, ...);

/*
.
  Issuing a Query
.

You can issue a query using this function. If the connection to the server
was dropped then it automatically tries to reconnect
*/

extern BOOL HTSQL_query (HTSQL * me, const char * query);

/*

Get information about the last inserted row's unique ID or how many rows
were affected by the last query:
*/

extern int HTSQL_getLastInsertId (HTSQL * me);
extern int HTSQL_GetAffectedRows (HTSQL * me);

/*
.
  Handle Query Results
.

Call this funciton to store the SQL query result
*/

extern MYSQL_RES * HTSQL_storeResult (HTSQL * me);

/*

When you are done with a query result then call this to clean up
*/

extern BOOL HTSQL_freeResult (MYSQL_RES * me);

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTSQL_H */

/*

  

  @(#) $Id$

*/
