/*								       HTWSRC.c
**	PARSE WAIS SOURCE FILE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module parses a stream with WAIS source file
**	format information on it and creates a structured stream.
**	That structured stream is then converted into whatever.
**
**	3 Jun 93	Bug fix: Won't crash if no description
**	8 Jul 94  FM	Insulate free() from _free structure element.
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTMLPDTD.h"
#include "HTMLGen.h"
#include "HTParse.h"
#include "HTReqMan.h"
#include "HTProxy.h"
#include "HTWSRC.h"					 /* Implemented here */

#define BIG 10000		/* Arbitrary limit to value length */
#define PARAM_MAX BIG
#define CACHE_PERIOD (7*86400)	/* Time to keep .src file in seconds */

#define HEX_ESCAPE '%'

struct _HTStructured {
	const HTStructuredClass *	isa;
	/* ... */
};

#define PUTC(c) (*me->target->isa->put_character)(me->target, c)
#define PUTS(s) (*me->target->isa->put_string)(me->target, s)
#define START(e) (*me->target->isa->start_element)(me->target, e, 0, 0)
#define END(e) (*me->target->isa->end_element)(me->target, e)


/*	Here are the parameters which can be specified in a  source file
*/
PRIVATE const char* par_name[] = {
	"version", 
	"ip-address",
#define PAR_IP_NAME 2
	"ip-name", 
#define PAR_TCP_PORT 3
	"tcp-port", 
#define PAR_DATABASE_NAME 4
	"database-name",
#define PAR_COST 5
	"cost", 
#define PAR_COST_UNIT 6
	"cost-unit", 
#define PAR_FREE 7
	"free",	
#define PAR_MAINTAINER 8
	"maintainer", 	
#define PAR_DESCRIPTION 9
	"description",
	"keyword-list", 	
	"source",
#define PAR_UNKNOWN 12
	"unknown",
	0,				/* Terminate list */
#define PAR_COUNT 13
} ;

#if 0
PRIVATE  const char * hex = "0123456789ABCDEF";
#endif

enum tokenstate { beginning, before_tag, colon, before_value,
		value, bracketed_value, quoted_value, escape_in_quoted, done };


/*		Stream Object
**		------------
**
**	The target is the structured stream down which the
**	parsed results will go.
**
**	all the static stuff below should go in here to make it reentrant
*/

struct _HTStream {
	const HTStreamClass *	isa;
	HTStructured *		target;
	HTRequest *		request;
	char *			par_value[PAR_COUNT];
	enum tokenstate 	state;
	char 			param[BIG+1];
	int			param_number;
	int			param_count;
};



#if 0
/*	Decode one hex character
*/

PRIVATE char from_hex (char c)
{
    return 		  (c>='0')&&(c<='9') ? c-'0'
			: (c>='A')&&(c<='F') ? c-'A'+10
			: (c>='a')&&(c<='f') ? c-'a'+10
			:		       0;
}
#endif

/*			State machine
**			-------------
**
** On entry,
**	me->state	is a valid state (see WSRC_init)
**	c		is the next character
** On exit,
** 	returns	1	Done with file
**		0	Continue. me->state is updated if necessary.
**		-1	Syntax error error
*/


/*		Treat One Character
**		-------------------
*/
PRIVATE int WSRCParser_put_character (HTStream* me, char c)
{
    switch (me->state) {
    case beginning:
        if (c=='(') me->state = before_tag;
	break;
	
    case before_tag:
        if (c==')') {
	    me->state = done;
	    return HT_OK;			/* Done with input file */
	} else if (c==':') {
	    me->param_count = 0;
	    me->state = colon;
	}				/* Ignore other text */
	break;

    case colon:
        if (isspace((int) c)) {
	    me->param[me->param_count++] = 0;	/* Terminate */
	    for(me->param_number = 0; par_name[me->param_number]; me->param_number++) {
		if (0==strcmp(par_name[me->param_number], me->param)) {
		    break;
		}
	    }
	    if (!par_name[me->param_number]) {	/* Unknown field */
	        HTTRACE(PROT_TRACE, "HTWSRC: Unknown field `%s' in source file\n" _ 
		    me->param);
		me->param_number = PAR_UNKNOWN;
		me->state = before_value;	/* Could be better ignore */
		return HT_OK;
	    }
	    me->state = before_value;
	} else {
	    if (me->param_count < PARAM_MAX)  me->param[me->param_count++] = c;
	}
	break;
	
    case before_value:
        if (c==')') {
	    me->state = done;
	    return HT_OK;			/* Done with input file */
	}
	if (isspace((int) c)) return HT_OK;		/* Skip white space */
	me->param_count = 0;
	if (c=='"') {
	    me->state = quoted_value;
	    break;
	}
	me->state = (c=='"') ? quoted_value : 
		    (c=='(') ? bracketed_value : value;
	me->param[me->param_count++] = c;	/* Don't miss first character */
	break;

    case value:
        if (isspace((int) c)) {
	    me->param[me->param_count] = 0;
	    StrAllocCopy(me->par_value[me->param_number], me->param);
	    me->state = before_tag;
	} else {
	    if (me->param_count < PARAM_MAX)  me->param[me->param_count++] = c;
	}
	break;

    case bracketed_value:
        if (c==')') {
	    me->param[me->param_count] = 0;
	    StrAllocCopy(me->par_value[me->param_number], me->param);
	    me->state = before_tag;
	    break;
	}
        if (me->param_count < PARAM_MAX)  me->param[me->param_count++] = c;
	break;
	
    case quoted_value:
        if (c=='"') {
	    me->param[me->param_count] = 0;
	    StrAllocCopy(me->par_value[me->param_number], me->param);
	    me->state = before_tag;
	    break;
	}
	
	if (c=='\\') {		/* Ignore escape but switch state */
	    me->state = escape_in_quoted;
	    break;
	}
	/* Fall through! */

    case escape_in_quoted:
        if (me->param_count < PARAM_MAX)  me->param[me->param_count++] = c;
	break;
	
    case done:				/* Ignore anything after EOF */
	return HT_OK;

    } /* switch me->state */
    return HT_OK;
}


/*			Open Cache file
**			===============
**
**   Bugs: Maybe for filesystem-challenged platforms (MSDOS for example) we
**   should make a hash code for the filename.
*/

#ifdef CACHE_FILE_PREFIX
PRIVATE BOOL write_cache (HTStream * me)
{
    FILE * fp;
    char cache_file_name[256];
    char * www_database;
    if (!me->par_value[PAR_DATABASE_NAME]
    	|| !me->par_value[PAR_IP_NAME]
	) return NO;
    
    www_database = HTEscape(me->par_value[PAR_DATABASE_NAME], URL_XALPHAS);
    sprintf(cache_file_name, "%sWSRC-%s:%s:%.100s.txt",
    	CACHE_FILE_PREFIX,
	me->par_value[PAR_IP_NAME],
	me->par_value[PAR_TCP_PORT] ? me->par_value[PAR_TCP_PORT] : "210",
	www_database);
    HT_FREE(www_database);
    fp = fopen(cache_file_name, "wb");
    if (!fp) return NO;
    
    if (me->par_value[PAR_DESCRIPTION])
        fputs(me->par_value[PAR_DESCRIPTION], fp);
    else 
        fputs("Description not available\n", fp);
    fclose(fp);
    return YES;
}
#endif

/*			Output equivalent HTML
**			----------------------
**
*/

void give_parameter (HTStream * me, int p)
{
    PUTS(par_name[p]);
    if (me->par_value[p]) {
	PUTS(": ");
	PUTS(me->par_value[p]);
	PUTS("; ");
    } else {
        PUTS(" NOT GIVEN in source file; ");
    }
}


/*			Generate Outout
**			===============
*/
PRIVATE void WSRC_gen_html (HTStream * me, BOOL source_file)

{
    if (me->par_value[PAR_DATABASE_NAME]) {
	char * shortname = NULL;
	int l;
	StrAllocCopy(shortname, me->par_value[PAR_DATABASE_NAME]);
	l = strlen(shortname);
	if ( l > 4 && !strcasecomp(shortname + l -4, ".src")) {
	    shortname[l-4] = 0;	/* Chop of .src -- boring! */
	}
	
	START(HTML_TITLE);
	PUTS(shortname);
	PUTS(source_file ? " WAIS source file" : " index");
	END(HTML_TITLE);
    
	START(HTML_H1);
	PUTS(shortname);
	PUTS(source_file ? " description" : " index");
	END(HTML_H1);
	HT_FREE(shortname);				  /* memleak, henrik */
    }
    
    START(HTML_DL);		/* Definition list of details */
    
    if (source_file) {
	START(HTML_DT);
	PUTS("Access link");
	START(HTML_DD);
	if (me->par_value[PAR_IP_NAME] &&
	    me->par_value[PAR_DATABASE_NAME]) {
	    char WSRC_address[256];
	    char *addr = HTAnchor_address((HTAnchor*) me->request->anchor);
	    char *gate = HTGateway_find(addr);
	    char *www_database = HTEscape(me->par_value[PAR_DATABASE_NAME],
					  URL_XALPHAS);
	    if (!gate) {
		sprintf(WSRC_address, "wais://%s%s%s/%s",
			me->par_value[PAR_IP_NAME],
			me->par_value[PAR_TCP_PORT] ? ":" : "",
			me->par_value[PAR_TCP_PORT] ?
			me->par_value[PAR_TCP_PORT] :"", www_database);
		HTStartAnchor(me->target, NULL, WSRC_address);
		PUTS("Direct access");
		END(HTML_A);
	    } else {
		sprintf(WSRC_address, "%s%s%s%s/%s",
			gate,
			me->par_value[PAR_IP_NAME],
			me->par_value[PAR_TCP_PORT] ? ":" : "",
			me->par_value[PAR_TCP_PORT] ?
			me->par_value[PAR_TCP_PORT] : "",
			www_database);
		HTStartAnchor(me->target, NULL, WSRC_address);
		PUTS("Through a gateway");
		END(HTML_A);
	    }
	    HT_FREE(gate);
	    HT_FREE(addr);
	    HT_FREE(www_database);
	    
	} else {
	    give_parameter(me, PAR_IP_NAME);
	    give_parameter(me, PAR_DATABASE_NAME);
	}
    
    } /* end if source_file */
    
    if (me->par_value[PAR_MAINTAINER]) {
	START(HTML_DT);
	PUTS("Maintainer");
	START(HTML_DD);
	PUTS(me->par_value[PAR_MAINTAINER]);
    }
    if (me->par_value[PAR_IP_NAME]) {
    	START(HTML_DT);
    	PUTS("Host");
    	START(HTML_DD);
    	PUTS(me->par_value[PAR_IP_NAME]);
    }
    
    END(HTML_DL);

    if (me->par_value[PAR_DESCRIPTION]) {
	START(HTML_PRE);		/* Preformatted description */
	PUTS(me->par_value[PAR_DESCRIPTION]);
	END(HTML_PRE);
    }
    
    (*me->target->isa->_free)(me->target);
    
    return;
} /* generate html */


PRIVATE int WSRCParser_put_string (HTStream * context, const char* s)
{
    while (*s)
        WSRCParser_put_character(context, *s++);
    return HT_OK;
}


PRIVATE int WSRCParser_write (HTStream * 	context,
				   const char* 	b,
				   int		l)
{
    while (l-- > 0)
	WSRCParser_put_character(context, *b++);
    return HT_OK;
}

PRIVATE int WSRCParser_flush (HTStream * me)
{
    return HT_OK;
}

PRIVATE int WSRCParser_free (HTStream * me)
{
    WSRC_gen_html(me, YES);
#ifdef CACHE_FILE_PREFIX
    write_cache(me);
#endif
    {
	int p;
	for(p=0; par_name[p]; p++) {	/* Clear out old values */
	    if (me->par_value[p]) {
		HT_FREE(me->par_value[p]);
	    }
	}
    }
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int WSRCParser_abort (HTStream * me, HTList * e)
{
    WSRCParser_free(me);
    return HT_ERROR;
}


/*		Stream subclass		-- method routines
**		---------------
*/

HTStreamClass WSRCParserClass = {
	"WSRCParser",
	WSRCParser_flush,
	WSRCParser_free,
	WSRCParser_abort,
	WSRCParser_put_character,
 	WSRCParser_put_string,
	WSRCParser_write

};


/*		Converter from WAIS Source to whatever
**		--------------------------------------
*/
PUBLIC HTStream* HTWSRCConvert (HTRequest *	request,
				void *		param,
				HTFormat	input_format,
				HTFormat	output_format,
				HTStream *	output_stream)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTWSRCConvert");
    me->isa = &WSRCParserClass;
    me->target = HTMLGenerator(request, param, input_format, output_format,
			       output_stream);
    me->request = request;
    me->state = beginning;

    return me;
}

