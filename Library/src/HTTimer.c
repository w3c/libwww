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

/* ------------------------------------------------------------------------- */

#ifdef WWW_WIN_ASYNC

#define SET_PLATFORM_TIMER(timer)	Timer_setWindowsTimer(timer)
#define DELETE_PLATFORM_TIMER(timer)	Timer_deleteWindowsTimer(timer)

PRIVATE int Timer_setWindowsTimer(HTTimer * timer)
{
    HWND hwnd;
    UINT id;
    hwnd = HTEventList_getWinHandle(&id);
    return SetTimer(hwnd, (UINT)timer, (UINT)timer->millis, NULL) != 0;
}

PRIVATE int Timer_deleteWindowsTimer(HTTimer * timer)
{
    HWND hwnd;
    UINT id;
    hwnd = HTEventList_getWinHandle(&id);
    return KillTimer(hwnd, (UINT)timer) != 0;
}

#else /* WWW_WIN_ASYNC */

#define SET_PLATFORM_TIMER(timer)
#define DELETE_PLATFORM_TIMER(timer)

#endif /* !WWW_WIN_ASYNC */

PUBLIC BOOL HTTimer_delete (HTTimer * timer)
{
    HTList * last;
    HTList * cur;
    if ((cur = HTList_elementOf(Timers, (void *)timer, &last)) == NULL)
	return NO;
    HTList_quickRemoveElement(cur, last);
    DELETE_PLATFORM_TIMER(timer);
    if (THD_TRACE) HTTrace("Timer....... Deleted timer %p\n", timer);
    HT_FREE(timer);
    return YES;
}

PUBLIC HTTimer * HTTimer_new (HTTimer * timer, HTTimerCallback * cbf,
			      void * param, ms_t millis, BOOL relative)
{
    HTList * last = Timers;
    HTList * cur = NULL;	/* will serve to flag newly created timers */
    ms_t now = HTGetTimeInMillis();
    ms_t expires;

    expires = millis;
    if (relative) expires += now;

    if (Timers == NULL)
	Timers = HTList_new();

    if (timer) {

	/*	if a timer is specified, it should already exist
	 */
	if ((cur = HTList_elementOf(Timers, (void *)timer, &last)) == NULL)
	    return NULL;
    } else {

	/*	create a new timer
	 */
	HTTimer * pres;
	if ((timer = (HTTimer *) HT_CALLOC(1, sizeof(HTTimer))) == NULL)
	    HT_OUTOFMEM("HTTimer_new");

	/*	sort new element into list
	 */
	for (last = cur = Timers; 
	     (pres = (HTTimer *) HTList_nextObject(cur)) != NULL && pres->expires < expires; 
	     last = cur);
	if (THD_TRACE)
	    HTTrace("Timer....... Created timer %p with callback %p, context %p, and %s timeout %d\n",
		    timer, cbf, param, relative ? "relative" : "absolute", millis);
    }
    if (!millis) return timer;
    timer->expires = expires;
    timer->cbf = cbf;
    timer->param = param;
    timer->millis = millis;
    timer->relative = relative;

    /*	may already be obsolete
     */
    if (timer->expires <= now) {
	int status;
	if ((status = (*timer->cbf)(timer, timer->param)) != HT_OK) {
	    if (cur)
		HTList_quickRemoveElement(cur, last);
	    HT_FREE(timer);
	    return NULL;
	}
    }

#if 0
    for (prev = &timers;
	 *prev && millis > (*prev)->millis;
	 prev = &(*prev)->next);
    timer->next = *prev;
    *prev = timer;
#endif

    /*
    **	add to list if timer is new
    */
    if (cur == NULL) HTList_addObject(last, (void *)timer);
    SET_PLATFORM_TIMER(timer);
    return timer;
}


PUBLIC BOOL HTTimer_deleteAll (void)
{
    HTList * cur = Timers;
    HTTimer * pres;
    if (Timers) {
	while ((pres = (HTTimer *) HTList_nextObject(cur))) {
	    DELETE_PLATFORM_TIMER(pres);
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
    if (timer == NULL)
	return HT_ERROR;
    if (timer->relative)
	HTTimer_new(timer, timer->cbf, timer->param, timer->millis, YES);
    else
	HTList_quickRemoveElement(cur, last);
    if (THD_TRACE) HTTrace("Timer....... Dispatch timer %p\n", timer);
    ret = (*timer->cbf) (timer, timer->param);
    if (!timer->relative)
	HT_FREE(timer);
    return ret;
}

PUBLIC int HTTimer_dispatch (HTTimer * timer)
{
    HTList * cur;
    HTList * last = Timers;
    HTTimer * pres;
    ms_t now = HTGetTimeInMillis();

    cur = HTList_elementOf(Timers, (void *)timer, &last);
    return Timer_dispatch(cur, last, now);
}

PUBLIC ms_t HTTimer_soonest (void)
{
    HTList * cur = Timers;
    HTList * last = Timers;
    HTTimer * pres = NULL;
    ms_t now = HTGetTimeInMillis();
    int ret;

    if (Timers == NULL)
	return 0;

    while ((pres = (HTTimer *) HTList_nextObject(cur)) && pres->expires <= now) {
	if ((ret = Timer_dispatch(cur, last, now)) != HT_OK)
	    return ret;
	last = cur;
    }

    return pres ? pres->expires - now : 0;
}

PUBLIC int HTTimer_dispatchAll (void)
{
    HTList * cur = Timers;
    HTList * last = Timers;
    HTTimer * pres;
    ms_t now = HTGetTimeInMillis();
    int ret;

    if (Timers) {
	/*	The Timers list may be modified during a dispatch
	**	so we have to build an intermediate list
	*/
	HTList * head = HTList_new();
	while ((pres = (HTTimer *) HTList_nextObject(cur)))
	    HTList_appendObject(head, (void *)pres);
	cur = last = head;
	while ((pres = (HTTimer *) HTList_nextObject(cur))) {
	    if ((ret = Timer_dispatch(cur, last, now)) != HT_OK)
		return ret;
	    last = cur;
	}
	return HT_OK;
    }
    return HT_ERROR;
}
