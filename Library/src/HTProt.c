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
    BOOL		preemptive;
    HTEventCallback *	client;
    HTEventCallback *	server;
};

PRIVATE HTList * protocols = NULL;           /* List of registered protocols */

/* --------------------------------------------------------------------------*/
/*		      Management of the HTProtocol structure		     */
/* --------------------------------------------------------------------------*/

/*
**	Register a Protocol module as an active access method
*/
PUBLIC BOOL HTProtocol_add (CONST char *       	name,
			    BOOL		preemptive,
			    HTEventCallback *	client,
			    HTEventCallback *	server)
{
    if (name && (client || server)) {
	HTProtocol *newProt;
	if ((newProt = (HTProtocol  *) HT_CALLOC(1, sizeof(HTProtocol))) == NULL)
	    HT_OUTOFMEM("HTProtocol_add");
	StrAllocCopy(newProt->name, name);
	{
	    char *ptr = newProt->name;
	    while ((*ptr = TOLOWER(*ptr))) ptr++;
	}
	newProt->preemptive = preemptive;
	newProt->client = client;
	newProt->server = server;
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
		HT_FREE(pres->name);
		return HTList_removeObject(protocols, (void *) pres);
	    }
	}
    }
    return NO;
}

/*
**	Returns the client callback function
*/
PUBLIC HTEventCallback * HTProtocol_client (HTProtocol * protocol)
{
    return protocol ? protocol->client : NULL;
}

/*
**	Returns the server callback function
*/
PUBLIC HTEventCallback * HTProtocol_server (HTProtocol * protocol)
{
    return protocol ? protocol->server : NULL;
}

/*
**	Returns YES if preemptive else NO
*/
PUBLIC BOOL HTProtocol_preemptive (HTProtocol * protocol)
{
    return protocol ? protocol->preemptive : NO;
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
	    HT_FREE(pres->name);
	    HT_FREE(pres);
	}
	HTList_delete(protocols);
	protocols = NULL;
	return YES;
    }
    return NO;
}

/*
**	Search registered protocols to find suitable protocol object.
**	Return protocol object or NULL
*/
PUBLIC HTProtocol * HTProtocol_find (HTRequest * request, CONST char * access)
{
    if (request && access) {
	HTList * cur = protocols;
	HTProtocol * pres;
	if (cur) {
	    while ((pres = (HTProtocol *) HTList_nextObject(cur))) {
		if (!strcmp(pres->name, access)) return pres;
	    }
	}
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_CLASS, (char*) access,
			   (int) strlen(access), "HTProtocol_find");
    }
    return NULL;
}


