/*
**	GENEREAL LIBRARY INFORMATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** Authors
**	HFN	Henrik Frystyk Nielsen, frystyk@w3.org
*/

#if !defined(HT_DIRECT_WAIS) && !defined(HT_DEFAULT_WAIS_GATEWAY)
#define HT_DEFAULT_WAIS_GATEWAY "http://www.w3.org:8001/"
#endif

/* Library include files */
#include "WWWUtil.h"
#include "HTAnchor.h"
#include "HTProt.h"
#include "HTDNS.h"
#include "HTUTree.h"
#include "HTLib.h"					 /* Implemented here */

#ifndef W3C_VERSION
#define W3C_VERSION	"unknown"
#endif

#ifndef HT_DEFAULT_USER
#define HT_DEFAULT_USER		"LIBWWW_GENERIC_USER"
#endif

PRIVATE char * HTAppName = NULL;	  /* Application name: please supply */
PRIVATE char * HTAppVersion = NULL;    /* Application version: please supply */

PRIVATE char * HTLibName = "libwww";
PRIVATE char * HTLibVersion = W3C_VERSION;

PRIVATE BOOL   HTSecure = NO;		 /* Can we access local file system? */

PRIVATE BOOL   initialized = NO;

PRIVATE HTUserProfile * UserProfile = NULL;	     /* Default user profile */

/* --------------------------------------------------------------------------*/

/*	Information about the Application
**	---------------------------------
*/
PUBLIC const char * HTLib_appName (void)
{
    return HTAppName ? HTAppName : "UNKNOWN";
}

PUBLIC BOOL HTLib_setAppName (const char * name)
{
    if (name) {
	char * ptr;
	StrAllocCopy(HTAppName, name);
	ptr = HTAppName;
	while (*ptr) {
	    if (isspace((int) *ptr)) *ptr = '_';
	    ptr++;
	}
	return YES;
    }
    return NO;
}

PUBLIC const char * HTLib_appVersion (void)
{
    return HTAppVersion ? HTAppVersion : "0.0";
}

PUBLIC BOOL HTLib_setAppVersion (const char * version)
{
    if (version) {
	char * ptr;
	StrAllocCopy(HTAppVersion, version);
	ptr = HTAppVersion;
	while (*ptr) {
	    if (isspace((int) *ptr)) *ptr = '_';
	    ptr++;
	}
	return YES;
    }
    return NO;
}

/*	Information about libwww
**	------------------------
*/
PUBLIC const char * HTLib_name (void)
{
    return HTLibName ? HTLibName : "UNKNOWN";
}

PUBLIC const char * HTLib_version (void)
{
    return HTLibVersion ? HTLibVersion : "0.0";
}

/*	Default User Profile
**	--------------------
*/
PUBLIC HTUserProfile * HTLib_userProfile (void)
{
    return UserProfile;
}

PUBLIC BOOL HTLib_setUserProfile (HTUserProfile * up)
{
    if (up) {
	UserProfile = up;
	return YES;
    }
    return NO;
}

/*	Access Local File System
**	------------------------
**	In this mode we do not tough the local file system at all
*/
PUBLIC BOOL HTLib_secure (void)
{
    return HTSecure;
}

PUBLIC void HTLib_setSecure (BOOL mode)
{
    HTSecure = mode;
}

/*	Have application initalized
**	---------------------------
*/
PUBLIC BOOL HTLib_isInitialized (void)
{
    return initialized;
}

/*								     HTLibInit
**
**	This function initiates the Library and it MUST be called when
**	starting up an application. See also HTLibTerminate()
*/
PUBLIC BOOL HTLibInit (const char * AppName, const char * AppVersion)
{
    HTTRACE(CORE_TRACE, "WWWLibInit.. INITIALIZING LIBRARY OF COMMON CODE\n");

    /* Set the application name and version */
    HTLib_setAppName(AppName);
    HTLib_setAppVersion(AppVersion);

    /* Initialize the timezone */
#ifdef HAVE_TZSET
    tzset();
#endif

    /* Create a default user profile and initialize it */
    UserProfile = HTUserProfile_new(HT_DEFAULT_USER, NULL);
    HTUserProfile_localize(UserProfile);

#ifdef WWWLIB_SIG
    /* On Solaris (and others?) we get a BROKEN PIPE signal when connecting
    ** to a port where we should get `connection refused'. We ignore this 
    ** using the following function call
    */
    HTSetSignal();				   /* Set signals in library */
#endif

    initialized = YES;
    return YES;
}


/*	HTLibTerminate
**	--------------
**	This function HT_FREEs memory kept by the Library and should be called
**	before exit of an application (if you are on a PC platform)
*/
PUBLIC BOOL HTLibTerminate (void)
{
    HTTRACE(CORE_TRACE, "WWWLibTerm.. Cleaning up LIBRARY OF COMMON CODE\n");

    HTNet_killAll();
    HTHost_deleteAll();		/* Delete remaining hosts */
    HTChannel_deleteAll();			/* Delete remaining channels */

    HT_FREE(HTAppName);	        /* Freed thanks to Wade Ogden <wade@ebt.com> */
    HT_FREE(HTAppVersion);

    HTAtom_deleteAll();					 /* Remove the atoms */
    HTDNS_deleteAll();				/* Remove the DNS host cache */
    HTAnchor_deleteAll(NULL);		/* Delete anchors and drop hyperdocs */

    HTProtocol_deleteAll();  /* Remove bindings between access and protocols */

    HTUserProfile_delete(UserProfile);	    /* Free our default User profile */

    HTUTree_deleteAll();			     /* Delete all URL Trees */

    initialized = NO;
    return YES;
}

