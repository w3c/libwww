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
#include "WWWCache.h"
#include "HTProfil.h"				         /* Implemented here */

PRIVATE HTList * converters = NULL;
PRIVATE HTList * encodings = NULL;
PRIVATE BOOL preemptive = NO;

/* ------------------------------------------------------------------------- */

PUBLIC void HTProfile_delete (void)
{
    if (!preemptive) HTEventTerminate();
    if (HTLib_isInitialized()) {

	/* Clean up the persistent cache (if any) */
	HTCacheTerminate();

	/* Clean up all the global preferences */
	HTFormat_deleteAll();

	/* Terminate libwww */
	HTLibTerminate();
    }
}

PRIVATE void client_profile (const char * AppName, const char * AppVersion,
			     BOOL preemptive)
{
    /* If the Library is not already initialized then do it */
    if (!HTLib_isInitialized()) HTLibInit(AppName, AppVersion);

    if (!converters) converters = HTList_new();
    if (!encodings) encodings = HTList_new();

    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of application protocol modules */
    if (preemptive)
	HTProtocolPreemptiveInit();
    else
	HTProtocolInit();

    /* Enable the persistent cache */
    /*    HTCacheInit(NULL, 20); disabled for testing */

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
    client_profile(AppName, AppVersion, NO);

    /* Set up default event loop */
    HTEventInit();
}

PUBLIC void HTProfile_newPreemptiveClient (const char * AppName,
					   const char * AppVersion)
{
    /* Do the default setup */
    client_profile(AppName, AppVersion, YES);

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
    HTNet_addBefore(HTRuleFilter, NULL, NULL, HT_FILTER_MIDDLE); 
    HTNet_addBefore(HTProxyFilter, NULL, NULL, HT_FILTER_MIDDLE); 
    HTNet_addAfter(HTLogFilter, NULL, NULL, HT_ALL, HT_FILTER_LATE); 
    HTNet_addAfter(HTInfoFilter, NULL, NULL, HT_ALL, HT_FILTER_LATE); 

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







