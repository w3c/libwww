/*                                                     Style Management for Line Mode Browser
                              STYLE DEFINITION FOR HYPERTEXT
                                             
 */
/*
**      (c) COPYRIGHT MIT 1995.
**      Please first read the full copyright statement in the file COPYRIGH.
*//*

   This module defines the HText functions referenced from the HTML module in the W3C
   Reference Library.
   
 */
#include "WWWLib.h"

extern HTChildAnchor * HText_childNumber PARAMS((HText * text, int n));

/* delete a hyper doc object with interfering with any anchors */
extern void hyperfree PARAMS((HText * me));

/* Is there any file left? */
extern BOOL HText_canScrollUp PARAMS((HText * text));
extern BOOL HText_canScrollDown PARAMS((HText * text));

/* Move display within window */
extern void HText_scrollUp PARAMS((HText * text));      /* One page */
extern void HText_scrollDown PARAMS((HText * text));    /* One page */
extern void HText_scrollTop PARAMS((HText * text));
extern void HText_scrollBottom PARAMS((HText * text));

extern int HText_sourceAnchors PARAMS((HText * text));
extern void HText_setStale PARAMS((HText * text));
extern void HText_refresh PARAMS((HText * text));

#ifdef CURSES
extern int HText_getTopOfScreen PARAMS((HText * text));
extern int HText_getLines PARAMS((HText * text));
#endif
/*

   End of declaration  */

