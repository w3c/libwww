/*								       HTList.c
**	MANAGEMENT OF LINKED LISTS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	A list is represented as a sequence of linked nodes of type HTList.
**	The first node is a header which contains no object.
**	New nodes are inserted between the header and the rest of the list.
*/

/* Library include files */
#include "wwwsys.h"
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
	HTTRACE(CORE_TRACE, "HTList...... Can not add object %p to nonexisting list\n" _ 
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

PUBLIC BOOL HTList_removeObject (HTList * me, void * oldObject)
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

PUBLIC HTList * HTList_addList (HTList * me, void * newObject)
{
    if (me) {
	HTList *newNode;
	if ((newNode = (HTList  *) HT_CALLOC(1, sizeof(HTList))) == NULL)
	    HT_OUTOFMEM("HTList_addObject");
	newNode->object = newObject;
	newNode->next = me->next;
	me->next = newNode;
	return newNode;
    } else {
	HTTRACE(CORE_TRACE, "HTList...... Can not add object %p to nonexisting List\n" _ newObject);
    }
    return (HTList *) NULL;
}

PUBLIC HTList * HTList_appendList (HTList * me, void *newObject)
{
    if (me) {
	while (me->next) 
	    me = me->next;
	return HTList_addList(me, newObject);
    }
    return (HTList *) NULL;
}


PUBLIC BOOL HTList_quickRemoveElement (HTList * me, HTList * last)
{
    if (me && last) {
	last->next = me->next;
	HT_FREE(me);
	return YES;	/* Success */
    }
    return NO;			/* object not found or NULL list */
}

PUBLIC BOOL HTList_removeObjectAll (HTList * me, void * oldObject)
{
 BOOL found = NO;

 if (me)
   {
    HTList* i;

    while ((i = me->next))
      {
       if (i->object == oldObject)
	 {
	  me->next = i->next;
	  HT_FREE(i);
	  found = YES;	/* At least one object found */
	 }
        else
	 {
	  me = i;
	 }
      }
   }

 return found;
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

PUBLIC void * HTList_firstObject  (HTList * me)
{
    if (me && me->next) {
	HTList * prevNode;
	while (me->next) {
	    prevNode = me;
	    me = me->next;
	}
	return me->object;
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

PUBLIC int HTList_indexOfElement (HTList * me, HTList * element)
{
    if (me) {
	int position = 0;
	if (me == element)
	    return -1;
	while ((me = me->next)) {
	    if (me == element)
		return position;
	    position++;
	}
    }
    return -2;
}

PUBLIC HTList * HTList_elementOf (HTList * cur, void * object, HTList ** pLast)
{
    HTList * 	last = cur;
    void *	pres;

    while ((pres = HTList_nextObject(cur))) {
        if (pres == object) {
	    if (pLast)
		*pLast = last;
	    return cur;
	}
	last = cur;
    }

    /*
    **	didn't find object, but return end of list so it is easy to append
    */
    if (pLast)
	*pLast = last;
    return NULL;
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

/*
**  Do an insertion sort algorithm on the list. An empty list is already
**  sorted and so is a single element list.
*/
PUBLIC BOOL HTList_insertionSort (HTList * head, HTComparer * comp)
{
    HTList *tail, *q, *r, *p;
    if (head && head->next && comp) {
	tail = head->next;
	while (tail->next) {
	    q = tail->next;

	    /*
	    **  Depending on the return status of the comparer we either move
	    **  the sentinel down the list or search the sorted sublist to
	    **  insert it.
	    */
	    if (comp(q->object, head->next->object) >= 0) {
		tail->next = q->next;
		q->next = head->next;
		head->next = q;
	    } else {
		r = head->next;
		p = r->next;
		while (comp(q->object, p->object) < 0) {
		    r = p;
		    p = r->next;
		}		    

		/*
		**  If sentinel was found then q is already in right place.
		**  Otherwise we'll have to move it
		*/
		if (q == p)
		    tail = q;
		else {
		    tail->next = q->next;
		    q->next = p;
		    r->next = q;
		}
	    }
	}
	return YES;
    } else {
	HTTRACE(CORE_TRACE, "List........ Empty list or no sort algorithm\n");
    }
    return NO;
}

