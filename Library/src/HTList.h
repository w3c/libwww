/*		List object
**
**	The list object is a generic container for storing collections
**	of things in order.
*/
#ifndef HTLIST_H
#define HTLIST_H

#include "HTUtils.h"  /* for BOOL type and PARAMS and ARGS*/

typedef struct _HTList HTList;

struct _HTList {
  void * object;
  HTList * next;
};

#ifdef SHORT_NAMES
#define HTList_new 			HTLiNew
#define HTList_delete			HTLiDele
#define HTList_addObject		HTLiAdOb
#define HTList_removeObject		HTLiReOb
#define HTList_removeLastObject		HTLiReLa
#define HTList_removeFirstObject	HTLiReFi
#define HTList_count			HTLiCoun
#define HTList_indexOf			HTLiInOf
#define HTList_objectAt			HTLiObAt
#endif

extern HTList *	HTList_new NOPARAMS;
extern void	HTList_delete PARAMS((HTList *this));
extern void	HTList_addObject PARAMS((HTList *this, void *newObject));
extern BOOL	HTList_removeObject PARAMS((HTList *this, void *oldObject));
extern void *	HTList_removeLastObject PARAMS((HTList *this));
extern void *	HTList_removeFirstObject PARAMS((HTList *this));
#define 	HTList_isEmpty(this) (this ? this->next == NULL : YES)
extern int	HTList_count PARAMS((HTList *this));
extern int	HTList_indexOf PARAMS((HTList *this, void *object));
#define 	HTList_lastObject(this) \
  (this && this->next ? this->next->object : NULL)
extern void *	HTList_objectAt PARAMS((HTList *this, int position));

/* Fast macro to traverse the list. Call it first with copy of list header :
   it returns the first object and increments the passed list pointer.
   Call it with the same variable until it returns NULL. */
#define HTList_nextObject(this) \
  (this && (this = this->next) ? this->object : NULL)

#endif /* HTLIST_H */
