/*								    HTHist.c
**	NAVIGATION MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	The history manager for the Library. This module is not called any
**	where in the Library so if the application does not call it, it is
**	not linked in at all.
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTAnchor.h"
#include "HTHist.h"					 /* Implemented here */

struct _HTHistory {
    HTList *	alist;					  /* List of anchors */
    int		pos;	       /* Current position in list. 1 is home anchor */
};

/* ------------------------------------------------------------------------- */

/*	Create a new History List
**	-------------------------
**      Creates a new history list and returns a handle to it. There can be 
**	multiple history lists - for example one for each open window in
**	an application.
**	Returns HTHistory object if OK, else NULL
*/
PUBLIC HTHistory *HTHistory_new (void)
{
    HTHistory *element;
    if ((element = (HTHistory  *) HT_CALLOC(1, (sizeof(HTHistory)))) == NULL)
        HT_OUTOFMEM("HTHistory_new");
    element->alist = HTList_new();
    return element;
}

/*	Delete a History list
**	---------------------
**      Deletes the history list.
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTHistory_delete (HTHistory * hist)
{
    if (hist) {
	HTList_delete(hist->alist);
	HT_FREE(hist);
	return YES;
    }
    return NO;
}

/*	Record an entry in a list
**	-------------------------
**      Registers the object in the linear list. The first entry is the
**	home page. No check is done for duplicates.
**	Returns YES if ok, else NO
*/
PUBLIC BOOL HTHistory_record (HTHistory * hist, HTAnchor * cur)
{
    return (hist && cur && HTList_addObject(hist->alist, cur) && hist->pos++);
}

/*	Replace list with new element
**	-----------------------------
**      Inserts the new element at the current position and removes all any
**	old list from current position. For example if c is cur pos
**		before: a b c d e
**		after : a b f
**	Returns YES if ok, else NO
*/
PUBLIC BOOL HTHistory_replace (HTHistory * hist, HTAnchor * cur)
{
    if (hist && cur) {
	HTHistory_removeFrom(hist, hist->pos);
	HTHistory_record(hist, cur);
    }
    return NO;
}

/*	Delete last entry in a list
**	---------------------------
**      Deletes the last object from the list
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTHistory_removeLast (HTHistory * hist)
{
    return (hist && HTList_removeLastObject(hist->alist) && hist->pos--);
}

/*	Remove the History list from position
**	------------------------------------
**      Deletes the history list FROM the entry at position 'cur' (excluded).
**	Home page has position 1.
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTHistory_removeFrom  (HTHistory * hist, int pos)
{
    if (hist && pos>=0) {
	int cnt = HTList_count(hist->alist) - pos;
	while (cnt-->0 && HTList_removeLastObject(hist->alist));
	if (hist->pos > pos)
	    hist->pos = pos;
	return YES;
    }
    return NO;
}

/*	Number of elements stored
**     	-------------------------
**	Returns the size of the history list or -1 if none.
*/
PUBLIC int HTHistory_count (HTHistory * hist)
{
    return (hist ? HTList_count(hist->alist) : -1);
}

/*	Current Location
**     	----------------
**	Returns the current position or -1
*/
PUBLIC int HTHistory_position (HTHistory * hist)
{
    return (hist ? hist->pos : -1);
}

/* ------------------------------------------------------------------------- */
/* 				    NAVIGATION				     */
/* ------------------------------------------------------------------------- */

/*	Find and re-register visited anchor
**     	-----------------------------------
**	Finds already registered anchor at given position and registers it
**	again EXCEPT if last entry. This allows for `circular' history lists
**	with duplicate entries. Position 1 is the home anchor.
*/
PUBLIC HTAnchor * HTHistory_recall (HTHistory * hist, int pos)
{
    HTAnchor *cur = NULL;
    if (hist && pos > 0) {
	int len = HTList_count(hist->alist);
	if ((cur = (HTAnchor *) HTList_objectAt(hist->alist, len-pos))) {
	    if (cur != HTList_lastObject (hist->alist)) {
		HTHistory_record(hist, cur);
	    } else
		hist->pos = pos;
	}
    }
    return cur;
}

/*	Find Entry at position
**	----------------------
**	Entry with the given index in the list (1 is the home page). Like
**	HTHistory_recall but without re-registration. Current position is
**	updated.
*/
PUBLIC HTAnchor * HTHistory_find (HTHistory * hist, int pos)
{
    HTAnchor *cur = NULL;
    if (hist && pos > 0) {
	if ((cur = (HTAnchor *)
	     (HTList_objectAt(hist->alist, HTList_count(hist->alist)-pos))))
	    hist->pos = pos;
    }
    return cur;
}

/*	List Entry at position
**	----------------------
**	Entry with the given index in the list (1 is the home page). Like
**	HTHistory_find but current position is NOT updated.
*/
PUBLIC HTAnchor * HTHistory_list (HTHistory * hist, int pos)
{
    return (hist ? (HTAnchor *)
	    (HTList_objectAt(hist->alist, HTList_count(hist->alist)-pos)) :
	    NULL);
}

/*	Can we back in history
**	----------------------
**	Returns YES if the current anchor is not the first entry (home page)
*/
PUBLIC BOOL HTHistory_canBacktrack (HTHistory * hist)
{
    return ((hist && hist->pos > 1) ? YES : NO);
}

/*
**	Backtrack with deletion
**	-----------------------
**	Returns the previous object and erases the last object. This does not
**	allow for 'forward' as we are always at the end of the list. If no
**	previous object exists, NULL is returned so that the application knows
**	that no previous object was found. See also HTHistory_back(). 
*/
PUBLIC HTAnchor * HTHistory_backtrack (HTHistory * hist)
{
    if (HTHistory_canBacktrack(hist)) {
	HTHistory_removeLast(hist);
	return (HTAnchor *) HTList_lastObject(hist->alist);
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
PUBLIC HTAnchor * HTHistory_back (HTHistory * hist)
{
    if (HTHistory_canBacktrack(hist)) {	
	int pos = HTList_count(hist->alist) - (--hist->pos);
	return ((HTAnchor *) HTList_objectAt(hist->alist, pos));
    }
    return NULL;
}

/*	Can we go forward
**	-----------------
**	Returns YES if the current anchor is not the last entry
*/
PUBLIC BOOL HTHistory_canForward (HTHistory * hist)
{
    return ((hist && hist->pos < HTList_count(hist->alist)) ? YES : NO);
}

/*
**	Forward
**	-------
**	Return the next object in the list or NULL if none
*/
PUBLIC HTAnchor * HTHistory_forward (HTHistory * hist)
{
    if (HTHistory_canForward(hist)) {
	int pos = HTList_count(hist->alist) - (++hist->pos);
	return ((HTAnchor *) HTList_objectAt(hist->alist, pos));
    }
    return NULL;
}
