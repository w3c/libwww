/*			MIME Message Parse			HTMIME.c
**			==================
**
**	This is RFC 1341-specific code.
**	The input stream pushed into this parser is assumed to be
**	stripped on CRs, ie lines end with LF, not CR LF.
**	(It is easy to change this except for the body part where
**	conversion can be slow.)
**
** History:
**	   Feb 92	Written Tim Berners-Lee, CERN
**
*/
#include "HTMIME.h"		/* Implemented here */
#include "HTAlert.h"


/*		MIME Object
**		-----------
*/

typedef enum _MIME_state {
	MIME_TRANSPARENT,	/* put straight through to target ASAP! */
	BEGINNING_OF_LINE,
	CONTENT_T,
	CONTENT_TRANSFER_ENCODING,
	CONTENT_TYPE,
	SKIP_GET_VALUE,		/* Skip space then get value */
	GET_VALUE,		/* Get value till white space */
	JUNK_LINE,		/* Ignore the rest of this folded line */
	NEWLINE,		/* Just found a LF .. maybe continuation */
	CHECK,			/* check against check_pointer */
	MIME_NET_ASCII,		/* Translate from net ascii */
	MIME_IGNORE		/* ignore entire file */
	/* TRANSPARENT and IGNORE are defined as stg else in _WINDOWS */
} MIME_state;

#define VALUE_SIZE 128		/* @@@@@@@ Arbitrary? */
struct _HTStream {
	CONST HTStreamClass *	isa;
	
	BOOL			net_ascii;	/* Is input net ascii? */
	MIME_state		state;		/* current state */
	MIME_state		if_ok;		/* got this state if match */
	MIME_state		field;		/* remember which field */
	MIME_state		fold_state;	/* state on a fold */
	CONST char *		check_pointer;	/* checking input */
	
	char *			value_pointer;	/* storing values */
	char 			value[VALUE_SIZE];
	
	HTParentAnchor *	anchor;		/* Given on creation */
	HTStream *		sink;		/* Given on creation */
	
	char *			boundary;	/* For multipart */
	
	HTFormat		encoding;	/* Content-Transfer-Encoding */
	HTFormat		format;		/* Content-Type */
	HTStream *		target;		/* While writing out */
	HTStreamClass		targetClass;
	
	HTAtom *		targetRep;	/* Converting into? */
};


/*_________________________________________________________________________
**
**			A C T I O N 	R O U T I N E S
*/

/*	Character handling
**	------------------
**
**	This is a FSM parser which is tolerant as it can be of all
**	syntax errors.  It ignores field names it does not understand,
**	and resynchronises on line beginnings.
*/

PRIVATE void HTMIME_put_character ARGS2(HTStream *, me, char, c)
{
    if (me->state == MIME_TRANSPARENT) {
    	(*me->targetClass.put_character)(me->target, c);/* MUST BE FAST */
	return;
    }
    
    /* This slightly simple conversion just strips CR and turns LF to
    ** newline. On unix LF is \n but on Mac \n is CR for example.
    ** See NetToText for an implementation which preserves single CR or LF.
    */
    if (me->net_ascii) {
        c = FROMASCII(c);
	if (c == CR) return;
	else if (c == LF) c = '\n';
    }
    
    switch(me->state) {

    case MIME_IGNORE:
    	return;

    case MIME_TRANSPARENT:		/* Not reached see above */
    	(*me->targetClass.put_character)(me->target, c);
	return;
	
    case MIME_NET_ASCII:
    	(*me->targetClass.put_character)(me->target, c); /* MUST BE FAST */
	return;

    case NEWLINE:
	if (c != '\n' && WHITE(c)) {		/* Folded line */
	    me->state = me->fold_state;	/* pop state before newline */
	    break;
	}
	
	/*	else Falls through */
	
    case BEGINNING_OF_LINE:
        switch(c) {
	case 'c':
	case 'C':
	    me->check_pointer = "ontent-t";
	    me->if_ok = CONTENT_T;
	    me->state = CHECK;
	    break;
	case '\n':			/* Blank line: End of Header! */
	    {
	        if (TRACE) fprintf(stderr,
			"HTMIME: MIME content type is %s, converting to %s\n",
			HTAtom_name(me->format), HTAtom_name(me->targetRep));
		me->target = HTStreamStack(me->format, me->targetRep,
	 		me->sink , me->anchor);
		if (!me->target) {
		    if (TRACE) fprintf(stderr, "MIME: Can't translate! ** \n");
		    me->target = me->sink;	/* Cheat */
		}
		if (me->target) {
		    me->targetClass = *me->target->isa;
		/* Check for encoding and select state from there @@ */
		
		    me->state = MIME_TRANSPARENT; /* From now push straigh through */
		} else {
		    me->state = MIME_IGNORE;		/* What else to do? */
		}
	    }
	    break;
	    
	default:
	   goto bad_field_name;
	   break;
	   
	} /* switch on character */
        break;
	
    case CHECK:				/* Check against string */
        if (TOLOWER(c) == *(me->check_pointer)++) {
	    if (!*me->check_pointer) me->state = me->if_ok;
	} else {		/* Error */
	    if (TRACE) fprintf(stderr,
	    	"HTMIME: Bad character `%c' found where `%s' expected\n",
		c, me->check_pointer - 1);
	    goto bad_field_name;
	}
	break;
	
    case CONTENT_T:
        switch(c) {
	case 'r':
	case 'R':
	    me->check_pointer = "ansfer-encoding:";
	    me->if_ok = CONTENT_TRANSFER_ENCODING;
	    me->state = CHECK;
	    break;
	    
	case 'y':
	case 'Y':
	    me->check_pointer = "pe:";
	    me->if_ok = CONTENT_TYPE;
	    me->state = CHECK;
	    break;
	    
	default:
	    goto bad_field_name;
	    
	} /* switch on character */
	break;
	
    case CONTENT_TYPE:
    case CONTENT_TRANSFER_ENCODING:
        me->field = me->state;		/* remember it */
	me->state = SKIP_GET_VALUE;
				/* Fall through! */
    case SKIP_GET_VALUE:
    	if (c == '\n') {
	   me->fold_state = me->state;
	   me->state = NEWLINE;
	   break;
	}
	if (WHITE(c)) break;	/* Skip white space */
	
	me->value_pointer = me->value;
	me->state = GET_VALUE;   
	/* Fall through to store first character */
	
    case GET_VALUE:
    	if (WHITE(c)) {			/* End of field */
	    *me->value_pointer = 0;
	    switch (me->field) {
	    case CONTENT_TYPE:
	        me->format = HTAtom_for(me->value);
		break;
	    case CONTENT_TRANSFER_ENCODING:
	        me->encoding = HTAtom_for(me->value);
		break;
	    default:		/* Should never get here */
	    	break;
	    }
	} else {
	    if (me->value_pointer < me->value + VALUE_SIZE - 1) {
	        *me->value_pointer++ = c;
		break;
	    } else {
	        goto value_too_long;
	    }
	}
	/* Fall through */
	
    case JUNK_LINE:
        if (c == '\n') {
	    me->state = NEWLINE;
	    me->fold_state = me->state;
	}
	break;
	
	
    } /* switch on state*/
    
    return;
    
value_too_long:
    if (TRACE) fprintf(stderr,
    	"HTMIME: *** Syntax error. (string too long)\n");
    
bad_field_name:				/* Ignore it */
    me->state = JUNK_LINE;
    return;
    
}



/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE void HTMIME_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    CONST char * p;
    if (me->state == MIME_TRANSPARENT)		/* Optimisation */
        (*me->targetClass.put_string)(me->target,s);
    else if (me->state != MIME_IGNORE)
        for (p=s; *p; p++) HTMIME_put_character(me, *p);
}


/*	Buffer write.  Buffers can (and should!) be big.
**	------------
*/
PRIVATE void HTMIME_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    CONST char * p;
    if (me->state == MIME_TRANSPARENT)		/* Optimisation */
        (*me->targetClass.put_block)(me->target, s, l);
    else
        for (p=s; p < s+l; p++) HTMIME_put_character(me, *p);
}




/*	Free an HTML object
**	-------------------
**
*/
PRIVATE void HTMIME_free ARGS1(HTStream *, me)
{
    if (me->target) (*me->targetClass.free)(me->target);
    free(me);
}

/*	End writing
*/

PRIVATE void HTMIME_abort ARGS2(HTStream *, me, HTError, e)
{
    if (me->target) (*me->targetClass.abort)(me->target, e);
    free(me);
}



/*	Structured Object Class
**	-----------------------
*/
PRIVATE CONST HTStreamClass HTMIME =
{		
	"MIMEParser",
	HTMIME_free,
	HTMIME_abort,
	HTMIME_put_character,
	HTMIME_put_string,
	HTMIME_write
}; 


/*	Subclass-specific Methods
**	-------------------------
*/

PUBLIC HTStream* HTMIMEConvert ARGS3(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,
	HTStream *,		sink)
{
    HTStream* me;
    
    me = malloc(sizeof(*me));
    if (me == NULL) outofmem(__FILE__, "HTML_new");
    me->isa = &HTMIME;       

    me->sink = 		sink;
    me->anchor = 	anchor;
    me->target = 	NULL;
    me->state = 	BEGINNING_OF_LINE;
    me->format = 	WWW_PLAINTEXT;
    me->targetRep = 	pres->rep_out;
    me->boundary = 	0;		/* Not set yet */
    me->net_ascii = 	NO;	/* Local character set */
    return me;
}

PUBLIC HTStream* HTNetMIME ARGS3(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,
	HTStream *,		sink)
{
    HTStream* me = HTMIMEConvert(pres,anchor, sink);
    if (!me) return NULL;
    
    me->net_ascii = YES;
    return me;
}


