/*								     HTProxy.c
**	GATEWAY AND PROXY MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
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
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTList.h"
#include "HTParse.h"
#include "HTWWWStr.h"
#include "HTProxy.h"					 /* Implemented here */

/* Variables and typedefs local to this module */

typedef struct _HTProxy {
    char *	access;
    char *	url;			          /* URL of Gateway or Proxy */
} HTProxy;

typedef struct _HTHostlist {
    char *	access;
    char *	host;				      /* Host or domain name */
    unsigned	port;
} HTHostList;

PRIVATE HTList * proxies = NULL;		    /* List of proxy servers */
PRIVATE HTList * gateways = NULL;			 /* List of gateways */
PRIVATE HTList * noproxy = NULL;   /* Don't proxy on these hosts and domains */

#if 0
PRIVATE HTList * onlyproxy = NULL;  /* Proxy only on these hosts and domains */
#endif

/* ------------------------------------------------------------------------- */

/*
**	Existing entries are replaced with new ones
*/
PRIVATE BOOL add_object (HTList * list, CONST char * access, CONST char * url)
{
    HTProxy *me;
    if (!list || !access || !url || !*url)
	return NO;
    if ((me = (HTProxy *) HT_CALLOC(1, sizeof(HTProxy))) == NULL)
	HT_OUTOFMEM("add_object");
    StrAllocCopy(me->access, access);		     	    /* Access method */
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
	    if (PROT_TRACE)
		TTYPrint(TDEST, "HTProxy..... replacing for `%s\' access %s\n",
			me->url, me->access);
	    HT_FREE(pres->access);
	    HT_FREE(pres->url);
	    HTList_removeObject(list, (void *) pres);
	    HT_FREE(pres);
	}
	if (PROT_TRACE)
	    TTYPrint(TDEST, "HTProxy..... adding for `%s\' access %s\n",
		    me->url, me->access);
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
PRIVATE BOOL add_hostname (HTList * list, CONST char * host,
			   CONST char * access, unsigned port)
{
    HTHostList *me;
    if (!list || !host || !*host)
	return NO;
    if ((me = (HTHostList *) HT_CALLOC(1, sizeof(HTHostList))) == NULL)
        HT_OUTOFMEM("add_hostname");
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
    if (PROT_TRACE)
	TTYPrint(TDEST, "HTHostList.. adding `%s\' to list\n", me->host);
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
PUBLIC BOOL HTProxy_add (CONST char * access, CONST char * proxy)
{
    if (!proxies)
	proxies = HTList_new();    
    return add_object(proxies, access, proxy);
}

/*
**	Removes all registered proxies
*/
PUBLIC BOOL HTProxy_deleteAll (void)
{
    if (remove_allObjects(proxies)) {
	HTList_delete(proxies);
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
PUBLIC BOOL HTGateway_add (CONST char * access, CONST char * gate)
{
    if (!gateways)
	gateways = HTList_new();
    return add_object(gateways, access, gate);
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
PUBLIC BOOL HTNoProxy_add (CONST char * host, CONST char * access,
			   unsigned port)
{
    if (!noproxy)
	noproxy = HTList_new();    
    return add_hostname(noproxy, host, access, port);
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
PUBLIC char * HTProxy_find (CONST char * url)
{
#ifndef HT_NO_PROXY
    char * access;
    char * proxy = NULL;
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
		if (!pres->access ||
		    (pres->access && !strcmp(pres->access, access))) {
		    if (pres->port == port) {
			char *np = pres->host+strlen(pres->host);
			char *hp = host+strlen(host);
			while (np>=pres->host && hp>=host && (*np--==*hp--));
			if (np==pres->host-1 && (hp==host-1 || *hp=='.')) {
			    if (PROT_TRACE)
				TTYPrint(TDEST, "GetProxy.... No proxy directive found: `%s\'\n", pres->host);
			    HT_FREE(access);
			    return NULL;
			}
		    }
		}
	    }
	}
	HT_FREE(host);
    }

    /* Now check if we have a proxy registered for this access method */
    {
	HTList *cur = proxies;
	HTProxy *pres;
	while ((pres = (HTProxy *) HTList_nextObject(cur)) != NULL) {
	    if (!strcmp(pres->access, access)) {
		StrAllocCopy(proxy, pres->url);
		if (PROT_TRACE)
		    TTYPrint(TDEST, "GetProxy.... Found: `%s\'\n", pres->url);
		break;
	    }
	}
    }
    HT_FREE(access);
    return proxy;
#else
    return NULL
#endif /* !HT_NO_PROXY */
    }


/*	HTGateway_find
**	--------------
**	This function evaluates the lists of registered gateways and if
**	one is found for the actual access method then it is returned
**
**	Returns: gateway If OK (must be freed by caller)
**		 NULL	 If no gateway is found or error
*/
PUBLIC char * HTGateway_find (CONST char * url)
{
#ifndef HT_NO_PROXY
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
		if (PROT_TRACE)
		    TTYPrint(TDEST, "GetGateway.. Found: `%s\'\n", pres->url);
		break;
	    }
	}
    }
    HT_FREE(access);
    return gateway;
#else
    return NULL
#endif /* !HT_NO_PROXY */
}


/*
**	This function maintains backwards compatibility with the old 
**	environment variables and searches for the most common values:
**	http, ftp, news, wais, and gopher
*/
PUBLIC void HTProxy_getEnvVar (void)
{
#ifndef HT_NO_PROXY
    char buf[80];
    static CONST char *accesslist[] = {
	"http",
	"ftp",
	"news",
	"wais",
	"gopher",
	NULL
    };
    CONST char **access = accesslist;
    while (*access) {
	char *gateway=NULL;
	char *proxy=NULL;

	/* search for proxy gateways */
	strcpy(buf, *access);
	strcat(buf, "_proxy");
	if ((proxy = (char *) getenv(buf)) && *proxy)
	    HTProxy_add(*access, proxy);

	/* search for gateway servers */
	strcpy(buf, "WWW_");
	strcat(buf, *access);
	strcat(buf, "_GATEWAY");
	if ((gateway = (char *) getenv(buf)) && *gateway)
	    HTGateway_add(*access, gateway);
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
#endif /* !HT_NO_PROXY */
}

