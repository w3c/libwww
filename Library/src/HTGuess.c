
/*		STREAM TO GUESS CONTENT-TYPE			HTGuess.c
**		============================
**
**	This version of the stream object just writes its input
**	to its output, but prepends Content-Type: field and an
**	empty line after it.
**
** HISTORY:
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**
*/


#define SAMPLE_SIZE	200	/* Number of chars to look at */

#include "HTGuess.h"

#include "HTFormat.h"
#include "HTAlert.h"
#include "HTList.h"

/*		Stream Object
**		------------
*/

struct _HTStream {
	CONST HTStreamClass *	isa;

	HTRequest *		req;
	HTStream *		output_stream;

	BOOL			discard;
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


PRIVATE BOOL is_html ARGS1(char *, buf)
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


#define PUT_CHAR(c)	\
    (*me->output_stream->isa->put_character)(me->output_stream,c)
#define PUT_STRING(s)	\
    (*me->output_stream->isa->put_string)(me->output_stream,s)
#define PUT_BLOCK(b,l)	\
    (*me->output_stream->isa->put_block)(me->output_stream,b,l)

#define CONTENT_TYPE(t)	\
    me->req->content_type = HTAtom_for(t)
#define CONTENT_ENCODING(t)	\
    me->req->content_encoding = HTAtom_for(t)


PRIVATE BOOL header_and_flush ARGS1(HTStream *, me)
{
    CTRACE(stderr,"GUESSING.... text=%d newlines=%d ctrl=%d high=%d\n",
	   me->text_cnt, me->lf_cnt, me->ctrl_cnt, me->high_cnt);
    if (me->cnt) {
	CTRACE(stderr,
	       "Percentages. text=%d%% newlines=%d%% ctrl=%d%% high=%d%%\n",
	       (int)(100*me->text_cnt/me->cnt + 0.5),
	       (int)(100*me->lf_cnt  /me->cnt + 0.5),
	       (int)(100*me->ctrl_cnt/me->cnt + 0.5),
	       (int)(100*me->high_cnt/me->cnt + 0.5));
    }

    if (!me->ctrl_cnt ||
	me->text_cnt + me->lf_cnt >= 16 * (me->ctrl_cnt + me->high_cnt)) {

	/* some kind of text */

	*me->write_ptr = 0;		/* terminate buffer */

	if (me->high_cnt > 0)
	    CONTENT_ENCODING("8bit");
	else
	    CONTENT_ENCODING("7bit");

	if (is_html(me->buffer))
	    CONTENT_TYPE("text/html");

	else if (!strncmp(me->buffer, "%!", 2))
	    CONTENT_TYPE("application/postscript");

	else if (strstr(me->buffer, "#define") &&
		 strstr(me->buffer, "_width") &&
		 strstr(me->buffer, "_bits"))
	    CONTENT_TYPE("image/x-xbitmap");

	else
	    CONTENT_TYPE("text/plain");
    }
    else {
	if (!strncmp(me->buffer, "GIF", 3))
	    CONTENT_TYPE("image/gif");

	else if (!strncmp(me->buffer, "\377\330\377\340", 4))
	    CONTENT_TYPE("image/jpeg");

	else if (!strcmp(me->buffer, "MM"))	/* MM followed by a zero */
	    CONTENT_TYPE("image/tiff");

	else if (!strncmp(me->buffer, ".snd", 4))
	    CONTENT_TYPE("audio/basic");

	else if (!strncmp(me->buffer, "\037\235", 2))
	    CONTENT_ENCODING("x-compress");

	else if (!strncmp(me->buffer, "\037\213", 2))
	    CONTENT_ENCODING("x-gzip");

	else
	    CONTENT_TYPE("application/octet-stream");
    }

    if (!me->req->content_type)  CONTENT_TYPE("www/unknown");
    if (!me->req->content_encoding)  CONTENT_ENCODING("binary");

    CTRACE(stderr,"Guessed..... %s\n", HTAtom_name(me->req->content_type));
    CTRACE(stderr,"Encoding.... %s\n", HTAtom_name(me->req->content_encoding));

    me->output_stream = HTStreamStack(me->req->content_type, me->req, NO);
    if (!me->output_stream) {
	me->discard = YES;	/* Turning into a black hole */
	return NO;
    }
    else {
	PUT_BLOCK(me->buffer, me->cnt);
	return YES;
    }
}


PRIVATE void HTGuess_put_character ARGS2(HTStream *, me, char, c)
{
    if (me->discard) return;
    if (me->output_stream) PUT_CHAR(c);
    else {
	me->cnt++;
#if 0
	if	(c < 0)	  me->high_cnt++;
	else
#endif
	if (c == LF) me->lf_cnt++;
	else if (c == CR) me->cr_cnt++;
	else if (c == 12) me->pg_cnt++;
	else if (c =='\t')me->text_cnt++;
	else if ((unsigned char)c < 32)  me->ctrl_cnt++;
	else if ((unsigned char)c < 128) me->text_cnt++;
	else		  me->high_cnt++;
	*me->write_ptr++ = c;
	if (me->cnt >= SAMPLE_SIZE) header_and_flush(me);
    }
}

PRIVATE void HTGuess_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    if (me->discard) return;
    if (me->output_stream) PUT_STRING(s);
    else {
	while (*s) {
	    HTGuess_put_character(me,*s);
	    s++;
	}
    }
}

PRIVATE void HTGuess_put_block ARGS3(HTStream *, me, CONST char*, b, int, l)
{
    if (me->discard) return;
    while (!me->output_stream && l > 0) {
	HTGuess_put_character(me, *b);
	b++;
	l--;
    }
    if (l > 0) PUT_BLOCK(b,l);
}

PRIVATE void HTGuess_free ARGS1(HTStream *, me)
{
    if (!me->discard && !me->output_stream)
	header_and_flush(me);
    if (me->output_stream)
	(*me->output_stream->isa->_free)(me->output_stream);
    free(me);
}

PRIVATE void HTGuess_abort ARGS2(HTStream *, me, HTError, e)
{
    if (me->output_stream)
	(*me->output_stream->isa->abort)(me,e);
    free(me);
}


/*	Guessing stream
**	---------------
*/
PRIVATE CONST HTStreamClass HTGuessClass =
{		
	"Guess",
	HTGuess_free,
	HTGuess_abort,
	HTGuess_put_character,
 	HTGuess_put_string,
	HTGuess_put_block
};



PUBLIC HTStream * HTGuess_new ARGS1(HTRequest *, req)
{
    HTStream * me = (HTStream*)calloc(1,sizeof(HTStream));
    if (!me) outofmem(__FILE__, "HTGuess_new");

    me->isa = &HTGuessClass;
    me->req =req;
    me->write_ptr = me->buffer;
    return me;
}

