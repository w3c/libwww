/*								      HTGuess.c
**	STREAM TO GUESS CONTENT-TYPE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This version of the stream object just writes its input
**	to its output, but prepends Content-Type: field and an
**	empty line after it.
**
** HISTORY:
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**
*/

/* Library include files */
#include "sysdep.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTFormat.h"
#include "HTAlert.h"
#include "HTAncMan.h"
#include "HTList.h"
#include "HTGuess.h"

#define SAMPLE_SIZE	200	/* Number of chars to look at */

/*		Stream Object
**		------------
*/

struct _HTStream {
	const HTStreamClass *	isa;

	HTRequest *		req;
	HTParentAnchor *	anchor;
	HTFormat		output_format;
	HTStream *		output_stream;
	HTStream *		target;

	BOOL			transparent;
	int			cnt;
	int			text_cnt;
	int			lf_cnt;
	int			cr_cnt;
	int			pg_cnt;
	int			ctrl_cnt;
	int			high_cnt;
	char *			write_ptr;
	char			buffer[ SAMPLE_SIZE + 1 ];
};

#define PUT_CHAR(c)		(*me->target->isa->put_character)(me->target,c)
#define PUT_STRING(s)		(*me->target->isa->put_string)(me->target,s)
#define PUT_BLOCK(b,l)		(*me->target->isa->put_block)(me->target,b,l)

/* ------------------------------------------------------------------------- */

PRIVATE BOOL is_html (char * buf)
{
    char * p = strchr(buf,'<');

    if (p && (!strncasecomp(p, "<HTML>", 6) ||
	      !strncasecomp(p, "<HEAD", 5) ||
	      !strncasecomp(p, "<TITLE>", 7) ||
	      !strncasecomp(p, "<BODY>", 6) ||
	      !strncasecomp(p, "<PLAINTEXT>", 11) ||
	      (p[0]=='<' && TOUPPER(p[1]) == 'H' && p[3]=='>')))
	return YES;
    else
	return NO;
}

PRIVATE int HTGuess_flush (HTStream * me)
{
    if (!me->transparent) {
	HTParentAnchor * anchor = me->anchor;
	if (STREAM_TRACE)
	    HTTrace("GUESSING.... text=%d newline=%d ctrl=%d high=%d\n",
		     me->text_cnt, me->lf_cnt, me->ctrl_cnt, me->high_cnt);
	if (me->cnt) {
	    if (STREAM_TRACE) HTTrace(
				    "Percentages. text=%d%% newlines=%d%% ctrl=%d%% high=%d%%\n",
				    (int)(100*me->text_cnt/me->cnt + 0.5),
				    (int)(100*me->lf_cnt  /me->cnt + 0.5),
				    (int)(100*me->ctrl_cnt/me->cnt + 0.5),
				    (int)(100*me->high_cnt/me->cnt + 0.5));
	}
	
	if (!me->ctrl_cnt ||
	    me->text_cnt + me->lf_cnt >= 16 * (me->ctrl_cnt + me->high_cnt)) {
	    char *ptr;
	    /* some kind of text */
	    
	    *me->write_ptr = 0;	/* terminate buffer */
	    
	    if (me->high_cnt > 0)
		HTAnchor_setTransfer(anchor, WWW_CTE_8BIT);
	    else
		HTAnchor_setTransfer(anchor, WWW_CTE_7BIT);
	    
	    if (is_html(me->buffer))
		HTAnchor_setFormat(anchor, HTAtom_for("text/html"));
	    
	    else if (!strncmp(me->buffer, "%!", 2))
		HTAnchor_setFormat(anchor, HTAtom_for("application/postscript"));
	    
	    else if (strstr(me->buffer, "#define") &&
		     strstr(me->buffer, "_width") &&
		     strstr(me->buffer, "_bits"))
		HTAnchor_setFormat(anchor, HTAtom_for("image/x-xbitmap"));
	    
	    else if ((ptr = strstr(me->buffer, "converted with BinHex"))!=NULL)
		HTAnchor_setTransfer(anchor, WWW_CTE_MACBINHEX);

	    else if (!strncmp(me->buffer, "begin ", 6))
		HTAnchor_setTransfer(anchor, WWW_CTE_BASE64);

	    else
		HTAnchor_setFormat(anchor, WWW_PLAINTEXT);
	}
	else {
	    if (!strncmp(me->buffer, "GIF", 3))
		HTAnchor_setFormat(anchor, WWW_GIF);

	    else if (!strncmp(me->buffer, "\377\330\377\340", 4))
		HTAnchor_setFormat(anchor, WWW_JPEG);

	    else if (!strcmp(me->buffer, "MM"))	/* MM followed by a zero */
		HTAnchor_setFormat(anchor, WWW_TIFF);

 	    else if (!strncmp(me->buffer, "\211PNG\r\n\032\n", 8))
 		HTAnchor_setFormat(anchor, WWW_PNG);

	    else if (!strncmp(me->buffer, ".snd", 4))
		HTAnchor_setFormat(anchor, WWW_AUDIO);

	    else if (!strncmp(me->buffer, "\037\235", 2))
		HTAnchor_addEncoding(anchor, WWW_CE_COMPRESS);

	    else if (!strncmp(me->buffer, "\037\213", 2))
		HTAnchor_addEncoding(anchor, WWW_CE_GZIP);

	    else
		HTAnchor_setFormat(anchor, WWW_BINARY);
	}
	
	if (anchor->content_type == WWW_UNKNOWN)
	    HTAnchor_setFormat(anchor, WWW_BINARY);
	if (!anchor->content_encoding)
	    HTAnchor_setTransfer(anchor, WWW_CTE_BINARY);
	
	if (STREAM_TRACE) {
	    HTTrace("Guessed..... C-T  : %s\n",
		    HTAtom_name(anchor->content_type));
	    HTTrace("............ C-E  : <...>\n");

	    /* @@@ */

	    HTTrace("............ C-T-E: <...>\n");

	    /* @@@ */

	}
	if ((me->target = HTStreamStack(anchor->content_type,
					me->output_format, me->output_stream,
					me->req, NO)) == NULL) {
	    if (STREAM_TRACE)
		HTTrace("HTGuess..... Can't convert media type\n");
	    me->target = HTErrorStream();
	}
	me->transparent = YES;
    }
    return PUT_BLOCK(me->buffer, me->cnt);
}


PRIVATE int HTGuess_put_block (HTStream * me, const char * b, int l)
{
    while (!me->transparent && l-- > 0) {
	int status;
	if (me->target) {
	    if ((status = HTGuess_flush(me)) != HT_OK)
		return status;
	} else {
	    me->cnt++;
	    if (*b == LF)
		me->lf_cnt++;
	    else if (*b == CR)
		me->cr_cnt++;
	    else if (*b == 12)
		me->pg_cnt++;
	    else if (*b =='\t')
		me->text_cnt++;
	    else if ((unsigned char)*b < 32)
		me->ctrl_cnt++;
	    else if ((unsigned char)*b < 128)
		me->text_cnt++;
	    else
		me->high_cnt++;
	    *me->write_ptr++ = *b++;
	    if (me->cnt >= SAMPLE_SIZE) {
		if ((status = HTGuess_flush(me)) != HT_OK)
		    return status;
	    }
	}
    }
    if (l > 0)
	return PUT_BLOCK(b, l);
    return HT_OK;
}

PRIVATE int HTGuess_put_character (HTStream * me, char c)
{
    return HTGuess_put_block(me, &c, 1);
}

PRIVATE int HTGuess_put_string (HTStream * me, const char * s)
{
    return HTGuess_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTGuess_free (HTStream * me)
{
    int status;
    if (!me->transparent && (status = HTGuess_flush(me)) != HT_OK)
	return status;
    else
	me->transparent = YES;
    if ((status = (*me->target->isa->_free)(me->target)) != HT_OK)
	return status;
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int HTGuess_abort (HTStream * me, HTList * e)
{
    if (me->target) (*me->target->isa->abort)(me->target, e);
    HT_FREE(me);
    return HT_ERROR;
}


/*	Guessing stream
**	---------------
*/
PRIVATE const HTStreamClass HTGuessClass =
{		
	"GuessWhat",
	HTGuess_flush,
	HTGuess_free,
	HTGuess_abort,
	HTGuess_put_character,
 	HTGuess_put_string,
	HTGuess_put_block
};

PUBLIC HTStream * HTGuess_new (HTRequest *	req,
			       void *		param,
			       HTFormat		input_format,
			       HTFormat		output_format,
			       HTStream *	output_stream)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1,sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTGuess_new");
    me->isa = &HTGuessClass;
    me->req = req;
    me->anchor = HTRequest_anchor(req);
    me->output_format = output_format;
    me->output_stream = output_stream;
    me->write_ptr = me->buffer;
    return me;
}
