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
#include "HTAccess.h"
#include "HTProt.h"
#include "HTTCP.h"
#include "HTStream.h"
#include "HTFormat.h"
#include "HTThread.h"
#include "HTError.h"
#include "HTSocket.h"					 /* Implemented here */

struct _HTInputSocket {
    char	input_buffer[INPUT_BUFFER_SIZE];
    char *	input_pointer;
    char *	input_limit;
    SOCKFD	input_file_number;
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
PUBLIC HTInputSocket * HTInputSocket_new ARGS1 (SOCKFD, file_number)
{
    HTInputSocket *isoc = (HTInputSocket *)calloc(1, sizeof(*isoc));
    if (!isoc) outofmem(__FILE__, "HTInputSocket_new");
    isoc->input_file_number = file_number;
    isoc->input_pointer = isoc->input_limit = isoc->input_buffer;
    return isoc;
}

/* This should return HT_INTERRUPTED if interrupted BUT the connection
   MUST not be closed */ 
PUBLIC int HTInputSocket_getCharacter ARGS1(HTInputSocket*, isoc)
{
    int ch;
    do {
	if (isoc->input_pointer >= isoc->input_limit) {
	    int status = NETREAD(isoc->input_file_number,
				 isoc->input_buffer, INPUT_BUFFER_SIZE);
	    if (status <= 0) {
		if (status == 0)
		    return EOF;
		if (status == HT_INTERRUPTED) {
		    if (TRACE)
			fprintf(TDEST, "Get Char.... Interrupted in HTInputSocket_getCharacter\n");
		    return HT_INTERRUPTED;
		}
		if (PROT_TRACE)
		    fprintf(TDEST, "Read Socket. READ ERROR %d\n", socerrno);
		return EOF; 	/* -1 is returned by UCX at end of HTTP link */
	    }
	    isoc->input_pointer = isoc->input_buffer;
	    isoc->input_limit = isoc->input_buffer + status;
	}
	ch = (unsigned char) *isoc->input_pointer++;
    } while (ch == 13);			     /* Ignore ASCII carriage return */
    
    return FROMASCII(ch);
}

PUBLIC void HTInputSocket_free ARGS1(HTInputSocket *, me)
{
    if (me) free(me);
}


PUBLIC char * HTInputSocket_getBlock ARGS2(HTInputSocket*,	isoc,
					   int *,		len)
{
    if (isoc->input_pointer >= isoc->input_limit) {
	int status = NETREAD(isoc->input_file_number,
			     isoc->input_buffer,
			     ((*len < INPUT_BUFFER_SIZE) ?
			      *len : INPUT_BUFFER_SIZE));
	if (status <= 0) {
	    isoc->input_limit = isoc->input_buffer;
	    if (status < 0) {
		if (PROT_TRACE)
		    fprintf(TDEST, "Read Socket. READ ERROR %d\n", socerrno);
	    }
	    *len = 0;
	    return NULL;
	}
	else {
	    *len = status;
	    return isoc->input_buffer;
	}
    }
    else {
	char * ret = isoc->input_pointer;
	*len = isoc->input_limit - isoc->input_pointer;
	isoc->input_pointer = isoc->input_limit;
	return ret;
    }
}


PRIVATE int fill_in_buffer ARGS1(HTInputSocket *, isoc)
{
    if (isoc) {
	int status;

	isoc->input_pointer = isoc->input_buffer;
	status = NETREAD(isoc->input_file_number,
			 isoc->input_buffer,
			 INPUT_BUFFER_SIZE);
	if (status <= 0) {
	    isoc->input_limit = isoc->input_buffer;
	    if (status < 0) {
		if (PROT_TRACE)
		    fprintf(TDEST, "Read Socket. READ ERROR %d\n", socerrno);
	    }
	}
	else 
	    isoc->input_limit = isoc->input_buffer + status;
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
	char *start = isoc->input_pointer;
	char *cur = isoc->input_pointer;
	char * line = NULL;

	for(;;) {
	    /*
	    ** Get more if needed to complete line
	    */
	    if (cur >= isoc->input_limit) { /* Need more data */
		ascii_cat(&line, start, cur);
		if (fill_in_buffer(isoc) <= 0)
		    return line;
		start = cur = isoc->input_pointer;
	    } /* if need more data */

	    /*
	    ** Find a line feed if there is one
	    */
	    for(; cur < isoc->input_limit; cur++) {
		char c = FROMASCII(*cur);
		if (!c) {
		    if (line) free(line);	/* Leak fixed AL 6 Feb 94 */
		    return NULL;	/* Panic! read a 0! */
		}
		if (check_unfold  &&  c != ' '  &&  c != '\t') {
		    return line;  /* Note: didn't update isoc->input_pointer */
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
			start = isoc->input_pointer = cur+1;

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
PUBLIC int HTCopy ARGS2(
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
		file_number, isoc->input_buffer, INPUT_BUFFER_SIZE);
	if (status <= 0) {
	    if (status == 0) break;
	    if (TRACE) fprintf(TDEST,
		"Socket Copy. Read error, read returns %d with errno=%d\n",
		status, socerrno);
	    break;
	}

#ifdef NOT_ASCII
	{
	    char * p;
	    for(p = isoc->input_buffer; p < isoc->input_buffer+status; p++) {
		*p = FROMASCII(*p);
	    }
	}
#endif

	(*targetClass.put_block)(sink, isoc->input_buffer, status);
	cnt += status;
    } /* next bufferload */

    HTInputSocket_free(isoc);

    return cnt;
}



/*	Push data from a file pointer down a stream
**	-------------------------------------
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the file.
**
**
*/
PUBLIC void HTFileCopy ARGS2(
	FILE *,			fp,
	HTStream*,		sink)
{
    HTStreamClass targetClass;    
    char input_buffer[INPUT_BUFFER_SIZE];
    
/*	Push the data down the stream
**
*/
    targetClass = *(sink->isa);	/* Copy pointers to procedures */
    
    /*	Push binary from socket down sink
    */
    for(;;) {
	int status = fread(
	       input_buffer, 1, INPUT_BUFFER_SIZE, fp);
	if (status == 0) { /* EOF or error */
	    if (ferror(fp) == 0) break;
	    if (TRACE) fprintf(TDEST,
		"File Copy... Read error, read returns %d\n", ferror(fp));
	    break;
	}
	(*targetClass.put_block)(sink, input_buffer, status);
    } /* next bufferload */	
}




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
PUBLIC void HTCopyNoCR ARGS2(
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
	if (TRACE) fprintf(TDEST, "ParseSocket. Called whith non-empty error stack, so I return right away!\n");
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
	if (TRACE) fprintf(TDEST, "ParseFile... Called whith non-empty error stack, so I return right away!\n");
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
**    		HT_INTERRUPTED	if interrupted
**     		HT_WOULD_BLOCK	if read would block
*/
PUBLIC int HTSocketRead ARGS2(HTRequest *, request, HTStream *, target)
{
    HTInputSocket *isoc = request->net_info->isoc;
    int b_read = isoc->input_limit-isoc->input_buffer;
    BOOL blocking = HTProtocol_isBlocking(request);
    int status;
    if (!isoc || isoc->input_file_number==INVSOC) {
	if (PROT_TRACE) fprintf(TDEST, "Read Socket. Bad argument\n");
	return HT_ERROR;
    }

    if (HTThreadIntr(isoc->input_file_number))		      /* Interrupted */
	return HT_INTERRUPTED;

    /* Read from socket if we got rid of all the data previously read */
    do {
	if (isoc->input_pointer >= isoc->input_limit) {
	    if ((b_read = NETREAD(isoc->input_file_number, isoc->input_buffer,
				  INPUT_BUFFER_SIZE)) < 0) {
#ifdef EAGAIN
		if (socerrno==EAGAIN || socerrno==EWOULDBLOCK)      /* POSIX */
#else
		    if (socerrno==EWOULDBLOCK) /* BSD */
#endif
			{
			    if (PROT_TRACE)
				fprintf(TDEST, "Read Socket. WOULD BLOCK soc %d\n",
					isoc->input_file_number);
			    HTThreadState(isoc->input_file_number, THD_SET_READ);
			    return HT_WOULD_BLOCK;
			} else { /* We have a real error */
			    if (PROT_TRACE)
				fprintf(TDEST, "Read Socket. READ ERROR %d\n",
					socerrno);
			    return HT_ERROR;
			}
	    } else if (!b_read) {
		if (PROT_TRACE)
		    fprintf(TDEST, "Read Socket. Finished loading socket %d\n",
			    isoc->input_file_number);
		HTThreadState(isoc->input_file_number, THD_CLR_READ);
		return HT_LOADED;
	    }

	    /* Remember how much we have read from the input socket */
	    isoc->input_pointer = isoc->input_buffer;
	    isoc->input_limit = isoc->input_buffer + b_read;

#ifdef NOT_ASCII
	    {
		char *p = isoc->input_buffer;
		while (p < isoc->input_limit) {
		    *p = FROMASCII(*p);
		    p++;
		}
	    }
#endif
	    if (PROT_TRACE)
		fprintf(TDEST, "Read Socket. %d bytes read from socket %d\n",
			b_read, isoc->input_file_number);
	}
	
	/* Now push the data down the stream */
	if ((status = (*target->isa->put_block)(target, isoc->input_buffer,
						b_read)) != HT_OK) {
	    if (status==HT_WOULD_BLOCK) {
		if (PROT_TRACE)
		    fprintf(TDEST, "Read Socket. Stream WOULD BLOCK\n");
		HTThreadState(isoc->input_file_number, THD_CLR_READ);
		return HT_WOULD_BLOCK;
	    } else {		/* We have a real error */
		if (PROT_TRACE)
		    fprintf(TDEST, "Read Socket. Stream ERROR\n");
		return status;
	    }
	}
	isoc->input_pointer = isoc->input_buffer + b_read;
	HTThreadState(isoc->input_file_number, THD_SET_READ);
    } while (blocking);
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
PUBLIC int HTFileRead ARGS3(FILE *, fp, HTRequest *, request,
			    HTStream *, target)
{
    HTInputSocket *isoc = request->net_info->isoc;
    int b_read;
    int status;
    if (!fp) {
	if (PROT_TRACE) fprintf(TDEST, "Read File... Bad argument\n");
	return HT_ERROR;
    }

    while(1) {
	if ((b_read = fread(isoc->input_buffer, 1, INPUT_BUFFER_SIZE, fp))==0){
	    if (ferror(fp)) {
		if (PROT_TRACE)
		    fprintf(TDEST, "Read File... READ ERROR\n");
	    } else
		return HT_LOADED;
	}
	isoc->input_pointer = isoc->input_buffer;
	isoc->input_limit = isoc->input_buffer + b_read;
	if (PROT_TRACE)
	    fprintf(TDEST, "Read File... %d bytes read from file %p\n",
		    b_read, fp);

	/* Now push the data down the stream (we use blocking I/O) */
	if ((status = (*target->isa->put_block)(target, isoc->input_buffer,
						b_read)) != HT_OK) {
	    if (PROT_TRACE)
		fprintf(TDEST, "Read File... Stream ERROR\n");
	    return status;
	}
	isoc->input_pointer = isoc->input_buffer + b_read;
    }
}


