/*								     HTAnchor.c
**	HYPERTEXT "ANCHOR" OBJECT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	An anchor represents a region of a hypertext document which is
**	linked to another anchor in the same or a different document.
**
** History
**         Nov 1990  Written in Objective-C for the NeXT browser (TBL)
**	24-Oct-1991 (JFG), written in C, browser-independant 
**	21-Nov-1991 (JFG), first complete version
**	 3-May-1995 (HF), Added a lot of methods and other stuff
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTFormat.h"
#include "HTParse.h"
#include "HTMethod.h"
#include "HTFWrite.h"					  /* for cache stuff */
#include "HTAnchor.h"					 /* Implemented here */

#define HASH_SIZE 101		/* Arbitrary prime. Memory/speed tradeoff */

typedef struct _HyperDoc Hyperdoc;
#ifdef VMS
struct _HyperDoc {
	int junk;	/* VMS cannot handle pointers to undefined structs */
};
#endif

PRIVATE HTList **adult_table=0;  /* Point to table of lists of all parents */

/* ------------------------------------------------------------------------- */
/*				Creation Methods			     */
/* ------------------------------------------------------------------------- */

/*
**	Do not use "new" by itself outside this module. In order to enforce
**	consistency, we insist that you furnish more information about the
**	anchor you are creating : use newWithParent or newWithAddress.
*/
PRIVATE HTParentAnchor * HTParentAnchor_new NOARGS
{
    HTParentAnchor *newAnchor =
	(HTParentAnchor *) calloc(1, sizeof (HTParentAnchor));
    newAnchor->parent = newAnchor;
    newAnchor->content_type = WWW_UNKNOWN;
    newAnchor->mainLink.method = METHOD_INVALID;
    return newAnchor;
}


PRIVATE HTChildAnchor * HTChildAnchor_new NOARGS
{
    return (HTChildAnchor *) calloc (1, sizeof (HTChildAnchor));
}


/*	Create new or find old child anchor
**	-----------------------------------
**
**	Me one is for a new anchor being edited into an existing
**	document. The parent anchor must already exist. All
**	children without tags (no NAME attribut) points to the same NULL
**	child.
*/
PUBLIC HTChildAnchor * HTAnchor_findChild ARGS2 (HTParentAnchor *, parent,
						 CONST char *,tag)
{
    HTChildAnchor *child;
    HTList *kids;
    
    if (!parent) {
	if (ANCH_TRACE)
	    fprintf(TDEST, "Find Child.. called with NULL parent.\n");
	return NULL;
    }

    /* First search list of children to see if tag is already there */
    if ((kids = parent->children)) {
	if (tag && *tag) {	/* TBL */
	    while ((child = (HTChildAnchor *) HTList_nextObject(kids))) {
		if (child->tag && !strcasecomp(child->tag, tag)) {
		    if (ANCH_TRACE)
			fprintf (TDEST,
				 "Find Child.. %p of parent %p with name `%s' already exists.\n",
				 (void *) child, (void *) parent, tag);
		    return child;
		}
	    }
	}
    } else				      /* Create new list of children */
	parent->children = HTList_new ();
    
    /* Did't find it so we need to create a new one */
    child = HTChildAnchor_new();
    HTList_addObject(parent->children, child);
    child->parent = parent;
    StrAllocCopy(child->tag, tag);
    if (ANCH_TRACE)
	fprintf(TDEST,"Find Child.. New Anchor %p named `%s' is child of %p\n",
		(void *) child, tag ? tag : (CONST char *) "", (void *)parent);
    return child;
}


/*	Create new or find old named anchor
**	-----------------------------------
**
**	Me one is for a reference which is found in a document, and might
**	not be already loaded.
**	Note: You are not guaranteed a new anchor -- you might get an old one,
**	like with fonts.
*/
PUBLIC HTAnchor * HTAnchor_findAddress ARGS1 (CONST char *, address)
{
    char *tag = HTParse (address, "", PARSE_ANCHOR);	        /* Any tags? */
    char *newaddr=NULL;
    
    StrAllocCopy(newaddr, address);		         /* Get our own copy */
    if (!HTImProxy)
	newaddr = HTSimplify(&newaddr);	     /* Proxy has already simplified */

    /* If the address represents a sub-anchor, we recursively load its parent,
       then we create a child anchor within that document. */
    if (*tag) {
	char *docAddress = HTParse(newaddr, "", PARSE_ACCESS | PARSE_HOST |
				   PARSE_PATH | PARSE_PUNCTUATION);
	HTParentAnchor * foundParent =
	    (HTParentAnchor *) HTAnchor_findAddress (docAddress);
	HTChildAnchor * foundAnchor = HTAnchor_findChild (foundParent, tag);
	free (docAddress);
	free (tag);
	free (newaddr);
	return (HTAnchor *) foundAnchor;
    } else {		       	     /* Else check whether we have this node */
	int hash;
	CONST char *p;
	HTList * adults;
	HTList *grownups;
	HTParentAnchor * foundAnchor;
	free (tag);
	
	/* Select list from hash table */
	for(p=newaddr, hash=0; *p; p++)
	    hash = (int) ((hash * 3 + (*(unsigned char*)p)) % HASH_SIZE);
	if (!adult_table)
	    adult_table = (HTList**) calloc(HASH_SIZE, sizeof(HTList*));
	if (!adult_table[hash]) adult_table[hash] = HTList_new();
	adults = adult_table[hash];

	/* Search list for anchor */
	grownups = adults;
	while ((foundAnchor = (HTParentAnchor *) HTList_nextObject(grownups))){
	    if (!strcasecomp(foundAnchor->address, newaddr)) {
		if (ANCH_TRACE)
		    fprintf(TDEST, "Find Parent. %p with address `%s' already exists.\n",
			    (void*) foundAnchor, newaddr);
		free(newaddr);			       /* We already have it */
		return (HTAnchor *) foundAnchor;
	    }
	}
	
	/* Node not found : create new anchor. */
	foundAnchor = HTParentAnchor_new();
	foundAnchor->address = newaddr;			/* Remember our copy */
	HTList_addObject (adults, foundAnchor);
	if (ANCH_TRACE) fprintf(TDEST, "Find Parent. %p with hash %d and address `%s' created\n", (void*)foundAnchor, hash, newaddr);
	return (HTAnchor *) foundAnchor;
    }
}

/* ------------------------------------------------------------------------- */
/*				   Link Methods				     */
/* ------------------------------------------------------------------------- */

/*	Create or find a child anchor with a possible link
**	--------------------------------------------------
**
**	Create new anchor with a given parent and possibly
**	a name, and possibly a link to a _relatively_ named anchor.
**	(Code originally in ParseHTML.h)
*/
PUBLIC HTChildAnchor * HTAnchor_findChildAndLink
  ARGS4(
       HTParentAnchor *,parent,	/* May not be 0 */
       CONST char *,tag,	/* May be "" or 0 */
       CONST char *,href,	/* May be "" or 0 */
       HTLinkType *,ltype	/* May be 0 */
       )
{
  HTChildAnchor * child = HTAnchor_findChild(parent, tag);
  if (href && *href) {
    char * relative_to = HTAnchor_address((HTAnchor *) parent);
    char * parsed_address = HTParse(href, relative_to, PARSE_ALL);
    HTAnchor * dest = HTAnchor_findAddress(parsed_address);
    HTAnchor_link((HTAnchor *) child, dest, ltype, METHOD_INVALID);
    free(parsed_address);
    free(relative_to);
  }
  return child;
}

/*	Link me Anchor to another given one
**	-------------------------------------
*/
PUBLIC BOOL HTAnchor_link ARGS4(HTAnchor *,	source,
				HTAnchor *,	destination, 
				HTLinkType *,	type,
				HTMethod,	method)
{
    if (!(source && destination))
	return NO;		/* Can't link to/from non-existing anchor */
    if (ANCH_TRACE)
	fprintf(TDEST, "Link Anchors anchor %p to anchor %p\n",
		(void *) source, (void *) destination);
    if (!source->mainLink.dest) {
	source->mainLink.dest = destination;
	source->mainLink.type = type;
	source->mainLink.method = method;
    } else {
	HTLink * newLink = (HTLink *) calloc(1, sizeof (HTLink));
	if (newLink == NULL) outofmem(__FILE__, "HTAnchor_link");
	newLink->dest = destination;
	newLink->type = type;
	newLink->method = method;
	if (!source->links)
	    source->links = HTList_new();
	HTList_addObject (source->links, newLink);
    }
    if (!destination->parent->sources)
	destination->parent->sources = HTList_new ();
    HTList_addObject (destination->parent->sources, source);
    return YES;
}


/*
**  Returns the main destination of this anchor
*/
PUBLIC HTAnchor * HTAnchor_followMainLink ARGS1(HTAnchor *, me)
{
    return me ? me->mainLink.dest : NULL;
}


/*
**  Returns the methods registered for the main destination of this
**  anchor
*/
PUBLIC HTMethod HTAnchor_mainLinkMethod ARGS1(HTAnchor *, me)
{
    return me ? me->mainLink.method : METHOD_INVALID;
}


/*
**  Moves all link information from one anchor to another.
**  This is used in redirection etc.
**  Returns YES if OK, else NO
*/
PUBLIC BOOL HTAnchor_moveLinks ARGS2(HTAnchor *, src, HTAnchor *, dest)
{
    if (!src || !dest) return NO;
    if (ANCH_TRACE)
	fprintf(TDEST, "Move Links.. from anchor %p to anchor %p\n",
		(void *) src, (void *) dest);

    /* Move main link information */
    dest->mainLink.dest = src->mainLink.dest;
    dest->mainLink.type = src->mainLink.type;
    dest->mainLink.method = src->mainLink.method;
    dest->mainLink.result = src->mainLink.result;

    src->mainLink.dest = NULL;
    src->mainLink.type = NULL;
    src->mainLink.method = METHOD_INVALID;
    src->mainLink.result = HT_OK;

    /* Move link information for other links */
    if (dest->links) {
	HTList *cur = dest->links;
	HTLink *pres;
	while ((pres = (HTLink *) HTList_nextObject(cur)))
	    free(pres);
	HTList_delete(dest->links);
    }
    dest->links = src->links;
    src->links = NULL;
    return YES;
}


/*
**  Returns a link with a given link type or NULL if nothing found
*/
PUBLIC HTAnchor * HTAnchor_followTypedLink ARGS2(HTAnchor *, me,
						 HTLinkType *,type)
{
    if (me->mainLink.type == type)
	return me->mainLink.dest;
    if (me->links) {
	HTList *links = me->links;
	HTLink *link;
	while ((link = (HTLink *) HTList_nextObject (links)))
	    if (link->type == type)
		return link->dest;
    }
    return NULL;		/* No link of me type */
}


/*
**  Is this anchor a destination link of the source anchor?
**  Return YES if so, else NO
*/
PUBLIC BOOL HTAnchor_isLink ARGS2(HTAnchor *, src, HTAnchor *, dest)
{
    if (src && dest) {
	if (src->mainLink.dest == dest)
	    return YES;
	if (src->links) {
	    HTList *cur = src->links;
	    HTLink *pres;
	    while ((pres = (HTLink *) HTList_nextObject(cur)) != NULL) {
		if (pres->dest == dest)
		    return YES;
	    }
	}
    }
    return NO;
}


/*
**  Upgrade the link to the main destination and and downgrade the
**  current main link to the list
*/
PUBLIC BOOL HTAnchor_makeMainLink ARGS2 (HTAnchor *, me, HTLink *, movingLink)
{
    if (! (me && HTList_removeObject (me->links, movingLink)))
	return NO;
    else {
	/* First push current main link onto top of links list */
	HTLink *newLink = (HTLink*) malloc (sizeof (HTLink));
	if (newLink == NULL) outofmem(__FILE__, "HTAnchor_makeMainLink");
	memcpy (newLink, & me->mainLink, sizeof (HTLink));
	HTList_addObject (me->links, newLink);

	/* Now make movingLink the new main link, and free it */
	memcpy (& me->mainLink, movingLink, sizeof (HTLink));
	free (movingLink);
	return YES;
    }
}

/*		Move an anchor to the head of the list of its siblings
**		------------------------------------------------------
**
**	This is to ensure that an anchor which might have already existed
**	is put in the correct order as we load the document.
*/

PUBLIC void HTAnchor_makeLastChild
  ARGS1(HTChildAnchor *,me)
{
  if (me->parent != (HTParentAnchor *) me) {  /* Make sure it's a child */
    HTList * siblings = me->parent->children;
    HTList_removeObject (siblings, me);
    HTList_addObject (siblings, me);
  }
}

/* ------------------------------------------------------------------------- */
/*				Deletion Methods			     */
/* ------------------------------------------------------------------------- */

/*	Delete an anchor and possibly related things (auto garbage collection)
**	--------------------------------------------
**
**	The anchor is only deleted if the corresponding document is not loaded.
**	All outgoing links from parent and children are deleted, and this
**	anchor is removed from the sources list of all its targets.
**	We also try to delete the targets whose documents are not loaded.
**	If this anchor's source list is empty, we delete it and its children.
*/

/*	Deletes all the memory allocated in a parent anchor and returns the
**	hyperdoc
*/
PRIVATE HyperDoc * delete_parent ARGS1(HTParentAnchor *, me)
{
    HyperDoc *doc = me->document;

    /* Remove link and address information */
    if (me->links) {
	HTList *cur = me->links;
	HTLink *pres;
	while ((pres = (HTLink *) HTList_nextObject(cur)))
	    free(pres);
	HTList_delete(me->links);
    }
    HTList_delete (me->children);
    HTList_delete (me->sources);
    FREE(me->physical);
    FREE(me->address);

    /* Then remove entity header information (metainformation) */
    FREE(me->title);
    FREE(me->derived_from);
    FREE(me->version);
    if (me->extra_headers) {
	HTList *cur = me->extra_headers;
	char *pres;
	while ((pres = (char *) HTList_nextObject(cur)))
	    free(pres);
	HTList_delete(me->extra_headers);
    }

    /* @@@ TEMPORARY SOLUTION FOR CACHE - SHOULD BE PERSISTENT */
    if (me->cacheItems) {
	HTCacheClear(me->cacheItems);
    }
    free(me);
    return doc;
}


/*	Delete a parent anchor and all its children. If a HyperDoc is found
**	then it's returned
*/
PRIVATE HyperDoc *delete_family ARGS1(HTAnchor *, me)
{
    HTParentAnchor *parent = me->parent;
    if (ANCH_TRACE)
	fprintf(TDEST, "AnchorDelete Remove parent %p and children\n", parent);
    if (!me) {
	if (ANCH_TRACE)
	    fprintf(TDEST, "AnchorDelete No anchor found\n");
	return NULL;
    }

    /* Delete children */
    if (parent->children) {
	HTChildAnchor *child;
	while ((child = (HTChildAnchor *)
		HTList_removeLastObject(parent->children))) {
	    FREE(child->tag);
	    if (child->links) {
		HTList *cur = child->links;
		HTLink *pres;
		while ((pres = (HTLink *) HTList_nextObject(cur)))
		    free(pres);
		HTList_delete(child->links);
	    }
	    free(child);
	}
    }
    return delete_parent(parent);
}


/*	DELETE ALL ANCHORS
**	------------------
**	Deletes all anchors and return a list of all the HyperDocs found.
**	It is for the application to delete any HyperDocs.
**	Return YES if OK, else NO
*/
PUBLIC BOOL HTAnchor_deleteAll ARGS1(HTList *, documents)
{
    int cnt;
    HTList *cur;
    if (!documents)
	return NO;
    for (cnt=0; cnt<HASH_SIZE; cnt++) {
	if ((cur = adult_table[cnt])) { 
	    HTParentAnchor *pres;
	    while ((pres = (HTParentAnchor *) HTList_nextObject(cur)) != NULL){
		HyperDoc *doc = delete_family((HTAnchor *) pres);
		if (doc) HTList_addObject(documents, (void *) doc);
	    }
	}
	HTList_delete(adult_table[cnt]);
    }
    FREE(adult_table);
    return YES;
}


PRIVATE void deleteLinks
  ARGS1 (HTAnchor *,me)
{
  if (! me)
    return;

  /* Recursively try to delete target anchors */
  if (me->mainLink.dest) {
    HTParentAnchor *parent = me->mainLink.dest->parent;
    HTList_removeObject (parent->sources, me);
    if (! parent->document)  /* Test here to avoid calling overhead */
      HTAnchor_delete (parent);
  }
  if (me->links) {  /* Extra destinations */
    HTLink *target;
    while ((target = (HTLink *) HTList_removeLastObject (me->links))) {
      HTParentAnchor *parent = target->dest->parent;
      HTList_removeObject (parent->sources, me);
      if (! parent->document)  /* Test here to avoid calling overhead */
	HTAnchor_delete (parent);
    }
  }
}

PUBLIC BOOL HTAnchor_delete
  ARGS1 (HTParentAnchor *,me)
{
  HTChildAnchor *child;

  /* Don't delete if document is loaded */
  if (me->document)
    return NO;

  /* Recursively try to delete target anchors */
  deleteLinks ((HTAnchor *) me);

  if (! HTList_isEmpty (me->sources)) {  /* There are still incoming links */
    /* Delete all outgoing links from children, if any */
    HTList *kids = me->children;
    while ((child = (HTChildAnchor *) HTList_nextObject (kids)))
      deleteLinks ((HTAnchor *) child);
    return NO;  /* Parent not deleted */
  }

  /* No more incoming links : kill everything */
  /* First, recursively delete children */
  while ((child = (HTChildAnchor *) HTList_removeLastObject (me->children))) {
    deleteLinks ((HTAnchor *) child);
    free (child->tag);
    free (child);
  }

  /* Now kill myself */
  delete_parent(me);
  return YES;  /* Parent deleted */
}

/* ------------------------------------------------------------------------- */
/*				Data Access Methods			     */
/* ------------------------------------------------------------------------- */

PUBLIC HTParentAnchor * HTAnchor_parent ARGS1 (HTAnchor *,me)
{
    return me ? me->parent : NULL;
}

PUBLIC void HTAnchor_setDocument ARGS2 (HTParentAnchor *, me, HyperDoc *,doc)
{
    if (me)
	me->document = doc;
}

PUBLIC HyperDoc * HTAnchor_document ARGS1 (HTParentAnchor *,me)
{
    return me ? me->document : NULL;
}


#if 0
/* We might want to use this when we have a link editing application */
PUBLIC void HTAnchor_setAddress
  ARGS2 (HTAnchor *,me, char *,addr)
{
  if (me)
    StrAllocCopy (me->parent->address, addr);
}
#endif

PUBLIC char * HTAnchor_address ARGS1 (HTAnchor *, me)
{
    char *addr = NULL;
    if (me) {
	if (((HTParentAnchor *) me == me->parent) ||
	    !((HTChildAnchor *) me)->tag) { /* it's an adult or no tag */
	    StrAllocCopy (addr, me->parent->address);
	}
	else {			/* it's a named child */
	    addr = (char *) malloc (2 + strlen (me->parent->address)
				    + strlen (((HTChildAnchor *) me)->tag));
	    if (addr == NULL) outofmem(__FILE__, "HTAnchor_address");
	    sprintf (addr, "%s#%s", me->parent->address,
		     ((HTChildAnchor *) me)->tag);
	}
    }
    return addr;
}

PUBLIC BOOL HTAnchor_hasChildren ARGS1 (HTParentAnchor *,me)
{
    return me ? ! HTList_isEmpty(me->children) : NO;
}

PUBLIC void HTAnchor_clearIndex ARGS1 (HTParentAnchor *,me)
{
    if (me)
	me->isIndex = NO;
}

PUBLIC void HTAnchor_setIndex ARGS1 (HTParentAnchor *,me)
{
  if (me)
    me->isIndex = YES;
}

PUBLIC BOOL HTAnchor_isIndex ARGS1 (HTParentAnchor *,me)
{
    return me ? me->isIndex : NO;
}

/*	Protocol
**	--------
*/

PUBLIC void * HTAnchor_protocol ARGS1(HTParentAnchor *, me)
{
    return me->protocol;
}

PUBLIC void HTAnchor_setProtocol ARGS2(HTParentAnchor *, me,
	void*,	protocol)
{
    me->protocol = protocol;
}

/*	Physical Address
**	----------------
*/

PUBLIC char * HTAnchor_physical ARGS1(HTParentAnchor *, me)
{
    return me->physical;
}

PUBLIC void HTAnchor_setPhysical ARGS2(HTParentAnchor *, me,
	char *,	physical)
{
    if (!me || !physical) {
	if (ANCH_TRACE)
	    fprintf(TDEST, "HTAnchor.... setPhysical, called with null argument\n");
	return;
    }
    StrAllocCopy(me->physical, physical);
}

/* ------------------------------------------------------------------------- */
/*			      Entity Header Information			     */
/* ------------------------------------------------------------------------- */

/*
**	Media Types (Content-Type)
*/
PUBLIC HTFormat HTAnchor_format ARGS1(HTParentAnchor *,me)
{
    return me ? me->content_type : NULL;
}

PUBLIC void HTAnchor_setFormat ARGS2(HTParentAnchor *,me, HTFormat, form)
{
    if (me) me->content_type = form;
}

/*
**	Charset parameter to Content-Type
*/
PUBLIC HTCharset HTAnchor_charset ARGS1(HTParentAnchor *, me)
{
    return me ? me->charset : NULL;
}

PUBLIC void HTAnchor_setCharset ARGS2(HTParentAnchor *,me, HTCharset, charset)
{
    if (me) me->charset = charset;
}

/*
**	Level parameter to Content-Type
*/
PUBLIC HTLevel HTAnchor_level ARGS1(HTParentAnchor *, me)
{
    return me ? me->level : NULL;
}

PUBLIC void HTAnchor_setLevel ARGS2(HTParentAnchor *,me, HTLevel, level)
{
    if (me) me->level = level;
}

/*
**	Content Encoding
*/
PUBLIC HTEncoding HTAnchor_encoding ARGS1(HTParentAnchor *,me)
{
    return me ? me->content_encoding : NULL;
}

PUBLIC void HTAnchor_setEncoding ARGS2(HTParentAnchor *,me,	
				       HTEncoding, encoding)
{
    if (me) me->content_encoding = encoding;
}

/*
**	Content Language
**	@@@ SHOULD BE A LIST @@@
*/
PUBLIC HTLanguage HTAnchor_language ARGS1(HTParentAnchor *,me)
{
    return me ? me->content_language : NULL;
}

PUBLIC void HTAnchor_setLanguage ARGS2(HTParentAnchor *,me,	
				       HTLanguage, language)
{
    if (me) me->content_language = language;
}

/*
**	Content Transfer Encoding
*/
PUBLIC HTCte HTAnchor_cte ARGS1(HTParentAnchor *,me)
{
    return me ? me->cte : NULL;
}

PUBLIC void HTAnchor_setCte ARGS2(HTParentAnchor *,me, HTCte, cte)
{
    if (me) me->cte = cte;
}

/*
**	Content Length
*/
PUBLIC long int HTAnchor_length ARGS1(HTParentAnchor *,me)
{
    return me ? me->content_length : -1;
}

PUBLIC void HTAnchor_setLength ARGS2(HTParentAnchor *,me, long int, length)
{
    if (me) me->content_length = length;
}

/*
**	Allowed methods	(Allow)
*/
PUBLIC int HTAnchor_methods ARGS1(HTParentAnchor *, me)
{
    return me ? me->methods : 0;
}

PUBLIC void HTAnchor_setMethods ARGS2(HTParentAnchor *,me, int, methodset)
{
    if (me) me->methods = methodset;
}

PUBLIC void HTAnchor_appendMethods ARGS2(HTParentAnchor *,me, int, methodset)
{
    if (me) me->methods += methodset;
}

/*
**	Title
*/
PUBLIC CONST char * HTAnchor_title ARGS1 (HTParentAnchor *,me)
{
    return me ? me->title : NULL;
}

PUBLIC void HTAnchor_setTitle ARGS2(HTParentAnchor *,me, CONST char *, title)
{
    if (me && title) StrAllocCopy(me->title, title);
}

PUBLIC void HTAnchor_appendTitle ARGS2(HTParentAnchor *,me, CONST char *,title)
{
    if (me && title) StrAllocCat(me->title, title);
}

/*
**	Version
*/
PUBLIC CONST char * HTAnchor_version ARGS1 (HTParentAnchor *,me)
{
    return me ? me->version : NULL;
}

PUBLIC void HTAnchor_setVersion ARGS2(HTParentAnchor *, me,
				      CONST char *, version)
{
    if (me && version) StrAllocCopy(me->version, version);
}

/*
**	Derived from
*/
PUBLIC CONST char * HTAnchor_derived ARGS1 (HTParentAnchor *,me)
{
    return me ? me->derived_from : NULL;
}

PUBLIC void HTAnchor_setDerived ARGS2(HTParentAnchor *, me,
				      CONST char *, derived_from)
{
    if (me && derived_from) StrAllocCopy(me->derived_from, derived_from);
}

/*
**	Extra Header List of unknown headers
*/
PUBLIC HTList * HTAnchor_Extra ARGS1 (HTParentAnchor *, me)
{
    return me ? me->extra_headers : NULL;
}

PUBLIC void HTAnchor_addExtra ARGS2(HTParentAnchor *, me,
				    CONST char *    , header)
{
    if (me) {
	char *newhead = NULL;
	StrAllocCopy(newhead, header);
	if (!me->extra_headers)
	    me->extra_headers = HTList_new();
	HTList_addObject(me->extra_headers, (void *) newhead);
    }
}

/*
**	Has header been parsed?
*/
PUBLIC BOOL HTAnchor_headerParsed ARGS1(HTParentAnchor *, me)
{
    return (me ? me->header_parsed : NO);
}

PUBLIC void HTAnchor_setHeaderParsed ARGS1(HTParentAnchor *, me)
{
    if (me) me->header_parsed = YES;
}

/*	Clear Header Information
**	------------------------
*/
PUBLIC void HTAnchor_clearHeader ARGS1(HTParentAnchor *, me)
{
    me->methods = METHOD_INVALID;
    me->content_encoding = NULL;
#ifdef NEW_CODE
    /* WAIT UNTIL WE HANDLE LANGUAGE AS A LIST */
    if (me->content_language) {
	HTList_delete(me->content_language);
	me->content_language = HTList_new();
    }
#else
    me->content_language = NULL;
#endif
    me->content_length = -1;					  /* Invalid */
    me->cte = NULL;
    me->content_type = WWW_UNKNOWN;
    me->charset = NULL;
    me->level = NULL;
    
    me->date = (time_t) 0;
    me->expires = (time_t) 0;
    me->last_modified = (time_t) 0;
    
    FREE(me->derived_from);
    FREE(me->version);

    if (me->extra_headers) {
	HTList *cur = me->extra_headers;
	char *pres;
	while ((pres = (char *) HTList_nextObject(cur)))
	    free(pres);
	HTList_delete(me->extra_headers);
	me->extra_headers = NULL;
    }
    me->header_parsed = NO;				      /* All cleared */
}
