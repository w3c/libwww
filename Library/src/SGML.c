/*									 SGML.c
**	GENERAL SGML PARSER CODE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This module implements an HTStream object. To parse an
**	SGML file, create this object which is a parser. The object
**	is (currently) created by being passed a DTD structure,
**	and a target HTStructured oject at which to throw the parsed stuff.
**	
**	 6 Feb 93  	Binary seraches used. Intreface modified.
**	 8 Jul 94  FM	Insulate free() from _free structure element.
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTChunk.h"
#include "SGML.h"

#define INVALID (-1)

/*	The State (context) of the parser
**
**	This is passed with each call to make the parser reentrant
**
*/



	
/*		Element Stack
**		-------------
**	This allows us to return down the stack reselcting styles.
**	As we return, attribute values will be garbage in general.
*/
typedef struct _HTElement HTElement;
struct _HTElement {
	HTElement *	next;	/* Previously nested element or 0 */
	HTTag*		tag;	/* The tag at this level  */
};


typedef enum _sgml_state {
    S_text, S_literal, S_tag, S_tag_gap, 
    S_attr, S_attr_gap, S_equals, S_value, S_after_open,
    S_nl, S_nl_tago,
    S_ero, S_cro,
#ifdef ISO_2022_JP
    S_esc, S_dollar, S_paren, S_nonascii_text,
#endif
    S_squoted, S_dquoted, S_end, S_entity, S_junk_tag
} sgml_state;


/*	Internal Context Data Structure
**	-------------------------------
*/
struct _HTStream {

    CONST HTStreamClass *	isa;		/* inherited from HTStream */
    
    CONST SGML_dtd 		*dtd;
    HTStructuredClass	*actions;	/* target class  */
    HTStructured	*target;	/* target object */

    HTTag 		*current_tag;
    int 		current_attribute_number;
    HTChunk		*string;
    HTElement		*element_stack;
    sgml_state		state;
#ifdef CALLERDATA		  
    void *		callerData;
#endif
    BOOL present[MAX_ATTRIBUTES];	/* Flags: attribute is present? */
    char * value[MAX_ATTRIBUTES];	/* malloc'd strings or NULL if none */
} ;


#define PUTC(ch) ((*context->actions->put_character)(context->target, ch))


/*	Find Attribute Number
**	---------------------
*/

PUBLIC int SGMLFindAttribute  (HTTag* tag, CONST char * s)
{
    attr* attributes = tag->attributes;

    int high, low, i, diff;		/* Binary search for attribute name */
    for(low=0, high=tag->number_of_attributes;
    		high > low ;
		diff < 0 ? (low = i+1) : (high = i) )  {
	i = (low + (high-low)/2);
	diff = strcasecomp(attributes[i].name, s);
	if (diff==0) return i;			/* success: found it */
    } /* for */
    
    return -1;
}


/*	Handle Attribute
**	----------------
*/
/* PUBLIC CONST char * SGML_default = "";   ?? */

PRIVATE void handle_attribute_name (HTStream * context, CONST char * s)
{

    HTTag * tag = context->current_tag;

    int i = SGMLFindAttribute(tag, s);
    if (i>=0) {
	context->current_attribute_number = i;
	context->present[i] = YES;
	if (context->value[i]) {
	    HT_FREE(context->value[i]);
	    context->value[i] = NULL;
	}
	return;
    } /* if */
	
    if (SGML_TRACE)
	TTYPrint(TDEST, "SGML: Unknown attribute %s for tag %s\n",
	    s, context->current_tag->name);
    context->current_attribute_number = INVALID;	/* Invalid */
}


/*	Handle attribute value
**	----------------------
*/
PRIVATE void handle_attribute_value (HTStream * context, CONST char * s)
{
    if (context->current_attribute_number != INVALID) {
	StrAllocCopy(context->value[context->current_attribute_number], s);
    } else {
        if (SGML_TRACE) TTYPrint(TDEST, "SGML: Attribute value %s ignored\n", s);
    }
    context->current_attribute_number = INVALID; /* can't have two assignments! */
}


/*	Handle entity
**	-------------
**
** On entry,
**	s	contains the entity name zero terminated
** Bugs:
**	If the entity name is unknown, the terminator is treated as
**	a printable non-special character in all cases, even if it is '<'
*/
PRIVATE void handle_entity (HTStream * context, char term)
{

    CONST char ** entities = context->dtd->entity_names;
    CONST char *s = context->string->data;
    
    int high, low, i, diff;
    for(low=0, high = context->dtd->number_of_entities;
    		high > low ;
		diff < 0 ? (low = i+1) : (high = i))   {  /* Binary serach */
	i = (low + (high-low)/2);
	diff = strcmp(entities[i], s);	/* Csse sensitive! */
	if (diff==0) {			/* success: found it */
	    (*context->actions->put_entity)(context->target, i);
	    return;
	}
    }
    /* If entity string not found, display as text */
    if (SGML_TRACE)
	TTYPrint(TDEST, "SGML: Unknown entity %s\n", s); 
    PUTC('&');
    {
	CONST char *p;
	for (p=s; *p; p++) {
	    PUTC(*p);
	}
    }
    PUTC(term);
}

/*
**	Helper function to check if the tag is on the stack
*/
PRIVATE BOOL lookup_element_stack (HTElement* stack, HTTag *tag)
{
    HTElement* elem;
    for (elem = stack; elem != NULL; elem = elem->next)
    {
        if (elem->tag == tag)  return YES;
    }
    return NO;
}

/*	End element
**	-----------
*/
PRIVATE void end_element (HTStream * context, HTTag * old_tag)
{
    if (SGML_TRACE) TTYPrint(TDEST, "SGML: End   </%s>\n", old_tag->name);
    if (old_tag->contents == SGML_EMPTY) {
        if (SGML_TRACE) TTYPrint(TDEST,"SGML: Illegal end tag </%s> found.\n",
		old_tag->name);
	return;
    }
    while (context->element_stack) 	{/* Loop is error path only */
	HTElement * N = context->element_stack;
	HTTag * t = N->tag;
	
	if (old_tag != t) {		/* Mismatch: syntax error */
	    /*
	    ** Patch from Maciej Puzio, puzio@laser.mimuw.edu.pl
	    ** See explanation in ../User/Patch/lib_4.0_1.fix
	    */
            if (context->element_stack->next   /* This is not the last level */
		&& lookup_element_stack(context->element_stack, old_tag)) {
		if (SGML_TRACE) TTYPrint(TDEST,
	    	"SGML: Found </%s> when expecting </%s>. </%s> assumed.\n",
		    old_tag->name, t->name, t->name);
	    } else {			/* last level */
		if (SGML_TRACE) TTYPrint(TDEST,
	            "SGML: Found </%s> when expecting </%s>. </%s> Ignored.\n",
		    old_tag->name, t->name, old_tag->name);
	        return;			/* Ignore */
	    }
	}
	
	context->element_stack = N->next;		/* Remove from stack */
	HT_FREE(N);
	(*context->actions->end_element)(context->target,
		 t - context->dtd->tags);
	if (old_tag == t) return;  /* Correct sequence */
	
	/* Syntax error path only */
	
    }
    if (SGML_TRACE) TTYPrint(TDEST,
	"SGML: Extra end tag </%s> found and ignored.\n", old_tag->name);
}


/*	Start an element
**	----------------
*/
PRIVATE void start_element (HTStream * context)
{
    HTTag * new_tag = context->current_tag;
    
    if (SGML_TRACE) TTYPrint(TDEST, "SGML: Start <%s>\n", new_tag->name);
    (*context->actions->start_element)(
    	context->target,
	new_tag - context->dtd->tags,
	context->present,
	(CONST char**) context->value);  /* coerce type for think c */
    if (new_tag->contents != SGML_EMPTY) {		/* i.e. tag not empty */
	HTElement * N;
	if ((N = (HTElement  *) HT_MALLOC(sizeof(HTElement))) == NULL)
	    HT_OUTOFMEM("start_element");
	N->next = context->element_stack;
	N->tag = new_tag;
	context->element_stack = N;
    }
}


/*		Find Tag in DTD tag list
**		------------------------
**
** On entry,
**	dtd	points to dtd structire including valid tag list
**	string	points to name of tag in question
**
** On exit,
**	returns:
**		NULL		tag not found
**		else		address of tag structure in dtd
*/
PUBLIC HTTag * SGMLFindTag (CONST SGML_dtd* dtd, CONST char * string)
{
    int high, low, i, diff;
    for(low=0, high=dtd->number_of_tags;
    		high > low ;
		diff < 0 ? (low = i+1) : (high = i))   {  /* Binary serach */
	i = (low + (high-low)/2);
	diff = strcasecomp(dtd->tags[i].name, string);	/* Case insensitive */
	if (diff==0) {			/* success: found it */
	    return &dtd->tags[i];
	}
    }
    return NULL;
}

/*________________________________________________________________________
**			Public Methods
*/


/*	Could check that we are back to bottom of stack! @@  */
PUBLIC int SGML_flush  (HTStream * context)
{
    while (context->element_stack) {
	HTElement *ptr = context->element_stack;
	if (SGML_TRACE)
	    TTYPrint(TDEST, "SGML........ Non-matched tag found: <%s>\n",
		    context->element_stack->tag->name);
	context->element_stack = ptr->next;
	HT_FREE(ptr);
    }
    return (*context->actions->flush)(context->target);
}

PUBLIC int SGML_free  (HTStream * context)
{
    int status;
    int cnt;
    while (context->element_stack) {    /* Make sure, that all tags are gone */
	HTElement *ptr = context->element_stack;

	if (SGML_TRACE)
	    TTYPrint(TDEST, "SGML........ Non-matched tag found: <%s>\n",
		    context->element_stack->tag->name);
	context->element_stack = ptr->next;
	HT_FREE(ptr);
    }
    if ((status = (*context->actions->_free)(context->target)) != HT_OK)
	return status;
    HTChunk_delete(context->string);
    for(cnt=0; cnt<MAX_ATTRIBUTES; cnt++)      	 /* Leak fix Henrik 18/02-94 */
	if(context->value[cnt])
	    HT_FREE(context->value[cnt]);
    HT_FREE(context);
    return HT_OK;
}

PUBLIC int SGML_abort  (HTStream * context, HTList * e)
{
    int cnt;
    while (context->element_stack) {    /* Make sure, that all tags are gone */
	HTElement *ptr = context->element_stack;
	if (SGML_TRACE)
	    TTYPrint(TDEST, "SGML........ Non-matched tag found: <%s>\n",
		    context->element_stack->tag->name);
	context->element_stack = ptr->next;
	HT_FREE(ptr);
    }
    (*context->actions->abort)(context->target, e);
    HTChunk_delete(context->string);
    for(cnt=0; cnt<MAX_ATTRIBUTES; cnt++)      	/* Leak fix Henrik 18/02-94 */
	if(context->value[cnt])
	    HT_FREE(context->value[cnt]);
    HT_FREE(context);
    return HT_ERROR;
}


/*	Read and write user callback handle
**	-----------------------------------
**
**   The callbacks from the SGML parser have an SGML context parameter.
**   These calls allow the caller to associate his own context with a
**   particular SGML context.
*/

#ifdef CALLERDATA		  
PUBLIC void* SGML_callerData (HTStream * context)
{
    return context->callerData;
}

PUBLIC void SGML_setCallerData (HTStream * context, void* data)
{
    context->callerData = data;
}
#else
#ifdef WWW_WIN_DLL
PUBLIC void * SGML_callerData (HTStream * context) {return NULL;}
PUBLIC void SGML_setCallerData (HTStream * context, void* data) {}
#endif /* WWW_WIN_DLL */
#endif /* CALLERDATA */

PUBLIC int SGML_character (HTStream * context, char c)

{
    CONST SGML_dtd	*dtd	=	context->dtd;
    HTChunk	*string = 	context->string;

    switch(context->state) {
    
    case S_after_open:	/* Strip one trainling newline
    			only after opening nonempty element.  - SGML:Ugh! */
        if (c=='\n' && (context->current_tag->contents != SGML_EMPTY)) {
	    break;
	}
	context->state = S_text;
	goto normal_text;
	/* (***falls through***) */
	
    case S_text:
normal_text:

#ifdef ISO_2022_JP
 	if (c=='\033') {
 	    context->state = S_esc;
 	    PUTC(c);
 	    break;
 	}
#endif /* ISO_2022_JP */
	if (c=='&' && (!context->element_stack || (
	    		 context->element_stack->tag  &&
	    		 ( context->element_stack->tag->contents == SGML_MIXED
			   || context->element_stack->tag->contents ==
			      				 SGML_RCDATA)
			))) {
	    string->size = 0;
	    context->state = S_ero;
	    
	} else if (c=='<') {
	    string->size = 0;
	    context->state = (context->element_stack &&
	    	context->element_stack->tag  &&
	    	context->element_stack->tag->contents == SGML_LITERAL) ?
	    			S_literal : S_tag;
	} else if (c=='\n') {	/* Newline - ignore if before tag end! */
	    context->state = S_nl;
	} else PUTC(c);
	break;

    case S_nl:
        if (c=='<') {
	    string->size = 0;
	    context->state = (context->element_stack &&
		context->element_stack->tag  &&
		context->element_stack->tag->contents == SGML_LITERAL) ?
				S_literal : S_nl_tago;
	} else {
	    PUTC('\n');
	    context->state = S_text;
	    goto normal_text;
	}
	break;

    case S_nl_tago:		/* Had newline and tag opener */
        if (c != '/') {
	    PUTC('\n');		/* Only ignore newline before </ */
	}
	context->state = S_tag;
	goto handle_S_tag;

#ifdef ISO_2022_JP
    case S_esc:
	if (c=='$') {
	    context->state = S_dollar;
	} else if (c=='(') {
	    context->state = S_paren;
	} else {
	    context->state = S_text;
	}
	PUTC(c);
	break;
    case S_dollar:
	if (c=='@' || c=='B') {
	    context->state = S_nonascii_text;
	} else {
	    context->state = S_text;
	}
	PUTC(c);
	break;
    case S_paren:
	if (c=='B' || c=='J') {
	    context->state = S_text;
	} else {
	    context->state = S_text;
	}
	PUTC(c);
	break;
    case S_nonascii_text:
	if (c=='\033') {
	    context->state = S_esc;
	    PUTC(c);
	} else {
	    PUTC(c);
	}
	break;
#endif /* ISO_2022_JP */

/*	In literal mode, waits only for specific end tag!
**	Only foir compatibility with old servers.
*/
    case S_literal :
	HTChunk_putc(string, c);
	if ( TOUPPER(c) != ((string->size ==1) ? '/'
		: context->element_stack->tag->name[string->size-2])) {
	    int i;
	    
	    /*	If complete match, end literal */
	    if ((c=='>') && (!context->element_stack->tag->name[string->size-2])) {
		end_element(context, context->element_stack->tag);
		string->size = 0;
		context->current_attribute_number = INVALID;
		context->state = S_text;
		break;
	    }		/* If Mismatch: recover string. */
	    PUTC( '<');
	    for (i=0; i<string->size; i++)	/* recover */
	       PUTC(
	       				      string->data[i]);
	    context->state = S_text;	
	}
	
        break;

/*	Character reference or Entity
*/
   case S_ero:
   	if (c=='#') {
	    context->state = S_cro;  /*   &# is Char Ref Open */ 
	    break;
	}
	context->state = S_entity;    /* Fall through! */
	
/*	Handle Entities
*/
    case S_entity:
	if (isalnum(c))
	    HTChunk_putc(string, c);
	else {
	    HTChunk_terminate(string);
	    handle_entity(context, c);
	    context->state = S_text;
	}
	break;

/*	Character reference
*/
    case S_cro:
	if (isalnum(c))
	    HTChunk_putc(string, c);	/* accumulate a character NUMBER */
	else {
	    int value;
	    HTChunk_terminate(string);
	    if (sscanf(string->data, "%d", &value)==1)
	        PUTC((char) value);
	    context->state = S_text;
	}
	break;

/*		Tag
*/	    
    case S_tag:				/* new tag */
handle_S_tag:

	if (isalnum(c))
	    HTChunk_putc(string, c);
	else {				/* End of tag name */
	    HTTag * t;
	    if (c=='/') {
		if (SGML_TRACE) if (string->size!=0)
		    TTYPrint(TDEST,"SGML:  `<%s/' found!\n", string->data);
		context->state = S_end;
		break;
	    }
	    HTChunk_terminate(string) ;

	    t = SGMLFindTag(dtd, string->data);
	    if (!t) {
		if(SGML_TRACE) TTYPrint(TDEST, "SGML: *** Unknown element %s\n",
			string->data);
		context->state = (c=='>') ? S_text : S_junk_tag;
		break;
	    }
	    context->current_tag = t;
	    
	    /*  Clear out attributes
	    */
	    
	    {
	        int i;
	        for (i=0; i< context->current_tag->number_of_attributes; i++)
	    	    context->present[i] = NO;
	    }
	    string->size = 0;
	    context->current_attribute_number = INVALID;
	    
	    if (c=='>') {
		if (context->current_tag->name) start_element(context);
		context->state = S_after_open;
	    } else {
	        context->state = S_tag_gap;
	    }
	}
	break;

		
    case S_tag_gap:		/* Expecting attribute or > */
	if (WHITE(c)) break;	/* Gap between attributes */
	if (c=='>') {		/* End of tag */
	    if (context->current_tag->name) start_element(context);
	    context->state = S_after_open;
	    break;
	}
	HTChunk_putc(string, c);
	context->state = S_attr;		/* Get attribute */
	break;
	
   				/* accumulating value */
    case S_attr:
	if (WHITE(c) || (c=='>') || (c=='=')) {		/* End of word */
	    HTChunk_terminate(string) ;
	    handle_attribute_name(context, string->data);
	    string->size = 0;
	    if (c=='>') {		/* End of tag */
		if (context->current_tag->name) start_element(context);
		context->state = S_after_open;
		break;
	    }
	    context->state = (c=='=' ?  S_equals: S_attr_gap);
	} else {
	    HTChunk_putc(string, c);
	}
	break;
		
    case S_attr_gap:		/* Expecting attribute or = or > */
	if (WHITE(c)) break;	/* Gap after attribute */
	if (c=='>') {		/* End of tag */
	    if (context->current_tag->name) start_element(context);
	    context->state = S_after_open;
	    break;
	} else if (c=='=') {
	    context->state = S_equals;
	    break;
	}
	HTChunk_putc(string, c);
	context->state = S_attr;		/* Get next attribute */
	break;
	
    case S_equals:			/* After attr = */ 
	if (WHITE(c)) break;	/* Before attribute value */
	if (c=='>') {		/* End of tag */
	    if (SGML_TRACE) TTYPrint(TDEST, "SGML: found = but no value\n");
	    if (context->current_tag->name) start_element(context);
	    context->state = S_after_open;
	    break;
	    
	} else if (c=='\'') {
	    context->state = S_squoted;
	    break;

	} else if (c=='"') {
	    context->state = S_dquoted;
	    break;
	}
	HTChunk_putc(string, c);
	context->state = S_value;
	break;
	
    case S_value:
	if (WHITE(c) || (c=='>')) {		/* End of word */
	    HTChunk_terminate(string) ;
	    handle_attribute_value(context, string->data);
	    string->size = 0;
	    if (c=='>') {		/* End of tag */
		if (context->current_tag->name) start_element(context);
		context->state = S_after_open;
		break;
	    }
	    else context->state = S_tag_gap;
	} else {
	    HTChunk_putc(string, c);
	}
	break;
		
    case S_squoted:		/* Quoted attribute value */
	if (c=='\'') {		/* End of attribute value */
	    HTChunk_terminate(string) ;
	    handle_attribute_value(context, string->data);
	    string->size = 0;
	    context->state = S_tag_gap;
	} else {
	    HTChunk_putc(string, c);
	}
	break;
	
    case S_dquoted:		/* Quoted attribute value */
	if (c=='"') {		/* End of attribute value */
	    HTChunk_terminate(string) ;
	    handle_attribute_value(context, string->data);
	    string->size = 0;
	    context->state = S_tag_gap;
	} else {
	    HTChunk_putc(string, c);
	}
	break;
	
    case S_end:					/* </ */
	if (isalnum(c))
	    HTChunk_putc(string, c);
	else {				/* End of end tag name */
	    HTTag * t;
	    HTChunk_terminate(string) ;
	    if (!*string->data)	{	/* Empty end tag */
	        t = context->element_stack->tag;
	    } else {
		t = SGMLFindTag(dtd, string->data);
	    }
	    if (!t) {
		if(SGML_TRACE) TTYPrint(TDEST,
		    "Unknown end tag </%s>\n", string->data); 
	    } else {
	        context->current_tag = t;
		end_element( context, context->current_tag);
	    }

	    string->size = 0;
	    context->current_attribute_number = INVALID;
	    if (c!='>') {
		if (SGML_TRACE && !WHITE(c))
		    TTYPrint(TDEST,"SGML:  `</%s%c' found!\n",
		    	string->data, c);
		context->state = S_junk_tag;
	    } else {
	        context->state = S_text;
	    }
	}
	break;

		
    case S_junk_tag:
	if (c=='>') {
	    context->state = S_text;
	}
    } /* switch on context->state */
    return HT_OK;
}


PUBLIC int SGML_string (HTStream * context, CONST char* s)
{
    while (*s)
        SGML_character(context, *s++);
    return HT_OK;
}


PUBLIC int SGML_write (HTStream * context, CONST char* b, int l)
{
    while (l-- > 0)
        SGML_character(context, *b++);
    return HT_OK;
}

/*_______________________________________________________________________
*/

/*	Structured Object Class
**	-----------------------
*/
PRIVATE CONST HTStreamClass SGMLParser = 
{		
    "SGMLParser",
    SGML_flush,
    SGML_free,
    SGML_abort,
    SGML_character, 
    SGML_string,
    SGML_write,
}; 

/*	Create SGML Engine
**	------------------
**
** On entry,
**	dtd		represents the DTD, along with
**	actions		is the sink for the data as a set of routines.
**
*/
PUBLIC HTStream * SGML_new (CONST SGML_dtd * dtd, HTStructured * target)
{
    int i;
    HTStream* context;
    if ((context = (HTStream  *) HT_MALLOC(sizeof(*context))) == NULL)
        HT_OUTOFMEM("SGML_begin");

    context->isa = &SGMLParser;
    context->string = HTChunk_new(128);	/* Grow by this much */
    context->dtd = dtd;
    context->target = target;
    context->actions = (HTStructuredClass*)(((HTStream*)target)->isa);
    					/* Ugh: no OO */
    context->state = S_text;
    context->element_stack = 0;			/* empty */
#ifdef CALLERDATA		  
    context->callerData = (void*) callerData;
#endif    
    for(i=0; i<MAX_ATTRIBUTES; i++) context->value[i] = 0;

    return context;
}
