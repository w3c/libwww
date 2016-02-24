/*

  					W3C Sample Code Library libwww SQL Interface


!
  W3C Sample Code Library libwww SQL Interface
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module is an easy to use interface to SQL databases. It contains both
a generic interface and some specific examples of how this can be used to
connect a Web client to an SQL server. This requires that you have linked
against the MySQL library. See the
installation instructions for details.
*/

#ifndef WWWSQL_H
#define WWWSQL_H

/*
*/

#ifdef __cplusplus
extern "C" { 
#endif

/*
(
  System dependencies
)

The wwwsys.h file includes system-specific include
files and flags for I/O to network and disk. The only reason for this file
is that the Internet world is more complicated than Posix and ANSI.
*/
#include "wwwsys.h"

/*
(
  Basic SQL Interface
)

This module interacts with the MYSQL C client library to perform SQL operations.
It is not intended as a complete SQL API but handles most of the typical
error situations talking to an SQL server so that the caller doesn't have
to think about it.
*/

#ifdef HT_MYSQL
#include "HTSQL.h"
#endif

/*
(
  SQL Client Side Logging
)

This SQL based log class generates a SQL database and a set of tables storing
the results of a request. The result is stored in different tables depending
on whether it is information about the request or the resource returned.
*/

#ifdef HT_MYSQL
#include "HTSQLLog.h"
#endif

/*
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif
#endif /* WWWSQL_H */

/*

  

  @(#) $Id$

*/
