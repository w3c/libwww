
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
#include "HText.h"
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

PUBLIC	BOOL HTOutputSource = NO;	/* Flag: shortcut parser to stdout */
extern  BOOL interactive;

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


/*	Presentation methods
**	--------------------
*/

PUBLIC HTList * HTConversions = NULL;


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
    HTInputSocket *isoc = (HTInputSocket *)malloc(sizeof(*isoc));
    if (!isoc) outofmem(__FILE__, "HTInputSocket_new");
    isoc->input_file_number = file_number;
    isoc->input_pointer = isoc->input_limit = isoc->input_buffer;
    return isoc;
}


PUBLIC char HTInputSocket_getCharacter ARGS1(HTInputSocket*, isoc)
{
    char ch;
    do {
	if (isoc-> input_pointer >= isoc->input_limit) {
	    int status = NETREAD(
		   isoc->input_file_number,
		   isoc->input_buffer, INPUT_BUFFER_SIZE);
	    if (status <= 0) {
		if (status == 0) return (char)EOF;
		if (TRACE) fprintf(stderr,
		    "HTFormat: File read error %d\n", status);
		return (char)EOF; /* -1 is returned by UCX at end of HTTP link */
	    }
	    isoc-> input_pointer = isoc->input_buffer;
	    isoc->input_limit = isoc->input_buffer + status;
	}
	ch = *isoc-> input_pointer++;
    } while (ch == (char) 13); /* Ignore ASCII carriage return */
    
    return FROMASCII(ch);
}

PUBLIC void HTInputSocket_free(HTInputSocket * me)
{
    if (me) free(me);
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
		if (TRACE) fprintf(stderr,
				   "HTInputSocket: File read error %d\n",
				   status);
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
#if 0
		if (TRACE) fprintf(stderr, "HTInputSocket: reading more data\n");
#endif
		ascii_cat(&line, start, cur);
		if (fill_in_buffer(isoc) <= 0)
		    return line;
		start = cur = isoc->input_pointer;
	    } /* if need more data */

	    /*
	    ** Find a line feed if there is one
	    */
#if 0
	    if (TRACE) fprintf(stderr, "HTInputSocket: processing read buffer\n");
#endif
	    for(; cur < isoc->input_limit; cur++) {
		char c = FROMASCII(*cur);
		if (!c) {
#if 0
		    if (TRACE) fprintf(stderr, "HTInputSocket: panic, read zero!\n");
#endif
		    return NULL;	/* Panic! read a 0! */
		}
		if (check_unfold  &&  c != ' '  &&  c != '\t') {
#if 0
		    if (TRACE) fprintf(stderr, "HTInputSocket: returning \"%s\"\n",
				       (line ? line : "(null)"));
#endif
		    return line;  /* Note: didn't update isoc->input_pointer */
		}
		else {
		    check_unfold = NO;
		}

		if (c=='\r') {
#if 0
		    if (TRACE) fprintf(stderr, "HTInputSocket: found linefeed\n");
#endif
		    prev_cr = 1;
		}
		else {
		    if (c=='\n') {		/* Found a line feed */
#if 0
			if (TRACE) fprintf(stderr, "HTInputSocket: found newline\n");
#endif
			ascii_cat(&line, start, cur-prev_cr);
			start = isoc->input_pointer = cur+1;

			if (line && strlen(line) > 0 && unfold) {
#if 0
			    if (TRACE) fprintf(stderr, "HTInputSocket: next time check unfolding\n");
#endif
			    check_unfold = YES;
			}
			else {
#if 0
			    if (TRACE) fprintf(stderr,
					       "HTInputSocket: no unfold check -- just return \"%s\"\n",
					       (line ? line : "(line)"));
#endif
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


PUBLIC char * HTInputSocket_getLine ARGS1(HTInputSocket *, isoc)
{
    return get_some_line(isoc, NO);
}

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
PUBLIC int HTOutputBinary ARGS3( HTInputSocket *, isoc,
				int, 		input,
				FILE *, 	output)
{
    do {
	    int status = NETREAD(
		    input, isoc->input_buffer, INPUT_BUFFER_SIZE);
	    if (status <= 0) {
		if (status == 0) return 0;
		if (TRACE) fprintf(stderr,
		    "HTFormat: File read error %d\n", status);
		return 2;			/* Error */
	    }
	    fwrite(isoc->input_buffer, sizeof(char), status, output);
    } while (YES);
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
**	The www/source format is special, in that if you can take
**	that you can take anything. However, we
*/
PUBLIC HTStream * HTStreamStack ARGS2(
	HTFormat,		rep_in,
	HTRequest *,		request)
{
    HTFormat rep_out = request->output_format;	/* Could be a param */
    HTList * conversion[2];
    HTAtom * wildcard = HTAtom_for("*");
    HTFormat source = WWW_SOURCE;
    int which_list;
    HTPresentation * pres, *match, *wildcard_match=0,
			*source_match=0, *source_wildcard_match=0;
    
    if (TRACE) fprintf(stderr,
    	"HTFormat: Constructing stream stack for %s to %s\n",
	HTAtom_name(rep_in),	
	HTAtom_name(rep_out));
		

#ifdef BUG_OUT	/* by Lou Montulli 16 Aug 93, put in by AL 6 Dec 93 */
    if (rep_out == WWW_SOURCE ||
    	rep_out == rep_in) return request->output_stream;
#endif
    if (rep_out == rep_in) return request->output_stream;

    conversion[0] = request->conversions;
    conversion[1] = HTConversions;
    
    for(which_list = 0; which_list<2; which_list++) {
	HTList * cur = conversion[which_list];
	
	while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
	    if (pres->rep == rep_in) {
	        if (pres->rep_out == rep_out)
	            return (*pres->converter)(request, pres->command,
					      rep_in, pres->rep_out,
					      request->output_stream);
		if (pres->rep_out == wildcard) {
		    wildcard_match = pres;
		}
	    }
	    if (pres->rep == source) {
	        if (pres->rep_out == rep_out)
	            source_match = pres;
		if (pres->rep_out == wildcard) {
		    source_wildcard_match = pres;
		}
	    }
	}
    }
    match = wildcard_match ? wildcard_match :
	    source_match ?	source_match : 
	    source_wildcard_match;
    
    if (match) return (*match->converter)(
		request, match->command, rep_in, rep_out,
		request->output_stream);

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
    HTAtom * wildcard = HTAtom_for("*");
    int which_list;
    HTList* conversion[2];
    
    if (TRACE) fprintf(stderr,
    	"HTFormat: Evaluating stream stack for %s worth %.3f to %s\n",
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
		(pres->rep_out == rep_out || pres->rep_out == wildcard)) {
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
*/
PUBLIC void HTCopy ARGS2(
	int,			file_number,
	HTStream*,		sink)
{
    HTStreamClass targetClass;    
    HTInputSocket * isoc;
    
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
		"HTFormat: Read error, read returns %d\n", status);
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
    } /* next bufferload */
    HTInputSocket_free(isoc);
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
		"HTFormat: Read error, read returns %d\n", ferror(fp));
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
*/
PUBLIC void HTCopyNoCR ARGS2(
	int,			file_number,
	HTStream*,		sink)
{
    HTStreamClass targetClass;
    HTInputSocket * isoc;   
    
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
    for(;;) {
	char character;
	character = HTInputSocket_getCharacter(isoc);
	if (character == (char)EOF) break;
	(*targetClass.put_character)(sink, character);           
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
*/

PUBLIC int HTParseSocket ARGS3(
	HTFormat,		rep_in,
	int,			file_number,
	HTRequest *,		request)
{
    HTStream * stream;
    HTStreamClass targetClass;    

    stream = HTStreamStack(rep_in, request);
    
    if (!stream) {
        char buffer[1024];	/* @@@@@@@@ */
	sprintf(buffer, "Sorry, can't convert from %s to %s.",
		HTAtom_name(rep_in), HTAtom_name(request->output_format));
	if (TRACE) fprintf(stderr, "HTFormat: %s\n", buffer);
        return HTLoadError(request->output_stream, 501, buffer);
    }
    
/*	Push the data, ignoring CRLF if necessary, down the stream
**
**
**   @@  Bug:  This decision ought to be made based on "encoding"
**   rather than on format.  @@@  When we handle encoding.
**   The current method smells anyway.
*/
    targetClass = *(stream->isa);	/* Copy pointers to procedures */
    if (rep_in == WWW_BINARY || HTOutputSource
        || strstr(HTAtom_name(rep_in), "image/")
	|| strstr(HTAtom_name(rep_in), "video/")) { /* @@@@@@ */
        HTCopy(file_number, stream);
    } else {   /* ascii text with CRLFs :-( */
        HTCopyNoCR(file_number, stream);
    }
    (*targetClass.free)(stream);
    
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

    stream = HTStreamStack(rep_in, request);
    
    if (!stream) {
        char buffer[1024];	/* @@@@@@@@ */
	sprintf(buffer, "Sorry, can't convert from %s to %s.",
		HTAtom_name(rep_in), HTAtom_name(request->output_format));
	if (TRACE) fprintf(stderr, "HTFormat(in HTParseFile): %s\n", buffer);
        return HTLoadError(request->output_stream, 501, buffer);
    }
    
/*	Push the data down the stream
**
**
**   @@  Bug:  This decision ought to be made based on "encoding"
**   rather than on content-type.  @@@  When we handle encoding.
**   The current method smells anyway.
*/
    targetClass = *(stream->isa);	/* Copy pointers to procedures */
    HTFileCopy(fp, stream);
    (*targetClass.free)(stream);
    
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
    me->sink->isa->free(me->sink);		/* Close rest of pipe */
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


