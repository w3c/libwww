/*    HyperText Browser for Dumb Terminals     		     HTBrowse.c
**    ====================================
**
**  Authors:
**	NP:  Nicola Pellow  Tech.Student CERN 1990-91
**	TBL: Tim Berners-Lee CERN (timbl@info.cern.ch)
**	JFG: Jean-Francois Groff, Cooperant CERN 1991-92 (jfg@info.cern.ch)
**	DR:  Dudu Rashty +972-2-584848 <RASHTY@hujivms.bitnet>
**	MD:  Mark Donszelmann, DELPHI CERN, (duns@vxdeop.cern.ch)
**	HFN: Henrik Frystyk Nielsen, CERN, (frystyk@dxcern.cern.ch)
**
**  Copyright CERN 1990-1992   See Copyright.html 
**
**  History:
**
**   4 Dec 90:  Written from scratch (NP)
**  11 Feb 91:  Code written by TBL so that the browser could be linked with
**              code produced by Bernd Pollermann, enabling access to the
**              data on CERNVM. This involved changing the code to handle file
**              numbers rather than file pointers.
**  18 Mar 91:  The feature of history mechanism was included, enabling a  
**              record of previous nodes visited to be kept.
**   6 Apr 91:  When a node is accessed, it is immediately read into a 
**              buffer, in an unformatted state, as soon as the connection is   
**              made, so that the server is freed as quickly as possible. 
**              The program now also uses the additional modules HTBufferFile.c
**              and HTBufferFile.h.
**  17 Apr 91:  Can be used on machines running ANSI C and ordinary C.
**  10 May 91:  Formatted text is stored in a linked list buffer which allows
**              scrolling and better page breaks in the middle of text.
**              Code incorporated by Tim BL, to enable anonymous FTP.          
**  21 May 91:  Accepts various parameters on the command line.
**  19 Aug 91:  Currently available in Unix, VAX/VMS and MVS environments.
**  21 Nov 91:	 Character grid uses new architecture. (TBL)
**              added -w option, new commands, print,
**	             ...	See Features.html for further details
**  16 Jan 92:	 Put in VIOLA-compatible options - see \017 characters.
**	             \017 and \016 bracket user-selectable input.
**  27 Feb 92:	New handling of user input, enhanced command syntax. (JFG)
**  18 May 92:	PS command see ifdef SLAVE_PRINTER (DR/TBL)
**   6 Oct 92:  Painful recovery from someone(?)'s attept to pretty print.(TBL)
**		Please see coding style guide before changing indentation etc!
**     Mar 93:	Force on HTFile's HTDirAccess and HTDirReadme flags.
**   3 Nov 93:	(MD) Changed vms into VMS
**		(MD) Assigne output in main, not at initialize (VMS only)
**
** Compilation-time macro options
**
**	REF_MARK	Printf string to be used for printing anchor numbers
**	END_MARK	String to be used to denote the end of a document
**	VL		Version number, quoted eg "1.2a"
*/
#ifndef VL
#define VL "unspecified"
#endif

/*	If the guy gives the "MANUAL" command, jump to this: */
#ifndef MANUAL
#define MANUAL "http://info.cern.ch/hypertext/WWW/LineMode/Defaults/QuickGuide.html"
#endif

#ifndef COM_HELP_FILE
#define COM_HELP_FILE "http://info.cern.ch/hypertext/WWW/LineMode/Defaults/CommandLine.html"
#endif

#ifndef DEFAULT_LOCAL_LOGFILE
#define DEFAULT_LOCAL_LOGFILE		"WWW-log"	/* Log file name for local execution */
#endif

#ifndef DEFAULT_OUTPUT_FILE
#define DEFAULT_OUTPUT_FILE		"WWW-out"	/* Output file name for non-interactive run */
#endif

#ifndef DEFAULT_REF_HEAD
#define DEFAULT_REF_HEAD		"*** References from this document ***"
#endif

/* Check Statements */
/* ================ */

#define NAME_CHECK	0	/* Trace to show NAME anchors */
/* #define LONG_PROMPT	1 */	/* Long or short user prompt */


/* Include Files */
/* ============= */

#include <ctype.h>
#include "HTUtils.h"	/* WWW general purpose macros */

#include "HTBrowse.h"	/* Things exported, short names */

#include "GridText.h"	/* Hypertext definition */

#include "HTFormat.h"
#include "HTTCP.h"	/* TCP/IP utilities */
#include "HTAnchor.h"   /* Anchor class */
#include "HTParse.h"    /* WWW address manipulation */
#include "HTAccess.h"   /* WWW document access network code */
#include "HTHistory.h"	/* Navigational aids */
#include "HTML.h"	/* For parser */
#include "HTFWriter.h"	/* For non-interactive output */
#include "HTMLGen.h"	/* For reformatting HTML */
#include "HTFile.h"	/* For Dir access flags */
#include "HTRules.h"    /* For loading rule file */
#include "HTError.h"

#ifdef THINK_C					 /* Macintosh Think C development system */
#include <console.h>
#include <time.h>
	extern int socketdebug;  /* Must be declared in the socket library */
#endif

extern HTStyleSheet * styleSheet;

/* Define Statements */
/* ================= */

#ifndef REF_MARK		/* May be redefined on command line */
#ifdef VIOLA
#define REF_MARK "[\017%d\016]"	/* Shift-in, shift-out around number */
#define PROMPT_MARK "\017%s\016"
#else
#define PROMPT_MARK "%s"
#ifdef VM
#define REF_MARK " <%d>"	/* IBM terminals can't handle [] well */
#else
#define REF_MARK "[%d]"
#endif
#endif
#endif

#ifndef END_MARK
#ifdef VM
#define END_MARK "     <End>"
#else
#define END_MARK "     [End]"
#endif
#endif

#ifndef EOF
#define EOF (-1)  /* End of file character defined as -1 */
#endif


#ifdef NEWLIB
#define SCREEN_WIDTH		78
#endif

#ifndef SCREEN_WIDTH      
#define SCREEN_WIDTH		79  /* Default width of the screen */ 
#endif

#ifndef MIN_SCREEN_WIDTH
#define MIN_SCREEN_WIDTH	10 
#endif

#ifndef MAX_SCREEN_WIDTH
#define MAX_SCREEN_WIDTH	150	 
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT		24 /* Default number of lines to the screen */
#endif

#ifndef MIN_SCREEN_HEIGHT
#define MIN_SCREEN_HEIGHT	5 
#endif

#ifndef MAX_SCREEN_HEIGHT
#define MAX_SCREEN_HEIGHT	200	 
#endif

#ifdef VM   			/* Needed to flush out the prompt line..*/
#ifndef NEWLIB		/* except on NEWLIB which can wrap. */
#define NEWLINE_PROMPT  /* before input */
#endif
#endif

#define INFINITY		1024		                     /*!! BUG*/
#define ADDRESS_LENGTH		INFINITY   /* Maximum address length of node */
#define TITLE_LENGTH		INFINITY	/* Maximum length of a title */
#define RESPONSE_LENGTH		INFINITY /* Maximum length of users response */

#ifndef MAXPATHLEN
#define NO_GETWD                      /* Assume no  getwd() if no MAXPATHLEN */
#endif

/*	Public Variables
**	================
*/

PUBLIC char * HTAppName = "CERN-LineMode";	         /* Application name */
PUBLIC char * HTAppVersion = VL; 	              /* Application version */
PUBLIC  int   HTScreenWidth   = SCREEN_WIDTH;		       /* By default */
PUBLIC  int   HTScreenHeight  = -1;	         		/* Undefined */
PUBLIC BOOL   display_anchors = YES;	    /* anchor will be shown in text? */

#ifdef OLD_CODE
#ifdef VMS
PUBLIC  BOOL interactive     = YES;        /*  e.g. shows prompts etc */
#else /* not VMS */
PRIVATE  BOOL interactive     = YES;        /*  e.g. shows prompts etc */
#endif /* not VMS */
#endif /* OLD_CODE */
					   
PUBLIC char * start_reference = NULL;      /* Format string for start anchor */
PUBLIC char * end_reference = REF_MARK;     		   /* for end anchor */
PUBLIC char * reference_mark = "[%d] ";     	      /* for reference lists */
PUBLIC char * end_mark = END_MARK;          	  /* Format string for [End] */

/* Moved into other files: */

/* PUBLIC char * HTClientHost = 0;	  HTAccess */
/* PUBLIC  int  WWW_TraceFlag   = 0;      HTString */
/* PUBLIC FILE * logfile = 0;		  HTAccess */
 
 /*	Private variables
 **	=================
 */
/* Arrays for storing the HyperText References */

PRIVATE HTParentAnchor * home_anchor = NULL;	    /* First document anchor */
PRIVATE char keywords[ADDRESS_LENGTH];     /* Search terms from command line */
PRIVATE char * output_file_name = NULL;                          /* -o xxxx */
PRIVATE char        choice[RESPONSE_LENGTH];    /* Users response  */
PRIVATE char * refhead = DEFAULT_REF_HEAD;  	   /* Reference list heading */
PRIVATE char *	     logfile_root = 0;	      	/* Log file name */
PRIVATE BOOL	     filter=0;		     	/* Load from stdin? */
PRIVATE BOOL	     reformat_html=0;		 /* Reformat html? */
PRIVATE BOOL	     listrefs_option = 0;	/* -listrefs option used?  */
PRIVATE HTRequest * request;
PRIVATE BOOL  OutSource = NO;			    /* Output source, YES/NO */
PRIVATE char *HTLogFileName = 0;	      	    /* Root of log file name */


#ifdef VMS
PRIVATE FILE *       output;		/* assignment done in main */
#ifdef __DECC
/* dummy declarations to make sure that LINKER will not complain */
PUBLIC char *HTBinDir;
PUBLIC int HTDiag;
PUBLIC char *HTPostScript;
PUBLIC char *HTPutScript;
PUBLIC char *HTSearchScript;
#endif /* DECC */
#else /* not VMS */
PRIVATE FILE *	     output = stdout;
#endif /* not VMS */ 

/* Forward Declaration of Functions */
/* ================================ */

PRIVATE void History_List NOPARAMS; 
PRIVATE BOOL Selection_Prompt NOPARAMS;
PRIVATE void Error_Selection NOPARAMS;
PRIVATE void help_screen NOPARAMS;
PRIVATE void Reference_List PARAMS((BOOL titles));
PRIVATE void ErrMsg PARAMS((char *Msg, char *Str));
PRIVATE BOOL SaveOutputStream PARAMS((char *This, char *Next));


#ifdef ultrix
#define GET_SCREEN_SIZE
#endif

#ifdef __osf__
#define GET_SCREEN_SIZE
#endif

#ifdef GET_SCREEN_SIZE
#include <sys/ioctl.h>

/*
 * Get size of the output screen. Stolen from less.
 */
void
scrsize(p_height, p_width)
      int *p_height;
      int *p_width;
{
      register char *s;
      int ioctl();
      struct winsize w;

      if (ioctl(2, TIOCGWINSZ, &w) == 0 && w.ws_row > 0)
              *p_height = w.ws_row;
      else
      if ((s = getenv("LINES")) != NULL)
              *p_height = atoi(s);
      else
              *p_height = SCREEN_HEIGHT;

      if (ioctl(2, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
              *p_width = w.ws_col;
      else
      if ((s = getenv("COLUMNS")) != NULL)
              *p_width = atoi(s);
      else
              *p_width = 80;
}
#endif /* GET_SCREEN_SIZE, BSN */


/* MAIN PROGRAM
** ------------
*/

int main
#ifdef __STDC__
		(int argc, char * argv[])
#else
		(argc, argv)
			int argc;
			char * argv[];
#endif

{
    int  return_status = 0;	
    int  arg;		         /* Argument number as we scan */
    BOOL argument_found = NO;
    BOOL logfile_flag = NO;
    BOOL interactive = YES;		      /* Default is interactive mode */
    BOOL first_keyword = YES;
    char* default_default=0;	 /* Parse home relative to this */
    HTFormat input_format = WWW_HTML;	                 /* Used with filter */

#if defined(ultrix) || defined(__osf__)
    int scr_height, scr_width;
#endif /* ultrix and OSF/1 */

#ifdef THINK_C /* command line from Think_C */
    int i;
    argc=ccommand(&argv);
#endif

    request =  HTRequest_new();
    /* request->conversions = HTList_new(); Done by HTRequest_new() Henrik 18/02-94 */
	 
#ifdef VMS
    output = stdout;
#endif /* VMS */
	
    StrAllocCopy(default_default, "file://");
    StrAllocCat(default_default, HTGetHostName()); /*eg file://dxcern.cern.ch*/

#ifdef GET_SCREEN_SIZE
    scrsize(&scr_height, &scr_width);
    HTScreenHeight = scr_height;
#endif /* ultrix or OSF/1, BSN */

#ifndef MAXPATHLEN
#define NO_GETWD		/* Assume no  getwd() if no MAXPATHLEN */
#endif

#ifdef NO_GETWD  		/* No getwd() on this machine */
#ifdef HAS_GETCWD		/* System V variant SIGN CHANGED TBL 921006 !! */

    {
	char wd[1024];			/*!! Arbitrary*/
	char * result = getcwd(wd, sizeof(wd)); 
	if (result) {

#ifdef VMS 
            /* convert directory name to Unix-style syntax */
	    char * disk = strchr (wd, ':');
	    char * dir = strchr (wd, '[');
	    if (disk) {
	        *disk = '\0';
		StrAllocCat (default_default, "/");  /* needs delimiter */
		StrAllocCat (default_default, wd);
	    }
	    if (dir) {
		char *p;
		*dir = '/';  /* Convert leading '[' */
		for (p = dir ; *p != ']'; ++p)
			if (*p == '.') *p = '/';
		*p = '\0';  /* Cut on final ']' */
		StrAllocCat (default_default, dir);
	    }
#else  /* not VMS */
	    StrAllocCat (default_default, wd);
#endif  /* not VMS */
	    } else {
	        ErrMsg("Can't read working directory (getcwd)", NULL);
	    }
	}  /* end if good getcwd result */
	
#else   /* has NO getcwd */

	ErrMsg("This platform does not support getwd() or getcwd()", NULL);

#endif	/* has no getcwd */

#else   /* has getwd */
	{
      	    char wd[MAXPATHLEN];
      	    char * result = (char *) getwd(wd);
	    if (result) {
	        StrAllocCat(default_default, wd);
	    } else {
	        ErrMsg("Can't read working directory.", NULL);
	    }
	}
#endif
		
#ifdef VMS  
      StrAllocCat(default_default, "default.html");
#else
      StrAllocCat(default_default, "/default.html");
#endif

/*	Check for command line options
**	------------------------------
*/
    keywords[0] = 0;				/* Clear string */
    for (arg=1; arg<argc ; arg++) {
	if (*argv[arg] == '-') {

	    /* - alone => filter */
	    if (argv[arg][1] == 0) {
		filter = YES;	   
		interactive = NO;     /* From stdin, Force non-interactive */
	    
	    /* -? or -help: show the command line help page */
	    } else if (!strcmp(argv[arg], "-?") ||
		       !strcmp(argv[arg], "-help")) {
		home_anchor = (HTParentAnchor *)
		    HTAnchor_findAddress(COM_HELP_FILE);
		argument_found = YES;	    /* Don't try to find other pages */

	    /* from -- Initial represntation (only with filter) */
	    } else if (!strcmp(argv[arg], "-from")) {
		input_format = (arg+1 >= argc || *argv[arg+1] == '-') ?
		    WWW_HTML : HTAtom_for(argv[++arg]);

#ifdef TRACE
	   /* Verify: Turns on trace */
	    } else if (!strcmp(argv[arg], "-v")) {
		WWW_TraceFlag = 1;
#endif
	    
	    /* Page size */
	    } else if (!strncmp(argv[arg], "-p", 2)) {
		if (*(argv[arg]+2)) {
		    if (sscanf(argv[arg]+2, "%d", &HTScreenHeight) < 1)
			HTScreenHeight = -1;		
		    else {
			if(HTScreenHeight < MIN_SCREEN_HEIGHT)
			    HTScreenHeight = MIN_SCREEN_HEIGHT;
			if(HTScreenHeight > MAX_SCREEN_HEIGHT)
			    HTScreenHeight = MAX_SCREEN_HEIGHT;
		    }
		} else if (arg+1 < argc && *argv[arg+1] != '-') {
		    if (sscanf(argv[++arg], "%d", &HTScreenHeight) < 1)
			HTScreenHeight = -1;
		    else {
			if(HTScreenHeight < MIN_SCREEN_HEIGHT)
			    HTScreenHeight = MIN_SCREEN_HEIGHT;
			if(HTScreenHeight > MAX_SCREEN_HEIGHT)
			    HTScreenHeight = MAX_SCREEN_HEIGHT;
		    }
		}

	    /* Page width */
	    } else if (!strncmp(argv[arg], "-w", 2)) {
		if (*(argv[arg]+2)) {
		    if (sscanf(argv[arg]+2, "%d", &HTScreenWidth) < 1)
			HTScreenWidth = SCREEN_WIDTH;
		} else if (arg+1 < argc && *argv[arg+1] != '-') {
		    if (sscanf(argv[++arg], "%d", &HTScreenWidth) < 1)
			HTScreenWidth = SCREEN_WIDTH;
		}
		if(HTScreenWidth < MIN_SCREEN_WIDTH)
		    HTScreenWidth = MIN_SCREEN_WIDTH;
		if(HTScreenWidth > MAX_SCREEN_WIDTH)
		    HTScreenWidth = MAX_SCREEN_WIDTH;
		    
	    /* reformat html */
	    } else if (!strcmp(argv[arg], "-reformat")) {
		request->output_format = WWW_HTML;
		    interactive = NO;
		    reformat_html = YES;

	    /* to -- Final represntation */
	    } else if (!strcmp(argv[arg], "-to")) {
		request->output_format =
		    (arg+1 >= argc || *argv[arg+1] == '-') ?
		    	WWW_PRESENT : 
		    	HTAtom_for(argv[++arg]);
		    interactive = NO;			/* JFG */

	    /* Telnet from */
	    } else if (!strcmp(argv[arg], "-h")) {
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    HTClientHost = argv[++arg]; 	    /* Use host name */
		}

	    /* Log file */
	    } else if (!strcmp(argv[arg], "-l")) {
		if (arg+1 < argc && *argv[arg+1] != '-')
		    logfile_root = argv[++arg];
		logfile_flag = YES;
		    
	    /* List References */
	    } else if (!strcmp(argv[arg], "-listrefs")) {
		listrefs_option = YES;
		interactive = NO;			/* non-interactive */

	    /* Non-interactive */
	    } else if (!strcmp(argv[arg], "-n")) {
		interactive = NO;

	    /* Output filename */
	    } else if (!strcmp(argv[arg], "-o")) { 
		output_file_name = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_OUTPUT_FILE;
		    interactive = NO;
		    
	    /* Anchor format */
	    } else if (!strcmp(argv[arg], "-a")) { 
		if (arg+1 < argc && *argv[arg+1] != '-')
		    end_reference = argv[++arg];      /* New representation */

	    /* Anchor format */
	    } else if (!strcmp(argv[arg], "-ar")) { 
		if (arg+1 < argc && *argv[arg+1] != '-')
		    reference_mark = argv[++arg];  /* Change representation */

	    /* Anchor format */
	    } else if (!strcmp(argv[arg], "-as")) { 
		if (arg+1 < argc && *argv[arg+1] != '-')
		    start_reference = argv[++arg]; /* Change representation */

	    /* No anchors */
	    } else if (!strcmp(argv[arg], "-na")) { 
		    display_anchors = NO;

#ifndef NO_RULES
	    } else if (!strcmp(argv[arg], "-r")) {
	        if (arg+1 < argc && *argv[arg+1] != '-') { 
		    if (HTLoadRules(argv[++arg]) < 0) {
			ErrMsg("Can't open rule file", argv[arg]);
			return_status = -1;
			goto endproc;
		    }
		}
#endif
#ifndef NO_DIR_OPTIONS
	    } else if (!strncmp(argv[arg], "-d", 2)) {
	    	char *p = argv[arg]+2;
		for(;*p;p++) {
		    switch (argv[arg][2]) {
		    case 'b':	HTDirReadme = HT_DIR_README_BOTTOM; break;
		    case 'n':	HTDirAccess = HT_DIR_FORBID; break;
		    case 'r':	HTDirReadme = HT_DIR_README_NONE; break;
		    case 's':   HTDirAccess = HT_DIR_SELECTIVE; break;
		    case 't':	HTDirReadme = HT_DIR_README_TOP; break;
		    case 'y':	HTDirAccess = HT_DIR_OK; break;
		    default:
			ErrMsg("HTDaemon: bad -d option", argv[arg]);
			return_status = -4;
		        goto endproc;
		    }
		} /* loop over characters */
#endif
	    /* Reference list heading */
	    } else if (!strcmp(argv[arg], "-refhead")) { 
		if (arg+1 < argc && *argv[arg+1] != '-')
		    refhead = argv[++arg];

	    /* Print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
    		printf("WWW LineMode Browser version %s (WWW Library %s)\n",
			    VL, HTLibraryVersion);
		goto endproc;				

	    /* Source please */
	    } else if (!strcmp(argv[arg], "-source")) {
		    request->output_format = WWW_SOURCE;
		    HTOutputSource = YES;		/* Turn on shortcut */
		    interactive = NO;			/* JFG */

#ifdef THINK_C
	    /* Echo to file */
	    } else if (!strcmp(argv[arg], "-e")){
		struct tm *tm_now; time_t time_now;
		cecho2file("ThinkCconsole",FALSE,stdout);
		time_now=time(NULL);tm_now=localtime(&time_now);
		printf("\n--------------------------------------------"
			"\n Time: %d.%.2d.%.2d %d:%d:%d\n"
			"--------------------------------------------\n",
			(*tm_now).tm_year,
			(*tm_now).tm_mon+1,
			(*tm_now).tm_mday,
			(*tm_now).tm_hour,
			(*tm_now).tm_min,
			(*tm_now).tm_sec);
    
	    /* debug socket library */
	    } else if (!strcmp(argv[arg], "-s")) {
		socketdebug=1;
#endif
	      /* endif long list of argument options */

	    } else {
		ErrMsg("Bad Command Line Argument", argv[arg]);
	    }
	} else {  /* it doesn't start with a dash */

/*      Check for main argument
**      -----------------------
*/
    	    if (!argument_found) {
		char * ref = HTParse(argv[arg], default_default, PARSE_ALL);
		home_anchor = (HTParentAnchor*)HTAnchor_findAddress(ref);
		free(ref);
		argument_found = YES;
		    

/*      Check for successive keyword arguments
**      -------------------------------------
*/
	    } else {   /* Argument already found */
		/* Collect keywords */
		if (first_keyword){
		    first_keyword = NO;
		} else {
		    strcat(keywords, " ");
		}
		strcat(keywords,HTStrip(argv[arg]));

	    } /* argument already found */
	} /* Not an option '-'*/
    } /* End of argument loop */

    if (HTClientHost) HTSecure = YES;
    
    if (HTScreenHeight == -1)		/* Default page size */
	HTScreenHeight = interactive ? SCREEN_HEIGHT : 999999;

#ifndef NO_RULES
    {
    	char * rules = getenv("WWW_CONFIG");
	if (rules && HTLoadRules(rules) < 0) {
	    ErrMsg("Cant't open rule file", rules);
	}
    }
#endif

/* Force predefined presentations etc to be set up. Now it is taking
   care of non-interactive, where no HTSaveLocally() of call backs are used */
    if (interactive)
	HTFormatInit(request->conversions);
    else
	HTFormatInitNIM(request->conversions);

/*	Open output file
**	----------------
*/
    if (!interactive)	{		/* check for file name @@@@@ */
    	if (output_file_name) {
	    FILE * fp = fopen(output_file_name, "w");
	    if (!fp) {
	        ErrMsg("Can't open file for writing", output_file_name);
		return_status = -3;
		goto endproc;
	    }
	    output = fp;
	}
	request->output_stream = HTFWriter_new(output, 
					       output == stdout ? YES : NO);  
	/* Just pump to stdout but YES: leave it open */
	
	/*	To reformat HTML, just put it through a parser running
	**	into a regenerator   tbl 940613 */
	
	if (reformat_html) {
	    request->output_stream = SGML_new(&HTMLP_dtd,
	    			  HTMLGenerator(request->output_stream));

	}
    }
    
    
/* Open Log File. Logfile via Telnet is now optional (HENRIK 11/02-94)
**	--------------------------
*/

/* OLD VERSION
   if (!logfile_root && HTClientHost)
     {
	logfile_root = DEFAULT_LOGFILE;
    }
*/
    if (logfile_flag) {
        if(!logfile_root)
            logfile_root = HTClientHost ?
		DEFAULT_LOGFILE : DEFAULT_LOCAL_LOGFILE;
	HTLogFileName = (char*) malloc(strlen(logfile_root)+20);

#ifdef NO_GETPID
	sprintf(HTLogFileName, "%s", logfile_root);  /* No getpid() */
#else
	sprintf(HTLogFileName, "%s-%d", logfile_root, (int) getpid());
#endif
	HTlogfile = fopen(HTLogFileName, "a");
	if (!HTlogfile)
	    ErrMsg("Can't open log file", HTLogFileName);
    };

/*	Enable local directory access
*/
    HTDirReadme = HT_DIR_README_TOP;	/* Readme presentation */
    HTDirAccess = HTClientHost ? HT_DIR_SELECTIVE : HT_DIR_OK;	/* browsing */
    

/*	Make home page address
**	----------------------
*/    
    if (!home_anchor)
	home_anchor = HTHomeAnchor ();


/*	Non-interactive use
**	-------------------
*/
    if (filter) {			             /* Just convert formats */
	HTBindAnchor((HTAnchor*)home_anchor, request);
     	HTParseSocket(input_format, 0, request);      /* From std UNIX input */
	    goto endproc;
    }
    
/*	Load first document
**	-------------------
*/
    if ( *keywords ? HTSearch(keywords, home_anchor, request)
    		   : HTLoadAnchor((HTAnchor*) home_anchor, request)) {
    	HTHistory_record((HTAnchor *)home_anchor);
	
    } else {	/* Can't even get last resort: give up */
    
	char *addr = HTAnchor_address((HTAnchor *)home_anchor);

	ErrMsg("Can't access document", addr);
	free(addr);
	if (!HTMainText) {
	    return_status = -2; /* Can't get first page */
	    goto endproc;
        }
    }

    if (!HTMainText)
	goto endproc;			/* Hypertext object was not created */
    

/* 	Main "Event Loop"
**	----------------
*/
    if (interactive) {
	while (Selection_Prompt());
	
    } else if (listrefs_option) {
	    Reference_List(NO);		/* List without titles */
    }

endproc:
    HTRequest_delete(request);
    if(default_default)
	free(default_default);

    if(!return_status) {		/* Good! */
#ifdef VMS 
    return 1;
#else
    return 0; /* Good */
#endif
    }
    return return_status;		/* Bad! */
} /* main() */



/*	Error_Selection
**	---------------
** Produce an error message.
**
*/                       

#ifdef __STDC__
PRIVATE void Error_Selection(void)
#else
PRIVATE void Error_Selection()
#endif

{    
    fprintf(stderr, "%s", "Bad command, for list of commands type help.\n");
    return;
}


/*		Display Help screen	 		 Help_screen
**		-------------------
**
** Produce a help screen, displaying the current document address and a list of 
** available commands *in this context*.
** ?? Perhaps this should be a hypertext page, not included in the history list.
*/

PRIVATE void help_screen NOARGS {

    char * current_address = HTAnchor_address((HTAnchor*)HTMainAnchor);
    CONST char * title = HTAnchor_title(HTMainAnchor);
    
    printf("\n\nWWW LineMode Browser version %s (WWWLib %s)   COMMANDS AVAILABLE\n\n",
			    VL, HTLibraryVersion);
    if (title) printf("You are reading\n \"%s\"\nwhose address is\n  %s\n\n",
						    title, current_address);
    else printf("You are reading a document whose address is\n    '%s' \n\n",
				    current_address);
    
    if (HText_canScrollDown(HTMainText)) {
	printf(
		"  <RETURN>        Move down one page within the document.\n");
	printf(
		"  BOttom          Go to the last page of the document.\n");
	};
	    
    if (HText_canScrollUp(HTMainText)) {
	printf(
		"  Top             Return to the first page of the document.\n");
	printf(
		"  Up              Move up one page within the document\n");
	};
	    
    if (HText_sourceAnchors(HTMainText) != 0) {
	printf("  List            List the references from this document.\n *");
	printf("  <number>        Select a referenced document by number (from 1 to %d).\n",
		HText_sourceAnchors(HTMainText));
    }
	    
    if (HTAnchor_isIndex(HTMainAnchor)) {
	printf(
		"  Find <words>    Search this index for given words (separated by spaces).\n"); 
    }
	    
    if (HTHistory_canBacktrack()) {
	printf("  Recall          List visited documents.\n");
	printf("  Recall <number> Return to a previously visited document\n");
	printf("                  as numbered in the recall list.\n");
	printf("  HOme            Return to the starting document.\n");
	printf("  Back            Move back to the last document.\n");
    }
	    
    if (HTHistory_canMoveBy(1))
	    printf("  Next            Take next link from last document.\n");
		    
    if (HTHistory_canMoveBy(-1))
	    printf("  Previous        Take previous link from last document.\n");

    printf("  REFresh         Refresh screen with current document\n");
    printf("  Go <address>    Go to document of given [relative] address\n");
	    
#ifdef GOT_SYSTEM
    if (!HTClientHost) {	/* NOT for telnet guest! */
	    printf("  PRInt           Print text of this document. *\n");
	    printf("  ! <command>     Execute shell <command> without leaving.\n");
	    printf("  > <file>        Save the text of this document in <file>. *\n");
	    printf("                  If <file> exists use '>!' to overwrite it.\n");
	    printf("  >> <file>       Append the text of this document to <file>. *\n");
	    printf("  | <command>     Pipe this document to the shell <command>. *\n");
#ifdef unix
	    printf("  CD <directory>  Change local working directory.\n");
#endif
	    printf("* Prefix these commands with \"Source \" to use raw source.\n\n");
    }
#endif
	    
#ifdef SLAVE_PRINTER
    printf(
    "  Ps              Print text of this document to Terminal's Slave printer.\n");
#endif
	    
    printf("  Verbose         Switch to %sverbose mode.\n", WWW_TraceFlag ? "non-" : "");
	    
    printf("  Help            Display this page.\n");
    printf("  Manual          Jump to the online manual for this program\n");
    printf("  Quit            Leave the www program.\n");	    
    printf("\n");
    free(current_address);    
} /* End of help_screen */


/*		Select_Reference
**		----------------
**
**  After a reference is selected by the user, opens document, links into the
**  history list and displays.
**
**  On Entry:
**       int  reference_num   Number corresponding to the hypertext reference
**                            given in the text.
*/

BOOL Select_Reference ARGS1(int,reference_num) {

    HTAnchor 		* destination;
    HTChildAnchor * source = HText_childNumber(HTMainText, reference_num);
    
    if (!source) return NO; /* No anchor */
    destination = HTAnchor_followMainLink((HTAnchor*) source);
    request->parentAnchor = HTAnchor_parent((HTAnchor *) source);
    if (!HTLoadAnchor(destination, request)) return NO;	/* No link */
    HTHistory_leavingFrom((HTAnchor*) source);
    HTHistory_record(destination);
    request->parentAnchor = NULL;
    return YES;
    
} /* Select_Reference*/


/* 	Reference_List
**	--------------
**  	Print out a list of HyperText References accumulated within the text.
**
** On entry
**	titles		Set:	if we want titles where available
**			Clear:  we only get addresses.
*/

PRIVATE void Reference_List ARGS1(BOOL, titles)

{
    int  n;
    
    if (HText_sourceAnchors(HTMainText) == 0) {
	fprintf(output,
	"\n\n     There are no references from this document.\n\n");
    } else {
	
	fprintf(output, "\n%s\n", refhead);
	for (n=1; n<=HText_sourceAnchors(HTMainText); n++) {
	    HTAnchor * destination =
	    HTAnchor_followMainLink(
		    (HTAnchor *)HText_childNumber(HTMainText, n)
		    );
	    HTParentAnchor * parent = HTAnchor_parent(destination);
	    char * address =  HTAnchor_address(destination);
	    CONST char * title = titles ? HTAnchor_title(parent) : 0 ;

	    fprintf(output, reference_mark, n);
	    fprintf(output, "%s%s\n",
		    ((HTAnchor*)parent!=destination) && title ? "in " : "",
		    (char *)(title ? title : address));
	    free(address);
	}
		
	fprintf(output, "\n");
    }
}      


/*		History_List
**		------------
*/
/*	Display a history list of nodes visited during the session.
**	?? This should really be a hypertext page (not itself in history!).
**	?? Should have option to display address even when anchor has a title.
*/

PRIVATE void History_List NOARGS {

    int  history_number = 1; 
    printf("\n  Documents you have visited:-\n\n");
    
    do {
    
	char * address;
	CONST char * title;
	HTAnchor * anchor = HTHistory_read(history_number);
	HTParentAnchor * parent;
	
	if (!anchor) break;
	parent = HTAnchor_parent(anchor);
	title = HTAnchor_title(parent);
	address = HTAnchor_address(anchor);
#ifdef VIOLA
	printf("\017R %2d\016)   %s%s\n",	/* SI user field SO */
#else
	printf("R %2d)   %s%s\n",
#endif
		history_number,
		((HTAnchor*)parent!=anchor) && title ? "in " : "",
		title ? title : address);
	free(address);
	
	history_number++;
	
    } while (YES);

    printf("\n");
}      


/*
**	Check_User_Input
**	----------------
**
** Accepts shortened versions of commands.
**
**  On Entry:
**       char *s  Correct and full version of command, to which the user's 
**                input is compared.
**
**  On Exit:
**         returns  YES  User's input corresponds to the command.
**                   NO  Not a recognized command.  
*/

#define Check_User_Input(command) \
    (!strncasecomp (command, this_word, strlen(this_word)))


/*			USER INTERFACE
**			==============
*/

/* Selection_Prompt
** ----------------
** Produces a prompt at the bottom of a page full of text. The prompt varies 
** depending on the options avaliable.
*/

#ifdef __STDC__
PRIVATE BOOL Selection_Prompt(void)
#else
PRIVATE BOOL Selection_Prompt()
#endif
{ 
    int length_of_prompt = 0;
    BOOL is_index = HTAnchor_isIndex(HTMainAnchor);
    
    if (!HText_canScrollDown(HTMainText) &&
	!HTAnchor_hasChildren(HTMainAnchor) && !is_index && 
	(!HTHistory_canBacktrack())) {
	ErrMsg("No way out of here, so I exit!", NULL);	    
	return NO;	                         /* Exit if no other options */
    }
    HText_setStale(HTMainText);                /* We corrupt the display */
	
#ifndef VM	/* Normal prompt */
    if (is_index){	
	printf("FIND <keywords>, ");
	length_of_prompt = length_of_prompt + 14;
    }
    if (HTAnchor_hasChildren(HTMainAnchor)!=0){
	int refs = HText_sourceAnchors(HTMainText);
	if (refs>1) {
	    printf("1-%d, ", refs);
	    length_of_prompt = length_of_prompt + 6;	/* Roughly */
	}
	else {
	    printf("1, ");	
	    length_of_prompt = length_of_prompt + 3;
	}
    }
    if (HTHistory_canBacktrack()){
	printf(PROMPT_MARK, "Back"); printf(", ");
#ifdef LONG_PROMPT
	printf(PROMPT_MARK, "Recall"); printf(", ");
#endif
	length_of_prompt = length_of_prompt + 6;
    }
    if (HText_canScrollUp(HTMainText)){ 
	printf(PROMPT_MARK,"Up"); printf(", ");
	length_of_prompt = length_of_prompt + 4;
    }
    if (HText_canScrollDown(HTMainText)) {
	printf("<RETURN> for more, ");
	length_of_prompt = length_of_prompt + 19;
    }
    if (length_of_prompt <= 47) {
	printf(PROMPT_MARK, "Quit"); printf(", ");
    }
    
    printf("or Help: ");	
	
#else	/* Special prompt for VM assuming PF keys set*/
    if (is_index) {	
	printf("FIND <words>, ");		       	               /* 14 */
	length_of_prompt = length_of_prompt + 14;
    }
    if (HTAnchor_hasChildren(HTMainAnchor)!=0){
	int refs = HText_sourceAnchors(HTMainText);
	if (refs>1) {
	    printf("1-%d, ", refs);
	    length_of_prompt = length_of_prompt + 6;             /* approx 6 */
	}
	else {
	    printf("1, ");
	    length_of_prompt = length_of_prompt + 3;
	}
    }
    if (HTHistory_canMoveBy(1)) {
	printf("PF2=Next ");
	length_of_prompt = length_of_prompt + 9;                    	/* 9 */
    }
    printf("PF3=Quit PF4=Return, ");
    length_of_prompt = length_of_prompt + 21;                          /* 21 */
    
    if (HTHistory_canBacktrack()){
	printf("Recall, ");
	length_of_prompt = length_of_prompt + 8;                    	/* 8 */
    }
    if (HText_canScrollUp(HTMainText)){
	printf(PROMPT_MARK,"PF7=Up "); printf(", ");
	length_of_prompt = length_of_prompt + 7;                    	/* 7 */
    }
    if (HText_canScrollDown(HTMainText)){
	printf("PF8=Down ");
	length_of_prompt = length_of_prompt + 9;                        /* 9 */
    }
    if (length_of_prompt<70) printf("PF11=Help");	                /* 9 */
                                                                 /* EQUAL 82 */
#endif
	
/* Read in the user's input, and deal with it as necessary.
**
**	Any Command which works returns from the routine. If nothing
**	works then a search or error message down at the bottom.
*/
    {   
	int  reference_num;
	char * the_choice = 0;		           /* preserved user command */
	char * this_word = 0;        	   	    /* First word of command */
	char * this_command;       	          /* this_word and following */
	char * next_word;		                      /* Second word */
	char * other_words;		        /* Second word and following */
	
#ifdef NEWLINE_PROMPT
	printf("\n");  	            /* For use on VM to flush out the prompt */
#endif
	if (!fgets(choice, RESPONSE_LENGTH, stdin))	  /* Read User Input */
	    return NO;					      /* Exit if EOF */

	StrAllocCopy (the_choice, choice);             /* Remember it as is, */
	if (the_choice[strlen(the_choice)-1] == '\n')        /* The final \n */
	    the_choice[strlen(the_choice)-1] = '\0';
	
#ifdef VM  /* Clear the screen (on screen-mode systems) */
	clear_screen();
#endif	
	this_word = strtok (choice, " \t\n\r");       /* Tokenize user input */
	this_command = the_choice;
	if (this_word) {
	    next_word = strtok (NULL, " \t\n\r");
	    other_words = the_choice + (next_word - choice);
	}
	else
	    goto down;  /* Empty input : scroll down */

/*		Process Command
**		---------------
*/
/* Giant switch for incoming commands, many of which are single-letter. */
loop:   switch (TOUPPER(*this_word)) {

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
	    sscanf(this_word,"%d",&reference_num);
	    if ((reference_num >= 1) &&
		(reference_num <= HText_sourceAnchors(HTMainText))) {
		Select_Reference(reference_num);       /* Select a reference */
		goto ret;
	    }
	    break;

	  case 'B':		
	    if (Check_User_Input("BACK")) {       /* Return to previous node */
		if (HTHistory_canBacktrack()) {
		    HTLoadAnchor(HTHistory_backtrack(), request);
		} else {
		    printf("\n  The BACK command cannot be used,");
		    printf(" as there are no previous documents\n"); 
		}
	    }
	    else if (Check_User_Input("BOTTOM")) {      /* Scroll to bottom  */
		HText_scrollBottom(HTMainText);
	    }
	    goto ret;
	    break;

#ifdef unix
	  case 'C':
	    if (Check_User_Input("CD"))	       /* Change working directory ? */
		goto lcd;
	    break;
#endif

	  case 'D':
	    if (Check_User_Input("DOWN")) {         /* Scroll down one page  */
down:		if (HText_canScrollDown(HTMainText))
                    HText_scrollDown(HTMainText);
		goto ret;
            }
	    break;

	  case 'E': /* Quit program ? Alternative command */
	    if (Check_User_Input("EXIT"))
		goto stop;
	    break;
	  
	  case 'F':                                      /* Keyword search ? */
	    if (is_index && Check_User_Input("FIND")) {
find:	       	if (next_word && HTSearch(other_words, HTMainAnchor, request))
                    HTHistory_record((HTAnchor*)HTMainAnchor);
                goto ret;
            }
	    break;

	  case 'G':
	    if (Check_User_Input("GOTO")) { 			     /* GOTO */
		if (next_word &&
		    HTLoadRelative(next_word, HTMainAnchor, request))
		    HTHistory_record((HTAnchor*) HTMainAnchor);
		goto ret;
	    }
	    break;
	
	  case '?':
		help_screen();
		goto ret;

	  case 'H':
	    if (Check_User_Input("HELP")){                   /* help menu, ..*/
		help_screen();                    /*!! or a keyword search ? */
		goto ret; 
	    }
	    else if (Check_User_Input("HOME")) { 		/* back HOME */
		if (!HTHistory_canBacktrack()){ 
		    HText_scrollTop(HTMainText);
		} else {
		    HTLoadAnchor(HTHistory_recall(1), request);
		    /*!! this assumes history is kept.*/
		}
		goto ret;
	    } /* if HOME */
	    break;

	  case 'K':                                      /* Keyword search ? */
	    if (is_index && Check_User_Input("KEYWORDS")) {
		goto find;
	    }
	    break;

	  case 'L':
	    if (Check_User_Input("LIST")){           /* List of references ? */
		Reference_List(!OutSource);
		goto ret;
	    }
#ifdef unix
	    else if (Check_User_Input ("LCD")) {       /* Local change dir ? */
	    extern int chdir();
lcd:	        if (!next_word) {                        /* Missing argument */
                    printf ("\nPlease specify the name of the new local directory.\n");
		    goto ret;
		}
                if (chdir (next_word)) {                 /* failed : say why */
		    fprintf (stderr, "\n  ");
		    perror (next_word);
		}
		else {  /* Success : display new local directory */
		    /* AS Sep 93 */
#ifdef NO_GETWD     /* No getwd() on this machine */
#ifdef HAS_GETCWD   /* System V variant SIGN CHANGED TBL 921006 !! */
		    printf ("\nLocal directory is now:\n %s\n",
			    getcwd (choice, sizeof(choice)));
#else   /* has NO getcwd */
		    ErrMsg("This platform does not support getwd() or getcwd()", NULL);
#endif	/* has no getcwd */
#else   /* has getwd */
		    printf("\nLocal directory is now:\n %s\n",
			   (char *) getwd (choice));
#endif  /* has getwd */
		    /* End AS Sep 93 */
		}
                goto ret;
            }
#endif
	    break;

	  case 'M':
	    if (Check_User_Input("MANUAL")){ 	         /* Read User manual */
		if (HTLoadRelative(MANUAL, HTMainAnchor, request))
		    HTHistory_record((HTAnchor*)HTMainAnchor);
		goto ret;
	    }
	    break;
	
	  case 'N':                    
	    if (Check_User_Input("NEXT")) {
		if (!HTHistory_canMoveBy(1)) {   /* No nodes to jump back to */
		    printf("\n  Can't take the NEXT link from the last");
		    if (!HTHistory_canBacktrack())
			printf(" document as there is no last");
		    printf(" document.\n");
		    goto ret; 
		}
		HTLoadAnchor(HTHistory_moveBy(1), request);
		goto ret;
	    }
	    break;

	  case 'P':                    
	    if (Check_User_Input("PREVIOUS")) {
		if (!HTHistory_canMoveBy(-1)){ 
		    printf("\n  Can't take the PREVIOUS link from the last");
		    if (!HTHistory_canBacktrack())
			printf(" document as there is no last");
		    printf(" document.\n");
		    goto ret;
		}
		HTLoadAnchor(HTHistory_moveBy(-1), request);
		goto ret;
	    }
#ifdef GOT_SYSTEM	    
	    else if (!HTClientHost && Check_User_Input("PRINT")) {
		char * address = HTAnchor_address((HTAnchor*)HTMainAnchor);
		char * command;
		char * template = (char*)getenv("WWW_PRINT_COMMAND");
		int result;
			
		if (!template) template = "www -n -na -p66 '%s' | lpr";
		command = (char *) malloc(strlen(address)+strlen(template)+20);
		sprintf(command, template, address);
		result = system(command);
		free(address);
		free(command);
		if (result) printf("  %s\n  returns %d\n", command, result);
		goto ret;
	    }
#endif
	    /* this command prints the entire current text to the
	    terminal's printer; at the end it displays the top of the text */
#ifdef SLAVE_PRINTER
#define SLAVE_PRINTER_ON  "\033\133\065\151"
#define SLAVE_PRINTER_OFF "\033\133\064\151"
	    
	    if (Check_User_Input("PS")) {
		printf ("%s",SLAVE_PRINTER_ON);
		printf("\f");                      /* Form feed for new page */
		HText_scrollTop(HTMainText);
		while(HText_canScrollDown(HTMainText)) {
		    HText_scrollDown(HTMainText);
		}
		printf("\f");  /* Form feed for new page */
		printf ("%s",SLAVE_PRINTER_OFF);
		HText_scrollTop(HTMainText);
		goto ret;
	    }	
#endif
	    break;

	  case 'Q':                                        /* Quit program ? */
	    if (Check_User_Input("QUIT")) {
#ifdef VM
		if (HTHistory_canBacktrack()){       /* Means one level only */
		    HTLoadAnchor(HTHistory_backtrack(), request);
		    goto ret;
		} else {
		    goto stop;		              /* On last level, exit */
		}
#endif

/* 	JFG 9/7/92, following a complaint of 'q' mis-typed for '1'.
	JFG Then made optional because I hate it !!!
	TBL made it only affect remote logged on users. 921122 */

		if (HTClientHost && (strcasecomp(this_word, "quit") != 0) ) {
		    printf ("\n Please type \"quit\" in full to leave www.\n");
		    goto ret;
		}
		goto stop;
	    }
	    break;

	  case 'R':	
#ifdef VM
	    if (Check_User_Input("RETURN"))	       /* Means quit program */
		goto stop;
#endif	    
	    if (Check_User_Input("RECALL")) {
		int  recall_node_num;
		
		if (!HTHistory_canBacktrack()) {       /* No nodes to recall */
		    printf("\n  No other documents to recall.\n");
		    goto ret;
		}
		
		/* Previous node number exists, or does the user just */
		/* require a list of nodes visited? */
		if (next_word) {
		    if ((recall_node_num = atoi(next_word)) > 0)  /* Parm OK */
		    	HTLoadAnchor(HTHistory_recall(recall_node_num),
				     request);
		    else
		    	Error_Selection();
		}
		else 
		    History_List();
		goto ret;
	    } else if (Check_User_Input("REFRESH")){
	        HText_select(HTMainText);		   /* Refresh screen */
		goto ret;
	    }
	    break;

	  case 'S':					       /* TBL 921009 */
	    if (Check_User_Input("SOURCE")) {     	  /* Apply to source */
		if (!next_word) goto ret;    /* Should refresh as source @@@ */
		OutSource = YES;		 /* Load and print as source */
		this_word = next_word;		         /* Move up one word */
		next_word = strtok (NULL, " \t\n\r");
		this_command = the_choice + (this_word - choice);
		other_words = the_choice + (next_word - choice);
		goto loop;	               	       /* Go treat as before */
	    } else if (Check_User_Input("SET")) {                  /* config */
		HTSetConfiguration(other_words);
		goto ret;
	    }
	    break;

	  case 'T':
	    if (Check_User_Input("TOP")) {                 /* Return to top  */
		HText_scrollTop(HTMainText);
		goto ret;
	    }
	    break;
	    
	  case 'U':
	    if (Check_User_Input("UP")) {             /* Scroll up one page  */
		HText_scrollUp(HTMainText);
		goto ret;
	    }
	    break;

	  case 'V':
	    if (Check_User_Input("VERBOSE")) {       /* Switch verbose mode  */
		WWW_TraceFlag = ! WWW_TraceFlag;
		printf ("\n  Verbose mode %s.\n", WWW_TraceFlag ? "ON":"OFF");
		goto ret;
	    }
	    break;
	
	  case '>':
	    if (!HTClientHost) {
		SaveOutputStream(this_word, next_word);
		goto ret;
	    }
	    break;
	 
#ifdef GOT_PIPE
	  case '|':
	    if (!HTClientHost) {	                   /* Local only!!!! */
		char * address = HTAnchor_address((HTAnchor*) HTMainAnchor);
		char * command;
		int result;
	    
		command = (char*) malloc(strlen(address)
					 +strlen(this_command)+30);

#ifdef VM	
		sprintf(command, "PIPE CMS WWW %s \"%s\" | %s",
		/* Note format is | > */ 
#else	
	        sprintf(command, "www %s \"%s\" %s", 
#endif
		OutSource ? "-source" : "-n -na -p", address, this_command);

		printf("Command: %s\n", command);
		result = system(command);
	        if (result)
		    printf("  %s  returns %d\n", command, result);
		free(command);
	        free(address);
	        goto ret;
	    }
#endif

#ifdef GOT_SYSTEM
	  case '!':
	    if (!HTClientHost) {	                /* Local only!!!!!!! */
		int result;
		ErrMsg("Executing", this_command);
		result = system(strchr(this_command, '!') + 1);
		if (result) printf("  %s  returns %d\n",
				   strchr(this_command, '!') + 1, result);
		goto ret;
	    }
#endif
	  default:
	    break;
        } /* Switch on 1st character */
	
	if (is_index && *this_word) {        /* No commands, search keywords */
	    next_word = other_words = this_command;
	    goto find;
	} else {             
	    Error_Selection();
	}

ret:
	free (the_choice);
	return YES;

stop:   free(the_choice);
	return NO;
    }	/* Get user selection */
}  /* Selection_Prompt */


/* -------------------------------------------------------------------------
   General Error Message for Line Mode Browser. If verbose mode is on, then
   the message is passed to stderr.

   Henrik Frystyk 02/03-94
   ------------------------------------------------------------------------- */
PRIVATE void ErrMsg ARGS2(char *, Msg, char *, Str)
{
    if (TRACE) {
	if (Str)	
	    fprintf(stderr, "Line Mode Browser: %s (%s)\n", Msg, Str);
	else
	    fprintf(stderr, "Line Mode Browser: %s\n", Msg);
    }
}


/* -------------------------------------------------------------------------
   This function puts up a stream to a file in order to save a document. This
   is activated by '>', '>>' or '>!' from the prompt line.
   Returns NO if Error and YES if OK

   Henrik Frystyk 02/03-94
   ------------------------------------------------------------------------- */
PRIVATE BOOL SaveOutputStream ARGS2(char *, This, char *, Next)
{
    FILE *fp;
    char *fname;
    char *fmode;
    
    /* Checks if file exists. Can be overruled by using '>!' */
    if (*(This+1) == '>') {	 		   	   /* Append to file */
	fmode = "a";
	fname = *(This+2) ? (This+2) : Next;
    } else if (*(This+1) == '!') {
	fmode = "w";				           /* Overwrite file */
	fname = *(This+2) ? (This+2) : Next;
    } else {						/* File name follows */
	fmode = "w";
	fname = *(This+1) ? (This+1) : Next;
	if (fname) {				       /* See if file exists */
	    if ((fp = fopen(fname, "r")) != NULL) {
		printf("%s: File exists\n", fname);
		fclose(fp);
		return NO;
	    }
	}
    }
    if (!fname)					       /* No file name given */
	return NO;
    if ((fp = fopen(fname, fmode)) == NULL) {
	ErrMsg("Can't access file", fname);
	return NO;
    }

    /* Now, file is open and OK: reload the text and put up a stream for it! */
    if (TRACE) fprintf(stderr, "Saving to file %s\n", fname);
    {
	BOOL ret = NO;
	HTRequest *req = HTRequest_new(); 	     /* Set up a new request */
	HTFormatInitNIM(req->conversions);	     /* Non-interactive mode */
	if(OutSource)
	    req->output_format = WWW_SOURCE;
	req->output_stream = HTFWriter_new(fp, NO);
	HTForceReload = YES;
	ret = HTLoadAnchor((HTAnchor*) HTMainAnchor, req);
	HTForceReload = NO;
	HTRequest_delete(req);
	return ret;
    }
}

/* End HTBrowse.c */

