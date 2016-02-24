/*

  
  					W3C Sample Code Library libwww SGML Parser


!
  SGML Parser
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The SGML parser is a state machine. It is called for every character of the
input stream. The DTD data structure contains pointers to functions which
are called to implement the actual effect of the text read. When these functions
are called, the attribute structures pointed to by the DTD are valid, and
the function is parsed a pointer to the curent tag structure, and an "element
stack" which represents the state of nesting within SGML elements. See also
the the generic Stream definition

The following aspects are from Dan Connolly's suggestions: Binary search,
Strcutured object scheme basically, SGML content enum type.

This module is implemented by SGML.c, and it is a part
of the W3C Sample Code Library.
*/

#ifndef SGML_H
#define SGML_H

#include "HTStream.h"
#include "HTStruct.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  SGML Content Types
.
*/

typedef enum _SGMLContent{
    SGML_EMPTY,    	/* no content */
    SGML_LITERAL, 	/* character data. Recognized exact close tag only.
  		    	   Old www server compatibility only! Not SGML */
    SGML_CDATA,    	/* character data. recognize </ only */
    SGML_RCDATA,   	/* replaceable character data. recognize </ and &ref; */
    SGML_MIXED,    	/* elements and parsed character data. recognize all markup */
    SGML_ELEMENT   	/* any data found will be returned as an error*/
} SGMLContent;

/*
.
  Attribute Types
.

Describes the SGML tag attribute
*/

typedef struct _HTAttr {
    char *	name;		/* The (constant) name of the attribute */
    				/* Could put type info in here */
} HTAttr;

extern char * HTAttr_name (HTAttr * attr);

/*
.
  Tag Structure Describing SGML Elements
.

  
    name
  
    is the string which comes after the tag opener "<".
  
    attributes
  
    points to a zero-terminated array of attribute names.
  
    litteral
  
    determines how the SGML engine parses the charaters within the element. If
    set, tag openers are ignored except for that which opens a matching closing
    tag.

*/

typedef struct _HTTag {
    char *      name;			/* The name of the tag */
    HTAttr *	attributes;		/* The list of acceptable attributes */
    int		number_of_attributes;	/* Number of possible attributes */
    SGMLContent contents;		/* End only on end tag @@ */		
} HTTag;

extern char * HTTag_name (HTTag * tag);
extern SGMLContent HTTag_content (HTTag * tag);
extern int HTTag_attributes (HTTag * tag);
extern char * HTTag_attributeName (HTTag * tag, int attribute_number);

/*
.
  DTD Information
.

Not the whole DTD, but all this parser uses of it.
*/

#define MAX_ATTRIBUTES 32	     /* Max number of attributes per element */

typedef struct {
    HTTag *		tags;		/* Must be in strcmp order by name */ 
    int			number_of_tags;
    const char **	entity_names;	/* Must be in strcmp order by name */
    int			number_of_entities;
} SGML_dtd;

extern HTTag * SGML_findTag (SGML_dtd * dtd, int element_number);
extern char * SGML_findTagName (SGML_dtd * dtd, int element_number);
extern SGMLContent SGML_findTagContents (SGML_dtd * dtd, int element_number);
extern int SGML_findElementNumber(SGML_dtd *dtd, char *name_element);

/*
.
  Create an SGML Parser Instance
.

Create an SGML parser instance which converts a stream to a structured stream.
*/

extern HTStream * SGML_new (const SGML_dtd * 	dtd,
			    HTStructured *	target);

/*
*/

#ifdef __cplusplus
}
#endif

#endif	/* SGML_H */

/*

  

  @(#) $Id$

*/
