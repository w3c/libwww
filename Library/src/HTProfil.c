/*
**	APPLICATION PROFILES INITALIZATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Defines a set of application profiles
*/

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTProfil.h"				         /* Implemented here */

PRIVATE HTList * converters = NULL;
PRIVATE HTList * encodings = NULL;
PRIVATE BOOL preemptive = NO;

/* ------------------------------------------------------------------------- */

PUBLIC void HTProfile_delete (void)
{
    if (!preemptive) HTEventTerminate();
    if (HTLib_isInitialized()) {
	HTFormat_deleteAll();
	HTLibTerminate();
    }
}

PRIVATE void client_profile (const char * AppName, const char * AppVersion)
{
    /* If the Library is not already initialized then do it */
    if (!HTLib_isInitialized()) HTLibInit(AppName, AppVersion);

    if (!converters) converters = HTList_new();
    if (!encodings) encodings = HTList_new();

    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of BEFORE and AFTER filters */
    HTNetInit();

    /* Set up the default set of Authentication schemes */
    HTAAInit();

    /* Get any proxy or gateway environment variables */
    HTProxy_getEnvVar();

    /* Register the default set of converters */
    HTConverterInit(converters);
    HTFormat_setConversion(converters);

    /* Register the default set of transfer encoders and decoders */
    HTEncoderInit(encodings);
    HTFormat_setTransferCoding(encodings);

    /* Register the default set of MIME header parsers */
    HTMIMEInit();

    /* Register the default set of file suffix bindings */
    HTFileInit();

    /* Register the default set of Icons for directory listings */
    HTIconInit(NULL);

    /* Register the default set of messages and dialog functions */
    HTAlertInit();
    HTAlert_setInteractive(YES);
}

PUBLIC void HTProfile_newClient (const char * AppName, const char * AppVersion)
{
    /* Do the default setup */
    client_profile(AppName, AppVersion);

    /* Set up default event loop */
    HTEventInit();

    /* Register the default set of application protocol modules */
    HTProtocolInit();
}

PUBLIC void HTProfile_newPreemptiveClient (const char * AppName,
					   const char * AppVersion)
{
    /* Do the default setup */
    client_profile(AppName, AppVersion);

    /* Register the default set of application protocol modules */
    HTProtocolPreemptiveInit();

    /* Remember that we are loading preemptively */
    preemptive = YES;
}

PRIVATE void robot_profile (const char * AppName, const char * AppVersion)
{
    /* If the Library is not already initialized then do it */
    if (!HTLib_isInitialized()) HTLibInit(AppName, AppVersion);

    if (!converters) converters = HTList_new();
    if (!encodings) encodings = HTList_new();

    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of application protocol modules */
    HTProtocolInit();

    /* Set max number of sockets we want open simultanously */ 
    HTNet_setMaxSocket(6);

    /* Register some default set of BEFORE and AFTER filters */
    HTNetCall_addBefore(HTRuleFilter, NULL, 0); 
    HTNetCall_addBefore(HTProxyFilter, NULL, 0); 
    HTNetCall_addAfter(HTLogFilter, NULL, HT_ALL); 
    HTNetCall_addAfter(HTInfoFilter, NULL, HT_ALL); 

    /* Get any proxy or gateway environment variables */
    HTProxy_getEnvVar();

    /* Register the default set of converters including the HTML parser */
    HTConverterInit(converters);
    HTFormat_setConversion(converters);

    /* Register the default set of transfer encoders and decoders */
    HTEncoderInit(encodings);
    HTFormat_setTransferCoding(encodings);

    /* Register the default set of MIME header parsers */
    HTMIMEInit();

    /* Register the default set of file suffix bindings */
    HTFileInit();

    /* Register the default set of Icons for directory listings */
    HTIconInit(NULL);

    /* Register some default messages and dialog functions */
    if (HTAlert_interactive()) {
	HTAlert_add(HTError_print, HT_A_MESSAGE);
	HTAlert_add(HTConfirm, HT_A_CONFIRM);
	HTAlert_add(HTPrompt, HT_A_PROMPT);
	HTAlert_add(HTPromptPassword, HT_A_SECRET);
	HTAlert_add(HTPromptUsernameAndPassword, HT_A_USER_PW);
    }
    HTAlert_setInteractive(YES);
}

PUBLIC void HTProfile_newRobot (const char * AppName, const char * AppVersion)
{    
  /* set up default event loop */
    HTEventInit();
    robot_profile(AppName, AppVersion);

    /* Register the default set of application protocol modules */
    HTProtocolInit();
}

PUBLIC void HTProfile_newPreemptiveRobot (const char * AppName,
					  const char * AppVersion)
{
    robot_profile(AppName, AppVersion);

    /* Register the default set of application protocol modules */
    HTProtocolPreemptiveInit();

    /* Remember that we are loading preemptively */
    preemptive = YES;
}







