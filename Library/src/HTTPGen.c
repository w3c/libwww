/*								     HTTPGen.c
**	HTTP GENERAL HEADER GENERATION
**
**	This module implements the output stream for General HTTP headers
**
** History:
**	Jan 96 HFN	Written
*/

/* Library Includes */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTWWWStr.h"
#include "HTParse.h"
#include "HTFormat.h"
#include "HTAncMan.h"
#include "HTNetMan.h"
#include "HTTCP.h"
#include "HTWriter.h"
#include "HTHeader.h"
#include "HTReqMan.h"
#include "HTTPReq.h"					       /* Implements */

#define MIME_VERSION	"MIME/1.0"
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
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
    char linebuf[256];
    if (request->GenMask & HT_G_DATE) {
	time_t local = time(NULL);
	sprintf(linebuf, "Date: %s%c%c", HTDateTimeStr(&local, NO), CR,LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (request->GenMask & HT_G_FORWARDED) {
	/* @@@@@@ */
    }
    if (request->GenMask & HT_G_MESSAGE_ID) {
	CONST char *msgid = HTMessageIdStr();
	if (msgid) {
	    sprintf(linebuf, "Message-ID: %s%c%c", msgid, CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
    }
    if (request->GenMask & HT_G_MIME) {
	sprintf(linebuf, "MIME-Version: %s%c%c", MIME_VERSION, CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (request->GenMask & HT_G_CONNECTION) {
	sprintf(linebuf, "Connection: Keep-Alive%c%c", CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (request->RequestMask & HT_G_NO_CACHE) {
	sprintf(linebuf, "Pragma: %s%c%c", "no-cache", CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }

    /* Put out extra information if any */
    {
	HTList * list;
	BOOL override;
	if ((list = HTRequest_generator(request, &override))) {
	    HTList *local = list;
	    HTPostCallback *pres;
	    if (STREAM_TRACE) TTYPrint(TDEST,"HTTPGen..... Extra local\n");
	    while ((pres = (HTPostCallback *) HTList_nextObject(local)))
		(*pres)(request, me->target);
	} else if (!override && (list = HTHeader_generator())) {
	    HTList *global = list;
	    HTPostCallback *pres;
	    if (STREAM_TRACE) TTYPrint(TDEST,"HTTPGen..... Extra global\n");
	    while ((pres = (HTPostCallback *) HTList_nextObject(global)))
		(*pres)(request, me->target);
	}
    }
    if (me->endHeader) {
	sprintf(linebuf, "%c%c", CR, LF);	   /* Blank line means "end" */
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (PROT_TRACE)TTYPrint(TDEST,"HTTP........ Generating General Headers\n");
    return HT_OK;
}

PRIVATE int HTTPGen_put_block (HTStream * me, CONST char * b, int l)
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

PRIVATE int HTTPGen_put_string (HTStream * me, CONST char * s)
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
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HT_FREE(me);
    if (PROT_TRACE) TTYPrint(TDEST, "HTTPGen..... ABORTING...\n");
    return HT_ERROR;
}

/*	HTTPGen Stream
**	-----------------
*/
PRIVATE CONST HTStreamClass HTTPGenClass =
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
			       BOOL endHeader)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTTPGen_new");
    me->isa = &HTTPGenClass;
    me->target = target;
    me->request = request;
    me->endHeader = endHeader;
    me->transparent = NO;
    return me;
}
