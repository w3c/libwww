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
#include "HTParse.h"
#include "HTString.h"
#include "HTProt.h"					 /* Implemented here */

/* Variables and typedefs local to this module */
PRIVATE HTList * protocols = NULL;           /* List of registered protocols */

/* --------------------------------------------------------------------------*/
/*		      Management of the HTProtocol structure		     */
/* --------------------------------------------------------------------------*/

/*
**	Register a Protocol module as an active access method
*/
PUBLIC BOOL HTProtocol_add ARGS1(HTProtocol *, prot)
{
    if (!protocols) protocols = HTList_new();
    HTList_addObject(protocols, (void *) prot);
    return YES;
}

/*
**	Deletes a Protocol module as an active access method
*/
PUBLIC BOOL HTProtocol_delete ARGS1(HTProtocol *, prot)
{
    return (protocols && prot) ?
	HTList_removeObject(protocols, (void *) prot) : NO;
}

/*
**	Delete the list of registered access methods. This is called from
**	within HTLibTerminate. Written by Eric Sink, eric@spyglass.com
*/
PUBLIC void HTProtocol_deleteAll NOARGS
{
    if (protocols) {
	HTList_delete(protocols);
	protocols = NULL;
    }
}


/*
**	Search registered protocols to find suitable one.
**	Return YES if found, else NO
*/
PUBLIC BOOL HTProtocol_get ARGS1(HTParentAnchor *, anchor)
{
    if (anchor) {
	char *access = HTParse(HTAnchor_physical(anchor), "", PARSE_ACCESS);
	HTList *cur = protocols;
	HTProtocol *p;
	if (!cur) {
	    if (TRACE)
		fprintf(TDEST, "HTProtocol.. NO PROTOCOL MODULES INITIATED\n");
	} else {
	    while ((p = (HTProtocol *) HTList_nextObject(cur))) {
		if (strcmp(p->name, access)==0) {	/* Case insensitive? */
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

