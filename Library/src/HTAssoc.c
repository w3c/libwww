/* 							       	      HTAssoc.c
**	ASSOCIATION LIST FOR STORING NAME-VALUE PAIRS.
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	NAMES NOT CASE SENSITIVE, AND ONLY COMMON LENGTH
**	IS CHECKED (allows abbreviations; well, length is
**	taken from lookup-up name, so if table contains
**	a shorter abbrev it is not found).
** AUTHORS:
**	AL	Ari Luotonen	luotonen@dxcern.cern.ch
**
** HISTORY:
**
**
** BUGS:
**
**
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTAssoc.h"					 /* Implemented here */

PUBLIC HTAssocList *HTAssocList_new (void)
{
    return HTList_new();
}


PUBLIC BOOL HTAssocList_delete (HTAssocList * alist)
{
    if (alist) {
	HTAssocList *cur = alist;
	HTAssoc *assoc;
	while (NULL != (assoc = (HTAssoc*)HTList_nextObject(cur))) {
	    if (assoc->name) HT_FREE(assoc->name);
	    if (assoc->value) HT_FREE(assoc->value);
	    HT_FREE(assoc);
	}
	return HTList_delete(alist);
    }
    return NO;
}


PUBLIC BOOL HTAssocList_add (HTAssocList * alist,
			     CONST char * name, CONST char * value)
{
    HTAssoc *assoc;
    if (alist) {
	if ((assoc = (HTAssoc *) HT_CALLOC(1, sizeof(HTAssoc))) == NULL)
	    HT_OUTOFMEM("HTAssoc_add");
	if (name) StrAllocCopy(assoc->name, name);
	if (value) StrAllocCopy(assoc->value, value);
	return HTList_addObject(alist, (void *) assoc);
    } else {
	if (WWWTRACE)
	    TTYPrint(TDEST, "HTAssoc_add: ERROR: assoc list NULL!!\n");
    }
    return NO;
}


PUBLIC char *HTAssocList_lookup (HTAssocList * alist, CONST char * name)
{
    HTAssocList *cur = alist;
    HTAssoc *assoc;
    while ((assoc = (HTAssoc *) HTList_nextObject(cur))) {
	if (!strncasecomp(assoc->name, name, strlen(name)))
	    return assoc->value;
    }
    return NULL;
}

