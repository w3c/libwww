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
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTError.h"					 /* Implemented here */
#include "HTAlert.h"					 /* Implemented here */

PUBLIC BOOL HTInteractive=YES;		    /* Any prompts from the Library? */

/* ------------------------------------------------------------------------- */

PUBLIC void HTProgress ARGS1(CONST char *, Msg)
{
    fprintf(TDEST, "   %s ...\n", Msg ? Msg : "UNKNWON");
}


PUBLIC void HTAlert ARGS1(CONST char *, Msg)
{
#ifdef NeXTStep
    NXRunAlertPanel(NULL, "%s", NULL, NULL, NULL, Msg);
#else
    fprintf(TDEST, "\nWARNING:  %s\n", Msg ? Msg : "UNKNOWN");
#endif
}

PUBLIC BOOL HTConfirm ARGS1(CONST char *, Msg)
{
  char Reply[4];	/* One more for terminating NULL -- AL */
  char *URep;
  
  fprintf(TDEST, "%s (y/n) ", Msg ? Msg : "UNKNOWN");
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
PUBLIC char * HTPrompt ARGS2(CONST char *, Msg, CONST char *, deflt)
{
    char buffer[200];
    char *reply = NULL;
    fprintf(TDEST, "%s ", Msg ? Msg : "UNKNOWN");
    if (deflt)
	fprintf(TDEST, "(RETURN for [%s]) ", deflt);

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
PUBLIC char * HTPromptPassword ARGS1(CONST char *, Msg)
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
PUBLIC void HTPromptUsernameAndPassword ARGS3(CONST char *,	Msg,
					      char **,		username,
					      char **,		password)
{
    fprintf(TDEST, "%s\n", Msg ? Msg : "UNKNOWN");
    *username = HTPrompt("Username:", *username);
    *password = HTPromptPassword("Password: ");
}


/*								HTErrorMsg
**
**	Default function that creates an error message using HTAlert() to
**	put out the contents of the error_stack messages. Furthermore, the
**	error_info structure contains a name of a help file that might be put
**	up as a link. This file can then be multi-linguistic.
**
**	This function might be overwritten by a smart server or client.
*/
PUBLIC void HTErrorMsg ARGS1(HTRequest *, request)
{
    HTList *cur = request->error_stack;
    BOOL highest = YES;
    HTChunk *chunk;
    HTErrorInfo *pres;
    if (!request) {
	if (TRACE) fprintf(TDEST, "HTErrorMsg.. Bad argument!\n");
	return;
    }

    /* This check is only necessary if the error message is put down the
       stream, because we have to know if a stream has been put up and/or
       taken down again. Here it is only put as an example */
#if 0
    if (request->error_block) {
	if (TRACE) fprintf(TDEST, "HTErrorMsg.. No messages are printed as no stream is available.\n");
	return;
    }
#endif

    /* Output messages */
    chunk = HTChunkCreate(128);
    while ((pres = (HTErrorInfo *) HTList_nextObject(cur))) {

	/* Check if we are going to show the message */
	if ((!pres->ignore || HTErrorShowMask & HT_ERR_SHOW_IGNORE) && 
	    (HTErrorShowMask & pres->severity)) {

	    /* Output code number */
	    if (highest) {			    /* If first time through */
		if (TRACE)
		    fprintf(TDEST,
			    "HTError..... Generating message.\n");
		
		/* Output title */
		if (pres->severity == ERR_WARN)
		    HTChunkPuts(chunk, "Warning ");
		else if (pres->severity == ERR_NON_FATAL)
		    HTChunkPuts(chunk, "Non Fatal Error ");
		else if (pres->severity == ERR_FATAL)
		    HTChunkPuts(chunk, "Fatal Error ");
		else if (pres->severity == ERR_INFO)
		    HTChunkPuts(chunk, "Information ");
		else {
		    if (TRACE)
			fprintf(TDEST, "HTError..... Unknown Classification of Error (%d)...\n", pres->severity);
		    HTChunkFree(chunk);
		    return;
		}

		/* Only output error code if it is a real HTTP code */
		if (pres->element < HTERR_HTTP_CODES_END) {
		    char codestr[10];
		    sprintf(codestr, "%d ", error_info[pres->element].code);
		    HTChunkPuts(chunk, codestr);
		}
		highest = NO;
	    } else
		HTChunkPuts(chunk, "\nReason: ");

	    /* Output error message */
	    if (pres->element != HTERR_SYSTEM) {
		HTChunkPuts(chunk, error_info[pres->element].msg);
		HTChunkPutc(chunk, ' ');
	    }

	    /* Output parameters */
	    if (pres->par && HTErrorShowMask & HT_ERR_SHOW_PARS) {
		unsigned int cnt;
		char ch;
		HTChunkPutc(chunk, '(');
		for (cnt=0; cnt<pres->par_length; cnt++) {
		    ch = *((char *)(pres->par)+cnt);
		    if (ch < 0x20 || ch >= 0x7F)
			HTChunkPutc(chunk, '#'); /* Can't print real content */
		    else
			HTChunkPutc(chunk, ch);
		}
		HTChunkPutc(chunk, ')');
	    }

	    /* Output location */
	    if (pres->where && HTErrorShowMask & HT_ERR_SHOW_LOCATION) {
		HTChunkPuts(chunk, "This occured in ");
		HTChunkPuts(chunk, pres->where);
		HTChunkPutc(chunk, '\n');
	    }

	    /* We don't want the message more than once */
	    HTErrorIgnore(request, pres->handle);
	    
	    /* If we only are going to show the higest entry */
	    if (HTErrorShowMask & HT_ERR_SHOW_FIRST)
		break;
	}
    }
    HTChunkPutc(chunk,  '\n');
    HTChunkTerminate(chunk);
    if (chunk->size > 2)
	HTAlert(chunk->data);
    HTChunkFree(chunk);
    return;
}
