/*         		    					     HTRobot.c
**	W3C MINI ROBOT
**
**	(c) COPRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This program illustrates how to travers links using the Anchor object
**
**  Authors:
**	HFN		Henrik Frystyk Nielsen, (frystyk@w3.org)
**
**  History:
**	Dec 04 95	First version
*/

#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWApp.h"				        /* Application stuff */
#include "WWWTrans.h"
#include "WWWInit.h"

#include "HText.h"

#include "HTMemLog.h"
#include "HTRobot.h"			     		 /* Implemented here */

#ifndef W3C_VERSION
#define W3C_VERSION 		"Unspecified"
#endif

#define APP_NAME		"W3CRobot"
#define APP_VERSION		W3C_VERSION

#define DEFAULT_OUTPUT_FILE	"robot.out"
#define DEFAULT_RULE_FILE	"robot.conf"
#define DEFAULT_LOG_FILE       	"robot.log"
#define DEFAULT_DEPTH		0

/* #define SHOW_MSG		(WWWTRACE || HTAlert_interactive()) */
#define SHOW_MSG		(!(mr->flags & MR_QUIET))

#define DEFAULT_TIMEOUT		10000		       /* timeout in millis */

#if defined(__svr4__)
#define CATCH_SIG
#endif

typedef enum _MRFlags {
    MR_IMG		= 0x1,
    MR_LINK		= 0x2,
    MR_PREEMPTIVE	= 0x4,
    MR_TIME		= 0x8,
    MR_SAVE	  	= 0x10,
    MR_QUIET	  	= 0x20,
    MR_VALIDATE		= 0x40,
    MR_END_VALIDATE	= 0x80
} MRFlags;

typedef struct _Robot {
    int			depth;			     /* How deep is our tree */
    int			cnt;				/* Count of requests */
    HTList *		hyperdoc;	     /* List of our HyperDoc Objects */
    HTList *		htext;			/* List of our HText Objects */
    HTList *		fingers;
    int 		timer;
    char *		cwd;				  /* Current dir URL */
    char *		rules;
    char *		logfile;
    char *		outputfile;
    FILE *	        output;
    MRFlags		flags;
} Robot;

typedef struct _Finger {
    Robot * robot;
    HTRequest * request;
    HTParentAnchor * dest;
} Finger;

typedef enum _LoadState {
    L_INVALID	= -2,
    L_LOADING	= -1,
    L_SUCCESS 	= 0,
    L_ERROR
} LoadState;

/*
**  The HyperDoc object is bound to the anchor and contains information about
**  where we are in the search for recursive searches
*/
typedef struct _HyperDoc {
    HTParentAnchor * 	anchor;
    LoadState		state;
    int			depth;
} HyperDoc;

/*
** This is the HText object that is created every time we start parsing a 
** HTML object
*/
struct _HText {
    HTRequest *		request;
};

PUBLIC HText * HTMainText = NULL;
PUBLIC HTParentAnchor * HTMainAnchor = NULL;
PUBLIC HTStyleSheet * styleSheet = NULL;

/* ------------------------------------------------------------------------- */

/*	Standard (non-error) Output
**	---------------------------
*/
PUBLIC int OutputData(const char  * fmt, ...)
{
    int ret;
    va_list pArgs;
    va_start(pArgs, fmt);
    ret = vfprintf(stdout, fmt, pArgs);
    va_end(pArgs);
    return ret;
}

/* ------------------------------------------------------------------------- */

/*	Create a "HyperDoc" object
**	--------------------------
**	A HyperDoc object contains information about whether we have already
**	started checking the anchor and the depth in our search
*/
PRIVATE HyperDoc * HyperDoc_new (Robot * mr,HTParentAnchor * anchor, int depth)
{
    HyperDoc * hd;
    if ((hd = (HyperDoc *) HT_CALLOC(1, sizeof(HyperDoc))) == NULL)
	HT_OUTOFMEM("HyperDoc_new");
    hd->state = L_INVALID;
    hd->depth = depth;
 
    /* Bind the HyperDoc object together with the Anchor Object */
    hd->anchor = anchor;
    HTAnchor_setDocument(anchor, (void *) hd);

    /* Add this HyperDoc object to our list */
    if (!mr->hyperdoc) mr->hyperdoc = HTList_new();
    HTList_addObject(mr->hyperdoc, (void *) hd);
    return hd;
}

/*	Delete a "HyperDoc" object
**	--------------------------
*/
PRIVATE BOOL HyperDoc_delete (HyperDoc * hd)
{
    if (hd) {
	HT_FREE (hd);
	return YES;
    }
    return NO;
}

/*	Create a Command Line Object
**	----------------------------
*/
PRIVATE Robot * Robot_new (void)
{
    Robot * me;
    if ((me = (Robot *) HT_CALLOC(1, sizeof(Robot))) == NULL)
	HT_OUTOFMEM("Robot_new");
    me->hyperdoc = HTList_new();
    me->htext = HTList_new();
    me->timer = DEFAULT_TIMEOUT;
    me->cwd = HTGetCurrentDirectoryURL();
    me->output = OUTPUT;
    me->cnt = 0;
    me->fingers = HTList_new();
    return me;
}

/*	Delete a Command Line Object
**	----------------------------
*/
PRIVATE BOOL Robot_delete (Robot * me)
{
    if (me) {
	HTList_delete(me->fingers);
	if (me->hyperdoc) {
	    HTList * cur = me->hyperdoc;
	    HyperDoc * pres;
	    while ((pres = (HyperDoc *) HTList_nextObject(cur)))
		HyperDoc_delete(pres);
	    HTList_delete(me->hyperdoc);
	}
	if (me->htext) {
	    HTList * cur = me->htext;
	    HText * pres;
	    while ((pres = (HText *) HTList_nextObject(cur)))
		HText_free(pres);
	    HTList_delete(me->htext);
	}
	if (me->logfile) HTLog_close();
	if (me->output && me->output != STDOUT) fclose(me->output);
	if (me->flags & MR_TIME) {
	    time_t local = time(NULL);
	    HTTrace("Robot terminated %s\n",HTDateTimeStr(&local,YES));
	}
	HT_FREE(me->cwd);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/*
**  This function creates a new finger object and initializes it with a new request
*/
PRIVATE Finger * Finger_new (Robot * robot, HTParentAnchor * dest, HTMethod method)
{
    Finger * me;
    HTRequest * request = HTRequest_new();
    if ((me = (Finger *) HT_CALLOC(1, sizeof(Finger))) == NULL)
	HT_OUTOFMEM("Finger_new");
    me->robot = robot;
    me->request = request;
    me->dest = dest;
    HTList_addObject(robot->fingers, (void *)me);

    /* Set the context for this request */
    HTRequest_setContext (request, me);

    /* Check the various flags to customize the request */
    if (robot->flags & MR_PREEMPTIVE)
	HTRequest_setPreemptive(request, YES);
    if (robot->flags & MR_VALIDATE)
	HTRequest_setReloadMode(request, HT_CACHE_VALIDATE);
    if (robot->flags & MR_END_VALIDATE)
	HTRequest_setReloadMode(request, HT_CACHE_END_VALIDATE);

    /* We wanna make sure that we are sending a Host header (default) */
    HTRequest_addRqHd(request, HT_C_HOST);

    /* Set the method for this request */
    HTRequest_setMethod(request, method);
    robot->cnt++;
    return me;
}

PRIVATE int Finger_delete (Finger * me)
{
    HTList_removeObject(me->robot->fingers, (void *)me);
    me->robot->cnt--;

    /*
    **  If we are down at one request then flush the output buffer
    */
    if (me->request) {
	if (me->robot->cnt == 1) HTRequest_forceFlush(me->request);
	HTRequest_delete(me->request);
    }

    /*
    **  Delete the request and free myself
    */
    HT_FREE(me);
    return YES;
}

/*
**  Cleanup and make sure we close all connections including the persistent
**  ones
*/
PRIVATE void Cleanup (Robot * me, int status)
{
    Robot_delete(me);
    HTProfile_delete();
#ifdef MEMLOG
    HTMemLog_close();
#endif

#ifdef VMS
    exit(status ? status : 1);
#else
    exit(status ? status : 0);
#endif
}

#ifdef CATCH_SIG
#include <signal.h>
/*								    SetSignal
**  This function sets up signal handlers. This might not be necessary to
**  call if the application has its own handlers (lossage on SVR4)
*/
PRIVATE void SetSignal (void)
{
    /* On some systems (SYSV) it is necessary to catch the SIGPIPE signal
    ** when attemting to connect to a remote host where you normally should
    ** get `connection refused' back
    */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
	if (PROT_TRACE) HTTrace("HTSignal.... Can't catch SIGPIPE\n");
    } else {
	if (PROT_TRACE) HTTrace("HTSignal.... Ignoring SIGPIPE\n");
    }

#ifdef MEMLOG
    HTMemLog_flush();
#endif

}
#endif /* CATCH_SIG */

PRIVATE void VersionInfo (void)
{
    OutputData("\n\nW3C Reference Software\n\n");
    OutputData("\tW3C Mini Robot (%s) version %s.\n",
	     APP_NAME, APP_VERSION);
    OutputData("\tW3C Reference Library version %s.\n\n",HTLib_version());
    OutputData("Please send feedback to <libwww@w3.org>\n");
}

/*	terminate_handler
**	-----------------
**	This function is registered to handle the result of the request.
**	If no more requests are pending then terminate program
*/
PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    Finger * finger = (Finger *) HTRequest_context(request);
    Robot * mr = finger->robot;
    if (SHOW_MSG) HTTrace("Robot....... done with %s\n", HTAnchor_physical(finger->dest));
    Finger_delete(finger);
    if (mr->cnt <= 0) {
	if (SHOW_MSG) HTTrace("             Everything is finished...\n");
	Cleanup(mr, 0);			/* No way back from here */
    }

    if (SHOW_MSG) HTTrace("             %d outstanding request%s\n", mr->cnt, mr->cnt == 1 ? "" : "s");
    return HT_OK;
}

/* ------------------------------------------------------------------------- */
/*				HTEXT INTERFACE				     */
/* ------------------------------------------------------------------------- */

PUBLIC HText * HText_new2 (HTRequest * request, HTParentAnchor * anchor,
			   HTStream * stream)
{
    HText * me;
    Finger * finger = (Finger *) HTRequest_context(request);
    Robot * mr = finger->robot;
    if ((me = (HText *) HT_CALLOC(1, sizeof(HText))) == NULL)
	HT_OUTOFMEM("HText_new2");

    /* Bind the HText object together with the Request Object */
    me->request = request;

    /* Add this HyperDoc object to our list */
    if (!mr->htext) mr->htext = HTList_new();
    HTList_addObject(mr->htext, (void *) me);
    return me;
}

PUBLIC void HText_free (HText * me) {
    if (me) HT_FREE (me);
}

PUBLIC void HText_beginAnchor (HText * text, HTChildAnchor * anchor)
{
    if (text && anchor) {
	Finger * finger = (Finger *) HTRequest_context(text->request);
	Robot * mr = finger->robot;
	HTAnchor * dest = HTAnchor_followMainLink((HTAnchor *) anchor);
	HTParentAnchor * dest_parent = HTAnchor_parent(dest);
	char * uri = HTAnchor_address((HTAnchor *) dest_parent);
	HyperDoc * hd = HTAnchor_document(dest_parent);

	if (SHOW_MSG) HTTrace("Robot....... Found `%s\' - ", uri ? uri : "NULL");
	
	/* Test whether we already have a hyperdoc for this document */
	if (mr->flags & MR_LINK && dest_parent && !hd) {
	    HTParentAnchor * parent = HTRequest_parent(text->request);
	    HyperDoc * last = HTAnchor_document(parent);
	    int depth = last ? last->depth+1 : 0;
	    Finger * newfinger = Finger_new(mr, dest_parent, METHOD_GET);
	    HTRequest * newreq = newfinger->request;
	    HyperDoc_new(mr, dest_parent, depth);
	    HTRequest_setParent(newreq, HTRequest_anchor(text->request));
	    if (depth >= mr->depth) {
		if (SHOW_MSG)
		    HTTrace("loading at depth %d using HEAD\n", depth);
		HTRequest_setMethod(newreq, METHOD_HEAD);
		HTRequest_setOutputFormat(newreq, WWW_DEBUG);
	    } else {
		if (SHOW_MSG) HTTrace("loading at depth %d\n", depth);
	    }
	    if (HTLoadAnchor((HTAnchor *) dest_parent, newreq) != YES) {
		if (SHOW_MSG) HTTrace("not tested!\n");
		Finger_delete(newfinger);
	    }
	} else {
	    if (SHOW_MSG) HTTrace("duplicate or max depth reached\n");
	}
	HT_FREE(uri);
    }
}

PUBLIC void HText_appendImage (HText * text, HTChildAnchor * anchor,
			       const char *alt, const char * align, BOOL isMap)
{
    if (text && anchor) {
	Finger * finger = (Finger *) HTRequest_context(text->request);
	Robot * mr = finger->robot;
	HTParentAnchor * dest = (HTParentAnchor *)
	    HTAnchor_followMainLink((HTAnchor *) anchor);
	HyperDoc * hd = HTAnchor_document(dest);

	/* Test whether we already have a hyperdoc for this document */
	if (mr->flags & MR_IMG && dest && !hd) {
	    HTParentAnchor * parent = HTRequest_parent(text->request);
	    HyperDoc * last = HTAnchor_document(parent);
	    int depth = last ? last->depth+1 : 0;
	    Finger * newfinger = Finger_new(mr, dest,
					    mr->flags & MR_SAVE ?
					    METHOD_GET : METHOD_HEAD);
	    HTRequest * newreq = newfinger->request;
	    HyperDoc_new(mr, dest, depth);
	    if (SHOW_MSG) {
		char * uri = HTAnchor_address((HTAnchor *) dest);
		HTTrace("Robot....... Checking Image `%s\'\n", uri);
		HT_FREE(uri);
	    }
	    if (HTLoadAnchor((HTAnchor *) dest, newreq) != YES) {
		if (SHOW_MSG)
		    HTTrace("Robot....... Image not tested!\n");
		Finger_delete(newfinger);
	    }
	}
    }
}

PUBLIC void HText_endAnchor (HText * text) {}
PUBLIC void HText_appendText (HText * text, const char * str) {}
PUBLIC void HText_appendCharacter (HText * text, char ch) {}
PUBLIC void HText_endAppend (HText * text) {}
PUBLIC void HText_setStyle (HText * text, HTStyle * style) {}
PUBLIC void HText_beginAppend (HText * text) {}
PUBLIC void HText_appendParagraph (HText * text) {}

PRIVATE int RobotTrace (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}

/* ------------------------------------------------------------------------- */
/*				  MAIN PROGRAM				     */
/* ------------------------------------------------------------------------- */

int main (int argc, char ** argv)
{
    int		status = 0;
    int		arg;
    BOOL	cache = NO;			     /* Use persistent cache */
    BOOL	flush = NO;		       /* flush the persistent cache */
    HTChunk *	keywords = NULL;			/* From command line */
    int		keycnt = 0;
    Robot *	mr = NULL;
    Finger *	finger = NULL;
    HTParentAnchor * startAnchor = NULL;

    /* Starts Mac GUSI socket library */
#ifdef GUSI
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef __MWERKS__ /* STR */
    InitGraf((Ptr) &qd.thePort); 
    InitFonts(); 
    InitWindows(); 
    InitMenus(); TEInit(); 
    InitDialogs(nil); 
    InitCursor();
    SIOUXSettings.asktosaveonclose = false;
    argc=ccommand(&argv);
#endif

#ifdef MEMLOG
    HTMemLog_open("/tmp/data.log", 8192, YES);
#endif

    /* Initiate W3C Reference Library with a robot profile */
    HTProfile_newRobot(APP_NAME, APP_VERSION);
    HTTrace_setCallback(RobotTrace);

    /* Add the default HTML parser to the set of converters */
    {
	HTList * converters = HTFormat_conversion();
	HTMLInit(converters);
    }

    /* Build a new robot object */
    mr = Robot_new();

    /* Scan command Line for parameters */
    for (arg=1; arg<argc; arg++) {
	if (*argv[arg] == '-') {
	    
	    /* non-interactive */
	    if (!strcmp(argv[arg], "-n")) {
		HTAlert_setInteractive(NO);

	    /* log file */
	    } else if (!strcmp(argv[arg], "-l")) {
		mr->logfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_LOG_FILE;

	    /* rule file */
	    } else if (!strcmp(argv[arg], "-r")) {
		mr->rules = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_RULE_FILE;

	    /* output filename */
	    } else if (!strcmp(argv[arg], "-o")) { 
		mr->outputfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_OUTPUT_FILE;

	    /* timeout -- Change the default request timeout */
	    } else if (!strcmp(argv[arg], "-timeout")) {
		int timeout = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_TIMEOUT;
		if (timeout > 0) mr->timer = timeout;

	    /* Start the persistent cache */
	    } else if (!strcmp(argv[arg], "-cache")) {
		cache = YES;

	    /* Persistent cache flush */
	    } else if (!strcmp(argv[arg], "-flush")) {
		flush = YES;

	    /* Do a cache validation */
	    } else if (!strcmp(argv[arg], "-validate")) {
		mr->flags |= MR_VALIDATE;

	    /* Do an end-to-end cache-validation */
	    } else if (!strcmp(argv[arg], "-endvalidate")) {
		mr->flags |= MR_END_VALIDATE;

	    /* preemptive or non-preemptive access */
	    } else if (!strcmp(argv[arg], "-single")) {
		mr->flags |= MR_PREEMPTIVE;

	    /* test inlined images */
	    } else if (!strcmp(argv[arg], "-img")) {
		mr->flags |= MR_IMG;

	    /* load inlined images */
	    } else if (!strcmp(argv[arg], "-saveimg")) {
		mr->flags |= (MR_IMG | MR_SAVE);

	    /* load anchors */
	    } else if (!strcmp(argv[arg], "-link")) {
		mr->flags |= MR_LINK;
		mr->depth = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_DEPTH;

	    /* Output start and end time */
	    } else if (!strcmp(argv[arg], "-ss")) {
		time_t local = time(NULL);
		HTTrace("Robot started on %s\n",
			 HTDateTimeStr(&local, YES));
		mr->flags |= MR_TIME;

	    /* print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
		VersionInfo();
		Cleanup(mr, 0);

	    /* run in quiet mode */
	    } else if (!strcmp(argv[arg], "-q")) { 
		mr->flags |= MR_QUIET;

#ifdef WWWTRACE
	    /* trace flags */
	    } else if (!strncmp(argv[arg], "-v", 2)) {
		HTSetTraceMessageMask(argv[arg]+2);
#endif

	    } else {
		if (SHOW_MSG) HTTrace("Bad Argument (%s)\n", argv[arg]);
	    }
       } else {	 /* If no leading `-' then check for URL or keywords */
    	    if (!keycnt) {
		char * ref = HTParse(argv[arg], mr->cwd, PARSE_ALL);
		startAnchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
		HyperDoc_new(mr, startAnchor, 0);
		keycnt = 1;
		HT_FREE(ref);
	    } else {		   /* Check for successive keyword arguments */
		char *escaped = HTEscape(argv[arg], URL_XALPHAS);
		if (keycnt++ <= 1)
		    keywords = HTChunk_new(128);
		else
		    HTChunk_putc(keywords, ' ');
		HTChunk_puts(keywords, HTStrip(escaped));
		HT_FREE(escaped);
	    }
	}
    }

#ifdef CATCH_SIG
    SetSignal();
#endif

    if (!keycnt) {
	if (SHOW_MSG) HTTrace("Please specify URL to check.\n");
	Cleanup(mr, -1);
    }

    /* Testing that HTTrace is working */
    if (SHOW_MSG) HTTrace ("Welcome to the W3C mini Robot\n");

    /* Rule file specified? */
    if (mr->rules) {
	char * rules = HTParse(mr->rules, mr->cwd, PARSE_ALL);
	if (!HTLoadRules(rules))
	    if (SHOW_MSG) HTTrace("Can't access rules\n");
	HT_FREE(rules);
    }

    /* Output file specified? */
    if (mr->outputfile) {
	if ((mr->output = fopen(mr->outputfile, "wb")) == NULL) {
	    if (SHOW_MSG) HTTrace("Can't open `%s'\n", mr->outputfile);
	    mr->output = OUTPUT;
	}
    }

    /* Should we use persistent cache? */
    if (cache) {
	HTCacheInit(NULL, 20);
	HTNet_addBefore(HTCacheFilter, "http://*", NULL, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTCacheUpdateFilter, "http://*", NULL,
		       HT_NOT_MODIFIED, HT_FILTER_MIDDLE);

	/* Should we start by flushing? */
	if (flush) HTCache_flushAll();
    }

    /* Log file specifed? */
    if (mr->logfile) HTLog_open(mr->logfile, YES, YES);

    /* Register our own someterminater filter */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Setting event timeout */
    HTHost_setEventTimeout(mr->timer);

    /* Start the request */
    finger = Finger_new(mr, startAnchor, METHOD_GET);

    /*
    ** Make sure that the first request is flushed immediately and not
    ** buffered in the output buffer
    */
    HTRequest_setFlush(finger->request, YES);

    /*
    ** Check whether we should do some kind of cache validation on
    ** the load
    */
    if (mr->flags & MR_VALIDATE)
	HTRequest_setReloadMode(finger->request, HT_CACHE_VALIDATE);
    if (mr->flags & MR_END_VALIDATE)
	HTRequest_setReloadMode(finger->request, HT_CACHE_END_VALIDATE);

    /*
    **  Now do the load
    */
    if (mr->flags & MR_PREEMPTIVE)
	HTRequest_setPreemptive(finger->request, YES);

    if (keywords)						   /* Search */
	status = HTSearchAnchor(keywords, (HTAnchor *)startAnchor, finger->request);
    else
	status = HTLoadAnchor((HTAnchor *)startAnchor, finger->request);

    if (keywords) HTChunk_delete(keywords);
    if (status != YES) {
	if (SHOW_MSG) HTTrace("Can't access resource\n");
	Cleanup(mr, -1);
    }

    /* Go into the event loop... */
    HTEventList_loop(finger->request);

    /* Only gets here if event loop fails */
    Cleanup(mr, 0);
    return 0;
}
