/*								     HTTeXGen.c
**	HTML -> LaTeX CONVERTER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** 	This version of the HTML object sends LaTeX to the output stream.
** 	No attributes are considered in the translation!
** 	The module uses simple 1:1 table-conversions, but this COULD be
**	expanded to a stack-machine. This would then be in start_element and
**	end_element...
**						Henrik 07/03-94
**
** HISTORY:
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**
*/

#define BUFFER_SIZE	80	/* Line buffer attempts to make neat breaks */
#define WORD_DELIMITERS ",;:[]()"

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTTeXGen.h"
#include "HTMLPDTD.h"
#include "HTStruct.h"
#include "HTFormat.h"

/*		HTML Object
**		-----------
*/

struct _HTStream {
	const HTStreamClass *		isa;	
	HTStream * 			target;
	HTStreamClass			targetClass;	   /* COPY for speed */
};

struct _HTStructured {
	const HTStructuredClass *	isa;
	HTStream * 			target;
	HTStreamClass			targetClass;	   /* COPY for speed */
	const SGML_dtd *		dtd;
	
	char				buffer[2*BUFFER_SIZE];   /* See note */
	char *				write_pointer;
	char *				line_break;
	BOOL				sensitive;          /* Can we put \n */
	BOOL				preformatted;     /* Is it verbatim? */
	BOOL				markup;     /* If doing LaTeX markup */
	BOOL				startup;      /* To skip MIME header */
};

/* The buffer has to be bigger than 80 as latex markup might make the line
   longer before we get to flush it. */

PRIVATE char *TeX_names[HTML_ELEMENTS][2] = {
    { "",       	""		},	/* HTML_A		*/
    { "",		""		},	/* HTML_ABBR		*/
    { "",		""		},	/* HTML_ACRONYM		*/
    { "{\\it ",		"}"		},	/* HTML_ADDRESS		*/
    { "",       	""		},	/* HTML_APPLET		*/
    { "",       	""		},	/* HTML_AREA		*/
    { "{\\bf ",		"}"		},	/* HTML_B		*/
    { "",		""		},	/* HTML_BASE		*/
    { "",       	""		},	/* HTML_BASEFONT	*/
    { "",       	""		},	/* HTML_BDO		*/
    { "",       	""		},	/* HTML_BIG		*/
    { "{\\sf ",		"}"		},	/* HTML_BLOCKQUOTE 	*/
    { "", 		""		},	/* HTML_BODY     	*/
    { "",		""		},	/* HTML_BR		*/
    { "",       	""		},	/* HTML_BUTTON		*/
    { "",		""		},	/* HTML_CAPTION		*/
    { "",		""		},	/* HTML_CENTER		*/
    { "\\cite{",      	"}"		},	/* HTML_CITE		*/
    { "{\\tt ",		"}"		},	/* HTML_CODE		*/
    { "",		""		},	/* HTML_COL		*/
    { "",		""		},	/* HTML_COLGROUP	*/
    { "]",		""		},	/* HTML_DD		*/
    { "",		""		},	/* HTML_DEL		*/
    { "",		""		},	/* HTML_DFN		*/
    { "",		""		},	/* HTML_DIR		*/
    { "",		""		},	/* HTML_DIV		*/
    { "\n\\begin{description}","\n\\end{description}\n"}, /* HTML_DL 	*/
    { "\n\\item[",	""		},	/* HTML_DT		*/
    { "{\\em ",		"}"		},	/* HTML_EM		*/
    { "",		""		},	/* HTML_FIELDSET	*/
    { "",		""		},	/* HTML_FONT		*/
    { "",	"" 			},	/* HTML_FORM		*/
    { "",       	""		},	/* HTML_FRAME		*/
    { "",       	""		},	/* HTML_FRAMESET	*/
    { "\n\\chapter{",	"}\n"		},	/* HTML_H1		*/
    { "\n\\section{",	"}\n"		},	/* HTML_H2		*/
    { "\n\\subsection{","}\n"  		},	/* HTML_H3		*/
    { "\n\\subsubsection{","}\n"	},	/* HTML_H4		*/
    { "\n\\paragraph{",	"}\n"		},	/* HTML_H5		*/
    { "\n\\subparagraph{","}\n"		},	/* HTML_H6		*/
    { "",		""		},	/* HTML_HEAD		*/
    { "",		""		},	/* HTML_HR		*/
    { "",		""		}, 	/* HTML_HTML		*/
    { "{\\it ",		"}"		},	/* HTML_I		*/
    { "",		""		},	/* HTML_IFRAME		*/
    { "_FIGUR_",	""		},	/* HTML_IMG		*/
    { "",		""		},	/* HTML_INPUT		*/
    { "",		""		},	/* HTML_INS		*/
    { "",		""		},	/* HTML_ISINDEX		*/
    { "{\\tt ",		"}"		},	/* HTML_KBD		*/
    { "",		""		},	/* HTML_LABEL		*/
    { "",		""		},	/* HTML_LEGEND		*/
    { "\n\\item ",      "" 	        },	/* HTML_LI		*/
    { "",		""		},	/* HTML_LINK		*/
    { "",		""		},	/* HTML_MAP		*/
    { "",		""		},	/* HTML_MENU		*/
    { "",		""		},	/* HTML_META		*/
    { "",		""		},	/* HTML_NEXTID		*/
    { "",		""		},	/* HTML_NOFRAME		*/
    { "",		""		},	/* HTML_NOSCRIPT	*/
    { "",       	""		},	/* HTML_OBJECT		*/
    { "\n\\begin{enumerate}\n","\n\\end{enumerate}\n"}, /* HTML_OL     	*/
    { "",		""		},	/* HTML_OPTGROUP	*/
    { "",		""		},	/* HTML_OPTION		*/
    { "\n\n",		""		},	/* HTML_P		*/
    { "",		""		},	/* HTML_PARAM		*/
    { "\n\\begin{verbatim}"," \\end{verbatim}\n"},  /* HTML_PRE        */
    { "",		""		},	/* HTML_Q		*/
    { "",		""		},	/* HTML_S		*/
    { "",		""		},	/* HTML_SAMP		*/
    { "",		""		},	/* HTML_SCRIPT		*/
    { "", 		""		},	/* HTML_SELECT		*/
    { "",		""		},	/* HTML_SMALL		*/
    { "",		""		},	/* HTML_SPAN		*/
    { "",		""		},	/* HTML_STRIKE		*/
    { "{\\bf ",		"}"		},	/* HTML_STRONG		*/
    { "", 		""		},	/* HTML_STYLE		*/
    { "", 		""		},	/* HTML_SUB		*/
    { "",		""		},	/* HTML_SUP		*/
    { "",		""		},	/* HTML_TABLE		*/
    { "",		""		},	/* HTML_TBODY		*/
    { "",		""		},	/* HTML_TD		*/
    { "",		""		},	/* HTML_TEXTAREA	*/
    { "",		""		},	/* HTML_TFOOT		*/
    { "",		""		},	/* HTML_TH		*/
    { "",		""		},	/* HTML_THEAD		*/
    { "\n\\title{",	"}\n\\author{}\n\\maketitle\n"},  /* HTML_TITLE */
    { "",		""		},	/* HTML_TR		*/
    { "",		""		},	/* HTML_TT		*/
    { "",		""		},	/* HTML_U		*/
    { "\n\\begin{itemize}","\n\\end{itemize}\n"},   /* HTML_UL		*/
    { "",		""		}	/* HTML_VAR		*/
};

PRIVATE char *TeX_entities[HTML_ENTITIES] = {
    "\\AE ",		/*"AElig",	 capital AE diphthong (ligature) */ 
    "\\\'{A}",		/*"Aacute",	 capital A, acute accent */ 
    "\\^{A}",		/*"Acirc",	 capital A, circumflex accent */ 
    "\\`{A}",		/*"Agrave",	 capital A, grave accent */ 
    "\\AA",   	 	/*"Aring",	 capital A, ring */ 
    "\\~{A}", 		/*"Atilde",	 capital A, tilde */ 
    "\\\"{A}",		/*"Auml",	 capital A, dieresis or umlaut mark */ 
    "\\c{C}",		/*"Ccedil",	 capital C, cedilla */ 
    "\\OE ",		/*"ETH",	 capital Eth, Icelandic */ 
    "\\\'{E}",		/*"Eacute",	 capital E, acute accent */ 
    "\\^{E}",		/*"Ecirc",	 capital E, circumflex accent */ 
    "\\`{E}",		/*"Egrave",	 capital E, grave accent */ 
    "\\\"{E}",		/*"Euml",	 capital E, dieresis or umlaut mark */ 
    "\\\'{I}",		/*"Iacute",	 capital I, acute accent */ 
    "\\^{I}",		/*"Icirc",	 capital I, circumflex accent */ 
    "\\`{I}",		/*"Igrave",	 capital I, grave accent */ 
    "\\\"{I}",		/*"Iuml",	 capital I, dieresis or umlaut mark */ 
    "\\~{N}",		/*"Ntilde",	 capital N, tilde */ 
    "\\\'{O}",		/*"Oacute",	 capital O, acute accent */ 
    "\\^{O}",		/*"Ocirc",	 capital O, circumflex accent */ 
    "\\`{O}",		/*"Ograve",	 capital O, grave accent */ 
    "\\O ",            	/*"Oslash",	 capital O, slash */ 
    "\\~{O}",		/*"Otilde",	 capital O, tilde */ 
    "\\\"{O}",        	/*"Ouml",	 capital O, dieresis or umlaut mark */ 
    " ",      	/*"THORN",	 capital THORN, Icelandic */ 
    "\\\'{U}",		/*"Uacute",	 capital U, acute accent */ 
    "\\^{U}",		/*"Ucirc",	 capital U, circumflex accent */ 
    "\\`{U}",		/*"Ugrave",	 capital U, grave accent */ 
    "\\\"{U}",		/*"Uuml",	 capital U, dieresis or umlaut mark */ 
    "\\\'{Y}",		/*"Yacute",	 capital Y, acute accent */ 
    "\\\'{a}",		/*"aacute",	 small a, acute accent */ 
    "\\^{a}",		/*"acirc",	 small a, circumflex accent */ 
    "\\ae ",		/*"aelig",	 small ae diphthong (ligature) */ 
    "\\`{a}",		/*"agrave",	 small a, grave accent */ 
    "&",            	/*"amp",	 ampersand */ 
    "\\aa ",		/*"aring",	 small a, ring */ 
    "\\~{a}",		/*"atilde",	 small a, tilde */ 
    "\\\"{a}",		/*"auml",	 small a, dieresis or umlaut mark */ 
    "\\c{c}",		/*"ccedil",	 small c, cedilla */ 
    "\\\'{e}",		/*"eacute",	 small e, acute accent */ 
    "\\^{c}",		/*"ecirc",	 small e, circumflex accent */ 
    "\\`{c}",		/*"egrave",	 small e, grave accent */ 
    "\\oe ",		/*"eth",	 small eth, Icelandic */ 
    "\\\"{e}",		/*"euml",	 small e, dieresis or umlaut mark */ 
    ">",		/*"gt",	 greater than */ 
    "\\\'{\\i}",      	/*"iacute",	 small i, acute accent */ 
    "\\^{\\i}",       	/*"icirc",	 small i, circumflex accent */ 
    "\\`{\\i}",       	/*"igrave",	 small i, grave accent */ 
    "\\\"{\\i}",     	/*"iuml",	 small i, dieresis or umlaut mark */ 
    "<",		/*"lt",	 less than */ 
    "\\~{n}",		/*"ntilde",	 small n, tilde */ 
    "\\\'{o}",		/*"oacute",	 small o, acute accent */ 
    "\\~{o}",		/*"ocirc",	 small o, circumflex accent */ 
    "\\`{o}",		/*"ograve",	 small o, grave accent */ 
    "\\o ",            	/*"oslash",	 small o, slash */ 
    "\\~{o}",         	/*"otilde",	 small o, tilde */ 
    "\\\"{o}",	   	/*"ouml",	 small o, dieresis or umlaut mark */ 
    "\"",	   	/*"quot",	 double quote sign - June 1994 */ 
    "\\ss ",		/*"szlig",	 small sharp s, German (sz ligature)*/ 
    " ",       	/*"thorn",	 small thorn, Icelandic */ 
    "\\\'{u}",        	/*"uacute",	 small u, acute accent */ 
    "\\^{u}",        	/*"ucirc",	 small u, circumflex accent */ 
    "\\`{u}",         	/*"ugrave",	 small u, grave accent */ 
    "\\\"{u}",        	/*"uuml",	 small u, dieresis or umlaut mark */ 
    "\\\'{y}",		/*"yacute",	 small y, acute accent */ 
    "\\\"{y}"	    	/*"yuml",	 small y, dieresis or umlaut mark */ 
};


/*	Flush Buffer
**	------------
*/
PRIVATE int HTTeXGen_flush (HTStructured * me)
{
    int status;
    if ((status =
	 (*me->targetClass.put_block)(me->target, me->buffer,
				      me->write_pointer-me->buffer)) != HT_OK)
	return status;
    me->write_pointer = me->buffer;
    me->line_break = me->buffer;
    return (*me->targetClass.flush)(me->target);
}


/*	Character handling
**	------------------
**
*/
PRIVATE int HTTeXGen_put_character (HTStructured * me, char c)
{
    if (!me->startup)		                      /* To skip MIME header */
	return HT_OK;
    if (c=='\n') {
	if (me->markup || me->preformatted) {     /* Put out as is and flush */
	    *me->write_pointer++ = c;
	    HTTeXGen_flush(me);
	    return HT_OK;
	} else if (me->sensitive || *(me->write_pointer-1)==' ') {
	    return HT_OK;
        } else
	    *me->write_pointer++ = ' ';		      /* Try to pretty print */
    } else if (me->markup || me->preformatted) {
	*me->write_pointer++ = c;
    } else if (c==' ' || c=='\t') {	              /* Skip space and tabs */
	if (*(me->write_pointer-1) != ' ')
	    *me->write_pointer++ = ' ';
	else
	    return HT_OK;
    } else {
	if (c=='$' || c=='&' || c=='%' || c=='#' ||         /* Special chars */
	    c=='{' || c=='}' || c=='_') {
	    *me->write_pointer++ = '\\';
	    *me->write_pointer++ = c;
	} else if (c=='\\') {			            /* Special names */
	    char *temp = "$\\backslash$";
	    strcpy(me->write_pointer, temp);
	    me->write_pointer += strlen(temp);
	} else if (c=='^') {
	    char *temp = "$\\hat{ }$";
	    strcpy(me->write_pointer, temp);
	    me->write_pointer += strlen(temp);	    
	} else if (c=='~') {
	    char *temp = "$\\tilde{ }$";
	    strcpy(me->write_pointer, temp);
	    me->write_pointer += strlen(temp);
	} else if (c=='|' || c=='<' || c=='>') {       	        /* Math mode */
	    *me->write_pointer++ = '$';
	    *me->write_pointer++ = c;
	    *me->write_pointer++ = '$';
	} else
	    *me->write_pointer++ = c;	      	        /* Char seems normal */
    }

    if (c==' ')						   /* Find delimiter */
	me->line_break = me->write_pointer;
    else if (strchr(WORD_DELIMITERS, c))
	me->line_break = me->write_pointer-1;

    /* Flush buffer out when full */
    if (me->write_pointer >= me->buffer+BUFFER_SIZE-3) {
#ifdef OLD_CODE
	if (me->markup || me->preformatted) {
#endif /* OLD_CODE */
	if (me->preformatted) {
	    *me->write_pointer = '\n';
	    (*me->targetClass.put_block)(me->target,
					 me->buffer,
					 me->write_pointer-me->buffer+1);
	    me->write_pointer = me->buffer;
	} else {		       		          /* Use break-point */
	    char line_break_char = *me->line_break;
	    char *saved = me->line_break;
	    *me->line_break = '\n';
	    (*me->targetClass.put_block)(me->target,
					 me->buffer,
					 me->line_break-me->buffer+1);
	    *me->line_break = line_break_char;
	    {                                           /* move next line in */
		char *p = saved;
		char *q;
		for(q=me->buffer; p<me->write_pointer; )
		    *q++ = *p++;
	    }
	    me->write_pointer = me->buffer + (me->write_pointer-saved);
	}	    
	me->line_break = me->buffer;
    }
    return HT_OK;
}



/*	String handling
**	---------------
*/
PRIVATE int HTTeXGen_put_string (HTStructured * me, const char* s)
{
    while (*s)
	HTTeXGen_put_character(me, *s++);
    return HT_OK;
}


PRIVATE int HTTeXGen_write (HTStructured * me, const char* b, int l)
{
    while (l-- > 0)
	HTTeXGen_put_character(me, *b++);
    return HT_OK;
}


/*	Start Element
**	-------------
**
**     	No attributes are put to the output		Henrik 07/03-94
**	Does no assumptions of WHAT element is started...
*/
PRIVATE void HTTeXGen_start_element (HTStructured * 	me,
				     int		element_number,
				     const BOOL *	present,
				     const char **	value)
{
    me->startup = YES;			        /* Now, let's get down to it */
    if (me->preformatted == YES) {	       /* Don't start markup in here */
	HTTRACE(SGML_TRACE, "LaTeX....... No Markup in verbatim mode\n");
	return;
    }
    if (element_number == HTML_PRE)
	me->preformatted = YES;
    if (element_number == HTML_CITE ||	              /* No \n here, please! */
	element_number == HTML_DT ||
	element_number == HTML_H1 ||
	element_number == HTML_H2 ||
	element_number == HTML_H3 ||
	element_number == HTML_H4 ||
	element_number == HTML_H5 ||
	element_number == HTML_H6 ||
	element_number == HTML_TITLE)
	me->sensitive = YES;
    else if (element_number == HTML_DD)         /* Only way to turn <DT> off */
	me->sensitive = NO;
    me->markup = element_number == HTML_A ? NO : YES;
    HTTeXGen_put_string(me, *TeX_names[element_number]);
    me->markup = NO;
}


/*		End Element
**		-----------
**
**	Ends an markup element			Henrik 07/03-94
**	Does no assumptions of WHAT element is ended...
*/
PRIVATE void HTTeXGen_end_element (HTStructured * me, int element_number)
{
    if (me->preformatted && element_number != HTML_PRE) {
	HTTRACE(SGML_TRACE, "LaTeX....... No markup in verbatim mode\n");
	return;
    }
    me->preformatted = NO;
    me->markup = YES;
    HTTeXGen_put_string(me, *(TeX_names[element_number]+1));
    me->markup = NO;
    if (element_number == HTML_CITE ||	
	element_number == HTML_DL ||
	element_number == HTML_H1 ||
	element_number == HTML_H2 ||
	element_number == HTML_H3 ||
	element_number == HTML_H4 ||
	element_number == HTML_H5 ||
	element_number == HTML_H6 ||
	element_number == HTML_TITLE)
	me->sensitive = NO;
}


/*		Expanding entities
**		------------------
**
*/
PRIVATE void HTTeXGen_put_entity (HTStructured * me, int entity_number)
{
    BOOL mark = me->markup;
    if (*TeX_entities[entity_number] != '&' && /* Theese are converted later */
	*TeX_entities[entity_number] != '<' &&
	*TeX_entities[entity_number] != '>')
	me->markup = YES;
    HTTeXGen_put_string(me, TeX_entities[entity_number]);
    me->markup = mark;
}

PRIVATE int HTTeXGen_unparsedBeginElement (HTStructured * me, const char * b, int l)
{
    return (*me->targetClass.put_block)(me->target, b, l);
}

PRIVATE int HTTeXGen_unparsedEndElement (HTStructured * me, const char * b, int l)
{
    return (*me->targetClass.put_block)(me->target, b, l);
}

PRIVATE int HTTeXGen_unparsedEntity (HTStructured * me, const char * b, int l)
{
    return (*me->targetClass.put_block)(me->target, b, l);
}

/*	Free an HTML object
**	-------------------
**
*/
PRIVATE int HTTeXGen_free (HTStructured * me)
{
    HTTeXGen_flush(me);
    (*me->targetClass.put_string)(me->target, "\n\\end{document}\n");
    HTTeXGen_flush(me);
    (*me->targetClass._free)(me->target);	/* ripple through */
    HT_FREE(me);
    return HT_OK;
}


PRIVATE int HTTeXGen_abort (HTStructured * me, HTList * e)
{
    HTTeXGen_free(me);
    return HT_ERROR;
}


/*	Structured Object Class
**	-----------------------
*/
PRIVATE const HTStructuredClass HTTeXGeneration = /* As opposed to print etc */
{		
	"HTMLToTeX",
	HTTeXGen_flush,
	HTTeXGen_free,
	HTTeXGen_abort,
	HTTeXGen_put_character,        	HTTeXGen_put_string,	HTTeXGen_write,
	HTTeXGen_start_element, 	HTTeXGen_end_element,
	HTTeXGen_put_entity,
	HTTeXGen_unparsedBeginElement,
	HTTeXGen_unparsedEndElement,
	HTTeXGen_unparsedEntity
}; 


/*	HTConverter from HTML to TeX Stream
**	------------------------------------------
**
*/
PUBLIC HTStream* HTMLToTeX (HTRequest *	request,
			    void *	param,
			    HTFormat	input_format,
			    HTFormat	output_format,
			    HTStream *	output_stream)
{
    HTStructured* me;
    if ((me = (HTStructured *) HT_CALLOC(1, sizeof(*me))) == NULL)
        HT_OUTOFMEM("HTMLToTeX");

    me->isa = (HTStructuredClass*) &HTTeXGeneration;
    me->dtd = HTML_dtd();
    me->target = output_stream;
    me->targetClass = *me->target->isa;/* Copy pointers to routines for speed*/
    me->write_pointer = me->buffer;
    me->line_break = 	me->buffer;
    (*me->targetClass.put_string)(me->target,
        "\\documentstyle[11pt]{report}\n\\begin{document}\n");
    return SGML_new(HTML_dtd(), me);
}
