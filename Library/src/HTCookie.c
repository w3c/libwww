/*
**	SIMPLE HTTP COOKIE FILTER HANDLER
**
**	(c) COPYRIGHT MIT 1999.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Written based on Jim Raven's code sent to the <www-lib@w3.org> list
**
**      History:
**         JR:  Sent code to the list
**         HFN: Made a libwww module
**
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWHTTP.h"
#include "WWWMIME.h"
#include "HTCookie.h"					 /* Implemented here */

/* Interface to persistent cookie jar */
PRIVATE HTCookieSetCallback * 	SetCookie = NULL;
PRIVATE void * 	SetCookieContext = NULL;

PRIVATE HTCookieFindCallback *	FindCookie = NULL;
PRIVATE void * FindCookieContext = NULL;

/* Are cookies enabled */
PRIVATE BOOL baking_cookies = NO;

/* Our cookies */
struct _HTCookie {
    char *		name;
    char *		value;
    char *		domain;
    char *		path;
    time_t		expiration;
    BOOL		secure;
};

/* Hold all cookies found for a single request */
typedef struct _HTCookieHolder {
    HTRequest *		request;
    HTList *		cookies;
} HTCookieHolder;

/* List of current cookie holders */
PRIVATE HTList *	cookie_holder = NULL;

/* What should we do with cookies? */
PRIVATE HTCookieMode CookieMode = HT_COOKIE_PROMPT | HT_COOKIE_ACCEPT | HT_COOKIE_SEND;

/* ------------------------------------------------------------------------- */

PRIVATE HTCookie * HTCookie_new (void)
{
    HTCookie * me = NULL;
    if ((me = (HTCookie *) HT_CALLOC(1, sizeof(HTCookie))) == NULL)
        HT_OUTOFMEM("HTCookie_new");
    return me;
} 

PRIVATE BOOL HTCookie_delete (HTCookie * me)
{
    if (me) {
	HT_FREE(me->name);
	HT_FREE(me->value);
	HT_FREE(me->domain);
	HT_FREE(me->path);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTCookie_setName (HTCookie * me, const char * name)
{
    if (me && name) {
	me->name = StrAllocCopy(me->name, name);
	return YES;
    }
    return NO;
}

PUBLIC char * HTCookie_name (HTCookie * me)
{
    return me ? me->name : NULL;
}

PUBLIC BOOL HTCookie_setValue (HTCookie * me, const char * value)
{
    if (me && value) {
	me->value = StrAllocCopy(me->value, value);
	return YES;
    }
    return NO;
}

PUBLIC char * HTCookie_value (HTCookie * me)
{
    return me ? me->value : NULL;
}

PUBLIC BOOL HTCookie_setDomain (HTCookie * me, const char * domain)
{
    if (me && domain) {
	me->domain = StrAllocCopy(me->domain, domain);
	return YES;
    }
    return NO;
}

PUBLIC char * HTCookie_domain (HTCookie * me)
{
    return me ? me->domain : NULL;
}

PUBLIC BOOL HTCookie_setPath (HTCookie * me, const char * path)
{
    if (me && path) {
	me->path = StrAllocCopy(me->path, path);
	return YES;
    }
    return NO;
}

PUBLIC char * HTCookie_path (HTCookie * me)
{
    return me ? me->path : NULL;
}

PUBLIC time_t HTCookie_setExpiration (HTCookie * me, time_t expiration)
{
    if (me) {
	me->expiration = expiration;
	return YES;
    }
    return NO;
}

PUBLIC time_t HTCookie_expiration (HTCookie * me)
{
    return me ? me->expiration : -1;
}

PUBLIC time_t HTCookie_setSecure (HTCookie * me, BOOL secure)
{
    if (me) {
	me->secure = secure;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTCookie_isSecure (HTCookie * me)
{
    return me ? me->secure : NO;
}

/* ------------------------------------------------------------------------- */

PRIVATE BOOL HTCookieHolder_addCookie (HTRequest * request, HTCookie * cookie)
{
    if (request && cookie) {
	HTList * cur = cookie_holder;
	HTCookieHolder * pres = NULL;

	/* Make sure that we have a cookie holder list */
	if (!cookie_holder) cookie_holder = HTList_new();

	/* See if we already have a cookie holder for this request */
	while ((pres = (HTCookieHolder *) HTList_nextObject(cur))) {
	    if (pres->request == request) break;
	}

	/* If found then use existing cookie holder, otherwise create new one */
	if (!pres) {
	    if ((pres = (HTCookieHolder *) HT_CALLOC(1, sizeof(HTCookieHolder))) == NULL)
		HT_OUTOFMEM("HTCookieHolder_newCookie");
	    pres->request = request;
	    pres->cookies = HTList_new();

	    /* Add to cookie holder list */
	    HTList_addObject(cookie_holder, pres);
	}

	/* Now add the cookie */
	HTList_addObject(pres->cookies, cookie);

	return YES;
    }
    return NO;
}

PRIVATE HTCookieHolder * HTCookieHolder_find (HTRequest * request)
{
    if (request) {
	HTList * cur = cookie_holder;
	HTCookieHolder * pres = NULL;
	while ((pres = (HTCookieHolder *) HTList_nextObject(cur))) {
	    if (pres->request == request) return pres;
	}
    }
    return NULL;
}

PRIVATE BOOL HTCookieHolder_delete (HTCookieHolder * me)
{
    if (me) {
	if (me->cookies) {
	    HTList * cookies = me->cookies;
	    HTCookie * cookie;
	    while ((cookie = (HTCookie *) HTList_nextObject(cookies)))
		HTCookie_delete(cookie);
	    HTList_delete(me->cookies);
	}
	HTList_removeObject(cookie_holder, me);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PRIVATE BOOL HTCookieHolder_deleteAll (void)
{
    if (cookie_holder) {
	HTList * cur = cookie_holder;
	HTCookieHolder * pres = NULL;
	while ((pres = (HTCookieHolder *) HTList_nextObject(cur))) {
	    HTCookieHolder_delete(pres);
	}
	HTList_delete(cookie_holder);
	cookie_holder = NULL;
	return YES;
    }
    return NO;
}

/* ------------------------------------------------------------------------- */

/*
** Added By Jesse Morgan <jesse@jesterpm.net> on 2006-05-22
** Splits a KEY=VALUE pair into a KEY and VALUE
*/
PRIVATE int HTCookie_splitPair (char * pair, char ** key, char ** value)
{
	char * index = strchr(pair, '=');

	if (index == NULL) {
			return HT_ERROR;
	}
	
	*key 	= pair;
	*index	= '\0';
	*value	= ++index;

	return HT_OK;
}

/*
**  MIME header parser for the Set-Cookie header field. We parse the cookies
**  and create HTCookie objects and store them in the cookie holder so that
**  the cookie after filter can deal with them accordingly.
*/
PRIVATE int HTCookie_parseSetCookie (HTRequest * request, HTResponse * response,
				     char * token, char * value)

{
    char * cookie_name = NULL;
    char * cookie_value = NULL;
  
    if (HTCookie_splitPair(HTNextParam(&value), &cookie_name, &cookie_value) != HT_OK) {
       return HT_ERROR; /* Malformed Cookie */
    }

    if (cookie_name && *cookie_name && cookie_value) {
	HTCookie * cookie = HTCookie_new();
	char * param_pair;

	HTCookie_setName(cookie, cookie_name);
	HTCookie_setValue(cookie, cookie_value);
	
	/* Add the cookie to our holder */
	HTCookieHolder_addCookie(request, cookie);

	/* Parse cookie parameters */
	while ((param_pair = HTNextParam(&value))) {
	    char * tok = NULL;
	    char * val = NULL;

	    if (HTCookie_splitPair(param_pair, &tok, &val) != HT_OK) {
	      return HT_ERROR; /* Malformed Cookie */
	    }
		
	    if (tok) {
		if (!strcasecomp(tok, "expires") && val && *val) {
		    HTTRACE(STREAM_TRACE, "Cookie...... Expires `%s\'\n" _ val);
		    HTCookie_setExpiration(cookie, HTParseTime(val, NULL, YES));
		} else if (!strcasecomp(tok, "domain") && val && *val) {
		    HTTRACE(STREAM_TRACE, "Cookie...... Domain `%s\'\n" _ val);
		    HTCookie_setDomain(cookie, val);
		} else if (!strcasecomp(tok, "path") && val && *val) {
		    HTTRACE(STREAM_TRACE, "Cookie...... Path `%s\'\n" _ val);
		    HTCookie_setPath(cookie, val);
		} else if (!strcasecomp(tok, "secure")) {
		    HTTRACE(STREAM_TRACE, "Cookie...... Secure `%s\'\n" _ val);
		    HTCookie_setSecure(cookie, YES);
		} else
		    HTTRACE(STREAM_TRACE, "Cookie...... Unknown `%s\' with value `%s\'\n" _ 
			    tok _ val ? val : "<null>");
	    }
	}
    }
    return HT_OK;
}

/*
**  Check whether the application provides us with a cookie or more.
*/
PRIVATE int HTCookie_beforeFilter (HTRequest * request, void * param, int mode)
{
    if ((CookieMode & HT_COOKIE_SEND) && FindCookie) {
	HTAssocList * cookies = (*FindCookie)(request, FindCookieContext);
	if (cookies) {
	    HTChunk * cookie_header = HTChunk_new(64);
	    HTAssocList * cur = cookies;
	    HTAssoc * pres;
	    BOOL first=YES;
	    while ((pres = (HTAssoc *) HTAssocList_nextObject(cur))) {
		if (!first) HTChunk_putc(cookie_header, ';');
		HTChunk_puts(cookie_header, HTAssoc_name(pres));
		HTChunk_putc(cookie_header, '=');
		HTChunk_puts(cookie_header, HTAssoc_value(pres));
		first = NO;
	    }
	    HTRequest_addExtraHeader(request, "Cookie", HTChunk_data(cookie_header));
	    HTChunk_delete(cookie_header);

	    /* Also delete the association list */
	    HTAssocList_delete(cookies);
	}
    }
    return HT_OK;
}

/*
**  Check the response to see if we got a cookie or more.
**  If so then figure out what to do with it (prompt user, store, etc.)
*/
PRIVATE int HTCookie_afterFilter (HTRequest * request, HTResponse * response,
				  void * param, int status)
{
    if ((CookieMode & HT_COOKIE_ACCEPT) && SetCookie) {
	HTCookieHolder * holder = HTCookieHolder_find(request);
	if (holder) {
	    HTList * cookies = holder->cookies;
	    HTCookie * pres;
	    while ((pres = (HTCookie *) HTAssocList_nextObject(cookies))) {

		/* Should we check to see if hosts match? */
		if (CookieMode & (HT_COOKIE_SAME_HOST|HT_COOKIE_SAME_DOMAIN)) {
		    char * cookie_host = HTCookie_domain(pres);
		    if (cookie_host) {
			int res;
			char * addr = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
			char * host = HTParse(addr, "", PARSE_HOST);
			
			if (CookieMode & HT_COOKIE_SAME_DOMAIN)
			    res = tailcasecomp(cookie_host, host);
			else
			    res = strcasecomp(cookie_host, host);

			if (res != 0) {
			    HTTRACE(APP_TRACE, "Cookie...... Host `%s\' doesn't match what is sent in cookie `%s\'\n" _ host _ cookie_host);
			    HT_FREE(addr);
			    continue;
			}
			HT_FREE(addr);
		    }
		}

		/* Should we prompt the user? */
		if (CookieMode & HT_COOKIE_PROMPT) {
		    HTAlertCallback * prompt = HTAlert_find(HT_A_CONFIRM);
		    if (prompt) {
			if ((*prompt)(request, HT_A_CONFIRM, HT_MSG_ACCEPT_COOKIE,
				      NULL, NULL, NULL) != YES)
			    continue;
		    } else
			continue;
		}

		/* Call the application with our new cookie */
		(*SetCookie)(request, pres, SetCookieContext);
	    }
	    
	    /* Delete cookie holder */
	    HTCookieHolder_delete(holder);
	}
    }
    return HT_OK;
}

/* ------------------------------------------------------------------------- */

/*
**  Start and stop the cookie engine
*/
PUBLIC BOOL HTCookie_init (void)
{
    if (!baking_cookies) {

	/* Register the SetCookie header parser */
	HTHeader_addParser("Set-Cookie", NO, HTCookie_parseSetCookie);

	/* Register the cookie before and after filters */
	HTNet_addBefore(HTCookie_beforeFilter, "http://*", NULL, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTCookie_afterFilter, "http://*", NULL, HT_ALL, HT_FILTER_MIDDLE);

	baking_cookies = YES;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTCookie_terminate (void)
{
    /* Unregister Set-Cookie header parser */
    HTHeader_deleteParser("Set-Cookie");

    /* Unregister the cookie before and after filters */
    HTNet_deleteBefore(HTCookie_beforeFilter);
    HTNet_deleteAfter(HTCookie_afterFilter);

    /* Delete all pending cookies */
    HTCookieHolder_deleteAll();

    baking_cookies = NO;
    return YES;
}

PUBLIC BOOL HTCookie_setCookieMode (HTCookieMode mode)
{
    CookieMode = mode;
    return YES;
}

PUBLIC HTCookieMode HTCookie_cookieMode (void)
{
    return CookieMode;
}

/*
**  Callbacks can be used by the application to set and retrieve cookies
**  from a persistent cookie jar as libwww doesn't provide a persistent
**  storage for this kind of thing and they probably should be secured
**  anyway.
*/
PUBLIC BOOL HTCookie_setCallbacks (HTCookieSetCallback *	setCookie,
				   void * 			setCookieContext,
				   HTCookieFindCallback *	findCookie,
				   void * 			findCookieContext)
{
    if (setCookie && findCookie) {
	HTTRACE(APP_TRACE, "Cookie...... Registering cookie callbacks\n");
	SetCookie = setCookie;
	SetCookieContext = setCookieContext;

	FindCookie = findCookie;
	FindCookieContext = findCookieContext;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTCookie_deleteCallbacks (void)
{
    HTTRACE(APP_TRACE, "Cookie...... Unregistering cookie callbacks\n");
    SetCookie = NULL;
    SetCookieContext = NULL;
    FindCookie = NULL;
    FindCookieContext = NULL;
    return YES;
}
