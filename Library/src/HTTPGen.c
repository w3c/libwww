/*								     HTTPGen.c
**	HTTP GENERAL HEADER GENERATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module implements the output stream for General HTTP headers
**
** History:
**	Jan 96 HFN	Written
*/

/* Library Includes */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTHeader.h"
#include "HTTPUtil.h"
#include "HTTPReq.h"					       /* Implements */

#define MIME_VERSION	"MIME/1.0"

#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    int				version;
    BOOL			endHeader;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */
/* 			    HTTP General Header Stream			     */
/* ------------------------------------------------------------------------- */

/*	HTTPGenMake
**	------------
**	Makes a MIME/1.0 request header.
*/
PRIVATE int HTTPGenMake (HTStream * me, HTRequest * request)
{
    char linebuf[256];				/* @@@ */
    char crlf[3];
    HTGnHd gen_mask = HTRequest_gnHd(request);
    *crlf = CR; *(crlf+1) = LF; *(crlf+2) = '\0';
    if (gen_mask & HT_G_CC) {			    /* Cache control */
	HTAssocList * cur = HTRequest_cacheControl(request);
	if (cur) {
	    BOOL first=YES;
	    HTAssoc * pres;
	    while ((pres = (HTAssoc *) HTAssocList_nextObject(cur))) {
		char * value = HTAssoc_value(pres);
		if (first) {
		    PUTS("Cache-Control: ");
		    first = NO;
		} else
		    PUTC(',');

		/* Output the name */
		PUTS(HTAssoc_name(pres));

		/* Only output the value if not empty string */
		if (*value) {
		    PUTS("=");
		    PUTS(value);
		}
	    }
	    PUTBLOCK(crlf, 2);
	}
    }
    if (gen_mask & HT_G_CONNECTION) {
	HTAssocList * cur = HTRequest_connection(request);
	if (cur) {
	    BOOL first=YES;
	    HTAssoc * pres;
	    while ((pres = (HTAssoc *) HTAssocList_nextObject(cur))) {
		char * value = HTAssoc_value(pres);
		if (first) {
		    PUTS("Connection: ");
		    first = NO;
		} else
		    PUTC(',');

		/* Output the name */
		PUTS(HTAssoc_name(pres));

		/* Only output the value if not empty string */
		if (*value) {
		    PUTS("=");
		    PUTS(value);
		}
	    }
	    PUTBLOCK(crlf, 2);
	}
    }
    if (gen_mask & HT_G_DATE) {
	time_t local = HTRequest_date(request);
	sprintf(linebuf, "Date: %s%c%c", HTDateTimeStr(&local, NO), CR,LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (gen_mask & HT_G_FORWARDED) {
	/* @@@@@@ */
    }
    if (gen_mask & HT_G_PRAGMA_NO_CACHE) {
	sprintf(linebuf, "Pragma: %s%c%c", "no-cache", CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (gen_mask & HT_G_MESSAGE_ID) {
	const char *msgid = HTMessageIdStr(HTRequest_userProfile(request));
	if (msgid) {
	    sprintf(linebuf, "Message-ID: %s%c%c", msgid, CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
    }
    if (gen_mask & HT_G_MIME) {
	sprintf(linebuf, "MIME-Version: %s%c%c", MIME_VERSION, CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }

    /* Put out extra information if any */
    {
	HTList * list;
	BOOL override;
	if ((list = HTRequest_generator(request, &override))) {
	    HTList *local = list;
	    HTPostCallback *pres;
	    HTTRACE(STREAM_TRACE, "HTTPGen..... Extra local\n");
	    while ((pres = (HTPostCallback *) HTList_nextObject(local)))
		(*pres)(request, me->target);
	} else if (!override && (list = HTHeader_generator())) {
	    HTList *global = list;
	    HTPostCallback *pres;
	    HTTRACE(STREAM_TRACE, "HTTPGen..... Extra global\n");
	    while ((pres = (HTPostCallback *) HTList_nextObject(global)))
		(*pres)(request, me->target);
	}
    }
    if (me->endHeader) {
	sprintf(linebuf, "%c%c", CR, LF);	   /* Blank line means "end" */
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    HTTRACE(PROT_TRACE, "HTTP........ Generating General Headers\n");
    return HT_OK;
}

PRIVATE int HTTPGen_put_block (HTStream * me, const char * b, int l)
{
    if (me->transparent)
	return b ? PUTBLOCK(b, l) : HT_OK;
    else {
	HTTPGenMake(me, me->request);
	me->transparent = YES;	
	return b ? PUTBLOCK(b, l) : HT_OK;
    }
}

PRIVATE int HTTPGen_put_character (HTStream * me, char c)
{
    return HTTPGen_put_block(me, &c, 1);
}

PRIVATE int HTTPGen_put_string (HTStream * me, const char * s)
{
    return HTTPGen_put_block(me, s, strlen(s));
}

/*
**	Flushes header but doesn't free stream object
*/
PRIVATE int HTTPGen_flush (HTStream * me)
{
    int status = HTTPGen_put_block(me, NULL, 0);
    return status==HT_OK ? (*me->target->isa->flush)(me->target) : status;
}

/*
**	Flushes data and frees stream object
*/
PRIVATE int HTTPGen_free (HTStream * me)
{
    int status = HTTPGen_flush(me);
    if (status != HT_WOULD_BLOCK) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	HT_FREE(me);
    }
    return status;
}

PRIVATE int HTTPGen_abort (HTStream * me, HTList * e)
{
    HTTRACE(PROT_TRACE, "HTTPGen..... ABORTING...\n");
    if (me) {
	if (me->target) (*me->target->isa->abort)(me->target, e);
	HT_FREE(me);
    }
    return HT_ERROR;
}

/*	HTTPGen Stream
**	-----------------
*/
PRIVATE const HTStreamClass HTTPGenClass =
{		
    "HTTPGen",
    HTTPGen_flush,
    HTTPGen_free,
    HTTPGen_abort,
    HTTPGen_put_character,
    HTTPGen_put_string,
    HTTPGen_put_block
};

PUBLIC HTStream * HTTPGen_new (HTRequest * request, HTStream * target,
			       BOOL endHeader, int version)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTTPGen_new");
    me->isa = &HTTPGenClass;
    me->target = target;
    me->request = request;
    me->endHeader = endHeader;
    me->transparent = NO;

    /*
    **  For backwards compatibility with HTTP applications that understand
    **  Connection: Keep-Alive, we send it along. However, we do NOT send
    **  it to a proxy as it may confuse HTTP/1.0 proxies. Also we do not
    **  send it if the app has set Connection: close
    */
    me->version = version;
    if (me->version == HTTP_10 && HTRequest_proxy(request) == NULL) {
	HTAssocList * alist = HTRequest_connection(request);
	if (!(alist && HTAssocList_findObject(alist, "close")))
	    HTRequest_addConnection(request, "Keep-Alive", "");
    }

    /*
    **  Check for any TE headers that are also hop-by-hop
    */
    if (HTFormat_transferCoding() != NULL || HTRequest_transfer(request) != NULL)
	HTRequest_addConnection(request, "TE", "");

    return me;
}
