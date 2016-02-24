/*

					W3C Sample Code Library libwww DIRECTORY BROWSING




!Directory Browsing!

*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The directory manager generates directory listings for FTP and HTTP
requests. This module contains the protocol independent code and it
produces the HTML object. It is only included if either the FTP or the File module
is included. 

This module is implemented by HTDir.c, and it is
a part of the  W3C
Sample Code Library.

*/

#ifndef HTDIR_H
#define HTDIR_H
#include "HTReq.h"
#include "HTIcons.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*

.What Should the Listings Look Like?.

If the HT_DS_DES is set then the Description
field is added as the last column in the listing.  File descriptions
are queried from the HTDescript
module. Make a full mask by adding/oring the following flags:

*/

typedef enum _HTDirShow {
    HT_DS_SIZE  = 0x1,			/* Show file size using K, M and G? */
    HT_DS_DATE  = 0x2,			/* Show last modified date? */
    HT_DS_HID   = 0x4,			/* Show hidden files? */
    HT_DS_DES	= 0x8,			/* Show descriptions? */
    HT_DS_ICON  = 0x10,			/* Show icons? */
    HT_DS_HOTI  = 0x20			/* Are Icons hot or cold? */
} HTDirShow;

typedef enum _HTDirKey {
    HT_DK_NONE	= 0,			/* No sorting */
    HT_DK_CSEN	= 1,			/* Case sensitive */
    HT_DK_CINS  = 2			/* Case insensitive */
} HTDirKey;

/*

(Length of Filenames and Descriptions)

The module automatically ajusts the width of the directory listing as
a function of the file name. The width can flows dynamically between
an upper and a lower limit.  The maximum length of
this field is specified by

*/

extern BOOL HTDir_setWidth (int minfile, int maxfile);

/*

.The Directory Object.

The directory object handles the generation of a directory listing. It
is a bit like a stream in that it accept data, but it must be
formatted in a special way which makes the normal stream architecture
inadequate.

(Width of File Names)

The module automatically ajusts the width of the directory listing as
a function of the file name. The width can flows dynamically between
an upper and a lower limit.

*/

extern BOOL HTDir_setWidth (int minfile, int maxfile);

/*

(Create a Directory Object)

Creates a structured stream object and sets up the initial HTML stuff
Returns the dir object if OK, else NULL

*/

typedef struct _HTDir HTDir;

extern HTDir * HTDir_new (HTRequest * request, HTDirShow show, HTDirKey key);

/*

(Add a Line to the List)

This function accepts a directory line. "data" and "size", and
"description" can all be NULL. Returns YES if OK, else NO

*/

extern BOOL HTDir_addElement	(HTDir *dir, char *name, char *date,
				 char *size, HTFileMode mode);

/*

(Free a Directory Obejct)

If we are sorting then do the sorting and put out the list,
else just append the end of the list.

*/

extern BOOL HTDir_free (HTDir * dir);

/*

*/

#ifdef __cplusplus
}
#endif

#endif /* HTDIR */

/*



@(#) $Id$


*/
