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
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTFormat.h"
#include "HTParse.h"
#include "HTMethod.h"
#include "HTWWWStr.h"
#include "HTAncMan.h"					 /* Implemented here */

#define PARENT_HASH_SIZE	HT_XL_HASH_SIZE
#define CHILD_HASH_SIZE		HT_L_HASH_SIZE

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
    newAnchor->age = (time_t) -1;
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
	HTTRACE(ANCH_TRACE, "Child Anchor Bad argument\n");
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
	    if (child->tag && !strcmp(child->tag, tag)) {
		HTTRACE(ANCH_TRACE, "Child Anchor %p of parent %p with name `%s' already exists.\n" _ 
			    (void *) child _ (void *) parent _ tag);
		return child;
	    }
	}
    }

    /* If not found then create a new child anchor */
    child = HTChildAnchor_new();
    HTList_addObject(kids, (void *) child);
    child->parent = parent;
    if (tag) StrAllocCopy(child->tag, tag);
    HTTRACE(ANCH_TRACE, "Child Anchor New Anchor %p named `%s' is child of %p\n" _ 
		(void *) child _ tag ? tag : (const char *) "" _ (void *)parent);
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
    char *tag = HTParse (address, "", PARSE_VIEW);	        /* Any tags? */
    
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
	    hash = (int) ((hash * 3 + (*(unsigned char*)p)) % PARENT_HASH_SIZE);
	if (!adult_table) {
	    if ((adult_table = (HTList* *) HT_CALLOC(PARENT_HASH_SIZE, sizeof(HTList*))) == NULL)
	        HT_OUTOFMEM("HTAnchor_findAddress");
	}
	if (!adult_table[hash]) adult_table[hash] = HTList_new();
	adults = adult_table[hash];

	/* Search list for anchor */
	grownups = adults;
	while ((foundAnchor = (HTParentAnchor *) HTList_nextObject(grownups))){
	    if (!strcmp(foundAnchor->address, newaddr)) {
		HTTRACE(ANCH_TRACE, "Find Parent. %p with address `%s' already exists.\n" _ 
			    (void*) foundAnchor _ newaddr);
		HT_FREE(newaddr);		       /* We already have it */
		return (HTAnchor *) foundAnchor;
	    }
	}
	
	/* Node not found : create new anchor. */
	foundAnchor = HTParentAnchor_new();
	foundAnchor->address = newaddr;			/* Remember our copy */
	HTList_addObject (adults, foundAnchor);
	HTTRACE(ANCH_TRACE, "Find Parent. %p with hash %d and address `%s' created\n" _ (void*)foundAnchor _ hash _ newaddr);
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

/*
**  Returns a link with a given link type or NULL if nothing found
*/
PUBLIC HTLink * HTAnchor_findLinkType (HTAnchor * me, HTLinkType type)
{
    if (me) {
	HTLink * link = HTAnchor_mainLink(me);
	HTList * sublinks = HTAnchor_subLinks(me);
	if (link && HTLink_type(link) == type)
	    return link;
	else if (sublinks) {
	    while ((link = (HTLink *) HTList_nextObject (sublinks)))
		if (HTLink_type(link) == type) return link;
	}
    }
    return NULL;
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
    HTAnchor_clearHeader(me);

    HT_FREE(me);
    return doc;
}


/*	Delete a parent anchor and all its children. If a hyperdoc object
**	is found hanging off the parent anchor then this is returned
*/
PRIVATE void * delete_family (HTAnchor * me)
{
    HTParentAnchor * parent = NULL;
    if (!me) {
	HTTRACE(ANCH_TRACE, "AnchorDelete No anchor found\n");
	return NULL;
    }
    parent = me->parent;
    HTTRACE(ANCH_TRACE, "AnchorDelete Remove parent %p and children\n" _ parent);

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
    for (cnt=0; cnt<PARENT_HASH_SIZE; cnt++) {
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

/*
**	Deletes all the metadata associated with anchors but doesn't
**	delete the anchor link structure itself. This is much safer
**	than deleting the complete anchor structure as this represents the
**	complete Web the application has been in touch with
*/
PUBLIC BOOL HTAnchor_clearAll (HTList * documents)
{
    int cnt;
    HTList * cur;
    if (!adult_table) return NO;
    for (cnt=0; cnt<PARENT_HASH_SIZE; cnt++) {
	if ((cur = adult_table[cnt])) { 
	    HTParentAnchor * pres;
	    while ((pres = (HTParentAnchor *) HTList_nextObject(cur))) {

		/* Then remove entity header information */
		HTAnchor_clearHeader(pres);

		/* Delete the physical address */
		HT_FREE(pres->physical);

		/* Register if we have a document on this anchor */
		if (documents && pres->document)
		    HTList_addObject(documents, pres->document);
	    }
	}
    }
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
	HTTRACE(ANCH_TRACE, "Anchor...... Not deleted\n");
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

    /* 2001/03/06: Bug fix by Serge Adda <sAdda@infovista.com>
       HTAnchor_delete wasn't removing the reference to the deleted
       anchor. This caused a bug whenever requesting another anchor
       for the same URL.
    */
    if (adult_table) {
      int hash;
      const char *p;
      HTList * adults;
      HTList * grownups;
      HTList * last;
      HTParentAnchor * foundAnchor;

      /* Select list from hash table */
      for(p=me->address, hash=0; *p; p++)
	hash = (int) ((hash * 3 + (*(unsigned char*)p)) %
		      PARENT_HASH_SIZE);
      adults = adult_table[hash];

      /* Search list for anchor */
      grownups = adults;
      last = grownups;
      while ((foundAnchor = (HTParentAnchor *)
	      HTList_nextObject(grownups))){
	if (!strcmp(foundAnchor->address, me->address)) {
	  HTList_quickRemoveElement (grownups, last);
	  break;
	}
	last = grownups;
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

/*	FLATTEN ALL ANCHORS
**	-------------------
**	Flattens the anchor web structure into an array.
**	This is useful for calculating statistics, sorting
**	the parent anchors etc.
**
**      The caller can indicate the size of the array (total
**      number of anchors if known - otherwise 0).
**
**	Return an array that must be freed by the caller or
**      NULL if no anchors.
*/
PUBLIC HTArray * HTAnchor_getArray (int growby)
{
    int cnt;
    HTArray * array = NULL;
    HTList * cur = NULL;
    if (!adult_table) return NULL;

    /* Allocate an array for the anchors */
    if (growby <= 0) growby = PARENT_HASH_SIZE;
    array = HTArray_new(growby);

    /* Traverse anchor structure */
    for (cnt=0; cnt<PARENT_HASH_SIZE; cnt++) {
	if ((cur = adult_table[cnt])) { 
	    HTParentAnchor * pres = NULL;
	    while ((pres = (HTParentAnchor *) HTList_nextObject(cur)) != NULL) {
                if (HTArray_addObject(array, pres) == NO) {
                    HTTRACE(ANCH_TRACE, "Anchor...... Can't add object %p to array %p\n" _ 
				pres _ array);
                    break;
                }
	    }
	}
    }
    return array;
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
	char * base = HTAnchor_base(parent);
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
	HTTRACE(ANCH_TRACE, "HTAnchor.... setPhysical, called with null argument\n");
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

/*
** Fix up a simple routine to see if this anchor is a (ChildAnchor *)
** Seem to be doing it all over the place, so simplify!
*/
PUBLIC BOOL HTAnchor_isChild (HTAnchor * me)
{
    return (me && (HTParentAnchor *) me != me->parent);
}

PUBLIC char * HTAnchor_view (HTAnchor * me)
{
    char * view = NULL;
    if (me && (HTParentAnchor *) me != me->parent && ((HTChildAnchor *) me)->tag)
	StrAllocCopy(view, ((HTChildAnchor *) me)->tag);
    return view;
}

/* ------------------------------------------------------------------------- */
/*			      Entity Header Information			     */
/* ------------------------------------------------------------------------- */

/*
**  Take the relevant infomration from the response object and cache it
**  in the anchor object. We inherit the information that is already
**  parsed in the response along with the unparsed headers.
*/
PUBLIC BOOL HTAnchor_update (HTParentAnchor * me, HTResponse * response)
{
    if (me && response) {
	HTCachable cachable = HTResponse_isCachable(response);

	if (cachable == HT_CACHE_ETAG) {
	    char * etag = HTResponse_etag(response);
	    HTTRACE(ANCH_TRACE, "HTAnchor.... Updating etag for %p\n" _ me);
	    if (etag) {
		HTAnchor_setEtag(me, etag);
		return YES;
	    }

	} else if (cachable == HT_CACHE_NOT_MODIFIED) {
	    HTTRACE(ANCH_TRACE, "HTAnchor.... Information is up to date for %p\n" _ me);
	    return YES;

	} else if (cachable == HT_CACHE_ALL) {
	    char * etag = HTResponse_etag(response);
	    HTTRACE(ANCH_TRACE, "HTAnchor.... Updating metainformation for %p\n" _ me);

	    /*
	    **  The content length and type is already parsed at this point
	    **  in time. We also check for format parameters like charset etc.
	    **  and copy the contents in the anchor object
	    */
	    me->content_length = HTResponse_length(response);
	    me->content_type = HTResponse_format(response);
	    me->type_parameters = HTResponse_formatParam(response);
	    me->content_encoding = HTResponse_encoding(response);
	
            /* Don't forget the etag as well */
       	    if (etag) HTAnchor_setEtag(me, etag);

	    /*
	    **  Inherit all the unparsed headers - we may need them later!
	    */
	    if (me->headers) HTAssocList_delete(me->headers);
	    me->headers = HTResponse_handOverHeader(response);

	    /*
	    **  Notifify the response object not to delete the lists that we
	    **  have inherited in the anchor object
	    */
	    HTResponse_isCached(response, YES);

	    /*
	    **  Set the datestamp of when the anchor was updated if we didn't
	    **  get any in the response
	    */
	    if (!HTAssocList_findObject(me->headers, "date"))
		HTAnchor_setDate(me, time(NULL));

	    return YES;
	}
    }
    return NO;
}

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
    if (me) {
	if (me->content_base) return me->content_base;
	if (me->headers) {
	    char * base = HTAssocList_findObject(me->headers, "content-base");
	    /*
	    **  If no base is found then take the content-location if this
	    **  is present and is absolute, else use the Request-URI.
	    */
	    if (base) StrAllocCopy(me->content_base, HTStrip(base));
	}

	/*
	**  Try the content location if any
	*/
	{
	    char * location = HTAnchor_location(me);
	    StrAllocCopy(me->content_base,
			 (location && HTURL_isAbsolute(location)) ?
			 location : me->address);
	}
	return me->content_base;
    }
    return NULL;
}

PUBLIC BOOL HTAnchor_setBase (HTParentAnchor * me, char * base)
{
    if (me && base) {
	StrAllocCopy(me->content_base, base);
	return YES;
    }
    return NO;
}

/*	Content Location
**	----------------
*/
PUBLIC char * HTAnchor_location (HTParentAnchor * me)
{
    if (me) {
	if (me->content_location)
	    return *me->content_location ? me->content_location : NULL;
	if (me->headers) {
	    char * location = HTAssocList_findObject(me->headers, "content-location");
	    StrAllocCopy(me->content_location, location ? HTStrip(location) : "");
	    return me->content_location;
	}
    }
    return NULL;
}

/*
**	Expand the location relative to the base URL if any, otherwise the 
**	anchor address it self
*/
PUBLIC BOOL HTAnchor_setLocation (HTParentAnchor * me, char * location)
{
    if (me && location) {
	char * base = HTAnchor_base(me);
	if (!base) base = me->address;
	me->content_location = HTParse(location, base, PARSE_ALL);
	return YES;
    }
    return NO;
}

/*	Meta tags
**	---------
*/
PUBLIC HTAssocList * HTAnchor_meta (HTParentAnchor * me)
{
    return me ? me->meta_tags : NULL;
}

PUBLIC BOOL HTAnchor_addMeta (HTParentAnchor * me,
			      const char * name, const char * value)
{
    if (me) {
	if (!me->meta_tags) me->meta_tags = HTAssocList_new();
	return HTAssocList_replaceObject(me->meta_tags, name, value);
    }
    return NO;
}

/*
**	robots meta tag
*/
PUBLIC char * HTAnchor_robots (HTParentAnchor * me)
{
    if (me && me->meta_tags) {
	char * robots = HTAssocList_findObject(me->meta_tags, "robots");
	return robots;
    }
    return NULL;
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

PUBLIC BOOL HTAnchor_addEncoding (HTParentAnchor * me, HTEncoding encoding)
{
    if (me && encoding) {
	if (!me->content_encoding) me->content_encoding = HTList_new();
	return HTList_addObject(me->content_encoding, encoding);
    }
    return NO;
}

PUBLIC BOOL HTAnchor_deleteEncoding (HTParentAnchor * me, HTEncoding encoding)
{
    return (me && me->content_encoding && encoding) ?
	HTList_removeObject(me->content_encoding, encoding) : NO;
}

PUBLIC BOOL HTAnchor_deleteEncodingAll (HTParentAnchor * me)
{
    if (me && me->content_encoding) {
	HTList_delete(me->content_encoding);
	me->content_encoding = NULL;
	return YES;
    }
    return NO;
}

/*
**	Content Language
*/
PUBLIC HTList * HTAnchor_language (HTParentAnchor * me)
{
    if (me) {
	if (me->content_language == NULL && me->headers) {
	    char * value = HTAssocList_findObject(me->headers, "content-language");
	    char * field;
	    if (!me->content_language) me->content_language = HTList_new();
	    while ((field = HTNextField(&value)) != NULL) {
		char * lc = field;
		while ((*lc = TOLOWER(*lc))) lc++;
		HTList_addObject(me->content_language, HTAtom_for(field));
	    }
	}
	return me->content_language;
    }
    return NULL;
}

PUBLIC BOOL HTAnchor_addLanguage (HTParentAnchor * me, HTLanguage language)
{
    if (me && language) {
	if (!me->content_language) me->content_language = HTList_new();
	return HTList_addObject(me->content_language, language);
    }
    return NO;
}

PUBLIC BOOL HTAnchor_deleteLanguageAll (HTParentAnchor * me)
{
    if (me && me->content_language) {
	HTList_delete(me->content_language);
	me->content_language = NULL;
	return YES;
    }
    return NO;
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
**	Content Transfer Encoding
*/
PUBLIC HTEncoding HTAnchor_contentTransferEncoding (HTParentAnchor * me)
{
    return me ? me->cte : NULL;
}

PUBLIC void HTAnchor_setContentTransferEncoding (HTParentAnchor * me, HTEncoding cte)
{
    if (me) me->cte = cte;
}

/*
**	Allowed methods	(Allow)
*/
PUBLIC HTMethod HTAnchor_allow (HTParentAnchor * me)
{
    if (me) {
	if (me->allow == 0 && me->headers) {
	    char * value = HTAssocList_findObject(me->headers, "allow");
	    char * field;

	    /*
	    **  We treat methods allowed on this object as case insensitive
	    **  in case we receive the information over the net - that is -
	    **  in the Allow header.
	    */
	    while ((field = HTNextField(&value)) != NULL) {
		HTMethod new_method;
		if ((new_method = HTMethod_enum(field)) != METHOD_INVALID)
		    me->allow |= new_method;
	    }
	}
	return me->allow;
    }	
    return METHOD_INVALID;
}

PUBLIC void HTAnchor_setAllow (HTParentAnchor * me, HTMethod methodset)
{
    if (me) me->allow = methodset;
}

PUBLIC void HTAnchor_appendAllow (HTParentAnchor * me, HTMethod methodset)
{
    if (me) me->allow |= methodset;
}

/*
**	Title
*/
PUBLIC const char * HTAnchor_title  (HTParentAnchor * me)
{
    if (me) {
	if (me->title)
	    return *me->title ? me->title : NULL;
	if (me->headers) {
	    char * value = HTAssocList_findObject(me->headers, "title");
	    char * title;
	    if ((title = HTNextField(&value))) StrAllocCopy(me->title, title);
	    return me->title;
	}
    }
    return NULL;
}

PUBLIC void HTAnchor_setTitle (HTParentAnchor * me, const char * title)
{
    if (me && title) {
	char * ptr;
	StrAllocCopy(me->title, title);
	ptr = me->title;
	while (*ptr) {
	    if (isspace((int) *ptr)) *ptr = ' ';		
	    ptr++;
	}
    }
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
    if (me) {
	if (me->version)
	    return *me->version ? me->version : NULL;
	if (me->headers) {
	    char * value = HTAssocList_findObject(me->headers, "version");
	    char * version;
	    if ((version = HTNextField(&value)))
		StrAllocCopy(me->version, version);
	    return me->version;
	}
    }
    return NULL;
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
    if (me) {
	if (me->derived_from)
	    return *me->derived_from ? me->derived_from : NULL;
	if (me->headers) {
	    char * value = HTAssocList_findObject(me->headers, "derived-from");
	    char * derived_from;
	    if ((derived_from = HTNextField(&value)))
		StrAllocCopy(me->derived_from, derived_from);
	    return me->derived_from;
	}
    }
    return NULL;
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
    if (me) {
	if (me->content_md5)
	    return *me->content_md5 ? me->content_md5 : NULL;
	if (me->headers) {
	    char * value = HTAssocList_findObject(me->headers, "content-md5");
	    char * md5;
	    if ((md5 = HTNextField(&value))) StrAllocCopy(me->content_md5,md5);
	    return me->content_md5;
	}
    }
    return NULL;
}

PUBLIC BOOL HTAnchor_setMd5 (HTParentAnchor * me, const char * hash)
{
    if (me && hash) {
	StrAllocCopy(me->content_md5, hash);
	return YES;
    }
    return NO;
}

/*
**	Date
*/
PUBLIC time_t HTAnchor_date (HTParentAnchor * me)
{
    if (me) {
	if (me->date == (time_t) -1 && me->headers) {
	    char * value = HTAssocList_findObject(me->headers, "date");
	    if (value) me->date = HTParseTime(value, NULL, YES);
	}
	return me->date;
    }	
    return (time_t) -1;
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
    if (me) {
	if (me->expires == (time_t) -1 && me->headers) {
	    char * value = HTAssocList_findObject(me->headers, "expires");
	    if (value) me->expires = HTParseTime(value, NULL, YES);
	}
	return me->expires;
    }	
    return (time_t) -1;
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
    if (me) {
	if (me->last_modified == (time_t) -1 && me->headers) {
	    char * value = HTAssocList_findObject(me->headers,"last-modified");
	    if (value) me->last_modified = HTParseTime(value, NULL, YES);
	}
	return me->last_modified;
    }	
    return (time_t) -1;
}

PUBLIC void HTAnchor_setLastModified (HTParentAnchor * me, const time_t lm)
{
    if (me) me->last_modified = lm;
}

/*
**	Age
*/
PUBLIC time_t HTAnchor_age (HTParentAnchor * me)
{
    if (me) {
	if (me->age == (time_t) -1 && me->headers) {
	    char * value = HTAssocList_findObject(me->headers, "age");
	    if (value) me->age = atol(value);
	}
	return me->age;
    }	
    return (time_t) -1;
}

PUBLIC void HTAnchor_setAge (HTParentAnchor * me, const time_t age)
{
    if (me) me->age = age;
}

/*
**	Entity Tag
*/
PUBLIC char * HTAnchor_etag (HTParentAnchor * me)
{
    if (me) {
	if (me->etag)
	    return *me->etag ? me->etag : NULL;
	if (me->headers) {
	    char * value = HTAssocList_findObject(me->headers, "etag");
	    char * etag;
	    if ((etag = HTNextField(&value))) StrAllocCopy(me->etag, etag);
	    return me->etag;
	}
    }

    return me ? me->etag : NULL;
}

PUBLIC void HTAnchor_setEtag (HTParentAnchor * me, const char * etag)
{
  /* JK: add a new etag if it doesn't exist or if the value has changed */
    if (me && etag && ((me->etag == NULL) || strcmp (me->etag, etag)))
	StrAllocCopy(me->etag, etag);
}

PUBLIC BOOL HTAnchor_isEtagWeak (HTParentAnchor * me)
{
    return (me && me->etag && !strncasecomp(me->etag, "W/", 2));
}

/*
**	Original headers (if any)
*/
PUBLIC HTAssocList * HTAnchor_header (HTParentAnchor * me)
{
    return me ? me->headers : NULL;
}

PUBLIC BOOL HTAnchor_setHeader (HTParentAnchor * me, HTAssocList * headers)
{
    if (me) {
	me->headers = headers;
	return YES;
    }
    return NO;
}

/*
**  Validate anchor values and finish up parsing
*/
PUBLIC void HTAnchor_setHeaderParsed (HTParentAnchor * me)
{
    if (me) {
	HTTRACE(ANCH_TRACE, "HTAnchor.... Anchor is parsed\n");
	me->header_parsed = YES;
    }
}

PUBLIC BOOL HTAnchor_headerParsed (HTParentAnchor * me)
{
    return (me ? me->header_parsed : NO);
}

/*	Clear Header Information
**	------------------------
*/
PUBLIC void HTAnchor_clearHeader (HTParentAnchor * me)
{
    HTTRACE(ANCH_TRACE, "HTAnchor.... Clear all header information\n");
    me->allow = METHOD_INVALID;
    if (me->content_encoding) {
	HTList_delete(me->content_encoding);
	me->content_encoding = NULL;
    }
    if (me->content_language) {
	HTList_delete(me->content_language);
	me->content_language = NULL;
    }
    HT_FREE(me->content_base);
    HT_FREE(me->content_location);
    me->content_length = -1;					  /* Invalid */

    /* Delete the title */
    HT_FREE(me->title);

    /* Clear the content type */
    me->content_type = WWW_UNKNOWN;
    if (me->type_parameters) {
	HTAssocList_delete(me->type_parameters);
	me->type_parameters = NULL;
    }    

    /* Meta tags */
    if (me->meta_tags) {
	HTAssocList_delete(me->meta_tags);
	me->meta_tags = NULL;
    }    

    /* Dates etc. */
    me->date = (time_t) -1;
    me->expires = (time_t) -1;
    me->last_modified = (time_t) -1;
    me->age = (time_t) -1;
    
    HT_FREE(me->derived_from);
    HT_FREE(me->version);
    HT_FREE(me->etag);

    /* Delete any original headers */
    if (me->headers) HTAssocList_delete(me->headers);
    me->headers = NULL;
}
