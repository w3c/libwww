/*

					W3C Sample Code Library libwww File descriptions




!File Descriptions!

*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

Descriptions appearing in directory listings
are produced by this module. This may be overridden by another module
for those who which descriptions to come from somewhere else. It's
only HTTP directory listings that contain a description field (if
enabled by the Directory browsing module.

This module is implemented by HTDescpt.c, and
it is a part of the  W3C
Sample Code Library.

*/

#ifndef HTDESCRIPT_H
#define HTDESCRIPT_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*

.Description File.

This module gets descriptions from the file defined by global variable
HTDescriptionFile in the same directory as the directoy
to be listed. The default value is .www_descript:

*/

extern char * HTDescriptionFile;

/*

In the description file lines starting with a word starting with 'D'
are taken to be descriptions (this looks funny now, but this is to
make it easy to extend these description files to contain also other
information. An example of the format of the description file is:

*/

/*
**	DESCRIBE  welcome.html	Our welcome page
**	DESCRIBE  map*.gif	Map as a GIF image
**	DESCRIBE  map*.ps	Map as a PostScript image
*/

/*

.HTML Titles.

If description is not specified for a file that is of type
text/html, this module uses the HTML TITLE as the
description.  This feature can be turned off by setting the
HTPeekTitles variable to false.

*/

extern BOOL HTPeekTitles;

/*

.Read Description File.

The description file for a directory is read in only once by
HTReadDescriptions(), and the result returned by it is
given as an argument when finding out a description for a single file.

*/

extern HTList * HTReadDescriptions (char * dirname);

/*

.Get Description For a File.

Once description file has been read and the list of descriptions is
returned by HTReadDescriptions(), the function
HTGetDescription() can be used to get a description for a
given file:

*/

extern char * HTGetDescription (HTList *	descriptions,
				       char *	dirname,
				       char *	filename,
				       HTFormat	format);

/*

Directory name has to be present because this function may then take a
peek at the file itself (to get the HTML TITLE, for example).
If format is WWW_HTML and description is not
found, this module may be configured to use the HTML TITLE as the
description. 

No string returned by this function should be freed!


.Freeing Descriptions.

Once descriptions have been gotten, the description list returned by
HTReadDescriptions() must be freed by
HTFreeDescriptions():
*/

extern void HTFreeDescriptions (HTList * descriptions);

/*

*/

#ifdef __cplusplus
}
#endif

#endif /* !HTDESCRIPT_H */

/*



@(#) $Id$


*/
