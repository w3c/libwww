/*		Manage different file formats			HTFormat.c
**		=============================
**
*/
#ifndef HTFORMAT_H
#define HTFORMAT_H

#include "HTUtils.h"

#ifdef SHORT_NAMES
#define HTOutputSource HTOuSour
#define HTOutputBinary HTOuBina
#endif

typedef int HTFormat;
				/* Can take the following values: */
#define WWW_INVALID   (-1)
#define WWW_SOURCE 	0	/* Whatever it was			*/
#define WWW_PLAINTEXT	1	/* plain ISO latin (ASCII)		*/
#define WWW_POSTSCRIPT	2	/* Postscript - encapsulated?		*/
#define	WWW_RICHTEXT	3	/* Microsoft RTF transfer format	*/
#define WWW_HTML	4	/* WWW HyperText Markup Language	*/
#define WWW_BINARY	5	/* Otherwise Unknown binary format */

#include "HTAnchor.h"


/*	Clear input buffer and set file number
*/
extern void HTInitInput PARAMS((int file_number));

/*	Get next character from buffer
*/
extern char HTGetChararcter NOPARAMS;


/*	Parse a file given its format
*/
extern void HTParseFormat PARAMS((
	HTFormat	format,
	HTParentAnchor	*anchor,
	int 		file_number));

extern BOOL HTOutputSource;	/* Flag: shortcut parser */
#endif
