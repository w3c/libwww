/*								   HTSQL.c
**	SQL LOGGING MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module contains a simple SQL based logging mechanism for requests
**	and anything else you want to log
**
** History:
**  	23 Apr 98	Henrik Frystyk, frystyk@w3.org
*/

/* Library include files */
#include "WWWLib.h"
#include "HTSQLLog.h"					 /* Implemented here */

#include <mysql.h>

struct _HTSQLLog {
    MYSQL		server;
    MYSQL *		psvr;
    const char *	host;
    const char *	user;
    const char *	password;		/* @@@ Should be scambled! @@@ */
    char *		relative;		/* Make URIs relative to */
    int			accesses;
};

#define DEFAULT_SQL_URIS_TABLE		"uris"
#define DEFAULT_SQL_REQUESTS_TABLE	"requests"
#define DEFAULT_SQL_RESOURCES_TABLE	"resources"
#define DEFAULT_SQL_LINKS_TABLE		"links"

#define DEFAULT_SQL_KEY_TYPE		"int unsigned not null "
#define MAX_URI_LENGTH			"255"

#define SHOWLOG		1
#undef SQL_DEBUG	

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

PRIVATE char * sql_printf (char * buf, int length, char * fmt, ...)
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

PRIVATE int find_uri(HTSQLLog * me, const char * uri)
{
    int index = -1;
    if (me && me->psvr && uri) {
	char buf[1024];
        char * query = NULL;
        MYSQL_RES * result = NULL;

	/* Make the query */
	query = sql_printf(buf, 1024, "select * from %s where uri=%S",
			   DEFAULT_SQL_URIS_TABLE, uri);
#if SQL_DEBUG
	fprintf(stderr, "%s\n", query);
#endif
	if (!mysql_query(me->psvr, query) &&
	    (result = mysql_store_result(me->psvr)) != NULL) {
	    MYSQL_ROW row;
	    if ((row = mysql_fetch_row(result)) && row[0])
		index = atoi(row[0]);
	    mysql_free_result(result);
	}
    }
    return index;
}

PRIVATE int add_uri (HTSQLLog * me, const char * uri)
{
    if (me && me->psvr && uri) {
	int index = -1;
	char * rel = me->relative ? HTRelative(uri, me->relative) : NULL;

	/* If we can't find the URI then add it */
	if ((index = find_uri(me, rel ? rel : uri)) < 0) {
	    char buf[1024];
	    char * query = NULL;
	    query = sql_printf(buf, 1024, "insert into %s (uri) values (%S)",
			       DEFAULT_SQL_URIS_TABLE, rel ? rel : uri);
#ifdef SQL_DEBUG
	    fprintf(stderr, "%s\n", query);
#endif
	    if (mysql_query(me->psvr, query) < 0) {
		if (SHOWLOG) HTTrace("SQLLog...... `%s\' on query `%s\' with errno %d\n",
				     mysql_error(me->psvr), query,mysql_errno(me->psvr));
		HT_FREE(rel);
		return -1;
	    }
	    index = mysql_insert_id(me->psvr);
	}
	HT_FREE(rel);
	return index;
    }
    return -1;
}

PRIVATE BOOL add_linktype (HTSQLLog * me, int srcidx, int dstidx,
			   const char * type, const char * comment)
{
    if (me && me->psvr && srcidx>=0 && dstidx>=0 && type) {
	char buf[1024];
	char * query = NULL;
	query = sql_printf(buf, 1024, "insert into %s values (%u,%u,%S,%S)",
			   DEFAULT_SQL_LINKS_TABLE,
			   srcidx, dstidx, type, comment);
#ifdef SQL_DEBUG
	fprintf(stderr, "%s\n", query);
#endif
	if (mysql_query(me->psvr, query) < 0 && mysql_errno(me->psvr) != 1062) {
	    if (SHOWLOG) HTTrace("SQLLog...... `%s\' on query `%s\' with errno %d\n",
				 mysql_error(me->psvr), query, mysql_errno(me->psvr));
	    return NO;
	}
	return YES;
    }
    return NO;
}

PRIVATE BOOL clear_table (HTSQLLog * me, const char * table)
{
    BOOL status = NO;
    char * query = NULL;
    if (me && me->psvr && table) {
	if (SHOWLOG) HTTrace("SQLLog...... Clearing table `%s\'\n", table);
	query = StrAllocMCopy(&query, "delete from ", table, NULL);
#ifdef SQL_DEBUG
	fprintf(stderr, "%s\n", query);
#endif
	if (mysql_query(me->psvr, query) < 0) {
	    if (SHOWLOG) HTTrace("SQLLog...... `%s\' on query `%s\' with errno %d\n",
				 mysql_error(me->psvr), query, mysql_errno(me->psvr));
	} else 
	    status = YES;
    }
    HT_FREE(query);
    return status;
}

PRIVATE BOOL drop_table (HTSQLLog * me, const char * table)
{
    BOOL status = NO;
    char * query = NULL;
    if (me && me->psvr && table) {
	if (SHOWLOG) HTTrace("SQLLog...... Clearing table `%s\'\n", table);
	query = StrAllocMCopy(&query, "drop table ", table, NULL);
#ifdef SQL_DEBUG
	fprintf(stderr, "%s\n", query);
#endif
	if (mysql_query(me->psvr, query) < 0) {
	    if (SHOWLOG) HTTrace("SQLLog...... `%s\' on query `%s\' with errno %d\n",
				 mysql_error(me->psvr), query, mysql_errno(me->psvr));
	} else 
	    status = YES;
    }
    HT_FREE(query);
    return status;
}

PRIVATE BOOL createTables (HTSQLLog * me, HTSQLLogFlags flags)
{
    if (me && me->psvr) {
	char * query = NULL;
	if (SHOWLOG) HTTrace("SQLLog...... Creating log tables\n");

	/* If we have to delete it first */
	if (flags & HTSQLLOG_DROP_URIS_TABLE) 
	    drop_table(me, DEFAULT_SQL_URIS_TABLE);

	/* Create URI table (which is the index) */
	query = StrAllocMCopy(&query, "create table ", DEFAULT_SQL_URIS_TABLE, " ( ",
			      "id ", DEFAULT_SQL_KEY_TYPE, " auto_increment, ",
			      "uri varchar(", MAX_URI_LENGTH, ") binary not null, ",
			      "primary key (id), "
			      "unique (uri), ",
			      "index uri_idx (uri(32))",
			      ")", NULL);
#ifdef SQL_DEBUG
	fprintf(stderr, "%s\n", query);
#endif
	if (mysql_query(me->psvr, query) < 0) {
	    if (SHOWLOG) HTTrace("SQLLog...... `%s\' on query `%s\' with errno %d\n",
				 mysql_error(me->psvr), query, mysql_errno(me->psvr));
	}

	/* If we have to clear it out */
	if (flags & HTSQLLOG_CLEAR_URIS_TABLE) 
	    clear_table(me, DEFAULT_SQL_URIS_TABLE);

	/* If we have to delete it first */
	if (flags & HTSQLLOG_DROP_REQUESTS_TABLE) 
	    drop_table(me, DEFAULT_SQL_REQUESTS_TABLE);

	/* Create Request table */
	query = StrAllocMCopy(&query, "create table ", DEFAULT_SQL_REQUESTS_TABLE, " ( ",
			      "uri ", DEFAULT_SQL_KEY_TYPE, " primary key, ",
			      "method char(16) not null, ",
			      "status int unsigned not null, ",
			      "request_time datetime, ",
			      "response_time datetime ",
			      ")", NULL);
#ifdef SQL_DEBUG
	fprintf(stderr, "%s\n", query);
#endif
	if (mysql_query(me->psvr, query) < 0) {
	    if (SHOWLOG) HTTrace("SQLLog...... `%s\' on query `%s\' with errno %d\n",
				 mysql_error(me->psvr), query, mysql_errno(me->psvr));
	}

	/* If we have to clear it out */
	if (flags & HTSQLLOG_CLEAR_REQUESTS_TABLE) 
	    clear_table(me, DEFAULT_SQL_REQUESTS_TABLE);

	/* If we have to delete it first */
	if (flags & HTSQLLOG_DROP_RESOURCES_TABLE) 
	    drop_table(me, DEFAULT_SQL_RESOURCES_TABLE);

	/* Create Resource table */
	query = StrAllocMCopy(&query, "create table ", DEFAULT_SQL_RESOURCES_TABLE," ( ",
			      "uri ", DEFAULT_SQL_KEY_TYPE, " primary key, ",
			      "length bigint unsigned not null, ",
			      "last_modified datetime, ",
			      "expires datetime, ",
			      "content_type char(32), ",
			      "charset char(32), ",
			      "content_encoding char(32), ",
			      "content_language char(32), ",
			      "title char(128) ",
			      ")", NULL);
#ifdef SQL_DEBUG
	fprintf(stderr, "%s\n", query);
#endif
	if (mysql_query(me->psvr, query) < 0) {
	    if (SHOWLOG) HTTrace("SQLLog...... `%s\' on query `%s\' with errno %d\n",
				 mysql_error(me->psvr), query, mysql_errno(me->psvr));
	}

	/* If we have to clear it out */
	if (flags & HTSQLLOG_CLEAR_RESOURCES_TABLE) 
	    clear_table(me, DEFAULT_SQL_RESOURCES_TABLE);

	/* If we have to delete it first */
	if (flags & HTSQLLOG_DROP_LINKS_TABLE) 
	    drop_table(me, DEFAULT_SQL_LINKS_TABLE);

	/* Create Link Relations table */
	query = StrAllocMCopy(&query, "create table ", DEFAULT_SQL_LINKS_TABLE, " ( ",
			      "source ", DEFAULT_SQL_KEY_TYPE, ", ",
			      "destination ", DEFAULT_SQL_KEY_TYPE, ", ",
			      "link_type char(32) not null, ",
			      "comment char(128), ",
			      "unique (source, destination, link_type) ",
			      ")", NULL);
#ifdef SQL_DEBUG
	fprintf(stderr, "%s\n", query);
#endif
	if (mysql_query(me->psvr, query) < 0) {
	    if (SHOWLOG) HTTrace("SQLLog...... `%s\' on query `%s\' with errno %d\n",
				 mysql_error(me->psvr), query, mysql_errno(me->psvr));
	}

	/* If we have to clear it out */
	if (flags & HTSQLLOG_CLEAR_LINKS_TABLE) 
	    clear_table(me, DEFAULT_SQL_LINKS_TABLE);

	HT_FREE(query);
	return YES;	
    }
    return NO;
}

/*	Open a connection
**	-----------------
**	Returns YES if OK, NO on error
*/
PUBLIC HTSQLLog * HTSQLLog_connect (const char * host,
				    const char * user, const char * pw)
{
    HTSQLLog * me;
    if (!host || !user || !pw) {
	if (SHOWLOG) HTTrace("SQLLog...... Missing SQLLog host, user, or password\n");
	return NULL;
    }
    if ((me = (HTSQLLog *) HT_CALLOC(1, sizeof(HTSQLLog))) == NULL)
        HT_OUTOFMEM("HTSQLLog_open");

    if (SHOWLOG) HTTrace("SQLLog...... Open a link to server `%s\'\n", host);
    if ((me->psvr = mysql_connect(&(me->server), host, user, pw)) == NULL) {
	if (SHOWLOG)
	    HTTrace("SQLLog...... `%s\' errno %d\n",
		    mysql_error(&me->server), mysql_errno(&me->server));
	HT_FREE(me);
	return NULL;
    }
    me->host = host;
    me->user = user;
    me->password = pw;		/* @@@ scramble!!! @@@ */
    return me;
}


/*	Close connection
**	----------------
**	Returns YES if OK, NO on error
*/
PUBLIC BOOL HTSQLLog_close (HTSQLLog * me)
{
    if (me && me->psvr) {
	if (SHOWLOG) HTTrace("SQLLog...... Closing link to host `%s\'\n", me->host);
	mysql_close(me->psvr);
	HT_FREE(me->relative);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTSQLLog_clearTables (HTSQLLog * me)
{
    if (me && me->psvr) {

	/* ... */

	return YES;
    }
    return NO;
}

PUBLIC BOOL HTSQLLog_makeRelativeTo (HTSQLLog * me, const char * relative)
{
    if (me) {
	StrAllocCopy(me->relative, relative);
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTSQLLog_openDB (HTSQLLog * me, const char * db,
			     HTSQLLogFlags flags)
{
    BOOL created = NO;
    if (me && me->psvr && db) {
	if (SHOWLOG) HTTrace("SQLLog...... Selecting database `%s\'\n", db);
	if (mysql_select_db(me->psvr, db) < 0) {
	    int err = mysql_errno(me->psvr);
	    if (SHOWLOG) HTTrace("SQLLog...... `%s\', errno %d\n",
				 mysql_error(me->psvr), err);

	    /* If the database couldn't be found then create a new one */
	    if (err == 1049) {
		if (mysql_create_db(me->psvr, db) < 0) {
		    if (SHOWLOG) HTTrace("SQLLog...... `%s\', errno %d\n",
					 mysql_error(me->psvr), err);
		    return NO;
		}
		created = YES;
	    } else
		return NO;
	}

	if (created) {
	    if (mysql_select_db(me->psvr, db) < 0) {
		int err = mysql_errno(me->psvr);
		if (SHOWLOG) HTTrace("SQLLog...... `%s\', errno %d\n",
				     mysql_error(me->psvr), err);
		return NO;
	    }
	}

	/* Create the log tables in the log db */ 
	createTables(me, flags);
	return YES;
    } 
   return NO;
}

PUBLIC int HTSQLLog_accessCount (HTSQLLog * me)
{
    return me ? me->accesses : -1;
}

PUBLIC BOOL HTSQLLog_addURI (HTSQLLog * me, const char * uri)
{
    return (me && me->psvr && (add_uri(me, uri)>=0));
}

PUBLIC BOOL HTSQLLog_addLinkRelationship (HTSQLLog * me,
					  const char * src_uri,
					  const char * dst_uri,
					  const char * link_type,
					  const char * comment)
{
    if (src_uri && dst_uri) {
	int srcidx = add_uri(me, src_uri);
	int dstidx = add_uri(me, dst_uri);
	return add_linktype(me, srcidx, dstidx, link_type, comment);
    }
    return NO;
}

PUBLIC BOOL HTSQLLog_addEntry (HTSQLLog * me, HTRequest * request, int status)
{
    if (me && me->psvr) {
	HTParentAnchor * anchor = HTRequest_anchor(request);
	HTParentAnchor * parent_anchor = HTRequest_parent(request);
	char * uri = HTAnchor_address((HTAnchor *) anchor);
	int index = 0;
	char buf[512];
	char * query = NULL;

	/* Insert into the URI table */
	if ((index = add_uri(me, uri)) < 0) return NO;

	/* Insert into the request table */
	query = sql_printf(buf, 512, "replace into %s values (%u,%S,%u,%T,%T)",
			   DEFAULT_SQL_REQUESTS_TABLE,
			   index,
			   HTMethod_name(HTRequest_method(request)),
			   abs(status),
			   HTRequest_date(request),
			   HTAnchor_date(anchor));
#ifdef SQL_DEBUG
	fprintf(stderr, "%s\n", query);
#endif
	if (mysql_query(me->psvr, query) < 0) {
	    if (SHOWLOG) HTTrace("SQLLog...... `%s\' on query `%s\' with errno %d\n",
				 mysql_error(me->psvr), query, mysql_errno(me->psvr));
	    return NO;
	}

	/* Insert into the resource table */
	{
	    HTList * encodings = HTAnchor_encoding(anchor);
	    HTList * languages = HTAnchor_language(anchor);
	    HTFormat format = HTAnchor_format(anchor);
	    HTCharset charset = HTAnchor_charset(anchor);
	    HTEncoding encoding = encodings ? HTList_firstObject(encodings) : NULL;
	    HTLanguage language = languages ? HTList_firstObject(languages) : NULL;
	    query = sql_printf(buf, 512, "replace into %s values (%u,%l,%T,%T,%S,%S,%S,%S,%S)",
			       DEFAULT_SQL_RESOURCES_TABLE,
			       index,
			       HTAnchor_length(anchor),
			       HTAnchor_lastModified(anchor),
			       HTAnchor_expires(anchor),
			       format != WWW_UNKNOWN ? HTAtom_name(format) : NULL,
			       charset ? HTAtom_name(charset) : NULL,
			       encoding ? HTAtom_name(encoding) : NULL,
			       language ? HTAtom_name(language) : NULL,
			       HTAnchor_title(anchor));
#ifdef SQL_DEBUG
	    fprintf(stderr, "%s\n", query);
#endif
	    if (mysql_query(me->psvr, query) < 0) {
		if (SHOWLOG) HTTrace("SQLLog...... `%s\' on query `%s\' with errno %d\n",
				     mysql_error(me->psvr), query,mysql_errno(me->psvr));
		return NO;
	    }
	}

	/* Insert into the relationship table */
	if (parent_anchor) {
	    char * parent = HTAnchor_address((HTAnchor *) parent_anchor);
	    if (uri && parent && *parent) {
		int srcidx = add_uri(me, parent);
		add_linktype(me, srcidx, index, "referer", NULL);
	    }
	    HT_FREE(parent);
	}

	/* Count one more access */
	me->accesses++;

	HT_FREE(uri);
	return YES;
    }
    return NO;
}


