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
#include "HTChunk.h"
#include "HTMethod.h"
#include "HTSocket.h"
#include "HTFWrite.h"
#include "HTMIME.h"					 /* Implemented here */

/*		MIME Object
**		-----------
*/
typedef enum _MIME_state {
    BEGINNING_OF_LINE=0,
    CHECK,				/* check against check_pointer */
    UNKNOWN,				/* Unknown header */
    JUNK_LINE,				/* Ignore rest of header */

    CONTENT,				/* Intermediate states */
    FIRSTLETTER_D,
    FIRSTLETTER_L,
    CONTENTLETTER_L,
    CONTENTLETTER_T,

    ALLOW,				/* Headers supported */
    AUTHENTICATE,
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
    HTStream *			target;
    HTFormat			target_format;
    HTChunk *			buffer;
    HTSocketEOL			EOLstate;
    BOOL			transparent;
};

/* ------------------------------------------------------------------------- */

/*
**	This is a FSM parser which is tolerant as it can be of all
**	syntax errors.  It ignores field names it does not understand,
**	and resynchronises on line beginnings.
*/
PRIVATE void parseheader ARGS3(HTStream *, me, HTRequest *, request,
			       HTParentAnchor *, anchor)
{
    MIME_state state = BEGINNING_OF_LINE;
    MIME_state ok_state;			  /* got this state if match */
    char *ptr = me->buffer->data-1;     /* We dont change the data in length */
    char *stop = ptr+me->buffer->size;			     /* When to stop */
    char *header = ptr;      				  /* For diagnostics */
    CONST char * check_pointer;				   /* checking input */
    char *value;
    me->transparent = YES;		  /* Pump rest of data right through */
    if (!me->buffer->data)			       /* No header to parse */
	return;
    while (ptr < stop) {
	switch (state) {
	  case BEGINNING_OF_LINE:
	    header = ++ptr;
	    switch (TOLOWER(*ptr)) {
	      case 'a':
		check_pointer = "llow";
		ok_state = ALLOW;
		state = CHECK;
		break;

	      case 'c':
		check_pointer = "ontent-";
		ok_state = CONTENT;
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

	      case 'l':
		state = FIRSTLETTER_L;
		break;

	      case 'm':
		check_pointer = "ime-version";
		ok_state = JUNK_LINE;  /* We don't use this but recognize it */
		state = CHECK;
		break;

	      case 'p':
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
	    while (TOLOWER(*ptr) == *(check_pointer)++) ptr++;
	    if (!*--check_pointer) {
		state = ok_state;
		while (*ptr && (WHITE(*ptr) || *ptr==':')) /* Spool to value */
		    ptr++;
	    } else
		state = UNKNOWN;
	    break;

	  case ALLOW:	    
	    while ((value = HTNextField(&ptr)) != NULL) {
		HTMethod new_method;
		/* We treat them as case-insensitive! */
		if ((new_method = HTMethod_enum(value)) != METHOD_INVALID)
		    anchor->methods += new_method;
	    }
	    if (STREAM_TRACE)
		fprintf(TDEST, "MIMEParser.. Methods allowed: %d\n",
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
		while ((value = HTNextField(&ptr)) != NULL) {	  /* Charset */
		    if (!strcasecomp(value, "charset")) {
			if ((value = HTNextField(&ptr)) != NULL) {
			    lc = value;
			    while ((*lc = TOLOWER(*lc))) lc++;
			    anchor->charset = HTAtom_for(value);
			}
		    } else if (!strcasecomp(value, "level")) {	    /* Level */
			if ((value = HTNextField(&ptr)) != NULL) {
			    lc = value;
			    while ((*lc = TOLOWER(*lc))) lc++;
			    anchor->level = HTAtom_for(value);
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
	    if ((value = HTNextField(&ptr)) != NULL)
		StrAllocCopy(request->redirect, value);
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
	    if (STREAM_TRACE)
		fprintf(TDEST,"MIMEParser.. Unknown header: `%s\'\n", header);
	    HTAnchor_addExtra(anchor, header);

	    /* Fall through */

	  case JUNK_LINE:
	    while (*ptr) ptr++;
	    state = BEGINNING_OF_LINE;
	    break;
	}
    }

    if (STREAM_TRACE)
	fprintf(TDEST, "MIMEParser.. Media type %s is converted to %s\n",
		HTAtom_name(anchor->content_type),
		HTAtom_name(me->target_format));
    if ((me->target = HTStreamStack(anchor->content_type,
				    me->target_format, me->target,
				    me->request, YES)) == NULL) {
	if (STREAM_TRACE)
	    fprintf(TDEST, "MIMEParser.. Can't convert media type\n");
	me->target = HTBlackHole();
    }
    anchor->header_parsed = YES;
}


/*
**	Header is terminated by CRCR, LFLF, CRLFLF, CRLFCRLF
**	Folding is either of CF LWS, LF LWS, CRLF LWS
*/
PRIVATE int HTMIME_put_block ARGS3(HTStream *, me, CONST char *, b, int, l)
{
    while (!me->transparent && l-- > 0) {
	if (me->EOLstate == EOL_FCR) {
	    if (*b == CR)				    /* End of header */
		parseheader(me, me->request, me->request->anchor);
	    else if (*b == LF)				   	     /* CRLF */
		me->EOLstate = EOL_FLF;
	    else if (WHITE(*b)) {			   /* Folding: CR SP */
		me->EOLstate = EOL_BEGIN;
		HTChunkPutc(me->buffer, ' ');
	    } else {						 /* New line */
		me->EOLstate = EOL_BEGIN;
		HTChunkPutc(me->buffer, '\0');
		HTChunkPutc(me->buffer, *b);
	    }
	} else if (me->EOLstate == EOL_FLF) {
	    if (*b == CR)				/* LF CR or CR LF CR */
		me->EOLstate = EOL_SCR;
	    else if (*b == LF)				    /* End of header */
		parseheader(me, me->request, me->request->anchor);
	    else if (WHITE(*b)) {	       /* Folding: LF SP or CR LF SP */
		me->EOLstate = EOL_BEGIN;
		HTChunkPutc(me->buffer, ' ');
	    } else {						/* New line */
		me->EOLstate = EOL_BEGIN;
		HTChunkPutc(me->buffer, '\0');
		HTChunkPutc(me->buffer, *b);
	    }
	} else if (me->EOLstate == EOL_SCR) {
	    if (*b==CR || *b==LF)			    /* End of header */
		parseheader(me, me->request, me->request->anchor);
	    else if (WHITE(*b)) {	 /* Folding: LF CR SP or CR LF CR SP */
		me->EOLstate = EOL_BEGIN;
		HTChunkPutc(me->buffer, ' ');
	    } else {						/* New line */
		me->EOLstate = EOL_BEGIN;
		HTChunkPutc(me->buffer, '\0');
		HTChunkPutc(me->buffer, *b);
	    }
	} else if (*b == CR) {
	    me->EOLstate = EOL_FCR;
	} else if (*b == LF) {
	    me->EOLstate = EOL_FLF;			       /* Line found */
	} else
	    HTChunkPutc(me->buffer, *b);
	b++;
    }
    if (me->target) {				    /* Is the stream set up? */
	if (l > 0)					   /* Anything left? */
	    return (*me->target->isa->put_block)(me->target, b, l);
	return HT_OK;
    }
    return HT_WOULD_BLOCK;
}


/*	Character handling
**	------------------
*/
PRIVATE int HTMIME_put_character ARGS2(HTStream *, me, char, c)
{
    return HTMIME_put_block(me, &c, 1);
}


/*	String handling
**	---------------
*/
PRIVATE int HTMIME_put_string ARGS2(HTStream *, me, CONST char *, s)
{
    return HTMIME_put_block(me, s, (int) strlen(s));
}


/*	Flush an stream object
**	---------------------
*/
PRIVATE int HTMIME_flush ARGS1(HTStream *, me)
{
    return (*me->target->isa->flush)(me->target);
}

/*	Free a stream object
**	--------------------
*/
PRIVATE int HTMIME_free ARGS1(HTStream *, me)
{
    int status = HT_OK;
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target))==HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    if (PROT_TRACE)
	fprintf(TDEST, "MIME........ FREEING....\n");
    HTChunkFree(me->buffer);
    free(me);
    return status;
}

/*	End writing
*/
PRIVATE int HTMIME_abort ARGS2(HTStream *, me, HTError, e)
{
    int status = HT_ERROR;
    if (me->target)
	status = (*me->target->isa->abort)(me->target, e);
    if (PROT_TRACE)
	fprintf(TDEST, "MIME........ ABORTING...\n");
    HTChunkFree(me->buffer);
    free(me);
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


/*	Subclass-specific Methods
**	-------------------------
*/
PUBLIC HTStream* HTMIMEConvert ARGS5(
	HTRequest *,		request,
	void *,			param,
	HTFormat,		input_format,
	HTFormat,		output_format,
	HTStream *,		output_stream)
{
    HTStream* me;
    if ((me=(HTStream *) calloc(1, sizeof(* me))) == NULL)
	outofmem(__FILE__, "HTMIMEConvert");
    me->isa = &HTMIME;       
    me->request = request;
    me->target = output_stream;
    me->target_format = output_format;
    me->buffer = HTChunkCreate(512);
    me->EOLstate = EOL_BEGIN;
    return me;
}
