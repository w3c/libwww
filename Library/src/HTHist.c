/*								    HTHist.c
**	NAVIGATION MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	A simple linear history list manager. The manager can only operate
**	with _one_ linear list. A tree structure would be nice!
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTAnchor.h"
#include "HTHist.h"					 /* Implemented here */

PRIVATE HTList * HTHistory = NULL;		  /* List of visited anchors */

/* ------------------------------------------------------------------------- */

/*	Record an entry in the list
**	---------------------------
**      Registers the object in the linear list. The first entry is the
**	home page. No check is done for duplicates.
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTHistory_record ARGS1(HTAnchor *, destination)
{
    if (destination) {
	if (!HTHistory)
	    HTHistory = HTList_new();
	return HTList_addObject(HTHistory, destination);	
    }
    return NO;
}

/*	Delete an entry
**	---------------
**      Deletes the object from the list
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTHistory_delete ARGS1(HTAnchor *, old)
{
    return (HTHistory ? HTList_removeObject(HTHistory, old) : NO);
}

/*	Clear the History list
**	----------------------
**      Deletes the history list FROM the last entered occurance of the
**	current entry and forward.
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTHistory_clearFrom ARGS1 (HTAnchor *, cur)
{    
    if (HTHistory && cur) {
	while (!HTList_isEmpty(HTHistory) && HTList_lastObject(HTHistory)!=cur)
	    HTList_removeLastObject(HTHistory);
	return YES;
    }
    return NO;
}

/*	Clear the History list
**	----------------------
**      Deletes the history list FROM the entry at position 'cur'. Home page
**	has position 1.
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTHistory_clearFromPos ARGS1 (int, ind)
{    
    if (HTHistory && ind>=0) {
	HTAnchor *cur = (HTAnchor *)
	    (HTList_objectAt(HTHistory, HTList_count(HTHistory) - ind));
	while (!HTList_isEmpty(HTHistory) && HTList_lastObject(HTHistory)!=cur)
	    HTList_removeLastObject(HTHistory);
	return YES;
    }
    return NO;
}

/*	Delete the whole list
**	---------------------
**      Deletes the whole list from memory, so a new list can be started
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTHistory_clearAll NOARGS
{
    if (HTHistory) {
	HTList_delete(HTHistory);
	HTHistory = NULL;
	return YES;
    }
    return NO;
}

/*	Number of elements stored
**     	-------------------------
**	Returns the size of the history list or -1 if none.
*/
PUBLIC int HTHistory_count NOARGS
{
    return (HTHistory ? HTList_count(HTHistory) : -1);
}

/* ------------------------------------------------------------------------- */
/* 				    NAVIGATION				     */
/* ------------------------------------------------------------------------- */

/*	Find and re-register visited anchor
**     	-----------------------------------
**	Finds already registered anchor with given index and registers it
**	again EXCEPT if last entry. This allows for `circular' history lists
**	with duplicate entries.
*/
PUBLIC HTAnchor * HTHistory_recall ARGS1(int, cnt)
{
    HTAnchor *cur = NULL;
    if (HTHistory) {
	cur=(HTAnchor*)HTList_objectAt(HTHistory, HTList_count(HTHistory)-cnt);
	if (cur && cur != HTList_lastObject (HTHistory))
	    HTList_addObject (HTHistory, cur);
    }
    return cur;
}

/*	Find Indexed entry
**	------------------
**	Entry with the given index in the list (1 is the home page). Like
**	HTHistory_recall but without re-registration
*/
PUBLIC HTAnchor * HTHistory_findPos ARGS1(int, n)
{
    return (HTHistory ?
	    (HTAnchor*)(HTList_objectAt(HTHistory,HTList_count(HTHistory)-n)) :
	    NULL);
}

/*	Can we back in history
**	----------------------
**	Returns YES if the current anchor is not the first entry (home page)
*/
PUBLIC BOOL HTHistory_canBacktrack ARGS1(HTAnchor *, cur)
{
    if (HTHistory)
	return (HTList_indexOf(HTHistory, cur) < HTList_count(HTHistory)-1);
    return NO;
}

/*
**	Backtrack with deletion
**	-----------------------
**	Returns the previous object and erases the last object. This does not
**	allow for 'forward' as we are always at the end of the list. If no
**	previous object exists, NULL is returned so that the application knows
**	that no previous object was found. See also HTHistory_back(). 
*/
PUBLIC HTAnchor * HTHistory_backtrack NOARGS
{
    if (HTHistory && HTList_count(HTHistory) > 1) {
	HTList_removeLastObject (HTHistory);
	return (HTAnchor *) HTList_lastObject (HTHistory);
    }
    return NULL;
}

/*
**	Backtrack without deletion
**	--------------------------
**	Returns the previos object but does not erase the last object. This
**	does not allow for 'forward'. If no previous object exists,  NULL is
**	returned so that the application knows that no previous object was
**	found. See also HTHistory_backtrack()
*/
PUBLIC HTAnchor * HTHistory_back ARGS1(HTAnchor *, cur)
{
    if (HTHistory && HTHistory_canBacktrack(cur)) {
	int pos = HTList_indexOf(HTHistory, cur);
	return ((HTAnchor *) HTList_objectAt(HTHistory, pos + 1));
    }
    return NULL;
}

/*	Can we go forward
**	-----------------
**	Returns YES if the current anchor is not the last entry
*/
PUBLIC BOOL HTHistory_canForward ARGS1(HTAnchor *, cur)
{
    return (HTHistory && cur && (cur != HTList_lastObject(HTHistory)));
}

/*
**	Forward
**	-------
**	Return the next object in the list or NULL if none
*/
PUBLIC HTAnchor * HTHistory_forward ARGS1(HTAnchor *, cur)
{
    if (HTHistory && HTHistory_canForward(cur)) {
	int pos = HTList_indexOf(HTHistory, cur);
	return ((HTAnchor *) HTList_objectAt(HTHistory, pos - 1));
    }
    return NULL;
}

/*
**	Can Move By Offset
**	------------------
**	Support for Next and Previous in order to allow for browsing through
**	the same parent node
*/
PUBLIC BOOL HTHistory_canMoveBy ARGS1(int, offset)
{
    HTAnchor *last = (HTAnchor *) HTList_lastObject(HTHistory);

    /* If we have a last entry and this is a child */
    if (last && last != (HTAnchor *) last->parent) {
	HTList *kids = last->parent->children;
	int cur = HTList_indexOf(kids, last); 
	return (HTList_objectAt(kids, cur-(offset)) != NULL);
    }
    return NO;
}

/*	Browse through references in the same parent node
**	-------------------------------------------------
**	Take the n-th child's link after or before the one we took to get here.
**	Positive offset means go towards most recently added children or "Next"
**	and negative value means "Previous"
*/
PUBLIC HTAnchor * HTHistory_moveBy ARGS1 (int, offset)
{
    HTAnchor *last = (HTAnchor *) HTList_lastObject(HTHistory);

    /* If we have a last entry and this is a child */
    if (last && last != (HTAnchor *) last->parent) {
	HTList *kids = last->parent->children;
	int cur = HTList_indexOf(kids, last); 
	HTAnchor *nextOne = (HTAnchor *) HTList_objectAt (kids, cur-(offset));
	if (nextOne) {
	    HTAnchor * destination = HTAnchor_followMainLink(nextOne);
	    if (destination) {
		HTList_removeLastObject(HTHistory);
		HTList_removeLastObject (HTHistory);
		HTList_addObject (HTHistory, nextOne);
		HTList_addObject (HTHistory, destination);
	    }
	    return destination;
	} else {
	    if (TRACE)
		fprintf(TDEST, 
			"HTHistory... Index out of range %d of list %p.\n",
			offset, (void*)kids);
	}
    }
    return NULL;
}

/*	Change last history entry
**	-------------------------
**	Sometimes we load a node by one anchor but leave by a different
**	one, and it is the one we left from which we want to remember.
*/
PUBLIC void HTHistory_leavingFrom ARGS1 (HTAnchor *, anchor)
{
    if (HTHistory) {
	if (HTList_removeLastObject (HTHistory))
	    HTList_addObject (HTHistory, anchor);
    }
}
