/*		Manage different file formats			HTFormat.c
**		=============================
**
** Bugs:
**	Not reentrant.
**
**	Assumes the incoming stream is ASCII, rather than a local file
**	format, and so ALWAYS converts from ASCII on non-ASCII machines.
**	Therefore, non-ASCII machines can't read local files.
*/

#include "HTUtils.h"
#include "tcp.h"
#include "HTFormat.h"

#include "HTML.h"
#include "HText.h"
#include "HTStyle.h"

extern HTStyleSheet * styleSheet;


PUBLIC	BOOL HTOutputSource = NO;	/* Flag: shortcut parser to stdout */

/*	File buffering
**	--------------
**
**	The input file is read using the macro which can read from
**	a socket or a file.
**	The input buffer size, if large will give greater efficiency and
**	release the server faster, and if small will save space on PCs etc.
*/
#define INPUT_BUFFER_SIZE 4096		/* Tradeoff */
PRIVATE char input_buffer[INPUT_BUFFER_SIZE];
PRIVATE char * input_pointer;
PRIVATE char * input_limit;
PRIVATE int input_file_number;


/*	Set up the buffering
**
**	These routines are public because they are in fact needed by
**	many parsers, and on PCs and Macs we should not duplicate
**	the static buffer area.
*/
PUBLIC void HTInitInput ARGS1 (int,file_number)
{
    input_file_number = file_number;
    input_pointer = input_limit = input_buffer;
}


PUBLIC char HTGetChararcter NOARGS
{
    char ch;
    do {
	if (input_pointer >= input_limit) {
	    int status = NETREAD(
		    input_file_number, input_buffer, INPUT_BUFFER_SIZE);
	    if (status <= 0) {
		if (status == 0) return (char)EOF;
		if (TRACE) fprintf(stderr,
		    "HTFormat: File read error %d\n", status);
		return (char)EOF; /* -1 is returned by UCX at end of HTTP link */
	    }
	    input_pointer = input_buffer;
	    input_limit = input_buffer + status;
	}
	ch = *input_pointer++;
    } while (ch == (char) 13); /* Ignore ASCII carriage return */
    
    return FROMASCII(ch);
}

/*	Stream the data to an ouput file as binary
*/
PUBLIC int HTOutputBinary ARGS2( int, 		input,
				  FILE *, 	output)
{
    do {
	    int status = NETREAD(
		    input, input_buffer, INPUT_BUFFER_SIZE);
	    if (status <= 0) {
		if (status == 0) return 0;
		if (TRACE) fprintf(stderr,
		    "HTFormat: File read error %d\n", status);
		return 2;			/* Error */
	    }
	    fwrite(input_buffer, sizeof(char), status, output);
    } while (YES);
}



/*	Parse a file given format and file number
**	------------
**
**   This routine is responsible for ceating and presenting any
**   graphic (or other) objects described by the file.
*/
PUBLIC void HTParseFormat ARGS3(
	HTFormat,format,
	HTParentAnchor *,anchor,
	int,file_number)
{

/*	Parse the file
*/
    if (HTOutputSource) {	/* Shortcut dump binary from the net */
	exit(HTOutputBinary(file_number, stdout));  /* @@@cheat */
    }
    
#ifdef CURSES
     long    bytecount = 0;
#endif
    HTInitInput(file_number);

    switch (format) {

    case WWW_HTML:		/* Parse HTML */
      {				/* Call SGML directly for speed */
	HTML_id html = HTML_new(anchor);
        HTSGMLContext context = HTML_SGMLContext(html);
	for(;;) {
	    char character;
	    character = HTGetChararcter();
	    if (character == (char)EOF) break;
#ifdef CURSES
              if (++bytecount % 1024 == 0) prompt_count(bytecount / 1024);
#endif
    
	    SGML_character(context, character);           
         }
	HTML_free(html);
      }
	break;

    default :			/* unknown format -- Parse plain text */
      {
        HText * text = HText_new(anchor);
	HText_setStyle(text, HTStyleNamed(styleSheet, "Example"));
	HText_beginAppend(text);
	for(;;) {
	    char character;
	    character = HTGetChararcter();
	    if (character == (char)EOF) break;
#ifdef CURSES
              if (++bytecount % 1024 == 0) prompt_count(bytecount / 1024);
#endif
	    HText_appendCharacter(text, character);           
	}
        HText_endAppend(text);
      }
	break;
	
    } /* end of switch (format) */
    
}
