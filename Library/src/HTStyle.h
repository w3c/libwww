/*

  					W3C Sample Code Library libwww Stylesheet Manager


!
  Stylesheet Manager
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

Styles allow the translation between a logical property of a piece of text
and its physical representation. A StyleSheet is a collection of styles,
defining the translation necessary to represent a document. It is a linked
list of styles.

A stylesheet is a collection of styles - these styles can either come from
over the net or they can be provided by the application as needed.

The manager doesn't contain any styles - it only provides a mechanism for
registering styles, looking them up and deleting them. That is, it doesn't
have any idea of what a style actually looks like - as this is highly application
dependent, it is not a good idea to provide this as common library code.

This module is implemented by HTStyle.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTSTYLE_H
#define HTSTYLE_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Creating and Deleting a StyleSheet
.

A Stylesheet is a container containing multiple individual styles. You must
therefore first create a stylesheet before you can create individual styles.
*/

typedef struct _HTStyleSheet HTStyleSheet;

extern HTStyleSheet * HTStyleSheet_new (const char * name);
extern BOOL HTStyleSheet_delete (HTStyleSheet * me);

/*
.
  Creating and Deleting Individual Styles
.
*/

typedef struct _HTStyle HTStyle;

extern HTStyle * HTStyle_new (const char * name, int element, void * context);
extern BOOL HTStyle_delete (HTStyle * me);

/*
.
  Adding and Deleting Individual styles from Stylesheet
.
*/

extern BOOL HTStyleSheet_addStyle (HTStyleSheet * me, HTStyle * style);
extern BOOL HTStyleSheet_deleteStyle (HTStyleSheet * me, HTStyle * style);

/*
.
  Searching for a Specific Style
.
*/

extern HTStyle * HTStyleSheet_findStyleWithName (HTStyleSheet * me, const char * name);
extern HTStyle * HTStyleSheet_findStyleForElement (HTStyleSheet * me, int element);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTSTYLE_H */

/*

  

  @(#) $Id$

*/
