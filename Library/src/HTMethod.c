/*								     HTMethod.c
**	MANAGES REQUEST METHODS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**
** HISTORY:
**	6 June 95  HFN	Spawned off from HTAccess. Can be extended to allow
**			registration of new methods
*/

/* Library Include files */
#include "sysdep.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTMethod.h"					 /* Implemented here */

PRIVATE char *method_names[] =
{
    "INVALID-METHOD",
    "GET",
    "HEAD",
    "POST",
    "PUT",
    "PATCH",
    "DELETE",
    "TRACE",
    "OPTIONS",
    "LINK",
    "UNLINK",
    NULL
};

/* ------------------------------------------------------------------------- */

/*	Get method enum value
**	---------------------
*/
PUBLIC HTMethod HTMethod_enum (const char * name)
{
    if (name) {
	if (!strcasecomp(name, *(method_names+1)))
	    return METHOD_GET;
	else if (!strcasecomp(name, *(method_names+2)))
	    return METHOD_HEAD;
	else if (!strcasecomp(name, *(method_names+3)))
	    return METHOD_POST;
	else if (!strcasecomp(name, *(method_names+4)))
	    return METHOD_PUT;
	else if (!strcasecomp(name, *(method_names+5)))
	    return METHOD_PATCH;
	else if (!strcasecomp(name, *(method_names+6)))
	    return METHOD_DELETE;
	else if (!strcasecomp(name, *(method_names+7)))
	    return METHOD_TRACE;
	else if (!strcasecomp(name, *(method_names+8)))
	    return METHOD_OPTIONS;
	else if (!strcasecomp(name, *(method_names+9)))
	    return METHOD_LINK;
	else if (!strcasecomp(name, *(method_names+10)))
	    return METHOD_UNLINK;
    }
    return METHOD_INVALID;
}


/*	Get method name
**	---------------
**	Returns pointer to entry in static table in memory
*/
PUBLIC const char * HTMethod_name (HTMethod method)
{
    if (method & METHOD_GET)
	return *(method_names+1);
    else if (method == METHOD_HEAD)
	return *(method_names+2);
    else if (method == METHOD_POST)
	return *(method_names+3);
    else if (method == METHOD_PUT)
	return *(method_names+4);
    else if (method == METHOD_PATCH)
	return *(method_names+5);
    else if (method == METHOD_DELETE)
	return *(method_names+6);
    else if (method == METHOD_TRACE)
	return *(method_names+7);
    else if (method == METHOD_OPTIONS)
	return *(method_names+8);
    else if (method == METHOD_LINK)
	return *(method_names+9);
    else if (method == METHOD_UNLINK)
	return *(method_names+10);
    else
	return *method_names;
}

