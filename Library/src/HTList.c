/*	A small List class					      HTList.c
**	==================
**
**	A list is represented as a sequence of linked nodes of type HTList.
**	The first node is a header which contains no object.
**	New nodes are inserted between the header and the rest of the list.
*/

#include "HTList.h"

#include <stdio.h>				/* joe@athena, TBL 921019 */

HTList * HTList_new NOARGS
{
  HTList *newList = (HTList *)malloc (sizeof (HTList));
  if (newList == NULL) outofmem(__FILE__, "HTList_new");
  newList->object = NULL;
  newList->next = NULL;
  return newList;
}

void HTList_delete ARGS1(HTList *,this)
{
  HTList *current;
  while (current = this) {
    this = this->next;
    free (current);
  }
}

void HTList_addObject ARGS2(HTList *,this, void *,newObject)
{
  if (this) {
    HTList *newNode = (HTList *)malloc (sizeof (HTList));
    if (newNode == NULL) outofmem(__FILE__, "HTList_addObject");
    newNode->object = newObject;
    newNode->next = this->next;
    this->next = newNode;
  }
  else
    if (TRACE) fprintf(stderr,
        "HTList: Trying to add object %p to a nonexisting list\n",
		       newObject);
}

BOOL HTList_removeObject ARGS2(HTList *,this, void *,oldObject)
{
  if (this) {
    HTList *previous;
    while (this->next) {
      previous = this;
      this = this->next;
      if (this->object == oldObject) {
	previous->next = this->next;
	free (this);
	return YES;  /* Success */
      }
    }
  }
  return NO;  /* object not found or NULL list */
}

void * HTList_removeLastObject ARGS1 (HTList *,this)
{
  if (this && this->next) {
    HTList *lastNode = this->next;
    void * lastObject = lastNode->object;
    this->next = lastNode->next;
    free (lastNode);
    return lastObject;
  } else  /* Empty list */
    return NULL;
}

void * HTList_removeFirstObject ARGS1 (HTList *,this)
{
  if (this && this->next) {
    HTList * prevNode;
    void *firstObject;
    while (this->next) {
      prevNode = this;
      this = this->next;
    }
    firstObject = this->object;
    prevNode->next = NULL;
    free (this);
    return firstObject;
  } else  /* Empty list */
    return NULL;
}

int HTList_count ARGS1 (HTList *,this)
{
  int count = 0;
  if (this)
    while (this = this->next)
      count++;
  return count;
}

int HTList_indexOf ARGS2(HTList *,this, void *,object)
{
  if (this) {
    int position = 0;
    while (this = this->next) {
      if (this->object == object)
	return position;
      position++;
    }
  }
  return -1;  /* Object not in the list */
}

void * HTList_objectAt ARGS2 (HTList *,this, int,position)
{
  if (position < 0)
    return NULL;
  if (this) {
    while (this = this->next) {
      if (position == 0)
	return this->object;
      position--;
    }
  }
  return NULL;  /* Reached the end of the list */
}
