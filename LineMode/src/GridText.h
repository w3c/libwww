/*                                                     Style Management for Line Mode Browser
                              STYLE DEFINITION FOR HYPERTEXT
                                             
 */
/*
**      (c) COPYRIGHT MIT 1995.
**      Please first read the full copyright statement in the file COPYRIGH.
*/
/*

   W3C Sample Code Library.
   
 */
#include "WWWLib.h"
#include "HText.h"

extern HTChildAnchor * HText_childNumber (HText * text, int n);

/* delete a hyper doc object with interfering with any anchors */
extern void hyperfree (HText * me);

/* Is there any file left? */
extern BOOL HText_canScrollUp (HText * text);
extern BOOL HText_canScrollDown (HText * text);

/* Move display within window */
extern void HText_scrollUp (HText * text);      /* One page */
extern void HText_scrollDown (HText * text);    /* One page */
extern void HText_scrollTop (HText * text);
extern void HText_scrollBottom (HText * text);

extern int HText_sourceAnchors (HText * text);
extern void HText_setStale (HText * text);
extern void HText_refresh (HText * text);

#ifdef CURSES
extern int HText_getTopOfScreen (HText * text);
extern int HText_getLines (HText * text);
#endif

/*

DELETE ALL DOCUMENTS IN MEMORY

 */
extern BOOL HText_freeAll (void);
/*

   */
