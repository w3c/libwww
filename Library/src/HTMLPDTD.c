/*								     HTMLPDTD.c
**	STATIC DTD FOR HTML (with frame support)
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	 6 Nov 93	MD	Increased size of img_attr array to make space
**  				for terminator.
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTMLPDTD.h"

struct _HTStructured {
    HTStructuredClass * isa;
	/* ... */
};

/*
**	ENTITY NAMES
**	This table must be matched exactly with ALL the translation tables
*/
PRIVATE const char* entities[HTML_ENTITIES] = {
  "AElig",	/* capital AE diphthong (ligature) */ 
  "Aacute",	/* capital A, acute accent */ 
  "Acirc",	/* capital A, circumflex accent */ 
  "Agrave",	/* capital A, grave accent */ 
  "Aring",	/* capital A, ring */ 
  "Atilde",	/* capital A, tilde */ 
  "Auml",	/* capital A, dieresis or umlaut mark */ 
  "Ccedil",	/* capital C, cedilla */ 
  "ETH",	/* capital Eth, Icelandic */ 
  "Eacute",	/* capital E, acute accent */ 
  "Ecirc",	/* capital E, circumflex accent */ 
  "Egrave",	/* capital E, grave accent */ 
  "Euml",	/* capital E, dieresis or umlaut mark */ 
  "Iacute",	/* capital I, acute accent */ 
  "Icirc",	/* capital I, circumflex accent */ 
  "Igrave",	/* capital I, grave accent */ 
  "Iuml",	/* capital I, dieresis or umlaut mark */ 
  "Ntilde",	/* capital N, tilde */ 
  "Oacute",	/* capital O, acute accent */ 
  "Ocirc",	/* capital O, circumflex accent */ 
  "Ograve",	/* capital O, grave accent */ 
  "Oslash",	/* capital O, slash */ 
  "Otilde",	/* capital O, tilde */ 
  "Ouml",	/* capital O, dieresis or umlaut mark */ 
  "THORN",	/* capital THORN, Icelandic */ 
  "Uacute",	/* capital U, acute accent */ 
  "Ucirc",	/* capital U, circumflex accent */ 
  "Ugrave",	/* capital U, grave accent */ 
  "Uuml",	/* capital U, dieresis or umlaut mark */ 
  "Yacute",	/* capital Y, acute accent */ 
  "aacute",	/* small a, acute accent */ 
  "acirc",	/* small a, circumflex accent */ 
  "aelig",	/* small ae diphthong (ligature) */ 
  "agrave",	/* small a, grave accent */ 
  "amp",	/* ampersand */ 
  "aring",	/* small a, ring */ 
  "atilde",	/* small a, tilde */ 
  "auml",	/* small a, dieresis or umlaut mark */ 
  "ccedil",	/* small c, cedilla */ 
  "eacute",	/* small e, acute accent */ 
  "ecirc",	/* small e, circumflex accent */ 
  "egrave",	/* small e, grave accent */ 
  "eth",	/* small eth, Icelandic */ 
  "euml",	/* small e, dieresis or umlaut mark */ 
  "gt",		/* greater than */ 
  "iacute",	/* small i, acute accent */ 
  "icirc",	/* small i, circumflex accent */ 
  "igrave",	/* small i, grave accent */ 
  "iuml",	/* small i, dieresis or umlaut mark */ 
  "lt",		/* less than */ 
  "nbsp",	/* non breaking space */
  "ntilde",	/* small n, tilde */ 
  "oacute",	/* small o, acute accent */ 
  "ocirc",	/* small o, circumflex accent */ 
  "ograve",	/* small o, grave accent */ 
  "oslash",	/* small o, slash */ 
  "otilde",	/* small o, tilde */ 
  "ouml",	/* small o, dieresis or umlaut mark */
  "quot",	/* double quote sign  - June 1994 */ 
  "szlig",	/* small sharp s, German (sz ligature) */ 
  "thorn",	/* small thorn, Icelandic */ 
  "uacute",	/* small u, acute accent */ 
  "ucirc",	/* small u, circumflex accent */ 
  "ugrave",	/* small u, grave accent */ 
  "uuml",	/* small u, dieresis or umlaut mark */ 
  "yacute",	/* small y, acute accent */ 
  "yuml",	/* small y, dieresis or umlaut mark */ 
};

/*
**	ATTRIBUTE LISTS
*/

PRIVATE HTAttr no_attr[1] = 
	{{ 0 }};

PRIVATE HTAttr body_attr[HTML_BODY_ATTRIBUTES+1] = {	/* to catch images */
	{ "BACKGROUND" },
	{ 0 }	/* Terminate list */
};	

PRIVATE HTAttr frame_attr[HTML_FRAME_ATTRIBUTES+1] = {	/* frame attributes */
	{ "SRC" },
	{ 0 }	/* Terminate list */
};

PRIVATE HTAttr frameset_attr[HTML_FRAMESET_ATTRIBUTES+1] = { /* frameset attributes */

	{ "BORDER" },
	{ "BORDERCOLOR" },
	{ "COLS" },
	{ "FRAMEBORDER" },
	{ "ONBLUR" },
	{ "ONFOCUS" },
	{ "ONLOAD" },
	{ "ONUNLOAD" },
	{ "ROWS" },	
	{ 0 }	/* Terminate list */
};

PRIVATE HTAttr a_attr[HTML_A_ATTRIBUTES+1] = {		/* Anchor attributes */
	{ "EFFECT" },
	{ "HREF"},
	{ "ID" },
	{ "METHODS" },
	{ "NAME" },			/* Should be ID */
	{ "PRINT" },
	{ "REL" },			/* Relationship */
	{ "REV" },			/* Reverse relationship */
	{ "SHAPE" },
	{ "TITLE" },
	{ 0 }	/* Terminate list */
};	

PRIVATE HTAttr area_attr[HTML_AREA_ATTRIBUTES+1] = {		/* Area attributes */
	{ "ALT" },
	{ "ACCESSKEY"},
	{ "COORDS" },
	{ "HREF" },
	{ "NOHREF" },
	{ "ONBLUR" },
	{ "ONFOCUS" },
	{ "SHAPE" },
	{ "TABINDEX" },
	{ 0 }	/* Terminate list */
};	

PRIVATE HTAttr base_attr[HTML_BASE_ATTRIBUTES+1] = {	/* BASE attributes */
	{ "HREF"},
	{ 0 }	/* Terminate list */
};	


PRIVATE HTAttr changed_attr[] = {
	{ "ID" },
	{ "IDREF" },
	{ 0 }	/* terminate list */
};

PRIVATE HTAttr fig_attr[] = {		/* Figures */
	{ "ALIGN" },
	{ "ID" },
	{ "INDEX" },
	{ "ISMAP" },				/* @@@ NO! */
	{ "LANG" },
	{ "SRC" },
	{ 0 }	/* terminate list */
};

PRIVATE HTAttr form_attr[] = {		/* General, for many things */
	{ "ACTION" },
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ "METHOD" },
	{ 0 }	/* terminate list */
};

PRIVATE HTAttr gen_attr[] = {		/* General, for many things */
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ 0 }	/* terminate list */
};

PRIVATE HTAttr htmlplus_attr[] = {		/* wrapper HTMLPLUS */
	{ "FORMS" },
	{ "VERSION" },
	{ 0 }				/* terminate list */
};

PRIVATE HTAttr id_attr[2] = {
	{ "ID" },
	{ 0 }				/* terminate list */
};

PRIVATE HTAttr image_attr[HTML_IMAGE_ATTRIBUTES+1] = {	/* Image attributes */
	{ "ALIGN" },
	{ "ISMAP"},			/* Use HTTP SpaceJump instead */
	{ "LANG" },
	{ "SEETHRU"},
	{ "SRC"},
	{ 0 }	/* Terminate list */
};	

PRIVATE HTAttr img_attr[HTML_IMG_ATTRIBUTES+1] = {	/* IMG attributes */
	{ "ALIGN" },
	{ "ALT" },
	{ "ISMAP"},			/* Use HTTP SpaceJump instead */
	{ "LOWSRC"},
	{ "SEETHRU"},
	{ "SRC"},
	{ 0 }	/* Terminate list */
};	

PRIVATE HTAttr input_attr[HTML_INPUT_ATTRIBUTES+1] = {
	{ "ALIGN" },
	{ "CHECKED" },
	{ "DISABLED" },
	{ "ERROR" },
	{ "MAXLENGTH" },		/* WSM bug fix, was MAX */
	{ "MIN" },
	{ "NAME" }, 
	{ "SIZE" },
	{ "SRC" },
	{ "TYPE" },
	{ "VALUE" },
	{ "0" }
};

PRIVATE HTAttr l_attr[] = {
	{ "ALIGN"},
	{ "ID" },
	{ "LANG" },
	{ "INDEX" },
	{ 0 }	/* Terminate list */
};

PRIVATE HTAttr li_attr[] = {
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ "SRC"},
	{ 0 }	/* Terminate list */
};

PRIVATE HTAttr link_attr[HTML_LINK_ATTRIBUTES+1] = {	/* link attributes */
    { "CHARSET"},
    { "HREF"},
    { "HREFLANG"},
    { "MEDIA" },
    { "REL" },		/* Relationship */
    { "REV" },		/* Reverse relationship */
    { "TYPE" },
    { 0 }		/* Terminate list */
};	

PRIVATE HTAttr object_attr[HTML_OBJECT_ATTRIBUTES+1] = {	/* object attributes */
    { "ARCHIVE" },
    { "CLASSID" },
    { "CODEBASE" },
    { "CODETYPE" },
    { "DATA" },
    { "DECLARE" },
    { "HIGHT" },
    { "NAME" },
    { "STANDBY" },
    { "TABINDEX" },
    { "TYPE" },
    { "USEMAP" },
    { "WIDTH" },
    { 0 }
};	

PRIVATE HTAttr list_attr[] = {
	{ "COMPACT"},
	{ "ID" },
	{ "LANG" },
	{ "INDEX" },
	{ 0 }	/* Terminate list */
};

PRIVATE HTAttr glossary_attr[HTML_DL_ATTRIBUTES+1] = {
	{ "ID" },
	{ "COMPACT " },
	{ "INDEX" },
	{ 0 }	/* Terminate list */
};

PRIVATE HTAttr meta_attr[HTML_META_ATTRIBUTES+1] = {
    { "CONTENT"},
    { "HTTP-EQUIV"},
    { "NAME"},
    { "SCHEME" },
    { 0 }		/* Terminate list */
};	

PRIVATE HTAttr nextid_attr[HTML_NEXTID_ATTRIBUTES+1] = {
	{ "N" },
	{ 0 }	/* Terminate list */
};

PRIVATE HTAttr note_attr[HTML_NOTE_ATTRIBUTES+1] = {	/* Footnotes etc etc */
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ "ROLE" },
	{ 0 }	/* terminate list */
};

PRIVATE HTAttr option_attr[HTML_OPTION_ATTRIBUTES+1] = {
	{ "DISABLED" },
	{ "LANG" },
	{ "SELECTED" },
	{ 0 }
};

PRIVATE HTAttr render_attr[HTML_RENDER_ATTRIBUTES+1] = {
	{ "STYLE" },
	{ "TAG" },
	{ 0 }	/* Terminate list */
};

PRIVATE HTAttr select_attr[HTML_SELECT_ATTRIBUTES+1] = {
	{ "ERROR" },
	{ "LANG" },
	{ "MULTIPLE" },			/* WSM bug fix, was SEVERAL */
	{ "NAME" },
	{ "SIZE" },			/* WSM bug fix, missing */
	{ 0 },
};

PRIVATE HTAttr tab_attr[HTML_TAB_ATTRIBUTES+1] = {
	{ "ALIGN" },
	{ "AT" },
	{ 0 }
};

PRIVATE HTAttr table_attr[HTML_TABLE_ATTRIBUTES+1] = {
	{ "BORDER" },
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ 0 }
};

PRIVATE HTAttr td_attr[HTML_TD_ATTRIBUTES+1] = {
	{ "ALIGN" },
	{ "COLSPAN" },
	{ "ROWSPAN" },
	{ "TAG" },
	{ 0 }
};

PRIVATE HTAttr textarea_attr[HTML_TEXTAREA_ATTRIBUTES+1] = {
	{ "COLS" },
	{ "DISABLED" },
	{ "ERROR" },
	{ "LANG" },
	{ "NAME" },
	{ "ROWS" },
	{ 0 }
};

PRIVATE HTAttr ul_attr[HTML_UL_ATTRIBUTES+1] = {
	{ "COMPACT" },
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ "PLAIN" },
	{ "WRAP" },
	{ 0 }
};

/*
**	ELEMENTS
**	Must match definitions in HTMLPDTD.html!
**	Must be in alphabetical order.
**
**    Name, 	Attributes, 		content
*/
PRIVATE HTTag tags[HTML_ELEMENTS] = {
    { "A"	, a_attr,	HTML_A_ATTRIBUTES,	SGML_MIXED },
    { "ABBREV"  , gen_attr,     HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "ABSTRACT" , gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "ACRONYM"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "ADDED"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "ADDRESS"	, no_attr,	0,			SGML_MIXED },
    { "AREA"	, area_attr,	HTML_AREA_ATTRIBUTES,	SGML_EMPTY },
    { "ARG"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "B"	, no_attr,	0,			SGML_MIXED },
    { "BASE"	, base_attr,	HTML_BASE_ATTRIBUTES,	SGML_EMPTY },
    { "BLOCKQUOTE", no_attr,	0,			SGML_MIXED },
    { "BODY"	, body_attr,	HTML_BODY_ATTRIBUTES,	SGML_MIXED },
    { "BOX"	, no_attr,	0,			SGML_MIXED },
    { "BR"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_EMPTY },
    { "BYLINE"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "CAPTION"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "CHANGED"	, changed_attr, HTML_CHANGED_ATTRIBUTES, SGML_MIXED },
    { "CITE"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "CMD"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "CODE"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "COMMENT"	, no_attr,	0,			SGML_MIXED },
    { "DD"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_EMPTY },
    { "DFN"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "DIR"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "DL"	, glossary_attr,HTML_DL_ATTRIBUTES,	SGML_MIXED },
    { "DT"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_EMPTY },
    { "EM"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "FIG" 	, fig_attr,	HTML_FIG_ATTRIBUTES,	SGML_MIXED },
    { "FOOTNOTE" , gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "FORM" 	, form_attr,	HTML_FORM_ATTRIBUTES,	SGML_MIXED },
    { "FRAME"	, frame_attr,	HTML_FRAME_ATTRIBUTES,	SGML_EMPTY },
    { "FRAMESET", frameset_attr,HTML_FRAMESET_ATTRIBUTES, SGML_EMPTY },
    { "H1"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "H2"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "H3"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "H4"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "H5"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "H6"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "H7"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "HEAD"	, no_attr,	0,			SGML_MIXED },
    { "HR"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_EMPTY },
    { "HTML"	, no_attr,	0,			SGML_MIXED },  /* */
    { "HTMLPLUS", htmlplus_attr,HTML_HTMLPLUS_ATTRIBUTES, SGML_MIXED },
    { "I"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "IMAGE"	, image_attr,	HTML_IMAGE_ATTRIBUTES,	SGML_EMPTY },
    { "IMG"     , img_attr,	HTML_IMG_ATTRIBUTES,	SGML_EMPTY },
    { "INPUT"	, input_attr,	HTML_INPUT_ATTRIBUTES,	SGML_EMPTY },
    { "ISINDEX" , no_attr,	0,			SGML_EMPTY },
    { "KBD"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "L"	, l_attr,	HTML_L_ATTRIBUTES,	SGML_MIXED },
    { "LI"	, li_attr,	HTML_LI_ATTRIBUTES,	SGML_EMPTY },
    { "LINK"	, link_attr,	HTML_LINK_ATTRIBUTES,	SGML_EMPTY },
    { "LISTING"	, no_attr,	0,			SGML_LITERAL },
    { "LIT"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "MARGIN"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "MATH"	, id_attr,	HTML_ID_ATTRIBUTE,	SGML_MIXED },
    { "MENU"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "META"	, meta_attr,	HTML_META_ATTRIBUTES,	SGML_EMPTY },
    { "NEXTID"  , nextid_attr,	1,			SGML_EMPTY },
    { "NOTE"	, note_attr,	HTML_NOTE_ATTRIBUTES,	SGML_EMPTY },
    { "NOFRAMES", no_attr,	0,			SGML_MIXED },
    { "OBJECT"	, object_attr,	HTML_OBJECT_ATTRIBUTES,	SGML_MIXED },
    { "OL"	, list_attr,	HTML_LIST_ATTRIBUTES,	SGML_MIXED },
    { "OPTION"	, option_attr,	HTML_OPTION_ATTRIBUTES,	SGML_EMPTY },/*Mixed?*/
    { "OVER"	, no_attr,	0,			SGML_MIXED },
    { "P"	, l_attr,	HTML_L_ATTRIBUTES,	SGML_EMPTY },
    { "PERSON"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "PLAINTEXT", no_attr,	0,			SGML_LITERAL },
    { "PRE"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "Q"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "QUOTE"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "RENDER"	, render_attr,	HTML_RENDER_ATTRIBUTES,	SGML_MIXED },
    { "REMOVED"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "S"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "SAMP"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "SELECT"	, select_attr,	HTML_SELECT_ATTRIBUTES,	SGML_MIXED },
    { "STRONG"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "SUB"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "SUP"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "TAB"	, tab_attr,	HTML_TAB_ATTRIBUTES,	SGML_EMPTY },
    { "TABLE"	, table_attr,	HTML_TABLE_ATTRIBUTES,	SGML_MIXED },
    { "TD"	, td_attr,	HTML_TABLE_ATTRIBUTES,	SGML_MIXED },
    { "TEXTAREA", textarea_attr,HTML_TEXTAREA_ATTRIBUTES,SGML_MIXED},
    { "TH"	, td_attr,	HTML_TD_ATTRIBUTES,	SGML_MIXED },
    { "TITLE"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "TR"	, id_attr,	HTML_ID_ATTRIBUTE,	SGML_MIXED },
    { "TT"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "U"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "UL"	, ul_attr,	HTML_UL_ATTRIBUTES,	SGML_MIXED },
    { "VAR"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "XMP"	, no_attr,	0,			SGML_LITERAL }
};

PRIVATE SGML_dtd HTMLP_dtd = {
    tags,
    HTML_ELEMENTS,
    entities,
    sizeof(entities)/sizeof(char**)
};

PRIVATE SGML_dtd * DTD = &HTMLP_dtd;

PUBLIC SGML_dtd * HTML_dtd (void)
{
    return DTD;
}

PUBLIC BOOL HTML_setDtd (const SGML_dtd * dtd)
{
    if (dtd) {
	DTD = (SGML_dtd *) dtd;
	return YES;
    }
    return NO;
}

/*	Start anchor element
**	--------------------
**
**	It is kinda convenient to have a particulr routine for
**	starting an anchor element, as everything else for HTML is
**	simple anyway.
*/
PUBLIC void HTStartAnchor (HTStructured * obj,
		const char *  name,
		const char *  href)
{
    BOOL		present[HTML_A_ATTRIBUTES];
    const char*		value[HTML_A_ATTRIBUTES];
    
    {
    	int i;
    	for(i=0; i<HTML_A_ATTRIBUTES; i++)
	    present[i] = NO;
    }
    if (name) {
    	present[HTML_A_NAME] = YES;
	value[HTML_A_NAME] = name;
    }
    if (href) {
        present[HTML_A_HREF] = YES;
        value[HTML_A_HREF] = href;
    }
    
    (*obj->isa->start_element)(obj, HTML_A , present, value);

}

/*	Put image element
**	--------------------
**
**	Hopefully as usefull as HTStartAnchor. Henrik 23/03-94
*/
PUBLIC void HTMLPutImg (HTStructured * obj,
			     const char * src,
			     const char * alt,
			     const char * align)
{
    BOOL		present[HTML_IMG_ATTRIBUTES];
    const char*		value[HTML_IMG_ATTRIBUTES];
    
    {
    	int i;
    	for(i=0; i<HTML_IMG_ATTRIBUTES; i++)
	    present[i] = NO;
    }
    if (src) {
    	present[HTML_IMG_SRC] = YES;
	value[HTML_IMG_SRC] = src;
    }
    if (alt) {
        present[HTML_IMG_ALT] = YES;
        value[HTML_IMG_ALT] = alt;
    }
    if (align) {
        present[HTML_IMG_ALIGN] = YES;
        value[HTML_IMG_ALIGN] = align;
    }
    (*obj->isa->start_element)(obj, HTML_IMG , present, value);
    /* (*obj->isa->end_element)(obj, HTML_IMG); */
}

PUBLIC void HTNextID (HTStructured * obj,
		const char *	next_one)
{
    BOOL		present[HTML_NEXTID_ATTRIBUTES];
    const char*		value[HTML_NEXTID_ATTRIBUTES];
    {
    	int i;
    	for(i=0; i<HTML_NEXTID_ATTRIBUTES; i++)
	    present[i] = NO;
    }
    present[HTML_NEXTID_N] = YES;
    value[HTML_NEXTID_N] = next_one;
    
    (*obj->isa->start_element)(obj, HTML_NEXTID , present, value);

}
