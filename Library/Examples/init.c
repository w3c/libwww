/*
**	@(#) $Id$
**	
**	Other libwww samples can be found at "http://www.w3.org/Library/Examples"
**	
**	Copyright (c© 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**      Issues a range request to an HTTP server and saves the result in a chunk
*/
#include "WWWLib.h"
#include "WWWHTTP.h"
#include "WWWInit.h"

int main()
{
    HTList * converters = HTList_new();		/* List of converters */
    HTList * encodings = HTList_new();		/* List of encoders */

    /* Set up the application's event loop. We use the 
       example event loop that comes with the Library.  */
    HTEventInit();

    /* Initialize libwww core */
    HTLibInit("TestApp", "1.0");

    /* Turn on TRACE so we can see what is going on */
    HTSetTraceMessageMask("sop");

    /* Register the default set of transport protocols */
    HTTransportInit();

    /* Register the default set of protocol modules */
    HTProtocolInit();

    /* Register the default set of BEFORE and AFTER filters */
    HTNetInit();

    /* Register the default set of converters */
    HTConverterInit(converters);
    HTFormat_setConversion(converters);

    /* Register the default set of transfer encoders and decoders */
    HTTransferEncoderInit(encodings);
    HTFormat_setTransferCoding(encodings);

    /* Register the default set of file suffix bindings */
    HTFileInit();

    /* Register the default set of MIME header parsers */
    HTMIMEInit();

    /* Register the default set of Icons for directory listings */
    HTIconInit(NULL);

    /* Register the default set of messages and dialog functions */
    HTAlertInit();

    /* Terminate the Library */
    HTLibTerminate();
    return 0;
}
