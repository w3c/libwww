/*								   HTLog.c
**	LOGGING MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module contains a simple logging mechanism for requests.
**	The user must open and close the log file!!!
**
** History:
**  	01 May 95	Henrik Frystyk, frystyk@w3.org
** Bugs:
**	ANSI file handling is not capable of handling simultanous writing
**	from several processes at the same time in a multi-process environment
*/

/* Library include files */
#include "WWWLib.h"
#include "HTLog.h"					 /* Implemented here */

struct _HTLog {
    FILE *		fp;
    BOOL		localtime;
    int			accesses;
};

/* ------------------------------------------------------------------------- */

/*	Open a Logfile
**	--------------
**	You can use either GMT or local time. If no filename is given,
**	no log is kept. New log entries can be either appended to an existing
**	file or overwriting an exsisting file.
**	Returns YES if OK, NO on error
*/
PUBLIC HTLog * HTLog_open (const char * filename, BOOL local, BOOL append)
{
    HTLog * log;
    if (!filename || !*filename) {
	HTTRACE(APP_TRACE, "Log......... No log file given\n");
	return NULL;
    }

    if ((log = (HTLog *) HT_CALLOC(1, sizeof(HTLog))) == NULL)
        HT_OUTOFMEM("HTLog_open");

    HTTRACE(APP_TRACE, "Log......... Open log file `%s\'\n" _ filename);
    log->fp = fopen(filename, append ? "a" : "w");
    if (!log->fp) {
	HTTRACE(APP_TRACE, "Log......... Can't open log file `%s\'\n" _ filename);
	HT_FREE(log);
	return NULL;
    }
    log->localtime = local;
    return log;
}


/*	Close the log file
**	------------------
**	Returns YES if OK, NO on error
*/
PUBLIC BOOL HTLog_close (HTLog * log)
{
    if (log && log->fp) {
	int status;
	HTTRACE(APP_TRACE, "Log......... Closing log file %p\n" _ log->fp);
	status = fclose(log->fp);
	HT_FREE(log);
	return (status != EOF);
    }
    return NO;
}

PUBLIC int HTLog_accessCount (HTLog * log)
{
    return log ? log->accesses : -1;
}

/*	Add entry to the log file
**	-------------------------
**	Format: <HOST> - - <DATE> <METHOD> <URI> <RESULT> <CONTENT_LENTGH>
**	which is almost equivalent to Common Logformat. Permissions on UNIX
**	are modified by umask.
**
**	Returns YES if OK, NO on error
**
**	BUG: No result code is produced :-( Should be taken from HTError.c
*/
PUBLIC BOOL HTLog_addCLF (HTLog * log, HTRequest * request, int status)
{
    if (log && log->fp) {
	time_t now = time(NULL);	
	HTParentAnchor * anchor = HTRequest_anchor(request);
	char * uri = HTAnchor_address((HTAnchor *) anchor);
	HTTRACE(APP_TRACE, "Log......... Writing CLF log\n");
	fprintf(log->fp, "localhost - - [%s] %s %s %d %ld\n",
		HTDateTimeStr(&now, log->localtime),
		HTMethod_name(HTRequest_method(request)),
		uri ? uri : "<null>",			/* Bill Rizzi */
		abs(status),
		HTAnchor_length(anchor));
	HT_FREE(uri);
	log->accesses++;
	return (fflush(log->fp) != EOF); /* Actually update it on disk */
    }
    return NO;
}

/*	Add entry to the referer log file
**	---------------------------------
**	
**	which is almost equivalent to Common Logformat. Permissions on UNIX
**	are modified by umask.
**
**	Returns YES if OK, NO on error
*/
PUBLIC BOOL HTLog_addReferer (HTLog * log, HTRequest * request, int status)
{
    if (log && log->fp && request) {
	HTParentAnchor * parent_anchor = HTRequest_parent(request);
	if (parent_anchor) {
	    char * me = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
	    char * parent = HTAnchor_address((HTAnchor *) parent_anchor);
	    HTTRACE(APP_TRACE, "Log......... Writing Referer log\n");
	    if (me && parent && *parent) {
		fprintf(log->fp, "%s -> %s\n", parent, me);
	    }
	    HT_FREE(me);
	    HT_FREE(parent);
	    log->accesses++;
	    return (fflush(log->fp) != EOF); /* Actually update it on disk */
	}
    }
    return NO;
}

/*
**	A generic logger - logs whatever you put in as the line.
*/
PUBLIC BOOL HTLog_addLine (HTLog * log, const char * line)
{
    if (log && log->fp && line) {
	fprintf(log->fp, "%s\n", line);
	log->accesses++;
	return (fflush(log->fp) != EOF); /* Actually update it on disk */
    }
    return NO;
}

/*
**	A generic logger with variable arguments
*/
PUBLIC BOOL HTLog_addText (HTLog * log, const char * fmt, ...)
{
    if (log && log->fp) {
	va_list pArgs;
	va_start(pArgs, fmt);
#ifdef HAVE_VPRINTF
	log->accesses++;
	(vfprintf(log->fp, fmt, pArgs));
	va_end(pArgs);
#endif
	return (fflush(log->fp) != EOF); /* Actually update it on disk */
    }
    return NO;
}
