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
#include "HTProt.h"
#include "HTInit.h"				         /* Implemented here */

/* ------------------------------------------------------------------------- */

/*	BINDINGS BETWEEN A SOURCE MEDIA TYPE AND A DEST MEDIA TYPE (CONVERSION)
**	----------------------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTConverterInit (HTList * c)
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

    /*
    ** You can get debug information out through the debug stream if you set
    ** the debug format appropriately
    */
    HTConversion_add(c,"*/*",			"www/debug",	HTBlackHoleConverter,	1.0, 0.0, 0.0);

    /*
    ** These are converters that converts to something other than www/present,
    ** that is not directly outputting someting to the user on the screen
    */
    HTConversion_add(c,"message/rfc822",	"*/*",		HTMIMEConvert,	1.0, 0.0, 0.0);
    HTConversion_add(c,"multipart/*",		"*/*",		HTBoundary,	1.0, 0.0, 0.0);
    HTConversion_add(c,"text/plain",		"text/html",	HTPlainToHTML,	1.0, 0.0, 0.0);

    /*
    ** The following conversions are converting ASCII output from various
    ** protocols to HTML objects.
    */
    HTConversion_add(c,"text/x-http",		"*/*",		HTTPStatus_new,	1.0, 0.0, 0.0);
#if 0
    HTConversion_add(c,"text/x-gopher",		"www/present",	HTGopherMenu,	1.0, 0.0, 0.0);
    HTConversion_add(c,"text/x-cso",		"www/present",	HTGopherCSO,	1.0, 0.0, 0.0);
#endif
    HTConversion_add(c,"text/x-nntp-list",	"*/*",		HTNewsList,	1.0, 0.0, 0.0);
    HTConversion_add(c,"text/x-nntp-over",	"*/*",		HTNewsGroup,	1.0, 0.0, 0.0);
    HTConversion_add(c,"text/x-wais-source",	"*/*",		HTWSRCConvert, 	1.0, 0.0, 0.0);

    /*
    ** We also register a special content type guess stream that can figure out
    ** the content type by reading the first bytes of the stream
    */
    HTConversion_add(c,"www/unknown",		"*/*",		HTGuess_new,	1.0, 0.0, 0.0);

    /*
    ** Handling Rule files is handled just like any other stream
    ** This converter reads a rule file and generates the rules
    */
    HTConversion_add(c,"application/x-www-rules","*/*",		HTRules,	1.0, 0.0, 0.0);

    /*
    ** This dumps all other formats to local disk without any further
    ** action taken
    */
    HTConversion_add(c,"*/*",			"www/present",	HTSaveLocally,	0.3, 0.0, 0.0);
}

/*	BINDINGS BETWEEN MEDIA TYPES AND EXTERNAL VIEWERS/PRESENTERS
**	------------------------------------------------------------
**	Not done automaticly - may be done by application!
**	The data objects are stored in temporary files before the external
**	program is called
*/
PUBLIC void HTPresenterInit (HTList * c)
{
#ifdef NeXT
    HTPresentation_add(c,"application/postscript", "open %s",	NULL, 1.0, 2.0, 0.0);
    /* The following needs the GIF previewer -- you might not have it. */

    HTPresentation_add(c,"image/gif", 		"open %s", 	NULL, 0.3, 2.0, 0.0);
    HTPresentation_add(c,"image/tiff", 	"open %s", 	NULL, 1.0, 2.0, 0.0);
    HTPresentation_add(c,"audio/basic", 	"open %s", 	NULL, 1.0, 2.0, 0.0);
    HTPresentation_add(c,"*/*", 		"open %s", 	NULL, 0.05, 0.0, 0.0); 
#else
    if (getenv("DISPLAY")) {	/* Must have X11 */
	HTPresentation_add(c,"application/postscript", "ghostview %s",	NULL, 1.0, 3.0, 0.0);
	HTPresentation_add(c,"image/gif", 	"xv %s",	NULL, 1.0, 3.0, 0.0);
	HTPresentation_add(c,"image/tiff", 	"xv %s",	NULL, 1.0, 3.0, 0.0);
	HTPresentation_add(c,"image/jpeg", 	"xv %s",	NULL, 1.0, 3.0, 0.0);
 	HTPresentation_add(c,"image/png",	"xv %s",	NULL, 1.0, 3.0, 0.0);
    }
#endif
}


/*	PRESENTERS AND CONVERTERS AT THE SAME TIME
**	------------------------------------------
**	Not done automaticly - may be done by application!
**	This function is only defined in order to preserve backward
**	compatibility.
*/
PUBLIC void HTFormatInit (HTList * c)
{
    HTConverterInit(c);
    HTPresenterInit(c);

}


/*	REGISTER CALLBACKS FOR THE NET MANAGER
**	--------------------------------------
**	We register two often used callback functions:
**	a BEFORE and a AFTER callback
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTNetInit (void)
{
    HTNetCall_addBefore(HTLoadStart, 0);
    HTNetCall_addAfter(HTLoadTerminate, HT_ALL);
}


/*	REGISTER CALLBACKS FOR THE ALERT MANAGER
**	----------------------------------------
**	We register a set of alert messages
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTAlertInit (void)
{
    HTAlert_add(HTProgress, HT_A_PROGRESS);
    HTAlert_add(HTError_print, HT_A_MESSAGE);
    HTAlert_add(HTConfirm, HT_A_CONFIRM);
    HTAlert_add(HTPrompt, HT_A_PROMPT);
    HTAlert_add(HTPromptPassword, HT_A_SECRET);
    HTAlert_add(HTPromptUsernameAndPassword, HT_A_USER_PW);
}


/*	REGISTER ALL KNOWN PROTOCOLS IN THE LIBRARY
**	-------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTAccessInit (void)
{
#ifndef DECNET
    HTProtocol_add("ftp", NO, HTLoadFTP, NULL);
    HTProtocol_add("nntp", NO, HTLoadNews, NULL);
    HTProtocol_add("news", NO, HTLoadNews, NULL);
    HTProtocol_add("gopher", NO, HTLoadGopher, NULL);
#ifdef HT_DIRECT_WAIS
    HTProtocol_add("wais", YES, HTLoadWAIS, NULL);
#endif
#endif /* DECNET */

    HTProtocol_add("http", NO, HTLoadHTTP, NULL);
    HTProtocol_add("file", NO, HTLoadFile, NULL);
    HTProtocol_add("telnet", YES, HTLoadTelnet, NULL);
    HTProtocol_add("tn3270", YES, HTLoadTelnet, NULL);
    HTProtocol_add("rlogin", YES, HTLoadTelnet, NULL);
}

#if 0
/*	BINDINGS BETWEEN ICONS AND MEDIA TYPES
**	--------------------------------------
**	Not done automaticly - may be done by application!
**	For directory listings etc. you can bind a set of icons to a set of
**	media types and special icons for directories and other objects that
**	do not have a media type.
*/
/* PUBLIC void HTStdIconInit (CONST char * url_prefix) */
{
    CONST char * p = url_prefix ? url_prefix : "/internal-icon/";

    HTAddBlankIcon  (prefixed(p,"blank.xbm"),	NULL	);
    HTAddDirIcon    (prefixed(p,"directory.xbm"),"DIR"	);
    HTAddParentIcon (prefixed(p,"back.xbm"),	"UP"	);
    HTAddUnknownIcon(prefixed(p,"unknown.xbm"),	NULL	);
    HTAddIcon(prefixed(p,"unknown.xbm"),	NULL,	"*/*");
    HTAddIcon(prefixed(p,"binary.xbm"),		"BIN",	"binary");
    HTAddIcon(prefixed(p,"unknown.xbm"),	NULL,	"www/unknown");
    HTAddIcon(prefixed(p,"text.xbm"),		"TXT",	"text/*");
    HTAddIcon(prefixed(p,"image.xbm"),		"IMG",	"image/*");
    HTAddIcon(prefixed(p,"movie.xbm"),		"MOV",	"video/*");
    HTAddIcon(prefixed(p,"sound.xbm"),		"AU",	"audio/*");
    HTAddIcon(prefixed(p,"tar.xbm"),		"TAR",	"multipart/x-tar");
    HTAddIcon(prefixed(p,"tar.xbm"),		"TAR",	"multipart/x-gtar");
    HTAddIcon(prefixed(p,"compressed.xbm"),	"CMP",	"x-compress");
    HTAddIcon(prefixed(p,"compressed.xbm"),	"GZP",	"x-gzip");
    HTAddIcon(prefixed(p,"index.xbm"),		"IDX",	"application/x-gopher-index");
    HTAddIcon(prefixed(p,"index2.xbm"),		"CSO",	"application/x-gopher-cso");
    HTAddIcon(prefixed(p,"telnet.xbm"),		"TEL",	"application/x-gopher-telnet");
    HTAddIcon(prefixed(p,"unknown.xbm"),       	"DUP",	"application/x-gopher-duplicate");
    HTAddIcon(prefixed(p,"unknown.xbm"),	"TN",	"application/x-gopher-tn3270");
}
#endif
