/*         		    					     HTLine.c
**	W3C COMMAND LINE TOOL
**
**	(c) COPRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**  Authors:
**	HFN		Henrik Frystyk Nielsen, (frystyk@w3.org)
**
**  History:
**	Nov 24 95	First version
*/

#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWApp.h"
#include "WWWMIME.h"				    /* MIME parser/generator */
#include "WWWHTML.h"				    /* HTML parser/generator */
#include "WWWNews.h"				       /* News access module */
#include "WWWHTTP.h"				       /* HTTP access module */
#include "WWWFTP.h"
#include "WWWFile.h"
#include "WWWGophe.h"
#include "WWWStream.h"
#include "WWWTrans.h"
#include "WWWInit.h"

#include "HTLine.h"			     		 /* Implemented here */

#ifndef W3C_VERSION
#define W3C_VERSION		"unspecified"
#endif

#define APP_NAME		"W3C-WebCon"
#define APP_VERSION		W3C_VERSION

/* Default page for "-help" command line option */
#define W3C_HELP		"http://www.w3.org/ComLine/README"

#define DEFAULT_OUTPUT_FILE	"w3c.out"
#define DEFAULT_RULE_FILE	"w3c.conf"
#define DEFAULT_LOG_FILE       	"w3c.log"

#define MILLIES			1000
#define DEFAULT_TIMEOUT		20		          /* timeout in secs */

#define DEFAULT_HOPS		0

#define DEFAULT_FORMAT		WWW_SOURCE

typedef enum _CLFlags {
    CL_FILTER		= 0x1,
    CL_COUNT		= 0x2,
    CL_QUIET		= 0x4,
    CL_VALIDATE		= 0x8,
    CL_END_VALIDATE	= 0x10,
    CL_CACHE_FLUSH	= 0x20
} CLFlags;

#define SHOW_MSG		(!(cl->flags & CL_QUIET))

typedef struct _ComLine {
    HTRequest *		request;
    HTParentAnchor *	anchor;
    HTParentAnchor *	dest;			 /* Destination for PUT etc. */
    int 		timer;				/* Timeout on socket */
    char *		cwd;				  /* Current dir URL */
    char *		rules;
    char *		logfile;
    HTLog *		log;
    char *		outputfile;
    FILE *	        output;
    HTFormat		format;		        /* Input format from console */

    char *		realm;			/* For automated authentication */
    char *		user;
    char *		password;

    CLFlags		flags;
} ComLine;

HTChunk * post_result = NULL;
	
/* ------------------------------------------------------------------------- */

PRIVATE int printer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stdout, fmt, pArgs));
}

PRIVATE int tracer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}

/*	Create a Command Line Object
**	----------------------------
*/
PRIVATE ComLine * ComLine_new (void)
{
    ComLine * me;
    if ((me = (ComLine *) HT_CALLOC(1, sizeof(ComLine))) == NULL)
	HT_OUTOFMEM("ComLine_new");
    me->timer = DEFAULT_TIMEOUT*MILLIES;
    me->cwd = HTGetCurrentDirectoryURL();
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
	if (me->log) HTLog_close(me->log);
	if (me->output && me->output != STDOUT) fclose(me->output);
	HT_FREE(me->cwd);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PRIVATE void Cleanup (ComLine * me, int status)
{
    ComLine_delete(me);
    HTProfile_delete();
#ifdef VMS
    exit(status ? status : 1);
#else
    exit(status ? status : 0);
#endif
}

PRIVATE void VersionInfo (const char * name)
{
    HTPrint("\nW3C OpenSource Software");
    HTPrint("\n-----------------------\n\n");
    HTPrint("\tWebCon version %s\n", APP_VERSION);
    HTPrint("\tusing the W3C libwww library version %s.\n\n",HTLib_version());
    HTPrint("\tTry \"%s -help\" for help\n\n", name ? name : APP_NAME);
    HTPrint("\tSee \"http://www.w3.org/ComLine/User/\" for user information\n");
    HTPrint("\tSee \"http://www.w3.org/ComLine/\" for general information\n\n");
    HTPrint("\tPlease send feedback to the <www-lib@w3.org> mailing list,\n");
    HTPrint("\tsee \"http://www.w3.org/Library/#Forums\" for details\n\n");
}

/*	terminate_handler
**	-----------------
**	This function is registered to handle the result of the request
*/
PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    ComLine * cl = (ComLine *) HTRequest_context(request);
    if (status == HT_LOADED) {
	if (cl) {
	    if (cl->flags & CL_COUNT) {
		HTPrint("Content Length found to be %ld\n",
			 HTAnchor_length(cl->anchor));
	    }
	}
    } else {
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) (*cbf)(request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			HTRequest_error(request), NULL);
    }
    Cleanup(cl, (status/100 == 2) ? 0 : -1);
    return HT_OK;
}

PRIVATE BOOL PromptUsernameAndPassword (HTRequest * request, HTAlertOpcode op,
					int msgnum, const char * dfault,
					void * input, HTAlertPar * reply)
{
    ComLine * cl = (ComLine *) HTRequest_context(request);
    char * realm = (char *) input;
    if (request && cl) {

	/*
	**  If we have a realm then check that it matches the realm
	**  that we got from the server.
	*/
	if (realm && cl->realm && !strcmp(cl->realm, realm)) {
	    HTAlert_setReplyMessage(reply, cl->user ? cl->user : "");
	    HTAlert_setReplySecret(reply, cl->password ? cl->password : "");
	    return YES;
	} else {
	    BOOL status = HTPrompt(request, op, msgnum, dfault, input, reply);
	    return status ?
		HTPromptPassword(request, op, HT_MSG_PW, dfault, input, reply) : NO;
	}
    }
    return NO;
}

PRIVATE BOOL ParseCredentials (ComLine * cl, char * credentials)
{
    if (cl && credentials) {
	char * start = credentials;
	char * end = credentials;

	/* Make sure we don't get inconsistent sets of information */
	cl->realm = NULL;
	cl->user = NULL;
	cl->password = NULL;

	/* Find the username */
	while (*end && *end!=':') end++;
	if (!*end) return NO;
	*end++ = '\0';
	cl->user = start;
	start = end;

	/* Find the password */
	while (*end && *end!='@') end++;
	if (!*end) return NO;
	*end++ = '\0';
	cl->password = start;
	start = end;

	/* Find the realm */
	cl->realm = start;
    }
    return YES;
}

/* ------------------------------------------------------------------------- */
/*				  MAIN PROGRAM				     */
/* ------------------------------------------------------------------------- */

int main (int argc, char ** argv)
{
    int		status = 0;	
    int		arg;
    int		tokencount = 0;
    BOOL	formdata = NO;
    HTChunk *	keywords = NULL;			/* From command line */
    HTAssocList*formfields = NULL;
    HTMethod	method = METHOD_GET;			    /* Default value */
    ComLine *	cl = ComLine_new();
    BOOL	cache = NO;			     /* Use persistent cache */
    BOOL	flush = NO;		       /* flush the persistent cache */
    char *	cache_root = NULL;

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

    /* Initiate W3C Reference Library with a client profile */
    HTProfile_newNoCacheClient(APP_NAME, APP_VERSION);

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /*
    ** Delete the default Username/password handler so that we can handle
    ** parameters handed to us from the command line. The default is set
    ** by the profile.
    */
    HTAlert_deleteOpcode(HT_A_USER_PW);
    HTAlert_add(PromptUsernameAndPassword, HT_A_USER_PW);

    /*
    ** Add default content decoder. We insert a through line as it doesn't
    ** matter that we get an encoding that we don't know.
    */
    HTFormat_addCoding("*", HTIdentityCoding, HTIdentityCoding, 0.3);

    /* Scan command Line for parameters */
    for (arg=1; arg<argc; arg++) {
	if (*argv[arg] == '-') {
	    
	    /* - alone => filter */
	    if (argv[arg][1] == '\0') {
		cl->flags |= CL_FILTER;	   
	    
	    /* -? or -help: show the command line help page */
	    } else if (!strcmp(argv[arg],"-?") || !strcmp(argv[arg],"-help")) {
		cl->anchor = (HTParentAnchor *) HTAnchor_findAddress(W3C_HELP);
		tokencount = 1;

	    /* non-interactive */
	    } else if (!strcmp(argv[arg], "-n")) {
		HTAlert_setInteractive(NO);

	    /* Treat the keywords as form data with a <name> "=" <value> */
	    } else if (!strcmp(argv[arg], "-form")) {
		formdata = YES;

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
		    HT_FREE(dest);
		}

	    /* source please */
	    } else if (!strcmp(argv[arg], "-source")) {
		HTRequest_setOutputFormat(cl->request, WWW_RAW);

	    /* log file */
	    } else if (!strcmp(argv[arg], "-l")) {
		cl->logfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_LOG_FILE;

	    /* Max forward hops in case of TRACE request */
	    } else if (!strcmp(argv[arg], "-hops") ||
		       !strcmp(argv[arg], "-maxforwards")) {
		int hops = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_HOPS;
		if (hops >= 0) HTRequest_setMaxForwards(cl->request, hops);

	    /* automated authentication of format user:password@realm */
	    } else if (!strncmp(argv[arg], "-auth", 5)) {
		char * credentials = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : NULL;
		if (credentials) ParseCredentials(cl, credentials);

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
		if (timeout >= 1) cl->timer = timeout*MILLIES;

	    /* preemptive or non-preemptive access */
	    } else if (!strcmp(argv[arg], "-single")) {
		HTRequest_setPreemptive(cl->request, YES);

	    /* content Length Counter */
	    } else if (!strcmp(argv[arg], "-cl")) { 
		cl->flags |= CL_COUNT;

	    /* print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
		VersionInfo(argv[0]);
		Cleanup(cl, 0);

	    /* run in quiet mode */
	    } else if (!strcmp(argv[arg], "-q")) { 
		cl->flags |= CL_QUIET;

	    /* Start the persistent cache */
	    } else if (!strcmp(argv[arg], "-cache")) {
		cache = YES;

	    /* Determine the cache root */
	    } else if (!strcmp(argv[arg], "-cacheroot")) { 
		cache_root = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : NULL;

	    /* Persistent cache flush */
	    } else if (!strcmp(argv[arg], "-flush")) {
		flush = YES;

	    /* Do a cache validation */
	    } else if (!strcmp(argv[arg], "-validate")) {
		cl->flags |= CL_VALIDATE;

	    /* Do an end-to-end cache-validation */
	    } else if (!strcmp(argv[arg], "-endvalidate")) {
		cl->flags |= CL_END_VALIDATE;

	    /* Force complete reload */
	    } else if (!strcmp(argv[arg], "-nocache")) {
		cl->flags |= CL_CACHE_FLUSH;

#ifdef WWWTRACE
	    /* trace flags */
	    } else if (!strncmp(argv[arg], "-v", 2)) {
		HTSetTraceMessageMask(argv[arg]+2);
#endif

	    /* GET method */
	    } else if (!strcasecomp(argv[arg], "-get")) {
		method = METHOD_GET;

	    /* HEAD method */
	    } else if (!strcasecomp(argv[arg], "-head")) {
		method = METHOD_HEAD;

	    /* DELETE method */
	    } else if (!strcasecomp(argv[arg], "-delete")) {
		method = METHOD_DELETE;

	    /* POST Method */
	    } else if (!strcasecomp(argv[arg], "-post")) {
		method = METHOD_POST;

	    /* PUT Method */
	    } else if (!strcasecomp(argv[arg], "-put")) {
		method = METHOD_PUT;

	    /* OPTIONS Method */
	    } else if (!strcasecomp(argv[arg], "-options")) {
		method = METHOD_OPTIONS;

	    /* TRACE Method */
	    } else if (!strcasecomp(argv[arg], "-trace")) {
		method = METHOD_TRACE;

	    } else {
		if (SHOW_MSG) HTPrint("Bad Argument (%s)\n", argv[arg]);
	    }
	} else {	 /* If no leading `-' then check for URL or keywords */
    	    if (!tokencount) {
		char * ref = HTParse(argv[arg], cl->cwd, PARSE_ALL);
		cl->anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
		tokencount = 1;
		HT_FREE(ref);
	    } else if (formdata) {		   /* Keywords are form data */
		char * string = argv[arg];
		if (tokencount++ <= 1) formfields = HTAssocList_new();
		HTParseFormInput(formfields, string);
	    } else {		   	       /* keywords are search tokens */
		char * escaped = HTEscape(argv[arg], URL_XALPHAS);
		if (tokencount++ <= 1)
		    keywords = HTChunk_new(128);
		else
		    HTChunk_putc(keywords, ' ');
		HTChunk_puts(keywords, HTStrip(escaped));
		HT_FREE(escaped);
	    }
	}
    }

    if (!tokencount && !cl->flags & CL_FILTER) {
	VersionInfo(argv[0]);
	Cleanup(cl, 0);
    }

    /* Should we use persistent cache? */
    if (cache) {
	HTCacheInit(cache_root, 20);

	/* Should we start by flushing? */
	if (flush) HTCache_flushAll();
    }

    /*
    ** Check whether we should do some kind of cache validation on
    ** the load
    */
    if (cl->flags & CL_VALIDATE)
	HTRequest_setReloadMode(cl->request, HT_CACHE_VALIDATE);
    else if (cl->flags & CL_END_VALIDATE)
	HTRequest_setReloadMode(cl->request, HT_CACHE_END_VALIDATE);
    else if (cl->flags & CL_CACHE_FLUSH)
	HTRequest_setReloadMode(cl->request, HT_CACHE_FLUSH);

    /* Add progress notification */
    if (cl->flags & CL_QUIET) HTAlert_deleteOpcode(HT_A_PROGRESS);

    /* Output file specified? */
    if (cl->outputfile) {
	if ((cl->output = fopen(cl->outputfile, "wb")) == NULL) {
	    if (SHOW_MSG) HTPrint("Can't open `%s'\\n",cl->outputfile);
	    cl->output = OUTPUT;
	}
    }

    /*
    ** Set up the output. Even though we don't use this explicit, it is
    ** required in order to show the stream stack that we know that we are
    ** getting raw data output on the output stream of the request object.
    */
    HTRequest_setOutputStream(cl->request,
			      HTFWriter_new(cl->request, cl->output, YES));

    /* Setting event timeout */
    HTHost_setEventTimeout(cl->timer);

    /*
    ** Make sure that the first request is flushed immediately and not
    ** buffered in the output buffer
    */
    HTRequest_setFlush(cl->request, YES);

    /* Log file specifed? */
    if (cl->logfile) {
	cl->log = HTLog_open(cl->logfile, YES, YES);
        if (cl->log) HTNet_addAfter(HTLogFilter, NULL, cl->log, HT_ALL, HT_FILTER_LATE);
    }

    /* Just convert formats */
    if (cl->flags & CL_FILTER) {
#ifdef STDIN_FILENO
	HTRequest_setAnchor(cl->request, (HTAnchor *) cl->anchor);
	HTRequest_setPreemptive(cl->request, YES);
	HTLoadSocket(STDIN_FILENO, cl->request);
#endif
	Cleanup(cl, 0);
    }
    
    /* Content Length Counter */
    if (cl->flags & CL_COUNT) {
	HTRequest_setOutputStream(cl->request,
				  HTContentCounter(HTBlackHole(),
						   cl->request, 0x2000));
    }

    /* Rule file specified? */
    if (cl->rules) {
	char * rules = HTParse(cl->rules, cl->cwd, PARSE_ALL);
	if (!HTLoadRulesAutomatically(rules))
	    if (SHOW_MSG) HTPrint("Can't access rules\n");
	HT_FREE(rules);
    }

    /* Add our own filter to update the history list */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Start the request */
    switch (method) {
    case METHOD_GET:

	if (formdata)
	    status = HTGetFormAnchor(formfields, (HTAnchor *) cl->anchor,
				     cl->request);
	else if (keywords)
	    status = HTSearchAnchor(keywords, (HTAnchor *) cl->anchor,
				    cl->request);
	else
	    status = HTLoadAnchor((HTAnchor *) cl->anchor, cl->request);
	break;

    case METHOD_HEAD:
	if (formdata) {
	    HTRequest_setMethod(cl->request, METHOD_HEAD);
	    status = HTGetFormAnchor(formfields, (HTAnchor *) cl->anchor,
				     cl->request);
	} else if (keywords) {
	    HTRequest_setMethod(cl->request, METHOD_HEAD);
	    status = HTSearchAnchor(keywords, (HTAnchor *) cl->anchor,
				    cl->request);
	} else
	    status = HTHeadAnchor((HTAnchor *) cl->anchor, cl->request);
	break;

    case METHOD_DELETE:
	status = HTDeleteAnchor((HTAnchor *) cl->anchor, cl->request);
	break;

    case METHOD_POST:
	if (formdata) {
	    HTParentAnchor * posted = NULL;
#if 1
	    posted = HTPostFormAnchor(formfields, (HTAnchor *) cl->anchor,
				      cl->request);
	    status = posted ? YES : NO;
#else
	    /* If we want output to a chunk instead */
	    post_result = HTPostFormAnchorToChunk(formfields, (HTAnchor *) cl->anchor,
						  cl->request);
	    status = post_result ? YES : NO;
#endif
	} else {
	    if (SHOW_MSG) HTPrint("Nothing to post to this address\n");
	    status = NO;	    
	}
	break;

    case METHOD_PUT:
	status = HTPutDocumentAnchor(cl->anchor, (HTAnchor *) cl->dest,
				     cl->request);
	break;

    case METHOD_OPTIONS:
	status = HTOptionsAnchor((HTAnchor *) cl->anchor, cl->request);
	break;	

    case METHOD_TRACE:
	status = HTTraceAnchor((HTAnchor *) cl->anchor, cl->request);
	break;	

    default:
	if (SHOW_MSG) HTPrint("Don't know this method\n");
	break;
    }

    if (keywords) HTChunk_delete(keywords);
    if (formfields) HTAssocList_delete(formfields);
    if (status != YES) {
	if (SHOW_MSG) HTPrint("Sorry, can't access resource\n");
	Cleanup(cl, -1);
    }

    /* Go into the event loop... */
    HTEventList_loop(cl->request);

    /* Only gets here if event loop fails */
    Cleanup(cl, 0);
    return 0;
}
