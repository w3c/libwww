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
	if (WWWTRACE) HTTrace("Log......... No log file given\n");
	return NULL;
    }

    if ((log = (HTLog *) HT_CALLOC(1, sizeof(HTLog))) == NULL)
        HT_OUTOFMEM("HTLog_open");

    if (WWWTRACE) HTTrace("Log......... Open log file `%s\'\n", filename);
    log->fp = fopen(filename, append ? "a" : "w");
    if (!log->fp) {
	if (WWWTRACE) HTTrace("Log......... Can't open log file `%s\'\n", filename);
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
	if (WWWTRACE) HTTrace("Log......... Closing log file %p\n", log->fp);
	status = fclose(log->fp);
	HT_FREE(log);
	return (status != EOF);
    }
    return NO;
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
	if (WWWTRACE) HTTrace("Log......... Writing log\n");
	fprintf(log->fp, "localhost - - [%s] %s %s %d %ld\n",
		HTDateTimeStr(&now, log->localtime),
		HTMethod_name(HTRequest_method(request)),
		uri ? uri : "<null>",			/* Bill Rizzi */
		status,
		HTAnchor_length(anchor));
	HT_FREE(uri);
	return (fflush(log->fp) != EOF); /* Actually update it on disk */
    }
    return NO;
}

/*
**	A generic logger - logs whatever you put in as the line.
**	Caller should add a line feed if needed.
*/
PUBLIC BOOL HTLog_addLine (HTLog * log, const char * line)
{
    if (log && log->fp && line) {
	fprintf(log->fp, line);
	return (fflush(log->fp) != EOF); /* Actually update it on disk */
    }
    return NO;
}
