/*
**	@(#) $Id$
**	
**	Other libwww samples can be found at "http://www.w3.org/Library/Examples"
**	
**	Copyright (c© 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**      A very simple event loop demo showing how to write an event driven
**	libwww app and also how to use various contexts and the HTML parser
**
**	Compile libwww with these flags:
**		CFLAGS='-DLIBWWW_SMALL -DNODEBUG -O2'
**	if you want to use the persistent cache and
**		CFLAGS='-DLIBWWW_SMALL -DNODEBUG -O2 -DNO_CACHE'
**	if not
*/

#include "WWWLib.h"
#include "WWWMIME.h"
#include "WWWHTTP.h"

#include "HTTCP.h"
#include "HTSocket.h"
#include "HTReader.h"
#include "HTWriter.h"
#include "HTBufWrt.h"

#include "HTML.h"
#include "HText.h"

#include "HTEvtLst.h"

#ifndef W3C_VERSION
#define W3C_VERSION		"Unspecified"
#endif

#define APP_NAME		"w3c-tiny"
#define APP_VERSION		W3C_VERSION

#define DEFAULT_HOME		"http://www.w3.org/Library/"

struct _HText {
    HTParentAnchor *	parent;
    HTList *		kids;	   /* List of anchors found in this document */
};

typedef struct _App {
    HTRequest *		console_request;
    HTEvent *		console_event;
    HTList *		active;			  /* List of active contexts */
    HText *		current;		         /* Current document */
} App;

PRIVATE HTList * documents = NULL;

PRIVATE HTEventCallback console_parser;
PRIVATE HTNetAfter request_terminater;

/* Our HTML parser callbacks */
PRIVATE HText_new	text_new;
PRIVATE HText_delete	text_delete;
PRIVATE HText_build 	text_build;
PRIVATE HText_foundLink	text_link;
PRIVATE HText_addText	text_add;

/* ------------------------------------------------------------------------- */
/*                                LIBWWW SETUP                               */
/* ------------------------------------------------------------------------- */

PRIVATE int printer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stdout, fmt, pArgs));
}

PRIVATE int tracer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}

PRIVATE void mime_setup (void)
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

/*
**  This is normally done by the libwww profiles but because we are more
**  careful only taking the things we need, we do it by hand
*/
PRIVATE void libwww_setup (void)
{
    /* Set up TCP as transport */
    HTTransport_add("buffered_tcp", HT_TP_SINGLE, HTReader_new, HTBufferWriter_new);

    /* Set up HTTP as protocol */
    HTProtocol_add("http", "buffered_tcp", 80, NO, HTLoadHTTP, NULL);

    /* Setup up transfer coders */
    HTFormat_addTransferCoding("chunked", HTChunkedEncoder, HTChunkedDecoder, 1.0);

    /* Setup MIME stream converters */
    HTFormat_addConversion("message/rfc822", "*/*", HTMIMEConvert, 1.0, 0.0, 0.0);
    HTFormat_addConversion("message/x-rfc822-foot", "*/*", HTMIMEFooter, 1.0, 0.0, 0.0);
    HTFormat_addConversion("message/x-rfc822-head", "*/*", HTMIMEHeader, 1.0, 0.0, 0.0);
    HTFormat_addConversion("message/x-rfc822-cont", "*/*", HTMIMEContinue, 1.0, 0.0, 0.0);
    HTFormat_addConversion("message/x-rfc822-partial","*/*", HTMIMEPartial, 1.0, 0.0, 0.0);
    HTFormat_addConversion("multipart/*", "*/*", HTBoundary, 1.0, 0.0, 0.0);

    /* Setup HTTP protocol stream */
    HTFormat_addConversion("text/x-http", "*/*", HTTPStatus_new, 1.0, 0.0, 0.0);

    /* Setup the HTML parser */
    HTFormat_addConversion("text/html", "www/present", HTMLPresent, 1.0, 0.0, 0.0);

    /* Setup blackhole stream */
    HTFormat_addConversion("*/*", "www/debug", HTBlackHoleConverter, 1.0, 0.0, 0.0);
    HTFormat_addConversion("*/*", "www/present", HTBlackHoleConverter, 0.3, 0.0, 0.0);

    /* Set max number of sockets we want open simultanously */ 
    HTNet_setMaxSocket(32);

    /* Register our HTML parser callbacks */
    HText_registerCDCallback (text_new, text_delete);
    HText_registerBuildCallback (text_build);
    HText_registerTextCallback(text_add);
    HText_registerLinkCallback (text_link);

    /* Register the default set of MIME header parsers */
    mime_setup();

    /* Set up default event loop */
    HTEventInit();

}

PRIVATE void libwww_terminate (void)
{
    /* Clean up all the global preferences */
    HTFormat_deleteAll();

    /* Terminate libwww */
    HTLibTerminate();
}

/* ------------------------------------------------------------------------- */
/*                            HTML Parser callbacks                          */
/* ------------------------------------------------------------------------- */

PRIVATE HText * text_new (HTRequest *		request,
			  HTParentAnchor *	anchor,
			  HTStream *		output_stream)
{
    App * app = (App *) HTRequest_context(request);
    HText * me;
    if ((me = (HText *) HT_CALLOC(1, sizeof(HText))) == NULL)
	HT_OUTOFMEM("HText_new");
    me->parent = anchor;

    /* Add to our list of HText objects */
    if (!documents) documents = HTList_new();
    HTList_addObject(documents, me);

    /* Associate this object with the anchor object */
    HTAnchor_setDocument(anchor, me);

    /* Now this is the current HText */
    if (app && app->current) text_delete(app->current);
    app->current = me;
    
    return me;
}

PRIVATE BOOL text_delete (HText * me)
{
    if (me) {

	/* Disassociate from anchor */
	HTAnchor_setDocument(me->parent, NULL);

	/* Delete the object */
	if (me->kids) HTList_delete(me->kids);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PRIVATE void text_build (HText * me, HTextStatus status)
{
    if (status==HTEXT_END || status==HTEXT_ABORT) {
	int kids = HTList_count(me->kids);
	HTPrint("\n");
	if (kids > 0)
	    HTPrint("\nHit a number between [0..%d]: ", kids-1);
	else
	    HTPrint("\nNo links found\n");
    }
}

PRIVATE void text_add (HText * me, const char * buf, int len)
{
    if (buf) fwrite(buf, 1, len, stdout);
}

PRIVATE void text_link (HText * 	me,
			int		element_number,
			int		attribute_number,
			HTChildAnchor *	anchor,	
			const BOOL * 	present,
			const char **	value)
{
    if (me && anchor) {
	if (element_number == HTML_A) {
	    if (!me->kids) me->kids = HTList_new();
	    HTList_appendObject(me->kids, anchor);
	    HTPrint("[%d]", HTList_count(me->kids)-1);
	} else if (element_number == HTML_IMG) {
	    if (present[HTML_IMG_ALT] && value[HTML_IMG_ALT])
		HTPrint("<%s>", value[HTML_IMG_ALT]);
	    else
		HTPrint("<IMAGE>");
	}
    }
}

PRIVATE HTAnchor * text_findAnchor (HText * me, int index)
{
    if (me && me->kids) {
	HTAnchor * anchor = HTList_objectAt(me->kids, index);
	HTLink * link = HTAnchor_mainLink(anchor);
	return HTLink_destination(link);
    }
    return NULL;
}

/* ------------------------------------------------------------------------- */
/*                             THE TINY APPLICATION                          */
/* ------------------------------------------------------------------------- */

PRIVATE BOOL get_document (HTRequest * request, HTAnchor * anchor)
{
    char * address = HTAnchor_address(anchor);
    HTPrint("fetching %s\n", address);
    HT_FREE(address);
    HTRequest_setAnchor(request, anchor);
    return HTLoad(request, NO);
}

PRIVATE HTRequest * Request_new (App * app)
{
    HTRequest * request = HTRequest_new();
    if (!app->active) app->active = HTList_new();
    HTList_addObject(app->active, request);
    HTRequest_setContext(request, app);
    return request;
}

PRIVATE BOOL Request_delete (App * app, HTRequest * request)
{
    if (app && app->active && request) {

	/* Remove from our list of requests */
	HTList_removeObject(app->active, request);

	/* Delete the object */
	HTRequest_delete(request);
	return YES;
    }
    return NO;
}

PRIVATE App * App_new (void)
{
    App * me = NULL;
    if ((me = (App *) HT_CALLOC(1, sizeof(App))) == NULL)	
	HT_OUTOFMEM("App_new");
    me->console_request = HTRequest_new();
    me->console_event = HTEvent_new(console_parser, me, HT_PRIORITY_MAX, -1);
    me->active = HTList_new();

    /* Register stdin as our console */
#ifdef STDIN_FILENO
    if (isatty(STDIN_FILENO)) HTEventList_register(STDIN_FILENO, HTEvent_READ, me->console_event);
#endif

    return me;
}

PRIVATE BOOL App_delete (App * me)
{
    if (me) {

	if (me->active) HTList_delete(me->active);

	/* Kill any remaining active requests */
	HTNet_killAll();

	/* Now do the rest of the cleanup */
	HTRequest_delete(me->console_request);
	HT_FREE(me);

	/* Terminate libwww */
	libwww_terminate();

	exit(0);
    }
    return NO;
}

/* ------------------------------------------------------------------------- */

PRIVATE int console_parser (SOCKET s, void * param, HTEventType type)
{
    App * app = (App *) param;
    HTRequest * new_request = NULL;
    HTAnchor * anchor = NULL;
    int index = 0;
    char buf[32];

    if (!fgets(buf, sizeof(buf), stdin)) return HT_ERROR;
    buf[sizeof(buf)-1] = '\0';

    switch (TOUPPER(buf[0])) {

    case 'Q':						   /* Quit program ? */
	App_delete(app);
	break;
	
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
	index = atoi(buf);
	new_request = Request_new(app);
	if (app->current && (anchor = text_findAnchor(app->current, index)))
	    get_document(new_request, anchor);
	else
	    HTPrint("Index out of range - try again\n");
	break;

    default:
	HTPrint("Unknown command - type a number of 'q' for quit\n");
    }
    
    return HT_OK;
}

PRIVATE int request_terminater (HTRequest * request, HTResponse * response,
				void * param, int status) 
{
    App * app = (App *) param;
    if (status!=HT_LOADED) HTPrint("Load couldn't be completed successfully\n");
    Request_delete(app, request);
    return HT_OK;
}

/* ------------------------------------------------------------------------- */

int main (int argc, char ** argv)
{
    App * app = NULL;
    HTRequest * request = NULL;
    HTAnchor * anchor = NULL;

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Handle command line args */
    if (argc >= 2) {
	char * uri = HTParse(argv[1], NULL, PARSE_ALL);
	if ((anchor = HTAnchor_findAddress(uri)) == NULL)
	    anchor = HTAnchor_findAddress(DEFAULT_HOME);
	HT_FREE(uri);
    } else {
	HTPrint("Type the URI of the first page you want to load\n");
	HTPrint("\t%s <address>\n", argv[0]);
	HTPrint("For example, %s http://www.w3.org\n", argv[0]);
	return -1;
    }

#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Initiate libwww */
    libwww_setup();

    /* Create a new app obect */
    app = App_new();

    /* Add our own request terminate handler */
    HTNet_addAfter(request_terminater, NULL, app, HT_ALL, HT_FILTER_LAST);

    /* Start the first request */
    request = Request_new(app);
    get_document(request, anchor);

    /* Go into the event loop... */
    HTEventList_newLoop();

    App_delete(app);
    return 0;
}

