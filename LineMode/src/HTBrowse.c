/*         		    					     HTBrowse.c
**	HYPERTEXT BROWSER FOR DUMB TERMINALS
**
**	(c) COPRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
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
*/

#include "WWWLib.h"
#include "WWWMIME.h"
#include "WWWCache.h"
#include "WWWApp.h"
#include "WWWTrans.h"
#include "WWWInit.h"

#include "GridText.h"				     /* Hypertext definition */
#include "HTBrowse.h"			     /* Things exported, short names */

#include "CSLApp.h" /* the PICApp library should provide everything the app needs */
#include "CSLL.h"
#include "CSUser.h"
#include "CSUsrLst.h"

#ifndef W3C_VERSION
#define W3C_VERSION		"Unspecified"
#endif

#define APP_NAME		"W3CLineMode"
#define APP_VERSION		W3C_VERSION

/* Default page for "Manual" command */
#define MANUAL	"http://www.w3.org/LineMode/User/QuickGuide.html"

/* Default page for "help" command */
#define C_HELP	"http://www.w3.org/LineMode/User/Commands.html"

/* Default page for "-help" command line option */
#define L_HELP	"http://www.w3.org/LineMode/User/CommandLine.html"

#define DEFAULT_OUTPUT_FILE	"www.out"
#define DEFAULT_RULE_FILE	"www.conf"
#define DEFAULT_LOG_FILE       	"www.log"
#define DEFAULT_MEMLOG		"www.mem"
#define DEFAULT_USERLIST_FILE	"PICSusrs.html"

#if 0
#define HT_MEMLOG
#endif

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

#define DEFAULT_I_TIMEOUT	1000       /* Interactive timeout in millies */
#define DEFAULT_NI_TIMEOUT	10000  /* Non-interactive timeout in millies */

#define DEFAULT_FORMAT		WWW_PRESENT

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

struct _LineMode {
    HTRequest *		console;			   /* For user input */
    HTParentAnchor *	anchor;
    HTList *		active;			  /* List of acitve contexts */
    HTList *		presenters;
    HTHistory *		history;    			     /* History list */
    char *		cwd;				  /* Current dir URL */
    char *		rules;
    char *		logfile;
    HTLog * 		log;
    char *		outputfile;
    char *		host;
    int			trace;
    int			timer;
    HTFormat		format;		        /* Input format from console */
    LMFlags		flags;
    HTView *		pView;
    char *		userList;
    CSUser_t * 		pCSUser;
    BOOL		noPICSPasswords;
};

typedef enum _LMState {
    LM_UPDATE	= 0x1,
    LM_NO_UPDATE= 0x2,
    LM_DONE	= 0x4,
    LM_INACTIVE	= 0x8,
    LM_IGNORE	= 0x10
} LMState;

typedef struct _Context {
    LMState		state;
    HTRequest *		request;
    LineMode *		lm;
    HTParentAnchor *	source;	   /* The source if we are using PUT or POST */
} Context;

PRIVATE int scan_command (SOCKET s, void * param, HTEventType type);

PRIVATE HTEvent * ConsoleEvent = NULL;

#ifndef WWW_WIN_WINDOW
PRIVATE FILE *		OUTPUT = NULL;
#endif
 
PRIVATE InputParser_t parse_command;
InputParser_t * PInputParser = &parse_command;

/* Net callback handlers */
PRIVATE HTNetBefore MemoryCacheFilter;
PRIVATE HTNetAfter  terminate_handler;

/* additions for browser update */
extern HText * HTMainText;
extern HTParentAnchor * HTMainAnchor;
extern void LMHText_beginElement (HText * text,
    int elem_num, const BOOL * present, const char ** value);
extern void LMHText_endElement (HText * text, int elem_num);

/* ------------------------------------------------------------------------- */

PUBLIC int HTWatch(int id, void * obj, const char * fmt, ...)
{
    va_list pArgs;
    va_start(pArgs, fmt);
    fprintf(stderr, "id: %x  obj: %p: ", id, obj);
    return vfprintf(stderr, fmt, pArgs);
}

/*	Create a Context Object
**	-----------------------
*/
PRIVATE Context * Context_new (LineMode *lm, HTRequest *request, LMState state)
{
    Context * me;
    if ((me = (Context  *) HT_CALLOC(1, sizeof (Context))) == NULL)
        HT_OUTOFMEM("Context_new");
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
    HT_FREE(old);
    return YES;
}

PUBLIC LineMode * Context_getLineMode(HTRequest * request)
{
    return ((Context *) HTRequest_context(request))->lm;
}

/*
**  This function creates a new request object and adds it to the global
**  list of active threads
*/
CSDisposition_callback PICSCallback;
PRIVATE HTRequest * Thread_new (LineMode * lm, BOOL Interactive, LMState state)
{
    HTRequest * newreq = HTRequest_new();
    if (!lm) return NULL;
    Context_new(lm, newreq, state);
    if (Interactive) HTRequest_setConversion(newreq, lm->presenters, NO);
    if (lm->flags & LM_PREEMTIVE) HTRequest_setPreemptive(newreq, YES);
    HTRequest_addRqHd(newreq, HT_C_HOST);
    CSApp_registerReq(newreq, lm->pCSUser, PICSCallback, 
		      CSApp_callOnBad, (void *)lm);
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
		    HTTRACE(APP_TRACE, "NOT FOUND\n");
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
PRIVATE LineMode * LineMode_new (void)
{
    LineMode * me;
    if ((me = (LineMode *) HT_CALLOC(1, sizeof(LineMode))) == NULL)	
	HT_OUTOFMEM("LineMode_new");
    me->cwd = HTGetCurrentDirectoryURL();
    me->active = HTList_new();
    me->console = HTRequest_new();
    Context_new(me, me->console, LM_UPDATE);
    me->trace = SHOW_ALL_TRACE;
    me->timer = -1;
    me->pCSUser = 0;
    if (!(me->pView = HTView_create("'nother Window", 25, 80, me)))
    	return 0;
    return me;
}

/*	Delete a Line Mode Object
**	-------------------------
*/
PRIVATE BOOL LineMode_delete (LineMode * lm)
{
    if (lm) {
	Thread_deleteAll(lm);
	HTPresentation_deleteAll(lm->presenters);
	HTHistory_delete(lm->history);
	HT_FREE(lm->cwd);
	if (lm->log) HTLog_close(lm->log);
	if (lm->pCSUser)
	    CSLoadedUser_remove(lm->pCSUser);
	HTView_destroy(lm->pView);
#ifndef WWW_WIN_WINDOW
	if (OUTPUT && OUTPUT != stdout) fclose(OUTPUT);
#endif
	HText_freeAll();	/* Free all loaded documents */
	HT_FREE(lm);
	return YES;
    }
    return NO;
}

PUBLIC HTRequest * LineMode_getConsole(LineMode * pLm)
{
    return pLm->console;
}

PUBLIC HTView * LineMode_getView(LineMode * pLm)
{
    return pLm ? pLm->pView : 0;
}

PRIVATE BOOL LineMode_load(LineMode * lm, HTParentAnchor * anchor, 
			   char * url, BOOL preemptive, HTRequest ** pPReq)
{
    char * fullURL;
    HTParentAnchor * pAnchor;

    *pPReq = Thread_new(lm, YES, LM_UPDATE);
    if (preemptive)
        HTRequest_setPreemptive(*pPReq, YES);
    if (anchor)
	return HTLoadRelative(url, anchor, *pPReq);
    fullURL = HTParse(url, lm->cwd, PARSE_ALL);
    pAnchor = (HTParentAnchor *) HTAnchor_findAddress(fullURL);
    HT_FREE(fullURL);
    return HTLoadAnchor((HTAnchor *) pAnchor, *pPReq);
}

PRIVATE void Cleanup (LineMode * me, int status)
{
    if (HTAlert_interactive())	   /* Terminate with a LF if not interactive */
	OutputData(me->pView, "\n");
    CSLoadedUser_deleteAll();
    CSApp_unregisterApp();

    LineMode_delete(me);
    HTProfile_delete();
#ifdef HT_MEMLOG
    HTMemLog_close();
#endif

#ifdef VMS
    exit(status ? status : 1);
#else
    exit(status ? status : 0);
#endif
}

/*
** Get size of the output screen. Stolen from less.
*/
PRIVATE void scrsize (int * p_height, int * p_width)
{
#if defined(HAVE_IOCTL) && defined(HAVE_WINSIZE) && defined(TIOCGWINSZ)
    register char *s;
    /* int ioctl(); - henrik */
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
#else
    *p_height = SCREEN_HEIGHT;
    *p_width = SCREEN_WIDTH;
#endif /* HAVE_IOCTL && HAVE_WINSIZE */
}

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
	HTTRACE(APP_TRACE, "HTSignal.... Can't catch SIGPIPE\n");
    } else {
	HTTRACE(APP_TRACE, "HTSignal.... Ignoring SIGPIPE\n");
    }

#ifdef HT_MEMLOG
    HTMemLog_flush();
#endif

}
#endif /* CATCH_SIG */

/*  Print version information
**  -------------------------
*/
PRIVATE void VersionInfo (LineMode * lm)
{
    HTView * pView = lm ? lm->pView : NULL;
    OutputData(pView, "\n\nW3C Reference Software\n\n");
    OutputData(pView, "\tW3C Line Mode Browser version %s.\n", APP_VERSION);
    OutputData(pView, "\tW3C Reference Library version %s.\n\n",HTLib_version());
    OutputData(pView, "Please send feedback to <libwww@w3.org>\n");
}

/* 	Reference_List
**	--------------
**  	Print out a list of HyperText References from current document
*/
PRIVATE void Reference_List (LineMode * lm, BOOL titles)
{
    int refs = HText_sourceAnchors(HTMainText);
    if (refs <= 0) {
	OutputData(lm->pView, "\n\nThere are no references from this document.\n\n");
    } else {
	int cnt;
	OutputData(lm->pView, "\n*** References from this document ***\n");
	for (cnt=1; cnt<=refs; cnt++) {
	    HTLink * link =
		HTAnchor_mainLink((HTAnchor*)HText_childNumber(HTMainText,cnt));
	    HTAnchor * dest = HTLink_destination(link);
	    HTParentAnchor * parent = HTAnchor_parent(dest);
	    char * address =  HTAnchor_address(dest);
	    const char * title = titles ? HTAnchor_title(parent) : NULL;
	    OutputData(lm->pView, reference_mark, cnt);
	    OutputData(lm->pView, "%s%s\n",
		    ((HTAnchor*)parent!=dest) && title ? "in " : "",
		    (char *)(title ? title : address));
	    HT_FREE(address);
	}
#ifndef WWW_WIN_WINDOW
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
    OutputData(lm->pView, "\n  Documents you have visited: ");
    if (max > MAX_HISTORY) {
	max = MAX_HISTORY;
	OutputData(lm->pView, "(truncated)\n");
    } else
	OutputData(lm->pView, "\n");
    for (cnt=1; cnt<=max; cnt++) {
	HTAnchor *anchor = HTHistory_list(lm->history, cnt);
    	char *address = HTAnchor_address(anchor);
	HTParentAnchor *parent = HTAnchor_parent(anchor);
	const char *title = HTAnchor_title(parent);
	OutputData(lm->pView, "%s R %d\t%s%s\n",
	       (cnt==current) ? "*" : " ",
	       cnt,
	       ((HTAnchor*)parent!=anchor) && title ? "in " : "",
	       title ? title : address);
	HT_FREE(address);
    }
    OutputData(lm->pView, "\n");
}

/*	Prompt for answer and get text back. Reply text is either NULL on
**	error or a dynamic string which the caller must HT_FREE.
*/
PRIVATE char * AskUser (HTRequest * request, const char * Msg,
			const char * deflt)
{
    char buffer[200];
    char *reply = NULL;
    HTPrint("%s ", Msg ? Msg : "UNKNOWN");
    if (deflt)
	HTPrint("(RETURN for [%s]) ", deflt);

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

PRIVATE BOOL confirm (HTRequest * request, const char * Msg)
{
    char response[4];
    HTPrint("%s (y/n) ", Msg ? Msg : "UNKNOWN");
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
	    OutputData(lm->pView, "1-%d, ", refs);
	else
	    OutputData(lm->pView, "1, ");	
    }
    if (HText_canScrollUp(HTMainText)) {
	OutputData(lm->pView, "Top, ");
	OutputData(lm->pView, "Up, ");
    }
    if (HText_canScrollDown(HTMainText)) {
	OutputData(lm->pView, "BOttom, ");
	OutputData(lm->pView, "Down or <RETURN> for more,");
    }

    /* Second Line */
    OutputData(lm->pView, "\n");
    if (HTHistory_canBacktrack(lm->history))
	OutputData(lm->pView, "Back, ");
    if (HTHistory_canForward(lm->history))
	OutputData(lm->pView, "Forward, ");
    if (is_index)
	OutputData(lm->pView, "FIND <keywords>, ");
    OutputData(lm->pView, "Quit, or Help: ");	
    fflush(stdout);  	           	  /* For use to flush out the prompt */
    return;
}

/*
**  PUT the current document to the destination specified by the user
**  Returns the result of the load function.
*/
PRIVATE int PutAnchor (LineMode * lm, HTRequest * request)
{
    char * dest = AskUser(request, "Destination: ", NULL);

    /*
    ** If user has typed a destination then create a new request object and
    ** start the PUT operation. The destination is taken relative to the
    ** current location.
    */
    if (dest) {
	HTRequest * new_request = Thread_new(lm, YES, LM_UPDATE);
	HTPutDocumentRelative(HTMainAnchor, dest, HTMainAnchor, new_request);
	HT_FREE(dest);
    }
    return YES;
}

/*
**  Delete the current document.
*/
PRIVATE int DeleteAnchor (LineMode * lm, HTRequest * request)
{
    int status = HT_INTERNAL;
    BOOL doit = confirm(request, "Delete the current document?");
    if (doit) {
	HTRequest * new_request = Thread_new(lm, YES, LM_UPDATE);
	status = HTDeleteAnchor((HTAnchor *) HTMainAnchor, new_request);
    }
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
	if (SHOW_MSG) HTPrint("Can't access file (%s)\n", fname);
	return NO;
    }
    HTRequest_setOutputStream(req, HTFWriter_new(req, fp, NO));
    if (SHOW_MSG) HTPrint("Saving to file `%s\'\n", fname);
    return (HTLoadAnchor((HTAnchor*) HTMainAnchor, req) != HT_WOULD_BLOCK);
}

CSError_t LoadedUsersCallback(CSUser_t * pCSUser, int index, void * pVoid)
{
    LineMode * lm = (LineMode *)pVoid;
    OutputData(lm->pView, "%d: %s\n", index, CSUser_name(pCSUser));
    return CSError_OK;
}

CSError_t UserListCallback(char * username, char * url, int index, void * pVoid)
{
    LineMode * lm = (LineMode *)pVoid;
    OutputData(lm->pView, "%d: %s %s\n", index, username, url);
    return CSError_OK;
}

PRIVATE BOOL ShowPICSUsers(LineMode * lm)
{
    OutputData(lm->pView, "Loaded users:\n");
    CSLoadedUser_enum(&LoadedUsersCallback, (void *)lm);
    OutputData(lm->pView, "Listed users:\n");
    CSUserList_enum(&UserListCallback, (void *)lm);
    return YES;
}

PRIVATE int PICS_userCallback(CSUser_t * pCSUser, void * pVoid)
{
    LineMode * lm = (LineMode *)pVoid;
    char * userName;
    char * password = NULL;
    HTAlertCallback *cbf;
    int ret;
    HTAlertPar * reply;

    if (lm->noPICSPasswords) {
        lm->pCSUser = pCSUser;
	return 0;
    }
    if (!(cbf = HTAlert_find(HT_A_USER_PW)))
        return -1;
    reply = HTAlert_newReply();
    userName = CSUser_name(pCSUser);
    if ((ret = (*cbf)(lm->console, HT_A_USER_PW, 
		      HT_MSG_NULL, userName, NULL, reply))) {
        userName = HTAlert_replyMessage(reply);
        password = HTAlert_replySecret(reply);
    }
    HTAlert_deleteReply(reply);
    if (!ret) {
        HTPrint("PICS set user *canceled*.\n");
	return -1;
    }
    ret = -1;
    if (!userName)
	HTPrint("PICS cannot set to no name.\n");
    else if (CSUser_checkPassword(pCSUser, password) == NO)
        HTPrint("PICS password was not valid.\n");
    else {
        ret = 0;
	lm->pCSUser = pCSUser;
    }
    HT_FREE(userName);
    HT_FREE(password);
    return ret;
}

PRIVATE BOOL SetPICSUser(LineMode * lm, char * userName)
{
    char * password = NULL;
    HTAlertCallback *cbf;
    BOOL ret;
    HTAlertPar * reply;

    if (!(cbf = HTAlert_find(HT_A_USER_PW)))
        return NO;
    reply = HTAlert_newReply();
    if ((ret = (*cbf)(lm->console, HT_A_USER_PW, 
		      HT_MSG_NULL, userName, NULL, reply))) {
        userName = HTAlert_replyMessage(reply);
        password = HTAlert_replySecret(reply);
    }
    HTAlert_deleteReply(reply);
    if (!ret) {
        HTPrint("PICS set user *canceled*.\n");
	return NO;
    }
    ret = NO;
    if (!userName)
	HTPrint("Canceled.\n");
    else if (!(lm->pCSUser = CSApp_registerUserByName(userName, password))) {
        char * url;
        if ((url = CSUserList_findURL(userName)) == NULL)
	    HTPrint("PICS user \"%s\" is unknown.\n", userName);
	else if (!CSLoadedUser_load(url, lm->cwd))
	    HTPrint("Can't load PICS user \"%s\".\n", userName);
	else if ((CSLoadedUser_find(userName)) == NO)
	    HTPrint("PICS user \"%s\" not found in \"%s\".\n", userName, url);
	else if (!(lm->pCSUser = CSApp_registerUserByName(userName, password)))
	    HTPrint("Failed to set PICS user to \"%s\".\n", userName);
	ret = YES;
	HT_FREE(url);
    }
    HT_FREE(userName);
    HT_FREE(password);
    return ret;
}

PRIVATE BOOL LoadPICSUser(LineMode * lm, char * url)
{
    CSUser_t * pCSUser;
    if (!(pCSUser = CSLoadedUser_load(url, lm->cwd)))
        return NO;
    lm->pCSUser = pCSUser;
    return YES;
}

CSError_t PICSCallback(HTRequest* pReq, CSLabel_t * pCSLabel, 
		       CSUser_t * pCSUser, CSError_t disposition, void * pVoid)
{
    LineMode * lm = (LineMode *)pVoid;
    char * mesg;
    switch (disposition) {
        case CSError_RATING_RANGE:
            {
	    char * labelStr = CSLabel_getRatingStr(pCSLabel);
	    char * userStr = CSUser_getRatingStr(pCSUser);
	    char * anchorStr = HTAnchor_address((HTAnchor*)HTRequest_anchor(pReq));
            OutputData(lm->pView, "PICS user %s is not allowed to see document: %s\n", 
		       CSUser_name(pCSUser), anchorStr);
            OutputData(lm->pView, "  %s's \"%s\" rating for service %s (%s) did not include %s\n", 
		       CSUser_name(pCSUser), 
		       CSLabel_getRatingName(pCSLabel),
		       CSLabel_getServiceName(pCSLabel), 
		       userStr, labelStr);
	    HT_FREE(userStr);
	    HT_FREE(labelStr);
	    HT_FREE(anchorStr);
	    }
	    return disposition;
        case CSError_BUREAU_NONE: mesg="label bureau was not contacted"; break;
        case CSError_RATING_VALUE: mesg="value"; break;
        case CSError_RATING_MISSING: mesg="rating not found"; break;
        case CSError_SINGLELABEL_MISSING: mesg="no single-labels found"; break;
        case CSError_SERVICE_MISSING: mesg="service not available";break;
        case CSError_SERVICE_NONE: mesg="no services available for document"; break;
        case CSError_RATING_NONE: mesg="no ratings in a service"; break;
        case CSError_BAD_DATE: mesg="labels were out of date"; break;
        default:HTPrint("PICSCallback: odd error code: %d\n", disposition); return disposition;
    }
    OutputData(lm->pView, "PICS disallowed document: %s\n", mesg);
    return disposition;
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
PRIVATE int parse_command (char* choice, SOCKET s, HTRequest *req, HTEventType type)
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
    HTRequest * cur_req = NULL;
    Context * cur_context = NULL;
    LineMode * lm = NULL;

    cur_req = req;
    cur_context = (Context *) HTRequest_context(req);
    lm = cur_context->lm;

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
		HTChildAnchor *source = HText_childNumber(HTMainText, ref_num);
		if (source) {
		    HTLink * link = HTAnchor_mainLink((HTAnchor *) source);
		    HTAnchor * destination = HTLink_destination(link);
		    req = Thread_new(lm, YES, LM_UPDATE);		    

		    /* Continous browsing, so we want Referer field */
		    HTRequest_setParent(req, HTAnchor_parent((HTAnchor*)source));
		    HTLoadAnchor(destination, req);
		} else {
		    status = NO;				/* No anchor */
		}
	    } else {
		if (SHOW_MSG)
		    HTPrint("Warning: Invalid Reference Number: (%d)\n",
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
		OutputData(lm->pView, "\nThis is the first document in history list\n");
	    }
	} else if (CHECK_INPUT("BOTTOM", token)) {	/* Scroll to bottom  */
	    HText_scrollBottom(HTMainText);
	} else if (CHECK_INPUT("BYE", token)) {		    /* Quit program? */
	    HTNet_killAll();				/* Kill all requests */
	    status = NO;
	} else
	    found = NO;
	break;
	
      case 'C':
#ifdef HAVE_CHDIR
	if (CHECK_INPUT("CD", token)) {        /* Change working directory ? */
	    goto lcd;
	} else
#endif /* HAVE_CHDIR */
	if (CHECK_INPUT("CLEAR", token)) {	       /* Clear History list */
	    HTHistory_removeFrom(lm->history, 1);
	} else
	    found = NO;
	break;
	
      case 'D':
	if (CHECK_INPUT("DELETE", token)) {	    		   /* DELETE */
	    status = DeleteAnchor(lm, req);
	} else if (CHECK_INPUT("DOWN", token)) {    /* Scroll down one page  */
	  down:
	    if (HText_canScrollDown(HTMainText))
		HText_scrollDown(HTMainText);
	} else
	    found = NO;
	break;
	
      case 'E':
	if (CHECK_INPUT("EDIT", token)) {
	    status = PutAnchor(lm, req);
	} else if (CHECK_INPUT("EXIT", token)) {	    /* Quit program? */
	    HTNet_killAll();				/* Kill all requests */
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
		    status = HTSearchString(other_words,
					    (HTAnchor *) HTMainAnchor, req);
		}
	    }
	} else if (CHECK_INPUT("FLUSH", token)) {	 /* Flush file cache */
	    if (confirm(req, "Flush Persistent Cache?") == YES)
		HTCache_flushAll();

	} else if (CHECK_INPUT("FORWARD", token)) {
	    if (HTHistory_canForward(lm->history)) {
		req = Thread_new(lm, YES, LM_NO_UPDATE);
		status = HTLoadAnchor(HTHistory_forward(lm->history), req);
	    } else {
		OutputData(lm->pView, "\nThis is the last document in history list.\n");
	    }
	} else
	    found = NO;
	break;
	
      case 'G':
	if (CHECK_INPUT("GOTO", token)) {			     /* GOTO */
	    if (next_word)
	        status = LineMode_load(lm, HTMainAnchor, next_word, NO, &req);
	} else
	    found = NO;
	break;
	
      case '?':
	status = LineMode_load(lm, HTMainAnchor, C_HELP, YES, &req);
	break;
	
      case 'H':
	if (CHECK_INPUT("HELP", token)) {		     /* help menu, ..*/
	    status = LineMode_load(lm, HTMainAnchor, C_HELP, YES, &req);
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
#ifdef HAVE_CHDIR
	else if (CHECK_INPUT ("LCD", token)) {	       /* Local change dir ? */
	  lcd:
	    if (!next_word) {				 /* Missing argument */
		OutputData(lm->pView, "\nName of the new local directory missing.\n");
	    } else if (chdir (next_word)) {		 /* failed : say why */
		OutputData(lm->pView, "\n  ");
		perror (next_word);
	    } else {		    /* Success : display new local directory */
#ifdef HAVE_GETCWD
		OutputData(lm->pView, "\nLocal directory is now:\n %s\n",
			   getcwd (choice, sizeof(choice)));
#else
#ifdef HAVE_GETWD
		OutputData(lm->pView, "\nLocal directory is now:\n %s\n",
			   (char *) getwd (choice));
#else
#error "This platform doesn't support getwd or getcwd"
		if (SHOW_MSG)
		    HTPrint("This platform doesn't support getwd or getcwd\n");
#endif /* HAVE_GETWD */
#endif /* HAVE_GETCWD */
	    }
	}
#endif /* HAVE_CHDIR */
	else if (CHECK_INPUT("LUSER", token)) {	     /* List of references ? */
	    if (next_word) {
	        LoadPICSUser(lm, next_word);
	    } else {
	        HTPrint("URL needed\n");
	    }
	} else
	    found = NO;
	break;
	
      case 'M':
	if (CHECK_INPUT("MANUAL", token)) {		 /* Read User manual */
	    status = LineMode_load(lm, HTMainAnchor, MANUAL, YES, &req);
	} else
	    found = NO;
	break;
	
      case 'O':
	if (CHECK_INPUT("OPTIONS", token)) {		   /* OPTIONS method */
	    req = Thread_new(lm, YES, LM_NO_UPDATE);
	    status = HTOptionsAnchor((HTAnchor*)
				     (HTMainAnchor ? HTMainAnchor:lm->anchor),
				     req);
	} else
	    found = NO;
	break;
	
      case 'P':                    
	if (CHECK_INPUT("POST", token)) {
	    status = PutAnchor(lm, req);
	}

#ifdef HAVE_SYSTEM	    
	else if (!lm->host && CHECK_INPUT("PRINT", token)) {
	    char * address = HTAnchor_address((HTAnchor *) HTMainAnchor);
	    char * command;
	    char * tmplate = (char *) getenv("WWW_PRINT_COMMAND");
	    int result;
	    
	    if (!tmplate) tmplate = "www -n -na -p66 '%s' | lpr";
	    if ((command = (char *) HT_MALLOC(strlen(address)+strlen(tmplate)+20)) == NULL)
	        HT_OUTOFMEM("command");
	    sprintf(command, tmplate, address);
	    result = system(command);
	    HT_FREE(address);
	    HT_FREE(command);
	    if (result) OutputData(lm->pView, "  %s\n  returns %d\n", command, result);
	}
#endif
	/* this command prints the entire current text to the
	   terminal's printer; at the end it displays the top of the text */
#ifdef SLAVE_PRINTER
#define SLAVE_PRINTER_ON  "\033\133\065\151"
#define SLAVE_PRINTER_OFF "\033\133\064\151"
	
	else if (CHECK_INPUT("PS", token)) {
	    OutputData(lm->pView, "%s",SLAVE_PRINTER_ON);
	    OutputData(lm->pView, "\f");			   /* Form feed for new page */
	    HText_scrollTop(HTMainText);
	    while(HText_canScrollDown(HTMainText)) {
		HText_scrollDown(HTMainText);
	    }
	    OutputData(lm->pView, "\f");  			   /* Form feed for new page */
	    OutputData(lm->pView, "%s",SLAVE_PRINTER_OFF);
	    HText_scrollTop(HTMainText);
	}	
#endif
	else if (CHECK_INPUT("PUT", token)) {
	    status = PutAnchor(lm, req);
	} else
	    found = NO;
	break;
	
      case 'Q':						   /* Quit program ? */
	if (CHECK_INPUT("QUIT", token)) {

	    /* 	JFG 9/7/92, following a complaint of 'q' mis-typed for '1'.
		JFG Then made optional because I hate it !!!
		TBL made it only affect remote logged on users. 921122 */
	    
	    if (lm->host && (strcasecomp(token, "quit") != 0) ) {
		OutputData(lm->pView, "\n Please type \"quit\" in full to leave www.\n");
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
		OutputData(lm->pView, "\n  No other documents to recall.\n");
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
			    HTPrint("Bad command (%s), for list of commands type help\n", this_command);
		    }
		} else {
		    History_List(lm);
		}
	    }
	} else if (CHECK_INPUT("REFRESH", token)) {
	    HText_setStale(HTMainText);			    /* Force refresh */
	    HText_refresh(HTMainText);			   /* Refresh screen */

	} else if (CHECK_INPUT("REVALIDATE", token)) {
	    req = Thread_new(lm, YES, LM_NO_UPDATE);

	    /*
	    **  Add a end-to-end validator to the request
	    */
	    HTRequest_setReloadMode(req, HT_CACHE_END_VALIDATE);
	    status = HTLoadAnchor((HTAnchor*) (HTMainAnchor ? HTMainAnchor : 
					       lm->anchor), req);
	} else if (CHECK_INPUT("RELOAD", token)) {
	    req = Thread_new(lm, YES, LM_NO_UPDATE);

	    /*
	    **  Make sure that we do a complete flush of the cache
	    */
	    HTRequest_setReloadMode(req, HT_CACHE_FLUSH);
	    status = HTLoadAnchor((HTAnchor*) (HTMainAnchor ? HTMainAnchor : 
				  lm->anchor), req);
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
	} else if (CHECK_INPUT("USER", token)) {
	    if (next_word) {
			if (!strcasecomp(next_word, "?")) {
				ShowPICSUsers(lm);
				SetPICSUser(lm, 0);
			} else {
				SetPICSUser(lm, next_word);
			}
	    } else {
	        SetPICSUser(lm, 0);
		}
	} else 
	    found = NO;
	break;
	
      case 'V':
	if (CHECK_INPUT("VALIDATE", token)) {    	   /* Cache validate */
	    req = Thread_new(lm, YES, LM_NO_UPDATE);

	    /*
	    **  Add a validator to the request
	    */
	    HTRequest_setReloadMode(req, HT_CACHE_VALIDATE);
	    status = HTLoadAnchor((HTAnchor*) (HTMainAnchor ? HTMainAnchor : 
					       lm->anchor), req);
	} else if (CHECK_INPUT("VERBOSE", token)) {  /* Switch verbose mode  */
	    WWWTRACE = WWWTRACE ? 0 : lm->trace;
	    OutputData(lm->pView, "\n  Verbose mode %s.\n", WWWTRACE ? "ON":"OFF");
	} else if (CHECK_INPUT("VERSION", token)) {	 	  /* Version */
	    VersionInfo(lm);
	} else
	    found = NO;
	break;

      case 'W':
	if (CHECK_INPUT("WHICH", token)) {     /* Show title/URL of current page */
	    HTView * pView = lm ? lm->pView : NULL;
	    char * current_address = HTAnchor_address((HTAnchor *) HTMainAnchor);
	    const char * title = HTAnchor_title(HTMainAnchor);
	    if (title)
		OutputData(pView, "\n\nYou are reading\n\t`%s\'\nwith address\n\t%s\n\n",
			   title, current_address);
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
	    HTRequest_setReloadMode(req, HT_CACHE_FLUSH_MEM);
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
	    if ((command = (char *) HT_MALLOC(strlen(address) +strlen(this_command)+30)) == NULL)
	        HT_OUTOFMEM("command");
	    sprintf(command, "www %s \"%s\" %s", 
		    OutSource ? "-source" : "-n -na -p", address,this_command);
	    OutputData(lm->pView, "Command: %s\n", command);
	    result = system(command);
	    if (result) OutputData(lm->pView, "  %s  returns %d\n", command, result);
	    HT_FREE(command);
     	    HT_FREE(address);
	}
	break;
#endif
	    
#ifdef HAVE_SYSTEM
      case '!':
	if (!lm->host) {				      /* Local only! */
	    int result;
	    if (SHOW_MSG) HTPrint("Executing `%s\'\n", this_command);
	    result = system(strchr(this_command, '!') + 1);
	    if (result) OutputData(lm->pView, "  %s  returns %d\n",
				 strchr(this_command, '!') + 1, result);
	}
	break;
#endif /* HAVE_SYSTEM */
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
		HTPrint("Bad command (%s), for list of commands type help\n", this_command);
	}
    }
    MakeCommandLine(lm, is_index);
    HT_FREE(the_choice);

    /* 
    ** If we have created a new Request and is to update the history list then
    ** we can set the inactive bit on this request object.
    */
    if (cur_req == req)
	cur_context->state |= LM_NO_UPDATE;
    else
	cur_context->state |= LM_INACTIVE;    
    return (status==YES) ? HT_OK : HT_ERROR;
}

/*	readConsole
**	-----------
**	non-blocking read of the WIN32 console. EGP
*/

#ifdef WWW_WIN_CONSOLE
PUBLIC BOOL readConsole(HANDLE conIn, char* buf, int len, int* pRed)
{
    DWORD recordIndex, bufferIndex, toRead, red;
    PINPUT_RECORD pInput;

    /* grab the pending input records (keystrokes plus other garbage). */
    GetNumberOfConsoleInputEvents(conIn, &toRead);
    if (len < (int)toRead)	/* we'll get the rest on the next pass(es). */
    toRead = len;
    if ((pInput = (PINPUT_RECORD) HT_MALLOC(toRead * sizeof(INPUT_RECORD))) == NULL)	/* room for n input records */
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
    		    OutputData(lm->pView, "\b ");
    		OutputData(lm->pView, "%c", buf[bufferIndex]);
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
    		    OutputData(lm->pView, "\b ");
    		OutputData(lm->pView, "%c", buf[bufferIndex]);
    		bufferIndex++;
    		pInput[recordIndex].Event.KeyEvent.wRepeatCount--;
    	    }
    	}
#endif

    }

    HT_FREE(pInput);
    *pRed = bufferIndex;		/* actual characters stuck into buffer */
    return (TRUE);
}
#endif /* WWW_WIN_CONSOLE */

/*	bufferInput
**	-----------
**	Read available characters from buf into stat. buf maybe bigger or
**	smaller than stat.
*/
PUBLIC int bufferInput (char* buf, int len, SOCKET s, HTRequest * req, HTEventType type)
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
		    if ((ret = (*PInputParser)(stat, s, req, type)) != HT_OK)
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
	    HTPrint("Read Console... BUFFER OVERRUN\n");
	    iStat = 0;
	    ignoreNext = 1;
	}
    }
    return (HT_OK);
}

/*	timeout_handler
**	---------------
**	This function is registered to handle timeout in select eventloop
*/
PRIVATE int timeout_handler (SOCKET s, void * param, HTEventType type)
{
    if (!HTAlert_interactive()) {
	HTRequest * req = (HTRequest *) param;
	Context * context = (Context *) HTRequest_context(req);
	LineMode * lm = context->lm;
	if (SHOW_MSG) HTPrint("Request timed out");
	HTNet_killAll();
	Cleanup(lm, -1);
    }
    if (HTNet_count() > 0)
	if (SHOW_MSG) HTPrint(".");
    return 0;
}

PRIVATE int scan_command (SOCKET s, void * param, HTEventType type)
{
    HTRequest * req = (HTRequest *)param;
    /* buf happens to == eatText's buffer but not neccesary */
    static char buf[RESPONSE_LENGTH];
#ifdef WWW_MSWINDOWS
    int red;
    int ret;
#endif

    /* Handle any timeout here */
    if (type == HTEvent_TIMEOUT) return timeout_handler (s, param, type);

#ifdef WWW_MSWINDOWS
    while(1) {
#ifdef WWW_WIN_CONSOLE
	if (!readConsole((HANDLE)s, buf, sizeof(buf), &red)) {
        HTTRACE(PROT_TRACE, "Read Console... READ ERROR\n");
	    return HT_ERROR;
	}
#endif /* WWW_WIN_CONSOLE */
	if (!red) return (HT_OK);
	ret = bufferInput(buf, red, s, req, type);
	if (ret != HT_OK) return (ret);
    }
#else /* WWW_MSWINDOWS */
    if (!fgets(buf, sizeof(buf), stdin))		  /* Read User Input */
        return HT_ERROR;				      /* Exit if EOF */
    return ((*PInputParser)(buf, s, req, type));
#endif /* !WWW_MSWINOWS */
}

/*	terminate_handler
**	-----------------
**	This function is registered to handle the result of the request
*/
PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    Context * context = (Context *) HTRequest_context(request);
    LineMode * lm;
    BOOL is_index;

    if (!context)
        return HT_OK;
    lm = context->lm;
    if (context->state == LM_IGNORE) return HT_OK;
    if (CSApp_unregisterReq(request) == NO && lm->pCSUser)
        HTPrint("PICS request not found\n");
    is_index = HTAnchor_isIndex(HTMainAnchor);
    if (status == HT_LOADED) {

	/* Should we output a command line? */
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

    } else { /* No page loaded so sit around and wait for a go command */
	/*	was MakeCommandLine(lm, is_index); */
	/*
	**	stolen from above
	*/
	if (HTAlert_interactive()) {
/*	    HText_setStale(HTMainText); */
	    MakeCommandLine(lm, is_index);
	} else {
	    if (lm->flags & LM_REFS) Reference_List(lm, NO);
	    Cleanup(lm, 0);
	}

    }
    context->state |= LM_DONE;
    Thread_cleanup(lm);
    if (!HTAlert_interactive()) Cleanup(lm, -1);
    return HT_OK;
}

/*
**	Check the Memory Cache (History list) BEFORE filter
**	---------------------------------------------------
**	Check if document is already loaded. The user can define whether
**	the history list should follow normal expiration or work as a
**	traditional history list where expired documents are not updated.
**	We don't check for anything but existence proof of a document
**	associated with the anchor as the definition is left to the application
*/
PRIVATE int MemoryCacheFilter (HTRequest * request, void * param, int mode)
{
    HTReload validation = HTRequest_reloadMode(request);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    void * document = HTAnchor_document(anchor);

    /*
    **  We only check the memory cache if it's a GET method
    */
    if (HTRequest_method(request) != METHOD_GET) {
	HTTRACE(APP_TRACE, "Mem Cache... We only check GET methods\n");
	return HT_OK;
    }

    /*
    **  If we are asked to flush the persistent cache then there is no reason
    **  to do anything here - we're flushing it anyway. Also if no document
    **  then just exit from this filter.
    */
    if (!document || validation > HT_CACHE_FLUSH_MEM) {
	HTTRACE(APP_TRACE, "Mem Cache... No fresh document...\n");
	return HT_OK;
    }

    /*
    **  If we have a document object associated with this anchor then we also
    **  have the object in the history list. Depending on what the user asked,
    **  we can add a cache validator
    */
    if (document && validation != HT_CACHE_FLUSH_MEM) {
	HTParentAnchor * parent = HTRequest_anchor(request);
	HTChildAnchor * child = HTRequest_childAnchor(request);
	HText * document =  HTAnchor_document(parent);
	HTTRACE(APP_TRACE, "Mem Cache... Document %p already in memory\n" _ document);

	/*
	**  Make sure that we have selected the HText object. This is normally
	**  done by the HText interface but must be repeated here.
	*/
	if (child && (HTAnchor *) child != (HTAnchor *) parent)
	    HText_selectAnchor(document, child);
	else
	    HText_select(document);
	return HT_LOADED;
    }
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
    HTRequest *	request = NULL;
    LineMode *	lm;
    char *      picsUser = NULL;

#ifndef WWW_WIN_WINDOW
    OUTPUT = stdout;
#endif

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

#ifdef HT_MEMLOG
    HTMemLog_open(DEFAULT_MEMLOG, 8192, YES);
    HTTraceData_setCallback(HTMemLog_callback);
#endif

    /* Initiate W3C Reference Library with a client profile */
    HTProfile_newClient(APP_NAME, APP_VERSION);
    
    /* It's confusing to have progress notofications in linemode browser */
    HTAlert_deleteOpcode(HT_A_PROGRESS);

    /* Add the default HTML parser to the set of converters */
    {
	HTList * converters = HTFormat_conversion();
	HTMLInit(converters);
    }

    /* Create a new Line Mode object */
    lm = LineMode_new();
    request = Thread_new(lm, NO, LM_UPDATE);

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
		HTRequest_setOutputFormat(request, format);
		HTAlert_setInteractive(NO);

	    /* reformat html */
	    } else if (!strcmp(argv[arg], "-reformat")) {
		HTRequest_setOutputFormat(request, WWW_HTML);
		lm->flags |= LM_REFORMAT;
		HTAlert_setInteractive(NO);

	    /* List References */
	    } else if (!strncmp(argv[arg], "-list", 5)) {
		lm->flags |= LM_REFS;
		HTAlert_setInteractive(NO);

	    /* original output */
	    } else if (!strcmp(argv[arg], "-raw")) {
		HTRequest_setOutputFormat(request, WWW_RAW);
		HTAlert_setInteractive(NO);

	    /* source please */
	    } else if (!strcmp(argv[arg], "-source")) {
		HTRequest_setOutputFormat(request, WWW_SOURCE);
		HTAlert_setInteractive(NO);

	    /* HEAD method */
	    } else if (!strcasecomp(argv[arg], "-head")) {
		HTRequest_setMethod(request, METHOD_HEAD);
		HTRequest_setOutputFormat(request, WWW_MIME);
		HTAlert_setInteractive(NO);

	    /* output filename */
	    } else if (!strcmp(argv[arg], "-o")) { 
		lm->outputfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_OUTPUT_FILE;
		HTAlert_setInteractive(NO);

	    /* print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
		VersionInfo(lm);
		Cleanup(lm, 0);

	    /* -? or -help: show the command line help page */
	    } else if (!strcmp(argv[arg],"-?") || !strcmp(argv[arg],"-help")) {
		lm->anchor = (HTParentAnchor *) HTAnchor_findAddress(L_HELP);
		keycnt = 1;

#ifdef CYRILLIC
	    /* HWL 18/7/94: applied patch from agl@glas2.glas.apc.org
	       (Anton Tropashko) */
	    } else if (!strcmp(argv[arg], "-koi2alt")) {
	        doia2k(); OutputData(lm->pView, "Ahak2a!");
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
		if (timeout > 0) lm->timer = timeout;

	    /* PICS user */
	    } else if (!strcmp(argv[arg], "-u")) {
		picsUser = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : "user";
	    } else if (!strcmp(argv[arg], "-unp")) {
	        lm->noPICSPasswords = 1;
#if 0 /* old version of user list file, still works when enabeled here */
	    /* PICS user list */
	    } else if (!strcmp(argv[arg], "-ul")) {
		picsUserList = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : "users.url";
#endif
	    /* PICS user list */
	    } else if (!strcmp(argv[arg], "-ul")) {
		lm->userList = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_USERLIST_FILE;

	    /* PICS mode */
	    } else if (!strcmp(argv[arg], "-pics")) {
		lm->userList = DEFAULT_USERLIST_FILE;

	    /* preemptive or non-preemptive access */
	    } else if (!strcmp(argv[arg], "-single")) {
		HTRequest_setPreemptive(request, YES);
		lm->flags |= LM_PREEMTIVE;

	    /* Disconnected operation */
	    } else if (!strncmp(argv[arg], "-discon", 7)) {
		HTCacheMode_setDisconnected(HT_DISCONNECT_NORMAL);

	    /* Disable the cache */
	    } else if (!strncmp(argv[arg], "-nocache", 8)) {
		HTCacheMode_setEnabled(NO);

	    /* Handling of Expire (cache) */
	    } else if (!strncmp(argv[arg], "-x", 2)) { 
	    	  char *p = argv[arg]+2;
		  for(;*p;p++) {
		      switch (argv[arg][2]) {
		      case 'i':
			  HTCacheMode_setExpires(HT_EXPIRES_IGNORE);
			  break;
		      case 'n':
			  HTCacheMode_setExpires(HT_EXPIRES_NOTIFY);
			  break;
		      case 'a':
			  HTCacheMode_setExpires(HT_EXPIRES_AUTO);
			  break;
		      default:
			  if (SHOW_MSG)
			      HTPrint("Bad parameter (%s) for option -x\n", argv[arg]);
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
			    HTPrint("Bad parameter (%s) in -d option\n",
				    argv[arg]);
		    }
		}
#endif

#ifdef WWWTRACE
	    /* trace flags */
	    } else if (!strncmp(argv[arg], "-v", 2)) {
		lm->trace = HTSetTraceMessageMask(argv[arg]+2);
#endif
	    } else {
		if (SHOW_MSG) HTPrint("Bad Argument (%s)\n", argv[arg]);
	    }
	} else {	   /* If no leading `-' then check for main argument */
    	    if (!keycnt) {
		char * ref = HTParse(argv[arg], lm->cwd, PARSE_ALL);
		lm->anchor = HTAnchor_parent(HTAnchor_findAddress(ref));
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

    /* Set up HText interface callbacks */
    if (HTAlert_interactive () ) {
	/* interactive mode */
	if (!HText_registerCDCallback (LMHText_new, LMHText_delete)) {
	    HTPrint("HText new/delete callback registration failed.\n");
	    exit (1);
	}
    } else {
	/* non-interactive mode */
	if (!HText_registerCDCallback (LMHText_new2, LMHText_delete)) {
	    HTPrint("HText new/delete callback registration failed.\n");
	    exit (1);
	}
    }
    if (!HText_registerTextCallback (LMHText_addText)) {
	HTPrint("HText addText callback registration failed.\n");
	exit (2);
    }
    if (!HText_registerLinkCallback (LMHText_beginAnchor)) {
	HTPrint("HText link callback registration failed.\n");
	exit (3);
    }
    if (!HText_registerElementCallback (
    	LMHText_beginElement, LMHText_endElement)) {
	HTPrint("HText element callback registration failed.\n");
	exit (3);
    }

    /* Make home page address */
    if (!lm->anchor) lm->anchor = HTHomeAnchor();

    /* Do we need list of presenters? (external viewers) */
    if (HTAlert_interactive()) {
	lm->presenters = HTList_new();
	HTPresenterInit(lm->presenters);
	HTRequest_setConversion(request, lm->presenters, NO);
    } else {

	/*
	** Add default content decoder. We insert a through line as it doesn't
	** matter that we get an encoding that we don't know when not in
	** interactive mode. In interactive mode, we ask the user.
	*/
	HTFormat_addCoding("*", HTIdentityCoding, HTIdentityCoding, 0.3);
    }

    if (HTScreenHeight == -1) {				/* Default page size */
	if (HTAlert_interactive())
	    scrsize(&HTScreenHeight, &HTScreenWidth);
	else
	    HTScreenHeight = 999999;
    }

    /* Disable free directory browsing when using telnet host */
    if (lm->host && HTFile_dirAccess() == HT_DIR_OK)
	HTFile_setDirAccess(HT_DIR_SELECTIVE);

    /* Open output file */
    if (!HTAlert_interactive()) {
#ifndef WWW_WIN_WINDOW
	if (lm->outputfile) {
	    if ((OUTPUT = fopen(lm->outputfile, "wb")) == NULL) {
		if (SHOW_MSG) HTPrint("Can't open `%s'\\n",
				       lm->outputfile);
		OUTPUT = stdout;
	    }
	}
	HTRequest_setOutputStream(request,
				  HTFWriter_new(request, OUTPUT, YES));
#endif

	/*
	** To reformat HTML, just put it through a parser running
	** into a regenerator   tbl 940613
	*/
	if (lm->flags & LM_REFORMAT) {
	    HTStructured * html =
		HTMLGenerator(request, NULL, WWW_HTML,
			      HTRequest_outputFormat(request),
			      HTRequest_outputStream(request));
	    HTRequest_setOutputStream(request, SGML_new(HTML_dtd(), html));
	}
    }
    
     /* Log file specifed? */
    if (lm->logfile) {
	lm->log = HTLog_open(lm->logfile, YES, YES);
        if (lm->log) HTNet_addAfter(HTLogFilter, NULL, lm->log, HT_ALL, HT_FILTER_LATE);
    }

    /* Just convert formats */
    if (lm->flags & LM_FILTER) {
#ifdef STDIN_FILENO
	HTRequest_setAnchor(request, (HTAnchor *) lm->anchor);
	HTRequest_setPreemptive(request, YES);
	HTLoadSocket(STDIN_FILENO, request);
#endif
	Cleanup(lm, 0);
    }

    /* Set timeout on sockets */
    if (lm->timer < 0) {
	lm->timer = HTAlert_interactive() ?
	    DEFAULT_I_TIMEOUT : DEFAULT_NI_TIMEOUT;
    }
    /* Set the DNS cache timeout */
    HTDNS_setTimeout(3600);

    /* Rule file specified? */
    if (lm->rules) {
	char * rules = HTParse(lm->rules, lm->cwd, PARSE_ALL);
	if (!HTLoadRules(rules))
	    if (SHOW_MSG) HTPrint("Can't access rules\n");
	HT_FREE(rules);
    }

    /* Set up PICS machinary */
    CSApp_registerApp(PICSCallback, CSApp_callOnBad, PICS_userCallback, 
		      (void *)lm);
     /* Add our own filter to update the history list */
    HTNet_addBefore(MemoryCacheFilter, NULL, NULL, HT_FILTER_EARLY);
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Should we load a PICS user profile? */
    if (picsUser && !LoadPICSUser(lm, picsUser))
        HTPrint("Unable to load PICS user \"%s\".\n", picsUser);

    /* PICS user list specified? */
    if (lm->userList)
        LoadPICSUser(lm, lm->userList);

    /* request was registered in Thread_new before command line parsing
       loaded the PICS user, so re-register to give it the correct user */
    CSApp_unregisterReq(request);
    CSApp_registerReq(request, lm->pCSUser, PICSCallback, 
		      CSApp_callOnBad, (void *)lm);

    /* Start the request */
    if (keywords)
	status = HTSearchAnchor(keywords, (HTAnchor *) lm->anchor, request);
    else
	status = HTLoadAnchor((HTAnchor *) lm->anchor, request);

    if (keywords) HTChunk_delete(keywords);
    if (status != YES) {
	if (SHOW_MSG) HTPrint("Couldn't load home page\n");
	CSApp_unregisterDefaultUser();
	CSApp_unregisterApp();
	Cleanup(lm, -1);
    }

    /* Set up the rest if we are in interactive mode */
    if (HTAlert_interactive()) {

	/* Start History manager */
	lm->history = HTHistory_new();

	/*
	** Register STDIN as the user socket IF not STDIN is connected to
	** /dev/null or other non-terminal devices
	*/
	ConsoleEvent = HTEvent_new(scan_command, lm->console, HT_PRIORITY_MAX, lm->timer);
#ifdef STDIN_FILENO
	if (isatty(STDIN_FILENO)) {
	    HTEventList_register(STDIN_FILENO, HTEvent_READ, ConsoleEvent);
	}
#else
	HTEventList_register(GetStdHandle(STD_INPUT_HANDLE), HTEvent_READ, ConsoleEvent);
#endif
    }

    /* Go into the event loop... */
    HTEventList_loop(request);

    /* Only gets here if event loop fails */
    Cleanup(lm, 0);
    return 0;
}

