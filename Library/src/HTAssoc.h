/*

  					W3C Sample Code Library libwww Association Pairs


!
  Association List For Storing Name-Value Pairs
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This Assoctiation List class is closely related to the
HTList Class as it simply is a list of a specific
list element containing a characters based name/value pair. Lookups from
association list can be case sensitive and or prefix based.

This module is implemented by HTAssoc.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTASSOC_H
#define HTASSOC_H

#include "HTList.h"

#ifdef __cplusplus
extern "C" { 
#endif 

typedef HTList HTAssocList;

typedef struct {
    char * name;
    char * value;
} HTAssoc;

/*
.
  Creation and Deletetion Methods
.

These methods create and deletes and association list
*/

extern HTAssocList * HTAssocList_new (void);
extern BOOL 	     HTAssocList_delete (HTAssocList * alist);

/*
.
  Add an Element to a List
.

We have two methods for adding new elements - you can either add unconditionally
or replace any existing element with the same name but a new value.
In replacing an element, the search is case insensitive
prefix match! A new list element is added to the beginning of the
list so that it is the first element just after the head element.
*/

extern BOOL HTAssocList_addObject (
	HTAssocList *	alist,
	const char *	name,
	const char *	value);

extern BOOL HTAssocList_replaceObject (
	HTAssocList *	list,
	const char * 	name,
	const char *	value);

/*
.
  Remove an Element from a List
.

Remove the element with the given name from the list. Search is
case insensitive prefix match!
*/

extern BOOL HTAssocList_removeObject (
	HTAssocList * 	list,
	const char * 	name);

/*
.
  Search for Element in a list
.

We have a small set of methods for searching a specific element within a
list.
(
  Case Insensitive, Prefix Match
)

In many situations, the values of associations are case insensitive - use
this method to search the list using case insensitive comparison. Also, you
can search for matching prefixes and not the whole string, for example
"foo" matches "football"
*/

extern char * HTAssocList_findObject (
	HTAssocList *	alist,
	const char *	name);

/*
(
  Case Insensitive, Exact Match
)

In case you want case insensitive, exact match
*/

extern char * HTAssocList_findObjectExact (
	HTAssocList *	alist,
	const char *	name);

/*
(
  Case Sensitive, Prefix Match
)

In case you want case sensitive, prefix match
*/

extern char * HTAssocList_findObjectCaseSensitive (
	HTAssocList *	list,
	const char *	name);

/*
(
  Case Sensitive, Exact Match
)

And finally if you want case sensitive, exact match
*/

extern char * HTAssocList_findObjectCaseSensitiveExact (
	HTAssocList *	list,
	const char * 	name);

/*
.
  Get Name and Values
.

Use this to get the name and value of a assoc object
*/

#define HTAssoc_name(me)	((me) ? (me)->name : NULL)
#define HTAssoc_value(me)	((me) ? (me)->value : NULL)

/*
.
  Traverse list
.

Fast macro to traverse the list. Call it first with copy of list header:
it returns the first object and increments the passed list pointer. Call
it with the same variable until it returns NULL.
*/

#define	HTAssocList_nextObject(me) \
	((me) && ((me) = (me)->next) ? (me)->object : NULL)

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* not HTASSOC_H */

/*

  

  @(#) $Id$

*/
