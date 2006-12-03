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
#include "HTSQL.h"
#include "HTSQLLog.h"					 /* Implemented here */

#include <mysql.h>

struct _HTSQLLog {
    HTSQL *		sql;
    char *		relative;		/* Make URIs relative to */
    int			accesses;
    HTSQLLogFlags	flags;
};

#define DEFAULT_SQL_URIS_TABLE		"uris"
#define DEFAULT_SQL_REQUESTS_TABLE	"requests"
#define DEFAULT_SQL_RESOURCES_TABLE	"resources"
#define DEFAULT_SQL_LINKS_TABLE		"links"

#define DEFAULT_SQL_KEY_TYPE		"int unsigned not null"
#define MAX_URI_LENGTH			255

/* ------------------------------------------------------------------------- */

PRIVATE int find_uri(HTSQLLog * me, const char * uri)
{
    int index = -1;
    if (me && me->sql && uri) {
	char buf[1024];
        char * query = NULL;
        MYSQL_RES * result = NULL;
	query = HTSQL_printf(buf, 1024, "select * from %s where uri=%S",
			     DEFAULT_SQL_URIS_TABLE, uri);
	if (HTSQL_query(me->sql, query) &&
	    (result = HTSQL_storeResult(me->sql)) != NULL) {
	    MYSQL_ROW row;
	    if ((row = mysql_fetch_row(result)) && row[0])
		index = atoi(row[0]);
	    HTSQL_freeResult(result);
	}
    }
    return index;
}

PRIVATE int add_uri (HTSQLLog * me, const char * uri)
{
    if (me && me->sql && uri) {
	int index = -1;
	char * rel = me->relative ? HTRelative(uri, me->relative) : NULL;

	/* If we can't find the URI then add it */
	if ((index = find_uri(me, rel ? rel : uri)) < 0) {
	    char buf[1024];
	    char * query = HTSQL_printf(buf, 1024, "insert into %s (uri) values (%S)",
					DEFAULT_SQL_URIS_TABLE, rel ? rel : uri);
	    if (HTSQL_query(me->sql, query) != YES) {
		HT_FREE(rel);
		return -1;
	    }
	    index = HTSQL_getLastInsertId(me->sql);
	}
	HT_FREE(rel);
	return index;
    }
    return -1;
}

PRIVATE BOOL add_linktype (HTSQLLog * me, int srcidx, int dstidx,
			   const char * type, const char * comment)
{
    if (me && me->sql && srcidx>=0 && dstidx>=0 && type) {
	char buf[1024];
	char * query = HTSQL_printf(buf, 1024, "insert into %s values (%u,%u,%S,%S)",
				    DEFAULT_SQL_LINKS_TABLE,
				    srcidx, dstidx, type, comment);
	return HTSQL_query(me->sql, query);
    }
    return NO;
}

PRIVATE BOOL clear_table (HTSQLLog * me, const char * table)
{
    if (me && me->sql && table) {
	char buf[1024];
	char * query = HTSQL_printf(buf, 1024, "delete from %s", table);
	return HTSQL_query(me->sql, query);
    }
    return NO;
}

PRIVATE BOOL drop_table (HTSQLLog * me, const char * table)
{
    if (me && me->sql && table) {
	char buf[1024];
	char * query = HTSQL_printf(buf, 1024, "drop table %s", table);
	return HTSQL_query(me->sql, query);
    }
    return NO;
}

PRIVATE BOOL createTables (HTSQLLog * me, HTSQLLogFlags flags)
{
    if (me && me->sql) {
	char buf[1024];
	char * query = NULL;

	/* If we have to delete it first */
	if (flags & HTSQLLOG_DROP_URIS_TABLE) 
	    drop_table(me, DEFAULT_SQL_URIS_TABLE);

	/* Create URI table (which is the index) */
	query = HTSQL_printf(buf, 1024,
			     "create table %s (id %s auto_increment, uri varchar(%u) binary not null, primary key(id), unique (uri), index uri_idx (uri(32)))",
			     DEFAULT_SQL_URIS_TABLE,
			     DEFAULT_SQL_KEY_TYPE,
			     MAX_URI_LENGTH);
	HTSQL_query(me->sql, query);

	/* If we have to clear it out */
	if (flags & HTSQLLOG_CLEAR_URIS_TABLE) 
	    clear_table(me, DEFAULT_SQL_URIS_TABLE);

	/* If we have to delete it first */
	if (flags & HTSQLLOG_DROP_REQUESTS_TABLE) 
	    drop_table(me, DEFAULT_SQL_REQUESTS_TABLE);

	/* Create Request table */
	query = HTSQL_printf(buf, 1024,
			     "create table %s (uri %s primary key, method char(16) not null, status int unsigned not null, request_time datetime, response_time datetime)",
			     DEFAULT_SQL_REQUESTS_TABLE,
			     DEFAULT_SQL_KEY_TYPE);
	HTSQL_query(me->sql, query);

	/* If we have to clear it out */
	if (flags & HTSQLLOG_CLEAR_REQUESTS_TABLE) 
	    clear_table(me, DEFAULT_SQL_REQUESTS_TABLE);

	/* If we have to delete it first */
	if (flags & HTSQLLOG_DROP_RESOURCES_TABLE) 
	    drop_table(me, DEFAULT_SQL_RESOURCES_TABLE);

	/* Create Resource table */
	query = HTSQL_printf(buf, 1024,
			     "create table %s (uri %s primary key, length bigint unsigned not null, last_modified datetime, expires datetime, content_type char(32), charset char(32), content_encoding char(32), content_language char(32), title char(128))",
			     DEFAULT_SQL_RESOURCES_TABLE,
			     DEFAULT_SQL_KEY_TYPE);
	HTSQL_query(me->sql, query);

	/* If we have to clear it out */
	if (flags & HTSQLLOG_CLEAR_RESOURCES_TABLE) 
	    clear_table(me, DEFAULT_SQL_RESOURCES_TABLE);

	/* If we have to delete it first */
	if (flags & HTSQLLOG_DROP_LINKS_TABLE) 
	    drop_table(me, DEFAULT_SQL_LINKS_TABLE);

	/* Create Link Relations table */
	query = HTSQL_printf(buf, 1024,
			     "create table %s (source %s, destination %s, link_type char(32) not null, comment char(128), unique(source,destination,link_type))",
			     DEFAULT_SQL_LINKS_TABLE,
			     DEFAULT_SQL_KEY_TYPE,
			     DEFAULT_SQL_KEY_TYPE);
	HTSQL_query(me->sql, query);

	/* If we have to clear it out */
	if (flags & HTSQLLOG_CLEAR_LINKS_TABLE) 
	    clear_table(me, DEFAULT_SQL_LINKS_TABLE);

	/* All done */
	return YES;	
    }
    return NO;
}

/*	Open a connection
**	-----------------
**	Returns YES if OK, NO on error
*/
PUBLIC HTSQLLog * HTSQLLog_open (const char * 	host,
				 const char * 	user,
				 const char * 	pw,
				 const char * 	db,
				 HTSQLLogFlags 	flags)
{
    HTSQLLog * me = NULL;
    if (!host || !user || !pw || !db) {
	HTTRACE(SQL_TRACE, "SQLLog...... Missing SQLLog host, user, password, or db\n");
	return NULL;
    }
    if ((me = (HTSQLLog *) HT_CALLOC(1, sizeof(HTSQLLog))) == NULL)
        HT_OUTOFMEM("HTSQLLog_open");
    if ((me->sql = HTSQL_new(host, user, pw, 0)) != NULL &&
	HTSQL_connect(me->sql) == YES && HTSQL_selectDB(me->sql, db) == YES) {

	/* Make sure we have the right set of tables */
	createTables(me, flags);

	me->flags = flags;
    } else {
	HTSQL_delete(me->sql);
	HT_FREE(me);
    }
    return me;
}

/*	Close connection
**	----------------
**	Returns YES if OK, NO on error
*/
PUBLIC BOOL HTSQLLog_close (HTSQLLog * me)
{
    if (me) {
	if (me->sql) HTSQL_close(me->sql);
	HT_FREE(me->relative);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTSQLLog_clearTables (HTSQLLog * me)
{
    if (me && me->sql) {

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

PUBLIC int HTSQLLog_accessCount (HTSQLLog * me)
{
    return me ? me->accesses : -1;
}

PUBLIC BOOL HTSQLLog_addURI (HTSQLLog * me, const char * uri)
{
    return (me && me->sql && (add_uri(me, uri)>=0));
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
    if (me && me->sql) {
	HTParentAnchor * anchor = HTRequest_anchor(request);
	HTParentAnchor * parent_anchor = HTRequest_parent(request);
	char * uri = HTAnchor_address((HTAnchor *) anchor);
	int index = 0;
	char buf[512];
	char * query = NULL;

	/* Insert into the URI table */
	if ((index = add_uri(me, uri)) < 0) {
	    HT_FREE(uri);
	    return NO;
	}

	/* Insert into the request table */
	query = HTSQL_printf(buf, 512, "replace into %s values (%u,%S,%u,%T,%T)",
			     DEFAULT_SQL_REQUESTS_TABLE,
			     index,
			     HTMethod_name(HTRequest_method(request)),
			     abs(status),
			     HTRequest_date(request),
			     HTAnchor_date(anchor));
	if (HTSQL_query(me->sql, query) != YES) {
	    HT_FREE(uri);
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
	    query = HTSQL_printf(buf, 512, "replace into %s values (%u,%l,%T,%T,%S,%S,%S,%S,%S)",
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
	    if (HTSQL_query(me->sql, query) != YES) {
		HT_FREE(uri);
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


