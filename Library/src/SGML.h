/*		SGML Parser definitions				SGML.h
**		=======================
**
**	The SGML parser is a state machine. It is called for every character
**	of the input stream. The DTD data structure contains pointers
**	to functions which are called to implement the actual effect of the
**	text read. When these functions are called, the attribute structures
**	pointed to by the DTD are valid, and the function is passed
**	a pointer to the curent tag structure, and an
**	"element stack" which represents the state of nesting within SGML
**	elements.
*/
/*	(c) Copyright CERN 1991 - See Copyright.html
*/
#include "HTUtils.h"

#ifndef SGML_H
#define SGML_H

typedef struct {
    char *	name;		/* The (constant) name of the attribute */
    BOOL	present;	/* Was attribute specified for this tag? */
    char *	value;		/* Malloced. Valid only if present is YES. */
} attr;


/*		A tag structure describes an SGML element.
**		-----------------------------------------
**
**	If the tag has an end tag, then treat and end must be non-zero.
**	If the tag does not, they must be zero.
**
**	name		is the string which comes after the tag opener "<".
**
**	attributes	points to a zero-terminated array
**			of attribute structures filled in by the parser.
**
**	style		is a pointer left for use by oter modules (typically
**		 	for storing the style used with this element)
**
**	litteral	determines how the SGML engine parses the charaters
**			within the element. If set, tag openers are ignored
**			except for that which opens a matching closing tag.
**
**	begin		is called when the tag has been parsed, and the
**			attribute values set. The element pointer points
**			to the top of the new element stack.
**
**	treat		is a pointer to a function which is called for
**			each text character within the element.
**
**	end		is called when the end tag has been parsed.
**			The element pointer points to the top of the
**			new element stack.
*/
typedef struct _tag HTTag;
typedef struct _HTElement HTElement;
struct _tag{
    char * name;		/* The name of the tag */
    attr * attributes;		/* The list of acceptable attributes */
    void * style;		/* Not used by the SGML engine */
    BOOL  litteral;		/* End only on end tag @@ */		
    void (*begin) PARAMS((
    	void* d, HTTag * t, HTElement * e));	/* Action on begin tag */
    void (*treat) PARAMS((
    	void* d, char c));	/* Action when character is parsed */
    void (*end) PARAMS((
    	void* d, HTTag * t, HTElement * e)); /* Action on </tag> or 0 if empty */
};

/*	Stack of previous tags:
**
**	This allows us to return down the stack reselcting styles.
**	As we return, attribute values will be garbage in general.
*/
struct _HTElement {
	HTElement *	next;	/* Previously nested element or 0 */
	HTTag	*	tag;	/* The tag at this level */
	void *		info;	/* Free to be used by caller only */
};


typedef struct _entity {
    char * name;
    char * representation;
} entity;

typedef struct {
    HTTag *	tags;
    HTTag *	default_tag;
    entity *	entities;
} SGML_dtd;


/*	SGML context passed to parsers
*/
typedef struct _HTSGMLContext *HTSGMLContext;


/*	Initialise the SGML parser
**
** On entry,
**	dtd	must point to a DTD structure as defined above
** On exit,
**		The default tag starter has been processed.
*/

extern HTSGMLContext SGML_begin PARAMS((SGML_dtd * dtd));


/*	Crank the SGML parser
**
** On entry,
**   context	must be a valid return from SGML_begin
**
**   c		is the next character of the input stream
**
** On exit,
**	return	value is a valid SGML context for the oetr routines
*/

extern void  SGML_character PARAMS((HTSGMLContext context, char c));
extern void  SGML_string PARAMS((HTSGMLContext context, char *s));


/*	Finish the SGML parser
**
** On entry,
**   context	must be a valid return from SGML_begin
**
** On exit,
**   context	this value will no longer be valid.
**		The document will be completed.
**		SGML_begin must be called again before any
**		more parsing is done.
*/

extern void  SGML_end PARAMS((HTSGMLContext context));

/*	Read and write user callback handle
**	-----------------------------------
**
**   The callbacks from the SGML parser have an SGML context parameter.
**   These calls allow the caller to associate his own context with a
**   particular SGML context.
*/

extern void* SGML_callerData PARAMS((HTSGMLContext context));
extern void SGML_setCallerData PARAMS((HTSGMLContext context, void* data));


#endif	/* SGML_H */
