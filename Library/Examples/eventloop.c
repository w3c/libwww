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
**	libwww app and also how to use various contexts
*/

#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWLib.h"
#include "WWWApp.h"
#include "WWWInit.h"

#ifndef W3C_VERSION
#define W3C_VERSION		"Unspecified"
#endif

#define APP_NAME		"W3CSample"
#define APP_VERSION		W3C_VERSION

#define ADDRESS			"http://www.w3.org/Library/test.html"

#if defined(__svr4__)
#define CATCH_SIG
#endif

typedef struct _App {
    HTRequest *		console_request;
    HTEvent *		console_event;
    HTList *		active;			  /* List of active contexts */
    HTAnchor *		anchor;			  /* The only anchor we load */
} App;

PRIVATE HTEventCallback console_parser;
PRIVATE HTNetAfter request_terminater;

/* ------------------------------------------------------------------------- */

PRIVATE HTRequest * Request_new (App * app)
{
    HTRequest * request = HTRequest_new();
    HTRequest_setOutputFormat(request, WWW_SOURCE);    
    HTRequest_setOutputStream(request, HTFWriter_new(request, stdout, YES));

    if (!app->active) app->active = HTList_new();
    HTList_addObject(app->active, request);

    return request;
}

PRIVATE BOOL Request_delete (App * app, HTRequest * request)
{
    if (app && app->active && request) {
	HTList_removeObject(app->active, request);
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

    /* We repeatedly ask for this single address */
    me->anchor = HTAnchor_findAddress(ADDRESS);

    /* Register stdin as our console */
    if (isatty(STDIN_FILENO)) HTEventList_register(STDIN_FILENO, HTEvent_READ, me->console_event);

    return me;
}

PRIVATE BOOL App_delete (App * me)
{
    if (me) {

	/* Kill any remaining active requests */
	HTNet_killAll();

	/* Now do the rest of the cleanup */
	HTRequest_delete(me->console_request);
	HT_FREE(me);

	/* Terminate libwww */
	HTProfile_delete();

	exit(0);
    }
    return NO;
}

/* ------------------------------------------------------------------------- */

PRIVATE int console_parser (SOCKET s, void * param, HTEventType type)
{
    App * app = (App *) param;
    HTRequest * new_request = NULL;
    char buf[128];

    if (!fgets(buf, sizeof(buf), stdin)) return HT_ERROR;

    switch (toupper(buf[0])) {

    case 'Q':						   /* Quit program ? */
	App_delete(app);
	break;
	
    default:
	new_request = Request_new(app);
	HTLoadAnchor(app->anchor, new_request);
	break;
    }
    
    return HT_OK;
}

PRIVATE int request_terminater (HTRequest * request, HTResponse * response,
				void * param, int status) 
{
    App * app = (App *) param;
    Request_delete(app, request);
    return HT_OK;
}

/* ------------------------------------------------------------------------- */

int main (int argc, char ** argv)
{
    App * app = NULL;
    HTRequest * request = NULL;

    /* Initiate libwww */
    HTProfile_newNoCacheClient(APP_NAME, APP_VERSION);

#if 0
    HTSetTraceMessageMask("ot");
#endif

    /* Add our own request terminate handler */
    HTNet_addAfter(request_terminater, NULL, app, HT_ALL, HT_FILTER_LAST);

    /* Create a new app obect */
    app = App_new();

    /* Start the first request */
    request = Request_new(app);
    HTLoadAnchor(app->anchor, request);

    /* Go into the event loop... */
    HTEventList_newLoop();

    App_delete(app);
    return 0;
}

