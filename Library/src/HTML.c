/*									 HTML.c
**	SIMPLE HTML PARSER WITHOUT ANY PRESENTATION CODE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This generates of a hypertext object.  It converts from the
**	structured stream interface foo HTML events into the style-
**	oriented interface of the HText interface.
**
** HISTORY:
**	 8 Jul 94  FM	Insulate free() from _free structure element.
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWHTML.h"
#include "HTML.h"
#include "HTextImp.h"

#define PUTC(t,c)	(*(t)->target->isa->put_character)((t)->target, (c))
#define PUTS(t,s)	(*(t)->target->isa->put_string)((t)->target, (s))
#define PUTB(s,b,l)	(*(t)->target->isa->put_block)((t)->target, (b), (l))
#define FLUSH_TARGET(t)	(*(t)->target->isa->flush)((t)->target)
#define FREE_TARGET(t)	(*(t)->target->isa->_free)((t)->target)
#define ABORT_TARGET(t)	(*(t)->target->isa->abort)((t)->target, e)

#define MAX_NESTING 40

struct _HTStream {
    const HTStreamClass *	isa;
    /* .... */
};

struct _HTStructured {
    const HTStructuredClass * 	isa;
    HTRequest *			request;
    HTParentAnchor * 		node_anchor;
    HTextImp * 			text;
    HTStream *			target;
    HTChunk * 			title;
    BOOL			in_word;
    SGML_dtd *			dtd;
    char *			comment_start;	/* for literate programming */
    char *			comment_end;
    BOOL			started;

    int				overflow;
    int * 			sp;
    int	 			stack[MAX_NESTING];
};

/*
** 	Entity values -- for ISO Latin 1 local representation
**	This MUST match exactly the table referred to in the DTD!
*/
static char * ISO_Latin1[HTML_ENTITIES] = {
/* 00 */
  	"\306",	/* capital AE diphthong (ligature) */ 
  	"\301",	/* capital A, acute accent */ 
  	"\302",	/* capital A, circumflex accent */ 
  	"\300",	/* capital A, grave accent */ 
  	"\305",	/* capital A, ring */ 
  	"\303",	/* capital A, tilde */ 
  	"\304",	/* capital A, dieresis or umlaut mark */ 
  	"\307",	/* capital C, cedilla */ 
  	"\320",	/* capital Eth, Icelandic */ 
  	"\311",	/* capital E, acute accent */ 
/* 10 */
  	"\312",	/* capital E, circumflex accent */ 
  	"\310",	/* capital E, grave accent */ 
  	"\313",	/* capital E, dieresis or umlaut mark */ 
  	"\315",	/* capital I, acute accent */ 
  	"\316",	/* capital I, circumflex accent */ 
  	"\314",	/* capital I, grave accent */ 
  	"\317",	/* capital I, dieresis or umlaut mark */ 
  	"\321",	/* capital N, tilde */ 
  	"\323",	/* capital O, acute accent */ 
  	"\324",	/* capital O, circumflex accent */ 
/* 20 */
  	"\322",	/* capital O, grave accent */ 
  	"\330",	/* capital O, slash */ 
  	"\325",	/* capital O, tilde */ 
  	"\326",	/* capital O, dieresis or umlaut mark */ 
  	"\336",	/* capital THORN, Icelandic */ 
  	"\332",	/* capital U, acute accent */ 
  	"\333",	/* capital U, circumflex accent */ 
  	"\331",	/* capital U, grave accent */ 
  	"\334",	/* capital U, dieresis or umlaut mark */ 
  	"\335",	/* capital Y, acute accent */ 
/* 30 */
  	"\341",	/* small a, acute accent */ 
  	"\342",	/* small a, circumflex accent */ 
  	"\264",	/* acute accent */
  	"\346",	/* small ae diphthong (ligature) */ 
  	"\340",	/* small a, grave accent */ 
  	"\046",	/* ampersand */ 
  	"\345",	/* small a, ring */ 
  	"\343",	/* small a, tilde */ 
  	"\344",	/* small a, dieresis or umlaut mark */ 
        "\246",	/* broken vertical bar */
/* 40 */
  	"\347",	/* small c, cedilla */ 
	"\270",	/* cedilla */
	"\242", /* cent sign */
        "\251",	/* copyright */
        "\244",	/* general currency sign */
  	"\260",	/* degree sign */
  	"\367",	/* division sign */
  	"\351",	/* small e, acute accent */ 
  	"\352",	/* small e, circumflex accent */ 
  	"\350",	/* small e, grave accent */ 
/* 50 */
  	"\360",	/* small eth, Icelandic */ 
  	"\353",	/* small e, dieresis or umlaut mark */ 
  	"\275",	/* fraction one-half */
  	"\274",	/* fraction one-fourth */
  	"\276",	/* fraction three-fourth */
  	"\076",	/* greater than */ 
  	"\355",	/* small i, acute accent */ 
  	"\356",	/* small i, circumflex accent */ 
	"\241", /* inverted exclamation */
  	"\354",	/* small i, grave accent */ 
/* 60 */
  	"\277",	/* inverted question mark */
  	"\357",	/* small i, dieresis or umlaut mark */ 
  	"\253",	/* left angle quote */
  	"\074",	/* less than */ 
  	"\257",	/* macron accent */
  	"\265",	/* micro sign (greek mu) */
  	"\267",	/* middle dot */
	"\040", /* non-breaking space */
  	"\254",	/* not sign */
  	"\361",	/* small n, tilde */ 
/* 70 */
  	"\363",	/* small o, acute accent */ 
  	"\364",	/* small o, circumflex accent */ 
  	"\362",	/* small o, grave accent */ 
  	"\252",	/* feminine ordinal */
  	"\272",	/* masculine ordinal */
  	"\370",	/* small o, slash */ 
  	"\365",	/* small o, tilde */ 
  	"\366",	/* small o, dieresis or umlaut mark */ 
  	"\266",	/* paragraph sign */
  	"\261",	/* plus or minus */
/* 80 */
	"\243", /* pound sign */
        "\042", /* double quote sign - June 94 */
	"\273",	/* right angle quote */
  	"\256",	/* registered trademark */
	"\247", /* section sign */
  	"\255",	/* soft hyphen */
  	"\271",	/* superscript 1 */
  	"\262",	/* superscript 2 */
  	"\263",	/* superscript 3 */
  	"\337",	/* small sharp s, German (sz ligature) */ 
/* 90 */
  	"\376",	/* small thorn, Icelandic */ 
  	"\327",	/* multiply sign */
  	"\372",	/* small u, acute accent */ 
  	"\373",	/* small u, circumflex accent */ 
  	"\371",	/* small u, grave accent */ 
        "\250",	/* dieresis or umlaut mark */
  	"\374",	/* small u, dieresis or umlaut mark */ 
  	"\375",	/* small y, acute accent */ 
	"\245", /* yen sign */
  	"\377"	/* small y, dieresis or umlaut mark */ 
/* 100 */
};

PRIVATE char ** CurrentEntityValues = ISO_Latin1;

PUBLIC BOOL HTMLUseCharacterSet (HTMLCharacterSet i)
{
    if (i == HTML_ISO_LATIN1) {
	CurrentEntityValues = ISO_Latin1;
	return YES;
    } else {
	HTTRACE(SGML_TRACE, "HTML Parser. Doesn't support this character set\n");
	return NO;
    }
}

PRIVATE int HTML_write (HTStructured * me, const char * b, int l)
{
    if (!me->started) {
	HTextImp_build(me->text, HTEXT_BEGIN);
	me->started = YES;
    }

    /* Look at what we got */
    switch (me->sp[0]) {

    case HTML_TITLE:
	HTChunk_putb(me->title, b, l);
	/* Fall through */
	
    default:
	HTextImp_addText(me->text, b, l);
    }
    return HT_OK;
}

PRIVATE int HTML_put_character (HTStructured * me, char c)
{
    return HTML_write(me, &c, sizeof(char));
}

PRIVATE int HTML_put_string (HTStructured * me, const char* s)
{
    return HTML_write(me, s, (int) strlen(s));
}

PRIVATE void HTML_start_element (HTStructured *	me,
				 int		element_number,
				 const BOOL * 	present,
				 const char **	value)
{
    HTChildAnchor * address = NULL;
    if (!me->started) {
	HTextImp_build(me->text, HTEXT_BEGIN);
	me->started = YES;
    }

    /* Look at what element was started */
    switch (element_number) {
    case HTML_A:
	if (present[HTML_A_HREF] && value[HTML_A_HREF]) {
	    address = HTAnchor_findChildAndLink(
		me->node_anchor,					/* parent */
		present[HTML_A_NAME] ? value[HTML_A_NAME] : NULL,	/* Tag */
		value[HTML_A_HREF],					/* Addresss */
		present[HTML_A_REL] && value[HTML_A_REL] ? 
		(HTLinkType) HTAtom_caseFor(value[HTML_A_REL]) : NULL);
	    
	    if (present[HTML_A_TITLE] && value[HTML_A_TITLE]) {
		HTLink * link = HTAnchor_mainLink((HTAnchor *) address);
		HTParentAnchor * dest = HTAnchor_parent(HTLink_destination(link));
		if (!HTAnchor_title(dest)) HTAnchor_setTitle(dest, value[HTML_A_TITLE]);
	    }
	    HTextImp_foundLink(me->text, element_number, HTML_A_HREF,
			       address, present, value);
	    HTTRACE(SGML_TRACE, "HTML Parser. Anchor `%s\'\n" _ value[HTML_A_HREF]);
	}
	break;

    case HTML_AREA:
	if (present[HTML_AREA_HREF] && value[HTML_AREA_HREF]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_AREA_HREF], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_AREA_HREF,
			       address, present, value);
	    HTTRACE(SGML_TRACE, "HTML Parser. Image map area `%s\'\n" _ value[HTML_AREA_HREF]);
	}
	break;

    case HTML_BASE:
	if (present[HTML_BASE_HREF] && value[HTML_BASE_HREF]) {
	    HTAnchor_setBase(me->node_anchor, (char *) value[HTML_BASE_HREF]);
	    HTTRACE(SGML_TRACE, "HTML Parser. New base `%s\'\n" _ value[HTML_BASE_HREF]);
	}
	break;

    case HTML_BODY:
	if (present[HTML_BODY_BACKGROUND] && value[HTML_BODY_BACKGROUND]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_BODY_BACKGROUND], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_BODY_BACKGROUND,
			       address, present, value);
	    HTTRACE(SGML_TRACE, "HTML Parser. Background `%s\'\n" _ value[HTML_BODY_BACKGROUND]);
	}
	break;

    case HTML_FORM:
	if (present[HTML_FORM_ACTION] && value[HTML_FORM_ACTION]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_FORM_ACTION], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_FORM_ACTION,
			       address, present, value);
	}
	break;

    case HTML_FRAME:
	if (present[HTML_FRAME_SRC] && value[HTML_FRAME_SRC]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_FRAME_SRC], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_FRAME_SRC,
			       address, present, value);
	    HTTRACE(SGML_TRACE, "HTML Parser. Frame `%s\'\n" _ value[HTML_FRAME_SRC]);
	}
	break;
	
    case HTML_INPUT:
	if (present[HTML_INPUT_SRC] && value[HTML_INPUT_SRC]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_INPUT_SRC], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_INPUT_SRC,
			       address, present, value);
	}
	break;

    case HTML_IMG:
	if (present[HTML_IMG_SRC] && value[HTML_IMG_SRC]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_IMG_SRC], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_IMG_SRC,
			       address, present, value);
	}
	break;

    case HTML_ISINDEX:
   	HTAnchor_setIndex(me->node_anchor);
	break;
	
    case HTML_LINK:
	if (present[HTML_LINK_HREF] && value[HTML_LINK_HREF]) {
	    HTParentAnchor * dest = NULL;
	    address = HTAnchor_findChildAndLink(
		me->node_anchor,					/* parent */
		present[HTML_A_NAME] ? value[HTML_A_NAME] : NULL,	/* Tag */
		present[HTML_A_HREF] ? value[HTML_A_HREF] : NULL,	/* Addresss */
		NULL);							/* Rels */
	    dest = HTAnchor_parent(HTAnchor_followMainLink((HTAnchor *) address));

	    /* If forward reference */
	    if ((present[HTML_LINK_REL] && value[HTML_LINK_REL])) {
		char * strval = NULL;
		char * ptr = NULL;
		char * relation = NULL;
		StrAllocCopy(strval, value[HTML_LINK_REL]);
		ptr = strval;
		while ((relation = HTNextLWSToken(&ptr)) != NULL) {
		    HTLink_add((HTAnchor *) me->node_anchor, (HTAnchor *) dest,
			       (HTLinkType) HTAtom_caseFor(relation),
			       METHOD_INVALID);
		}
		HT_FREE(strval);
	    }

	    /* If reverse reference */
	    if ((present[HTML_LINK_REV] && value[HTML_LINK_REV])) {
		char * strval = NULL;
		char * ptr = NULL;
		char * relation = NULL;
		StrAllocCopy(strval, value[HTML_LINK_REV]);
		ptr = strval;
		while ((relation = HTNextLWSToken(&ptr)) != NULL) {
		    HTLink_add((HTAnchor *) dest, (HTAnchor *) me->node_anchor,
			       (HTLinkType) HTAtom_caseFor(relation),
			       METHOD_INVALID);
		}
		HT_FREE(strval);
	    }

	    /* If we got any type information as well */
	    if (present[HTML_LINK_TYPE] && value[HTML_LINK_TYPE]) {
		if (HTAnchor_format(dest) == WWW_UNKNOWN)
		    HTAnchor_setFormat(dest,
				       (HTFormat) HTAtom_caseFor(value[HTML_LINK_TYPE]));
	    }

	    /* Call out to the layout engine */
	    HTextImp_foundLink(me->text, element_number, HTML_LINK_HREF,
			       address, present, value);
	}
	break;

    case HTML_META:
	if (present[HTML_META_NAME] && value[HTML_META_NAME]) {
	    HTAnchor_addMeta (me->node_anchor,
			      value[HTML_META_NAME],
			      (present[HTML_META_CONTENT] && value[HTML_META_CONTENT]) ?
			      value[HTML_META_CONTENT] : "");
	}
	break;

    case HTML_OBJECT:
	if (present[HTML_OBJECT_CLASSID] && value[HTML_OBJECT_CLASSID]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_OBJECT_CLASSID], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_OBJECT_CLASSID,
			       address, present, value);
	}

	if (present[HTML_OBJECT_CODEBASE] && value[HTML_OBJECT_CODEBASE]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_OBJECT_CODEBASE], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_OBJECT_CODEBASE,
			       address, present, value);
	}

	if (present[HTML_OBJECT_DATA] && value[HTML_OBJECT_DATA]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_OBJECT_DATA], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_OBJECT_DATA,
			       address, present, value);
	}

	if (present[HTML_OBJECT_ARCHIVE] && value[HTML_OBJECT_ARCHIVE]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_OBJECT_ARCHIVE], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_OBJECT_ARCHIVE,
			       address, present, value);
	}

	if (present[HTML_OBJECT_USEMAP] && value[HTML_OBJECT_USEMAP]) {
	    address = HTAnchor_findChildAndLink(me->node_anchor, NULL,
						value[HTML_OBJECT_USEMAP], NULL);
	    HTextImp_foundLink(me->text, element_number, HTML_OBJECT_USEMAP,
			       address, present, value);
	}
	break;

    case HTML_PRE:
    	if (me->comment_end)
	    HTextImp_addText(me->text, me->comment_end, strlen(me->comment_end));
	break;

    case HTML_TITLE:
        HTChunk_truncate(me->title,0);
	break;
    }

    /* Update our parse stack */
    if (SGML_findTagContents(me->dtd, element_number) != SGML_EMPTY) {
        if (me->sp == me->stack) {
	    HTTRACE(SGML_TRACE, "HTML Parser. Maximum nesting of %d exceded!\n" _ MAX_NESTING); 
	    me->overflow++;
	    return;
	}
    	--(me->sp);
	me->sp[0] = element_number;
    }	

    /* Call out to the layout engine */
    HTextImp_beginElement(me->text, element_number, present, value);
}

PRIVATE void HTML_end_element (HTStructured * me, int element_number)
{
    if (!me->started) {
	HTextImp_build(me->text, HTEXT_BEGIN);
	me->started = YES;
    }

    /* Update our parse stack */
    if (me->overflow > 0) {
	me->overflow--;
	return;
    }
    me->sp++;
    if (me->sp > me->stack + MAX_NESTING - 1) {
	HTTRACE(SGML_TRACE, "HTML Parser. Bottom of parse stack reached\n");
	me->sp = me->stack + MAX_NESTING - 1;
    }

    /* Look at what element was closed */
    switch(element_number) {
    case HTML_TITLE:
    	HTAnchor_setTitle(me->node_anchor, HTChunk_data(me->title));
	break;
	
    case HTML_PRE:
    	if (me->comment_start)
	    HTextImp_addText(me->text, me->comment_start, strlen(me->comment_start));
	break;
    }

    /* Call out to the layout engine */
    HTextImp_endElement(me->text, element_number);
}

PRIVATE void HTML_put_entity (HTStructured * me, int entity_number)
{
    if (!me->started) {
	HTextImp_build(me->text, HTEXT_BEGIN);
	me->started = YES;
    }
    if (entity_number>=0 && entity_number<HTML_ENTITIES)
	HTML_put_string(me, *(CurrentEntityValues+entity_number));
}

PUBLIC int HTML_flush (HTStructured * me)
{
    if (!me->started) {
	HTextImp_build(me->text, HTEXT_BEGIN);
	me->started = YES;
    }
    if (me->comment_end) HTML_put_string(me, me->comment_end);
    return me->target ? FLUSH_TARGET(me) : HT_OK;
}

PRIVATE int HTML_unparsedBeginElement (HTStructured * me, const char * b, int l)
{
    if (!me->started) {
	HTextImp_build(me->text, HTEXT_BEGIN);
	me->started = YES;
    }
    HTextImp_unparsedBeginElement(me->text, b, l);
    return HT_OK;
}

PRIVATE int HTML_unparsedEndElement (HTStructured * me, const char * b, int l)
{
    if (!me->started) {
	HTextImp_build(me->text, HTEXT_BEGIN);
	me->started = YES;
    }
    HTextImp_unparsedEndElement(me->text, b, l);
    return HT_OK;
}

PRIVATE int HTML_unparsedEntity (HTStructured * me, const char * b, int l)
{
    if (!me->started) {
	HTextImp_build(me->text, HTEXT_BEGIN);
	me->started = YES;
    }
    HTextImp_unparsedEntity(me->text, b, l);
    return HT_OK;
}

PUBLIC int HTML_free (HTStructured * me)
{
    if (!me->started) HTextImp_build(me->text, HTEXT_BEGIN);
    if (me->comment_end) HTML_put_string(me, me->comment_end);
    HTextImp_build(me->text, HTEXT_END);
    HTextImp_delete(me->text);
    HTChunk_delete(me->title);
    if (me->target) FREE_TARGET(me);
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int HTML_abort (HTStructured * me, HTList * e)
{
    if (!me->started) HTextImp_build(me->text, HTEXT_BEGIN);
    HTextImp_build(me->text, HTEXT_ABORT);
    HTextImp_delete(me->text);
    HTChunk_delete(me->title);
    if (me->target) ABORT_TARGET(me);
    HT_FREE(me);
    return HT_ERROR;
}

/*	Structured Object Class
**	-----------------------
*/
PRIVATE const HTStructuredClass HTMLPresentation = /* As opposed to print etc */
{		
    "text/html",
    HTML_flush,
    HTML_free,
    HTML_abort,
    HTML_put_character,
    HTML_put_string,
    HTML_write,
    HTML_start_element,
    HTML_end_element,
    HTML_put_entity,
    HTML_unparsedBeginElement,
    HTML_unparsedEndElement,
    HTML_unparsedEntity
};

/*	Structured Text object
**	----------------------
**
**	The structured stream can generate either presentation,
**	or plain text, or HTML.
*/
PRIVATE HTStructured * HTML_new (HTRequest *	request,
				 void *		param,
				 HTFormat	input_format,
				 HTFormat	output_format,
				 HTStream *	output_stream)
{
    HTStructured * me = NULL;
    if (request) {
	if ((me = (HTStructured *) HT_CALLOC(1, sizeof(HTStructured))) == NULL)
	    HT_OUTOFMEM("HTML_new");
	me->isa = &HTMLPresentation;
	me->dtd = HTML_dtd();
	me->request = request;
	me->node_anchor =  HTRequest_anchor(request);
	me->title = HTChunk_new(128);
	me->comment_start = NULL;
	me->comment_end = NULL;
	me->target = output_stream;
	me->sp = me->stack + MAX_NESTING - 1;

	/* Create the text object */
	me->text = HTextImp_new(me->request, me->node_anchor, me->target);
    }
    return me;
}

/*	HTConverter for HTML to plain text
**	----------------------------------
**
**	This will convert from HTML to presentation or plain text.
*/
PUBLIC HTStream * HTMLToPlain (HTRequest *	request,
			       void *		param,
			       HTFormat		input_format,
			       HTFormat		output_format,
			       HTStream *	output_stream)
{
    return SGML_new(HTML_dtd(), HTML_new(
    	request, NULL, input_format, output_format, output_stream));
}


/*	HTConverter for HTML to C code
**	------------------------------
**
**	C code is like plain text but all non-preformatted code
**	is commented out.
**	This will convert from HTML to presentation or plain text.
*/
PUBLIC HTStream * HTMLToC (HTRequest *	request,
			   void *	param,
			   HTFormat	input_format,
			   HTFormat	output_format,
			   HTStream *	output_stream)
{
    if (output_stream) {
	HTStructured * html = NULL;
	(*output_stream->isa->put_string)(output_stream, "/* "); /* Before title */
	html = HTML_new(request, NULL, input_format, output_format, output_stream);
	html->comment_start = "\n/* ";
	html->dtd = HTML_dtd();
	html->comment_end = " */\n";	/* Must start in col 1 for cpp */
	return SGML_new(HTML_dtd(), html);
    } else
	return HTErrorStream();
}


/*	Presenter for HTML
**	------------------
**
**	This will convert from HTML to presentation or plain text.
**
**	Override this if you have a windows version
*/
PUBLIC HTStream * HTMLPresent (HTRequest *	request,
			       void *		param,
			       HTFormat		input_format,
			       HTFormat		output_format,
			       HTStream *	output_stream)
{
    return SGML_new(HTML_dtd(), HTML_new(
    	request, NULL, input_format, output_format, output_stream));
}

