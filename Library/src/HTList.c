/*								       HTList.c
**	MANAGEMENT OF LINKED LISTS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	A list is represented as a sequence of linked nodes of type HTList.
**	The first node is a header which contains no object.
**	New nodes are inserted between the header and the rest of the list.
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTList.h"

PUBLIC HTList * HTList_new (void)
{
    HTList *newList;
    if ((newList = (HTList  *) HT_CALLOC(1, sizeof (HTList))) == NULL)
        HT_OUTOFMEM("HTList_new");
    newList->object = NULL;
    newList->next = NULL;
    return newList;
}

PUBLIC BOOL HTList_delete (HTList * me)
{
    if (me) {
	HTList *current;
	while ((current = me)) {
	    me = me->next;
	    HT_FREE(current);
	}
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTList_addObject (HTList * me, void * newObject)
{
    if (me) {
	HTList *newNode;
	if ((newNode = (HTList  *) HT_CALLOC(1, sizeof(HTList))) == NULL)
	    HT_OUTOFMEM("HTList_addObject");
	newNode->object = newObject;
	newNode->next = me->next;
	me->next = newNode;
	return YES;
    } else {
	if (WWWTRACE)
	    TTYPrint(TDEST,
		    "HTList...... Can not add object %p to nonexisting list\n",
		    newObject);
    }
    return NO;
}

PUBLIC BOOL HTList_appendObject (HTList * me, void * newObject)
{
    if (me) {
	while (me->next) me = me->next;
	return HTList_addObject(me, newObject);
    }
    return NO;
}

PUBLIC BOOL HTList_removeObject (HTList *  me, void *  oldObject)
{
    if (me) {
	HTList *previous;
	while (me->next) {
	    previous = me;
	    me = me->next;
	    if (me->object == oldObject) {
		previous->next = me->next;
		HT_FREE(me);
		return YES;	/* Success */
	    }
	}
    }
    return NO;			/* object not found or NULL list */
}

PUBLIC void * HTList_removeLastObject  (HTList * me)
{
    if (me && me->next) {
	HTList *lastNode = me->next;
	void * lastObject = lastNode->object;
	me->next = lastNode->next;
	HT_FREE(lastNode);
	return lastObject;
    } else			/* Empty list */
	return NULL;
}

PUBLIC void * HTList_removeFirstObject  (HTList * me)
{
    if (me && me->next) {
	HTList * prevNode;
	void *firstObject;
	while (me->next) {
	    prevNode = me;
	    me = me->next;
	}
	firstObject = me->object;
	prevNode->next = NULL;
	HT_FREE(me);
	return firstObject;
    } else			/* Empty list */
	return NULL;
}

PUBLIC int HTList_count  (HTList * me)
{
    int count = 0;
    if (me)
	while ((me = me->next))
	    count++;
    return count;
}

PUBLIC int HTList_indexOf (HTList * me, void * object)
{
    if (me) {
	int position = 0;
	while ((me = me->next)) {
	    if (me->object == object)
		return position;
	    position++;
	}
    }
    return -1;
}

PUBLIC void * HTList_objectAt  (HTList * me, int position)
{
    if (position < 0)
	return NULL;
    if (me) {
	while ((me = me->next)) {
	    if (position == 0)
		return me->object;
	    position--;
	}
    }
    return NULL;		/* Reached the end of the list */
}

PUBLIC void * HTList_removeObjectAt  (HTList * me, int position)
{
    if (position < 0)
	return NULL;
    if (me) {
	HTList * prevNode;
	prevNode = me;
	while ((me = me->next)) {
	    if (position == 0) {
		prevNode->next = me->next;
		return me->object;
	    }
	    prevNode = me;
	    position--;
	}
    }
    return NULL;  /* Reached the end of the list */
}
