/*
**	URL DATA TREE STRUCTURE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** AUTHORS:
**	HFN	Henrik Frystyk Nielsen <frystyk@w3.org>
**
**	An infobase is a tree structure where we can store all the information
**	we know about a remote server. Typically, each remote server will
**	have its own infobase which gets richer as we get to know more about
**	the remote web site.
**
**	This module maintains an URL information database
**	which can contain information stored by filters. This can for
**	example be used to store challenges received from remote server.
**	A infobase has the advantage that it can be searched using URLs _or_
**	using realms. The letter is most useful to "guess" information
**	about a remote URL that we haven't seen before
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTUTree.h"					 /* Implemented here */

#define TREE_TIMEOUT		43200L	     /* Default tree timeout is 12 h */

struct _HTUTree {			  /* Server URL info base */
    char *		name;
    char *		host;
    int			port;

    HTList *		templates;	  /* List of templates for this tres */
    HTList * 		realms;		     /* List of realms for this tree */

    time_t		created;	     /* Creation time of this object */
    HTUTree_gc * 	gc;			/* Contect garbage collector */
};

struct _HTURealm {			  		  /* Realm specifics */
    char *		realm;
    void *		context;
    HTUTemplate *	tm_ptr;
};

struct _HTUTemplate {				 /* Hierarchical information */
    char *		tmplate;
    HTURealm *	       	rm_ptr;
};

PRIVATE HTList ** InfoTable = NULL;    		/* List of information bases */
PRIVATE time_t UTreeTimeout = TREE_TIMEOUT;

/* ------------------------------------------------------------------------- */

/*
**	Create a new realm
**	Returns new object or NULL if error
*/
PRIVATE HTURealm * HTUTree_newRealm (HTUTree * tree, const char * realm,
				     void * context)
{
    if (tree) {
	HTURealm * me;
	if ((me = (HTURealm *) HT_CALLOC(1, sizeof(HTURealm))) == NULL)
	    HT_OUTOFMEM("HTURealm_new");
	if (realm) StrAllocCopy(me->realm, realm);
	me->context = context;
	HTList_addObject(tree->realms, (void *) me);
	return me;
    }
    return NULL;
}

/*
**	Delete a realm. We call the scheme gc callback to free the opaque
**	context object.
*/
PRIVATE BOOL HTUTree_deleteRealm (HTUTree * tree, HTURealm * me)
{
    if (tree && me) {
	if (tree->gc && me->context) (*tree->gc)(me->context);
	HTList_removeObject(tree->realms, (void *) me);
	HT_FREE(me->realm);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/*
**	Find a realm
*/
PRIVATE HTURealm * HTUTree_findRealm (HTUTree * tree, const char * realm)
{
    if (tree && tree->realms && realm) {
	HTList * cur = tree->realms;
	HTURealm * pres;
	while ((pres = (HTURealm *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->realm, realm)) {
		HTTRACE(CORE_TRACE, "URL Node.... Realm `%s\' found\n" _ realm);
		return pres;
	    }
	}
    }
    return NULL;
}


/*
**	Create a new template and add to URL tree
**	Returns new object or NULL if error
*/
PRIVATE HTUTemplate * HTUTree_newTemplate (HTUTree * tree,const char * tmplate)
{
    if (tree && tmplate) {
	HTUTemplate * me;
	if ((me = (HTUTemplate *) HT_CALLOC(1, sizeof(HTUTemplate))) == NULL)
	    HT_OUTOFMEM("HTUTemplate_new");
	StrAllocCopy(me->tmplate, tmplate);
	HTList_addObject(tree->templates, (void *) me);
	return me;
    }
    return NULL;
}

/*
**	Delete a template
*/
PRIVATE BOOL HTUTree_deleteTemplate (HTUTree * tree, HTUTemplate * me)
{
    if (tree && me) {
	HTList_removeObject(tree->templates, (void *) me);
	HT_FREE(me->tmplate);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/*
**	Find a template
*/
PRIVATE HTUTemplate * HTUTree_findTemplate (HTUTree * tree, const char * path)
{
    if (tree && tree->templates && path) {
	HTUTemplate * pres;
	HTList * cur = tree->templates;
	while ((pres = (HTUTemplate *) HTList_nextObject(cur))) {
	    if (HTStrMatch(pres->tmplate, path)) {
		HTTRACE(CORE_TRACE, "URL Node.... Found template `%s\' for for `%s\'\n" _ 
			    pres->tmplate _ path);
		return pres;
	    }
	}
    }
    return NULL;
}

/*
**	Search a URL Tree for a matching template or realm
**	Return the opaque context object found or NULL if none
**	Please regard this as a first simple attempt - it can be done
**	much more efficient!
*/
PUBLIC void * HTUTree_findNode (HTUTree * tree,
				const char * realm, const char * path)
{
    HTURealm * rm = HTUTree_findRealm(tree, realm);
    if (rm)
	return rm->context;
    else {
	HTUTemplate * tm = HTUTree_findTemplate(tree, path);
	if (tm) return tm->rm_ptr ? tm->rm_ptr->context : NULL;
    }
    HTTRACE(CORE_TRACE, "URL Node.... Not found\n");
    return NULL;
}

/*
**	Add a new node (a template and a realm) to the tree
*/
PUBLIC BOOL HTUTree_addNode (HTUTree * tree,
			     const char * realm, const char * path,
			     void * context)
{
    if (tree) {
	if (realm && path) { 		       /* If both a path and a realm */
	    HTUTemplate * new_template = HTUTree_newTemplate(tree, path);
	    HTURealm * new_realm = HTUTree_newRealm(tree, realm, context);
	    new_realm->tm_ptr = new_template;
	    new_template->rm_ptr = new_realm;
	    return YES;
	} else if (realm) {				  /* If only a realm */
	    HTUTree_newRealm(tree, realm, context);
	    return YES;
	}
	HTTRACE(CORE_TRACE, "URL Node.... At least realm must be present\n");
    }
    return NO;
}

/*
**	Replace node (insert a new context at the same node)
*/
PUBLIC BOOL HTUTree_replaceNode (HTUTree * tree,
				 const char * realm, const char * path,
				 void * context)
{
    HTURealm * rm = HTUTree_findRealm(tree, realm);
    if (!rm) {
	HTUTemplate * tm = HTUTree_findTemplate(tree, path);
	if (tm) rm = tm->rm_ptr;
    }
    if (rm) {
	if (tree->gc && rm->context) (*tree->gc)(rm->context);
	rm->context = context;
	return YES;
    }
    HTTRACE(CORE_TRACE, "URL Node.... Not found\n");
    return NO;
}

/*
**	Remove a node (a template and a realm) from the tree
*/
PUBLIC BOOL HTUTree_deleteNode (HTUTree * tree,
				const char * realm, const char * path)
{
    if (tree) {
	HTURealm * rm = HTUTree_findRealm(tree, realm);
	HTUTemplate * tm = rm ? rm->tm_ptr : HTUTree_findTemplate(tree, path);
	if (!rm) rm = tm ? tm->rm_ptr : NULL;
	HTUTree_deleteRealm(tree, rm);
	HTUTree_deleteTemplate(tree, tm);
	return YES;
    }
    return NO; 
}


PRIVATE BOOL delete_tree (HTUTree * tree)
{
    if (tree) {
	HTList * cur;

	/* Free all templates */
	if ((cur = tree->templates)) {
	    HTUTemplate * pres;
	    while ((pres = (HTUTemplate *) HTList_lastObject(cur)))
		HTUTree_deleteTemplate(tree, pres);
	    HTList_delete(tree->templates);
	}

	/* Free all nodes */
	if ((cur = tree->realms)) {
	    HTURealm * pres;
	    while ((pres = (HTURealm *) HTList_lastObject(cur)))
		HTUTree_deleteRealm(tree, pres);
	    HTList_delete(tree->realms);	    
	}

	HT_FREE(tree->name);
	HT_FREE(tree->host);
	HT_FREE(tree);
	return YES;
    }
    return NO;
}

/*
**	Find an existing URL Tree
**	Returns tree or NULL if error
*/
PRIVATE HTUTree * find_tree (const char * 	name,
			     const char *	host,
			     int		port,
			     HTList **		hashlist)
{
    HTUTree * pres = NULL;
    *hashlist = NULL;
    if (!name || !host) {
	HTTRACE(CORE_TRACE, "URL Tree.... Bad argument\n");
	return NULL;
    }

    /* Find a hash for this host */
    {
	int hash;
	const unsigned char * p;
	for (p=host, hash=0; *p; p++) {
	    hash = (hash * 3 + *p) % HT_L_HASH_SIZE;
	}
	if (!InfoTable) {
	    if ((InfoTable = (HTList **) HT_CALLOC(HT_L_HASH_SIZE,
						   sizeof(HTList *))) == NULL)
	        HT_OUTOFMEM("HTUTree_find");
	}
	if (!InfoTable[hash])
	    InfoTable[hash] = *hashlist = HTList_new();
	else
	    *hashlist = InfoTable[hash];
    }

    /* Search the existing list to see if we already have this entry */
    {
	HTList * cur = *hashlist;
	while ((pres = (HTUTree *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->name, name) && !strcmp(pres->host, host) &&
		pres->port==port) {
		if (time(NULL) > pres->created + UTreeTimeout) {
		    HTTRACE(CORE_TRACE, "URL Tree.... Collecting URL Tree %p\n" _ pres);
		    HTList_removeObject(*hashlist, pres);
		    delete_tree(pres);
		    pres = NULL;
		}
		return pres;
	    }
	}
    }
    return NULL;
}

/*
**	Create a new URL Tree (or return an aready existing one)
**	Returns new object (or the one found) or NULL if error
*/
PUBLIC HTUTree * HTUTree_new (const char * 		name,
			      const char * 		host,
			      int 			port,
			      HTUTree_gc *	 	gc)
{
    if (name && host) {
	HTList * hashlist = NULL;
	HTUTree * pres = find_tree(name, host, port, &hashlist);

	/* If not found (or gc'ed) then create a new URL tree */
	if (!pres) {
	    if ((pres = (HTUTree *) HT_CALLOC(1, sizeof(HTUTree))) == NULL)
		HT_OUTOFMEM("HTUTree_new");
	    StrAllocCopy(pres->name, name);
	    StrAllocCopy(pres->host, host);
	    pres->port = (port > 0 ? port : 80);
	    pres->templates = HTList_new();
	    pres->realms = HTList_new();
	    pres->created = time(NULL);
	    pres->gc = gc;

	    /* Add the new URL tree to the hash table */
	    HTList_addObject(hashlist, (void *) pres);
	    HTTRACE(CORE_TRACE, "URL Tree.... Created %p with name `%s\'\n" _ 
				   pres _ pres->name);
	} else {
	    HTTRACE(CORE_TRACE, "URL Tree.... Found %p with name `%s\'\n" _ 
				    pres _ pres->name);
	}
	return pres;
    } else {
	HTTRACE(CORE_TRACE, "URL Tree.... Bad argument\n");
	return NULL;
    }
}

/*
**	Find a URL tree
*/
PUBLIC HTUTree * HTUTree_find (const char *	name,
			       const char * 	host,
			       int		port)
{
    if (name && host) {
	HTList * hashlist = NULL;
	HTUTree * pres = find_tree(name, host, port, &hashlist);
	HTTRACE(CORE_TRACE, "URL Tree.... did %sfind `%s\'\n" _ 
				pres ? "" : "NOT " _ name);
	return pres;
    } else {
	HTTRACE(CORE_TRACE, "URL Tree.... Bad augument\n");
    }
    return NULL;
}


/*
**	Delete a complete server tree and everything within it.
*/
PUBLIC BOOL HTUTree_delete (const char * 	name,
			    const char * 	host,
			    int			port)
{
    if (name && host) {
	HTList * hashlist = NULL;
	HTUTree * pres = find_tree(name, host, port, &hashlist);
	if (pres) {
	    HTList_removeObject(hashlist, pres);
	    delete_tree(pres);
	    HTTRACE(CORE_TRACE, "URL Tree.... deleted %p\n" _ pres);
	    return YES;
	}
    }
    return NO;
}

/*
**	Delete all URL Trees
*/
PUBLIC BOOL HTUTree_deleteAll (void)
{
    if (InfoTable) {
	int cnt;
	HTList * cur;
	for (cnt=0; cnt<HT_L_HASH_SIZE; cnt++) {
	    if ((cur = InfoTable[cnt])) { 
		HTUTree * pres;
		while ((pres = (HTUTree *) HTList_nextObject(cur)))
		    delete_tree(pres);
	    }
	    HTList_delete(InfoTable[cnt]);
	}
	HT_FREE(InfoTable);
	return YES;
    }
    return NO;
}
