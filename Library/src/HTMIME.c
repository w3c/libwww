/*								       HTMIME.c
**	MIME MESSAGE PARSE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This is RFC 1341-specific code.
**	The input stream pushed into this parser is assumed to be
**	stripped on CRs, ie lines end with LF, not CR LF.
**	(It is easy to change this except for the body part where
**	conversion can be slow.)
**
** History:
**	   Feb 92	Written Tim Berners-Lee, CERN
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**	14 Mar 95  HFN	Now using response for storing data. No more `\n',
**			static buffers etc.
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTReqMan.h"
#include "HTNetMan.h"
#include "HTHeader.h"
#include "HTWWWStr.h"

#ifndef NO_CACHE
#include "HTTee.h"
#include "HTConLen.h"
#include "HTMerge.h"
#include "WWWCache.h"
#endif

#include "HTMIME.h"					 /* Implemented here */

typedef enum _HTMIMEMode {
    HT_MIME_HEADER	= 0x1,
    HT_MIME_FOOTER	= 0x2,
    HT_MIME_PARTIAL	= 0x4,
    HT_MIME_CONT	= 0x8,
    HT_MIME_UPGRADE	= 0x10
} HTMIMEMode;

struct _HTStream {
    const HTStreamClass *	isa;
    HTRequest *			request;
    HTResponse *		response;
    HTNet *			net;
    HTStream *			target;
    HTConverter *		save_stream;
    HTFormat			target_format;
    HTChunk *			token;
    HTChunk *			value;
    int				hash;
    HTEOLState			EOLstate;
    HTMIMEMode			mode;
    BOOL			transparent;
    BOOL			haveToken;
    BOOL			hasBody;
};

PRIVATE HTConverter * LocalSaveStream = NULL; /* Where to save unknown stuff */

/* ------------------------------------------------------------------------- */

PRIVATE int pumpData (HTStream * me)
{
    HTRequest * request = me->request;
    HTResponse * response = me->response;
    HTFormat format = HTResponse_format(response);
    HTList * te = HTResponse_transfer(response);
    HTList * ce = HTResponse_encoding(response);
    long length = HTResponse_length(response);
    HTStream * BlackHole = HTBlackHole();
    BOOL savestream = NO;
    me->transparent = YES;		  /* Pump rest of data right through */

    /*
    **  Cache the metainformation in the anchor object by copying
    **  it from the response object. This we do regardless if
    **  we have a persistent cache or not as the memory cache will
    **  use it as well. If we are updating a cache entry using
    **  byte ranges then we already have the metainformation and
    **  hence we can ignore the new one as it'd better be the same.
    */
    if (!(me->mode & HT_MIME_PARTIAL) &&
	HTResponse_isCachable(me->response) != HT_NO_CACHE)
	HTAnchor_update(HTRequest_anchor(request), me->response);

    /*
    **  If we asked only to read the header or footer or we used a HEAD
    **  method then we stop here as we don't expect any body part.
    */
    if (me->mode & (HT_MIME_HEADER | HT_MIME_FOOTER) ||
	HTRequest_method(request) == METHOD_HEAD) {
        HTAlertCallback * cbf = HTAlert_find(HT_PROG_DONE);
        if (cbf) (*cbf)(request, HT_PROG_DONE, HT_MSG_NULL, NULL, NULL, NULL);
        return HT_LOADED;
    }

    /*
    **  If we are paring a 1xx response then return HT_CONTINUE
    */
    if (me->mode & HT_MIME_CONT)
	return HT_CONTINUE;

    /*
    **  If we get a 101 Protocol Switch then we are done here
    **  but not done with the response (which we don't know
    **  how to go about parsing
    */
    if (me->mode & HT_MIME_UPGRADE) {
	me->hasBody = YES;
	return HT_OK;
    }

    /*
    **  If there is no content-length, no transfer encoding and no
    **  content type then we assume that there is no body part in
    **  the message and we can return HT_LOADED
    */
    {
	HTHost * host = HTNet_host(me->net);
	if (length<0 && te==NULL &&
	    HTHost_isPersistent(host) && !HTHost_closeNotification(host)) {
	    if (format != WWW_UNKNOWN) {
		HTTRACE(STREAM_TRACE, "MIME Parser. BAD - there seems to be a body but no length. This must be an HTTP/1.0 server pretending that it is HTTP/1.1\n");
		HTHost_setCloseNotification(host, YES);
	    } else {
                HTAlertCallback * cbf = HTAlert_find(HT_PROG_DONE);
                if (cbf) (*cbf)(request, HT_PROG_DONE, HT_MSG_NULL, NULL, NULL, NULL);
		HTTRACE(STREAM_TRACE, "MIME Parser. No body in this message\n");
		return HT_LOADED;
	    }
	}
    }

    /*
    **  Deal with the body
    */
    me->hasBody = YES;

    /*
    **  Handle any Content Type
    */
    if (!(me->mode & HT_MIME_PARTIAL) &&
	(format != WWW_UNKNOWN || length > 0 || te)) {
	HTStream * target;
	HTTRACE(STREAM_TRACE, "Building.... C-T stack from %s to %s\n" _ 
				  HTAtom_name(format) _ 
				  HTAtom_name(me->target_format));
	if ((target = HTStreamStack(format, me->target_format,
				    me->target, request, YES))==BlackHole) {
	    if (!savestream) {
                if (me->target) (*me->target->isa->abort)(me->target, NULL);
                me->target = me->save_stream(request, NULL,
					     format, me->target_format, me->target);
		savestream = YES;
	    }
	} else
	    me->target = target;
    }

    /*
    **  Handle any Content Encodings
    */
    HTTRACE(STREAM_TRACE, "Building.... Content-Decoding stack\n");
    if (ce) {
	HTStream * target = HTContentDecodingStack(ce, me->target, request, NULL);
	if (target == BlackHole) {
	    if (!savestream) {
		if (me->target) (*me->target->isa->abort)(me->target, NULL);
                me->target = me->save_stream(request, NULL,
					     format, me->target_format, me->target);
		savestream = YES;
	    }
	} else
	    me->target = target;
    }

    /*
    **  Can we cache the data object? If so then create a T stream and hook it 
    **  into the stream pipe. We do it before the transfer decoding so that we
    **  don't have to deal with that when we retrieve the object from cache.
    **  If we are appending to a cache entry then use a different stream than
    **  if creating a new entry.
    */
#ifndef NO_CACHE
    if (HTCacheMode_enabled()) {
	if (me->mode & HT_MIME_PARTIAL) {
	    HTStream * append = HTStreamStack(WWW_CACHE_APPEND,
					      me->target_format,
					      me->target, request, NO);
	    if (append) me->target = HTTee(me->target, append, NULL);
	} else if (HTResponse_isCachable(me->response) == HT_CACHE_ALL) {
	    HTStream * cache = HTStreamStack(WWW_CACHE, me->target_format,
					     me->target, request, NO);
	    if (cache) me->target = HTTee(me->target, cache, NULL);
	}
    }
#endif
    
    /*
    **  Handle any Transfer Encodings
    */
    HTTRACE(STREAM_TRACE, "Building.... Transfer-Decoding stack\n");
    if (te) {
	HTStream * target = HTTransferDecodingStack(te, me->target, request, NULL);
	if (target == BlackHole) {
	    if (!savestream) {
		if (me->target) (*me->target->isa->abort)(me->target, NULL);
                me->target = me->save_stream(request, NULL,
					     format, me->target_format, me->target);
		savestream = YES;
	    }
	} else
	    me->target = target;
    }


    /*
    ** If we for some reason couldn't find a target stream
    */
    if (!me->target) me->target = HTBlackHole();
    return HT_OK;
}

/* _dispatchParsers
 * call request's MIME header parser. Use global parser if no 
 * appropriate one is found for request.
*/
PRIVATE int _dispatchParsers (HTRequest * req, char * token, char * value)
{
    int status;
    BOOL found = NO;
    BOOL local = NO;
    HTMIMEParseSet * parseSet;

    /* In case we get an empty header consisting of a CRLF, we fall thru */
    HTTRACE(STREAM_TRACE, "MIME header. %s: %s\n" _ 
			      token ? token : "<null>" _ 
			      value ? value : "<null>");
    if (!token) return HT_OK;			    /* Ignore noop token */

    /*
    ** Search the local set of MIME parsers
    */
    if ((parseSet = HTRequest_MIMEParseSet(req, &local)) != NULL) {
        status = HTMIMEParseSet_dispatch(parseSet, req, 
					 token, value, &found);
	if (found) return status;
    }

    /*
    ** Search the global set of MIME parsers
    */
    if (local==NO && (parseSet = HTHeader_MIMEParseSet()) != NULL) {
	status = HTMIMEParseSet_dispatch(parseSet, req, 
					 token, value, &found);
	if (found) return status;
    }

    return HT_OK;
}

/* _stream2dispatchParsers - extracts the arguments from a
 * MIME stream before calling the generic _dispatchParser
 * function.
 */
PRIVATE int _stream2dispatchParsers (HTStream * me)
{
    char * token = HTChunk_data(me->token);
    char * value = HTChunk_data(me->value);

    /* In case we get an empty header consisting of a CRLF, we fall thru */
    HTTRACE(STREAM_TRACE, "MIME header. %s: %s\n" _ 
			      token ? token : "<null>" _ 
			      value ? value : "<null>");
    if (!token) return HT_OK;			    /* Ignore noop token */

    /*
    ** Remember the original header
    */
    HTResponse_addHeader(me->response, token, value);

    /* call the parsers to set the headers */
    return (_dispatchParsers (me->request, token, value));
}

/*
**	Header is terminated by CRCR, LFLF, CRLFLF, CRLFCRLF
**	Folding is either of CF LWS, LF LWS, CRLF LWS
*/
PRIVATE int HTMIME_put_block (HTStream * me, const char * b, int l)
{
    const char * start = b;
    const char * end = start;
    const char * value = HTChunk_size(me->value) > 0 ? b : NULL;
    int length = l;
    int status;

    while (!me->transparent) {
	if (me->EOLstate == EOL_FCR) {
	    if (*b == CR)				    /* End of header */
	        me->EOLstate = EOL_END;
	    else if (*b == LF)			   	     /* CRLF */
		me->EOLstate = EOL_FLF;
	    else if (isspace((int) *b))			   /* Folding: CR SP */
	        me->EOLstate = EOL_FOLD;
	    else						 /* New line */
	        me->EOLstate = EOL_LINE;
	} else if (me->EOLstate == EOL_FLF) {
	    if (*b == CR)				/* LF CR or CR LF CR */
		me->EOLstate = EOL_SCR;
	    else if (*b == LF)				    /* End of header */
	        me->EOLstate = EOL_END;
	    else if (isspace((int) *b))	       /* Folding: LF SP or CR LF SP */
		me->EOLstate = EOL_FOLD;
	    else						/* New line */
		me->EOLstate = EOL_LINE;
	} else if (me->EOLstate == EOL_SCR) {
	    if (*b==CR || *b==LF)			    /* End of header */
	        me->EOLstate = EOL_END;
	    else if (isspace((int) *b))	 /* Folding: LF CR SP or CR LF CR SP */
		me->EOLstate = EOL_FOLD;
	    else						/* New line */
		me->EOLstate = EOL_LINE;
	} else if (*b == CR)
	    me->EOLstate = EOL_FCR;
	else if (*b == LF)
	    me->EOLstate = EOL_FLF;			       /* Line found */
	else {
	    if (!me->haveToken) {
	        if (*b == ':' || isspace((int) *b)) {
		    HTChunk_putb(me->token, start, end-start);
		    HTChunk_putc(me->token, '\0');
		    me->haveToken = YES;
		} else {
		    unsigned char ch = *(unsigned char *) b;
		    ch = TOLOWER(ch);
		    me->hash = (me->hash * 3 + ch) % MIME_HASH_SIZE;
		}
	    } else if (value == NULL && *b != ':' && !isspace((int) *b))
	        value = b;
	    end++;
	}
	switch (me->EOLstate) {
	    case EOL_LINE:
	    case EOL_END:
	    {
		int ret = HT_ERROR;
		HTChunk_putb(me->value, value, end-value);
		HTChunk_putc(me->value, '\0');
		ret =  _stream2dispatchParsers(me);
		HTNet_addBytesRead(me->net, b-start);
		start=b, end=b;
		if (me->EOLstate == EOL_END) {		/* EOL_END */
		    if (ret == HT_OK) {
			b++, l--;
			ret = pumpData(me);
			HTNet_addBytesRead(me->net, 1);
			if (me->mode & (HT_MIME_FOOTER | HT_MIME_CONT)) {
			    HTHost_setConsumed(HTNet_host(me->net), length - l);
			    return ret;
                        } else {
                            HTNet_setHeaderBytesRead(me->net, HTNet_bytesRead(me->net));
                        }
		    }
	        } else {				/* EOL_LINE */
		    HTChunk_truncate(me->token,0);
		    HTChunk_truncate(me->value,0);
		    me->haveToken = NO;
		    me->hash = 0;
		    value = NULL;
		}
		me->EOLstate = EOL_BEGIN;
		if (ret != HT_OK && ret != HT_LOADED) return ret;
		break;
	    }
	    case EOL_FOLD:
		me->EOLstate = EOL_BEGIN;
	        if (!me->haveToken) {
		    HTChunk_putb(me->token, start, end-start);
		    HTChunk_putc(me->token, '\0');
		    me->haveToken = YES;
	        } else if (value) {
		    HTChunk_putb(me->value, value, end-value);
		    HTChunk_putc(me->value, ' ');
		}
		start=b, end=b;
		break;
	    default: 
	        b++, l--;
	        if (!l) {
		    BOOL stop = NO;
		    if (!me->haveToken) {
			/* If empty header then prepare to stop */
			if (end-start)
			    HTChunk_putb(me->token, start, end-start);
			else
			    stop = YES;
		    } else if (value)
		        HTChunk_putb(me->value, value, end-value);
		    HTHost_setConsumed(HTNet_host(me->net), length - l);
		    return stop ? pumpData(me) : HT_OK;
		}
	}
    }

    if (length != l) HTHost_setConsumed(HTNet_host(me->net), length - l);

    /* 
    ** Put the rest down the stream without touching the data but make sure
    ** that we get the correct content length of data. If we have a CL in
    ** the headers then this stream is responsible for the accountance.
    */
    if (me->hasBody) {
	HTNet * net = me->net;
	/* Check if CL at all - thanks to jwei@hal.com (John Wei) */
	long cl = HTResponse_length(me->response);
	if (cl >= 0) {
	    long bodyRead = HTNet_bytesRead(net) - HTNet_headerBytesRead(net);

	    /*
	    **  If we have more than we need then just take what belongs to us.
	    */
	    if (bodyRead + l >= cl) {
		int consume = cl - bodyRead;
		if ((status = (*me->target->isa->put_block)(me->target, b, consume)) < 0)
		    return status;	    
                else {
                    HTAlertCallback * cbf = HTAlert_find(HT_PROG_DONE);
                    HTNet_addBytesRead(net, consume);
                    HTHost_setConsumed(HTNet_host(net), consume);
                    if (cbf) (*cbf)(me->request, HT_PROG_DONE, HT_MSG_NULL, NULL, NULL, NULL);
                    return HT_LOADED;
                }
            } else {
		if ((status = (*me->target->isa->put_block)(me->target, b, l)) < 0)
		    return status;
		HTNet_addBytesRead(net, l);
		HTHost_setConsumed(HTNet_host(net), l);
		return status;
	    }
	}
	return (*me->target->isa->put_block)(me->target, b, l);
    } else {
        HTAlertCallback * cbf = HTAlert_find(HT_PROG_DONE);
        if (cbf) (*cbf)(me->request, HT_PROG_DONE, HT_MSG_NULL, NULL, NULL, NULL);
    }
    return HT_LOADED;
}


/*	Character handling
**	------------------
*/
PRIVATE int HTMIME_put_character (HTStream * me, char c)
{
    return HTMIME_put_block(me, &c, 1);
}


/*	String handling
**	---------------
*/
PRIVATE int HTMIME_put_string (HTStream * me, const char * s)
{
    return HTMIME_put_block(me, s, (int) strlen(s));
}


/*	Flush an stream object
**	---------------------
*/
PRIVATE int HTMIME_flush (HTStream * me)
{
    return me->target ? (*me->target->isa->flush)(me->target) : HT_OK;
}

/*	Free a stream object
**	--------------------
*/
PRIVATE int HTMIME_free (HTStream * me)
{
    int status = HT_OK;
    if (!me->transparent)
        if (_stream2dispatchParsers(me) == HT_OK)
	    pumpData(me);
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    HTTRACE(PROT_TRACE, "MIME........ FREEING....\n");
    HTChunk_delete(me->token);
    HTChunk_delete(me->value);
    HT_FREE(me);
    return status;
}

/*	End writing
*/
PRIVATE int HTMIME_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;
    if (me->target) status = (*me->target->isa->abort)(me->target, e);
    HTTRACE(PROT_TRACE, "MIME........ ABORTING...\n");
    HTChunk_delete(me->token);
    HTChunk_delete(me->value);
    HT_FREE(me);
    return status;
}



/*	Structured Object Class
**	-----------------------
*/
PRIVATE const HTStreamClass HTMIME =
{		
	"MIMEParser",
	HTMIME_flush,
	HTMIME_free,
	HTMIME_abort,
	HTMIME_put_character,
	HTMIME_put_string,
	HTMIME_put_block
}; 


/*	MIME header parser stream.
**	-------------------------
**	This stream parses a complete MIME header and if a content type header
**	is found then the stream stack is called. Any left over data is pumped
**	right through the stream
*/
PUBLIC HTStream* HTMIMEConvert (HTRequest *	request,
				void *		param,
				HTFormat	input_format,
				HTFormat	output_format,
				HTStream *	output_stream)
{
    HTStream * me;
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(* me))) == NULL)
        HT_OUTOFMEM("HTMIMEConvert");
    me->isa = &HTMIME;       
    me->request = request;
    me->response = HTRequest_response(request);
    me->net = HTRequest_net(request);
    me->target = output_stream;
    me->target_format = output_format;
    me->save_stream = LocalSaveStream ? LocalSaveStream : HTBlackHoleConverter;
    me->token = HTChunk_new(256);
    me->value = HTChunk_new(256);
    me->hash = 0;
    me->EOLstate = EOL_BEGIN;
    me->haveToken = NO;
    return me;
}

/*	MIME header ONLY parser stream
**	------------------------------
**	This stream parses a complete MIME header and then returnes HT_PAUSE.
**	It does not set up any streams and resting data stays in the buffer.
**	This can be used if you only want to parse the headers before you
**	decide what to do next. This is for example the case in a server app.
*/
PUBLIC HTStream * HTMIMEHeader (HTRequest *	request,
				void *		param,
				HTFormat	input_format,
				HTFormat	output_format,
				HTStream *	output_stream)
{
    HTStream * me = HTMIMEConvert(request, param, input_format,
				  output_format, output_stream);
    me->mode |= HT_MIME_HEADER;
    return me;
}

PUBLIC HTStream * HTMIMEContinue (HTRequest *	request,
				  void *	param,
				  HTFormat	input_format,
				  HTFormat	output_format,
				  HTStream *	output_stream)
{
    HTStream * me = HTMIMEConvert(request, param, input_format,
				  output_format, output_stream);
    me->mode |= HT_MIME_CONT;
    return me;
}

PUBLIC HTStream * HTMIMEUpgrade  (HTRequest *	request,
				  void *	param,
				  HTFormat	input_format,
				  HTFormat	output_format,
				  HTStream *	output_stream)
{
    HTStream * me = HTMIMEConvert(request, param, input_format,
				  output_format, output_stream);
    me->mode |= HT_MIME_UPGRADE;
    return me;
}

/*	MIME footer ONLY parser stream
**	------------------------------
**	Parse only a footer, for example after a chunked encoding.
*/
PUBLIC HTStream * HTMIMEFooter (HTRequest *	request,
				void *		param,
				HTFormat	input_format,
				HTFormat	output_format,
				HTStream *	output_stream)
{
    HTStream * me = HTMIMEConvert(request, param, input_format,
				  output_format, output_stream);
    me->mode |= HT_MIME_FOOTER;
    return me;
}

#ifndef NO_CACHE
/*
**	A small BEFORE filter that just finds a cache entry unconditionally
**	and loads the entry. All freshness and any other constraints are 
**	ignored.
*/
PRIVATE int HTCacheLoadFilter (HTRequest * request, void * param, int mode)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * default_name;
    HTCache * cache;

    default_name = HTRequest_defaultPutName (request);
    cache = HTCache_find(anchor, default_name);

    HTTRACE(STREAM_TRACE, "Cache Load.. loading partial cache entry\n");
    if (cache) {
	char * name = HTCache_name(cache);
	HTAnchor_setPhysical(anchor, name);
	HTCache_addHit(cache);
	HT_FREE(name);
    }
    return HT_OK;
}

/*
**	A small AFTER filter that flushes the PIPE buffer so that we can
**	get the rest of the data
*/
PRIVATE int HTCacheFlushFilter (HTRequest * request, HTResponse * response,
				void * param, int mode)
{
    HTStream * pipe = (HTStream *) param;    
    if (pipe) {
	HTTRACE(STREAM_TRACE, "Cache Flush. Flushing PIPE buffer\n");
	(*pipe->isa->flush)(pipe);
    }

    /*
    **  We also delete the request obejct and stop more filters from being called.
    **  As this is our own request, it's OK to do that
    */
    HTRequest_delete(request);
    return HT_ERROR;
}
#endif

/*	Partial Response MIME parser stream
**	-----------------------------------
**	In case we sent a Range conditional GET we may get back a partial
**	response. This response must be appended to the already existing
**	cache entry before presented to the user.
**	We do this by continuing to load the new object into a temporary 
**	buffer and at the same time start the cache load of the already
**	existing object. When we have loaded the cache we merge the two
**	buffers.
*/
PUBLIC HTStream * HTMIMEPartial (HTRequest *	request,
				 void *		param,
				 HTFormat	input_format,
				 HTFormat	output_format,
				 HTStream *	output_stream)
{
#ifndef NO_CACHE
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTFormat format = HTAnchor_format(anchor);
    HTStream * pipe = NULL;

    /*
    **  The merge stream is a place holder for where we can put data when it
    **  arrives. We have two feeds: one from the cache and one from the net.
    **  We call the stream stack already now to get the right output stream.
    **  We can do this as we already know the content type from when we got the
    **  first part of the object.
    */
    HTStream * merge = HTMerge(HTStreamStack(format,
					     output_format, output_stream,
					     request, YES), 2);

    /*
    **  Now we create the MIME parser stream in partial data mode. We also
    **  set the target to our merge stream.
    */
    HTStream * me = HTMIMEConvert(request, param, input_format,
				  output_format, output_stream);
    me->mode |= HT_MIME_PARTIAL;
    me->target = merge;

    /*
    **  Create the pipe buffer stream to buffer the data that we read
    **  from the network
    */
    if ((pipe = HTPipeBuffer(me->target, 0))) me->target = pipe;

    /*
    **  Now start the second load from the cache. First we read this data from
    **  the cache and then we flush the data that we have read from the net.
    */
    {
	HTRequest * cache_request = HTRequest_new();

	/*
	**  Set the output format to source and the output stream to the
	**  merge stream. As we have already set up the stream pipe, we just 
	**  load it as source.
	*/
	HTRequest_setOutputFormat(cache_request, WWW_SOURCE);
	HTRequest_setOutputStream(cache_request, merge);

	/*
	**  Bind the anchor to the new request and also register a local
	**  AFTER filter to flush the pipe buffer so that we can get
	**  rest of the data through. 
	*/
	HTRequest_setAnchor(cache_request, (HTAnchor *) anchor);
	HTRequest_addBefore(cache_request, HTCacheLoadFilter, NULL, NULL,
			    HT_FILTER_FIRST, YES);
	HTRequest_addAfter(cache_request, HTCacheFlushFilter, NULL, pipe,
			   HT_ALL, HT_FILTER_FIRST, YES);

	HTTRACE(STREAM_TRACE, "Partial..... Starting cache load\n");
	HTLoad(cache_request, NO);
    }
    return me;
#else
    return NULL;
#endif
}

PUBLIC void HTMIME_setSaveStream (HTConverter * save_stream)
{
    LocalSaveStream = save_stream;
}

PUBLIC HTConverter * HTMIME_saveStream (void)
{
    return LocalSaveStream;
}

#ifndef NO_CACHE
/* HTMIME_anchor2response
 * Copies the anchor HTTP headers into a response object by means
 * of the generic _dispatchParsers function. Written so that we can
 * copy the HTTP headers stored in the cache to the response object.
 */
PRIVATE void HTMIME_anchor2response (HTRequest * req)
{
  char * token;
  char * value;
  HTAssocList * header;
  HTAssoc * pres;
  HTResponse * res;
  HTParentAnchor * anchor;

  if (!req)
    return;
  
  anchor = HTRequest_anchor (req);
  header = HTAnchor_header (anchor);
  if (!anchor || !header)
    return;

  while ((pres = (HTAssoc *) HTAssocList_nextObject (header)))
    {
      token = HTAssoc_name (pres);
      value = HTAssoc_value (pres);
      _dispatchParsers (req, token, value);
    }
  
  /*
  **  Notify the response object not to delete the lists that we
  **  have inherited from the anchor object
  */
  res = HTRequest_response (req);
  HTResponse_isCached (res, YES);  
}

/*
**	A small AFTER filter that is a frontend to the 
**      HTMIME_anchor2headers function.
*/

PUBLIC HTStream * HTCacheCopyHeaders   (HTRequest *	request,
					void *		param,
					HTFormat	input_format,
					HTFormat	output_format,
					HTStream *	output_stream)
{
    HTTRACE(STREAM_TRACE, "Cache Copy Headers.. Copying headers into the response object\n");
    HTMIME_anchor2response (request);
    return HT_OK;
}
#endif /* NO_CACHE */
