/*                  /Net/dxcern/userd/timbl/hypertext/WWW/Library/Implementation/HTMLDTD.html
 */

/*              HTML Definition Interface                       HTMLDTD.h
**              =========================
**
**      SGML purists should excuse the use of the term "DTD" in this
**      file to represent DTD-related information which is not exactly
**      a DTD itself.
**
**      The C modular structure doesn't work very well here, as the
**      dtd is partly in the .h and partly in the .c which are not very
**      independent.  Tant pis.
*/

#ifndef HTMLDTD_H
#define HTMLDTD_H

#include "HTUtils.h"
#include "SGML.h"

/*      Element Numbers
**      ---------------
**
**      Must Match all tables by element!
*/
typedef enum _HTMLElement {
        HTML_A,         HTML_ADDRESS,   HTML_BLOCKQUOTE, HTML_COMMENT,
        HTML_DD,        HTML_DIR,       HTML_DL,        HTML_DLC,
        HTML_DT,
        HTML_H1,        HTML_H2,        HTML_H3,
        HTML_H4,        HTML_H5,        HTML_H6,        HTML_H7,
        HTML_ISINDEX,
        HTML_LI,        HTML_LINK,      HTML_LISTING,
        HTML_NEXTID,    HTML_MENU,
        HTML_OL,        HTML_P,         HTML_PLAINTEXT, HTML_PRE,
        HTML_TITLE,     HTML_UL,        HTML_XMP } HTMLElement;

#define HTML_ELEMENTS 29

/*      Attribute numbers
**      -----------------
**
**      Name is 60;element>_60;attribute>.  These must match the tables in .c!
*/
#define HTML_A_HREF             0
#define HTML_A_NAME             1
#define HTML_A_TITLE            2
#define HTML_A_TYPE             3
#define HTML_A_URN              4
#define HTML_A_ATTRIBUTES       5

#define DL_COMPACT 0

#define NEXTID_N 0

extern CONST SGML_dtd HTML_dtd;


/*

Start anchor element

   It is kinda convenient to have a particulr routine for starting an anchor element, as
   everything else for HTML is simple anyway.
   
  ON ENTRY
  
   targetstream poinst to a structured stream object.
   
   name and href point to attribute strings or are NULL if the attribute is to be omitted.
   
 */
extern void HTStartAnchor PARAMS((
                HTStructured * targetstream,
                CONST char *    name,
                CONST char *    href));


#endif /* HTMLDTD_H */

/*

   End of module definition  */
