/* 							       	      HTAssoc.c
**	ASSOCIATION LIST FOR STORING NAME-VALUE PAIRS.
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	NAMES NOT CASE SENSITIVE, AND ONLY COMMON LENGTH
**	IS CHECKED (allows abbreviations; well, length is
**	taken from lookup-up name, so if table contains
**	a shorter abbrev it is not found).
** AUTHORS:
**	AL	Ari Luotonen	luotonen@dxcern.cern.ch
**	HFN	Henrik Frystyk	frystyk@w3.org
**
** HISTORY:
**
**
** BUGS:
**
**
*/

/* Library include files */
#include "sysdep.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTAssoc.h"					 /* Implemented here */

PUBLIC HTAssocList * HTAssocList_new (void)
{
    return HTList_new();
}

PUBLIC BOOL HTAssocList_delete (HTAssocList * list)
{
    if (list) {
	HTAssocList *cur = list;
	HTAssoc *assoc;
	while (NULL != (assoc = (HTAssoc*)HTList_nextObject(cur))) {
	    HT_FREE(assoc->name);
	    HT_FREE(assoc->value);
	    HT_FREE(assoc);
	}
	return HTList_delete(list);
    }
    return NO;
}

PUBLIC BOOL HTAssocList_addObject (HTAssocList * list,
				   const char * name, const char * value)
{
    if (list && name) {
	HTAssoc * assoc;
	if ((assoc = (HTAssoc *) HT_CALLOC(1, sizeof(HTAssoc))) == NULL)
	    HT_OUTOFMEM("HTAssoc_add");
	StrAllocCopy(assoc->name, name);
	if (value) StrAllocCopy(assoc->value, value);
	return HTList_addObject(list, (void *) assoc);
    } else {
	if (WWWTRACE)
	    HTTrace("HTAssoc_add: ERROR: assoc list NULL!!\n");
    }
    return NO;
}

PUBLIC BOOL HTAssocList_replaceObject (HTAssocList * list,
				       const char * name, const char * value)
{
    if (list && name) {
	HTAssocList * cur = list;
	HTAssoc * assoc;
	int len = strlen(name);
	while ((assoc = (HTAssoc *) HTList_nextObject(cur))) {
	    if (!strncasecomp(assoc->name, name, len)) {
		StrAllocCopy(assoc->name, name);
		if (value) StrAllocCopy(assoc->value, value);
		return YES;
	    }
	}
	return HTAssocList_addObject(list, name, value);
    }
    return NO;
}

PUBLIC char * HTAssocList_findObject (HTAssocList * list, const char * name)
{
    if (list && name) {
	HTAssocList * cur = list;
	HTAssoc * assoc;
	int len = strlen(name);
	while ((assoc = (HTAssoc *) HTList_nextObject(cur))) {
	    if (!strncasecomp(assoc->name, name, len))
		return assoc->value;
	}
    }
    return NULL;
}

/*
**  Searches the whole list and removes all elements with this name
*/
PUBLIC BOOL HTAssocList_removeObject (HTAssocList * list, const char * name)
{
    BOOL found = NO;
    if (list && name) {
	HTAssocList * cur = list;
	HTAssoc * assoc;
	int len = strlen(name);
	while ((assoc = (HTAssoc *) HTList_nextObject(cur))) {
	    if (!strncasecomp(assoc->name, name, len)) {
		HTList_removeObject(list, assoc);
		HT_FREE(assoc);
		found = YES;
	    }
	}
    }
    return found;
}
