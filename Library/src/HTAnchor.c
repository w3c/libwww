/*								     HTAnchor.c
**	HYPERTEXT "ANCHOR" OBJECT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	An anchor represents a region of a hypertext document which is
**	linked to another anchor in the same or a different document.
**
** History
**         Nov 1990  Written in Objective-C for the NeXT browser (TBL)
**	24-Oct-1991 (JFG), written in C, browser-independant 
**	21-Nov-1991 (JFG), first complete version
**	 3-May-1995 (HF), Added a lot of methods and other stuff made an object
**	July 1996	Patch for adding hash of children Michael Farrar
*/

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "HTFormat.h"
#include "HTParse.h"
#include "HTMethod.h"
#include "HTAncMan.h"					 /* Implemented here */

#define HASH_SIZE	101	   /* Arbitrary prime. Memory/speed tradeoff */
#define CHILD_HASH_SIZE	 67	       /* Often smaller than hash of parents */

PRIVATE HTList **adult_table=0;  /* Point to table of lists of all parents */

/* ------------------------------------------------------------------------- */
/*				Creation Methods			     */
/* ------------------------------------------------------------------------- */

/*
**	Do not use "new" by itself outside this module. In order to enforce
**	consistency, we insist that you furnish more information about the
**	anchor you are creating : use newWithParent or newWithAddress.
*/
PRIVATE HTParentAnchor * HTParentAnchor_new (void)
{
    HTParentAnchor *newAnchor;
    if ((newAnchor = (HTParentAnchor *) HT_CALLOC(1, sizeof (HTParentAnchor))) == NULL)
	HT_OUTOFMEM("HTParentAnchor_new");
    newAnchor->parent = newAnchor;
    newAnchor->content_type = WWW_UNKNOWN;
    newAnchor->mainLink.method = METHOD_INVALID;
    newAnchor->content_length = -1;			 /* howcome 6 dec 95 */
    newAnchor->date = (time_t) -1;
    newAnchor->expires = (time_t) -1;
    newAnchor->last_modified = (time_t) -1;
    return newAnchor;
}


PRIVATE HTChildAnchor * HTChildAnchor_new (void)
{
    HTChildAnchor *child;
    if ((child = (HTChildAnchor  *) HT_CALLOC(1, sizeof(HTChildAnchor))) == NULL)
        HT_OUTOFMEM("HTChildAnchor_new");
    return child;
}

/*	Create new or find old child anchor
**	-----------------------------------
**
**	Me one is for a new anchor being edited into an existing
**	document. The parent anchor must already exist. All
**	children without tags (no NAME attribut) points to the same NULL
**	child.
**	Children are now hashed for performance reasons. Thanks to
**	Michael Farrar
*/
PUBLIC HTChildAnchor * HTAnchor_findChild (HTParentAnchor *	parent,
					   const char *		tag)
{
    HTChildAnchor * child = NULL;
    HTList * kids = NULL;
    if (!parent) {
	if (ANCH_TRACE) HTTrace("Child Anchor Bad argument\n");
	return NULL;
    }

    /* Find a hash for this tag (if any) */
    {
	int hash = 0;
	/*
	** If tag is empty then use hash value 0
	*/
	if (tag) {
	    const char * ptr = tag;
	    for(; *ptr; ptr++)
		hash = (int) ((hash*3 + (*(unsigned char*)ptr)) % CHILD_HASH_SIZE);
	}
	if (!parent->children) {
	    if (!(parent->children = (HTList **)	
		  HT_CALLOC(CHILD_HASH_SIZE, sizeof(HTList *))))
		HT_OUTOFMEM("HTAnchor_findChild");
	}
	if (!parent->children[hash]) parent->children[hash] = HTList_new();
	kids = parent->children[hash];
    }

    /* First search list of children to see if tag is already there */
    if (tag && *tag) {
	HTList * cur = kids;
	while ((child = (HTChildAnchor *) HTList_nextObject(cur))) {
	    if (!strcmp(child->tag, tag)) {
		if (ANCH_TRACE)
		    HTTrace("Child Anchor %p of parent %p with name `%s' already exists.\n",
			    (void *) child, (void *) parent, tag);
		return child;
	    }
	}
    }

    /* If not found then create a new child anchor */
    child = HTChildAnchor_new();
    HTList_addObject(kids, (void *) child);
    child->parent = parent;
    if (tag) StrAllocCopy(child->tag, tag);
    if (ANCH_TRACE)
	HTTrace("Child Anchor New Anchor %p named `%s' is child of %p\n",
		(void *) child, tag ? tag : (const char *) "", (void *)parent);
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
PUBLIC HTAnchor * HTAnchor_findAddress (const char * address)
{
    char *tag = HTParse (address, "", PARSE_ANCHOR);	        /* Any tags? */
    
    /* If the address represents a sub-anchor, we recursively load its parent,
       then we create a child anchor within that document. */
    if (*tag) {
	char *addr = HTParse(address, "", PARSE_ACCESS | PARSE_HOST |
			     PARSE_PATH | PARSE_PUNCTUATION);
	HTParentAnchor * parent = (HTParentAnchor*) HTAnchor_findAddress(addr);
	HTChildAnchor * child = HTAnchor_findChild(parent, tag);
	HT_FREE(addr);
	HT_FREE(tag);
	return (HTAnchor *) child;
    } else {		       	     /* Else check whether we have this node */
	int hash;
	const char *p;
	HTList * adults;
	HTList *grownups;
	HTParentAnchor * foundAnchor;
	char *newaddr = NULL;
	StrAllocCopy(newaddr, address);		         /* Get our own copy */
	HT_FREE(tag);
	newaddr = HTSimplify(&newaddr);

	/* Select list from hash table */
	for(p=newaddr, hash=0; *p; p++)
	    hash = (int) ((hash * 3 + (*(unsigned char*)p)) % HASH_SIZE);
	if (!adult_table) {
	    if ((adult_table = (HTList* *) HT_CALLOC(HASH_SIZE, sizeof(HTList*))) == NULL)
	        HT_OUTOFMEM("HTAnchor_findAddress");
	}
	if (!adult_table[hash]) adult_table[hash] = HTList_new();
	adults = adult_table[hash];

	/* Search list for anchor */
	grownups = adults;
	while ((foundAnchor = (HTParentAnchor *) HTList_nextObject(grownups))){
	    if (!strcmp(foundAnchor->address, newaddr)) {
		if (ANCH_TRACE)
		    HTTrace("Find Parent. %p with address `%s' already exists.\n",
			    (void*) foundAnchor, newaddr);
		HT_FREE(newaddr);		       /* We already have it */
		return (HTAnchor *) foundAnchor;
	    }
	}
	
	/* Node not found : create new anchor. */
	foundAnchor = HTParentAnchor_new();
	foundAnchor->address = newaddr;			/* Remember our copy */
	HTList_addObject (adults, foundAnchor);
	if (ANCH_TRACE) HTTrace("Find Parent. %p with hash %d and address `%s' created\n", (void*)foundAnchor, hash, newaddr);
	return (HTAnchor *) foundAnchor;
    }
}

/*	Create or find a child anchor with a possible link
**	--------------------------------------------------
**
**	Create new anchor with a given parent and possibly
**	a name, and possibly a link to a _relatively_ named anchor.
**	All parameters EXCEPT parent can be NULL
*/
PUBLIC HTChildAnchor * HTAnchor_findChildAndLink (HTParentAnchor *	parent,
						  const char *		tag,
						  const char *		href,
						  HTLinkType		ltype)
{
    HTChildAnchor * child = HTAnchor_findChild(parent, tag);
    if (child && href && *href) {
	char * relative_to = HTAnchor_expandedAddress((HTAnchor *) parent);
	char * parsed_address = HTParse(href, relative_to, PARSE_ALL);
	HTAnchor * dest = HTAnchor_findAddress(parsed_address);
	HTLink_add((HTAnchor *) child, dest, ltype, METHOD_INVALID);
	HT_FREE(parsed_address);
	HT_FREE(relative_to);
    }
    return child;
}

/* ------------------------------------------------------------------------- */
/*				   Link Methods				     */
/* ------------------------------------------------------------------------- */

/*
**  Upgrade the link to the main destination and and downgrade the
**  current main link to the list
*/
PUBLIC HTLink * HTAnchor_mainLink (HTAnchor * me)
{
    return me ? &(me->mainLink) : NULL;
}

PUBLIC BOOL HTAnchor_setMainLink  (HTAnchor * me, HTLink * movingLink)
{
    if (!(me && me->links && movingLink &&
	  HTList_removeObject(me->links, movingLink)))
	return NO;
    else {
	/* First push current main link onto top of links list */
	HTLink * newLink = HTLink_new();
	memcpy ((void *) newLink, & me->mainLink, sizeof (HTLink));
	HTList_addObject (me->links, newLink);

	/* Now make movingLink the new main link, and delete it */
	memcpy ((void *) &me->mainLink, movingLink, sizeof (HTLink));
	HTLink_delete(movingLink);
	return YES;
    }
}

/*
**	Handling sub links
*/
PUBLIC HTList * HTAnchor_subLinks (HTAnchor * anchor)
{
    return anchor ? anchor->links : NULL;
}

PUBLIC BOOL HTAnchor_setSubLinks (HTAnchor * anchor, HTList * list)
{
    if (anchor) {
	anchor->links = list;
	return YES;
    }
    return NO;
}

/*
**  Returns the main destination of this anchor
*/
PUBLIC HTAnchor * HTAnchor_followMainLink (HTAnchor * me)
{
    return me ? HTLink_destination(&me->mainLink) : NULL;
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

/*	Deletes all the memory allocated in a parent anchor and returns any
**	hyperdoc object hanging of this anchor
*/
PRIVATE void * delete_parent (HTParentAnchor * me)
{
    void * doc = me->document;

    /* Remove link and address information */
    if (me->links) {
	HTList *cur = me->links;
	HTLink *pres;
	while ((pres = (HTLink *) HTList_nextObject(cur)))
	    HTLink_delete(pres);
	HTList_delete(me->links);
    }

    /* Remove children */
    if (me->children) {
	int cnt = 0;
	for (; cnt<CHILD_HASH_SIZE; cnt++) {
	    if (me->children[cnt]) HTList_delete(me->children[cnt]);
	}
	HT_FREE(me->children);
    }

    HTList_delete (me->sources);
    HTList_delete (me->variants);
    HT_FREE(me->physical);
    HT_FREE(me->address);

    /* Then remove entity header information (metainformation) */
    HT_FREE(me->title);
    if (me->content_encoding) HTList_delete(me->content_encoding);
    if (me->content_language) HTList_delete(me->content_language);
    HT_FREE(me->derived_from);
    HT_FREE(me->version);
    if (me->extra_headers) {
	HTList *cur = me->extra_headers;
	char *pres;
	while ((pres = (char *) HTList_nextObject(cur)))
	    HT_FREE(pres);
	HTList_delete(me->extra_headers);
    }
    HT_FREE(me);
    return doc;
}


/*	Delete a parent anchor and all its children. If a hyperdoc object
**	is found hanging off the parent anchor then this is returned
*/
PRIVATE void * delete_family (HTAnchor * me)
{
    HTParentAnchor *parent = me->parent;
    if (ANCH_TRACE)
	HTTrace("AnchorDelete Remove parent %p and children\n", parent);
    if (!me) {
	if (ANCH_TRACE)
	    HTTrace("AnchorDelete No anchor found\n");
	return NULL;
    }

    /* Delete children */
    if (parent->children) {
	int cnt = 0;
	for (; cnt<CHILD_HASH_SIZE; cnt++) {
	    HTList * kids = parent->children[cnt];
	    if (kids) {
		HTChildAnchor * child;
		while ((child=(HTChildAnchor*)HTList_removeLastObject(kids))) {
		    HT_FREE(child->tag);
		    if (child->links) {
			HTList * cur = child->links;
			HTLink * pres;
			while ((pres = (HTLink *) HTList_nextObject(cur)))
			    HTLink_delete(pres);
			HTList_delete(child->links);
		    }
		    HT_FREE(child);
		}
		HTList_delete(kids);
		parent->children[cnt] = NULL;
	    }
	}
    }
    return delete_parent(parent);
}


/*	DELETE ALL ANCHORS
**	------------------
**	Deletes all anchors and return a list of all the HyperDocs found.
**	It is for the application to delete any HyperDocs.
**	If NULL then no hyperdocs are returned
**	Return YES if OK, else NO
*/
PUBLIC BOOL HTAnchor_deleteAll (HTList * documents)
{
    int cnt;
    HTList *cur;
    if (!adult_table)
	return NO;
    for (cnt=0; cnt<HASH_SIZE; cnt++) {
	if ((cur = adult_table[cnt])) { 
	    HTParentAnchor *pres;
	    while ((pres = (HTParentAnchor *) HTList_nextObject(cur)) != NULL){
		void * doc = delete_family((HTAnchor *) pres);
		if (doc && documents) HTList_addObject(documents, doc);
	    }
	}
	HTList_delete(adult_table[cnt]);
    }
    HT_FREE(adult_table);
    return YES;
}


PRIVATE void delete_links (HTAnchor * me)
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

PUBLIC BOOL HTAnchor_delete (HTParentAnchor * me)
{
    /* Don't delete if document is loaded */
    if (!me || me->document) {
	if (ANCH_TRACE) HTTrace("Anchor...... Not deleted\n");
	return NO;
    }

    /* Recursively try to delete target anchors */
    delete_links ((HTAnchor *) me);

    if (!HTList_isEmpty(me->sources)) {    /* There are still incoming links */

	/*
	** Delete all outgoing links from children, if any
	*/
	if (me->children) {
	    int cnt = 0;
	    for (; cnt<CHILD_HASH_SIZE; cnt++) {
		HTList * kids = me->children[cnt];
		if (kids) {
		    HTChildAnchor * child;
		    while ((child = (HTChildAnchor *) HTList_nextObject(kids)))
			delete_links((HTAnchor *) child);
		    return NO;	/* Parent not deleted */
		}
	    }
	}

	/*
	** No more incoming links : kill everything
	** First, recursively delete children
	*/
	if (me->children) {
	    int cnt = 0;
	    for (; cnt<CHILD_HASH_SIZE; cnt++) {
		HTList * kids = me->children[cnt];
		if (kids) {
		    HTChildAnchor * child;
		    while ((child=(HTChildAnchor *) HTList_removeLastObject(kids)))
			delete_links((HTAnchor *) child);
		    HT_FREE(child->tag);
		    HT_FREE(child);
		}
	    }
	}
    }

    /* Now kill myself */
    delete_parent(me);
    return YES;  /* Parent deleted */
#if 0
  if (! HTList_isEmpty (me->sources)) {  /* There are still incoming links */
    /* Delete all outgoing links from children, if any */
    HTList *kids = me->children;
    while ((child = (HTChildAnchor *) HTList_nextObject (kids)))
      delete_links ((HTAnchor *) child);
    return NO;  /* Parent not deleted */
  }

  /* No more incoming links : kill everything */
  /* First, recursively delete children */
  while ((child = (HTChildAnchor *) HTList_removeLastObject (me->children))) {
    delete_links ((HTAnchor *) child);
    HT_FREE(child->tag);
    HT_FREE(child);
  }
#endif
}

/* ------------------------------------------------------------------------- */
/*				Data Access Methods			     */
/* ------------------------------------------------------------------------- */

PUBLIC HTParentAnchor * HTAnchor_parent  (HTAnchor * me)
{
    return me ? me->parent : NULL;
}

PUBLIC void HTAnchor_setDocument  (HTParentAnchor * me, void * doc)
{
    if (me) me->document = doc;
}

PUBLIC void * HTAnchor_document  (HTParentAnchor * me)
{
    return me ? me->document : NULL;
}

PUBLIC char * HTAnchor_address  (HTAnchor * me) 
{ 
    char *addr = NULL;
    if (me) {
        if (((HTParentAnchor *) me == me->parent) ||
            !((HTChildAnchor *) me)->tag) { /* it's an adult or no tag */
            StrAllocCopy (addr, me->parent->address);
        }
        else {                  /* it's a named child */
            if ((addr = (char  *) HT_MALLOC(2 + strlen (me->parent->address) + \
strlen (((HTChildAnchor *) me)->tag))) == NULL)
                HT_OUTOFMEM("HTAnchor_address");
            sprintf (addr, "%s#%s", me->parent->address,
                     ((HTChildAnchor *) me)->tag);
        }
    }
    return addr;
}

/*
**	We resolve the child address with respect to either a base URL,
**	a content-location, or to the request-URI
*/
PUBLIC char * HTAnchor_expandedAddress  (HTAnchor * me)
{
    char *addr = NULL;
    if (me) {
	HTParentAnchor * parent = me->parent;
	char * base = parent->content_location ? parent->content_location :
	    parent->content_base ? parent->content_base : parent->address;
	if (((HTParentAnchor *) me == me->parent) ||
	    !((HTChildAnchor *) me)->tag) { /* it's an adult or no tag */
	    StrAllocCopy(addr, base);
	} else {			/* it's a named child */
	    if ((addr = (char *) HT_MALLOC(2 + strlen(base) + strlen(((HTChildAnchor *) me)->tag))) == NULL)
	        HT_OUTOFMEM("HTAnchor_address");
	    sprintf (addr, "%s#%s", base, ((HTChildAnchor *) me)->tag);
	}
    }
    return addr;
}

/*	Physical Address
**	----------------
*/
PUBLIC char * HTAnchor_physical (HTParentAnchor * me)
{
    return me ? me->physical ? me->physical : me->address : NULL;
}

PUBLIC void HTAnchor_setPhysical (HTParentAnchor * me, char * physical)
{
    if (!me || !physical) {
	if (ANCH_TRACE)
	    HTTrace("HTAnchor.... setPhysical, called with null argument\n");
	return;
    }
    StrAllocCopy(me->physical, physical);
}

PUBLIC void HTAnchor_clearPhysical(HTParentAnchor * me)
{
    if (me) HT_FREE(me->physical);
}

/*
**	Children information
*/
PUBLIC BOOL HTAnchor_hasChildren  (HTParentAnchor * me)
{
    return (me && me->children);
}

/*	Cache Information
**	-----------------
*/
PUBLIC BOOL HTAnchor_cacheHit (HTParentAnchor * me)
{
    return me ? me->cacheHit : NO;
}

PUBLIC void HTAnchor_setCacheHit (HTParentAnchor * me, BOOL cacheHit)
{
    if (me) me->cacheHit = cacheHit;
}

/* ------------------------------------------------------------------------- */
/*			      Entity Header Information			     */
/* ------------------------------------------------------------------------- */

/*
**	Variants. If this anchor has any variants then keep them in a list
**	so that we can find them later. The list is simply a list of 
**	parent anchors.
*/
PUBLIC HTList * HTAnchor_variants (HTParentAnchor * me)
{
    return me ? me->variants : NULL;
}

PUBLIC BOOL HTAnchor_addVariant (HTParentAnchor * me,
				 HTParentAnchor * variant)
{
    if (me && variant) {
	if (!me->variants) me->variants = HTList_new();
	return HTList_addObject(me->variants, variant);
    }
    return NO;
}

PUBLIC BOOL HTAnchor_deleteVariant (HTParentAnchor * me,
				    HTParentAnchor * variant)
{
    return (me && variant) ? HTList_removeObject(me->variants, variant) : NO;
}

/*
**	Is this resource an index?
*/
PUBLIC void HTAnchor_clearIndex  (HTParentAnchor * me)
{
    if (me) me->isIndex = NO;
}

PUBLIC void HTAnchor_setIndex  (HTParentAnchor * me)
{
  if (me) me->isIndex = YES;
}

PUBLIC BOOL HTAnchor_isIndex  (HTParentAnchor * me)
{
    return me ? me->isIndex : NO;
}

/*	Content Base
**	------------
*/
PUBLIC char * HTAnchor_base (HTParentAnchor * me)
{
    return me ? me->content_base : NULL;
}

PUBLIC BOOL HTAnchor_setBase (HTParentAnchor * me, char * base)
{
    if (!me || !base) {
	if (ANCH_TRACE)
	    HTTrace("HTAnchor.... set base called with null argument\n");
	return NO;
    }
    StrAllocCopy(me->content_base, base);
    return YES;
}

/*	Content Location
**	----------------
*/
PUBLIC char * HTAnchor_location (HTParentAnchor * me)
{
    return me ? me->content_location : NULL;
}

/*
**	Expand the location relative to the base URL if any, otherwise the 
**	anchor address it self
*/
PUBLIC BOOL HTAnchor_setLocation (HTParentAnchor * me, char * location)
{
    if (me && location) {
	char * base = me->content_base ? me->content_base : me->address;
	me->content_location = HTParse(location, base, PARSE_ALL);
	return YES;
    }
    return NO;
}

/*	Content-Type
**	------------
*/
PUBLIC HTFormat HTAnchor_format (HTParentAnchor * me)
{
    return me ? me->content_type : NULL;
}

PUBLIC void HTAnchor_setFormat (HTParentAnchor * me, HTFormat form)
{
    if (me) me->content_type = form;
}

PUBLIC HTAssocList * HTAnchor_formatParam (HTParentAnchor * me)
{
    return me ? me->type_parameters : NULL;
}

PUBLIC BOOL HTAnchor_addFormatParam (HTParentAnchor * me,
				     const char * name, const char * value)
{
    if (me) {
	if (!me->type_parameters) me->type_parameters = HTAssocList_new();
	return HTAssocList_replaceObject(me->type_parameters, name, value);
    }
    return NO;
}

/*
**	Charset parameter to Content-Type
*/
PUBLIC HTCharset HTAnchor_charset (HTParentAnchor * me)
{
    if (me && me->type_parameters) {
	char * charset = HTAssocList_findObject(me->type_parameters,"charset");
	return HTAtom_for(charset);
    }
    return NULL;
}

PUBLIC BOOL HTAnchor_setCharset (HTParentAnchor * me, HTCharset charset)
{
    return HTAnchor_addFormatParam(me, "charset", HTAtom_name(charset));
}

/*
**	Level parameter to Content-Type
*/
PUBLIC HTLevel HTAnchor_level (HTParentAnchor * me)
{
    if (me && me->type_parameters) {
	char * level = HTAssocList_findObject(me->type_parameters, "level");
	return HTAtom_for(level);
    }
    return NULL;
}

PUBLIC BOOL HTAnchor_setLevel (HTParentAnchor * me, HTLevel level)
{
    return HTAnchor_addFormatParam(me, "level", HTAtom_name(level));
}

/*
**	Content Encoding
*/
PUBLIC HTList * HTAnchor_encoding (HTParentAnchor * me)
{
    return me ? me->content_encoding : NULL;
}

PUBLIC BOOL HTAnchor_addEncoding (HTParentAnchor * me, HTEncoding  encoding)
{
    if (me && encoding) {
	if (!me->content_encoding) me->content_encoding = HTList_new();
	return HTList_addObject(me->content_encoding, encoding);
    }
    return NO;
}

/*
**	Content Language
*/
PUBLIC HTList * HTAnchor_language (HTParentAnchor * me)
{
    return me ? me->content_language : NULL;
}

PUBLIC BOOL HTAnchor_addLanguage (HTParentAnchor * me, HTLanguage  language)
{
    if (me && language) {
	if (!me->content_language) me->content_language = HTList_new();
	return HTList_addObject(me->content_language, language);
    }
    return NO;
}

/*
**	Content Transfer Encoding
*/
PUBLIC HTEncoding HTAnchor_transfer (HTParentAnchor * me)
{
    return me ? me->transfer : NULL;
}

PUBLIC void HTAnchor_setTransfer (HTParentAnchor * me, HTEncoding transfer)
{
    if (me) me->transfer = transfer;
}

/*
**	Content Length
*/
PUBLIC long int HTAnchor_length (HTParentAnchor * me)
{
    return me ? me->content_length : -1;
}

PUBLIC void HTAnchor_setLength (HTParentAnchor * me, long int length)
{
    if (me) me->content_length = length;
}

PUBLIC void HTAnchor_addLength (HTParentAnchor * me, long int deltalength)
{
    if (me) {
	if (me->content_length < 0)
	    me->content_length = deltalength;
	else
	    me->content_length += deltalength;
    }
}

/*
**	Allowed methods	(Allow)
*/
PUBLIC HTMethod HTAnchor_methods (HTParentAnchor * me)
{
    return me ? me->methods : METHOD_INVALID;
}

PUBLIC void HTAnchor_setMethods (HTParentAnchor * me, HTMethod methodset)
{
    if (me) me->methods = methodset;
}

PUBLIC void HTAnchor_appendMethods (HTParentAnchor * me, HTMethod methodset)
{
    if (me) me->methods |= methodset;
}

/*
**	Title
*/
PUBLIC const char * HTAnchor_title  (HTParentAnchor * me)
{
    return me ? me->title : NULL;
}

PUBLIC void HTAnchor_setTitle (HTParentAnchor * me, const char * title)
{
    if (me && title) StrAllocCopy(me->title, title);
}

PUBLIC void HTAnchor_appendTitle (HTParentAnchor * me, const char * title)
{
    if (me && title) StrAllocCat(me->title, title);
}

/*
**	Version
*/
PUBLIC char * HTAnchor_version (HTParentAnchor * me)
{
    return me ? me->version : NULL;
}

PUBLIC void HTAnchor_setVersion (HTParentAnchor * me, const char * version)
{
    if (me && version) StrAllocCopy(me->version, version);
}

/*
**	Derived from
*/
PUBLIC char * HTAnchor_derived (HTParentAnchor * me)
{
    return me ? me->derived_from : NULL;
}

PUBLIC void HTAnchor_setDerived (HTParentAnchor * me, const char *derived_from)
{
    if (me && derived_from) StrAllocCopy(me->derived_from, derived_from);
}

/*
**	Content MD5
*/
PUBLIC char * HTAnchor_md5 (HTParentAnchor * me)
{
    return me ? me->content_md5 : NULL;
}

PUBLIC void HTAnchor_setMd5 (HTParentAnchor * me, const char * hash)
{
    if (me && hash) StrAllocCopy(me->content_md5, hash);
}

/*
**	Date
*/
PUBLIC time_t HTAnchor_date (HTParentAnchor * me)
{
    return me ? me->date : -1;
}

PUBLIC void HTAnchor_setDate (HTParentAnchor * me, const time_t date)
{
    if (me) me->date = date;
}

/*
**	Expires
*/
PUBLIC time_t HTAnchor_expires (HTParentAnchor * me)
{
    return me ? me->expires : -1;
}

PUBLIC void HTAnchor_setExpires (HTParentAnchor * me, const time_t expires)
{
    if (me) me->expires = expires;
}

/*
**	Last Modified
*/
PUBLIC time_t HTAnchor_lastModified (HTParentAnchor * me)
{
    return me ? me->last_modified : -1;
}

PUBLIC void HTAnchor_setLastModified (HTParentAnchor * me, const time_t lm)
{
    if (me) me->last_modified = lm;
}

/*
**	Entity Tag
*/
PUBLIC char * HTAnchor_etag (HTParentAnchor * me)
{
    return me ? me->etag : NULL;
}

PUBLIC void HTAnchor_setEtag (HTParentAnchor * me, const char * etag)
{
    if (me && etag) StrAllocCopy(me->etag, etag);
}

PUBLIC BOOL HTAnchor_isEtagWeak (HTParentAnchor * me)
{
    return (me && me->etag && !strncasecomp(me->etag, "W/", 2));
}

/*
**	Extra Header List of unknown headers
*/
PUBLIC HTList * HTAnchor_Extra  (HTParentAnchor * me)
{
    return me ? me->extra_headers : NULL;
}

PUBLIC void HTAnchor_addExtra (HTParentAnchor * me, const char * header)
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
PUBLIC BOOL HTAnchor_headerParsed (HTParentAnchor * me)
{
    return (me ? me->header_parsed : NO);
}

PUBLIC void HTAnchor_setHeaderParsed (HTParentAnchor * me)
{
    if (me) me->header_parsed = YES;
}

/*	Clear Header Information
**	------------------------
*/
PUBLIC void HTAnchor_clearHeader (HTParentAnchor * me)
{
    me->methods = METHOD_INVALID;
    if (me->content_encoding) {
	HTList_delete(me->content_encoding);
	me->content_encoding = HTList_new();
    }
    if (me->content_language) {
	HTList_delete(me->content_language);
	me->content_language = HTList_new();
    }
    HT_FREE(me->content_base);
    HT_FREE(me->content_location);
    me->content_length = -1;					  /* Invalid */
    me->transfer = NULL;
    me->content_type = WWW_UNKNOWN;
    if (me->type_parameters) {
	HTAssocList_delete(me->type_parameters);
	me->type_parameters = NULL;
    }    
    me->date = (time_t) -1;
    me->expires = (time_t) -1;
    me->last_modified = (time_t) -1;
    
    HT_FREE(me->derived_from);
    HT_FREE(me->version);

    if (me->extra_headers) {
	HTList *cur = me->extra_headers;
	char *pres;
	while ((pres = (char *) HTList_nextObject(cur)))
	    HT_FREE(pres);
	HTList_delete(me->extra_headers);
	me->extra_headers = NULL;
    }
    me->header_parsed = NO;				      /* All cleared */
}
