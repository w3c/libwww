/*								      HTAlert.c
**	DIALOG MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	REPLACE THIS MODULE with a GUI version in a GUI environment!
**
** History:
**	   Jun 92 Created May 1992 By C.T. Barker
**	   Feb 93 Simplified, portablised TBL
**	   Sep 93 Corrected 3 bugs in HTConfirm() :-( AL
**	   Nov 95 Rewritten using callbacks HFN
*/

/* Library include files */
#include "WWWLib.h"
#include "HTError.h"					 /* Implemented here */
#include "HTAlert.h"					 /* Implemented here */

typedef struct _HTAlert {
    HTAlertCallback *	cbf;
    HTAlertOpcode	opcode;
} HTAlert;

struct _HTAlertPar {
    char *	message;
    char *	secret;
    void *	output;
};

PRIVATE BOOL HTInteractive=YES;		    /* Any prompts from the Library? */

PRIVATE HTList * HTMessages = NULL;	   /* Global list of alert functions */

/* ------------------------------------------------------------------------- */

/*
**	All messaging can be turned on or off as you like
*/
PUBLIC void HTAlert_setInteractive (BOOL interactive)
{
    HTInteractive = interactive;
}

PUBLIC BOOL HTAlert_interactive (void)
{
    return HTInteractive;
}

/*	HTAlertCall_add
**	---------------
**	Register a call back function that is to be called when generating
**	messages, dialog, prompts, progress reports etc.
**
**	The opcode signifies which call back function to call depending of the 
**	type of the message. Opcode can be one of the enumerations defined
**	by HTAlertOpcode.
*/
PUBLIC BOOL HTAlertCall_add (HTList * list, HTAlertCallback * cbf,
			     HTAlertOpcode opcode)
{
    if (WWWTRACE) 
	TTYPrint(TDEST, "Alert Add... HTAlertCallback %p\n", (void *) cbf);
    if (list && cbf) {
	HTAlert *me;
	if ((me = (HTAlert  *) HT_CALLOC(1, sizeof(HTAlert))) == NULL)
	    HT_OUTOFMEM("HTAlertCall_add");
	me->cbf = cbf;
	me->opcode = opcode;
	return HTList_addObject(list, (void *) me);
    }
    return NO;
}

/*	HTAlertCall_delete
**	------------------
**	Unregister a call back function from a list
*/
PUBLIC BOOL HTAlertCall_delete (HTList * list, HTAlertCallback *cbf)
{
    if (WWWTRACE) 
	TTYPrint(TDEST, "Call delete HTAlertCallback %p\n", (void *) cbf);
    if (list && cbf) {
	HTList *cur = list;
	HTAlert *pres;
	while ((pres = (HTAlert *) HTList_nextObject(cur))) {
	    if (pres->cbf == cbf) {
		HTList_removeObject(list, (void *) pres);
		HT_FREE(pres);
		return YES;
	    }
	}
    }
    return NO;
}

/*	HTAlertCall_deleteAll
**	---------------------
**	Unregisters all call back functions
*/
PUBLIC BOOL HTAlertCall_deleteAll (HTList * list)
{
    if (WWWTRACE) 
	TTYPrint(TDEST, "Call delete All callback functions\n");
    if (list) {
	HTList *cur = list;
	HTAlert *pres;
	while ((pres = (HTAlert *) HTList_nextObject(cur))) {
	    HTList_removeObject(list, (void *) pres);
	    HT_FREE(pres);
	}
	HTList_delete(list);
	return YES;
    }
    return NO;
}

/*	HTAlertCall_find
**	----------------
**	Finds a callback function corresponding to the opcode. If none has
**	been registered then NULL is returned.
*/
PUBLIC HTAlertCallback * HTAlertCall_find (HTList * list, HTAlertOpcode opcode)
{
    if (list && HTInteractive) {
	HTAlert * pres;
	while ((pres = (HTAlert *) HTList_nextObject(list)) != NULL) {
	    if (pres->opcode & opcode)
		return pres->cbf;
	}
	if (WWWTRACE)
	    TTYPrint(TDEST, "Alert Find.. No entry found for opcode %d\n",opcode);
    }
    return NULL;
}

/*
**	Global List of Alert functions. list can be NULL
*/
PUBLIC void HTAlert_setGlobal (HTList * list)
{
    HTMessages = list;
}

PUBLIC HTList * HTAlert_global (void)
{
    return HTMessages;
}

PUBLIC BOOL HTAlert_add (HTAlertCallback * cbf, HTAlertOpcode opcode)
{
    if (!HTMessages) HTMessages = HTList_new();
    return HTAlertCall_add(HTMessages, cbf, opcode);
}

PUBLIC BOOL HTAlert_delete (HTAlertCallback * cbf)
{
    if (!HTMessages) HTMessages = HTList_new();
    return HTAlertCall_delete(HTMessages, cbf);
}

/*	HTAlert_find
**	------------
**	Finds a global callback function corresponding to the opcode
*/
PUBLIC HTAlertCallback * HTAlert_find (HTAlertOpcode opcode)
{
    return HTAlertCall_find(HTMessages, opcode);
}

PUBLIC HTAlertPar * HTAlert_newReply (void)
{
    HTAlertPar * me;
    if ((me = (HTAlertPar  *) HT_CALLOC(1, sizeof(HTAlertPar))) == NULL)
        HT_OUTOFMEM("HTAlert_newReply");
    return me;
}

/*	HTAlert_deleteReply
**	-------------------
**	Delete reply structure but don't touch the replies themselves.
*/
PUBLIC void HTAlert_deleteReply (HTAlertPar * old)
{
    if (old) HT_FREE(old);
}

PUBLIC char * HTAlert_replyMessage (HTAlertPar * me)
{
    return me ? me->message : NULL;
}

PUBLIC BOOL HTAlert_setReplyMessage (HTAlertPar * me, CONST char * message)
{
    if (me && message) {
	StrAllocCopy(me->message, message);
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTAlert_assignReplyMessage (HTAlertPar * me, char * message)
{
    if (me) me->message = message;
    return YES;
}

PUBLIC char * HTAlert_replySecret (HTAlertPar * me)
{
    return me ? me->secret : NULL;
}

PUBLIC BOOL HTAlert_setReplySecret (HTAlertPar * me, CONST char * secret)
{
    if (me && secret) {
	StrAllocCopy(me->secret, secret);
	return YES;
    }
    return NO;
}

PUBLIC void * HTAlert_replyOutput (HTAlertPar * me)
{
    return me ? me->output : NULL;
}

PUBLIC BOOL HTAlert_setReplyOutput (HTAlertPar * me, void * output)
{
    if (me) {
	me->output = output;
	return YES;
    }
    return NO;
}

/*	TTYPrint
**	--------
**	Single function through which all trace messages must pass - EGP
*/
#if WWWTRACE_MODE == WWWTRACE_TTY && !defined(WWW_WIN_WINDOW)
int TTYPrint(FILE* file, CONST char* fmt, ...)
{
    int len;
    char space[513];
    char* pArgs;

    pArgs = (char*)(&fmt + 1);
    len = vsprintf(space, (char*)fmt, (char*)pArgs);
    fprintf(file, space);
    return (len);
}
/* otherwise handled in www.c with the rest of the window stuff */
#endif

