/*

					W3C Sample Code Library libwww Balanced Binary Tree




!Balanced Binary Tree!

*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

Tree creation, traversal and freeing. User-supplied comparison
routine.

This module is implemented by HTBTree.c, and
it is a part of the  W3C
Sample Code Library.

*/

#ifndef HTBTTEE_H
#define HTBTREE_H

#include "HTArray.h"

#ifdef __cplusplus
extern "C" { 
#endif 


/*

*/

typedef struct _HTBTree HTBTree;

typedef struct _HTBTree_element HTBTElement;

/*

.Create a Binary Tree.

This function creates a new binary tree and uses the comparison
function when building the tree.

*/

extern HTBTree * HTBTree_new (HTComparer * comp);

/*

.Free storage of the tree but not of the objects.

*/

extern void HTBTree_free (HTBTree * tree);

/*

.Free storage of the tree and of the objects.

*/

extern void HTBTreeAndObject_free (HTBTree * tree);

/*

.Add an object to a binary tree.

*/

extern void HTBTree_add (HTBTree* tree, void * object);

/*

.Return an Object.

*/

extern void * HTBTree_object (HTBTElement * element);

/*

.Find next element in depth-first order.

(On entry,)

If element is NULL then start with leftmost element. if
not NULL then give next object to the right. The function returns a
pointer to element or NULL if none left.

*/

extern HTBTElement * HTBTree_next (HTBTree * tree, HTBTElement * element);

/*

*/

#ifdef __cplusplus
}
#endif

#endif /* HTBTTEE_H */

/*



@(#) $Id$


*/
