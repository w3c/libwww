/*								       HTAtom.c
**	HTHashList: hash indexed array of MIME header parsers
**
**	(c) COPYRIGHT MIT 1996.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**      See HTHshLst.html for a description of HashLists.
**
** Authors:
**	EGP	Eric Prud'hommeaux w3
**
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTMIMPrs.h"

struct _HTMIMEParseEl{
    HTMIMEParseEl * 	next;
    char *		token;
    BOOL		caseSensitive;
    HTParserCallback * 	pFunk;
};

PRIVATE HTMIMEParseEl * HTMIMEParseEl_new(HTMIMEParseEl ** pBefore, 
					  const char * token, 
					  BOOL caseSensitive, 
					  HTParserCallback * callback)
{
    HTMIMEParseEl * ret;
    if ((ret = (HTMIMEParseEl *) HT_MALLOC(sizeof(HTMIMEParseEl))) == NULL)
        HT_OUTOFMEM("HTMIMEParseEl");
    ret->next = *pBefore;
    *pBefore = ret;
    if ((ret->token = (char *) HT_MALLOC(strlen(token)+1)) == NULL)
        HT_OUTOFMEM("token");
    strcpy((char *)ret->token, token);
    ret->caseSensitive = caseSensitive;
    ret->pFunk = callback;
    
    return ret;
}

PRIVATE int HTMIMEParseEl_delete(HTMIMEParseEl * me, HTMIMEParseEl ** pBefore)
{
    *pBefore = me->next;
    HT_FREE(me->token);
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int HTMIMEParseSet_hash(HTMIMEParseSet * me, const char * token)
{
    int ret;
    const char * p;

    for (p=token, ret=0; *p; p++) {
        char ch;
        ch = *(unsigned char *) p;
	ch = TOLOWER(ch);
        ret = (ret * 3 +(ch)) % me->size;
    }
    return ret;
}

PUBLIC HTMIMEParseSet * HTMIMEParseSet_new(int hashSize)
{
    HTMIMEParseSet * me;

    if ((me = (HTMIMEParseSet *) HT_CALLOC(1, sizeof(HTMIMEParseSet))) == NULL)
        HT_OUTOFMEM("HTMIMEParseSet");
    me->size = hashSize;
    return me;
}

PUBLIC int HTMIMEParseSet_deleteAll (HTMIMEParseSet * me)
{
    int i;
    HTMIMEParseEl * pEl, * next;

    if (me && me->parsers) {
	for (i=0; i<me->size; i++) {
	    for (pEl = me->parsers[i]; pEl; pEl = next) {
		next = pEl->next;
		HT_FREE(pEl->token);
		HT_FREE(pEl);
	    }
	}
	HT_FREE(me->parsers);
	HT_FREE(me);
    }
    return HT_OK;
}

PUBLIC HTMIMEParseEl * HTMIMEParseSet_add (HTMIMEParseSet * me, 
					   const char * token, 
					   BOOL caseSensitive, 
					   HTParserCallback * callback)
{
    int hash;

    /*		Insure hash list
    */
    if (!me->parsers) {
        if (!me->size)
	    me->size = HT_S_HASH_SIZE;
	if ((me->parsers = (HTMIMEParseEl **) HT_CALLOC(me->size, sizeof(HTMIMEParseEl *))) == NULL)
	    HT_OUTOFMEM("HTMIME parsers");
    }
    hash = HTMIMEParseSet_hash(me, token);

    /*		Add a new entry
    */
    return HTMIMEParseEl_new(&me->parsers[hash], token, 
			     caseSensitive, callback);
}


PUBLIC HTMIMEParseEl * HTMIMEParseSet_addRegex (HTMIMEParseSet * me, 
						const char * token, 
						BOOL caseSensitive, 
						HTParserCallback * callback)
{
    return HTMIMEParseEl_new(&me->regexParsers, token, 
			     caseSensitive, callback);
}

PUBLIC int HTMIMEParseSet_delete (HTMIMEParseSet * me, const char * token)
{
    int hash, i;
    HTMIMEParseEl * pEl, ** last;
    
    hash = HTMIMEParseSet_hash(me, token);

    pEl = me->parsers[hash];
    last = &me->parsers[hash];
    for (i = 0; i < 2; i++) { /* do both  */
        for (; pEl; last = &pEl->next, pEl = pEl->next) {
	    if ((pEl->caseSensitive && !strcmp(pEl->token, token)) || 
		(!pEl->caseSensitive && !strcasecomp(pEl->token, token))) {
	        return HTMIMEParseEl_delete(pEl, last);
	    }
	}
	pEl = me->regexParsers;
	last = &me->regexParsers;
    }
    return HT_ERROR;
}

/*
**	Search registered parsers to find suitable one for this token
**	If a parser isn't found, the function returns HT_OK
*/
PUBLIC int HTMIMEParseSet_dispatch (HTMIMEParseSet * me, HTRequest * request, 
				    char * token, char * value, BOOL * pFound)
{
    int hash;
    HTResponse * response = HTRequest_response(request);
    HTMIMEParseEl * pEl;
    
    if (pFound) *pFound = NO;

    /*
    **  Get a hash value for this token. This has is a function of the hash
    **  size given when the MIME header parse set was created.
    */
    hash = HTMIMEParseSet_hash(me, token);

    /*
    **  Search for an exact match
    */
    for (pEl = me->parsers[hash]; pEl; pEl = pEl->next) {
        if ((pEl->caseSensitive && !strcmp(pEl->token, token)) || 
	    (!pEl->caseSensitive && !strcasecomp(pEl->token, token))) {
	    if (pFound) *pFound = YES;
	    if (!pEl->pFunk) return HT_OK; /* registered with no callback*/
	    return (*pEl->pFunk)(request, response, token, value);
	}
    }

    /*
    **  Search for best match using regular expressions. 
    */
    for (pEl = me->regexParsers; pEl; pEl = pEl->next) {
        if ((pEl->caseSensitive && HTStrMatch(pEl->token, token)) || 
	    (!pEl->caseSensitive && HTStrCaseMatch(pEl->token, token))) {
	    if (pFound) *pFound = YES;
	    if (!pEl->pFunk) return HT_OK; /* registered with no callback*/
	    return (*pEl->pFunk)(request, response, token, value);
	}
    }

    return HT_OK;
}

