/*	Specialities of GridText as subclass of HText
*/
#include "HText.h"		/* Superclass */

#ifdef SHORT_NAMES
#define HText_childNumber		HTGTChNu
#define HText_canScrollUp		HTGTCaUp
#define HText_canScrollDown		HTGTCaDo
#define HText_scrollUp			HTGTScUp
#define HText_scrollDown		HTGTScDo
#define HText_scrollTop			HTGTScTo
#define HText_scrollBottom		HTGTScBo
#define HText_sourceAnchors		HTGTSoAn
#define HText_setStale			HTGTStal
#define HText_refresh			HTGTRefr
#endif


extern HTChildAnchor * HText_childNumber PARAMS((HText * text, int n));

/*	Is there any file left?
*/
extern BOOL HText_canScrollUp PARAMS((HText * text));
extern BOOL HText_canScrollDown PARAMS((HText * text));

/*	Move display within window
*/
extern void HText_scrollUp PARAMS((HText * text));	/* One page */
extern void HText_scrollDown PARAMS((HText * text));	/* One page */
extern void HText_scrollTop PARAMS((HText * text));
extern void HText_scrollBottom PARAMS((HText * text));

extern int HText_sourceAnchors PARAMS((HText * text));
extern void HText_setStale PARAMS((HText * text));
extern void HText_refresh PARAMS((HText * text));

#ifdef CURSES
extern int HText_getTopOfScreen PARAMS((HText * text));
extern int HText_getLines PARAMS((HText * text));
#endif
