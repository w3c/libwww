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
#include "HTParse.h"
#include "HTSocket.h"
#include "HTReq.h"
#include "HTNDir.h"
#include "HTNews.h"
#include "HTNewsLs.h"					 /* Implemented here */

#define DELIMITER		'\t'
#define ATSIGN			'@'

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

PRIVATE HTNewsDirKey		dir_key = HT_NDK_NONE;

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
    return HTNewsDir_addElement(dir, 0, line, NULL, (time_t) 0, line, 0);
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
    time_t t=0;
    char *subject = line;
    char *from;
    char *date;
    char *msgid;
    char *ptr=NULL;
    while (*subject && *subject != DELIMITER) subject++;
    *subject++ = '\0';					/* Index */
    index = atoi(line);
    from = subject;
    while (*from && *from != DELIMITER) from++;
    *from++ = '\0';					/* Subject */
    date = from;
    while (*date && *date != DELIMITER) {
	if (*date=='<' || *date=='(') {
	    ptr = date+1;
	    *date = '\0';
	}
	if (*date=='>' || *date==')') *date = '\0';
	date++;
    }
    *date++ = '\0';
    if (strchr(from, ATSIGN) && ptr) from = ptr;	/* From */
    msgid = date;
    while (*msgid && *msgid != DELIMITER) msgid++;
    *msgid++ = '\0';					/* Date */
    if (*msgid=='<') msgid++;
#if 0
    t = HTParseTime(date);
#endif
    ptr = msgid;
    while (*ptr && *ptr != DELIMITER) {
	if (*ptr=='>') *ptr = '\0';
	ptr++;
    }
    *ptr++ = '\0';					/* MsgId */
    while (!isdigit(*ptr)) {
	while (*ptr && *ptr != DELIMITER) ptr++;
	*ptr++ = '\0';
	refcnt++;
    }
    return HTNewsDir_addElement(dir, index, subject, from, t, msgid, refcnt);
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
    HT_FREE(me);
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
    HTStream *me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTNewsList_new");
    me->isa = &HTNewsListClass;
    me->request = request;
    me->state = EOL_BEGIN;
    me->dir = HTNewsDir_new(request, "Newsgroups", HT_NDK_GROUP);
    if (me->dir == NULL) HT_FREE(me);
    return me;
}

PUBLIC HTStream *HTNewsGroup (HTRequest *	request,
			      void *		param,  
			      HTFormat		input_format,
			      HTFormat		output_format,
			      HTStream *	output_stream)
{
    char * url = HTAnchor_physical(HTRequest_anchor(request));
    char * title = NULL;
    HTStream *me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTNewsList_new");
    StrAllocCopy(title, "Newsgroup: ");
    if (!strncasecomp(url, "news:", 5))
	StrAllocCat(title, url+5);
    else
	StrAllocCat(title, HTParse(url, "", PARSE_PATH));
    me->isa = &HTNewsListClass;
    me->request = request;
    me->state = EOL_BEGIN;
    me->group = YES;
    me->dir = HTNewsDir_new(request, title, dir_key);
    if (me->dir == NULL) HT_FREE(me);
    HT_FREE(title);
    return me;
}
