/*								      HTStyle.c
**	STYLE IMPLEMENTATION FOR HYPERTEXT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	Styles allow the translation between a logical property
**	of a piece of text and its physical representation.
**
**	A StyleSheet is a collection of styles, defining the
**	translation necessary to
**	represent a document. It is a linked list of styles.
*/

#include "WWWLib.h"
#include "HTUtils.h"
#include "HTStyle.h"

/*	Local definition of style
**	-------------------------
*/
/*      The Style Structure
**      -------------------
*/

typedef double HTCoord;
typedef int HTColor;

typedef struct {
    short               kind;           /* only NX_LEFTTAB implemented*/
    HTCoord             position;       /* x coordinate for stop */
} HTTabStop;


typedef long int HTLMFont;	/* For now */

#define HT_NON_BREAK_SPACE ((char)1)	/* For now */

#define HT_FONT		0
#define HT_CAPITALS	1
#define HT_BOLD		2
#define HT_UNDERLINE	4
#define HT_INVERSE	8
#define HT_DOUBLE	0x10

#define HT_BLACK	0
#define HT_WHITE	1


struct _HTStyle {

/*      Style management information
*/
    struct _HTStyle     *next;          /* Link for putting into stylesheet */
    char *              name;           /* Style name */
    char *              SGMLTag;        /* Tag name to start */


/*      Character attributes    (a la NXRun)
*/
    HTFont              font;           /* Font id */
    HTCoord             fontSize;       /* The size of font, not independent */
    HTColor             color;  /* text gray of current run */
    int                 superscript;    /* superscript (-sub) in points */

    HTAnchor            *anchor;        /* Anchor id if any, else zero */

/*      Paragraph Attribtes     (a la NXTextStyle)
*/
    HTCoord             indent1st;      /* how far first line in paragraph is
                                 * indented */
    HTCoord             leftIndent;     /* how far second line is indented */
    HTCoord             rightIndent;    /* (Missing from NeXT version */
    short               alignment;      /* quad justification */
    HTCoord             lineHt;         /* line height */
    HTCoord             descentLine;    /* descender bottom from baseline */
    HTTabStop           *tabs;          /* array of tab stops, 0 terminated */

    BOOL                wordWrap;       /* Yes means wrap at space not char */
    BOOL                freeFormat;     /* Yes means \n is just white space */
    HTCoord             spaceBefore;    /* Omissions from NXTextStyle */
    HTCoord             spaceAfter;
    int                 paraFlags;      /* Paragraph flags, bits as follows: */

#define PARA_KEEP       1       /* Do not break page within this paragraph */
#define PARA_WITH_NEXT  2       /* Do not break page after this paragraph */

#define HT_JUSTIFY 0            /* For alignment */
#define HT_LEFT 1
#define HT_RIGHT 2
#define HT_CENTER 3
};


/*	Create a new style
*/
PUBLIC HTStyle* HTStyleNew (void)
{
    HTStyle *style = (HTStyle *) calloc(1, sizeof(HTStyle));
    if (!style) outofmem(__FILE__, "HTStyleNew");
    return style;
}

/*	Create a new style with a name
*/
PUBLIC HTStyle* HTStyleNewNamed (CONST char * name)
{
    HTStyle * self = HTStyleNew();
    StrAllocCopy(self->name, name);
    return self;
}


/*	Free a style
*/
PUBLIC HTStyle * HTStyleFree (HTStyle * self)
{
    if (self->name) free(self->name);
    if (self->SGMLTag) free(self->SGMLTag);
    free(self);
    return 0;
}


/*                      Make a style from an element number
*/

#ifdef HTML2_ONLY

PUBLIC HTStyle * HTModfyStyle (HTStyle *  old, void *  nesting,
			       int element_number)

{
    HTStyle * s;
    s = (HTStyle*)malloc(sizeof(*s));
    if (!s) outofmem(__FILE__, "new style");

    *s = *old;
    switch(element_number) {
    case HTML_UL:
    case HTML_OL:
      break;
    deault: break;
    }
    return s;
}
#endif


/*			StyleSheet Functions
**			====================
*/

/*	Searching for styles:
*/
HTStyle * HTStyleNamed (HTStyleSheet * self, CONST char * name)
{
    HTStyle * scan;
    for (scan=self->styles; scan; scan=scan->next)
        if (0==strcmp(scan->name, name)) return scan;
    if (SGML_TRACE)
	TTYPrint(TDEST, "StyleSheet: No style named `%s'\n", name);
    return 0;
}



/*	Add a style to a sheet
**	----------------------
*/
HTStyleSheet * HTStyleSheetAddStyle (HTStyleSheet * self, HTStyle * style)
{
    style->next = 0;		/* The style will go on the end */
    if (!self->styles) {
    	self->styles = style;
    } else {
    	HTStyle * scan;
        for(scan=self->styles; scan->next; scan=scan->next); /* Find end */
	scan->next=style;
    }
    return self;
}


/*	Remove the given object from a style sheet if it exists
*/
HTStyleSheet * HTStyleSheetRemoveStyle (HTStyleSheet * self, HTStyle * style)
{
    if (self->styles == style) {
    	self->styles = style->next;
	return self;
    } else {
    	HTStyle * scan;
	for(scan = self->styles; scan; scan = scan->next) {
	    if (scan->next == style) {
	        scan->next = style->next;
		return self;
	    }
	}
    }
    return 0;
}

/*	Create new style sheet
*/

HTStyleSheet * HTStyleSheetNew (void)
{
    HTStyleSheet * style = (HTStyleSheet *) calloc(1, sizeof(HTStyleSheet));
    if (!style) outofmem(__FILE__, "HTStyleSheetNew");
    return style;
}


/*	Free off a style sheet pointer
*/
HTStyleSheet * HTStyleSheetFree (HTStyleSheet * self)
{
    HTStyle * style;
    while((style=self->styles)!=0) {
        self->styles = style->next;
	HTStyleFree(style);
    }
    free(self);
    return 0;
}


/*	Read a stylesheet from a typed stream
**	-------------------------------------
**
**	Reads a style sheet from a stream.  If new styles have the same names
**	as existing styles, they replace the old ones without changing the ids.
*/

#ifdef NEXT_SUPRESS  /* Only on the NeXT */
HTStyleSheet * HTStyleSheetRead(HTStyleSheet * self, NXStream * stream)
{
    int numStyles;
    int i;
    HTStyle * style;
    char styleName[80];
    NXScanf(stream, " %d ", &numStyles);
    if (SGML_TRACE)
	TTYPrint(TDEST, "Stylesheet: Reading %d styles\n", numStyles);
    for (i=0; i<numStyles; i++) {
        NXScanf(stream, "%s", styleName);
        style = HTStyleNamed(self, styleName);
	if (!style) {
	    style = HTStyleNewNamed(styleName);
	    (void) HTStyleSheetAddStyle(self, style);
	}
	(void) HTStyleRead(style, stream);
	if (SGML_TRACE)
	    HTStyleDump(style);
    }
    return self;
}

/*	Write a stylesheet to a typed stream
**	------------------------------------
**
**	Writes a style sheet to a stream.
*/

HTStyleSheet * HTStyleSheetWrite(HTStyleSheet * self, NXStream * stream)
{
    int numStyles = 0;
    HTStyle * style;
    
    for(style=self->styles; style; style=style->next) numStyles++;
    NXPrintf(stream, "%d\n", numStyles);
    
    if (SGML_TRACE)
	TTYPrint(TDEST, "StyleSheet: Writing %d styles\n", numStyles);
    for (style=self->styles; style; style=style->next) {
        NXPrintf(stream, "%s ", style->name);
	(void) HTStyleWrite(style, stream);
    }
    return self;
}
#endif
