/*		Configuration-specific Initialialization	HTInit.c
**		----------------------------------------
*/

/*	Define a basic set of suffixes and presentations
**	------------------------------------------------
**
*/

/* Implements:
*/
#include "HTInit.h"

#include "HTML.h"
#include "HTList.h"
#include "HTPlain.h"
#include "HTMLGen.h"
#include "HTFile.h"
#include "HTFormat.h"
#include "HTMIME.h"
#include "HTWSRC.h"
#include "HTFWriter.h"
#include "HTNews.h"

/* Note: Wildcard is no longer `*' (see further dovn) */
PUBLIC void HTFormatInit ARGS1(HTList *, c)
{
#ifdef NeXT
    HTSetPresentation(c,"application/postscript", "open %s",	1.0, 2.0, 0.0);
    /* The following needs the GIF previewer -- you might not have it. */
    HTSetPresentation(c,"image/gif", 		"open %s", 	0.3, 2.0, 0.0);
    HTSetPresentation(c,"image/x-tiff", 	"open %s", 	1.0, 2.0, 0.0);
    HTSetPresentation(c,"audio/basic", 		"open %s", 	1.0, 2.0, 0.0);
    HTSetPresentation(c,"*/*", 			"open %s", 	0.05, 0.0, 0.0); 
#else
    if (getenv("DISPLAY")) {	/* Must have X11 */
	HTSetPresentation(c,"application/postscript", "ghostview %s",	1.0, 3.0, 0.0);
	HTSetPresentation(c,"image/gif", 	"xv %s",	1.0, 3.0, 0.0);
	HTSetPresentation(c,"image/x-tiff", 	"xv %s",	1.0, 3.0, 0.0);
	HTSetPresentation(c,"image/jpeg", 	"xv %s",	1.0, 3.0, 0.0);
    }
#endif
    HTSetConversion(c,"www/mime",		"*/*",		HTMIMEConvert,	1.0, 0.0, 0.0);
    HTSetConversion(c,"text/html",		"text/x-c",	HTMLToC,	0.5, 0.0, 0.0);
    HTSetConversion(c,"text/html",		"text/plain",	HTMLToPlain,	0.5, 0.0, 0.0);
    HTSetConversion(c,"text/html",		"www/present",	HTMLPresent,	1.0, 0.0, 0.0);
    HTSetConversion(c,"text/html",	       	"text/latex",	HTMLToTeX,	1.0, 0.0, 0.0);
    HTSetConversion(c,"text/plain",		"text/html",	HTPlainToHTML,	1.0, 0.0, 0.0);
    HTSetConversion(c,"text/plain",		"www/present",	HTPlainPresent,	1.0, 0.0, 0.0);
#ifdef NEW_CODE
    HTSetConversion(c,"text/newslist",		"www/present",	HTNewsList,	1.0, 0.0, 0.0);
    HTSetConversion(c,"text/newslist",		"text/html",	HTNewsList,	1.0, 0.0, 0.0);
#endif
    HTSetConversion(c,"application/octet-stream","www/present",	HTSaveLocally,	0.1, 0.0, 0.0);
    HTSetConversion(c,"application/x-wais-source","*/*",	HTWSRCConvert, 	1.0, 0.0, 0.0);
    HTSetConversion(c,"*/*",			"www/present",	HTSaveLocally,	0.3, 0.0, 0.0);
}


/* -----------------
   This function is for init of non-interactive mode, where no extern 'pop-up
   programs' are wanted during execution. Nor should functions be used that
   redirects the output stream to a temp file, e.g., HTSaveLocally()
   Henrik 01/03-94
   ----------------- */ 
PUBLIC void HTFormatInitNIM ARGS1(HTList *, c)
{
    HTSetConversion(c,"www/mime",		"*/*",		HTMIMEConvert,	1.0, 0.0, 0.0);
    HTSetConversion(c,"text/html",		"text/x-c",	HTMLToC,	0.5, 0.0, 0.0);
    HTSetConversion(c,"text/html",		"text/plain",	HTMLToPlain,	0.5, 0.0, 0.0);
    HTSetConversion(c,"text/html",		"www/present",	HTMLPresent,	1.0, 0.0, 0.0);
    HTSetConversion(c,"text/html",	       	"text/latex",	HTMLToTeX,	1.0, 0.0, 0.0);
    HTSetConversion(c,"text/plain",		"text/html",	HTPlainToHTML,	1.0, 0.0, 0.0);
    HTSetConversion(c,"text/plain",		"www/present",	HTPlainPresent,	1.0, 0.0, 0.0);
#ifdef NEW_CODE
    HTSetConversion(c,"text/newslist",		"www/present",	HTNewsList,	1.0, 0.0, 0.0);
    HTSetConversion(c,"text/newslist",		"text/html",	HTNewsList,	1.0, 0.0, 0.0);
#endif
    HTSetConversion(c,"application/x-wais-source","*/*",	HTWSRCConvert, 	1.0, 0.0, 0.0);
    HTSetConversion(c,"application/octet-stream","www/present",	HTThroughLine,	0.1, 0.0, 0.0);
    HTSetConversion(c,"*/*",			"www/present",	HTThroughLine,	0.3, 0.0, 0.0);
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
    /*		Suffix     Contenet-Type	Content-Encoding  Quality			*/

    HTAddType(".mime",   "www/mime",			"8bit",   1.0);	/* Internal -- MIME is	*/
                                                                        /* not recursive	*/
    HTAddType(".bin",    "application/octet-stream",	"binary", 1.0); /* Uninterpreted binary	*/
    HTAddType(".oda",    "application/oda",		"binary", 1.0);
    HTAddType(".pdf",    "application/pdf",		"binary", 1.0);
    HTAddType(".ai",     "application/postscript",	"8bit",   0.5);	/* Adobe Illustrator	*/
    HTAddType(".PS",     "application/postscript",	"8bit",	  0.8);	/* PostScript		*/
    HTAddType(".eps",    "application/postscript",	"8bit",   0.8);
    HTAddType(".ps",     "application/postscript",	"8bit",   0.8);
    HTAddType(".rtf",    "application/x-rtf",		"7bit",   1.0);	/* RTF			*/
    HTAddType(".Z",      "application/x-compressed",	"binary", 1.0);	/* Compressed data	*/
    HTAddType(".csh",    "application/x-csh",		"7bit",   0.5);	/* C-shell script	*/
    HTAddType(".dvi",    "application/x-dvi",		"binary", 1.0);	/* TeX DVI		*/
    HTAddType(".hdf",    "application/x-hdf",		"binary", 1.0);	/* NCSA HDF data file	*/
    HTAddType(".latex",  "application/x-latex",		"8bit",   1.0);	/* LaTeX source		*/
    HTAddType(".nc",     "application/x-netcdf",	"binary", 1.0);	/* Unidata netCDF data	*/
    HTAddType(".cdf",    "application/x-netcdf",	"binary", 1.0);
    HTAddType(".sh",     "application/x-sh",		"7bit",   0.5);	/* Shell-script		*/
    HTAddType(".tcl",    "application/x-tcl",		"7bit",   0.5);	/* TCL-script		*/
    HTAddType(".tex",    "application/x-tex",		"8bit",   1.0);	/* TeX source		*/
    HTAddType(".texi",   "application/x-texinfo",	"7bit",   1.0);	/* Texinfo		*/
    HTAddType(".texinfo","application/x-texinfo",	"7bit",   1.0);
    HTAddType(".t",      "application/x-troff",		"7bit",   0.5);	/* Troff		*/
    HTAddType(".roff",   "application/x-troff",		"7bit",   0.5);
    HTAddType(".tr",     "application/x-troff",		"7bit",   0.5);
    HTAddType(".man",    "application/x-troff-man",	"7bit",   0.5);	/* Troff with man macros*/
    HTAddType(".me",     "application/x-troff-me",	"7bit",   0.5);	/* Troff with me macros	*/
    HTAddType(".ms",     "application/x-troff-ms",	"7bit",   0.5);	/* Troff with ms macros	*/
    HTAddType(".src",    "application/x-wais-source",	"7bit",   1.0);	/* WAIS source		*/
    HTAddType(".zip",    "application/zip",		"binary", 1.0);	/* PKZIP		*/
    HTAddType(".bcpio",  "application/x-bcpio",		"binary", 1.0);	/* Old binary CPIO	*/
    HTAddType(".cpio",   "application/x-cpio",		"binary", 1.0);	/* POSIX CPIO		*/
    HTAddType(".gtar",   "application/x-gtar",		"binary", 1.0);	/* Gnu tar		*/
    HTAddType(".shar",   "application/x-shar",		"8bit",   1.0);	/* Shell archive	*/
    HTAddType(".sv4cpio","application/x-sv4cpio",	"binary", 1.0);	/* SVR4 CPIO		*/
    HTAddType(".sv4crc", "application/x-sv4crc",	"binary", 1.0);	/* SVR4 CPIO with CRC	*/
    HTAddType(".tar",    "application/x-tar",		"binary", 1.0);	/* 4.3BSD tar		*/
    HTAddType(".ustar",  "application/x-ustar",		"binary", 1.0);	/* POSIX tar		*/
    HTAddType(".snd",    "audio/basic",			"binary", 1.0);	/* Audio		*/
    HTAddType(".au",     "audio/basic",			"binary", 1.0);
    HTAddType(".aiff",   "audio/x-aiff",		"binary", 1.0);
    HTAddType(".aifc",   "audio/x-aiff",		"binary", 1.0);
    HTAddType(".aif",    "audio/x-aiff",		"binary", 1.0);
    HTAddType(".wav",    "audio/x-wav",			"binary", 1.0);	/* Windows+ WAVE format	*/
    HTAddType(".gif",    "image/gif",			"binary", 1.0);	/* GIF			*/
    HTAddType(".ief",    "image/ief",			"binary", 1.0);	/* Image Exchange fmt	*/
    HTAddType(".jpg",    "image/jpeg",			"binary", 1.0);	/* JPEG			*/
    HTAddType(".JPG",    "image/jpeg",			"binary", 1.0);
    HTAddType(".JPE",    "image/jpeg",			"binary", 1.0);
    HTAddType(".jpe",    "image/jpeg",			"binary", 1.0);
    HTAddType(".JPEG",   "image/jpeg",			"binary", 1.0);
    HTAddType(".jpeg",   "image/jpeg",			"binary", 1.0);
    HTAddType(".tif",    "image/tiff",			"binary", 1.0);	/* TIFF			*/
    HTAddType(".tiff",   "image/tiff",			"binary", 1.0);
    HTAddType(".ras",    "image/cmu-raster",		"binary", 1.0);
    HTAddType(".pnm",    "image/x-portable-anymap",	"binary", 1.0);	/* PBM Anymap format	*/
    HTAddType(".pbm",    "image/x-portable-bitmap",	"binary", 1.0);	/* PBM Bitmap format	*/
    HTAddType(".pgm",    "image/x-portable-graymap",	"binary", 1.0);	/* PBM Graymap format	*/
    HTAddType(".ppm",    "image/x-portable-pixmap",	"binary", 1.0);	/* PBM Pixmap format	*/
    HTAddType(".rgb",    "image/x-rgb",			"binary", 1.0);
    HTAddType(".xbm",    "image/x-xbitmap",		"binary", 1.0);	/* X bitmap		*/
    HTAddType(".xpm",    "image/x-xpixmap",		"binary", 1.0);	/* X pixmap format	*/
    HTAddType(".xwd",    "image/x-xwindowdump",		"binary", 1.0);	/* X window dump (xwd)	*/
    HTAddType(".html",   "text/html",			"8bit",   1.0);	/* HTML			*/
    HTAddType(".c",      "text/plain",			"7bit",   0.5);	/* C source		*/
    HTAddType(".h",      "text/plain",			"7bit",   0.5);	/* C headers		*/
    HTAddType(".C",      "text/plain",			"7bit",   0.5);	/* C++ source		*/
    HTAddType(".cc",     "text/plain",			"7bit",   0.5);	/* C++ source		*/
    HTAddType(".hh",     "text/plain",			"7bit",   0.5);	/* C++ headers		*/
    HTAddType(".m",      "text/plain",			"7bit",   0.5);	/* Objective-C source	*/
    HTAddType(".f90",    "text/plain",			"7bit",   0.5);	/* Fortran 90 source	*/
    HTAddType(".txt",    "text/plain",			"7bit",   0.5);	/* Plain text		*/
    HTAddType(".rtx",    "text/richtext",		"7bit",   1.0);	/* MIME Richtext format	*/
    HTAddType(".tsv",    "text/tab-separated-values",	"7bit",   1.0);	/* Tab-separated values	*/
    HTAddType(".etx",    "text/x-setext",		"7bit",   0.9);	/* Struct Enchanced Txt	*/
    HTAddType(".MPG",    "video/mpeg",			"binary", 1.0);	/* MPEG			*/
    HTAddType(".mpg",    "video/mpeg",			"binary", 1.0);
    HTAddType(".MPE",    "video/mpeg",			"binary", 1.0);
    HTAddType(".mpe",    "video/mpeg",			"binary", 1.0);
    HTAddType(".MPEG",   "video/mpeg",			"binary", 1.0);
    HTAddType(".mpeg",   "video/mpeg",			"binary", 1.0);
    HTAddType(".qt",     "video/quicktime",		"binary", 1.0);	/* QuickTime		*/
    HTAddType(".mov",    "video/quicktime",		"binary", 1.0);
    HTAddType(".avi",    "video/x-msvideo",		"binary", 1.0);	/* MS Video for Windows	*/
    HTAddType(".movie",  "video/x-sgi-movie",		"binary", 1.0);	/* SGI "moviepalyer"	*/

    HTAddType("*.*",     "www/unknown",			"binary", 0.1);
    HTAddType("*",       "text/plain",			"7bit",   0.5);

}
#endif /* !NO_INIT */

