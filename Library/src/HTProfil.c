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
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWCache.h"
#include "WWWStream.h"
#include "HTInit.h"
#include "HTProfil.h"				         /* Implemented here */

PRIVATE HTList * converters = NULL;
PRIVATE HTList * transfer_encodings = NULL;
PRIVATE HTList * content_encodings = NULL;
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

	/* The following lists have been cleaned up by HTFormat_deleteAll */
	transfer_encodings = 0;
	content_encodings = 0;
	converters = 0;

	/* Remove bindings between suffixes, media types */
	HTBind_deleteAll();

	/* Terminate libwww */
	HTLibTerminate();
    }
}

PRIVATE void client_profile (const char * AppName, const char * AppVersion,
			     BOOL preemptive, BOOL cache, BOOL HTMLParser)
{
    /* If the Library is not already initialized then do it */
    if (!HTLib_isInitialized()) HTLibInit(AppName, AppVersion);

    /* Register the default set of messages and dialog functions */
    HTAlertInit();
    HTAlert_setInteractive(YES);

    if (!converters) {
      converters = HTList_new();
      /* Register the default set of converters */
      HTConverterInit(converters);
      /* Register the default libwww HTML parser */
      if (HTMLParser) HTMLInit(converters);
      /* Set the converters as global converters for all requests */
      HTFormat_setConversion(converters);
    }

    if (!transfer_encodings) {
      transfer_encodings = HTList_new();
      /* Register the default set of transfer encoders and decoders */
      HTTransferEncoderInit(transfer_encodings);
      HTFormat_setTransferCoding(transfer_encodings);
    }

    if (!content_encodings) {
      content_encodings = HTList_new();
      /* Register the default set of content encoders and decoders */
      HTContentEncoderInit(content_encodings);
      if (HTList_count(content_encodings) > 0)
	HTFormat_setContentCoding(content_encodings);
      else {
	HTList_delete(content_encodings);
	content_encodings = NULL;
      }
    }

    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of application protocol modules */
    if (preemptive)
	HTProtocolPreemptiveInit();
    else
	HTProtocolInit();

    /* Initialize suffix bindings for local files */
    HTBind_init();

    /* Set max number of sockets we want open simultanously */ 
    HTNet_setMaxSocket(32);

    /* The persistent cache does not work in preemptive mode */
    if (cache) HTCacheInit(NULL, 20);

    /* Register the default set of BEFORE and AFTER filters */
    HTNetInit();

    /* Set up the default set of Authentication schemes */
    HTAAInit();

    /* Get any proxy or gateway environment variables */
    HTProxy_getEnvVar();

    /* Register the default set of MIME header parsers */
    HTMIMEInit();

    /* Register the default set of file suffix bindings */
    HTFileInit();

    /* Register the default set of Icons for directory listings */
    HTIconInit(NULL);
}

PUBLIC void HTProfile_newClient (const char * AppName, const char * AppVersion)
{
    /* Do the default setup */
    client_profile(AppName, AppVersion, NO, YES, NO);

    /* Set up default event loop */
    HTEventInit();
}

PUBLIC void HTProfile_newHTMLClient (const char * AppName, const char * AppVersion)
{
    /* Do the default setup */
    client_profile(AppName, AppVersion, NO, YES, YES);

    /* Set up default event loop */
    HTEventInit();
}

PUBLIC void HTProfile_newPreemptiveClient (const char * AppName,
					   const char * AppVersion)
{
    /* Do the default setup */
    client_profile(AppName, AppVersion, YES, NO, NO);

    /* Set up default event loop */
    HTEventInit();
    /* Remember that we are loading preemptively */
    preemptive = YES;
}

PUBLIC void HTProfile_newNoCacheClient (const char * AppName,
					const char * AppVersion)
{
    /* Do the default setup */
    client_profile(AppName, AppVersion, NO, NO, NO);

    /* Set up default event loop */
    HTEventInit();
}

PUBLIC void HTProfile_newHTMLNoCacheClient (const char * AppName,
					    const char * AppVersion)
{
    /* Do the default setup */
    client_profile(AppName, AppVersion, NO, NO, YES);

    /* Set up default event loop */
    HTEventInit();
}

PRIVATE void robot_profile (const char * AppName, const char * AppVersion)
{
    /* If the Library is not already initialized then do it */
    if (!HTLib_isInitialized()) HTLibInit(AppName, AppVersion);

    if (!converters) {
      converters = HTList_new();
      /* Register the default set of converters including the HTML parser */
      HTConverterInit(converters);
      HTMLInit(converters);
      /* Set the converters as global converters for all requests */
      HTFormat_setConversion(converters);
    }

    if (!transfer_encodings) {
      transfer_encodings = HTList_new();
      /* Register the default set of transfer encoders and decoders */
      HTTransferEncoderInit(transfer_encodings);
      HTFormat_setTransferCoding(transfer_encodings);
    }

    if (!content_encodings) {
      content_encodings = HTList_new();
      /* Register the default set of content encoders and decoders */
      HTContentEncoderInit(content_encodings);
      if (HTList_count(content_encodings) > 0)
	HTFormat_setContentCoding(content_encodings);
      else {
	HTList_delete(content_encodings);
	content_encodings = NULL;
      }
    }

    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of application protocol modules */
    HTProtocolInit();

    /* Initialize suffix bindings for local files */
    HTBind_init();

    /* Set max number of sockets we want open simultanously */ 
    HTNet_setMaxSocket(32);

    /* Register some default set of BEFORE and AFTER filters */
    HTNet_addBefore(HTRuleFilter, NULL, NULL, HT_FILTER_MIDDLE); 
    HTNet_addBefore(HTProxyFilter, NULL, NULL, HT_FILTER_MIDDLE); 
    HTNet_addAfter(HTInfoFilter, NULL, NULL, HT_ALL, HT_FILTER_LATE); 

    /* Get any proxy or gateway environment variables */
    HTProxy_getEnvVar();

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







