/*								     HTNewsRq.c
**	NNTP MESSAGE GENERATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module implements the output stream for MIME used for sending
**	requests with or without a entity body to HTTP, NEWS, etc.
**
** History:
**	Jan 95 HFN	Written
*/

/* Library Includes */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"

#include "HTReqMan.h"
#include "HTMIMERq.h"
#include "HTNewsRq.h"					       /* Implements */

#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    SOCKET			sockfd;
    HTChunk *  			buffer;
    int				version;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */
/* 			    News Output Post Stream			     */
/* ------------------------------------------------------------------------- */

/*	NewsPost_start
**	--------------
**	NNTP needs two extra headers: "From" and "Newsgroups".
**	Take the newsgroups from the Postweb model as destinations for this
**	anchor.
**	Return YES if OK else NO
*/
PRIVATE BOOL NewsPost_start (HTStream * me, HTRequest * request)
{
    char linebuf[128];		/* @@@ */
    HTChunk *header = me->buffer;
    HTUserProfile * up = HTRequest_userProfile(request);
    const char * mailaddress = HTUserProfile_email(up);
    if (mailaddress) {
	sprintf(linebuf, "From: %s%c%c", mailaddress, CR, LF);
	HTChunk_puts(header, linebuf);
    }

    /*
    **	Find all the newsgroups we are posting to by looking at all the
    **  destinations from the source of this request.
    ** 	First the main link and then the sub links
    */
    HTChunk_puts(header, "Newsgroups :");    
    if (HTRequest_isDestination(request)) {
	HTRequest *src_req = HTRequest_source(request);
	HTParentAnchor *src_anchor = HTRequest_anchor(src_req);
	HTLink *link = HTAnchor_mainLink((HTAnchor *) src_anchor);
	HTAnchor *dest = HTLink_destination(link);
	HTMethod method = HTLink_method(link);
	if (link && method == METHOD_POST &&
	    HTLink_result(link) == HT_LINK_NONE) {
	    char *desturl = HTAnchor_physical((HTParentAnchor *) dest);
	    char *access = HTParse(desturl, "", PARSE_ACCESS);
	    if (!strcasecomp(access, "news") || !strcasecomp(access, "nntp")) {
		char *newsgroup = HTParse(desturl, "", PARSE_PATH);
		HTUnEscape(newsgroup);
		HTCleanTelnetString(newsgroup);
		HTChunk_puts(header, newsgroup);
		HT_FREE(newsgroup);
	    }
	    HT_FREE(access);
	}

	/* DO FOR ALL SUB ANCHOR DESTINATION S AS WELL */
	
    }
    HTTRACE(PROT_TRACE, "News Tx..... %s" _ HTChunk_data(header));
    return YES;
}

/*	NewsPost_end
**	------------
**	End the posting by CRLF.CRLF
**	returns whatever PUT_BLOCK returns
*/
PRIVATE int NewsPost_end (HTStream * me)
{
    char buf[6];
    *buf = CR;
    *(buf+1) = LF;
    *(buf+2) = '.';
    *(buf+3) = CR;
    *(buf+4) = LF;
    *(buf+5) = '\0';
    return PUTBLOCK(buf, 5);
}

PRIVATE int NewsPost_put_block (HTStream * me, const char* b, int l)
{
    if (!me->target) {
	return HT_WOULD_BLOCK;
    } else if (me->transparent)
	return b ? PUTBLOCK(b, l) : HT_OK;
    else {
	int status;
	NewsPost_start(me, me->request);
	if ((status = PUTBLOCK(HTChunk_data(me->buffer),
			       HTChunk_size(me->buffer))) == HT_OK) {
	    me->transparent = YES;
	    return b ? PUTBLOCK(b, l) : HT_OK;
	}
	return status;
    }
}

PRIVATE int NewsPost_put_character (HTStream * me, char c)
{
    return NewsPost_put_block(me, &c, 1);
}

PRIVATE int NewsPost_put_string (HTStream * me, const char * s)
{
    return NewsPost_put_block(me, s, strlen(s));
}

/*
**	Flushes header but doesn't free stream object
*/
PRIVATE int NewsPost_flush (HTStream * me)
{
    return NewsPost_put_block(me, NULL, 0);
}

/*
**	Flushes data and frees stream object
*/
PRIVATE int NewsPost_free (HTStream * me)
{
    int status;
    if ((status = NewsPost_flush(me)) != HT_OK ||
	(status = NewsPost_end(me)) != HT_OK ||
	(status = (*me->target->isa->_free)(me->target)) != HT_OK)
	return status;
    HTChunk_delete(me->buffer);
    HT_FREE(me);
    return status;
}

PRIVATE int NewsPost_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HTChunk_delete(me->buffer);
    HT_FREE(me);
    HTTRACE(PROT_TRACE, "NewsPost.... ABORTING...\n");
    return HT_ERROR;
}

/*	NewsPost Stream
**	-----------------
*/
PRIVATE const HTStreamClass NewsPostClass =
{		
    "NewsPost",
    NewsPost_flush,
    NewsPost_free,
    NewsPost_abort,
    NewsPost_put_character,
    NewsPost_put_string,
    NewsPost_put_block
};

PUBLIC HTStream * HTNewsPost_new (HTRequest * request, HTStream * target)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("NewsPost_new");
    me->isa = &NewsPostClass;
    me->target = target;
    me->request = request;
    me->buffer = HTChunk_new(256);
    me->transparent = NO;
    return HTMIMERequest_new(request, me, YES);
}
