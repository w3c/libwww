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
#define DEFAULT_SQL_LOG_TABLE		"logs"

#define DEFAULT_SQL_KEY_TYPE		"int unsigned not null"
#define MAX_KEY_LENGTH			255

#define DEFAULT_CVS_USER		"unknown"

#define BUFSIZE				1024

typedef enum _SQLFlags {
    SQL_CLEAR_LOCATION_TABLE 	= 0x1,
    SQL_CLEAR_USER_TABLE	= 0x2,
    SQL_CLEAR_LOG_TABLE		= 0x4,
    SQL_DROP_LOCATION_TABLE 	= 0x8, 
    SQL_DROP_USER_TABLE		= 0x10,
    SQL_DROP_LOG_TABLE		= 0x20
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

	/* If we have to delete it first */
	if (flags & SQL_DROP_LOCATION_TABLE) 
	    drop_table(sql, DEFAULT_SQL_LOCATION_TABLE);

	/* Create location table */
	query = HTSQL_printf(buf, 1024,
			     "create table %s (id %s auto_increment, location varchar(%u) binary not null, primary key(id), unique(location), index loc_idx(location(32)))",
			     DEFAULT_SQL_LOCATION_TABLE,
			     DEFAULT_SQL_KEY_TYPE,
			     MAX_KEY_LENGTH);
	HTSQL_query(sql, query);

	/* If we have to clear it out */
	if (flags & SQL_CLEAR_LOCATION_TABLE) 
	    clear_table(sql, DEFAULT_SQL_LOCATION_TABLE);

	/* If we have to delete it first */
	if (flags & SQL_DROP_USER_TABLE) 
	    drop_table(sql, DEFAULT_SQL_USER_TABLE);

	/* Create User table */
	query = HTSQL_printf(buf, 1024,
			     "create table %s (id %s auto_increment, username varchar(%u) binary not null, primary key(id), unique(username), index username_idx(username(32)))",
			     DEFAULT_SQL_USER_TABLE,
			     DEFAULT_SQL_KEY_TYPE,
			     MAX_KEY_LENGTH);
	HTSQL_query(sql, query);

	/* If we have to clear it out */
	if (flags & SQL_CLEAR_USER_TABLE) 
	    clear_table(sql, DEFAULT_SQL_USER_TABLE);

	/* If we have to delete it first */
	if (flags & SQL_DROP_LOG_TABLE) 
	    drop_table(sql, DEFAULT_SQL_LOG_TABLE);

	/* Create Log table */
	query = HTSQL_printf(buf, 1024,
			     "create table %s (location %s, username %s, primary key(location, username), date datetime, operation char(32), comment text)",
			     DEFAULT_SQL_LOG_TABLE,
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
	char buf[1024];
        char * query = NULL;
        MYSQL_RES * result = NULL;
	query = HTSQL_printf(buf, 1024, "select * from %s where location=%S",
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
	    char buf[1024];
	    char * query = HTSQL_printf(buf, 1024, "insert into %s (location) values (%S)",
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
		WWWTRACE |= SHOW_SQL_TRACE;

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
	char * operation = HTNextField(&ptr);
	char * noop = HTNextField(&ptr);
	char * root = HTNextField(&ptr);
	char * files = NULL;
	char * logmessage = NULL;
	char * q;
	if ((q = strcasestr(ptr, "\nLog Message:")) != NULL) {
	    logmessage = q+14;
	    *q = '\0';
	}
	if ((q = strcasestr(ptr, "\nModified Files:")) != NULL)
	    files = q+17;

	if (files && logmessage) {
	    char * file;
	    int location_id = -1;
	    int user_id = -1;

	    /* Add/find user and get id */
	    if ((user_id = add_user(sql, cvsuser)) < 0)
		Cleanup(-1, sql, loginfo);

	    /* Add log entry for each file */
	    ptr = files;
	    while ((file = HTNextField(&ptr)) != NULL) {		
		char * path = NULL;		
		StrAllocMCopy(&path, root, "/", file, NULL);

		/* Add/find location and get id */
		if ((location_id = add_location(sql, path)) < 0) {
		    Cleanup(-1, sql, loginfo);
		    break;
		}

		/* Add log entry */
		{
		    char buf[1024];
		    char * query = HTSQL_printf(buf, 1024, "insert into %s values (%u,%u,%T,%S,%S)",
						DEFAULT_SQL_LOG_TABLE,
						location_id,
						user_id,
						cvsdate,
						operation,
						logmessage);
		    if (HTSQL_query(sql, query) != YES) {
			Cleanup(-1, sql, loginfo);
			break;
		    }
		}
		
		HT_FREE(path);
	    }
	}

	/* 
	** Update of /libwww/Library/Test
	** In directory rufus.w3.org:/tmp/cvs-serv29893
	**
	** Modified Files:
	**        site.exp 
	** Log Message:
	**test commit
	*/


    }

    return 0;
}


