/*
**	TIMER MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Timers based on the X server timers
**
** Authors:
**	EGP	Eric Prud'hommeaux (eric@w3.org)
**	HFN	Henrik Frystyk Nielsen
** Bugs
**
*/

/* Implementation dependent include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTReqMan.h"
#include "HTTimer.h"					 /* Implemented here */

struct _HTTimer {
    ms_t	millis;		/* Relative value in millis */
    ms_t	expires;	/* Absolute value in millis */
    BOOL	relative;
    BOOL	repetitive;
    void *	param;		/* Client supplied context */
    HTTimerCallback * cbf;
};

PRIVATE HTList * Timers = NULL;			   /* List of timers */

PRIVATE HTTimerSetCallback * SetPlatformTimer = NULL;
PRIVATE HTTimerSetCallback * DeletePlatformTimer = NULL;

#ifdef WATCH_RECURSION

PRIVATE HTTimer * InTimer = NULL;
#define CHECKME(timer) if (InTimer != NULL) HTDEBUGBREAK("check timer\n"); InTimer = timer;
#define CLEARME(timer) if (InTimer != timer) HTDEBUGBREAK("clear timer\n"); InTimer = NULL;
#define SETME(timer) InTimer = timer;

#else /* WATCH_RECURSION */

#define CHECKME(timer)
#define CLEARME(timer)
#define SETME(timer)

#endif /* !WATCH_RECURSION */

/* JK: used by Amaya */
PUBLIC BOOL HTTimer_expireAll (void)
{
  HTList * cur;
  HTTimer * timer;
  if (Timers) {
    /*
    **  first delete all plattform specific timers to
    **  avoid having a concurrent callback
    */
    cur = Timers;
    while ((timer = (HTTimer *) HTList_nextObject(cur))) {
      if (DeletePlatformTimer) DeletePlatformTimer(timer);
    }
 
    /*
    ** simulate a timer timeout thru timer_dispatch
    ** to kill its context
    */
    cur = Timers;
    while ((timer = (HTTimer *) HTList_nextObject(cur))) {
          /* avoid having it being refreshed */
      timer->repetitive = NO;
      HTTimer_dispatch (timer);
      /* as the timer is erased, we start again from the top of the list */
      cur = Timers;
    }
    return YES;
  }
  return NO;
}

/* ------------------------------------------------------------------------- */

/*
**  When a timer has expired, we dispatch the event handler and re-register the
**  timer with the next expiration time if repetitive. Otherwise we just leave
**  it
*/
PRIVATE int Timer_dispatch (HTList * cur, HTList * last)
{
    HTTimer * timer;
    int ret = HT_ERROR;

    timer = (HTTimer *)HTList_objectOf(cur);
    if (timer == NULL) {
#if 0
        HTDEBUGBREAK("Timer dispatch couldn't find a timer\n");
#endif
        CLEARME(timer);
	return HT_ERROR;
    }
#ifdef WWW_WIN_ASYNC
    /* 2000/07/31 Jens Meggers (meggers@firepad.com):
       On Windows, timers are always repetitive, so we have to delete the 
       timer */
    if (DeletePlatformTimer)
      DeletePlatformTimer(timer);
#endif /* WWW_WIN_ASYNC */
    if (timer->repetitive)
	HTTimer_new(timer, timer->cbf, timer->param, timer->millis, YES, YES);
    else
	HTList_quickRemoveElement(cur, last);
    HTTRACE(THD_TRACE, "Timer....... Dispatch timer %p\n" _ timer);
    ret = (*timer->cbf) (timer, timer->param, HTEvent_TIMEOUT);
    return ret;
}

PUBLIC BOOL HTTimer_registerSetTimerCallback (HTTimerSetCallback * cbf)
{
    HTTRACE(CORE_TRACE, "Timer....... registering %p as timer set cbf\n" _ cbf);
    if (cbf) {
	SetPlatformTimer = cbf;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTTimer_registerDeleteTimerCallback (HTTimerSetCallback * cbf)
{
    HTTRACE(CORE_TRACE, "Timer....... registering %p as timer delete cbf\n" _ cbf);
    if (cbf) {
	DeletePlatformTimer = cbf;
	return YES;
    }
    return NO;
}

PUBLIC ms_t HTTimer_expiresRelative (HTTimer * timer)
{
    return timer ? timer->millis : 0;
}

PUBLIC ms_t HTTimer_expiresAbsolute (HTTimer * timer)
{
    return timer ? timer->expires : 0;
}

PUBLIC HTTimerCallback * HTTimer_callback (HTTimer * timer)
{
    return timer ? timer->cbf : NULL;
}

PUBLIC BOOL HTTimer_isRelative (HTTimer * timer)
{
    return timer ? timer->relative : NO;
}

PUBLIC BOOL HTTimer_delete (HTTimer * timer)
{
    HTList * last;
    HTList * cur;
    CHECKME(timer);
    if ((cur = HTList_elementOf(Timers, (void *)timer, &last)) == NULL) CLEARME(timer);
    if (HTList_quickRemoveElement(cur, last)) {
	HTTRACE(THD_TRACE, "Timer....... Deleted active timer %p\n" _ timer);
    } else { 
	HTTRACE(THD_TRACE, "Timer....... Deleted expired timer %p\n" _ timer);
    }

    /*
    **  Call any platform specific timer handler
    */
    if (DeletePlatformTimer) DeletePlatformTimer(timer);

    CLEARME(timer);
    HT_FREE(timer);
    return YES;
}

PUBLIC HTTimer * HTTimer_new (HTTimer * timer, HTTimerCallback * cbf,
			      void * param, ms_t millis, BOOL relative,
			      BOOL repetitive)
{
    HTList * last;
    HTList * cur;
    ms_t now = HTGetTimeInMillis();
    ms_t expires;
    HTTimer * pres;

    CHECKME(timer);
    expires = millis;
    if (relative)
	expires += now;
    else
	millis = expires-now;

    if (Timers == NULL)
	Timers = HTList_new();

    if (timer) {

	/*	if a timer is specified, it should already exist
	 */
	if ((cur = HTList_elementOf(Timers, (void *)timer, &last)) == NULL) {
	    HTDEBUGBREAK("Timer %p not found\n" _ timer);
	    CLEARME(timer);
	    return NULL;
	}
	HTList_quickRemoveElement(cur, last);
	HTTRACE(THD_TRACE, "Timer....... Found timer %p with callback %p, context %p, and %s timeout %d\n" _ 
		    timer _ cbf _ param _ relative ? "relative" : "absolute" _ millis);
	/* could optimize by sorting from last when ((HTList *)(last->object))->expires < expires (most common case) */
    } else {

	/*	create a new timer
	 */
	if ((timer = (HTTimer *) HT_CALLOC(1, sizeof(HTTimer))) == NULL)
	    HT_OUTOFMEM("HTTimer_new");
	last = Timers;
	HTTRACE(THD_TRACE, "Timer....... Created %s timer %p with callback %p, context %p, and %s timeout %d\n" _ 
		    repetitive ? "repetitive" : "one shot" _ 
		    timer _ cbf _ param _ 
		    relative ? "relative" : "absolute" _ millis);
    }

    /*
    **  Sort new element into list
    */
    for (cur = last; 
	 (pres = (HTTimer *) HTList_nextObject(cur)) != NULL && pres->expires < expires; 
	 last = cur);

    /*
    **  If the expiration is 0 then we still register it but dispatch it immediately.
    */
    if (!millis) HTTRACE(THD_TRACE, "Timer....... Timeout is 0 - expires NOW\n");

    timer->expires = expires;
    timer->cbf = cbf;
    timer->param = param;
    timer->millis = millis;
    timer->relative = relative;
    timer->repetitive = repetitive;
    SETME(timer);

    /*
    **	add to list if timer is new
    */
    cur = HTList_addList(last, (void *)timer);

    /*
    **  Call any platform specific timer handler
    */
    if (SetPlatformTimer) SetPlatformTimer(timer);

    /* Check if the timer object has already expired. If so then dispatch */
    if (timer->expires <= now) Timer_dispatch(cur, last);

    CLEARME(timer);
    return timer;
}


PUBLIC BOOL HTTimer_refresh (HTTimer * timer, ms_t now)
{
    if (timer == NULL || timer->repetitive == NO)
	return NO;
    if (HTTimer_new(timer, timer->cbf, timer->param, timer->millis, YES, YES) == NULL)
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

PUBLIC int HTTimer_dispatch (HTTimer * timer)
{
    HTList * cur;
    HTList * last = Timers;
    cur = HTList_elementOf(Timers, (void *)timer, &last);
    return Timer_dispatch(cur, last);
}

/*
**  Check if the timer object has already expired
*/
PUBLIC BOOL HTTimer_hasTimerExpired (HTTimer * timer)
{
    return (timer && timer->expires <= HTGetTimeInMillis());
}

PUBLIC int HTTimer_next (ms_t * pSoonest)
{
    HTList * cur = Timers;
    HTList * last = Timers;
    HTTimer * pres;
    ms_t now = HTGetTimeInMillis();
    int ret = HT_OK;

    /*
    **  Dispatch all timers that have expired
    */
    while (Timers && (pres = (HTTimer *) HTList_nextObject(cur))) {
	if (pres->expires <= now) {
	    if ((ret = Timer_dispatch(cur, last)) != HT_OK) break;
	    cur = last = Timers;
	} else {
	    last = cur;
	}	
    }

    if (pSoonest) {
	/*
	**	First element in Timers is the next to expire.
	*/
	HTList * cur = Timers;	/* for now */
	pres = (HTTimer *) HTList_nextObject(cur);
	*pSoonest = pres ? pres->expires - now : 0;
    }
    return ret;
}

#ifdef WATCH_RECURSION
extern void CheckSockEvent(HTTimer * timer, HTTimerCallback * cbf, void * param);
PRIVATE void CheckTimers(void)
{
    HTList * cur = Timers;
    HTTimer * pres;
    while ((pres = (HTTimer *) HTList_nextObject(cur))) {
	CheckSockEvent(pres, pres->cbf, pres->param);
    }
}
#endif


