/*

  
  
  					W3C Sample Code Library libwww Local File Access


!
  Local File Access
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

These are routines for local file access used by WWW browsers and servers.

This module is implemented by HTFile.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTFILE_H
#define HTFILE_H

#include "HTProt.h"

#ifdef __cplusplus
extern "C" { 
#endif 

extern HTProtCallback HTLoadFile;

/*
.
  Directory Access
.

You can define the directory access for file URLs by using the following
function.
*/

typedef enum _HTDirAccess {
    HT_DIR_FORBID,			/* Altogether forbidden */
    HT_DIR_SELECTIVE,			/* Only if "selfile" exists */
    HT_DIR_OK				/* Directory reading always OK */
} HTDirAccess;

#define DEFAULT_DIR_FILE	".www_browsable"    /* If exists, can browse */

extern HTDirAccess  HTFile_dirAccess	(void);
extern BOOL HTFile_setDirAccess		(HTDirAccess mode);

/*
.
  Readme Files
.

You can specify the module to look for a README file and to put into a directory
listing. These are the possibilities:
*/

typedef enum _HTDirReadme {
    HT_DIR_README_NONE,
    HT_DIR_README_TOP,
    HT_DIR_README_BOTTOM
} HTDirReadme;

#define DEFAULT_README		"README"

extern HTDirReadme  HTFile_dirReadme	(void);
extern BOOL HTFile_setDirReadme		(HTDirReadme mode);

/*
.
  Should we do Automatic File Suffix Binding?
.

By default, libwww uses the file suffix bindings
to figure out the media type, natural language, charset, content encoding,
etc and assign the values to the anchor of the local file.

However, in some situations, the application can either already have done
this or isn't interested in using the libwww bindings. You can control whether
libwww should do thi or not using the following methods:
*/

extern BOOL HTFile_doFileSuffixBinding (BOOL mode);
extern BOOL HTFile_fileSuffixBinding (void);

/*
Calculate the required buffer size (in bytes) for directory       
entries read from the given directory handle, as per security advisory
from Ben Hutchings.
Return -1 if this this cannot be done.   
*/

extern size_t HTFile_dirent_buf_size(DIR * dirp);

/*

*/

#ifdef __cplusplus
}
#endif

#endif /* HTFILE_H */

/*

  

  @(#) $Id$

*/
