/*								      HTMLGen.c
**	HTML GENERATOR
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This version of the HTML object sends HTML markup to the output stream.
**
** Bugs:	Line wrapping is not done at all.
**		All data handled as PCDATA.
**		Should convert old XMP, LISTING and PLAINTEXT to PRE.
**
**	It is not obvious to me right now whether the HEAD should be generated
**	from the incomming data or the anchor.  Currently it is from the former
**	which is cleanest. TBL
**
** HISTORY:
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTMLPDTD.h"
#include "HTStruct.h"
#include "HTFormat.h"
#include "HTMLGen.h"				         /* Implemented here */

#define BUFFER_SIZE	80	/* Line buffer attempts to make neat breaks */
#define MAX_CLEANNESS	10

#define PUT_CHAR(c)	(*me->target->isa->put_character)(me->target, c)
#define PUT_STR(s)	(*me->target->isa->put_string)(me->target, s)
#define PUT_BLOCK(s,l)	(*me->target->isa->put_block)(me->target, s, l)

/* HTML Generator Object */
struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *			target;
};

struct _HTStructured {
    const HTStructuredClass *	isa;
    HTStream * 			target;
    const SGML_dtd *		dtd;
    BOOL			seven_bit;		  /* restrict output */
	
    char			buffer[BUFFER_SIZE+1];
    char *			write_pointer;
    char *			line_break [MAX_CLEANNESS+1];
    int				cleanness;
    BOOL			overflowed;
    BOOL			delete_line_break_char[MAX_CLEANNESS+1];
    char			preformatted;
};

/*			OUTPUT FUNCTIONS
**
**	These function output the finished SGML stream doing the
**	line wrap
*/

/*	Flush Buffer
**	------------
*/

PRIVATE void flush_breaks (HTStructured * me)
{
    int i;
    for (i=0; i<= MAX_CLEANNESS; i++) {
        me->line_break[i] = NULL;
    }
}


PRIVATE int HTMLGen_flush (HTStructured * me)
{
    PUT_BLOCK(me->buffer, me->write_pointer - me->buffer);
    me->write_pointer = me->buffer;
    flush_breaks(me);
    me->cleanness = 0;
    return HT_OK;
}


/*	Weighted optional line break
**
**	We keep track of all the breaks for when we chop the line
*/

PRIVATE void allow_break (HTStructured * me, int new_cleanness, BOOL dlbc)
{
    me->line_break[new_cleanness] = 
			 dlbc ? me->write_pointer - 1 /* Point to space */
			      : me->write_pointer ;   /* point to gap */
    me->delete_line_break_char[new_cleanness] = dlbc;
    if (new_cleanness >= me->cleanness)
	me->cleanness = new_cleanness;
}


/*	Character handling
**	------------------
**
**	The tricky bits are the line break handling.  This attempts
**	to synchrononise line breaks on sentence or phrase ends. This
**	is important if one stores SGML files in a line-oriented code
**	repository, so that if a small change is made, line ends don't
**	shift in a ripple-through to apparently change a large part of the
**	file. We give extra "cleanness" to spaces appearing directly
**	after periods (full stops), [semi]colons and commas.
**	   This should make the source files easier to read and modify
**	by hand, too, though this is not a primary design consideration. TBL
*/
PRIVATE char delims[] = ",;:.";		/* @@ english bias */
PRIVATE int HTMLGen_output_character (HTStructured * me, char c)
{

    *me->write_pointer++ = c;
    
    if (c=='\n') {		/* Newlines */
        if (me->preformatted) {
	    HTMLGen_flush(me);
	    return HT_OK;
	} else {
	    me->write_pointer[-1] = c = ' ';	/* Treat same as space */
	}
    }
    
    /* Figure our whether we can break at this point
    */
    if ((!me->preformatted  && c==' ')) {
        int new_cleanness = 1;
	if (me->write_pointer > (me->buffer + 1)) {
	    char * p;
	    p = strchr(delims, me->write_pointer[-2]);
	    if (p) new_cleanness = p - delims + 4;
	}
	allow_break(me, new_cleanness, YES);
    }
    
    /* Flush buffer out when full, or whenever the line is over
       the nominal maximum and we can break at all
    */
    if (me->write_pointer >= me->buffer + BUFFER_SIZE-1
        ||  (me->overflowed && me->cleanness)) {
    	if (me->cleanness) {
	    char line_break_char = me->line_break[me->cleanness][0];
	    char * saved = me->line_break[me->cleanness];
	    
	    if (me->delete_line_break_char[me->cleanness]) saved++; 
	    me->line_break[me->cleanness][0] = '\n';
	    PUT_BLOCK(me->buffer, me->line_break[me->cleanness]-me->buffer+1);
	    me->line_break[me->cleanness][0] = line_break_char;
	    {  /* move next line in */
	    	char * p=saved;
		char *q;
		for(q=me->buffer; p < me->write_pointer; )
			*q++ = *p++;
	    }
	    me->cleanness = 0;
	    /* Now we have to check whether ther are any perfectly good breaks
	    ** which weren't good enough for the last line but may be
	    **  good enough for the next
	    */
	    {
	        int i;
		for(i=0; i <= MAX_CLEANNESS; i++) {
		    if (me->line_break[i] > saved) {
		        me->line_break[i] = me->line_break[i] -
						(saved-me->buffer);
			me->cleanness = i;
		    } else {
		        me->line_break[i] = NULL;
		    }
		}
	    }

	    me->write_pointer = me->write_pointer - (saved-me->buffer);
	    me->overflowed = NO;
	} else {   /* No break- just output with no newline */
	    PUT_BLOCK(me->buffer, me->write_pointer - me->buffer);
	    me->write_pointer = me->buffer;
	    flush_breaks(me);
	    me->overflowed = YES;
	}
    }
    return HT_OK;
}


/*	String handling
**	---------------
*/
PRIVATE int HTMLGen_output_string (HTStructured * me, const char* s)
{
    while (*s)
	HTMLGen_output_character(me, *s++);
    return HT_OK;
}


/*			INPUT FUNCTIONS
**
**	These take data from the structured stream.  In the input
**	stream, entities are in raw form.  The seven_bit flag controls
**	whether the ISO Latin-1 charactrs are represented in SGML entity
**	form.  This is only recommended for viewing on older non-latin-1
**	capable equipment, or for mailing for example. 
**
** Bug: assumes local encoding is ISO!
*/	
PRIVATE int HTMLGen_put_character (HTStructured * me, char c)
{
    if (c=='&') HTMLGen_output_string(me, "&amp;");
    else if (c=='<') HTMLGen_output_string(me, "&lt;");
    else if (me->seven_bit && ((unsigned char)c > 127)) {
        char temp[8];
	sprintf(temp, "&%d;", c);
	HTMLGen_output_string(me, temp);
    } else
	HTMLGen_output_character(me, c);
    return HT_OK;
}

PRIVATE int HTMLGen_put_string (HTStructured * me, const char* s)
{
    while (*s)
	HTMLGen_put_character(me, *s++);
    return HT_OK;
}

PRIVATE int HTMLGen_write (HTStructured * me, const char* b, int l)
{
    while (l-- > 0)
	HTMLGen_put_character(me, *b++);
    return HT_OK;
}


/*	Start Element
**	-------------
**
**	Within the opening tag, there may be spaces
**	and the line may be broken at these spaces.
*/
PRIVATE void HTMLGen_start_element (
	HTStructured * 	me,
	int			element_number,
	const BOOL*	 	present,
	const char **		value)
{
    int i;
    HTTag * tag = &me->dtd->tags[element_number];

    /* Control line breaks allowed within tag! */
    int was_preformatted = me->preformatted;	/* save state */
    me->preformatted = 1;	/* Can break between attributes */

    HTMLGen_output_character(me, '<');
    HTMLGen_output_string(me, tag->name);
    if (present) for (i=0; i< tag->number_of_attributes; i++) {
        if (present[i]) {
	    HTMLGen_output_character(me, ' ');
	    allow_break(me, 1, YES);
	    HTMLGen_output_string(me, tag->attributes[i].name);
	    if (value[i]) {
	 	HTMLGen_output_string(me, "=\"");
		HTMLGen_output_string(me, value[i]);
		HTMLGen_output_character(me, '"');
	    }
	}
    }
    me->preformatted = was_preformatted;	/* Restore state */

    /* Nested PRE is no more a problem! */
    if (element_number == HTML_PRE)
	me->preformatted++;

    HTMLGen_output_character(me, '>');
    
    /* Here is a funny one.  In PRE, newlines are significant, except of
    course for one after the <PRE> which is ignored. This means that
    we MUST put in a dummy one after the <PRE> to protect any real newline
    within the pre section.
    
    However, *within* a PRE section, although we can break after
    (for example) emphasis start tags, it will probably confuse some
    parsers so we won't.*/
    
    if (element_number == HTML_PRE) {
        HTMLGen_output_character(me, '\n');
    } else  if (!me->preformatted && 
    	 tag->contents != SGML_EMPTY) {  /* can break after element start */ 
    	allow_break(me, 3, NO);
    }
}


/*	End Element
**	-----------
**
**      The rules for insertring CR LF into SGML are weird, strict, and
** 	nonintitive.
**	See comment also about PRE above.
*/
PRIVATE void HTMLGen_end_element (HTStructured * me, int element_number)
{
    if (element_number == HTML_PRE) {
        HTMLGen_output_character(me, '\n');
    } else  if (!me->preformatted) { /* can break before element end */ 
    	allow_break(me, 1, NO);
    }
    HTMLGen_output_string(me, "</");
    HTMLGen_output_string(me, me->dtd->tags[element_number].name);
    HTMLGen_output_character(me, '>');	   /* NO break after. TBL 940501 */
    if (element_number == HTML_PRE && me->preformatted)
	me->preformatted--;
}


/*	Expanding entities
**	------------------
**
*/

PRIVATE void HTMLGen_put_entity (HTStructured * me, int entity_number)
{
    HTMLGen_output_character(me, '&');
    HTMLGen_output_string(me, me->dtd->entity_names[entity_number]);
    HTMLGen_output_character(me, ';');
}

/*	Free an object
**	--------------
**
*/
PRIVATE int HTMLGen_free (HTStructured * me)
{
    HTMLGen_flush(me);
    PUT_CHAR('\n');
    (*me->target->isa->_free)(me->target);
    HT_FREE(me);
    return HT_OK;
}


PRIVATE int PlainToHTML_free (HTStructured * me)
{
    HTMLGen_end_element(me, HTML_PRE);
    HTMLGen_end_element(me, HTML_BODY);
    HTMLGen_end_element(me, HTML_HTML);
    HTMLGen_free(me);
    return HT_OK;
}



PRIVATE int HTMLGen_abort (HTStructured * me, HTList * e)
{
    HTMLGen_free(me);
    return HT_ERROR;
}


PRIVATE int PlainToHTML_abort (HTStructured * me, HTList * e)
{
    PlainToHTML_free(me);
    return HT_ERROR;
}



/*	Structured Object Class
**	-----------------------
*/
PRIVATE const HTStructuredClass HTMLGeneration = /* As opposed to print etc */
{		
	"text/html",
	HTMLGen_flush,
	HTMLGen_free,
	HTMLGen_abort,
	HTMLGen_put_character, 	HTMLGen_put_string, HTMLGen_write,
	HTMLGen_start_element,	HTMLGen_end_element,
	HTMLGen_put_entity
}; 


/*	Subclass-specific Methods
**	-------------------------
*/
PUBLIC HTStructured* HTMLGenerator (HTRequest *	request,
				    void *	param,
				    HTFormat	input_format,
				    HTFormat	output_format,
				    HTStream *	output_stream)
{
    HTStructured* me;
    if ((me = (HTStructured  *) HT_CALLOC(1, sizeof(HTStructured))) == NULL)
        HT_OUTOFMEM("HTMLGenerator");
    me->isa = &HTMLGeneration;       
    me->dtd = HTML_dtd();
    if ((me->target = HTStreamStack(WWW_HTML, output_format, output_stream,
				    request, YES)) == NULL) {
	HTTRACE(STREAM_TRACE, "HTMLGen..... Can't convert to media type\n");
	HT_FREE(me);
	me->target = HTErrorStream();
    }
    me->write_pointer = me->buffer;
    flush_breaks(me);
    return me;
}

/*	Stream Object Class
**	-------------------
**
**	This object just converts a plain text stream into HTML
**	It is officially a structured stream but only the stream bits exist.
**	This is just the easiest way of typecasting all the routines.
*/
PRIVATE const HTStructuredClass PlainToHTMLConversion =
{		
	"plaintexttoHTML",
	HTMLGen_flush,
	PlainToHTML_free,	/* HTMLGen_free,  Henrik 03/03-94 */
	PlainToHTML_abort,	
	HTMLGen_put_character,
	HTMLGen_put_string,
	HTMLGen_write,
	NULL,		/* Structured stuff */
	NULL,
	NULL
}; 


/*	HTConverter from plain text to HTML Stream
**	------------------------------------------
**
** Changed by henrik 03/03-94, so no more core dumps etc. (I hope!!!)
*/

PUBLIC HTStream* HTPlainToHTML (HTRequest *	request,
				void *		param,
				HTFormat	input_format,
				HTFormat	output_format,
				HTStream *	output_stream)
{
    BOOL present[MAX_ATTRIBUTES];	/* Flags: attribute is present? */
    const char *value[MAX_ATTRIBUTES];	/* malloc'd strings or NULL if none */
    HTStructured* me;
    if ((me = (HTStructured *) HT_CALLOC(1,sizeof(*me))) == NULL)
        HT_OUTOFMEM("PlainToHTML");
    
    memset((void *) present, '\0', MAX_ATTRIBUTES);
    memset((void *) value, '\0', MAX_ATTRIBUTES*sizeof(char *));
    
    me->isa = (HTStructuredClass*) &PlainToHTMLConversion;
    me->dtd = HTML_dtd();
    me->target = output_stream;
    me->write_pointer = me->buffer;
    flush_breaks(me);
    
    if (me->target) {
	HTMLGen_start_element(me, HTML_HTML, present, value);
	HTMLGen_start_element(me, HTML_BODY, present, value);
	HTMLGen_start_element(me, HTML_PRE, present, value);
    }
    return (HTStream*) me;
}


/* 	A safe version for making 7-bit restricted HTML
**	Beware that thsi makes it horrible for the Scandinavians
**	to actually read it.
**	ehh - not horrible - THIS REALLY PISSES THEM OFF - Henrik ;-)
*/

PUBLIC HTStream* HTPlainTo7BitHTML (HTRequest *	request,
				    void *	param,
				    HTFormat	input_format,
				    HTFormat	output_format,
				    HTStream *	output_stream)
{
    HTStream* me = HTPlainToHTML(request,param,input_format,
    		output_format, output_stream);
    ((HTStructured*)me)->seven_bit = YES;
    return me;
}

