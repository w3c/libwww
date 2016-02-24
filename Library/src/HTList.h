/*

  
  					W3C Sample Code Library libwww List Class


!
  The List Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The list class defines a generic container for storing collections of things
in order. In principle it could be implemented in many ways, but in practice
knowing that it is a linked list is important for speed.

This module is implemented by HTList.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTLIST_H
#define HTLIST_H

#include "HTArray.h"

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTList HTList;

struct _HTList {
  void * object;
  HTList * next;
};

/*
.
  Creation and Deletion Methods
.

These two functions create and deletes a list
*/

extern HTList *	HTList_new	(void);
extern BOOL	HTList_delete	(HTList *me);

/*
.
  Add an Element to List
.

A new list element is added to the beginning of the list so that it is first
element just after the head element.
*/
extern BOOL HTList_addObject (HTList *me, void *newObject);

/*

 You can also append an element to the end of
the list (the end is the first entered object) by using the following function:
*/

extern BOOL HTList_appendObject (HTList * me, void * newObject);

/*
The following two functions, contributed by Vic 
Bancroft (bancroft@america.net) that do the same operation as above, but return
a pointer to the new HTList element that was added or appended.  This allows
one to keep a  reference to the end of the list outside of the list itself, 
which can be used to speed up certain list operations.
*/

extern HTList * HTList_addList (HTList * me, void * newObject);
extern HTList * HTList_appendList (HTList * me, void * newObject);

/*
.
  Remove List Elements
.

You can delete elements in a list using the following methods. The
first method only removes the first entry that it finds matching the
oldObject whereas the second method removes all
occurances of oldObject.

*/

extern BOOL	HTList_removeObject		(HTList * me, void * oldObject);
extern BOOL	HTList_quickRemoveElement	(HTList * me, HTList * last);
extern BOOL	HTList_removeObjectAll		(HTList * me, void * oldObject);

extern void *	HTList_removeLastObject		(HTList * me);
extern void *	HTList_removeFirstObject	(HTList * me);

/*
.
  Size of a List
.

Two small function to ask for the size
*/

#define 	HTList_isEmpty(me)		(me ? me->next == NULL : YES)
extern int	HTList_count			(HTList *me);

/*
.
  Reference List Elements By Index
.

In some situations is is required to use an index in order to refer to a
list element. This is for example the case if an element can be registered
multiple times.
*/

extern int	HTList_indexOf	 (HTList * me, void * object);
extern int	HTList_indexOfElement (HTList * me, HTList * element);
extern void *	HTList_objectAt	 (HTList * me, int position);
extern HTList * HTList_elementOf (HTList * me, void * object, HTList ** pLast);
#define 	HTList_objectOf(me)		(me ? me->object: NULL)

/*
.
  Find List Elements
.

This method returns the last element to the list or NULL if list is
empty
*/

#define		HTList_lastObject(me) \
		((me) && (me)->next ? (me)->next->object : NULL)

/*

This method returns the first element to the list or NULL if list
is empty
*/
extern void * HTList_firstObject  (HTList * me);

/*
.
  Traverse list
.

Fast macro to traverse the list. Call it first with copy of list header:
it returns the first object and increments the passed list pointer. Call
it with the same variable until it returns NULL.
*/

#define		HTList_nextObject(me) \
		((me) && ((me) = (me)->next) ? (me)->object : NULL)

/*
.
  Insertion Sort a List
.

This function sorts a list using the insertion sort mechanism. The comparison
function is passed as a parameter and you can &nbsp;find the definition of
HTComparer in the HTArray module.
Insertion sort is good method whenever a list is nearly in the correct order
and few items are many positions away from their sorted location. If the
list gets very long then you may wanna use a quicksort instead.
*/
extern BOOL HTList_insertionSort(HTList * list, HTComparer * comp);

/*
.
  Free list
.
*/

#define HTList_free(x)  HT_FREE(x)

#ifdef __cplusplus
}
#endif

#endif /* HTLIST_H */

/*

  

  @(#) $Id$

*/
