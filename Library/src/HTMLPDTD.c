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
/* 00 */
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
/* 10 */
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
/* 20 */
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
/* 30 */
  "aacute",	/* small a, acute accent */ 
  "acirc",	/* small a, circumflex accent */ 
  "acute",	/* acute accent */
  "aelig",	/* small ae diphthong (ligature) */ 
  "agrave",	/* small a, grave accent */ 
  "amp",	/* ampersand */ 
  "aring",	/* small a, ring */ 
  "atilde",	/* small a, tilde */ 
  "auml",	/* small a, dieresis or umlaut mark */ 
  "brvbar",	/* broken vertical bar */
/* 40 */
  "ccedil",	/* small c, cedilla */ 
  "cedil",	/* cedilla */
  "cent",	/* cent sign */
  "copy",	/* copyright */
  "curren",	/* general currency sign */
  "deg",	/* degree sign */
  "divide",	/* division sign */
  "eacute",	/* small e, acute accent */ 
  "ecirc",	/* small e, circumflex accent */ 
  "egrave",	/* small e, grave accent */ 
/* 50 */
  "eth",	/* small eth, Icelandic */ 
  "euml",	/* small e, dieresis or umlaut mark */ 
  "frac12",	/* fraction one-half */
  "frac14",	/* fraction one-fourth */
  "frac34",	/* fraction three-fourth */
  "gt",		/* greater than */ 
  "iacute",	/* small i, acute accent */ 
  "icirc",	/* small i, circumflex accent */ 
  "iexcl",	/* inverted exclamation */
  "igrave",	/* small i, grave accent */ 
/* 60 */
  "iquest",	/* inverted question mark */
  "iuml",	/* small i, dieresis or umlaut mark */ 
  "laquo",	/* left angle quote */
  "lt",		/* less than */ 
  "macr",	/* macron accent */
  "micro",	/* micro sign (greek mu) */
  "middot",	/* middle dot */
  "nbsp",	/* non breaking space */
  "not",	/* not sign */
  "ntilde",	/* small n, tilde */ 
/* 70 */
  "oacute",	/* small o, acute accent */ 
  "ocirc",	/* small o, circumflex accent */ 
  "ograve",	/* small o, grave accent */ 
  "ordf",	/* feminine ordinal */
  "ordm",	/* masculine ordinal */
  "oslash",	/* small o, slash */ 
  "otilde",	/* small o, tilde */ 
  "ouml",	/* small o, dieresis or umlaut mark */
  "para",	/* paragraph sign */
  "plusmn",	/* plus or minus */
/* 80 */
  "pound",	/* pound sign */
  "quot",	/* double quote sign  - June 1994 */ 
  "raquo",	/* right angle quote */
  "reg",	/* registered trademark */
  "sect",	/* section sign */
  "shi",	/* soft hyphen */
  "sup1",	/* superscript 1 */
  "sup2",	/* superscript 2 */
  "sup3",	/* superscript 3 */
  "szlig",	/* small sharp s, German (sz ligature) */ 
/* 90 */
  "thorn",	/* small thorn, Icelandic */ 
  "times",	/* multiply sign */
  "uacute",	/* small u, acute accent */ 
  "ucirc",	/* small u, circumflex accent */ 
  "ugrave",	/* small u, grave accent */ 
  "uml",	/* dieresis or umlaut mark */
  "uuml",	/* small u, dieresis or umlaut mark */ 
  "yacute",	/* small y, acute accent */ 
  "yen",	/* yen sign */
  "yuml"	/* small y, dieresis or umlaut mark */ 
/* 100 */
};

/*
**	ATTRIBUTE DEFINITION MACROS (see HTMLPDTD.h)
*/

/*
 * redefine the macros, so that the "stringized" attribute name
 * is generated
 */

#undef	HTML_ATTR
#define	HTML_ATTR(t,a)		{ #a }
#undef	HTML_ATTRIBUTES
#define	HTML_ATTRIBUTES(t)	{ 0 }

/*
**	ATTRIBUTE LISTS
*/

PRIVATE HTAttr no_attr[1] = 
	{{ 0 }};

PRIVATE HTAttr body_attr[HTML_BODY_ATTRIBUTES+1] = {	/* to catch images */
	HTML_ATTR(BODY,ALINK),
	HTML_ATTR(BODY,BACKGROUND),
	HTML_ATTR(BODY,BGCOLOR),
	HTML_ATTR(BODY,CLASS),
	HTML_ATTR(BODY,DIR),
	HTML_ATTR(BODY,ID),
	HTML_ATTR(BODY,LANG),
	HTML_ATTR(BODY,LINK),
	HTML_ATTR(BODY,STYLE),
	HTML_ATTR(BODY,TEXT),
	HTML_ATTR(BODY,TITLE),
	HTML_ATTR(BODY,VLINK),
	HTML_ATTRIBUTES(BODY)
};	

PRIVATE HTAttr frame_attr[HTML_FRAME_ATTRIBUTES+1] = {	/* frame attributes */
	HTML_ATTR(FRAME,CLASS),
	HTML_ATTR(FRAME,FRAMEBORDER),
	HTML_ATTR(FRAME,ID),
	HTML_ATTR(FRAME,NAME),
	HTML_ATTR(FRAME,MARGINHEIGHT),
	HTML_ATTR(FRAME,MARGINWIDTH),
	HTML_ATTR(FRAME,NORESIZE),
	HTML_ATTR(FRAME,LONGDESC),
	HTML_ATTR(FRAME,SCROLLING),
	HTML_ATTR(FRAME,SRC),
	HTML_ATTR(FRAME,STYLE),
	HTML_ATTR(FRAME,TARGET),
	HTML_ATTR(FRAME,TITLE),
	HTML_ATTRIBUTES(FRAME)
};

PRIVATE HTAttr frameset_attr[HTML_FRAMESET_ATTRIBUTES+1] = { /* frameset attributes */
	HTML_ATTR(FRAMESET,CLASS),
	HTML_ATTR(FRAMESET,COLS),
	HTML_ATTR(FRAMESET,ID),
	HTML_ATTR(FRAMESET,ROWS),
	HTML_ATTR(FRAMESET,STYLE),
	HTML_ATTR(FRAMESET,TITLE),
	HTML_ATTRIBUTES(FRAMESET)
};

PRIVATE HTAttr a_attr[HTML_A_ATTRIBUTES+1] = {		/* Anchor attributes */
	HTML_ATTR(A,ACCESSKEY),
	HTML_ATTR(A,CHARSET),
	HTML_ATTR(A,CLASS),
	HTML_ATTR(A,COORDS),
	HTML_ATTR(A,DIR),
	HTML_ATTR(A,HREF),
	HTML_ATTR(A,HREFLANG),
	HTML_ATTR(A,ID),
	HTML_ATTR(A,NAME),
	HTML_ATTR(A,REL),
	HTML_ATTR(A,REV),
	HTML_ATTR(A,SHAPE),
	HTML_ATTR(A,STYLE),
	HTML_ATTR(A,TABINDEX),
	HTML_ATTR(A,TARGET),
	HTML_ATTR(A,TYPE),
	HTML_ATTR(A,TITLE),
	HTML_ATTRIBUTES(A)
};	

PRIVATE HTAttr applet_attr[HTML_APPLET_ATTRIBUTES+1] = {
	HTML_ATTR(APPLET,ALIGN),
	HTML_ATTR(APPLET,ALT),
	HTML_ATTR(APPLET,ARCHIVE),
	HTML_ATTR(APPLET,CLASS),
	HTML_ATTR(APPLET,CODE),
	HTML_ATTR(APPLET,CODEBASE),
	HTML_ATTR(APPLET,HEIGHT),
	HTML_ATTR(APPLET,HSPACE),
	HTML_ATTR(APPLET,ID),
	HTML_ATTR(APPLET,NAME),
	HTML_ATTR(APPLET,OBJECT),
	HTML_ATTR(APPLET,STYLE),
	HTML_ATTR(APPLET,TITLE),
	HTML_ATTR(APPLET,VSPACE),
	HTML_ATTR(APPLET,WIDTH),
	HTML_ATTRIBUTES(APPLET)
};

PRIVATE HTAttr area_attr[HTML_AREA_ATTRIBUTES+1] = {		/* Area attributes */
	HTML_ATTR(AREA,ACCESSKEY),
	HTML_ATTR(AREA,ALT),
	HTML_ATTR(AREA,CLASS),
	HTML_ATTR(AREA,COORDS),
	HTML_ATTR(AREA,DIR),
	HTML_ATTR(AREA,HREF),
	HTML_ATTR(AREA,ID),
	HTML_ATTR(AREA,NAME),
	HTML_ATTR(AREA,NOHREF),
	HTML_ATTR(AREA,LANG),
	HTML_ATTR(AREA,SHAPE),
	HTML_ATTR(AREA,STYLE),
	HTML_ATTR(AREA,TABINDEX),
	HTML_ATTR(AREA,TARGET),
	HTML_ATTR(AREA,TITLE),
	HTML_ATTRIBUTES(AREA)
};	

PRIVATE HTAttr base_attr[HTML_BASE_ATTRIBUTES+1] = {	/* BASE attributes */
	HTML_ATTR(BASE,HREF),
        HTML_ATTR(BASE,TARGET),
        HTML_ATTRIBUTES(BASE)
};	

PRIVATE HTAttr bdo_attr[HTML_BDO_ATTRIBUTES+1] = {
	HTML_ATTR(BDO,CLASS),
	HTML_ATTR(BDO,DIR),
	HTML_ATTR(BDO,ID),
	HTML_ATTR(BDO,LANG),
	HTML_ATTR(BDO,STYLE),
	HTML_ATTR(BDO,TITLE),
	HTML_ATTRIBUTES(BDO)
};

PRIVATE HTAttr bq_attr[HTML_BQ_ATTRIBUTES+1] = {
	HTML_ATTR(BQ,CITE),
	HTML_ATTR(BQ,CLASS),
	HTML_ATTR(BQ,DIR),
	HTML_ATTR(BQ,ID),
	HTML_ATTR(BQ,LANG),
	HTML_ATTR(BQ,STYLE),
	HTML_ATTR(BQ,TITLE),
	HTML_ATTRIBUTES(BQ)
};

PRIVATE HTAttr br_attr[HTML_BR_ATTRIBUTES+1] = {
	HTML_ATTR(BR,CLASS),
	HTML_ATTR(BR,CLEAR),
	HTML_ATTR(BR,ID),
	HTML_ATTR(BR,STYLE),
	HTML_ATTR(BR,TITLE),
	HTML_ATTRIBUTES(BR)
};

PRIVATE HTAttr button_attr[HTML_BUTTON_ATTRIBUTES+1] = {
	HTML_ATTR(BUTTON,ACCESSKEY),
	HTML_ATTR(BUTTON,CLASS),
	HTML_ATTR(BUTTON,DIR),
	HTML_ATTR(BUTTON,DISABLED),
	HTML_ATTR(BUTTON,ID),
	HTML_ATTR(BUTTON,LANG),
	HTML_ATTR(BUTTON,NAME),
	HTML_ATTR(BUTTON,STYLE),
	HTML_ATTR(BUTTON,TABINDEX),
	HTML_ATTR(BUTTON,TITLE),
	HTML_ATTR(BUTTON,TYPE),
	HTML_ATTR(BUTTON,VALUE),
	HTML_ATTRIBUTES(BUTTON),
};

PRIVATE HTAttr col_attr[HTML_COL_ATTRIBUTES+1] = {
	HTML_ATTR(COL,CLASS),
	HTML_ATTR(COL,DIR),
	HTML_ATTR(COL,ID),
	HTML_ATTR(COL,LANG),
	HTML_ATTR(COL,SPAN),
	HTML_ATTR(COL,STYLE),
	HTML_ATTR(COL,TITLE),
	HTML_ATTR(COL,WIDTH),
	HTML_ATTRIBUTES(COL)
};

PRIVATE HTAttr changes_attr[HTML_CHANGES_ATTRIBUTES+1] = {
	HTML_ATTR(CHANGES,CITE),
	HTML_ATTR(CHANGES,CLASS),
	HTML_ATTR(CHANGES,DATETIME),
	HTML_ATTR(CHANGES,DIR),
	HTML_ATTR(CHANGES,ID),
	HTML_ATTR(CHANGES,LANG),
	HTML_ATTR(CHANGES,STYLE),
	HTML_ATTR(CHANGES,TITLE),
	HTML_ATTRIBUTES(CHANGES)
};

PRIVATE HTAttr font_attr[HTML_FONT_ATTRIBUTES+1] = {
	HTML_ATTR(FONT,CLASS),
	HTML_ATTR(FONT,COLOR),
	HTML_ATTR(FONT,DIR),
	HTML_ATTR(FONT,FACE),
	HTML_ATTR(FONT,ID),
	HTML_ATTR(FONT,LANG),
	HTML_ATTR(FONT,SIZE),
	HTML_ATTR(FONT,STYLE),
	HTML_ATTR(FONT,TITLE),
	HTML_ATTRIBUTES(FONT)
};

PRIVATE HTAttr form_attr[HTML_FORM_ATTRIBUTES+1] = {
	HTML_ATTR(FORM,ACCEPT),
	{ "ACCEPT-CHARSET" }, /* HTML_ATTR(FORM,ACCEPT_CHARSET) */
	HTML_ATTR(FORM,ACTION),
	HTML_ATTR(FORM,CLASS),
	HTML_ATTR(FORM,DIR),
	HTML_ATTR(FORM,ENCTYPE),
	HTML_ATTR(FORM,ID),
	HTML_ATTR(FORM,LANG),
	HTML_ATTR(FORM,METHOD),
	HTML_ATTR(FORM,STYLE),
	HTML_ATTR(FORM,TARGET),
	HTML_ATTR(FORM,TITLE),
	HTML_ATTRIBUTES(FORM)
};

PRIVATE HTAttr gen_attr[HTML_GEN_ATTRIBUTES+1] = { /* General, for many things */
	HTML_ATTR(GEN,CLASS),
	HTML_ATTR(GEN,DIR),
	HTML_ATTR(GEN,ID),
	HTML_ATTR(GEN,LANG),
	HTML_ATTR(GEN,STYLE),
	HTML_ATTR(GEN,TITLE),
	HTML_ATTRIBUTES(GEN)
};

PRIVATE HTAttr block_attr[HTML_BLOCK_ATTRIBUTES+1] = { /* DIV, SPAN, H1-H6 */
	HTML_ATTR(BLOCK,ALIGN),
	HTML_ATTR(BLOCK,CLASS),
	HTML_ATTR(BLOCK,DIR),
	HTML_ATTR(BLOCK,ID),
	HTML_ATTR(BLOCK,LANG),
	HTML_ATTR(BLOCK,STYLE),
	HTML_ATTR(BLOCK,TITLE),
	HTML_ATTRIBUTES(BLOCK)
};

PRIVATE HTAttr head_attr[HTML_HEAD_ATTRIBUTES+1] = {
	HTML_ATTR(HEAD,DIR),
	HTML_ATTR(HEAD,LANG),
	HTML_ATTR(HEAD,PROFILE),
	HTML_ATTRIBUTES(HEAD)
};

PRIVATE HTAttr hr_attr[HTML_HR_ATTRIBUTES+1] = {
	HTML_ATTR(HR,ALIGN),
	HTML_ATTR(HR,CLASS),
	HTML_ATTR(HR,DIR),
	HTML_ATTR(HR,ID),
	HTML_ATTR(HR,LANG),
	HTML_ATTR(HR,NOSHADE),
	HTML_ATTR(HR,SIZE),
	HTML_ATTR(HR,STYLE),
	HTML_ATTR(HR,TITLE),
	HTML_ATTR(HR,WIDTH),
	HTML_ATTRIBUTES(HR)
};

PRIVATE HTAttr html_attr[HTML_HTML_ATTRIBUTES+1] = {
	HTML_ATTR(HTML,DIR),
	HTML_ATTR(HTML,LANG),
	HTML_ATTR(HTML,VERSION),
	HTML_ATTRIBUTES(HTML)
};

PRIVATE HTAttr iframe_attr[HTML_IFRAME_ATTRIBUTES+1] = {
	HTML_ATTR(IFRAME,ALIGN),
	HTML_ATTR(IFRAME,CLASS),
	HTML_ATTR(IFRAME,FRAMEBORDER),
	HTML_ATTR(IFRAME,HEIGHT),
	HTML_ATTR(IFRAME,ID),
	HTML_ATTR(IFRAME,LONGDESC),
	HTML_ATTR(IFRAME,MARGINHEIGHT),
	HTML_ATTR(IFRAME,MARGINWIDTH),
	HTML_ATTR(IFRAME,NAME),
	HTML_ATTR(IFRAME,SCROLLING),
	HTML_ATTR(IFRAME,SRC),
	HTML_ATTR(IFRAME,STYLE),
	HTML_ATTR(IFRAME,TARGET),
	HTML_ATTR(IFRAME,TITLE),
	HTML_ATTR(IFRAME,WIDTH),
	HTML_ATTRIBUTES(IFRAME)
};

PRIVATE HTAttr img_attr[HTML_IMG_ATTRIBUTES+1] = {	/* IMG attributes */
	HTML_ATTR(IMG,ALIGN),
	HTML_ATTR(IMG,ALT),
	HTML_ATTR(IMG,BORDER),
	HTML_ATTR(IMG,CLASS),
	HTML_ATTR(IMG,DIR),
	HTML_ATTR(IMG,HEIGHT),
	HTML_ATTR(IMG,HSPACE),
	HTML_ATTR(IMG,ID),
	HTML_ATTR(IMG,ISMAP),
	HTML_ATTR(IMG,LANG),
	HTML_ATTR(IMG,LONGDESC),
	HTML_ATTR(IMG,SRC),
	HTML_ATTR(IMG,STYLE),
	HTML_ATTR(IMG,TITLE),
	HTML_ATTR(IMG,USEMAP),
	HTML_ATTR(IMG,VSPACE),
	HTML_ATTR(IMG,WIDTH),
	HTML_ATTRIBUTES(IMG)
};	

PRIVATE HTAttr input_attr[HTML_INPUT_ATTRIBUTES+1] = {
	HTML_ATTR(INPUT,ACCEPT),
	HTML_ATTR(INPUT,ACCESSKEY),
	HTML_ATTR(INPUT,ALIGN),
	HTML_ATTR(INPUT,ALT),
	HTML_ATTR(INPUT,CHECKED),
	HTML_ATTR(INPUT,CLASS),
	HTML_ATTR(INPUT,DIR),
	HTML_ATTR(INPUT,DISABLED),
	HTML_ATTR(INPUT,ID),
	HTML_ATTR(INPUT,LANG),
	HTML_ATTR(INPUT,MAXLENGTH),
	HTML_ATTR(INPUT,NAME),
	HTML_ATTR(INPUT,READONLY),
	HTML_ATTR(INPUT,SIZE),
	HTML_ATTR(INPUT,SRC),
	HTML_ATTR(INPUT,STYLE),
	HTML_ATTR(INPUT,TABINDEX),
	HTML_ATTR(INPUT,TITLE),
	HTML_ATTR(INPUT,TYPE),
	HTML_ATTR(INPUT,USEMAP),
	HTML_ATTR(INPUT,VALUE),
	HTML_ATTRIBUTES(INPUT)
};

PRIVATE HTAttr isindex_attr[HTML_ISINDEX_ATTRIBUTES+1] = {
	HTML_ATTR(ISINDEX,CLASS),
	HTML_ATTR(ISINDEX,DIR),
	HTML_ATTR(ISINDEX,ID),
	HTML_ATTR(ISINDEX,LANG),
	HTML_ATTR(ISINDEX,PROMPT),
	HTML_ATTR(ISINDEX,STYLE),
	HTML_ATTR(ISINDEX,TITLE),
	HTML_ATTRIBUTES(ISINDEX)
};

PRIVATE HTAttr label_attr[HTML_LABEL_ATTRIBUTES+1] = {
	HTML_ATTR(LABEL,ACCESSKEY),
	HTML_ATTR(LABEL,CLASS),
	HTML_ATTR(LABEL,DIR),
	HTML_ATTR(LABEL,FOR),
	HTML_ATTR(LABEL,ID),
	HTML_ATTR(LABEL,LANG),
	HTML_ATTR(LABEL,STYLE),
	HTML_ATTR(LABEL,TITLE),
	HTML_ATTRIBUTES(LABEL)
};

PRIVATE HTAttr legend_attr[HTML_LEGEND_ATTRIBUTES+1] = {
    	HTML_ATTR(LEGEND,ACCESSKEY),
    	HTML_ATTR(LEGEND,ALIGN),
	HTML_ATTR(LEGEND,CLASS),
	HTML_ATTR(LEGEND,DIR),
	HTML_ATTR(LEGEND,ID),
	HTML_ATTR(LEGEND,LANG),
	HTML_ATTR(LEGEND,STYLE),
	HTML_ATTR(LEGEND,TITLE),
	HTML_ATTRIBUTES(LEGEND)
};

PRIVATE HTAttr li_attr[HTML_LI_ATTRIBUTES+1] = {
	HTML_ATTR(LI,CLASS),
	HTML_ATTR(LI,COMPACT),
	HTML_ATTR(LI,DIR),
	HTML_ATTR(LI,ID),
	HTML_ATTR(LI,LANG),
	HTML_ATTR(LI,STYLE),
	HTML_ATTR(LI,TITLE),
	HTML_ATTR(LI,TYPE),
	HTML_ATTR(LI,VALUE),
	HTML_ATTRIBUTES(LI)
};

PRIVATE HTAttr link_attr[HTML_LINK_ATTRIBUTES+1] = {	/* link attributes */
	HTML_ATTR(LINK,CHARSET),
	HTML_ATTR(LINK,CLASS),
	HTML_ATTR(LINK,DIR),
	HTML_ATTR(LINK,HREF),
	HTML_ATTR(LINK,HREFLANG),
	HTML_ATTR(LINK,ID),
	HTML_ATTR(LINK,LANG),
	HTML_ATTR(LINK,MEDIA),
	HTML_ATTR(LINK,REL),
	HTML_ATTR(LINK,REV),
	HTML_ATTR(LINK,STYLE),
	HTML_ATTR(LINK,TARGET),
	HTML_ATTR(LINK,TITLE),
	HTML_ATTR(LINK,TYPE),
	HTML_ATTRIBUTES(LINK)
};	

PRIVATE HTAttr map_attr[HTML_MAP_ATTRIBUTES+1] = {
    	HTML_ATTR(MAP,CLASS),
	HTML_ATTR(MAP,DIR),
	HTML_ATTR(MAP,ID),
	HTML_ATTR(MAP,LANG),
	HTML_ATTR(MAP,NAME),
	HTML_ATTR(MAP,STYLE),
	HTML_ATTR(MAP,TITLE),
	HTML_ATTRIBUTES(MAP)
};

PRIVATE HTAttr meta_attr[HTML_META_ATTRIBUTES+1] = {
	HTML_ATTR(META,CONTENT),
	HTML_ATTR(META,DIR),
	{ "HTTP-EQUIV" },   /* HTML_ATTR(META,HTTP_EQUIV) */
	HTML_ATTR(META,LANG),
	HTML_ATTR(META,NAME),
	HTML_ATTR(META,SCHEME),
	HTML_ATTRIBUTES(META)
};	

PRIVATE HTAttr nextid_attr[HTML_NEXTID_ATTRIBUTES+1] = {
	{ "N" },
	{ 0 }	/* Terminate list */
};

PRIVATE HTAttr object_attr[HTML_OBJECT_ATTRIBUTES+1] = {	/* object attributes */
	HTML_ATTR(OBJECT,ALIGN),
	HTML_ATTR(OBJECT,ARCHIVE),
	HTML_ATTR(OBJECT,BORDER),
	HTML_ATTR(OBJECT,CLASS),
	HTML_ATTR(OBJECT,CLASSID),
	HTML_ATTR(OBJECT,CODEBASE),
	HTML_ATTR(OBJECT,CODETYPE),
	HTML_ATTR(OBJECT,DATA),
	HTML_ATTR(OBJECT,DECLARE),
	HTML_ATTR(OBJECT,DIR),
	HTML_ATTR(OBJECT,HEIGHT),
	HTML_ATTR(OBJECT,HSPACE),
	HTML_ATTR(OBJECT,ID),
	HTML_ATTR(OBJECT,LANG),
	HTML_ATTR(OBJECT,NAME),
	HTML_ATTR(OBJECT,STANDBY),
	HTML_ATTR(OBJECT,STYLE),
	HTML_ATTR(OBJECT,TABINDEX),
	HTML_ATTR(OBJECT,TITLE),
	HTML_ATTR(OBJECT,TYPE),
	HTML_ATTR(OBJECT,USEMAP),
	HTML_ATTR(OBJECT,VSPACE),
	HTML_ATTR(OBJECT,WIDTH),
	HTML_ATTRIBUTES(OBJECT)
};	

PRIVATE HTAttr ol_attr[HTML_OL_ATTRIBUTES+1] = {
	HTML_ATTR(OL,CLASS),
	HTML_ATTR(OL,COMPACT),
	HTML_ATTR(OL,DIR),
	HTML_ATTR(OL,ID),
	HTML_ATTR(OL,LANG),
	HTML_ATTR(OL,START),
	HTML_ATTR(OL,STYLE),
	HTML_ATTR(OL,TITLE),
	HTML_ATTR(OL,TYPE),
	HTML_ATTRIBUTES(OL)
};

PRIVATE HTAttr optgroup_attr[HTML_OPTGROUP_ATTRIBUTES+1] = {
	HTML_ATTR(OPTGROUP,CLASS),
	HTML_ATTR(OPTGROUP,DISABLED),
	HTML_ATTR(OPTGROUP,DIR),
	HTML_ATTR(OPTGROUP,ID),
	HTML_ATTR(OPTGROUP,LABEL),
	HTML_ATTR(OPTGROUP,LANG),
	HTML_ATTR(OPTGROUP,STYLE),
	HTML_ATTR(OPTGROUP,TITLE),
	HTML_ATTRIBUTES(OPTGROUP)
};

PRIVATE HTAttr option_attr[HTML_OPTION_ATTRIBUTES+1] = {
	HTML_ATTR(OPTION,CLASS),
	HTML_ATTR(OPTION,DISABLED),
	HTML_ATTR(OPTION,DIR),
	HTML_ATTR(OPTION,ID),
	HTML_ATTR(OPTION,LABEL),
	HTML_ATTR(OPTION,LANG),
	HTML_ATTR(OPTION,SELECTED),
	HTML_ATTR(OPTION,STYLE),
	HTML_ATTR(OPTION,TITLE),
	HTML_ATTR(OPTION,VALUE),
	HTML_ATTRIBUTES(OPTION)
};

PRIVATE HTAttr param_attr[HTML_PARAM_ATTRIBUTES+1] = {
	HTML_ATTR(PARAM,ID),
	HTML_ATTR(PARAM,NAME),
	HTML_ATTR(PARAM,TYPE),
	HTML_ATTR(PARAM,VALUE),
	HTML_ATTR(PARAM,VALUETYPE),
	HTML_ATTRIBUTES(PARAM)
};

PRIVATE HTAttr pre_attr[HTML_PRE_ATTRIBUTES+1] = {
	HTML_ATTR(PRE,CLASS),
	HTML_ATTR(PRE,DIR),
	HTML_ATTR(PRE,ID),
	HTML_ATTR(PRE,LANG),
	HTML_ATTR(PRE,STYLE),
	HTML_ATTR(PRE,TITLE),
	HTML_ATTR(PRE,WIDTH),
	HTML_ATTRIBUTES(PRE)
};

PRIVATE HTAttr script_attr[HTML_SCRIPT_ATTRIBUTES+1] = {
	HTML_ATTR(SCRIPT,CHARSET),
	HTML_ATTR(SCRIPT,DEFER),
	HTML_ATTR(SCRIPT,LANGUAGE),
	HTML_ATTR(SCRIPT,SRC),
	HTML_ATTR(SCRIPT,TYPE),
	HTML_ATTRIBUTES(SCRIPT)
};

PRIVATE HTAttr select_attr[HTML_SELECT_ATTRIBUTES+1] = {
	HTML_ATTR(SELECT,CLASS),
	HTML_ATTR(SELECT,DIR),
	HTML_ATTR(SELECT,DISABLED),
	HTML_ATTR(SELECT,ID),
	HTML_ATTR(SELECT,LANG),
	HTML_ATTR(SELECT,MULTIPLE),
	HTML_ATTR(SELECT,NAME),
	HTML_ATTR(SELECT,SIZE),
	HTML_ATTR(SELECT,STYLE),
	HTML_ATTR(SELECT,TABINDEX),
	HTML_ATTR(SELECT,TITLE),
	HTML_ATTRIBUTES(SELECT)
};

PRIVATE HTAttr style_attr[HTML_STYLE_ATTRIBUTES+1] = {
	HTML_ATTR(STYLE,DIR),
	HTML_ATTR(STYLE,LANG),
	HTML_ATTR(STYLE,MEDIA),
	HTML_ATTR(STYLE,TITLE),
	HTML_ATTR(STYLE,TYPE),
	HTML_ATTRIBUTES(STYLE)
};

PRIVATE HTAttr table_attr[HTML_TABLE_ATTRIBUTES+1] = {
	HTML_ATTR(TABLE,ALIGN),
	HTML_ATTR(TABLE,BGCOLOR),
	HTML_ATTR(TABLE,BORDER),
	HTML_ATTR(TABLE,CELLPADDING),
	HTML_ATTR(TABLE,CELLSPACING),
	HTML_ATTR(TABLE,CLASS),
	HTML_ATTR(TABLE,DIR),
	HTML_ATTR(TABLE,FRAME),
	HTML_ATTR(TABLE,ID),
	HTML_ATTR(TABLE,LANG),
	HTML_ATTR(TABLE,RULES),
	HTML_ATTR(TABLE,SUMMARY),
	HTML_ATTR(TABLE,STYLE),
	HTML_ATTR(TABLE,TITLE),
	HTML_ATTR(TABLE,WIDTH),
	HTML_ATTRIBUTES(TABLE)
};

PRIVATE HTAttr tele_attr[HTML_TELE_ATTRIBUTES+1] = {
	HTML_ATTR(TELE,ALIGN),
	HTML_ATTR(TELE,CHAR),
	HTML_ATTR(TELE,CHAROFF),
	HTML_ATTR(TELE,CLASS),
	HTML_ATTR(TELE,DIR),
	HTML_ATTR(TELE,ID),
	HTML_ATTR(TELE,LANG),
	HTML_ATTR(TELE,STYLE),
	HTML_ATTR(TELE,TITLE),
	HTML_ATTR(TELE,VALIGN),
	HTML_ATTRIBUTES(TELE)
};

PRIVATE HTAttr td_attr[HTML_TD_ATTRIBUTES+1] = {
	HTML_ATTR(TD,ABBR),
	HTML_ATTR(TD,ALIGN),
	HTML_ATTR(TD,AXIS),
	HTML_ATTR(TD,BGCOLOR),
	HTML_ATTR(TD,CHAR),
	HTML_ATTR(TD,CHAROFF),
	HTML_ATTR(TD,CLASS),
	HTML_ATTR(TD,COLSPAN),
	HTML_ATTR(TD,DIR),
	HTML_ATTR(TD,ID),
	HTML_ATTR(TD,HEADERS),
	HTML_ATTR(TD,HEIGHT),
	HTML_ATTR(TD,LANG),
	HTML_ATTR(TD,NOWRAP),
	HTML_ATTR(TD,ROWSPAN),
	HTML_ATTR(TD,SCOPE),
	HTML_ATTR(TD,STYLE),
	HTML_ATTR(TD,TITLE),
	HTML_ATTR(TD,VALIGN),
	HTML_ATTR(TD,WIDTH),
	HTML_ATTRIBUTES(TD)
};

PRIVATE HTAttr textarea_attr[HTML_TEXTAREA_ATTRIBUTES+1] = {
	HTML_ATTR(TEXTAREA,CLASS),
	HTML_ATTR(TEXTAREA,COLS),
	HTML_ATTR(TEXTAREA,DIR),
	HTML_ATTR(TEXTAREA,DISABLED),
	HTML_ATTR(TEXTAREA,ID),
	HTML_ATTR(TEXTAREA,LANG),
	HTML_ATTR(TEXTAREA,NAME),
	HTML_ATTR(TEXTAREA,READONLY),
	HTML_ATTR(TEXTAREA,ROWS),
	HTML_ATTR(TEXTAREA,STYLE),
	HTML_ATTR(TEXTAREA,TABINDEX),
	HTML_ATTR(TEXTAREA,TITLE),
	HTML_ATTRIBUTES(TEXTAREA)
};

PRIVATE HTAttr title_attr[HTML_TITLE_ATTRIBUTES+1] = {
	HTML_ATTR(TITLE,DIR),
	HTML_ATTR(TITLE,LANG),
	HTML_ATTRIBUTES(TITLE)
};

PRIVATE HTAttr ul_attr[HTML_UL_ATTRIBUTES+1] = {
	HTML_ATTR(UL,CLASS),
	HTML_ATTR(UL,COMPACT),
	HTML_ATTR(UL,DIR),
	HTML_ATTR(UL,ID),
	HTML_ATTR(UL,LANG),
	HTML_ATTR(UL,STYLE),
	HTML_ATTR(UL,TITLE),
	HTML_ATTR(UL,TYPE),
	HTML_ATTRIBUTES(UL)
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
    { "ABBR"    , gen_attr,     HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "ACRONYM"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "ADDRESS"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "APPLET"  , applet_attr,  HTML_APPLET_ATTRIBUTES, SGML_MIXED },
    { "AREA"	, area_attr,	HTML_AREA_ATTRIBUTES,	SGML_EMPTY },
    { "B"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "BASE"	, base_attr,	HTML_BASE_ATTRIBUTES,	SGML_EMPTY },
    { "BASEFONT", font_attr,	HTML_FONT_ATTRIBUTES,	SGML_EMPTY },
    { "BDO"     , bdo_attr,     HTML_BDO_ATTRIBUTES,    SGML_EMPTY },
    { "BIG"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "BLOCKQUOTE", bq_attr,	HTML_BQ_ATTRIBUTES,	SGML_MIXED },
    { "BODY"	, body_attr,	HTML_BODY_ATTRIBUTES,	SGML_MIXED },
    { "BR"	, br_attr,	HTML_BR_ATTRIBUTES,	SGML_EMPTY },
    { "BUTTON"	, button_attr,	HTML_BUTTON_ATTRIBUTES,	SGML_MIXED },
    { "CAPTION"	, block_attr,	HTML_BLOCK_ATTRIBUTES,	SGML_MIXED },
    { "CENTER"	, no_attr,	0,			SGML_MIXED },
    { "CITE"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "CODE"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "COL"	, col_attr,	HTML_COL_ATTRIBUTES,	SGML_MIXED },
    { "COLGROUP", col_attr,	HTML_COL_ATTRIBUTES,	SGML_MIXED },
    { "DD"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_EMPTY },
    { "DEL"	, changes_attr,	HTML_CHANGES_ATTRIBUTES,SGML_EMPTY },
    { "DFN"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "DIR"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "DIV"	, block_attr,	HTML_BLOCK_ATTRIBUTES,	SGML_MIXED },
    { "DL"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "DT"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_EMPTY },
    { "EM"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "FIELDSET", gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "FONT"	, font_attr,	HTML_FONT_ATTRIBUTES,	SGML_MIXED },
    { "FORM" 	, form_attr,	HTML_FORM_ATTRIBUTES,	SGML_MIXED },
    { "FRAME"	, frame_attr,	HTML_FRAME_ATTRIBUTES,	SGML_EMPTY },
    { "FRAMESET", frameset_attr,HTML_FRAMESET_ATTRIBUTES,SGML_MIXED },
    { "H1"	, block_attr,	HTML_BLOCK_ATTRIBUTES,	SGML_MIXED },
    { "H2"	, block_attr,	HTML_BLOCK_ATTRIBUTES,	SGML_MIXED },
    { "H3"	, block_attr,	HTML_BLOCK_ATTRIBUTES,	SGML_MIXED },
    { "H4"	, block_attr,	HTML_BLOCK_ATTRIBUTES,	SGML_MIXED },
    { "H5"	, block_attr,	HTML_BLOCK_ATTRIBUTES,	SGML_MIXED },
    { "H6"	, block_attr,	HTML_BLOCK_ATTRIBUTES,	SGML_MIXED },
    { "HEAD"	, head_attr,	HTML_HEAD_ATTRIBUTES,	SGML_MIXED },
    { "HR"	, hr_attr,	HTML_HR_ATTRIBUTES,	SGML_EMPTY },
    { "HTML"	, html_attr,	HTML_HTML_ATTRIBUTES,	SGML_MIXED },  /* */
    { "I"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "IFRAME"	, iframe_attr,	HTML_IFRAME_ATTRIBUTES,	SGML_MIXED },
    { "IMG"     , img_attr,	HTML_IMG_ATTRIBUTES,	SGML_EMPTY },
    { "INPUT"	, input_attr,	HTML_INPUT_ATTRIBUTES,	SGML_EMPTY },
    { "INS"	, changes_attr,	HTML_CHANGES_ATTRIBUTES,SGML_MIXED },
    { "ISINDEX" , isindex_attr,	HTML_ISINDEX_ATTRIBUTES,SGML_EMPTY },
    { "KBD"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "LABEL"	, label_attr,	HTML_LABEL_ATTRIBUTES,	SGML_MIXED },
    { "LEGEND"	, legend_attr,	HTML_LEGEND_ATTRIBUTES,	SGML_MIXED },
    { "LI"	, li_attr,	HTML_LI_ATTRIBUTES,	SGML_EMPTY },
    { "LINK"	, link_attr,	HTML_LINK_ATTRIBUTES,	SGML_EMPTY },
    { "MAP"	, map_attr,	HTML_MAP_ATTRIBUTES,	SGML_MIXED },
    { "MENU"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "META"	, meta_attr,	HTML_META_ATTRIBUTES,	SGML_EMPTY },
    { "NEXTID"  , nextid_attr,	1,			SGML_EMPTY },
    { "NOFRAMES", gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "NOSCRIPT", gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "OBJECT"	, object_attr,	HTML_OBJECT_ATTRIBUTES,	SGML_MIXED },
    { "OL"	, ol_attr,	HTML_OL_ATTRIBUTES,	SGML_MIXED },
    { "OPTGROUP", optgroup_attr,HTML_OPTGROUP_ATTRIBUTES,SGML_MIXED },
    { "OPTION"	, option_attr,	HTML_OPTION_ATTRIBUTES,	SGML_EMPTY },/*Mixed?*/
    { "P"	, block_attr,	HTML_BLOCK_ATTRIBUTES,	SGML_EMPTY },
    { "PARAM"	, param_attr,	HTML_PARAM_ATTRIBUTES,	SGML_EMPTY },
    { "PRE"	, pre_attr,	HTML_PRE_ATTRIBUTES,	SGML_MIXED },
    { "Q"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "S"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "SAMP"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "SCRIPT"	, script_attr,	HTML_SCRIPT_ATTRIBUTES,	SGML_MIXED },
    { "SELECT"	, select_attr,	HTML_SELECT_ATTRIBUTES,	SGML_MIXED },
    { "SMALL"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "SPAN"	, block_attr,	HTML_BLOCK_ATTRIBUTES,	SGML_MIXED },
    { "STRIKE"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "STRONG"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "STYLE"	, style_attr,	HTML_STYLE_ATTRIBUTES,	SGML_MIXED },
    { "SUB"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "SUP"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "TABLE"	, table_attr,	HTML_TABLE_ATTRIBUTES,	SGML_MIXED },
    { "TBODY"	, tele_attr,	HTML_TELE_ATTRIBUTES,	SGML_MIXED },
    { "TD"	, td_attr,	HTML_TD_ATTRIBUTES,	SGML_EMPTY /*SGML_MIXED*/ },
    { "TEXTAREA", textarea_attr,HTML_TEXTAREA_ATTRIBUTES,SGML_MIXED},
    { "TFOOT"	, tele_attr,	HTML_TELE_ATTRIBUTES,	SGML_MIXED },
    { "TH"	, td_attr,	HTML_TD_ATTRIBUTES,	SGML_EMPTY /*SGML_MIXED*/},
    { "THEAD"	, tele_attr,	HTML_TELE_ATTRIBUTES,	SGML_MIXED },
    { "TITLE"	, title_attr,	HTML_TITLE_ATTRIBUTES,	SGML_MIXED },
    { "TR"	, tele_attr,	HTML_TELE_ATTRIBUTES,	SGML_EMPTY /*SGML_MIXED*/},
    { "TT"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "U"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
    { "UL"	, ul_attr,	HTML_UL_ATTRIBUTES,	SGML_MIXED },
    { "VAR"	, gen_attr,	HTML_GEN_ATTRIBUTES,	SGML_MIXED },
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
