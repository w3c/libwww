/*    HyperText Browser for Dumb Terminals     		     HTBrowse.c
**    ====================================
**
**  Authors:
**	NP:  Nicola Pellow  Tech.Student CERN 1990-91
**	TBL: Tim Berners-Lee CERN (timbl@info.cern.ch)
**	JFG: Jean-Francois Groff, Cooperant CERN 1991-92 (jfg@info.cern.ch)
**	DR:  Dudu Rashty +972-2-584848 <RASHTY@hujivms.bitnet>
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

/*	If the guy gives the "MANUAL" command, jump to this:
*/
#ifndef MANUAL
#define MANUAL "http://info.cern.ch/hypertext/WWW/LineMode/Defaults/QuickGuide.html"
#endif

/* Check Statements */
/* ================ */

#define NAME_CHECK 0                /* Trace to show NAME anchors */


/* Include Files */
/* ============= */

#include <ctype.h>
#include "HTUtils.h"		    /* WWW general purpose macros */

#include "HTBrowse.h"		 /* Things exported, short names */

#include "GridText.h"	 /* Hypertext definition */

#include "HTFormat.h"
#include "HTTCP.h"	/* TCP/IP utilities */
#include "HTAnchor.h"   /* Anchor class */
#include "HTParse.h"    /* WWW address manipulation */
#include "HTAccess.h"   /* WWW document access network code */
#include "HTHistory.h"	/* Navigational aids */
#include "HTML.h"	/* For parser */
#include "HTFWriter.h"	/* For non-interactive output */
#include "HTFile.h"	/* For Dir access flags */

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
#define SCREEN_WIDTH 78
#endif

#ifndef SCREEN_WIDTH      
#define SCREEN_WIDTH 79  /* Default width of the screen */ 
#endif
#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 24 /* Default number of lines to the screen */
#endif

#ifdef VM   			/* Needed to flush out the prompt line..*/
#ifndef NEWLIB		/* except on NEWLIB which can wrap. */
#define NEWLINE_PROMPT  /* before input */
#endif
#endif

#define INFINITY 1024		   		/*!! BUG*/
#define ADDRESS_LENGTH  INFINITY	   /* Maximum address length of node */
#define TITLE_LENGTH    INFINITY    /* Maximum length of a title */
#define RESPONSE_LENGTH INFINITY    /* Maximum length of users response */


/*	Public Variables
**	================
*/

PUBLIC  int  HTScreenWidth   = SCREEN_WIDTH;	/* By default */
PUBLIC  int  HTScreenHeight  = -1;	         /* Undefined */
PUBLIC  BOOL display_anchors = YES;	         /* anchor will be shown in text? */
PRIVATE  BOOL interactive     = YES;          /*  e.g. shows prompts etc */
PRIVATE  char * output_file_name = NULL;     /* -o xxxx */
					   
PUBLIC char * reference_mark = REF_MARK;     /* Format string for [1] &c */
PUBLIC char * end_mark = END_MARK;           /* Format string for [End] */

/* Moved into other files: */

/* PUBLIC char * HTClientHost = 0;	  HTAccess */
/* PUBLIC  int  WWW_TraceFlag   = 0;      HTString */
/* PUBLIC FILE * logfile = 0;		  HTAccess */
 
 
 /*	Private variables
 **	=================
 */
/* Arrays for storing the HyperText References */

PRIVATE HTParentAnchor * home_anchor = NULL;	/* First document anchor */
PRIVATE char keywords[ADDRESS_LENGTH]; /* Search terms from command line */

PRIVATE char        choice[RESPONSE_LENGTH];    /* Users response  */

PRIVATE char *	     logfile_root = 0;	         /* Log file name */
PUBLIC char *	     log_file_name = 0;	         /* Root of log file name */
PRIVATE BOOL	     filter=0;		               /* Load from stdin? */
PRIVATE BOOL	     listrefs_option = 0;	/* -listrefs option used?  */

/* Forward Declaration of Functions */
/* ================================ */

PRIVATE void History_List NOPARAMS; 
PRIVATE void Selection_Prompt NOPARAMS;
/* PRIVATE BOOL Check_User_Input PARAMS((char *s)); */
PRIVATE void Error_Selection NOPARAMS;
PRIVATE void help_screen NOPARAMS;
PRIVATE void Reference_List PARAMS((BOOL titles));

PRIVATE HTFormat HTInputFormat = NULL;

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
    int  arg;		         /* Argument number as we scan */
    BOOL argument_found = NO;
    BOOL first_keyword = YES;
    BOOL default_used = NO;	 /* Fell back on home page? */
    char* default_default=0;	 /* Parse home relative to this */

    HTFormat format_in = WWW_HTML;		/* By default */
    HTOutputFormat = WWW_PRESENT;	        /* By default */
    
#ifdef THINK_C /* command line from Think_C */
    int i;
    argc=ccommand(&argv);
#endif
	
    StrAllocCopy(default_default, "file://");
    StrAllocCat(default_default, HTHostName()); /* eg file://cernvax.cern.ch */

#ifndef MAXPATHLEN
#define NO_GETWD		/* Assume no  getwd() if no MAXPATHLEN */
#endif

#ifdef NO_GETWD  		/* No getwd() on this machine */
#ifdef HAS_GETCWD		/* System V variant SIGN CHANGED TBL 921006 !! */

    {
	char wd[1024];			/*!! Arbitrary*/
	extern char * getcwd();
	char * result = getcwd(wd, sizeof(wd)); 
	if (result) {

#ifdef vms  /* convert directory name to Unix-style syntax */
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
	        fprintf(stderr,
		    "HTBrowse: Can't read working directory (getcwd).\n");
	    }
	}  /* end if good getcwd result */
	
#else   /* has NO getcwd */

	if (TRACE) fprintf(stderr,
	    "HTBrowse: This platform does not support getwd() or getcwd()\n");
#endif	/* has no getcwd */

#else   /* has getwd */
	{
      	    char wd[MAXPATHLEN];
	    extern char * getwd();
      	    char * result = getwd(wd);
	    if (result) {
	        StrAllocCat(default_default, wd);
	    } else {
	        fprintf(stderr, "HTBrowse: Can't read working directory.\n");
	    }
	}
#endif
		
#ifdef vms
      StrAllocCat(default_default, "default.html");
#else
      StrAllocCat(default_default, "/default.html");
#endif

/*	Check for command line options
**	------------------------------
*/

    keywords[0] = 0;				/* Clear string */
    for (arg=1; arg<argc ; arg++) {
	if (*argv[arg]=='-'){
		
#ifdef TRACE
	   /* Verify: Turns on trace */
	    if (0==strcmp(argv[arg], "-v")) {
		WWW_TraceFlag = 1;
	    }
#endif
	
	    /* - alone => filter */
	    else if (argv[arg][1] == 0) {
		filter = YES;	   
		/* Take from stdin, Force non-interactive */
		interactive = NO;
	    
	    /* Page size */
	    } else if (argv[arg][1] == 'p') {
		if (sscanf(argv[arg]+2, "%d", &HTScreenHeight) <1)  /* fail */
		    HTScreenHeight = -1;	   /* Undefined */
	    
	    /* Page width */
	    } else if (argv[arg][1] == 'w') {
		if (sscanf(argv[arg]+2, "%d", &HTScreenWidth) <1)  /* fail */
			HTScreenWidth = SCREEN_WIDTH;
	    
	    /* from -- Initial represntation */
	    } else if (0==strcmp(argv[arg], "-from")) {
		if (++arg < argc)
		    HTInputFormat = HTAtom_for(argv[arg]);

	    /* Telnet from */
	    } else if (argv[arg][1] == 'h') {
		if (++arg < argc)
		    HTClientHost = argv[arg]; /* Use original host name */

			
	    /* Log file */
	    } else if (0==strcmp(argv[arg], "-l")) {
		if (++arg <argc) {
		    logfile_root = argv[arg];
		}
		    
	    /* Log file */
	    } else if (0==strcmp(argv[arg], "-listrefs")) {
		listrefs_option = YES;
		interactive = NO;	/* Force non-interactive */

	    /* Non-interactive */
	    } else if (0==strcmp(argv[arg], "-n")) {
		interactive = NO;

	    /* Output filename */
	    } else if (0==strcmp(argv[arg], "-o")) { 
		if (++arg < argc) {
		    output_file_name = argv[arg];/* Change representation */
		    interactive = NO;
		}
	    /* Anchor format */
	    } else if (0==strcmp(argv[arg], "-a")) { 
		if (++arg < argc)
		    reference_mark = argv[arg];	  /* Change representation */

	    /* No anchors */
	    } else if (0==strcmp(argv[arg], "-na")) { 
		    display_anchors = NO;

	    /* Source please */
	    } else if (0==strcmp(argv[arg], "-source")) {
		    HTOutputFormat = WWW_SOURCE;
		    HTOutputSource = YES;	/* Turn on shortcut */
		    interactive = NO;	/* JFG */
		    
	    /* to -- Final represntation */
	    } else if (0==strcmp(argv[arg], "-to")) {
		if (++arg < argc) {
		    HTOutputFormat = HTAtom_for(argv[arg]);
		    HTOutputSource = YES;	/* Turn on shortcut */
		    interactive = NO;	/* JFG */
		}
	    /* Print version and exit */
	    } else if (0==strcmp(argv[arg], "-version")) { 
    		printf("WWW LineMode Browser version %s (WWW Library %s)\n",
			    VL, HTLibraryVersion);
		exit(0);
	    }
#ifdef THINK_C
	    /* Echo to file */
	    else if (0==strcmp(argv[arg], "-e")){
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
	    } else if (0==strcmp(argv[arg], "-s")) {
		    socketdebug=1;
	    }
#endif
	      /* endif long list of argument options */

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

    if (HTScreenHeight == -1)		/* Default page size */
	HTScreenHeight = interactive ? SCREEN_HEIGHT : 999999;

/*	Open output file
**	----------------
*/
    if (!interactive)	{		/* check for file name @@@@@ */
    	if (output_file_name) {
	    FILE * fp = fopen(output_file_name, "w");
	    if (!fp) {
	        fprintf(stderr, "WWW: Can't open file `%s' for writing\n",
			output_file_name);
		exit(-4);
	    }
            HTOutputStream = HTFWriter_new(fp);   /* Out to file */
	} else {
            HTOutputStream = HTFWriter_new(stdout);   /* Just pump to stdout */
	}
    }
    
    
/*	Open Log File if necessary
**	--------------------------
*/
    if (!logfile_root && HTClientHost) {
	logfile_root = DEFAULT_LOGFILE;
    }
    
        
    if (logfile_root) {
	log_file_name = (char*) malloc(strlen(logfile_root)+20);
#ifdef NO_GETPID
	sprintf(log_file_name, "%s", logfile_root);  /* No getpid() */
#else
	sprintf(log_file_name, "%s-%d", logfile_root, getpid());
#endif
	logfile = fopen(log_file_name, "a");
	if (!logfile)
	    fprintf(stderr, "WWW: Can't open log file %s\n",log_file_name);
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

    if (filter) {			/* Just convert formats */
        HTParseSocket(format_in, HTOutputFormat, 
                home_anchor,
		0,			/* stdin unix file */
		HTOutputStream);
        goto good;
    }
    
/*	Load first document
**	-------------------
*/
    if ( *keywords ? HTSearch(keywords, home_anchor)
    		   : HTLoadAnchor((HTAnchor*)home_anchor)){
		   
	HTHistory_record((HTAnchor *)home_anchor);
	
    } else {	/* Can't even get last resort: give up */
    
	fprintf(stderr, "\nWWW: Can't access `%s'\n", 
		HTAnchor_address((HTAnchor *)home_anchor));/* not freed */
	if (!HTMainText) exit(2); /* Can't get first page */
    }
    
    if (!HTMainText) exit(0);	/* Hypertext object was not created */
    
/* 	Main "Event Loop"
**	----------------
*/

    if (interactive) {
	while (YES) Selection_Prompt();
	
    } else if (!HTOutputSource) {	/* Non-interactive but formatted */
	printf("\f");			/* Form feed for new page */
	while(HText_canScrollDown(HTMainText)) {
	    HText_scrollDown(HTMainText);
	    printf("\f");		/* Form feed for new page */
	}
	
	if (listrefs_option) {
	    Reference_List(NO);		/* List without titles */
	}
    }
good:	
#ifdef vms
    return 1;
#else
    return 0; /* Good */
#endif

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


/*		Display Help screen					 Help_screen
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
	printf("  List            List the references from this document.\n");
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

    printf("  Go address      Go to document of given [relative] address\n");
	    
#ifdef GOT_SYSTEM
    if (!HTClientHost) {	/* NOT for telnet guest! */
	    printf("  PRInt           Print text of this document. *\n");
	    printf("  ! command       Execute shell command without leaving.\n");
	    printf("  > file          Save the text of this document in a file. *\n");
	    printf("  >> file         Append the text of this document to a file. *\n");
	    printf("  | command       Pipe this document to a shell command. *\n");
#ifdef unix
	    printf("  CD directory    Change local working directory.\n");
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
    
}


/*		Select_Reference
**		----------------
**
**  After a reference is selected by the user, opens document, links into the history
**  list and displays.
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
    if (!HTLoadAnchor(destination)) return NO;	/* No link */
    HTHistory_leavingFrom((HTAnchor*) source);
    HTHistory_record(destination);
    
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
	    printf("\n\n     There are no references from this document.\n\n");
    } else {
	    
	printf("\n\n     References from this document:-\n\n");
	    
	for (n=1; n<=HText_sourceAnchors(HTMainText); n++) {
	    HTAnchor * destination =
	    HTAnchor_followMainLink(
		    (HTAnchor *)HText_childNumber(HTMainText, n)
		    );
	    HTParentAnchor * parent = HTAnchor_parent(destination);
	    char * address =  HTAnchor_address(destination);
	    CONST char * title = titles ? HTAnchor_title(parent) : 0 ;

	    printf(reference_mark, n);
	    printf("  %s%s\n",
		    ((HTAnchor*)parent!=destination) && title ? "in " : "",
		    (char *)(title ? title : address));
	    free(address);
	}
		
	printf("\n");
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
PRIVATE void Selection_Prompt(void)
#else
PRIVATE void Selection_Prompt()
#endif

{ 
	BOOL HTDiag = NO;	/* Flag == source asked for? */	
	int length_of_prompt = 0;
	BOOL is_index = HTAnchor_isIndex(HTMainAnchor);
	
	if ( !HText_canScrollDown(HTMainText) &&
		    !HTAnchor_hasChildren(HTMainAnchor) &&
		    !is_index && 
		    (!HTHistory_canBacktrack())){
	    printf("\n");
	    
	    exit(0); /* Exit if no other options */
	}
	
	HText_setStale(HTMainText);	/* We corrupt the display */
	
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
		if (HText_canScrollDown(HTMainText)){
			printf("<RETURN> for more, ");
			length_of_prompt = length_of_prompt + 19;
			}
		if (length_of_prompt <= 47){
			printf(PROMPT_MARK, "Quit"); printf(", ");
			}

		printf("or Help: ");	
	
#else	/* Special prompt for VM assuming PF keys set*/
		if (is_index){	
			printf("FIND <words>, ");			/*	14	*/
			length_of_prompt = length_of_prompt + 14;
			}
		if (HTAnchor_hasChildren(HTMainAnchor)!=0){
			int refs = HText_sourceAnchors(HTMainText);
			if (refs>1) {
				printf("1-%d, ", refs);
				length_of_prompt = length_of_prompt + 6;	/* approx 6	*/
				}
			else {
				printf("1, ");
				length_of_prompt = length_of_prompt + 3;
				}
			}
		if (HTHistory_canMoveBy(1)) {
			printf("PF2=Next ");
			length_of_prompt = length_of_prompt + 9; /* 9 */
			}
		printf("PF3=Quit PF4=Return, ");
		length_of_prompt = length_of_prompt + 21;  /*21	*/
		
		if (HTHistory_canBacktrack()){
			printf("Recall, ");
			length_of_prompt = length_of_prompt + 8; /* 8*/
			}
		if (HText_canScrollUp(HTMainText)){
			printf(PROMPT_MARK,"PF7=Up "); printf(", ");
			length_of_prompt = length_of_prompt + 7; /* 7 */
			}
		if (HText_canScrollDown(HTMainText)){
			printf("PF8=Down ");
			length_of_prompt = length_of_prompt + 9; /* 9 */
			}
		if (length_of_prompt<70) printf("PF11=Help");	/* 9 */
		/* ------	*/
		/*     82 	*/
#endif

	
/* Read in the user's input, and deal with it as necessary.
**
**	Any Command which works returns from the routine. If nothing
**	works then a search or error message down at the bottom.
*/
	
	{   
	int  reference_num;
	char * the_choice = 0;		/* preserved user command */
	char * this_word;	   	/* First word of command */
	char * this_command;		/* this_word and following */
	char * next_word;		   /* Second word */
	char * other_words;		   /* Second word and following */
	
	
#ifdef NEWLINE_PROMPT
	printf("\n");  /* For use on VM to flush out the prompt */ 
#endif
	fgets(choice, RESPONSE_LENGTH, stdin);	/* Read User Input */
	StrAllocCopy (the_choice, choice);     /* Remember it as is, */
	if (the_choice[strlen(the_choice)-1] == '\n')  /* except the final \n */
	   the_choice[strlen(the_choice)-1] = '\0';
	
#ifdef VM  /* Clear the screen (on screen-mode systems) */
	clear_screen();
#endif
	
	this_word = strtok (choice, " \t\n\r");  /* Tokenize user input */
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
/* Giant switch for incoming commands, many of which are single-letter.
*/
loop:

	switch (TOUPPER(*this_word)) {

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
		if ((reference_num >= 1)&&
			(reference_num <= HText_sourceAnchors(HTMainText))){
																			/* Selection of a reference */
				Select_Reference(reference_num);
				goto ret;
				}            
		break;




	case 'B':		
		if (Check_User_Input("BACK")) { /* Return to previous node if this is possible */
			if (!HTHistory_canBacktrack()){
				printf("\n  The BACK command cannot be used,");
				printf(" as there are no previous documents\n");
				goto ret; 
				}
			HTLoadAnchor(HTHistory_backtrack());
			goto ret;
			}
		else if (Check_User_Input("BOTTOM")) { /* Scroll to bottom  */
			HText_scrollBottom(HTMainText);
			goto ret;
			}
		break;




#ifdef unix
		case 'C':
			if (Check_User_Input("CD"))
				/* Change working directory ? */
				goto lcd;
			break;
#endif




	case 'D':
		if (Check_User_Input("DOWN")) { /* Scroll down one page  */
down:
			if (HText_canScrollDown(HTMainText))
				HText_scrollDown(HTMainText);
			goto ret;
			}
		break;




	case 'E': /* Quit program ? Alternative command */
		if (Check_User_Input("EXIT"))
			exit(0);
		break;




	case 'F':
		if (is_index && Check_User_Input("FIND")){ /* Keyword search ? */
find:
			if (HTSearch(other_words, HTMainAnchor))
				HTHistory_record((HTAnchor*)HTMainAnchor);
			goto ret;
			}
		break;




	case 'G':
	if (Check_User_Input("GOTO")){ /* GOTO */
		if (HTLoadRelative(next_word, HTMainAnchor))
			HTHistory_record((HTAnchor*)HTMainAnchor);
		goto ret;
		}
	break;
	
	case '?':
		help_screen();
		goto ret;

	case 'H':
		if (Check_User_Input("HELP")){  /* help menu, ..*/
			help_screen();                /*!! or a keyword search ? */
			goto ret; 
			}
	
		else if (Check_User_Input("HOME")){ /* back HOME */
			if (!HTHistory_canBacktrack()){ 
				HText_scrollTop(HTMainText);
				} 
			else {
				HTLoadAnchor(HTHistory_recall(1)); /*!! this assumes history is kept.*/
				}
			goto ret;
			} /* if HOME */
		break;




	case 'K':                                /* Keyword search ? */
		if (is_index && Check_User_Input("KEYWORDS")){
			goto find;
			}
		break;




	case 'L':
		if (Check_User_Input("LIST")){      /* List of references ? */
			Reference_List(!HTDiag);
			goto ret;
		}
#ifdef unix
			else if (Check_User_Input ("LCD")) {  /* Local change dir ? */
lcd:	      if (!next_word) {  /* Missing argument */
					printf ("\n  Please specify the name of the new local directory.\n");
					goto ret;
					}
				if (chdir (next_word)) {  /* failed : say why */
					fprintf (stderr, "\n  ");
					perror (next_word);
					}
				else {  /* Success : display new local directory */
					printf ("\n  Local directory is now:\n    %s\n", getwd (choice));
					}
				goto ret;
				}
#endif
		break;


	case 'M':
	if (Check_User_Input("MANUAL")){ 	/* Read User manual */
		if (HTLoadRelative(MANUAL, HTMainAnchor))
			HTHistory_record((HTAnchor*)HTMainAnchor);
		goto ret;
		}
	break;
	


	case 'N':                    
		if (Check_User_Input("NEXT")) {
			if (!HTHistory_canMoveBy(1)){   /* No nodes to jump back to */
				printf("\n  Can't take the NEXT link from the last");
				if (!HTHistory_canBacktrack())
				printf(" document as there is no last");
				printf(" document.\n");
				goto ret; 
				}
			HTLoadAnchor(HTHistory_moveBy(1));
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
			HTLoadAnchor(HTHistory_moveBy(-1));
			goto ret;
			}
#ifdef GOT_SYSTEM	    
			else if (!HTClientHost && Check_User_Input("PRINT")) {
				char * address = HTAnchor_address((HTAnchor*)HTMainAnchor);
				char * command;
				char * template = (char*)getenv("WWW_PRINT_COMMAND");
				int result;
			
				if (!template) template = "www -n -na -p66 '%s' | lpr";
				command  = (char *) malloc(strlen(address)+strlen(template)+20);
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
				printf("\f");                   /* Form feed for new page */
				HText_scrollTop(HTMainText);
				while(HText_canScrollDown(HTMainText)) {
					HText_scrollDown(HTMainText);
					}
				printf("\f");  /* Form feed for new page */
				printf ("%s",SLAVE_PRINTER_OFF);
				HText_scrollTop(HTMainText);
				return;
				}
			
#endif
		break;




	case 'Q':                                /* Quit program ? */
		if (Check_User_Input("QUIT")) {
#ifdef VM
		    if (HTHistory_canBacktrack()){  /* Means one level only */
			    HTLoadAnchor(HTHistory_backtrack());
			    goto ret;
		    } else {
		        exit(0);		/* On last level, exit */
		    }
#endif
/* 	JFG 9/7/92, following a complaint of 'q' mis-typed for '1'.
	JFG Then made optional because I hate it !!!
	TBL made it only affect remote logged on users. 921122 */

		    if (HTClientHost && (strcasecomp(this_word, "quit") != 0) ) {
			printf ("\n  Please type \"quit\" in full to leave www.\n");
			goto ret;
		    }

		    exit(0);
		}
		break;




	case 'R':
#ifdef VM
	    if (Check_User_Input("RETURN"))	/* Means quit program */
			    exit(0);
#endif	    
	    if (Check_User_Input("RECALL")){
		int  recall_node_num;
		if (!HTHistory_canBacktrack()){    /* No nodes to recall */
			printf("\n  No other documents to recall.\n");
			goto ret;
		}
		/* Is there a previous node number to recall, or does the user just require
		a list of nodes visited? */
		
		if (next_word) {
		    if ((recall_node_num = atoi(next_word)) > 0)  /* Good parameter */
		    	HTLoadAnchor(HTHistory_recall(recall_node_num));
		    else
		    	Error_Selection();
		}
		else 
			History_List();
		
		goto ret;

	    } else if (Check_User_Input("REFRESH")){
	        HText_select(HTMainText);		/* Refresh screen */
		goto ret;
	    }
	    break;

	case 'S':					/* TBL 921009 */
		if (Check_User_Input("SOURCE")) {   	/* Apply to source */
			if (!next_word) goto ret;	/* Should refresh as source @@@ */

			HTDiag = YES;			/* Load, print source */
			this_word = next_word;		/* Move up one word */
			next_word = strtok (NULL, " \t\n\r");
			this_command = the_choice + (this_word - choice);
			other_words = the_choice + (next_word - choice);
			goto loop;		/* Go treat as before */
		}
		break;
	case 'T':
		if (Check_User_Input("TOP")) {   /* Return to top  */
			HText_scrollTop(HTMainText);
			goto ret;
			}
		break;

	case 'U':
		if (Check_User_Input("UP")) {   /* Scroll up one page  */
			HText_scrollUp(HTMainText);
			goto ret;
			}
		break;

	case 'V':
		if (Check_User_Input("VERBOSE")) {   /* Switch verbose mode  */
			WWW_TraceFlag = ! WWW_TraceFlag;
			printf ("\n  Verbose mode %s.\n", WWW_TraceFlag ? "ON" : "OFF");
			goto ret;
			}
		break;
	
#ifdef GOT_PIPE
	case '>':
	case '|':
	if (!HTClientHost) {	/* Local only!!!! */
	    char * address = HTAnchor_address((HTAnchor*)HTMainAnchor);
	    char * command;
	    int result;
	    
	    command  = (char *) malloc(
	    	strlen(address)+strlen(this_command)+30);
#ifdef VM
	    sprintf(command, "PIPE CMS WWW %s \"%s\" | %s",
	    /* Note format is | > */ 
#else
	    sprintf(command, "www %s \"%s\" %s", 
#endif
       	    HTDiag ? "-source" : "-n -na -p", address, this_command);
	    result = system(command);
	    if (result) printf("  %s  returns %d\n", command, result);
	    free(command);
	    free(address);
	    goto ret;
	}
#endif

#ifdef GOT_SYSTEM
	case '!':
	if (!HTClientHost) {	/* Local only!!!!!!! */
		int result;
		result = system(strchr(this_command, '!') + 1);
		if (result) printf("  %s  returns %d\n",
			strchr(this_command, '!') + 1, result);
		goto ret;
		}
#endif
	
	default:
		break;

	}	/* Switch on 1st character */
	

	if (is_index && *this_word) {  /* No commands, search keywords */
	    if (HTSearch(this_command + (this_word - choice), HTMainAnchor))
	    HTHistory_record((HTAnchor*)HTMainAnchor);
	} else {             
	    Error_Selection();
	}

ret:	free (the_choice);
	return;
	
    }	/* Get user selection */
} /* Selection_Prompt */


/* *end HTBrowse.c* */