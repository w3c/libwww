/*								     HTNewsLs.c
**	NEWS (NNTP) GROUP LISTINGS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** Authors
**	FTLO	Felix Lo
**	HFN	Henrik Frystyk <frystyk@w3.org>
**	MP	Maciej Puzio <puzio@zodiac1.mimuw.edu.pl>
**
** History:
**	Oct 95	HFN	Written
**  Mar 96  MP  Modified
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTUTree.h"
#include "HTNDir.h"
#include "HTNews.h"
#include "HTNewsLs.h"					 /* Implemented here */

#define DELIMITER		'\t'
#define ATSIGN			'@'

#define NEWS_TREE		"w3c-news"

struct _HTStream {
    const HTStreamClass *	isa;
    HTRequest *			request;
    HTEOLState			state;
    HTNewsDir *			dir;
    BOOL			group;
    BOOL			junk;
    char			buffer[MAX_NEWS_LINE+1];
    int				buflen;
};

typedef struct _HTNewsCache {
    char *	host;
    HTArray *	cache;
} HTNewsCache;

PRIVATE HTNewsDirKey dir_key = HT_NDK_REFTHREAD;
PRIVATE HTNewsDirKey list_key = HT_NDK_GROUP;     /* Added by MP. */

/* ------------------------------------------------------------------------- */

/* Helper function added by MP. */
PRIVATE char* GetNewsGroupTitle (HTRequest* request)
{
    char * url = HTAnchor_physical(HTRequest_anchor(request));
    char * title = NULL;
    if (strrchr(url, '*'))
        StrAllocCopy(title, "Newsgroups: ");
    else
        StrAllocCopy(title, "Newsgroup: ");
    if (!strncasecomp(url, "news:", 5))
	StrAllocCat(title, url+5);
    else
	StrAllocCat(title, HTParse(url, "", PARSE_PATH));
    return title;
}

PRIVATE BOOL ParseList (HTNewsDir * dir, char * line)
{
    char *ptr = line;
    while (*ptr && !isspace((int) *ptr)) ptr++;
    *ptr = '\0';
    /* Changed by MP */
    return (HTNewsDir_addGroupElement(dir, line, NO) != NULL);
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
PRIVATE BOOL ParseGroup (HTRequest * request, HTNewsDir *dir, char * line)
{
    int index;
    int refcnt=0;
    time_t t=0;
    char *subject = line;
    char *from;
    char *date;
    char *msgid;
    char *ptr=NULL;
    HTList* reflist = NULL;  /* Added by MP. */
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
    t = HTParseTime(date,  HTRequest_userProfile(request), YES);
    ptr = msgid;
    while (*ptr && *ptr != DELIMITER) {
	if (*ptr=='>') *ptr = '\0';
	ptr++;
    }
    *ptr++ = '\0';					/* MsgId */
    while (ptr && *ptr && !isdigit((int) *ptr)) {
        char* refstart = ptr;       /* Added by MP. */
        char* refcopy = NULL;
	    char* refstop;
	    while (*ptr && *ptr != DELIMITER && *ptr != ' ') ptr++;
	    refstop = ptr - 1;
	    *ptr++ = '\0';
        if (strlen(refstart) > 0)  /* Added by MP. */
	    {
	        refcnt++;
            if (*refstart == '<')  refstart++;
            if (*refstop == '>')  *refstop = '\0';
            if (reflist == NULL)  reflist = HTList_new();
            StrAllocCopy (refcopy, refstart);
            HTList_addObject (reflist, (void*) refcopy);
        }
    }
    /* Changed by MP. */
    return (HTNewsDir_addElement(dir, index, subject, from, t, msgid, 
        refcnt, reflist) != NULL);
}

/* ------------------------------------------------------------------------- */
/*				NEWS CACHE				     */
/* ------------------------------------------------------------------------- */

PRIVATE HTNewsCache * HTNewsCache_new (const char * newshost, HTArray * array)
{
    if (newshost && array) {
	HTNewsCache * me;
     	if ((me = (HTNewsCache *) HT_CALLOC(1, sizeof(HTNewsCache))) == NULL)
	    HT_OUTOFMEM("HTNewsCache_new");
	StrAllocCopy(me->host, newshost);
	me->cache = array;
	return me;
    }
    return NULL;
}

/*
**	Instead of just deleting we could save it to file.
*/
PRIVATE int HTNewsCache_delete (void * context)
{
    HTNewsCache * me = (HTNewsCache *) context;
    if (me) {
	if (me->cache) {
    	    void ** data;
    	    char * line = (char *) HTArray_firstObject(me->cache, data);
    	    while (line) {
		HT_FREE(line);
		line = (char *) HTArray_nextObject(me->cache, data);
    	    }
	    HTArray_delete(me->cache);
	}
	HT_FREE(me->host);
	HTTRACE(PROT_TRACE, "News Cache.. Deleted cache %p\n" _ me);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/*
**  Look for cached information for this news server
**  We store the information in a URL Tree so that we can have multiple
**  servers stored simultanously
*/
PRIVATE HTNewsCache * HTNewsCache_find (HTRequest * request, const char * url)
{
    HTUTree * tree = NULL;
    if (request && url) {
	char * newshost = NULL;
	HTNewsCache * element = NULL;
	if (!strncasecomp(url, "news:", 5)) {
	    HTUserProfile * up = HTRequest_userProfile(request);
	    StrAllocCopy(newshost, HTUserProfile_news(up));
	} else if (!strncasecomp(url, "nntp:", 5)) {
	    newshost = HTParse(url, "", PARSE_HOST);
	}

	/* If we have a news server then continue to find a URL tree */
	if (newshost) {
	    char * colon = strchr(newshost, ':');
	    int port = NEWS_PORT;
	    if (colon ) {
		*(colon++) = '\0';		     /* Chop off port number */
		port = atoi(colon);
	    }
	    tree = HTUTree_find(NEWS_TREE, newshost, port);
	    HT_FREE(newshost);
	    if (!tree) {
		HTTRACE(PROT_TRACE, "News Cache.. No information for `%s\'\n" _ url);
		return NULL;
	    }

	    /* Find a cache element (if any) */
	    element = (HTNewsCache *) HTUTree_findNode(tree, "", "/");
	    return element;
	}
    }
    return NULL;
}

PRIVATE BOOL HTNewsCache_update (HTRequest * request,
				 const char * url, HTArray * array)
{
    HTUTree * tree = NULL;
    if (request && url) {
	char * newshost = NULL;
	if (!strncasecomp(url, "news:", 5)) {
	    HTUserProfile * up = HTRequest_userProfile(request);
	    StrAllocCopy(newshost, HTUserProfile_news(up));
	} else if (!strncasecomp(url, "nntp:", 5)) {
	    newshost = HTParse(url, "", PARSE_HOST);
	}

	/* 
	**  If the news server was found then update the data entry. Otherwise
	**  create a new entry
	*/
	if (newshost) {
	    char * colon = strchr(newshost, ':');
	    int port = NEWS_PORT;
	    if (colon ) {
		*(colon++) = '\0';		     /* Chop off port number */
		port = atoi(colon);
	    }
	    tree = HTUTree_new(NEWS_TREE, newshost, port, HTNewsCache_delete);
	    HT_FREE(newshost);
	    if (!tree) {
		HTTRACE(PROT_TRACE, "News Cache.. Can't create tree\n");
		return NO;
	    }

	    /* Add new cache information to the tree */
	    {
		HTNewsCache * element = NULL;
		BOOL status;
		if ((element=(HTNewsCache *) HTUTree_findNode(tree, "", "/"))){
		    element->cache = array;
		    status = YES;
		} else {
		    element = HTNewsCache_new(url, array);
		    status = HTUTree_addNode(tree, "", "/", element);
		}
		return status;
	    }
	}
    }
    return NO;
}

/*
**	Before filter: Check whether we have a cache entry for this host
*/
PUBLIC int HTNewsCache_before (HTRequest * request, void * context, int mode)
{
    char * url = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
    HTNewsCache * element = HTNewsCache_find(request, url);
    HT_FREE(url);
    /*
    **  If we have found a cache object then create a new dir obejct and fill
    **  it with data from the cache
    */
    if (element) {
	char * title = GetNewsGroupTitle(request);
	HTNewsDir * dir = HTNewsDir_new(request, title, list_key, NO);
	void ** data = NULL;
	char * line = (char *) HTArray_firstObject(element->cache, data);
	while (line) {
	    HTNewsDir_addGroupElement(dir, line, NO);
	    line = (char *) HTArray_nextObject(element->cache, data);
	}

	/*
	**  After filling the new dir object we write it out and free it again
	*/
	HTNewsDir_free(dir);
	HT_FREE(title);
	return HT_LOADED;
    }
    return HT_OK;
}

/*
**	After filter: Update the cache entry for this host
*/
PUBLIC int HTNewsCache_after (HTRequest * request, HTResponse * response,
			      void * context, int status)
{
    HTArray * array = (HTArray *) context;
    HTTRACE(PROT_TRACE, "News Cache.. AFTER filter\n");
    if (request && array) {
	char * url = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
	HTNewsCache_update(request, url, array);
	HT_FREE(url);
    }
    return HT_OK;
}

/* ------------------------------------------------------------------------- */

/*
**	Searches for News line until buffer fills up or a CRLF or LF is found
*/
PRIVATE int HTNewsList_put_block (HTStream * me, const char * b, int l)
{
    while (l-- > 0) {
	if (me->state == EOL_FCR) {
	    if (*b == LF && me->buflen) {
		if (!me->junk) {
		    *(me->buffer+me->buflen) = '\0';
		    me->group ? ParseGroup(me->request, me->dir, me->buffer) :
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
		me->group ? ParseGroup(me->request, me->dir, me->buffer) :
		    ParseList(me->dir, me->buffer);
	    } else
		me->junk = NO;				   /* back to normal */
	    me->buflen = 0;
	    me->state = EOL_BEGIN;
	} else {
	    *(me->buffer+me->buflen++) = *b;
	    if (me->buflen >= MAX_NEWS_LINE) {
		HTTRACE(PROT_TRACE, "News Dir.... Line too long - chopped\n");
		*(me->buffer+me->buflen) = '\0';
		me->group ? ParseGroup(me->request, me->dir, me->buffer) :
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

PRIVATE int HTNewsList_put_string (HTStream * me, const char * s)
{
    return HTNewsList_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTNewsList_flush (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTNewsList_free (HTStream * me)
{
    HTNewsList_put_character (me, '\n');  /* to flush the last item; added by MP. */
    HTNewsDir_free(me->dir);
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int HTNewsList_abort (HTStream * me, HTList * e)
{
    HTTRACE(PROT_TRACE, "News Dir.... ABORTING...\n");
    HTNewsList_free(me);
    return HT_ERROR;
}

PRIVATE const HTStreamClass HTNewsListClass =
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
    {
	char * title = GetNewsGroupTitle(request);
	me->dir = HTNewsDir_new(request, title, list_key,YES);
	HT_FREE(title);
    }
    /* Modified by MP. */
    if (me->dir == NULL) HT_FREE(me);
    return me;
}

PUBLIC HTStream *HTNewsGroup (HTRequest *	request,
			      void *		param,  
			      HTFormat		input_format,
			      HTFormat		output_format,
			      HTStream *	output_stream)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTNewsList_new");
    me->isa = &HTNewsListClass;
    me->request = request;
    me->state = EOL_BEGIN;
    me->group = YES;
    {
	char * title = GetNewsGroupTitle(request);
	me->dir = HTNewsDir_new(request, title, dir_key, YES);
	HT_FREE(title);
    }
    /* Modified by MP. */
    if (me->dir == NULL) HT_FREE(me);
    return me;
}
