/*	Style Definition for Hypertext				HTStyle.h
**	==============================
**
**	Styles allow the translation between a logical property of a piece of
**	text and its physical representation.
**
**	A StyleSheet is a collection of styles, defining the
**	translation necessary to represent a document.
**	It is a linked list of styles.
*/
#ifndef HTStyle_H
#define HTStyle_H

#include "HTUtils.h"
#include "HTAnchor.h"

/* #include "HTFont.h"		bad dependency */
typedef long int HTFont;	/* Dummy definition instead */

#ifdef SHORT_NAMES
#define HTStyleNew			HTStNew
#define HTStyleFree			HTStFree
#define HTStyleRead			HTStRead
#define HTStyleWrite			HTStWrite
#define HTStyleSheetNew			HTStShNe
#define HTStyleSheetFree		HTStShFr
#define HTStyleNamed			HTStName
#define HTStyleForParagraph		HTStFoPa
#define HTStyleMatching			HTStMatc
#define HTStyleForRun			HTStFoRu
#define HTStyleSheetAddStyle		HTStShAd
#define HTStyleSheetRemoveStyle		HTStShRm
#define HTStyleSheetRead		HTStShRe
#define HTStyleSheetWrite		HTStShWr
#endif

#ifdef NeXT_suppressed
#include <appkit/appkit.h>
typedef NXCoord HTCoord;
#define HTParagraphStyle NXTextStyle
#define HTCoord NXCoord
typedef struct _color {
	float	grey;
	int	RGBColor;
} HTColor;
#else

typedef float HTCoord;

typedef struct _HTParagraphStyle {
    HTCoord	left_indent;		/* @@@@ junk! etc etc*/
} HTParagraphStyle;

typedef int HTColor;		/* Sorry about the US spelling! */

#endif



#define STYLE_NAME_LENGTH	80	/* @@@@@@@@@@@ */
	 
typedef struct {
    short           	kind;		/* only NX_LEFTTAB implemented*/
    HTCoord         	position;	/* x coordinate for stop */
} HTTabStop;


/*	The Style Structure
**	-------------------
*/

typedef struct _HTStyle {

/*	Style management information
*/
    struct _HTStyle	*next;		/* Link for putting into stylesheet */
    char *		name;		/* Style name */
    char *		SGMLTag;	/* Tag name to start */


/*	Character attributes	(a la NXRun)
*/
    HTFont 	    	font;		/* Font id */
    HTCoord		fontSize;	/* The size of font, not independent */
    HTColor	    	color;	/* text gray of current run */
    int   		superscript;	/* superscript (-sub) in points */

    HTAnchor		*anchor;	/* Anchor id if any, else zero */

/*	Paragraph Attribtes	(a la NXTextStyle)
*/
    HTCoord		indent1st;	/* how far first line in paragraph is
				 * indented */
    HTCoord		leftIndent;	/* how far second line is indented */
    HTCoord		rightIndent;	/* (Missing from NeXT version */
    short       	alignment;	/* quad justification */
    HTCoord		lineHt;		/* line height */
    HTCoord     	descentLine;	/* descender bottom from baseline */
    HTTabStop   	*tabs;		/* array of tab stops, 0 terminated */

    BOOL		wordWrap;	/* Yes means wrap at space not char */
    BOOL		freeFormat;	/* Yes means \n is just white space */
    HTCoord		spaceBefore;	/* Omissions from NXTextStyle */
    HTCoord		spaceAfter;
    int			paraFlags;	/* Paragraph flags, bits as follows: */

#define PARA_KEEP	1	/* Do not break page within this paragraph */
#define	PARA_WITH_NEXT	2	/* Do not break page after this paragraph */

#define HT_JUSTIFY 0		/* For alignment */
#define HT_LEFT	1
#define HT_RIGHT 2
#define HT_CENTER 3
  
} HTStyle;


/*	Style functions:
*/
extern HTStyle * HTStyleNew NOPARAMS;
extern HTStyle* HTStyleNewNamed PARAMS ((CONST char * name));
extern HTStyle * HTStyleFree PARAMS((HTStyle * self));
#ifdef SUPRESS
extern HTStyle * HTStyleRead PARAMS((HTStyle * self, HTStream * stream));
extern HTStyle * HTStyleWrite PARAMS((HTStyle * self, HTStream * stream));
#endif
/*		Style Sheet
**		-----------
*/
typedef struct _HTStyleSheet {
	char *		name;
	HTStyle *	styles;
} HTStyleSheet;


/*	Stylesheet functions:
*/
extern HTStyleSheet * HTStyleSheetNew NOPARAMS;
extern HTStyleSheet * HTStyleSheetFree PARAMS((HTStyleSheet * self));
extern HTStyle * HTStyleNamed PARAMS((HTStyleSheet * self, CONST char * name));
extern HTStyle * HTStyleForParagraph PARAMS((HTStyleSheet * self,
	HTParagraphStyle * paraStyle));
extern HTStyle * HTStyleMatching PARAMS((HTStyleSheet *self, HTStyle * style));
/* extern HTStyle * HTStyleForRun PARAMS((HTStyleSheet *self, NXRun * run)); */
extern HTStyleSheet * HTStyleSheetAddStyle PARAMS((HTStyleSheet * self,
	HTStyle * style));
extern HTStyleSheet * HTStyleSheetRemoveStyle PARAMS((HTStyleSheet * self,
	HTStyle * style));
#ifdef SUPPRESS
extern HTStyleSheet * HTStyleSheetRead PARAMS((HTStyleSheet * self,
						HTStream * stream));
extern HTStyleSheet * HTStyleSheetWrite PARAMS((HTStyleSheet * self,
						HTStream * stream));
#endif
#define CLEAR_POINTER ((void *)-1)	/* Pointer value means "clear me" */
#endif /* HTStyle_H */
