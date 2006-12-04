/*								   HTSQL.c
**	MYSQL INTERFACE MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module interacts with the MYSQL C client library to perform
**	SQL operations. It is not intended as a complete SQL API but handles
**	most of the typical error situations talking to an SQL server so that
**	the caller doesn't have to think about it.
**
** History:
**  	23 Apr 98	Henrik Frystyk, frystyk@w3.org
*/

/* Library include files */
#include "WWWLib.h"
#include "HTSQL.h"					 /* Implemented here */

#include <mysql.h>
#include <errmsg.h>

/* updates for to remove deprecated mysql functions */
#define mysql_connect(m,h,u,p) mysql_real_connect((m),(h),(u),(p),NULL,0,NULL,0) 
/* mysql documentation indicates that database names have a maximun of 65 characters */
#define CREATE_DB_BUFFER_SIZE 128
#define CREATE_DB_QUERY_FORMAT "create database %s"

struct _HTSQL {
    MYSQL		server;
    MYSQL *		psvr;
    const char *	host;
    const char *	user;
    const char *	password;		/* @@@ Should be scambled! @@@ */
    const char *	db;
};

/* ------------------------------------------------------------------------- */

PRIVATE int sql_datetimestr (char ** ptr, time_t t)
{
    int length = -1;
    if (t > 0 && *ptr) {
#if defined(HT_REENTRANT) || defined(SOLARIS)
	struct tm gmt;
	struct * pgmt = gmtime_r(&t, &gmt);
#else
	struct tm *pgmt = gmtime(&t);
#endif /* SOLARIS || HT_REENTRANT */

#ifdef HAVE_STRFTIME
    	length = strftime(*ptr, 40, "\'%Y-%m-%d %H:%M:%S\'", pgmt);
#else
	length = sprintf(*ptr, "\'%04d-%02d-%02d %02d:%02d:%02d\'",
		pgmt->tm_year + 1900,
		pgmt->tm_mon + 1,
		pgmt->tm_wday + 1,
		pgmt->tm_hour,
		pgmt->tm_min,
		pgmt->tm_sec);
#endif  /* HAVE_STRFTIME */

	/* Remove the  trailing zero */
	*ptr = *ptr + strlen(*ptr);
	**ptr++ = ' ';
    }
    return length;
}

PUBLIC char * HTSQL_printf (char * buf, int length, char * fmt, ...)
{
    va_list pArgs;
    char * p=fmt;
    char * q=buf;
    char * cpar;
    unsigned upar;
    long lpar;
    time_t tpar;
    va_start(pArgs, fmt);
    while (*p && length>0) {
	if (*p == '%') {
	    switch (*++p) {
	    case 's':
		if ((cpar = va_arg(pArgs, char *)) != NULL) {
		    char * cp = cpar;
		    while ((*q++ = *cp++) && length-->0); q--;
		} else {
		    *q++='n'; *q++='u'; *q++='l'; *q++='l';
		    length -= 4;
		}
		break;
	    case 'S':
		if ((cpar = va_arg(pArgs, char *)) != NULL) {
		    char * cp = cpar;
		    *q++='\'';
		    mysql_escape_string(q, cp, strlen(cp));
		    while (*q) q++, length--;
		    *q++='\'';
		} else {
		    *q++='n'; *q++='u'; *q++='l'; *q++='l';
		    length -= 4;
		}
		break;
	    case 'T':
		if ((tpar = va_arg(pArgs, time_t)) > 0) {
		    length -= sql_datetimestr(&q, tpar);
		} else {
		    *q++='n'; *q++='u'; *q++='l'; *q++='l';
		    length -= 4;
		}
		break;
	    case 'u':
		if ((upar = va_arg(pArgs, unsigned int)) >= 0) {
		    char num[32];
		    char * nptr = num;
		    int i;
		    do {
			i = upar % 10;
			*nptr++ = i+'0';
			upar /= 10;
		    } while (upar > 0);
		    while (nptr > num && length>0) *q++=*--nptr, length--;
		} else {
		    *q++='0';
		    length--;
		}
		break;
	    case 'l':
		if ((lpar = va_arg(pArgs, unsigned long)) >= 0) {
		    char num[32];
		    char * nptr = num;
		    int i;
		    do {
			i = lpar % 10;
			*nptr++ = i+'0';
			lpar /= 10;
		    } while (lpar > 0);
		    while (nptr > num && length>0) *q++=*--nptr, length--;
		} else {
		    *q++='0';
		    length--;
		}
		break;
	    default:
		*q++=*p++;
		length--;
	    }
	    p++;
	} else {
	    *q++=*p++;
	    length--;
	}
    }
    *q = '\0';
    va_end(pArgs);
    return buf;
}

PUBLIC HTSQL * HTSQL_new (const char * host,
			  const char * user, const char * pw,
			  int flags)
{
    HTSQL * me = NULL;
    if (!host || !user || !pw) {
	HTTRACE(SQL_TRACE, "SQL new..... Missing host, user, or password\n");
	return NULL;
    }
    if ((me = (HTSQL *) HT_CALLOC(1, sizeof(HTSQL))) == NULL)
        HT_OUTOFMEM("HTSQL_new");
    me->host = host;
    me->user = user;
    me->password = pw;		/* @@@ scramble!!! @@@ */
    return me;
}

PUBLIC BOOL HTSQL_delete (HTSQL * me)
{
    if (me) {
	if (me->psvr) HTSQL_close(me);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTSQL_connect (HTSQL * me)
{
    me->psvr = mysql_init( &(me->server) );
    if (me && me->host) {
	HTTRACE(SQL_TRACE, "SQL connect. Open a link to server `%s\'\n" _ me->host);
	me->psvr = mysql_real_connect( &(me->server),
				       me->host,
				       me->user ? me->user : "",
				       me->password ? me->password : "",
				       NULL,0,NULL,0) ;
	if ( NULL == me->psvr ) {
	    HTTRACE( SQL_TRACE, 
		     "SQL connect. `%s\' errno %d\n" _ 
		     mysql_error( &(me->server) ) _ mysql_errno( &(me->server) ) );
	    return NO;
	}
	return YES;
    }
    return NO;
}

PUBLIC MYSQL * HTSQL_getMysql (HTSQL * me)
{
    return me ? me->psvr : NULL;
}

PUBLIC BOOL HTSQL_close (HTSQL * me)
{
    if (me && me->psvr) {
	HTTRACE(SQL_TRACE, "SQL close... Link to host `%s\'\n" _ me->host);
	mysql_close(me->psvr);
	me->psvr = NULL;
	me->db = NULL;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTSQL_version (HTSQL *		me,
			   char **		server,
			   unsigned int * 	protocol_version,
			   char **		server_version,
			   char **		client_version)
{
    if (me && me->psvr) {
	if (*server) *server = mysql_get_server_info(me->psvr);
	if (protocol_version) *protocol_version = mysql_get_proto_info(me->psvr);
	if (*server_version) *server_version = mysql_get_server_info(me->psvr);
	if (*client_version) *client_version = mysql_get_client_info();
	return YES;
    }
    return NO;
}


PUBLIC BOOL HTSQL_selectDB (HTSQL * me, const char * db)
{
    BOOL created = NO;
    if (me && me->psvr && db) {
	HTTRACE(SQL_TRACE, "SQL select.. Database `%s\'\n" _ db);
	me->db = NULL;
	if (mysql_select_db(me->psvr, db) < 0) {
	    int err = mysql_errno(me->psvr);
	    HTTRACE(SQL_TRACE, "SQL select.. `%s\', errno %d\n" _ 
				   mysql_error(me->psvr) _ err);

	    /* If the database couldn't be found then create a new one */
	    if (err == 1049) {
	      char CREATE_DB_BUFFER[CREATE_DB_BUFFER_SIZE];
	      snprintf( CREATE_DB_BUFFER, 
			(size_t)CREATE_DB_BUFFER_SIZE,
			CREATE_DB_QUERY_FORMAT,
			db );
		if (mysql_query(me->psvr, CREATE_DB_BUFFER) < 0) {
		    HTTRACE(SQL_TRACE, "SQL error... `%s\', errno %d\n" _ 
					   mysql_error(me->psvr) _ err);
		    return NO;
		}
		created = YES;
	    } else
		return NO;
	}

	if (created) {
	    if (mysql_select_db(me->psvr, db) < 0) {
		int err = mysql_errno(me->psvr);
		HTTRACE(SQL_TRACE, "SQL select.. `%s\', errno %d\n" _ 
				       mysql_error(me->psvr) _ err);
		return NO;
	    }
	}

	/* The database is now up and running */
	me->db = db;
	return YES;
    } 
    return NO;
}

PUBLIC BOOL HTSQL_query (HTSQL * me, const char * query)
{
    HTTRACE(SQL_TRACE, "SQL query... `%s\'\n" _ query ? query : "<null>");
    if (me && me->psvr && query) {
	if (mysql_query(me->psvr, query) < 0) {
	    int status = mysql_errno(me->psvr);
	    HTTRACE(SQL_TRACE, "SQL query... `%s\' on query `%s\' with errno %d\n" _ 
			mysql_error(me->psvr) _ query _ status);

	    /* Check to see if we should try and reconnect */
	    if (status==CR_SERVER_GONE_ERROR || status==CR_SERVER_LOST) {
		if (HTSQL_connect(me)==NO || HTSQL_selectDB(me, me->db)==NO)
		    return NO;
		return (mysql_query(me->psvr, query) == 0);
	    }
	    return NO;
	}
	return YES;
    }
    return NO;
}

PUBLIC int HTSQL_getLastInsertId (HTSQL * me)
{
    return (me && me->psvr) ? mysql_insert_id(me->psvr) : -1;
}

PUBLIC int HTSQL_GetAffectedRows (HTSQL * me)
{
    return (me && me->psvr) ? mysql_affected_rows(me->psvr) : -1; 
}
 

PUBLIC MYSQL_RES * HTSQL_storeResult (HTSQL * me)
{
    MYSQL_RES * result = NULL;
    if (me && me->psvr) {
	if ((result = mysql_store_result(me->psvr)) == NULL) {
	    int status = mysql_errno(me->psvr);
	    HTTRACE(SQL_TRACE, "SQL store... `%s\' with errno %d\n" _ 
			mysql_error(me->psvr) _ status);
	}
    }
    return result;
}

PUBLIC BOOL HTSQL_freeResult (MYSQL_RES * me)
{
    if (me) {
	mysql_free_result(me);
	return YES;
    }
    return NO;
}
