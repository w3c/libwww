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
#include "sysdep.h"
#include "WWWUtil.h"
#include "HTInet.h"
#include "HTUser.h"					 /* Implemented here */

/* The default directory for "save locally" and "save and execute" files: */
#ifndef HT_TMP_ROOT
#define HT_TMP_ROOT		"/tmp"
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

	if (CORE_TRACE) HTTrace("User Profile Adding `%s\'\n", name);
	StrAllocCopy(me->user, name);

	/* Find the FQDN */
	me->fqdn = HTGetHostName();

	/* Find the user email address */
	me->email = HTGetMailAddress();

	/* Find the news server */
	me->news = HTGetNewsServer();

	/* Find the timezone offset */
	me->timezone = HTGetTimeZoneOffset();

	/* Find the default location for temporary files */
	StrAllocCopy(me->tmp, HT_TMP_ROOT);

	/* Set the context */
	me->context = context;
    }
    return me;
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

