
/*		Manage different file formats			HTFormat.c
**		=============================
**
** Bugs:
**	Not reentrant.
**
**	Assumes the incoming stream is ASCII, rather than a local file
**	format, and so ALWAYS converts from ASCII on non-ASCII machines.
**	Therefore, non-ASCII machines can't read local files.
**
** HISTORY:
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**
*/


/* Implements:
*/
#include "HTFormat.h"

PUBLIC float HTMaxSecs = 1e10;		/* No effective limit */
PUBLIC float HTMaxLength = 1e10;	/* No effective limit */

#ifdef unix
#ifdef NeXT
#define PRESENT_POSTSCRIPT "open %s; /bin/rm -f %s\n"
#else
#define PRESENT_POSTSCRIPT "(ghostview %s ; /bin/rm -f %s)&\n"	
	/* Full pathname would be better! */
#endif
#endif


#include "HTUtils.h"
#include "tcp.h"

#include "HTML.h"
#include "HTMLPDTD.h"
#include "HTAlert.h"
#include "HTList.h"
#include "HTInit.h"
/*	Streams and structured streams which we use:
*/
#include "HTFWriter.h"
#include "HTPlain.h"
#include "SGML.h"
#include "HTML.h"
#include "HTMLGen.h"
#include "HTTCP.h"
#include "HTGuess.h"
#include "HTError.h"


PUBLIC	BOOL HTOutputSource = NO;	/* Flag: shortcut parser to stdout */

#ifdef ORIGINAL
struct _HTStream {
      CONST HTStreamClass*	isa;
      /* ... */
};
#endif

/* this version used by the NetToText stream */
struct _HTStream {
	CONST HTStreamClass *		isa;
	BOOL			had_cr;
	HTStream * 		sink;
};


/*
** Accept-Encoding and Accept-Language
*/
typedef struct _HTAcceptNode {
    HTAtom *	atom;
    float	quality;
} HTAcceptNode;




/*	Presentation methods
**	--------------------
*/

PUBLIC HTList * HTConversions = NULL;

/* -------------------------------------------------------------------------
   This function replaces the code in HTRequest_delete() in order to keep
   the data structure hidden (it is NOT a joke!)
   Henrik 14/03-94
   ------------------------------------------------------------------------- */
PUBLIC void HTFormatDelete ARGS1(HTList *, me)
{
    HTList *cur = me;
    HTPresentation *pres;
    if (!me)
	return;
    while ((pres = (HTPresentation*) HTList_nextObject(cur))) {
	FREE(pres->command);			 /* Leak fixed AL 6 Feb 1994 */
	free(pres);
    }
    HTList_delete(me);				 /* Leak fixed AL 6 Feb 1994 */
}


/*	Define a presentation system command for a content-type
**	-------------------------------------------------------
*/
PUBLIC void HTSetPresentation ARGS6(
	HTList *,	conversions,
	CONST char *, 	representation,
	CONST char *, 	command,
	float,		quality,
	float,		secs, 
	float,		secs_per_byte
){

    HTPresentation * pres = (HTPresentation *)malloc(sizeof(HTPresentation));
    if (pres == NULL) outofmem(__FILE__, "HTSetPresentation");
    
    pres->rep = HTAtom_for(representation);
    pres->rep_out = WWW_PRESENT;		/* Fixed for now ... :-) */
    pres->converter = HTSaveAndExecute;		/* Fixed for now ...     */
    pres->quality = quality;
    pres->secs = secs;
    pres->secs_per_byte = secs_per_byte;
    pres->rep = HTAtom_for(representation);
    pres->command = 0;
    StrAllocCopy(pres->command, command);
    
/*    if (!HTPresentations) HTPresentations = HTList_new(); */
    
#ifdef OLD_CODE
    if (strcmp(representation, "*")==0) {
        if (default_presentation) free(default_presentation);
	default_presentation = pres;
    } else 
#endif
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
	float,		quality,
	float,		secs, 
	float,		secs_per_byte
){

    HTPresentation * pres = (HTPresentation *)malloc(sizeof(HTPresentation));
    if (pres == NULL) outofmem(__FILE__, "HTSetPresentation");
    
    pres->rep = HTAtom_for(representation_in);
    pres->rep_out = HTAtom_for(representation_out);
    pres->converter = converter;
    pres->command = NULL;		/* Fixed */
    pres->quality = quality;
    pres->secs = secs;
    pres->secs_per_byte = secs_per_byte;
    pres->command = 0;
    
/*    if (!HTPresentations) HTPresentations = HTList_new();  */
    
#ifdef OLD_CODE
    if (strcmp(representation_in, "*")==0) {
        if (default_presentation) free(default_presentation);
	default_presentation = pres;
    } else 
#endif
    HTList_addObject(conversions, pres);
}



PUBLIC void HTAcceptEncoding ARGS3(HTList *,	list,
				   char *,	enc,
				   float,	quality)
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
				   float,	quality)
{
    HTAcceptNode * node;

    if (!list || !lang || !*lang) return;

    node = (HTAcceptNode*)calloc(1, sizeof(HTAcceptNode));
    if (!node) outofmem(__FILE__, "HTAcceptLanguage");

    HTList_addObject(list, (void*)node);
    node->atom = HTAtom_for(lang);
    node->quality = quality;
}


PRIVATE BOOL wild_match ARGS2(HTAtom *,	template,
			      HTAtom *,	actual)
{
    char *t, *a, *st, *sa;
    BOOL match = NO;

    if (template && actual && (t = HTAtom_name(template))) {
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
PRIVATE BOOL lang_match ARGS2(HTAtom *,	template,
			      HTAtom *,	actual)
{
    char *t, *a, *st, *sa;
    BOOL match = NO;

    if (template && actual &&
	(t = HTAtom_name(template)) && (a = HTAtom_name(actual))) {
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



PRIVATE float type_value ARGS2(HTAtom *,	content_type,
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


PRIVATE float lang_value ARGS2(HTAtom *,	language,
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


PRIVATE float encoding_value ARGS2(HTAtom *,	encoding,
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
	float tv = type_value(d->content_type, accepted_content_types);
	float lv = lang_value(d->content_language, accepted_languages);
	float ev = encoding_value(d->content_encoding, accepted_encodings);

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

    CTRACE(stderr, "Ranking.....\n");
    CTRACE(stderr,
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
	    CTRACE(stderr, "%d.   %.4f  %-20.20s %-8.8s %-10.10s %s\n",
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
    CTRACE(stderr, "\n");
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
PUBLIC HTInputSocket * HTInputSocket_new ARGS1 (int,file_number)
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
	if (isoc-> input_pointer >= isoc->input_limit) {
	    int status = NETREAD(
		   isoc->input_file_number,
		   isoc->input_buffer, INPUT_BUFFER_SIZE);
	    if (status <= 0) {
		if (status == 0)
		    return EOF;
		if (status == HT_INTERRUPTED) {
		    if (TRACE)
			fprintf(stderr, "Get Char.... Interrupted in HTInputSocket_getCharacter\n");
		    return HT_INTERRUPTED;
		}
		HTInetStatus("read");
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
	    if (status < 0)
		HTInetStatus("read");
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
	    if (status < 0)
		HTInetStatus("read");
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


/*
** Read HTTP status line (if there is one).
**
** Kludge to trap binary responses from illegal HTTP0.9 servers.
** First look at the stub in ASCII and check if it starts "HTTP/".
**
** Bugs: A HTTP0.9 server returning a document starting "HTTP/"
**	 will be taken as a HTTP 1.0 server.  Failure.
*/
#define STUB_LENGTH 20
PUBLIC char * HTInputSocket_getStatusLine ARGS1(HTInputSocket *, isoc)
{
    if (!isoc) {
	return NULL;
    }
    else {
	char buf[STUB_LENGTH + 1];
	int i;
	char server_version[STUB_LENGTH+1];
	int server_status;

	/*
	** Read initial buffer
	*/
	if (isoc->input_pointer >= isoc->input_limit &&
	    fill_in_buffer(isoc) <= 0) {
	    return NULL;
        }

	for (i=0; i < STUB_LENGTH; i++)
	    buf[i] = FROMASCII(isoc->input_buffer[i]);
	buf[STUB_LENGTH] = 0;

	if (0 != strncmp(buf, "HTTP/", 5) ||
	    sscanf(buf, "%20s%d", server_version, &server_status) < 2)
	    return NULL;
	else
	    return get_some_line(isoc, NO);
    }
}


/*
** Do heuristic test to see if this is binary.
**
** We check for characters above 128 in the first few bytes, and
** if we find them we forget the html default.
** Kludge to trap binary responses from illegal HTTP0.9 servers.
**
** Bugs: An HTTP 0.9 server returning a binary document with
**	 characters < 128 will be read as ASCII.
*/
PUBLIC BOOL HTInputSocket_seemsBinary ARGS1(HTInputSocket *, isoc)
{
    if (isoc &&
	(isoc->input_pointer < isoc->input_limit ||
	 fill_in_buffer(isoc) > 0)) {
	char *p = isoc->input_buffer;
	int i = STUB_LENGTH;

	for( ; i && p < isoc->input_limit; p++, i++)
	    if (((int)*p)&128)
		return YES;
    }
    return NO;
}



/*	Stream the data to an ouput file as binary
*/
PUBLIC int HTOutputBinary ARGS3(HTInputSocket *,isoc,
				int, 		input,
				FILE *, 	output)
{
    do {
	    int status = NETREAD(
		    input, isoc->input_buffer, INPUT_BUFFER_SIZE);
	    if (status <= 0) {
		if (status == 0) return 0;
		if (TRACE) fprintf(stderr,
		    "Out Binary.. Socket read error %d\n", status);
		return 2;			/* Error */
	    }
	    fwrite(isoc->input_buffer, sizeof(char), status, output);
    } while (YES);
}


/*
 * Normal HTTP headers are never bigger than 2K.
 */
#define S_BUFFER_SIZE 2000

PUBLIC void HTInputSocket_startBuffering ARGS1(HTInputSocket *,	isoc)
{
    if (isoc) {
	isoc->s_do_buffering = YES;
	if (!isoc->s_buffer) {
	    isoc->s_buffer = (char*)malloc(S_BUFFER_SIZE + 1);
	    isoc->s_buffer_size = S_BUFFER_SIZE;
	}
	isoc->s_buffer_cur = isoc->s_buffer;
    }
}

PUBLIC void HTInputSocket_stopBuffering ARGS1(HTInputSocket *, isoc)
{
    if (isoc) {
	isoc->s_do_buffering = NO;
	if (isoc->s_buffer_cur)
	    *isoc->s_buffer_cur = 0;
    }
}

PUBLIC int HTInputSocket_getBuffer ARGS2(HTInputSocket *,	isoc,
					 char **,		buffer_ptr)
{
    if (!isoc || !isoc->s_buffer || !isoc->s_buffer_cur)
	return 0;
    else {
	*isoc->s_buffer_cur = 0;
	if (buffer_ptr)
	    *buffer_ptr = isoc->s_buffer;
	return (int) (isoc->s_buffer_cur - isoc->s_buffer);
    }
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
PUBLIC HTStream * HTStreamStack ARGS3(HTFormat,		rep_in,
				      HTRequest *,	request,
				      BOOL,		guess)
{
    HTFormat rep_out = request->output_format;	/* Could be a param */
    HTList * conversion[2];
    int which_list;
    float best_quality = -1e30;		/* Pretty bad! */
    HTPresentation *pres, *match, *best_match=0;
    
    request->error_block = YES;		   /* No more error output to stream */
    if (TRACE) fprintf(stderr,
    	"StreamStack. Constructing stream stack for %s to %s\n",
	HTAtom_name(rep_in),	
	HTAtom_name(rep_out));

    if (guess  &&  rep_in == WWW_UNKNOWN) {
	CTRACE(stderr, "Returning... guessing stream\n");
	return HTGuess_new(request);
    }

    if (rep_out == WWW_SOURCE || rep_out == rep_in) {
	return request->output_stream;
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
		    best_match = pres;
		    best_quality = pres->quality;
		}
	    }
	}
    }

    match = best_match ? best_match : NULL;
    if (match) {
	if (match->rep == WWW_SOURCE) {
	    if (TRACE) fprintf(stderr, "StreamStack. Don't know how to handle this, so put out %s to %s\n",
			       HTAtom_name(match->rep), 
			       HTAtom_name(rep_out));
	}
	return (*match->converter)(
	request, match->command, rep_in, rep_out,
	request->output_stream);
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
PUBLIC float HTStackValue ARGS5(
	HTList *,		theseConversions,
	HTFormat,		rep_in,
	HTFormat,		rep_out,
	float,			initial_value,
	long int,		length)
{
    int which_list;
    HTList* conversion[2];
    
    if (TRACE) fprintf(stderr,
    	"StackValue.. Evaluating stream stack for %s worth %.3f to %s\n",
	HTAtom_name(rep_in),	initial_value,
	HTAtom_name(rep_out));
		
    if (rep_out == WWW_SOURCE ||
    	rep_out == rep_in) return 0.0;

 /*   if (!HTPresentations) HTFormatInit();	 set up the list */
    
    conversion[0] = theseConversions;
    conversion[1] = HTConversions;
    
    for(which_list = 0; which_list<2; which_list++)
     if (conversion[which_list]) {
        HTList * cur = conversion[which_list];
	HTPresentation * pres;
	while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
	    if (pres->rep == rep_in &&
		(pres->rep_out == rep_out || wild_match(pres->rep_out, rep_out))) {
	        float value = initial_value * pres->quality;
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
	int,			file_number,
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
	    if (TRACE) fprintf(stderr,
		"Socket Copy. Read error, read returns %d with errno=%d\n",
		status, errno);
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
	    if (TRACE) fprintf(stderr,
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
	int,			file_number,
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


/* To be replaced by a stream */
PUBLIC void HTCopyDot ARGS2(int,	file_number,
			    HTStream *,	sink)
{
    HTStreamClass targetClass;
    HTInputSocket * isoc;   
    int ch;
    int state=3;
    
    /* Push the data, ignoring CRLF, down the stream */
    targetClass = *(sink->isa);		      /* Copy pointers to procedures */
    isoc = HTInputSocket_new(file_number);
    while (state && (ch = HTInputSocket_getCharacter(isoc)) >= 0) {
	if (ch == '\n')
	    state--;
	else if (state==2 && ch=='.')
	    state--;
	else
	    state = 3;
	(*targetClass.put_character)(sink, ch);
    }
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
	int,			file_number,
	HTRequest *,		request)
{
    HTStream * stream;
    HTStreamClass targetClass;    

    if (request->error_stack) {
	if (TRACE) fprintf(stderr, "ParseSocket. Called whith non-empty error stack, so I return right away!\n");
	return -1;
    }

    /* Set up stream stack */
    if ((stream = HTStreamStack(rep_in, request, YES)) == NULL)
	return -1;
    
/*	Push the data, ignoring CRLF if necessary, down the stream
**
**
**   @@  Bug:  This decision ought to be made based on "encoding"
**   rather than on format.  @@@  When we handle encoding.
**   The current method smells anyway.
*/
    targetClass = *(stream->isa);	/* Copy pointers to procedures */
    if (request->output_format == WWW_SOURCE && request->net_info->CRLFdotCRLF)
	HTCopyDot(file_number, stream);
    else if (rep_in == WWW_BINARY || rep_in == WWW_UNKNOWN || HTOutputSource
	|| (request->content_encoding &&
	    request->content_encoding != HTAtom_for("8bit") &&
	    request->content_encoding != HTAtom_for("7bit"))
        || strstr(HTAtom_name(rep_in), "image/")
	|| strstr(HTAtom_name(rep_in), "video/")) { /* @@@@@@ */
	HTCopy(file_number, stream);
    } else if (request->net_info->CRLFdotCRLF)
        HTCopyDot(file_number, stream);
    else
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
	if (TRACE) fprintf(stderr, "ParseFile... Called whith non-empty error stack, so I return right away!\n");
	return -1;
    }

    /* Set up stream stack */
    if ((stream = HTStreamStack(rep_in, request, YES)) == NULL)
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
**	The input is assumed to be in ASCII, with lines delimited
**	by (13,10) pairs, These pairs are converted into (CR,LF)
**	pairs in the local representation.  The (CR,LF) sequence
**	when found is changed to a '\n' character, the internal
**	C representation of a new line.
*/


PRIVATE void NetToText_put_character ARGS2(HTStream *, me, char, net_char)
{
    char c = FROMASCII(net_char);
    if (me->had_cr) {
        if (c==LF) {
	    me->sink->isa->put_character(me->sink, '\n');	/* Newline */
	    me->had_cr = NO;
	    return;
        } else {
	    me->sink->isa->put_character(me->sink, CR);	/* leftover */
	}
    }
    me->had_cr = (c==CR);
    if (!me->had_cr)
	me->sink->isa->put_character(me->sink, c);		/* normal */
}

PRIVATE void NetToText_put_string ARGS2(HTStream *, me, CONST char *, s)
{
    CONST char * p;
    for(p=s; *p; p++) NetToText_put_character(me, *p);
}

PRIVATE void NetToText_put_block ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    CONST char * p;
    for(p=s; p<(s+l); p++) NetToText_put_character(me, *p);
}

PRIVATE void NetToText_free ARGS1(HTStream *, me)
{
    me->sink->isa->_free(me->sink);		/* Close rest of pipe */
    free(me);
}

PRIVATE void NetToText_abort ARGS2(HTStream *, me, HTError, e)
{
    me->sink->isa->abort(me->sink,e);		/* Abort rest of pipe */
    free(me);
}

/*	The class structure
*/
PRIVATE HTStreamClass NetToTextClass = {
    "NetToText",
    NetToText_free,
    NetToText_abort,
    NetToText_put_character,
    NetToText_put_string,
    NetToText_put_block
};

/*	The creation method
*/
PUBLIC HTStream * HTNetToText ARGS1(HTStream *, sink)
{
    HTStream* me = (HTStream*)malloc(sizeof(*me));
    if (me == NULL) outofmem(__FILE__, "NetToText");
    me->isa = &NetToTextClass;
    
    me->had_cr = NO;
    me->sink = sink;
    return me;
}


