/*								   HTBInit.c
**	STANDARD BINDINGS BETWEEN FILE SUFFIXES AND MEDIA TYPES
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Define a basic set of suffixes and presentations
*/

/* Library include files */
#include "wwwsys.h"
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
**
**		C-T	Content type
**		C-T-E	Content Transfer encoding
**		C-E	Content encoding
*/
PUBLIC void HTFileInit (void)
{
    /*		Suffix	C-T				C-E		C-T-E 		Lang	Quality */

    HTBind_add("ai",	"application/postscript",	NULL,		"8bit",		NULL,	0.5);	/* Adobe Illustrator	*/
    HTBind_add("aif",	"audio/x-aiff",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("aifc",	"audio/x-aiff",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("aiff",	"audio/x-aiff",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("au",	"audio/basic",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("asf",	"video/x-ms-asf",		NULL,		"binary",	NULL,	1.0);	/* MS Active Streaming	*/
    HTBind_add("avi",	"video/x-msvideo",		NULL,		"binary",	NULL,	1.0);	/* MS Video for Windows	*/
    HTBind_add("bat",	"application/octet-stream",	NULL,		"binary",	NULL,	1.0);	/* Uninterpreted binary	*/
    HTBind_add("bcpio",	"application/x-bcpio",		NULL,		"binary",	NULL,	1.0);	/* Old binary CPIO	*/
    HTBind_add("bin",	"application/octet-stream",	NULL,		"binary",	NULL,	1.0);	/* Uninterpreted binary	*/
    HTBind_add("bmp",	"image/bmp",			NULL,		"binary",	NULL,	1.0);	/* MS Windows bitmap	*/
    HTBind_add("c",	"text/plain",			NULL,		"7bit",		NULL,	0.5);	/* C source		*/
    HTBind_add("cc",	"text/plain",			NULL,		"7bit",		NULL,	0.5);	/* C++ source		*/
    HTBind_add("cpp",	"text/plain",			NULL,		"7bit",		NULL,	0.5);	/* C++ source		*/
    HTBind_add("cdf",	"application/x-netcdf",		NULL,		"binary",	NULL,	1.0);
    HTBind_add("cgm",	"image/cgm",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("conf",	"application/x-www-rules",	NULL,		"8bit",		NULL,	1.0);	/* CONFIGURATION FILE	*/
    HTBind_add("cpio",	"application/x-cpio",		NULL,		"binary",	NULL,	1.0);	/* POSIX CPIO		*/
    HTBind_add("csh",	"application/x-csh",		NULL,		"7bit",		NULL,	0.5);	/* C-shell script	*/
    HTBind_add("css",	"text/css",			NULL,		"8bit",		NULL,	1.0);	/* CSS Stylesheet	*/
    HTBind_add("dll",	"application/octet-stream",	NULL,		"binary",	NULL,	1.0);	/* Uninterpreted binary	*/
    HTBind_add("doc",	"application/msword",		NULL,		"binary",	NULL,	0.5);	/* MS Word format	*/
    HTBind_add("dot",	"application/msword",		NULL,		"binary",	NULL,	0.5);	/* MS Word format	*/
    HTBind_add("dvi",	"application/x-dvi",		NULL,		"binary",	NULL,	1.0);	/* TeX DVI		*/
    HTBind_add("eps",	"application/postscript",	NULL,		"8bit",		NULL,	0.8);
    HTBind_add("etx",	"text/x-setext",		NULL,		"7bit",		NULL,	0.9);	/* Struct Enchanced Txt	*/
    HTBind_add("exe",	"application/octet-stream",	NULL,		"binary",	NULL,	1.0);	/* Uninterpreted binary	*/
    HTBind_add("f90",	"text/plain",			NULL,		"7bit",		NULL,	0.5);	/* Fortran 90 source	*/
    HTBind_add("gif",	"image/gif",			NULL,		"binary",	NULL,	1.0);	/* GIF			*/
    HTBind_add("h",	"text/plain",			NULL,		"7bit",		NULL,	0.5);	/* C headers		*/
    HTBind_add("hdf",	"application/x-hdf",		NULL,		"binary",	NULL,	1.0);	/* NCSA HDF data file	*/
    HTBind_add("hh",	"text/plain",			NULL,		"7bit",		NULL,	0.5);	/* C++ headers		*/
    HTBind_add("htm",	"text/html",			NULL,		"8bit",		NULL,	1.0);	/* HTML			*/
    HTBind_add("html",	"text/html",			NULL,		"8bit",		NULL,	1.0);	/* HTML			*/
    HTBind_add("ief",	"image/ief",			NULL,		"binary",	NULL,	1.0);	/* Image Exchange fmt	*/
    HTBind_add("jpe",	"image/jpeg",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("jpeg",	"image/jpeg",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("jpg",	"image/jpeg",			NULL,		"binary",	NULL,	1.0);	/* JPEG			*/
    HTBind_add("latex",	"text/latex",			NULL,		"8bit",		NULL,	1.0);	/* LaTeX source		*/
    HTBind_add("m",	"text/plain",			NULL,		"7bit",		NULL,	0.5);	/* Objective-C source	*/
    HTBind_add("man",	"application/x-troff-man",	NULL,		"7bit",		NULL,	0.5);	/* Troff with man macros*/
    HTBind_add("me",	"application/x-troff-me",	NULL,		"7bit",		NULL,	0.5);	/* Troff with me macros	*/
    HTBind_add("mht",	"message/rfc822",		NULL,		"8bit",		NULL,	1.0);	/* MHTML format	*/
    HTBind_add("mhtml",	"message/rfc822",		NULL,		"8bit",		NULL,	1.0);	/* MHTML format	*/
    HTBind_add("mime",	"message/rfc822",		NULL,		"8bit",		NULL,	1.0);
    HTBind_add("mov",	"video/quicktime",		NULL,		"binary",	NULL,	1.0);
    HTBind_add("movie",	"video/x-sgi-movie",		NULL,		"binary",	NULL,	1.0);	/* SGI "moviepalyer"	*/
    HTBind_add("mpe",	"video/mpeg",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("mpeg",	"video/mpeg",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("mpg",	"video/mpeg",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("ms",	"application/x-troff-ms",	NULL,		"7bit",		NULL,	0.5);	/* Troff with ms macros	*/
    HTBind_add("nc",	"application/x-netcdf",		NULL,		"binary",	NULL,	1.0);	/* Unidata netCDF data	*/
    HTBind_add("oda",	"application/oda",		NULL,		"binary",	NULL,	1.0);
    HTBind_add("pbm",	"image/x-portable-bitmap",	NULL,		"binary",	NULL,	1.0);	/* PBM Bitmap format	*/
    HTBind_add("pcu",	"application/x-pics-user",	NULL,		"8bit",		NULL,	1.0);	/* PICS User */
    HTBind_add("pdf",	"application/pdf",		NULL,		"binary",	NULL,	1.0);
    HTBind_add("pgm",	"image/x-portable-graymap",	NULL,		"binary",	NULL,	1.0);	/* PBM Graymap format	*/
    HTBind_add("pgp",  	"application/pgp-encrypted",	NULL,		"binary",	NULL,	1.0);	/* PGP encrypted 	*/
    HTBind_add("png",	"image/png",			NULL,		"binary",	NULL,	1.0);	/* PNG			*/
    HTBind_add("pnm",	"image/x-portable-anymap",	NULL,		"binary",	NULL,	1.0);	/* PBM Anymap format	*/
    HTBind_add("ppt",	"application/vnd.ms-powerpoint",NULL,		"binary",	NULL,	0.5);	/* MS Powerpoint	*/
    HTBind_add("ps",	"application/postscript",	NULL,		"8bit",		NULL,	0.8);
    HTBind_add("qt",	"video/quicktime",		NULL,		"binary",	NULL,	1.0);	/* QuickTime		*/
    HTBind_add("ras",	"image/cmu-raster",		NULL,		"binary",	NULL,	1.0);
    HTBind_add("rdf",	"text/rdf",			NULL,		"binary",	NULL,	1.0);	/* rdf	*/
    HTBind_add("rgb",	"image/x-rgb",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("roff",	"application/x-troff",		NULL,		"7bit",		NULL,	0.5);
    HTBind_add("rtf",	"text/rtf",			NULL,		"7bit",		NULL,	1.0);	/* RTF			*/
    HTBind_add("rtx",	"text/richtext",		NULL,		"7bit",		NULL,	1.0);	/* MIME Richtext format	*/
    HTBind_add("sh",	"application/x-sh",		NULL,		"7bit",		NULL,	0.5);	/* Shell-script		*/
    HTBind_add("sig",	"application/pgp-signature",	NULL,		"binary",	NULL,	1.0);	/* PGP signature	*/
    HTBind_add("shar",	"application/x-shar",		NULL,		"8bit",		NULL,	1.0);	/* Shell archive	*/
    HTBind_add("snd",	"audio/basic",			NULL,		"binary",	NULL,	1.0);	/* Audio		*/
    HTBind_add("src",	"application/x-wais-source",	NULL,		"7bit",		NULL,	1.0);	/* WAIS source		*/
    HTBind_add("sv4cpio","application/x-sv4cpio",	NULL,		"binary",	NULL,	1.0);	/* SVR4 CPIO		*/
    HTBind_add("sv4crc","application/x-sv4crc",		NULL,		"binary",	NULL,	1.0);	/* SVR4 CPIO with CRC	*/
    HTBind_add("t",	"application/x-troff",		NULL,		"7bit",		NULL,	0.5);	/* Troff		*/
    HTBind_add("tar",	"application/tar",		NULL,		"binary",	NULL,	1.0);	/* 4.3BSD tar		*/
    HTBind_add("tar",	"application/x-tar",		NULL,		"binary",	NULL,	1.0);	/* 4.3BSD tar		*/
    HTBind_add("tcl",	"application/x-tcl",		NULL,		"7bit",		NULL,	0.5);	/* TCL-script		*/
    HTBind_add("tex",	"text/tex",			NULL,		"8bit",		NULL,	1.0);	/* TeX source		*/
    HTBind_add("texi",	"application/x-texinfo",	NULL,		"7bit",		NULL,	1.0);	/* Texinfo		*/
    HTBind_add("tif",	"image/tiff",			NULL,		"binary",	NULL,	1.0);	/* TIFF			*/
    HTBind_add("tiff",	"image/tiff",			NULL,		"binary",	NULL,	1.0);
    HTBind_add("tr",	"application/x-troff",		NULL,		"7bit",		NULL,	0.5);
    HTBind_add("tsv",	"text/tab-separated-values",	NULL,		"7bit",		NULL,	1.0);	/* Tab-separated values	*/
    HTBind_add("txt",	"text/plain",			NULL,		"7bit",		NULL,	0.5);	/* Plain text		*/
    HTBind_add("ustar",	"application/x-ustar",		NULL,		"binary",	NULL,	1.0);	/* POSIX tar		*/
    HTBind_add("wav",	"audio/x-wav",			NULL,		"binary",	NULL,	1.0);	/* Windows+ WAVE format	*/
    HTBind_add("xbm",	"image/x-xbitmap",		NULL,		"binary",	NULL,	1.0);	/* X bitmap		*/
    HTBind_add("xls",	"application/vnd.ms-excel",	NULL,		"binary",	NULL,	0.5);	/* MS Excel 		*/
    HTBind_add("xml",	"application/xml",    		NULL,		"binary",	NULL,	1.0);	/* xml			*/
    HTBind_add("xpm",	"image/x-xpixmap",		NULL,		"binary",	NULL,	1.0);	/* X pixmap format	*/
    HTBind_add("xwd",	"image/x-xwindowdump",		NULL,		"binary",	NULL,	1.0);	/* X window dump (xwd)	*/

    HTBind_add("zip",	"application/zip",		NULL,		"binary",	NULL,	1.0);	/* PKZIP		*/
    HTBind_add("Z",	 NULL,				"compress", 	"binary",	NULL,	1.0);	/* Compressed data	*/
    HTBind_add("gz",	 NULL,				"gzip",		"binary",	NULL,	1.0);	/* Gnu Compressed data	*/

    HTBind_add("*.*",	"www/unknown",			NULL,		"binary",	NULL,	0.1);	/* Unknown suffix */
    HTBind_add("*",	"www/unknown",			NULL,		"7bit",		NULL,	0.5);	/* No suffix */
}

