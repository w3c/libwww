/*

  					W3C Sample Code Library libwww Internal Hypertext Object


!
  Internal implementation of Hypertext Object Builder
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

*/

#ifndef HTEXTIMP_H
#define HTEXTIMP_H

#include "HTReq.h"
#include "HTAnchor.h"
#include "HText.h"

/*
*/

typedef struct _HTextImp HTextImp;

extern HTextImp * HTextImp_new (
	HTRequest *	request,
	HTParentAnchor *anchor,
	HTStream *	output_stream);

extern BOOL HTextImp_delete (
	HTextImp *	me);

extern void HTextImp_build (
	HTextImp *	text,
	HTextStatus	status);

extern void HTextImp_addText (
	HTextImp * 	me,
	const char * 	buffer,
	int 		length);

extern void HTextImp_foundLink (
	HTextImp * 	me,
	int 		element_number,
	int 		attribute_number,
	HTChildAnchor *	anchor,
	const BOOL *	present,
	const char **	value);

extern void HTextImp_beginElement (
	HTextImp * 	me,
	int		element_number,
	const BOOL *	present,
	const char **	value);

extern void HTextImp_endElement (
	HTextImp * 	me,
	int		element_number);

extern void HTextImp_unparsedBeginElement (
	HTextImp * 	me,
	const char * 	buffer,
	int 		length);

extern void HTextImp_unparsedEndElement (
	HTextImp * 	me,
	const char * 	buffer,
	int 		length);

extern void HTextImp_unparsedEntity (
	HTextImp * 	me,
	const char * 	buffer,
	int 		length);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTEXTIMP_H */

/*

  

  @(#) $Id$

*/
