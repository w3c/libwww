/*                                                          Main Module for Line Mode Browser
                            MAIN MODULE FOR LINE MODE BROWSER
                                             
 */
/*
**      (c) COPYRIGHT MIT 1995.
**      Please first read the full copyright statement in the file COPYRIGH.
*//*

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

#endif /* HTBROWSE_H */
/*

   End of HTBrowse Declaration */

