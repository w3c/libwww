/*			General SGML Parser code		SGML.c
**			========================
**
**	This module implements an HTSGMLContext object. To parse an
**	SGML file, create this object which is a parser. The object
**	is (currently) created by being parsed a DTD structure.
**
**	
*/
#include "SGML.h"

#include <ctype.h>
#include <stdio.h>
#include "HTUtils.h"
#include "HTChunk.h"
#include "tcp.h"		/* For FROMASCII */

/*	The State (context) of the parser
**
**	This is passed with each call to make the parser recursive
**
*/

struct _HTSGMLContext {
    SGML_dtd 		*dtd;
    void 		(*contents_treatment) PARAMS((void * data, char c));
    HTTag 		*current_tag;
    attr 		*current_attribute;
    HTChunk		*string;
    HTElement		*element_stack;
    enum sgml_state { S_text, S_litteral, S_tag, S_tag_gap, 
		S_attr, S_attr_gap, S_equals, S_value,
		S_ero, S_cro,
		  S_squoted, S_dquoted, S_end, S_entity, S_junk_tag} state;
    void *		callerData;
};


/*	Handle Attribute
**	----------------
*/
/* PUBLIC CONST char * SGML_default = "";   ?? */

#ifdef __STDC__
PRIVATE void handle_attribute_name(HTSGMLContext context, const char * s)
#else
PRIVATE void handle_attribute_name(context, s)
    HTSGMLContext context;
    char *s;
#endif
{
    attr* a;
    for(    a = context->current_tag->attributes;
	    a->name;
	    a++) {
	if (0==strcasecomp(a->name, s))
	    break;
    }
    if (!a->name) {
	if (TRACE)
	    fprintf(stderr, "SGML: Unknown attribute %s for tag %s\n",
		s, context->current_tag->name);
        context->current_attribute = 0;	/* Invalid */
	return;
    }
    a->present = YES;
    if (a->value) {
        free(a->value);
	a->value = 0;
    }
    context->current_attribute = a;
}


/*	Handle attribute value
**	----------------------
*/
#ifdef __STDC__
PRIVATE void handle_attribute_value(HTSGMLContext context, const char * s)
#else
PRIVATE void handle_attribute_value(context, s)
    HTSGMLContext context;
    char *s;
#endif
{
    if (context->current_attribute) {
	StrAllocCopy(context->current_attribute->value, s);
    } else {
        if (TRACE) fprintf(stderr, "SGML: Attribute value %s ignored\n", s);
    }
    context->current_attribute = 0;	/* can't have two assignments! */
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
#ifdef __STDC__
PRIVATE void handle_entity(HTSGMLContext context, char term)
#else
PRIVATE void handle_entity(context, term)
    HTSGMLContext context;
    char term;
#endif
{
    entity * e;
    entity * entities = context->dtd->entities;
    CONST char *s = context->string->data;

    for(e = entities; e->name; e++) {
	if (0==strcmp(e->name, s)) {
	    char * p;
	    for (p=e->representation; *p; p++) {
		(*context->contents_treatment)(context->callerData, *p);
	    }
	    return;	/* Good */
	}
    }
    /* If entity string not found, display as text */
    if (TRACE)
	fprintf(stderr, "SGML: Unknown entity %s\n", s); 
    (*context->contents_treatment)(context->callerData, '&');
    {
	CONST char *p;
	for (p=s; *p; p++) {
	    (*context->contents_treatment)(context->callerData, *p);
	}
    }
    (*context->contents_treatment)(context->callerData, term);
}

/*	End element
*/
#ifdef __STDC__
PRIVATE void end_element(HTSGMLContext context, HTTag * old_tag)
#else
PRIVATE void end_element(context, old_tag)
    HTTag * old_tag;
    HTSGMLContext context;
#endif
{
    if (TRACE) fprintf(stderr, "SGML: End   </%s>\n", old_tag->name);
    if (!old_tag->end) {
        if (TRACE) fprintf(stderr,"SGML: Illegal end tag </%s> found.\n",
		old_tag->name);
	return;
    }
    while (context->element_stack) 	{/* Loop is error path only */
	HTElement * N = context->element_stack;
	HTTag * t = N->tag;
	
	if (old_tag != t) {		/* Mismatch: syntax error */
	    if (context->element_stack->next) {	/* This is not the last level */
		if (TRACE) fprintf(stderr,
	    	"SGML: Found </%s> when expecting </%s>. </%s> assumed.\n",
		    old_tag->name, t->name, t->name);
	    } else {			/* last level */
		if (TRACE) fprintf(stderr,
	            "SGML: Found </%s> when expecting </%s>. </%s> Ignored.\n",
		    old_tag->name, t->name, old_tag->name);
	        return;			/* Ignore */
	    }
	}
	
	context->element_stack = N->next;		/* Remove from stack */
	free(N);
	(t->end)(context->callerData,
		 t,
		 context->element_stack);	/* Assume tag end */
	if (context->element_stack)		/* not end of document */
	    context->contents_treatment = context->element_stack->tag->treat;
	if (old_tag == t) return;  /* Correct sequence */
	
	/* Syntax error path only */
	
    }
    fprintf(stderr,
	"SGML: Extra end tag </%s> found and ignored.\n", old_tag->name);
}


/*	Start a element
*/
#ifdef __STDC__
PRIVATE void start_element(HTSGMLContext context)
#else
PRIVATE void start_element(context)
    HTSGMLContext context;
#endif
{
    HTTag * new_tag = context->current_tag;
    
    if (TRACE) fprintf(stderr, "SGML: Start <%s>\n", new_tag->name);
    (*new_tag->begin)(context->callerData, new_tag, context->element_stack);
    if (new_tag->end) {		/* i.e. tag not empty */
	HTElement * N = (HTElement *)malloc(sizeof(HTElement));
        if (N == NULL) outofmem(__FILE__, "start_element");
	N->next = context->element_stack;
	N->tag = new_tag;
	context->element_stack = N;
	context->contents_treatment = new_tag->treat;
    }
}

/*________________________________________________________________________
**			Public Methods
*/

/*	Create SGML Engine
**	------------------
**
** On entry,
**	dtd->tags		represents the DTD, along with
**	dtd->entities
**
**	default_tag	represents the initial and final actions,
**			and the character processing, for data outside
**			any tags. May not be empty.
*/

PUBLIC HTSGMLContext SGML_begin  ARGS1(SGML_dtd *,dtd)
{
    HTSGMLContext context = (HTSGMLContext) malloc(sizeof(*context));
    if (!context) outofmem(__FILE__, "SGML_begin");

    context->string = HTChunkCreate(128);	/* Grow by this much */
    context->dtd = dtd;
    context->state = S_text;
    context->element_stack = 0;			/* empty */
    context->callerData = (void*) 0;		/* unspcified as yet */
    context->current_tag = dtd->default_tag;
    start_element(context);		/* Start document */
    return context;
}


PUBLIC void SGML_end  ARGS1(HTSGMLContext, context)
{
    end_element(context, context->dtd->default_tag);	/* End document */
    HTChunkFree(context->string);
    free(context);
}

/*	Read and write user callback handle
**	-----------------------------------
**
**   The callbacks from the SGML parser have an SGML context parameter.
**   These calls allow the caller to associate his own context with a
**   particular SGML context.
*/

PUBLIC void* SGML_callerData ARGS1(HTSGMLContext, context)
{
    return context->callerData;
}

PUBLIC void SGML_setCallerData ARGS2(HTSGMLContext, context, void*, data)
{
    context->callerData = data;
}


PUBLIC void SGML_string ARGS2(HTSGMLContext, context, char*, str)
{
    char *p;
    for(p=str; *p; p++)
        SGML_character(context, *p);
}

PUBLIC void SGML_character ARGS2(HTSGMLContext, context, char,c)

{
    SGML_dtd	*dtd	=	context->dtd;
    HTChunk	*string = 	context->string;

    switch(context->state) {
    case S_text:
	if (c=='&' && !(context->element_stack &&
	    		context->element_stack->tag  &&
	    		context->element_stack->tag->litteral)) {
	    string->size = 0;
	    context->state = S_ero;
	    
	} else if (c=='<') {
	    string->size = 0;
	    context->state = (context->element_stack &&
	    		context->element_stack->tag  &&
	    		context->element_stack->tag->litteral) ?
	    			S_litteral : S_tag;
	} else (*context->contents_treatment)(context->callerData, c);
	break;

/*	In litteral mode, waits only for specific end tag!
*/
    case S_litteral :
	HTChunkPutc(string, c);
	if ( TOUPPER(c) != ((string->size ==1) ? '/'
		: context->element_stack->tag->name[string->size-2])) {
	    int i;
	    
	    /*	If complete match, end litteral */
	    if ((c=='>') && (!context->element_stack->tag->name[string->size-2])) {
		end_element(context, context->element_stack->tag);
		string->size = 0;
		context->current_attribute = (attr *) 0;
		context->state = S_text;
		break;
	    }		/* If Mismatch: recover string. */
	    (*context->contents_treatment)(context->callerData, '<');
	    for (i=0; i<string->size; i++)	/* recover */
	       (*context->contents_treatment)(context->callerData,
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
	    HTChunkPutc(string, c);
	else {
	    HTChunkTerminate(string);
	    handle_entity(context, c);
	    context->state = S_text;
	}
	break;

/*	Character reference
*/
    case S_cro:
	if (isalnum(c))
	    HTChunkPutc(string, c);	/* accumulate a character NUMBER */
	else {
	    int value;
	    HTChunkTerminate(string);
	    if (sscanf(string->data, "%d", &value)==1)
	        (*context->contents_treatment)(context->callerData,
					       FROMASCII((char)value));
	    context->state = S_text;
	}
	break;

/*		Tag
*/	    
    case S_tag:				/* new tag */
	if (isalnum(c))
	    HTChunkPutc(string, c);
	else {				/* End of tag name */
	    attr * a;
	    if (c=='/') {
		if (TRACE) if (string->size!=0)
		    fprintf(stderr,"SGML:  `<%s/' found!\n", string->data);
		context->state = S_end;
		break;
	    }
	    HTChunkTerminate(string) ;
	    for(context->current_tag = dtd->tags;
	    	context->current_tag->name; context->current_tag++) {
		if (0==strcasecomp(context->current_tag->name, string->data)) {
		    break;
		}
	    }
	    if (!context->current_tag->name) {
		if(TRACE) fprintf(stderr, "Unknown tag %s\n",
			string->data);
		context->state = (c=='>') ? S_text : S_junk_tag;
		break;
	    }
	    
	    for (a = context->current_tag->attributes; a->name; a++ ) {
		a->present = NO;
	    }
	    string->size = 0;
	    context->current_attribute = (attr *) 0;
	    
	    if (c=='>') {
		if (context->current_tag->name) start_element(context);
		context->state = S_text;
	    } else {
	        context->state = S_tag_gap;
	    }
	}
	break;

		
    case S_tag_gap:		/* Expecting attribute or > */
	if (WHITE(c)) break;	/* Gap between attributes */
	if (c=='>') {		/* End of tag */
	    if (context->current_tag->name) start_element(context);
	    context->state = S_text;
	    break;
	}
	HTChunkPutc(string, c);
	context->state = S_attr;		/* Get attribute */
	break;
	
   				/* accumulating value */
    case S_attr:
	if (WHITE(c) || (c=='>') || (c=='=')) {		/* End of word */
	    HTChunkTerminate(string) ;
	    handle_attribute_name(context, string->data);
	    string->size = 0;
	    if (c=='>') {		/* End of tag */
		if (context->current_tag->name) start_element(context);
		context->state = S_text;
		break;
	    }
	    context->state = (c=='=' ?  S_equals: S_attr_gap);
	} else {
	    HTChunkPutc(string, c);
	}
	break;
		
    case S_attr_gap:		/* Expecting attribute or = or > */
	if (WHITE(c)) break;	/* Gap after attribute */
	if (c=='>') {		/* End of tag */
	    if (context->current_tag->name) start_element(context);
	    context->state = S_text;
	    break;
	} else if (c=='=') {
	    context->state = S_equals;
	    break;
	}
	HTChunkPutc(string, c);
	context->state = S_attr;		/* Get next attribute */
	break;
	
    case S_equals:			/* After attr = */ 
	if (WHITE(c)) break;	/* Before attribute value */
	if (c=='>') {		/* End of tag */
	    fprintf(stderr, "SGML: found = but no value\n");
	    if (context->current_tag->name) start_element(context);
	    context->state = S_text;
	    break;
	    
	} else if (c=='\'') {
	    context->state = S_squoted;
	    break;

	} else if (c=='"') {
	    context->state = S_dquoted;
	    break;
	}
	HTChunkPutc(string, c);
	context->state = S_value;
	break;
	
    case S_value:
	if (WHITE(c) || (c=='>')) {		/* End of word */
	    HTChunkTerminate(string) ;
	    handle_attribute_value(context, string->data);
	    string->size = 0;
	    if (c=='>') {		/* End of tag */
		if (context->current_tag->name) start_element(context);
		context->state = S_text;
		break;
	    }
	    else context->state = S_tag_gap;
	} else {
	    HTChunkPutc(string, c);
	}
	break;
		
    case S_squoted:		/* Quoted attribute value */
	if (c=='\'') {		/* End of attribute value */
	    HTChunkTerminate(string) ;
	    handle_attribute_value(context, string->data);
	    string->size = 0;
	    context->state = S_tag_gap;
	} else {
	    HTChunkPutc(string, c);
	}
	break;
	
    case S_dquoted:		/* Quoted attribute value */
	if (c=='"') {		/* End of attribute value */
	    HTChunkTerminate(string) ;
	    handle_attribute_value(context, string->data);
	    string->size = 0;
	    context->state = S_tag_gap;
	} else {
	    HTChunkPutc(string, c);
	}
	break;
	
    case S_end:					/* </ */
	if (isalnum(c))
	    HTChunkPutc(string, c);
	else {				/* End of end tag name */
	    HTChunkTerminate(string) ;
	    if (c!='>') {
		if (TRACE) fprintf(stderr,"SGML:  `</%s%c' found!\n",
		    string->data, c);
		context->state = S_junk_tag;
		break;
	    }
	    for(context->current_tag = dtd->tags;
	    	context->current_tag->name; context->current_tag++) {
		if (0==strcasecomp(context->current_tag->name, string->data)) {
		    end_element( context, context->current_tag);
		    break;
		}
	    }
	    if (!context->current_tag->name) {
		if(TRACE) fprintf(stderr,
		    "Unknown end tag </%s>\n", string->data); 
	    }
	    string->size = 0;
	    context->current_attribute = (attr *) 0;
	    context->state = S_text;
	}
	break;

		
    case S_junk_tag:
	if (c=='>') {
	    context->state = S_text;
	}
	
    } /* switch on context->state */

}  /* SGML_character */
