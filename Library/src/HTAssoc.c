/* 							       	      HTAssoc.c
**	ASSOCIATION LIST FOR STORING NAME-VALUE PAIRS.
**
**	(c) COPYRIGHT CERN 1994.
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
#include "HTAAUtil.h"
#include "HTAssoc.h"
#include "HTString.h"

PUBLIC HTAssocList *HTAssocList_new NOARGS
{
    return HTList_new();
}


PUBLIC void HTAssocList_delete ARGS1(HTAssocList *, alist)
{
    if (alist) {
	HTAssocList *cur = alist;
	HTAssoc *assoc;
	while (NULL != (assoc = (HTAssoc*)HTList_nextObject(cur))) {
	    if (assoc->name) free(assoc->name);
	    if (assoc->value) free(assoc->value);
	    free(assoc);
	}
	HTList_delete(alist);
    }
}


PUBLIC void HTAssocList_add ARGS3(HTAssocList *,	alist,
				  CONST char *,		name,
				  CONST char *,		value)
{
    HTAssoc *assoc;

    if (alist) {
	if (!(assoc = (HTAssoc*)malloc(sizeof(HTAssoc))))
	    outofmem(__FILE__, "HTAssoc_add");
	assoc->name = NULL;
	assoc->value = NULL;

	if (name) StrAllocCopy(assoc->name, name);
	if (value) StrAllocCopy(assoc->value, value);
	HTList_addObject(alist, (void*)assoc);
    }
    else if (TRACE) fprintf(TDEST, "HTAssoc_add: ERROR: assoc list NULL!!\n");
}


PUBLIC char *HTAssocList_lookup ARGS2(HTAssocList *,	alist,
				      CONST char *,	name)
{
    HTAssocList *cur = alist;
    HTAssoc *assoc;

    while (NULL != (assoc = (HTAssoc*)HTList_nextObject(cur))) {
	if (!strncasecomp(assoc->name, name, strlen(name)))
	    return assoc->value;
    }
    return NULL;
}

