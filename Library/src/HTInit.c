/*		Configuration-specific Initialialization	HTInit.c
**		----------------------------------------
*/

/*	Define a basic set of suffixes and presentations
**	------------------------------------------------
*/

/* Implements:
*/
#include "HTInit.h"

#include "HTML.h"
#include "HTPlain.h"
#include "HTMLGen.h"
#include "HTFile.h"
#include "HTFormat.h"
#include "HTMIME.h"

PUBLIC void HTFormatInit NOARGS
{
#ifdef NeXT
  HTSetPresentation("application/postscript", "open %s",	1.0, 2.0, 0.0);
  HTSetPresentation("image/gif", 		"open %s", 	1.0, 2.0, 0.0);
  HTSetPresentation("audio/basic", 		"open %s", 	1.0, 2.0, 0.0);
  HTSetPresentation("*", 			"open %s", 	1.0, 0.0, 0.0);
#else
 if (getenv("DISPLAY")) {	/* Must have X11 */
  HTSetPresentation("application/postscript", "ghostview %s",	1.0, 3.0, 0.0);
  HTSetPresentation("image/gif", 		"xv %s", 	1.0, 3.0, 0.0);
  HTSetPresentation("image/jpeg", 		"xv %s", 	1.0, 3.0, 0.0);
 }
#endif
 HTSetConversion("www/mime",  "*",  	     HTMIMEConvert, 	1.0, 0.0, 0.0);
 HTSetConversion("text/html", "text/x-c",    HTMLToC, 	        0.5, 0.0, 0.0);
 HTSetConversion("text/html", "text/plain",  HTMLToPlain, 	0.5, 0.0, 0.0);
 HTSetConversion("text/html", "www/present", HTMLPresent, 	1.0, 0.0, 0.0);
 HTSetConversion("text/plain", "text/html",  HTPlainToHTML,	1.0, 0.0, 0.0);
 HTSetConversion("text/plain", "www/present", HTPlainPresent,	1.0, 0.0, 0.0);
}



/*	Define a basic set of suffixes
**	------------------------------
**
**	The LAST suffix for a type is that used for temporary files
**	of that type.
**	The quality is an apriori bias as to whether the file should be
**	used.  Not that different suffixes can be used to represent files
**	which are of the same format but are originals or regenerated,
**	with different values.
*/

#ifndef NO_INIT
PUBLIC void HTFileInit NOARGS
{
    HTSetSuffix(".mime","www/mime", 1.0);	/* Internal -- MIME is not recursive */
    
    HTSetSuffix(".PS",	"application/postscript", 0.8);
    HTSetSuffix(".eps",	"application/postscript", 0.8);
    HTSetSuffix(".ai",	"application/postscript", 0.5);	/* Adobe illustator */
    HTSetSuffix(".ps",	"application/postscript", 0.8);
    
    HTSetSuffix(".html","text/html", 1.0);		/* Jumping the gun a little */

    HTSetSuffix(".c",	"text/plain", 0.5);
    HTSetSuffix(".h",	"text/plain", 0.5);		/* html better */
    HTSetSuffix(".m",	"text/plain", 0.5);		/* Objective-C code */
    HTSetSuffix(".txt", "text/plain", 0.5);

    HTSetSuffix(".rtf",	"application/x-rtf", 1.0);
    
    HTSetSuffix(".snd", "audio/basic", 1.0);
    
    HTSetSuffix(".bin",	"application/octet-stream", 1.0);	/* @@@@@@@@@@@@@@@@ */

    HTSetSuffix(".Z",	"application/x-compressed", 1.0);	/* @@@@@@@@@@@@@@@@ */
    
    HTSetSuffix(".gif", "image/gif", 1.0);

    HTSetSuffix(".tiff","image/x-tiff", 1.0);
    
    HTSetSuffix(".jpg", "image/jpeg", 1.0);
    HTSetSuffix(".JPG", "image/jpeg", 1.0);
    HTSetSuffix(".JPEG","image/jpeg", 1.0);
    HTSetSuffix(".jpeg","image/jpeg", 1.0);
    
    HTSetSuffix(".MPEG","video/mpeg", 1.0);
    HTSetSuffix(".mpg","video/mpeg", 1.0);
    HTSetSuffix(".MPG","video/mpeg", 1.0);
    HTSetSuffix(".mpeg","video/mpeg", 1.0);

}
#endif /* NO_INIT */

