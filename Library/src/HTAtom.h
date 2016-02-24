/*

  
  					W3C Sample Code Library libwww Atom Class


!
  The Atom Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Atom Class defines strings which are given representative
pointer values so that they can be stored more efficiently, and comparisons
for equality done more efficiently. The list of atoms is stored
in a hash table, so when asking for a new atom you might in fact get back an
existing one.

Note: There are a whole bunch of
MIME-types defined as
atoms, so please use them!

This module is implemented by HTAtom.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTATOM_H
#define HTATOM_H

#include "HTList.h"

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTAtom HTAtom;
struct _HTAtom {
	HTAtom *	next;
	char *		name;
}; /* struct _HTAtom */

/*
(
  Get an Atom
)

This function returns a representative value (an atom) such
that it will always (within one run of the program) return the same value
for the same given string. The former is case sensitive, the latter is case
insensitive.
*/

extern HTAtom * HTAtom_for	(const char * string);
extern HTAtom * HTAtom_caseFor	(const char * string);

/*
(
  Get Content of an Atom
)
*/

#define HTAtom_name(a) ((a) ? (a)->name : NULL)

/*

This macro returns the string pointed to by the atom.
(
  Search For Atoms
)

Returns a list of atoms which matches the template given. It
is especially made for MIME-types so that for example a template like
text<slash><star> returns a list of all MIME-types
of type text.
*/

extern HTList * HTAtom_templateMatches (const char * templ);

/*
(
  Cleanup Memory
)

In order to cleanup memory, call this function. This is done automaticly
from the HTLibTerminate function.
*/

extern void HTAtom_deleteAll (void);

#ifdef __cplusplus
}
#endif

#endif

/*

  

  @(#) $Id$

*/
