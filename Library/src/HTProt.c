/*								     HTProt.c
**	ACCESS SCHEME MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**
** HISTORY:
**	6 July 95  HFN	Spawned off from HTAccess
*/

/* Library Include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTString.h"
#include "HTTrans.h"
#include "HTProt.h"					 /* Implemented here */

/* Variables and typedefs local to this module */
struct _HTProtocol {
    char *		name;	      /* Name of this protocol access scheme */
    char *		transport;		    /* Name of the transport */
    HTProtocolId	id;		   /* Default port for this protocol */
    BOOL		preemptive;
    HTProtCallback *	client;
    HTProtCallback *	server;
};

PRIVATE HTList * protocols = NULL;           /* List of registered protocols */

/* --------------------------------------------------------------------------*/
/*		      Management of the HTProtocol structure		     */
/* --------------------------------------------------------------------------*/

/*
**	Register a Protocol module as an active access method
*/
PUBLIC BOOL HTProtocol_add (const char *       	name,
			    const char *	transport,
			    HTProtocolId	protocolId,
			    BOOL		preemptive,
			    HTProtCallback *	client,
			    HTProtCallback *	server)
{
    if (name && (client || server)) {
	HTProtocol *newProt;
	if ((newProt=(HTProtocol *) HT_CALLOC(1, sizeof(HTProtocol))) == NULL)
	    HT_OUTOFMEM("HTProtocol_add");
	StrAllocCopy(newProt->name, name);
	{
	    char *ptr = newProt->name;
	    while ((*ptr = TOLOWER(*ptr))) ptr++;
	}
	StrAllocCopy(newProt->transport, transport);
	{
	    char *ptr = newProt->transport;
	    while ((*ptr = TOLOWER(*ptr))) ptr++;
	}
	newProt->id = protocolId;
	newProt->preemptive = preemptive;
	newProt->client = client;
	newProt->server = server;
	if (!protocols) protocols = HTList_new();
	else HTProtocol_delete(name); /* Ensure not listed twice */
	HTTRACE(CORE_TRACE, "Protocol.... Adding `%s'\n" _ name);
	return HTList_addObject(protocols, (void *) newProt);
    }
    return NO;
}

/*
**	Deletes a Protocol module as an active access method
*/
PUBLIC BOOL HTProtocol_delete (const char * name)
{
    if (protocols) {
	HTList *cur = protocols;
	HTProtocol *pres;
	while ((pres = (HTProtocol *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->name, name)) {
		BOOL status = HTList_removeObject(protocols, (void *) pres);
		HT_FREE(pres->name);
		HT_FREE(pres->transport);
		HT_FREE(pres);
		return status;
	    }
	}
    }
    return NO;
}

/*
**	Returns the client callback function
*/
PUBLIC HTProtCallback * HTProtocol_client (HTProtocol * protocol)
{
    return protocol ? protocol->client : NULL;
}

/*
**	Returns the server callback function
*/
PUBLIC HTProtCallback * HTProtocol_server (HTProtocol * protocol)
{
    return protocol ? protocol->server : NULL;
}

/*
**	Returns the default port
*/
PUBLIC HTProtocolId HTProtocol_id (HTProtocol * protocol)
{
    return protocol ? protocol->id : 0;
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
	    HT_FREE(pres->transport);
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
PUBLIC HTProtocol * HTProtocol_find (HTRequest * request, const char * access)
{
    if (access) {
	HTList * cur = protocols;
	HTProtocol * pres;
	if (cur) {
	    while ((pres = (HTProtocol *) HTList_nextObject(cur))) {
		if (!strcmp(pres->name, access)) return pres;
	    }
	}
	if (request)
	    HTRequest_addError(request, ERR_FATAL, NO, HTERR_CLASS, (char*) access,
			       (int) strlen(access), "HTProtocol_find");
    }
    return NULL;
}

/*
**  Get the transport name of this protocol
*/
PUBLIC BOOL HTProtocol_setTransport (HTProtocol * protocol,
				     const char * transport)
{
    if (protocol && transport) {
	StrAllocCopy(protocol->transport, transport);
	{
	    char *ptr = protocol->transport;
	    while ((*ptr = TOLOWER(*ptr))) ptr++;
	}
	return YES;
    }
    return NO;
}

PUBLIC const char * HTProtocol_transport (HTProtocol * protocol)
{
    return (protocol ? protocol->transport : NULL);
}

PUBLIC const char * HTProtocol_name (HTProtocol * protocol)
{
    return (protocol ? protocol->name : NULL);
}
