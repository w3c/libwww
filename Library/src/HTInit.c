/*								       HTInit.c
**	CONFIGURATION-SPECIFIC INITIALIALIZATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	General initialization functions.
**
**      @@@A lot of these should be moved to the various modules instead
**      of being here
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"

#ifdef HT_MUX
#include "WWWMux.h"
#endif

#include "HTInit.h"				         /* Implemented here */

#ifndef W3C_ICONS
#define W3C_ICONS	"w3c-icons"
#endif

#define ICON_LOCATION	"/icons/"

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
    **  Set our own local file save stream for the MIME parser so that 
    **  we know how to dump to local disk in case we get content type
    **  application/octect stream, or an encoding that we don't know.
    */
    HTMIME_setSaveStream (HTSaveLocally);

    /*
    ** These are converters that converts to something other than www/present,
    ** that is not directly outputting someting to the user on the screen
    */
    HTConversion_add(c,"message/rfc822",	"*/*",		HTMIMEConvert,	1.0, 0.0, 0.0);
    HTConversion_add(c,"message/x-rfc822-foot",	"*/*",		HTMIMEFooter,	1.0, 0.0, 0.0);
    HTConversion_add(c,"message/x-rfc822-head",	"*/*",		HTMIMEHeader,	1.0, 0.0, 0.0);
    HTConversion_add(c,"message/x-rfc822-cont",	"*/*",		HTMIMEContinue,	1.0, 0.0, 0.0);
    HTConversion_add(c,"message/x-rfc822-upgrade","*/*",	HTMIMEUpgrade,	1.0, 0.0, 0.0);
    HTConversion_add(c,"message/x-rfc822-partial","*/*",	HTMIMEPartial,	1.0, 0.0, 0.0);
#ifndef NO_CACHE
    HTConversion_add(c,"www/x-rfc822-headers","*/*",        HTCacheCopyHeaders,  1.0, 0.0, 0.0);
#endif
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
    ** If we have the XML expat parser linked in
    */
#ifdef HT_EXPAT
    HTConversion_add(c, "text/xml",		"*/*",		HTXML_new,	1.0, 0.0, 0.0);
    HTConversion_add(c, "application/xml",	"*/*",		HTXML_new,	1.0, 0.0, 0.0);
#endif

    /*
    ** We also register a special content type guess stream that can figure out
    ** the content type by reading the first bytes of the stream
    */
    HTConversion_add(c,"www/unknown",		"*/*",		HTGuess_new,	1.0, 0.0, 0.0);

    /*
    ** Register a persistent cache stream which can save an object to local
    ** file
    */
    HTConversion_add(c,"www/cache",		"*/*",		HTCacheWriter,	1.0, 0.0, 0.0);
    HTConversion_add(c,"www/cache-append",	"*/*",		HTCacheAppend,	1.0, 0.0, 0.0);

    /*
    ** Handling Rule files is handled just like any other stream
    ** This converter reads a rule file and generates the rules
    */
    HTConversion_add(c,"application/x-www-rules","*/*",		HTRules,	1.0, 0.0, 0.0);

    /*
    ** This dumps all other formats to local disk without any further
    ** action taken
    */
    HTConversion_add(c,"*/*",			"www/present",	HTSaveConverter, 0.3, 0.0, 0.0);
}

/*	BINDINGS BETWEEN MEDIA TYPES AND EXTERNAL VIEWERS/PRESENTERS
**	------------------------------------------------------------
**	Not done automaticly - may be done by application!
**	The data objects are stored in temporary files before the external
**	program is called
*/
PUBLIC void HTPresenterInit (HTList * c)
{
    /*
    **  First we set the special "presenter" stream that writes to a
    **  temporary file before executing the external presenter
    */
    HTPresentation_setConverter(HTSaveAndExecute);

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

/*	BINDINGS BETWEEN A TRANSFER ENCODING AND CODERS / DECODERS
**	----------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTTransferEncoderInit (HTList * c)
{
#ifdef HT_ZLIB
    HTCoding_add(c, "deflate", NULL, HTZLib_inflate, 1.0);
#endif
    HTCoding_add(c, "chunked", HTChunkedEncoder, HTChunkedDecoder, 1.0);
}

/*	BINDINGS BETWEEN A CONTENT ENCODING AND CODERS / DECODERS
**	---------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTContentEncoderInit (HTList * c)
{
#ifdef HT_ZLIB
    HTCoding_add(c, "deflate", NULL, HTZLib_inflate, 1.0);
#endif /* HT_ZLIB */
}

/*	REGISTER BEFORE FILTERS
**	-----------------------
**	The BEFORE filters handle proxies, caches, rule files etc.
**	The filters are called in the order by which the are registered
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTBeforeInit (void)
{
    HTNet_addBefore(HTCredentialsFilter,	"http://*",	NULL, HT_FILTER_LATE);
    HTNet_addBefore(HTPEP_beforeFilter, 	"http://*",	NULL, HT_FILTER_LATE);
    HTNet_addBefore(HTRuleFilter,		NULL,		NULL, HT_FILTER_LATE);
    HTNet_addBefore(HTProxyFilter,		NULL,		NULL, HT_FILTER_LATE);
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
    HTNet_addAfter(HTAuthFilter, 	"http://*",	NULL, HT_NO_ACCESS, 	HT_FILTER_MIDDLE);
    HTNet_addAfter(HTAuthFilter, 	"http://*",	NULL, HT_REAUTH, 	HT_FILTER_MIDDLE);
    HTNet_addAfter(HTPEP_afterFilter, 	"http://*",	NULL, HT_ALL, 		HT_FILTER_MIDDLE);
    HTNet_addAfter(HTRedirectFilter, 	"http://*",	NULL, HT_PERM_REDIRECT, HT_FILTER_MIDDLE);
    HTNet_addAfter(HTRedirectFilter, 	"http://*",	NULL, HT_FOUND, 	HT_FILTER_MIDDLE);
    HTNet_addAfter(HTRedirectFilter,	"http://*",	NULL, HT_SEE_OTHER,	HT_FILTER_MIDDLE);
    HTNet_addAfter(HTRedirectFilter, 	"http://*",	NULL, HT_TEMP_REDIRECT, HT_FILTER_MIDDLE);
    HTNet_addAfter(HTAuthInfoFilter, 	"http://*",	NULL, HT_ALL, 		HT_FILTER_MIDDLE);
    HTNet_addAfter(HTUseProxyFilter, 	"http://*",	NULL, HT_USE_PROXY, 	HT_FILTER_MIDDLE);
    HTNet_addAfter(HTInfoFilter, 	NULL,		NULL, HT_ALL,		HT_FILTER_LATE);
}

/*	REGISTER DEFAULT AUTHENTICATION SCHEMES
**	---------------------------------------
**	This function registers the BASIC access authentication
*/
PUBLIC void HTAAInit (void)
{
    HTAA_newModule ("basic", HTBasic_generate, HTBasic_parse, NULL,
		     HTBasic_delete);
#ifdef HT_MD5
    HTAA_newModule ("digest", HTDigest_generate, HTDigest_parse, 
		     HTDigest_updateInfo,  HTDigest_delete);
#endif /* HT_MD5 */
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
    HTTransport_add("tcp", HT_TP_SINGLE, HTReader_new, HTWriter_new);
    HTTransport_add("buffered_tcp", HT_TP_SINGLE, HTReader_new, HTBufferWriter_new);
#ifdef HT_MUX
    HTTransport_add("mux", HT_TP_INTERLEAVE, HTReader_new, HTBufferWriter_new);
#endif /* HT_MUX */
#ifndef NO_UNIX_IO
    HTTransport_add("local", HT_TP_SINGLE, HTReader_new, HTWriter_new);
#else
    HTTransport_add("local", HT_TP_SINGLE, HTANSIReader_new, HTANSIWriter_new);
#endif
}

/*	REGISTER ALL KNOWN PROTOCOLS IN THE LIBRARY
**	-------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTProtocolInit (void)
{
#ifndef DECNET
    HTProtocol_add("ftp", 	"tcp",	FTP_PORT,	NO, 	HTLoadFTP,	NULL);
    HTProtocol_add("nntp",	"tcp",	NEWS_PORT,	NO, 	HTLoadNews,	NULL);
    HTProtocol_add("news",	"tcp",	NEWS_PORT,	NO, 	HTLoadNews,	NULL);
    HTProtocol_add("gopher",	"tcp",	GOPHER_PORT,	NO, 	HTLoadGopher,	NULL);
#ifdef HT_DIRECT_WAIS
    HTProtocol_add("wais",	"tcp",	WAIS_PORT,	YES, 	HTLoadWAIS,	NULL);
#endif
#endif /* DECNET */
#ifdef HT_MUX
    HTProtocol_add("http", 	"mux",	HTTP_PORT,	NO,	HTLoadHTTP,	NULL);
#else
    HTProtocol_add("http", 	"buffered_tcp", HTTP_PORT,	NO,	HTLoadHTTP,	NULL);
#endif /* !HT_MUX */
#ifndef NO_UNIX_IO
    HTProtocol_add("file", 	"local", 	0, 	NO, 	HTLoadFile, 	NULL);
    HTProtocol_add("cache", 	"local", 	0, 	NO, 	HTLoadCache, 	NULL);
#else
    HTProtocol_add("file", 	"local", 	0, 	YES, 	HTLoadFile, 	NULL);
    HTProtocol_add("cache", 	"local", 	0, 	YES, 	HTLoadCache, 	NULL);
#endif
    HTProtocol_add("telnet", 	"", 		0,	YES, 	HTLoadTelnet, 	NULL);
    HTProtocol_add("tn3270", 	"", 		0,	YES, 	HTLoadTelnet, 	NULL);
    HTProtocol_add("rlogin", 	"", 		0,	YES, 	HTLoadTelnet, 	NULL);
}

/*	REGISTER ALL KNOWN PROTOCOLS IN THE LIBRARY PREEMPTIVELY
**	--------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTProtocolPreemptiveInit (void)
{
#ifndef DECNET
    HTProtocol_add("ftp", "tcp", FTP_PORT, YES, HTLoadFTP, NULL);
    HTProtocol_add("nntp", "tcp", NEWS_PORT, YES, HTLoadNews, NULL);
    HTProtocol_add("news", "tcp", NEWS_PORT, YES, HTLoadNews, NULL);
    HTProtocol_add("gopher", "tcp", GOPHER_PORT, YES, HTLoadGopher, NULL);
#ifdef HT_DIRECT_WAIS
    HTProtocol_add("wais", "", WAIS_PORT, YES, HTLoadWAIS, NULL);
#endif
#endif /* DECNET */

    HTProtocol_add("http", "buffered_tcp", HTTP_PORT, YES, HTLoadHTTP, NULL);
    HTProtocol_add("file", "local", 0, YES, HTLoadFile, NULL);
    HTProtocol_add("telnet", "", 0, YES, HTLoadTelnet, NULL);
    HTProtocol_add("tn3270", "", 0, YES, HTLoadTelnet, NULL);
    HTProtocol_add("rlogin", "", 0, YES, HTLoadTelnet, NULL);
    HTProtocol_add("cache","local",0,YES,HTLoadCache,  NULL);
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

    HTIcon_deleteAll(); /* Start fresh */
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
    {
	char * curdir = HTGetCurrentDirectoryURL();
	char * virtual = HTParse(ICON_LOCATION, curdir,
				 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	char * physical = NULL;
	StrAllocCat(virtual, "*");

	{
	    char * str = NULL;
	    if ((str = (char *) HT_MALLOC(strlen(W3C_ICONS) + 4)) == NULL)
		HT_OUTOFMEM("HTIconInit");
	    strcpy(str, W3C_ICONS);
	    if (*(str + strlen(str) - 1) != '/') strcat(str, "/");
	    strcat(str, "*");
	    physical = HTParse(str, curdir,
			       PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	    HT_FREE(str);
	}
	HTRule_addGlobal(HT_Pass, virtual, physical);
	HT_FREE(virtual);
	HT_FREE(physical);
	HT_FREE(curdir);
    }
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
	{"authorization", NULL},
	{"cache-control", &HTMIME_cacheControl},
	{"connection", &HTMIME_connection}, 
	{"content-encoding", &HTMIME_contentEncoding}, 
	{"content-length", &HTMIME_contentLength}, 
	{"content-range", &HTMIME_contentRange},
	{"content-transfer-encoding", &HTMIME_contentTransferEncoding}, 
	{"content-type", &HTMIME_contentType},
	{"digest-MessageDigest", &HTMIME_messageDigest}, 
	{"keep-alive", &HTMIME_keepAlive}, 
	{"link", &HTMIME_link},
	{"location", &HTMIME_location},
	{"max-forwards", &HTMIME_maxForwards}, 
	{"mime-version", NULL}, 
	{"pragma", &HTMIME_pragma},
        {"protocol", &HTMIME_protocol},
        {"protocol-info", &HTMIME_protocolInfo},
        {"protocol-request", &HTMIME_protocolRequest},
	{"proxy-authenticate", &HTMIME_authenticate},
	{"proxy-authorization", &HTMIME_proxyAuthorization},
	{"public", &HTMIME_public},
	{"range", &HTMIME_range},
	{"referer", &HTMIME_referer},
	{"retry-after", &HTMIME_retryAfter}, 
	{"server", &HTMIME_server}, 
	{"trailer", &HTMIME_trailer},
	{"transfer-encoding", &HTMIME_transferEncoding}, 
	{"upgrade", &HTMIME_upgrade},
	{"user-agent", &HTMIME_userAgent},
	{"vary", &HTMIME_vary},
	{"via", &HTMIME_via},
	{"warning", &HTMIME_warning},
	{"www-authenticate", &HTMIME_authenticate}, 
        {"authentication-info", &HTMIME_authenticationInfo},
        {"proxy-authentication-info", &HTMIME_proxyAuthenticationInfo}
    };
    int i;

    for (i = 0; i < sizeof(fixedHandlers)/sizeof(fixedHandlers[0]); i++)
        HTHeader_addParser(fixedHandlers[i].string, NO, 
			   fixedHandlers[i].pHandler);
}

