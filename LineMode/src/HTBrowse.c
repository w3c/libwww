/*         		    					     HTBrowse.c
**	HYPERTEXT BROWSER FOR DUMB TERMINALS
**
**	(c) COPRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**  Authors:
**	NP:  Nicola Pellow  Tech.Student CERN 1990-91
**	TBL: Tim Berners-Lee CERN (timbl@w3.org)
**	JFG: Jean-Francois Groff, Cooperant CERN 1991-92 (jfg@dxcern.cern.ch)
**	DR:  Dudu Rashty +972-2-584848 <RASHTY@hujivms.bitnet>
**	MD:  Mark Donszelmann, DELPHI CERN, (duns@vxdeop.cern.ch)
**	HFN: Henrik Frystyk Nielsen, CERN, (frystyk@w3.org)
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

#include "WWWLib.h"			      /* Global Library Include file */
#include "HTBrowse.h"			     /* Things exported, short names */
#include "GridText.h"				     /* Hypertext definition */

/* Macros and other defines */
#ifndef VL
#define VL "unspecified"
#endif

/* Default page for "Manual" command */
#define MANUAL	"http://www.w3.org/hypertext/WWW/LineMode/Defaults/QuickGuide.html"

/* Default page for "help" command */
#define C_HELP	"http://www.w3.org/hypertext/WWW/LineMode/Defaults/Commands.html"

/* Default page for "-help" command line option */
#define L_HELP	"http://www.w3.org/hypertext/WWW/LineMode/Defaults/CommandLine.html"

#define DEFAULT_OUTPUT_FILE	"www.out"
#define DEFAULT_RULE_FILE	"www.conf"
#define DEFAULT_LOG_FILE       	"www.log"

#define PROMPT			"%s"
#define REF_MARK		"[%d]"
#define END_MARK		"[END]"

#define SCREEN_WIDTH		79  	      /* Default width of the screen */
#define MIN_SCREEN_WIDTH	10 
#define MAX_SCREEN_WIDTH	150	 
#define SCREEN_HEIGHT		24 		  /* Default number of lines */
#define MIN_SCREEN_HEIGHT	5 
#define MAX_SCREEN_HEIGHT	200	 

#define MAX_HISTORY		20	/* Don't list more than this in list */

#define ADDRESS_LENGTH		512	   /* Maximum address length of node */
#define RESPONSE_LENGTH		1024     /* Maximum length of users response */

#define SHOW_MSG		(TRACE || HTPrompt_interactive())
#define CHECK_INPUT(a, b)	(!strncasecomp ((a), (b), strlen((b))))

#if defined(ultrix) || defined(__osf__)
#define GET_SCREEN_SIZE
#endif

#if defined(__svr4__)
#define CATCH_SIG
#endif

/* HWL 18/7/94: applied patch from agl@glas2.glas.apc.org (Anton Tropashko) */
#ifdef CYRILLIC
#include "a_stdio.h"
struct ARc arc;
#endif

/* These must be defined as they are used in the Library */
PUBLIC char *		HTAppName = "LineModeBrowser";   /* Application name */
PUBLIC char *		HTAppVersion = VL;            /* Application version */

/* Screen size parameters */
PUBLIC int		HTScreenWidth   = SCREEN_WIDTH;	       /* By default */
PUBLIC int		HTScreenHeight  = -1;		   /* -1 = Undefined */

/* Anchor specific information */
PUBLIC BOOL		display_anchors = YES;	    /* Show anchors in text? */
PUBLIC char *		start_reference = NULL;   /* Format for start anchor */
PUBLIC char *		end_reference = REF_MARK;	   /* for end anchor */
PUBLIC char *		reference_mark = "[%d] ";     /* for reference lists */
PUBLIC char *		end_mark = END_MARK;   	  /* Format string for [End] */
 
/* Type definitions and global variables local to this module */
typedef enum _HTHistState {
    HT_HIST_UPDATE = 0,
    HT_HIST_NO_UPDATE,
    HT_HIST_NP
} HTHistState;

PRIVATE BOOL		Blocking = NO;			/* Use multithreaded */
PRIVATE	HTList *	reqlist = NULL;		  /* List of active requests */
PRIVATE HTParentAnchor*	home_anchor = NULL;	    /* First document anchor */
PRIVATE int		OldTraceFlag = SHOW_ALL_TRACE;
PRIVATE FILE *	        output = stdout;	   /* Destination for output */

/* History Management */
PRIVATE HTHistory *	hist = NULL;			     /* History list */

/* Context Swapping */
typedef struct _HTContext {
    HTHistState	history;				 /* State of history */
} HTContext;

#ifdef VMS
#ifdef __DECC
/* dummy declarations to make sure that LINKER will not complain */
PUBLIC char *HTBinDir;
PUBLIC int HTDiag;
PUBLIC char *HTPostScript;
PUBLIC char *HTPutScript;
PUBLIC char *HTSearchScript;
#endif /* DECC */
#endif /* not VMS */ 

#include "HTReqMan.h"

/* ------------------------------------------------------------------------- */
/*				THREAD FUNCTIONS			     */
/* ------------------------------------------------------------------------- */

/*  Create a new context structure
**  ------------------------------
**  Creates a new context structure for keeping track of the context related
**  to a specific request.
*/
PRIVATE HTContext *HTContext_new NOARGS
{
    HTContext *me = (HTContext *) calloc(1, sizeof (HTContext));
    if (!me) outofmem(__FILE__, "HTContext_new");
    me->history = HT_HIST_UPDATE;		      /* Add to history list */
    return me;
}

/*  Delete a context structure
**  --------------------------
**  Creates a new context structure for keeping track of the context related
**  to a specific request.
*/
PRIVATE BOOL HTContext_delete ARGS1(HTContext *, old)
{
    FREE(old);
    return YES;
}

/*
**  This function creates a new request structure and adds it to the global
**  list of active threads
*/
PRIVATE HTRequest *Thread_new ARGS1(BOOL, Interactive)
{
    HTRequest *newreq = HTRequest_new(); 	     /* Set up a new request */
    if (!reqlist) reqlist = HTList_new();
    HTRequest_setContext(newreq, (void *) HTContext_new());
    if (Interactive)
	HTPresenterInit(newreq->conversions);		/* Set up local list */
    if (Blocking)
	newreq->preemtive = YES;			 /* Use blocking I/O */
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
	HTContext_delete((HTContext *) oldreq->context);
	if (reqlist)
	    HTList_removeObject(reqlist, (void *) oldreq);
	HTRequest_delete(oldreq);
    }
}


/*
**  This function deletes the whole list of active threads.
*/
PRIVATE void Thread_deleteAll NOARGS
{
    if (reqlist) {
	HTList *cur = reqlist;
	HTRequest* pres;
	while ((pres = (HTRequest *) HTList_nextObject(cur)) != NULL) {
	    HTRequest_delete(pres);
	}
	HTList_delete(reqlist);
	reqlist = NULL;
    }
}

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
	if (PROT_TRACE) fprintf(TDEST, "HTSignal.... Can't catch SIGPIPE\n");
    } else {
	if (PROT_TRACE) fprintf(TDEST, "HTSignal.... Ignoring SIGPIPE\n");
    }
}
#endif /* CATCH_SIG */


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
    int refs = HText_sourceAnchors(HTMainText);
    if (refs <= 0) {
	fprintf(output, "\n\nThere are no references from this document.\n\n");
    } else {
	int cnt;
	fprintf(output, "\n*** References from this document ***\n");
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
	fflush(output);
    }
}      


/*		History_List
**		------------
**	Display a history list of nodes visited during the session.
**	?? This should really be a hypertext page (not itself in history!).
**	?? Should have option to display address even when anchor has a title.
*/
PRIVATE void History_List NOARGS {

    int current = HTHistory_position(hist);
    int max = HTHistory_count(hist);
    int cnt;
    printf("\n  Documents you have visited: ");
    if (max > MAX_HISTORY) {
	max = MAX_HISTORY;
	printf("(truncated)\n");
    } else
	printf("\n");
    for (cnt=1; cnt<=max; cnt++) {
	HTAnchor *anchor = HTHistory_list(hist, cnt);
    	char *address = HTAnchor_address(anchor);
	HTParentAnchor *parent = HTAnchor_parent(anchor);
	CONST char *title = HTAnchor_title(parent);
	printf("%s R %d\t%s%s\n",
	       (cnt==current) ? "*" : " ",
	       cnt,
	       ((HTAnchor*)parent!=anchor) && title ? "in " : "",
	       title ? title : address);
	free(address);
    }
    printf("\n");
}


/*								MakeCommandLine
**
**	Generate the Prompt line and flush it to the user
*/
PRIVATE void MakeCommandLine ARGS1(BOOL, is_index)
{
    /* First Line */
    if (HTAnchor_hasChildren(HTMainAnchor)) {
	int refs = HText_sourceAnchors(HTMainText);
	if (refs>1)
	    printf("1-%d, ", refs);
	else
	    printf("1, ");	
    }
    if (HText_canScrollUp(HTMainText)) {
	printf(PROMPT, "Top, ");
	printf(PROMPT, "Up, ");
    }
    if (HText_canScrollDown(HTMainText)) {
	printf(PROMPT, "BOttom, ");
	printf(PROMPT, "Down or <RETURN> for more,");
    }

    /* Second Line */
    printf("\n");
    if (HTHistory_canBacktrack(hist))
	printf(PROMPT, "Back, ");
    if (HTHistory_canForward(hist))
	printf(PROMPT, "Forward, ");
    if (is_index)
	printf("FIND <keywords>, ");
    printf("Quit, or Help: ");	
    fflush(stdout);  	           	  /* For use to flush out the prompt */
    return;
}

/*  Print version information
**  -------------------------
*/
PRIVATE void VersionInfo NOARGS
{
    printf("\n\nW3C Reference Software\n\n");
    printf("\tW3C Line Mode Browser version %s.\n", VL);
    printf("\tW3C Reference Library version %s.\n\n",HTLibraryVersion);
    printf("Please send feedback to <www-bug@w3.org>\n");
}


/*
**  Upload a document either from local file or from a HTTP server
**  to a HTTP server. The method can be either PUT or POST.
**  Returns the result of the load function.
*/
PRIVATE int Upload ARGS2(HTRequest *, req, HTMethod, method)
{
    char *this_addr = HTAnchor_address((HTAnchor*) HTMainAnchor);
    char *sc, *de;
    int status = HT_INTERNAL;
    if ((sc = HTPrompt(req, "Source:", this_addr)) != NULL &&
	(de = HTPrompt(req, "Destination:", NULL)) != NULL) {
	BOOL confirm = YES;
	char *fd=HTParse(HTStrip(de), this_addr,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	char *fs=HTParse(HTStrip(sc), this_addr,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	HTParentAnchor *dest = (HTParentAnchor *) HTAnchor_findAddress(fd);
	HTParentAnchor *src = (HTParentAnchor *) HTAnchor_findAddress(fs);
	HTLink *link = HTAnchor_findLink((HTAnchor *) src, (HTAnchor *) dest);
	
	/* Now link the two anchors together if not already done */
	if (link) {
	    char *msg = (char *) malloc(128);
	    if (!msg) outofmem(__FILE__, "Upload");
	    sprintf(msg, "The destination is already related to the source with a %s method - result %d, continue?",
		    HTMethod_name(HTAnchor_linkMethod(link)),
		    HTAnchor_linkResult(link));
	    confirm = HTConfirm(req, msg);
	    free(msg);
	} else
	    HTAnchor_link((HTAnchor *) src, (HTAnchor *) dest, NULL, method);

	if (confirm) {
	    req = Thread_new(YES);		       /* This is the source */
	    status = HTCopyAnchor((HTAnchor *) src, req);
	}
	free(this_addr);
	free(sc);
	free(de);
	free(fd);
	free(fs);
    }
    return status;
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
PRIVATE BOOL SaveOutputStream ARGS3(HTRequest *, req, char *,This, char *,Next)
{
    FILE *fp;
    char *fname;
    char *fmode;
    
    /* Checks if file exists. Can be overruled by using '>!' */
    if (*(This+1) == '>') {	 		   	   /* Append to file */
	fmode = "ab";
	fname = *(This+2) ? (This+2) : Next;
    } else if (*(This+1) == '!') {
	fmode = "wb";				           /* Overwrite file */
	fname = *(This+2) ? (This+2) : Next;
    } else {						/* File name follows */
	fmode = "wb";
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
	if (SHOW_MSG) fprintf(TDEST, "Can't access file (%s)\n", fname);
	return NO;
    }
    req->output_stream = HTFWriter_new(fp, NO);

    /* Now, file is open and OK: reload the text and put up a stream for it! */
    if (TRACE)
	fprintf(TDEST, "Saving to file %s\n", fname);
    return (HTLoadAnchor((HTAnchor*) HTMainAnchor, req) != HT_WOULD_BLOCK);
}


/* ------------------------------------------------------------------------- */
/*				EVENT FUNCTIONS				     */
/* ------------------------------------------------------------------------- */

/*	scan_command
**	------------
**	Read in the user's input, and deal with it as necessary.
**	Any Command which works returns from the routine. If nothing
**	works then a search or error message down at the bottom.
**
**	Returns		HT_ERROR	Error has occured or we quit
**			HT_OK		Call back was OK
*/
PRIVATE int scan_command (SOCKET s, HTRequest * req, SockOps ops)
{ 
    char choice[RESPONSE_LENGTH];			  /* Users response  */
    char * the_choice=NULL;		           /* preserved user command */
    char * token=NULL;        	   	    	    /* First word of command */
    char * this_command;	       	              /* token and following */
    char * next_word;			                      /* Second word */
    char * other_words;				/* Second word and following */
    BOOL is_index = HTAnchor_isIndex(HTMainAnchor);
    BOOL found = YES;
    BOOL OutSource = NO;			    /* Output source, YES/NO */
    int status = YES;

    if (!fgets(choice, RESPONSE_LENGTH, stdin))		  /* Read User Input */
	return HT_ERROR;				      /* Exit if EOF */
    
    StrAllocCopy (the_choice, choice);		       /* Remember it as is, */
    if (the_choice[strlen(the_choice)-1] == '\n')            /* The final \n */
	the_choice[strlen(the_choice)-1] = '\0';
    
    token = strtok (choice, " \t\n\r");	      /* Tokenize user input */
    this_command = the_choice;
    if (token) {
	next_word = strtok (NULL, " \t\n\r");
	other_words = the_choice + (next_word - choice);
    }
    else
	goto down;				/* Empty input : scroll down */
    
    /* Process Command */
  loop:
    switch (TOUPPER(*token)) {
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
	{
	    int  ref_num;
	    sscanf(token,"%d",&ref_num);
	    if (ref_num>0 && ref_num<=HText_sourceAnchors(HTMainText)) {
		HTAnchor *destination;
		HTChildAnchor *source = HText_childNumber(HTMainText, ref_num);
		if (source) {
		    req = Thread_new(YES);
		    destination = HTAnchor_followMainLink((HTAnchor*) source);

		    /* Continous browsing, so we want Referer field */
		    req->parentAnchor=HTAnchor_parent((HTAnchor*)source);
		    status = HTLoadAnchor(destination, req);
		} else {
		    status = NO;				/* No anchor */
		}
	    } else {
		if (SHOW_MSG)
		    fprintf(TDEST,"Warning: Invalid Reference Number: (%d)\n",
			    ref_num);
	    }
	}
	break;
	
      case 'B':		
	if (CHECK_INPUT("BACK", token)) {	  /* Return to previous node */
	    if (HTHistory_canBacktrack(hist)) {
		req = Thread_new(YES);
		((HTContext *) req->context)->history = HT_HIST_NO_UPDATE;
		status = HTLoadAnchor(HTHistory_back(hist), req);
	    } else {
		printf("\nThis is the first document in history list\n");
	    }
	} else if (CHECK_INPUT("BOTTOM", token)) {	/* Scroll to bottom  */
	    HText_scrollBottom(HTMainText);
	} else
	    found = NO;
	break;
	
      case 'C':
#ifdef unix
	if (CHECK_INPUT("CD", token)) {        /* Change working directory ? */
	    goto lcd;
	} else
#endif
	if (CHECK_INPUT("CLEAR", token)) {	       /* Clear History list */
	    HTHistory_removeFrom(hist, 1);
	} else
	    found = NO;
	break;
	
      case 'D':
	if (CHECK_INPUT("DOWN", token)) {	    /* Scroll down one page  */
	  down:
	    if (HText_canScrollDown(HTMainText))
		HText_scrollDown(HTMainText);
	} else
	    found = NO;
	break;
	
      case 'E':
	if (CHECK_INPUT("EDIT", token)) {

	    /* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
	    
	} else if (CHECK_INPUT("EXIT", token)) {	    /* Quit program? */
	    status = NO;
	} else
	    found = NO;
	break;
	
      case 'F':						 /* Keyword search ? */
	if (is_index && CHECK_INPUT("FIND", token)) {
	  find:
	    {
		if (next_word) {
		    req = Thread_new(YES);
		    status = HTSearch(other_words, HTMainAnchor, req);
		}
	    }
	} else if (CHECK_INPUT("FORWARD", token)) {
	    if (HTHistory_canForward(hist)) {
		req = Thread_new(YES);
		((HTContext *) req->context)->history = HT_HIST_NO_UPDATE;
		status = HTLoadAnchor(HTHistory_forward(hist), req);
	    } else {
		printf("\nThis is the last document in history list.\n");
	    }
	} else
	    found = NO;
	break;
	
      case 'G':
	if (CHECK_INPUT("GOTO", token)) {			     /* GOTO */
	    if (next_word) {
		req = Thread_new(YES);
		status = HTLoadRelative(next_word, HTMainAnchor, req);
	    }
	} else
	    found = NO;
	break;
	
      case '?':
	req = Thread_new(YES);
	req->preemtive = YES;
	status = HTLoadRelative(C_HELP, HTMainAnchor, req);
	break;
	
      case 'H':
	if (CHECK_INPUT("HELP", token)) {		     /* help menu, ..*/
	    req = Thread_new(YES);
	    req->preemtive = YES;
	    status = HTLoadRelative(C_HELP, HTMainAnchor, req);
	} else if (CHECK_INPUT("HOME", token)) {		/* back HOME */
	    if (!HTHistory_canBacktrack(hist)) {
		HText_scrollTop(HTMainText);
	    } else {
		req = Thread_new(YES);
		((HTContext *) req->context)->history = HT_HIST_NO_UPDATE;
		status = HTLoadAnchor(HTHistory_find(hist, 1), req);
	    }
	} else
	    found = NO;
	break;
	
      case 'K':						 /* Keyword search ? */
	if (is_index && CHECK_INPUT("KEYWORDS", token)) {
	    goto find;
	} else
	    found = NO;
	break;
	
      case 'L':
	if (CHECK_INPUT("LIST", token)) {	     /* List of references ? */
	    Reference_List(!OutSource);
	}
#ifdef unix
	else if (CHECK_INPUT ("LCD", token)) {	       /* Local change dir ? */
	  lcd:
	    if (!next_word) {				 /* Missing argument */
		printf ("\nName of the new local directory missing.\n");
	    } else if (chdir (next_word)) {		 /* failed : say why */
		fprintf (TDEST, "\n  ");
		perror (next_word);
	    } else {		    /* Success : display new local directory */
		/* AS Sep 93 */
#ifdef NO_GETWD     /* No getwd() on this machine */
#ifdef HAS_GETCWD   /* System V variant SIGN CHANGED TBL 921006 !! */
		printf ("\nLocal directory is now:\n %s\n",
			getcwd (choice, sizeof(choice)));
#else   /* has NO getcwd */
		if (SHOW_MSG)
		    fprintf(TDEST, "This platform does not support getwd() or getcwd()\n");
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
	if (CHECK_INPUT("MANUAL", token)) {		 /* Read User manual */
	    req = Thread_new(YES);
	    req->preemtive = YES;
	    status = HTLoadRelative(MANUAL, HTMainAnchor,req);
	} else
	    found = NO;
	break;
	
      case 'P':                    
	if (CHECK_INPUT("POST", token)) {
	    status = Upload(req, METHOD_POST);
	}

#ifdef GOT_SYSTEM	    
	else if (!HTClientHost && CHECK_INPUT("PRINT", token)) {
	    char * address = HTAnchor_address((HTAnchor *) HTMainAnchor);
	    char * command;
	    char * tmplate = (char *) getenv("WWW_PRINT_COMMAND");
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
	
	else if (CHECK_INPUT("PS", token)) {
	    printf ("%s",SLAVE_PRINTER_ON);
	    printf("\f");			   /* Form feed for new page */
	    HText_scrollTop(HTMainText);
	    while(HText_canScrollDown(HTMainText)) {
		HText_scrollDown(HTMainText);
	    }
	    printf("\f");  			   /* Form feed for new page */
	    printf ("%s",SLAVE_PRINTER_OFF);
	    HText_scrollTop(HTMainText);
	}	
#endif
	else if (CHECK_INPUT("PUT", token)) {
	    status = Upload(req, METHOD_PUT);
	} else
	    found = NO;
	break;
	
      case 'Q':						   /* Quit program ? */
	if (CHECK_INPUT("QUIT", token)) {

	    /* 	JFG 9/7/92, following a complaint of 'q' mis-typed for '1'.
		JFG Then made optional because I hate it !!!
		TBL made it only affect remote logged on users. 921122 */
	    
	    if (HTClientHost && (strcasecomp(token, "quit") != 0) ) {
		printf ("\n Please type \"quit\" in full to leave www.\n");
	    } else {
		HTNet_killAll();			/* Kill all requests */
		status = NO;
	    }
	} else
	    found = NO;
	break;
	
      case 'R':	
	if (CHECK_INPUT("RECALL", token)) {
	    if (HTHistory_count(hist) <= 1) {
		printf("\n  No other documents to recall.\n");
	    } else {
		/* Previous node number exists, or does the user just */
		/* require a list of nodes visited? */
		if (next_word) {	
		    int cnt;
		    if ((cnt = atoi(next_word)) > 0) {
			req = Thread_new(YES);
			((HTContext *)req->context)->history=HT_HIST_NO_UPDATE;
			status = HTLoadAnchor(HTHistory_find(hist,cnt), req);
		    } else {
			if (SHOW_MSG)
			    fprintf(TDEST, "Bad command (%s), for list of commands type help\n", this_command);
		    }
		} else {
		    History_List();
		}
	    }
	} else if (CHECK_INPUT("REFRESH", token)) {
	    HText_setStale(HTMainText);			    /* Force refresh */
	    HText_refresh(HTMainText);			   /* Refresh screen */
	} else if (CHECK_INPUT("RELOAD", token)) {
	    req = Thread_new(YES);
	    req->reload = HT_FORCE_RELOAD;	/* Force full reload */
	    ((HTContext *) req->context)->history = HT_HIST_NO_UPDATE;
	    status = HTLoadAnchor((HTAnchor*) HTMainAnchor, req);
	} else
	    found = NO;
	break;
	
      case 'S':						       /* TBL 921009 */
	if (CHECK_INPUT("SOURCE", token)) {    	  	  /* Apply to source */
	    if (next_word) {
		OutSource = YES;		 /* Load and print as source */
		token = next_word;		         /* Move up one word */
		next_word = strtok (NULL, " \t\n\r");
		this_command = the_choice + (token - choice);
		other_words = the_choice + (next_word - choice);
		goto loop;	                       /* Go treat as before */
	    }
	} else if (CHECK_INPUT("SET", token)) {        	           /* config */
	    HTSetConfiguration(other_words);
	} else
	    found = NO;
	break;
	
      case 'T':
	if (CHECK_INPUT("TOP", token)) {		   /* Return to top  */
	    HText_scrollTop(HTMainText);
	} else
	    found = NO;
	break;
	
      case 'U':
	if (CHECK_INPUT("UP", token)) {		      /* Scroll up one page  */
	    HText_scrollUp(HTMainText);
	} else
	    found = NO;
	break;
	
      case 'V':
	if (CHECK_INPUT("VERBOSE", token)) {	     /* Switch verbose mode  */
	    WWW_TraceFlag = WWW_TraceFlag ? 0 : OldTraceFlag;
	    printf ("\n  Verbose mode %s.\n", WWW_TraceFlag ? "ON":"OFF");
	} else if (CHECK_INPUT("VERSION", token)) {	 	  /* Version */
	    VersionInfo();
	} else
	    found = NO;
	break;
	
      case 'Z':
	HText_setStale(HTMainText);			    /* Force refresh */
	HText_refresh(HTMainText);			   /* Refresh screen */
	HTNet_killAll();				/* Kill all requests */
	break;

      case '>':
	if (!HTClientHost) {
	    HText *curText = HTMainText;     /* Remember current main vindow */
	    req = Thread_new(NO);
	    req->reload = HT_MEM_REFRESH;
	    if (OutSource) req->output_format = WWW_SOURCE;
	    SaveOutputStream(req, token, next_word);
	    HText_select(curText);
	}
	break;
	
#ifdef GOT_PIPE
      case '|':
	if (!HTClientHost) {	             	           /* Local only!!!! */
	    char * address = HTAnchor_address((HTAnchor *) HTMainAnchor);
	    char * command;
	    int result;
	    command = (char*) malloc(strlen(address) +strlen(this_command)+30);
	    sprintf(command, "www %s \"%s\" %s", 
		    OutSource ? "-source" : "-n -na -p", address,this_command);
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
	    if (SHOW_MSG) fprintf(TDEST, "Executing `%s\'\n", this_command);
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
	if (is_index && *token) {  /* No commands, search keywords */
	    next_word = other_words = this_command;
	    found = YES;
	    goto find;
	} else {             
	    if (SHOW_MSG)
		fprintf(TDEST, "Bad command (%s), for list of commands type help\n", this_command);
	}
    }
    MakeCommandLine(is_index);
    free (the_choice);
    return (status==YES) ? HT_OK : HT_ERROR;
}


/*	terminate_handler
**	-----------------
**	React to the status of the terminated request
**	returns		HT_ERROR	Error has occured
**			HT_OK		Call back was OK
*/
PRIVATE int terminate_handler (HTRequest * request, int status) 
{
    BOOL is_index = HTAnchor_isIndex(HTMainAnchor);
    if (status == HT_LOADED) {
	HTContext *context = (HTContext *) HTRequest_context(request);

	/* Record new history if we have not moved around in the old one */
	if (context && context->history != HT_HIST_NO_UPDATE)
	    HTHistory_replace(hist, (HTAnchor *) HTMainAnchor);

	/* Now generate the new prompt line as a function of the result */
	if (!HText_canScrollDown(HTMainText) &&
	    !HTAnchor_hasChildren(HTMainAnchor) && !is_index &&
	    (!HTHistory_canBacktrack(hist))) {
	    if (SHOW_MSG)
		fprintf(TDEST, "No way out of here, so I exit!\n");
	    return HT_ERROR;
	}
	HText_setStale(HTMainText);		   /* We corrupt the display */
    }
    MakeCommandLine(is_index);
    Thread_delete(request);
    return HT_OK;
}

/* ------------------------------------------------------------------------- */
/*				  MAIN PROGRAM				     */
/* ------------------------------------------------------------------------- */

int main ARGS2(int, argc, char **, argv)
{
    HTRequest *	request;			    /* For the first request */
    int		status = 0;	
    int		arg;			       		  /* Index into argv */
    HTChunk *	keywords = NULL;			/* From command line */
    int		keycnt = 0;
    char *	abs_home = HTFindRelatedName();

    BOOL	filter = NO;		      	 	 /* Load from stdin? */
    HTFormat	input_format = WWW_HTML;	         /* Used with filter */
    BOOL       	show_refs = NO;	  	  	  /* -listrefs option used?  */
    BOOL	reformat_html = NO;		   	   /* Reformat html? */

    char *     	logfile = NULL;			 	    /* Log file name */
    char *	outputfile = NULL;		 	 	  /* -o xxxx */
    char *	rulefile = NULL;

    HTLibInit();			   /* Start up W3C Reference Library */
    HTStdIconInit(NULL);				 /* Initialize icons */
    HTProxy_getEnvVar();		   /* Read the environment variables */

#ifdef GUSI				   /* Starts Mac GUSI socket library */
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

    /* HWL 18/7/94: patch from agl@glas2.glas.apc.org (Anton Tropashko) */
#ifdef CYRILLIC
    arc.locale=0; arc.encoding=0; arc.i_encoding=0; doinull();
#endif

    request =  HTRequest_new();        /* Create the first request structure */

    for (arg=1; arg<argc ; arg++) {	   /* Check for command line options */
	if (*argv[arg] == '-') {

	    /* - alone => filter */
	    if (argv[arg][1] == 0) {
		filter = YES;	   
		HTPrompt_setInteractive(NO);
	    
	    /* -? or -help: show the command line help page */
	    } else if (!strcmp(argv[arg], "-?") ||
		       !strcmp(argv[arg], "-help")) {
		if (!keycnt) {
		    home_anchor=(HTParentAnchor *)HTAnchor_findAddress(L_HELP);
		    keycnt = 1;
		}

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
		    HTPrompt_setInteractive(NO);
		    reformat_html = YES;

	    /* Specify a cache root (caching is otherwise disabled) */
	    } else if (!strcmp(argv[arg], "-cacheroot")) {
		HTCache_enable((arg+1>=argc || *argv[arg+1]=='-') ?
			       NULL : argv[++arg]);

	    /* to -- Final representation */
	    } else if (!strcmp(argv[arg], "-to")) {
		request->output_format =
		    (arg+1 >= argc || *argv[arg+1] == '-') ?
		    	WWW_PRESENT : 
		    	HTAtom_for(argv[++arg]);
		    HTPrompt_setInteractive(NO);

	    /* Telnet from */
	    } else if (!strcmp(argv[arg], "-h")) {
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    HTClientHost = argv[++arg]; 	    /* Use host name */
		}

	    /* Log file */
	    } else if (!strcmp(argv[arg], "-l")) {
		logfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_LOG_FILE;

	    /* List References */
	    } else if (!strcmp(argv[arg], "-listrefs")) {
		show_refs = YES;
		HTPrompt_setInteractive(NO);

	    } else if (!strcasecomp(argv[arg], "-delete")) {  	   /* DELETE */
		request->method = METHOD_DELETE;
		HTPrompt_setInteractive(NO);

	    } else if (!strcasecomp(argv[arg], "-head")) {    /* HEAD Method */
		request->method = METHOD_HEAD;
		request->output_format = WWW_MIME;
		HTPrompt_setInteractive(NO);

	    /* @@@ NOT FINISHED @@@ */
#if 0
	    } else if (!strcasecomp(argv[arg], "-post")) {    /* POST Method */
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    char *ref = HTParse(argv[arg], abs_home, PARSE_ALL);
		    post_anchor = (HTParentAnchor*) HTAnchor_findAddress(ref);
		    free(ref);
		}
#endif

	    /* Non-interactive */
	    } else if (!strcmp(argv[arg], "-n")) {
		HTPrompt_setInteractive(NO);

	    /* Multithreaded ot not? */
	    } else if (!strcmp(argv[arg], "-single")) {
		request->preemtive = YES;
		Blocking = YES;

	    /* Output filename */
	    } else if (!strcmp(argv[arg], "-o")) { 
		outputfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_OUTPUT_FILE;
		    HTPrompt_setInteractive(NO);

	    /* Handling of Expire: */
	    } else if (!strncmp(argv[arg], "-x", 2)) { 
	    	char *p = argv[arg]+2;
		for(;*p;p++) {
		    switch (argv[arg][2]) {
		      case 'i':
			HTCache_setExpiresMode(HT_EXPIRES_IGNORE, NULL);
			break;
		      case 'n':
			HTCache_setExpiresMode(HT_EXPIRES_NOTIFY, "Document has expired, but I show it anyway");
			break;
		      case 'a':
			HTCache_setExpiresMode(HT_EXPIRES_AUTO, NULL);
			break;
		      default:
			if (SHOW_MSG)
			    fprintf(TDEST, "Bad parameter (%s) for option -x\n", argv[arg]);
			break;
		    }
		}

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
		    rulefile = (arg+1 < argc && *argv[arg+1] != '-') ?
			argv[++arg] : DEFAULT_RULE_FILE;
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
		      case 's': HTDirAccess = HT_DIR_SELECTIVE; break;
		      case 't':	HTDirReadme = HT_DIR_README_TOP; break;
		      case 'y':	HTDirAccess = HT_DIR_OK; break;
		      default:
			if (SHOW_MSG)
			    fprintf(TDEST,"Bad parameter (%s) for -d option\n",
				    argv[arg]);
		    }
		} /* loop over characters */
#endif

	    /* Print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
		VersionInfo();
		goto endproc;				

#ifdef TRACE
	   /* Verify: Turns on trace */
	    } else if (!strncmp(argv[arg], "-v", 2)) {
	    	char *p = argv[arg]+2;
		WWW_TraceFlag = 0;
		for(; *p; p++) {
		    switch (*p) {
		      case 'a': WWW_TraceFlag |= SHOW_ANCHOR_TRACE; break;
		      case 'b': WWW_TraceFlag |= SHOW_BIND_TRACE; break;
		      case 'c': WWW_TraceFlag |= SHOW_CACHE_TRACE; break;
		      case 'g':	WWW_TraceFlag |= SHOW_SGML_TRACE; break;
		      case 'p':	WWW_TraceFlag |= SHOW_PROTOCOL_TRACE; break;
		      case 's':	WWW_TraceFlag |= SHOW_STREAM_TRACE; break;
		      case 't':	WWW_TraceFlag |= SHOW_THREAD_TRACE; break;
		      case 'u': WWW_TraceFlag |= SHOW_URI_TRACE; break;
		      default:
			if (SHOW_MSG)
			    fprintf(TDEST,"Bad parameter (%s) for -v option\n",
				    argv[arg]);
		    }
		}/* loop over characters */
		if (!WWW_TraceFlag)
		    WWW_TraceFlag = SHOW_ALL_TRACE;
		OldTraceFlag = WWW_TraceFlag;		 /* Remember setting */
#endif
	    
	    /* Source please */
	    } else if (!strcmp(argv[arg], "-source")) {
		    request->output_format = WWW_SOURCE;
		    HTPrompt_setInteractive(NO);

	    } else {
		if (SHOW_MSG)
		    fprintf(TDEST, "Bad Command Line Argument (%s)\n",
			    argv[arg]);
	    }
	} else {	   /* If no leading `-' then check for main argument */
    	    if (!keycnt) {
		char * ref = HTParse(argv[arg], abs_home, PARSE_ALL);
		home_anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
		free(ref);
		keycnt = 1;
	    } else {		   /* Check for successive keyword arguments */
		if (keycnt++ <= 1)
		    keywords = HTChunkCreate(128);
		else
		    HTChunkPutc(keywords, ' ');
		HTChunkPuts(keywords, HTStrip(argv[arg]));
	    }
	} /* Not an option '-'*/
    } /* End of argument loop */

    /* Do remaining Initialization */
#ifdef CATCH_SIG
    SetSignal();
#endif
    if (HTClientHost) HTSecure = YES;		   /* Access to local files? */
    if (HTScreenHeight == -1) {				/* Default page size */
	if (HTPrompt_interactive()) {
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

    /* Disable free directory browsing when using telnet host */
    if (HTClientHost && HTDirAccess==HT_DIR_OK)
	HTDirAccess = HT_DIR_SELECTIVE;

#ifndef HT_NO_RULES
    {
    	char * rules = rulefile ? rulefile : getenv("WWW_CONFIG");
	if (rules && HTLoadRules(rules) < 0) {
	    if (SHOW_MSG)
		fprintf(TDEST, "Can't open rule file (%s) - ignored\n",rules);
	}
    }
#endif

    /* Force predefined presentations etc to be set up  */
    HTConverterInit(HTConversions);		       /* Set up global list */
    if (HTPrompt_interactive())
	HTPresenterInit(request->conversions);		       /* Local list */

    /* Open output file */
    if (!HTPrompt_interactive()) {
    	if (outputfile) {	    
	    if ((output = fopen(outputfile, "wb")) == NULL) {
	        if (SHOW_MSG)
		    fprintf(TDEST, "Can't open file for writing (%s)\n",
			    outputfile);
	    }
	}
	request->output_stream = HTFWriter_new(output, YES);

	/* To reformat HTML, just put it through a parser running
	** into a regenerator   tbl 940613
	*/
	if (reformat_html) {
	    request->output_stream =
		SGML_new(&HTMLP_dtd, HTMLGenerator(request, NULL, WWW_HTML,
						   request->output_format,
						   request->output_stream));
	}
    }
    
    /* Open Log File */
    if (logfile)
	HTLog_enable(logfile, YES, NO);

    /* Make home page address */
    if (!home_anchor)
	home_anchor = HTHomeAnchor();

    /* Just convert formats */
    if (filter) {
	HTRequest_setAnchor (request, (HTAnchor *) home_anchor);
     	HTParseSocket(input_format, 0, request);      /* From std UNIX input */
	    goto endproc;
    }
    
    /* Register a call back function for the Net Manager */
    if (HTPrompt_interactive())
	HTNet_register(terminate_handler, HT_ALL);
    
    /* Register our own "unknwon" header handler (see GridText.c) */
    HTMIME_register(HTHeaderParser);

    /* Load the first page. This is done using blocking I/O */
    request->preemtive = YES;
    if ((keywords ?
	 HTSearch(HTChunkData(keywords), home_anchor, request) :
	 HTLoadAnchor((HTAnchor*) home_anchor, request)) != YES) {
	if (SHOW_MSG)
	    fprintf(TDEST, "Couldn't load home page\n");
	status = -1;
	goto endproc;
    }

    /* If in interactive mode then start the event loop which will run until
       the program terminates */
    if (HTPrompt_interactive()) {

	/* Set max number of sockets we want open simultanously */
	HTNet_setMaxSocket(6);

	/* Start own list of pending requests */
	reqlist = HTList_new();

	/* Start History manager and record first page */
	hist = HTHistory_new();
	HTHistory_record(hist, (HTAnchor *) home_anchor);

	/* Register our own memory cache handler (implemented in GridText.c) */
	HTMemoryCache_register(HTMemoryCache);

	/* Make first command line */
 	MakeCommandLine(HTAnchor_isIndex(home_anchor));

	/* Register STDIN as the user socket */
	HTEvent_RegisterTTY(STDIN_FILENO, request, (SockOps)FD_READ,
			    scan_command, 1);

	/* Set the DNS timeout */
	HTDNS_setTimeout(300);

	/* Go into the event loop... */
	HTEvent_Loop(request);
    } else if (show_refs) {
	Reference_List(NO);	       /* Show references from this document */
    }

endproc:
    if (keywords) HTChunkFree(keywords);
    if (output && output!=stdout)
	fclose(output);
    Thread_deleteAll();
    if (abs_home)
	free(abs_home);
    if (logfile)
	HTLog_disable();
    {
	HTList *hyperdocs = HTList_new();
	HTAnchor_deleteAll(hyperdocs);
	if (hyperdocs) {
	    HTList *cur = hyperdocs;
	    HText *pres;
	    while ((pres = (HText *) HTList_nextObject(cur)))
		hyperfree(pres);
	    HTList_delete(hyperdocs);
	}
    }
    HTLibTerminate();
    if (HTPrompt_interactive())	   /* Terminate with a LF if not interactive */
	printf("\n");

#ifdef VMS
    return status ? status : 1;
#else
    return status ? status : 0;
#endif
}

/* End HTBrowse.c */

