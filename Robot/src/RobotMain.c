/*
**	@(#) $Id$
**	
**	W3C Webbot can be found at "http://www.w3.org/Robot/"
**	
**	Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**  Authors:
**	HFN		Henrik Frystyk Nielsen, (frystyk@w3.org)
**	BR		Bob Racko
**	JP		John Punin
**
**  History:
**	Dec 04 95	First version
**	Oct 1998	Split into separate files
*/

#include "HTRobMan.h"
#include "RobotTxt.h"

#define SHOW_QUIET(mr)		((mr) && !((mr)->flags & MR_QUIET))
#define SHOW_REAL_QUIET(mr)	((mr) && !((mr)->flags & MR_REAL_QUIET))

/* ------------------------------------------------------------------------- */
/*				  MAIN PROGRAM				     */
/* ------------------------------------------------------------------------- */

PRIVATE int printer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stdout, fmt, pArgs));
}

PRIVATE int tracer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}

int main (int argc, char ** argv)
{
    int		status = 0;
    int		arg;
    BOOL	cache = NO;			     /* Use persistent cache */
    BOOL	flush = NO;		       /* flush the persistent cache */
    char *	cache_root = NULL;
    int		cache_size = DEFAULT_CACHE_SIZE;
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
#endif /* __MWERKS__ */

#ifdef HT_MEMLOG
    HTMemLog_open(DEFAULT_MEMLOG, 8192, YES);
#endif

    /* Initiate W3C Reference Library with a robot profile */
    HTProfile_newRobot(APP_NAME, APP_VERSION);

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Build a new robot object */
    mr = Robot_new();

    /* Scan command Line for parameters */
    for (arg=1; arg<argc; arg++) {
	if (*argv[arg] == '-') {
	    
	    /* non-interactive */
	    if (!strcmp(argv[arg], "-n")) {
		HTAlert_setInteractive(NO);

  	    /* help */
	    } else if (!strcmp(argv[arg], "-h") || !strcmp(argv[arg], "-?")) {
		VersionInfo();
		Cleanup(mr, 0);

  	    /* clf log file */
	    } else if (!strcmp(argv[arg], "-l")) {
		mr->logfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_LOG_FILE;
		mr->flags |= MR_LOGGING;

  	    /* referer log file */
	    } else if (!strncmp(argv[arg], "-ref", 4)) {
		mr->reffile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_REFERER_FILE;
		mr->flags |= MR_LOGGING;

  	    /* Not found error log file */
	    } else if (!strncmp(argv[arg], "-404", 4)) {
		mr->notfoundfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_NOTFOUND_FILE;
		mr->flags |= MR_LOGGING;

  	    /* reject log file */
	    } else if (!strncmp(argv[arg], "-rej", 4)) {
		mr->rejectfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_REJECT_FILE;
		mr->flags |= MR_LOGGING;

  	    /* no alt tags log file */
	    } else if (!strncmp(argv[arg], "-alt", 4)) {
		mr->noalttagfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_NOALTTAG_FILE;
		mr->flags |= MR_LOGGING;

  	    /* negotiated resource log file */
	    } else if (!strncmp(argv[arg], "-neg", 4)) {
		mr->connegfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_CONNEG_FILE;
		mr->flags |= MR_LOGGING;

  	    /* hit file log */
	    } else if (!strcmp(argv[arg], "-hit")) {
		mr->hitfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_HIT_FILE;
		mr->flags |= MR_DISTRIBUTIONS;

  	    /* link relations file log */
	    } else if (!strcmp(argv[arg], "-rellog")) {
		mr->relfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_REL_FILE;
		mr->flags |= MR_DISTRIBUTIONS;

  	    /* Specific link relation to look for (only used i also -rellog) */
	    } else if (!strcmp(argv[arg], "-relation")) {
		mr->relation = (arg+1 < argc && *argv[arg+1] != '-') ?
		    (HTLinkType) HTAtom_caseFor(argv[++arg]) : NULL;
		mr->flags |= MR_DISTRIBUTIONS;

  	    /* last modified log file */
	    } else if (!strcmp(argv[arg], "-lm")) {
		mr->lmfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_LM_FILE;
		mr->flags |= (MR_KEEP_META | MR_DISTRIBUTIONS);

  	    /* title log file */
	    } else if (!strcmp(argv[arg], "-title")) {
		mr->titlefile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_TITLE_FILE;
		mr->flags |= (MR_KEEP_META | MR_DISTRIBUTIONS);

  	    /* mediatype distribution log file */
	    } else if (!strncmp(argv[arg], "-for", 4)) {
		mr->mtfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_FORMAT_FILE;
		mr->flags |= (MR_KEEP_META | MR_DISTRIBUTIONS);

  	    /* charset distribution log file */
	    } else if (!strncmp(argv[arg], "-char", 5)) {
		mr->charsetfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_CHARSET_FILE;
		mr->flags |= (MR_KEEP_META | MR_DISTRIBUTIONS);
		

            /* rule file */
	    } else if (!strcmp(argv[arg], "-r")) {
		mr->rules = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_RULE_FILE;

	    /* Don't follow HTML META tags with robot information */
	    } else if (!strcmp(argv[arg], "-nometatags")) {
		mr->flags |= MR_NOMETATAGS;

	    /* output filename */
	    } else if (!strcmp(argv[arg], "-o")) { 
		mr->outputfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_OUTPUT_FILE;

	    /* URI prefix */
	    } else if (!strcmp(argv[arg], "-prefix")) {
		char * prefix = NULL;
		prefix = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_PREFIX;
		if (*prefix && *prefix != '*') {
		    StrAllocCopy(mr->prefix, prefix);
		    StrAllocCat(mr->prefix, "*");
		}

	    /* timeout -- Change the default request timeout */
	    } else if (!strcmp(argv[arg], "-timeout")) {
		int timeout = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_TIMEOUT;
		if (timeout > 1) mr->timer = timeout*MILLIES;

	    /* wait -- Change the default pwait time */ /* This is new */
	    } else if (!strcmp(argv[arg], "-wait")) {
		int waits = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : 0;
		if (waits > 0) mr->waits = waits;

	    /* Force no pipelined requests */
	    } else if (!strcmp(argv[arg], "-nopipe")) {
		HTTP_setConnectionMode(HTTP_11_NO_PIPELINING);

	    /* Stream write flush delay in ms */
	    } else if (!strcmp(argv[arg], "-delay")) {
		int delay = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_DELAY;
		HTHost_setDefaultWriteDelay(delay);

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
		mr->flags |= MR_VALIDATE;

	    } else if (!strcmp(argv[arg], "-cache_size")) {
		cache_size = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_CACHE_SIZE;

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

	    /* URI prefix for inlined images */
	    } else if (!strcmp(argv[arg], "-imgprefix")) {
		char * prefix = NULL;
		prefix = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_IMG_PREFIX;
		if (*prefix && *prefix!='*') {
		    StrAllocCopy(mr->img_prefix, prefix);
		    StrAllocCat(mr->img_prefix, "*");
		}

	    /* load anchors */
	    } else if (!strcmp(argv[arg], "-link") || !strcmp(argv[arg], "-depth")) {
		mr->flags |= MR_LINK;
		mr->depth = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_DEPTH;

	    /* load fixed number of anchors */
	    } else if (!strcmp(argv[arg], "-ndoc")) {
		mr->ndoc = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : -1 ;

	    /* Output start and end time */
	    } else if (!strcmp(argv[arg], "-ss")) {
		mr->flags |= MR_TIME;

	    /* print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
		VersionInfo();
		Cleanup(mr, 0);
		
	    /* run in BFS mode */
	    } else if (!strcmp(argv[arg], "-bfs")) { 
		mr->flags |= MR_BFS;

	    /* run in quiet mode */
	    } else if (!strcmp(argv[arg], "-q")) { 
		mr->flags |= MR_QUIET;

	    /* run in really quiet mode */
	    } else if (!strcmp(argv[arg], "-Q")) { 
		mr->flags |= MR_REAL_QUIET;

	    /* run in redirection mode */
	    } else if (!strcmp(argv[arg], "-redir")) { 
		mr->flags |= MR_REDIR;
		mr->redir_code = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : 0;

#ifdef HT_POSIX_REGEX

	    /* If we can link against a POSIX regex library */
	    } else if (!strncmp(argv[arg], "-inc", 4)) {
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    mr->include = get_regtype(mr, argv[++arg], W3C_DEFAULT_REGEX_FLAGS);
		}
	    } else if (!strncmp(argv[arg], "-exc", 4)) {
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    mr->exclude = get_regtype(mr, argv[++arg], W3C_DEFAULT_REGEX_FLAGS);
		}
	    } else if (!strncmp(argv[arg], "-check", 6)) {
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    mr->check = get_regtype(mr, argv[++arg], W3C_DEFAULT_REGEX_FLAGS);
		}
	    } else if (!strcmp(argv[arg], "-norobotstxt")) {
	      mr->flags |= MR_NOROBOTSTXT;
#endif

#ifdef HT_MYSQL
	    /* If we can link against a MYSQL database library */
	    } else if (!strncmp(argv[arg], "-sqldb", 5)) {
		mr->sqldb = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_DB;

	    } else if (!strncmp(argv[arg], "-sqlclearlinks", 10)) {
		mr->sqlflags |= HTSQLLOG_CLEAR_LINKS_TABLE;

	    } else if (!strncmp(argv[arg], "-sqlclearrequests", 12)) {
		mr->sqlflags |= HTSQLLOG_CLEAR_REQUESTS_TABLE;

	    } else if (!strncmp(argv[arg], "-sqlclearresources", 12)) {
		mr->sqlflags |= HTSQLLOG_CLEAR_RESOURCES_TABLE;

	    } else if (!strncmp(argv[arg], "-sqlclearuris", 10)) {
		mr->sqlflags |= HTSQLLOG_CLEAR_URIS_TABLE;

	    } else if (!strncmp(argv[arg], "-sqlexternals", 5)) {
		mr->sqlexternals = YES;

	    } else if (!strncmp(argv[arg], "-sqlpassword", 5)) {
		mr->sqlpw = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_PW;

	    } else if (!strncmp(argv[arg], "-sqlrelative", 5)) {
		mr->sqlrelative = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : NULL;

	    } else if (!strncmp(argv[arg], "-sqlserver", 5)) {
		mr->sqlserver = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_SERVER;

	    } else if (!strncmp(argv[arg], "-sqluser", 5)) {
		mr->sqluser = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_USER;

#endif

#ifdef HT_SSL
            } else if (!strncmp(argv[arg], "-verifydepth", 12)) {
               mr->sslverifydepth = (arg+1 < argc && *argv[arg+1] != '-') ?
                    atoi(argv[++arg]) : DEFAULT_SSL_VDEPTH;
            } else if (!strncmp(argv[arg], "-sslprot", 8)) {
               if (arg+1 < argc && *argv[arg+1] != '-') {
                  if (!strcmp(argv[arg], "v2")) {
                      mr->sslprot = HTSSL_V2;
                  } else if (!strcmp(argv[arg], "v3")) {
                      mr->sslprot = HTSSL_V3;
                  } else if (!strcmp(argv[arg], "v23")) {
                      mr->sslprot = HTSSL_V23;
                  } else {
                      mr->sslprot = DEFAULT_SSL_PROT;
                  }
               }
            } else if (!strncmp(argv[arg], "-keyfile", 8)) {
                mr->sslkeyfile = (arg+1 < argc && *argv[arg+1] != '-') ?
                    argv[++arg] : DEFAULT_SSL_KFILE;
            } else if (!strncmp(argv[arg], "-certfile", 9)) {
                mr->sslcertfile = (arg+1 < argc && *argv[arg+1] != '-') ?
                    argv[++arg] : DEFAULT_SSL_CFILE;

#endif

#ifdef WWWTRACE
	    /* trace flags */
	    } else if (!strncmp(argv[arg], "-v", 2)) {
		HTSetTraceMessageMask(argv[arg]+2);
#endif


	    } else {
		if (SHOW_REAL_QUIET(mr)) HTPrint("Bad Argument (%s)\n", argv[arg]);
	    }
       } else {	 /* If no leading `-' then check for URL or keywords */
    	    if (!keycnt) {
	      HyperDoc *hd; /* This is new variable */
		mr->furl = HTParse(argv[arg], mr->cwd, PARSE_ALL);
		startAnchor = HTAnchor_parent(HTAnchor_findAddress(mr->furl));
		hd = HyperDoc_new(mr, startAnchor, 0);
		hd->method = METHOD_GET;
		keycnt = 1;
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

    if (!keycnt) {
	VersionInfo();
	Cleanup(mr, 0);
    }
#ifdef HT_SSL
    /* Set the SSL protocol method. By default, it is the highest
       available protocol. Setting it up to SSL_V23 allows the client
       to negotiate with the server and set up either TSLv1, SSLv3,
       or SSLv2 */
    HTSSL_protMethod_set (mr->sslprot ? mr->sslprot : DEFAULT_SSL_PROT);

    /* Set the certificate verification depth to 2 in order to be
       able to
       validate self signed certificates */
    HTSSL_verifyDepth_set (mr->sslverifydepth ? 
			   mr->sslverifydepth : 
			   DEFAULT_SSL_VDEPTH);
 
    /* Setup cert stuff */
    if (mr->sslcertfile) {
        HTSSL_certFile_set(mr->sslcertfile);
        HTPrint("Setting certfile %s\n", HTSSL_certFile());
    }
    /* Setup key stuff */
    if (mr->sslkeyfile) {
        HTSSL_keyFile_set(mr->sslkeyfile);
        HTPrint("Setting keyfile %s\n", HTSSL_keyFile());
    }

    /* Register SSL stuff for handling ssl access */
    HTSSLhttps_init(YES);
#endif /* HT_SSL */

    if (mr->depth != DEFAULT_DEPTH && 
	(mr->prefix == NULL || *mr->prefix == '*')) {
	if (SHOW_REAL_QUIET(mr))
	    HTPrint("A depth of more than 0 requires that you also specify a URI prefix.\n",
		    mr->depth);
	Cleanup(mr, -1);
    }

    /* Testing that HTPrint is working */
    if (mr->flags & MR_TIME) {
	if (SHOW_REAL_QUIET(mr)) {
	    time_t local = time(NULL);
	    HTPrint("Welcome to the W3C mini Robot version %s - started on %s\n",
		    APP_VERSION, HTDateTimeStr(&local, YES));
	}
    }

    /* Rule file specified? */
    if (mr->rules) {
	char * rules = HTParse(mr->rules, mr->cwd, PARSE_ALL);
	if (!HTLoadRulesAutomatically(rules))
	    if (SHOW_REAL_QUIET(mr)) HTPrint("Can't access rules\n");
	HT_FREE(rules);
    }

    /* Output file specified? */
    if (mr->outputfile) {
	if ((mr->output = fopen(mr->outputfile, "wb")) == NULL) {
	    if (SHOW_REAL_QUIET(mr)) HTPrint("Can't open `%s'\n", mr->outputfile);
	    mr->output = OUTPUT;
	}
    }

    /* This is new */
    if ((mr->cdepth = (int *) HT_CALLOC(mr->depth+2, sizeof(int)))==NULL)
	HT_OUTOFMEM("main");

    /* Should we use persistent cache? */
    if (cache) {
	HTCacheInit(cache_root, cache_size);

	/* Should we start by flushing? */
	if (flush) HTCache_flushAll();
    }

    /* SQL Log specified? */
#ifdef HT_MYSQL
    if (mr->sqlserver) {
	if ((mr->sqllog =
	     HTSQLLog_open(mr->sqlserver,
			   mr->sqluser ? mr->sqluser : DEFAULT_SQL_USER,
			   mr->sqlpw ? mr->sqlpw : DEFAULT_SQL_PW,
			   mr->sqldb ? mr->sqldb : DEFAULT_SQL_DB,
			   mr->sqlflags)) != NULL) {
	    if (mr->sqlrelative) HTSQLLog_makeRelativeTo(mr->sqllog, mr->sqlrelative);
	}
    }
#endif

    /* CLF Log file specified? */
    if (mr->logfile) {
        mr->log = HTLog_open(mr->logfile, YES, YES);
        if (mr->log) HTNet_addAfter(HTLogFilter, NULL, mr->log, HT_ALL, HT_FILTER_LATE);
    }

    /* Referer Log file specified? */
    if (mr->reffile) {
        mr->ref = HTLog_open(mr->reffile, YES, YES);
        if (mr->ref)
	    HTNet_addAfter(HTRefererFilter, NULL, mr->ref, HT_ALL, HT_FILTER_LATE);
    }

    /* Not found error log specified? */
    if (mr->notfoundfile) {
        mr->notfound = HTLog_open(mr->notfoundfile, YES, YES);
        if (mr->notfound)
	    HTNet_addAfter(HTRefererFilter, NULL, mr->notfound, -404, HT_FILTER_LATE);
    }

    /* Check that the redirection code is valid */
    if (mr->flags & MR_REDIR) {
	BOOL isredir = NO;
	if (mr->redir_code == HT_PERM_REDIRECT || mr->redir_code == 0) {
	    HTNet_addAfter(redirection_handler, "http://*" , NULL, HT_PERM_REDIRECT, HT_FILTER_LATE);
	    isredir = YES;
	}
	if (mr->redir_code == HT_TEMP_REDIRECT || mr->redir_code == 0) {
	    HTNet_addAfter(redirection_handler, "http://*", NULL, HT_TEMP_REDIRECT, HT_FILTER_LATE);
	    isredir = YES;
	}
	if (mr->redir_code == HT_FOUND || mr->redir_code == 0) {
	    HTNet_addAfter(redirection_handler, "http://*", NULL, HT_FOUND, HT_FILTER_LATE);
	    isredir = YES;
	}
	if (mr->redir_code == HT_SEE_OTHER || mr->redir_code == 0) {
	    HTNet_addAfter(redirection_handler, "http://*", NULL, HT_SEE_OTHER, HT_FILTER_LATE);
	    isredir = YES;
	}
	if (!isredir) {
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("%d is not a valid redirection code\n", mr->redir_code);
	    Cleanup(mr, -1);
	}
    }

    /* Negotiated resource log specified? */
    if (mr->connegfile) mr->conneg = HTLog_open(mr->connegfile, YES, YES);

    /* No alt tags log file specified? */
    if (mr->noalttagfile) mr->noalttag = HTLog_open(mr->noalttagfile, YES, YES);

    /* Reject Log file specified? */
    if (mr->rejectfile) mr->reject = HTLog_open(mr->rejectfile, YES, YES);

#ifdef HT_POSIX_REGEX
    if(!(mr->flags & MR_NOROBOTSTXT))
      {
      char *ruri = HTParse(ROBOTS_TXT, mr->furl, PARSE_ALL);
      char *robot_str = get_robots_txt(ruri);
      char *reg_exp_robot = robot_str ? 
	scan_robots_txt(robot_str,APP_NAME) : NULL;
      if (SHOW_REAL_QUIET(mr)) HTPrint("robots.txt uri is `%s'\n", ruri);
      if(robot_str)
	  HT_FREE(robot_str);
      if(reg_exp_robot)
	{
	  mr->exc_robot = get_regtype(mr, reg_exp_robot, W3C_DEFAULT_REGEX_FLAGS);
	  HT_FREE(reg_exp_robot);
	}
      HT_FREE(ruri);
    }
#endif

    /* Add our own HTML HText functions */
    Robot_registerHTMLParser();

    /* Register our own terminate filter */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* If doing breath first search */
    if (mr->flags & MR_BFS)
	HTNet_addAfter(bfs_terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Setting event timeout */
    HTHost_setEventTimeout(mr->timer);

    mr->time = HTGetTimeInMillis();

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
	if (SHOW_REAL_QUIET(mr)) HTPrint("Can't access resource\n");
	Cleanup(mr, -1);
    }

    /* Go into the event loop... */

    if((mr->flags & MR_PREEMPTIVE) && (mr->flags & MR_BFS))
      Serving_queue(mr);
    else
      HTEventList_loop(finger->request);


    /* Only gets here if event loop fails */
    Cleanup(mr, 0);
    return 0;
}
