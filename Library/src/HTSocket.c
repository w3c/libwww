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
#include "sysdep.h"
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

/* A channel occupies exactly one socket */
struct _HTChannel {
    HTChannel *		next;
    HTChannelMode	mode;
    BOOL		active;			/* Active or passive channel */
    SOCKET 		sockfd;
    char *		write;				/* Last byte written */
    char *		read;				   /* Last byte read */
    char		data [CHANNEL_BUFFER_SIZE];		   /* buffer */
};

struct _HTStream {
    const HTStreamClass *	isa;
};

#define PRIME_TABLE_SIZE	67
#define HASH(s)			((s) % PRIME_TABLE_SIZE) 

PRIVATE HTChannel * channels [PRIME_TABLE_SIZE];

struct _HTFileBuffer {
    char *	write;					/* Last byte written */
    char *	read;					   /* Last byte read */
    char	data [CHANNEL_BUFFER_SIZE];
};

/*	Set up the buffering
**
**	These routines are public because they are in fact needed by
**	many parsers, and on PCs and Macs we should not duplicate
**	the static buffer area.
*/
PUBLIC HTFileBuffer * HTFileBuffer_new (void)
{
    HTFileBuffer * isoc;
    if ((isoc = (HTFileBuffer  *) HT_CALLOC(1, sizeof(*isoc))) == NULL)
        HT_OUTOFMEM("HTFileBuffer_new");
    isoc->write = isoc->read = isoc->data;
    return isoc;
}

PUBLIC void HTFileBuffer_free (HTFileBuffer * me)
{
    HT_FREE(me);
}

/* ------------------------------------------------------------------------- */
/*				CHANNEL CONTROL				     */
/* ------------------------------------------------------------------------- */

/*
**	Look for a channel object
*/
PUBLIC HTChannel * HTChannel_find (SOCKET sockfd)
{
    if (sockfd != INVSOC) {
	int hash = HASH(sockfd);
	HTChannel *ch=NULL;
	for (ch = channels[hash]; ch; ch=ch->next) {
	    if (ch->sockfd == sockfd) {
		if (PROT_TRACE) HTTrace("Channel..... Found %p\n", ch);
		return ch;
	    }
	}
    }
    return NULL;
}

/*
**	A channel is uniquely identified by a socket.
**	If we are not using mux then a channel is simply a normal TCP socket.
**	Before creating a new channel, we check to see if it already exists.
**	You may get back an already existing channel - you're not promised a
**	new one each time. ANSI C file channels can not be created by using
**	the INVSOC socket descriptor.
*/
PUBLIC HTChannel * HTChannel_new (SOCKET sockfd, HTChannelMode mode,
				  BOOL active)
{
    if (sockfd != INVSOC) {
	int hash = HASH(sockfd);
	HTChannel *ch=NULL, **chp=NULL;
	for (chp = &channels[hash]; (ch = *chp) != NULL ; chp = &ch->next) {
	    if (ch->sockfd == sockfd) break;
	}
	if (!ch) {
	    if ((*chp=ch=(HTChannel *) HT_CALLOC(1, sizeof(HTChannel)))==NULL)
		HT_OUTOFMEM("HTChannel_new");	    
	    ch->sockfd = sockfd;
	    ch->write = ch->read = ch->data;
	    ch->mode = mode;
	    ch->active = active;
	    if (PROT_TRACE)
		HTTrace("Channel..... Created %p with id %d\n", ch,ch->sockfd);
	} else {
	    if (PROT_TRACE)
		HTTrace("Channel..... Found %p with id %d\n", ch, ch->sockfd);
	    ch->write = ch->read = ch->data;
	}
	return ch;
    }
    return NULL;
}

PUBLIC BOOL HTChannel_delete (SOCKET sockfd)
{
    if (sockfd != INVSOC) {
	int hash = HASH(sockfd);
	HTChannel *ch=NULL, **chp=NULL;
	for (chp = &channels[hash]; (ch = *chp) != NULL ; chp = &ch->next) {
	    if (ch->sockfd == sockfd) {

		/* Walk through and ABORT all remaining sessions */

		if (PROT_TRACE) HTTrace("Channel..... Deleted %p with id %d\n",
					ch, ch->sockfd);
		*chp = ch->next;
		HT_FREE(ch);
		return YES;
	    }
	}
    }
    return NO;
}

PUBLIC BOOL HTChannel_deleteAll (void)
{

    /* @@@ */

    return NO;
}

/*
**	Set and Get the mode of a channel. A channel may change mode in the 
**	middle of a connection.
*/
PUBLIC HTChannelMode HTChannel_mode (SOCKET sockfd, BOOL *active)
{
    if (sockfd != INVSOC) {
	int hash = HASH(sockfd);
	HTChannel *ch=NULL, **chp=NULL;
	for (chp = &channels[hash]; (ch = *chp) != NULL ; chp = &ch->next) {
	    if (ch->sockfd == sockfd) {
		if (active) *active = ch->active;
		return ch->mode;
	    }
	}
    }
    return HT_CH_UNKNOWN;
}

PUBLIC BOOL HTChannel_setMode (SOCKET sockfd, HTChannelMode mode)
{
    if (sockfd != INVSOC) {
	int hash = HASH(sockfd);
	HTChannel *ch=NULL, **chp=NULL;
	for (chp = &channels[hash]; (ch = *chp) != NULL ; chp = &ch->next) {
	    if (ch->sockfd == sockfd) {
		ch->mode = mode;
		return YES;
	    }
	}
    }
    return NO;
}

PUBLIC BOOL HTChannel_addMode (SOCKET sockfd, HTChannelMode mode)
{
    if (sockfd != INVSOC) {
	int hash = HASH(sockfd);
	HTChannel *ch=NULL, **chp=NULL;
	for (chp = &channels[hash]; (ch = *chp) != NULL ; chp = &ch->next) {
	    if (ch->sockfd == sockfd) {
		ch->mode |= mode;
		return YES;
	    }
	}
    }
    return NO;
}

/* ------------------------------------------------------------------------- */
/*				READ ROUTINES  				     */
/* ------------------------------------------------------------------------- */

/*	Push data from a socket down a stream
**	-------------------------------------
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the file. As this function
**   max reads a chunk of data on size CHANNEL_BUFFER_SIZE, it can be used
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
PUBLIC int HTChannel_readSocket (HTRequest * request, HTNet * net)
{
    HTChannel * ch = HTChannel_find(net->sockfd);
    int b_read = ch->read - ch->data;
    int status;

    /* Read from socket if we got rid of all the data previously read */
    do {
	if (ch->write >= ch->read) {
	    if ((b_read = NETREAD(net->sockfd, ch->data,
				  CHANNEL_BUFFER_SIZE)) < 0) {
#ifdef EAGAIN
		if (socerrno==EAGAIN || socerrno==EWOULDBLOCK)      /* POSIX */
#else
		if (socerrno==EWOULDBLOCK) 			      /* BSD */
#endif	
		{
		    if (PROT_TRACE)
			HTTrace("Read Socket. WOULD BLOCK soc %d\n",net->sockfd);
		    HTEvent_Register(net->sockfd, request, (SockOps) FD_READ,
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
		    HTTrace("Read Socket. Finished loading socket %d\n",
			     net->sockfd);
		if(cbf)(*cbf)(request,HT_PROG_DONE,HT_MSG_NULL,NULL,NULL,NULL);
	        HTEvent_UnRegister(net->sockfd, FD_READ);
		return HT_CLOSED;
	    }

	    /* Remember how much we have read from the input socket */
	    ch->write = ch->data;
	    ch->read = ch->data + b_read;

#ifdef NOT_ASCII
	    {
		char *p = ch->data;
		while (p < ch->read) {
		    *p = FROMASCII(*p);
		    p++;
		}
	    }
#endif

	    if (PROT_TRACE)
		HTTrace("Read Socket. %d bytes read from socket %d\n",
			b_read, net->sockfd);

	    if (!(ch->mode & HT_CH_INTERLEAVED)) {
		HTAlertCallback * cbf = HTAlert_find(HT_PROG_READ);
		net->bytes_read += b_read;
		if (cbf)
		    (*cbf)(request, HT_PROG_READ, HT_MSG_NULL,NULL,NULL,NULL);
	    }
	}

	/* Now push the data down the stream */
	if ((status = (*net->target->isa->put_block)(net->target, ch->data,
						     b_read)) != HT_OK) {
	    if (status==HT_WOULD_BLOCK) {
		if (PROT_TRACE)
		    HTTrace("Read Socket. Target WOULD BLOCK\n");
		HTEvent_UnRegister(net->sockfd, FD_READ);
		return HT_WOULD_BLOCK;
	    } else if (status == HT_PAUSE) {
		if (PROT_TRACE) HTTrace("Read Socket. Target PAUSED\n");
		HTEvent_UnRegister(net->sockfd, FD_READ);
		return HT_PAUSE;
	    } else if (status>0) {	      /* Stream specific return code */
		if (PROT_TRACE)
		    HTTrace("Read Socket. Target returns %d\n",status);
		ch->write = ch->data + b_read;
		return status;
	    } else {				     /* We have a real error */
		if (PROT_TRACE)
		    HTTrace("Read Socket. Target ERROR\n");
		return status;
	    }
	}
	ch->write = ch->data + b_read;
    } while (net->preemptive);
    HTEvent_Register(net->sockfd, request, (SockOps) FD_READ,
		     net->cbf, net->priority);
    return HT_WOULD_BLOCK;
}

/*	HTSocket_DLLHackFopen and close
**	-------------------------------
** 	Work around the problem that an app can't open a file and have a dll
** 	read from it!
*/
#ifdef WWW_WIN_DLL
PUBLIC FILE * HTSocket_DLLHackFopen (const char * filename, const char * mode)
{
    return (fopen(filename, mode));
}

PUBLIC int HTSocket_DLLHackFclose (FILE * file)
{
    return (fclose(file));
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
PUBLIC int HTChannel_readFile (HTRequest * request, HTNet * net,
			       HTFileBuffer * isoc, FILE * fp)
{
    int b_read;
    int status;
    if (!fp) {
	if (PROT_TRACE) HTTrace("Read File... Bad argument\n");
	return HT_ERROR;
    }

    while(1) {
	if ((b_read = fread(isoc->data, 1, CHANNEL_BUFFER_SIZE, fp))==0){
	    if (ferror(fp)) {
		if (PROT_TRACE) HTTrace("Read File... READ ERROR\n");
	    } else
		return HT_LOADED;
	}
	isoc->write = isoc->data;
	isoc->read = isoc->data + b_read;
	if (PROT_TRACE)
	    HTTrace("Read File... %d bytes read from file %p\n", b_read, fp);

	/* Now push the data down the stream (we use blocking I/O) */
	if ((status = (*net->target->isa->put_block)(net->target, isoc->data,
						     b_read)) != HT_OK) {
	    if (PROT_TRACE) HTTrace("Read File... Target ERROR\n");
	    return status;
	}
	isoc->write = isoc->data + b_read;
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
	    if (PROT_TRACE) HTTrace("Load Socket. invalid socket\n");
	    return HT_ERROR;
	}
	if (PROT_TRACE) HTTrace("Load Socket. Loading socket %d\n",soc);
	me = HTNet_new(request, soc);
	me->sockfd = soc;
	me->target = request->output_stream;
	HTChannel_new(net->sockfd, HT_CH_PLAIN, NO);
	net = me;
    } else if (ops == FD_CLOSE) {			      /* Interrupted */
	HTNet_delete(request->net, HT_INTERRUPTED);
	return HT_OK;
    } else
	net = request->net;
    if (!net) {
	if (PROT_TRACE) HTTrace("Load Socket. invalid argument\n");
	return HT_ERROR;
    }

    /* In this load function we only have one state: READ */
    {
	int status = HTChannel_readSocket(request, net);
	if (status == HT_WOULD_BLOCK)
	    return HT_OK;
	else if (status == HT_CLOSED)
	    HTNet_delete(request->net, HT_LOADED);
	else
	    HTNet_delete(request->net, HT_ERROR);
    }
    return HT_OK;
}

