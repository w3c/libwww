/*								       HTAtom.c
**	ATOMS: STRINSGS TO NUMBERS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Atoms are names which are given representative pointer values
**	so that they can be stored more efficiently, and comparisons
**	for equality done more efficiently.
**
**	Atoms are kept in a hash table consisting of an array of linked lists.
**
** Authors:
**	TBL	Tim Berners-Lee, WorldWideWeb project, CERN
**
*/

/* Library include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTList.h"
#include "HTAtom.h"

PRIVATE HTAtom * hash_table[HT_XL_HASH_SIZE];
PRIVATE BOOL initialised = NO;

/*
**	Finds an atom representation for a string. The atom doesn't have to be
**	a new one but can be an already existing atom.
*/
PUBLIC HTAtom * HTAtom_for (const char * string)
{
    int hash;
    const unsigned char * p;
    HTAtom * a;

    if (!string) return NULL;			/* prevent core dumps */
    
    /*		First time around, clear hash table
    */
    if (!initialised) {
        memset((void *) hash_table, '\0', sizeof(HTAtom *) * HT_XL_HASH_SIZE);
	initialised = YES;
    }
    
    /*		Generate hash function
    */
    for (p=string, hash=0; *p; p++) {
        hash = (hash * 3 + TOLOWER(*p)) % HT_XL_HASH_SIZE;
    }
    
    /*		Search for the string in the list
    */
    for (a=hash_table[hash]; a; a=a->next) {
	if (0==strcmp(a->name, string)) {
    	    /* HTTRACE(UTIL_TRACE, "HTAtom: Old atom %p for `%s'\n" _ a _ string); */
	    return a;				/* Found: return it */
	}
    }
    
    /*		Generate a new entry
    */
    if ((a = (HTAtom  *) HT_MALLOC(sizeof(*a))) == NULL)
        HT_OUTOFMEM("HTAtom_for");
    if ((a->name = (char  *) HT_MALLOC(strlen(string)+1)) == NULL)
        HT_OUTOFMEM("HTAtom_for");
    strcpy(a->name, string);
    a->next = hash_table[hash];		/* Put onto the head of list */
    hash_table[hash] = a;
/*    HTTRACE(UTIL_TRACE, "HTAtom: New atom %p for `%s'\n" _ a _ string); */
    return a;
}


/*
**	CASE INSENSITIVE VERSION OF HTAtom_for()
**	Finds an atom representation for a string. The atom doesn't have to be
**	a new one but can be an already existing atom.
*/
PUBLIC HTAtom * HTAtom_caseFor (const char * string)
{
    int hash;
    const unsigned char * p;
    HTAtom * a;

    if (!string) return NULL;			/* prevent core dumps */
    
    /*		First time around, clear hash table
    */
    if (!initialised) {
        memset((void *) hash_table, '\0', sizeof(HTAtom *) * HT_XL_HASH_SIZE);
	initialised = YES;
    }
    
    /*		Generate hash function
    */
    for(p=string, hash=0; *p; p++) {
        hash = (hash * 3 + TOLOWER(*p)) % HT_XL_HASH_SIZE;
    }
    
    /*		Search for the string in the list
    */
    for (a=hash_table[hash]; a; a=a->next) {
	if (!strcasecomp(a->name, string)) {
	    return a;					/* Found: return it */
	}
    }
    
    /*		Generate a new entry
    */
    if ((a = (HTAtom  *) HT_MALLOC(sizeof(*a))) == NULL)
        HT_OUTOFMEM("HTAtom_for");
    if ((a->name = (char  *) HT_MALLOC(strlen(string)+1)) == NULL)
        HT_OUTOFMEM("HTAtom_for");
    strcpy(a->name, string);
    a->next = hash_table[hash];		/* Put onto the head of list */
    hash_table[hash] = a;
    return a;
}


/*
**	This function cleans up the memory used by atoms.
**	Written by Eric Sink, eric@spyglass.com
*/
PUBLIC void HTAtom_deleteAll (void)
{
    int i;
    HTAtom *cur;
    HTAtom *next;
    
    for (i=0; i<HT_XL_HASH_SIZE; i++) {
	if (hash_table[i]) {
	    cur = hash_table[i];
	    while (cur) {
		next = cur->next;
		HT_FREE(cur->name);
		HT_FREE(cur);
		cur = next;	
	    }	
	}
    }
    initialised = NO;
}


PRIVATE BOOL mime_match (const char * name, const char * templ)
{
    if (name && templ) {
	static char *n1 = NULL;
	static char *t1 = NULL;
	char *n2;
	char *t2;

	StrAllocCopy(n1, name);		/* These also HT_FREE the ones	*/
	StrAllocCopy(t1, templ);	/* from previous call.		*/

	if (!(n2 = strchr(n1, '/'))  ||  !(t2 = strchr(t1, '/')))
	    return NO;

	*(n2++) = (char)0;
	*(t2++) = (char)0;

	if ((0==strcmp(t1, "*") || 0==strcmp(t1, n1)) &&
	    (0==strcmp(t2, "*") || 0==strcmp(t2, n2)))
	    return YES;
    }
    return NO;
}
	

PUBLIC HTList *HTAtom_templateMatches (const char * templ)
{
    HTList *matches = HTList_new();

    if (initialised && templ) {
	int i;
	HTAtom *cur;

	for (i=0; i<HT_XL_HASH_SIZE; i++) {
	    for (cur = hash_table[i];  cur;  cur=cur->next) {
		if (mime_match(cur->name, templ))
		    HTList_addObject(matches, (void*)cur);
	    }
	}
    }
    return matches;
}

