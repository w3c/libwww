/*								      HTMLDTD.c
**	STATIC DTD FOR HTML - THIS IS OLD AND NOT USED - SEE HTMLPDTD.c
**
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	 6 Nov 93	MD	Increased size of img_attr array to make space
**  				for terminator.
*/

#include "sysdep.h"
#include "HTMLDTD.h"

/* 	Entity Names
**	------------
**
**	This table must be matched exactly with ALL the translation tables
*/
static CONST char* entities[] = {
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
  "szlig",	/* small sharp s, German (sz ligature) */ 
  "thorn",	/* small thorn, Icelandic */ 
  "uacute",	/* small u, acute accent */ 
  "ucirc",	/* small u, circumflex accent */ 
  "ugrave",	/* small u, grave accent */ 
  "uuml",	/* small u, dieresis or umlaut mark */ 
  "yacute",	/* small y, acute accent */ 
  "yuml",	/* small y, dieresis or umlaut mark */ 
};

#define HTML_ENTITIES 65


/*		Attribute Lists
**		---------------
**
**	Lists must be in alphatbetical order by attribute name
**	The tag elements contain the number of attributes
*/
static attr no_attr[1] = 
	{{ 0 }};

static attr a_attr[] = {		/* Anchor attributes */
	{ "HREF"},
	{ "NAME" },			/* Should be ID */
	{ "REL" },			/* Relationship */
	{ "REV" },			/* Reverse relationship */
	{ "TITLE" },
	{ "TYPE" },
	{ "URN" },
	{ 0 }	/* Terminate list */
};	

static attr img_attr[HTML_IMG_ATTRIBUTES+1] = {	/* Anchor attributes */
	{ "ALIGN" },
	{ "ALT" },
	{ "ISMAP"},			/* Use HTTP SpaceJump instead */
	{ "SRC"},
	{ 0 }	/* Terminate list */
};	

static attr list_attr[] = {
	{ "COMPACT"},
	{ 0 }	/* Terminate list */
};

static attr glossary_attr[] = {
	{ "COMPACT" },
	{ 0 }	/* Terminate list */
};

static attr nextid_attr[HTML_NEXTID_ATTRIBUTES+1] = {
	{ "N" },
	{ 0 }	/* Terminate list */
};

static attr pre_attr[HTML_PRE_ATTRIBUTES+1] = {
	{ "WIDTH" },
	{ 0 }	/* Terminate list */
};


/*	Elements
**	--------
**
**	Must match definitions in HTMLDTD.html!
**	Must be in alphabetical order.
**
**    Name, 	Attributes, 		content
*/
static HTTag tags[HTML_ELEMENTS] = {
    { "A"	, a_attr,	HTML_A_ATTRIBUTES,	SGML_MIXED },
    { "ADDRESS"	, no_attr,	0,		SGML_MIXED },
    { "B"	, no_attr,	0,		SGML_MIXED },
    { "BLOCKQUOTE", no_attr,	0,		SGML_MIXED },
    { "BODY"	, no_attr,	0,		SGML_MIXED },
    { "BR"	, no_attr,	0,		SGML_EMPTY },
    { "CITE"	, no_attr,	0,		SGML_MIXED },
    { "CODE"	, no_attr,	0,		SGML_MIXED },
    { "COMMENT",  no_attr,	0,		SGML_MIXED },
    { "DD"	, no_attr,	0,		SGML_EMPTY },
    { "DFN"	, no_attr,	0,		SGML_MIXED },
    { "DIR"	, list_attr,	1,		SGML_MIXED },
    { "DL"	, glossary_attr,1,		SGML_MIXED },
    { "DLC"	, glossary_attr,1,		SGML_MIXED },
    { "DT"	, no_attr,	0,		SGML_EMPTY },
    { "EM"	, no_attr,	0,		SGML_MIXED },
    { "H1"	, no_attr,	0,		SGML_MIXED },
    { "H2"	, no_attr,	0,		SGML_MIXED },
    { "H3"	, no_attr,	0,		SGML_MIXED },
    { "H4"	, no_attr,	0,		SGML_MIXED },
    { "H5"	, no_attr,	0,		SGML_MIXED },
    { "H6"	, no_attr,	0,		SGML_MIXED },
    { "H7"	, no_attr,	0,		SGML_MIXED },
    { "HEAD"	, no_attr,	0,		SGML_MIXED },
    { "HR"	, no_attr,	0,		SGML_EMPTY },
    { "HTML"	, no_attr,	0,		SGML_MIXED },
    { "I"	, no_attr,	0,		SGML_MIXED },
    { "IMG"     , img_attr,	2,		SGML_EMPTY },
    { "ISINDEX" , no_attr,	0,		SGML_EMPTY },
    { "KBD"	, no_attr,	0,		SGML_MIXED },
    { "LI"	, list_attr,	1,		SGML_EMPTY },
    { "LINK"	, a_attr,	HTML_A_ATTRIBUTES,	SGML_EMPTY },
    { "LISTING"	, no_attr,	0,		SGML_LITERAL },
    { "MENU"	, list_attr,	1,		SGML_MIXED },
    { "NEXTID"  , nextid_attr,	1,		SGML_EMPTY },
    { "OL"	, list_attr,	1,		SGML_MIXED },
    { "P"	, no_attr,	0,		SGML_EMPTY },
    { "PLAINTEXT", no_attr,	0,		SGML_LITERAL },
    { "PRE"	, pre_attr,	0,		SGML_MIXED },
    { "SAMP"	, no_attr,	0,		SGML_MIXED },
    { "STRONG"	, no_attr,	0,		SGML_MIXED },
    { "TITLE", 	  no_attr,	0,		SGML_CDATA },
    { "TT"	, no_attr,	0,		SGML_MIXED },
    { "U"	, no_attr,	0,		SGML_MIXED },
    { "UL"	, list_attr,	1,		SGML_MIXED },
    { "VAR"	, no_attr,	0,		SGML_MIXED },
    { "XMP"	, no_attr,	0,		SGML_LITERAL },
};


PUBLIC CONST SGML_dtd HTML_dtd = {
	tags,
	HTML_ELEMENTS,
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

PUBLIC void HTStartAnchor ARGS3(HTStructured *, obj,
		CONST char *,  name,
		CONST char *,  href)
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

PUBLIC void HTNextID ARGS2(HTStructured *, obj,
		int,	next_one)
{
    BOOL		present[HTML_NEXTID_ATTRIBUTES];
    CONST char*		value[HTML_NEXTID_ATTRIBUTES];
    char string[10];
    
    sprintf(string, "z%i", next_one);
    {
    	int i;
    	for(i=0; i<HTML_NEXTID_ATTRIBUTES; i++)
	    present[i] = NO;
    }
    present[HTML_NEXTID_N] = YES;
    value[HTML_NEXTID_N] = string;
    
    (*obj->isa->start_element)(obj, HTML_NEXTID , present, value);

}

