/*
**	HASH TABLE CLASS
**
**	This HashTable class implements a simple hash table to keep
**	objects associated with key words.
**
** Author:
**	JP	John Punin
**
*/

#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTHash.h"

/*
(
  Creation and Deletion Methods
)

These methods create and deletes a Hash Table
*/

PUBLIC HTHashtable * HTHashtable_new (int size)
{
    HTHashtable *newHashtable;
    int c = size > 0 ? size : HT_L_HASH_SIZE;
    if ((newHashtable = (HTHashtable  *) HT_CALLOC(1, sizeof (HTHashtable))) == NULL)
        HT_OUTOFMEM("HTHashtable_new");

    if((newHashtable->table = (void **) HT_CALLOC(c, sizeof (void *))) == NULL)
	HT_OUTOFMEM("HTHashtable_new");

    newHashtable->count = 0;
    newHashtable->size = c;
    return newHashtable;
}

PUBLIC BOOL HTHashtable_delete (HTHashtable *me)
{
    if (me) {
	int i;
	for(i = 0; i< me->size; i++) {
	    HTList * l = (HTList *)me->table[i];
	    if (l) {
		HTList *cur = l;
		keynode *kn;
		while ((kn = (keynode *) HTList_nextObject(cur))) {
		    HT_FREE(kn->key);
		    HT_FREE(kn);
		}
		HTList_delete(l);
	    }
	}
        HT_FREE(me->table);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PRIVATE int hash_number (const char *key, int size)
{
    int hash = 0;

    if (key) {
	const char * ptr = key;
	for(; *ptr; ptr++)
	    hash = (int) ((hash*3 + (*(unsigned char*)ptr)) % size);
    }
    return hash;
}

/*
(
  Add an Element to a HashTable
)
*/

PUBLIC BOOL HTHashtable_addObject (HTHashtable *me, const char *key,
				   void *newObject)
{
    if(me) {
	int size = me->size;
	int i = hash_number(key,size);
	HTList *l = (HTList *)me->table[i];
	keynode *kn;
	if(!l)
	    l = me->table[i] = HTList_new();
	if ((kn = (keynode  *) HT_CALLOC(1, sizeof (keynode))) == NULL)
	    HT_OUTOFMEM("HTHashtable_addObject");
	StrAllocCopy(kn->key,key);
	kn->object = newObject;
	HTList_addObject(l,kn);
	me->count++;
	return YES;
    }
    return NO;
}

/*
(
  Remove an Element from the HashTable
)
*/

PUBLIC BOOL HTHashtable_removeObject (HTHashtable *me, const char *key)
{
    if(me) {
	int size = me->size;
	int i = hash_number(key,size);
	HTList *l = (HTList *)me->table[i];
	if(l) {
	    HTList *cur = l;
	    keynode *kn;
	    while ((kn = (keynode *) HTList_nextObject(cur))) {
		if(!strcmp(key,kn->key)) {
		    HTList_removeObject(l,kn);
		    me->count--;
		    return YES;
		}
	    }
	}
    }
    return NO;
}

/*
(
  Search for an Element in a Hash Table
)
*/

PUBLIC void *HTHashtable_object (HTHashtable * me, const char *key)
{
    if(me) {
	int size = me->size;
	int i = hash_number(key,size);
	HTList * l = (HTList *)me->table[i];
	if (l) {
	    HTList *cur = l;
	    keynode *kn;
	    while ((kn = (keynode *) HTList_nextObject(cur))) {
		if(!strcmp(key,kn->key))
		    return kn->object;
	    }
	}
    }
    return NULL;
}

/*
(
  Size of a Hash Table
)
*/

PUBLIC int HTHashtable_count (HTHashtable *me)
{
    if(me)
	return me->count;
    return -1;
}

/*
(
   Walk all Elements in the HashTable
)
*/

PUBLIC BOOL HTHashtable_walk (HTHashtable *me,
			      int (*walkFunc)(HTHashtable *,char *, void *))
{
    if(me) {
	int i, j;
	for(i = 0; i< me->size; i++) {
	    HTList *l = (HTList *)me->table[i];
	    if(l) {
		HTList *cur = l;
		keynode *kn, *nextkn;
		for(kn = (keynode *)HTList_nextObject(cur); kn; kn = nextkn) {
		    j = walkFunc(me, kn->key, kn->object);
		    if(j == 0)
			return YES;
		    nextkn = (keynode *)HTList_nextObject(cur);
		    if (j < 0) {
			HTList_removeObject(l, kn);
			me->count--;
		    }
		}
	    }
	}
	return YES;
    }
    return NO;
}

/*
(
   Extract in a dynamic array all keys of the Hash Table
)
*/

PUBLIC HTArray * HTHashtable_keys (HTHashtable *me)
{
    if(me) {
	HTArray *keys = HTArray_new(me->count);
	int i;
    
	for(i = 0; i< me->size; i++) {
	    HTList * l = (HTList *)me->table[i];
	    if (l) {
		HTList *cur = l;
		keynode *kn;
		while ((kn = (keynode *) HTList_nextObject(cur))) {
		    char * nkey = NULL;
		    StrAllocCopy(nkey,kn->key);
		    HTArray_addObject(keys,nkey);
		}
	    }
	}
	return keys;
    }
    return NULL;
}

/*
(
   Print the keys of the Hash Table
)
*/

PUBLIC void HTHashtable_print (HTHashtable *me)
{
    HTArray *keys = HTHashtable_keys(me);
    int i;
    HTPrint("Printing Hash Table of size %d\n", HTArray_size(keys));
    for(i = 0; i< HTArray_size(keys); i++) {
	HTPrint("Key %d %s\n",i,HTArray_data(keys)[i]);
    }
    for(i = 0; i< HTArray_size(keys); i++) {
	HT_FREE(HTArray_data(keys)[i]);
    }
    HTArray_delete(keys);
}

