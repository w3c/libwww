/*		Manage different file formats			HTFormat.c
**		=============================
**
*/
#ifndef HTFORMAT_H
#define HTFORMAT_H

#include "HTUtils.h"
#include "HTStream.h"
#include "HTAtom.h"

#ifdef SHORT_NAMES
#define HTOutputSource HTOuSour
#define HTOutputBinary HTOuBina
#endif

typedef HTAtom * HTFormat;
						/* Internal ones */
#define WWW_SOURCE HTAtom_for("www/source")	/* Whatever it was originally*/
#define WWW_PRESENT HTAtom_for("www/present")	/* The user's perception */
#define WWW_TERMINAL HTAtom_for("www/terminal") /* On a terminal (?) */
#define WWW_MIME HTAtom_for("www/mime")		/* A MIME message */
#define WWW_PRINT HTAtom_for("www/print")	/* A printed copy */

#define WWW_PLAINTEXT 	HTAtom_for("text/plain")
#define WWW_POSTSCRIPT 	HTAtom_for("application/postscript")
#define WWW_RICHTEXT 	HTAtom_for("application/rtf")
#define WWW_HTML 	HTAtom_for("text/html")
#define WWW_BINARY 	HTAtom_for("application/binary")

/* After defining HTFormat: */

#include "HTAnchor.h"

typedef struct _HTPresentation HTPresentation;

typedef HTStream * (*HTConverter) PARAMS((
	HTPresentation *	pres,
	HTParentAnchor *	anchor,
	HTStream *		sink));
	
struct _HTPresentation {
	HTAtom* rep;		/* representation name atmoized */
	HTAtom* rep_out;	/* resulting representation */
	HTConverter converter;	/* The routine to gen the stream stack */
	char *  command;	/* MIME-format string */
	float	quality;	/* Between 0 (bad) and 1 (good) */
	float   secs;
	float   secs_per_byte;
};

extern HTList * HTPresentations;

/*	Initialize a standard set of formats
**	------------------------------------
*/

extern void HTFormatInit  NOPARAMS;


/*	Define a system command to present a representation
**	---------------------------------------------------
**
** On entry,
**	rep	is the MIME - style format name
**	command is the MAILCAP - style command template
**	quality		A degradation faction 0..1
**	maxbytes	A limit on the length acceptable as input (0 infinite)
**	maxsecs		A limit on the time user will wait (0 for infinity)
*/

extern void HTSetPresentation PARAMS((
	CONST char * representation,
	CONST char * command,
	float	quality,
	float	secs, 
	float	secs_per_byte
));


/*	Define a system command to present a representation
**	---------------------------------------------------
**
** On entry,
**	rep_in		is the content-type input
**	rep_out		is the resulting content-type
**	converter	is the routine to make the stream to do it
*/
extern void HTSetConversion PARAMS((
	CONST char * 	rep_in,
	CONST char * 	rep_out,
	HTConverter	converter,
	float		quality,
	float		secs, 
	float		secs_per_byte
));


/*	Create a stack of streams
**	-------------------------
*/
extern HTStream * HTStreamStack PARAMS((
	HTFormat		format_in,
	HTFormat		rep_out,
	HTStream*		stream_out,
	HTParentAnchor*		anchor));

/*		Find the cost of a filter stack
**		-------------------------------
**
**	Must return the cost of the same stack which
**	HTStreamStack would set up.
**
** On entry,
**	length	The size of the data to be converted
*/
extern float HTStackValue PARAMS((
	HTFormat		format_in,
	HTFormat		rep_out,
	float			initial_value,
	long int		length));

#define NO_VALUE_FOUND	-1e20		/* returned if none found */

/*	Copy a socket to a stream
**	-------------------------
*/
extern void HTCopy PARAMS((
	int			file_number,
	HTStream*		sink));

	
/*	Copy an ASCII telnet socket to a stream
**	---------------------------------------
**
** 	This one strips CR characters. It is slower.
*/
extern void HTCopyNoCR PARAMS((
	int			file_number,
	HTStream*		sink));


/*	Clear input buffer and set file number
*/
extern void HTInitInput PARAMS((int file_number));

/*	Get next character from buffer
*/
extern char HTGetChararcter NOPARAMS;


/*	Parse a socket given its format
**	-------------------------------
**
**	Returns HT_LOADED if succesful, <0 if error.
*/
extern int HTParseSocket PARAMS((
	HTFormat	format_in,
	HTFormat	format_out,
	HTParentAnchor	*anchor,
	int 		file_number,
	HTStream*	sink));

extern BOOL HTOutputSource;	/* Flag: shortcut parser */
#endif
