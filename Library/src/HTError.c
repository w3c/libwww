/*								      HTError.c
**	ERROR REPORT MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This is the implementaion of an error message reporting system that 
**	reports errors occured either in a stream module (structured streams
**	inclusive) or in a protocol module. A list of errors are put into the
**	a list which can be bound to a request object or a stream
**
** History:
**  	05 May 94	Written by Henrik Frystyk, frystyk@w3.org
**	   Nov 95	Made list as basic data structure
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTInet.h"
#include "HTError.h"					 /* Implemented here */

struct _HTError {
    HTErrorElement 	element;        /* Index number into HTError */
    HTSeverity 		severity; 	/* A la VMS */
    BOOL               	ignore;         /* YES if msg should not go to user */
    void *  		par;          	/* Explanation, e.g. filename  */
    int 		length;   	/* For copying by generic routine */
    char *       	where;          /* Which function */
};

PRIVATE HTErrorShow HTShowMask = HT_ERR_SHOW_DEFAULT;

/* ------------------------------------------------------------------------- */

/*	HTError_add
**	-----------
**	Add an error message to the error list. `par' and `where'
**	might be set to NULL. If par is a string, it is sufficient to let
**	length be unspecified, i.e., 0. If only a part of the string is
**	wanted then specify a length inferior to strlen((char *) par).
**	The string is '\0' terminated automaticly.
**	NOTE: See also HTError_addSystem for system errors
**	Return YES if OK, else NO
*/
PUBLIC BOOL HTError_add (HTList * 	list,
			 HTSeverity	severity,
			 BOOL		ignore,
			 int		element,
			 void *		par,
			 unsigned int	length,
			 char *		where)
{
    HTError *newError;
    if (!list) return NO;
    if ((newError = (HTError *) HT_CALLOC(1, sizeof(HTError))) == NULL)
        HT_OUTOFMEM("HTError_add");
    newError->element = (HTErrorElement) element;
    newError->severity = severity;
    newError->ignore = ignore;
    if (par) {
	if (!length) length = (int) strlen((char *) par);
	if ((newError->par = HT_MALLOC(length+1)) == NULL)
	    HT_OUTOFMEM("HTErrorError");
	memcpy(newError->par, par, length);
	*(((char *) newError->par)+length) = '\0';
	newError->length = length;
    }
    newError->where = where;
    HTTRACE(CORE_TRACE, "Error....... Add %3d\tSeverity: %d\tParameter: `%s\'\tWhere: `%s\'\n" _
	    element _
	    newError->severity _
	    newError->par ? (char *) newError->par : "Unspecified" _
	    newError->where ? newError->where : "Unspecified");
    return HTList_addObject(list, (void *) newError);
}


/*	HTError_addSystem
**	-----------------
**	Add a system error message to the error list. syscall
**	is the name of the system call, e.g. "close". The message put to the
**	list is that corresponds to the error number passed. 
**	Returns YES if OK, else NO
**	See also HTError_add.
*/
PUBLIC BOOL HTError_addSystem (HTList *		list,
			       HTSeverity	severity,
			       int		errornumber,
			       BOOL		ignore,
			       char *		syscall)
{
    BOOL status = NO;
    if (list) {
	char * errsysmsg = HTErrnoString(errornumber);
	status = HTError_add(list, severity, ignore, HTERR_SYSTEM,
			     errsysmsg, errsysmsg ? (int) strlen(errsysmsg) : 0,
			     syscall ? syscall : "unknown");
	HT_FREE(errsysmsg);
    }
    return status;
}

/*	HTError_deleteAll
**	-----------------
**	Remove all errors from the list
*/
PUBLIC BOOL HTError_deleteAll (HTList * list)
{
    if (list) {
	HTList *cur = list;
	HTError *pres;
	while ((pres = (HTError *) HTList_nextObject(cur))) {
	    HT_FREE(pres->par);
	    HT_FREE(pres);
	}
	HTList_delete(list);
	return YES;
    }
    return NO;
}

/*	HTError_deleteLast
**	------------------
**	Deletes the last error entry added to the list
**	Return YES if OK, else NO
*/
PUBLIC BOOL HTError_deleteLast (HTList * list)
{
    if (list) {
	HTError * old = (HTError *) HTList_removeLastObject(list);
	if (old) {
	    HTTRACE(CORE_TRACE, "Error....... Delete %p\n" _ old);
	    HT_FREE(old->par);
	    HT_FREE(old);
	    return YES;
	}
    }
    return NO;
}

/*	HTError_ignoreLast
**	------------------
**	Turns on the `ignore' flag for the most recent error entered the
**	error list. Returns YES if OK else NO
*/
PUBLIC BOOL HTError_ignoreLast (HTList * list)
{
    if (list) {
	HTError * last = (HTError *) HTList_lastObject(list);
	if (last) {
	    HTTRACE(CORE_TRACE, "Error....... Ignore %p\n" _ last);
	    last->ignore = YES;
	    return YES;
	}
    }
    return NO;
}

/*	HTError_setIgnore
**	-----------------
*/
PUBLIC BOOL HTError_setIgnore (HTError * info)
{
    if (info) {
	info->ignore = YES;
	return YES;
    }
    return NO;
}

/*
**	Get show preferences
*/
PUBLIC HTErrorShow HTError_show (void)
{
    return HTShowMask;
}

PUBLIC BOOL HTError_setShow (HTErrorShow mask)
{
    HTShowMask = mask;
    return YES;
}

/*
**	Should we show this error entry?
*/
PUBLIC BOOL HTError_doShow (HTError *info)
{
    return (info && ((HTShowMask & info->severity) &&
		     (!info->ignore || HTShowMask & HT_ERR_SHOW_IGNORE)));
}

/*
**	Various Fields in the HTError structure
*/
PUBLIC HTSeverity HTError_severity (HTError *info)
{
    return info ? info->severity : ERR_UNKNOWN;
}


/*
**	Any error more serious than this?
*/
PUBLIC BOOL HTError_hasSeverity (HTList * list, HTSeverity severity)
{
    if (list) {
	HTList * cur = list;
	HTError * pres;
	while ((pres = (HTError *) HTList_nextObject(cur))) {
	    if (pres->severity < severity) {
		HTTRACE(CORE_TRACE, "Severity.... Found a severe error\n");
		return YES;
	    }
	}
    }
    return NO;
}

PUBLIC int HTError_index (HTError * info)
{
    return info ? info->element : HTERR_INTERNAL;
}

PUBLIC void * HTError_parameter (HTError * info, int * length)
{
    if (info) {
	*length = info->length;
	return info->par;
    }
    return NULL;
}

PUBLIC const char * HTError_location (HTError * info)
{
    return info ? info->where : NULL;
}
