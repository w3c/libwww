/*								       HTMIME.c
**	MIME MESSAGE PARSE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
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
**	14 Mar 95  HFN	Now using anchor for storing data. No more `\n',
**			static buffers etc.
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTFormat.h"
#include "HTCache.h"
#include "HTAlert.h"
#include "HTAncMan.h"
#include "HTChunk.h"
#include "HTMethod.h"
#include "HTHeader.h"
#include "HTSocket.h"
#include "HTWWWStr.h"
#include "HTFWrite.h"
#include "HTNetMan.h"
#include "HTReqMan.h"
#include "HTMIME.h"					 /* Implemented here */

/*		MIME Object
**		-----------
*/
typedef enum _MIME_state {
    BEGINNING_OF_LINE=0,
    CHECK,				/* check against check_pointer */
    UNKNOWN,				/* Unknown header */
    JUNK_LINE,				/* Ignore rest of header */

    CON,				/* Intermediate states */
    CONTENT,
    FIRSTLETTER_A,
    FIRSTLETTER_D,
    FIRSTLETTER_L,
    CONTENTLETTER_L,
    CONTENTLETTER_T,

    ACCEPT_TYPE,			/* Headers supported */
    ACCEPT_CHARSET,
    ACCEPT_ENCODING,
    ACCEPT_LANGUAGE,
    ALLOW,
    AUTHENTICATE,
    CONNECTION,
    CONTENT_ENCODING,
    CONTENT_LANGUAGE,
    CONTENT_LENGTH,
    CONTENT_TRANSFER_ENCODING,
    CONTENT_TYPE,
    MIME_DATE,
    DERIVED_FROM,
    EXPIRES,
    LAST_MODIFIED,
    LINK,
    LOCATION,
    PUBLIC_METHODS,
    RETRY_AFTER,
    TITLE,
    URI_HEADER,
    VERSION
} MIME_state;

struct _HTStream {
    CONST HTStreamClass *	isa;
    HTRequest *			request;
    HTNet *			net;
    HTParentAnchor *		anchor;
    HTStream *			target;
    HTFormat			target_format;
    HTChunk *			buffer;
    HTSocketEOL			EOLstate;
    BOOL			transparent;
    BOOL			head_only;
    BOOL			nntp;
};

/* ------------------------------------------------------------------------- */

/*
**	This is a FSM parser which is tolerant as it can be of all
**	syntax errors.  It ignores field names it does not understand,
**	and resynchronises on line beginnings.
*/
PRIVATE int parseheader (HTStream * me, HTRequest * request,
			 HTParentAnchor * anchor)
{
    MIME_state state = BEGINNING_OF_LINE;
    MIME_state ok_state;			  /* got this state if match */
    char *ptr = me->buffer->data-1;     /* We dont change the data in length */
    char *stop = ptr+me->buffer->size;			     /* When to stop */
    char *header = ptr;      				  /* For diagnostics */
    CONST char * check_pointer;				   /* checking input */
    char *value;

    /* In case we get an empty header consisting of a CRLF, we fall thru */
    while (ptr < stop) {
	switch (state) {
	  case BEGINNING_OF_LINE:
	    header = ++ptr;
	    switch (TOLOWER(*ptr)) {
	      case '\0':
		state = BEGINNING_OF_LINE;		       /* Empty line */
		continue;

	      case 'a':
		state = FIRSTLETTER_A;
		break;

	      case 'c':
		check_pointer = "on";
		ok_state = CON;
		state = CHECK;
		break;

	      case 'd':
		state = FIRSTLETTER_D;
		break;

	      case 'e':
		check_pointer = "xpires";
		ok_state = EXPIRES;
		state = CHECK;
		break;

	      case 'k':
		check_pointer = "eep-alive";
		ok_state = JUNK_LINE;  /* We don't use this but recognize it */
		state = CHECK;
		break;

	      case 'l':
		state = FIRSTLETTER_L;
		break;

	      case 'm':
		check_pointer = "ime-version";
		ok_state = JUNK_LINE;  /* We don't use this but recognize it */
		state = CHECK;
		break;

	      case 'n':
		check_pointer = "ewsgroups";
		me->nntp = YES;			 /* Due to news brain damage */
		ok_state = JUNK_LINE;  /* We don't use this but recognize it */
		state = CHECK;
		break;

	      case 'r':
		check_pointer = "etry-after";
		ok_state = RETRY_AFTER;
		state = CHECK;
		break;

	      case 's':
		check_pointer = "erver";
		ok_state = JUNK_LINE;  /* We don't use this but recognize it */
		state = CHECK;
		break;

	      case 't':
		check_pointer = "itle";
		ok_state = TITLE;
		state = CHECK;
		break;

	      case 'u':
		check_pointer = "ri";
		ok_state = URI_HEADER;
		state = CHECK;
		break;

	      case 'v':
		check_pointer = "ersion";
		ok_state = VERSION;
		state = CHECK;
		break;

	      case 'w':
		check_pointer = "ww-authenticate";
		ok_state = AUTHENTICATE;
		state = CHECK;
		break;

	      default:
		state = UNKNOWN;
		break;
	    }
	    ptr++;
	    break;
	
	  case FIRSTLETTER_A:
	    if (!strncasecomp(ptr, "llow", 4)) {
		state = ALLOW;
		ptr += 4;
	    } else if (!strncasecomp(ptr, "ccept-language", 14)) {
		state = ACCEPT_LANGUAGE;
		ptr += 14;
	    } else if (!strncasecomp(ptr, "ccept-charset", 13)) {
		state = ACCEPT_CHARSET;
		ptr += 13;
	    } else if (!strncasecomp(ptr, "ccept", 5)) {
		state = ACCEPT_TYPE;
		ptr += 5;
	    } else
		state = UNKNOWN;
	    ptr++;
	    break;

	  case FIRSTLETTER_D:
	    switch (TOLOWER(*ptr)) {
	      case 'a':
		check_pointer = "te";
		ok_state = MIME_DATE;
		state = CHECK;
		break;

	      case 'e':
		check_pointer = "rived-from";
		ok_state = DERIVED_FROM;
		state = CHECK;
		break;

	      default:
		state = UNKNOWN;
		break;
	    }
	    ptr++;
	    break;

	  case FIRSTLETTER_L:
	    switch (TOLOWER(*ptr)) {
	      case 'a':
		check_pointer = "st-modified";
		ok_state = LAST_MODIFIED;
		state = CHECK;
		break;

	      case 'i':
		check_pointer = "nk";
		ok_state = LINK;
		state = CHECK;
		break;

	      case 'o':
		check_pointer = "cation";
		ok_state = LOCATION;
		state = CHECK;
		break;

	      default:
		state = UNKNOWN;
		break;
	    }
	    ptr++;
	    break;

	  case CON:
	    switch (TOLOWER(*ptr)) {
	      case 'n':
		check_pointer = "ection";
		ok_state = CONNECTION;
		state = CHECK;
		break;

	      case 't':
		check_pointer = "ent-";
		ok_state = CONTENT;
		state = CHECK;
		break;

	      default:
		state = UNKNOWN;
		break;
	    }
	    ptr++;
	    break;

	  case CONTENT:
	    switch (TOLOWER(*ptr)) {
	      case 'e':
		check_pointer = "ncoding";
		ok_state = CONTENT_ENCODING;
		state = CHECK;
		break;

	      case 'l':
		state = CONTENTLETTER_L;
		break;

	      case 't':
		state = CONTENTLETTER_T;
		break;

	      default:
		state = UNKNOWN;
		break;
	    }
	    ptr++;
	    break;

	  case CONTENTLETTER_L:
	    switch (TOLOWER(*ptr)) {
	      case 'a':
		check_pointer = "nguage";
		ok_state = CONTENT_LANGUAGE;
		state = CHECK;
		break;

	      case 'e':
		check_pointer = "ngth";
		ok_state = CONTENT_LENGTH;
		state = CHECK;
		break;

	      default:
		state = UNKNOWN;
		break;
	    }
	    ptr++;
	    break;

	  case CONTENTLETTER_T:
	    switch (TOLOWER(*ptr)) {
	      case 'r':
		check_pointer = "ansfer-encoding";
		ok_state = CONTENT_TRANSFER_ENCODING;
		state = CHECK;
		break;

	      case 'y':
		check_pointer = "pe";
		ok_state = CONTENT_TYPE;
		state = CHECK;
		break;

	      default:
		state = UNKNOWN;
		break;
	    }
	    ptr++;
	    break;

	  case CHECK:				     /* Check against string */
	    while (TOLOWER(*ptr) == *check_pointer) ptr++, check_pointer++;
	    if (!*check_pointer) {
		state = ok_state;
		while (*ptr && (WHITE(*ptr) || *ptr==':')) /* Spool to value */
		    ptr++;
	    } else
		state = UNKNOWN;
	    break;

	  case ACCEPT_TYPE:			/* @@@ */
	    state = JUNK_LINE;
	    break;

	  case ACCEPT_CHARSET:			/* @@@ */
	    state = JUNK_LINE;
	    break;

	  case ACCEPT_ENCODING:			/* @@@ */
	    state = JUNK_LINE;
	    break;

	  case ACCEPT_LANGUAGE:			/* @@@ */
	    state = JUNK_LINE;
	    break;

	  case ALLOW:
	    while ((value = HTNextField(&ptr)) != NULL) {
		HTMethod new_method;
		/* We treat them as case-insensitive! */
		if ((new_method = HTMethod_enum(value)) != METHOD_INVALID)
		    anchor->methods += new_method;
	    }
	    if (STREAM_TRACE)
		TTYPrint(TDEST, "MIMEParser.. Methods allowed: %d\n",
			anchor->methods);
	    state = JUNK_LINE;
	    break;

	  case AUTHENTICATE:
	    if ((value = HTNextField(&ptr)) != NULL) {
		StrAllocCopy(request->WWWAAScheme, value);

		/* The parsing is done in HTSSUtils.c for the moment */
		if (*ptr) StrAllocCopy(request->WWWAARealm, ptr);
	    }
	    state = JUNK_LINE;
	    break;

	  case CONNECTION:
 	    if ((value = HTNextField(&ptr)) != NULL) {
		if (!strcasecomp(value, "keep-alive")) {
		    if (STREAM_TRACE)
			TTYPrint(TDEST,"MIMEParser.. Persistent Connection\n");
		    me->net->persistent = YES;
		    HTDNS_setSocket(me->net->dns, me->net->sockfd);
		}
	    }
	    state = JUNK_LINE;
	    break;

	  case CONTENT_ENCODING:
	    if ((value = HTNextField(&ptr)) != NULL) {
		char *lc = value;
		while ((*lc = TOLOWER(*lc))) lc++;
		anchor->content_encoding = HTAtom_for(value);
	    }
	    state = JUNK_LINE;
	    break;

	  case CONTENT_LANGUAGE:		 /* @@@ SHOULD BE A LIST @@@ */
	    if ((value = HTNextField(&ptr)) != NULL) {
		char *lc = value;
		while ((*lc = TOLOWER(*lc))) lc++;
		anchor->content_language = HTAtom_for(value);
	    }
	    state = JUNK_LINE;
	    break;

	  case CONTENT_LENGTH:
	    if ((value = HTNextField(&ptr)) != NULL)
		anchor->content_length = atol(value);
	    state = JUNK_LINE;
	    break;

	  case CONTENT_TRANSFER_ENCODING:
	    if ((value = HTNextField(&ptr)) != NULL) {
		char *lc = value;
		while ((*lc = TOLOWER(*lc))) lc++;
		anchor->cte = HTAtom_for(value);
	    }
	    state = JUNK_LINE;
	    break;

	  case CONTENT_TYPE:
	    if ((value = HTNextField(&ptr)) != NULL) {
		char *lc = value;
		while ((*lc = TOLOWER(*lc))) lc++; 
		anchor->content_type = HTAtom_for(value);
		while ((value = HTNextField(&ptr)) != NULL) {
		    if (!strcasecomp(value, "charset")) {
			if ((value = HTNextField(&ptr)) != NULL) {
			    lc = value;
			    while ((*lc = TOLOWER(*lc))) lc++;
			    anchor->charset = HTAtom_for(value);
			}
		    } else if (!strcasecomp(value, "level")) {
			if ((value = HTNextField(&ptr)) != NULL) {
			    lc = value;
			    while ((*lc = TOLOWER(*lc))) lc++;
			    anchor->level = HTAtom_for(value);
			}
		    } else if (!strcasecomp(value, "boundary")) {
			if ((value = HTNextField(&ptr)) != NULL) {
			    StrAllocCopy(request->boundary, value);
			}
		    }
		}
	    }
	    state = JUNK_LINE;
	    break;

	  case MIME_DATE:
	    anchor->date = HTParseTime(ptr);
	    state = JUNK_LINE;
	    break;

	  case DERIVED_FROM:
	    if ((value = HTNextField(&ptr)) != NULL)
		StrAllocCopy(anchor->derived_from, value);
	    state = JUNK_LINE;
	    break;

	  case EXPIRES:
	    anchor->expires = HTParseTime(ptr);
	    state = JUNK_LINE;
	    break;

	  case LAST_MODIFIED:
	    anchor->last_modified = HTParseTime(ptr);
	    state = JUNK_LINE;
	    break;

	  case LINK:
	    state = UNKNOWN;				/* @@@@@@@@@@@ */
	    break;

	  case LOCATION:
	    request->redirectionAnchor = HTAnchor_findAddress(HTStrip(ptr));
	    state = JUNK_LINE;
	    break;

	  case PUBLIC_METHODS:
	    state = UNKNOWN;				/* @@@@@@@@@@@ */
	    break;

	  case RETRY_AFTER:
	    request->retry_after = HTParseTime(ptr);
	    state = JUNK_LINE;
	    break;

	  case TITLE:	  /* Can't reuse buffer as HTML version might differ */
	    if ((value = HTNextField(&ptr)) != NULL)
		StrAllocCopy(anchor->title, value);
	    state = JUNK_LINE;
	    break;

	  case URI_HEADER:
	    state = LOCATION;			/* @@@ Need extended parsing */
	    break;

	  case VERSION:
	    if ((value = HTNextField(&ptr)) != NULL)
		StrAllocCopy(anchor->version, value);
	    state = JUNK_LINE;
	    break;

	  case UNKNOWN:
	    {
		HTList * list;
		HTParserCallback *cbf;
		int status;
		BOOL override;
		if (STREAM_TRACE)
		    TTYPrint(TDEST,"MIMEParser.. Unknown `%s\'\n", header);
		if ((list = HTRequest_parser(request, &override)) &&
		    (cbf = HTParser_find(list, header)) &&
		    (status = (*cbf)(request, header) != HT_OK)) {
		    return status;
		} else if (!override &&
			   (list = HTHeader_parser()) &&
			   (cbf = HTParser_find(list, header)) &&
			   (status = (*cbf)(request, header) != HT_OK)) {
		    return status;
		}
	    }

	  case JUNK_LINE:
	    while (*ptr) ptr++;
	    state = BEGINNING_OF_LINE;
	    break;
	}
    }
    me->transparent = YES;		  /* Pump rest of data right through */
#if 0
    HTChunk_clear(me->buffer);			/* Get ready for next header */
#endif

    /* If this request us a source in PostWeb then pause here */
    if (me->head_only || HTRequest_isSource(request)) return HT_PAUSE;

    /* If HEAD method then we just stop here */
    if (request->method == METHOD_HEAD) return HT_LOADED;

    /* News server almost never send content type or content length */
    if (anchor->content_type != WWW_UNKNOWN || me->nntp) {
	if (STREAM_TRACE) TTYPrint(TDEST, "MIMEParser.. Convert %s to %s\n",
				   HTAtom_name(anchor->content_type),
				   HTAtom_name(me->target_format));
	me->target = HTStreamStack(anchor->content_type, me->target_format,
				   me->target, request, YES);
    }
    if (!me->target) me->target = HTErrorStream();
    return HT_OK;
}


/*
**	Header is terminated by CRCR, LFLF, CRLFLF, CRLFCRLF
**	Folding is either of CF LWS, LF LWS, CRLF LWS
*/
PRIVATE int HTMIME_put_block (HTStream * me, CONST char * b, int l)
{
    CONST char * start = b;
    CONST char * end = start;
    while (!me->transparent && l-- > 0) {
	if (me->EOLstate == EOL_FCR) {
	    if (*b == CR) {				    /* End of header */
		int status;
		HTChunk_putb(me->buffer, start, end-start);
		status = parseheader(me, me->request, me->anchor);
		HTNet_setBytesRead(me->net, l);
		if (status != HT_OK)
		    return status;
	    } else if (*b == LF)			   	     /* CRLF */
		me->EOLstate = EOL_FLF;
	    else if (WHITE(*b)) {			   /* Folding: CR SP */
		me->EOLstate = EOL_BEGIN;
		HTChunk_putb(me->buffer, start, end-start);
		HTChunk_putc(me->buffer, ' ');
		start=b, end=b+1;
	    } else {						 /* New line */
		me->EOLstate = EOL_BEGIN;
		HTChunk_putb(me->buffer, start, end-start);
		HTChunk_putc(me->buffer, '\0');
		start=b, end=b+1;
	    }
	} else if (me->EOLstate == EOL_FLF) {
	    if (*b == CR)				/* LF CR or CR LF CR */
		me->EOLstate = EOL_SCR;
	    else if (*b == LF) {			    /* End of header */
		int status;
		HTChunk_putb(me->buffer, start, end-start);
		status = parseheader(me, me->request, me->anchor);
		HTNet_setBytesRead(me->net, l);
		if (status != HT_OK)
		    return status;
	    } else if (WHITE(*b)) {	       /* Folding: LF SP or CR LF SP */
		me->EOLstate = EOL_BEGIN;
		HTChunk_putb(me->buffer, start, end-start);
		HTChunk_putc(me->buffer, ' ');
		start=b, end=b+1;
	    } else {						/* New line */
		me->EOLstate = EOL_BEGIN;
		HTChunk_putb(me->buffer, start, end-start);
		HTChunk_putc(me->buffer, '\0');
		start=b, end=b+1;
	    }
	} else if (me->EOLstate == EOL_SCR) {
	    if (*b==CR || *b==LF) {			    /* End of header */
		int status;
		HTChunk_putb(me->buffer, start, end-start);
		status = parseheader(me, me->request, me->anchor);
		HTNet_setBytesRead(me->net, l);
		if (status != HT_OK)
		    return status;
	    } else if (WHITE(*b)) {	 /* Folding: LF CR SP or CR LF CR SP */
		me->EOLstate = EOL_BEGIN;
		HTChunk_putb(me->buffer, start, end-start);
		HTChunk_putc(me->buffer, ' ');
		start=b, end=b+1;
	    } else {						/* New line */
		me->EOLstate = EOL_BEGIN;
		HTChunk_putb(me->buffer, start, end-start);
		HTChunk_putc(me->buffer, '\0');
		start=b, end=b+1;
	    }
	} else if (*b == CR) {
	    me->EOLstate = EOL_FCR;
	} else if (*b == LF) {
	    me->EOLstate = EOL_FLF;			       /* Line found */
	} else
	    end++;
	b++;
    }

    /* 
    ** Put the rest down the stream without touching the data but make sure
    ** that we get the correct content length of data
    */
    if (me->transparent) {
	int status = (*me->target->isa->put_block)(me->target, b, l);
	if (status == HT_OK) {
	    /* Check if CL at all - thanks to jwei@hal.com (John Wei) */
	    long cl = HTAnchor_length(me->anchor);
	    return (cl>=0 && HTNet_bytesRead(me->net)>=cl) ? HT_LOADED : HT_OK;
	} else
	    return status;
    }
    return HT_OK;
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
PRIVATE int HTMIME_put_string (HTStream * me, CONST char * s)
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
    if (!me->transparent) parseheader(me, me->request, me->anchor);
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    if (PROT_TRACE)
	TTYPrint(TDEST, "MIME........ FREEING....\n");
    HTChunk_delete(me->buffer);
    HT_FREE(me);
    return status;
}

/*	End writing
*/
PRIVATE int HTMIME_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;
    if (me->target) status = (*me->target->isa->abort)(me->target, e);
    if (PROT_TRACE)
	TTYPrint(TDEST, "MIME........ ABORTING...\n");
    HTChunk_delete(me->buffer);
    HT_FREE(me);
    return status;
}



/*	Structured Object Class
**	-----------------------
*/
PRIVATE CONST HTStreamClass HTMIME =
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
    HTStream* me;
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(* me))) == NULL)
        HT_OUTOFMEM("HTMIMEConvert");
    me->isa = &HTMIME;       
    me->request = request;
    me->anchor = request->anchor;
    me->net = request->net;
    me->target = output_stream;
    me->target_format = output_format;
    me->buffer = HTChunk_new(512);
    me->EOLstate = EOL_BEGIN;
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
    HTStream * me;
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTMIMEConvert");
    me->isa = &HTMIME;       
    me->request = request;
    me->anchor = request->anchor;
    me->net = request->net;
    me->target = output_stream;
    me->target_format = output_format;
    me->buffer = HTChunk_new(512);
    me->EOLstate = EOL_BEGIN;
    me->head_only = YES;		    /* We want to pause after header */
    return me;
}
