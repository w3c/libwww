/*
**	PEP ENGINE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**
** AUTHORS:
**	HFN	Henrik Frystyk
**
** HISTORY:
**	July 96	HFN	Written first version
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTPEP.h"					 /* Implemented here */

#define PEP_NAME		"w3c-pep"	     /* Name of the PEP tree */
#define DEFAULT_PORT		80		      /* Concentrate on HTTP */

struct _HTPEPModule {
    char *		name;
    HTNetBefore *	before;
    HTNetAfter *	after;
    HTUTree_gc *	gc;
};

typedef struct _HTPEPElement {
    char * 		name;
    void * 		context;
} HTPEPElement;

PRIVATE HTList ** HTModules;		   /* List of registered PEP modules */

/* ------------------------------------------------------------------------- */
/*				PEP MODULE MANAGEMENT			     */
/* ------------------------------------------------------------------------- */

PRIVATE BOOL delete_module (HTPEPModule * module)
{
    if (module) {
	HT_FREE(module->name);
	HT_FREE(module);
	return YES;
    }
    return NO;
}

/*
**	Case sensitive search for module
*/
PRIVATE HTPEPModule * find_module (const char * name, HTList ** hashlist)
{
    HTPEPModule * pres = NULL;
    *hashlist = NULL;
    if (!name) {
	HTTRACE(APP_TRACE, "PEP Engine.. Bad argument\n");
	return NULL;
    }

    /* Find a hash for this PEP module */
    {
	int hash;
	const unsigned char * p;
	for (p=name, hash=0; *p; p++) {
	    hash = (hash * 3 + *p) % HT_M_HASH_SIZE;
	}
	if (!HTModules) {
	    if ((HTModules = (HTList **) HT_CALLOC(HT_M_HASH_SIZE,
						   sizeof(HTList *))) == NULL)
	        HT_OUTOFMEM("find_module");
	}
	if (!HTModules[hash])
	    HTModules[hash] = *hashlist = HTList_new();
	else
	    *hashlist = HTModules[hash];
    }

    /* Search the existing list to see if we already have this entry */
    {
	HTList * cur = *hashlist;
	while ((pres = (HTPEPModule *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->name, name)) return pres;
	}
    }
    return NULL;
}

PUBLIC HTPEPModule * HTPEP_newModule (const char *	name,
				      HTNetBefore *	before,
				      HTNetAfter *	after,
				      HTUTree_gc *	gc)
{
    if (name) {
	HTList * hashlist = NULL;
	HTPEPModule * pres = find_module(name, &hashlist);

	/* If found then update entry - else create a new one */
	if (!pres) {
	    if (!(pres = (HTPEPModule *) HT_CALLOC(1, sizeof(HTPEPModule))))
		HT_OUTOFMEM("HTPEP_newModule");
	    StrAllocCopy(pres->name, name);
	    pres->before = before;
	    pres->after = after;
	    pres->gc = gc;

	    /* Add the new PEP Module to the hash table */
	    HTList_addObject(hashlist, (void *) pres);
	    HTTRACE(APP_TRACE, "PEP Engine.. Created module %p\n" _ pres);
	} else {
	    HTTRACE(APP_TRACE, "PEP Engine.. Found module %p\n" _ pres);
	}
	return pres;
    } else {
	HTTRACE(APP_TRACE, "PEP Engine.. Bad argument\n");
	return NULL;
    }
}

PUBLIC HTPEPModule * HTPEP_findModule (const char * name)
{
    if (name) {
	HTList * hashlist = NULL;
	HTPEPModule * pres = find_module(name, &hashlist);
	HTTRACE(APP_TRACE, "PEP Engine.. did %sfind %s\n" _ pres ? "" : "NOT " _ name);
	return pres;
    } else {
	HTTRACE(APP_TRACE, "PEP Engine.. Bad augument\n");
    }
    return NULL;
}

PUBLIC BOOL HTPEP_deleteModule (const char * name)
{
    if (name) {
	HTList * hashlist = NULL;
	HTPEPModule * pres = find_module(name, &hashlist);
	if (pres) {
	    HTList_removeObject(hashlist, pres);
	    HTTRACE(APP_TRACE, "PEP Engine.. deleted %p\n" _ pres);
	    delete_module(pres);
	    return YES;
	}
    }
    return NO;
}

PUBLIC BOOL HTPEP_deleteAllModules (void)
{
    if (HTModules) {
	int cnt;
	HTList * cur;
	for (cnt=0; cnt<HT_M_HASH_SIZE; cnt++) {
	    if ((cur = HTModules[cnt])) { 
		HTPEPModule * pres;
		while ((pres = (HTPEPModule *) HTList_nextObject(cur)))
		    delete_module(pres);
	    }
	    HTList_delete(HTModules[cnt]);
	}
	HT_FREE(HTModules);
	return YES;
    }
    return NO;
}

/* ------------------------------------------------------------------------- */
/*			    HANDLE THE PEP URL TREE			     */
/* ------------------------------------------------------------------------- */

/*
**	A PEP element is a particular PEP procotol associated with a
**	particular point in the URL tree. The name is the name of the
**	protocol known to be able to handle this context. This protocol
**	must have been registered as a PEP module.
*/
PRIVATE HTPEPElement * HTPEP_newElement (const char * name, void * context)
{
    if (name) {
	HTPEPElement * me;
	if ((me = (HTPEPElement *) HT_CALLOC(1, sizeof(HTPEPElement))) == NULL)
	    HT_OUTOFMEM("HTPEPElement_new");
	StrAllocCopy(me->name, name);
	me->context = context;
	HTTRACE(APP_TRACE, "PEP Engine.. Created element %p\n" _ me);
	return me;
    }
    return NULL;
}

PRIVATE BOOL HTPEP_deleteElement (HTPEPElement * me)
{
    if (me) {
	HTTRACE(APP_TRACE, "PEP Engine.. Deleted element %p\n" _ me);
	HT_FREE(me->name);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/*
**	A PEP list is a list of PEP elements and the "thing" we store in the
**	PEP URL Tree
*/
PRIVATE HTList * HTPEP_newList (void)
{
    HTList * me = HTList_new();
    HTTRACE(APP_TRACE, "PEP Engine.. Created list %p\n" _ me);
    return me;
}

PRIVATE int HTPEP_deleteList (void * context)
{
    HTList * list = (HTList *) context;
    if (list) {
	HTPEPElement * pres;
	while ((pres = (HTPEPElement *) HTList_nextObject(list)))
	    HTPEP_deleteElement(pres);
	HTTRACE(APP_TRACE, "PEP Engine.. Deleted list %p\n" _ list);
	HTList_delete(list);
	return YES;
    }
    return NO;
}

/*
**	Find a PEP List
**	---------------
**	Seaches the set PEP tree for a match. We do the following:
**
**		1) Find the PEP tree for this host
**		2) See if there is a realm match
**		3) See if there is a template match for URL
**
**	Return the node found else NULL which means that we don't have any
**	authentication information to hook on to this request or response
*/
PRIVATE HTList * HTPEP_findList (const char * realm, const char * url)
{
    HTUTree * tree;
    if (!url) {
	HTTRACE(APP_TRACE, "PEP Engine.. Bad argument\n");
	return NULL;
    }
    HTTRACE(APP_TRACE, "PEP Engine.. Looking for info on `%s'\n" _ url);

    /* Find a URLTree for this URL (if any) */
    {
	char * host = HTParse(url, "", PARSE_HOST);
	char * colon = strchr(host, ':');
	int port = DEFAULT_PORT;
	if (colon ) {
	    *(colon++) = '\0';			     /* Chop off port number */
	    port = atoi(colon);
	}	
	tree = HTUTree_find(PEP_NAME, host, port);
	HT_FREE(host);
	if (!tree) {
	    HTTRACE(APP_TRACE, "PEP Engine.. No information\n");
	    return NULL;
	}
    }

    /* Find a matchind PEP element (if any)) */
    {
	char * path = HTParse(url, "", PARSE_PATH);
	HTList * list = (HTList *) HTUTree_findNode(tree, realm, path);
	HT_FREE(path);
	return list;
    }
    return NULL;						 /* No match */
}

/*	Add a PEP element to the URL tree
**	---------------------------------
**	Add a protocol context to the PEP tree
*/
PUBLIC BOOL HTPEP_addNode (const char * protocol,
			   const char * realm, const char * url,
			   void * context)
{
    HTUTree * tree = NULL;
    HTPEPModule * module = NULL;
    if (!protocol || !url) {
	HTTRACE(APP_TRACE, "PEP Engine.. Bad argument\n");
	return NO;
    }
    HTTRACE(APP_TRACE, "PEP Engine.. Adding info for `%s'\n" _ url);

    /* Find the PEP module with this name */
    if ((module = HTPEP_findModule(protocol)) == NULL) {
	HTTRACE(APP_TRACE, "PEP Engine.. Module `%s\' not registered\n" _ 
			       protocol ? protocol : "<null>");
	return NO;
    }

    /* Find an existing PEP tree or create a new one */
    {
	char * host = HTParse(url, "", PARSE_HOST);
	char * colon = strchr(host, ':');
	int port = DEFAULT_PORT;
	if (colon ) {
	    *(colon++) = '\0';			     /* Chop off port number */
	    port = atoi(colon);
	}
	tree = HTUTree_new(PEP_NAME, host, port, HTPEP_deleteList);
	HT_FREE(host);
	if (!tree) {
	    HTTRACE(APP_TRACE, "PEP Engine.. Can't create tree\n");
	    return NO;
	}
    }

    /* Find a matching PEP element or create a new one */
    {
	char * path = HTParse(url, "", PARSE_PATH);
	HTList * list = NULL;
	HTPEPElement * element = HTPEP_newElement(protocol, context);

	/*
	** If existing list not found then create a new list and add it
	** to the tree
	*/
	if ((list = (HTList *) HTUTree_findNode(tree, realm, path)) == NULL) {
	    list = HTPEP_newList();
	    HTUTree_addNode(tree, realm, path, list);
	}
	HT_FREE(path);

	/* Now add the new element */
	return HTList_addObject(list, element);
    }
}

/* ------------------------------------------------------------------------- */
/*				   PEP ENGINE 				     */
/* ------------------------------------------------------------------------- */

/*	HTPEP_beforeFilter
**	------------------
**	Return YES or whatever callback returns
*/
PUBLIC int HTPEP_beforeFilter (HTRequest * request, void * param, int mode)
{
    int ret = HT_OK;
    char * url = HTAnchor_physical(HTRequest_anchor(request));
    const char * realm = HTRequest_realm(request);
    HTList * list = HTPEP_findList(realm, url);
    if (list) {
	HTPEPElement * pres;
	HTTRACE(APP_TRACE, "PEP Engine.. Calling BEFORE protocols %p\n" _ list);
	while ((pres = (HTPEPElement *) HTList_nextObject(list))) {
	    HTPEPModule * module = HTPEP_findModule(pres->name);
	    if (module) {
		ret = (*module->before)(request, pres->context, mode);
		if (ret != HT_OK) break;
	    }
	}
    }
    return HT_OK;
}

/*	HTPEP_afterFilter
**	-----------------
**	Return YES or whatever callback returns
*/
PUBLIC int HTPEP_afterFilter (HTRequest * request, HTResponse * response,
			      void * param, int status)
{
    int ret = HT_OK;
    HTAssocList * protocols = HTResponse_protocol(response);
    if (protocols) {
	HTPEPElement * pres;
	HTTRACE(APP_TRACE, "PEP Engine.. Calling AFTER protocols %p\n" _ protocols);
	while ((pres = (HTPEPElement *) HTList_nextObject(protocols))) {
	    HTPEPModule * module = HTPEP_findModule(pres->name);
	    if (module) {
		ret = (*module->after)(request, response, pres->context, status);
		if (ret != HT_OK) break;
	    }
	}
    }
    return HT_OK;
}
