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

#include "HTRobot.h"			     		 /* Implemented here */

#ifndef VR
#define VR "unspecified"
#endif

#define APP_NAME		"W3CRobot"
#define APP_VERSION		VR

/* Default page for "-help" command line option */
#define HELP	"http://www.w3.org/pub/WWW/Robot/User/CommandLine.html"

#define DEFAULT_OUTPUT_FILE	"robot.out"
#define DEFAULT_RULE_FILE	"robot.conf"
#define DEFAULT_LOG_FILE       	"robot.log"

#define SHOW_MSG		(WWWTRACE || HTAlert_interactive())

#define DEFAULT_TIMEOUT		10		       /* timeout in seconds */

#if defined(__svr4__)
#define CATCH_SIG
#endif

typedef enum _MRFlags {
    MR_IMG	= 0x1,
    MR_LINK	= 0x2,
    MR_PREEMTIVE= 0x4
} MRFlags;

typedef struct _Robot {
    HTRequest *		request;
    HTParentAnchor *	anchor;
    int			depth;			     /* How deep is our tree */
    HTList *		hyperdoc;	     /* List of our HyperDoc Objects */
    struct timeval *	tv;				/* Timeout on socket */
    char *		cwd;				  /* Current dir URL */
    HTList *		converters;
    char *		rules;
    char *		logfile;
    char *		outputfile;
    FILE *	        output;
    MRFlags		flags;
} Robot;
	
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
typedef struct _HText {
    HTRequest *		request;
} HText;

typedef struct _HTStyle HTStyle;
typedef struct _HTStyleSheet HTStyleSheet;

PUBLIC HText * HTMainText = NULL;
PUBLIC HTParentAnchor * HTMainAnchor = NULL;
PUBLIC HTStyleSheet * styleSheet = NULL;

/* ------------------------------------------------------------------------- */

/*	Create a "HyperDoc" object
**	--------------------------
**	A HyperDoc object contains information about whether we have already
**	started checking the anchor and the depth in our search
*/
PRIVATE HyperDoc * HyperDoc_new (Robot * mr,HTParentAnchor * anchor, int depth)
{
    HyperDoc * hd;
    if ((hd = (HyperDoc *) calloc(1, sizeof(HyperDoc))) == NULL)
	outofmem(__FILE__, "HyperDoc_new");
    hd->state = L_INVALID;
    hd->depth = depth;
 
    /* Bind the HyperDoc object together with the Anchor Object */
    hd->anchor = anchor;
    HTAnchor_setDocument(anchor, (void *) hd);

    /* Add this HyperDoc object to our list */
    if (!mr->hyperdoc) mr->hyperdoc = HTList_new();
    HTList_addObject(mr->hyperdoc, (void *) hd);

    if (SHOW_MSG)
	TTYPrint(TDEST, "HyperDoc.... %p bound to anchor %p with depth %d\n",
		 hd, anchor, depth);
    return hd;
}

/*	Delete a "HyperDoc" object
**	--------------------------
*/
PRIVATE BOOL HyperDoc_delete (HyperDoc * hd)
{
    if (hd) {
	free (hd);
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
    if ((me = (Robot *) calloc(1, sizeof(Robot))) == NULL ||
	(me->tv = (struct timeval*) calloc(1, sizeof(struct timeval))) == NULL)
	outofmem(__FILE__, "Robot_new");
    me->hyperdoc = HTList_new();
    me->tv->tv_sec = DEFAULT_TIMEOUT;
    me->cwd = HTFindRelatedName();
    me->output = OUTPUT;

    /* Bind the Robot object together with the Request Object */
    me->request = HTRequest_new();
    HTRequest_setContext (me->request, me);
    return me;
}

/*	Delete a Command Line Object
**	----------------------------
*/
PRIVATE BOOL Robot_delete (Robot * me)
{
    if (me) {
	if (me->hyperdoc) {
	    HTList * cur = me->hyperdoc;
	    HyperDoc * pres;
	    while ((pres = (HyperDoc *) HTList_nextObject(cur)))
		HyperDoc_delete(pres);
	    HTList_delete(me->hyperdoc);
	}
	if (me->logfile) HTLog_close();
	if (me->output && me->output != STDOUT) fclose(me->output);
	FREE(me->cwd);
	free(me->tv);
	free(me);
	return YES;
    }
    return NO;
}

/*
**  This function creates a new request object and initializes it
*/
PRIVATE HTRequest * Thread_new (Robot * mr, HTMethod method)
{
    HTRequest * newreq = HTRequest_new();
    HTRequest_setContext (newreq, mr);
    if (mr->flags & MR_PREEMTIVE) HTRequest_setPreemtive(newreq, YES);
    HTRequest_addRqHd(newreq, HT_HOST);
    HTRequest_setMethod(newreq, method);
    return newreq;
}

PRIVATE BOOL Thread_delete (Robot * mr, HTRequest * request)
{
    if (mr && request) {
	HTRequest_delete(request);
	return YES;
    }
    return NO;
}

/*
**  Cleanup and make sure we close all connections including the persistent
**  ones
*/
PRIVATE void Cleanup (Robot * me, int status)
{
    HTNet_killAll();
    Robot_delete(me);
    HTLibTerminate();
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
	if (PROT_TRACE) TTYPrint(TDEST, "HTSignal.... Can't catch SIGPIPE\n");
    } else {
	if (PROT_TRACE) TTYPrint(TDEST, "HTSignal.... Ignoring SIGPIPE\n");
    }
}
#endif /* CATCH_SIG */

PRIVATE void VersionInfo (void)
{
    TTYPrint(OUTPUT,"\n\nW3C Reference Software\n\n");
    TTYPrint(OUTPUT,"\tW3C Mini Robot (%s) version %s.\n",
	     APP_NAME, APP_VERSION);
    TTYPrint(OUTPUT,"\tW3C Reference Library version %s.\n\n",HTLib_version());
    TTYPrint(OUTPUT,"Please send feedback to <libwww@w3.org>\n");
}

/*	terminate_handler
**	-----------------
**	This function is registered to handle the result of the request.
**	If no more requests are pending then terminate program
*/
PRIVATE int terminate_handler (HTRequest * request, int status) 
{
    Robot * mr = (Robot *) HTRequest_context(request);
    if (mr->logfile) HTLog_add(request, status);
    Thread_delete(mr, request);
    if (HTNet_isEmpty()) Cleanup(mr, 0);
    return HT_OK;
}

/*	timeout_handler
**	---------------
**	This function is registered to handle timeout in select eventloop
*/
PRIVATE int timeout_handler (HTRequest * request)
{
    Robot * mr = (Robot *) HTRequest_context(request);
    if (SHOW_MSG) TTYPrint(TDEST, "Robot....... Request timeout...\n");
    HTRequest_kill(request);
    Thread_delete(mr, request);
    return 0;
}

/* ------------------------------------------------------------------------- */
/*				HTEXT INTERFACE				     */
/* ------------------------------------------------------------------------- */

PUBLIC HText * HText_new2 (HTRequest * request, HTParentAnchor * anchor,
			   HTStream * stream)
{
    HText * me;
    if ((me = (HText *) calloc(1, sizeof(HText))) == NULL)
	outofmem(__FILE__, "HText_new2");
    me->request = request;
    return me;
}

PUBLIC void HText_beginAnchor (HText * text, HTChildAnchor * anchor)
{
    if (text && anchor) {
	Robot * mr = (Robot *) HTRequest_context(text->request);
	HTAnchor * dest = HTAnchor_followMainLink((HTAnchor *) anchor);
	HTParentAnchor * dest_parent = HTAnchor_parent(dest);
	HyperDoc * hd = HTAnchor_document(dest_parent);

	/* Test whether we already have a hyperdoc for this document */
	if (mr->flags & MR_LINK && dest_parent && !hd) {
	    HTParentAnchor * parent = HTRequest_parent(text->request);
	    HyperDoc * last = HTAnchor_document(parent);
	    int depth = last ? last->depth+1 : 0;
	    HTRequest * newreq = Thread_new(mr, METHOD_GET);
	    HyperDoc_new(mr, dest_parent, depth);
	    if (SHOW_MSG) {
		char * uri = HTAnchor_address((HTAnchor *) dest_parent);
		TTYPrint(TDEST, "Robot....... Loading `%s\'\n", uri);
		free(uri);
	    }
	    if (HTLoadAnchor((HTAnchor *) dest_parent, newreq) != YES) {
		if (SHOW_MSG) TTYPrint(TDEST, "Robot...... URI Not tested!\n");
		Thread_delete(mr, newreq);
	    }
	}
    }
}

PUBLIC void HText_appendImage (HText * text, HTChildAnchor * anchor,
			       CONST char *alt, CONST char * align, BOOL isMap)
{
    if (text && anchor) {
	Robot * mr = (Robot *) HTRequest_context(text->request);
	HTParentAnchor * dest = (HTParentAnchor *)
	    HTAnchor_followMainLink((HTAnchor *) anchor);
	HyperDoc * hd = HTAnchor_document(dest);

	/* Test whether we already have a hyperdoc for this document */
	if (mr->flags & MR_IMG && dest && !hd) {
	    HTParentAnchor * parent = HTRequest_parent(text->request);
	    HyperDoc * last = HTAnchor_document(parent);
	    int depth = last ? last->depth+1 : 0;
	    HTRequest * newreq = Thread_new(mr, METHOD_HEAD);
	    HyperDoc_new(mr, dest, depth);
	    if (SHOW_MSG) {
		char * uri = HTAnchor_address((HTAnchor *) dest);
		TTYPrint(TDEST, "Robot....... Checking Image `%s\'\n", uri);
		free(uri);
	    }
	    if (HTLoadAnchor((HTAnchor *) dest, newreq) != YES) {
		if (SHOW_MSG)
		    TTYPrint(TDEST, "Robot....... Image not tested!\n");
		Thread_delete(mr, newreq);
	    }
	}
    }
}

PUBLIC void HText_endAnchor (HText * text) {}
PUBLIC void HText_appendText (HText * text, CONST char * str) {}
PUBLIC void HText_appendCharacter (HText * text, char ch) {}
PUBLIC void HText_endAppend (HText * text) {}
PUBLIC void HText_setStyle (HText * text, HTStyle * style) {}
PUBLIC void HText_beginAppend (HText * text) {}
PUBLIC void HText_appendParagraph (HText * text) {}
PUBLIC BOOL HText_delete (HText * me) { return YES; }

/* ------------------------------------------------------------------------- */
/*				  MAIN PROGRAM				     */
/* ------------------------------------------------------------------------- */

int main (int argc, char ** argv)
{
    int		status = 0;	
    int		arg;
    HTChunk *	keywords = NULL;			/* From command line */
    int		keycnt = 0;
    Robot *	mr = Robot_new();		/* Create new Robot instance */

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

    /* Initiate W3C Reference Library */
    HTLibInit(APP_NAME, APP_VERSION);

    /* Initialize the protocol modules */
    HTAccessInit();

    /* Initialize set of converters */
    mr->converters = HTList_new();
    HTConverterInit(mr->converters);
    HTFormat_setConversion(mr->converters);

    /* Initialize bindings between file suffixes and media types */
    HTFileInit();

    /* Get any proxy or gateway environment variables */
    HTProxy_getEnvVar();

    /* Scan command Line for parameters */
    for (arg=1; arg<argc; arg++) {
	if (*argv[arg] == '-') {
	    
	    /* -? or -help: show the command line help page */
	    if (!strcmp(argv[arg],"-?") || !strcmp(argv[arg],"-help")) {
		mr->anchor = (HTParentAnchor *) HTAnchor_findAddress(HELP);
		keycnt = 1;

	    /* non-interactive */
	    } else if (!strcmp(argv[arg], "-n")) {
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
		if (timeout > 0) mr->tv->tv_sec = timeout;

	    /* preemtive or non-preemtive access */
	    } else if (!strcmp(argv[arg], "-single")) {
		HTRequest_setPreemtive(mr->request, YES);
		mr->flags |= MR_PREEMTIVE;

	    /* test inlined images */
	    } else if (!strcmp(argv[arg], "-img")) {
		mr->flags |= MR_IMG;

	    /* load anchors */
	    } else if (!strcmp(argv[arg], "-link")) {
		mr->flags |= MR_LINK;

	    /* preemtive or non-preemtive access */
	    } else if (!strcmp(argv[arg], "-single")) {
		HTRequest_setPreemtive(mr->request, YES);
		mr->flags |= MR_PREEMTIVE;

	    /* print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
		VersionInfo();
		Cleanup(mr, 0);

#ifdef WWWTRACE
	    /* trace flags */
	    } else if (!strncmp(argv[arg], "-v", 2)) {
	    	char *p = argv[arg]+2;
		WWWTRACE = 0;
		for(; *p; p++) {
		    switch (*p) {
		      case 'a': WWWTRACE |= SHOW_ANCHOR_TRACE; break;
		      case 'b': WWWTRACE |= SHOW_BIND_TRACE; break;
		      case 'c': WWWTRACE |= SHOW_CACHE_TRACE; break;
		      case 'g':	WWWTRACE |= SHOW_SGML_TRACE; break;
		      case 'p':	WWWTRACE |= SHOW_PROTOCOL_TRACE; break;
		      case 's':	WWWTRACE |= SHOW_STREAM_TRACE; break;
		      case 't':	WWWTRACE |= SHOW_THREAD_TRACE; break;
		      case 'u': WWWTRACE |= SHOW_URI_TRACE; break;
		      default:
			if (SHOW_MSG)
			    TTYPrint(TDEST,"Bad parameter (%s) in -v option\n",
				     argv[arg]);
		    }
		}
		if (!WWWTRACE) WWWTRACE = SHOW_ALL_TRACE;
#endif

	    } else {
		if (SHOW_MSG) TTYPrint(TDEST,"Bad Argument (%s)\n", argv[arg]);
	    }
	} else {	 /* If no leading `-' then check for URL or keywords */
    	    if (!keycnt) {
		char * ref = HTParse(argv[arg], mr->cwd, PARSE_ALL);
		mr->anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
		keycnt = 1;
		FREE(ref);
	    } else {		   /* Check for successive keyword arguments */
		char *escaped = HTEscape(argv[arg], URL_XALPHAS);
		if (keycnt++ <= 1)
		    keywords = HTChunkCreate(128);
		else
		    HTChunkPutc(keywords, ' ');
		HTChunkPuts(keywords, HTStrip(escaped));
		free(escaped);
	    }
	}
    }

#ifdef CATCH_SIG
    SetSignal();
#endif

    if (!keycnt) {
	if (SHOW_MSG) TTYPrint(TDEST, "Please specify URL to check.\n");
	Cleanup(mr, -1);
    }

    /* Rule file specified? */
    if (mr->rules) {
	HTList * list = HTList_new();
	HTRequest * rr = HTRequest_new();
	char * rules = HTParse(mr->rules, mr->cwd, PARSE_ALL);
	HTParentAnchor * ra = (HTParentAnchor *) HTAnchor_findAddress(rules);
	HTRequest_setPreemtive(rr, YES);
	HTConversion_add(list, "application/x-www-rules", "*/*", HTRules,
			 1.0, 0.0, 0.0);
	HTRequest_setConversion(rr, list, YES);
	if (HTLoadAnchor((HTAnchor *) ra, rr) != YES)
	    if (SHOW_MSG) TTYPrint(TDEST, "Can't access rules\n");
	HTConversion_deleteAll(list);
	HTRequest_delete(rr);
	FREE(rules);
    }

    /* Output file specified? */
    if (mr->outputfile) {
	if ((mr->output = fopen(mr->outputfile, "wb")) == NULL) {
	    if (SHOW_MSG) TTYPrint(TDEST, "Can't open `%s'\n", mr->outputfile);
	    mr->output = OUTPUT;
	}
    }

    /* Set up the output */
    HTRequest_setOutputStream(mr->request, HTFWriter_new(mr->output, YES));

    /* Log file specifed? */
    if (mr->logfile) HTLog_open(mr->logfile, YES, YES);

    /* Register our User Prompts etc in the Alert Manager */
    if (HTAlert_interactive()) {
	HTAlert_add(HTError_print, HT_A_MESSAGE);
	HTAlert_add(HTConfirm, HT_A_CONFIRM);
	HTAlert_add(HTPrompt, HT_A_PROMPT);
	HTAlert_add(HTPromptPassword, HT_A_SECRET);
	HTAlert_add(HTPromptUsernameAndPassword, HT_A_USER_PW);
    }

    /* Register a call back function for the Net Manager */
    HTNetCall_addAfter(terminate_handler, HT_ALL);
    
    /* Set timeout on sockets */
    HTEvent_registerTimeout(mr->tv, mr->request, timeout_handler, NO);

    /* Start the request */
    if (keywords)						   /* Search */
	status = HTSearch(HTChunkData(keywords), mr->anchor, mr->request);
    else
	status = HTLoadAnchor((HTAnchor *) mr->anchor, mr->request);

    if (keywords) HTChunkFree(keywords);
    if (status != YES) {
	if (SHOW_MSG) TTYPrint(TDEST, "Can't access resource\n");
	Cleanup(mr, -1);
    }

    /* Go into the event loop... */
    HTEvent_Loop(mr->request);

    /* Only gets here if event loop fails */
    Cleanup(mr, 0);
    return 0;
}
