/*								     HTMIMERq.c
**	MIME MESSAGE GENERATION
**
**	This module implements the output stream for MIME used for sending
**	requests with or without a entity body to HTTP, NEWS, etc.
**
** History:
**	Jan 95 HFN	Written
*/

/* Library Includes */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTFormat.h"
#include "HTNetMan.h"
#include "HTDNS.h"
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
    SOCKFD			sockfd;
    int				version;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */
/* 			    MIME Output Request Stream			     */
/* ------------------------------------------------------------------------- */

/*	MIMEMakeRequest
**	---------------
**	Makes a MIME/1.0 request header.
*/
PRIVATE int MIMEMakeRequest (HTStream * me, HTRequest * request)
{
    char linebuf[256];
    HTParentAnchor *entity = (request->source && request->source->anchor) ?
	request->source->anchor : request->anchor;

    /* General Headers */
    if (request->GenMask & HT_DATE) {
	time_t local = time(NULL);
	sprintf(linebuf, "Date: %s%c%c", HTDateTimeStr(&local, NO), CR,LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (request->GenMask & HT_FORWARDED) {
	/* @@@@@@ */
    }
    if (request->GenMask & HT_MESSAGE_ID) {
	CONST char *msgid = HTMessageIdStr();
	if (msgid) {
	    sprintf(linebuf, "Message-ID: %s%c%c", msgid, CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
    }
    if (request->GenMask & HT_MIME) {
	sprintf(linebuf, "MIME-Version: %s%c%c", MIME_VERSION, CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (request->GenMask & HT_CONNECTION) {
	sprintf(linebuf, "Connection: Keep-Alive%c%c", CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (request->RequestMask & HT_NO_CACHE) {
	sprintf(linebuf, "Pragma: %s%c%c", "no-cache", CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }

    /* Now put out entity headers if we are using PUT or POST. If we have a
    ** PostAnchor then we take the information from this and uses the
    ** destination anchor to contain the reply. Otherwise, we have created an
    ** anchor (using internal editing etc) and we can use the destination
    ** anchor directly.
    */
    if (request->method==METHOD_PUT || request->method==METHOD_POST) {
	if (request->EntityMask & HT_ALLOW) {
	    /* @@@@@@@@@@ */
	}
	if (request->EntityMask & HT_CONTENT_ENCODING &&
	    entity->content_encoding) {
	    sprintf(linebuf, "Content-Encoding: %s%c%c",
		    HTAtom_name(entity->content_encoding), CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}

	/* @@@ SHOULD BE A LIST @@@ */
	if (request->EntityMask & HT_CONTENT_LANGUAGE &&
	    entity->content_language) {
	    sprintf(linebuf, "Content-Language: %s%c%c",
		    HTAtom_name(entity->content_language), CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
	if (request->EntityMask & HT_CONTENT_LENGTH) {   /* Must be there!!! */
	    sprintf(linebuf, "Content-Length: %ld%c%c",
		    entity->content_length, CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));	
	}
	if (request->EntityMask & HT_CTE && entity->cte) {
	    sprintf(linebuf, "Content-Transfer-Encoding: %s%c%c",
		    HTAtom_name(entity->cte), CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
	if (request->EntityMask & HT_CONTENT_TYPE && entity->content_type) {
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
	if (request->EntityMask & HT_DERIVED_FROM && entity->derived_from) {
	    sprintf(linebuf, "Derived-From: %s%c%c", entity->derived_from,
		    CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
	if (request->EntityMask & HT_EXPIRES) {
	    if (entity->expires != -1) {
		sprintf(linebuf, "Expires: %s%c%c",
			HTDateTimeStr(&entity->expires, NO), CR,LF);
		PUTBLOCK(linebuf, (int) strlen(linebuf));
	    }
	}
	if (request->EntityMask & HT_LAST_MODIFIED) {
	    if (entity->last_modified != -1) {
		sprintf(linebuf, "Last-Modified: %s%c%c",
			HTDateTimeStr(&entity->last_modified, NO), CR,LF);
		PUTBLOCK(linebuf, (int) strlen(linebuf));
	    }
	}
	if (request->EntityMask & HT_LINK) {		/* @@@@@@@@@@ */

	}
	if (request->EntityMask & HT_TITLE) {		/* @@@@@@@@@@ */

	}
	if (request->EntityMask & HT_URI) {		/* @@@@@@@@@@ */

	}
	if (request->EntityMask & HT_VERSION && entity->version) {
	    sprintf(linebuf, "Version: %s%c%c", entity->version, CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
    }

    /* Put out extra information if any */
    {
	HTList * list;
	BOOL override;
	if ((list = HTRequest_generator(request, &override))) {
	    HTList *local = list;
	    HTPostCallback *pres;
	    if (STREAM_TRACE) TTYPrint(TDEST,"MIMEGen..... Extra local\n");
	    while ((pres = (HTPostCallback *) HTList_nextObject(local)))
		(*pres)(request, me->target);
	} else if (!override && (list = HTHeader_generator())) {
	    HTList *global = list;
	    HTPostCallback *pres;
	    if (STREAM_TRACE) TTYPrint(TDEST,"MIMEGen..... Extra global\n");
	    while ((pres = (HTPostCallback *) HTList_nextObject(global)))
		(*pres)(request, me->target);
	}
    }

    /* Blank line means "end" */
    sprintf(linebuf, "%c%c", CR, LF);
    PUTBLOCK(linebuf, (int) strlen(linebuf));
    return HT_OK;
}

PRIVATE int MIMERequest_put_block (HTStream * me, CONST char * b, int l)
{
    if (me->transparent)
	return b ? PUTBLOCK(b, l) : HT_OK;
    else {
	MIMEMakeRequest(me, me->request);
#if 0
	if ((status = PUTBLOCK(HTChunkData(me->buffer),
			       HTChunkSize(me->buffer))) == HT_OK) {
	    me->transparent = YES;
	    return b ? PUTBLOCK(b, l) : HT_OK;
	}
#endif
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
	free(me);
    }
    return status;
}

PRIVATE int MIMERequest_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    free(me);
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

PUBLIC HTStream * HTMIMERequest_new (HTRequest * request, HTStream * target)
{
    HTStream * me = (HTStream *) calloc(1, sizeof(HTStream));
    if (!me) outofmem(__FILE__, "HTMIMERequest_new");
    me->isa = &MIMERequestClass;
    me->target = target;
    me->request = request;
    me->transparent = NO;
    return me;
}
