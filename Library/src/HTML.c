/*									 HTML.c
**	STRUCTURED STREAM TO RICH HYPERTEXT CONVERTER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This generates of a hypertext object.  It converts from the
**	structured stream interface fro HTMl events into the style-
**	oriented iunterface of the HText interface.  This module is
**	only used in clients and shouldnot be linked into servers.
**
**	Override this module if making a new GUI browser.
**
** HISTORY:
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTAtom.h"
#include "HTChunk.h"
#include "HText.h"
#include "HTStyle.h"
#include "HTAlert.h"
#include "HTMLGen.h"
#include "HTParse.h"
#include "HTML.h"

extern HTStyleSheet * styleSheet;	/* Application-wide */

/*	Module-wide style cache
*/
PRIVATE int 		got_styles = 0;
PRIVATE HTStyle *styles[HTMLP_ELEMENTS];
PRIVATE HTStyle *default_style;


/*		HTML Object
**		-----------
*/
#define MAX_NESTING 20		/* Should be checked by parser */

typedef struct _stack_element {
        HTStyle *	style;
	int		tag_number;
} stack_element;

struct _HTStructured {
    CONST HTStructuredClass * 	isa;
    HTRequest *			request;
    HTParentAnchor * 		node_anchor;
    HText * 			text;

    HTStream*			target;			/* Output stream */
    HTStreamClass		targetClass;		/* Output routines */

    HTChunk * 			title;		/* Grow by 128 */
    
    char *			comment_start;	/* for literate programming */
    char *			comment_end;
    
    CONST SGML_dtd*		dtd;
    
    HTTag *			current_tag;
    BOOL			style_change;
    HTStyle *			new_style;
    HTStyle *			old_style;
    BOOL			in_word;  /* Have just had a non-white char */

    stack_element 		stack[MAX_NESTING];
    stack_element 		*sp;		      /* Style stack pointer */
    int				overflow;  /* Keep track of overflow nesting */
};

struct _HTStream {
    CONST HTStreamClass *	isa;
    /* .... */
};

/*		Forward declarations of routines
*/
PRIVATE void get_styles (void);


PRIVATE void actually_set_style (HTStructured * me);
PRIVATE void change_paragraph_style (HTStructured * me, HTStyle * style);

/*	Style buffering avoids dummy paragraph begin/ends.
*/
#define UPDATE_STYLE if (me->style_change) { actually_set_style(me); }


#ifdef OLD_CODE
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
#endif


/* 	Entity values -- for ISO Latin 1 local representation
**
**	This MUST match exactly the table referred to in the DTD!
*/
static char * ISO_Latin1[] = {
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
  	"\341",	/* small a, acute accent */ 
  	"\342",	/* small a, circumflex accent */ 
  	"\346",	/* small ae diphthong (ligature) */ 
  	"\340",	/* small a, grave accent */ 
  	"\046",	/* ampersand */ 
  	"\345",	/* small a, ring */ 
  	"\343",	/* small a, tilde */ 
  	"\344",	/* small a, dieresis or umlaut mark */ 
  	"\347",	/* small c, cedilla */ 
  	"\351",	/* small e, acute accent */ 
  	"\352",	/* small e, circumflex accent */ 
  	"\350",	/* small e, grave accent */ 
  	"\360",	/* small eth, Icelandic */ 
  	"\353",	/* small e, dieresis or umlaut mark */ 
  	"\076",	/* greater than */ 
  	"\355",	/* small i, acute accent */ 
  	"\356",	/* small i, circumflex accent */ 
  	"\354",	/* small i, grave accent */ 
  	"\357",	/* small i, dieresis or umlaut mark */ 
  	"\074",	/* less than */ 
  	"\361",	/* small n, tilde */ 
  	"\363",	/* small o, acute accent */ 
  	"\364",	/* small o, circumflex accent */ 
  	"\362",	/* small o, grave accent */ 
  	"\370",	/* small o, slash */ 
  	"\365",	/* small o, tilde */ 
  	"\366",	/* small o, dieresis or umlaut mark */ 
        "\042", /* double quote sign - June 94 */
  	"\337",	/* small sharp s, German (sz ligature) */ 
  	"\376",	/* small thorn, Icelandic */ 
  	"\372",	/* small u, acute accent */ 
  	"\373",	/* small u, circumflex accent */ 
  	"\371",	/* small u, grave accent */ 
  	"\374",	/* small u, dieresis or umlaut mark */ 
  	"\375",	/* small y, acute accent */ 
  	"\377",	/* small y, dieresis or umlaut mark */ 
};


/* 	Entity values -- for NeXT local representation
**
**	This MUST match exactly the table referred to in the DTD!
**
*/
static char * NeXTCharacters[] = {
  	"\341",	/* capital AE diphthong (ligature) 	*/ 
  	"\202",	/* capital A, acute accent		*/ 
  	"\203",	/* capital A, circumflex accent 	*/ 
  	"\201",	/* capital A, grave accent 		*/ 
  	"\206",	/* capital A, ring 			*/ 
  	"\204",	/* capital A, tilde 			*/ 
  	"\205",	/* capital A, dieresis or umlaut mark	*/ 
  	"\207",	/* capital C, cedilla 			*/ 
  	"\220",	/* capital Eth, Icelandic 		*/ 
  	"\211",	/* capital E, acute accent 				*/ 
  	"\212",	/* capital E, circumflex accent 			*/ 
  	"\210",	/* capital E, grave accent 				*/ 
  	"\213",	/* capital E, dieresis or umlaut mark 			*/ 
  	"\215",	/* capital I, acute accent 				*/ 
  	"\216",	/* capital I, circumflex accent 	these are	*/ 
  	"\214",	/* capital I, grave accent		ISO -100 hex	*/ 
  	"\217",	/* capital I, dieresis or umlaut mark			*/ 
  	"\221",	/* capital N, tilde 					*/ 
  	"\223",	/* capital O, acute accent 				*/ 
  	"\224",	/* capital O, circumflex accent 			*/ 
  	"\222",	/* capital O, grave accent 				*/ 
  	"\351",	/* capital O, slash 		'cept this */ 
  	"\225",	/* capital O, tilde 					*/ 
  	"\226",	/* capital O, dieresis or umlaut mark			*/ 
  	"\234",	/* capital THORN, Icelandic */ 
  	"\230",	/* capital U, acute accent */ 
  	"\231",	/* capital U, circumflex accent */ 
  	"\227",	/* capital U, grave accent */ 
  	"\232",	/* capital U, dieresis or umlaut mark */ 
  	"\233",	/* capital Y, acute accent */ 
  	"\326",	/* small a, acute accent */ 
  	"\327",	/* small a, circumflex accent */ 
  	"\361",	/* small ae diphthong (ligature) */ 
  	"\325",	/* small a, grave accent */ 
  	"\046",	/* ampersand */ 
  	"\332",	/* small a, ring */ 
  	"\330",	/* small a, tilde */ 
  	"\331",	/* small a, dieresis or umlaut mark */ 
  	"\333",	/* small c, cedilla */ 
  	"\335",	/* small e, acute accent */ 
  	"\336",	/* small e, circumflex accent */ 
  	"\334",	/* small e, grave accent */ 
  	"\346",	/* small eth, Icelandic 	*/ 
  	"\337",	/* small e, dieresis or umlaut mark */ 
  	"\076",	/* greater than */ 
  	"\342",	/* small i, acute accent */ 
  	"\344",	/* small i, circumflex accent */ 
  	"\340",	/* small i, grave accent */ 
  	"\345",	/* small i, dieresis or umlaut mark */ 
  	"\074",	/* less than */ 
  	"\347",	/* small n, tilde */ 
  	"\355",	/* small o, acute accent */ 
  	"\356",	/* small o, circumflex accent */ 
  	"\354",	/* small o, grave accent */ 
  	"\371",	/* small o, slash */ 
  	"\357",	/* small o, tilde */ 
  	"\360",	/* small o, dieresis or umlaut mark */ 
        "\042", /* double quote sign - June 94 */
  	"\373",	/* small sharp s, German (sz ligature) */ 
  	"\374",	/* small thorn, Icelandic */ 
  	"\363",	/* small u, acute accent */ 
  	"\364",	/* small u, circumflex accent */ 
  	"\362",	/* small u, grave accent */ 
  	"\366",	/* small u, dieresis or umlaut mark */ 
  	"\367",	/* small y, acute accent */ 
  	"\375",	/* small y, dieresis or umlaut mark */ 
};

/* 	Entity values -- for IBM/PC Code Page 850 (International)
**
**	This MUST match exactly the table referred to in the DTD!
**
*/
/* @@@@@@@@@@@@@@@@@ TBD */



/*		Set character set
**		----------------
*/

PRIVATE char** p_entity_values = ISO_Latin1;	/* Pointer to translation */

PUBLIC void HTMLUseCharacterSet (HTMLCharacterSet i)
{
    p_entity_values = (i == HTML_NEXT_CHARS) ? NeXTCharacters
    					     : ISO_Latin1;
}


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
PRIVATE void actually_set_style (HTStructured * me)
{
    if (!me->text) {			/* First time through */
	    me->text = HText_new2(me->request, me->node_anchor, me->target);
	    HText_beginAppend(me->text);
	    HText_setStyle(me->text, me->new_style);
	    me->in_word = NO;
    } else {
	    HText_setStyle(me->text, me->new_style);
    }
    me->old_style = me->new_style;
    me->style_change = NO;
}

/*      If you THINK you need to change style, call this
*/

PRIVATE void change_paragraph_style (HTStructured * me, HTStyle *style)
{
    if (me->new_style!=style) {
    	me->style_change = YES;
	me->new_style = style;
    }
    me->in_word = NO;
}

/*_________________________________________________________________________
**
**			A C T I O N 	R O U T I N E S
*/

/*	Character handling
**	------------------
*/
PRIVATE int HTML_put_character (HTStructured * me, char c)
{

    switch (me->sp[0].tag_number) {
    case HTML_COMMENT:
    	break;					/* Do Nothing */
	
    case HTML_TITLE:	
    	HTChunk_putb(me->title, &c, 1);
	break;

	
    case HTML_LISTING:				/* Litteral text */
    case HTML_XMP:
    case HTML_PLAINTEXT:
    case HTML_PRE:
/*	We guarrantee that the style is up-to-date in begin_litteral
*/
    	HText_appendCharacter(me->text, c);
	break;
	
    default:					/* Free format text */
	if (me->style_change) {
	    if ((c=='\n') || (c==' ')) return HT_OK;	/* Ignore it */
	    UPDATE_STYLE;
	}
	if (c=='\n') {
	    if (me->in_word) {
		HText_appendCharacter(me->text, ' ');
		me->in_word = NO;
	    }
	} else {
	    HText_appendCharacter(me->text, c);
	    me->in_word = YES;
	}
    } /* end switch */
    return HT_OK;
}



/*	String handling
**	---------------
**
**	This is written separately from put_character becuase the loop can
**	in some cases be promoted to a higher function call level for speed.
*/
PRIVATE int HTML_put_string (HTStructured * me, CONST char* s)
{

    switch (me->sp[0].tag_number) {
    case HTML_COMMENT:
    	break;					/* Do Nothing */
	
    case HTML_TITLE:	
    	HTChunk_putb(me->title, s, strlen(s));
	break;

	
    case HTML_LISTING:				/* Litteral text */
    case HTML_XMP:
    case HTML_PLAINTEXT:
    case HTML_PRE:

/*	We guarrantee that the style is up-to-date in begin_litteral
*/
    	HText_appendText(me->text, s);
	break;
	
    default:					/* Free format text */
        {
	    CONST char *p = s;
	    if (me->style_change) {
		for (; *p && ((*p=='\n') || (*p==' ')); p++)  ;  /* Ignore leaders */
		if (!*p) return HT_OK;
		UPDATE_STYLE;
	    }
	    for(; *p; p++) {
		if (me->style_change) {
		    if ((*p=='\n') || (*p==' ')) continue;  /* Ignore it */
		    UPDATE_STYLE;
		}
		if (*p=='\n') {
		    if (me->in_word) {
			HText_appendCharacter(me->text, ' ');
			me->in_word = NO;
		    }
		} else {
		    HText_appendCharacter(me->text, *p);
		    me->in_word = YES;
		}
	    } /* for */
	}
    } /* end switch */
    return HT_OK;
}


/*	Buffer write
**	------------
*/
PRIVATE int HTML_write (HTStructured * me, CONST char* s, int l)
{
    while (l-- > 0)
	HTML_put_character(me, *s++);
    return HT_OK;
}


/*	Start Element
**	-------------
*/
PRIVATE void HTML_start_element (
	HTStructured * 	me,
	int			element_number,
	CONST BOOL*	 	present,
	CONST char **		value)
{
    switch (element_number) {
    case HTML_A:
	{
	    HTChildAnchor * source;
	    char * href = NULL;
	    if (present[HTML_A_HREF])
	    	StrAllocCopy(href, value[HTML_A_HREF]);
	    source = HTAnchor_findChildAndLink(
		me->node_anchor,				/* parent */
		present[HTML_A_NAME] ? value[HTML_A_NAME] : 0,	/* Tag */
		present[HTML_A_HREF] ? href : 0,		/* Addresss */
		present[HTML_A_REL] && value[HTML_A_REL] ? 
			(HTLinkType) HTAtom_for(value[HTML_A_REL])
					       : 0);
	    
	    if (present[HTML_A_TITLE] && value[HTML_A_TITLE]) {
	        HTParentAnchor * dest = 
		    HTAnchor_parent(
			HTAnchor_followMainLink((HTAnchor*)source)
				    );
		if (!HTAnchor_title(dest))
			HTAnchor_setTitle(dest, value[HTML_A_TITLE]);
	    }
	    UPDATE_STYLE;
	    HText_beginAnchor(me->text, source);
	    HT_FREE(href);		        /* Leak fix Henrik 17/02-94 */
	}
    	break;
	
    case HTML_TITLE:
        HTChunk_clear(me->title);
	break;
	
    case HTML_NEXTID:
    	/* if (present[NEXTID_N] && value[NEXTID_N])
		HText_setNextId(me->text, atoi(value[NEXTID_N])); */
    	break;
	
    case HTML_ISINDEX:
   	HTAnchor_setIndex(me->node_anchor);
	break;
	
    case HTML_BR: 
	UPDATE_STYLE;
	HText_appendCharacter(me->text, '\n');
	me->in_word = NO;
	break;
	
    case HTML_HR: 
	UPDATE_STYLE;
	HText_appendCharacter(me->text, '\n');
	HText_appendText(me->text, "___________________________________");
	HText_appendCharacter(me->text, '\n');
	me->in_word = NO;
	break;
	
    case HTML_P:
	UPDATE_STYLE;
	HText_appendParagraph(me->text);
	me->in_word = NO;
	break;

    case HTML_DL:
        change_paragraph_style(me, present && present[DL_COMPACT]
    		? styles[HTML_DL]
		: styles[HTML_DL]);
	break;
	
    case HTML_DT:
        if (!me->style_change) {
	    HText_appendParagraph(me->text);
	    me->in_word = NO;
	}
	break;
	
    case HTML_DD:
        UPDATE_STYLE;
	HTML_put_character(me, '\t');	/* Just tab out one stop */
	me->in_word = NO;
	break;

    case HTML_UL:
    case HTML_OL:
    case HTML_MENU:
    case HTML_DIR:
	change_paragraph_style(me, styles[element_number]);
	break;
	
    case HTML_LI:
        UPDATE_STYLE;
	if (me->sp[0].tag_number != HTML_DIR)
	    HText_appendParagraph(me->text);
	else
	    HText_appendCharacter(me->text, '\t');	/* Tab @@ nl for UL? */
	me->in_word = NO;
	break;
	
    case HTML_LISTING:				/* Litteral text */
    case HTML_XMP:
    case HTML_PLAINTEXT:
    case HTML_PRE:
	change_paragraph_style(me, styles[element_number]);
	UPDATE_STYLE;
    	if (me->comment_end)
    	    HText_appendText(me->text, me->comment_end);
	break;

    case HTML_IMG:			/* Images */
	{
	    HTChildAnchor *source;
	    char *src = NULL;
	    if (present[HTML_IMG_SRC])
		StrAllocCopy(src, value[HTML_IMG_SRC]);
	    source = HTAnchor_findChildAndLink(
					       me->node_anchor,	   /* parent */
					       0,                     /* Tag */
					       src ? src : 0,    /* Addresss */
					       0);
	    UPDATE_STYLE;
	    HText_appendImage(me->text, source,
		      present[HTML_IMG_ALT] ? value[HTML_IMG_ALT] : NULL,
		      present[HTML_IMG_ALIGN] ? value[HTML_IMG_ALIGN] : NULL,
		      present[HTML_IMG_ISMAP] ? YES : NO);
	    HT_FREE(src);
	}	
	break;

    case HTML_HTML:			/* Ignore these altogether */
    case HTML_HEAD:
    case HTML_BODY:
    
    case HTML_TT:			/* Physical character highlighting */
    case HTML_B:			/* Currently ignored */
    case HTML_I:
    case HTML_U:
    
    case HTML_EM:			/* Logical character highlighting */
    case HTML_STRONG:			/* Currently ignored */
    case HTML_CODE:
    case HTML_SAMP:
    case HTML_KBD:
    case HTML_VAR:
    case HTML_DFN:
    case HTML_CITE:
    	break;
	
    case HTML_H1:			/* paragraph styles */
    case HTML_H2:
    case HTML_H3:
    case HTML_H4:
    case HTML_H5:
    case HTML_H6:
    case HTML_H7:
    case HTML_ADDRESS:
    case HTML_BLOCKQUOTE:
    	change_paragraph_style(me, styles[element_number]);	/* May be postponed */
	break;

    } /* end switch */

    if (me->dtd->tags[element_number].contents!= SGML_EMPTY) {
        if (me->sp == me->stack) {
	    if (SGML_TRACE)
		TTYPrint(TDEST, "HTML........ Maximum nesting of %d exceded!\n",
			MAX_NESTING); 
	    me->overflow++;
	    return;
	}
    	--(me->sp);
	me->sp[0].style = me->new_style;	/* Stack new style */
	me->sp[0].tag_number = element_number;
    }	
}


/*		End Element
**		-----------
**
*/
/*	When we end an element, the style must be returned to that
**	in effect before that element.  Note that anchors (etc?)
**	don't have an associated style, so that we must scan down the
**	stack for an element with a defined style. (In fact, the styles
**	should be linked to the whole stack not just the top one.)
**	TBL 921119
**
**	We don't turn on "CAREFUL" check because the parser produces
**	(internal code errors apart) good nesting. The parser checks
**	incoming code errors, not this module.
*/
PRIVATE void HTML_end_element (HTStructured * me, int element_number)
{
#ifdef CAREFUL			/* parser assumed to produce good nesting */
    if (element_number != me->sp[0].tag_number) {
        TTYPrint(TDEST, "HTMLText: end of element %s when expecting end of %s\n",
		me->dtd->tags[element_number].name,
		me->dtd->tags[me->sp->tag_number].name);
		/* panic */
    }
#endif

    /* HFN, If overflow of nestings, we need to get back to reality */
    if (me->overflow > 0) {
	me->overflow--;
	return;
    }

    me->sp++;				/* Pop state off stack */

    switch(element_number) {

    case HTML_A:
	UPDATE_STYLE;
	HText_endAnchor(me->text);
	break;

    case HTML_TITLE:
    	HTAnchor_setTitle(me->node_anchor, HTChunk_data(me->title));
	break;
	
    case HTML_LISTING:				/* Litteral text */
    case HTML_XMP:
    case HTML_PLAINTEXT:
    case HTML_PRE:
    	if (me->comment_start)
    	    HText_appendText(me->text, me->comment_start);
	/* Fall through */
	
    default:

	/* Often won't really change */
	change_paragraph_style(me, me->sp->style);
	break;
	
    } /* switch */
}


/*		Expanding entities
**		------------------
*/
/*	(In fact, they all shrink!)
*/

PRIVATE void HTML_put_entity (HTStructured * me, int entity_number)
{
    HTML_put_string(me, ISO_Latin1[entity_number]);	/* @@ Other representations */
}

/*	Flush an HTML object
**	--------------------
*/
PUBLIC int HTML_flush (HTStructured * me)
{
    UPDATE_STYLE;			     /* Creates empty document here! */
    if (me->comment_end) HTML_put_string(me,me->comment_end);
    return me->target ? (*me->targetClass.flush)(me->target) : HT_OK;
}

/*	Free an HTML object
**	-------------------
**
** If the document is empty, the text object will not yet exist.
   So we could in fact abandon creating the document and return
   an error code.  In fact an empty document is an important type
   of document, so we don't.
**
**	If non-interactive, everything is freed off.   No: crashes -listrefs
**	Otherwise, the interactive object is left.	
*/
PUBLIC int HTML_free (HTStructured * me)
{
    UPDATE_STYLE;		/* Creates empty document here! */
    if (me->comment_end)
		HTML_put_string(me,me->comment_end);
    HText_endAppend(me->text);

    if (me->target) {
        (*me->targetClass._free)(me->target);
    }
    HTChunk_delete(me->title);
    HT_FREE(me);
    return HT_OK;
}


PRIVATE int HTML_abort (HTStructured * me, HTList * e)

{
    if (me->target) {
        (*me->targetClass.abort)(me->target, e);
    }
    HTChunk_delete(me->title);
    HT_FREE(me);
    return HT_ERROR;
}


/*	Get Styles from style sheet
**	---------------------------
*/
PRIVATE void get_styles (void)
{
    got_styles = YES;
    
    default_style =		HTStyleNamed(styleSheet, "Normal");

    styles[HTML_H1] =		HTStyleNamed(styleSheet, "Heading1");
    styles[HTML_H2] =		HTStyleNamed(styleSheet, "Heading2");
    styles[HTML_H3] =		HTStyleNamed(styleSheet, "Heading3");
    styles[HTML_H4] =		HTStyleNamed(styleSheet, "Heading4");
    styles[HTML_H5] =		HTStyleNamed(styleSheet, "Heading5");
    styles[HTML_H6] =		HTStyleNamed(styleSheet, "Heading6");
    styles[HTML_H7] =		HTStyleNamed(styleSheet, "Heading7");

    styles[HTML_DL] =		HTStyleNamed(styleSheet, "Glossary");
    styles[HTML_UL] =
    styles[HTML_OL] =		HTStyleNamed(styleSheet, "List");
    styles[HTML_MENU] =		HTStyleNamed(styleSheet, "Menu");
    styles[HTML_DIR] =		HTStyleNamed(styleSheet, "Dir");    
/*  styles[HTML_DLC] =		HTStyleNamed(styleSheet, "GlossaryCompact"); */
    styles[HTML_ADDRESS]=	HTStyleNamed(styleSheet, "Address");
    styles[HTML_BLOCKQUOTE]=	HTStyleNamed(styleSheet, "BlockQuote");
    styles[HTML_PLAINTEXT] =
    styles[HTML_XMP] =		HTStyleNamed(styleSheet, "Example");
    styles[HTML_PRE] =		HTStyleNamed(styleSheet, "Preformatted");
    styles[HTML_LISTING] =	HTStyleNamed(styleSheet, "Listing");
}
/*				P U B L I C
*/

/*	Structured Object Class
**	-----------------------
*/
PRIVATE CONST HTStructuredClass HTMLPresentation = /* As opposed to print etc */
{		
	"text/html",
	HTML_flush,
	HTML_free,
	HTML_abort,
	HTML_put_character, 	HTML_put_string,  HTML_write,
	HTML_start_element, 	HTML_end_element,
	HTML_put_entity
}; 


/*		New Structured Text object
**		--------------------------
**
**	The structured stream can generate either presentation,
**	or plain text, or HTML.
*/
PRIVATE HTStructured* HTML_new (HTRequest *	request,
				     void *		param,
				     HTFormat		input_format,
				     HTFormat		output_format,
				     HTStream *	output_stream)
{

    HTStructured * me;
    
#if 0
    if (output_format != WWW_PLAINTEXT
    	&& output_format != WWW_PRESENT
	&& output_format != HTAtom_for("text/x-c")) {
        HTStream * intermediate = HTStreamStack(WWW_HTML, output_format,
						output_stream, request, NO);
	if (intermediate) return HTMLGenerator(intermediate);
	if (SGML_TRACE)
	    TTYPrint(TDEST, "HTML........ Can't parse HTML to %s\n",
		    HTAtom_name(output_format));
	exit (-99);
    }
#endif

    if ((me = (HTStructured *) HT_CALLOC(1, sizeof(*me))) == NULL)
        HT_OUTOFMEM("HTML_new");

    if (!got_styles) get_styles();

    me->isa = &HTMLPresentation;
    me->dtd = &HTMLP_dtd;
    me->request = request;
    me->node_anchor =  HTRequest_anchor(request);
    me->title = HTChunk_new(128);
    me->text = 0;
    me->style_change = YES; /* Force check leading to text creation */
    me->new_style = default_style;
    me->old_style = 0;
    me->sp = me->stack + MAX_NESTING - 1;
    me->sp->tag_number = -1;				/* INVALID */
    me->sp->style = default_style;			/* INVALID */
    
    me->comment_start = NULL;
    me->comment_end = NULL;
    me->target = output_stream;
    if (output_stream) me->targetClass = *output_stream->isa;	/* Copy pointers */
    
    return (HTStructured*) me;
}


/*	HTConverter for HTML to plain text
**	----------------------------------
**
**	This will convert from HTML to presentation or plain text.
*/
PUBLIC HTStream* HTMLToPlain (
	HTRequest *		request,
	void *			param,
	HTFormat		input_format,
	HTFormat		output_format,
	HTStream *		output_stream)
{
    return SGML_new(&HTMLP_dtd, HTML_new(
    	request, NULL, input_format, output_format, output_stream));
}


/*	HTConverter for HTML to C code
**	------------------------------
**
**	C code is like plain text but all non-preformatted code
**	is commented out.
**	This will convert from HTML to presentation or plain text.
*/
PUBLIC HTStream* HTMLToC (
	HTRequest *		request,
	void *			param,
	HTFormat		input_format,
	HTFormat		output_format,
	HTStream *		output_stream)
{
    
    HTStructured * html;
    
    (*output_stream->isa->put_string)(output_stream, "/* "); /* Before title */
    html = HTML_new(request, NULL, input_format, output_format, output_stream);
    html->comment_start = "\n/* ";
    html->dtd = &HTMLP_dtd;
    html->comment_end = " */\n";	/* Must start in col 1 for cpp */
    return SGML_new(&HTMLP_dtd, html);
}


/*	Presenter for HTML
**	------------------
**
**	This will convert from HTML to presentation or plain text.
**
**	Override this if you have a windows version
*/
#ifndef GUI
PUBLIC HTStream* HTMLPresent (
	HTRequest *		request,
	void *			param,
	HTFormat		input_format,
	HTFormat		output_format,
	HTStream *		output_stream)
{
    return SGML_new(&HTMLP_dtd, HTML_new(
    	request, NULL, input_format, output_format, output_stream));
}
#endif

