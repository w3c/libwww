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

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
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
    HTStyle *style;
    if ((style = (HTStyle  *) HT_CALLOC(1, sizeof(HTStyle))) == NULL)
        HT_OUTOFMEM("HTStyleNew");
    return style;
}

/*	Create a new style with a name
*/
PUBLIC HTStyle* HTStyleNewNamed  (CONST char *name)
{
    HTStyle * self = HTStyleNew();
    StrAllocCopy(self->name, name);
    return self;
}


/*	Free a style
*/
PUBLIC HTStyle * HTStyleFree  (HTStyle *self)
{
    if (self->name) HT_FREE(self->name);
    if (self->SGMLTag) HT_FREE(self->SGMLTag);
    HT_FREE(self);
    return 0;
}


#ifdef SUPPRESS  /* Only on the NeXT */
/*	Read a style from a stream	(without its name)
**	--------------------------
**
**	Reads a style with paragraph information from a stream.
**	The style name is not read or written by these routines.
*/
#define NONE_STRING "(None)"
#define HTStream NXStream

HTStyle * HTStyleRead (HTStyle * style, HTStream * stream)
{
    char myTag[STYLE_NAME_LENGTH];
    char fontName[STYLE_NAME_LENGTH];
    NXTextStyle *p;
    int	tab;
    int gotpara;		/* flag: have we got a paragraph definition? */
	
    NXScanf(stream, "%s%s%f%d",
	myTag,
	fontName,
	&style->fontSize,
	&gotpara);
    if (gotpara) {
	if (!style->paragraph) {
	    if ((style->paragraph = HT_MALLOC(sizeof(*(style->paragraph)))) == NULL)
		HT_OUTOFMEM("paragraph");
	    style->paragraph->tabs = 0;
	}
	p = style->paragraph;
	NXScanf(stream, "%f%f%f%f%hd%f%f%hd",
	    &p->indent1st,
	    &p->indent2nd,
	    &p->lineHt,
	    &p->descentLine,
	    &p->alignment,
	    &style->spaceBefore,
	    &style->spaceAfter,
	    &p->numTabs);
	if (p->tabs) HT_FREE(p->tabs);
	if ((p->tabs = HT_MALLOC(p->numTabs * sizeof(p->tabs[0]))) == NULL)
	    HT_OUTOFMEM("tabs");
	for (tab=0; tab < p->numTabs; tab++) {
	    NXScanf(stream, "%hd%f",
		    &p->tabs[tab].kind,
		    &p->tabs[tab].x);
	}
    } else { /* No paragraph */
        if (style->paragraph) {
    	    HT_FREE(style->paragraph);
    	    style->paragraph = 0;
	}
    } /* if no paragraph */
    StrAllocCopy(style->SGMLTag, myTag);
    if (strcmp(fontName, NONE_STRING)==0)
        style->font = 0;
    else
        style->font = [Font newFont:fontName size:style->fontSize];
    return 0;

}


/*	Write a style to a stream in a compatible way
*/
HTStyle * HTStyleWrite (HTStyle * style, NXStream * stream)
{
    int tab;
    NXTextStyle *p = style->paragraph;
    NXPrintf(stream, "%s %s %f %d\n",
	style->SGMLTag,
	style->font ? [style->font name] : NONE_STRING,
	style->fontSize,
	p!=0);

    if (p) {
	NXPrintf(stream, "\t%f %f %f %f %d %f %f\t%d\n",
	    p->indent1st,
	    p->indent2nd,
	    p->lineHt,
	    p->descentLine,
	    p->alignment,
	    style->spaceBefore,
	    style->spaceAfter,
	    p->numTabs);
	    
	for (tab=0; tab < p->numTabs; tab++)
	    NXPrintf(stream, "\t%d %f\n",
		    p->tabs[tab].kind,
		    p->tabs[tab].x);
	}
    return style;
}


/*	Write a style to stdout for diagnostics
*/
HTStyle * HTStyleDump (HTStyle * style)
{
    int tab;
    NXTextStyle *p = style->paragraph;
    TTYPrint(TDEST, "Style %d `%s' SGML:%s. Font %s %.1f point.\n",
    	style,
	style->name,
	style->SGMLTag,
	[style->font name],
	style->fontSize);
    if (p) {
        TTYPrint(TDEST, 
    	"\tIndents: first=%.0f others=%.0f, Height=%.1f Desc=%.1f\n"
	"\tAlign=%d, %d tabs. (%.0f before, %.0f after)\n",
	    p->indent1st,
	    p->indent2nd,
	    p->lineHt,
	    p->descentLine,
	    p->alignment,
	    p->numTabs,
	    style->spaceBefore,
	    style->spaceAfter);
	    
	for (tab=0; tab < p->numTabs; tab++) {
	    TTYPrint(TDEST, "\t\tTab kind=%d at %.0f\n",
		    p->tabs[tab].kind,
		    p->tabs[tab].x);
    	}
	TTYPrint(TDEST, "\n");
    } /* if paragraph */
    return style;
}
#endif


/*			StyleSheet Functions
**			====================
*/

/*	Searching for styles:
*/
HTStyle * HTStyleNamed  (HTStyleSheet *self, CONST char *name)
{
    if (self && name) {				     /* added by HWL 11/8/94 */
	HTStyle * scan;
	for (scan=self->styles; scan; scan=scan->next)
	    if (!strcmp(scan->name, name)) return scan;
	if (SGML_TRACE)
	    TTYPrint(TDEST, "StyleSheet.. No style named `%s'\n", name);
    }
    return NULL;
}

#ifdef NEXT_SUPRESS		/* Not in general common code */

HTStyle * HTStyleMatching (HTStyleSheet * self, HTStyle *style)
{
    HTStyle * scan;
    for (scan=self->styles; scan; scan=scan->next)
        if (scan->paragraph == para) return scan;
    return 0;
}

/*	Find the style which best fits a given run
**	------------------------------------------
**
**	This heuristic is used for guessing the style for a run of
**	text which has been pasted in. In order, we try:
**
**	A style whose paragraph structure is actually used by the run.
**	A style matching in font
**	A style matching in paragraph style exactly
**	A style matching in paragraph to a degree
*/

HTStyle * HTStyleForRun (HTStyleSheet *self, NXRun *run)
{
    HTStyle * scan;
    HTStyle * best = 0;
    int	bestMatch = 0;
    NXTextStyle * rp = run->paraStyle;
    for (scan=self->styles; scan; scan=scan->next)
        if (scan->paragraph == run->paraStyle) return scan;	/* Exact */

    for (scan=self->styles; scan; scan=scan->next){
    	NXTextStyle * sp = scan->paragraph;
    	if (sp) {
	    int match = 0;
	    if (sp->indent1st ==	rp->indent1st)	match = match+1;
	    if (sp->indent2nd ==	rp->indent2nd)	match = match+2;
	    if (sp->lineHt ==		rp->lineHt)	match = match+1;
	    if (sp->numTabs ==		rp->numTabs)	match = match+1;
	    if (sp->alignment ==	rp->alignment)	match = match+3;
	    if (scan->font ==		run->font)	match = match+10;
	    if (match>bestMatch) {
		    best=scan;
		    bestMatch=match;
	    }
	}
    }
    if (WWWTRACE) TTYPrint(TDEST, "HTStyleForRun: Best match for style is %d out of 18\n",
    			 bestMatch);
    return best;
}
#endif


/*	Add a style to a sheet
**	----------------------
*/
HTStyleSheet * HTStyleSheetAddStyle 
  (HTStyleSheet *self, HTStyle *style)
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
HTStyleSheet * HTStyleSheetRemoveStyle 
  (HTStyleSheet *self, HTStyle *style)
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
    HTStyleSheet * style;
    if ((style = (HTStyleSheet  *) HT_CALLOC(1, sizeof(HTStyleSheet))) == NULL)
        HT_OUTOFMEM("HTStyleSheetNew");
    return style;
}


/*	Free off a style sheet pointer
*/
HTStyleSheet * HTStyleSheetFree  (HTStyleSheet *self)
{
    HTStyle * style;
    while((style=self->styles)!=0) {
        self->styles = style->next;
	HTStyleFree(style);
    }
    HT_FREE(self);
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
    if (WWWTRACE) TTYPrint(TDEST, "Stylesheet: Reading %d styles\n", numStyles);
    for (i=0; i<numStyles; i++) {
        NXScanf(stream, "%s", styleName);
        style = HTStyleNamed(self, styleName);
	if (!style) {
	    style = HTStyleNewNamed(styleName);
	    (void) HTStyleSheetAddStyle(self, style);
	}
	(void) HTStyleRead(style, stream);
	if (WWWTRACE) HTStyleDump(style);
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
    
    if (WWWTRACE) TTYPrint(TDEST, "StyleSheet: Writing %d styles\n", numStyles);
    for (style=self->styles; style; style=style->next) {
        NXPrintf(stream, "%s ", style->name);
	(void) HTStyleWrite(style, stream);
    }
    return self;
}
#endif
