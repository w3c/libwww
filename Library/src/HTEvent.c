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
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTEvent.h"					 /* Implemented here */

PRIVATE HTEvent_registerCallback * RegisterCBF = NULL;
PRIVATE HTEvent_unregisterCallback * UnregisterCBF = NULL;

/* ------------------------------------------------------------------------- */

PUBLIC void HTEvent_setRegisterCallback(HTEvent_registerCallback * registerCBF)
{
    HTTRACE(CORE_TRACE, "Event....... registering %p\n" _ registerCBF);
    RegisterCBF = registerCBF;
}

PUBLIC void HTEvent_setUnregisterCallback(HTEvent_unregisterCallback * unregisterCBF)
{
    HTTRACE(CORE_TRACE, "Event....... registering %p\n" _ unregisterCBF);
    UnregisterCBF = unregisterCBF;
}

PUBLIC BOOL HTEvent_isCallbacksRegistered (void)
{
    return (RegisterCBF && UnregisterCBF);
}

PUBLIC int HTEvent_unregister (SOCKET s, HTEventType type)
{
    if (!UnregisterCBF) {
	HTTRACE(CORE_TRACE, "Event....... No handler registered\n");
        return -1;
    }
    return (*UnregisterCBF)(s, type);
}

PUBLIC int HTEvent_register(SOCKET s, HTEventType type, HTEvent * event)
{
    if (!RegisterCBF) {
	HTTRACE(CORE_TRACE, "Event....... No handler registered\n");
        return -1;
    }
    return (*RegisterCBF)(s, type, event);
}

PUBLIC BOOL HTEvent_setCallback(HTEvent * event, HTEventCallback * cbf)
{
    if (!event) return NO;
    event->cbf = cbf;
    return YES;
}

PUBLIC HTEvent * HTEvent_new (HTEventCallback * cbf, void * context,
			      HTPriority priority, int millis)
{
    if (cbf) {
	HTEvent * me;
	if ((me = (HTEvent *) HT_CALLOC(1, sizeof(HTEvent))) == NULL)
	    HT_OUTOFMEM("HTEvent_new");
	me->cbf = cbf;
	me->param = context;
	me->priority = priority;
	me->millis = millis;
	HTTRACE(CORE_TRACE, "Event....... Created event %p with context %p, priority %d, and timeout %d\n" _ 
		    me _ context _ priority _ millis);
	return me;
    }
    return NULL;
}

PUBLIC BOOL HTEvent_delete (HTEvent * me)
{
    if (me) {
	HT_FREE(me);
	HTTRACE(CORE_TRACE, "Event....... Deleted event %p\n" _ me);
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTEvent_setParam(HTEvent * event, void * param)
{
    if (!event) return NO;
    event->param = param;
    return YES;
}

PUBLIC BOOL HTEvent_setPriority(HTEvent * event, HTPriority priority)
{
    if (!event) return NO;
    event->priority = priority;
    return YES;
}

PUBLIC BOOL HTEvent_setTimeout(HTEvent * event, int timeout)
{
    if (event) {
	event->millis = timeout;
	return YES;
    }
    return NO;
}

PUBLIC char * HTEvent_type2str(HTEventType type)
{
    int i;
    static char space[20]; /* in case we have to sprintf type */
    static struct {int type; char * str;} match[] = {HT_EVENT_INITIALIZER};
    for (i = 0; i < sizeof(match)/sizeof(match[0]); i++)
	if (match[i].type == type)
	    return match[i].str;
    sprintf(space, "0x%x", type);
    return space;
}

