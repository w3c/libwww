/*	Configuration manager for Hypertext Daemon		HTRules.c
**	==========================================
**
**
** History:
**	 3 Jun 91	Written TBL
**	10 Aug 91	Authorisation added after Daniel Martin (pass, fail)
**			Rule order in file changed
**			Comments allowed with # on 1st char of rule line
**      17 Jun 92       Bug fix: pass and fail failed if didn't contain '*' TBL
*/

/* (c) CERN WorldWideWeb project 1990,91. See Copyright.html for details */
#include "HTRules.h"

#include <stdio.h>
#include "HTUtils.h"
#include "tcp.h"
#include "HTFile.h"

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
/*	Note that if BYTE_ADDRESSING is set, the three blocks required
**	are allocated and deallocated as one. This will save time and
**	storage, when malloc's allocation units are large.
*/
#ifdef __STDC__
PUBLIC int HTAddRule (HTRuleOp op, const char * pattern, const char * equiv)
#else
int HTAddRule(op, pattern, equiv)
    HTRuleOp	op;
    char *	pattern;
    char *	equiv;
#endif
{
#ifdef BYTE_ADDRESSING	/* economise on malloc() blocks */
    int		length_of_pattern = strlen(pattern)+1;
    rule *	temp = malloc(sizeof(*temp) +
    			size_of_pattern + strlen(equiv) + 1);
    char *	pPattern = (char *) &temp[1];
#else
    rule *	temp = (rule *)malloc(sizeof(*temp));
    char *	pPattern = (char *)malloc(strlen(pattern)+1);
#endif

    if (equiv) {		/* Two operands */
#ifdef BYTE_ADDRESSING	/* economise on malloc() blocks */
        char *	pEquiv = pPattern + length_of_pattern;
#else
	char *	pEquiv = (char *)malloc(strlen(equiv)+1);
#endif
        temp->equiv = pEquiv;
        strcpy(pEquiv, equiv);
    } else {
        temp->equiv = 0;
    }
    temp->pattern = pPattern;
    temp->op = op;

    strcpy(pPattern, pattern);
    if (TRACE) printf("Rule: For `%s' op %i `%s'\n", pattern, op, equiv);

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
#ifdef __STDC__
int HTClearRules(void)
#else
int HTClearRules()
#endif
{
    while (rules) {
    	rule * temp = rules;
	rules = temp->next;
#ifdef BYTE_ADDRESSING
	free(temp);
#else
	free(temp->pattern);
	free(temp->equiv);
	free(temp);
#endif
    }
#ifndef PUT_ON_HEAD
    rule_tail = 0;
#endif

    return 0;
}


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
**			occured, then it is a copy of te original.
*/
#ifdef __STDC__
char * HTTranslate(const char * required)
#else
char * HTTranslate(required)
	char * required;
#endif
{
    rule * r;
    char * current = (char *)malloc(strlen(required)+1);
    strcpy(current, required);
    
    for(r = rules; r; r = r->next) {
        char * p = r->pattern;
	int m;   /* Number of characters matched against wildcard */
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
		    if (TRACE) printf("HTRule: Pass `%s'\n", current);
		    return current;
	        }
		/* Fall through ...to map and pass */
		
	case HT_Map:
	    if (*p == *q) { /* End of both strings, no wildcard */
    	          if (TRACE) printf(
			       "For `%s' using `%s'\n", current, r->equiv);  
	          StrAllocCopy(current, r->equiv); /* use entire translation */
	    } else {
		  char * ins = strchr(r->equiv, '*');	/* Insertion point */
	          if (ins) {	/* Consistent rule!!! */
			char * temp = (char *)malloc(
				strlen(r->equiv)-1 + m + 1);
			strncpy(temp, 	r->equiv, ins-r->equiv);
			/* Note: temp may be unterminated now! */
			strncpy(temp+(ins-r->equiv), q, m);  /* Matched bit */
			strcpy (temp+(ins-r->equiv)+m, ins+1);	/* Last bit */
    			if (TRACE) printf("For `%s' using `%s'\n",
						current, temp);
			free(current);
			current = temp;			/* Use this */

		    } else {	/* No insertion point */
			char * temp = (char *)malloc(strlen(r->equiv)+1);
			strcpy(temp, r->equiv);
    			if (TRACE) printf("For `%s' using `%s'\n",
						current, temp);
			free(current);
			current = temp;			/* Use this */
		    } /* If no insertion point exists */
		}
		if (r->op == HT_Pass) {
		    if (TRACE) printf("HTRule: ...and pass `%s'\n", current);
		    return current;
		}
		break;

	    case HT_Invalid:
	    case HT_Fail:				/* Unauthorised */
    		    if (TRACE) printf("HTRule: *** FAIL `%s'\n", current);
		    return (char *)0;
		    		    
	        } /* if tail matches ... switch operation */

    } /* loop over rules */


    return current;
}



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

int HTLoadRules ARGS1(CONST char *, filename)
{
    FILE * fp = fopen(filename, "r");
    char line[LINE_LENGTH+1];
    char pattern[LINE_LENGTH+1];
    char operation [LINE_LENGTH+1];
    char equiv[LINE_LENGTH+1];
    float quality;
    int status;
    
    if (!fp) {
        if (TRACE) printf("HTRules: Can't open rules file %s\n", filename);
	return -1; /* File open error */
    }
    for(;;) {
        HTRuleOp op;
	if (!fgets(line, LINE_LENGTH+1, fp)) break;	/* EOF or error */
	
	{
	    char * p = strchr(line, '#');	/* Chop off comments */
	    if (p) *p = 0;
	}
	status = sscanf(line, "%256s%256s%256s%f",
		operation, pattern, equiv, &quality);
	if (status<=0) continue;	/* Comment only or blank */    
	if (status<2) {
	    fprintf(stderr, "HTRule: Insufficient operands: %s\n", line);
	    return -2;	/*syntax error */
	}
	
	if (0==strcasecomp(operation, "suffix")) {
	    HTSetSuffix(pattern, equiv, quality);
	} else {
	    op =	0==strcasecomp(operation, "map")  ?	HT_Map
		:	0==strcasecomp(operation, "pass") ?	HT_Pass
		:	0==strcasecomp(operation, "fail") ?	HT_Fail
		:						HT_Invalid;
	    if (op==HT_Invalid) {
		fprintf(stderr, "HTRule: Bad rule `%s'\n", line);
		/* return -2; */
	    } else {  
	        HTAddRule(op, pattern, status > 2 ? equiv : NULL);
	    } 
	}
    }
    fclose(fp);
    return 0;		/* No error */
}
