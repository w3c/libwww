/*	Displaying messages and getting input for LineMode Browser
**	==========================================================
**
**	REPLACE THIS MODULE with a GUI version in a GUI environment!
**
** History:
**	   Jun 92 Created May 1992 By C.T. Barker
**	   Feb 93 Simplified, portablised TBL
**
*/


#include "HTAlert.h"

#include "tcp.h"		/* for TOUPPER */
#include <ctype.h> 		/* for toupper - should be in tcp.h */

PUBLIC void HTAlert ARGS1(CONST char *, Msg)
{
#ifdef NeXTStep
    NXRunAlertPanel(NULL, "%s", NULL, NULL, NULL, Msg);
#else
    fprintf(stderr, "WWW Alert:  %s\n", Msg);
#endif
}


PUBLIC void HTProgress ARGS1(CONST char *, Msg)
{
    fprintf(stderr, "   %s ...\n", Msg);
}


PUBLIC BOOL HTConfirm ARGS1(CONST char *, Msg)
{
  char Reply[3];
  char *URep;
  
  fprintf(stderr, "WWW: %s (y/n) ", Msg);
  fprintf(stderr, "(y/n) ");

  scanf("%3s",Reply); /* get reply, max 3 characters */
  URep=Reply;
  while (*URep)
    *URep++=TOUPPER(*URep);
    
  if ((strcmp(Reply,"YES")==0) || (strcmp(Reply,"Y")==0))
    return(YES);
  else
    return(NO);
}

/*	Prompt for answer and get text back
*/
PUBLIC char * HTPrompt ARGS2(CONST char *, Msg, CONST char *, deflt)
{
    char Tmp[200];
    char * rep = 0;
    fprintf(stderr, "WWW: %s", Msg);
    if (deflt) fprintf(stderr, "\n (RETURN for %s)\n", deflt);
    
    fgets(Tmp, 199, stdin); 
   
    StrAllocCopy(rep, *Tmp ? Tmp : deflt);
    return rep;
}
