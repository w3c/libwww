/*                                                          Main Module for Line Mode Browser
                            MAIN MODULE FOR LINE MODE BROWSER
                                             
 */
/*
**      (c) COPYRIGHT MIT 1995.
**      Please first read the full copyright statement in the file COPYRIGH.
*/
/*

   This module defines the global variables etc. for the Line Mode Browser. The module is
   implemented by HTBrowse.c, and it is a part of the Line Mode Browser.
   
 */
#ifndef HTBROWSE_H
#define HTBROWSE_H

extern int      HTScreenWidth;
extern int      HTScreenHeight;

/* Anchor specific information */
extern BOOL     display_anchors;                    /* Show anchors in text? */
extern char *   start_reference;                  /* Format for start anchor */
extern char *   end_reference;                             /* for end anchor */
extern char *   reference_mark;                       /* for reference lists */
extern char *   end_mark;                         /* Format string for [End] */

typedef struct _LineMode LineMode;
typedef struct _HTView HTView;

extern int OutputData(HTView * pView, const char  * fmt, ...);
#endif /* HTBROWSE_H */

/* for access to windows app's windows procedures */
typedef int InputParser_t (char* buf, SOCKET s, HTRequest * req, SockOps ops);
extern InputParser_t * PInputParser;
PUBLIC int bufferInput (char* buf, int len, SOCKET s, HTRequest * req, SockOps ops);
extern LineMode * Context_getLineMode(HTRequest * request);
extern HTRequest * LineMode_getConsole(LineMode * pLm);
extern HTView * LineMode_getView(LineMode * pLm);
extern HTView * HTView_create(char* name, int rows, int cols, LineMode * pLm);
extern BOOL HTView_destroy(HTView * pView);
/*

   End of HTBrowse Declaration */
