/*			File Access				HTFile.h
**			===========
**
**	These are routines for file access used by WWW browsers.
**
*/

#ifndef HTFILE_H
#define HTFILE_H

#include "HTFormat.h"

/*	Convert filenames between local and WWW formats
**	-----------------------------------------------
**	Make up a suitable name for saving the node in
**
**	E.g.	$(HOME)/WWW/news/1234@cernvax.cern.ch
**		$(HOME)/WWW/http/crnvmc/FIND/xx.xxx.xx
*/
#ifdef __STDC__
extern char * HTLocalName(const char * name);
#else
extern char * HTLocalName();
#endif

/*	Make a WWW name from a full local path name
**
*/
extern char * WWW_nameOfFile PARAMS((const char * name));


/*	Generate the name of a cache file
*/

extern char * HTCacheFileName PARAMS((CONST char * name));


/*	Determine file format from file name
**	------------------------------------
*/

#ifdef __STDC__
extern int HTFileFormat(const char * filename);
#else
extern int HTFileFormat();
#endif	


/*	Determine write access to a file
//	--------------------------------
//
// On exit,
//	return value	YES if file can be accessed and can be written to.
//
//	Isn't there a quicker way?
*/

#ifdef __STDC__
extern BOOL HTEditable(const char * filename);
#else
extern BOOL HTEditable();
#endif


/*	Open a file descriptor for a document
**	-------------------------------------
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**
** On exit,
**	returns		<0		Error has occured.
**			HT_LOADED	Done OK
**
**	*pFormat	Set to the format of the file, if known.
**			(See WWW.h)
**
*/
extern int HTLoadFile
PARAMS
((
  const char * addr,
  HTFormat * pFormat,
  HTParentAnchor * anchor,
  int diagnostic
));

extern HTProtocol HTFTP, HTFile;
#endif /* HTFILE_H /
