/*		HTML Parser
**		===========
**
**  An HTML displayable object has associated with it
**
**		- The underlying text object for display
**		- An SGML parsing context
**		- An anchor representing the whole object
**		- A style sheet, in the case os a style-oriented version
**
**  The first three could logically be represented by multiple inheritance if
**  that were supported, as an HTML object is like a subclass of all three.
**
**  In practice in C,
**
**	- a HText object is created by this module (when needed)
**	- an SGML parsing object is created by this module
**	- the anchor representing the object is given at creation time
**
**  Those using structured HTML objects will wish to override this module
**  completely
*/
#include "HTML.h"

#include <ctype.h>
#include <stdio.h>

#include "HTAtom.h"
#include "HTChunk.h"
#include "HText.h"
#include "HTStyle.h"


/*				SPECIAL HTML CODE
**				=================
*/

extern HTStyleSheet * styleSheet;	/* Application-wide */

/*	Module-wide style cache
*/
PRIVATE HTStyle * glossary_style;
PRIVATE HTStyle * list_compact_style;
PRIVATE HTStyle * glossary_compact_style;
PRIVATE int 		got_styles = 0;


/*		HTML Object
**		-----------
*/
struct _HTML {
    HTParentAnchor * 	node_anchor;
    HText * 		text;
    HTSGMLContext	context;

    HTChunk title;	/* Grow by 128 */

/* Used in parsing: */

    BOOL	style_change;
    HTStyle *	new_style;
    HTStyle *	old_style;
    BOOL	in_word;  /* Have just had a non-white character */
};


/*		Forward declarations of routines
*/
PRIVATE void get_styles NOPARAMS;

/* For dtd: */
PRIVATE void no_change PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void begin_litteral PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void begin_element PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void end_element PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void begin_document PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void end_document PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void begin_anchor PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void end_anchor PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void begin_list PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void list_element PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void end_list PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void begin_glossary PARAMS((void*this, HTTag * t, HTElement * e));
PRIVATE void end_glossary PARAMS((void*this, HTTag * t, HTElement * e));

PRIVATE void actually_set_style PARAMS((HTML_id this));
PRIVATE void change_style PARAMS((HTML_id this, HTStyle * style));

/*	Style buffering avoids dummy paragraph begin/ends.
*/
#define UPDATE_STYLE if (THIS->style_change) { actually_set_style(THIS); }

#define THIS ((HTML_id)this)

/*	Things affecting the anchor but not the document itself
**	-------------------------------------------------------
*/


/*		TITLE
*/

/*	Accumulate a character of title
*/
static void accumulate_string ARGS2(void *, this, char, c)

{
    HTChunkPutc(&THIS->title, c);
}


/*		Clear the title
*/
PRIVATE  void clear_string ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    HTChunkClear(&THIS->title);
}

PRIVATE void set_title ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    HTChunkTerminate(&THIS->title);
    HTAnchor_setTitle(THIS->node_anchor, THIS->title.data);
}

PRIVATE void set_index ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    HTAnchor_setIndex(THIS->node_anchor);
}

/*			Things affecting the document
**			-----------------------------
*/
/*		Character handling
*/
PRIVATE void pass_character ARGS2(void *, this, char, c)
{
    if (THIS->style_change) {
        if ((c=='\n') || (c==' ')) return;	/* Ignore it */
        UPDATE_STYLE;
    }
    if (c=='\n') {
        if (THIS->in_word) {
	    HText_appendCharacter(THIS->text, ' ');
	    THIS->in_word = NO;
	}
    } else {
        HText_appendCharacter(THIS->text, c);
	THIS->in_word = YES;
    }
}

PRIVATE void litteral_text ARGS2(void *, this, char, c)
{
/*	We guarrantee that the style is up-to-date in begin_litteral
*/
    HText_appendCharacter(THIS->text, c);		/* @@@@@ */
}

PRIVATE void ignore_text ARGS2(void *, this, char, c)
{
    /* Do nothing */
}

PRIVATE void set_next_id  ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    /* Not needed */
}

PRIVATE void new_paragraph  ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    UPDATE_STYLE;
    HText_appendParagraph(THIS->text);
    THIS->in_word = NO;
}

PRIVATE void term ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    if (!THIS->style_change) {
        HText_appendParagraph(THIS->text);
	THIS->in_word = NO;
    }
}

PRIVATE void definition ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    UPDATE_STYLE;
    pass_character(this, '\t');	/* Just tab out one stop */
    THIS->in_word = NO;
}

/*		Our Static DTD for HTML
**		-----------------------
*/

static entity entities[] = {
	{ "lt",	"<" },
	{ "gt", ">" },
	{ "amp", "&" },
#ifdef NeXT
	{ "bullet" , "\267" },			/* @@@ NeXT only */
#endif
/* The following accented characters are from peter Flynn, curia project */

/* these ifdefs don't solve the problem of a simple terminal emulator
** with a different character set to the client machine. But nothing does,
** except looking at the TERM setting */

        { "ocus" , "&" },       /* for CURIA */
#ifdef IBMPC
        { "aacute" , "\240" },	/* For PC display */
        { "eacute" , "\202" },
        { "iacute" , "\241" },
        { "oacute" , "\242" },
        { "uacute" , "\243" },
        { "Aacute" , "\101" },
        { "Eacute" , "\220" },
        { "Iacute" , "\111" },
        { "Oacute" , "\117" },
        { "Uacute" , "\125" },
#else
        { "aacute" , "\341" },	/* Works for openwindows -- Peter Flynn */
        { "eacute" , "\351" },
        { "iacute" , "\355" },
        { "oacute" , "\363" },
        { "uacute" , "\372" },
        { "Aacute" , "\301" },
        { "Eacute" , "\310" },
        { "Iacute" , "\315" },
        { "Oacute" , "\323" },
        { "Uacute" , "\332" }, 
#endif
	{ 0,	0 }  /* Terminate list */
};

static attr no_attr[] = {{ 0, 0 , 0}};

static attr a_attr[] = {				/* Anchor attributes */
#define A_ID 0
	{ "NAME", 0, 0 },				/* Should be ID */
#define A_TYPE 1
	{ "TYPE", 0, 0 },
#define A_HREF 2
	{ "HREF", 0, 0 },
	{ 0, 0 , 0}	/* Terminate list */
};	
static attr list_attr[] = {
#define LIST_COMPACT 0
	{ "COMPACT", 0, 0 },
	{ 0, 0, 0 }	/* Terminate list */
};

static attr glossary_attr[] = {
#define GLOSSARY_COMPACT 0
	{ "COMPACT", 0, 0 },
	{ 0, 0, 0 }	/* Terminate list */
};

static HTTag default_tag =
    { "DOCUMENT", no_attr , 0, 0, begin_document, pass_character, end_document };
/*	NAME ATTR  STYLE LITERAL?  ON_BEGIN   ON__CHARACTER     ON_END
*/
static HTTag tags[] = {
#define TITLE_TAG 0
    { "TITLE", no_attr, 0, 0, clear_string, accumulate_string, set_title },
#define ISINDEX_TAG 1
    { "ISINDEX", no_attr, 0, 0, set_index, 0 , 0 },
#define NEXTID_TAG 2
    { "NEXTID", no_attr, 0, 0, set_next_id, 0, 0 },
#define ADDRESS_TAG 3
    { "ADDRESS"	, no_attr, 0, 0, begin_element, pass_character, end_element },
#define H1_TAG 4
    { "H1"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H2"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H3"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H4"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H5"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H6"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H7"	, no_attr, 0, 0, begin_element, pass_character, end_element },
#define UL_TAG 11
    { "UL"	, list_attr, 0, 0, begin_list, pass_character, end_list },
#define OL_TAG 12
    { "OL"	, list_attr, 0, 0, begin_list, pass_character, end_list },
#define MENU_TAG 13
    { "MENU"	, list_attr, 0, 0, begin_list, pass_character, end_list },
#define DIR_TAG 14
    { "DIR"	, list_attr, 0, 0, begin_list, pass_character, end_list },
#define LI_TAG 15
    { "LI"	, list_attr, 0, 0, list_element, pass_character, 0 },
#define DL_TAG 16
    { "DL"	, glossary_attr, 0, 0, begin_glossary, pass_character, end_glossary },
    { "DT"	, no_attr, 0, 0, term, pass_character, 0 },
    { "DD"	, no_attr, 0, 0, definition, pass_character, 0 },
    { "A"	, a_attr,  0, 0, begin_anchor, pass_character, end_anchor },
#define P_TAG 20
    { "P"	, no_attr, 0, 0, new_paragraph, pass_character, 0 },
#define XMP_TAG 21
  { "XMP"	, no_attr, 0, YES, begin_litteral, litteral_text, end_element },
#define PRE_TAG 22
  { "PRE"	, no_attr, 0, 0, begin_litteral, litteral_text, end_element },
#define LISTING_TAG 23
  { "LISTING"	, no_attr, 0, YES,begin_litteral, litteral_text, end_element },
#define PLAINTEXT_TAG 24
  { "PLAINTEXT", no_attr, 0, YES, begin_litteral, litteral_text, end_element },
#define COMMENT_TAG 25
    { "COMMENT", no_attr, 0, YES, no_change, ignore_text, no_change },
    { 0, 0, 0, 0,  0, 0 , 0}	/* Terminate list */
};

PUBLIC SGML_dtd HTML_dtd = { tags, &default_tag, entities };


/*		Flattening the style structure
**		------------------------------
**
On the NeXT, and on any read-only browser, it is simpler for the text to have
a sequence of styles, rather than a nested tree of styles. In this
case we have to flatten the structure as it arrives from SGML tags into
a sequence of styles.
*/

/*		If style really needs to be set, call this
*/
PRIVATE void actually_set_style ARGS1(HTML_id, this)
{
    if (!THIS->text) {			/* First time through */
	    THIS->text = HText_new(THIS->node_anchor);
	    HText_beginAppend(THIS->text);
	    HText_setStyle(THIS->text, THIS->new_style);
	    THIS->in_word = NO;
    } else {
	    HText_setStyle(THIS->text, THIS->new_style);
    }
    THIS->old_style = THIS->new_style;
    THIS->style_change = NO;
}

/*      If you THINK you need to change style, call this
*/

PRIVATE void change_style ARGS2(HTML_id, this, HTStyle *,style)
{
    if (THIS->new_style!=style) {
    	THIS->style_change = YES /* was old_style == new_style */ ;
	THIS->new_style = style;
    }
}

/*	Anchor handling
**	---------------
*/
PRIVATE void begin_anchor ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    HTChildAnchor * source = HTAnchor_findChildAndLink(
    	THIS->node_anchor,						/* parent */
	a_attr[A_ID].present	? a_attr[A_ID].value : 0,	/* Tag */
	a_attr[A_HREF].present	? a_attr[A_HREF].value : 0,	/* Addresss */
	a_attr[A_TYPE].present	? 
		(HTLinkType*)HTAtom_for(a_attr[A_TYPE].value)
		 : 0);
    
    UPDATE_STYLE;
    HText_beginAnchor(THIS->text, source);
}

PRIVATE void end_anchor ARGS3(void *, this, HTTag *,	 t,
			HTElement *,	e)
{
    UPDATE_STYLE;
    HText_endAnchor(THIS->text);
}


/*	General SGML Element Handling
**	-----------------------------
*/
PRIVATE void begin_element ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    change_style(THIS, (HTStyle*)(t->style));
}
PRIVATE void no_change ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    /* Do nothing */;
}
PRIVATE void begin_litteral ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    change_style(THIS, t->style);
    UPDATE_STYLE;
}
/*		End Element
**
**	When we end an element, the style must be returned to that
**	in effect before that element.  Note that anchors (etc?)
**	don't have an associated style, so that we must scan down the
**	stack for an element with a defined style. (In fact, the styles
**	should be linked to the whole stack not just the top one.)
**	TBL 921119
*/
PRIVATE void end_element ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
 /*   if (e) change_style(THIS, e->tag->style); */
    while (e) {
	if (e->tag->style) {
		change_style(THIS, e->tag->style);
		return;
	}
	e = e->next;
    }
}

/*			Lists
*/
PRIVATE void begin_list ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    change_style(THIS, list_attr[LIST_COMPACT].present
    		? list_compact_style
		: (HTStyle*)(t->style));
    THIS->in_word = NO;
}

PRIVATE void end_list ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    change_style(THIS, e->tag->style);
    THIS->in_word = NO;
}

PRIVATE void list_element  ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    UPDATE_STYLE;
    if (e->tag != &tags[DIR_TAG])
	HText_appendParagraph(THIS->text);
    else
        HText_appendCharacter(THIS->text, '\t');	/* Tab @@ nl for UL? */
    THIS->in_word = NO;
}


PRIVATE void begin_glossary ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    change_style(THIS, glossary_attr[GLOSSARY_COMPACT].present
    		? glossary_compact_style
		: glossary_style);
    THIS->in_word = NO;
}

PRIVATE void end_glossary ARGS3(void *, this, HTTag *,t, HTElement *,e)
{
    change_style(THIS, e->tag->style);
    THIS->in_word = NO;
}


/*	Create an HTML object
**	---------------------
*/
PUBLIC HTML_id HTML_new ARGS1(HTParentAnchor *,anchor)
{

    HTML_id this = malloc(sizeof(*this));

    if (!got_styles) get_styles();

    this->node_anchor = anchor;
    this->title.size = 0;
    this->title.growby = 128;
    this->title.allocated = 0;
    this->title.data = 0;
    this->text = 0;
    this->style_change = YES; /* Force check leading to text creation */
    this->new_style = this->old_style = 0;
    
    this->context = SGML_begin(&HTML_dtd);
    SGML_setCallerData(this->context, this);
    
    return this;
}


/*	Free an HTML object
**	-------------------
**
**	Note that the SGML parsing context is freed, but the created object is not,
**	as it takes on an existence of its own unless explicitly freed.
*/
PUBLIC void HTML_free ARGS1(HTML_id, this)
{
    SGML_end(this->context);
    free(this);
}

PUBLIC HTSGMLContext HTML_SGMLContext ARGS1(HTML_id, this)
{
    return this->context;
}

PRIVATE void begin_document ARGS3(void *, this, HTTag *, t, HTElement *, e)
{
    /* Can't do much, THIS is undefined here */
}

PRIVATE void end_document ARGS3(void *, this, HTTag *, t, HTElement *, e)
/* If the document is empty, the text object will not yet exist.
   So we could in fact abandon creating the document and return
   an error code.  In fact an empty document is an important type
   of document, so we don't.
*/
{
    UPDATE_STYLE;		/* Create empty document here! */
    HText_endAppend(THIS->text);

}

/*	Get Styles from style sheet
**	---------------------------
*/
PRIVATE void get_styles NOARGS
{
    got_styles = YES;
    
    tags[P_TAG].style =
    default_tag.style =		HTStyleNamed(styleSheet, "Normal");
    tags[H1_TAG].style =	HTStyleNamed(styleSheet, "Heading1");
    tags[H1_TAG+1].style =	HTStyleNamed(styleSheet, "Heading2");
    tags[H1_TAG+2].style =	HTStyleNamed(styleSheet, "Heading3");
    tags[H1_TAG+3].style =	HTStyleNamed(styleSheet, "Heading4");
    tags[H1_TAG+4].style =	HTStyleNamed(styleSheet, "Heading5");
    tags[H1_TAG+5].style =	HTStyleNamed(styleSheet, "Heading6");
    tags[H1_TAG+6].style =	HTStyleNamed(styleSheet, "Heading7");
    tags[DL_TAG].style =	HTStyleNamed(styleSheet, "Glossary");
    tags[UL_TAG].style =	HTStyleNamed(styleSheet, "List");
    tags[OL_TAG].style =	HTStyleNamed(styleSheet, "List");
    tags[MENU_TAG].style =	HTStyleNamed(styleSheet, "Menu");
    list_compact_style =
    tags[DIR_TAG].style =	HTStyleNamed(styleSheet, "Dir");    
    glossary_style =		HTStyleNamed(styleSheet, "Glossary");
    glossary_compact_style =	HTStyleNamed(styleSheet, "GlossaryCompact");
    tags[ADDRESS_TAG].style=	HTStyleNamed(styleSheet, "Address");
    tags[PLAINTEXT_TAG].style =
    tags[XMP_TAG].style =	HTStyleNamed(styleSheet, "Example");
    tags[PRE_TAG].style =	HTStyleNamed(styleSheet, "Preformatted");
    tags[LISTING_TAG].style =	HTStyleNamed(styleSheet, "Listing");
}


/*	Parse an HTML file
**	------------------
**
**	This version takes a pointer to the routine to call
**	to get each character.
*/
BOOL HTML_Parse
#ifdef __STDC__
  (HTParentAnchor * anchor, char (*next_char)() )
#else
  (anchor, next_char)
    HTParentAnchor * anchor;
    char (*next_char)();
#endif
{
	HTSGMLContext context;
        HTML_id this = HTML_new(anchor);
	context = SGML_begin(&HTML_dtd);
	SGML_setCallerData(context, this);
	for(;;) {
	    char character;
	    character = (*next_char)();
	    if (character == (char)EOF) break;
    
	    SGML_character(context, character);           
         }
	SGML_end(context);
	free(this);
	return YES;
}
