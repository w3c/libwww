/*				    				     HTEvntrg.c
**	EVENT DISPATCHER
**
**	(c) COPYRIGHT MIT 1996.
**	Please first read the full copyright statement in the file COPYRIGH.
**      @(#) $Id$
**
**	The event dispatcher allows applications to register their own event
**	models. They may register the standard HTEventrg methods from 
**	HTEventrg.c, of implement their functionality with their on methods.
**
** Authors:
**	EGP	Eric Gordon Prud'hommeuax
** Bugs
**
*/

/* Implementation dependent include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "HTReqMan.h"
#include "HTEvent.h"					 /* Implemented here */

PRIVATE HTEvent_registerCallback * RegisterCBF = NULL;
PRIVATE HTEvent_unregisterCallback * UnregisterCBF = NULL;

PUBLIC void HTEvent_setRegisterCallback(HTEvent_registerCallback * registerCBF)
{
    if (CORE_TRACE) HTTrace("Event....... registering %p\n", registerCBF);
    RegisterCBF = registerCBF;
}

PUBLIC void HTEvent_setUnregisterCallback(HTEvent_unregisterCallback * unregisterCBF)
{
    if (CORE_TRACE) HTTrace("Event....... registering %p\n", unregisterCBF);
    UnregisterCBF = unregisterCBF;
}


PUBLIC int HTEvent_register (SOCKET s, HTRequest * rq, SockOps ops,
			     HTEventCallback *cbf, HTPriority p) 
{
    if (!RegisterCBF) {
	if (CORE_TRACE) HTTrace("Event....... No handler registered\n");
        return -1;
    }
    return (*RegisterCBF)(s, rq, ops, cbf, p);
}

PUBLIC int HTEvent_unregister (SOCKET s, SockOps ops)
{
    if (!UnregisterCBF) {
	if (CORE_TRACE) HTTrace("Event....... No handler registered\n");
        return -1;
    }
    return (*UnregisterCBF)(s, ops);
}

