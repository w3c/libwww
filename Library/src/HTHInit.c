/*
**	CONFIGURATION-SPECIFIC INITIALIALIZATION FOR HTML PARSER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Set up the HTML parsers in libwww
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTHInit.h"				         /* Implemented here */

/* ------------------------------------------------------------------------- */

/*	BINDINGS BETWEEN A SOURCE MEDIA TYPE AND A DEST MEDIA TYPE (CONVERSION)
**	----------------------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTMLInit (HTList * c)
{
    /*
    ** This set of converters uses the HTML/HText interface.
    ** If you do not want this interface then replace them!
    */
    HTConversion_add(c,"text/html",		"www/present",	HTMLPresent,	1.0, 0.0, 0.0);
    HTConversion_add(c,"text/plain",		"www/present",	HTPlainPresent,	1.0, 0.0, 0.0);
    HTConversion_add(c,"text/html",		"text/x-c",	HTMLToC,	0.5, 0.0, 0.0);
    HTConversion_add(c,"text/html",		"text/plain",	HTMLToPlain,	0.5, 0.0, 0.0);
    HTConversion_add(c,"text/html",	       	"text/latex",	HTMLToTeX,	1.0, 0.0, 0.0);
}
