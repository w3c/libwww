/*								     HTProxy.c
**	GATEWAY AND PROXY MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Replaces the old env variables for gateways and proxies. However for
**	backward compatibility there is a function that reads the env variables
**	at start up. Note that there is a difference between a proxy and a
**	gateway!
**
** Authors
**	HF	Henrik Frystyk, frystyk@w3.org
** History
**       4 Jun 95 Written on a rainy day
*/

#if !defined(HT_DIRECT_WAIS) && !defined(HT_DEFAULT_WAIS_GATEWAY)
#define HT_DEFAULT_WAIS_GATEWAY "http://www.w3.org:8001/"
#endif

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWHTTP.h"
#include "WWWApp.h"
#include "HTProxy.h"					 /* Implemented here */

/* Variables and typedefs local to this module */

typedef struct _HTProxy {
    char *	access;
    char *	url;			          /* URL of Gateway or Proxy */
#ifdef HT_POSIX_REGEX
    regex_t *	regex;				  /* Compiled regex */
#endif
} HTProxy;

typedef struct _HTHostlist {
    char *	access;
    char *	host;				  /* Host or domain name */
    unsigned	port;
#ifdef HT_POSIX_REGEX
    regex_t *	regex;				  /* Compiled regex */
#endif
} HTHostList;

PRIVATE HTList * proxies = NULL;		    /* List of proxy servers */
PRIVATE HTList * gateways = NULL;			 /* List of gateways */
PRIVATE HTList * noproxy = NULL;   /* Don't proxy on these hosts and domains */
PRIVATE int      noproxy_is_onlyproxy = 0; /* Interpret the noproxy list as an onlyproxy one */

#if 0
PRIVATE HTList * onlyproxy = NULL;  /* Proxy only on these hosts and domains */
#endif

/* ------------------------------------------------------------------------- */

#ifdef HT_POSIX_REGEX
PRIVATE char * get_regex_error (int errcode, regex_t * compiled)
{
    size_t length = regerror (errcode, compiled, NULL, 0);
    char * str = NULL;
    if ((str = (char *) HT_MALLOC(length+1)) == NULL)
	HT_OUTOFMEM("get_regex_error");
    (void) regerror (errcode, compiled, str, length);
    return str;
}

PRIVATE regex_t * get_regex_t (const char * regex_str, int cflags)
{
    regex_t * regex = NULL;
    if (regex_str && *regex_str) {
	int status;
	if ((regex = (regex_t *) HT_CALLOC(1, sizeof(regex_t))) == NULL)
	    HT_OUTOFMEM("get_regex_t");
	if ((status = regcomp(regex, regex_str, cflags))) {
	    char * err_msg = get_regex_error(status, regex);
	    HTTRACE(PROT_TRACE, "HTProxy..... Regular expression error: %s\n" _ err_msg);
	    HT_FREE(err_msg);
	    HT_FREE(regex);
	}
    }
    return regex;
}
#endif

/*
**	Existing entries are replaced with new ones
*/
PRIVATE BOOL add_object (HTList * list, const char * access, const char * url,
			 BOOL regex, int regex_flags)
{
    HTProxy *me;
    if (!list || !access || !url || !*url)
	return NO;
    if ((me = (HTProxy *) HT_CALLOC(1, sizeof(HTProxy))) == NULL)
	HT_OUTOFMEM("add_object");
    StrAllocCopy(me->access, access);		     	    /* Access method */

#ifdef HT_POSIX_REGEX
    /* 
    **  If we support regular expressions then compile one up for
    **  this regular expression. Otherwise use is as a normal
    **  access scheme.
    */
    if (regex) {
	me->regex = get_regex_t(access,
				regex_flags < 0 ?
				W3C_DEFAULT_REGEX_FLAGS : regex_flags);
    } else
#endif
    {
	char *ptr = me->access;
	while ((*ptr = TOLOWER(*ptr))) ptr++;
    }

    me->url = HTParse(url, "", PARSE_ACCESS+PARSE_HOST+PARSE_PUNCTUATION);
    if (*(me->url+strlen(me->url)-1) != '/')
	StrAllocCat(me->url, "/");
    me->url = HTSimplify(&me->url);

    /* See if we already have this one */
    {
	HTList *cur = list;
	HTProxy *pres;
	while ((pres = (HTProxy *) HTList_nextObject(cur)) != NULL) {
	    if (!strcmp(pres->access, me->access))
		break;				       /* We already have it */
	}
	if (pres) {
	    HTTRACE(PROT_TRACE, "HTProxy..... replacing for `%s\' access %s\n" _ 
			me->url _ me->access);
	    HT_FREE(pres->access);
	    HT_FREE(pres->url);
#ifdef HT_POSIX_REGEX
	    if (pres->regex) regfree(pres->regex);
#endif
	    HTList_removeObject(list, (void *) pres);
	    HT_FREE(pres);
	}
	HTTRACE(PROT_TRACE, "HTProxy..... adding for `%s\' access %s\n" _ 
		    me->url _ me->access);
	HTList_addObject(list, (void *) me);
    }
    return YES;
}

PRIVATE BOOL remove_allObjects (HTList * list)
{
    if (list) {
	HTList *cur = list;
	HTProxy *pres;
	while ((pres = (HTProxy *) HTList_nextObject(cur)) != NULL) {
	    HT_FREE(pres->access);
	    HT_FREE(pres->url);
#ifdef HT_POSIX_REGEX
	    if (pres->regex) regfree(pres->regex);
#endif
	    HT_FREE(pres);
	}
	return YES;
    }
    return NO;
}

/*	Add an entry to a list of host names
**	------------------------------------
**	Existing entries are replaced with new ones
*/
PRIVATE BOOL add_hostname (HTList * list, const char * host,
			   const char * access, unsigned port,
			   BOOL regex, int regex_flags)
{
    HTHostList *me;
    if (!list || !host || !*host)
	return NO;
    if ((me = (HTHostList *) HT_CALLOC(1, sizeof(HTHostList))) == NULL)
        HT_OUTOFMEM("add_hostname");
#ifdef HT_POSIX_REGEX
    if (regex)
	me->regex = get_regex_t(host,
				regex_flags < 0 ?
				W3C_DEFAULT_REGEX_FLAGS : regex_flags);
#endif

    if (access) {
	char *ptr;
	StrAllocCopy(me->access, access);      	     	    /* Access method */
	ptr = me->access;
	while ((*ptr = TOLOWER(*ptr))) ptr++;
    }
    StrAllocCopy(me->host, host);		     	    	/* Host name */
    {
	char *ptr = me->host;
	while ((*ptr = TOLOWER(*ptr))) ptr++;
    }
    me->port = port;					      /* Port number */
    HTTRACE(PROT_TRACE, "HTHostList.. adding `%s\' to list\n" _ me->host);
    HTList_addObject(list, (void *) me);
    return YES;
}

PRIVATE BOOL remove_AllHostnames (HTList * list)
{
    if (list) {
	HTList *cur = list;
	HTHostList *pres;
	while ((pres = (HTHostList *) HTList_nextObject(cur)) != NULL) {
	    HT_FREE(pres->access);
	    HT_FREE(pres->host);
#ifdef HT_POSIX_REGEX
	    if (pres->regex) regfree(pres->regex);
#endif
	    HT_FREE(pres);
	}
	return YES;
    }
    return NO;
}

/*	HTProxy_add
**	-----------
**	Registers a proxy as the server to contact for a specific
**	access method. `proxy' should be a fully valid name, like
**	"http://proxy.w3.org:8001" but domain name is not required.
**	If an entry exists for this access then delete it and use the 
**	ne one. Returns YES if OK, else NO
*/
PUBLIC BOOL HTProxy_add (const char * access, const char * proxy)
{
    /*
    **  If this is the first time here then also add a before filter to handle
    **  proxy authentication and the normal AA after filter as well.
    **  These filters will be removed if we remove all proxies again.
    */
    if (!proxies) {
	proxies = HTList_new();
	HTNet_addBefore(HTAA_proxyBeforeFilter, NULL, NULL,
			HT_FILTER_MIDDLE);
	HTNet_addAfter(HTAuthFilter, NULL, NULL,
		       HT_NO_PROXY_ACCESS, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTAuthFilter, NULL, NULL,
		       HT_PROXY_REAUTH, HT_FILTER_MIDDLE);
    }
    return add_object(proxies, access, proxy, NO, -1);
}

/*	HTProxy_addRegex
**	----------------
**	Registers a proxy as the server to contact for any URL matching the
**	regular expression. `proxy' should be a fully valid name, like
**	"http://proxy.w3.org:8001".
**	If an entry exists for this access then delete it and use the 
**	new one. Returns YES if OK, else NO
*/
PUBLIC BOOL HTProxy_addRegex (const char * regex,
			      const char * proxy,
			      int regex_flags)
{
    /*
    **  If this is the first time here then also add a before filter to handle
    **  proxy authentication and the normal AA after filter as well.
    **  These filters will be removed if we remove all proxies again.
    */
    if (!proxies) {
	proxies = HTList_new();
	HTNet_addBefore(HTAA_proxyBeforeFilter, NULL, NULL,
			HT_FILTER_MIDDLE);
	HTNet_addAfter(HTAuthFilter, NULL, NULL,
		       HT_NO_PROXY_ACCESS, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTAuthFilter, NULL, NULL,
		       HT_PROXY_REAUTH, HT_FILTER_MIDDLE);
    }
#ifdef HT_POSIX_REGEX
    return add_object(proxies, regex, proxy, YES, regex_flags);
#else
    return add_object(proxies, regex, proxy, NO, -1);
#endif
}

/*
**	Removes all registered proxies
*/
PUBLIC BOOL HTProxy_deleteAll (void)
{
    if (remove_allObjects(proxies)) {
	HTList_delete(proxies);

	/*
	** If we have no more proxies then there is no reason for checking
	** proxy authentication. We therefore unregister the filters for
	** handling proxy authentication
	*/
	HTNet_deleteBefore(HTAA_proxyBeforeFilter);
        HTNet_deleteAfterStatus(HT_NO_PROXY_ACCESS);
        HTNet_deleteAfterStatus(HT_PROXY_REAUTH);

	proxies = NULL;
	return YES;
    }
    return NO;
}

/*	HTGateway_add
**	-------------
**	Registers a gateway as the server to contact for a specific
**	access method. `gateway' should be a fully valid name, like
**	"http://gateway.w3.org:8001" but domain name is not required.
**	If an entry exists for this access then delete it and use the 
**	ne one. Returns YES if OK, else NO
*/
PUBLIC BOOL HTGateway_add (const char * access, const char * gate)
{
    if (!gateways)
	gateways = HTList_new();
    return add_object(gateways, access, gate, NO, -1);
}

/*
**	Removes all registered gateways
*/
PUBLIC BOOL HTGateway_deleteAll (void)
{
    if (remove_allObjects(gateways)) {
	HTList_delete(gateways);
	gateways = NULL;
	return YES;
    }
    return NO;
}

/*	HTNoProxy_add
**	-------------
**	Registers a host name or a domain as a place where no proxy should
**	be contacted - for example a very fast link. If `port' is '0' then
**	it applies to all ports and if `access' is NULL then it applies to
**	to all access methods.
**
**	Examples:	w3.org
**			www.close.com
*/
PUBLIC BOOL HTNoProxy_add (const char * host, const char * access,
			   unsigned port)
{
    if (!noproxy)
	noproxy = HTList_new();    
    return add_hostname(noproxy, host, access, port, NO, -1);
}

/*	HTNoProxy_addRegex
**	------------------
**	Registers a regular expression where URIs matching this expression
**      should go directly and not via a proxy.
**
*/
PUBLIC BOOL HTNoProxy_addRegex (const char * regex, int regex_flags)
{
    if (!noproxy)
	noproxy = HTList_new();    
#ifdef HT_POSIX_REGEX
    return add_hostname(noproxy, regex, NULL, 0, YES, regex_flags);
#else
    return add_hostname(noproxy, regex, NULL, 0, NO, -1);
#endif
}

/*	HTNoProxy_deleteAll
**	-------------------
**	Removes all registered no_proxy directives
*/
PUBLIC BOOL HTNoProxy_deleteAll (void)
{
    if (remove_AllHostnames(noproxy)) {
	HTList_delete(noproxy);
	noproxy = NULL;
	return YES;
    }
    return NO;
}

/*      HTNProxy_noProxyIsOnlyProxy
**     `----------------------------
**      Returns the state of the noproxy_is_onlyproxy flag
*/
PUBLIC int HTProxy_NoProxyIsOnlyProxy (void)
{
  return noproxy_is_onlyproxy;
}

/*      HTNProxy_setNoProxyisOnlyProxy
**      --------------------------
**      Sets the state of the noproxy_is_onlyproxy flag
*/
PUBLIC void HTProxy_setNoProxyIsOnlyProxy (int value)
{
  noproxy_is_onlyproxy = value;
}

/*	HTProxy_find
**	------------
**	This function evaluates the lists of registered proxies and if
**	one is found for the actual access method and it is not registered
**	in the `noproxy' list, then a URL containing the host to be contacted
**	is returned to the caller. This string must be freed be the caller.
**
**	Returns: proxy	If OK (must be freed by caller)
**		 NULL	If no proxy is found or error
*/
PUBLIC char * HTProxy_find (const char * url)
{
    char * access;
    char * proxy = NULL;
    int no_proxy_found = 0;

    if (!url || !proxies)
	return NULL;
    access = HTParse(url, "", PARSE_ACCESS);

    /* First check if the host (if any) is registered in the noproxy list */
    if (noproxy) {
	char *host = HTParse(url, "", PARSE_HOST);
	char *ptr;
	unsigned port=0;
	if ((ptr = strchr(host, ':')) != NULL) {
	    *ptr++ = '\0';				    /* Chop off port */
	    if (*ptr) port = (unsigned) atoi(ptr);
	}
	if (*host) {				   /* If we have a host name */
	    HTList *cur = noproxy;
	    HTHostList *pres;
	    while ((pres = (HTHostList *) HTList_nextObject(cur)) != NULL) {
#ifdef HT_POSIX_REGEX
		if (pres->regex) {
		    BOOL match = regexec(pres->regex, url, 0, NULL, 0) ? NO : YES;
		    if (match) {
			HTTRACE(PROT_TRACE, "GetProxy.... No proxy directive found: `%s\'\n" _ pres->host);
			no_proxy_found = 1;
			break;
		    }
		} else
#endif
		if (!pres->access ||
		    (pres->access && !strcmp(pres->access, access))) {
		    if ((pres->port == 0) || (pres->port == port)) {
			char *np = pres->host+strlen(pres->host);
			char *hp = host+strlen(host);
			while (np>=pres->host && hp>=host && (*np--==*hp--));
			if (np==pres->host-1 && (hp==host-1 || *hp=='.')) {
			    HTTRACE(PROT_TRACE, "GetProxy.... No proxy directive found: `%s\'\n" _ pres->host);
			    no_proxy_found = 1;
			    break;
			}
		    }
		}
	    }
	}
	HT_FREE(host);
    }

    if ((no_proxy_found && !noproxy_is_onlyproxy)
        || (!no_proxy_found && noproxy_is_onlyproxy)) {
      HT_FREE(access);
      return NULL;
    }

    /* Now check if we have a proxy registered for this access method */
    {
	HTList *cur = proxies;
	HTProxy *pres;
	while ((pres = (HTProxy *) HTList_nextObject(cur)) != NULL) {
#ifdef HT_POSIX_REGEX
	    if (pres->regex) {
		BOOL match = regexec(pres->regex, url, 0, NULL, 0) ? NO : YES;
		if (match) {
		    StrAllocCopy(proxy, pres->url);
		    HTTRACE(PROT_TRACE, "GetProxy.... Found: `%s\'\n" _ pres->url);
		    break;
		}
	    } else
#endif
	    if (!strcmp(pres->access, access)) {
		StrAllocCopy(proxy, pres->url);
		HTTRACE(PROT_TRACE, "GetProxy.... Found: `%s\'\n" _ pres->url);
		break;
	    }
	}
    }
    HT_FREE(access);
    return proxy;
}


/*	HTGateway_find
**	--------------
**	This function evaluates the lists of registered gateways and if
**	one is found for the actual access method then it is returned
**
**	Returns: gateway If OK (must be freed by caller)
**		 NULL	 If no gateway is found or error
*/
PUBLIC char * HTGateway_find (const char * url)
{
    char * access;
    char * gateway = NULL;
    if (!url || !gateways)
	return NULL;
    access = HTParse(url, "", PARSE_ACCESS);

    /* Check if we have a gateway registered for this access method */
    {
	HTList *cur = gateways;
	HTProxy *pres;
	while ((pres = (HTProxy *) HTList_nextObject(cur)) != NULL) {
	    if (!strcmp(pres->access, access)) {
		StrAllocCopy(gateway, pres->url);
		HTTRACE(PROT_TRACE, "GetGateway.. Found: `%s\'\n" _ pres->url);
		break;
	    }
	}
    }
    HT_FREE(access);
    return gateway;
}


/*
**	This function maintains backwards compatibility with the old 
**	environment variables and searches for the most common values:
**	http, ftp, news, wais, and gopher
*/
PUBLIC void HTProxy_getEnvVar (void)
{
    char buf[80];
    static const char *accesslist[] = {
	"http",
	"ftp",
	"news",
	"wais",
	"gopher",
	NULL
    };
    const char **access = accesslist;
    HTTRACE(PROT_TRACE, "Proxy....... Looking for environment variables\n");
    while (*access) {
	BOOL found = NO;
	char *gateway=NULL;
	char *proxy=NULL;

	/* Search for proxy gateways */
	if (found == NO) {
	    strcpy(buf, *access);
	    strcat(buf, "_proxy");
	    if ((proxy = (char *) getenv(buf)) && *proxy) {
		HTProxy_add(*access, proxy);
		found = YES;
	    }

	    /* Try the same with upper case */
	    if (found == NO) {
		char * up = buf;
		while ((*up = TOUPPER(*up))) up++;
		if ((proxy = (char *) getenv(buf)) && *proxy) {
		    HTProxy_add(*access, proxy);
		    found = YES;
		}
	    }
	}

	/* As a last resort, search for gateway servers */
	if (found == NO) {
	    strcpy(buf, "WWW_");
	    strcat(buf, *access);
	    strcat(buf, "_GATEWAY");
	    if ((gateway = (char *) getenv(buf)) && *gateway) {
		HTGateway_add(*access, gateway);
		found = YES;
	    }
	}
	++access;
    }

    /* Search for `noproxy' directive */
    {
	char *noproxy = getenv("no_proxy");
	if (noproxy && *noproxy) {
	    char *str = NULL;
	    char *strptr;
	    char *name;
	    StrAllocCopy(str, noproxy);		 /* Get copy we can mutilate */
	    strptr = str;
	    while ((name = HTNextField(&strptr)) != NULL) {
		char *portstr = strchr(name, ':');
		unsigned port=0;
		if (portstr) {
		    *portstr++ = '\0';
		    if (*portstr) port = (unsigned) atoi(portstr);
		}

		/* Register it for all access methods */
		HTNoProxy_add(name, NULL, port);
	    }
	    HT_FREE(str);
	}
    }
}

