/*								     HTFTPDir.c
**	FILE TRANSFER PROTOCOL (FTP) DIRECTORY LISTINGS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** Authors
**	HF	Henrik Frystyk <frystyk@w3.org>
**
** History:
**	Sep 95		Spawned off from HTFTP.c and made a stream
**
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWDir.h"
#include "WWWTrans.h"
#include "HTFTPDir.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
    HTRequest *			request;
    FTPServerType		server;
    HTEOLState			state;
    HTDir *			dir;
    BOOL			first;
    BOOL			junk;
    char			buffer[MAX_FTP_LINE+1];
    int				buflen;
};

PRIVATE HTDirShow	dir_show = HT_DS_SIZE+HT_DS_DATE+HT_DS_DES+HT_DS_ICON;
PRIVATE HTDirKey	dir_key = HT_DK_CINS;

/* ------------------------------------------------------------------------- */

/*	ParseUnix
**	---------
**	Extract the name, size, and date from an 'ls'. The function expects
**	the following format of the ls-line:
**
**	<permission> <nlink> <owner> [<group>] <size> <date> <filename>
**
**	Returns YES if OK, NO on error
*/
PRIVATE BOOL ParseUnix (HTDir *dir, char * line)
{
    int cnt;
    char *ptr = line;
    char *column = NULL;
    char *date = NULL;
    char sizestr[10];    
    HTFileMode mode = (*line == 'd') ? HT_IS_DIR : HT_IS_FILE;

    /* Spool past permission, link, and owner */
    for (cnt=0; cnt<4; cnt++) {
	if ((column = HTNextField(&ptr)) == NULL) break;
    }
    
    /*
    ** This field can either be group or size. We find out by looking at the
    ** next field. If this is a non-digit then this field is the size.
    */
    while (*ptr && isspace((int) *ptr)) ptr++;
    if (isdigit((int) *ptr)) {
	column = HTNextField(&ptr);
	while (*ptr && isspace((int) *ptr)) ptr++;
    }

    if (mode == HT_IS_FILE) {
	long ls = atol(column);
	HTNumToStr(ls, sizestr, 10);
    } else
	strcpy(sizestr, "-");

    /* Find date field */
    date = ptr;
    ptr += 12;
    *ptr++ = '\0';
    date = HTStrip(date);

    /* Take the reminder as the filename */
    while (*ptr && isspace((int) *ptr)) ptr++;
    if ((column = strstr(ptr, " -> ")))
	*column = '\0';					   /* Strip any '->' */
    
    /* Add the element with what we've got */
    return HTDir_addElement(dir, ptr, date, sizestr, mode);
}


/*	ParseVMS
**	--------
**      Parse the VMS line and send it to the directory module
**	Returns YES if OK, NO on error
*/
PRIVATE BOOL ParseVMS (HTDir *dir, char * line)
{
    char *ptr = NULL;
    char *date = NULL;
    char *size = NULL;
    char sizestr[10];
    HTFileMode mode;
    
    /* Valid lines have the semi-colon version number token */
    if ((ptr = strchr(line, ';')) == NULL)
	return YES;
    *ptr++ ='\0';
    if (HTNextField(&ptr) == NULL) return YES;

    /* Cast VMS file and directory names to lowercase except .Z and _Z */
    {
	char *lp = line;
	while (*lp) {
	    if (strcmp(lp, ".Z")==0 || strcmp(lp, "_Z")==0) break;
	    *lp = TOLOWER(*lp);
	    lp++;
	}
	if ((lp = strstr(line, ".dir"))) {		   /* Strip any .dir */
	    mode = HT_IS_DIR;
	    *lp = '\0';
	} else
	    mode = HT_IS_FILE;
    }

    /* Find the size */
    if ((size = HTNextField(&ptr))) {
	if (mode == HT_IS_FILE) {
	    long ls = atol(size) * 512;			    /* Assume blocks */
	    HTNumToStr(ls, sizestr, 10);
	} else
	    strcpy(sizestr, "-");
    } else
	*sizestr = '\0';
	
    /* Find the date */
    {
	char *end = strchr(ptr, '[');
	if (end) *end = '\0';
	date = HTStrip(ptr);
    }
    return HTDir_addElement(dir, line, date, sizestr, mode);
}


/*	ParseFTPLine
**	-----------
**	Determines what to do with a line read from a FTP listing
**	Returns YES if OK, else NO
*/
PRIVATE BOOL ParseFTPLine (HTStream *me)
{
    if (!me->buflen) return YES;			    /* If empty line */
    switch (me->server) {
      case FTP_WINNT:
      case FTP_UNIX:
      case FTP_PETER_LEWIS:
      case FTP_MACHTEN:
	if (me->first) {
	    if (strncmp(me->buffer, "total ", 6) &&
		!strstr(me->buffer, "not available"))
		ParseUnix(me->dir, me->buffer);
	    me->first = NO;
	} else
	    ParseUnix(me->dir, me->buffer);
	break;

      case FTP_VMS:
	/* Interpret and edit LIST output from VMS server */
	/* and convert information lines to zero length.  */
	ParseVMS(me->dir, me->buffer);
	break;
	
      case FTP_NCSA:
      case FTP_TCPC:
	/* Directories identified by trailing "/" characters */
	{
	    HTFileMode mode = (*(me->buffer+me->buflen-1)=='/') ?
		HT_IS_DIR : HT_IS_FILE;
	    return HTDir_addElement(me->dir, me->buffer, NULL, NULL, mode);
	}
	break;
	
      default:
	return HTDir_addElement(me->dir, me->buffer, NULL, NULL, HT_IS_FILE);
	break;
    }
    return NO;
}

/*
**	Searches for FTP line until buffer fills up or a CRLF or LF is found
*/
PRIVATE int FTPDir_put_block (HTStream * me, const char * b, int l)
{
    while (l-- > 0) {
	if (me->state == EOL_FCR) {
	    if (*b == LF && me->buflen) {
		if (!me->junk) {
		    *(me->buffer+me->buflen) = '\0';
		    ParseFTPLine(me);
		} else
		    me->junk = NO;			   /* back to normal */
	    }
	    me->buflen = 0;
	    me->state = EOL_BEGIN;
	} else if (*b == CR) {
	    me->state = EOL_FCR;
	} else if (*b == LF && me->buflen) {
	    if (!me->junk) {
		*(me->buffer+me->buflen) = '\0';
		ParseFTPLine(me);
	    } else
		me->junk = NO;				   /* back to normal */
	    me->buflen = 0;
	    me->state = EOL_BEGIN;
	} else {
	    *(me->buffer+me->buflen++) = *b;
	    if (me->buflen >= MAX_FTP_LINE) {
		HTTRACE(PROT_TRACE, "FTP Dir..... Line too long - ignored\n");
		me->buflen = 0;
		me->junk = YES;
	    }
	}
	b++;
    }
    return HT_OK;
}

PRIVATE int FTPDir_put_string (HTStream * me, const char * s)
{
    return FTPDir_put_block(me, s, (int) strlen(s));
}

PRIVATE int FTPDir_put_character (HTStream * me, char c)
{
    return FTPDir_put_block(me, &c, 1);
}

PRIVATE int FTPDir_flush (HTStream * me)
{
    return HT_OK;
}

PRIVATE int FTPDir_free (HTStream * me)
{
    HTDir_free(me->dir);
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int FTPDir_abort (HTStream * me, HTList * e)
{
    HTTRACE(PROT_TRACE, "FTPDir...... ABORTING...\n");
    FTPDir_free(me);
    return HT_ERROR;
}

/*	FTPDir Stream
**	-----------------
*/
PRIVATE const HTStreamClass FTPDirClass =
{		
    "FTPDir",
    FTPDir_flush,
    FTPDir_free,
    FTPDir_abort,
    FTPDir_put_character,
    FTPDir_put_string,
    FTPDir_put_block
};

PUBLIC HTStream * HTFTPDir_new (HTRequest *	request,
				FTPServerType	server,
				char		list)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTFTPDir");
    me->isa = &FTPDirClass;
    me->request = request;    
    me->server = server;
    me->state = EOL_BEGIN;
    me->dir = HTDir_new(request, (list=='L' ? dir_show : 0), dir_key);
    me->first = YES;
    if (me->dir == NULL) {
	HT_FREE(me);
	return HTErrorStream();
    }
    return me;
}
