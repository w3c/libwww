/*								     HTNewsLs.c
**	NEWS (NNTP) GROUP LISTINGS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
** Authors
**	FTLO	Felix Lo
**	HFN	Henrik Frystyk <frystyk@w3.org>
**
** History:
**	Oct 95	HFN	Written
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTStream.h"
#include "HTFormat.h"
#include "HTSocket.h"
#include "HTNDir.h"
#include "HTNews.h"
#include "HTNewsLs.h"					 /* Implemented here */

#define DELIMITER		'\t'

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTRequest *			request;
    HTSocketEOL			state;
    HTNewsDir *			dir;
    BOOL			group;
    BOOL			junk;
    char			buffer[MAX_NEWS_LINE+1];
    int				buflen;
};

PRIVATE HTNewsDirKey		dir_key = HT_NDK_THREAD;

/* ------------------------------------------------------------------------- */


/*	ParseList
**	---------
**	Extract the group name from a LIST listing
**	Returns YES if OK, NO on error
*/
PRIVATE BOOL ParseList (HTNewsDir *dir, char * line)
{
    char *ptr = line;
    while (*ptr && !WHITE(*ptr)) ptr++;
    *ptr = '\0';
    return HTNewsDir_addElement(dir, line, 0, NULL, NULL, 0);
}


/*	ParseGroup
**	----------
**	Extract the index number, subject etc, from a XOVER command. Expects
**	the following format of the line:
**
**		<index> <subject> <from> <data> <msgid> [*<thread>] ...
**
**	Returns YES if OK, NO on error
*/
PRIVATE BOOL ParseGroup (HTNewsDir *dir, char * line)
{
    int index;
    int refcnt=0;
    char *msgid;
    char *from;
    char *ptr;
    char *subject = line;				/* Index */
    while (*subject && *subject != DELIMITER) subject++;
    *subject++ = '\0';
    index = atoi(line);
    from = subject;					/* Subject */
    while (*from && *from != DELIMITER) from++;
    *from++ = '\0';
    ptr = from;						/* Date */
    while (*ptr && *ptr != DELIMITER) ptr++;
    *ptr++ = '\0';
    msgid = ptr;					/* Msg-ID */
    while (*msgid && *msgid != DELIMITER) msgid++;
    *msgid++ = '\0';
    ptr = msgid;					/* Bytes or ref */
    while (*ptr && *ptr != DELIMITER) ptr++;
    *ptr++ = '\0';
    while (!isdigit(*ptr)) {
	while (*ptr && *ptr != DELIMITER) ptr++;
	*ptr++ = '\0';
	refcnt++;
    }
    return HTNewsDir_addElement(dir, msgid, index, subject, from, refcnt);
}

/*
**	Searches for News line until buffer fills up or a CRLF or LF is found
*/
PRIVATE int HTNewsList_put_block (HTStream * me, CONST char * b, int l)
{
    while (l-- > 0) {
	if (me->state == EOL_FCR) {
	    if (*b == LF && me->buflen) {
		if (!me->junk) {
		    *(me->buffer+me->buflen) = '\0';
		    me->group ? ParseGroup(me->dir, me->buffer) :
			ParseList(me->dir, me->buffer);
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
		me->group ? ParseGroup(me->dir, me->buffer) :
		    ParseList(me->dir, me->buffer);
	    } else
		me->junk = NO;				   /* back to normal */
	    me->buflen = 0;
	    me->state = EOL_BEGIN;
	} else {
	    *(me->buffer+me->buflen++) = *b;
	    if (me->buflen >= MAX_NEWS_LINE) {
		if (PROT_TRACE)
		    TTYPrint(TDEST, "News Dir.... Line too long - chopped\n");
		*(me->buffer+me->buflen) = '\0';
		me->group ? ParseGroup(me->dir, me->buffer) :
		    ParseList(me->dir, me->buffer);
		me->buflen = 0;
		me->junk = YES;
	    }
	}
	b++;
    }
    return HT_OK;
}

PRIVATE int HTNewsList_put_character (HTStream * me, char ch)
{
    return HTNewsList_put_block(me, &ch, 1);
}

PRIVATE int HTNewsList_put_string (HTStream * me, CONST char * s)
{
    return HTNewsList_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTNewsList_flush (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTNewsList_free (HTStream * me)
{
    HTNewsDir_free(me->dir);
    free(me);
    return HT_OK;
}

PRIVATE int HTNewsList_abort (HTStream * me, HTList * e)
{
    if (PROT_TRACE) TTYPrint(TDEST, "News Dir.... ABORTING...\n");
    HTNewsList_free(me);
    return HT_ERROR;
}

PRIVATE CONST HTStreamClass HTNewsListClass =
{               
    "NewsList",
    HTNewsList_flush,
    HTNewsList_free,
    HTNewsList_abort,
    HTNewsList_put_character,
    HTNewsList_put_string,
    HTNewsList_put_block
};

PUBLIC HTStream *HTNewsList (HTRequest *	request,
			     void *		param,  
			     HTFormat		input_format,
			     HTFormat		output_format,
			     HTStream *		output_stream)
{
    HTStream *me = (HTStream *) calloc(1, sizeof(HTStream));
    if (!me) outofmem(__FILE__, "HTNewsList_new");
    me->isa = &HTNewsListClass;
    me->request = request;
    me->state = EOL_BEGIN;
    me->dir = HTNewsDir_new(request, "Newsgroups", HT_NDK_GROUP);
    if (me->dir == NULL) FREE(me);
    return me;
}

PUBLIC HTStream *HTNewsGroup (HTRequest *	request,
			      void *		param,  
			      HTFormat		input_format,
			      HTFormat		output_format,
			      HTStream *	output_stream)
{
    HTStream *me = (HTStream *) calloc(1, sizeof(HTStream));
    if (!me) outofmem(__FILE__, "HTNewsList_new");
    me->isa = &HTNewsListClass;
    me->request = request;
    me->state = EOL_BEGIN;
    me->group = YES;
    me->dir = HTNewsDir_new(request, "Newsgroup", dir_key);
    if (me->dir == NULL) FREE(me);
    return me;
}
