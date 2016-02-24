/*

					W3C Sample Code Library libwww Dynamic Array Pointer Class




!Dynamic Array Pointer Class!

*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module implements a flexible array of pointers. It is a general
utility module.  An array is a structure which may be extended.  These
routines create and append data to arrays, automatically reallocating
them as necessary.  It is garanteed that the last entry in an array is
NULL

This module is implemented by HTArray.c, and
it is a part of the  W3C
Sample Code Library.

*/

#ifndef HTARRAY_H
#define HTARRAY_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*

.Private Data Structure.

This structure should not be referenced outside this module. If I find
out I'll make it private ;-)

*/

typedef struct {
    int		size;		/* In numbers of elements	*/
    int		growby;		/* Allocation unit in elements	*/
    int		allocated;	/* Current size of *data	*/
    void **	data;		/* Pointer to malloced area or 0 */
} HTArray;

/*

.Create a new Array.

Create a new array and specify the number of bytes to allocate at a
time when the array is later extended. Arbitrary but normally a
trade-off time vs. memory

*/

extern HTArray * HTArray_new (int grow);

/*

.Delete an Array.

Delete an array created by HTArray_new

*/

extern BOOL HTArray_delete (HTArray * array);

/*

.Clear an Array.

Clears an array but keeps it around

*/

extern BOOL HTArray_clear (HTArray * array);

/*

.Append an element to the Array.

Add the element to the array.

*/

extern BOOL HTArray_addObject (HTArray * array, void * object);

/*

.Traverse an Array.

Fast macros to traverse a macro ending in a NULL element.

*/

#define HTArray_firstObject(me, dp) \
	((me) && ((dp)=(me)->data) ? *(dp)++ : NULL)
#define HTArray_nextObject(me, dp) \
	((me) && (dp) ? *(dp)++ : NULL)

/*

.Sort an Array.

An array can be sorted in any way you like, for example with
qsort(). This module provides an easy interface to the qsort()
function using where you can define you own comparison routine as a
function of the type:

*/

typedef int HTComparer (const void * a, const void * b);

/*

The sort function returns YES if sorting OK, else NO.

*/

extern BOOL HTArray_sort (HTArray * array, HTComparer * comp);

/*

.Returns Data Vector.

Returns a pointer to the actual data

*/

#define HTArray_data(me)	((me) ? (me)->data : NULL)

/*


.Return Current Size.

Returns the current size of the chunk

*/

#define HTArray_size(me)	((me) ? (me)->size : -1)

/*

*/

#ifdef __cplusplus
}
#endif

#endif

/*



@(#) $Id$


*/
