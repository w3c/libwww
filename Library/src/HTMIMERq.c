/*								     HTMIMERq.c
**	MIME ENTITY HEADERS GENERATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module implements the output stream for MIME used for sending
**	requests with a entity body to HTTP, NEWS, etc. or for generating
**	responses
**
** History:
**	Jan 95 HFN	Written
*/

/* Library Includes */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTAncMan.h"
#include "HTNetMan.h"
#include "HTReqMan.h"
#include "HTHeader.h"
#include "HTMIMERq.h"					       /* Implements */

#define PUTC(c)		(*me->target->isa->put_character)(me->target, c)
#define PUTS(s)		(*me->target->isa->put_string)(me->target, s)
#define PUTBLOCK(b, l)	(*me->target->isa->put_block)(me->target, b, l)

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    BOOL			put_fix;
    BOOL			endHeader;
    BOOL			transparent;
};

#define HT_MAX_WAIT		8      /* Max number of secs to wait for PUT */

/* ------------------------------------------------------------------------- */
/* 			    MIME Output Request Stream			     */
/* ------------------------------------------------------------------------- */

/*	MIMEMakeRequest
**	---------------
**	Generates the BODY parts of a MIME message.
*/
PRIVATE int MIMEMakeRequest (HTStream * me, HTRequest * request)
{
    char crlf[3];
    char linebuf[256];			/* @@@ */
    HTParentAnchor * entity = HTRequest_entityAnchor(request);
    HTEnHd EntityMask = HTRequest_enHd(request);
    *crlf = CR; *(crlf+1) = LF; *(crlf+2) = '\0';

    if (EntityMask & HT_E_ALLOW) {
	/* @@@@@@@@@@ */
    }
    if (EntityMask & HT_E_CONTENT_ENCODING && entity->content_encoding){
	BOOL first = YES;
	HTList * cur = entity->content_encoding;
	HTEncoding pres;
	while ((pres = (HTEncoding) HTList_nextObject(cur))) {
	    if (first) {
		PUTS("Content-Encoding: ");
		first = NO;
	    } else
		PUTC(',');
	    PUTS(HTAtom_name(pres));
	}
	if (!first) PUTBLOCK(crlf, 2);
    }
    
    if (EntityMask & HT_E_CONTENT_LANGUAGE && entity->content_language){
	BOOL first = YES;
	HTList * cur = entity->content_language;
	HTLanguage pres;
	while ((pres = (HTLanguage) HTList_nextObject(cur))) {
	    if (first) {
		PUTS("Content-Language: ");
		first = NO;
	    } else
		PUTC(',');
	    PUTS(HTAtom_name(pres));
	}
	if (!first) PUTBLOCK(crlf, 2);
    }
    if (EntityMask & HT_E_CONTENT_LENGTH) {
 	if (entity->content_length >= 0) {
	    sprintf(linebuf, "Content-Length: %ld%c%c",
		    entity->content_length, CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));	
	} else {
	    HTEncoding chunked = HTAtom_for("chunked");
	    HTAnchor_setTransfer(entity, chunked);
	}
    }
    if (EntityMask & HT_E_CTE && entity->transfer) {
	HTEncoding transfer = HTAnchor_transfer(entity);
	if (!HTFormat_isUnityTransfer(transfer)) {
	    sprintf(linebuf, "Transfer-Encoding: %s%c%c",
		    HTAtom_name(transfer), CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
    }
    if (EntityMask & HT_E_CONTENT_TYPE && entity->content_type &&
	entity->content_type != WWW_UNKNOWN) {
	HTAssocList * parameters = HTAnchor_formatParam(entity);

	/* Output the content type */
	PUTS("Content-Type: ");
	PUTS(HTAtom_name(entity->content_type));

	/* Add all parameters */
	if (parameters) {
	    HTAssoc * pres;
	    while ((pres = (HTAssoc *) HTAssocList_nextObject(parameters))) {
		PUTS(";");
		PUTS(HTAssoc_name(pres));
		PUTS("=");
		PUTS(HTAssoc_value(pres));
	    }
	}
	PUTBLOCK(crlf, 2);
#if 0
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
#endif
    }
    if (EntityMask & HT_E_DERIVED_FROM && entity->derived_from) {
	sprintf(linebuf, "Derived-From: %s%c%c", entity->derived_from,
		CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (EntityMask & HT_E_EXPIRES) {
	if (entity->expires != -1) {
	    sprintf(linebuf, "Expires: %s%c%c",
		    HTDateTimeStr(&entity->expires, NO), CR,LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
    }
    if (EntityMask & HT_E_LAST_MODIFIED) {
	if (entity->last_modified != -1) {
	    sprintf(linebuf, "Last-Modified: %s%c%c",
		    HTDateTimeStr(&entity->last_modified, NO), CR,LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
    }
    if (EntityMask & HT_E_LINK) {		/* @@@@@@@@@@ */

    }
    if (EntityMask & HT_E_TITLE) {		/* @@@@@@@@@@ */

    }
    if (EntityMask & HT_E_URI) {		/* @@@@@@@@@@ */

    }
    if (EntityMask & HT_E_VERSION && entity->version) {
	sprintf(linebuf, "Version: %s%c%c", entity->version, CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (me->endHeader) {
	sprintf(linebuf, "%c%c", CR, LF);	   /* Blank line means "end" */
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }

    /*
    ** Now make sure that the body has the right format
    */

    /* Handle any Transfer encoding */
    {
	HTEncoding transfer = HTAnchor_transfer(entity);
	if (!HTFormat_isUnityTransfer(transfer)) {
	    if (STREAM_TRACE) HTTrace("Building.... C-T-E stack\n");
	    me->target = HTTransferCodingStack(transfer, me->target,
					       request, NULL, YES);
	}
    }

    /* Handle any Content Encoding */
    {
	HTList * cc = HTAnchor_encoding(entity);
	if (cc) {
	    if (STREAM_TRACE) HTTrace("Building.... C-E stack\n");
	    me->target = HTContentDecodingStack(cc, me->target, request, NULL);
	}
    }

    if (PROT_TRACE) HTTrace("MIME........ Generating Entity Headers\n");
    return HT_OK;
}

PRIVATE int MIMERequest_put_block (HTStream * me, const char * b, int l)
{
    HTNet * net = HTRequest_net(me->request);
    if (me->transparent) {
	if (me->put_fix) {
	    HTNet * net = HTRequest_net(me->request);
	    HTEvent_unregister(HTNet_socket(net), HTEvent_READ);
	    HTEvent_unregister(HTNet_socket(net), HTEvent_WRITE);
	    me->put_fix = NO;
	}
    } else {
	MIMEMakeRequest(me, me->request);
	if (HTRequest_isDestination(me->request)) {
	    HTNet * net = HTRequest_net(me->request);
	    (*me->target->isa->flush)(me->target);
	    HTNet_setBytesWritten(net, 0);
	}
	me->transparent = YES;	

	/*
	**  If we know we are talking to an HTTP 1.1 server and we are sending
	**  an entity body then wait until we have received a 100 response
	*/
	if (HTMethod_hasEntity(HTRequest_method(me->request))) {
	    HTHost * host = HTNet_host(net);
	    char * class = HTHost_class(host);
	    if (class && !strcmp(class, "http")) {

		/*
		** If this is a HTTP/1.1 or later then wait for 100 code. If
		** it is a HTTP/1.0 server then wait a bit and hope the best.
		*/
		if (HTHost_version(host) >= 3) {
		    if (STREAM_TRACE)
			HTTrace("MIME........ Waiting for 100...\n");
		    (*me->target->isa->flush)(me->target);
		    return HT_PAUSE;
		} else {
		    HTNet * net = HTRequest_net(me->request);
		    int zzzz = HTRequest_retrys(me->request);
		    zzzz = zzzz ? zzzz * 2 : 2;
		    if (zzzz > HT_MAX_WAIT) zzzz = HT_MAX_WAIT;
		    (*me->target->isa->flush)(me->target);
		    if (STREAM_TRACE) HTTrace("MIME........ Sleeping for %d secs\n", zzzz);
		    HTEvent_register(HTNet_socket(net), HTEvent_READ, &net->event);
		    HTEvent_register(HTNet_socket(net), HTEvent_WRITE, &net->event);
		    SLEEP(zzzz);
		    me->put_fix = YES;
		    return HT_WOULD_BLOCK;
		}
	    }
	}
    }

    /* Check if we have written it all */
    if (b) {
	HTParentAnchor * entity = HTRequest_entityAnchor(me->request);
	long cl = HTAnchor_length(entity);
	return (cl>=0 && HTNet_bytesWritten(net) >= cl) ?
	    HT_LOADED : PUTBLOCK(b, l);
    }
    return HT_OK;
}

PRIVATE int MIMERequest_put_character (HTStream * me, char c)
{
    return MIMERequest_put_block(me, &c, 1);
}

PRIVATE int MIMERequest_put_string (HTStream * me, const char * s)
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
    if (PROT_TRACE) HTTrace("MIMERequest. ABORTING...\n");
    return HT_ERROR;
}

/*	MIMERequest Stream
**	-----------------
*/
PRIVATE const HTStreamClass MIMERequestClass =
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
