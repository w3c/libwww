/*								       HTList.c
**	MANAGEMENT OF LINKED LISTS
**
**	(c) COPYRIGHT CERN 1994.
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

HTList * HTList_new NOARGS
{
  HTList *newList = (HTList *)malloc (sizeof (HTList));
  if (newList == NULL) outofmem(__FILE__, "HTList_new");
  newList->object = NULL;
  newList->next = NULL;
  return newList;
}

void HTList_delete ARGS1(HTList *,me)
{
  HTList *current;
  while ((current = me)) {
    me = me->next;
    free (current);
  }
}

void HTList_addObject ARGS2(HTList *,me, void *,newObject)
{
  if (me) {
    HTList *newNode = (HTList *)malloc (sizeof (HTList));
    if (newNode == NULL) outofmem(__FILE__, "HTList_addObject");
    newNode->object = newObject;
    newNode->next = me->next;
    me->next = newNode;
  }
  else {
    if (TRACE) fprintf(TDEST,
        "HTList: Trying to add object %p to a nonexisting list\n",
		       newObject);
    abort();
  }
}

BOOL HTList_removeObject ARGS2(HTList *,me, void *,oldObject)
{
  if (me) {
    HTList *previous;
    while (me->next) {
      previous = me;
      me = me->next;
      if (me->object == oldObject) {   /* HWL: sigbus error */
	previous->next = me->next;
	free (me);
	return YES;  /* Success */
      }
    }
  }
  return NO;  /* object not found or NULL list */
}

void * HTList_removeLastObject ARGS1 (HTList *,me)
{
  if (me && me->next) {
    HTList *lastNode = me->next;
    void * lastObject = lastNode->object;
    me->next = lastNode->next;
    free (lastNode);
    return lastObject;
  } else  /* Empty list */
    return NULL;
}

void * HTList_removeFirstObject ARGS1 (HTList *,me)
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
    free (me);
    return firstObject;
  } else  /* Empty list */
    return NULL;
}

int HTList_count ARGS1 (HTList *,me)
{
  int count = 0;
  if (me)
    while ((me = me->next))
      count++;
  return count;
}

int HTList_indexOf ARGS2(HTList *,me, void *,object)
{
  if (me) {
    int position = 0;
    while ((me = me->next)) {
      if (me->object == object)
	return position;
      position++;
    }
  }
  return -1;  /* Object not in the list */
}

void * HTList_objectAt ARGS2 (HTList *,me, int,position)
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
  return NULL;  /* Reached the end of the list */
}


void * HTList_removeObjectAt ARGS2 (HTList *,me, int,position)
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

