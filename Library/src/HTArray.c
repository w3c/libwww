/*								      HTArray.c
**	ARRAY HANDLING:	FLEXIBLE ARRAYS OF POINTERS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
** history:	Sep 95 HFN	Written after looking at Hancock Tower
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTArray.h"				         /* Implemented here */

/*	Create a array with a certain allocation unit
**	--------------
*/
PUBLIC HTArray * HTArray_new (int grow)
{
    HTArray * array = (HTArray *) calloc(1, sizeof(HTArray));
    if (array == NULL) outofmem(__FILE__, "HTArray_new");
    array->growby = grow;
    return array;
}

/*	Clear a array of all data
**	--------------------------
*/
PUBLIC BOOL HTArray_clear (HTArray * array)
{
    if (array) {
	FREE(array->data);
	array->size = 0;
	array->allocated = 0;
	return YES;
    }
    return NO;
}

/*	Free an array
**	-------------
*/
PUBLIC BOOL HTArray_delete (HTArray * array)
{
    if (array) {
	FREE(array->data);
	free(array);
	return YES;
    }
    return NO;
}

/*	Add an object
**	-------------
*/
PUBLIC BOOL HTArray_addObject (HTArray * array, void * object)
{
    if (array) {
	if (array->size >= array->allocated-1) {
	    if (array->data) {
		array->data = (void **)
		    realloc(array->data, (array->allocated+array->growby) * sizeof(void *));
		memset((array->data+array->allocated), '\0', array->growby * sizeof(void *));
	    } else {
		array->data = (void **) calloc(array->growby, sizeof(void *));
	    }
	    if (!array->data) outofmem(__FILE__, "HTArray_add");
	    array->allocated += array->growby;
	}
	*(array->data+array->size++) = object;
	return YES;
    }
    return NO;
}

/*	Sort an array
**	-------------
*/
PUBLIC BOOL HTArray_sort (HTArray * array,
			  int (*compar) (CONST void *, CONST void *))
{
    if (array && compar) {
	qsort(array->data, array->size, sizeof (void *), compar);
	return YES;
    }
    return NO;
}
