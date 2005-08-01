/*						     HTHeader.c
**	EXTRA HEADER MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** HISTORY:
**	2 Nov 95 HFN	Written on a rainy night
*/

/* Library Include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTString.h"
#include "HTReq.h"
#include "HTHeader.h"					 /* Implemented here */
#include "HTMIMPrs.h"

HTMIMEParseSet * ParseSet = NULL;
PRIVATE HTList * HTGenerators = NULL;

/* --------------------------------------------------------------------------*/

/*
**	Register a Header generator to be called when we make request
*/
PUBLIC BOOL HTGenerator_add (HTList * gens, HTPostCallback * callback)
{
    return callback ? HTList_addObject(gens, (void *) callback) : NO;
}

/*
**	Unregister a Header generator
*/
PUBLIC BOOL HTGenerator_delete (HTList * gens, HTPostCallback * callback)
{
    return callback ? HTList_removeObject(gens, (void *) callback) : NO;
}

/*
**	Delete the list of registered header generators.
*/
PUBLIC BOOL HTGenerator_deleteAll (HTList * gens)
{
    return HTList_delete(gens);
}

/* ------------------------------------------------------------------------- */
/* 		     GLOBAL LIST OF PARSERS AND GENERATORS		     */
/* ------------------------------------------------------------------------- */

/*
**	Global List of parsers. list can be NULL
*/
PUBLIC void HTHeader_setMIMEParseSet (HTMIMEParseSet * list)
{
    ParseSet = list;
}

PUBLIC HTMIMEParseSet * HTHeader_MIMEParseSet (void)
{
    return ParseSet;
}

PUBLIC BOOL HTHeader_addParser (const char * token, BOOL case_sensitive,
				HTParserCallback * callback)
{
    if (!ParseSet) ParseSet = HTMIMEParseSet_new(MIME_HASH_SIZE);
    else HTHeader_deleteParser(token); /* Remove duplicates */
    return (HTMIMEParseSet_add(ParseSet, token, case_sensitive, callback) != NULL);
}

PUBLIC BOOL HTHeader_addRegexParser (const char * token, BOOL case_sensitive,
				     HTParserCallback * callback)
{
    if (!ParseSet) ParseSet = HTMIMEParseSet_new(MIME_HASH_SIZE);
    return (HTMIMEParseSet_addRegex(ParseSet, token, case_sensitive, callback) != NULL);
}

PUBLIC BOOL HTHeader_deleteParser (const char * token)
{
    return HTMIMEParseSet_delete(ParseSet, token);
}

/*
**	Global List of header generators. list can be NULL
*/
PUBLIC void HTHeader_setGenerator (HTList * list)
{
    HTGenerators = list;
}

PUBLIC HTList * HTHeader_generator (void)
{
    return HTGenerators;
}

PUBLIC BOOL HTHeader_addGenerator (HTPostCallback  * callback)
{
    if (!HTGenerators) HTGenerators = HTList_new();
    return HTGenerator_add(HTGenerators, callback);
}

PUBLIC BOOL HTHeader_deleteGenerator (HTPostCallback  * callback)
{
    if (!HTGenerators) return NO;
    return HTGenerator_delete(HTGenerators, callback);
}

/*
**	Convenience function to clean up
*/
PUBLIC void HTHeader_deleteAll (void)
{
    HTMIMEParseSet_deleteAll(ParseSet);
    if (HTGenerators) {
	HTGenerator_deleteAll(HTGenerators);
	HTGenerators = NULL;
    }
}

