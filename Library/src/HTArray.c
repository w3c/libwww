/*								      HTArray.c
**	ARRAY HANDLING:	FLEXIBLE ARRAYS OF POINTERS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** history:	Sep 95 HFN	Written after looking at Hancock Tower
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTArray.h"				         /* Implemented here */

/*	Create a array with a certain allocation unit
**	--------------
*/
PUBLIC HTArray * HTArray_new (int grow)
{
    HTArray * array;
    if ((array = (HTArray  *) HT_CALLOC(1, sizeof(HTArray))) == NULL)
        HT_OUTOFMEM("HTArray_new");
    array->growby = grow;
    return array;
}

/*	Clear a array of all data
**	--------------------------
*/
PUBLIC BOOL HTArray_clear (HTArray * array)
{
    if (array) {
	HT_FREE(array->data);
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
	HT_FREE(array->data);
	HT_FREE(array);
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
		if ((array->data = (void * *) HT_REALLOC(array->data, (array->allocated+array->growby) * sizeof(void *))) == NULL)
		    HT_OUTOFMEM("HTArray_add");
		memset((array->data+array->allocated), '\0', array->growby * sizeof(void *));
	    } else {
		if ((array->data = (void * *) HT_CALLOC(array->growby, sizeof(void *))) == NULL)
		    HT_OUTOFMEM("HTArray_add");
	    }
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
PUBLIC BOOL HTArray_sort (HTArray * array, HTComparer * comp)
{
    if (array && comp) {
	qsort(array->data, array->size, sizeof (void *), comp);
	return YES;
    }
    return NO;
}
