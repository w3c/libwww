/*								     HTProt.c
**	ACCESS SCHEME MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**
** HISTORY:
**	6 July 95  HFN	Spawned off from HTAccess
*/

/* Library Include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTString.h"
#include "HTProt.h"					 /* Implemented here */

/* Variables and typedefs local to this module */
struct _HTProtocol {
    char *		name;
    BOOL		preemtive;
    HTEventCallback	*callback;
};

PRIVATE HTList * protocols = NULL;           /* List of registered protocols */

/* --------------------------------------------------------------------------*/
/*		      Management of the HTProtocol structure		     */
/* --------------------------------------------------------------------------*/

/*
**	Register a Protocol module as an active access method
*/
PUBLIC BOOL HTProtocol_add (CONST char *       	name,
			    BOOL		preemtive,
			    HTEventCallback *	callback)
{
    if (name && callback) {
	HTProtocol *newProt = (HTProtocol *) calloc(1, sizeof(HTProtocol));
	if (newProt == NULL) outofmem(__FILE__, "HTProtocol_add");
	StrAllocCopy(newProt->name, name);
	newProt->preemtive = preemtive;
	newProt->callback = callback;
	if (!protocols) protocols = HTList_new();
	return HTList_addObject(protocols, (void *) newProt);
    }
    return NO;
}

/*
**	Deletes a Protocol module as an active access method
*/
PUBLIC BOOL HTProtocol_delete (CONST char * name)
{
    if (protocols) {
	HTList *cur = protocols;
	HTProtocol *pres;
	while ((pres = (HTProtocol *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->name, name)) {
		FREE(pres->name);
		return HTList_removeObject(protocols, (void *) pres);
	    }
	}
    }
    return NO;
}

/*
**	Returns the callback function
*/
PUBLIC HTEventCallback *HTProtocol_callback (HTProtocol * protocol)
{
    return protocol ? protocol->callback : NULL;
}

/*
**	Returns YES if preemtive else NO
*/
PUBLIC BOOL HTProtocol_preemtive (HTProtocol * protocol)
{
    return protocol ? protocol->preemtive : NO;
}

/*
**	Delete the list of registered access methods. This is called from
**	within HTLibTerminate. Thanks to Eric Sink, eric@spyglass.com
*/
PUBLIC BOOL HTProtocol_deleteAll (void)
{
    if (protocols) {
	HTList *cur = protocols;
	HTProtocol *pres;
	while ((pres = (HTProtocol *) HTList_nextObject(cur))) {
	    FREE(pres->name);
	    free(pres);
	}
	HTList_delete(protocols);
	protocols = NULL;
	return YES;
    }
    return NO;
}


/*
**	Search registered protocols to find suitable one.
**	Return YES if found, else NO
*/
PUBLIC BOOL HTProtocol_find (HTParentAnchor * anchor)
{
    if (anchor) {
	char *access = HTParse(HTAnchor_physical(anchor), "", PARSE_ACCESS);
	HTList *cur = protocols;
	HTProtocol *p;
	if (!cur) {
	    if (WWWTRACE)
		fprintf(TDEST, "HTProtocol.. NO PROTOCOL MODULES INITIATED\n");
	} else {
	    while ((p = (HTProtocol *) HTList_nextObject(cur))) {
		if (strcasecomp(p->name, access)==0) {
		    HTAnchor_setProtocol(anchor, p);
		    free(access);
		    return YES;
		}
	    }
	}
	free(access);
    }
    return NO;
}

