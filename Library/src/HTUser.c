/*
**	USER PROFILE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Contains information about local host, email, etc.
**
**	May 96  HFN	Written
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTInet.h"
#include "HTUser.h"					 /* Implemented here */

/* The default directory for "save locally" and "save and execute" files: */
#ifndef HT_TMP_ROOT
#ifdef WWW_MSWINDOWS
#define HT_TMP_ROOT		"C:\\Temp\\"
#else
#define HT_TMP_ROOT		"/tmp/"
#endif /* WWW_MSWINDOWS */
#endif

struct _HTUserProfile {
    char *	user;
    char * 	fqdn;			      /* Fully qualified domain name */
    char * 	email;			    /* Email address of current user */
    char * 	news;				   /* The news server to use */
    char *	tmp;			     /* Location for temporary files */
    time_t	timezone;			     /* Time zone in seconds */
    void *	context;			     /* Application specific */
};

/* ------------------------------------------------------------------------- */

/*
**	Create a new host profile object and initialize it with what we can
**	find on this host.
*/
PUBLIC HTUserProfile * HTUserProfile_new (const char * name, void * context)
{
    HTUserProfile * me = NULL;
    if (name) {
	if ((me = (HTUserProfile *) HT_CALLOC(1, sizeof(HTUserProfile)))==NULL)
	    HT_OUTOFMEM("HTUserProfile_new");

	HTTRACE(CORE_TRACE, "User Profile Adding `%s\'\n" _ name);
	StrAllocCopy(me->user, name);

	/* Set the context */
	me->context = context;
    }
    return me;
}

/*
**	Localize a user profile by filling in all the information that we
**	can figure out automatically, for example the email address, news
**	server etc.
*/
PUBLIC BOOL HTUserProfile_localize (HTUserProfile * up)
{
    if (up) {
	HTTRACE(CORE_TRACE, "User Profile Localizing %p\n" _ up);

	/* Find the FQDN */
	up->fqdn = HTGetHostName();

	/* Find the user email address */
	up->email = HTGetMailAddress();

	/* Find the news server */
	up->news = HTGetNewsServer();

	/* Find the timezone offset */
	up->timezone = HTGetTimeZoneOffset();

	/* Find the default location for temporary files */
	StrAllocCopy(up->tmp, HT_TMP_ROOT);
	if (*(up->tmp+strlen(up->tmp)-1) != DIR_SEPARATOR_CHAR)
	    StrAllocCat(up->tmp, DIR_SEPARATOR_STR);

	return YES;
    }
    return NO;
}

/*
**	Delete a user profile
*/
PUBLIC BOOL HTUserProfile_delete (HTUserProfile * up)
{
    if (up) {
	HT_FREE(up->user);
	HT_FREE(up->fqdn);
	HT_FREE(up->email);
	HT_FREE(up->news);
	HT_FREE(up->tmp);
	HT_FREE(up);
	return YES;
    }
    return NO;
}

PUBLIC char * HTUserProfile_fqdn (HTUserProfile * up)
{
    return up ? up->fqdn : NULL;
}

PUBLIC BOOL HTUserProfile_setFqdn (HTUserProfile * up, const char * fqdn)
{
    if (up && fqdn) {
	StrAllocCopy(up->fqdn, fqdn);
	return YES;
    }
    return NO;
}

PUBLIC char * HTUserProfile_email (HTUserProfile * up)
{
    return up ? up->email : NULL;
}

PUBLIC BOOL HTUserProfile_setEmail (HTUserProfile * up, const char * email)
{
    if (up && email) {
	StrAllocCopy(up->email, email);
	return YES;
    }
    return NO;
}

PUBLIC char * HTUserProfile_news (HTUserProfile * up)
{
    return up ? up->news : NULL;
}

PUBLIC BOOL HTUserProfile_setNews (HTUserProfile * up, const char * news)
{
    if (up && news) {
	StrAllocCopy(up->news, news);
	return YES;
    }
    return NO;
}

PUBLIC char * HTUserProfile_tmp (HTUserProfile * up)
{
    return up ? up->tmp : NULL;
}

PUBLIC BOOL HTUserProfile_setTmp (HTUserProfile * up, const char * tmp)
{
    if (up && tmp) {
	StrAllocCopy(up->tmp, tmp);
	if (*(up->tmp+strlen(up->tmp)-1) != DIR_SEPARATOR_CHAR)
	    StrAllocCat(up->tmp, DIR_SEPARATOR_STR);
	return YES;
    }
    return NO;
}

PUBLIC time_t HTUserProfile_timezone (HTUserProfile * up)
{
    return up ? up->timezone : 0;
}

PUBLIC BOOL HTUserProfile_setTimezone (HTUserProfile * up, time_t timezone)
{
    if (up) {
	up->timezone = timezone;
	return YES;
    }
    return NO;
}

PUBLIC void * HTUserProfile_context (HTUserProfile * up)
{
    return up ? up->context : NULL;
}

PUBLIC BOOL HTUserProfile_setContext (HTUserProfile * up, void * context)
{
    if (up) {
	up->context = context;
	return YES;
    }
    return NO;
}

