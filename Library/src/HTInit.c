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
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTInit.h"				         /* Implemented here */

#ifndef W3C_ICONS
#define W3C_ICONS	"/tmp"
#endif

#define ICON_LOCATION	"/internal-icon/"

/* ------------------------------------------------------------------------- */

/*	BINDINGS BETWEEN A SOURCE MEDIA TYPE AND A DEST MEDIA TYPE (CONVERSION)
**	----------------------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTConverterInit (HTList * c)
{
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
    HTConversion_add(c,"text/x-wais-source",	"*/*",		HTWSRCConvert, 	1.0, 0.0, 0.0);
#endif

    HTConversion_add(c,"text/x-nntp-list",	"*/*",		HTNewsList,	1.0, 0.0, 0.0);
    HTConversion_add(c,"text/x-nntp-over",	"*/*",		HTNewsGroup,	1.0, 0.0, 0.0);

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
    HTCoding_add(c, "chunked", HTChunkedEncoder, HTChunkedDecoder, 1.0);
}

/*	REGISTER BEFORE FILTERS
**	-----------------------
**	The BEFORE filters handle proxies, caches, rule files etc.
**	The filters are called in the order by which the are registered
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTBeforeInit (void)
{
    HTNetCall_addBefore(HTCacheFilter, NULL, 0);
    HTNetCall_addBefore(HTCredentialsFilter, NULL, 0);
    HTNetCall_addBefore(HTRuleFilter, NULL, 0);
    HTNetCall_addBefore(HTProxyFilter, NULL, 0);
}

/*	REGISTER AFTER FILTERS
**	----------------------
**	The AFTER filters handle error messages, logging, redirection,
**	authentication etc.
**	The filters are called in the order by which the are registered
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTAfterInit (void)
{
    HTNetCall_addAfter(HTAuthFilter, NULL, HT_NO_ACCESS);
    HTNetCall_addAfter(HTRedirectFilter, NULL, HT_TEMP_REDIRECT);
    HTNetCall_addAfter(HTRedirectFilter, NULL, HT_PERM_REDIRECT);
    HTNetCall_addAfter(HTLogFilter, NULL, HT_ALL);
    HTNetCall_addAfter(HTInfoFilter, NULL, HT_ALL);
}

/*	REGISTER DEFAULT AUTHENTICATION SCHEMES
**	---------------------------------------
**	This function registers the BASIC access authentication
*/
PUBLIC void HTAAInit (void)
{
    HTAA_newModule ("basic", HTBasic_generate, HTBasic_parse, HTBasic_delete);
}

/*	REGISTER BEFORE AND AFTER FILTERS
**	---------------------------------
**	We register a commonly used set of BEFORE and AFTER filters.
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTNetInit (void)
{
    HTBeforeInit();
    HTAfterInit();
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
PUBLIC void HTProtocolInit (void)
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

/*	REGISTER ALL KNOWN PROTOCOLS IN THE LIBRARY PREEMPTIVELY
**	--------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTProtocolPreemptiveInit (void)
{
#ifndef DECNET
    HTProtocol_add("ftp", "tcp", YES, HTLoadFTP, NULL);
    HTProtocol_add("nntp", "tcp", YES, HTLoadNews, NULL);
    HTProtocol_add("news", "tcp", YES, HTLoadNews, NULL);
    HTProtocol_add("gopher", "tcp", YES, HTLoadGopher, NULL);
#ifdef HT_DIRECT_WAIS
    HTProtocol_add("wais", "", YES, HTLoadWAIS, NULL);
#endif
#endif /* DECNET */

    HTProtocol_add("http", "buffered_tcp", YES, HTLoadHTTP, NULL);
    HTProtocol_add("file", "local", YES, HTLoadFile, NULL);
    HTProtocol_add("telnet", "", YES, HTLoadTelnet, NULL);
    HTProtocol_add("tn3270", "", YES, HTLoadTelnet, NULL);
    HTProtocol_add("rlogin", "", YES, HTLoadTelnet, NULL);
}

/*	BINDINGS BETWEEN ICONS AND MEDIA TYPES
**	--------------------------------------
**	Not done automaticly - may be done by application!
**	For directory listings etc. you can bind a set of icons to a set of
**	media types and special icons for directories and other objects that
**	do not have a media type.
*/
PUBLIC void HTIconInit (const char * url_prefix)
{
    const char * prefix = url_prefix ? url_prefix : ICON_LOCATION;

    HTIcon_addBlank("blank.xbm", 	prefix,	NULL);
    HTIcon_addDir("directory.xbm", 	prefix, "DIR");
    HTIcon_addParent("back.xbm", 	prefix,	"UP");
    HTIcon_addUnknown("unknown.xbm", 	prefix, NULL);

    HTIcon_add("unknown.xbm",	prefix,	NULL,	"*/*");
    HTIcon_add("binary.xbm", 	prefix,	"BIN",	"binary");
    HTIcon_add("unknown.xbm",	prefix,	NULL,	"www/unknown");
    HTIcon_add("text.xbm", 	prefix,	"TXT",	"text/*");
    HTIcon_add("image.xbm",	prefix,	"IMG",	"image/*");
    HTIcon_add("movie.xbm", 	prefix,	"MOV",	"video/*");
    HTIcon_add("sound.xbm", 	prefix,	"AU",	"audio/*");
    HTIcon_add("tar.xbm", 	prefix,	"TAR",	"multipart/x-tar");
    HTIcon_add("tar.xbm", 	prefix,	"TAR",	"multipart/x-gtar");
    HTIcon_add("compressed.xbm",prefix,	"CMP",	"x-compress");
    HTIcon_add("compressed.xbm",prefix,	"GZP",	"x-gzip");
    HTIcon_add("index.xbm", 	prefix,	"IDX",	"application/x-gopher-index");
    HTIcon_add("index2.xbm", 	prefix,	"CSO",	"application/x-gopher-cso");
    HTIcon_add("telnet.xbm", 	prefix,	"TEL",	"application/x-gopher-telnet");
    HTIcon_add("unknown.xbm",	prefix,	"DUP",	"application/x-gopher-duplicate");
    HTIcon_add("unknown.xbm",	prefix,	"TN",	"application/x-gopher-tn3270");

    /* Add global  mapping to where to find the internal icons */
    HTRule_addGlobal(HT_Pass, ICON_LOCATION, W3C_ICONS);
}

/*	REGISTER ALL HTTP/1.1 MIME HEADERS
**	--------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTMIMEInit (void)
{
    struct {
        char * string;
	HTParserCallback * pHandler;
    } fixedHandlers[] = {
	{"accept", &HTMIME_accept}, 
	{"accept-charset", &HTMIME_acceptCharset}, 
	{"accept-encoding", &HTMIME_acceptEncoding}, 
	{"accept-language", &HTMIME_acceptLanguage}, 
	{"accept-ranges", &HTMIME_acceptRanges}, 
	{"age", &HTMIME_age}, 
	{"allow", &HTMIME_allow},
	{"authorization", NULL},
	{"cache-control", NULL},
	{"connection", &HTMIME_connection}, 
	{"content-base", &HTMIME_contentBase}, 
	{"content-encoding", &HTMIME_contentEncoding}, 
	{"content-language", &HTMIME_contentLanguage}, 
	{"content-length", &HTMIME_contentLength}, 
	{"content-location", &HTMIME_contentLocation}, 
	{"content-md5", &HTMIME_contentMD5},
	{"content-range", &HTMIME_contentRange},
	{"content-transfer-encoding", &HTMIME_contentTransferEncoding}, 
	{"content-type", &HTMIME_contentType},
	{"date", &HTMIME_date},
  	{"derived-from", &HTMIME_derivedFrom}, 
	{"digest-MessageDigest", &HTMIME_messageDigest}, 
	{"etag", &HTMIME_etag},
        {"expires", &HTMIME_expires},
	{"from", &HTMIME_from},
	{"host", &HTMIME_host},
	{"if-modified-since", &HTMIME_ifModifiedSince}, 
	{"if-match", &HTMIME_ifMatch}, 
	{"if-none-match", &HTMIME_ifNoneMatch}, 
	{"if-range", &HTMIME_ifRange}, 
	{"if-unmodified-since", &HTMIME_ifUnmodifiedSince}, 
	{"keep-alive", NULL}, 
	{"last-modified", &HTMIME_lastModified}, 
	{"link", &HTMIME_link},
	{"location", &HTMIME_location},
	{"max-forwards", &HTMIME_maxForwards}, 
	{"mime-version", NULL}, 
	{"pragma", &HTMIME_pragma},
	{"proxy-authenticate", &HTMIME_authenticate},
	{"proxy-authorization", &HTMIME_proxyAuthorization},
	{"public", &HTMIME_public},
	{"range", &HTMIME_range},
	{"referer", &HTMIME_referer},
	{"retry-after", &HTMIME_retryAfter}, 
	{"server", &HTMIME_server}, 
	{"title", &HTMIME_title}, 
	{"transfer-encoding", &HTMIME_contentTransferEncoding}, 
	{"upgrade", &HTMIME_upgrade},
	{"uri", &HTMIME_uri},
	{"user-agent", &HTMIME_userAgent},
	{"vary", &HTMIME_vary},
	{"version", &HTMIME_version},
	{"via", &HTMIME_via},
	{"warning", &HTMIME_warning},
	{"www-authenticate", &HTMIME_authenticate}, 
    };
    int i;

    for (i = 0; i < sizeof(fixedHandlers)/sizeof(fixedHandlers[0]); i++)
        HTHeader_addParser(fixedHandlers[i].string, NO, 
			   fixedHandlers[i].pHandler);
}

