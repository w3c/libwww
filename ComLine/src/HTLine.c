/*         		    					     HTLine.c
**	W3C COMMAND LINE TOOL
**
**	(c) COPRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**  Authors:
**	HFN		Henrik Frystyk Nielsen, (frystyk@w3.org)
**
**  History:
**	Nov 24 95	First version
*/

#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWNews.h"				       /* News access module */
#include "WWWHTTP.h"				       /* HTTP access module */
#include "HTFTP.h"
#include "HTFile.h"
#include "HTGopher.h"

#include "WWWMIME.h"				    /* MIME parser/generator */
#include "HTPlain.h"
#include "HTMLGen.h"
#include "HTGuess.h"

#include "HTHome.h"
#include "HTDialog.h"
#include "HTLog.h"
#include "HTRules.h"
#include "HTProxy.h"

#include "HTLine.h"			     		 /* Implemented here */

#ifndef VT
#define VT "unspecified"
#endif

#define APP_NAME		"W3CCommandLine"
#define APP_VERSION		VT

/* Default page for "-help" command line option */
#define HELP	"http://www.w3.org/pub/WWW/ComLine/User/CommandLine.html"

#define DEFAULT_OUTPUT_FILE	"w3c.out"
#define DEFAULT_RULE_FILE	"w3c.conf"
#define DEFAULT_LOG_FILE       	"w3c.log"

#define SHOW_MSG		(WWWTRACE || HTAlert_interactive())

#define DEFAULT_TIMEOUT		10		       /* timeout in seconds */

#define DEFAULT_FORMAT		WWW_SOURCE

#if defined(__svr4__)
#define CATCH_SIG
#endif

typedef enum _CLFlags {
    CL_FILTER	=0x1,
    CL_COUNT	=0x2
} CLFlags;

typedef struct _ComLine {
    HTRequest *		request;
    HTParentAnchor *	anchor;
    HTParentAnchor *	dest;			 /* Destination for PUT etc. */
    struct timeval *	tv;				/* Timeout on socket */
    char *		cwd;				  /* Current dir URL */
    char *		rules;
    char *		logfile;
    char *		outputfile;
    FILE *	        output;
    HTFormat		format;		        /* Input format from console */
    CLFlags		flags;
} ComLine;
	
/* ------------------------------------------------------------------------- */

/*	Create a Command Line Object
**	----------------------------
*/
PRIVATE ComLine * ComLine_new (void)
{
    ComLine * me;
    if ((me = (ComLine *) calloc(1, sizeof(ComLine))) == NULL ||
	(me->tv = (struct timeval*) calloc(1, sizeof(struct timeval))) == NULL)
	outofmem(__FILE__, "ComLine_new");
    me->tv->tv_sec = DEFAULT_TIMEOUT;
    me->cwd = HTFindRelatedName();
    me->output = OUTPUT;

    /* Bind the ConLine object together with the Request Object */
    me->request = HTRequest_new();
    HTRequest_setOutputFormat(me->request, DEFAULT_FORMAT);
    HTRequest_setContext (me->request, me);
    return me;
}

/*	Delete a Command Line Object
**	----------------------------
*/
PRIVATE BOOL ComLine_delete (ComLine * me)
{
    if (me) {
	HTRequest_delete(me->request);
	if (me->logfile) HTLog_close();
	if (me->output && me->output != STDOUT) fclose(me->output);
	FREE(me->cwd);
	free(me->tv);
	free(me);
	return YES;
    }
    return NO;
}

PRIVATE void Cleanup (ComLine * me, int status)
{
    ComLine_delete(me);
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
    TTYPrint(OUTPUT,"\tW3C Command Line Tool (%s) version %s.\n",
	     APP_NAME, APP_VERSION);
    TTYPrint(OUTPUT,"\tW3C Reference Library version %s.\n\n",HTLib_version());
    TTYPrint(OUTPUT,"Please send feedback to <libwww@w3.org>\n");
}

/*	terminate_handler
**	-----------------
**	This function is registered to handle the result of the request
*/
PRIVATE int terminate_handler (HTRequest * request, int status) 
{
    ComLine * cl = (ComLine *) HTRequest_context(request);
    if (status == HT_LOADED) {
	if (cl->flags & CL_COUNT) {
	    TTYPrint(OUTPUT, "Content Length found to be %ld\n",
		     HTAnchor_length(cl->anchor));
	}
    }
    if (cl->logfile) HTLog_add(request, status);
    Cleanup(cl, status == HT_LOADED ? 0 : -1);
    return HT_OK;
}

/*	timeout_handler
**	---------------
**	This function is registered to handle timeout in select eventloop
*/
PRIVATE int timeout_handler (HTRequest * request)
{
    ComLine * cl = (ComLine *) HTRequest_context(request);
    if (SHOW_MSG) TTYPrint(TDEST, "Request timeout...\n");
    HTRequest_kill(request);
    Cleanup(cl, -1);
    return 0;
}

/*	header_handler
**	---------------
**	This function is registered to handle unknown MIME headers
*/
PRIVATE int header_handler (HTRequest * request, CONST char * token)
{
    if (SHOW_MSG) TTYPrint(TDEST, "Parsing unknown header `%s\'\n", token);
    return HT_OK;
}

/* ------------------------------------------------------------------------- */
/*				  MAIN PROGRAM				     */
/* ------------------------------------------------------------------------- */

int main (int argc, char ** argv)
{
    int		status = 0;	
    int		arg;
    HTChunk *	keywords = NULL;			/* From command line */
    int		keycnt = 0;
    HTList *	conv = NULL;
    ComLine *	cl = ComLine_new();

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
    HTProtocol_add("http", NO, HTLoadHTTP, NULL);
    HTProtocol_add("file", NO, HTLoadFile, NULL);
    HTProtocol_add("ftp", NO, HTLoadFTP, NULL);
    HTProtocol_add("nntp", NO, HTLoadNews, NULL);
    HTProtocol_add("news", NO, HTLoadNews, NULL);
    HTProtocol_add("gopher", NO, HTLoadGopher, NULL);

    /* Initialize set of converters */
    conv = HTList_new();

    /* GENERIC converters */
    HTConversion_add(conv,"text/x-nntp-list","*/*", HTNewsList, 1.0, 0.0, 0.0);
    HTConversion_add(conv,"text/x-nntp-over","*/*",HTNewsGroup, 1.0, 0.0, 0.0);

    HTConversion_add(conv,"message/rfc822","*/*",HTMIMEConvert, 1.0, 0.0, 0.0);
    HTConversion_add(conv,"multipart/*", "*/*", HTBoundary, 1.0, 0.0, 0.0);

    HTConversion_add(conv,"www/unknown", "*/*",	HTGuess_new, 1.0, 0.0, 0.0);

    /* Normal converters */
    HTConversion_add(conv, "*/*", "www/present", HTSaveLocally, 0.5, 0.0, 0.0);

    /* Bind the list of converters to the request object */
    HTRequest_setConversion(cl->request, conv, YES);
    
    /* Get any proxy or gateway environment variables */
    HTProxy_getEnvVar();

    /* Scan command Line for parameters */
    for (arg=1; arg<argc; arg++) {
	if (*argv[arg] == '-') {
	    
	    /* - alone => filter */
	    if (argv[arg][1] == '\0') {
		cl->flags |= CL_FILTER;	   
	    
	    /* -? or -help: show the command line help page */
	    } else if (!strcmp(argv[arg],"-?") || !strcmp(argv[arg],"-help")) {
		cl->anchor = (HTParentAnchor *) HTAnchor_findAddress(HELP);
		keycnt = 1;

	    /* non-interactive */
	    } else if (!strcmp(argv[arg], "-n")) {
		HTAlert_setInteractive(NO);

	    /* from -- Initial represntation (only with filter) */
	    } else if (!strcmp(argv[arg], "-from")) {
		cl->format = (arg+1 < argc && *argv[arg+1] != '-') ?
		    HTAtom_for(argv[++arg]) : WWW_HTML;

	    /* to -- Final representation */
	    } else if (!strcmp(argv[arg], "-to")) {
		HTFormat format = (arg+1 < argc && *argv[arg+1] != '-') ?
		    HTAtom_for(argv[++arg]) : DEFAULT_FORMAT;
		HTRequest_setOutputFormat(cl->request, format);

	    /* destination for PUT, POST etc. */
	    } else if (!strcmp(argv[arg], "-dest")) {
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    char * dest = HTParse(argv[++arg], cl->cwd, PARSE_ALL);
		    cl->dest = (HTParentAnchor *) HTAnchor_findAddress(dest);
		    FREE(dest);
		}

	    /* source please */
	    } else if (!strcmp(argv[arg], "-source")) {
		HTRequest_setOutputFormat(cl->request, WWW_RAW);

	    /* log file */
	    } else if (!strcmp(argv[arg], "-l")) {
		cl->logfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_LOG_FILE;

	    /* rule file */
	    } else if (!strcmp(argv[arg], "-r")) {
		cl->rules = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_RULE_FILE;

	    /* output filename */
	    } else if (!strcmp(argv[arg], "-o")) { 
		cl->outputfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_OUTPUT_FILE;

	    /* timeout -- Change the default request timeout */
	    } else if (!strcmp(argv[arg], "-timeout")) {
		int timeout = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_TIMEOUT;
		if (timeout > 0) cl->tv->tv_sec = timeout;

	    /* preemtive or non-preemtive access */
	    } else if (!strcmp(argv[arg], "-single")) {
		HTRequest_setPreemtive(cl->request, YES);

	    /* content Length Counter */
	    } else if (!strcmp(argv[arg], "-cl")) { 
		cl->flags |= CL_COUNT;

	    /* print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
		VersionInfo();
		Cleanup(cl, 0);

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

	    /* GET method */
	    } else if (!strcasecomp(argv[arg], "-get")) {
		HTRequest_setMethod(cl->request, METHOD_GET);

	    /* HEAD method */
	    } else if (!strcasecomp(argv[arg], "-head")) {
		HTRequest_setMethod(cl->request, METHOD_HEAD);
		HTRequest_setOutputFormat(cl->request, WWW_MIME);

	    /* DELETE method */
	    } else if (!strcasecomp(argv[arg], "-delete")) {
		HTRequest_setMethod(cl->request, METHOD_DELETE);

	    /* POST Method */
	    } else if (!strcasecomp(argv[arg], "-post")) {
		HTRequest_setMethod(cl->request, METHOD_POST);

	    /* PUT Method */
	    } else if (!strcasecomp(argv[arg], "-put")) {
		HTRequest_setMethod(cl->request, METHOD_PUT);

	    } else {
		if (SHOW_MSG) TTYPrint(TDEST,"Bad Argument (%s)\n", argv[arg]);
	    }
	} else {	 /* If no leading `-' then check for URL or keywords */
    	    if (!keycnt) {
		char * ref = HTParse(argv[arg], cl->cwd, PARSE_ALL);
		cl->anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
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

    if (!keycnt && !cl->flags & CL_FILTER) {
	if (SHOW_MSG) TTYPrint(TDEST, "No URL specified\n");
	Cleanup(cl, -1);
    }

    /* Rule file specified? */
    if (cl->rules) {
	HTList * list = HTList_new();
	HTRequest * rr = HTRequest_new();
	char * rules = HTParse(cl->rules, cl->cwd, PARSE_ALL);
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

    /* Destination specified? */
    if (cl->dest) {
	HTMethod method = HTRequest_method(cl->request);
	HTAnchor_link((HTAnchor*)cl->anchor, (HTAnchor*)cl->dest, NULL,method);
    }

    /* Output file specified? */
    if (cl->outputfile) {
	if ((cl->output = fopen(cl->outputfile, "wb")) == NULL) {
	    if (SHOW_MSG) TTYPrint(TDEST, "Can't open `%s'\\n",cl->outputfile);
	    cl->output = OUTPUT;
	}
    }

    /* Set up the output */
    HTRequest_setOutputStream(cl->request, HTFWriter_new(cl->output, YES));

    /* Log file specifed? */
    if (cl->logfile) HTLog_open(cl->logfile, YES, YES);

    /* Just convert formats */
    if (cl->flags & CL_FILTER) {
#ifdef STDIN_FILENO
	HTRequest_setAnchor(cl->request, (HTAnchor *) cl->anchor);
	HTRequest_setPreemtive(cl->request, YES);
	HTLoadSocket(STDIN_FILENO, cl->request, FD_NONE);
#endif
	Cleanup(cl, 0);
    }
    
    /* Content Length Counter */
    if (cl->flags & CL_COUNT) {
	HTRequest_setOutputStream(cl->request,
				  HTContentCounter(HTBlackHole(),
						   cl->request, 0x2000));
    }

    /* Register our User Prompts etc in the Alert Manager */
    if (HTAlert_interactive()) {
	HTAlert_add(HTError_print, HT_A_MESSAGE);
	HTAlert_add(HTPromptUsernameAndPassword, HT_A_USER_PW);
	HTAlert_add(HTError_print, HT_A_MESSAGE);
    }

    /* Register a call back function for the Net Manager */
    HTNetCall_addAfter(terminate_handler, HT_ALL);
    
    /* Register our own MIME header handler for extra headers */
    HTHeader_addParser("*", NO, header_handler);

    /* Set timeout on sockets */
    HTEvent_registerTimeout(cl->tv, cl->request, timeout_handler, NO);

    /* Start the request */
    if (cl->dest)					   /* PUT, POST etc. */
	status = HTCopyAnchor((HTAnchor *) cl->anchor, cl->request);
    else if (keywords)						   /* Search */
	status = HTSearch(HTChunkData(keywords), cl->anchor, cl->request);
    else						   /* GET, HEAD etc. */
	status = HTLoadAnchor((HTAnchor *) cl->anchor, cl->request);

    if (keywords) HTChunkFree(keywords);
    if (status != YES) {
	if (SHOW_MSG) TTYPrint(TDEST, "Can't access resource\n");
	Cleanup(cl, -1);
    }

    /* Go into the event loop... */
    HTEvent_Loop(cl->request);

    /* Only gets here if event loop fails */
    Cleanup(cl, 0);
    return 0;
}
