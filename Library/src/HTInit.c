/*								       HTInit.c
**	CONFIGURATION-SPECIFIC INITIALIALIZATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	Define a basic set of suffixes and presentations
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTFormat.h"
#include "HTList.h"

/* Binding Managers */
#include "HTBind.h"
#include "HTProt.h"

/* Converters and Presenters */
#include "HTML.h"
#include "HTPlain.h"
#include "HTMLGen.h"
#include "HTMIME.h"
#include "HTWSRC.h"
#include "HTFWrite.h"

/* Protocol Modules */
#include "HTTP.h"
#include "HTFile.h"
#include "HTFTP.h"
#include "HTGopher.h"
#include "HTTelnet.h"
#include "HTNews.h"

#ifdef HT_DIRECT_WAIS
#include "HTWAIS.h"
#endif

#include "HTInit.h"				         /* Implemented here */

/*	BINDINGS BETWEEN A SOURCE MEDIA TYPE AND A DEST MEDIA TYPE (CONVERSION)
**	----------------------------------------------------------------------
**
**	Not done automaticly - must be done by application!
*/
PUBLIC void HTConverterInit ARGS1(HTList *, c)
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
    HTSetConversion(c,"application/octet-stream","www/present",	HTSaveLocally,	0.1, 0.0, 0.0);
    HTSetConversion(c,"application/x-wais-source","*/*",	HTWSRCConvert, 	1.0, 0.0, 0.0);
    HTSetConversion(c,"*/*",			"www/present",	HTSaveLocally,	0.3, 0.0, 0.0);
}

/*	BINDINGS BETWEEN MEDIA TYPES AND EXTERNAL VIEWERS/PRESENTERS
**	------------------------------------------------------------
**
**	The data objects are stored in temporary files before the external
**	program is called
**
**	Not done automaticly - must be done by application!
*/
PUBLIC void HTPresenterInit ARGS1(HTList *, c)
{
#ifdef NeXT
    HTSetPresentation(c,"application/postscript", "open %s",	NULL, 1.0, 2.0, 0.0);
    /* The following needs the GIF previewer -- you might not have it. */

    HTSetPresentation(c,"image/gif", 		"open %s", 	NULL, 0.3, 2.0, 0.0);
    HTSetPresentation(c,"image/x-tiff", 	"open %s", 	NULL, 1.0, 2.0, 0.0);
    HTSetPresentation(c,"audio/basic", 		"open %s", 	NULL, 1.0, 2.0, 0.0);
    HTSetPresentation(c,"*/*", 			"open %s", 	NULL, 0.05, 0.0, 0.0); 
#else
    if (getenv("DISPLAY")) {	/* Must have X11 */
	HTSetPresentation(c,"application/postscript", "ghostview %s",	NULL, 1.0, 3.0, 0.0);
	HTSetPresentation(c,"image/gif", 		"xv %s",	NULL, 1.0, 3.0, 0.0);
	HTSetPresentation(c,"image/x-tiff", 	"xv %s",	NULL, 1.0, 3.0, 0.0);
	HTSetPresentation(c,"image/jpeg", 	"xv %s",	NULL, 1.0, 3.0, 0.0);
    }
#endif
}


/*	PRESENTERS AND CONVERTERS AT THE SAME TIME
**	------------------------------------------
**
**	This function is only defined in order to preserve backward
**	compatibility.
*/
PUBLIC void HTFormatInit ARGS1(HTList *, c)
{
    HTConverterInit(c);
    HTPresenterInit(c);

}

/*	REGISTER ALL KNOWN PROTOCOLS IN THE LIBRARY
**	-------------------------------------------
**
**	Add to or subtract from this list if you add or remove protocol
**	modules. This function is called from HTLibInit()
**	Compiling with HT_NO_INIT prevents all known protocols from being
**	force in at link time.
*/
#ifndef HT_NO_INIT
PUBLIC void HTAccessInit NOARGS
{
#ifndef DECNET
    HTProtocol_add(&HTFTP);
    HTProtocol_add(&HTNews);
    HTProtocol_add(&HTGopher);
#ifdef HT_DIRECT_WAIS
    HTProtocol_add(&HTWAIS);
#endif
#endif /* DECNET */

    HTProtocol_add(&HTTP);
    HTProtocol_add(&HTFile);
    HTProtocol_add(&HTTelnet);
    HTProtocol_add(&HTTn3270);
    HTProtocol_add(&HTRlogin);
}
#endif /* !HT_NO_INIT */


/*	BINDINGS BETWEEN FILE EXTENSIONS AND MEDIA TYPES
**	------------------------------------------------
**
**	The LAST suffix for a type is that used for temporary files of that
**	type. The quality is an apriori bias as to whether the file should be
**	used.  Not that different suffixes can be used to represent files
**	which are of the same format but are originals or regenerated,
**	with different values. Called from HTLibraryInit().
*/

#ifndef HT_NO_INIT
PUBLIC void HTFileInit NOARGS
{
    /*		       Suffix	 Content-Type		        Encoding  Lang	Quality	*/

    HTBind_setBinding("mime",   "www/mime",			"8bit",   NULL, 1.0);	/* Internal -- MIME is	*/
                                                                       /* not recursive	*/
    HTBind_setBinding("bin",    "application/octet-stream",	"binary", NULL, 1.0); /* Uninterpreted binary	*/
    HTBind_setBinding("oda",    "application/oda",		"binary", NULL, 1.0);
    HTBind_setBinding("pdf",    "application/pdf",		"binary", NULL, 1.0);
    HTBind_setBinding("ai",     "application/postscript",	"8bit",   NULL, 0.5);	/* Adobe Illustrator	*/
    HTBind_setBinding("PS",     "application/postscript",	"8bit",	  NULL, 0.8);	/* PostScript		*/
    HTBind_setBinding("eps",    "application/postscript",	"8bit",   NULL, 0.8);
    HTBind_setBinding("ps",     "application/postscript",	"8bit",   NULL, 0.8);
    HTBind_setBinding("gtar",   "application/x-gtar",		"binary", NULL, 1.0);	/* Gnu tar		*/
    HTBind_setBinding("rtf",    "application/x-rtf",		"7bit",	  NULL, 1.0);	/* RTF			*/
    HTBind_setBinding("csh",    "application/x-csh",		"7bit",   NULL, 0.5);	/* C-shell script	*/
    HTBind_setBinding("dvi",    "application/x-dvi",		"binary", NULL, 1.0);	/* TeX DVI		*/
    HTBind_setBinding("hdf",    "application/x-hdf",		"binary", NULL, 1.0);	/* NCSA HDF data file	*/
    HTBind_setBinding("latex",  "application/x-latex",		"8bit",   NULL, 1.0);	/* LaTeX source		*/
    HTBind_setBinding("nc",     "application/x-netcdf",	"binary", NULL, 1.0);	/* Unidata netCDF data	*/
    HTBind_setBinding("cdf",    "application/x-netcdf",	"binary", NULL, 1.0);
    HTBind_setBinding("sh",     "application/x-sh",		"7bit",   NULL, 0.5);	/* Shell-script		*/
    HTBind_setBinding("tar",    "application/x-tar",		"binary", NULL, 1.0);	/* 4.3BSD tar		*/
    HTBind_setBinding("tcl",    "application/x-tcl",		"7bit",   NULL, 0.5);	/* TCL-script		*/
    HTBind_setBinding("tex",    "application/x-tex",		"8bit",   NULL, 1.0);	/* TeX source		*/
    HTBind_setBinding("texi",   "application/x-texinfo",	"7bit",   NULL, 1.0);	/* Texinfo		*/
    HTBind_setBinding("texinfo","application/x-texinfo",	"7bit",   NULL, 1.0);
    HTBind_setBinding("t",      "application/x-troff",		"7bit",   NULL, 0.5);	/* Troff		*/
    HTBind_setBinding("roff",   "application/x-troff",		"7bit",   NULL, 0.5);
    HTBind_setBinding("tr",     "application/x-troff",		"7bit",   NULL, 0.5);
    HTBind_setBinding("man",    "application/x-troff-man",	"7bit",   NULL, 0.5);	/* Troff with man macros*/
    HTBind_setBinding("me",     "application/x-troff-me",	"7bit",   NULL, 0.5);	/* Troff with me macros	*/
    HTBind_setBinding("ms",     "application/x-troff-ms",	"7bit",   NULL, 0.5);	/* Troff with ms macros	*/
    HTBind_setBinding("src",    "application/x-wais-source",	"7bit",   NULL, 1.0);	/* WAIS source		*/
    HTBind_setBinding("bcpio",  "application/x-bcpio",		"binary", NULL, 1.0);	/* Old binary CPIO	*/
    HTBind_setBinding("cpio",   "application/x-cpio",		"binary", NULL, 1.0);	/* POSIX CPIO		*/
    HTBind_setBinding("shar",   "application/x-shar",		"8bit",   NULL, 1.0);	/* Shell archive	*/
    HTBind_setBinding("sv4cpio","application/x-sv4cpio",	"binary", NULL, 1.0);	/* SVR4 CPIO		*/
    HTBind_setBinding("sv4crc", "application/x-sv4crc",	"binary", NULL, 1.0);	/* SVR4 CPIO with CRC	*/
    HTBind_setBinding("ustar",  "application/x-ustar",		"binary", NULL, 1.0);	/* POSIX tar		*/
    HTBind_setBinding("snd",    "audio/basic",			"binary", NULL, 1.0);	/* Audio		*/
    HTBind_setBinding("au",     "audio/basic",			"binary", NULL, 1.0);
    HTBind_setBinding("aiff",   "audio/x-aiff",		"binary", NULL, 1.0);
    HTBind_setBinding("aifc",   "audio/x-aiff",		"binary", NULL, 1.0);
    HTBind_setBinding("aif",    "audio/x-aiff",		"binary", NULL, 1.0);
    HTBind_setBinding("wav",    "audio/x-wav",			"binary", NULL, 1.0);	/* Windows+ WAVE format	*/
    HTBind_setBinding("gif",    "image/gif",			"binary", NULL, 1.0);	/* GIF			*/
    HTBind_setBinding("ief",    "image/ief",			"binary", NULL, 1.0);	/* Image Exchange fmt	*/
    HTBind_setBinding("jpg",    "image/jpeg",			"binary", NULL, 1.0);	/* JPEG			*/
    HTBind_setBinding("JPG",    "image/jpeg",			"binary", NULL, 1.0);
    HTBind_setBinding("JPE",    "image/jpeg",			"binary", NULL, 1.0);
    HTBind_setBinding("jpe",    "image/jpeg",			"binary", NULL, 1.0);
    HTBind_setBinding("JPEG",   "image/jpeg",			"binary", NULL, 1.0);
    HTBind_setBinding("jpeg",   "image/jpeg",			"binary", NULL, 1.0);
    HTBind_setBinding("tif",    "image/tiff",			"binary", NULL, 1.0);	/* TIFF			*/
    HTBind_setBinding("tiff",   "image/tiff",			"binary", NULL, 1.0);
    HTBind_setBinding("ras",    "image/cmu-raster",		"binary", NULL, 1.0);
    HTBind_setBinding("pnm",    "image/x-portable-anymap",	"binary", NULL, 1.0);	/* PBM Anymap format	*/
    HTBind_setBinding("pbm",    "image/x-portable-bitmap",	"binary", NULL, 1.0);	/* PBM Bitmap format	*/
    HTBind_setBinding("pgm",    "image/x-portable-graymap",	"binary", NULL, 1.0);	/* PBM Graymap format	*/
    HTBind_setBinding("ppm",    "image/x-portable-pixmap",	"binary", NULL, 1.0);	/* PBM Pixmap format	*/
    HTBind_setBinding("rgb",    "image/x-rgb",			"binary", NULL, 1.0);
    HTBind_setBinding("xbm",    "image/x-xbitmap",		"binary", NULL, 1.0);	/* X bitmap		*/
    HTBind_setBinding("xpm",    "image/x-xpixmap",		"binary", NULL, 1.0);	/* X pixmap format	*/
    HTBind_setBinding("xwd",    "image/x-xwindowdump",		"binary", NULL, 1.0);	/* X window dump (xwd)	*/
    HTBind_setBinding("html",   "text/html",			"8bit",   NULL, 1.0);	/* HTML			*/
    HTBind_setBinding("c",      "text/plain",			"7bit",   NULL, 0.5);	/* C source		*/
    HTBind_setBinding("h",      "text/plain",			"7bit",   NULL, 0.5);	/* C headers		*/
    HTBind_setBinding("C",      "text/plain",			"7bit",   NULL, 0.5);	/* C++ source		*/
    HTBind_setBinding("cc",     "text/plain",			"7bit",   NULL, 0.5);	/* C++ source		*/
    HTBind_setBinding("hh",     "text/plain",			"7bit",   NULL, 0.5);	/* C++ headers		*/
    HTBind_setBinding("m",      "text/plain",			"7bit",   NULL, 0.5);	/* Objective-C source	*/
    HTBind_setBinding("f90",    "text/plain",			"7bit",   NULL, 0.5);	/* Fortran 90 source	*/
    HTBind_setBinding("txt",    "text/plain",			"7bit",   NULL, 0.5);	/* Plain text		*/
    HTBind_setBinding("rtx",    "text/richtext",		"7bit",   NULL, 1.0);	/* MIME Richtext format	*/
    HTBind_setBinding("tsv",    "text/tab-separated-values",	"7bit",   NULL, 1.0);	/* Tab-separated values	*/
    HTBind_setBinding("etx",    "text/x-setext",		"7bit",   NULL, 0.9);	/* Struct Enchanced Txt	*/
    HTBind_setBinding("MPG",    "video/mpeg",			"binary", NULL, 1.0);	/* MPEG			*/
    HTBind_setBinding("mpg",    "video/mpeg",			"binary", NULL, 1.0);
    HTBind_setBinding("MPE",    "video/mpeg",			"binary", NULL, 1.0);
    HTBind_setBinding("mpe",    "video/mpeg",			"binary", NULL, 1.0);
    HTBind_setBinding("MPEG",   "video/mpeg",			"binary", NULL, 1.0);
    HTBind_setBinding("mpeg",   "video/mpeg",			"binary", NULL, 1.0);
    HTBind_setBinding("qt",     "video/quicktime",		"binary", NULL, 1.0);	/* QuickTime		*/
    HTBind_setBinding("mov",    "video/quicktime",		"binary", NULL, 1.0);
    HTBind_setBinding("avi",    "video/x-msvideo",		"binary", NULL, 1.0);	/* MS Video for Windows	*/
    HTBind_setBinding("movie",  "video/x-sgi-movie",		"binary", NULL, 1.0);	/* SGI "moviepalyer"	*/

    HTBind_setBinding("zip",    NULL,				"zip",      NULL, 1.0);	/* PKZIP		*/
    HTBind_setBinding("Z",	NULL,				"compress", NULL, 1.0);	/* Compressed data	*/
    HTBind_setBinding("gz",	NULL,				"gzip",	    NULL, 1.0);	/* Gnu Compressed data	*/

    HTBind_setBinding("*.*",     "www/unknown",			"binary", NULL, 0.1);	/* Unknown suffix */
    HTBind_setBinding("*",       "www/unknown",			"7bit",   NULL, 0.5);	/* No suffix */
}
#endif /* !HT_NO_INIT */

