/*								       HTInit.c
**	CONFIGURATION-SPECIFIC INITIALIALIZATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Define a basic set of suffixes and presentations
*/

/* Library include files */
#include "sysdep.h"
#include "HTUtils.h"
#include "HTFormat.h"
#include "HTList.h"
#include "HTProt.h"
#include "HTReqMan.h"
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
    HTConversion_add(c,"message/x-rfc822-foot",	"*/*",		HTMIMEFooter,	1.0, 0.0, 0.0);
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
    HTConversion_add(c,"text/x-nntp-list",	"*/*",		HTNewsList,	1.0, 0.0, 0.0);
    HTConversion_add(c,"text/x-nntp-over",	"*/*",		HTNewsGroup,	1.0, 0.0, 0.0);
    HTConversion_add(c,"text/x-wais-source",	"*/*",		HTWSRCConvert, 	1.0, 0.0, 0.0);
#endif

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

/*	BINDINGS BETWEEN A ENCODING AND CODERS / DECODERS
**	--------------------------- ---------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTEncoderInit (HTList * c)
{
    HTCoding_add(c, "chunked", NULL, HTChunkedDecoder, 1.0);
}

/*	REGISTER CALLBACKS FOR THE NET MANAGER
**	--------------------------------------
**	We register two often used callback functions:
**	a BEFORE and a AFTER callback
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTNetInit (void)
{
    HTNetCall_addBefore(HTLoadStart, NULL, 0);
    HTNetCall_addAfter(HTLoadTerminate, NULL, HT_ALL);
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

/*	REGISTER ALL KNOWN TRANSPORTS IN THE LIBRARY
**	--------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTTransportInit (void)
{
    HTTransport_add("tcp", HT_CH_SINGLE, HTReader_new, HTWriter_new);
    HTTransport_add("buffered_tcp", HT_CH_SINGLE, HTReader_new, HTBufferWriter_new);
#ifndef NO_UNIX_IO
    HTTransport_add("local", HT_CH_SINGLE, HTReader_new, HTWriter_new);
#else
    HTTransport_add("local", HT_CH_SINGLE, HTANSIReader_new, HTANSIWriter_new);
#endif
}

/*	REGISTER ALL KNOWN PROTOCOLS IN THE LIBRARY
**	-------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTAccessInit (void)
{
#ifndef DECNET
    HTProtocol_add("ftp", "tcp", NO, HTLoadFTP, NULL);
    HTProtocol_add("nntp", "tcp", NO, HTLoadNews, NULL);
    HTProtocol_add("news", "tcp", NO, HTLoadNews, NULL);
    HTProtocol_add("gopher", "tcp", NO, HTLoadGopher, NULL);
#ifdef HT_DIRECT_WAIS
    HTProtocol_add("wais", "", YES, HTLoadWAIS, NULL);
#endif
#endif /* DECNET */

    HTProtocol_add("http", "buffered_tcp", NO, HTLoadHTTP, NULL);
    HTProtocol_add("file", "local", NO, HTLoadFile, NULL);
    HTProtocol_add("telnet", "", YES, HTLoadTelnet, NULL);
    HTProtocol_add("tn3270", "", YES, HTLoadTelnet, NULL);
    HTProtocol_add("rlogin", "", YES, HTLoadTelnet, NULL);
}

/*	REGISTER DEFULT EVENT MANAGER
**	-----------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTEventInit (void)
{
    HTEvent_setRegisterCallback(HTEventrg_register);
    HTEvent_setUnregisterCallback(HTEventrg_unregister);
}

#if 0
/*	BINDINGS BETWEEN ICONS AND MEDIA TYPES
**	--------------------------------------
**	Not done automaticly - may be done by application!
**	For directory listings etc. you can bind a set of icons to a set of
**	media types and special icons for directories and other objects that
**	do not have a media type.
*/
/* PUBLIC void HTStdIconInit (const char * url_prefix) */
{
    const char * p = url_prefix ? url_prefix : "/internal-icon/";

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

/*     standard MIME parsers
 */
PRIVATE int allow (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    while ((field = HTNextField(&value)) != NULL) {
        HTMethod new_method;
	/* We treat them as case-insensitive! */
	if ((new_method = HTMethod_enum(field)) != METHOD_INVALID)
	    HTAnchor_appendMethods(anchor, new_method);
    }
    if (STREAM_TRACE)
        HTTrace("MIMEParser.. Methods allowed: %d\n",
		HTAnchor_methods(anchor));
    return HT_OK;
}

PRIVATE int authenticate (HTRequest * request, char * token, char * value)
{
    if (!request->challenge) request->challenge = HTAssocList_new();

    StrAllocCopy(request->scheme, "basic");	/* @@@@@@@@@ */

    HTAssocList_add(request->challenge, "WWW-authenticate", value);
    return HT_OK;
}

PRIVATE int connection (HTRequest * request, char * token, char * value)
{
    char * field;
    if ((field = HTNextField(&value)) != NULL) {
        if (!strcasecomp(field, "keep-alive")) {
	    HTNet_setPersistent(request->net, YES);
	    if (STREAM_TRACE) HTTrace("MIMEParser.. Persistent Connection\n");
	}
    }
    return HT_OK;
}

PRIVATE int content_encoding (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    while ((field = HTNextField(&value)) != NULL) {
        char * lc = field;
	while ((*lc = TOLOWER(*lc))) lc++;
	HTAnchor_addEncoding(anchor, HTAtom_for(field));
    }
    return HT_OK;
}

PRIVATE int content_language (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    while ((field = HTNextField(&value)) != NULL) {
        char * lc = field;
	while ((*lc = TOLOWER(*lc))) lc++;
	HTAnchor_addLanguage(anchor, HTAtom_for(field));
    }
    return HT_OK;
}

PRIVATE int content_length (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL)
        HTAnchor_setLength(anchor, atol(field));
    return HT_OK;
}

PRIVATE int content_transfer_encoding (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL) {
        char *lc = field;
	while ((*lc = TOLOWER(*lc))) lc++;
	HTAnchor_setTransfer(anchor, HTAtom_for(field));
    }
    return HT_OK;
}

PRIVATE int content_type (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL) {
        char *lc = field;
	while ((*lc = TOLOWER(*lc))) lc++; 
	HTAnchor_setFormat(anchor, HTAtom_for(field));
	while ((field = HTNextField(&value)) != NULL) {
	    if (!strcasecomp(field, "charset")) {
	        if ((field = HTNextField(&value)) != NULL) {
		    lc = field;
		    while ((*lc = TOLOWER(*lc))) lc++;
		    HTAnchor_setCharset(anchor, HTAtom_for(field));
		}
	    } else if (!strcasecomp(field, "level")) {
	        if ((field = HTNextField(&value)) != NULL) {
		    lc = field;
		    while ((*lc = TOLOWER(*lc))) lc++;
		    HTAnchor_setLevel(anchor, HTAtom_for(field));
		}
	    } else if (!strcasecomp(field, "boundary")) {
	        if ((field = HTNextField(&value)) != NULL) {
		    StrAllocCopy(request->boundary, field);
	        }
	    }
	}
    }
    return HT_OK;
}

PRIVATE int derived_from (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL)
        HTAnchor_setDerived(anchor, field);
    return HT_OK;
}

PRIVATE int expires (HTRequest * request, char * token, char * value)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTAnchor_setExpires(anchor, HTParseTime(value, 
					    HTRequest_userProfile(request)));
    return HT_OK;
}

PRIVATE int last_modified (HTRequest * request, char * token, char * value)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTAnchor_setLastModified(anchor, HTParseTime(value, 
					    HTRequest_userProfile(request)));
    return HT_OK;
}

PRIVATE int location (HTRequest * request, char * token, char * value)
{
    request->redirectionAnchor = HTAnchor_findAddress(HTStrip(value));
    return HT_OK;
}

PRIVATE int message_digest (HTRequest * request, char * token, char * value)
{
    if (!request->challenge) request->challenge = HTAssocList_new();
    HTAssocList_add(request->challenge, "Digest-MessageDigest", value);
    return HT_OK;
}

PRIVATE int mime_date (HTRequest * request, char * token, char * value)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTAnchor_setDate(anchor, HTParseTime(value, 
					 HTRequest_userProfile(request)));
    return HT_OK;
}

PRIVATE int newsgroups (HTRequest * request, char * token, char * value)
{
    /* HTRequest_net(request)->nntp = YES; */	       	/* Due to news brain damage */
    return HT_OK;
}

PRIVATE int retry_after (HTRequest * request, char * token, char * value)
{
    request->retry_after = HTParseTime(value, 
				       HTRequest_userProfile(request));
    return HT_OK;
}

PRIVATE int title (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL)
        HTAnchor_setTitle(anchor, field);
    return HT_OK;
}

PRIVATE int version (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL)
        HTAnchor_setVersion(anchor, field);
    return HT_OK;
}


/*	REGISTER ALL HTTP/1.1 MIME HEADERS
**	--------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTMIMEInit()
{
    struct {
        char * string;
	HTParserCallback * pHandler;
    } fixedHandlers[] = {
	{"allow", &allow}, 
	{"accept-language", NULL}, 
	{"accept-charset", NULL}, 
	{"accept", NULL}, 
	{"accept-encoding", NULL}, 
	{"connection", &connection}, 
	{"content-encoding", &content_encoding}, 
	{"content-language", &content_language}, 
	{"content-length", &content_length}, 
	{"content-transfer-encoding", &content_transfer_encoding}, 
	{"content-type", &content_type},
	{"date", &mime_date}, 
	{"derived-from", &derived_from}, 
	{"digest-MessageDigest", &message_digest}, 
        {"expires", &expires}, 
	{"keep-alive", NULL}, 
	{"last-modified", &last_modified}, 
/*	{"link", &link},  */
	{"location", &location}, 
	{"mime-version", NULL}, 
	{"newsgroups", &newsgroups}, 
	{"retry-after", &retry_after}, 
	{"server", NULL}, 
	{"title", &title}, 
	{"transfer-encoding", &content_transfer_encoding}, 
/*	{"uri", &uri_header},  */
	{"version", &version}, 
	{"www-authenticate", &authenticate}, 
    };
    int i;

    for (i = 0; i < sizeof(fixedHandlers)/sizeof(fixedHandlers[0]); i++)
        HTHeader_addParser(fixedHandlers[i].string, NO, 
			   fixedHandlers[i].pHandler);
}

