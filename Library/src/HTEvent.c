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

#include <assert.h>			/* @@@ Should be in sysdep.h @@@ */

/* Implementation dependent include files */
#include "sysdep.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTReqMan.h"
#include "HTError.h"
#include "HTDNS.h"
#include "HTEvent.h"					 /* Implemented here */

PRIVATE HTEvent_registerCallback * RegisterCBF = 0;
PRIVATE HTEvent_unregisterCallback * UnregisterCBF = 0;

PUBLIC void HTEvent_setRegisterCallback(HTEvent_registerCallback * registerCBF)
{
    RegisterCBF = registerCBF;
}

PUBLIC void HTEvent_setUnregisterCallback(HTEvent_unregisterCallback * unregisterCBF)
{
    UnregisterCBF = unregisterCBF;
}


PUBLIC int HTEvent_register (SOCKET s, HTRequest * rq, SockOps ops,
			     HTEventCallback *cbf, HTPriority p) 
{
    if (!RegisterCBF)
        return -1;
    return (*RegisterCBF)(s, rq, ops, cbf, p);
}

PUBLIC int HTEvent_unregister (SOCKET s, SockOps ops)
{
    if (!UnregisterCBF)
        return -1;
    return (*UnregisterCBF)(s, ops);
}

