/*								     HTHeader.c
**	EXTRA HEADER MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
** HISTORY:
**	2 Nov 95 HFN	Written on a rainy night
*/

/* Library Include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTString.h"
#include "HTReq.h"
#include "HTHeader.h"					 /* Implemented here */

typedef struct _HTParser {
    char *		token;
    BOOL		case_sensitive;
    HTParserCallback	*Pcbf;
} HTParser;

PRIVATE HTList * HTParsers = NULL;
PRIVATE HTList * HTGenerators = NULL;

/* --------------------------------------------------------------------------*/

/*
**	Register a Header parser to be called
**	Tokens can contain a wildcard '*' which will match zero or more 
**	arbritary chars.
*/
PUBLIC BOOL HTParser_add (HTList *		parsers,
			  CONST char *       	token,
			  BOOL			case_sensitive,
			  HTParserCallback *	callback)
{
    if (token && callback) {
	HTParser *me;
	if ((me = (HTParser  *) HT_CALLOC(1, sizeof(HTParser))) == NULL)
	    HT_OUTOFMEM("HTParser_add");
	StrAllocCopy(me->token, token);
	me->case_sensitive = case_sensitive;
	me->Pcbf = callback;
	return HTList_addObject(parsers, (void *) me);
    }
    return NO;
}

/*
**	Unregister a Header parser
*/
PUBLIC BOOL HTParser_delete (HTList * parsers, CONST char * token)
{
    if (parsers) {
	HTList *cur = parsers;
	HTParser *pres;
	while ((pres = (HTParser *) HTList_nextObject(cur))) {
	    if (!strcmp(pres->token, token)) {
		HT_FREE(pres->token);
		return HTList_removeObject(parsers, (void *) pres);
	    }
	}
    }
    return NO;
}

/*
**	Delete the list of registered header parsers.
*/
PUBLIC BOOL HTParser_deleteAll (HTList * parsers)
{
    if (parsers) {
	HTList *cur = parsers;
	HTParser *pres;
	while ((pres = (HTParser *) HTList_nextObject(cur))) {
	    HT_FREE(pres->token);
	    HT_FREE(pres);
	}
	HTList_delete(parsers);
	parsers = NULL;
	return YES;
    }
    return NO;
}

/*
**	Search registered parsers to find suitable one for this token
**	If a parser isn't found, the function returns NULL
*/
PUBLIC HTParserCallback * HTParser_find (HTList *parsers, CONST char * token)
{
    HTList * cur = parsers;
    HTParser * pres;
    if (token && cur) {
	while ((pres = (HTParser *) HTList_nextObject(cur))) {
	    char *match = pres->case_sensitive ?
		HTStrCaseMatch(pres->token, token) : 
		    HTStrMatch(pres->token, token);
	    if (match) return pres->Pcbf;
	}
    }
    return NULL;
}

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
PUBLIC void HTHeader_setParser (HTList * list)
{
    HTParsers = list;
}

PUBLIC HTList * HTHeader_parser (void)
{
    return HTParsers;
}

PUBLIC BOOL HTHeader_addParser (CONST char * token, BOOL case_sensitive,
				HTParserCallback * callback)
{
    if (!HTParsers) HTParsers = HTList_new();
    return HTParser_add(HTParsers, token, case_sensitive, callback);
}

PUBLIC BOOL HTHeader_deleteParser (CONST char * token)
{
    if (!HTParsers) return NO;
    return HTParser_delete(HTParsers, token);
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
    if (HTParsers) {
	HTParser_deleteAll(HTParsers);
	HTParsers = NULL;
    }
    if (HTGenerators) {
	HTGenerator_deleteAll(HTGenerators);
	HTGenerators = NULL;
    }
}
