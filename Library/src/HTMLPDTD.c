/*								     HTMLPDTD.c
**	STATIC DTD FOR HTML
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	 6 Nov 93	MD	Increased size of img_attr array to make space
**  				for terminator.
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTMLPDTD.h"

/* 	Entity Names
**	------------
**
**	This table must be matched exactly with ALL the translation tables
*/

PRIVATE CONST char* entities[HTML_ENTITIES] = {
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



/*		Attribute Lists
**		---------------
**
**	Lists must be in alphatbetical order by attribute name
**	The tag elements contain the number of attributes
*/
PRIVATE attr no_attr[1] = 
	{{ 0 }};

PRIVATE attr a_attr[HTML_A_ATTRIBUTES+1] = {		/* Anchor attributes */
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

PRIVATE attr base_attr[] = {		/* BASE attributes */
	{ "HREF"},
	{ 0 }	/* Terminate list */
};	


PRIVATE attr changed_attr[] = {
	{ "ID" },
	{ "IDREF" },
	{ 0 }	/* terminate list */
};

PRIVATE attr fig_attr[] = {		/* Figures */
	{ "ALIGN" },
	{ "ID" },
	{ "INDEX" },
	{ "ISMAP" },				/* @@@ NO! */
	{ "LANG" },
	{ "SRC" },
	{ 0 }	/* terminate list */
};

PRIVATE attr form_attr[] = {		/* General, for many things */
	{ "ACTION" },
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ "METHOD" },
	{ 0 }	/* terminate list */
};

PRIVATE attr gen_attr[] = {		/* General, for many things */
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ 0 }	/* terminate list */
};

PRIVATE attr htmlplus_attr[] = {		/* wrapper HTMLPLUS */
	{ "FORMS" },
	{ "VERSION" },
	{ 0 }				/* terminate list */
};

PRIVATE attr id_attr[2] = {
	{ "ID" },
	{ 0 }				/* terminate list */
};

PRIVATE attr image_attr[HTML_IMAGE_ATTRIBUTES+1] = {	/* Image attributes */
	{ "ALIGN" },
	{ "ISMAP"},			/* Use HTTP SpaceJump instead */
	{ "LANG" },
	{ "SEETHRU"},
	{ "SRC"},
	{ 0 }	/* Terminate list */
};	

PRIVATE attr img_attr[HTML_IMG_ATTRIBUTES+1] = {	/* IMG attributes */
	{ "ALIGN" },
	{ "ALT" },
	{ "ISMAP"},			/* Use HTTP SpaceJump instead */
	{ "SEETHRU"},
	{ "SRC"},
	{ 0 }	/* Terminate list */
};	

PRIVATE attr input_attr[HTML_INPUT_ATTRIBUTES+1] = {
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

PRIVATE attr l_attr[] = {
	{ "ALIGN"},
	{ "ID" },
	{ "LANG" },
	{ "INDEX" },
	{ 0 }	/* Terminate list */
};

PRIVATE attr li_attr[] = {
	{ "ID" },
	{ "LANG" },
	{ "INDEX" },
	{ "SRC"},
	{ 0 }	/* Terminate list */
};

PRIVATE attr link_attr[HTML_LINK_ATTRIBUTES+1] = {	/* link attributes */
	{ "HREF"},
	{ "IDREF" },
	{ "METHODS" },
	{ "REL" },			/* Relationship */
	{ "REV" },			/* Reverse relationship */
	{ 0 }	/* Terminate list */
};	

PRIVATE attr list_attr[] = {
	{ "COMPACT"},
	{ "ID" },
	{ "LANG" },
	{ "INDEX" },
	{ 0 }	/* Terminate list */
};

PRIVATE attr glossary_attr[HTML_DL_ATTRIBUTES+1] = {
	{ "ID" },
	{ "COMPACT " },
	{ "INDEX" },
	{ 0 }	/* Terminate list */
};

PRIVATE attr nextid_attr[HTML_NEXTID_ATTRIBUTES+1] = {
	{ "N" },
	{ 0 }	/* Terminate list */
};

PRIVATE attr note_attr[HTML_NOTE_ATTRIBUTES+1] = {	/* Footnotes etc etc */
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ "ROLE" },
	{ 0 }	/* terminate list */
};

PRIVATE attr option_attr[HTML_OPTION_ATTRIBUTES+1] = {
	{ "DISABLED" },
	{ "LANG" },
	{ "SELECTED" },
	{ 0 }
};

PRIVATE attr render_attr[HTML_RENDER_ATTRIBUTES+1] = {
	{ "STYLE" },
	{ "TAG" },
	{ 0 }	/* Terminate list */
};

PRIVATE attr select_attr[HTML_SELECT_ATTRIBUTES+1] = {
	{ "ERROR" },
	{ "LANG" },
	{ "MULTIPLE" },			/* WSM bug fix, was SEVERAL */
	{ "NAME" },
	{ "SIZE" },			/* WSM bug fix, missing */
	{ 0 },
};

PRIVATE attr tab_attr[HTML_TAB_ATTRIBUTES+1] = {
	{ "ALIGN" },
	{ "AT" },
	{ 0 }
};

PRIVATE attr table_attr[HTML_TABLE_ATTRIBUTES+1] = {
	{ "BORDER" },
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ 0 }
};

PRIVATE attr td_attr[HTML_TD_ATTRIBUTES+1] = {
	{ "ALIGN" },
	{ "COLSPAN" },
	{ "ROWSPAN" },
	{ "TAG" },
	{ 0 }
};

PRIVATE attr textarea_attr[HTML_TEXTAREA_ATTRIBUTES+1] = {
	{ "COLS" },
	{ "DISABLED" },
	{ "ERROR" },
	{ "LANG" },
	{ "NAME" },
	{ "ROWS" },
	{ 0 }
};

PRIVATE attr ul_attr[HTML_UL_ATTRIBUTES+1] = {
	{ "COMPACT" },
	{ "ID" },
	{ "INDEX" },
	{ "LANG" },
	{ "PLAIN" },
	{ "WRAP" },
	{ 0 }
};


/*	Elements
**	--------
**
**	Must match definitions in HTMLPDTD.html!
**	Must be in alphabetical order.
**
**	HTML is included to allow HTML documents to be parsed as a subset.
**	TITLE is changed to type SGML_MIXED so it get parsed as well
**	Henrik 08/03-94
**
**    Name, 	Attributes, 		content
*/
PRIVATE HTTag tags[HTMLP_ELEMENTS] = {
    { "A"	, a_attr,	HTML_A_ATTRIBUTES,	SGML_MIXED },
    { "ABBREV"  , gen_attr,     HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "ABSTRACT" , gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "ACRONYM"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "ADDED"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "ADDRESS"	, no_attr,	0,			SGML_MIXED },
    { "ARG"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "B"	, no_attr,	0,			SGML_MIXED },
    { "BASE"	, base_attr,	HTML_BASE_ATTRIBUTES,	SGML_MIXED },
    { "BLOCKQUOTE", no_attr,	0,			SGML_MIXED },
    { "BODY"	, no_attr,	0,			SGML_MIXED },
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
    { "NEXTID"  , nextid_attr,	1,			SGML_EMPTY },
    { "NOTE"	, note_attr,	HTML_NOTE_ATTRIBUTES,	SGML_EMPTY },
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


PUBLIC CONST SGML_dtd HTMLP_dtd = {
    tags,
    HTMLP_ELEMENTS,
    entities,
    sizeof(entities)/sizeof(char**)
};

/*	Utility Routine: useful for people building HTML objects */

/*	Start anchor element
**	--------------------
**
**	It is kinda convenient to have a particulr routine for
**	starting an anchor element, as everything else for HTML is
**	simple anyway.
*/
struct _HTStructured {
    HTStructuredClass * isa;
	/* ... */
};

PUBLIC void HTStartAnchor (HTStructured * obj,
		CONST char *  name,
		CONST char *  href)
{
    BOOL		present[HTML_A_ATTRIBUTES];
    CONST char*		value[HTML_A_ATTRIBUTES];
    
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

/*	Utility Routine: useful for people building HTML objects */

/*	Put image element
**	--------------------
**
**	Hopefully as usefull as HTStartAnchor. Henrik 23/03-94
*/
PUBLIC void HTMLPutImg (HTStructured * obj,
			     CONST char * src,
			     CONST char * alt,
			     CONST char * align)
{
    BOOL		present[HTML_IMG_ATTRIBUTES];
    CONST char*		value[HTML_IMG_ATTRIBUTES];
    
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
		CONST char *	next_one)
{
    BOOL		present[HTML_NEXTID_ATTRIBUTES];
    CONST char*		value[HTML_NEXTID_ATTRIBUTES];
    {
    	int i;
    	for(i=0; i<HTML_NEXTID_ATTRIBUTES; i++)
	    present[i] = NO;
    }
    present[HTML_NEXTID_N] = YES;
    value[HTML_NEXTID_N] = next_one;
    
    (*obj->isa->start_element)(obj, HTML_NEXTID , present, value);

}
