/* Simple LaTeX Generator that converts in a 1:1 manner from HTML to LaTeX
** =======================================================================
**
** 	This version of the HTML object sends LaTeX to the output stream.
** 	No attributes are considered in the translation!
** 	The module uses simple 1:1 table-conversions, but this COULD be
**	expanded to a stack-machine. This would then be in start_element and
**	end_element...
**						Henrik 07/03-94
*/

#define BUFFER_SIZE	80	/* Line buffer attempts to make neat breaks */
#define WORD_DELIMITERS ",/;:\"[]()"

/* Implements: */
#include "HTTeXGen.h"
#include <stdio.h>
#include "HTMLPDTD.h"
#include "HTStream.h"
#include "SGML.h"
#include "HTFormat.h"
#include "tcp.h"


/*		HTML Object
**		-----------
*/

struct _HTStream {
	CONST HTStreamClass *		isa;	
	HTStream * 			target;
	HTStreamClass			targetClass;	   /* COPY for speed */
};

struct _HTStructured {
	CONST HTStructuredClass *	isa;
	HTStream * 			target;
	HTStreamClass			targetClass;	   /* COPY for speed */
	CONST SGML_dtd *		dtd;
	
	char				buffer[BUFFER_SIZE+20];  /* Needed!! */
	char *				write_pointer;
	char *				line_break;
	BOOL				sensitive;          /* Can we put \n */
	BOOL				preformatted;     /* Is it verbatim? */
	BOOL				markup;     /* If doing LaTeX markup */
	BOOL				startup;      /* To skip MIME header */
};

PRIVATE char *TeX_names[][2] = {
    { "",       	""		},	/* HTML_A		*/
    { "",		""		},	/* HTML_ABBREV		*/
    { "\n\\begin{abstract}\n","\n\\end{abstract}\n"},  /* HTML_ABSTRACT	*/
    { "",		""		},	/* HTML_ACRONYM		*/
    { "",		""		},	/* HTML_ADDED		*/
    { "{\\it ",		"}"		},	/* HTML_ADDRESS		*/
    { "",		""		},	/* HTML_ARG		*/
    { "{\\bf ",		"}"		},	/* HTML_B		*/
    { "",		""		},	/* HTML_BASE		*/
    { "{\\sf ",		"}"		},	/* HTML_BLOCKQUOTE 	*/
    { "", 		""		},	/* HTML_BODY     	*/
    { "",		""		},	/* HTML_BOX		*/
    { "",		""		},	/* HTML_BR		*/
    { "",		""		},	/* HTML_BYLINE		*/
    { "",		""		},	/* HTML_CAPTION		*/
    { "",		""		},	/* HTML_CHANGED		*/
    { "\\cite{",      	"}"		},	/* HTML_CITE		*/
    { "",		""		},	/* HTML_CMD		*/
    { "{\\tt ",		"}"		},	/* HTML_CODE		*/
    { "\n\\typeout{",	"}\n"		},	/* HTML_COMMENT		*/
    { "]",		""		},	/* HTML_DD		*/
    { "",		""		},	/* HTML_DFN		*/
    { "",		""		},	/* HTML_DIR		*/
    { "\n\\begin{description}","\n\\end{description}\n"}, /* HTML_DL 	*/
    { "\n\\item[",	""		},	/* HTML_DT		*/
    { "{\\em ",		"}"		},	/* HTML_EM		*/
    { "",	""			},	/* HTML_FIG		*/
    { "\n\\footnote{",	"}\n"	       	},	/* HTML_FOOTNOTE	*/
    { "",	"" 			},	/* HTML_FORM		*/
    { "\n\\chapter{",	"}\n"		},	/* HTML_H1		*/
    { "\n\\section{",	"}\n"		},	/* HTML_H2		*/
    { "\n\\subsection{","}\n"  		},	/* HTML_H3		*/
    { "\n\\subsubsection{","}\n"	},	/* HTML_H4		*/
    { "\n\\paragraph{",	"}\n"		},	/* HTML_H5		*/
    { "\n\\subparagraph{","}\n"		},	/* HTML_H6		*/
    { "",		"\n"		},	/* HTML_H7		*/
    { "",		""		},	/* HTML_HEAD		*/
    { "",		""		},	/* HTML_HR		*/
    { "",		""		}, 	/* HTML_HTML		*/
    { ""		""		},    	/* HTML_HTMLPLUS	*/
    { "{\\it ",		"}"		},	/* HTML_I		*/
    { "",		""		},	/* HTML_IMAGE		*/
    { "",		""		},	/* HTML_IMG		*/
    { "",		""		},	/* HTML_INPUT		*/
    { "",		""		},	/* HTML_ISINDEX		*/
    { "{\\tt ",		"}"		},	/* HTML_KBD		*/
    { "",		""		},	/* HTML_L		*/
    { "\n\\item ",      "" 	        },	/* HTML_LI		*/
    { "",		""		},	/* HTML_LINK		*/
    { "",		""		},	/* HTML_LISTING		*/
    { "",		""		},	/* HTML_LIT		*/
    { "",		""		},	/* HTML_MARGIN		*/
    { "",		""		},	/* HTML_MATH		*/
    { "",		""		},	/* HTML_MENU		*/
    { "",		""		},	/* HTML_NEXTID		*/
    { "",		""		},	/* HTML_NOTE		*/
    { "\n\\begin{enumerate}\n","\n\\end{enumerate}\n"}, /* HTML_OL     	*/
    { "",		""		},	/* HTML_OPTION		*/
    { "",		""		},	/* HTML_OVER		*/
    { "\n\n",		""		},	/* HTML_P		*/
    { "",		""		},	/* HTML_PERSON		*/
    { "",		""		},	/* HTML_PLAINTEXT	*/
    { "\n\\begin{verbatim}"," \\end{verbatim}\n"},  /* HTML_PRE        */
    { "",		""		},	/* HTML_Q		*/
    { "\\begin{quote}",	"\\end{quote}"}, 	/* HTML_QUOTE		*/
    { "",		""		},	/* HTML_RENDER		*/
    { "",		""		},	/* HTML_REMOVED		*/
    { "",		""		},	/* HTML_S		*/
    { "",		""		},	/* HTML_SAMP		*/
    { "", 		""		},	/* HTML_SELECT		*/
    { "{\\bf ",		"}"		},	/* HTML_STRONG		*/
    { "", 		""		},	/* HTML_SUB		*/
    { "",		""		},	/* HTML_SUP		*/
    { "",		""		},	/* HTML_TAB		*/
    { "",		""		},	/* HTML_TABLE		*/
    { "",		""		},	/* HTML_TD		*/
    { "",		""		},	/* HTML_TEXTAREA	*/
    { "",		""		},	/* HTML_TH		*/
    { "\n\\title{",	"}\n\\author{}\n\\maketitle\n"},  /* HTML_TITLE */
    { "",		""		},	/* HTML_TR		*/
    { "",		""		},	/* HTML_TT		*/
    { "",		""		},	/* HTML_U		*/
    { "\n\\begin{itemize}","\n\\end{itemize}\n"},   /* HTML_UL		*/
    { "",		""		},	/* HTML_VAR		*/
    { "{\\sf ",		"}"		}	/* HTML_XMP		*/
};

PRIVATE char *TeX_entities[] = {
    "\\AE ",		/*"AElig",	 capital AE diphthong (ligature) */ 
    "\\\'{A} ",		/*"Aacute",	 capital A, acute accent */ 
    "\\^{A} ",		/*"Acirc",	 capital A, circumflex accent */ 
    "\\`{A} ",		/*"Agrave",	 capital A, grave accent */ 
    "\\AA ",   	 	/*"Aring",	 capital A, ring */ 
    "\\~{A} ", 		/*"Atilde",	 capital A, tilde */ 
    "\\\"{A} ",		/*"Auml",	 capital A, dieresis or umlaut mark */ 
    "\\c{C} ",		/*"Ccedil",	 capital C, cedilla */ 
    "\\OE",		/*"ETH",	 capital Eth, Icelandic */ 
    "\\\'{E} ",		/*"Eacute",	 capital E, acute accent */ 
    "\\^{E} ",		/*"Ecirc",	 capital E, circumflex accent */ 
    "\\`{E} ",		/*"Egrave",	 capital E, grave accent */ 
    "\\\"{E} ",		/*"Euml",	 capital E, dieresis or umlaut mark */ 
    "\\\'{I} ",		/*"Iacute",	 capital I, acute accent */ 
    "\\^{I} ",		/*"Icirc",	 capital I, circumflex accent */ 
    "\\`{I} ",		/*"Igrave",	 capital I, grave accent */ 
    "\\\"{I} ",		/*"Iuml",	 capital I, dieresis or umlaut mark */ 
    "\\~{N} ",		/*"Ntilde",	 capital N, tilde */ 
    "\\\'{O} ",		/*"Oacute",	 capital O, acute accent */ 
    "\\^{O} ",		/*"Ocirc",	 capital O, circumflex accent */ 
    "\\`{O} ",		/*"Ograve",	 capital O, grave accent */ 
    "\\O ",            	/*"Oslash",	 capital O, slash */ 
    "\\~{O} ",		/*"Otilde",	 capital O, tilde */ 
    "\\\"{O} ",        	/*"Ouml",	 capital O, dieresis or umlaut mark */ 
    "",		/*"THORN",	 capital THORN, Icelandic */ 
    "\\\'{U} ",		/*"Uacute",	 capital U, acute accent */ 
    "\\^{U} ",		/*"Ucirc",	 capital U, circumflex accent */ 
    "\\`{U} ",		/*"Ugrave",	 capital U, grave accent */ 
    "\\\"{U} ",		/*"Uuml",	 capital U, dieresis or umlaut mark */ 
    "\\\'{Y} ",		/*"Yacute",	 capital Y, acute accent */ 
    "\\\'{a} ",		/*"aacute",	 small a, acute accent */ 
    "\\^{a} ",		/*"acirc",	 small a, circumflex accent */ 
    "\\ae ",		/*"aelig",	 small ae diphthong (ligature) */ 
    "\\`{a} ",		/*"agrave",	 small a, grave accent */ 
    "&",            	/*"amp",	 ampersand */ 
    "\\aa ",		/*"aring",	 small a, ring */ 
    "\\~{a} ",		/*"atilde",	 small a, tilde */ 
    "\\\"{a} ",		/*"auml",	 small a, dieresis or umlaut mark */ 
    "\\c{c} ",		/*"ccedil",	 small c, cedilla */ 
    "\\\'{e} ",		/*"eacute",	 small e, acute accent */ 
    "\\^{c} ",		/*"ecirc",	 small e, circumflex accent */ 
    "\\`{c} ",		/*"egrave",	 small e, grave accent */ 
    "\\oe",		/*"eth",	 small eth, Icelandic */ 
    "\\\"{e} ",		/*"euml",	 small e, dieresis or umlaut mark */ 
    ">",		/*"gt",	 greater than */ 
    "\\\'{\\i} ",      	/*"iacute",	 small i, acute accent */ 
    "\\^{\\i} ",       	/*"icirc",	 small i, circumflex accent */ 
    "\\`{\\i} ",       	/*"igrave",	 small i, grave accent */ 
    "\\\"{\\i} ",     	/*"iuml",	 small i, dieresis or umlaut mark */ 
    "<",		/*"lt",	 less than */ 
    "\\~{n} ",		/*"ntilde",	 small n, tilde */ 
    "\\\'{o} ",		/*"oacute",	 small o, acute accent */ 
    "\\~{o} ",		/*"ocirc",	 small o, circumflex accent */ 
    "\\`{o} ",		/*"ograve",	 small o, grave accent */ 
    "\\o ",            	/*"oslash",	 small o, slash */ 
    "\\~{o} ",         	/*"otilde",	 small o, tilde */ 
    "\\\"{o} ",         /*"ouml",	 small o, dieresis or umlaut mark */ 
    "\\ss ",		/*"szlig",	 small sharp s, German (sz ligature)*/ 
    "",		/*"thorn",	 small thorn, Icelandic */ 
    "\\\'{u} ",        	/*"uacute",	 small u, acute accent */ 
    "\\^{u} ",        	/*"ucirc",	 small u, circumflex accent */ 
    "\\`{u} ",         	/*"ugrave",	 small u, grave accent */ 
    "\\\"{u} ",        	/*"uuml",	 small u, dieresis or umlaut mark */ 
    "\\\'{y} ",		/*"yacute",	 small y, acute accent */ 
    "\\\"{y} "	    	/*"yuml",	 small y, dieresis or umlaut mark */ 
};


/*	Flush Buffer
**	------------
*/
PRIVATE void HTTeXGen_flush ARGS1(HTStructured *, me)
{
    (*me->targetClass.put_block)(me->target, 
				 me->buffer,
				 me->write_pointer - me->buffer);
    me->write_pointer = me->buffer;
    me->line_break = me->buffer;
}


/*	Character handling
**	------------------
**
*/
PRIVATE void HTTeXGen_put_character ARGS2(HTStructured *, me, char, c)
{
    if (!me->startup)		                      /* To skip MIME header */
	return;
    if (c=='\n') {
	if (me->markup || me->preformatted) {     /* Put out as is and flush */
	    *me->write_pointer++ = c;
	    HTTeXGen_flush(me);
	    return;
	} else if (me->sensitive || *(me->write_pointer-1)==' ') {
	    return;
        } else
	    *me->write_pointer++ = ' ';		      /* Try to pretty print */
    } else if (me->markup || me->preformatted) {
	*me->write_pointer++ = c;
    } else if (c==' ' || c=='\t') {	              /* Skip space and tabs */
	if (*(me->write_pointer-1) != ' ')
	    *me->write_pointer++ = ' ';
	else
	    return;
    } else {
	if (c=='$' || c=='&' || c=='%' || c=='#' ||         /* Special chars */
	    c=='{' || c=='}' || c=='_') {
	    *me->write_pointer++ = '\\';
	    *me->write_pointer++ = c;
	    *me->write_pointer++ = ' ';
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

    if (c==' ')						   /* Find deliniter */
	me->line_break = me->write_pointer;
    else if (strchr(WORD_DELIMITERS, c))
	me->line_break = me->write_pointer-1;

    /* Flush buffer out when full */
    if (me->write_pointer >= me->buffer+BUFFER_SIZE-3) {
	if (me->markup || me->preformatted) {
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
}



/*	String handling
**	---------------
*/
PRIVATE void HTTeXGen_put_string ARGS2(HTStructured *, me, CONST char*, s)
{
    CONST char * p;
    for (p=s; *p; p++)
	HTTeXGen_put_character(me, *p);
}


PRIVATE void HTTeXGen_write ARGS3(HTStructured *, me, CONST char*, s, int, l)
{
    CONST char * p;
    for(p=s; p<s+l; p++)
	HTTeXGen_put_character(me, *p);
}


/*	Start Element
**	-------------
**
**     	No attributes are put to the output		Henrik 07/03-94
**	Does no assumptions of WHAT element is started...
*/
PRIVATE void HTTeXGen_start_element ARGS4(
	HTStructured *, 	me,
	int,			element_number,
	CONST BOOL*,	 	present,
	CONST char **,		value)
{
    me->startup = YES;			        /* Now, let's get down to it */
    if (me->preformatted == YES)	       /* Don't start markup in here */
	return;
    if (element_number == HTML_PRE)
	me->preformatted = YES;
    if (element_number == HTML_CITE ||	              /* No \n here, please! */
	element_number == HTML_COMMENT ||
	element_number == HTML_DT ||
	element_number == HTML_H1 ||
	element_number == HTML_H2 ||
	element_number == HTML_H3 ||
	element_number == HTML_H4 ||
	element_number == HTML_H5 ||
	element_number == HTML_H6 ||
	element_number == HTML_H7 ||
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
PRIVATE void HTTeXGen_end_element ARGS2(HTStructured *, me,
					int , element_number)
{
    if (me->preformatted && element_number != HTML_PRE)
	return;			
    me->preformatted = NO;
    me->markup = YES;
    HTTeXGen_put_string(me, *(TeX_names[element_number]+1));
    me->markup = NO;
    if (element_number == HTML_CITE ||	
	element_number == HTML_COMMENT ||
	element_number == HTML_DL ||
	element_number == HTML_H1 ||
	element_number == HTML_H2 ||
	element_number == HTML_H3 ||
	element_number == HTML_H4 ||
	element_number == HTML_H5 ||
	element_number == HTML_H6 ||
	element_number == HTML_H7 ||
	element_number == HTML_TITLE)
	me->sensitive = NO;
}


/*		Expanding entities
**		------------------
**
*/
PRIVATE void HTTeXGen_put_entity ARGS2(HTStructured *, me, int, entity_number)
{
    BOOL mark = me->markup;
    if (*TeX_entities[entity_number] != '&' && /* Theese are converted later */
	*TeX_entities[entity_number] != '<' &&
	*TeX_entities[entity_number] != '>')
	me->markup = YES;
    HTTeXGen_put_string(me, TeX_entities[entity_number]);
    me->markup = mark;
}



/*	Free an HTML object
**	-------------------
**
*/
PRIVATE void HTTeXGen_free ARGS1(HTStructured *, me)
{
    HTTeXGen_flush(me);
    (*me->targetClass.put_string)(me->target, "\n\\end{document}\n");
    HTTeXGen_flush(me);
    (*me->targetClass.free)(me->target);	/* ripple through */
    free(me);
}


PRIVATE void HTTeXGen_abort ARGS2(HTStructured *, me, HTError, e)
{
    HTTeXGen_free(me);
}


/*	Structured Object Class
**	-----------------------
*/
PRIVATE CONST HTStructuredClass HTTeXGeneration = /* As opposed to print etc */
{		
	"HTMLToTeX",
	HTTeXGen_free,
	HTTeXGen_abort,
	HTTeXGen_put_character,        	HTTeXGen_put_string,	HTTeXGen_write,
	HTTeXGen_start_element, 	HTTeXGen_end_element,
	HTTeXGen_put_entity
}; 


/*	HTConverter from HTML to TeX Stream
**	------------------------------------------
**
*/
PUBLIC HTStream* HTMLToTeX ARGS5(
	HTRequest *,		request,
	void *,			param,
	HTFormat,		input_format,
	HTFormat,		output_format,
	HTStream *,		output_stream)
{
    HTStructured* me = (HTStructured*) calloc(1, sizeof(*me));
    if (me == NULL) outofmem(__FILE__, "HTMLToTeX");    

    me->isa = (HTStructuredClass*) &HTTeXGeneration;
    me->dtd = &HTMLP_dtd;
    me->target = output_stream;
    me->targetClass = *me->target->isa;/* Copy pointers to routines for speed*/
    me->write_pointer = me->buffer;
    me->line_break = 	me->buffer;
    (*me->targetClass.put_string)(me->target,
        "\\documentstyle[11pt]{report}\n\\begin{document}\n");
    return SGML_new(&HTMLP_dtd, me);
}


/* END OF FILE HTTeXGen.c */








