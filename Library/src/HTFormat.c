/*								     HTFormat.c
**	MANAGE DIFFERENT FILE FORMATS
**
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
**
** Bugs:
**	Assumes the incoming stream is ASCII, rather than a local file
**	format, and so ALWAYS converts from ASCII on non-ASCII machines.
**	Therefore, non-ASCII machines can't read local files.
**
** HISTORY:
**	8 Jul 94  FM	Insulate free() from _free structure element.
**	8 Nov 94  HFN	Changed a lot to make reentrant
*/

/* Library Include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTTCP.h"
#include "HTFWrite.h"
#include "HTGuess.h"
#include "HTThread.h"
#include "HTError.h"
#include "HTFormat.h"					 /* Implemented here */

/* Public variables */
PUBLIC double HTMaxSecs = 1e10;		/* No effective limit */
PUBLIC double HTMaxLength = 1e10;	/* No effective limit */
PUBLIC HTList * HTConversions = NULL;

/* Typedefs and global variable local to thid module */
struct _HTStream {
	CONST HTStreamClass *		isa;
	BOOL			had_cr;
	HTStream * 		sink;
};

/* Accept-Encoding and Accept-Language */
typedef struct _HTAcceptNode {
    HTAtom *	atom;
    double	quality;
} HTAcceptNode;

/* ------------------------------------------------------------------------- */

/*
**   This function replaces the code in HTRequest_delete() in order to keep
**   the data structure hidden (it is NOT a joke!)
**   Henrik 14/03-94
*/
PUBLIC void HTFormatDelete ARGS1(HTRequest *, request)
{
    if (request && request->conversions) {
	HTList *cur = request->conversions;
	HTPresentation *pres;
	while ((pres = (HTPresentation*) HTList_nextObject(cur))) {
	    FREE(pres->command); /* Leak fixed AL 6 Feb 1994 */
	    free(pres);
	}
	HTList_delete(request->conversions);
	request->conversions = NULL;
    }
}


/*	Define a presentation system command for a content-type
**	-------------------------------------------------------
** INPUT:
**	conversions:	The list of conveters and presenters
**	representation:	the MIME-style format name
**	command:	the MAILCAP-style command template
**	quality:	A degradation faction [0..1]
**	maxbytes:	A limit on the length acceptable as input (0 infinite)
**	maxsecs:	A limit on the time user will wait (0 for infinity)
*/
PUBLIC void HTSetPresentation ARGS7(
	HTList *,	conversions,
	CONST char *, 	representation,
	CONST char *, 	command,
	CONST char *, 	test_command,  /* HWL 27/9/94: mailcap functionality */
	double,		quality,
	double,		secs, 
	double,		secs_per_byte)
{
    HTPresentation * pres = (HTPresentation *)malloc(sizeof(HTPresentation));
    if (pres == NULL) outofmem(__FILE__, "HTSetPresentation");
    
    pres->rep = HTAtom_for(representation);
    pres->rep_out = WWW_PRESENT;		/* Fixed for now ... :-) */
    pres->converter = HTSaveAndExecute;		/* Fixed for now ...     */
    pres->quality = quality;
    pres->secs = secs;
    pres->secs_per_byte = secs_per_byte;
    pres->rep = HTAtom_for(representation);
    pres->command = NULL;
    StrAllocCopy(pres->command, command);
    pres->test_command = NULL;
    StrAllocCopy(pres->test_command, test_command);
    HTList_addObject(conversions, pres);
}


/*	Define a built-in function for a content-type
**	---------------------------------------------
*/
PUBLIC void HTSetConversion ARGS7(
	HTList *,	conversions,
	CONST char *, 	representation_in,
	CONST char *, 	representation_out,
	HTConverter*,	converter,
	double,		quality,
	double,		secs, 
	double,		secs_per_byte)
{
    HTPresentation * pres = (HTPresentation *)malloc(sizeof(HTPresentation));
    if (pres == NULL) outofmem(__FILE__, "HTSetPresentation");
    
    pres->rep = HTAtom_for(representation_in);
    pres->rep_out = HTAtom_for(representation_out);
    pres->converter = converter;
    pres->command = NULL;		/* Fixed */
    pres->test_command = NULL;
    pres->quality = quality;
    pres->secs = secs;
    pres->secs_per_byte = secs_per_byte;
    HTList_addObject(conversions, pres);
}


/*
**	Cleanup memory after the GLOBAL list of converters. Note that there
**	is also a LOCAL conversion list associated with each HTRequest
**	structure. Written by Eric Sink, eric@spyglass.com
*/
PUBLIC void HTDisposeConversions NOARGS
{
    if (HTConversions) {
	HTList_delete(HTConversions);
	HTConversions = NULL;
    }
}


PUBLIC void HTAcceptEncoding ARGS3(HTList *,	list,
				   char *,	enc,
				   double,	quality)
{
    HTAcceptNode * node;
    char * cur;

    if (!list || !enc || !*enc) return;

    for(cur=enc; *cur; cur++) *cur=TOLOWER(*cur);

    node = (HTAcceptNode*)calloc(1, sizeof(HTAcceptNode));
    if (!node) outofmem(__FILE__, "HTAcceptEncoding");
    HTList_addObject(list, (void*)node);

    node->atom = HTAtom_for(enc);
    node->quality = quality;
}


PUBLIC void HTAcceptLanguage ARGS3(HTList *,	list,
				   char *,	lang,
				   double,	quality)
{
    HTAcceptNode * node;

    if (!list || !lang || !*lang) return;

    node = (HTAcceptNode*)calloc(1, sizeof(HTAcceptNode));
    if (!node) outofmem(__FILE__, "HTAcceptLanguage");

    HTList_addObject(list, (void*)node);
    node->atom = HTAtom_for(lang);
    node->quality = quality;
}


PRIVATE BOOL wild_match ARGS2(HTAtom *,	tmplate,
			      HTAtom *,	actual)
{
    char *t, *a, *st, *sa;
    BOOL match = NO;

    if (tmplate && actual && (t = HTAtom_name(tmplate))) {
	if (!strcmp(t, "*"))
	    return YES;

	if (strchr(t, '*') &&
	    (a = HTAtom_name(actual)) &&
	    (st = strchr(t, '/')) && (sa = strchr(a,'/'))) {

	    *sa = 0;
	    *st = 0;

	    if ((*(st-1)=='*' &&
		 (*(st+1)=='*' || !strcasecomp(st+1, sa+1))) ||
		(*(st+1)=='*' && !strcasecomp(t,a)))
		match = YES;

	    *sa = '/';
	    *st = '/';
	}    
    }
    return match;
}

/*
 * Added by takada@seraph.ntt.jp (94/04/08)
 */
PRIVATE BOOL lang_match ARGS2(HTAtom *,	tmplate,
			      HTAtom *,	actual)
{
    char *t, *a, *st, *sa;
    BOOL match = NO;

    if (tmplate && actual &&
	(t = HTAtom_name(tmplate)) && (a = HTAtom_name(actual))) {
	st = strchr(t, '_');
	sa = strchr(a, '_');
	if ((st != NULL) && (sa != NULL)) {
	    if (!strcasecomp(t, a))
	      match = YES;
	    else
	      match = NO;
	}
	else {
	    if (st != NULL) *st = 0;
	    if (sa != NULL) *sa = 0;
	    if (!strcasecomp(t, a))
	      match = YES;
	    else
	      match = NO;
	    if (st != NULL) *st = '_';
	    if (sa != NULL) *sa = '_';
	}
    }
    return match;
}
/* end of addition */



PRIVATE double type_value ARGS2(HTAtom *,	content_type,
			       HTList *,	accepted)
{
    HTList * cur = accepted;
    HTPresentation * pres;
    HTPresentation * wild = NULL;

    if (!content_type || !accepted) return -1;

    while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
	if (pres->rep == content_type)
	    return pres->quality;
	else if (wild_match(pres->rep, content_type))
	    wild = pres;
    }
    if (wild) return wild->quality;
    else return -1;
}


PRIVATE double lang_value ARGS2(HTAtom *,	language,
			       HTList *,	accepted)
{
    HTList * cur = accepted;
    HTAcceptNode * node;
    HTAcceptNode * wild = NULL;

    if (!language || !accepted || HTList_isEmpty(accepted)) {
	return 0.1;
    }

    while ((node = (HTAcceptNode*)HTList_nextObject(cur))) {
	if (node->atom == language) {
	    return node->quality;
	}
	/*
	 * patch by takada@seraph.ntt.jp (94/04/08)
	 * the original line was
	 * else if (wild_match(node->atom, language)) {
	 * and the new line is
	 */
	else if (lang_match(node->atom, language)) {
	    wild = node;
	}
    }

    if (wild) {
	return wild->quality;
    }
    else {
	return 0.1;
    }
}


PRIVATE double encoding_value ARGS2(HTAtom *,	encoding,
				   HTList *,	accepted)
{
    HTList * cur = accepted;
    HTAcceptNode * node;
    HTAcceptNode * wild = NULL;
    char * e;

    if (!encoding || !accepted || HTList_isEmpty(accepted))
	return 1;

    e = HTAtom_name(encoding);
    if (!strcmp(e, "7bit") || !strcmp(e, "8bit") || !strcmp(e, "binary"))
	return 1;

    while ((node = (HTAcceptNode*)HTList_nextObject(cur))) {
	if (node->atom == encoding)
	    return node->quality;
	else if (wild_match(node->atom, encoding))
	    wild = node;
    }
    if (wild) return wild->quality;
    else return 1;
}


PUBLIC BOOL HTRank ARGS4(HTList *, possibilities,
			 HTList *, accepted_content_types,
			 HTList *, accepted_languages,
			 HTList *, accepted_encodings)
{
    int accepted_cnt = 0;
    HTList * accepted;
    HTList * sorted;
    HTList * cur;
    HTContentDescription * d;

    if (!possibilities) return NO;

    accepted = HTList_new();
    cur = possibilities;
    while ((d = (HTContentDescription*)HTList_nextObject(cur))) {
	double tv = type_value(d->content_type, accepted_content_types);
	double lv = lang_value(d->content_language, accepted_languages);
	double ev = encoding_value(d->content_encoding, accepted_encodings);

	if (tv > 0) {
	    d->quality *= tv * lv * ev;
	    HTList_addObject(accepted, d);
	    accepted_cnt++;
	}
	else {
	    if (d->filename) free(d->filename);
	    free(d);
	}
    }

    if (PROT_TRACE) fprintf(TDEST, "Ranking.....\n");
    if (PROT_TRACE) fprintf(TDEST,
	   "\nRANK QUALITY CONTENT-TYPE         LANGUAGE ENCODING    FILE\n");

    sorted = HTList_new();
    while (accepted_cnt-- > 0) {
	HTContentDescription * worst = NULL;
	cur = accepted;
	while ((d = (HTContentDescription*)HTList_nextObject(cur))) {
	    if (!worst || d->quality < worst->quality)
		worst = d;
	}
	if (worst) {
	    if (PROT_TRACE)
		fprintf(TDEST, "%d.   %.4f  %-20.20s %-8.8s %-10.10s %s\n",
			accepted_cnt+1,
			worst->quality,
			(worst->content_type
		         ? HTAtom_name(worst->content_type)      : "-"),
			(worst->content_language
		         ? HTAtom_name(worst->content_language)  :"-"),
			(worst->content_encoding
		         ? HTAtom_name(worst->content_encoding)  :"-"),
			(worst->filename
		         ? worst->filename                       :"-"));
	    HTList_removeObject(accepted, (void*)worst);
	    HTList_addObject(sorted, (void*)worst);
	}
    }
    if (PROT_TRACE) fprintf(TDEST, "\n");
    HTList_delete(accepted);
    HTList_delete(possibilities->next);
    possibilities->next = sorted->next;
    sorted->next = NULL;
    HTList_delete(sorted);

    if (!HTList_isEmpty(possibilities)) return YES;
    else return NO;
}





/*			Socket Input Buffering
**			----------------------
**
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


PRIVATE BOOL better_match ARGS2(HTFormat, f,
				HTFormat, g)
{
    CONST char *p, *q;

    if (f && g  &&  (p = HTAtom_name(f))  &&  (q = HTAtom_name(g))) {
	int i,j;
	for(i=0 ; *p; p++) if (*p == '*') i++;
	for(j=0 ; *q; q++) if (*q == '*') j++;
	if (i < j) return YES;
    }
    return NO;
}


/*		Create a filter stack
**		---------------------
**
**	If a wildcard match is made, a temporary HTPresentation
**	structure is made to hold the destination format while the
**	new stack is generated. This is just to pass the out format to
**	MIME so far.  Storing the format of a stream in the stream might
**	be a lot neater.
**
**	The star/star format is special, in that if you can take
**	that you can take anything.
**
**	On succes, request->error_block is set to YES so no more error
**	messages to the stream as the stream might be of any format.
*/
PUBLIC HTStream * HTStreamStack ARGS5(HTFormat,		rep_in,
				      HTFormat,		rep_out,
				      HTStream *,	output_stream,
				      HTRequest *,	request,
				      BOOL,		guess)
{
    HTList * conversion[2];
    int which_list;
    double best_quality = -1e30;		/* Pretty bad! */
    HTPresentation *pres, *match, *best_match=0;
    
    request->error_block = YES;		   /* No more error output to stream */
    if (TRACE) fprintf(TDEST,
    	"StreamStack. Constructing stream stack for %s to %s\n",
	HTAtom_name(rep_in),	
	HTAtom_name(rep_out));

    if (guess  &&  rep_in == WWW_UNKNOWN) {
	if (PROT_TRACE) fprintf(TDEST, "Returning... guessing stream\n");
	return HTGuess_new(request, NULL, rep_in, rep_out, output_stream);
    }

    if (rep_out == WWW_SOURCE || rep_out == rep_in) {
	return output_stream;
    }

    conversion[0] = request->conversions;
    conversion[1] = HTConversions;

    for(which_list = 0; which_list<2; which_list++) {
	HTList * cur = conversion[which_list];
	
	while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
	    if	((pres->rep == rep_in || wild_match(pres->rep, rep_in)) &&
		 (pres->rep_out == rep_out || wild_match(pres->rep_out, rep_out))) {
		if (!best_match ||
		    better_match(pres->rep, best_match->rep) ||
		    (!better_match(best_match->rep, pres->rep) &&
		     pres->quality > best_quality)) {
#ifdef GOT_SYSTEM
		    if (!pres->test_command||(system(pres->test_command)==0)){ 
			if (TRACE && pres->test_command) 
			    fprintf(TDEST, "HTStreamStack testing %s %d\n",pres->test_command,system(pres->test_command)); 
			best_match = pres;
			best_quality = pres->quality;
		    }
#endif /* GOT_SYSTEM */
		}
	    }
	}
    }

    match = best_match ? best_match : NULL;
    if (match) {
	if (match->rep == WWW_SOURCE) {
	    if (TRACE) fprintf(TDEST, "StreamStack. Don't know how to handle this, so put out %s to %s\n",
			       HTAtom_name(match->rep), 
			       HTAtom_name(rep_out));
	}
	return (*match->converter)(request, match->command, rep_in, rep_out,
				   output_stream);
    }
    {
	char *msg = NULL;
	StrAllocCopy(msg, "Can't convert from ");
	StrAllocCat(msg, HTAtom_name(rep_in));
	StrAllocCat(msg, " to ");
	StrAllocCat(msg, HTAtom_name(rep_out));
	HTErrorAdd(request, ERR_FATAL, NO, HTERR_NOT_IMPLEMENTED,
		   (void *) msg, (int) strlen(msg), "HTStreamStack");
	free(msg);
    }
    request->error_block = NO;		 /* We didn't put up a stream anyway */
    return NULL;
}
	

/*		Find the cost of a filter stack
**		-------------------------------
**
**	Must return the cost of the same stack which StreamStack would set up.
**
** On entry,
**	length	The size of the data to be converted
*/
PUBLIC double HTStackValue ARGS5(
	HTList *,		theseConversions,
	HTFormat,		rep_in,
	HTFormat,		rep_out,
	double,			initial_value,
	long int,		length)
{
    int which_list;
    HTList* conversion[2];
    
    if (TRACE) fprintf(TDEST,
    	"StackValue.. Evaluating stream stack for %s worth %.3f to %s\n",
	HTAtom_name(rep_in),	initial_value,
	HTAtom_name(rep_out));
		
    if (rep_out == WWW_SOURCE ||
    	rep_out == rep_in) return 0.0;

    conversion[0] = theseConversions;
    conversion[1] = HTConversions;
    
    for(which_list = 0; which_list<2; which_list++)
     if (conversion[which_list]) {
        HTList * cur = conversion[which_list];
	HTPresentation * pres;
	while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
	    if (pres->rep == rep_in &&
		(pres->rep_out == rep_out || wild_match(pres->rep_out, rep_out))) {
	        double value = initial_value * pres->quality;
		if (HTMaxSecs != 0.0)
		    value = value - (length*pres->secs_per_byte + pres->secs)
			                 /HTMaxSecs;
		return value;
	    }
	}
    }
    
    return -1e30;		/* Really bad */
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
	|| (request->content_encoding &&
	    request->content_encoding != HTAtom_for("8bit") &&
	    request->content_encoding != HTAtom_for("7bit"))
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
PUBLIC int HTParseFile ARGS3(
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


/*	Converter stream: Network Telnet to internal character text
**	-----------------------------------------------------------
**
**	The input is assumed to be in local representation with lines
**	delimited by (CR,LF) pairs in the local representation.
**	The (CR,LF) sequenc when found is changed to a '\n' character,
**	the internal C representation of a new line.
*/
PRIVATE void NetToText_put_character ARGS2(HTStream *, me, char, c)
{
    if (me->had_cr) {
        if (c==LF) {
	    me->sink->isa->put_character(me->sink, '\n');	  /* Newline */
	    me->had_cr = NO;
	    return;
        } else {
	    me->sink->isa->put_character(me->sink, CR);		 /* leftover */
	}
    }
    me->had_cr = (c==CR);
    if (!me->had_cr)
	me->sink->isa->put_character(me->sink, c);		   /* normal */
}

PRIVATE void NetToText_put_string ARGS2(HTStream *, me, CONST char *, s)
{    
    CONST char *p=s;
    while (*p) {
	if (me->had_cr) {
	    if (*p==LF) {
		me->sink->isa->put_character(me->sink, '\n');	  /* Newline */
		s++;
	    } else
		me->sink->isa->put_character(me->sink, CR);	 /* leftover */
	}
	me->had_cr = (*p==CR);
	if (me->had_cr) {
	    me->sink->isa->put_block(me->sink, s, p-s);
	    s=p+1;
	}
	p++;
    }
    if (p-s)
	me->sink->isa->put_block(me->sink, s, p-s);
}


PRIVATE void NetToText_put_block ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    CONST char *p=s;
    while (l-- > 0) {
	if (me->had_cr) {
	    if (*p==LF) {
		me->sink->isa->put_character(me->sink, '\n');	  /* Newline */
		s++;
	    } else
		me->sink->isa->put_character(me->sink, CR);	 /* leftover */
	}
	me->had_cr = (*p==CR);
	if (me->had_cr) {
	    me->sink->isa->put_block(me->sink, s, p-s);
	    s=p+1;
	}
	p++;
    }
    if (p-s)
	me->sink->isa->put_block(me->sink, s, p-s);
}

PRIVATE int NetToText_free ARGS1(HTStream *, me)
{
    me->sink->isa->_free(me->sink);		/* Close rest of pipe */
    free(me);
    return 0;
}

PRIVATE int NetToText_abort ARGS2(HTStream *, me, HTError, e)
{
    me->sink->isa->abort(me->sink,e);		/* Abort rest of pipe */
    free(me);
    return 0;
}

PRIVATE HTStreamClass NetToTextClass = {
    "NetToText",
    NetToText_free,
    NetToText_abort,
    NetToText_put_character,
    NetToText_put_string,
    NetToText_put_block
};

PUBLIC HTStream * HTNetToText ARGS1(HTStream *, sink)
{
    HTStream* me = (HTStream *) calloc(1, sizeof(*me));
    if (me == NULL) outofmem(__FILE__, "NetToText");
    me->isa = &NetToTextClass;
    
    me->had_cr = NO;
    me->sink = sink;
    return me;
}

/* ------------------------------------------------------------------------- */
/* MULTI THREADED IMPLEMENTATIONS					     */
/* ------------------------------------------------------------------------- */

/*	Push data from a socket down a stream
**	-------------------------------------
**
**   This routine is responsible for creating and PRESENTING any
**   graphic (or other) objects described by the file.
**
**
** Returns      HT_LOADED	if finished reading
**	      	EOF		if error,
**    		HT_INTERRUPTED 	if interrupted
**     		HT_WOULD_BLOCK  if read would block
*/
PUBLIC int HTInputSocket_read ARGS2(HTInputSocket *, isoc, HTStream *, target)
{
    int b_read;

    if (!isoc || isoc->input_file_number==INVSOC) {
	if (PROT_TRACE) fprintf(TDEST, "Read Socket. Bad argument\n");
	return EOF;
    }

    /*	Push binary from socket down sink */
    for(;;) {
	if (HTThreadIntr(isoc->input_file_number))	      /* Interrupted */
	    return HT_INTERRUPTED;
	if ((b_read = NETREAD(isoc->input_file_number, isoc->input_buffer,
			      INPUT_BUFFER_SIZE)) < 0) {
#ifdef EAGAIN
	    if (socerrno==EAGAIN || socerrno==EWOULDBLOCK)    /* POSIX, SVR4 */
#else
	    if (socerrno == EWOULDBLOCK)			      /* BSD */
#endif
	    {
		if (THD_TRACE)
		    fprintf(TDEST, "Read Socket. WOULD BLOCK soc %d\n",
			    isoc->input_file_number);
		HTThreadState(isoc->input_file_number, THD_SET_READ);
		return HT_WOULD_BLOCK;
	    } else {				     /* We have a real error */
		if (PROT_TRACE)
		    fprintf(TDEST, "Read Socket. READ ERROR %d\n", socerrno);
		return EOF;
	    }
	} else if (!b_read) {
	    HTThreadState(isoc->input_file_number, THD_CLR_READ);
	    return HT_LOADED;
	}

#ifdef NOT_ASCII
	isoc->input_limit = isoc->input_buffer + b_read;
	{
	    char *p;
	    for(p = isoc->input_buffer; p < isoc->input_limit; p++)
		*p = FROMASCII(*p);
	}
#endif

	/* This is based on the assumption that we actually get rid of ALL
	   the bytes we have read. Maybe more feedback! */
	if (PROT_TRACE)
	    fprintf(TDEST, "Read Socket. %d bytes read\n", b_read);
	(*target->isa->put_block)(target, isoc->input_buffer, b_read);
	isoc->input_pointer += b_read;
    }
}
