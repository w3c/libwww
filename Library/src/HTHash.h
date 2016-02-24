/*

  					W3C Sample Code Library libwww Hash Table Class


!
  Hash Table Class
!

Written and integrated into libwww by John Punin - thanks!

This module is implemented by HTHash.c, and is a part
of the  W3C Sample Code Library.

This HashTable class implements a simple hash table to keep objects associated
with key words.
*/

#ifndef HTHASH_H
#define HTHASH_H

#include "HTList.h"

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTHashtable HTHashtable;

struct _HTHashtable {
    void **table;
    int count;
    int size;
};
    
typedef struct _keynode keynode;

struct _keynode {
    char *key;
    void *object;
};

/*
.
  Creation and Deletion Methods
.

These methods create and deletes a Hash Table
*/

extern HTHashtable *	HTHashtable_new	(int size);

extern BOOL	HTHashtable_delete (HTHashtable *me);

/*
.
  Add an Element to a HashTable
.
*/

extern BOOL HTHashtable_addObject (HTHashtable *me, const char *key, void *newObject);

/*
.
  Remove an Element from a HashTable
.
*/

extern BOOL HTHashtable_removeObject (HTHashtable *me, const char *key);

/*
.
  Search for an Element in a Hash Table
.
*/

extern void *	HTHashtable_object (HTHashtable * me, const char *key);

/*
.
  Size of a Hash Table
.
*/

extern int	HTHashtable_count  (HTHashtable *me);

/*
.
  Walk all the elements in a Hash Table
.

Walking the hashtable calls the specified function pointer with each key
and object that is in the hash table.  If the callback function returns 0,
the walking stops.  If it returns a negative number, the current element
is removed from the hash table.  Return a positive number to keep going.

Note that it is legal for the walkFunc to call HTHashtable_removeObject()
on any element in the current hash table except the current
one (if you intend to keep going, that is).  The only legal way to delete the
current element while continuing to walk the table is to use the negative
return value.
*/

extern BOOL	HTHashtable_walk (HTHashtable *me, int (*walkFunc)(HTHashtable *, char *, void *));

/*
.
  Extract in a dynamic array all keys of the Hash Table
.
*/

extern HTArray * HTHashtable_keys  (HTHashtable *me);

/*
.
  Print the keys of the Hash Table
.
*/

extern void HTHashtable_print (HTHashtable *me);

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTHASH_H */

/*

  

  @(#) $Id$

*/
