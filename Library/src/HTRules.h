/*	Configuration manager for Hypertext Daemon		HTRules.h
**	==========================================
**
*/

/* (c) CERN WorldWideWeb project 1990,91. See Copyright.html for details */


#ifndef HTRULE_H
#define HTRULE_H

#include "HTUtils.h"

typedef enum _HTRuleOp {HT_Invalid, HT_Map, HT_Pass, HT_Fail} HTRuleOp;

/*	Add rule to the list					HTAddRule()
**	--------------------
**
**  On entry,
**	pattern		points to 0-terminated string containing a single "*"
**	equiv		points to the equivalent string with * for the
**			place where the text matched by * goes.
**  On exit,
**	returns		0 if success, -1 if error.
*/
/*	Note that if BYTE_ADDRESSING is set, the three blocks required
**	are allocated and deallocated as one. This will save time and
**	storage, when malloc's allocation units are large.
*/
extern int HTAddRule PARAMS((HTRuleOp op, const char * pattern, const char * equiv));


/*	Clear all rules						HTClearRules()
**	---------------
**
** On exit,
**	There are no rules
**	returns		0 if success, -1 if error.
**
** See also
**	HTAddRule()
*/
#ifdef __STDC__
extern int HTClearRules(void);
#else
extern int HTClearRules();
#endif


/*	Translate by rules					HTTranslate()
**	------------------
**
**	The most recently defined rules are applied first.
**
** On entry,
**	required	points to a string whose equivalent value is neeed
** On exit,
**	returns		the address of the equivalent string allocated from
**			the heap which the CALLER MUST FREE. If no translation
**			occured, then it is a copy of the original.
*/
#ifdef __STDC__
extern char * HTTranslate(const char * required);
#else
extern char * HTTranslate();
#endif


/*	Load the rules from a file				HtLoadRules()
**	--------------------------
**
** On entry,
**	Rules can be in any state
** On exit,
**	Any existing rules will have been kept.
**	Any new rules will have been loaded on top, so as to be tried first.
**	Returns		0 if no error.
**
** Bugs:
**	The strings may not contain spaces.
*/

#ifdef __STDC__
extern int HTLoadRules(const char * filename);
#else
extern int HTLoadRules();
#endif


#endif /* HTUtils.h */

