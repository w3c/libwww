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
**	HFN: Henrik Frystyk Nielsen, MIT, (frystyk@w3.org)
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
**		(MD) Assigne OUTPUT in main, not at initialize (VMS only)
**
** Compilation-time macro options
**
**	REF_MARK	Printf string to be used for printing anchor numbers
**	END_MARK	String to be used to denote the end of a document
**	VL		Version number, quoted eg "1.2a"
*/

#include "WWWLib.h"
#include "WWWMIME.h"
#include "WWWApp.h"

#include "GridText.h"				     /* Hypertext definition */
#include "HTBrowse.h"			     /* Things exported, short names */

#ifndef VL
#define VL "unspecified"
#endif

#define APP_NAME		"W3CLineMode"
#define APP_VERSION		VL

/* Default page for "Manual" command */
#define MANUAL	"http://www.w3.org/pub/WWW/LineMode/Defaults/QuickGuide.html"

/* Default page for "help" command */
#define C_HELP	"http://www.w3.org/pub/WWW/LineMode/Defaults/Commands.html"

/* Default page for "-help" command line option */
#define L_HELP	"http://www.w3.org/pub/WWW/LineMode/Defaults/CommandLine.html"

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

#define RESPONSE_LENGTH		1024     /* Maximum length of users response */

#define SHOW_MSG		(WWWTRACE || HTAlert_interactive())
#define CHECK_INPUT(a, b)	(!strncasecomp ((a), (b), strlen((b))))

#define DEFAULT_I_TIMEOUT	1          /* Interactive timeout in seconds */
#define DEFAULT_NI_TIMEOUT	10     /* Non-interactive timeout in seconds */

#define DEFAULT_FORMAT		WWW_PRESENT

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

/* Screen size parameters */
PUBLIC int		HTScreenWidth   = SCREEN_WIDTH;	       /* By default */
PUBLIC int		HTScreenHeight  = -1;		   /* -1 = Undefined */

/* Anchor specific information */
PUBLIC BOOL		display_anchors = YES;	    /* Show anchors in text? */
PUBLIC char *		start_reference = NULL;   /* Format for start anchor */
PUBLIC char *		end_reference = REF_MARK;	   /* for end anchor */
PUBLIC char *		reference_mark = "[%d] ";     /* for reference lists */
PUBLIC char *		end_mark = END_MARK;   	  /* Format string for [End] */

typedef enum _LMFlags {
    LM_FILTER	= 0x1,
    LM_REFS	= 0x2,
    LM_REFORMAT = 0x4,
    LM_PREEMTIVE= 0x8
} LMFlags;

typedef struct _LineMode {
    HTRequest *		request;
    HTParentAnchor *	anchor;
    HTParentAnchor *	dest;			 /* Destination for PUT etc. */
    struct timeval *	tv;				/* Timeout on socket */
    HTList *		active;			  /* List of acitve contexts */
    HTList *		converters;
    HTList *		presenters;
    HTHistory *		history;    			     /* History list */
    char *		cwd;				  /* Current dir URL */
    char *		rules;
    char *		logfile;
    char *		outputfile;
    char *		host;
    int			trace;
    HTFormat		format;		        /* Input format from console */
    LMFlags		flags;
} LineMode;

typedef enum _LMState {
    LM_UPDATE	= 0x1,
    LM_NO_UPDATE= 0x2,
    LM_DONE	= 0x4,
    LM_INACTIVE	= 0x8
} LMState;

typedef struct _Context {
    LMState		state;
    HTRequest *		request;
    LineMode *		lm;
} Context;

#ifndef _WINDOWS
PRIVATE FILE *		output = STDOUT;
#endif
 
/* ------------------------------------------------------------------------- */

/*	Create a Context Object
**	-----------------------
*/
PRIVATE Context * Context_new (LineMode *lm, HTRequest *request, LMState state)
{
    Context * me = (Context *) calloc(1, sizeof (Context));
    if (!me) outofmem(__FILE__, "Context_new");
    me->state = state;
    me->request = request;
    me->lm = lm;
    HTRequest_setContext(request, (void *) me);
    HTList_addObject(lm->active, (void *) me);
    return me;
}

/*	Delete a Context Object
**	-----------------------
*/
PRIVATE BOOL Context_delete (Context * old)
{
    FREE(old);
    return YES;
}

/*
**  This function creates a new request object and adds it to the global
**  list of active threads
*/
PRIVATE HTRequest * Thread_new (LineMode * lm, BOOL Interactive, LMState state)
{
    HTRequest * newreq = HTRequest_new();
    if (!lm) return NULL;
    Context_new(lm, newreq, state);
    if (Interactive) HTRequest_setConversion(newreq, lm->presenters, NO);
    if (lm->flags & LM_PREEMTIVE) HTRequest_setPreemtive(newreq, YES);
    HTRequest_addRqHd(newreq, HT_C_HOST);
    return newreq;
}

/*
**  This function deletes all unused request objects
*/
PRIVATE void Thread_cleanup (LineMode * lm)
{
    if (lm && lm->active) {
	HTList * cur = lm->active;
	Context * pres;
	while ((pres = (Context *) HTList_nextObject(cur))) {
	    if (pres->state&LM_DONE && pres->state&LM_INACTIVE) {
		if ((HTList_removeObject(lm->active, pres)) == NO)
		    TTYPrint(TDEST, "NOT FOUND\n");
		HTRequest_delete(pres->request);
		Context_delete(pres);
		cur = lm->active;
	    }
	}
    }
}

/*
**  This function deletes the whole list of active threads.
*/
PRIVATE void Thread_deleteAll (LineMode * lm)
{
    if (lm && lm->active) {
	HTList * cur = lm->active;
	Context * pres;
	while ((pres = (Context *) HTList_nextObject(cur))) {
	    if (pres->request) {
		HTRequest_delete(pres->request);
		Context_delete(pres);
	    }
	}
	HTList_delete(lm->active);
	lm->active = NULL;
    }
}

/*	Create a Line Mode Object
**	-------------------------
*/
#ifdef _WINDOWS
HTRequest * TTYReq = 0; /* The windowed version doesn't get the HTRequest* when
                           it gets key events so save it in a global here */
#endif

PRIVATE LineMode * LineMode_new (void)
{
    LineMode * me;
    if ((me = (LineMode *) calloc(1, sizeof(LineMode))) == NULL ||
	(me->tv = (struct timeval*) calloc(1, sizeof(struct timeval))) == NULL)
	outofmem(__FILE__, "LineMode_new");
    me->tv->tv_sec = -1;
    me->cwd = HTFindRelatedName();
    me->active = HTList_new();
    me->request = HTRequest_new();
    Context_new(me, me->request, LM_UPDATE);
#ifdef _WINDOWS
    TTYReq = me->request;
#endif
    me->trace = SHOW_ALL_TRACE;
    return me;
}

/*	Delete a Line Mode Object
**	-------------------------
*/
PRIVATE BOOL LineMode_delete (LineMode * lm)
{
    if (lm) {
	free(lm->tv);
	Thread_deleteAll(lm);
	HTConversion_deleteAll(lm->converters);
	HTPresentation_deleteAll(lm->presenters);
	HTHistory_delete(lm->history);
	FREE(lm->cwd);
	if (lm->logfile) HTLog_close();
#ifndef _WINDOWS
	if (OUTPUT && OUTPUT != STDOUT) fclose(OUTPUT);
#endif
	free(lm);
	return YES;
    }
    return NO;
}

PRIVATE void Cleanup (LineMode * me, int status)
{
    if (HTAlert_interactive())	   /* Terminate with a LF if not interactive */
	TTYPrint(OUTPUT, "\n");
    LineMode_delete(me);
    HTLibTerminate();
#ifdef VMS
    exit(status ? status : 1);
#else
    exit(status ? status : 0);
#endif
}

#ifdef GET_SCREEN_SIZE
#include <sys/ioctl.h>
/*
** Get size of the output screen. Stolen from less.
*/
PRIVATE void scrsize (int * p_height, int * p_width)
{
    register char *s;
    int ioctl();
    struct winsize w;
    if (ioctl(2, TIOCGWINSZ, &w) == 0 && w.ws_row > 0)
	*p_height = w.ws_row;
    else if ((s = getenv("LINES")) != NULL)
	*p_height = atoi(s);
    else
	*p_height = SCREEN_HEIGHT;
    
    if (ioctl(2, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
	*p_width = w.ws_col;
    else if ((s = getenv("COLUMNS")) != NULL)
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

/*  Print version information
**  -------------------------
*/
PRIVATE void VersionInfo (void)
{
    TTYPrint(OUTPUT,"\n\nW3C Reference Software\n\n");
    TTYPrint(OUTPUT,"\tW3C Line Mode Browser version %s.\n", APP_VERSION);
    TTYPrint(OUTPUT,"\tW3C Reference Library version %s.\n\n",HTLib_version());
    TTYPrint(OUTPUT,"Please send feedback to <www-bug@w3.org>\n");
}

/* 	Reference_List
**	--------------
**  	Print out a list of HyperText References from current document
*/
PRIVATE void Reference_List (LineMode * lm, BOOL titles)
{
    int refs = HText_sourceAnchors(HTMainText);
    if (refs <= 0) {
	TTYPrint(OUTPUT,"\n\nThere are no references from this document.\n\n");
    } else {
	int cnt;
	TTYPrint(OUTPUT, "\n*** References from this document ***\n");
	for (cnt=1; cnt<=refs; cnt++) {
	    HTAnchor *dest =
		HTAnchor_followMainLink((HTAnchor *)
					HText_childNumber(HTMainText, cnt));
	    HTParentAnchor * parent = HTAnchor_parent(dest);
	    char * address =  HTAnchor_address(dest);
	    CONST char * title = titles ? HTAnchor_title(parent) : NULL;
	    TTYPrint(OUTPUT, reference_mark, cnt);
	    TTYPrint(OUTPUT, "%s%s\n",
		    ((HTAnchor*)parent!=dest) && title ? "in " : "",
		    (char *)(title ? title : address));
	    free(address);
	}
#ifndef _WINDOWS
	fflush(OUTPUT);
#endif
    }
}      

/*	History_List
**	------------
**	Display a history list of nodes visited during the session.
*/
PRIVATE void History_List (LineMode * lm)
{
    int current = HTHistory_position(lm->history);
    int max = HTHistory_count(lm->history);
    int cnt;
    TTYPrint(OUTPUT, "\n  Documents you have visited: ");
    if (max > MAX_HISTORY) {
	max = MAX_HISTORY;
	TTYPrint(OUTPUT, "(truncated)\n");
    } else
	TTYPrint(OUTPUT, "\n");
    for (cnt=1; cnt<=max; cnt++) {
	HTAnchor *anchor = HTHistory_list(lm->history, cnt);
    	char *address = HTAnchor_address(anchor);
	HTParentAnchor *parent = HTAnchor_parent(anchor);
	CONST char *title = HTAnchor_title(parent);
	TTYPrint(OUTPUT, "%s R %d\t%s%s\n",
	       (cnt==current) ? "*" : " ",
	       cnt,
	       ((HTAnchor*)parent!=anchor) && title ? "in " : "",
	       title ? title : address);
	free(address);
    }
    TTYPrint(OUTPUT, "\n");
}

/*	Prompt for answer and get text back. Reply text is either NULL on
**	error or a dynamic string which the caller must free.
*/
PRIVATE char * AskUser (HTRequest * request, CONST char * Msg,
			CONST char * deflt)
{
    char buffer[200];
    char *reply = NULL;
    TTYPrint(TDEST, "%s ", Msg ? Msg : "UNKNOWN");
    if (deflt)
	TTYPrint(TDEST, "(RETURN for [%s]) ", deflt);

#ifndef NO_STDIO
    if (!fgets(buffer, 200, stdin))
	return NULL;		       	     /* NULL string on error, Henrik */
    buffer[strlen(buffer)-1] = '\0';		        /* Overwrite newline */
    if (*buffer)
	StrAllocCopy(reply, buffer);
    else if (deflt)
	StrAllocCopy(reply, deflt);
#endif
    return reply;
}

PRIVATE BOOL confirm (HTRequest * request, CONST char * Msg)
{
    char response[4];
    TTYPrint(TDEST, "%s (y/n) ", Msg ? Msg : "UNKNOWN");
#ifndef NO_STDIO
    if (fgets(response, 4, stdin)) 		   /* get reply, max 3 chars */
#endif
    {
	char *ptr = response;
	while (*ptr) {
	    if (*ptr == '\n') {
		*ptr = '\0';
		break;
	    }
	    *ptr = TOUPPER(*ptr);
	    ptr++;
	}
	return (!strcmp(response, "YES") || !strcmp(response, "Y")) ? YES : NO;
    }
    return NO;
}

/*	MakeCommandLine
**	---------------
**	Generate the Prompt line and flush it to the user
*/
PRIVATE void MakeCommandLine (LineMode * lm, BOOL is_index)
{
    /* First Line */
    if (HTAnchor_hasChildren(HTMainAnchor)) {
	int refs = HText_sourceAnchors(HTMainText);
	if (refs>1)
	    TTYPrint(OUTPUT, "1-%d, ", refs);
	else
	    TTYPrint(OUTPUT, "1, ");	
    }
    if (HText_canScrollUp(HTMainText)) {
	TTYPrint(OUTPUT, "Top, ");
	TTYPrint(OUTPUT, "Up, ");
    }
    if (HText_canScrollDown(HTMainText)) {
	TTYPrint(OUTPUT, "BOttom, ");
	TTYPrint(OUTPUT, "Down or <RETURN> for more,");
    }

    /* Second Line */
    TTYPrint(OUTPUT, "\n");
    if (HTHistory_canBacktrack(lm->history))
	TTYPrint(OUTPUT, "Back, ");
    if (HTHistory_canForward(lm->history))
	TTYPrint(OUTPUT, "Forward, ");
    if (is_index)
	TTYPrint(OUTPUT, "FIND <keywords>, ");
    TTYPrint(OUTPUT, "Quit, or Help: ");	
    fflush(stdout);  	           	  /* For use to flush out the prompt */
    return;
}

/*
**  Upload a document either from local file or from a HTTP server
**  to a HTTP server. The method can be either PUT or POST.
**  Returns the result of the load function.
*/
PRIVATE int Upload (LineMode * lm, HTRequest * req, HTMethod method)
{
    char * base = HTAnchor_address((HTAnchor*) HTMainAnchor);
    char * scr_url = NULL;
    char * dest_url = NULL;
    int status = HT_INTERNAL;
    if ((scr_url = AskUser(req, "Source:", base)) != NULL &&
	(dest_url = AskUser(req, "Destination:", NULL)) != NULL) {
	BOOL doit = YES;
	char * fd = HTParse(HTStrip(dest_url), base, PARSE_ALL);
	char * fs = HTParse(HTStrip(scr_url), base, PARSE_ALL);
	HTParentAnchor * dest = (HTParentAnchor *) HTAnchor_findAddress(fd);
	HTParentAnchor * src = (HTParentAnchor *) HTAnchor_findAddress(fs);
	HTLink * link = HTAnchor_findLink((HTAnchor *) src, (HTAnchor *) dest);
	
	/* Now link the two anchors together if not already done */
	if (link) {
	    char *msg = (char *) malloc(128);
	    if (!msg) outofmem(__FILE__, "Upload");
	    sprintf(msg, "The destination is already related to the source with a %s method - result %d, continue?",
		    HTMethod_name(HTLink_method(link)), HTLink_result(link));
	    doit = confirm(req, msg);
	    free(msg);
	} else
	    HTAnchor_link((HTAnchor *) src, (HTAnchor *) dest, NULL, method);
	if (doit) {
	    req = Thread_new(lm, YES, LM_UPDATE);
	    status = HTCopyAnchor((HTAnchor *) src, req);
	}
	free(fd);
	free(fs);
    }
    FREE(scr_url);
    FREE(dest_url);
    FREE(base);
    return status;
}

/*
**  This function puts up a stream to a file in order to save a document. This
**  is activated by '>', '>>' or '>!' from the prompt line.
*/
PRIVATE BOOL SaveOutputStream (HTRequest * req, char * This, char * Next)
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
		fclose(fp);
		if (!confirm(req, "File exists - overwrite?"))
		    return NO;
	    }
	}
    }
    if (!fname)					       /* No file name given */
	return NO;
    if ((fp = fopen(fname, fmode)) == NULL) {
	if (SHOW_MSG) TTYPrint(TDEST, "Can't access file (%s)\n", fname);
	return NO;
    }
    HTRequest_setOutputStream(req, HTFWriter_new(fp, NO));
    if (SHOW_MSG) TTYPrint(TDEST, "Saving to file `%s\'\n", fname);
    return (HTLoadAnchor((HTAnchor*) HTMainAnchor, req) != HT_WOULD_BLOCK);
}

/* ------------------------------------------------------------------------- */
/*				EVENT FUNCTIONS				     */
/* ------------------------------------------------------------------------- */

/*	parse_command
**	------------
**      Given the user's input, deal with it as necessary.
**	Any Command which works returns from the routine. If nothing
**	works then a search or error message down at the bottom.
**
**	Returns		HT_ERROR	Error has occured or we quit
**			HT_OK		Call back was OK
*/
PRIVATE int parse_command (char* choice, SOCKET s, HTRequest *req, SockOps ops)
{
    char * the_choice=NULL;		           /* preserved user command */
    char * token=NULL;        	   	    	    /* First word of command */
    char * this_command;	       	              /* token and following */
    char * next_word;			                      /* Second word */
    char * other_words;				/* Second word and following */
    BOOL is_index = HTAnchor_isIndex(HTMainAnchor);
    BOOL found = YES;
    BOOL OutSource = NO;			    /* Output source, YES/NO */
    int status = YES;
    HTRequest * cur_req = req;
    Context * cur_context = (Context *) HTRequest_context(req);
    LineMode * lm = cur_context->lm;

#ifdef _WINDOWS
    req = TTYReq;
#endif

    StrAllocCopy (the_choice, choice);		       /* Remember it as is, */
    if (*the_choice && the_choice[strlen(the_choice)-1] == '\n') /* final \n */
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
		    req = Thread_new(lm, YES, LM_UPDATE);
		    destination = HTAnchor_followMainLink((HTAnchor*) source);

		    /* Continous browsing, so we want Referer field */
		    HTRequest_setParent(req,
					HTAnchor_parent((HTAnchor*)source));
		    status = HTLoadAnchor(destination, req);
		} else {
		    status = NO;				/* No anchor */
		}
	    } else {
		if (SHOW_MSG)
		    TTYPrint(TDEST,"Warning: Invalid Reference Number: (%d)\n",
			    ref_num);
	    }
	}
	break;
	
      case 'B':		
	if (CHECK_INPUT("BACK", token)) {	  /* Return to previous node */
	    if (HTHistory_canBacktrack(lm->history)) {
		req = Thread_new(lm, YES, LM_NO_UPDATE);
		status = HTLoadAnchor(HTHistory_back(lm->history), req);
	    } else {
		TTYPrint(OUTPUT, "\nThis is the first document in history list\n");
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
	    HTHistory_removeFrom(lm->history, 1);
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
		    req = Thread_new(lm, YES, LM_UPDATE);
		    status = HTSearch(other_words, HTMainAnchor, req);
		}
	    }
	} else if (CHECK_INPUT("FORWARD", token)) {
	    if (HTHistory_canForward(lm->history)) {
		req = Thread_new(lm, YES, LM_NO_UPDATE);
		status = HTLoadAnchor(HTHistory_forward(lm->history), req);
	    } else {
		TTYPrint(OUTPUT, "\nThis is the last document in history list.\n");
	    }
	} else
	    found = NO;
	break;
	
      case 'G':
	if (CHECK_INPUT("GOTO", token)) {			     /* GOTO */
	    if (next_word) {
		req = Thread_new(lm, YES, LM_UPDATE);
		status = HTLoadRelative(next_word, HTMainAnchor, req);
	    }
	} else
	    found = NO;
	break;
	
      case '?':
	req = Thread_new(lm, YES, LM_NO_UPDATE);
	HTRequest_setPreemtive(req, YES);
	status = HTLoadRelative(C_HELP, HTMainAnchor, req);
	break;
	
      case 'H':
	if (CHECK_INPUT("HELP", token)) {		     /* help menu, ..*/
	    req = Thread_new(lm, YES, LM_NO_UPDATE);
	    HTRequest_setPreemtive(req, YES);
	    status = HTLoadRelative(C_HELP, HTMainAnchor, req);
	} else if (CHECK_INPUT("HOME", token)) {		/* back HOME */
	    if (!HTHistory_canBacktrack(lm->history)) {
		HText_scrollTop(HTMainText);
	    } else {
		req = Thread_new(lm, YES, LM_NO_UPDATE);
		status = HTLoadAnchor(HTHistory_find(lm->history, 1), req);
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
	    Reference_List(lm, !OutSource);
	}
#ifdef unix
	else if (CHECK_INPUT ("LCD", token)) {	       /* Local change dir ? */
	  lcd:
	    if (!next_word) {				 /* Missing argument */
		TTYPrint(OUTPUT, "\nName of the new local directory missing.\n");
	    } else if (chdir (next_word)) {		 /* failed : say why */
		TTYPrint(OUTPUT, "\n  ");
		perror (next_word);
	    } else {		    /* Success : display new local directory */
		/* AS Sep 93 */
#ifdef NO_GETWD     /* No getwd() on this machine */
#ifdef HAS_GETCWD   /* System V variant SIGN CHANGED TBL 921006 !! */
		TTYPrint(OUTPUT, "\nLocal directory is now:\n %s\n",
			getcwd (choice, sizeof(choice)));
#else   /* has NO getcwd */
		if (SHOW_MSG)
		    TTYPrint(TDEST, "This platform does not support getwd() or getcwd()\n");
#endif	/* has no getcwd */
#else   /* has getwd */
		TTYPrint(OUTPUT, "\nLocal directory is now:\n %s\n",
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
	    req = Thread_new(lm, YES, LM_NO_UPDATE);
	    HTRequest_setPreemtive(req, YES);
	    status = HTLoadRelative(MANUAL, HTMainAnchor,req);
	} else
	    found = NO;
	break;
	
      case 'P':                    
	if (CHECK_INPUT("POST", token)) {
	    status = Upload(lm, req, METHOD_POST);
	}

#ifdef GOT_SYSTEM	    
	else if (!lm->host && CHECK_INPUT("PRINT", token)) {
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
	    if (result) TTYPrint(OUTPUT, "  %s\n  returns %d\n", command, result);
	}
#endif
	/* this command prints the entire current text to the
	   terminal's printer; at the end it displays the top of the text */
#ifdef SLAVE_PRINTER
#define SLAVE_PRINTER_ON  "\033\133\065\151"
#define SLAVE_PRINTER_OFF "\033\133\064\151"
	
	else if (CHECK_INPUT("PS", token)) {
	    TTYPrint(OUTPUT, "%s",SLAVE_PRINTER_ON);
	    TTYPrint(OUTPUT, "\f");			   /* Form feed for new page */
	    HText_scrollTop(HTMainText);
	    while(HText_canScrollDown(HTMainText)) {
		HText_scrollDown(HTMainText);
	    }
	    TTYPrint(OUTPUT, "\f");  			   /* Form feed for new page */
	    TTYPrint(OUTPUT, "%s",SLAVE_PRINTER_OFF);
	    HText_scrollTop(HTMainText);
	}	
#endif
	else if (CHECK_INPUT("PUT", token)) {
	    status = Upload(lm, req, METHOD_PUT);
	} else
	    found = NO;
	break;
	
      case 'Q':						   /* Quit program ? */
	if (CHECK_INPUT("QUIT", token)) {

	    /* 	JFG 9/7/92, following a complaint of 'q' mis-typed for '1'.
		JFG Then made optional because I hate it !!!
		TBL made it only affect remote logged on users. 921122 */
	    
	    if (lm->host && (strcasecomp(token, "quit") != 0) ) {
		TTYPrint(OUTPUT, "\n Please type \"quit\" in full to leave www.\n");
	    } else {
		HTNet_killAll();			/* Kill all requests */
		status = NO;
	    }
	} else
	    found = NO;
	break;
	
      case 'R':	
	if (CHECK_INPUT("RECALL", token)) {
	    if (HTHistory_count(lm->history) <= 1) {
		TTYPrint(OUTPUT, "\n  No other documents to recall.\n");
	    } else {
		/* Previous node number exists, or does the user just */
		/* require a list of nodes visited? */
		if (next_word) {	
		    int cnt;
		    if ((cnt = atoi(next_word)) > 0) {
			req = Thread_new(lm, YES, LM_NO_UPDATE);
			status = HTLoadAnchor(HTHistory_find(lm->history,cnt), req);
		    } else {
			if (SHOW_MSG)
			    TTYPrint(TDEST, "Bad command (%s), for list of commands type help\n", this_command);
		    }
		} else {
		    History_List(lm);
		}
	    }
	} else if (CHECK_INPUT("REFRESH", token)) {
	    HText_setStale(HTMainText);			    /* Force refresh */
	    HText_refresh(HTMainText);			   /* Refresh screen */
	} else if (CHECK_INPUT("RELOAD", token)) {
	    req = Thread_new(lm, YES, LM_NO_UPDATE);
	    HTRequest_setReloadMode(req, HT_FORCE_RELOAD);
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
	    HTList * rules = HTRule_global();
	    HTRule_parseLine(rules, other_words);
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
	    WWWTRACE = WWWTRACE ? 0 : lm->trace;
	    TTYPrint(OUTPUT, "\n  Verbose mode %s.\n", WWWTRACE ? "ON":"OFF");
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
	if (!lm->host) {
	    HText *curText = HTMainText;     /* Remember current main vindow */
	    req = Thread_new(lm, NO, LM_NO_UPDATE);
	    HTRequest_setReloadMode(req, HT_MEM_REFRESH);
	    if (OutSource) HTRequest_setOutputFormat(req, WWW_SOURCE);
	    SaveOutputStream(req, token, next_word);
	    HText_select(curText);
	}
	break;
	
#ifdef GOT_PIPE
      case '|':
	if (!lm->host) {	             	           /* Local only!!!! */
	    char * address = HTAnchor_address((HTAnchor *) HTMainAnchor);
	    char * command;
	    int result;
	    command = (char*) malloc(strlen(address) +strlen(this_command)+30);
	    sprintf(command, "www %s \"%s\" %s", 
		    OutSource ? "-source" : "-n -na -p", address,this_command);
	    TTYPrint(OUTPUT, "Command: %s\n", command);
	    result = system(command);
	    if (result) TTYPrint(OUTPUT,"  %s  returns %d\n", command, result);
	    free(command);
     	    free(address);
	}
	break;
#endif
	    
#ifdef GOT_SYSTEM
      case '!':
	if (!lm->host) {				      /* Local only! */
	    int result;
	    if (SHOW_MSG) TTYPrint(TDEST, "Executing `%s\'\n", this_command);
	    result = system(strchr(this_command, '!') + 1);
	    if (result) TTYPrint(OUTPUT, "  %s  returns %d\n",
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
		TTYPrint(TDEST, "Bad command (%s), for list of commands type help\n", this_command);
	}
    }
    MakeCommandLine(lm, is_index);
    free (the_choice);

    /* 
    ** If we have created a new Request and is to update the history list then
    ** we can set the inactive bit on this request object.
    */
    if (cur_req == req)
	cur_context->state |= LM_NO_UPDATE;
    else if (cur_req != lm->request) {
	printf("HERE\n");
	cur_context->state |= LM_INACTIVE;    
    }
    return (status==YES) ? HT_OK : HT_ERROR;
}

/*	readConsole
**	-----------
**	non-blocking read of the WIN32 console. EGP
*/

#ifdef _CONSOLE
PUBLIC BOOL readConsole(HANDLE conIn, char* buf, int len, int* pRed)
{
    DWORD recordIndex, bufferIndex, toRead, red;
    PINPUT_RECORD pInput;

    /* grab the pending input records (keystrokes plus other garbage). */
    GetNumberOfConsoleInputEvents(conIn, &toRead);
    if (len < (int)toRead)	/* we'll get the rest on the next pass(es). */
    toRead = len;
    if (!(pInput = (PINPUT_RECORD)malloc(toRead * sizeof(INPUT_RECORD))))	/* room for n input records */
	return (FALSE);
    ReadConsoleInput(conIn, pInput, toRead, &red);

    for (recordIndex = bufferIndex = 0; recordIndex < red; recordIndex++) {
	/* grab all keydown events */
#if 1
        KEY_EVENT_RECORD keyEvent = pInput[recordIndex].Event.KeyEvent;     /* only used if EventType == KEY_EVENT */
    	if (pInput[recordIndex].EventType == KEY_EVENT && keyEvent.bKeyDown) {
    	    while (keyEvent.wRepeatCount && keyEvent.uChar.AsciiChar) {
    		/* stuff the buffer with the keys */
    		buf[bufferIndex] = keyEvent.uChar.AsciiChar;
    		if (buf[bufferIndex] == '\r')
    		    buf[bufferIndex] = '\n';
    		if (buf[bufferIndex] == '\b')
    		    TTYPrint(STDOUT, "\b ");
    		TTYPrint(STDOUT, "%c", buf[bufferIndex]);
    		bufferIndex++;
    		keyEvent.wRepeatCount--;
    	    }
    	}
#else
    	if (pInput[recordIndex].EventType == KEY_EVENT && pInput[recordIndex].Event.KeyEvent.bKeyDown) {
    	    while (pInput[recordIndex].Event.KeyEvent.wRepeatCount && pInput[recordIndex].Event.KeyEvent.uChar.AsciiChar) {
    		/* stuff the buffer with the keys */
    		buf[bufferIndex] = pInput[recordIndex].Event.KeyEvent.uChar.AsciiChar;
    		if (buf[bufferIndex] == '\r')
    		    buf[bufferIndex] = '\n';
    		if (buf[bufferIndex] == '\b')
    		    TTYPrint(STDOUT, "\b ");
    		TTYPrint(STDOUT, "%c", buf[bufferIndex]);
    		bufferIndex++;
    		pInput[recordIndex].Event.KeyEvent.wRepeatCount--;
    	    }
    	}
#endif

    }

    free(pInput);
    *pRed = bufferIndex;		/* actual characters stuck into buffer */
    return (TRUE);
}
#endif /* _CONSOLE */

/*	bufferInput
**	-----------
**	Read available characters from buf into stat. buf maybe bigger or
**	smaller than stat.
*/
PUBLIC int bufferInput (char* buf, int len, SOCKET s, HTRequest * req,	
			SockOps ops)
{
    static char stat[RESPONSE_LENGTH];
    static int iStat = 0;
    static int ignoreNext = 0;
    int iBuf;
    for (iBuf = 0; iBuf < len; iBuf++) {
	switch (buf[iBuf]) {
	    case '\r':
	    case '\n':
		if (ignoreNext)
		    ignoreNext = 0;
		else {
		    int ret;
		    stat[iStat] = 0;
		    iStat = 0;
		    if ((ret = parse_command(stat, 0, 0, 0)) != HT_OK)
			return (ret);
		}
		break;
	    case '\b':
		if (iStat) /* don't worry about ignoreNext as iStat will be 0*/
		    iStat--;
		break;
	    default:
		if (!ignoreNext)
		    stat[iStat++] = buf[iBuf];
	}
	if (iStat == sizeof(stat)) {
	    TTYPrint(OUTPUT, "Read Console... BUFFER OVERRUN\n");
	    iStat = 0;
	    ignoreNext = 1;
	}
    }
    return (HT_OK);
}

PRIVATE int scan_command (SOCKET s, HTRequest * req, SockOps ops)
{
    /* buf happens to == eatText's buffer but not neccesary */
    static char buf[RESPONSE_LENGTH];
    int red;
    int ret;

#ifndef _CONSOLE
    if (!fgets(buf, sizeof(buf), stdin))		  /* Read User Input */
	return HT_ERROR;				      /* Exit if EOF */
    return (parse_command(buf, s, req, ops));
#else

    while(1) {
#ifdef _CONSOLE
	if (!readConsole((HANDLE)s, buf, sizeof(buf), &red)) {
	    if (PROT_TRACE) TTYPrint(TDEST, "Read Console... READ ERROR\n");
	    return HT_ERROR;
	}
#else	/* _CONSOLE */
	if ((red = read(s, buf, sizeof(buf))) < 0) {
#ifdef EAGAIN
	    if (socerrno==EINPROGRESS || socerrno==EAGAIN)
#else
	    if (socerrno==EINPROGRESS)
#endif
		return (HT_OK);
	    if (PROT_TRACE) TTYPrint(TDEST, "Read Console... READ ERROR\n");
	    return HT_ERROR;
	}
#endif /* _CONSOLE */
	if (!red) return (HT_OK);
	ret = bufferInput(buf, red, s, req, ops);
	if (ret != HT_OK) return (ret);
    }
#endif
}

/*	terminate_handler
**	-----------------
**	This function is registered to handle the result of the request
*/
PRIVATE int terminate_handler (HTRequest * request, int status) 
{
    Context * context = (Context *) HTRequest_context(request);
    LineMode * lm = context->lm;
    BOOL is_index = HTAnchor_isIndex(HTMainAnchor);
    if (status == HT_LOADED) {

	if (HTAlert_interactive()) {
	    HText_setStale(HTMainText);
	    MakeCommandLine(lm, is_index);
	} else {
	    if (lm->flags & LM_REFS) Reference_List(lm, NO);
	    Cleanup(lm, 0);
	}

	/* Record new history if we have not moved around in the old one */
	if (context->state & LM_UPDATE)
	    HTHistory_replace(lm->history, (HTAnchor *) HTMainAnchor);

	/* Now generate the new prompt line as a function of the result */
	if (!HText_canScrollDown(HTMainText) &&
	    !HTAnchor_hasChildren(HTMainAnchor) && !is_index &&
	    (!HTHistory_canBacktrack(lm->history))) {
	    return HT_OK;
	}

    }
    context->state |= LM_DONE;
    Thread_cleanup(lm);
    if (!HTAlert_interactive()) Cleanup(lm, -1);
    return HT_OK;
}

/*	timeout_handler
**	---------------
**	This function is registered to handle timeout in select eventloop
*/
PRIVATE int timeout_handler (HTRequest * request)
{
    if (!HTAlert_interactive()) {
	Context * context = (Context *) HTRequest_context(request);
	LineMode * lm = context->lm;
	HTNet_killAll();
	Cleanup(lm, -1);
    }
    if (SHOW_MSG) TTYPrint(TDEST, ".");
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
    int		arg;			       		  /* Index into argv */
    HTChunk *	keywords = NULL;			/* From command line */
    int		keycnt = 0;
    LineMode *	lm;

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

    /* HWL 18/7/94: patch from agl@glas2.glas.apc.org (Anton Tropashko) */
#ifdef CYRILLIC
    arc.locale=0; arc.encoding=0; arc.i_encoding=0; doinull();
#endif

    /* Initiate W3C Reference Library */
    HTLibInit(APP_NAME, APP_VERSION);
    lm = LineMode_new();

    /* Initialize the protocol modules */
    HTAccessInit();

    /* Initialize set of converters */
    lm->converters = HTList_new();
    HTConverterInit(lm->converters);
    HTFormat_setConversion(lm->converters);

    /* Initialize bindings between file suffixes and media types */
    HTFileInit();

    /* Set up default set of icons */
    HTStdIconInit(NULL);

    /* Get any proxy or gateway environment variables */
    HTProxy_getEnvVar();

    /* Scan command Line for parameters */
    for (arg=1; arg<argc ; arg++) {
	if (*argv[arg] == '-') {

	    /* - alone => filter */
	    if (argv[arg][1] == '\0') {
		lm->flags |= LM_FILTER;
		HTAlert_setInteractive(NO);
	    
	    /* non-interactive */
	    } else if (!strcmp(argv[arg], "-n")) {
		HTAlert_setInteractive(NO);

	    /* from -- Initial represntation (only with filter) */
	    } else if (!strcmp(argv[arg], "-from")) {
		lm->format = (arg+1 < argc && *argv[arg+1] != '-') ?
		    HTAtom_for(argv[++arg]) : WWW_HTML;
		HTAlert_setInteractive(NO);

	    /* to -- Final representation */
	    } else if (!strcmp(argv[arg], "-to")) {
		HTFormat format = (arg+1 < argc && *argv[arg+1] != '-') ?
		    HTAtom_for(argv[++arg]) : DEFAULT_FORMAT;
		HTRequest_setOutputFormat(lm->request, format);
		HTAlert_setInteractive(NO);

	    /* reformat html */
	    } else if (!strcmp(argv[arg], "-reformat")) {
		HTRequest_setOutputFormat(lm->request, WWW_HTML);
		lm->flags |= LM_REFORMAT;
		HTAlert_setInteractive(NO);

	    /* List References */
	    } else if (!strncmp(argv[arg], "-list", 5)) {
		lm->flags |= LM_REFS;
		HTAlert_setInteractive(NO);

	    /* original output */
	    } else if (!strcmp(argv[arg], "-raw")) {
		HTRequest_setOutputFormat(lm->request, WWW_RAW);
		HTAlert_setInteractive(NO);

	    /* source please */
	    } else if (!strcmp(argv[arg], "-source")) {
		HTRequest_setOutputFormat(lm->request, WWW_SOURCE);
		HTAlert_setInteractive(NO);

	    /* HEAD method */
	    } else if (!strcasecomp(argv[arg], "-head")) {
		HTRequest_setMethod(lm->request, METHOD_HEAD);
		HTRequest_setOutputFormat(lm->request, WWW_MIME);
		HTAlert_setInteractive(NO);

	    /* output filename */
	    } else if (!strcmp(argv[arg], "-o")) { 
		lm->outputfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_OUTPUT_FILE;
		HTAlert_setInteractive(NO);

	    /* print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
		VersionInfo();
		Cleanup(lm, 0);

	    /* -? or -help: show the command line help page */
	    } else if (!strcmp(argv[arg],"-?") || !strcmp(argv[arg],"-help")) {
		lm->anchor = (HTParentAnchor *) HTAnchor_findAddress(L_HELP);
		keycnt = 1;

#ifdef CYRILLIC
	    /* HWL 18/7/94: applied patch from agl@glas2.glas.apc.org
	       (Anton Tropashko) */
	    } else if (!strcmp(argv[arg], "-koi2alt")) {
	        doia2k(); TTYPrint(OUTPUT, "Ahak2a!");
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
		    
	    /* Telnet from */
	    } else if (!strcmp(argv[arg], "-h")) {
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    lm->host = argv[++arg]; 		    /* Use host name */
		    HTLib_setSecure(YES);		   /* No easy access */
		}

	    /* log file */
	    } else if (!strcmp(argv[arg], "-l")) {
		lm->logfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_LOG_FILE;

	    /* rule file */
	    } else if (!strcmp(argv[arg], "-r")) {
		lm->rules = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_RULE_FILE;

	    /* timeout -- Change the default request timeout */
	    } else if (!strcmp(argv[arg], "-timeout")) {
		int timeout = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : -1;
		if (timeout > 0) lm->tv->tv_sec = timeout;

	    /* preemtive or non-preemtive access */
	    } else if (!strcmp(argv[arg], "-single")) {
		HTRequest_setPreemtive(lm->request, YES);
		lm->flags |= LM_PREEMTIVE;

	    /* Specify a cache root (caching is otherwise disabled) */
	    } else if (!strcmp(argv[arg], "-cacheroot")) {
		HTCache_enable((arg+1>=argc || *argv[arg+1]=='-') ?
			       NULL : argv[++arg]);

	    /* Handling of Expire (cache) */
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
			    TTYPrint(TDEST, "Bad parameter (%s) for option -x\n", argv[arg]);
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

#ifndef NO_DIR_OPTIONS
	    } else if (!strncmp(argv[arg], "-d", 2)) {
	    	char *p = argv[arg]+2;
		for(;*p;p++) {
		    switch (argv[arg][2]) {
		      case 'r':HTFile_setDirReadme(HT_DIR_README_NONE); break;
		      case 't':HTFile_setDirReadme(HT_DIR_README_TOP); break;
		      case 'b':HTFile_setDirReadme(HT_DIR_README_BOTTOM);break;
		      case 'n':HTFile_setDirAccess(HT_DIR_FORBID); break;
		      case 's':HTFile_setDirAccess(HT_DIR_SELECTIVE); break;
		      case 'y':HTFile_setDirAccess(HT_DIR_OK); break;
		      default:
			if (SHOW_MSG)
			    TTYPrint(TDEST,"Bad parameter (%s) in -d option\n",
				    argv[arg]);
		    }
		}
#endif

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
		lm->trace = WWWTRACE;			 /* Remember setting */
#endif
	    
	    } else {
		if (SHOW_MSG) TTYPrint(TDEST,"Bad Argument (%s)\n", argv[arg]);
	    }
	} else {	   /* If no leading `-' then check for main argument */
    	    if (!keycnt) {
		char * ref = HTParse(argv[arg], lm->cwd, PARSE_ALL);
		lm->anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
		keycnt = 1;
		FREE(ref);
	    } else {		   /* Check for successive keyword arguments */
		char *escaped = HTEscape(argv[arg], URL_XALPHAS);
		if (keycnt++ <= 1)
		    keywords = HTChunk_new(128);
		else
		    HTChunk_putc(keywords, ' ');
		HTChunk_puts(keywords, HTStrip(escaped));
		free(escaped);
	    }
	}
    }

#ifdef CATCH_SIG
    SetSignal();
#endif

    /* Make home page address */
    if (!lm->anchor) lm->anchor = HTHomeAnchor();

    /* Do we need list of presenters? */
    if (HTAlert_interactive()) {
	lm->presenters = HTList_new();
	HTPresenterInit(lm->presenters);
	HTRequest_setConversion(lm->request, lm->presenters, NO);
    }

    if (HTScreenHeight == -1) {				/* Default page size */
	if (HTAlert_interactive()) {
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
    if (lm->host && HTFile_dirAccess() == HT_DIR_OK)
	HTFile_setDirAccess(HT_DIR_SELECTIVE);

    /* Open output file */
    if (!HTAlert_interactive()) {
#ifndef _WINDOWS
	if (lm->outputfile) {
	    if ((OUTPUT = fopen(lm->outputfile, "wb")) == NULL) {
		if (SHOW_MSG) TTYPrint(TDEST, "Can't open `%s'\\n",
				       lm->outputfile);
		OUTPUT = STDOUT;
	    }
	}
	HTRequest_setOutputStream(lm->request, HTFWriter_new(OUTPUT, YES));
#endif

	/*
	** To reformat HTML, just put it through a parser running
	** into a regenerator   tbl 940613
	*/
	if (lm->flags & LM_REFORMAT) {
	    HTStructured * html =
		HTMLGenerator(lm->request, NULL, WWW_HTML,
			      HTRequest_outputFormat(lm->request),
			      HTRequest_outputStream(lm->request));
	    HTRequest_setOutputStream(lm->request, SGML_new(&HTMLP_dtd, html));
	}
    }
    
     /* Log file specifed? */
    if (lm->logfile) HTLog_open(lm->logfile, YES, YES);

    /* Just convert formats */
    if (lm->flags & LM_FILTER) {
#ifdef STDIN_FILENO
	HTRequest_setAnchor(lm->request, (HTAnchor *) lm->anchor);
	HTRequest_setPreemtive(lm->request, YES);
	HTLoadSocket(STDIN_FILENO, lm->request, FD_NONE);
#endif
	Cleanup(lm, 0);
    }

    /* Register our User Prompts etc in the Alert Manager */
    if (HTAlert_interactive()) {
	HTAlert_add(HTError_print, HT_A_MESSAGE);
	HTAlert_add(HTConfirm, HT_A_CONFIRM);
	HTAlert_add(HTPrompt, HT_A_PROMPT);
	HTAlert_add(HTPromptPassword, HT_A_SECRET);
	HTAlert_add(HTPromptUsernameAndPassword, HT_A_USER_PW);
    }

    /* Register a call back function for the Net Manager */
    HTNetInit();
    HTNetCall_addAfter(terminate_handler, HT_ALL);

    /* Register our own MIME header handler for extra headers */
    HTHeader_addParser("*", NO, header_handler);

    /* Set timeout on sockets */
    if (lm->tv->tv_sec < 0) {
	lm->tv->tv_sec = HTAlert_interactive() ?
	    DEFAULT_I_TIMEOUT : DEFAULT_NI_TIMEOUT;
    }
    HTEvent_registerTimeout(lm->tv, lm->request, timeout_handler, NO);

    /* Set max number of sockets we want open simultanously */
    HTNet_setMaxSocket(6);

    /* Set the DNS cache timeout */
    HTDNS_setTimeout(3600);

    /* Rule file specified? */
    if (lm->rules) {
	HTList * list = HTList_new();
	HTRequest * rr = Thread_new(lm, NO, LM_NO_UPDATE);
	char * rules = HTParse(lm->rules, lm->cwd, PARSE_ALL);
	HTParentAnchor * ra = (HTParentAnchor *) HTAnchor_findAddress(rules);
	HTRequest_setPreemtive(rr, YES);
	HTConversion_add(list, "application/x-www-rules", "*/*", HTRules,
			 1.0, 0.0, 0.0);
	HTRequest_setConversion(rr, list, YES);
	if (HTLoadAnchor((HTAnchor *) ra, rr) != YES)
	    if (SHOW_MSG) TTYPrint(TDEST, "Can't access rules\n");
	HTConversion_deleteAll(list);
	FREE(rules);
    }

    /* Start the request */
    if (keywords)
	status = HTSearch(HTChunk_data(keywords), lm->anchor, lm->request);
    else
	status = HTLoadAnchor((HTAnchor *) lm->anchor, lm->request);

    if (keywords) HTChunk_delete(keywords);
    if (status != YES) {
	if (SHOW_MSG) TTYPrint(TDEST, "Couldn't load home page\n");
	Cleanup(lm, -1);
    }

    /* Set up the rest if we are in interactive mode */
    if (HTAlert_interactive()) {

	/* Start History manager */
	lm->history = HTHistory_new();

	/* Register our own memory cache handler (implemented in GridText.c) */
	HTMemoryCache_register(HTMemoryCache);

	/*
	** Register STDIN as the user socket IF not STDIN is connected to
	** /dev/null or other non-terminal devices
	*/
#ifdef STDIN_FILENO
	if (isatty(STDIN_FILENO)) {
	    HTEvent_RegisterTTY(STDIN_FILENO, lm->request, (SockOps)FD_READ,
				scan_command, HT_PRIORITY_MAX);
	}
#else
	HTEvent_RegisterTTY(0, lm->request, (SockOps)FD_READ, scan_command, 1);
#endif
    }

    /* Go into the event loop... */
    HTEvent_Loop(lm->request);

    /* Only gets here if event loop fails */
    Cleanup(lm, 0);
    return 0;
}
