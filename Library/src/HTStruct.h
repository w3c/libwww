/*

  
  					W3C Sample Code Library libwww Structured Stream Class


!
  Structured Stream Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Structured stream class defines objects which accepts a structured
sequence of characters for eaxmple a SGML document. I'll rephrase that. A
structured object is am ordered tree-structured arrangement of data which
is representable as text. An example is the SGML parser
which outputs to a Structured Object. A Structured object can output its
contents to another Structured Object. It's a kind of typed
stream. The architecure is largely Dan Conolly's. Elements and entities are
passed to the sob by number, implying a knowledge of the DTD.

The Structured Stream is a subclass of a
Generic Stream Object. As always, we don't have
classes in basic C so we have to do this by hand!

This module is a part of the 
W3C Sample Code Library.
*/
#ifndef HTSTRUCT_H
#define HTSTRUCT_H

#include "HTStream.h"
#include "HTList.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
*/
typedef struct _HTStructured HTStructured;

typedef struct _HTStructuredClass {

    char * name;

    int (*flush)	(HTStructured * me);

    int (*_free)	(HTStructured *	me);

    int (*abort)	(HTStructured *	me, HTList * errorlist);

    int (*put_character)(HTStructured *	me, char ch);

    int (*put_string)	(HTStructured *	me, const char * str);

    int (*put_block)	(HTStructured * me, const char * str, int len);

/*

See the Generic Stream Definition for an explanation
of these methods. Note that they all have a HTStructured object
a the parameter, not a generic stream. This is to avoid incompatible
pointer warnings
*/

    void (*start_element)(HTStructured *me,
			  int		element_number,
			  const BOOL *	attribute_present,
			  const char **	attribute_value);

    void (*end_element)	(HTStructured *	me, int element_number);

    void (*put_entity)	(HTStructured *	me, int entity_number);

    int (*unparsed_begin_element)(HTStructured * me, const char * str, int len);

    int (*unparsed_end_element)(HTStructured * me, const char * str, int len);

    int (*unparsed_entity)(HTStructured * me, const char * str, int len);
		
} HTStructuredClass;

#ifdef __cplusplus
}
#endif

#endif  /* HTSTRUCT_H */

/*

  

  @(#) $Id$

*/
