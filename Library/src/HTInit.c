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
#include "HTWSRC.h"
#include "HTFWriter.h"

PUBLIC void HTFormatInit NOARGS
{
#ifdef NeXT
  HTSetPresentation("application/postscript", "open %s",	1.0, 2.0, 0.0);
  /* The following needs the GIF previewer -- you might not have it. */
  HTSetPresentation("image/gif", 		"open %s", 	0.3, 2.0, 0.0);
  HTSetPresentation("image/x-tiff", 		"open %s", 	1.0, 2.0, 0.0);
  HTSetPresentation("audio/basic", 		"open %s", 	1.0, 2.0, 0.0);
  HTSetPresentation("*", 			"open %s", 	1.0, 0.0, 0.0);
#else
 if (getenv("DISPLAY")) {	/* Must have X11 */
  HTSetPresentation("application/postscript", "ghostview %s",	1.0, 3.0, 0.0);
  HTSetPresentation("image/gif", 		"xv %s", 	1.0, 3.0, 0.0);
  HTSetPresentation("image/x-tiff", 		"xv %s", 	1.0, 3.0, 0.0);
  HTSetPresentation("image/jpeg", 		"xv %s", 	1.0, 3.0, 0.0);
 }
#endif
 HTSetConversion("www/mime",  "*",  	     HTMIMEConvert, 	1.0, 0.0, 0.0);
 HTSetConversion("application/x-wais-source",
 		      "*",  	     HTWSRCConvert, 	1.0, 0.0, 0.0);
 HTSetConversion("text/html", "text/x-c",    HTMLToC, 	        0.5, 0.0, 0.0);
 HTSetConversion("text/html", "text/plain",  HTMLToPlain, 	0.5, 0.0, 0.0);
 HTSetConversion("text/html", "www/present", HTMLPresent, 	1.0, 0.0, 0.0);
 HTSetConversion("text/plain", "text/html",  HTPlainToHTML,	1.0, 0.0, 0.0);
 HTSetConversion("text/plain", "www/present", HTPlainPresent,	1.0, 0.0, 0.0);
 HTSetConversion("application/octet-stream", "www/present",
 					      HTSaveLocally,	0.1, 0.0, 0.0);
 HTSetConversion("www/unknown", "www/present",
 					      HTSaveLocally,	0.3, 0.0, 0.0);
 HTSetConversion("www/source", "www/present",
 					      HTSaveLocally,	0.3, 0.0, 0.0);
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
{	/* 	Suffix     Contenet-Type   Content-Encoding    Quality  */
    
    HTSetSuffix(".ai",	"application/postscript", "8bit", 0.5);	/* Adobe illustator */
    HTSetSuffix(".au",  "audio/basic", "binary", 1.0);
    HTSetSuffix(".mime","www/mime", "8bit", 1.0); /* Internal -- MIME is not recursive? */
    
    HTSetSuffix(".PS",	"application/postscript", "8bit", 0.8);
    HTSetSuffix(".eps",	"application/postscript", "8bit", 0.8);
    HTSetSuffix(".ps",	"application/postscript", "8bit", 0.8);

    HTSetSuffix(".execme.csh", "application/x-csh", "7bit", 0.5);
    
    HTSetSuffix(".html","text/html", "8bit", 1.0);
    
    HTSetSuffix(".c",	"text/plain", "7bit", 0.5);
    HTSetSuffix(".h",	"text/plain", "7bit", 0.5);	/* html better */
    HTSetSuffix(".m",	"text/plain", "7bit", 0.5);	/* Objective-C code */
    HTSetSuffix(".f90",	"text/plain", "7bit", 0.5);	/* Fortran 90 */
    HTSetSuffix(".txt", "text/plain", "7bit", 0.5);

    HTSetSuffix(".rtf",	"application/x-rtf", "8bit", 1.0);

    HTSetSuffix(".src",	"application/x-wais-source", "7bit", 1.0);
    
    HTSetSuffix(".snd", "audio/basic", "binary", 1.0);
    
    HTSetSuffix(".bin",	"application/octet-stream", "binary", 1.0);

    HTSetSuffix(".Z",	"application/x-compressed", "binary", 1.0);
    
    HTSetSuffix(".gif", "image/gif", "binary", 1.0);

    HTSetSuffix(".tiff","image/x-tiff", "binary", 1.0);
    
    HTSetSuffix(".jpg", "image/jpeg", "binary", 1.0);
    HTSetSuffix(".JPG", "image/jpeg", "binary", 1.0);
    HTSetSuffix(".JPEG","image/jpeg", "binary", 1.0);
    HTSetSuffix(".jpeg","image/jpeg", "binary", 1.0);
    
    HTSetSuffix(".MPEG","video/mpeg", "binary", 1.0);
    HTSetSuffix(".mpg","video/mpeg", "binary", 1.0);
    HTSetSuffix(".MPG","video/mpeg", "binary", 1.0);
    HTSetSuffix(".mpeg","video/mpeg", "binary", 1.0);

}
#endif /* NO_INIT */

