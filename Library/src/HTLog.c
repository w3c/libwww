/*								   HTLog.c
**	LOGGING MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
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

PRIVATE FILE *HTLogFile = NULL;          /* Log of requests in common format */
PRIVATE BOOL HTloglocal = YES;		     /* Use local or GMT for logfile */

/* ------------------------------------------------------------------------- */

/*	Open a Logfile
**	--------------
**	You can use either GMT or local time. If no filename is given,
**	no log is kept. New log entries can be either appended to an existing
**	file or overwriting an exsisting file.
**	Returns YES if OK, NO on error
*/
PUBLIC BOOL HTLog_open (CONST char * filename, BOOL local, BOOL append)
{
    if (!filename || !*filename) {
	if (WWWTRACE) TTYPrint(TDEST, "Log......... No log file given\n");
	return NO;
    }
    if (WWWTRACE)
	TTYPrint(TDEST, "Log......... Open log file `%s\'\n", filename);
    if (HTLogFile) {
	if (WWWTRACE)
	    TTYPrint(TDEST, "Log......... Already open\n");
	return NO;
    }
    HTLogFile = fopen(filename, append ? "a" : "w");
    if (!HTLogFile) {
	if (WWWTRACE)
	    TTYPrint(TDEST, "Log......... Can't open log file `%s\'\n",
		    filename);
	return NO;
    }
    HTloglocal = local;					   /* remember state */
    return YES;
}


/*	Close the log file
**	------------------
**	Returns YES if OK, NO on error
*/
PUBLIC BOOL HTLog_close (void)
{
    if (WWWTRACE)
	TTYPrint(TDEST, "Log......... Closing log file\n");
    if (HTLogFile) {
	int status = fclose(HTLogFile);
	HTLogFile = NULL;
	return (status!=EOF);
    }
    return NO;
}

PUBLIC BOOL HTLog_isOpen (void)
{
    return HTLogFile ? YES : NO;
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
PUBLIC BOOL HTLog_add (HTRequest * request, int status)
{
    if (HTLogFile) {
	time_t now = time(NULL);	
	HTParentAnchor *anchor = HTRequest_anchor(request);
	char * uri = HTAnchor_address((HTAnchor *) anchor);
	if (WWWTRACE) TTYPrint(TDEST, "Log......... Writing log\n");
	fprintf(HTLogFile, "localhost - - [%s] %s %s %d %ld\n",
		HTDateTimeStr(&now, HTloglocal),
		HTMethod_name(HTRequest_method(request)),
		uri,
		status,
		HTAnchor_length(anchor));
	HT_FREE(uri);
	return (fflush(HTLogFile)!=EOF);       /* Actually update it on disk */
    }
    return NO;
}
