/*	Configuration manager for Clients			HTRules.c
**	=================================
**
**	This module is replaced by HTConfig.c in httpd.
**
** History:
**	 3 Jun 91	Written TBL
**	10 Aug 91	Authorisation added after Daniel Martin (pass, fail)
**			Rule order in file changed
**			Comments allowed with # on 1st char of rule line
**      17 Jun 92       Bug fix: pass and fail failed if didn't contain '*' TBL
**       1 Sep 93       Bug fix: no memory check - Nathan Torkington
**                      BYTE_ADDRESSING removed - Arthur Secret
**	11 Sep 93  MD	Changed %i into %d in debug printf. 
**			VMS does not recognize %i.
**			Bug Fix: in case of PASS, only one parameter to printf.
**	19 Sep 93  AL	Added Access Authorization stuff.
**	 1 Nov 93  AL	Added htbin.
**	30 Nov 93  AL	Added HTTranslateReq().
**	 4 Feb 94  AL	Took away all the daemon-specific stuff.
**
*/

/* (c) CERN WorldWideWeb project 1990,91. See Copyright.html for details */
#include "HTRules.h"

#include "HTFile.h"
#include "HTParse.h"	/* HTParse() */
#include "HTAAUtil.h"

#ifndef VMS
#include <pwd.h>	/* Unix password file routine: getpwnam() */
#endif /* not VMS */


#define LINE_LENGTH 256


typedef struct _rule {
	struct _rule *	next;
	HTRuleOp	op;
	char *		pattern;
	char *		equiv;
} rule;


/*	Module-wide variables
**	---------------------
*/

PRIVATE rule * rules = 0;	/* Pointer to first on list */
#ifndef PUT_ON_HEAD
PRIVATE rule * rule_tail = 0;	/* Pointer to last on list */
#endif


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

PUBLIC int HTAddRule ARGS3(HTRuleOp,		op,
			   CONST char *,	pattern,
			   CONST char *,	equiv)
{ /* BYTE_ADDRESSING removed and memory check - AS - 1 Sep 93 */
    rule *      temp;
    char *      pPattern;

    temp = (rule *)malloc(sizeof(*temp));
    if (temp==NULL) 
	outofmem(__FILE__, "HTAddRule"); 
    pPattern = (char *)malloc(strlen(pattern)+1);
    if (pPattern==NULL) 
	outofmem(__FILE__, "HTAddRule"); 
    if (equiv) {		/* Two operands */
	char *	pEquiv = (char *)malloc(strlen(equiv)+1);
	if (pEquiv==NULL) 
	    outofmem(__FILE__, "HTAddRule"); 
        temp->equiv = pEquiv;
        strcpy(pEquiv, equiv);
    } else {
        temp->equiv = 0;
    }
    temp->pattern = pPattern;
    temp->op = op;

    strcpy(pPattern, pattern);
    if (TRACE) {
       if (equiv)
          fprintf(stderr, "Rule: For `%s' op %d `%s'\n", pattern, op, equiv);
       else
          fprintf(stderr, "Rule: For `%s' op %d\n", pattern, op);
    }

#ifdef PUT_ON_HEAD
    temp->next = rules;
    rules = temp;
#else
    temp->next = 0;
    if (rule_tail) rule_tail->next = temp;
    else rules = temp;
    rule_tail = temp;
#endif

        
    return 0;
}


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
PUBLIC int HTClearRules NOARGS
{
    while (rules) {
    	rule * temp = rules;
	rules = temp->next;
	free(temp->pattern);
	free(temp->equiv);
	free(temp);
    }
#ifndef PUT_ON_HEAD
    rule_tail = 0;
#endif

    return 0;
}



/*	Translate by rules					HTTranslate()
**	------------------
**
**	The most recently defined rules are applied last.
**
** On entry,
**	required	points to a string whose equivalent value is neeed
** On exit,
**	returns		the address of the equivalent string allocated from
**			the heap which the CALLER MUST FREE. If no translation
**			occured, then it is a copy of te original.
*/
PUBLIC char * HTTranslate ARGS1(CONST char *, required)
{
    rule * r;
    char *current = NULL;
    StrAllocCopy(current, required);

    for(r = rules; r; r = r->next) {
        char * p = r->pattern;
	int m=0;   /* Number of characters matched against wildcard */
	CONST char * q = current;
	for(;*p && *q; p++, q++) {   /* Find first mismatch */
	    if (*p!=*q) break;
	}

	if (*p == '*') {		/* Match up to wildcard */
	    m = strlen(q) - strlen(p+1); /* Amount to match to wildcard */
	    if(m<0) continue;           /* tail is too short to match */
	    if (0!=strcmp(q+m, p+1)) continue;	/* Tail mismatch */
	} else 				/* Not wildcard */
	    if (*p != *q) continue;	/* plain mismatch: go to next rule */

	switch (r->op) {		/* Perform operation */

	case HT_Pass:				/* Authorised */
    		if (!r->equiv) {
		    CTRACE(stderr, "HTRule: Pass `%s'\n", current);
		    return current;
	        }
		/* Else fall through ...to map and pass */
		
	case HT_Map:
	    if (*p == *q) { /* End of both strings, no wildcard */
		CTRACE(stderr, "For `%s' using `%s'\n", current, r->equiv);  
		StrAllocCopy(current, r->equiv); /* use entire translation */
	    } else {
		  char * ins = strchr(r->equiv, '*');	/* Insertion point */
	          if (ins) {	/* Consistent rule!!! */
			char * temp = (char *)malloc(
				strlen(r->equiv)-1 + m + 1);
			if (temp==NULL) 
			    outofmem(__FILE__, "HTTranslate"); /* NT & AS */
			strncpy(temp, 	r->equiv, ins-r->equiv);
			/* Note: temp may be unterminated now! */
			strncpy(temp+(ins-r->equiv), q, m);  /* Matched bit */
			strcpy (temp+(ins-r->equiv)+m, ins+1);	/* Last bit */
    			CTRACE(stderr, "For `%s' using `%s'\n",
						current, temp);
			free(current);
			current = temp;			/* Use this */

		    } else {	/* No insertion point */
			char * temp = (char *)malloc(strlen(r->equiv)+1);
			if (temp==NULL) 
			    outofmem(__FILE__, "HTTranslate"); /* NT & AS */
			strcpy(temp, r->equiv);
    			CTRACE(stderr, "For `%s' using `%s'\n", current, temp);
			free(current);
			current = temp;			/* Use this */
		    } /* If no insertion point exists */
		}
		if (r->op == HT_Pass) {
		    CTRACE(stderr, "HTRule: ...and pass `%s'\n", current);
		    return current;
		}
		break;

	case HT_Fail:				/* Unauthorised */
	default:
	    CTRACE(stderr,"HTRule: *** FAIL `%s'\n", current);
	    return (char *)0;
		    		    
	} /* if tail matches ... switch operation */

    } /* loop over rules */

    return current;	/* For clients default is to pass */
}



/*	Load one line of configuration
**	------------------------------
**
**	Call this, for example, to load a X resource with config info.
**
** returns	0 OK, < 0 syntax error.
*/
PUBLIC int HTSetConfiguration ARGS1(CONST char *, config)
{
    HTRuleOp op;
    char * line = NULL;
    char * pointer = line;
    char *word1, *word2, *word3;
    float quality, secs, secs_per_byte;
    int status;
    
    StrAllocCopy(line, config);
    {
	char * p = strchr(line, '#');	/* Chop off comments */
	if (p) *p = 0;
    }
    pointer = line;
    word1 = HTNextField(&pointer);
    if (!word1) {
    	free(line);
	return 0;
    } ;	/* Comment only or blank */

    word2 = HTNextField(&pointer);
    word3 = HTNextField(&pointer);

    if (!word2) {
	fprintf(stderr, "HTRule: Insufficient operands: %s\n", line);
	free(line);
	return -2;	/*syntax error */
    }

    if (0==strcasecomp(word1, "suffix") ||
	0==strcasecomp(word1, "addtype")) {
        char * encoding = HTNextField(&pointer);
	if (pointer) status = sscanf(pointer, "%f", &quality);
	else status = 0;
	HTAddType(word2,	word3,
				encoding ? encoding : "binary",
				status >= 1? quality : 1.0);

    } else if (0==strcasecomp(word1, "addencoding")) {
	if (pointer)
	    status = sscanf(pointer, "%f", &quality);
	else status = 0;
	HTAddEncoding(word2, word3,
		      status >= 1 ? quality : 1.0);

    } else if (0==strcasecomp(word1, "addlanguage")) {
	if (pointer)
	    status = sscanf(pointer, "%f", &quality);
	else status = 0;
	HTAddLanguage(word2, word3,
		      status >= 1 ? quality : 1.0);

    } else if (0==strcasecomp(word1, "presentation")) {
        if (pointer) status = sscanf(pointer, "%f%f%f",
			    &quality, &secs, &secs_per_byte);
        else status = 0;
	if (!HTConversions) HTConversions = HTList_new();
	HTSetPresentation(HTConversions, word2, word3,
		    status >= 1? quality 		: 1.0,
		    status >= 2 ? secs 			: 0.0,
		    status >= 3 ? secs_per_byte 	: 0.0 );

    } else {
	op =	0==strcasecomp(word1, "map")  ?	HT_Map
	    :	0==strcasecomp(word1, "pass") ?	HT_Pass
	    :	0==strcasecomp(word1, "fail") ?	HT_Fail
	    :					HT_Invalid;
	if (op==HT_Invalid) {
	    CTRACE(stderr, "HTRule: Bad rule `%s'\n", config);
	} else {  
	    HTAddRule(op, word2, word3);
	} 
    }
    free(line);
    return 0;
}


/*	Load the rules from a file				HTLoadRules()
**	--------------------------
**
** On entry,
**	Rules can be in any state
** On exit,
**	Any existing rules will have been kept.
**	Any new rules will have been loaded.
**	Returns		0 if no error, 0 if error!
**
** Bugs:
**	The strings may not contain spaces.
*/

int HTLoadRules ARGS1(CONST char *, filename)
{
    FILE * fp = fopen(filename, "r");
    char line[LINE_LENGTH+1];
    
    if (!fp) {
        CTRACE(stderr, "HTRules: Can't open rules file %s\n", filename);
	return -1; /* File open error */
    }
    for(;;) {
	if (!fgets(line, LINE_LENGTH+1, fp)) break;	/* EOF or error */
	(void) HTSetConfiguration(line);
    }
    fclose(fp);
    return 0;		/* No error or syntax errors ignored */
}

