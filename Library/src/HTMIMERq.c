/*								     HTMIMERq.c
**	MIME ENTITY HEADERS GENERATION
**
**	This module implements the output stream for MIME used for sending
**	requests with a entity body to HTTP, NEWS, etc. or for generating
**	responses
**
** History:
**	Jan 95 HFN	Written
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
#include "HTDNS.h"
#include "HTTCP.h"
#include "HTWriter.h"
#include "HTHeader.h"
#include "HTReqMan.h"
#include "HTTPReq.h"					       /* Implements */

#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    BOOL			endHeader;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */
/* 			    MIME Output Request Stream			     */
/* ------------------------------------------------------------------------- */

/*	MIMEMakeRequest
**	---------------
**	Generates the BODY parts of a MIME message.
*/
PRIVATE int MIMEMakeRequest (HTStream * me, HTRequest * request)
{
    char linebuf[256];			/* @@@ */
    HTParentAnchor *entity = request->source_anchor ?
	request->source_anchor : request->anchor;

    if (request->EntityMask & HT_E_ALLOW) {
	/* @@@@@@@@@@ */
    }
    if (request->EntityMask & HT_E_CONTENT_ENCODING &&
	entity->content_encoding) {
	sprintf(linebuf, "Content-Encoding: %s%c%c",
		HTAtom_name(entity->content_encoding), CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    
    /* @@@ SHOULD BE A LIST @@@ */
    if (request->EntityMask & HT_E_CONTENT_LANGUAGE &&
	entity->content_language) {
	sprintf(linebuf, "Content-Language: %s%c%c",
		HTAtom_name(entity->content_language), CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (request->EntityMask & HT_E_CONTENT_LENGTH) { /* Must be there!!! */
	sprintf(linebuf, "Content-Length: %ld%c%c",
		entity->content_length, CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));	
    }
    if (request->EntityMask & HT_E_CTE && entity->cte) {
	sprintf(linebuf, "Content-Transfer-Encoding: %s%c%c",
		HTAtom_name(entity->cte), CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (request->EntityMask & HT_E_CONTENT_TYPE && entity->content_type) {
	int len;
	sprintf(linebuf, "Content-Type: %s",
		HTAtom_name(entity->content_type));
	if (entity->charset) {
	    strcat(linebuf, "; charset=");
	    strcat(linebuf, HTAtom_name(entity->charset));
	}
	if (entity->level) {
	    strcat(linebuf, "; level=");
	    strcat(linebuf, HTAtom_name(entity->level));
	}
	len = strlen(linebuf);
	*(linebuf+len) = CR;
	*(linebuf+len+1) = LF;
	*(linebuf+len+2) = '\0';
	PUTBLOCK(linebuf, (int) len+2);
    }
    if (request->EntityMask & HT_E_DERIVED_FROM && entity->derived_from) {
	sprintf(linebuf, "Derived-From: %s%c%c", entity->derived_from,
		CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (request->EntityMask & HT_E_EXPIRES) {
	if (entity->expires != -1) {
	    sprintf(linebuf, "Expires: %s%c%c",
		    HTDateTimeStr(&entity->expires, NO), CR,LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
    }
    if (request->EntityMask & HT_E_LAST_MODIFIED) {
	if (entity->last_modified != -1) {
	    sprintf(linebuf, "Last-Modified: %s%c%c",
		    HTDateTimeStr(&entity->last_modified, NO), CR,LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
    }
    if (request->EntityMask & HT_E_LINK) {		/* @@@@@@@@@@ */

    }
    if (request->EntityMask & HT_E_TITLE) {		/* @@@@@@@@@@ */

    }
    if (request->EntityMask & HT_E_URI) {		/* @@@@@@@@@@ */

    }
    if (request->EntityMask & HT_E_VERSION && entity->version) {
	sprintf(linebuf, "Version: %s%c%c", entity->version, CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (me->endHeader) {
	sprintf(linebuf, "%c%c", CR, LF);	   /* Blank line means "end" */
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (PROT_TRACE) TTYPrint(TDEST,"MIME........ Generating Entity Headers\n");
    return HT_OK;
}

PRIVATE int MIMERequest_put_block (HTStream * me, CONST char * b, int l)
{
    if (me->transparent)
	return b ? PUTBLOCK(b, l) : HT_OK;
    else {
	MIMEMakeRequest(me, me->request);
	if (HTRequest_isDestination(me->request)) {
	    (*me->target->isa->flush)(me->target);
	    HTNet_setBytesWritten(me->request->net, 0);
	}
	me->transparent = YES;	
	return b ? PUTBLOCK(b, l) : HT_OK;
    }
}

PRIVATE int MIMERequest_put_character (HTStream * me, char c)
{
    return MIMERequest_put_block(me, &c, 1);
}

PRIVATE int MIMERequest_put_string (HTStream * me, CONST char * s)
{
    return MIMERequest_put_block(me, s, strlen(s));
}

/*
**	Flushes header but doesn't free stream object
*/
PRIVATE int MIMERequest_flush (HTStream * me)
{
    int status = MIMERequest_put_block(me, NULL, 0);
    return status==HT_OK ? (*me->target->isa->flush)(me->target) : status;
}

/*
**	Flushes data and frees stream object
*/
PRIVATE int MIMERequest_free (HTStream * me)
{
    int status = MIMERequest_flush(me);
    if (status != HT_WOULD_BLOCK) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
	HT_FREE(me);
    }
    return status;
}

PRIVATE int MIMERequest_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HT_FREE(me);
    if (PROT_TRACE) TTYPrint(TDEST, "MIMERequest. ABORTING...\n");
    return HT_ERROR;
}

/*	MIMERequest Stream
**	-----------------
*/
PRIVATE CONST HTStreamClass MIMERequestClass =
{		
    "MIMERequest",
    MIMERequest_flush,
    MIMERequest_free,
    MIMERequest_abort,
    MIMERequest_put_character,
    MIMERequest_put_string,
    MIMERequest_put_block
};

PUBLIC HTStream * HTMIMERequest_new (HTRequest * request, HTStream * target,
				     BOOL endHeader)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTMIMERequest_new");
    me->isa = &MIMERequestClass;
    me->target = target;
    me->request = request;
    me->endHeader = endHeader;
    me->transparent = NO;
    return me;
}
