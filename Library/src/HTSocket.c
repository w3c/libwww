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
    SOCKFD	sockfd;
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
PUBLIC HTInputSocket * HTInputSocket_new (SOCKFD file_number)
{
    HTInputSocket *isoc = (HTInputSocket *)calloc(1, sizeof(*isoc));
    if (!isoc) outofmem(__FILE__, "HTInputSocket_new");
    isoc->sockfd = file_number;
    isoc->write = isoc->read = isoc->buffer;
    return isoc;
}

PUBLIC void HTInputSocket_free (HTInputSocket * me)
{
    if (me) free(me);
}

/* ------------------------------------------------------------------------- */

/* This should return HT_INTERRUPTED if interrupted BUT the connection
   MUST not be closed */ 
PUBLIC int HTInputSocket_getCharacter ARGS1(HTInputSocket*, isoc)
{
    int ch;
    do {
	if (isoc->write >= isoc->read) {
	    int status = NETREAD(isoc->sockfd, isoc->buffer,INPUT_BUFFER_SIZE);
	    if (status <= 0) {
		if (status == 0)
		    return EOF;
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read Socket. READ ERROR %d\n", socerrno);
		return EOF; 	/* -1 is returned by UCX at end of HTTP link */
	    }
	    isoc->write = isoc->buffer;
	    isoc->read = isoc->buffer + status;
	}
	ch = (unsigned char) *isoc->write++;
    } while (ch == 13);			     /* Ignore ASCII carriage return */
    
    return FROMASCII(ch);
}

PUBLIC char * HTInputSocket_getBlock ARGS2(HTInputSocket*,	isoc,
					   int *,		len)
{
    if (isoc->write >= isoc->read) {
	int status = NETREAD(isoc->sockfd,
			     isoc->buffer,
			     ((*len < INPUT_BUFFER_SIZE) ?
			      *len : INPUT_BUFFER_SIZE));
	if (status <= 0) {
	    isoc->read = isoc->buffer;
	    if (status < 0) {
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read Socket. READ ERROR %d\n", socerrno);
	    }
	    *len = 0;
	    return NULL;
	}
	else {
	    *len = status;
	    return isoc->buffer;
	}
    }
    else {
	char * ret = isoc->write;
	*len = isoc->read - isoc->write;
	isoc->write = isoc->read;
	return ret;
    }
}


PRIVATE int fill_in_buffer ARGS1(HTInputSocket *, isoc)
{
    if (isoc) {
	int status;

	isoc->write = isoc->buffer;
	status = NETREAD(isoc->sockfd,
			 isoc->buffer,
			 INPUT_BUFFER_SIZE);
	if (status <= 0) {
	    isoc->read = isoc->buffer;
	    if (status < 0) {
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read Socket. READ ERROR %d\n", socerrno);
	    }
	}
	else 
	    isoc->read = isoc->buffer + status;
	return status;
    }
    return -1;
}


PRIVATE void ascii_cat ARGS3(char **,	linep,
			     char *,	start,
			     char *,	end)
{
    if (linep && start && end && start <= end) {
	char *ptr;

	if (*linep) {
	    int len = strlen(*linep);
	    *linep = (char*)realloc(*linep, len + end-start + 1);
	    ptr = *linep + len;
	}
	else {
	    ptr = *linep = (char*)malloc(end-start + 1);
	}

	while (start < end) {
	    *ptr = FROMASCII(*start);
	    ptr++;
	    start++;
	}
	*ptr = 0;
    }
}


PRIVATE char * get_some_line ARGS2(HTInputSocket *,	isoc,
				   BOOL,		unfold)
{
    if (!isoc)
	return NULL;
    else {
	BOOL check_unfold = NO;
	int prev_cr = 0;
	char *start = isoc->write;
	char *cur = isoc->write;
	char * line = NULL;

	for(;;) {
	    /*
	    ** Get more if needed to complete line
	    */
	    if (cur >= isoc->read) { /* Need more data */
		ascii_cat(&line, start, cur);
		if (fill_in_buffer(isoc) <= 0)
		    return line;
		start = cur = isoc->write;
	    } /* if need more data */

	    /*
	    ** Find a line feed if there is one
	    */
	    for(; cur < isoc->read; cur++) {
		char c = FROMASCII(*cur);
		if (!c) {
		    if (line) free(line);	/* Leak fixed AL 6 Feb 94 */
		    return NULL;	/* Panic! read a 0! */
		}
		if (check_unfold  &&  c != ' '  &&  c != '\t') {
		    return line;  /* Note: didn't update isoc->write */
		}
		else {
		    check_unfold = NO;
		}

		if (c=='\r') {
		    prev_cr = 1;
		}
		else {
		    if (c=='\n') {		/* Found a line feed */
			ascii_cat(&line, start, cur-prev_cr);
			start = isoc->write = cur+1;

			if (line && (int) strlen(line) > 0 && unfold) {
			    check_unfold = YES;
			}
			else {
			    return line;
			}
		    } /* if NL */
		    /* else just a regular character */
		    prev_cr = 0;
		} /* if not CR */
	    } /* while characters in buffer remain */
	} /* until line read or end-of-file */
    } /* valid parameters to function */
}

/* The returned string must be freed by the caller */
PUBLIC char * HTInputSocket_getLine ARGS1(HTInputSocket *, isoc)
{
    return get_some_line(isoc, NO);
}

/* The returned string must be freed by the caller */
PUBLIC char * HTInputSocket_getUnfoldedLine ARGS1(HTInputSocket *, isoc)
{
    return get_some_line(isoc, YES);
}


/*	Push data from a socket down a stream
**	-------------------------------------
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the file.
**
**   The file number given is assumed to be a TELNET stream ie containing
**   CRLF at the end of lines which need to be stripped to LF for unix
**   when the format is textual.
**
**   RETURNS the number of bytes transferred.
**
*/
PRIVATE int HTCopy ARGS2(
	SOCKFD,			file_number,
	HTStream*,		sink)
{
    HTStreamClass targetClass;    
    HTInputSocket * isoc;
    int cnt = 0;

/*	Push the data down the stream
**
*/
    targetClass = *(sink->isa);	/* Copy pointers to procedures */
    isoc = HTInputSocket_new(file_number);
    
    /*	Push binary from socket down sink
    **
    **		This operation could be put into a main event loop
    */
    for(;;) {
	int status = NETREAD(
		file_number, isoc->buffer, INPUT_BUFFER_SIZE);
	if (status <= 0) {
	    if (status == 0) break;
	    if (WWWTRACE) TTYPrint(TDEST,
		"Socket Copy. Read error, read returns %d with errno=%d\n",
		status, socerrno);
	    break;
	}

#ifdef NOT_ASCII
	{
	    char * p;
	    for(p = isoc->buffer; p < isoc->buffer+status; p++) {
		*p = FROMASCII(*p);
	    }
	}
#endif

	(*targetClass.put_block)(sink, isoc->buffer, status);
	cnt += status;
    } /* next bufferload */

    HTInputSocket_free(isoc);

    return cnt;
}


#if 0
/*	Push data from a file pointer down a stream
**	-------------------------------------
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the file.
**
**
*/
PRIVATE void HTFileCopy ARGS2(
	FILE *,			fp,
	HTStream*,		sink)
{
    HTStreamClass targetClass;    
    char buffer[INPUT_BUFFER_SIZE];
    
/*	Push the data down the stream
**
*/
    targetClass = *(sink->isa);	/* Copy pointers to procedures */
    
    /*	Push binary from socket down sink
    */
    for(;;) {
	int status = fread(
	       buffer, 1, INPUT_BUFFER_SIZE, fp);
	if (status == 0) { /* EOF or error */
	    if (ferror(fp) == 0) break;
	    if (WWWTRACE) TTYPrint(TDEST,
		"File Copy... Read error, read returns %d\n", ferror(fp));
	    break;
	}
	(*targetClass.put_block)(sink, buffer, status);
    } /* next bufferload */	
}

#endif


/*	Push data from a socket down a stream STRIPPING CR
**	--------------------------------------------------
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the socket.
**
**   The file number given is assumed to be a TELNET stream ie containing
**   CRLF at the end of lines which need to be stripped to LF for unix
**   when the format is textual.
**	
**	Character handling is now of type int, Henrik, May 09-94
*/
PRIVATE void HTCopyNoCR ARGS2(
	SOCKFD,			file_number,
	HTStream*,		sink)
{
    HTStreamClass targetClass;
    HTInputSocket * isoc;   
    int ch;
    
/*	Push the data, ignoring CRLF, down the stream
**
*/
    targetClass = *(sink->isa);	/* Copy pointers to procedures */

/*	Push text from telnet socket down sink
**
**	@@@@@ To push strings could be faster? (especially is we
**	cheat and don't ignore CR! :-}
*/  
    isoc = HTInputSocket_new(file_number);
    while ((ch = HTInputSocket_getCharacter(isoc)) >= 0)
	(*targetClass.put_character)(sink, ch);
    HTInputSocket_free(isoc);
}


/*	Parse a socket given format and file number
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the file.
**
**   The file number given is assumed to be a TELNET stream ie containing
**   CRLF at the end of lines which need to be stripped to LF for unix
**   when the format is textual.
**
**	Returns <0 on error, HT_LOADED on success.
*/

/* The parameter to this function and HTParsefile should be HTRequest */

PUBLIC int HTParseSocket ARGS3(
	HTFormat,		rep_in,
	SOCKFD,			file_number,
	HTRequest *,		request)
{
    HTStream * stream;
    HTStreamClass targetClass;    

    if (request->error_stack) {
	if (WWWTRACE) TTYPrint(TDEST, "ParseSocket. Called whith non-empty error stack, so I return right away!\n");
	return -1;
    }

    /* Set up stream stack */
    if ((stream = HTStreamStack(rep_in, request->output_format,
				request->output_stream,
				request, YES)) == NULL)
	return -1;
    
/*	Push the data, ignoring CRLF if necessary, down the stream
**
**
**   @@  Bug:  This decision ought to be made based on "encoding"
**   rather than on format.  @@@  When we handle encoding.
**   The current method smells anyway.
*/
    targetClass = *(stream->isa);	/* Copy pointers to procedures */
    if (rep_in == WWW_BINARY || rep_in == WWW_UNKNOWN
	|| (HTAnchor_encoding(request->anchor) != HTAtom_for("8bit") &&
	    HTAnchor_encoding(request->anchor) != HTAtom_for("7bit"))
        || strstr(HTAtom_name(rep_in), "image/")
	|| strstr(HTAtom_name(rep_in), "video/")) { /* @@@@@@ */
	HTCopy(file_number, stream);
    } else
        HTCopyNoCR(file_number, stream);
    (*targetClass._free)(stream);
    
    return HT_LOADED;
}


#if 0
/*	Parse a file given format and file pointer
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the file.
**
**   The file number given is assumed to be a TELNET stream ie containing
**   CRLF at the end of lines which need to be stripped to \n for unix
**   when the format is textual.
**
*/
PRIVATE int HTParseFile ARGS3(
	HTFormat,		rep_in,
	FILE *,			fp,
	HTRequest *,		request)
{
    HTStream * stream;
    HTStreamClass targetClass;    

    if (request->error_stack) {
	if (WWWTRACE) TTYPrint(TDEST, "ParseFile... Called whith non-empty error stack, so I return right away!\n");
	return -1;
    }

    /* Set up stream stack */
    if ((stream = HTStreamStack(rep_in, request->output_format,
				request->output_stream, request, YES)) == NULL)
	return -1;
    
/*	Push the data down the stream
**
**
**   @@  Bug:  This decision ought to be made based on "encoding"
**   rather than on content-type.  @@@  When we handle encoding.
**   The current method smells anyway.
*/
    targetClass = *(stream->isa);	/* Copy pointers to procedures */
    HTFileCopy(fp, stream);
    (*targetClass._free)(stream);
    
    return HT_LOADED;
}
#endif

/* ------------------------------------------------------------------------- */
/*			MULTI THREADED IMPLEMENTATIONS			     */
/* ------------------------------------------------------------------------- */

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
**     		HT_WOULD_BLOCK	if read would block
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
		} else { /* We have a real error */
		    if (PROT_TRACE)
			TTYPrint(TDEST, "Read Socket. READ ERROR %d\n",
				socerrno);
		    return HT_ERROR;
		}
	    } else if (!b_read) {
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read Socket. Finished loading socket %d\n",
			    isoc->sockfd);
		HTProgress(request, HT_PROG_DONE, NULL);
	        HTEvent_UnRegister(isoc->sockfd, FD_READ);
		return HT_LOADED;
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
	    HTProgress(request, HT_PROG_READ, NULL);
	}
	
	/* Now push the data down the stream */
	if ((status = (*target->isa->put_block)(target, isoc->buffer,
						b_read)) != HT_OK) {
	    if (status==HT_WOULD_BLOCK) {
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read Socket. Target WOULD BLOCK\n");
		HTEvent_UnRegister(isoc->sockfd, FD_READ);
		return HT_WOULD_BLOCK;
	    } else if (status>0) {	      /* Stream specific return code */
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read Socket. Target returns %d\n", status);
		isoc->write = isoc->buffer + b_read;
		return status;
	    } else {				     /* We have a real error */
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Read Socket. Target ERROR\n");
		return status;
	    }
	}
	isoc->write = isoc->buffer + b_read;
    } while (net->preemtive);
    HTEvent_Register(isoc->sockfd, request, (SockOps) FD_READ,
		     net->cbf, net->priority);
    return HT_WOULD_BLOCK;
}



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


