/*

  					W3C Sample Code Library libwww Rule File Configuration manager


!
  Rule File Configuration Manager
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The W3C Library provides this module for handling configuration files (a.k.a.
rule files). Rule files can be used to initialize as much as the application
desires including setting up new protocol modules etc. Also the rules file
do not have to be a fil - it can be a database or any other way of storage
information. This implementation is not used by the Library at all and is
part of the Application interface.

This module is implemented by HTRules.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTRULE_H
#define HTRULE_H

#include "HTList.h"
#include "HTReq.h"
#include "HTFormat.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Parse a Rule File
.

Parsing a whole rule file is done using a converter stream. This means
that a rule file can come from anywhere, even accross the network. We
have defined a special content type for rule files called
WWW_RULES in HTFormat and
made a file suffix binding for all files ending in
".conf" in the default
initialization of the file suffix bindings
module.

The rule file parser comes in two variants: one that asks the user and
one that doesn't. Needless to say that you have to be carefull with
the latter one. You can also define HT_AUTOMATIC_RULES
when compiling libwww to determine
whether it is OK to parse a rule file without asking the user.
*/

extern HTConverter HTRules, HTRules_parseAutomatically;

/*
.
  Parse a single line from a Rule File
.

This routine may be used for loading configuration information from sources
other than the rule file, for example INI files for X resources.
config is a string in the syntax of a rule file line.
*/

extern BOOL HTRule_parseLine (HTList * list, const char * config);

/*
.
  Add a Rule to the List
.

This function adds a rule to the list of rules. The pattern
is a 0-terminated string containing a single "*". equiv points
to the equivalent string with * for the place where the text matched by *
goes.
*/

typedef struct _HTRule HTRule;

typedef enum _HTRuleOp {
    HT_Invalid, 
    HT_Map, 
    HT_Pass, 
    HT_Fail,
    HT_DefProt,
    HT_Protect,
    HT_Exec,
    HT_Redirect,
    HT_UseProxy
} HTRuleOp;

extern BOOL HTRule_add (HTList * list, HTRuleOp op,
			const char * pattern, const char * replace);

/*
.
  Delete all Registered Rules
.

This function clears all rules registered
*/

extern BOOL HTRule_deleteAll (HTList *list);

/*
.
  Global Rules
.

Rules are handled as list as everything else that has to do with preferences.
We provide two functions for getting and setting the global rules:
*/

extern HTList * HTRule_global(void);
extern BOOL HTRule_setGlobal (HTList * list);
extern BOOL HTRule_addGlobal (HTRuleOp op, const char * pattern, const char * replace);

/*
.
  Translate by rules
.

This function walks through the list of rules and translates the reference
when matches are found. The list is traversed in order starting from the
head of the list. It returns the address of the equivalent string allocated
from the heap which the CALLER MUST FREE. If no translation occured, then
it is a copy of the original.
*/

extern char * HTRule_translate (HTList * list, const char * token,
				BOOL ignore_case);

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTRULE_H */

/*

  

  @(#) $Id$

*/
