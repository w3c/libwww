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
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTAccess.h"
#include "HTLog.h"					 /* Implemented here */

PRIVATE FILE *HTlogfile = NULL;          /* Log of requests in common format */
PRIVATE BOOL HTloglocal = YES;		     /* Use local or GMT for logfile */

/* ------------------------------------------------------------------------- */

/*	Open a Logfile
**	--------------
**	You can use either GMT or local time. If no filename is given,
**	DEFAULT_LOGFILE.
**	Returns YES if OK, NO on error
*/
PUBLIC BOOL HTLog_enable ARGS2(CONST char *, filename, BOOL, local)
{
    CONST char *file = (filename && *filename) ? filename : DEFAULT_LOGFILE;
    if (TRACE)
	fprintf(TDEST, "Log......... Open log file `%s\'\n", file);
    if (HTlogfile) {
	if (TRACE)
	    fprintf(TDEST, "Log......... Already open\n");
	return NO;
    }
    if ((HTlogfile = fopen(file, "a")) == NULL) {
	if (TRACE)
	    fprintf(TDEST, "Log......... Can't open log file `%s\'\n", file);
	return NO;
    }
    HTloglocal = local;					   /* remember state */
    return YES;
}


/*	Close the log file
**	------------------
**	Returns YES if OK, NO on error
*/
PUBLIC BOOL HTLog_disable NOARGS
{
    if (TRACE)
	fprintf(TDEST, "Log......... Closing log file\n");
    if (HTlogfile) {
	int status = fclose(HTlogfile);
	HTlogfile = NULL;
	return (status!=EOF);
    }
    return NO;
}


/*	Log the result of a request
**	---------------------------
**	Format: <HOST> - - <DATE> <METHOD> <URI> <RESULT> <CONTENT_LENTGH>
**	which is almost equivalent to Common Logformat. Permissions on UNIX
**	are modified by umask.
**
**	Returns YES if OK, NO on error
**
**	BUG: No result code is produced :-( Should be taken from HTError.c
*/
PUBLIC BOOL HTLog_request ARGS1(HTRequest *, request)
{
    if (HTlogfile) {
	time_t now = time(NULL);	
	char * uri = HTAnchor_address((HTAnchor*)request->anchor);
	fprintf(HTlogfile, "%s - - [%s] %s %s %ld\n",
		HTClientHost ? HTClientHost : "localhost",
		HTDateTimeStr(&now, HTloglocal),
		HTMethod_name(request->method),
		uri,
		request->anchor->content_length);
	return (fflush(HTlogfile)!=EOF);       /* Actually update it on disk */
    }
    return NO;
}
