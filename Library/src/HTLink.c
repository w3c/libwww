/*
**	HYPERTEXT LINK CLASS
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
**		    HF - moved all Link stuff to HTLink
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTFormat.h"
#include "HTParse.h"
#include "HTMethod.h"
#include "HTAncMan.h"
#include "HTLink.h"					 /* Implemented here */

/* ------------------------------------------------------------------------- */

PUBLIC HTLink * HTLink_new (void)
{
    HTLink * link;
    if ((link = (HTLink *) HT_CALLOC(1, sizeof (HTLink))) == NULL)
        HT_OUTOFMEM("HTLink_new");
    return link;
}

PUBLIC BOOL HTLink_delete (HTLink * me)
{
    HT_FREE(me);
    return YES;
}

/*	Link me Anchor to another given one
**	-------------------------------------
*/
PUBLIC BOOL HTLink_add (HTAnchor *	source,
			HTAnchor * 	destination, 
			HTLinkType	type,
			HTMethod	method)
{
    if (source && destination) {
	HTTRACE(ANCH_TRACE, "Link create. from anchor %p to %p with type %s, method %s\n" _ 
		    (void *) source _ (void *) destination _ 
		    type ? HTAtom_name(type) : "NONE" _ 
		    method != METHOD_INVALID ? HTMethod_name(method) : "NONE");
	if (!source->mainLink.dest) {
	    source->mainLink.dest = destination;
	    source->mainLink.type = type;
	    source->mainLink.method = method;
	} else {
	    HTLink * newLink = HTLink_new();
	    newLink->dest = destination;
	    newLink->type = type;
	    newLink->method = method;
	    if (!source->links) source->links = HTList_new();
	    HTList_addObject (source->links, newLink);
	}
	if (!destination->parent->sources)
	    destination->parent->sources = HTList_new();
	HTList_addObject (destination->parent->sources, source);
	return YES;
    } else
	HTTRACE(ANCH_TRACE, "Link........ Bad argument\n");
    return NO;
}

/*
**  Removes link information from one anchor to another.
**  Returns YES if OK, else NO
*/
PUBLIC BOOL HTLink_remove (HTAnchor * source, HTAnchor * destination)
{
    if (!source || !destination) return NO;
    HTTRACE(ANCH_TRACE, "Link delete. from anchor %p to anchor %p\n" _ 
		(void *) source _ (void *) destination);

    /* Remove if dest is the main link */
    if (source->mainLink.dest == destination) {
	source->mainLink.dest = NULL;
	source->mainLink.type = NULL;
	source->mainLink.method = METHOD_INVALID;
	source->mainLink.result = HT_LINK_INVALID;
	return YES;
    }

    /* Remove link information for other links */
    if (source->links) {
	HTList *cur = source->links;
	HTLink *pres;
	while ((pres = (HTLink *) HTList_nextObject(cur))) {
	    if (pres->dest == destination) {
		HTList_removeObject(source->links, pres);
		HT_FREE(pres);
		return YES;
	    }
	}
    }
    return NO;
}

/*
**  Removes all link information
**  Returns YES if OK, else NO
*/
PUBLIC BOOL HTLink_removeAll (HTAnchor * me)
{
    if (!me) return NO;
    HTTRACE(ANCH_TRACE, "Link delete. from anchor %p\n" _ (void *) me);

    /* Remove if dest is the main link */
    me->mainLink.dest = NULL;
    me->mainLink.type = NULL;
    me->mainLink.method = METHOD_INVALID;
    me->mainLink.result = HT_LINK_INVALID;

    /* Remove link information for other links */
    if (me->links) {
	HTList *cur = me->links;
	HTLink *pres;
	while ((pres = (HTLink *) HTList_nextObject(cur)))
	    HT_FREE(pres);
	HTList_delete(me->links);
	me->links = NULL;
    }
    return YES;
}

/*
**  Moves all link information from one anchor to another.
**  This is used in redirection etc.
**  Returns YES if OK, else NO
*/
PUBLIC BOOL HTLink_moveAll (HTAnchor * src, HTAnchor * dest)
{
    if (!src || !dest) return NO;
    HTTRACE(ANCH_TRACE, "Link move... all from anchor %p to anchor %p\n" _ 
		(void *) src _ (void *) dest);

    /* Move main link information */
    dest->mainLink.dest = src->mainLink.dest;
    dest->mainLink.type = src->mainLink.type;
    dest->mainLink.method = src->mainLink.method;
    dest->mainLink.result = src->mainLink.result;

    src->mainLink.dest = NULL;
    src->mainLink.type = NULL;
    src->mainLink.method = METHOD_INVALID;
    src->mainLink.result = HT_LINK_INVALID;

    /* Move link information for other links */
    if (dest->links) {
	HTList *cur = dest->links;
	HTLink *pres;
	while ((pres = (HTLink *) HTList_nextObject(cur)))
	    HT_FREE(pres);
	HTList_delete(dest->links);
    }
    dest->links = src->links;
    src->links = NULL;
    return YES;
}

/*
**  Find the anchor object between a destination and a source ancher.
**  Return link object if any, else NULL
*/
PUBLIC HTLink * HTLink_find (HTAnchor * src, HTAnchor * dest)
{
    if (src && dest) {
	if (src->mainLink.dest == dest)
	    return &(src->mainLink);
	if (src->links) {
	    HTList *cur = src->links;
	    HTLink *pres;
	    while ((pres = (HTLink *) HTList_nextObject(cur)) != NULL) {
		if (pres->dest == dest)
		    return pres;
	    }
	}
    }
    return NULL;
}

/*
**  Returns a link with a given link type or NULL if nothing found
*/
PUBLIC HTLink * HTLink_findType (HTAnchor * me, HTLinkType type)
{
    if (me) {
	HTLink * link = HTAnchor_mainLink(me);
	HTList * sublinks = HTAnchor_subLinks(me);
	if (link && link->type==type)
	    return link;
	else if (sublinks) {
	    while ((link = (HTLink *) HTList_nextObject (sublinks)))
		if (link->type == type) return link;
	}
    }
    return NULL;
}

/*
**	Link destinations
*/
PUBLIC BOOL HTLink_setDestination (HTLink * link, HTAnchor * dest)
{
    if (link) {
	link->dest = dest;
	return YES;
    }
    return NO;
}

PUBLIC HTAnchor * HTLink_destination (HTLink * link)
{
    return link ? link->dest : NULL;
}

PUBLIC BOOL HTLink_setType (HTLink * link, HTLinkType type)
{
    if (link) {
	link->type = type;
	return YES;
    }
    return NO;
}

PUBLIC HTLinkType HTLink_type (HTLink * link)
{
    return link ? link->type : NULL;
}

/* 
**  When a link has been used for posting an object from a source to a
**  destination link, the result of the operation is stored as part of the
**  link information.
*/
PUBLIC BOOL HTLink_setResult (HTLink * link, HTLinkResult result)
{
    if (link) {
	link->result = result;
	return YES;
    }
    return NO;
}

PUBLIC HTLinkResult HTLink_result (HTLink * link)
{
    return link ? link->result : HT_LINK_INVALID;
}

PUBLIC BOOL HTLink_setMethod (HTLink * link, HTMethod method)
{
    if (link) {
	link->method = method;
	return YES;
    }
    return NO;
}

PUBLIC HTMethod HTLink_method (HTLink * link)
{
    return link ? link->method : METHOD_INVALID;
}
