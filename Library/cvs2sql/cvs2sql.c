/*
**	W3C CVS To SQL Log Converter
**
**	(c) COPRIGHT MIT 1999.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This program takes cvs log info via stdin and writes it to an SQL DB
**
**  Author:
**	Henrik Frystyk Nielsen, http://www.w3.org/People/Frystyk
**
*/

#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWSQL.h"
#include "HTextImp.h"

#define APP_NAME			"cvs2sql"
#define APP_VERSION			"1.0"

#define DEFAULT_SQL_SERVER		"localhost"
#define DEFAULT_SQL_DB			"cvs"
#define DEFAULT_SQL_USER		"none"
#define DEFAULT_SQL_PW			"none"

#define DEFAULT_SQL_LOCATION_TABLE	"locations"
#define DEFAULT_SQL_USER_TABLE		"users"
#define DEFAULT_SQL_COMMENTS_TABLE	"comments"
#define DEFAULT_SQL_LOG_TABLE		"logs"

#define DEFAULT_SQL_KEY_TYPE		"int unsigned not null"
#define MAX_KEY_LENGTH			255

#define DEFAULT_CVS_USER		"unknown"

#define DELIMITERS			" \n"

#define BUFSIZE				8196

typedef enum _SQLFlags {
    SQL_CLEAR_LOCATION_TABLE 	= 0x1,
    SQL_DROP_LOCATION_TABLE 	= 0x2, 

    SQL_CLEAR_USER_TABLE	= 0x4,
    SQL_DROP_USER_TABLE		= 0x8,

    SQL_CLEAR_COMMENTS_TABLE	= 0x10,
    SQL_DROP_COMMENTS_TABLE	= 0x20,

    SQL_CLEAR_LOG_TABLE		= 0x40,
    SQL_DROP_LOG_TABLE		= 0x80
} SQLFlags;

/* ------------------------------------------------------------------------- */

PRIVATE void VersionInfo (void)
{
    fprintf(stdout, "W3C Sample Software\n\n");
    fprintf(stdout, "\t%s, W3C CVS Log to SQL converter version %s\n", APP_NAME, APP_VERSION);
    fprintf(stdout, "\tlibwww, W3C Sample Library version %s\n\n", HTLib_version());
    fprintf(stdout, "Please send feedback to <www-lib@w3.org>\n");
}

PRIVATE void Cleanup (int status, HTSQL * sql, HTChunk * chunk)
{
    HTChunk_delete(chunk);
    HTSQL_delete(sql);
    HTLibTerminate();

#ifdef VMS
    exit(status ? status : 1);
#else
    exit(status ? status : 0);
#endif
}

PRIVATE BOOL clear_table (HTSQL * me, const char * table)
{
    if (me && table) {
	char buf[1024];
	char * query = HTSQL_printf(buf, 1024, "delete from %s", table);
	return HTSQL_query(me, query);
    }
    return NO;
}

PRIVATE BOOL drop_table (HTSQL * me, const char * table)
{
    if (me && table) {
	char buf[1024];
	char * query = HTSQL_printf(buf, 1024, "drop table %s", table);
	return HTSQL_query(me, query);
    }
    return NO;
}

/* 
** Create the three CVS tables if not already there
** 	location:	filename, id
** 	user: 		username, id
** 	log:		user_id, location_id, date, operation, message
*/
PRIVATE BOOL createTables (HTSQL * sql, SQLFlags flags)
{
    if (sql) {
	char buf[1024];
	char * query = NULL;

	/* Create location table */

	/* If we have to delete it first */
	if (flags & SQL_DROP_LOCATION_TABLE) 
	    drop_table(sql, DEFAULT_SQL_LOCATION_TABLE);

	query = HTSQL_printf(buf, 1024,
			     "create table %s (id %s auto_increment, location varchar(%u) binary not null, primary key(id), unique(location), index loc_idx(location(32)))",
			     DEFAULT_SQL_LOCATION_TABLE,
			     DEFAULT_SQL_KEY_TYPE,
			     MAX_KEY_LENGTH);
	HTSQL_query(sql, query);

	/* If we have to clear it out */
	if (flags & SQL_CLEAR_LOCATION_TABLE) 
	    clear_table(sql, DEFAULT_SQL_LOCATION_TABLE);

	/* Create user table */

	/* If we have to delete it first */
	if (flags & SQL_DROP_USER_TABLE) 
	    drop_table(sql, DEFAULT_SQL_USER_TABLE);

	query = HTSQL_printf(buf, 1024,
			     "create table %s (id %s auto_increment, username varchar(%u) binary not null, primary key(id), unique(username), index username_idx(username(32)))",
			     DEFAULT_SQL_USER_TABLE,
			     DEFAULT_SQL_KEY_TYPE,
			     MAX_KEY_LENGTH);
	HTSQL_query(sql, query);

	/* If we have to clear it out */
	if (flags & SQL_CLEAR_USER_TABLE) 
	    clear_table(sql, DEFAULT_SQL_USER_TABLE);

	/* Create comments table */

	/* If we have to delete it first */
	if (flags & SQL_DROP_COMMENTS_TABLE) 
	    drop_table(sql, DEFAULT_SQL_COMMENTS_TABLE);

	query = HTSQL_printf(buf, 1024,
			     "create table %s (id %s auto_increment, comment text, primary key(id))",
			     DEFAULT_SQL_COMMENTS_TABLE,
			     DEFAULT_SQL_KEY_TYPE);
	HTSQL_query(sql, query);

	/* If we have to clear it out */
	if (flags & SQL_CLEAR_COMMENTS_TABLE) 
	    clear_table(sql, DEFAULT_SQL_COMMENTS_TABLE);

	/* Create logs table */

	/* If we have to delete it first */
	if (flags & SQL_DROP_LOG_TABLE) 
	    drop_table(sql, DEFAULT_SQL_LOG_TABLE);

	query = HTSQL_printf(buf, 1024,
			     "create table %s (location %s, username %s, date datetime, operation char(32), comment %s)",
			     DEFAULT_SQL_LOG_TABLE,
			     DEFAULT_SQL_KEY_TYPE,
			     DEFAULT_SQL_KEY_TYPE,
			     DEFAULT_SQL_KEY_TYPE);
	HTSQL_query(sql, query);

	/* If we have to clear it out */
	if (flags & SQL_CLEAR_LOG_TABLE) 
	    clear_table(sql, DEFAULT_SQL_LOG_TABLE);

	/* All done */
	return YES;	
    }
    return NO;
}

PRIVATE int find_location(HTSQL * sql, const char * location)
{
    int index = -1;
    if (sql && location) {
	char buf[4096];
        char * query = NULL;
        MYSQL_RES * result = NULL;
	query = HTSQL_printf(buf, 4096, "select * from %s where location=%S",
			     DEFAULT_SQL_LOCATION_TABLE, location);
	if (HTSQL_query(sql, query) &&
	    (result = HTSQL_storeResult(sql)) != NULL) {
	    MYSQL_ROW row;
	    if ((row = mysql_fetch_row(result)) && row[0])
		index = atoi(row[0]);
	    HTSQL_freeResult(result);
	}
    }
    return index;
}

PRIVATE int add_location (HTSQL * sql, const char * location)
{
    if (sql && location) {
	int index = -1;

	/* If we can't find the LOCATION then add it */
	if ((index = find_location(sql, location)) < 0) {
	    char buf[4096];
	    char * query = HTSQL_printf(buf, 4096, "insert into %s (location) values (%S)",
					DEFAULT_SQL_LOCATION_TABLE, location);
	    if (HTSQL_query(sql, query) != YES) {
		return -1;
	    }
	    index = HTSQL_getLastInsertId(sql);
	}
	return index;
    }
    return -1;
}

PRIVATE int find_user(HTSQL * sql, const char * user)
{
    int index = -1;
    if (sql && user) {
	char buf[1024];
        char * query = NULL;
        MYSQL_RES * result = NULL;
	query = HTSQL_printf(buf, 1024, "select * from %s where username=%S",
			     DEFAULT_SQL_USER_TABLE, user);
	if (HTSQL_query(sql, query) &&
	    (result = HTSQL_storeResult(sql)) != NULL) {
	    MYSQL_ROW row;
	    if ((row = mysql_fetch_row(result)) && row[0])
		index = atoi(row[0]);
	    HTSQL_freeResult(result);
	}
    }
    return index;
}

PRIVATE int add_user (HTSQL * sql, const char * user)
{
    if (sql && user) {
	int index = -1;

	/* If we can't find the USER then add it */
	if ((index = find_user(sql, user)) < 0) {
	    char buf[1024];
	    char * query = HTSQL_printf(buf, 1024, "insert into %s (username) values (%S)",
					DEFAULT_SQL_USER_TABLE, user);
	    if (HTSQL_query(sql, query) != YES) {
		return -1;
	    }
	    index = HTSQL_getLastInsertId(sql);
	}
	return index;
    }
    return -1;
}

PRIVATE int add_comment (HTSQL * sql, const char * comment)
{
    if (sql && comment) {
	char buf[8196];
	char * query = HTSQL_printf(buf, 8196, "insert into %s (comment) values (%S)",
				    DEFAULT_SQL_COMMENTS_TABLE, comment);
	if (HTSQL_query(sql, query) != YES) return -1;
	return HTSQL_getLastInsertId(sql);
    }
    return -1;
}

int main (int argc, char ** argv)
{
    HTSQL *	sql = NULL;
    char *	sqlserver = DEFAULT_SQL_SERVER;
    char *	sqldb = DEFAULT_SQL_DB;
    char *	sqluser = DEFAULT_SQL_USER;
    char *	sqlpw = DEFAULT_SQL_PW;
    char *	cvsuser = DEFAULT_CVS_USER;
    time_t	cvsdate = -1;
    FILE * fin = stdin;
    char *	input_buffer[BUFSIZE];
    HTChunk *	loginfo = HTChunk_new(BUFSIZE);
    int		arg = 0;
    BOOL	create_db = YES;

    /* Initiate libwww */
    HTLibInit(APP_NAME, APP_VERSION);

    /* Scan command line for parameters */
    for (arg=1; arg<argc; arg++) {
	if (*argv[arg] == '-') {
	    
	    if (!strcmp(argv[arg], "-h") || !strcmp(argv[arg], "-?")) {
		VersionInfo();
		Cleanup(0, sql, loginfo);

	    } else if (!strcmp(argv[arg], "-v")) {
		HTSetTraceMessageMask("q");

	    } else if (!strcmp(argv[arg], "-nocreate")) {
		create_db = NO;

	    } else if (!strncmp(argv[arg], "-sqldb", 5)) {
		sqldb = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_DB;

	    } else if (!strncmp(argv[arg], "-sqlpassword", 5)) {
		sqlpw = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_PW;

	    } else if (!strncmp(argv[arg], "-sqlserver", 5)) {
		sqlserver = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_SERVER;

	    } else if (!strncmp(argv[arg], "-sqluser", 5)) {
		sqluser = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_USER;

	    } else if (!strncmp(argv[arg], "-cvsuser", 5)) {
		cvsuser = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_CVS_USER;

	    } else if (!strncmp(argv[arg], "-cvsdate", 5)) {
		cvsdate = (arg+1 < argc && *argv[arg+1] != '-') ?
		    HTParseTime(argv[++arg], NULL, NO) : -1;

	    } else {
		fprintf(stderr, "Bad Argument (%s)\n", argv[arg]);
	    }
	} else {
	    fprintf(stderr, "Bad Argument (%s)\n", argv[arg]);
	}
    }

    /* Get an SQL object */
    if ((sql = HTSQL_new(sqlserver, sqluser, sqlpw, 0)) == NULL)
	Cleanup(-1, sql, loginfo);

    /* Connect to the SQL server */
    if (HTSQL_connect(sql) != YES) Cleanup(-1, sql, loginfo);

    /* Select our database */
    if (HTSQL_selectDB(sql, sqldb) != YES) Cleanup(-1, sql, loginfo);

    /* Create our tables */
    if (create_db) createTables(sql, 0);

    /* Read the arguments from stdin */
    for (;;) {
	int status = fread(input_buffer, 1, BUFSIZE, fin);
	if (status < 0) Cleanup(-1, sql, loginfo);
	if (status == 0) break;
	HTChunk_putb(loginfo, (const char *) input_buffer, status);
    }

    /* Parse the input chunk */
    {
	char * ptr = HTChunk_data(loginfo);
	char * noop1 = HTNextField(&ptr);
	char * noop2 = HTNextField(&ptr);
	char * root = HTNextField(&ptr);
	char * operation = NULL;
	char * files = NULL;
	char * comment = NULL;
	int comment_id = -1;
	int user_id = -1;
	char * p, * q;
#ifdef HT_REENTRANT
	char *lasts;					     /* For strtok_r */
#endif

	/* Find shared log message and get id */
	if ((q = HTStrCaseStr(ptr, "\nLog Message:")) != NULL) {
	    comment = q+14;
	    *q = '\0';
	}
	if ((comment_id = add_comment(sql, comment)) < 0) 
	    Cleanup(-1, sql, loginfo);

	/* Add/find user and get id */
	if ((user_id = add_user(sql, cvsuser)) < 0)
	    Cleanup(-1, sql, loginfo);

	/* For each operation, find the files involved */
	while ((q = HTStrCaseStr(ptr, " Files:")) != NULL) {

	    /* Find the operation */
	    files = q+9;
	    for (p=q; p>HTChunk_data(loginfo) && *p && *p!='\n'; p--);
	    p++;
	    operation = HTNextField(&p);

	    /* Find the next line */
	    if ((q = strchr(files, '\n')) != NULL) {
		*q++ = '\0';
		ptr = q;
	    }

	    /* Create the query */
	    if (operation && files && comment) {
		char * file;
		int location_id = -1;

#ifdef HT_REENTRANT
		if ((file = strtok_r(files, DELIMITERS, &lasts)) != NULL) {
#else
		if ((file = strtok(files, DELIMITERS)) != NULL) {
#endif /* HT_REENTRANT */
		    do {
			char * path = NULL;		
			StrAllocMCopy(&path, root, "/", file, NULL);

			/* Add/find location and get id */
			if ((location_id = add_location(sql, path)) < 0) {
			    Cleanup(-1, sql, loginfo);
			    break;
			}

#if 0
			fprintf(stderr, "location: `%s\', user: `%s\', operation: `%s\', comment: `%s\'\n",
				path, cvsuser, operation, comment);
#endif

			/* Add log entry */
			{
			    char buf[16384];
			    char * query = HTSQL_printf(buf, 16384, "insert into %s values (%u,%u,%T,%S,%u)",
							DEFAULT_SQL_LOG_TABLE,
							location_id,
							user_id,
							cvsdate,
							operation,
							comment_id);
			    if (HTSQL_query(sql, query) != YES) {
				Cleanup(-1, sql, loginfo);
				break;
			    }
			}
		
			HT_FREE(path);
#ifdef HT_REENTRANT
		    } while ((file = (char *) strtok_r(NULL, DELIMITERS, &lasts)) != NULL);
#else
		    } while ((file = strtok(NULL, DELIMITERS)) != NULL);
#endif /* HT_REENTRANT */
		}
	    }
	}
    }

    return 0;
}


