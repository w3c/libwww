/*								   HTBInit.c
**	STANDARD BINDINGS BETWEEN FILE SUFFIXES AND MEDIA TYPES
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
#include "HTBind.h"
#include "HTBInit.h"				         /* Implemented here */

/* ------------------------------------------------------------------------- */

/*	BINDINGS BETWEEN FILE EXTENSIONS AND MEDIA TYPES
**	------------------------------------------------
**	Not done automaticly - may be done by application!
**	The LAST suffix for a type is that used for temporary files of that
**	type. The quality is an apriori bias as to whether the file should be
**	used.  Not that different suffixes can be used to represent files
**	which are of the same format but are originals or regenerated,
**	with different values.
*/
PUBLIC void HTFileInit (void)
{
    /*		       Suffix	 Content-Type		        Encoding  Lang	Quality	*/

    HTBind_add("mime",   "message/rfc822",		"8bit",   NULL, 1.0);	/* Internal -- MIME is	*/
                                                                       /* not recursive	*/
    HTBind_add("ai",     "application/postscript",	"8bit",   NULL, 0.5);	/* Adobe Illustrator	*/
    HTBind_add("aif",    "audio/x-aiff",		"binary", NULL, 1.0);
    HTBind_add("aifc",   "audio/x-aiff",		"binary", NULL, 1.0);
    HTBind_add("aiff",   "audio/x-aiff",		"binary", NULL, 1.0);
    HTBind_add("au",     "audio/basic",			"binary", NULL, 1.0);
    HTBind_add("avi",    "video/x-msvideo",		"binary", NULL, 1.0);	/* MS Video for Windows	*/
    HTBind_add("bcpio",  "application/x-bcpio",		"binary", NULL, 1.0);	/* Old binary CPIO	*/
    HTBind_add("bin",    "application/octet-stream",	"binary", NULL, 1.0); /* Uninterpreted binary	*/
    HTBind_add("c",      "text/plain",			"7bit",   NULL, 0.5);	/* C source		*/
    HTBind_add("C",      "text/plain",			"7bit",   NULL, 0.5);	/* C++ source		*/
    HTBind_add("cc",     "text/plain",			"7bit",   NULL, 0.5);	/* C++ source		*/
    HTBind_add("cdf",    "application/x-netcdf",	"binary", NULL, 1.0);
    HTBind_add("cpio",   "application/x-cpio",		"binary", NULL, 1.0);	/* POSIX CPIO		*/
    HTBind_add("csh",    "application/x-csh",		"7bit",   NULL, 0.5);	/* C-shell script	*/
    HTBind_add("dvi",    "application/x-dvi",		"binary", NULL, 1.0);	/* TeX DVI		*/
    HTBind_add("eps",    "application/postscript",	"8bit",   NULL, 0.8);
    HTBind_add("etx",    "text/x-setext",		"7bit",   NULL, 0.9);	/* Struct Enchanced Txt	*/
    HTBind_add("f90",    "text/plain",			"7bit",   NULL, 0.5);	/* Fortran 90 source	*/
    HTBind_add("gif",    "image/gif",			"binary", NULL, 1.0);	/* GIF			*/
    HTBind_add("gtar",   "application/x-gtar",		"binary", NULL, 1.0);	/* Gnu tar		*/
    HTBind_add("h",      "text/plain",			"7bit",   NULL, 0.5);	/* C headers		*/
    HTBind_add("hdf",    "application/x-hdf",		"binary", NULL, 1.0);	/* NCSA HDF data file	*/
    HTBind_add("hh",     "text/plain",			"7bit",   NULL, 0.5);	/* C++ headers		*/
    HTBind_add("html",   "text/html",			"8bit",   NULL, 1.0);	/* HTML			*/
    HTBind_add("ief",    "image/ief",			"binary", NULL, 1.0);	/* Image Exchange fmt	*/
    HTBind_add("JPE",    "image/jpeg",			"binary", NULL, 1.0);
    HTBind_add("jpe",    "image/jpeg",			"binary", NULL, 1.0);
    HTBind_add("JPEG",   "image/jpeg",			"binary", NULL, 1.0);
    HTBind_add("jpeg",   "image/jpeg",			"binary", NULL, 1.0);
    HTBind_add("JPG",    "image/jpeg",			"binary", NULL, 1.0);
    HTBind_add("jpg",    "image/jpeg",			"binary", NULL, 1.0);	/* JPEG			*/
    HTBind_add("latex",  "application/x-latex",		"8bit",   NULL, 1.0);	/* LaTeX source		*/
    HTBind_add("m",      "text/plain",			"7bit",   NULL, 0.5);	/* Objective-C source	*/
    HTBind_add("man",    "application/x-troff-man",	"7bit",   NULL, 0.5);	/* Troff with man macros*/
    HTBind_add("me",     "application/x-troff-me",	"7bit",   NULL, 0.5);	/* Troff with me macros	*/
    HTBind_add("mov",    "video/quicktime",		"binary", NULL, 1.0);
    HTBind_add("movie",  "video/x-sgi-movie",		"binary", NULL, 1.0);	/* SGI "moviepalyer"	*/
    HTBind_add("MPE",    "video/mpeg",			"binary", NULL, 1.0);
    HTBind_add("mpe",    "video/mpeg",			"binary", NULL, 1.0);
    HTBind_add("MPEG",   "video/mpeg",			"binary", NULL, 1.0);
    HTBind_add("mpeg",   "video/mpeg",			"binary", NULL, 1.0);
    HTBind_add("mpg",    "video/mpeg",			"binary", NULL, 1.0);
    HTBind_add("MPG",    "video/mpeg",			"binary", NULL, 1.0);	/* MPEG			*/
    HTBind_add("ms",     "application/x-troff-ms",	"7bit",   NULL, 0.5);	/* Troff with ms macros	*/
    HTBind_add("nc",     "application/x-netcdf",	"binary", NULL, 1.0);	/* Unidata netCDF data	*/
    HTBind_add("oda",    "application/oda",		"binary", NULL, 1.0);
    HTBind_add("pbm",    "image/x-portable-bitmap",	"binary", NULL, 1.0);	/* PBM Bitmap format	*/
    HTBind_add("pdf",    "application/pdf",		"binary", NULL, 1.0);
    HTBind_add("pgm",    "image/x-portable-graymap",	"binary", NULL, 1.0);	/* PBM Graymap format	*/
    HTBind_add("png",    "image/png",			"binary", NULL, 1.0);	/* PNG			*/
    HTBind_add("pnm",    "image/x-portable-anymap",	"binary", NULL, 1.0);	/* PBM Anymap format	*/
    HTBind_add("ppm",    "image/x-portable-pixmap",	"binary", NULL, 1.0);	/* PBM Pixmap format	*/
    HTBind_add("PS",     "application/postscript",	"8bit",	  NULL, 0.8);	/* PostScript		*/
    HTBind_add("ps",     "application/postscript",	"8bit",   NULL, 0.8);
    HTBind_add("qt",     "video/quicktime",		"binary", NULL, 1.0);	/* QuickTime		*/
    HTBind_add("ras",    "image/cmu-raster",		"binary", NULL, 1.0);
    HTBind_add("rgb",    "image/x-rgb",			"binary", NULL, 1.0);
    HTBind_add("roff",   "application/x-troff",		"7bit",   NULL, 0.5);
    HTBind_add("rtf",    "application/x-rtf",		"7bit",	  NULL, 1.0);	/* RTF			*/
    HTBind_add("rtx",    "text/richtext",		"7bit",   NULL, 1.0);	/* MIME Richtext format	*/
    HTBind_add("sh",     "application/x-sh",		"7bit",   NULL, 0.5);	/* Shell-script		*/
    HTBind_add("shar",   "application/x-shar",		"8bit",   NULL, 1.0);	/* Shell archive	*/
    HTBind_add("snd",    "audio/basic",			"binary", NULL, 1.0);	/* Audio		*/
    HTBind_add("src",    "application/x-wais-source",	"7bit",   NULL, 1.0);	/* WAIS source		*/
    HTBind_add("sv4cpio","application/x-sv4cpio",	"binary", NULL, 1.0);	/* SVR4 CPIO		*/
    HTBind_add("sv4crc", "application/x-sv4crc",	"binary", NULL, 1.0);	/* SVR4 CPIO with CRC	*/
    HTBind_add("t",      "application/x-troff",		"7bit",   NULL, 0.5);	/* Troff		*/
    HTBind_add("tar",    "application/x-tar",		"binary", NULL, 1.0);	/* 4.3BSD tar		*/
    HTBind_add("tcl",    "application/x-tcl",		"7bit",   NULL, 0.5);	/* TCL-script		*/
    HTBind_add("tex",    "application/x-tex",		"8bit",   NULL, 1.0);	/* TeX source		*/
    HTBind_add("texi",   "application/x-texinfo",	"7bit",   NULL, 1.0);	/* Texinfo		*/
    HTBind_add("texinfo","application/x-texinfo",	"7bit",   NULL, 1.0);
    HTBind_add("tif",    "image/tiff",			"binary", NULL, 1.0);	/* TIFF			*/
    HTBind_add("tiff",   "image/tiff",			"binary", NULL, 1.0);
    HTBind_add("tr",     "application/x-troff",		"7bit",   NULL, 0.5);
    HTBind_add("tsv",    "text/tab-separated-values",	"7bit",   NULL, 1.0);	/* Tab-separated values	*/
    HTBind_add("txt",    "text/plain",			"7bit",   NULL, 0.5);	/* Plain text		*/
    HTBind_add("ustar",  "application/x-ustar",		"binary", NULL, 1.0);	/* POSIX tar		*/
    HTBind_add("wav",    "audio/x-wav",			"binary", NULL, 1.0);	/* Windows+ WAVE format	*/
    HTBind_add("xbm",    "image/x-xbitmap",		"binary", NULL, 1.0);	/* X bitmap		*/
    HTBind_add("xpm",    "image/x-xpixmap",		"binary", NULL, 1.0);	/* X pixmap format	*/
    HTBind_add("xwd",    "image/x-xwindowdump",		"binary", NULL, 1.0);	/* X window dump (xwd)	*/

    HTBind_add("zip",    NULL,				"zip",      NULL, 1.0);	/* PKZIP		*/
    HTBind_add("Z",	NULL,				"compress", NULL, 1.0);	/* Compressed data	*/
    HTBind_add("gz",	NULL,				"gzip",	    NULL, 1.0);	/* Gnu Compressed data	*/

    HTBind_add("conf",    "application/x-www-rules",		"8bit",	  NULL, 1.0);	/* CONFIGURATION FILE	*/

    HTBind_add("*.*",     "www/unknown",			"binary", NULL, 0.1);	/* Unknown suffix */
    HTBind_add("*",       "www/unknown",			"7bit",   NULL, 0.5);	/* No suffix */
}

