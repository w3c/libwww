/*				    				     HTEvntrg.c
**	EVENT MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Updated HTEvent module 
**	This new module combines the functions of the old HTEvent module and 
**	the HTThread module. We retain the old HTThread module, but it
**	consists of calls to the HTEvent interfaces
**
** Authors:
**	EGP	Eric Prud'hommeaux (eric@w3.org)
** Bugs
**
*/

/* Implementation dependent include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTReqMan.h"
#include "HTTimer.h"					 /* Implemented here */

struct _HTTimer {
    ms_t	millis;		/* Relative value in millis */
    ms_t	expires;	/* Absolute value in millis */
    BOOL	relative;
    void *	param;		/* Client supplied context */
    HTTimerCallback * cbf;
};

PRIVATE HTList * Timers = NULL;			   /* List of timers */

PRIVATE HTTimerSetCallback * SetPlatformTimer = NULL;
PRIVATE HTTimerSetCallback * DeletePlatformTimer = NULL;

#if 1 /* WATCH_RECURSION */

PRIVATE HTTimer * InTimer = NULL;
#define CHECKME(timer) if (InTimer != NULL) HTDebugBreak(); InTimer = timer;
#define CLEARME(timer) if (InTimer != timer) HTDebugBreak(); InTimer = NULL;
#define SETME(timer) InTimer = timer;

#else /* WATCH_RECURSION */

#define CHECKME(timer)
#define CLEARME(timer)
#define SETME(timer)

#endif /* !WATCH_RECURSION */
/* ------------------------------------------------------------------------- */

PUBLIC BOOL HTTimer_registerSetTimerCallback (HTTimerSetCallback * cbf)
{
    if (CORE_TRACE) HTTrace("Timer....... registering %p as timer set cbf\n", cbf);
    if (cbf) {
	SetPlatformTimer = cbf;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTTimer_registerDeleteTimerCallback (HTTimerSetCallback * cbf)
{
    if (CORE_TRACE) HTTrace("Timer....... registering %p as timer delete cbf\n", cbf);
    if (cbf) {
	DeletePlatformTimer = cbf;
	return YES;
    }
    return NO;
}

PUBLIC ms_t HTTimer_getTime(HTTimer * timer)
{
    if (timer)
	return timer->millis;
    return 0;
}

PUBLIC BOOL HTTimer_delete (HTTimer * timer)
{
    HTList * last;
    HTList * cur;
    CHECKME(timer);
    if ((cur = HTList_elementOf(Timers, (void *)timer, &last)) == NULL) {
	HTDebugBreak();
	CLEARME(timer);
	return NO;
    }
    if (HTList_quickRemoveElement(cur, last))
	if (THD_TRACE) HTTrace("Timer....... Deleted timer %p\n", timer);
    else
	if (THD_TRACE) HTTrace("Timer....... Could not delete timer %p\n", timer);

    /*
    **  Call any platform specific timer handler
    */
    if (DeletePlatformTimer) DeletePlatformTimer(timer);

    CLEARME(timer);
    HT_FREE(timer);
    return YES;
}

PUBLIC HTTimer * HTTimer_new (HTTimer * timer, HTTimerCallback * cbf,
			      void * param, ms_t millis, BOOL relative)
{
    HTList * last;
    HTList * cur;
    ms_t now = HTGetTimeInMillis();
    ms_t expires;
    HTTimer * pres;

    CHECKME(timer);
    expires = millis;
    if (relative) expires += now;

    if (Timers == NULL)
	Timers = HTList_new();

    if (timer) {

	/*	if a timer is specified, it should already exist
	 */
	if ((cur = HTList_elementOf(Timers, (void *)timer, &last)) == NULL) {
	    HTDebugBreak();
	    CLEARME(timer);
	    return NULL;
	}
	HTList_quickRemoveElement(cur, last);
	if (THD_TRACE)
	    HTTrace("Timer....... Found timer %p with callback %p, context %p, and %s timeout %d\n",
		    timer, cbf, param, relative ? "relative" : "absolute", millis);
	/* could optimize by sorting from last when ((HTList *)(last->object))->expires < expires (most common case) */
    } else {

	/*	create a new timer
	 */
	if ((timer = (HTTimer *) HT_CALLOC(1, sizeof(HTTimer))) == NULL)
	    HT_OUTOFMEM("HTTimer_new");
	last = Timers;
	if (THD_TRACE)
	    HTTrace("Timer....... Created timer %p with callback %p, context %p, and %s timeout %d\n",
		    timer, cbf, param, relative ? "relative" : "absolute", millis);
    }
    /*	sort new element into list
     */
    for (cur = last; 
	 (pres = (HTTimer *) HTList_nextObject(cur)) != NULL && pres->expires < expires; 
	 last = cur);
    if (!millis) return timer;
    timer->expires = expires;
    timer->cbf = cbf;
    timer->param = param;
    timer->millis = millis;
    timer->relative = relative;
    SETME(timer);
    /*	may already be obsolete
     */
    if (timer->expires <= now) {
	int status;
	if ((status = (*timer->cbf)(timer, timer->param, HTEvent_TIMEOUT)) != HT_OK) {
	    if (cur)
		HTList_quickRemoveElement(cur, last);
	    HT_FREE(timer);
	    CLEARME(timer);
	    return NULL;
	}
    }

    /*
    **	add to list if timer is new
    */
    HTList_addObject(last, (void *)timer);

    /*
    **  Call any platform specific timer handler
    */
    if (SetPlatformTimer) SetPlatformTimer(timer);

    CLEARME(timer);
    return timer;
}


PUBLIC BOOL HTTimer_refresh (HTTimer * timer, ms_t now)
{
    if (timer == NULL || timer->relative == NO)
	return NO;
    if (HTTimer_new(timer, timer->cbf, timer->param, timer->millis, YES) == NULL)
	return NO;
    return YES;
}

PUBLIC BOOL HTTimer_deleteAll (void)
{
    HTList * cur = Timers;
    HTTimer * pres;
    if (Timers) {
	while ((pres = (HTTimer *) HTList_nextObject(cur))) {

	    /*
	    **  Call any platform specific timer handler
	    */
	    if (DeletePlatformTimer) DeletePlatformTimer(pres);
	    HT_FREE(pres);
	}
	HTList_delete(Timers);
	Timers = NULL;
	return YES;
    }
    return NO;
}

/*
**  When a timer has expired, we dispatch the event handler and re-register the
**  timer with the next expiration time.
*/
PRIVATE int Timer_dispatch (HTList * cur, HTList * last, int now)
{
    HTTimer * timer;
    int ret;

    timer = (HTTimer *)HTList_objectOf(cur);
    if (timer == NULL) {
	HTDebugBreak();
	CLEARME(timer);
	return HT_ERROR;
    }
    if (timer->relative)
	HTTimer_new(timer, timer->cbf, timer->param, timer->millis, YES);
    else
	HTList_quickRemoveElement(cur, last);
    if (THD_TRACE) HTTrace("Timer....... Dispatch timer %p\n", timer);
/*    CHECKME(timer); all entries to this function are now re-entry save */
    ret = (*timer->cbf) (timer, timer->param, HTEvent_TIMEOUT);
/*    CLEARME(timer); */
    if (!timer->relative)
	HT_FREE(timer);
    return ret;
}

PUBLIC int HTTimer_dispatch (HTTimer * timer)
{
    HTList * cur;
    HTList * last = Timers;
    ms_t now = HTGetTimeInMillis();

    cur = HTList_elementOf(Timers, (void *)timer, &last);
    return Timer_dispatch(cur, last, now);
}

PUBLIC int HTTimer_next (ms_t * pSoonest)
{
    HTList * cur;
    HTList * last;
    HTTimer * pres;
    ms_t now = HTGetTimeInMillis();
    int ret = HT_OK;
    HTList * head;

    if (Timers == NULL)
	return HT_OK;

    /*	The Timers list may be modified during a dispatch
    **	so we have to build an intermediate list
    */
    head = last = HTList_new();
    cur = Timers;
    while ((pres = (HTTimer *) HTList_nextObject(cur)) && pres->expires <= now) {
	HTList_addObject(last, (void *)pres);
	last = HTList_nextObject(last);
    }

    /*
    **	Now dispatch the intermediate list
    */
    cur = last = head;
    while ((pres = (HTTimer *) HTList_nextObject(cur)) && ret == HT_OK) {
	ret = Timer_dispatch(cur, last, now);
	last = cur;
    }

    if (pSoonest) {
	/*
	**	First element in Timers is the next to expire.
	*/
	HTList * cur = Timers;	/* for now */
	pres = (HTTimer *) HTList_nextObject(cur);
	*pSoonest = pres ? pres->expires - now : 0;
    }
    HTList_delete(head);
    return ret;
}
