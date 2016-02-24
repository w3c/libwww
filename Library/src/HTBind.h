/*

  
  					W3C Sample Code Library libwww File Suffix Binding


!
  File Suffix Binding Manager
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The preferences that we described in section
Request Preferences did not mention
what libwww should do if it doesn't know the data format of a document. In
many protocols this information is provided by the remote server. Typical
examples are MIME like protocols where the metainformation such as the
Content-Type and the Content-Language is provided together
with the document. However, applications often have access to the local file
system using file URLs which in general do not keep any or at least
very little information of the file type. It is therefore required to have
some kind of binding between the file system and the preferences registered
in the Library which provides this mateinformation about the object.

Often files in a file system is classified by some sort of a suffix, for
example, GIF files are often ending in .gif, text files
in .txt etc. This binding is not static and it is therefore required
to have a dynamic binding just like the preferences themselves. An example
of the latter is HTML files which on most Unix systems end in .html
whereas they on many MS-DOS based systems end in .htm.

This module provides a generic binding mechanism between a file and its
representation internally in libwww. It is not limited to simple file suffix
classification but can also be used in more advanced environments using data
bases etc. However, at this point we are interested in how we can register
bindings between file suffixes and for example content types, content languages
etc. The Bind manager is born with a certain knowledge about the set of
delimiters but more can be added to provide the functionality desired.

All the binding management could of course be replaced by a database interface.

This module is implemented by HTBind.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTBIND_H
#define HTBIND_H

#include "HTFormat.h"
#include "HTAnchor.h"

#ifdef __cplusplus
extern "C" { 
#endif 


/*
.
  Initialization of the Module
.

These functions must be called on startup and termination of the application.
This is done automatically by HTLibInit() and
HTLibTerminate().
*/

extern BOOL HTBind_init		(void);
extern BOOL HTBind_deleteAll	(void);

/*
.
  Case Sensitivity
.

Should the search for suffixes be case sensitive or not? The default value
is case sensitive.
*/

extern void HTBind_caseSensitive	(BOOL sensitive);

/*
.
  Suffix Delimiters
.

Change the set of suffix delimiters. The default is a platform dependent
set defined in the tcp module.
*/

extern const char *HTBind_delimiters	(void);
extern void HTBind_setDelimiters	(const char * new_suffixes);

/*
.
  Set up Bindings Associated with a File Suffix
.

There are four types of bindings:
	 
	   o 
	     Content Type (media type)
	   o 
	     Language
	   o 
	     Content Encoding
	   o 
	     Content Transfer Encoding
	 
	 
And the associated set of methods is defined as:
*/

extern BOOL HTBind_add		(const char *	suffix,
				 const char *	representation,
				 const char *	encoding,
				 const char *	transfer,
				 const char *	language,
				 double		value);

extern BOOL HTBind_addType	(const char *	suffix,
				 const char *	format,
				 double		value);

extern BOOL HTBind_addEncoding	(const char *	suffix,
				 const char *	encoding,
				 double		value);

extern BOOL HTBind_addTransfer	(const char *	suffix,
				 const char *	transfer,
				 double		value);

extern BOOL HTBind_addLanguage	(const char *	suffix,
				 const char *	language,
				 double		value);

/*

The first method is a "super" method for binding information to a file suffic.
Any of the string values can be NULL. If filename suffix is
already defined its previous definition is overridden or modified. For example,
a HTBind_setType and HTBind_setEncoding can be
called with the same suffix.

Calling this with suffix set to "*" will set the default representation.
Calling this with suffix set to "*.*" will set the default representation
for unknown suffix files which contain a "."

NOTE: The suffixes can contain characters that must be escaped
in a URL. However, they should not be encoded when parsed as the
suffix parameter.
.
  Determine a suitable suffix
.

Use the set of bindings to find a suitable suffix (or index) for a certain
combination of language, media type and encoding given in the anchor. Returns
a pointer to a suitable suffix string that must be freed by the caller. If
more than one suffix is found they are all concatenated. If no suffix is
found, NULL is returned.
*/

extern char * HTBind_getSuffix	(HTParentAnchor * anchor);

/*
.
  Determine the content of an Anchor
.

Use the set of bindings to find the combination of language, media type and
encoding of a given anchor. If more than one suffix is found they are all
searched. The last suffix has highest priority, the first one lowest. Returns
the HTAnchor object with the representations
found. See also HTBind_getFormat
*/

extern BOOL HTBind_getAnchorBindings	(HTParentAnchor * anchor);

/*
.
  Determine the content of a Response
.

Use the set of bindings to find the combination of language, media type and
encoding of a given anchor. If more than one suffix is found they are all
searched. The last suffix has highest priority, the first one lowest. Returns
the HTResponse object with the representations
found. See also HTBind_getFormat
*/

extern BOOL HTBind_getResponseBindings	(HTResponse * response,
                                         const char * url);

/*
.
  Determine the content of File
.

Use the set of bindings to find the combination of language, media type and
encoding of a given anchor. If more than one suffix is found they are all
searched. The last suffix has highest priority, the first one lowest. Returns
the format, encoding, and language found. See also
HTBind_getBindings.
*/

extern BOOL HTBind_getFormat (const char *	filename,
			      HTFormat *	format,
			      HTEncoding *	enc,
			      HTEncoding *	cte,
			      HTLanguage *	lang,
			      double *		quality);

/*

End of declaration module
*/

#ifdef __cplusplus
}
#endif

#endif /* HTBIND_H */

/*

  

  @(#) $Id$

*/
