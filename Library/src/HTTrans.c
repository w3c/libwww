/*								     HTTrans.c
**	TRANSPORT LAYER MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** HISTORY:
**	Marts 96 HFN	Written
*/

/* Library Include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTParse.h"
#include "HTReq.h"
#include "HTIOStream.h"
#include "HTTrans.h"					 /* Implemented here */

PRIVATE HTList * transports = NULL;         /* List of registered transports */

/* --------------------------------------------------------------------------*/

/*
**	Register a Transport module
*/
PUBLIC BOOL HTTransport_add (const char *		name,
			     HTTransportMode		mode,
			     HTInput_new *		get_input,
			     HTOutput_new *		get_output)
{
    if (name && (get_input || get_output)) {
	HTTransport * tp;
	if ((tp = (HTTransport *) HT_CALLOC(1, sizeof(HTTransport))) == NULL)
	    HT_OUTOFMEM("HTTransport_add");
	StrAllocCopy(tp->name, name);
	{
	    char *ptr = tp->name;
	    while ((*ptr = TOLOWER(*ptr))) ptr++;
	}
	tp->mode = mode;
	tp->input_new = get_input;
	tp->output_new = get_output;
	if (!transports) transports = HTList_new();
	else HTTransport_delete(name); /* Ensure not listed twice */
	HTTRACE(CORE_TRACE, "Transport... Adding `%s'\n" _ name);
	return HTList_addObject(transports, (void *) tp);
    } else HTTRACE(CORE_TRACE, "Transport... Can't add this...\n");
    return NO;
}

/*
**	Deletes a Transport module
*/
PUBLIC BOOL HTTransport_delete (const char * name)
{
    if (transports) {
	HTList *cur = transports;
	HTTransport *pres;
	while ((pres = (HTTransport *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->name, name)) {
		BOOL status = HTList_removeObject(transports, (void *) pres);
		HT_FREE(pres->name);
		HT_FREE(pres);
		return status;
	    }
	}
    }
    return NO;
}

/*
**	Delete the list of registered transports. This is called from
**	within HTLibTerminate.
*/
PUBLIC BOOL HTTransport_deleteAll (void)
{
    if (transports) {
	HTList * cur = transports;
	HTTransport * pres;
	while ((pres = (HTTransport *) HTList_nextObject(cur))) {
	    HT_FREE(pres->name);
	    HT_FREE(pres);
	}
	HTList_delete(transports);
	transports = NULL;
	return YES;
    }
    return NO;
}

/*
**	Search registered transports to find a suitable transport object.
**	Return transport object or NULL
*/
PUBLIC HTTransport * HTTransport_find (HTRequest * request, const char * name)
{
    if (name) {
	HTList * cur = transports;
	HTTransport * pres;
	if (cur) {
	    while ((pres = (HTTransport *) HTList_nextObject(cur))) {
		if (!strcmp(pres->name, name)) return pres;
	    }
	}
	if (request)
	    HTRequest_addError(request, ERR_FATAL, NO, HTERR_CLASS, (char *) name,
			       (int) strlen(name), "HTTransport_find");
    }
    return NULL;
}

PUBLIC HTTransportMode HTTransport_mode (HTTransport * tp)
{
    return (tp ? tp->mode : HT_TP_SINGLE);
}

PUBLIC BOOL HTTransport_setMode (HTTransport * tp, HTTransportMode mode)
{
    if (tp) {
        tp->mode = mode;
        return YES;
    }
    return NO;
}

