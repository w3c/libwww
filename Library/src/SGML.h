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
**
**      Dan suggested	- Binary search
**			- Strcutured object scheme basically
**			- SGML content enum type
*/
/*	(c) Copyright CERN 1991 - See Copyright.html
*/

#ifndef SGML_H
#define SGML_H

#include "HTUtils.h"
#include "HTStream.h"

/*
 * SGML content types				DC
 */

typedef enum _SGMLContent{
  SGML_EMPTY,    /* no content */
  SGML_LITTERAL, /* character data. Recognised excat close tag only. litteral
  		    Old www server compatibility only! Not SGML */
  SGML_CDATA,    /* character data. recognize </ only */
  SGML_RCDATA,   /* replaceable character data. recognize </ and &ref; */
  SGML_MIXED,    /* elements and parsed character data. recognize all markup */
  SGML_ELEMENT   /* any data found will be returned as an error*/
  } SGMLContent;


typedef struct {
    char *	name;		/* The (constant) name of the attribute */
    				/* Could put type info in here */
} attr;


/*		A tag structure describes an SGML element.
**		-----------------------------------------
**
**
**	name		is the string which comes after the tag opener "<".
**
**	attributes	points to a zero-terminated array
**			of attribute names.
**
**	litteral	determines how the SGML engine parses the charaters
**			within the element. If set, tag openers are ignored
**			except for that which opens a matching closing tag.
**
*/
typedef struct _tag HTTag;
struct _tag{
    char * 	name;			/* The name of the tag */
    attr * 	attributes;		/* The list of acceptable attributes */
    int		number_of_attributes;	/* Number of possible attributes */
    SGMLContent contents;		/* End only on end tag @@ */		
};




/*		DTD Information
**		---------------
**
** Not the whole DTD, but all this parser usues of it.
*/
typedef struct {
    HTTag *	tags;			/* Must be in strcmp order by name */ 
    int		number_of_tags;
    char **	entity_names;		/* Must be in strcmp order by name */
    int		number_of_entities;
} SGML_dtd;


/*	SGML context passed to parsers
*/
typedef struct _HTSGMLContext *HTSGMLContext;	/* Hidden */


/*__________________________________________________________________________
*/
/*		Structured Object definition
**
**	A structured object is something which can reasonably be
**	represented in SGML.  I'll rephrase that.  A structured
**	object is am ordered tree-structured arrangement of data
**	which is representable as text.
**
**	The SGML parer outputs to a Structured object. 
**	A Structured object can output its contents
**	to another Structured Object. 
**	It's a kind of typed stream.  The architecure
**	is largely Dan Conolly's.
**	Elements and entities are passed to the sob by number, implying
**	a knowledge of the DTD.
**	Knowledge of the SGML syntax is not here, though.
**
**	Superclass: HTStream
*/


/* 	The creation methods will vary on the type of Structured Object.
**	Maybe the callerData is enough info to pass along.
*/

typedef struct _HTStructured HTStructured;

typedef struct _HTStructuredClass{

	char*  name;				/* Just for diagnostics */

	void (*free) PARAMS((
		HTStructured*	me));

	void (*end_document) PARAMS((
		HTStructured*	me));
		
	void (*put_character) PARAMS((
		HTStructured*	me,
		char		ch));
				
	void (*put_string) PARAMS((
		HTStructured*	me,
		CONST char *	str));
		
	void (*write) PARAMS((
		HTStream*	me,
		CONST char *	str,
		int		len));
		
	void (*start_element) PARAMS((
		HTStructured*	me,
		int		element_number,
		BOOL*		attribute_present,
		char**		attribute_value));
		
	void (*end_element) PARAMS((
		HTStructured*	me,
		int		element_number));

	void (*put_entity) PARAMS((
		HTStructured*	me,
		int		entity_number));
		
}HTStructuredClass;



/*	Create an SGML parser
**
** On entry,
**	dtd		must point to a DTD structure as defined above
**	callbacks	must point to user routines.
**	callData	is returned in callbacks transparently.
** On exit,
**		The default tag starter has been processed.
*/


extern HTStream* SGML_new PARAMS((
	CONST SGML_dtd * 		dtd,
	HTStructured *		target));

extern HTStreamClass SGMLParser;





/*______________________________________________________________________
obsolete
*/
#ifdef SUPPRESS

/*	Crank the SGML parser
**
** On entry,
**   context	must be a valid return from SGML_begin
**
**   c		is the next character of the input stream
**
** On exit,
**	return	value is a valid SGML context for the other routines
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
extern void  SGML_setCallerData PARAMS((HTSGMLContext context, void* data));
#endif /* obsolete */

#endif	/* SGML_H */
