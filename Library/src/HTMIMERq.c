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
#include "wwwsys.h"
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
    BOOL			endHeader;
    BOOL			transparent;
};

#define HT_MAX_WAIT		8      /* Max number of secs to wait for PUT */

PRIVATE int MIMERequest_put_block (HTStream * me, const char * b, int l);

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
    BOOL transfer_coding = NO;		/* We should get this from the Host object */
    *crlf = CR; *(crlf+1) = LF; *(crlf+2) = '\0';

    if (EntityMask & HT_E_ALLOW) {
	BOOL first = YES;
	int cnt;
	HTMethod methodset = HTAnchor_allow(entity);
	for (cnt=0; cnt<sizeof(HTMethod)<<3 ; cnt++) {
	    if (methodset & (1<<cnt)) {
		if (first) {
		    PUTS("Allow: ");
		    first = NO;
		} else
		    PUTC(',');
		PUTS(HTMethod_name(1<<cnt));
	    }
	}
	if (!first) PUTBLOCK(crlf, 2);
    }
    if (EntityMask & HT_E_CONTENT_ENCODING && entity->content_encoding) {
	BOOL first = YES;
	HTList * cur = entity->content_encoding;
	HTEncoding pres;
	while ((pres = (HTEncoding) HTList_nextObject(cur)) &&
	       !HTFormat_isUnityContent(pres)) {
	    if (first) {
		PUTS("Content-Encoding: ");
		first = NO;
	    } else
		PUTC(',');
	    PUTS(HTAtom_name(pres));
	}
	if (!first) PUTBLOCK(crlf, 2);
    }
    if (EntityMask & HT_E_CTE && entity->cte) {
	HTEncoding cte = HTAnchor_contentTransferEncoding(entity);
	if (!HTFormat_isUnityTransfer(cte)) {
	    sprintf(linebuf, "Content-Transfer-Encoding: %s%c%c",
		    HTAtom_name(cte), CR, LF);
	    PUTBLOCK(linebuf, (int) strlen(linebuf));
	}
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

    /* Only send out Content-Length if we don't have a transfer coding */
    if (!HTRequest_transfer(request)) {
	if (EntityMask & HT_E_CONTENT_LENGTH) {
	    if (entity->content_length >= 0) {
		sprintf(linebuf, "Content-Length: %ld%c%c",
			entity->content_length, CR, LF);
		PUTBLOCK(linebuf, (int) strlen(linebuf));	
	    } else {
		transfer_coding = YES;
    	        sprintf(linebuf, "Transfer-Encoding: %s%c%c",
                        HTAtom_name(WWW_CODING_CHUNKED), CR, LF);
	        PUTBLOCK(linebuf, (int) strlen(linebuf));
	    }
	}
    }
    if (EntityMask & HT_E_CONTENT_TYPE && entity->content_type) {
	HTFormat format = entity->content_type != WWW_UNKNOWN ?
	    entity->content_type : WWW_BINARY;
	HTAssocList * parameters = HTAnchor_formatParam(entity);

	/* Output the content type */
	PUTS("Content-Type: ");
	PUTS(HTAtom_name(format));

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
    if (EntityMask & HT_E_LINK) {
	HTLink * link = HTAnchor_mainLink((HTAnchor *) entity);
	HTList * sublinks = HTAnchor_subLinks((HTAnchor *) entity);
	HTLinkType linktype = NULL;

	/* First look in the main link */
	if (link && (linktype = HTLink_type(link))) {		    
	    char * src = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
	    HTParentAnchor * dest = HTAnchor_parent(HTLink_destination(link));
	    char * dst = HTAnchor_address((HTAnchor *) dest);
	    char * rel_dst = HTRelative(dst, src);
	    if (rel_dst) {
		PUTS("Link: \"");
		PUTS(rel_dst);
		PUTS("\"");
		sprintf(linebuf, ";rel=\"%s\"", HTAtom_name(linktype));
		PUTBLOCK(linebuf, (int) strlen(linebuf));
		HT_FREE(rel_dst);
		HT_FREE(dst);
	    }

	    /* ... and then in any sublinks */
	    if (sublinks) {
		HTLink * pres;
		while ((pres = (HTLink *) HTList_nextObject(sublinks))) {
		    if ((linktype = HTLink_type(pres))) {
			dest = HTAnchor_parent(HTLink_destination(pres));
			dst = HTAnchor_address((HTAnchor *) dest);
			rel_dst = HTRelative(dst, src);
			if (rel_dst) {
			    PUTS(", \"");
			    PUTS(rel_dst);
			    PUTS("\"");
			    sprintf(linebuf, ";rel=\"%s\"", HTAtom_name(linktype));
			    PUTBLOCK(linebuf, (int) strlen(linebuf));
			    HT_FREE(rel_dst);
			    HT_FREE(dst);
			}
		    }
		}
	    }
	    PUTBLOCK(crlf, 2);
	    HT_FREE(src);
	}
    }
    if (EntityMask & HT_E_TITLE && entity->title) {
	sprintf(linebuf, "Title: %s%c%c", entity->title, CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (EntityMask & HT_E_URI) {		/* @@@@@@@@@@ */

    }
    if (EntityMask & HT_E_VERSION && entity->version) {
	sprintf(linebuf, "Content-Version: %s%c%c", entity->version, CR, LF);
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }
    if (me->endHeader) {
	sprintf(linebuf, "%c%c", CR, LF);	   /* Blank line means "end" */
	PUTBLOCK(linebuf, (int) strlen(linebuf));
    }

    /*
    **  Handle any Transfer encoding. This is really a transport issue but
    **  as it often pops up when we are sending an entity then we put it
    **  here for now. A better place whould be in the HTTPGen stream.
    **  the real problem is that the server doesn't have any mechanism of
    **  telling the client what transports it can handle. The best we can
    **  hope for is that the server understands "chunked" although we are
    **  certainly capable of handling nested encodings :(
    */
    if (transfer_coding) {
	HTStream * target = HTTransferCodingStack(WWW_CODING_CHUNKED,
						  me->target, request, NULL, YES);
	HTTRACE(STREAM_TRACE, "Building.... Transfer-Encoding stack\n");
	if (target == HTBlackHole()) {
	    if (me->target) (*me->target->isa->abort)(me->target, NULL);
	    me->target = HTErrorStream();
	} else
	    me->target = target;
    }

#if 0
    /*
    **  We expect the anchor object already to have the right encoding and
    **  we therefore should not set up extra streams for doing this.
    */

    /* Handle any Content Transfer encoding */
    {
	HTEncoding cte = HTAnchor_contentTransferEncoding(entity);
	if (!HTFormat_isUnityTransfer(cte)) {
	    HTTRACE(STREAM_TRACE, "Building.... C-T-E stack\n");
	    me->target = HTContentTransferCodingStack(cte, me->target,
						      request, NULL, YES);
	}
    }

    /* Handle any Content Encodings */
    {
	HTList * cc = HTAnchor_encoding(entity);
	if (cc) {
	    HTTRACE(STREAM_TRACE, "Building.... C-E stack\n");
	    me->target = HTContentEncodingStack(cc, me->target, request, NULL);
	}
    }
#endif

    HTTRACE(PROT_TRACE, "MIME........ Generating Entity Headers\n");
    return HT_OK;
}

/*
**	Flushes header but doesn't free stream object
*/
PRIVATE int MIMERequest_flush (HTStream * me)
{
    int status = MIMERequest_put_block(me, NULL, 0);
    return status==HT_OK ? (*me->target->isa->flush)(me->target) : status;
}

PRIVATE int MIMERequest_put_block (HTStream * me, const char * b, int l)
{
    HTNet * net = HTRequest_net(me->request);
    if (!me->transparent) {
	MIMEMakeRequest(me, me->request);
	me->transparent = YES;	

	/*
	**  First we only send the header
	*/
	if (HTMethod_hasEntity(HTRequest_method(me->request))) {
	    HTHost * host = HTNet_host(net);
	    char * class = HTHost_class(host);
	    if (class && !strcmp(class, "http")) {
		MIMERequest_flush(me);
		HTNet_setHeaderBytesWritten(net, HTNet_bytesWritten(net));
		return HT_PAUSE;
	    }
	}
    }
    
    /* Check if we have written it all */
    if (b) {
	HTParentAnchor * entity = HTRequest_entityAnchor(me->request);
	long cl = HTAnchor_length(entity);
	return (cl>=0 && HTNet_bytesWritten(net)-HTNet_headerBytesWritten(net) >= cl) ?
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
    HTTRACE(PROT_TRACE, "MIMERequest. ABORTING...\n");
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
