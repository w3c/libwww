/*                                            HTFormat: The format manager in the WWW Library
                            MANAGE DIFFERENT DOCUMENT FORMATS
                                             
   Here we describe the functions of the HTFormat module which handles conversion between
   different data representations.  (In MIME parlance, a representation is known as a
   content-type. In WWW  the term "format" is often used as it is shorter).
   
   This module is implemented by HTFormat.c. This hypertext document is used to generate
   the HTFormat.h inlude file.
   
Preamble

 */
#ifndef HTFORMAT_H
#define HTFORMAT_H

#include "HTUtils.h"
#include "HTStream.h"
#include "HTAtom.h"
#include "HTList.h"

#ifdef SHORT_NAMES
#define HTOutputSource HTOuSour
#define HTOutputBinary HTOuBina
#endif

/*

The HTFormat type

   We use the HTAtom object for holding representations. This allows faster manipulation
   (comparison and copying) that if we stayed with strings.
   
 */
typedef HTAtom * HTFormat;
                        
/*

   These macros (which used to be constants) define some basic internally referenced
   representations.  The www/xxx ones are of course not MIME standard.
   
   www/source  is an output format which leaves the input untouched. It is useful for
   diagnostics, and for users who want to see the original, whatever it is.
   
 */
                        /* Internal ones */
#define WWW_SOURCE HTAtom_for("www/source")     /* Whatever it was originally*/

/*

   www/present represents the user's perception of the document.  If you convert to
   www/present, you present the material to the user.
   
 */
#define WWW_PRESENT HTAtom_for("www/present")   /* The user's perception */

/*

   The message/rfc822 format means a MIME message or a plain text message with no MIME
   header. This is what is returned by an HTTP server.
   
 */
#define WWW_MIME HTAtom_for("www/mime")         /* A MIME message */
/*

   www/print is like www/present except it represents a printed copy.
   
 */
#define WWW_PRINT HTAtom_for("www/print")       /* A printed copy */

#define WWW_PLAINTEXT   HTAtom_for("text/plain")
#define WWW_POSTSCRIPT  HTAtom_for("application/postscript")
#define WWW_RICHTEXT    HTAtom_for("application/rtf")
#define WWW_HTML        HTAtom_for("text/html")
#define WWW_BINARY      HTAtom_for("application/binary")
/*

   We must include the following file after defining HTFormat, to which it makes
   reference.
   
 */
#include "HTAnchor.h"

/*

The HTPresentation and HTConverter types

   This HTPresentation structure represents a possible conversion algorithm from one
   format to annother.  It includes a pointer to a conversion routine. The conversion
   routine returns a stream to which data should be fed. See also HTStreamStack which
   scans the list of registered converters and calls one. See the initialisation module
   for a list of conversion routines.
   
 */
typedef struct _HTPresentation HTPresentation;

typedef HTStream * HTConverter PARAMS((
        HTPresentation *        pres,
        HTParentAnchor *        anchor,
        HTStream *              sink));
        
struct _HTPresentation {
        HTAtom* rep;            /* representation name atmoized */
        HTAtom* rep_out;        /* resulting representation */
        HTConverter *converter; /* The routine to gen the stream stack */
        char *  command;        /* MIME-format string */
        float   quality;        /* Between 0 (bad) and 1 (good) */
        float   secs;
        float   secs_per_byte;
};

/*

   The list of presentations is kept by this module.  It is also scanned by modules which
   want to know the set of formats supported. for example.
   
 */
extern HTList * HTPresentations;

/*

HTSetPresentation: Register a system command to present a format

On entry,

  rep                     is the MIME - style format name
                         
  command                 is the MAILCAP - style command template
                         
  quality                 A degradation faction 0..1
                         
  maxbytes                A limit on the length acceptable as input (0 infinite)
                         
  maxsecs                 A limit on the time user will wait (0 for infinity)
                         
 */
extern void HTSetPresentation PARAMS((
        CONST char * representation,
        CONST char * command,
        float   quality,
        float   secs,
        float   secs_per_byte
));


/*

HTSetConversion:   Register a converstion routine

On entry,

  rep_in                  is the content-type input
                         
  rep_out                 is the resulting content-type
                         
  converter               is the routine to make the stream to do it
                         
 */

extern void HTSetConversion PARAMS((
        CONST char *    rep_in,
        CONST char *    rep_out,
        HTConverter *   converter,
        float           quality,
        float           secs,
        float           secs_per_byte
));


/*

HTStreamStack:   Create a stack of streams

   This is the routine which actually sets up the conversion. It currently checks only for
   direct conversions, but indirect conversions are forseen. It takes a stream into which
   the output should be sent in the final format, builds the conversion stack, and returns
   a stream into which the data in the input format should be fed.  The anchor is passed
   because hypertxet objects load information into the anchor object which represents
   them.
   
 */
extern HTStream * HTStreamStack PARAMS((
        HTFormat                format_in,
        HTFormat                format_out,
        HTStream*               stream_out,
        HTParentAnchor*         anchor));

/*

HTStackValue: Find the cost of a filter stack

   Must return the cost of the same stack which HTStreamStack would set up.
   
On entry,

  format_in               The fomat of the data to be converted
                         
  format_out              The format required
                         
  initial_value           The intrinsic "value" of the data before conversion on a scale
                         from 0 to 1
                         
  length                  The number of bytes expected in the input format
                         
 */
extern float HTStackValue PARAMS((
        HTFormat                format_in,
        HTFormat                rep_out,
        float                   initial_value,
        long int                length));

#define NO_VALUE_FOUND  -1e20           /* returned if none found */

/*

HTCopy:  Copy a socket to a stream

   This is used by the protocol engines to send data down a stream, typiclly one which has
   been generated by HTStreamStack.
   
 */
extern void HTCopy PARAMS((
        int                     file_number,
        HTStream*               sink));

        
/*

   HTCopyNoCRThis one strips CR characters. It is slower than HTCopy .
   
 */

extern void HTCopyNoCR PARAMS((
        int                     file_number,
        HTStream*               sink));


/*

Clear input buffer and set file number

   This routine and the one below provide simple character input from sockets. (They are
   left over from the older architecure and may not be used very much.)  The existence of
   a common routine and buffer saves memory space in small implementations.
   
 */
extern void HTInitInput PARAMS((int file_number));

/*

Get next character from buffer

 */
extern char HTGetChararcter NOPARAMS;


/*

HTParseSocket: Parse a socket given its format

   This routine is called by protocol modules to load an object.  uses HTStreamStack and
   the copy routines above.  Returns HT_LOADED if succesful, &lt;0 if not.
   
 */
extern int HTParseSocket PARAMS((
        HTFormat        format_in,
        HTFormat        format_out,
        HTParentAnchor  *anchor,
        int             file_number,
        HTStream*       sink));

/*

Epilogue

 */
extern BOOL HTOutputSource;     /* Flag: shortcut parser */
#endif

/*

   end  */
