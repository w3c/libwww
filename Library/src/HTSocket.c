/*								     HTSocket.c
**	MANAGES READ AND WRITE TO AND FROM THE NETWORK
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**
** HISTORY:
**	6 June 95  HFN	Spawned off from HTFormat
*/

/* Library Include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTReqMan.h"
#include "HTProt.h"
#include "HTTCP.h"
#include "HTStream.h"
#include "HTAlert.h"
#include "HTFormat.h"
#include "HTNetMan.h"
#include "HTError.h"
#include "HTSocket.h"					 /* Implemented here */

struct _HTInputSocket {
    char	buffer[INPUT_BUFFER_SIZE];
    char *	write;					/* Last byte written */
    char *	read;					   /* Last byte read */
    SOCKET	sockfd;
};

struct _HTStream {
    CONST HTStreamClass *	isa;
};

/* ------------------------------------------------------------------------- */
/* 				SOCKET INPUT BUFFERING			     */
/* ------------------------------------------------------------------------- */
/*			
**	This code is used because one cannot in general open a
**	file descriptor for a socket.
**
**	The input file is read using the macro which can read from
**	a socket or a file, but this should not be used for files
**	as fopen() etc is more portable of course.
**
**	The input buffer size, if large will give greater efficiency and
**	release the server faster, and if small will save space on PCs etc.
*/


/*	Set up the buffering
**
**	These routines are public because they are in fact needed by
**	many parsers, and on PCs and Macs we should not duplicate
**	the static buffer area.
*/
PUBLIC HTInputSocket * HTInputSocket_new (SOCKET file_number)
{
    HTInputSocket *isoc;
    if ((isoc = (HTInputSocket  *) HT_CALLOC(1, sizeof(*isoc))) == NULL)
        HT_OUTOFMEM("HTInputSocket_new");
    isoc->sockfd = file_number;
    isoc->write = isoc->read = isoc->buffer;
    return isoc;
}

PUBLIC void HTInputSocket_free (HTInputSocket * me)
{
    if (me) HT_FREE(me);
}

/*	Push data from a socket down a stream
**	-------------------------------------
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the file. As this function
**   max reads a chunk of data on size INPUT_BUFFER_SIZE, it can be used
**   with both blocking or non-blocking sockets. It will always return to
**   the event loop, however if we are using blocking I/O then we get a full
**   buffer read, otherwise we get what's available.
**
** Returns      HT_LOADED	if finished reading
**		HT_OK		if OK, but more to read
**	      	HT_ERROR	if error,
**     		HT_WOULD_BLOCK	if read or write would block
**		HT_PAUSE	if stream is paused
*/
PUBLIC int HTSocketRead (HTRequest * request, HTNet * net)
{
    HTInputSocket *isoc = net->isoc;
    HTStream *target = net->target;
    int b_read = isoc->read - isoc->buffer;
    int status;
    if (!isoc || isoc->sockfd==INVSOC) {
	if (PROT_TRACE) TTYPrint(TDEST, "Read Socket. Bad argument\n");
	return HT_ERROR;
    }

    /* Read from socket if we got rid of all the data previously read */
    do {
	if (isoc->write >= isoc->read) {
	    if ((b_read = NETREAD(isoc->sockfd, isoc->buffer,
				  INPUT_BUFFER_SIZE)) < 0) {
#ifdef EAGAIN
		if (socerrno==EAGAIN || socerrno==EWOULDBLOCK)      /* POSIX */
#else
		if (socerrno==EWOULDBLOCK) 			      /* BSD */
#endif	
		{
		    if (PROT_TRACE)
			TTYPrint(TDEST, "Read Socket. WOULD BLOCK soc %d\n",
				isoc->sockfd);
		    HTEvent_Register(isoc->sockfd, request, (SockOps) FD_READ,
				     net->cbf, net->priority);
		    return HT_WOULD_BLOCK;
#ifdef __svr4__
    /* 
    ** In Solaris envirnoment, SIGPOLL is used to signal end of buffer for
    ** /dev/audio.  If your process is also doing a socket read, it will cause
    ** an EINTR error.  This error will cause the www library request to 
    ** terminate prematurly.
    */
                } else if (socerrno == EINTR) {
                    continue;
#endif
		} else { /* We have a real error */
		    HTRequest_addSystemError(request,  ERR_FATAL, socerrno, NO,
					     "NETREAD");
		    return HT_ERROR;
		}
	    } else if (!b_read) {
		HTAlertCallback *cbf = HTAlert_find(HT_PROG_DONE);
		if (PROT_TRACE)
		    TTYPrint(TDEST,"Read Socket. Finished loading socket %d\n",
			     isoc->sockfd);
		if(cbf)(*cbf)(request,HT_PROG_DONE,HT_MSG_NULL,NULL,NULL,NULL);
	        HTEvent_UnRegister(isoc->sockfd, FD_READ);
		return HT_CLOSED;
	    }

	    /* Remember how much we have read from the input socket */
	    isoc->write = isoc->buffer;
	    isoc->read = isoc->buffer + b_read;

#ifdef NOT_ASCII
	    {
		char *p = isoc->buffer;
		while (p < isoc->read) {
		    *p = FROMASCII(*p);
		    p++;
		}
	    }
#endif
	    if (PROT_TRACE)
		TTYPrint(TDEST, "Read Socket. %d bytes read from socket %d\n",
			b_read, isoc->sockfd);
	    net->bytes_read += b_read;

	    {
		HTAlertCallback *cbf = HTAlert_find(HT_PROG_READ);
		if (cbf)
		    (*cbf)(request, HT_PROG_READ, HT_MSG_NULL,NULL,NULL,NULL);
	    }
	}
	
	/* Now push the data down the stream */
	if ((status = (*target->isa->put_block)(target, isoc->buffer,
						b_read)) != HT_OK) {
	    if (status==HT_WOULD_BLOCK) {
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read Socket. Target WOULD BLOCK\n");
		HTEvent_UnRegister(isoc->sockfd, FD_READ);
		return HT_WOULD_BLOCK;
	    } else if (status == HT_PAUSE) {
		if (PROT_TRACE) TTYPrint(TDEST,"Read Socket. Target PAUSED\n");
		HTEvent_UnRegister(isoc->sockfd, FD_READ);
		return HT_PAUSE;
	    } else if (status>0) {	      /* Stream specific return code */
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read Socket. Target returns %d\n",status);
		isoc->write = isoc->buffer + b_read;
		return status;
	    } else {				     /* We have a real error */
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read Socket. Target ERROR\n");
		return status;
	    }
	}
	isoc->write = isoc->buffer + b_read;
    } while (net->preemptive);
    HTEvent_Register(isoc->sockfd, request, (SockOps) FD_READ,
		     net->cbf, net->priority);
    return HT_WOULD_BLOCK;
}

/*	HTSocket_DLLHackFopen
**	---------------------
** 	Work around the problem that an app can't open a file and have a dll
** 	read from it!
*/
#ifdef WWW_WIN_DLL
PUBLIC FILE * HTSocket_DLLHackFopen (const char * filename, const char * mode)
{
    return (fopen(filename, mode));
}
#endif /* WWW_WIN_DLL */

/*	Push data from an ANSI file descriptor down a stream
**	----------------------------------------------------
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the file.
**
**   Bugs: When we can wait on a file then this should also check interrupts!
**
**   Returns    HT_LOADED	if finished reading
**	      	HT_ERROR	if error,
*/
PUBLIC int HTFileRead (HTRequest * request, HTNet * net, FILE * fp)
{
    HTInputSocket *isoc = net->isoc;
    HTStream *target = net->target;
    int b_read;
    int status;
    if (!fp) {
	if (PROT_TRACE) TTYPrint(TDEST, "Read File... Bad argument\n");
	return HT_ERROR;
    }

    while(1) {
	if ((b_read = fread(isoc->buffer, 1, INPUT_BUFFER_SIZE, fp))==0){
	    if (ferror(fp)) {
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read File... READ ERROR\n");
	    } else
		return HT_LOADED;
	}
	isoc->write = isoc->buffer;
	isoc->read = isoc->buffer + b_read;
	if (PROT_TRACE)
	    TTYPrint(TDEST, "Read File... %d bytes read from file %p\n",
		    b_read, fp);

	/* Now push the data down the stream (we use blocking I/O) */
	if ((status = (*target->isa->put_block)(target, isoc->buffer,
						b_read)) != HT_OK) {
	    if (PROT_TRACE)
		TTYPrint(TDEST, "Read File... Target ERROR\n");
	    return status;
	}
	isoc->write = isoc->buffer + b_read;
    }
}

/*	HTLoadSocket
**	------------
**	Given an open socket, this routine loads what ever is on the socket
**
** On entry,
**      request		This is the request structure
** On Exit
**	returns		HT_ERROR	Error has occured in call back
**			HT_OK		Call back was OK
*/
PUBLIC int HTLoadSocket (SOCKET soc, HTRequest * request, SockOps ops)
{
    HTNet * net = NULL;
    if (!request) return HT_ERROR;
    if (ops == FD_NONE) {
	HTNet * me;
	if (soc==INVSOC) {
	    if (PROT_TRACE) TTYPrint(TDEST, "Load Socket. invalid socket\n");
	    return HT_ERROR;
	}
	if (PROT_TRACE) TTYPrint(TDEST,"Load Socket. Loading socket %d\n",soc);
	me = HTNet_new(request, soc);
	me->sockfd = soc;
	me->target = request->output_stream;
	me->isoc = HTInputSocket_new(soc);
	net = me;
    } else if (ops == FD_CLOSE) {			      /* Interrupted */
	HTNet_delete(request->net, HT_INTERRUPTED);
	return HT_OK;
    } else
	net = request->net;
    if (!net) {
	if (PROT_TRACE) TTYPrint(TDEST, "Load Socket. invalid argument\n");
	return HT_ERROR;
    }

    /* In this load function we only have one state: READ */
    {
	int status = HTSocketRead(request, net);
	if (status == HT_WOULD_BLOCK)
	    return HT_OK;
	else if (status == HT_CLOSED)
	    HTNet_delete(request->net, HT_LOADED);
	else
	    HTNet_delete(request->net, HT_ERROR);
    }
    return HT_OK;
}
