/*								      HTAlert.c
**	DISPLAYING MESSAGES AND GETTING INPUT FOR LINEMODE BROWSER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	REPLACE THIS MODULE with a GUI version in a GUI environment!
**
** History:
**	   Jun 92 Created May 1992 By C.T. Barker
**	   Feb 93 Simplified, portablised TBL
**	   Sep 93 Corrected 3 bugs in HTConfirm() :-( AL
*/

/* Library include files */
#include "WWWLib.h"
#include "HTError.h"					 /* Implemented here */
#include "HTAlert.h"					 /* Implemented here */

typedef struct _HTProgMsg {
    HTProgressState	state;
    char *		msg;
} HTProgMsg;

PRIVATE BOOL HTInteractive=YES;		    /* Any prompts from the Library? */

/* ------------------------------------------------------------------------- */

PUBLIC void HTPrompt_setInteractive (BOOL interactive)
{
    HTInteractive = interactive;
}

PUBLIC BOOL HTPrompt_interactive (void)
{
    return HTInteractive;
}

PUBLIC void HTProgress (HTRequest * request, HTProgressState state,
			void * param)
{
    /* This is just to avoid that we get a lot of progress messages in LMB */
    if (!(WWWTRACE)) return;

    if (!request) {
	if (WWWTRACE)
	    TTYPrint(TDEST, "HTProgress.. Bad argument\n");
	return;
    }
    switch (state) {
      case HT_PROG_DNS:
	TTYPrint(TDEST, "Looking up %s\n", (char *) param);
	break;

      case HT_PROG_CONNECT:
	TTYPrint(TDEST, "Contacting host...\n");
	break;

      case HT_PROG_ACCEPT:
	TTYPrint(TDEST, "Waiting for connection...\n");
	break;

      case HT_PROG_READ:
	{
	    long cl = HTAnchor_length(HTRequest_anchor(request));
	    if (cl > 0) {
		long b_read = HTRequest_bytesRead(request);
		double pro = (double) b_read/cl*100;
		char buf[10];
		HTNumToStr((unsigned long) cl, buf, 10);
		TTYPrint(TDEST, "Read (%d%% of %s)\n", (int) pro, buf);
	    } else
		TTYPrint(TDEST, "Reading...\n");
	}
	break;

      case HT_PROG_WRITE:
	TTYPrint(TDEST, "Writing...\n");
	break;

      case HT_PROG_DONE:
	TTYPrint(TDEST, "Finished\n");
	break;

      case HT_PROG_WAIT:
	TTYPrint(TDEST, "Waiting for free socket...\n");
	break;

      default:
	TTYPrint(TDEST, "UNKNOWN PROGRESS STATE\n");
	break;
    }
}


PUBLIC void HTAlert (HTRequest * request, CONST char * Msg)
{
#ifdef NeXTStep
    NXRunAlertPanel(NULL, "%s", NULL, NULL, NULL, Msg);
#else
    TTYPrint(TDEST, "\nWARNING:  %s\n", Msg ? Msg : "UNKNOWN");
#endif
}

PUBLIC BOOL HTConfirm (HTRequest * request, CONST char * Msg)
{
  char Reply[4];	/* One more for terminating NULL -- AL */
  char *URep;
  
  TTYPrint(TDEST, "%s (y/n) ", Msg ? Msg : "UNKNOWN");
#ifndef NO_STDIO
  if (!HTInteractive || !fgets(Reply, 4, stdin))   /* get reply, max 3 chars */
#endif
      return NO;
  URep=Reply;
  while (*URep) {
    if (*URep == '\n') {
	*URep = (char)0;	/* Overwrite newline */
	break;
    }
    *URep=TOUPPER(*URep);
    URep++;	/* This was previously embedded in the TOUPPER */
                /* call an it became evaluated twice because   */
                /* TOUPPER is a macro -- AL */
  }

  if ((strcmp(Reply,"YES")==0) || (strcmp(Reply,"Y")==0))
    return(YES);
  else
    return(NO);
}

/*	Prompt for answer and get text back. Reply text is either NULL on
**	error or a dynamic string which the caller must free.
*/
PUBLIC char * HTPrompt (HTRequest * request, CONST char * Msg,
			CONST char * deflt)
{
    char buffer[200];
    char *reply = NULL;
    TTYPrint(TDEST, "%s ", Msg ? Msg : "UNKNOWN");
    if (deflt)
	TTYPrint(TDEST, "(RETURN for [%s]) ", deflt);

    if (HTInteractive) {
#ifndef NO_STDIO
	if (!fgets(buffer, 200, stdin))
	    return NULL;	       	     /* NULL string on error, Henrik */
	buffer[strlen(buffer)-1] = '\0';	        /* Overwrite newline */
	if (*buffer)
	    StrAllocCopy(reply, buffer);
	else if (deflt)
	    StrAllocCopy(reply, deflt);
#endif
    }
    return reply;
}


/*	Prompt for password without echoing the reply. Reply text is
**	either NULL on error or a dynamic string which the caller must free.
*/
PUBLIC char * HTPromptPassword (HTRequest * request, CONST char * Msg)
{
    char *reply = NULL;
    if (HTInteractive) {
#ifndef NO_PASSWD
	char *pw = (char *) getpass(Msg ? Msg : "Password: ");
	if (pw)
	    StrAllocCopy(reply, pw);
#endif
    }
    return reply;
}


/*	Prompt both username and password	HTPromptUsernameAndPassword()
**	---------------------------------
** On entry,
**	Msg		is the prompting message.
**	*username and
**	*password	are char pointers; they are changed
**			to point to result strings.
**
**			If *username is not NULL, it is taken
**			to point to  a default value.
**			Initial value of *password is
**			completely discarded.
**
** On exit,
**	*username and *password point to newly allocated
**	strings -- original strings pointed to by them
**	are NOT freed.
**	
*/
PUBLIC void HTPromptUsernameAndPassword (HTRequest *	request,
					 CONST char *	Msg,
					 char **	username,
					 char **	password)
{
    TTYPrint(TDEST, "%s\n", Msg ? Msg : "UNKNOWN");
    *username = HTPrompt(request, "Username:", *username);
    *password = HTPromptPassword(request, "Password: ");
}


/*								HTError_print
**
**	Default function that creates an error message using HTAlert() to
**	put out the contents of the error_stack messages. Furthermore, the
**	error_info structure contains a name of a help file that might be put
**	up as a link. This file can then be multi-linguistic.
*/
PUBLIC void HTError_print (HTRequest * request, HTList * list)
{
    HTList *cur = list;
    HTError *pres;
    HTErrorShow showmask = HTError_show();
    HTChunk *msg = NULL;
    int code;
    if (WWWTRACE) TTYPrint(TDEST, "HTError..... Generating message\n");
    if (!request || !list) return;
    while ((pres = (HTError *) HTList_nextObject(cur))) {
	if (HTError_doShow(pres)) {
	    if (!msg) {
		HTSeverity severity = HTError_severity(pres);
		msg = HTChunkCreate(128);
		if (severity == ERR_WARN)
		    HTChunkPuts(msg, "Warning: ");
		else if (severity == ERR_NON_FATAL)
		    HTChunkPuts(msg, "Non Fatal Error: ");
		else if (severity == ERR_FATAL)
		    HTChunkPuts(msg, "Fatal Error: ");
		else if (severity == ERR_INFO)
		    HTChunkPuts(msg, "Information: ");
		else {
		    if (WWWTRACE)
			TTYPrint(TDEST, "HTError..... Unknown Classification of Error (%d)...\n", severity);
		    HTChunkFree(msg);
		    return;
		}
		if ((code = HTError_code(pres)) > 0) {	     /* Error number */
		    char buf[10];
		    sprintf(buf, "%d ", code);
		    HTChunkPuts(msg, buf);
		}
	    } else
		HTChunkPuts(msg, "\nReason: ");
	    HTChunkPuts(msg, HTError_message(pres));	    /* Error message */

	    if (showmask & HT_ERR_SHOW_PARS) {		 /* Error parameters */
		int length;
		int cnt;		
		char *pars = (char *) HTError_parameter(pres, &length);
		if (length && pars) {
		    HTChunkPuts(msg, " (");
		    for (cnt=0; cnt<length; cnt++) {
			char ch = *(pars+cnt);
			if (ch < 0x20 || ch >= 0x7F)
			    HTChunkPutc(msg, '#');
			else
			    HTChunkPutc(msg, ch);
		    }
		    HTChunkPuts(msg, ") ");
		}
	    }

	    if (showmask & HT_ERR_SHOW_LOCATION) {	   /* Error Location */
		HTChunkPuts(msg, "This occured in ");
		HTChunkPuts(msg, HTError_location(pres));
		HTChunkPutc(msg, '\n');
	    }

	    /*
	    ** Make sure that we don't get this error more than once even
	    ** if we are keeping the error stack from one request to another
	    */
	    HTError_setIgnore(pres);
	    
	    /* If we only are show the most recent entry then break here */
	    if (showmask & HT_ERR_SHOW_FIRST)
		break;
	}
    }

    if (msg) {
	HTChunkPutc(msg, '\n');
	HTAlert(request, HTChunkData(msg));
	HTChunkFree(msg);
    }
    return;
}
