/*								      HTAlert.c
**	DISPLAYING MESSAGES AND GETTING INPUT FOR LINEMODE BROWSER
**
**	(c) COPYRIGHT CERN 1994.
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
#include "HTAlert.h"

PUBLIC BOOL HTInteractive=YES;		    /* Any prompts from the Library? */

PUBLIC void HTAlert ARGS1(CONST char *, Msg)
{
#ifdef NeXTStep
    NXRunAlertPanel(NULL, "%s", NULL, NULL, NULL, Msg);
#else
    fprintf(TDEST, "\nWWW Alert:  %s\n", Msg);
#endif
}


PUBLIC void HTProgress ARGS1(CONST char *, Msg)
{
    fprintf(TDEST, "   %s ...\n", Msg);
}


PUBLIC BOOL HTConfirm ARGS1(CONST char *, Msg)
{
  char Reply[4];	/* One more for terminating NULL -- AL */
  char *URep;
  
  fprintf(TDEST, "WWW: %s (y/n) ", Msg);	   /* (y/n) came twice -- AL */
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
    fprintf(TDEST, "WWW: %s", Msg);
    if (deflt)
	fprintf(TDEST, " (RETURN for [%s]) ", deflt);

    if (HTInteractive) {
#ifndef NO_STDIO
	if (!fgets(buffer, 200, stdin))
	    return NULL;	       	     /* NULL string on error, Henrik */
	buffer[strlen(buffer)-1] = '\0';	        /* Overwrite newline */
	StrAllocCopy(reply, *buffer ? buffer : deflt);
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
    if (Msg && *Msg)
	fprintf(TDEST, "WWW: %s\n", Msg);
    *username = HTPrompt("Username: ", *username);
    *password = HTPromptPassword("Password: ");
}

