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
    HTTimer *	next;		/* The next guy in line */
    int		millis;
    int		expires;	/* Abs value in millis */
    BOOL	relative;
    void *	param;		/* Client supplied context */
    HTTimerCallback * cbf;
};

PRIVATE HTTimer * timers = NULL;			   /* List of timers */

/* ------------------------------------------------------------------------- */

PUBLIC BOOL HTTimer_delete (HTTimer * timer)
{
    if (timer && timers) {
	HTTimer ** prev;
	for (prev = &timers; *prev; prev = &(*prev)->next) {
	    if (*prev == timer) {
		*prev = timer->next;
		break;
	    }
	}
	if (THD_TRACE) HTTrace("Timer....... Deleted timer %p\n", timer);
	HT_FREE(timer);
	return YES;
    }
    return NO;
}

PUBLIC HTTimer * HTTimer_new (HTTimer * timer, HTTimerCallback * cbf,
			      void * param, int millis, BOOL relative)
{
    HTTimer ** prev;
    int now = HTGetTimeInMillis();
    if (!timer) {
	if ((timer = (HTTimer *) HT_CALLOC(1, sizeof(HTTimer))) == NULL)
	    HT_OUTOFMEM("HTTimer_set");
	if (THD_TRACE) HTTrace("Timer....... Created timer %p\n", timer);
    } else {
	for (prev = &timers; *prev; prev = &(*prev)->next) {
	    if (*prev == timer) {
		*prev = timer->next;
		break;
	    }
	}
    }
    if (!millis) return timer;
    timer->expires = millis;
    if (relative) timer->expires += now;
    timer->cbf = cbf;
    timer->param = param;
    timer->millis = millis;
    timer->relative = relative;
    if (timer->expires <= now) {
	int status;
	timer->next = NULL;
	if ((status = (*timer->cbf)(timer, timer->param)) != HT_OK) {
	    HTTimer_delete(timer);
	    return NULL;
	}
    }
    for (prev = &timers;
	 *prev && millis > (*prev)->expires;
	 prev = &(*prev)->next);
    timer->next = *prev;
    *prev = timer;
    return timer;
}


PUBLIC BOOL HTTimer_deleteAll (void)
{
    if (timers) {
	HTTimer * timer = NULL;
	while ((timer = timers)) {
	    timers = timers->next;
	    HT_FREE(timer);
	}
	return YES;
    }
    return NO;
}

/*
**  When a timer has expired, we dispatch the event handler and re-register the
**  timer with the next expiration time.
*/
PRIVATE int HTTimer_dispatch (HTTimer * timer, int now, HTTimer ** prev)
{
    *prev = timer->next;
    timer->next = NULL;
    if (timer->relative)
	HTTimer_new(timer, timer->cbf, timer->param, timer->millis, YES);
    if (THD_TRACE) HTTrace("Timer....... Dispatch timer %p\n", timer);
    return (*timer->cbf) (timer, timer->param);
}

PUBLIC int HTTimer_soonest (void)
{
    int now;
    if (timers == NULL) return 0;
    now = HTGetTimeInMillis();
    while (timers && timers->expires <= now)
	HTTimer_dispatch(timers, now, &timers);
    if (timers)
	return timers->expires - now;
    return 0;
}

PUBLIC int HTTimer_dispatchAll (void)
{
    int now;
    if (timers) {
	now = HTGetTimeInMillis();
	while (timers && timers->expires <= now)
	    HTTimer_dispatch(timers, now, &timers);
    }
    return HT_OK;
}
