/*								   HTErrorMsg.c
**	ERROR OUTPUT MODULE
**
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This module contains the error / information generating function 
**	HTErrorMsg() so that it can be overwritten by smart browsers and
**	servers.
**
** History:
**  	05 May 94	Written by Henrik Frystyk, frystyk@dxcern.cern.ch
*/

#include "sysdep.h"

/* Library include files */
#include "HTUtils.h"
#include "HTAccess.h"
#include "HTAlert.h"
#include "HTTCP.h"
#include "HTChunk.h"
#include "HTError.h"					 /* Implemented here */


/*								HTErrorMsg
**
**	Default function that creates an error message using HTAlert() to
**	put out the contents of the error_stack messages. Furthermore, the
**	error_info structure contains a name of a help file that might be put
**	up as a link. This file can then be multi-linguistic.
**
**	This function might be overwritten by a smart server or client.
*/
PUBLIC void HTErrorMsg ARGS1(HTRequest *, request)
{
    HTList *cur = request->error_stack;
    BOOL highest = YES;
    HTChunk *chunk;
    HTErrorInfo *pres;
    if (!request) {
	if (TRACE) fprintf(stderr, "HTErrorMsg.. Bad argument!\n");
	return;
    }

    /* This check is only necessary if the error message is put down the
       stream, because we have to know if a stream has been put up and/or
       taken down again. Here it is only put as an example */
#if 0
    if (request->error_block) {
	if (TRACE) fprintf(stderr, "HTErrorMsg.. No messages are printed as no stream is available.\n");
	return;
    }
#endif

    /* Output messages */
    chunk = HTChunkCreate(128);
    while ((pres = (HTErrorInfo *) HTList_nextObject(cur))) {

	/* Check if we are going to show the message */
	if ((!pres->ignore || HTErrorShowMask & HT_ERR_SHOW_IGNORE) && 
	    (HTErrorShowMask & pres->severity)) {

	    /* Output code number */
	    if (highest) {			    /* If first time through */
		if (TRACE)
		    fprintf(stderr,
			    "HTError..... Generating message.\n");
		
		/* Output title */
		if (pres->severity == ERR_WARNING)
		    HTChunkPuts(chunk, "Warning ");
		else if (pres->severity == ERR_NON_FATAL)
		    HTChunkPuts(chunk, "Non Fatal Error ");
		else if (pres->severity == ERR_FATAL)
		    HTChunkPuts(chunk, "Fatal Error ");
		else if (pres->severity == ERR_INFO)
		    HTChunkPuts(chunk, "Information ");
		else {
		    if (TRACE)
			fprintf(stderr, "HTError..... Unknown Classification of Error (%d)...\n", pres->severity);
		    HTChunkFree(chunk);
		    return;
		}

		/* Only output error code if it is a real HTTP code */
		if (pres->element < HTERR_HTTP_CODES_END) {
		    char codestr[10];
		    sprintf(codestr, "%d ", error_info[pres->element].code);
		    HTChunkPuts(chunk, codestr);
		}
		highest = NO;
	    } else
		HTChunkPuts(chunk, "\nReason: ");

	    /* Output error message */
	    if (pres->element != HTERR_SYSTEM) {
		HTChunkPuts(chunk, error_info[pres->element].msg);
		HTChunkPutc(chunk, ' ');
	    }

	    /* Output parameters */
	    if (pres->par && HTErrorShowMask & HT_ERR_SHOW_PARS) {
		int cnt;
		char ch;
		for (cnt=0; cnt<pres->par_length; cnt++) {
		    ch = *((char *)(pres->par)+cnt);
		    if (ch < 0x20 || ch >= 0x7F)
			HTChunkPutc(chunk, '#'); /* Can't print real content */
		    else
			HTChunkPutc(chunk, ch);
		}
	    }

	    /* Output location */
	    if (pres->where && HTErrorShowMask & HT_ERR_SHOW_LOCATION) {
		HTChunkPuts(chunk, "This occured in ");
		HTChunkPuts(chunk, pres->where);
		HTChunkPutc(chunk, '\n');
	    }

	    /* We don't want the message more than once */
	    HTErrorIgnore(request, pres->handle);
	    
	    /* If we only are going to show the higest entry */
	    if (HTErrorShowMask & HT_ERR_SHOW_FIRST)
		break;
	}
    }
    HTChunkPutc(chunk,  '\n');
    HTChunkTerminate(chunk);
    if (chunk->size > 2)
	HTAlert(chunk->data);
    HTChunkFree(chunk);
    return;
}

