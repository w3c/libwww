/*         		    					     HTBrowse.c
**	HYPERTEXT BROWSER FOR DUMB TERMINALS
**
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**  Authors:
**	NP:  Nicola Pellow  Tech.Student CERN 1990-91
**	TBL: Tim Berners-Lee CERN (timbl@info.cern.ch)
**	JFG: Jean-Francois Groff, Cooperant CERN 1991-92 (jfg@info.cern.ch)
**	DR:  Dudu Rashty +972-2-584848 <RASHTY@hujivms.bitnet>
**	MD:  Mark Donszelmann, DELPHI CERN, (duns@vxdeop.cern.ch)
**	HFN: Henrik Frystyk Nielsen, CERN, (frystyk@dxcern.cern.ch)
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

#include "WWWLib.h"
#include "HTBrowse.h"	/* Things exported, short names */
#include "GridText.h"	/* Hypertext definition */

#if 0
#include <ctype.h>
#include "HTFormat.h"
#include "HTTCP.h"	/* TCP/IP utilities */
#include "HTAnchor.h"   /* Anchor class */
#include "HTParse.h"    /* WWW address manipulation */
#include "HTAccess.h"   /* WWW document access network code */
#include "HTHist.h"	/* Navigational aids */
#include "HTML.h"	/* For parser */
#include "HTFWrite.h"	/* For non-interactive output */
#include "HTMLGen.h"	/* For reformatting HTML */
#include "HTFile.h"	/* For Dir access flags */
#include "HTRules.h"    /* For loading rule file */
#include "HTError.h"
#include "HTAlert.h"
#include "HTTP.h"
#include "HTEvent.h"
#endif

/* HWL 18/7/94: applied patch from agl@glas2.glas.apc.org (Anton Tropashko) */
#ifdef CYRILLIC		
#include "a_stdio.h"
#endif

#ifdef THINK_C			     /* Macintosh Think C development system */
#include <console.h>
#include <time.h>
extern int socketdebug;		   /* Must be declared in the socket library */
#endif

/* Macros and other defines */
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

#define INFINITY		1024		                     /*!! BUG*/
#define ADDRESS_LENGTH		INFINITY   /* Maximum address length of node */
#define RESPONSE_LENGTH		INFINITY /* Maximum length of users response */

/* #define LONG_PROMPT	1 */			/* Long or short user prompt */

#if defined(ultrix) || defined(__osf__)
#define GET_SCREEN_SIZE
#endif

#if defined(__svr4__)
#define CATCH_SIG
#endif

#define Check_User_Input(command) \
    (!strncasecomp (command, this_word, strlen(this_word)))

/* Public Variables etc. */
/* HWL 18/7/94: applied patch from agl@glas2.glas.apc.org (Anton Tropashko) */
#ifdef CYRILLIC
struct ARc arc;
#endif

PUBLIC char *		HTAppName = "CERN-LineMode";     /* Application name */
PUBLIC char *		HTAppVersion = VL;            /* Application version */
PUBLIC int		HTScreenWidth   = SCREEN_WIDTH;	       /* By default */
PUBLIC int		HTScreenHeight  = -1;		   /* -1 = Undefined */
PUBLIC BOOL		display_anchors = YES;	    /* Show anchors in text? */
PUBLIC char *		start_reference = NULL;   /* Format for start anchor */
PUBLIC char *		end_reference = REF_MARK;	   /* for end anchor */
PUBLIC char *		reference_mark = "[%d] ";     /* for reference lists */
PUBLIC char *		end_mark = END_MARK;   	  /* Format string for [End] */
 
/* Type definitions and global variables local to this module */
PRIVATE HTRequest *	request = NULL;
PRIVATE	HTList *	reqlist = NULL;		  /* List of active requests */
PRIVATE HTParentAnchor*	home_anchor = NULL;	    /* First document anchor */
PRIVATE char		keywords[ADDRESS_LENGTH];       /* From command line */
PRIVATE char *		output_file_name = NULL;		  /* -o xxxx */
PRIVATE char		choice[RESPONSE_LENGTH];	  /* Users response  */
PRIVATE char *		refhead = DEFAULT_REF_HEAD;	 /* Ref list heading */
PRIVATE char *		logfile_root = NULL;	 	    /* Log file name */
PRIVATE BOOL		filter = NO;		      	 /* Load from stdin? */
PRIVATE BOOL		reformat_html = NO;		   /* Reformat html? */
PRIVATE BOOL		listrefs_option = NO;	  /* -listrefs option used?  */
PRIVATE BOOL		OutSource = NO;		    /* Output source, YES/NO */
PRIVATE char *		HTLogFileName = NULL;  	    /* Root of log file name */
PRIVATE int		OldTraceFlag = SHOW_ALL_TRACE;
PRIVATE HTList *	conversions = NULL;	       /* Format conversions */
PRIVATE BOOL		UseMulti = YES;			/* Use multithreaded */

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

/* ------------------------------------------------------------------------- */
/*				HELP FUNCTIONS				     */
/* ------------------------------------------------------------------------- */

#ifdef GET_SCREEN_SIZE
#include <sys/ioctl.h>
/*
** Get size of the output screen. Stolen from less.
*/
PRIVATE void scrsize ARGS2(int *, p_height, int *, p_width)
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

#ifdef CATCH_SIG
#include <signal.h>
/*								    SetSignal
**  This function sets up signal handlers. This might not be necessary to
**  call if the application has its own handlers.
*/
PRIVATE void SetSignal NOARGS
{
    /* On some systems (SYSV) it is necessary to catch the SIGPIPE signal
    ** when attemting to connect to a remote host where you normally should
    ** get `connection refused' back
    */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
	if (PROT_TRACE) fprintf(stderr, "HTSignal.... Can't catch SIGPIPE\n");
    } else {
	if (PROT_TRACE) fprintf(stderr, "HTSignal.... Ignoring SIGPIPE\n");
    }
}
#endif /* CATCH_SIG */


/*		Display Help screen	 		 Help_screen
**		-------------------
** Produce a help screen, displaying the current document address and
** a list of * available commands *in this context*.  * ?? Perhaps this
** should be a hypertext page, not included in the history list.
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
	    
    if (UseMulti) {
	printf("  z               Interrupt a request\n");
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
    int cnt;
    int refs = HText_sourceAnchors(HTMainText);
    if (refs <= 0) {
	fprintf(output, "\n\nThere are no references from this document.\n\n");
    } else {
	fprintf(output, "\n%s\n", refhead);
	for (cnt=1; cnt<=refs; cnt++) {
	    HTAnchor *dest =
		HTAnchor_followMainLink((HTAnchor *)
					HText_childNumber(HTMainText, cnt));
	    HTParentAnchor * parent = HTAnchor_parent(dest);
	    char * address =  HTAnchor_address(dest);
	    CONST char * title = titles ? HTAnchor_title(parent) : NULL;
	    fprintf(output, reference_mark, cnt);
	    fprintf(output, "%s%s\n",
		    ((HTAnchor*)parent!=dest) && title ? "in " : "",
		    (char *)(title ? title : address));
	    free(address);
	}
	fflush(stdout);
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


/*								MakeCommandLine
**
**	Generate the Prompt line and flush it to the user
*/
PRIVATE void MakeCommandLine ARGS1(BOOL, is_index)
{
    int length_of_prompt = 0;

#ifndef VM	/* Normal prompt */
    if (is_index){	
	printf("FIND <keywords>, ");
	length_of_prompt = length_of_prompt + 14;
    }
    if (HTAnchor_hasChildren(HTMainAnchor)!=0){
	int refs = HText_sourceAnchors(HTMainText);
	if (refs>1) {
	    printf("1-%d, ", refs);
	    length_of_prompt = length_of_prompt + 6;		  /* Roughly */
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

    fflush(stdout);  	           	  /* For use to flush out the prompt */
    return;
}

/*									 ErrMsg
**
**   General Error Message for Line Mode Browser. If verbose mode is on, then
**   the message is passed to stderr.
**
*/
PRIVATE void ErrMsg ARGS2(char *, Msg, char *, Str)
{
    if (TRACE || HTInteractive)
	fprintf(stderr, "Warning: %s (%s)\n", Msg, Str ? Str : "");
}

/* ------------------------------------------------------------------------- */
/*				EVENT FUNCTIONS				     */
/* ------------------------------------------------------------------------- */

/*
**  This function creates a new request structure and adds it to the global
**  list of active threads
*/
PRIVATE HTRequest *Thread_new ARGS1(BOOL, Interactive)
{
    HTRequest *newreq = HTRequest_new(); 	     /* Set up a new request */
    if (!reqlist)
	reqlist = HTList_new();
    if (Interactive) {
	HTList_delete(newreq->conversions);
	newreq->conversions = conversions;	    /* Take from global list */
    } else
	HTFormatInitNIM(newreq->conversions);
    if (!UseMulti)
	request->BlockingIO = YES;			 /* Use blocking I/O */
    HTList_addObject(reqlist, (void *) newreq);
    return newreq;
}


/*
**  This function deletes a request structure and takes it out of the list
**  of active threads.
*/
PRIVATE void Thread_delete ARGS1(HTRequest *, oldreq)
{
    if (oldreq) {
	if (reqlist)
	    HTList_removeObject(reqlist, (void *) oldreq);
	if (oldreq->conversions == conversions)
	    oldreq->conversions = NULL;	    /* We keep them in a global list */
	HTRequest_delete(oldreq);
    }
}


/*
**  This function deletes the whole list of active threads.
*/
PRIVATE void Thread_deleteAll NOARGS
{
    BOOL first=YES;	      /* We only have one global list of conversions */
    if (reqlist) {
	HTList *cur = reqlist;
	HTRequest* pres;
	while ((pres = (HTRequest *) HTList_nextObject(cur)) != NULL) {
	    if (first) {
		HTRequest_delete(pres);
		first = NO;
	    } else {
		pres->conversions = NULL;
		HTRequest_delete(pres);
	    }
	}
	HTList_delete(reqlist);
	reqlist = NULL;
    }
}


/*
**  This function puts up a stream to a file in order to save a document. This
**  is activated by '>', '>>' or '>!' from the prompt line.
**  Returns:
**	YES if load terminated (either success or failure)
**	 NO if error or operation blocked
**
**  Henrik Frystyk 02/03-94
*/
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
    if (TRACE)
	fprintf(stderr, "Saving to file %s\n", fname);
    {
	int status;
	HTRequest *req = Thread_new(NO); 	     /* Set up a new request */
	if (OutSource)
	    req->output_format = WWW_SOURCE;
	req->output_stream = HTFWriter_new(fp, NO);
	HTForceReload = YES;
	status = HTLoadAnchor((HTAnchor*) HTMainAnchor, req);
	HTForceReload = NO;
	return (status != HT_WOULD_BLOCK);
    }
}


/*								   EventHandler
**
**	Read in the user's input, and deal with it as necessary.
**
**	Any Command which works returns from the routine. If nothing
**	works then a search or error message down at the bottom.
*/
PUBLIC HTEventState EventHandler ARGS1(HTRequest **, actreq)
{ 
    int  ref_num;
    HTEventState status = EVENT_OK;
    char * the_choice = 0;		           /* preserved user command */
    char * this_word = 0;        	   	    /* First word of command */
    char * this_command;	       	          /* this_word and following */
    char * next_word;			                      /* Second word */
    char * other_words;				/* Second word and following */
    BOOL is_index = HTAnchor_isIndex(HTMainAnchor);
    BOOL found = YES;
    int loadstat = HT_INTERNAL;

    if (!fgets(choice, RESPONSE_LENGTH, stdin))		  /* Read User Input */
	return EVENT_QUIT;				      /* Exit if EOF */
    
    StrAllocCopy (the_choice, choice);		       /* Remember it as is, */
    if (the_choice[strlen(the_choice)-1] == '\n')            /* The final \n */
	the_choice[strlen(the_choice)-1] = '\0';
    
#ifdef VM  /* Clear the screen (on screen-mode systems) */
    clear_screen();
#endif	
    this_word = strtok (choice, " \t\n\r");	      /* Tokenize user input */
    this_command = the_choice;
    if (this_word) {
	next_word = strtok (NULL, " \t\n\r");
	other_words = the_choice + (next_word - choice);
    }
    else
	goto down;				/* Empty input : scroll down */
    
    /* Process Command */
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
	sscanf(this_word,"%d",&ref_num);
	if (ref_num>0 && ref_num<=HText_sourceAnchors(HTMainText)) {
	    HTAnchor *destination;
	    HTChildAnchor *source = HText_childNumber(HTMainText, ref_num);
	    if (source) {
		*actreq = Thread_new(YES);
		destination = HTAnchor_followMainLink((HTAnchor*) source);
		(*actreq)->parentAnchor = HTAnchor_parent((HTAnchor *) source);
		loadstat = HTLoadAnchor(destination, *actreq);
	    } else {
		status = EVENT_QUIT;				/* No anchor */
	    }
	} else {
	    if (TRACE || HTInteractive)
		fprintf(stderr, "Warning: Invalid Reference Number: (%d)\n",
			ref_num);
	}
	break;
	
      case 'B':		
	if (Check_User_Input("BACK")) {		  /* Return to previous node */
	    if (HTHistory_canBacktrack()) {
		*actreq = Thread_new(YES);
		loadstat = HTLoadAnchor(HTHistory_backtrack(), *actreq);
	    } else {
		printf("\n  The BACK command cannot be used,");
		printf(" as there are no previous documents\n"); 
	    }
	} else if (Check_User_Input("BOTTOM")) {	/* Scroll to bottom  */
	    HText_scrollBottom(HTMainText);
	} else
	    found = NO;
	break;
	
#ifdef unix
      case 'C':
	if (Check_User_Input("CD")) {	       /* Change working directory ? */
	    goto lcd;
	} else
	    found = NO;
	break;
#endif
	
      case 'D':
	if (Check_User_Input("DOWN")) {		    /* Scroll down one page  */
	  down:
	    if (HText_canScrollDown(HTMainText))
		HText_scrollDown(HTMainText);
	} else
	    found = NO;
	break;
	
      case 'E':			       /* Quit program ? Alternative command */
	if (Check_User_Input("EXIT")) {
	    status = EVENT_QUIT;
	} else
	    found = NO;
	break;
	
      case 'F':						 /* Keyword search ? */
	if (is_index && Check_User_Input("FIND")) {
	  find:
	    {
		if (next_word) {
		    *actreq = Thread_new(YES);
		    loadstat = HTSearch(other_words, HTMainAnchor, *actreq);
		}
	    }
	} else
	    found = NO;
	break;
	
      case 'G':
	if (Check_User_Input("GOTO")) {				     /* GOTO */
	    if (next_word) {
		*actreq = Thread_new(YES);
		loadstat = HTLoadRelative(next_word, HTMainAnchor, *actreq);
	    }
	} else
	    found = NO;
	break;
	
      case '?':
	help_screen();
	break;
	
      case 'H':
	if (Check_User_Input("HELP")) {			     /* help menu, ..*/
	    help_screen();			  /*!! or a keyword search ? */
	} else if (Check_User_Input("HOME")) {			/* back HOME */
	    if (!HTHistory_canBacktrack()) {
		HText_scrollTop(HTMainText);
	    } else {
		*actreq = Thread_new(YES);
		loadstat = HTLoadAnchor(HTHistory_recall(1), *actreq);
	    }
	} else
	    found = NO;
	break;
	
      case 'K':						 /* Keyword search ? */
	if (is_index && Check_User_Input("KEYWORDS")) {
	    goto find;
	} else
	    found = NO;
	break;
	
      case 'L':
	if (Check_User_Input("LIST")) {		     /* List of references ? */
	    Reference_List(!OutSource);
	}
#ifdef unix
	else if (Check_User_Input ("LCD")) {	       /* Local change dir ? */
	  lcd:
	    if (!next_word) {				 /* Missing argument */
		printf ("\nName of the new local directory missing.\n");
	    } else if (chdir (next_word)) {		 /* failed : say why */
		fprintf (stderr, "\n  ");
		perror (next_word);
	    } else {		    /* Success : display new local directory */
		/* AS Sep 93 */
#ifdef NO_GETWD     /* No getwd() on this machine */
#ifdef HAS_GETCWD   /* System V variant SIGN CHANGED TBL 921006 !! */
		printf ("\nLocal directory is now:\n %s\n",
			getcwd (choice, sizeof(choice)));
#else   /* has NO getcwd */
		ErrMsg("This platform does not support getwd() or getcwd()",
		       NULL);
#endif	/* has no getcwd */
#else   /* has getwd */
		printf("\nLocal directory is now:\n %s\n",
		       (char *) getwd (choice));
#endif  /* has getwd */
		/* End AS Sep 93 */
	    }
	}
#endif
	else
	    found = NO;
	break;
	
      case 'M':
	if (Check_User_Input("MANUAL")) {		 /* Read User manual */
	    *actreq = Thread_new(YES);
	    loadstat = HTLoadRelative(MANUAL, HTMainAnchor,*actreq);
	} else
	    found = NO;
	break;
	
      case 'N':                    
	if (Check_User_Input("NEXT")) {
	    if (!HTHistory_canMoveBy(1)) {	 /* No nodes to jump back to */
		printf("\n  Can't take the NEXT link from the last");
		if (!HTHistory_canBacktrack())
		    printf(" document as there is no last");
		printf(" document.\n");
	    } else {
		*actreq = Thread_new(YES);
		loadstat = HTLoadAnchor(HTHistory_moveBy(1),*actreq);
	    }
	} else
	    found = NO;
	break;
	
      case 'P':                    
	if (Check_User_Input("PREVIOUS")) {
	    if (!HTHistory_canMoveBy(-1)){ 
		printf("\n  Can't take the PREVIOUS link from the last");
		if (!HTHistory_canBacktrack())
		    printf(" document as there is no last");
		printf(" document.\n");
	    } else {
		*actreq = Thread_new(YES);
		loadstat = HTLoadAnchor(HTHistory_moveBy(-1), *actreq);
	    }
	}
#ifdef GOT_SYSTEM	    
	else if (!HTClientHost && Check_User_Input("PRINT")) {
	    char * address = HTAnchor_address((HTAnchor *) HTMainAnchor);
	    char * command;
	    char * tmplate = (char*)getenv("WWW_PRINT_COMMAND");
	    int result;
	    
	    if (!tmplate) tmplate = "www -n -na -p66 '%s' | lpr";
	    command = (char *) malloc(strlen(address)+strlen(tmplate)+20);
	    sprintf(command, tmplate, address);
	    result = system(command);
	    free(address);
	    free(command);
	    if (result) printf("  %s\n  returns %d\n", command, result);
	}
#endif
	/* this command prints the entire current text to the
	   terminal's printer; at the end it displays the top of the text */
#ifdef SLAVE_PRINTER
#define SLAVE_PRINTER_ON  "\033\133\065\151"
#define SLAVE_PRINTER_OFF "\033\133\064\151"
	
	else if (Check_User_Input("PS")) {
	    printf ("%s",SLAVE_PRINTER_ON);
	    printf("\f");			   /* Form feed for new page */
	    HText_scrollTop(HTMainText);
	    while(HText_canScrollDown(HTMainText)) {
		HText_scrollDown(HTMainText);
	    }
	    printf("\f");  /* Form feed for new page */
	    printf ("%s",SLAVE_PRINTER_OFF);
	    HText_scrollTop(HTMainText);
	}	
#endif
	else
	    found = NO;
	break;
	
      case 'Q':						   /* Quit program ? */
	if (Check_User_Input("QUIT")) {

	    /* 	JFG 9/7/92, following a complaint of 'q' mis-typed for '1'.
		JFG Then made optional because I hate it !!!
		TBL made it only affect remote logged on users. 921122 */
	    
	    if (HTClientHost && (strcasecomp(this_word, "quit") != 0) ) {
		printf ("\n Please type \"quit\" in full to leave www.\n");
	    } else
		status = EVENT_QUIT;
	} else
	    found = NO;
	break;
	
      case 'R':	
#ifdef VM
	if (Check_User_Input("RETURN"))		       /* Means quit program */
	    status = EVENT_QUIT;
	else
#endif	    
	if (Check_User_Input("RECALL")) {
	    int  recall_node_num;
	    
	    if (!HTHistory_canBacktrack()) {	       /* No nodes to recall */
		printf("\n  No other documents to recall.\n");
	    } else {
		/* Previous node number exists, or does the user just */
		/* require a list of nodes visited? */
		if (next_word) {
		    if ((recall_node_num = atoi(next_word)) > 0) {
			*actreq = Thread_new(YES);
			loadstat =
			    HTLoadAnchor(HTHistory_recall(recall_node_num),
					 *actreq);
		    } else {
			ErrMsg("Bad command, for list of commands type help.",
			       this_command);
		    }
		} else {
		    History_List();
		}
	    }
	} else if (Check_User_Input("REFRESH")) {
	    HText_setStale(HTMainText);			    /* Force refresh */
	    HText_refresh(HTMainText);			   /* Refresh screen */
	} else
	    found = NO;
	break;
	
      case 'S':						       /* TBL 921009 */
	if (Check_User_Input("SOURCE")) {     	  	  /* Apply to source */
	    if (next_word) {
		OutSource = YES;		 /* Load and print as source */
		this_word = next_word;		         /* Move up one word */
		next_word = strtok (NULL, " \t\n\r");
		this_command = the_choice + (this_word - choice);
		other_words = the_choice + (next_word - choice);
		goto loop;	                       /* Go treat as before */
	    }
	} else if (Check_User_Input("SET")) {          	           /* config */
	    HTSetConfiguration(other_words);
	} else
	    found = NO;
	break;
	
      case 'T':
	if (Check_User_Input("TOP")) {			   /* Return to top  */
	    HText_scrollTop(HTMainText);
	} else
	    found = NO;
	break;
	
      case 'U':
	if (Check_User_Input("UP")) {		      /* Scroll up one page  */
	    HText_scrollUp(HTMainText);
	} else
	    found = NO;
	break;
	
      case 'V':
	if (Check_User_Input("VERBOSE")) {	     /* Switch verbose mode  */
	    WWW_TraceFlag = WWW_TraceFlag ? 0 : OldTraceFlag;
	    printf ("\n  Verbose mode %s.\n", WWW_TraceFlag ? "ON":"OFF");
	} else
	    found = NO;
	break;
	
      case 'Z':
	HText_setStale(HTMainText);			    /* Force refresh */
	HText_refresh(HTMainText);			   /* Refresh screen */
	status = EVENT_INTR_ALL;
	break;

      case '>':
	if (!HTClientHost) {
	    if (SaveOutputStream(this_word, next_word))
		loadstat = HT_WOULD_BLOCK;
	}
	break;
	
#ifdef GOT_PIPE
      case '|':
	if (!HTClientHost) {	             	           /* Local only!!!! */
	    char * address = HTAnchor_address((HTAnchor *) HTMainAnchor);
	    char * command;
	    int result;
	    command = (char*) malloc(strlen(address) +strlen(this_command)+30);
	    
#ifdef VM	
	    sprintf(command, "PIPE CMS WWW %s \"%s\" | %s",
		    OutSource ? "-source" : "-n -na -p", address,this_command);

#else	
	    sprintf(command, "www %s \"%s\" %s", 
		    OutSource ? "-source" : "-n -na -p", address,this_command);
#endif
	    printf("Command: %s\n", command);
	    result = system(command);
	    if (result)
	        printf("  %s  returns %d\n", command, result);
	    free(command);
     	    free(address);
	}
	break;
#endif
	    
#ifdef GOT_SYSTEM
      case '!':
	if (!HTClientHost) {				      /* Local only! */
	    int result;
	    ErrMsg("Executing", this_command);
	    result = system(strchr(this_command, '!') + 1);
	    if (result) printf("  %s  returns %d\n",
			       strchr(this_command, '!') + 1, result);
	}
	break;
#endif
      default:
	found = NO;
	break;
    } /* Switch on 1st character */

    if (!found) {
	if (is_index && *this_word) {  /* No commands, search keywords */
	    next_word = other_words = this_command;
	    found = YES;
	    goto find;
	} else {             
	    ErrMsg("Bad command, for list of commands type help.",
		   this_command);
	}
    }
    if (loadstat != HT_INTERNAL && loadstat != HT_WOULD_BLOCK)
	status = EVENT_TERM;
    if (loadstat != HT_LOADED && loadstat != HT_ERROR)
	MakeCommandLine(is_index);
    free (the_choice);
    return status;
}


/*						        HTEventRequestTerminate
**
**	React to the status of the terminated request
*/
PUBLIC HTEventState HTEventRequestTerminate ARGS2(HTRequest *,	actreq,
						  int,		status) 
{
    BOOL is_index = HTAnchor_isIndex(HTMainAnchor);
    if (status == HT_LOADED) {
	if (actreq->parentAnchor) {
	    HTParentAnchor *parent = actreq->parentAnchor;
	    HTParentAnchor *destination = actreq->anchor;
	    HTHistory_leavingFrom((HTAnchor *) parent);
	    HTHistory_record((HTAnchor *) destination);
	    actreq->parentAnchor = NULL;
	}
	/* Now generate the new prompt line as a function of the result */
	if (!HText_canScrollDown(HTMainText) &&
	    !HTAnchor_hasChildren(HTMainAnchor) && !is_index &&
	    (!HTHistory_canBacktrack())) {
	    ErrMsg("No way out of here, so I exit!", NULL);	    
	    return EVENT_QUIT;	                 /* Exit if no other options */
	}
	HText_setStale(HTMainText);		   /* We corrupt the display */
	MakeCommandLine(is_index);
    } else if (!HTMainText)			      /* If first try failed */
	return EVENT_QUIT;
    else
	MakeCommandLine(is_index);
    Thread_delete(actreq);
    return EVENT_OK;
}

/* ------------------------------------------------------------------------- */
/*				  MAIN PROGRAM				     */
/* ------------------------------------------------------------------------- */

int main ARGS2(int, argc, char **, argv)
{
    int		return_status = 0;	
    int		arg;			       /* Argument number as we scan */
    BOOL	argument_found = NO;
    BOOL	logfile_flag = NO;
    BOOL	first_keyword = YES;
    char *	default_default = HTFindRelatedName();
    HTFormat	input_format = WWW_HTML;	         /* Used with filter */
    HTEventCallBack user;		/* To register STDIN for user events */

#ifdef THINK_C /* command line from Think_C */
    int i;
    argc=ccommand(&argv);
#endif
    
    /* HWL 18/7/94: applied patch from
       agl@glas2.glas.apc.org (Anton Tropashko) */
#ifdef CYRILLIC
    arc.locale=0; arc.encoding=0; arc.i_encoding=0; doinull();
#endif

#ifdef VMS
    output = stdout;
#endif /* VMS */
	
    request =  HTRequest_new();

    /* Check for command line options */
    *keywords = '\0';
    for (arg=1; arg<argc ; arg++) {
	if (*argv[arg] == '-') {

	    /* - alone => filter */
	    if (argv[arg][1] == 0) {
		filter = YES;	   
		HTInteractive = NO;     /* From stdin, Force non-interactive */
	    
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

#ifdef CYRILLIC
	    /* HWL 18/7/94: applied patch from agl@glas2.glas.apc.org
	       (Anton Tropashko) */
	    } else if (!strcmp(argv[arg], "-koi2alt")) {
	        doia2k(); printf("Ahak2a!");
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
		    HTInteractive = NO;
		    reformat_html = YES;

	    /* Specify a cache root (caching is otherwise disabled) */
	    } else if (!strcmp(argv[arg], "-cacheroot")) {
		StrAllocCopy(HTCacheDir,
			     (arg+1>=argc || *argv[arg+1]=='-') ?
			     CACHE_HOME_DIR : argv[++arg]);

	    /* to -- Final representation */
	    } else if (!strcmp(argv[arg], "-to")) {
		request->output_format =
		    (arg+1 >= argc || *argv[arg+1] == '-') ?
		    	WWW_PRESENT : 
		    	HTAtom_for(argv[++arg]);
		    HTInteractive = NO;			/* JFG */

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
		HTInteractive = NO;			/* non-interactive */

	    /* Method Used (Currently on GET and HEAD are supported) */
	    } else if (!strcmp(argv[arg], "-m")) {
		HTInteractive = NO;
		if (++arg < argc) {
		    if (!strcasecomp(argv[arg], "head")) {
			request->method = METHOD_HEAD;
			request->output_format = WWW_MIME;
		    } else if (!strcasecomp(argv[arg], "get"))
			request->method = METHOD_GET;
		    else
			request->method = METHOD_INVALID;
		}

	    /* Non-interactive */
	    } else if (!strcmp(argv[arg], "-n")) {
		HTInteractive = NO;

	    /* Multithreaded ot not? */
	    } else if (!strcmp(argv[arg], "-single")) {
		UseMulti = NO;

	    /* Output filename */
	    } else if (!strcmp(argv[arg], "-o")) { 
		output_file_name = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_OUTPUT_FILE;
		    HTInteractive = NO;
		    
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
			ErrMsg("Bad parameter for directory listing -d option",
			       argv[arg]);
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

#ifdef TRACE
	   /* Verify: Turns on trace */
	    } else if (!strncmp(argv[arg], "-v", 2)) {
	    	char *p = argv[arg]+2;
		WWW_TraceFlag = 0;
		for(; *p; p++) {
		    switch (*p) {
		      case 'a': WWW_TraceFlag += SHOW_ANCHOR_TRACE; break;
		      case 'p':	WWW_TraceFlag += SHOW_PROTOCOL_TRACE; break;
		      case 's':	WWW_TraceFlag += SHOW_SGML_TRACE; break;
		      case 't':	WWW_TraceFlag += SHOW_THREAD_TRACE; break;
		      case 'u': WWW_TraceFlag += SHOW_URI_TRACE; break;
		      default:
			ErrMsg("Bad parameter for verbose mode", argv[arg]);
		    }
		}/* loop over characters */
		if (!WWW_TraceFlag)
		    WWW_TraceFlag = SHOW_ALL_TRACE;
		OldTraceFlag = WWW_TraceFlag;		 /* Remember setting */
#endif
	    
	    /* Source please */
	    } else if (!strcmp(argv[arg], "-source")) {
		    request->output_format = WWW_SOURCE;
		    HTInteractive = NO;			/* JFG */

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
	} else {	   /* If no leading `-' then check for main argument */
    	    if (!argument_found) {
		char * ref = HTParse(argv[arg], default_default, PARSE_ALL);
		home_anchor = (HTParentAnchor*) HTAnchor_findAddress(ref);
		free(ref);
		argument_found = YES;
	    } else {		   /* Check for successive keyword arguments */
		if (first_keyword) {			/* Collect keywords */
		    first_keyword = NO;
		} else {
		    strcat(keywords, " ");
		}
		strcat(keywords,HTStrip(argv[arg]));
	    } /* argument already found */
	} /* Not an option '-'*/
    } /* End of argument loop */

    /* Initialization */
    HTLibInit();
    if (HTClientHost) HTSecure = YES;		   /* Access to local files? */
    HTEnableFrom = YES;			     /* Send `From:' in the request? */
#ifdef CATCH_SIG
    SetSignal();
#endif
    if (HTScreenHeight == -1) {				/* Default page size */
	if (HTInteractive) {
#ifdef GET_SCREEN_SIZE
	    int scr_height, scr_width;
	    scrsize(&scr_height, &scr_width);
	    HTScreenHeight = scr_height;
#else
	    HTScreenHeight = SCREEN_HEIGHT;
#endif
	} else
	    HTScreenHeight = 999999;
    }

    /* Siable free directory browsing when using telnet host */
    if (HTClientHost && HTDirAccess==HT_DIR_OK)
	HTDirAccess = HT_DIR_SELECTIVE;

#ifndef NO_RULES
    {
    	char * rules = getenv("WWW_CONFIG");
	if (rules && HTLoadRules(rules) < 0) {
	    ErrMsg("Cant't open rule file", rules);
	}
    }
#endif

    /* Force predefined presentations etc to be set up */
    if (HTInteractive) {
	conversions = HTList_new();
	HTFormatInit(conversions);
	HTList_delete(request->conversions);
	request->conversions = conversions;
    } else
	HTFormatInitNIM(request->conversions);

    /* Open output file */
    if (!HTInteractive)	{
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
    
    /* Open Log File. Logfile via Telnet is now optional (HENRIK 11/02-94) */
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

    /* Make home page address */
    if (!home_anchor)
	home_anchor = HTHomeAnchor();

    /* Now we are ready to start the program! If in interactive mode then start
       the event loop which will run until the program terminates */
    if (filter) {			             /* Just convert formats */
	HTBindAnchor((HTAnchor*)home_anchor, request);
     	HTParseSocket(input_format, 0, request);      /* From std UNIX input */
	    goto endproc;
    }
    if (HTInteractive) {
	reqlist = HTList_new();
	user.sockfd = STDIN_FILENO;
	user.callback = EventHandler;
	HTList_addObject(reqlist, (void *) request);
	HTEventRegister(&user);				   /* Register STDIN */
	HTHistory_record((HTAnchor *) home_anchor);	    /* Setup history */
	return_status = HTEventLoop(request, home_anchor,
				    (keywords && *keywords) ? keywords : NULL);
    } else {
	request->BlockingIO = YES;		/* Turn off non-blocking I/O */
	if (((keywords && *keywords) ?
	     HTSearch(keywords, home_anchor, request) :
	     HTLoadAnchor((HTAnchor*) home_anchor, request)) != HT_LOADED) {
	    char *addr = HTAnchor_address((HTAnchor *) home_anchor);
	    ErrMsg("Can't access document", addr);
	    free(addr);
	    if (!HTMainText) {
		return_status = -2;		     /* Can't get first page */
	    }
	} else if (listrefs_option) {
	    Reference_List(NO);		   	      /* List without titles */
	}
    }

endproc:
    Thread_deleteAll();
    if (default_default)
	free(default_default);
    HTLibTerminate();
    printf("\n");
    if (!return_status) {			/* Everything is working :-) */
#ifdef VMS 
	return 1;
#else
	return 0;	/* Good */
#endif
    }
    return return_status;		       /* An error occured somewhere */
}

/* End HTBrowse.c */

