/* 								     HTAAUtil.c
**	COMMON PARTS OF ACCESS AUTHORIZATION MODULE
**	FOR BOTH SERVER AND BROWSER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	The authentication information is stored in a list of authentication
**	data bases, each uniquely identified by a hostname and a port number.
**	Each data base contains a set of templates which can be used to predict
**	what information to use in a hierarchical tree. All authentication
**	dependent information is stored as opaque data in a anode. Normally
**	a server application would only keep one auth base but if it wants
**	different protection setup as a function of different interfaces then
**	it can have one auth base representing each interface. For example a
**	server with interfaces "www.foo.com" and "internal.foo.com" can have
**	different protection setups for each interface.
**
** AUTHORS:
**	AL	Ari Luotonen	luotonen@dxcern.cern.ch
**	MD 	Mark Donszelmann    duns@vxdeop.cern.ch
**	HFN	Henrik Frystyk
**
** HISTORY:
**	 8 Nov 93  MD	(VMS only) Added case insensitive comparison
**			in HTAA_templateCaseMatch
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTReqMan.h"
#include "HTAssoc.h"
#include "HTAAUtil.h"					 /* Implemented here */

typedef struct _HTAuthScheme {
    char *		scheme;
    HTAuthParCallback *	parser;
    HTAuthGenCallback *	generator;
    HTAuthGcCallback *	gc;
} HTAuthScheme;

PRIVATE HTList * HTSchemes;	/* List of registered authentication schemes */

typedef struct _HTABase {		  /* Server Authentication info base */
    char *	host;
    int		port;
    HTList *	templates;		  /* List of templates for this base */
    HTList *	nodes;			      /* List of nodes for this base */
} HTABase;

typedef struct _HTANode { 		  /* Authentication scheme specifics */
    char *	realm;
    char *	scheme;
    void *	data;
} HTANode;

typedef struct _HTATemplate {			 /* Hierarchical information */
    char *	tmplate;
    HTANode *	node;
} HTATemplate;

PRIVATE HTList * AuthBases = NULL;	      /* Current authentication base */

/* ------------------------------------------------------------------------- */
/*			     AUTHENTICATION SCHEMES 			     */
/* ------------------------------------------------------------------------- */

/*	HTAuthCall_add
**	--------------
**	Register a callback functions that is to be called when we want to
**	parse challenges and to generate credentials - or the other way round.
**	If you are a server then you want to do the latter and if you are a 
**	client then you want to do the former.
*/
PUBLIC BOOL HTAuthCall_add (CONST char *	scheme,
			    HTAuthParCallback *	parser,
			    HTAuthGenCallback *	generator,
			    HTAuthGcCallback *	gc)
{
    if (scheme && parser && generator && gc) {
	HTAuthScheme * me;
	if (AUTH_TRACE)
	    HTTrace("Auth add.... %s with parser %p and generator %p\n",
		    scheme, (void *) parser, (void *) generator);
	if ((me = (HTAuthScheme *) HT_CALLOC(1, sizeof(HTAuthScheme))) == NULL)
	    HT_OUTOFMEM("HTAuthCall_add");
	StrAllocCopy(me->scheme, scheme);
	me->parser = parser;
	me->generator = generator;
	me->gc = gc;
	if (!HTSchemes) HTSchemes = HTList_new();	
	return HTList_addObject(HTSchemes, (void *) me);
    }
    return NO;
}

/*	HTAuthCall_delete
**	-------------------
**	Unregister a authentication scheme from the list
**	Return YES if OK, else NO
*/
PUBLIC BOOL HTAuthCall_delete (CONST char * scheme)
{
    HTList * cur = HTSchemes;
    if (scheme && cur) {
	HTAuthScheme * pres;
	while ((pres = (HTAuthScheme *) HTList_nextObject(cur))) {
	    if (!strcmp(scheme, pres->scheme)) {
		HTList_removeObject(HTSchemes, (void *) pres);
		HT_FREE(pres->scheme);
		HT_FREE(pres);
		return YES;
	    }
	}
    }
    return NO;
}

/*	HTAuthCall_deleteAll
**	----------------------
**	Unregisters all call back functions
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTAuthCall_deleteAll (void)
{
    HTList * cur = HTSchemes;
    if (AUTH_TRACE) HTTrace("Auth delete. all schemes\n");
    if (cur) {
	HTAuthScheme * pres;
	while ((pres = (HTAuthScheme *) HTList_nextObject(cur))) {
	    HT_FREE(pres->scheme);
	    HT_FREE(pres);
	}
	HTList_delete(HTSchemes);
	HTSchemes = NULL;
	return YES;
    }
    return NO;
}

/* ------------------------------------------------------------------------- */
/*		       AUTHENTICATION INFORMATION DATA BASE		     */
/* ------------------------------------------------------------------------- */

/*	This module maintains an authentication information database
**	which contains informations for generate either credentials or
**	challenges. The database is symmetric for both server and client
**	applications and the implementation can be changed independent of the 
**	API so if you fell like using a fancy database toolkit then feel free
**	to go right ahead :-)
*/

/*
**	Create a new anode
**	Returns new object or NULL if error
*/
PRIVATE HTANode * HTANode_new (HTABase * base, CONST char * realm,
			       CONST char * scheme, void * data)
{
    if (base && realm && scheme) {
	HTANode * me;
	if ((me = (HTANode *) HT_CALLOC(1, sizeof(HTANode))) == NULL)
	    HT_OUTOFMEM("HTANode_new");
	StrAllocCopy(me->realm, realm);
	StrAllocCopy(me->scheme, scheme);
	me->data = data;
	HTList_addObject(base->nodes, (void *) me);
	return me;
    }
    return NULL;
}

/*
**	Delete a node. We call the scheme gc callback to handle the opaque
**	data object.
*/
PRIVATE BOOL HTANode_delete (HTABase * base, HTANode * me)
{
    if (base && me) {
	HTAuth_cleanup(me->scheme, me->data);
	HT_FREE(me->realm);
	HT_FREE(me->scheme);
	HTList_removeObject(base->nodes, (void *) me);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/*
**	Search an authentication base for a matching anode.
**	Return the anode object found or NULL if none
*/
PRIVATE HTANode * HTANode_find (HTABase * base, CONST char * realm)
{    
    if (base && base->nodes && realm) {
	HTList * cur = base->nodes;
	HTANode * pres;
	while ((pres = (HTANode *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->realm, realm)) return pres;
	}
    }
    return NULL;
}

/*
**	Create a new template and add to authentication base
**	Returns new object or NULL if error
*/
PRIVATE HTATemplate * HTATemplate_new (HTABase * base, char * tmplate,
				       HTANode * node)
{
    if (base && tmplate && node) {
	HTATemplate * me;
	if ((me = (HTATemplate *) HT_CALLOC(1, sizeof(HTATemplate))) == NULL)
	    HT_OUTOFMEM("HTATemplate_new");
	me->node = node;
	me->tmplate = tmplate;
	HTList_addObject(base->templates, (void *) me);
	return me;
    }
    return NULL;
}

/*
**	Delete a template
*/
PRIVATE BOOL HTATemplate_delete (HTABase * base, HTATemplate * me)
{
    if (base && me) {
	HT_FREE(me->tmplate);
	HTList_removeObject(base->templates, (void *) me);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/*
**	Search an authentication base for a matching template.
**	Return the template object found or NULL if none
*/
PRIVATE HTATemplate * HTATemplate_find (HTABase * base, CONST char *docname)
{
    if (base && base->templates && docname) {
	HTList * cur = base->templates;
	HTATemplate * pres;
	while ((pres = (HTATemplate *) HTList_nextObject(cur))) {
	    if (HTStrMatch(pres->tmplate, docname)) {
		if (AUTH_TRACE)
		    HTTrace("Template.... `%s' matched `%s'\n",
			    docname, pres->tmplate);
		return pres;
	    }
	}
    }
    return NULL;
}

/*
**	Search an authentication base for a HTATemplate object in order to
**	update it with a new HTANode. We do this whenever we replace a anode
**	Return YES if OK, else NO
*/
PRIVATE BOOL HTATemplate_update (HTABase * base, HTANode * old, HTANode *me)
{
    if (base && old && me) {
	HTList * cur = base->templates;
	HTATemplate * pres;
	while ((pres = (HTATemplate *) HTList_nextObject(cur))) {
	    if (pres->node == old) {
		pres->node = me;
		if (AUTH_TRACE) HTTrace("Template.... updating %p\n", pres);
		return YES;
	    }
	}
    }
    return NO;
}

/*
**	Create a new authentication base
**	Returns new object or NULL if error
*/
PRIVATE HTABase * HTABase_new (CONST char * host, int port)
{
    if (host) {
	HTABase * me;
	if ((me = (HTABase *) HT_CALLOC(1, sizeof(HTABase))) == NULL)
	    HT_OUTOFMEM("HTABase_new");
	StrAllocCopy(me->host, host);
	me->port = (port > 0 ? port : 80);
	me->templates = HTList_new();
	me->nodes = HTList_new();
	HTList_addObject(AuthBases, (void *) me);
	if (AUTH_TRACE) HTTrace("Auth Base... %p created\n", me);
	return me;
    }
    return NULL;
}

/*
**	Delete a complete server tree and everything within it.
*/
PRIVATE BOOL HTABase_delete (HTABase * base)
{
    if (base) {
	HTList * cur;

	/* Free all templates */
	if ((cur = base->templates)) {
	    HTATemplate * pres;
	    while ((pres = (HTATemplate *) HTList_lastObject(cur)))
		HTATemplate_delete(base, pres);
	    HTList_delete(base->templates);
	}

	/* Free all nodes */
	if ((cur = base->nodes)) {
	    HTANode * pres;
	    while ((pres = (HTANode *) HTList_lastObject(cur)))
		HTANode_delete(base, pres);
	    HTList_delete(base->nodes);	    
	}

	HT_FREE(base->host);
	HT_FREE(base);
	return YES;
    }
    return NO;
}

/*
**	Find a authentication base. Return NULL if not found
*/
PRIVATE HTABase * HTABase_find (CONST char * host, int port)
{
    HTList * cur = AuthBases;
    if (port <= 0) port = 80;
    if (host && cur) {
	HTABase * pres;
	while ((pres = (HTABase *) HTList_nextObject(cur))) {
	    if (pres->port==port && !strcmp(pres->host, host)) return pres;
	}
    }
    return NULL;
}

/* ------------------------------------------------------------------------- */
/*		   HANDLING THE AUTHENTICATION INFORMATION BASE		     */
/* ------------------------------------------------------------------------- */

/*	HTAuthInfo_add
**	--------------
**	Add an access authentication information node to the database. If
**	the entry is already found then it is replaced with the new one.
**	The template must follow normal URI syntax but can include a wildcard
**	Return YES if added (or replaced), else NO
*/
PUBLIC BOOL HTAuthInfo_add (CONST char * scheme, char * url,
			    char * realm, void * data)
{
    HTABase * base;
    HTANode * anode;
    if (!scheme || !url || !realm || !data) return NO;
    if (AUTH_TRACE) HTTrace("Auth base... adding info for `%s'\n", url);
    if (!AuthBases) AuthBases = HTList_new();

    /* Find an existing authentication base or create new */
    {
	char * host = HTParse(url, "", PARSE_HOST);
	char * colon = strchr(host, ':');
	int port = 80;
	if (colon ) {
	    *(colon++) = '\0';			     /* Chop off port number */
	    port = atoi(colon);
	}
	if ((base = HTABase_find(host, port)) == NULL)
	    base = HTABase_new(host, port);
	HT_FREE(host);
	if (!base) return NO;		       /* Couldn't create a new base */
    }

    /*
    ** Find a matching anode or create a new one. If we find an existing one
    ** we also update the template pointing to the anode. Otherwise we create
    ** a new template as well.
    */
    {
	HTANode * old = HTANode_find(base, realm);
	anode = HTANode_new(base, realm, scheme, data);
	if (old) {
	    HTATemplate_update(base, old, anode);
	    HTANode_delete(base, old);
	} else {
	    char * docname = HTParse(url, "", PARSE_PATH);
	    HTATemplate_new(base, docname, anode);
	}
    }
    return anode ? YES : NO;
}

/*	HTAuthInfo_deleteAll
**	--------------------
**	Remove the Database
*/
PUBLIC BOOL HTAuthInfo_deleteAll (void)
{
    if (AuthBases) {
	HTList * cur = AuthBases;
	HTABase * pres;
	while ((pres = (HTABase *) HTList_nextObject(cur)))
	    HTABase_delete(pres);
	HTList_delete(AuthBases);
	AuthBases = NULL;
	return YES;
    }
    return NO;
}

/*	AuthInfo_find
**	-------------
**	Seaches the set of authentication information bases for a match
**	In order to find an anode we do the following:
**
**		1) Find the right auth base
**		2) See if there is a realm match
**		3) See if there is a template match for URL
**
**	Return the node found else NULL which means that we don't have any
**	authentication information to hook on to this request or response
*/
PRIVATE HTANode * HTAuthInfo_find (char * url, char * realm)
{
    HTABase * base;
    HTATemplate * tmplate;
    HTANode * anode;
    if (AUTH_TRACE)
	HTTrace("Auth base... looking for info on `%s'\n", url);

    /* Find an existing authentication base */
    {
	char * host = HTParse(url, "", PARSE_HOST);
	char * colon = strchr(host, ':');
	int port = 80;
	if (colon ) {
	    *(colon++) = '\0';			     /* Chop off port number */
	    port = atoi(colon);
	}	
	base = HTABase_find(host, port);
	HT_FREE(host);
	if (base == NULL) return NULL;			   /* Base not found */
    }

    /* Do we have a realm to look for? */
    if (realm) {
	if ((anode = HTANode_find(base, realm)) != NULL) {
	    if (AUTH_TRACE)
		HTTrace("Auth info... found matching realm `%s\'\n", realm);
	    return anode;
	}
    }

    /* If no realm or realm not found then look for template */
    {
	char * docname = HTParse(url, "", PARSE_PATH);
	if ((tmplate = HTATemplate_find(base, docname)) != NULL)
	    anode = tmplate->node;
	HT_FREE(docname);
	return anode;
    }
    return NULL;						 /* No match */
}

/* ------------------------------------------------------------------------- */
/*		   PARSE AND GENERATE CHELLENGES AND CREDENTIALS	     */
/* ------------------------------------------------------------------------- */

/*	HTAuth_parse
**	------------
**	This function looks for a authentication scheme that matches what we
**	have in the request object and calls the parser callback function.
**	Case is not significant.
**	Return YES or whatever callback returns
*/
PUBLIC BOOL HTAuth_parse (HTRequest * request)
{
    HTList * cur = HTSchemes;
    if (request && request->scheme && request->challenge && cur) {
	HTAuthScheme * pres;
	while ((pres = (HTAuthScheme *) HTList_nextObject(cur))) {
	    if (!strcasecomp(request->scheme, pres->scheme)) {
		if (AUTH_TRACE)
		    HTTrace("Auth Calling Parser %p\n", pres->parser);
		return (*(pres->parser))(request, pres->scheme);
	    }
	}
    }
    if (AUTH_TRACE)HTTrace("Auth Parse.. No challenge or credentials found\n");
    return YES;
}

/*	HTAuth_generate
**	---------------
**	This function looks for a any authentication scheme that protects 
**	this resource and calls the generator callback in order to make a
**	challenge or setup credentials depending on whether we are a server
**	or a client.
**	Return YES or whatever callback returns
*/
PUBLIC BOOL HTAuth_generate (HTRequest * request)
{
    HTList * cur = HTSchemes;
    if (request && cur) {
	char * url = HTAnchor_physical(request->anchor);
	HTANode * node = HTAuthInfo_find(url, request->realm);
	if (node && node->data) {
	    HTAuthScheme * pres;
	    while ((pres = (HTAuthScheme *) HTList_nextObject(cur))) {
		if (!strcasecomp(node->scheme, pres->scheme)) {
		    if (AUTH_TRACE)
			HTTrace("Auth Calling Generator %p\n",pres->generator);
		    return (*(pres->generator))(request, node->scheme,
						node->realm, node->data);
		}
	    }
	}
    }
    if (AUTH_TRACE)HTTrace("Auth Gen.... No challenge or credentials found\n");
    return YES;
}

/*	HTAuth_cleanup
**	--------------
**	This function looks for a authentication scheme that matches what we
**	have in the request object and calls the cleanup callback function.
**	Case is not significant. If the scheme is not registered then
**	Return YES if callback found else NO
*/
PUBLIC BOOL HTAuth_cleanup (CONST char * scheme, void * data)
{
    HTList * cur = HTSchemes;
    if (scheme && cur && data) {
	HTAuthScheme * pres;
	while ((pres = (HTAuthScheme *) HTList_nextObject(cur))) {
	    if (!strcasecomp(scheme, pres->scheme)) {
		if (AUTH_TRACE) HTTrace("Auth Calling gc %p\n", pres->gc);
		(*(pres->gc))(scheme, data);
		return YES;
	    }
	}
    }
    return NO;
}

