/*								     GridText.c
**	CHARACTER GRID HYPERTEXT OBJECT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This is the definition of the HyperDoc object and the HText interface
**	which is used in the current Library HTML parser
*/

#include <assert.h>

#include "WWWLib.h"
#include "WWWCache.h"
#include "WWWApp.h"
#include "WWWHTML.h"
#include "HTBrowse.h"
#include "HTFont.h"
#include "GridStyle.h"
#include "GridText.h"

/* HWL 18/7/94: applied patch from agl@glas2.glas.apc.org (Anton Tropashko) */
#ifdef CYRILLIC
#include "a_stdio.h"
#endif

#define MAX_LINE	HTScreenWidth	/* No point in accumulating more */
#define LOADED_LIMIT 40

#ifdef CURSES
#define DISPLAY_LINES (HTScreenHeight)
#define       TITLE_LINES      0
#else
#define DISPLAY_LINES (HTScreenHeight - 2)   /* Exclude prompt line */
#define       TITLE_LINES      1
#endif

struct _HTStream {			/* only know it as object */
    const HTStreamClass *	isa;
    /* ... */
};

/*	From default style sheet:
*/
extern HTStyleSheet * styleSheet;	/* Default or overridden */

/*	Exports
*/ 
PUBLIC HText * HTMainText = 0;		/* Equivalent of main window */
PUBLIC HTParentAnchor * HTMainAnchor = 0;	/* Anchor for HTMainText */

typedef struct _line {
	struct _line	*next;
	struct _line	*prev;
	short unsigned	offset;		/* Implicit initial spaces */
	short unsigned	size;		/* Number of characters */
	BOOL	split_after;		/* Can we split after? */
	BOOL	bullet;			/* Do we bullet? */
	char	data[1];		/* Space for terminator at least! */
} HTLine;

#define LINE_SIZE(l) (sizeof(HTLine)+(l))	/* allow for terminator */

typedef struct _TextAnchor {
	struct _TextAnchor *	next;
	int			number;		/* For user interface */
	int			start;		/* Characters */
	int			extent;		/* Characters */
	HTChildAnchor *		anchor;
} TextAnchor;


/*	Notes on struct _Htext:
**	next_line is valid iff state is false.
**	top_of_screen line means the line at the top of the screen
**			or just under the title if there is one.
*/
struct _HText {
	HTParentAnchor *	node_anchor;
	char *			title;
	HTLine * 		last_line;
	int			lines;		/* Number of them */
	int			chars;		/* Number of them */
	TextAnchor *		first_anchor;	/* Singly linked list */
	TextAnchor *		last_anchor;
	int			last_anchor_number;	/* user number */
	TextAnchor *		current_anchor;
/* For Internal use: */	
	HTStyle *		style;			/* Current style */
	int			display_on_the_fly;	/* Lines left */
	BOOL			all_pages;		/* Loop on the fly */
	int			top_of_screen;		/* Line number */
	HTLine *		top_of_screen_line;	/* Top */
	HTLine *		next_line;		/* Bottom + 1 */
	int			permissible_split;	/* in last line */
	BOOL			in_line_1;		/* of paragraph */
	BOOL			stale;			/* Must refresh */
	
	HTStream*		target;			/* Output stream */
	HTStreamClass		targetClass;		/* Output routines */
	LineMode *		pLm;
};

PRIVATE HTTabStop tabs_8[] = {
      { 0, 8 }, {0, 16}, {0, 24}, {0, 32}, {0, 40},
      { 0, 48 }, {0, 56}, {0, 64}, {0, 72}, {0, 80},
      { 0, 88 }, {0, 96}, {0, 104}, {0, 112}, {0, 120},
      { 0, 128 }, {0, 136}, {0, 144}, {0, 152}, {0, 160},
      {0, 168}, {0, 176},
      {0, 0 }         /* Terminate */
};

#define PUTC(c) (*text->targetClass.put_character)(text->target, c)
#define PUTS(s) (*text->targetClass.put_string)(text->target, s)

/*	Boring static variable used for moving cursor across
*/

#define SPACES(n) (&space_string[HTScreenWidth - (n)])
                                   /* String containing blank spaces only */
PRIVATE char * space_string;

PRIVATE HTStyle default_style =
	{ 0,  "(Unstyled)", "",
	(HTFont)0, 1.0, HT_BLACK,		0, 0,
	0, 0, 0, HT_LEFT,		1, 0,	tabs_8, 
	NO, NO, 0, 0,			0 };	


PUBLIC void clear_screen (void);	/* Forward */

PRIVATE HTList * loaded_texts;	/* A list of all those in memory */

/*			Creation Method
**			---------------
**
**	Interactive version
**
*/
extern LineMode * Context_getLineMode(HTRequest * request);
PUBLIC HText *	LMHText_new (
	HTRequest * request,
	HTParentAnchor * anchor,
	HTStream *outstrm)
{
    HTLine * line;
    HText * self;
    if ((self = (HText  *) HT_CALLOC(1, sizeof(*self))) == NULL)
/*        HT_OUTOFMEM("HText"); */
	return self;
    
    self->pLm = Context_getLineMode(request);
    if (!loaded_texts) loaded_texts = HTList_new();
    HTList_addObject(loaded_texts, self);
    if (HTList_count(loaded_texts) >= LOADED_LIMIT) {
        HTTRACE(CACHE_TRACE, "MemoryCache. Freeing off cached doc.\n"); 
        HText_free((HText *)HTList_removeFirstObject(loaded_texts));
    }
    
    if ((line = self->last_line = (HTLine *) HT_MALLOC(LINE_SIZE(MAX_LINE))) == NULL)
	HT_OUTOFMEM("HText_New");
    line->next = line->prev = line;
    line->offset = line->size = 0;
    self->lines = self->chars = 0;
    self->title = 0;
    self->first_anchor = self->last_anchor = self->current_anchor = 0;
    self->style = &default_style;
    self->top_of_screen = 0;
    self->node_anchor = anchor;
    self->last_anchor_number = 0;	/* Numbering of them for references */
    self->stale = YES;
    
    self->target = NULL;
    
    HTAnchor_setDocument(anchor, (void *) self);

    clear_screen();
    HTMainText = self;
    HTMainAnchor = anchor;
    self->display_on_the_fly = DISPLAY_LINES;
    self->all_pages = NO;	/* One page at a time on the fly */
    
    if (!space_string) {	/* Make a blank line */
        char *p;
	if ((space_string = (char  *) HT_MALLOC(HTScreenWidth+1)) == NULL)
	    HT_OUTOFMEM("HText_New");
        for (p=space_string; p<space_string+HTScreenWidth; p++) 
            *p = ' '; 		/* Used for printfs later */
        space_string[HTScreenWidth] = '\0'; 
    }
    
    return self;
}


/*			Creation Method 2
**			---------------
**
**	Non-interative  OR interactive if stream is NULL
**	Stream is assumed open and left open.
*/
PUBLIC HText *	LMHText_new2 (HTRequest *		request,
			    HTParentAnchor * 	anchor,
			    HTStream * 		stream)
{
    HText * me = LMHText_new(request, anchor, stream);
        
    if (stream) {
        me->target = stream;
	me->targetClass = *stream->isa;	/* copy action procedures */
	me->all_pages = YES;	/* Display whole file on the fly */    
    }
    return me;
}

/*	Free a data object
**	------------------
**	Removes the data object from the anchor
*/
PUBLIC void hyper_free (HText *  self)
{
    if (self) {
	while (1) {				      /* Free off line array */
	    HTLine * last = self->last_line;
	    if (last) {
		last->next->prev = last->prev;
		last->prev->next = last->next;
		self->last_line = last->prev;
		if (last == self->last_line) break;
		HT_FREE(last);
	    } else
		break;
	}
	while (self->first_anchor) {		    /* Free off anchor array */
	    TextAnchor * last = self->first_anchor;
	    self->first_anchor = last->next;
	    HT_FREE(last);
	}
	if (self == HTMainText) HTMainText = NULL;
	HT_FREE(self->last_line);
	HT_FREE(self);
    }
}


/*	Free Entire Text
**	----------------
*/
PUBLIC void 	HText_free (HText * self)
{
    if (self) {
	HTAnchor_setDocument(self->node_anchor, NULL);
	hyper_free(self);
    }
}

PUBLIC BOOL LMHText_delete (HText * self)
{
    if (self) {
	HTAnchor_setDocument(self->node_anchor, NULL);
	hyper_free(self);
	return YES;
    }
    return NO;
}

/*
**	Free all registered hypertext documents in memory
*/
PUBLIC BOOL HText_freeAll (void)
{
    if (loaded_texts) {
	HTList * cur = loaded_texts;
	HText * pres;
	while ((pres = (HText *) HTList_nextObject(cur)))
	    HText_free(pres);
	HTList_delete(loaded_texts);
	return YES;
    }
    return NO;
}


/*		Display Methods
**		---------------
*/

/*	Clear the screen (on screen-mode systems)
**	----------------
*/
PUBLIC void clear_screen (void)
{
    if (WWWTRACE)
	return;     		    /* in trace mode, don't clear trace away */
#ifdef CURSES
    if (w_text != NULL) {
	wmove(w_text,0,0);
	wclear(w_text);
    }
#endif /* Not CURSES */
    if (HTMainText) HTMainText->stale = YES;
}


/*	Output a line
**	-------------
*/
PRIVATE void display_line (HText * text, HTLine * line)
{
#ifdef CURSES
      int     y, x;

      waddstr(w_text, SPACES(line->offset));
      waddstr(w_text, line->data);
      getyx(w_text, y, x);
      if (y < DISPLAY_LINES-1) {
              wmove(w_text, ++y, 0);
      }
#else
   if (!text->target)
   {
#ifdef CYRILLIC
       /* HWL 18/7/94: applied patch from agl@glas2.glas.apc.org (Anton Tropashko) */
       a_print(SPACES(line->offset),H,stdout); 
       a_print(line->data,H,stdout);
       fputc('\n',stdout);
#else
       OutputData(LineMode_getView(text->pLm), "%s%s\n", SPACES(line->offset), line->data);
#endif
   }
   else {
       PUTS(SPACES(line->offset));
       PUTS(line->data);
       PUTC('\n');
   }
#endif
   
}

/*	Output the title line
**	---------------------
*/
PRIVATE void display_title (HText * text)
{
    const char * title = HTAnchor_title(text->node_anchor);
    char percent[20], format[20];
    if (text->lines > (DISPLAY_LINES-1)) {
#ifdef NOPE
	sprintf(percent, " (p%d of %d)",
	    (text->top_of_screen/(DISPLAY_LINES-1)) + 1,
	    (text->lines-1)/(DISPLAY_LINES-1) + 1);
	sprintf(percent, " (%d%%)",
	    100*(text->top_of_screen+DISPLAY_LINES-1)/	/* Seen */
	    	(text->lines));				/* Total */
#else
	sprintf(percent, " (%d/%d)",
	    text->top_of_screen+DISPLAY_LINES-1,	/* Seen */
	    text->lines);				/* Total */
#endif
    } else {
	percent[0] = 0;	/* Null string */
    }

    sprintf(format, "%%%d.%ds%%s\n",	/* Generate format string */
		    (int)(HTScreenWidth-strlen(percent)),
		    (int)(HTScreenWidth-strlen(percent)) );
#ifdef CURSES
    mvwprintw(w_top, 0, 0, format, title, percent);
    wrefresh(w_top);
#else
    if (!text->target) OutputData(LineMode_getView(text->pLm), format, title, percent);
    else {
    	char * line;
    	if ((line = (char *) HT_MALLOC(HTScreenWidth+10)) == NULL)
    	    HT_OUTOFMEM("display_titile");
        sprintf(line, format, title, percent);
	PUTS(line);
	HT_FREE(line);
    }
#endif
}


/*	Fill the screen with blank after the file
**	--------------------------
*/
PRIVATE void fill_screen (HText *  text, int n)
{
    if (n<=0 || n>1000) return;		/* Large value means no pagination */
    if (text->target) return;
#ifdef CURSES
    waddstr(w_text, end_mark);
    wclrtobot(w_text);
    wrefresh(w_text);
#else
#ifndef VIOLA    
    if (!text->target) OutputData(LineMode_getView(text->pLm), "%s\n", end_mark);
    else { PUTS(end_mark); PUTC('\n'); }
    n--;
    
    for (; n; n--) {
        if (!text->target) OutputData(LineMode_getView(text->pLm), "\n");
	else PUTC('\n');
    }
#endif
#endif        /* Not CURSES */
}


/*	Output a page
**	-------------
*/
PRIVATE void display_page (HText * text, int line_number)
{
    HTLine * line = text->last_line->prev;
    int i;
    const char * title = HTAnchor_title(text->node_anchor);
    int lines_of_text = title ? (DISPLAY_LINES-TITLE_LINES) : DISPLAY_LINES;
    int last_screen = text->lines - lines_of_text;

/*	Constrain the line number to be within the document
*/
    if (text->lines <= lines_of_text) line_number = 0;
    else if (line_number>last_screen) line_number = last_screen;
    else if (line_number < 0) line_number = 0;
    
    for(i=0,  line = text->last_line->next;		/* Find line */
    	i<line_number && (line!=text->last_line);
      i++, line=line->next) /* Loop */ assert(line->next != NULL);

    while((line!=text->last_line) && (line->size==0)) {	/* Skip blank lines */
        assert(line->next != NULL);
      line = line->next;
        line_number ++;
    }

/*	Can we just scroll to it?
*/ 
#ifndef VM			/* No scrolling */
    if (!text->stale && (line_number>=text->top_of_screen) &&
    	(line_number < text->top_of_screen + DISPLAY_LINES)) {	/* Yes */
	lines_of_text = line_number - text->top_of_screen;
	line = text->next_line;
        text->top_of_screen = line_number;
#ifdef CURSES
        scrollok(w_text, TRUE);
        for (i = 0; i < lines_of_text; i++) {
              scroll(w_text);
        }
#endif  
    } else
#endif
    {
	clear_screen();						/* No */
        text->top_of_screen = line_number;
	if (title) display_title(text);
    }
    
/* Bug: when we scroll to a part slightly futher down a page which previously
 fitted all on one screen including the [End], the code below will add an
 extra [End] to the screen, giving a total of two, one underneath the other.
*/
    
 /*	print it
 */
    if (line) {
      for(i=0;
	  (i< lines_of_text) && (line != text->last_line); i++)  {
      assert(line != NULL);
        display_line(text, line);
	line = line->next;
      }
      fill_screen(text, lines_of_text - i);
    }

    text->next_line = line;	/* Line after screen */
    text->stale = NO;		/* Display is up-to-date */
}


/*			Object Building methods
**			-----------------------
**
**	These are used by a parser to build the text in an object
*/
PUBLIC void HText_beginAppend (HText * text)
{
    text->permissible_split = 0;
    text->in_line_1 = YES;
}


/*	Add a new line of text
**	----------------------
**
** On entry,
**
**	split	is zero for newline function, else number of characters
**		before split.
**	text->display_on_the_fly
**		may be set to indicate direct output of the finished line.
**	text->all_pages
**		if set indicates all pages are to be done on the fly.
** On exit,
**		A new line has been made, justified according to the
**		current style. Text after the split (if split nonzero)
**		is taken over onto the next line.
**
**		If display_on_the_fly is set, then it is decremented and
**		the finished line is displayed.
*/
#define new_line(text) split_line(text, 0)

PRIVATE void split_line (HText * text, int split)
{
    HTStyle * style = text->style;
    int spare;
    int indent = (int) (text->in_line_1 ? text->style->indent1st
			: text->style->leftIndent);

/*	Make new line
*/
    HTLine * previous = text->last_line;
    HTLine * line;
    if ((line = (HTLine  *) HT_MALLOC(LINE_SIZE(MAX_LINE))) == NULL)
        HT_OUTOFMEM("split_line");

    text->lines++;
    
    previous->next->prev = line;
    line->prev = previous;
    line->next = previous->next;
    previous->next = line;
    text->last_line = line;
    line->size = 0;
    line->offset = 0;

/*	Split at required point
*/    
    if (split) {	/* Delete space at "split" splitting line */
	char * p;
	previous->data[previous->size] = 0;
	for (p = &previous->data[split]; *p; p++)
	    if (*p != ' ') break;
	strcpy(line->data, p);
	line->size = strlen(line->data);
	previous->size = split;
    }
    
    while ((previous->size > 0) &&
	   (previous->data[previous->size-1] == ' '))	/* Strip trailers */
        previous->size--;

    if ((previous = (HTLine *)
	 HT_REALLOC(previous, LINE_SIZE(previous->size)))==NULL)
	HT_OUTOFMEM("split_line");
    previous->prev->next = previous;	/* Link in new line */
    previous->next->prev = previous;	/* Could be same node of course */

/*	Terminate finished line for printing
*/
    previous->data[previous->size] = 0;

/*	Align left, right or center
*/

    spare =  (int) (HTScreenWidth - style->rightIndent + style->leftIndent -
		    previous->size);		     /* @@ first line indent */
		
    switch (style->alignment) {
	case HT_CENTER :
	    previous->offset = previous->offset + indent + spare/2;
	    break;
	case HT_RIGHT :
	    previous->offset = previous->offset + indent + spare;
	    break;
	case HT_LEFT :
	case HT_JUSTIFY :		/* Not implemented */
	default:
	    previous->offset = previous->offset + indent;
	    break;
    } /* switch */

    text->chars = text->chars + previous->size + 1;	/* 1 for the line */
    text->in_line_1 = NO;		/* unless caller sets it otherwise */
    
/*	If displaying as we go, output it:
*/
    if (text->display_on_the_fly) {
        if (text->display_on_the_fly == DISPLAY_LINES) { /* First line */
	    if (previous->size == 0) {
	        text->top_of_screen++;	/* Scroll white space off top */
		return;
	    }
	    if (HTAnchor_title(text->node_anchor)) { /* Title exists */
	        display_title(text);
	        text->display_on_the_fly--;
	    }
	}
	display_line(text, previous);
	text->display_on_the_fly--;
	
	/* Loop to top of next page? */
	if (!text->display_on_the_fly && text->all_pages) {
	    PUTS("\f\n"); /* Form feed on its own line a la rfc1111 */
	    text->display_on_the_fly = DISPLAY_LINES;
	}
    }
} /* split_line */


/*	Allow vertical blank space
**	--------------------------
*/
PRIVATE void blank_lines (HText * text, int newlines)
{
    if (text->last_line->size == 0) {	/* No text on current line */
	HTLine * line = text->last_line->prev;
	while ((line!=text->last_line) && (line->size == 0)) {
	    if (newlines==0) break;
	    newlines--;		/* Don't bother: already blank */
	    line = line->prev;
	}
    } else {
	newlines++;			/* Need also to finish this line */
    }

    for(;newlines;newlines--) {
	new_line(text);
    }
    text->in_line_1 = YES;
}


/*	New paragraph in current style
**	------------------------------
** See also: setStyle.
*/

PUBLIC void HText_appendParagraph (HText * text)
{
    int after = (int) text->style->spaceAfter;
    int before = (int) text->style->spaceBefore;
    blank_lines(text, after>before ? after : before);
}


/*	Set Style
**	---------
**
**	Does not filter unnecessary style changes.
*/
PUBLIC void HText_setStyle (HText * text, HTStyle * style)
{
    int after, before;

    if (!style) return;				/* Safety */
    after = (int) text->style->spaceAfter;
    before = (int) style->spaceBefore;
    HTTRACE(SGML_TRACE, "Rendering... Change to style %s\n" _ style->name);
    blank_lines (text, after>before ? after : before);
    text->style = style;
}


/*	Append a character to the text object
**	-------------------------------------
*/
PUBLIC void HText_appendCharacter (HText * text, char ch)
{
    HTLine * line = text->last_line;
    HTStyle * style = text->style;
    int indent = (int)(text->in_line_1 ? style->indent1st : style->leftIndent);
    
/*		New Line
*/
    if (ch == '\n') {
	    new_line(text);
	    text->in_line_1 = YES;	/* First line of new paragraph */
	    return;
    }

/* 		Tabs
*/

    if (ch == '\t') {
        HTTabStop * tab;
	int target;	/* Where to tab to */
	int here = line->size + line->offset +indent;
        if (style->tabs) {	/* Use tab table */
	    for (tab = style->tabs;
	    	tab->position <= here;
		tab++)
		if (!tab->position) {
		    new_line(text);
		    return;
		}
	    target = (int) tab->position;
	} else if (text->in_line_1) {	/* Use 2nd indent */
	    if (here >= style->leftIndent) {
	        new_line(text); /* wrap */
		return;
	    } else {
	        target = (int) style->leftIndent;
	    }
	} else {		/* Default tabs align with left indent mod 8 */
#ifdef DEFAULT_TABS_8
	    target = ((line->offset + line->size + 8) & (-8))
	    		+ style->leftIndent;
#else
	    new_line(text);
	    return;
#endif
	}
	if (target > HTScreenWidth - style->rightIndent) {
	    new_line(text);
	    return;
	} else {
            text->permissible_split = line->size;	/* Can split here */
	    if (line->size == 0) line->offset = line->offset + target - here;
	    else for(; here<target; here++) {
                line->data[line->size++] = ' ';	/* Put character into line */
	    }
	    return;
	}
	/*NOTREACHED*/
    } /* if tab */ 

    
    if (ch==' ') {
        text->permissible_split = line->size;	/* Can split here */
    }

/*	Check for end of line
*/    
    if (indent + line->offset + line->size + style->rightIndent
    		>= HTScreenWidth) {
        if (style->wordWrap) {
	    if(text->permissible_split > line->size)	/* HENRIK 21/02-94 */
		text->permissible_split = line->size;
	    split_line(text, text->permissible_split);
	    if (ch==' ') return;	/* Ignore space causing split */
	} else new_line(text);
    }

/*	Insert normal characters
*/
    if (ch == HT_NON_BREAK_SPACE) {
        ch = ' ';
    }
    {
        HTLine * line = text->last_line;	/* May have changed */
        HTFont font = style->font;
        line->data[line->size++] =	/* Put character into line */
           font & HT_CAPITALS ? TOUPPER(ch) : ch;
        if (font & HT_DOUBLE)		/* Do again if doubled */
            HText_appendCharacter(text, HT_NON_BREAK_SPACE);
	    /* NOT a permissible split */ 
    }
}

PUBLIC void HText_appendText (HText * text, const char * str)
{
    const char * p;
    for(p=str; *p; p++) {
        HText_appendCharacter(text, *p);
    }
}


PUBLIC void HText_endAppend (HText * text)
{
    new_line(text);
    
    if (text->display_on_the_fly) {		/* Not finished? */
        fill_screen(text, text->display_on_the_fly);	/* Finish it */
	text->display_on_the_fly = 0;
	text->next_line = text->last_line;	/* Bug fix after EvA 920117 */
	text->stale = NO;
    }
}

/*		Anchor handling
**		---------------
*/
/*	Start an anchor field
*/
PUBLIC void LMHText_beginAnchor (HText * text,
    int elem_num, int attr_num, HTChildAnchor * anc,
    const BOOL *present, const char **value)
{
    TextAnchor * a;

			/* this is because it's called as link callback */
    if (elem_num != HTML_A)
	return;

    if ((a = (TextAnchor  *) HT_MALLOC(sizeof(*a))) == NULL)
        HT_OUTOFMEM("HText_beginAnchor");
    a->start = text->chars + text->last_line->size;
    a->extent = 0;
    if (text->last_anchor) {
        text->last_anchor->next = a;
    } else {
        text->first_anchor = a;
    }
    a->next = 0;
    a->anchor = anc;
    text->last_anchor = a;
     text->current_anchor = a;
    
    if (HTAnchor_followMainLink((HTAnchor*)anc)) {
        a->number = ++(text->last_anchor_number);
    } else {
        a->number = 0;
    }
}


PUBLIC void LMHText_endAnchor (HText * text)
{
    TextAnchor * a = text->current_anchor;
    char marker[100];

    if (!a)			/* </A> without <A> */
	return;

    if (a->number && display_anchors) {	 /* If it goes somewhere */
	sprintf(marker, end_reference, a->number);
	HText_appendText(text, marker);
    }
    a->extent = text->chars + text->last_line->size - a->start;
    text->current_anchor = 0;
}


/* LMHText_addText() satisfies HText callback requirement.  */
PUBLIC void LMHText_addText (HText * text, const char * str, int length)
{
    const char * p;
    int i;
    for (i=0,p=str; i<length; ++i,++p) {
        HText_appendCharacter(text, *p);
    }
}

/*		IMAGES
*/
PUBLIC void HText_appendImage (
	HText * 		text,
	HTChildAnchor * 	anc,
	const char * 		alt,
	const char *  		alignment,
	BOOL			isMap)
{
    HText_appendText(text, alt? alt : "[IMAGE]");
}

PUBLIC void HText_appendObject (HText * text, int element_number,
	                        const BOOL * present, const char ** value)
{
}

PUBLIC void HText_appendLink (HText * text, HTChildAnchor * anchor,
			      const BOOL * present, const char ** value)
{
}

/*	Return the anchor associated with this node
*/
PUBLIC HTParentAnchor * HText_nodeAnchor (HText * text)
{
    return text->node_anchor;
}

/*				GridText specials
**				=================
*/
/*	Return the anchor with index N
**
**	The index corresponds to the number we print in the anchor.
*/
PUBLIC HTChildAnchor * HText_childNumber (HText * text, int number)
{
    TextAnchor * a;
    for (a = text->first_anchor; a; a = a->next) {
        if (a->number == number) return a->anchor;
    }
    return (HTChildAnchor *)0;	/* Fail */
}

PUBLIC void HText_setStale (HText * text)
{
    if (text)
	text->stale = YES;
}

PUBLIC void HText_refresh (HText * text)
{
    if (text && text->stale)
	display_page(text, text->top_of_screen);
}

PUBLIC int HText_sourceAnchors (HText * text)
{
    return (text ? text->last_anchor_number : -1);
}

PUBLIC BOOL HText_canScrollUp (HText * text)
{
    return (text && text->top_of_screen != 0);
}

PUBLIC BOOL HText_canScrollDown (HText * text)
{
    return (text && (text->top_of_screen + DISPLAY_LINES -
		     (text->title ? TITLE_LINES : 0)) < text->lines);
}

/*		Scroll actions
*/
PUBLIC void HText_scrollTop (HText * text)
{
    display_page(text, 0);
}

PUBLIC void HText_scrollDown (HText * text)
{
    display_page(text, text->top_of_screen + DISPLAY_LINES -1);
}

PUBLIC void HText_scrollUp (HText * text)
{
    display_page(text, text->top_of_screen - DISPLAY_LINES +1);
}

PUBLIC void HText_scrollBottom (HText * text)
{
    display_page(text, text->lines - DISPLAY_LINES +1);
}


/*		Browsing functions
**		==================
*/

/* Bring to front and highlight it
*/

PRIVATE int line_for_char (HText * text, int char_num)
{
    int line_number =0;
    int characters = 0;
    HTLine * line = text->last_line->next;
    for(;;) {
	if (line == text->last_line) return 0;	/* Invalid */
        characters = characters + line->size + 1;
	if (characters > char_num) return line_number;
	line_number ++;
	line = line->next;
    }
}

PUBLIC BOOL HText_select (HText * text)
{
    if (text) {
        HTMainText = text;
	HTMainAnchor = text->node_anchor;
	display_page(text, text->top_of_screen);
	return YES;
    }
    HTTRACE(SGML_TRACE, "Rendering... Nothing to select!\n");
    return NO;
}

PUBLIC BOOL HText_selectAnchor (HText * text, HTChildAnchor * anchor)
{
    TextAnchor * a;

    for(a=text->first_anchor; a; a=a->next) {
        if (a->anchor == anchor) break;
    }
    if (!a) {
        HTTRACE(SGML_TRACE, "Rendering... No such anchor in this text!\n");
        return NO;
    }

    if (text != HTMainText) {		/* Comment out by ??? */
        HTMainText = text;		/* Put back in by tbl 921208 */
	HTMainAnchor = text->node_anchor;
    }

    {
	int l = line_for_char(text, a->start);
	HTTRACE(SGML_TRACE, "Rendering... Selecting anchor [%d] at char %d, line %d\n" _ 
		    a->number _ a->start _ l);

	if ( !text->stale &&
	    (l >= text->top_of_screen) &&
	    ( l < text->top_of_screen + DISPLAY_LINES-1))
	    return YES;
        display_page(text, l - (DISPLAY_LINES/3));	/* Scroll to it */
    }
    return YES;
}
 

/*		Editing functions		- NOT IMPLEMENTED
**		=================
**
**	These are called from the application. There are many more functions
**	not included here from the orginal text object.
*/

/*	Style handling:
*/
/*	Apply this style to the selection
*/
PUBLIC void HText_applyStyle (HText *  me, HTStyle * style)
{
    
}


/*	Update all text with changed style.
*/
PUBLIC void HText_updateStyle (HText *  me, HTStyle * style)
{
    
}


/*	Return style of  selection
*/
PUBLIC HTStyle * HText_selectionStyle (
	HText * me,
	HTStyleSheet * sheet)
{
    return 0;
}


/*	Paste in styled text
*/
PUBLIC void HText_replaceSel (
	HText * me,
	const char * aString, 
	HTStyle * aStyle)
{
}


/*	Apply this style to the selection and all similarly formatted text
**	(style recovery only)
*/
PUBLIC void HTextApplyToSimilar (HText * me, HTStyle * style)
{
    
}

 
/*	Select the first unstyled run.
**	(style recovery only)
*/
PUBLIC void HTextSelectUnstyled (HText * me, HTStyleSheet * sheet)
{
    
}


/*	Anchor handling:
*/
PUBLIC void		HText_unlinkSelection (HText * me)
{
    
}

PUBLIC HTAnchor *	HText_referenceSelected (HText * me)
{
     return 0;   
}


#ifdef CURSES
PUBLIC int HText_getTopOfScreen (HText * text)
{
      return text->top_of_screen;
}

PUBLIC int HText_getLines (HText * text)
{
      return text->lines;
}
#endif
PUBLIC HTAnchor *	HText_linkSelTo (HText * me, HTAnchor * anchor)
{
    return 0;
}

/*	HTML callback functions
*/
PUBLIC void LMHText_beginElement (HText * text,
    int elem_num, const BOOL * present, const char ** value)
{
    return;
}

PUBLIC void LMHText_endElement (HText * text, int elem_num)
{
    switch (elem_num) {
    case HTML_A:
	LMHText_endAnchor (text);
	break;
    default:
	break;
    }
    return;   
}
