/*							       	    HTStream.c
**	BASIC STREAMS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This version of the stream object just writes to a C file.
**	The file is assumed open and left open.
**
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTStream.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
};

PRIVATE HTStream HTBlackHoleStreamInstance;		      /* Made static */
PRIVATE HTStream HTErrorStreamInstance;

/* ------------------------------------------------------------------------- */

/*
**
**		B L A C K    H O L E    C L A S S
**
**	There is only one black hole instance shared by anyone
**	who wants a black hole.  These black holes don't radiate,
**	they just absorb data.
*/
PRIVATE int HTBlackHole_put_character (HTStream * me, char c)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_put_string (HTStream * me, const char * s)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_write (HTStream * me, const char * s, int l)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_flush (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_free (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_abort (HTStream * me, HTList * e)
{
    return HT_ERROR;
}


/*	Black Hole stream
**	-----------------
*/
PRIVATE const HTStreamClass HTBlackHoleClass =
{		
    "BlackHole",
    HTBlackHole_flush,
    HTBlackHole_free,
    HTBlackHole_abort,
    HTBlackHole_put_character,
    HTBlackHole_put_string,
    HTBlackHole_write
}; 

PUBLIC HTStream * HTBlackHole (void)
{
    HTBlackHoleStreamInstance.isa = &HTBlackHoleClass;      /* The rest is random */
    return &HTBlackHoleStreamInstance;
}

/*
**	ERROR STREAM
**	------------
**	There is only one error stream shared by anyone who wants a
**	generic error returned from all stream methods.
*/
PRIVATE int HTErrorStream_put_character (HTStream * me, char c)
{
    return HT_ERROR;
}

PRIVATE int HTErrorStream_put_string (HTStream * me, const char * s)
{
    return HT_ERROR;
}

PRIVATE int HTErrorStream_write (HTStream * me, const char * s, int l)
{
    return HT_ERROR;
}

PRIVATE int HTErrorStream_flush (HTStream * me)
{
    return HT_ERROR;
}

PRIVATE int HTErrorStream_free (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTErrorStream_abort (HTStream * me, HTList * e)
{
    return HT_ERROR;
}

PRIVATE const HTStreamClass HTErrorStreamClass =
{		
    "ErrorStream",
    HTErrorStream_flush,
    HTErrorStream_free,
    HTErrorStream_abort,
    HTErrorStream_put_character,
    HTErrorStream_put_string,
    HTErrorStream_write
}; 

PUBLIC HTStream * HTErrorStream (void)
{
    HTErrorStreamInstance.isa = &HTErrorStreamClass;    /* The rest is random */
    return &HTErrorStreamInstance;
}
