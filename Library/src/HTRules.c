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
**       1 Sep 93       Bug fix: no memory check - Nathan Torkington
**                      BYTE_ADDRESSING removed - Arthur Secret
**	11 Sep 93  MD	Changed %i into %d in debug printf. 
**			VMS does not recognize %i.
**			Bug Fix: in case of PASS, only one parameter to printf.
**	19 Sep 93  AL	Added Access Authorization stuff.
**	 1 Nov 93  AL	Added htbin.
**	30 Nov 93  AL	Added HTTranslateReq().
**
*/

/* (c) CERN WorldWideWeb project 1990,91. See Copyright.html for details */
#include "HTRules.h"

#include <stdio.h>
#include "tcp.h"
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

/*	Global variables
**	----------------
*/
PUBLIC char * HTBinDir = NULL;		/* Physical /htbin directory	*/
                                        /* path (this is obsolite, and	*/
                                        /* only used if HTSearchScript	*/
                                        /* is given as non-absolute --	*/
                                        /* and this is only for rule	*/
                                        /* file backward-compatibility.	*/
PUBLIC char * HTSearchScript = NULL;	/* Search script name.		*/
PUBLIC char * HTPutScript = NULL;	/* Script to handle PUT		*/
PUBLIC char * HTPostScript = NULL;	/* Script to handle POST	*/
PRIVATE char * HTUserDir = NULL;	/* User supported directory name*/



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


PRIVATE char * wrap ARGS1(char *, s)
{
    if (s && *s=='/') {
	char * n = (char*)malloc(strlen(s) + 6);
	if (!n) outofmem(__FILE__, "HTRules:wrap");
	sprintf(n, "file:%s", s);
	free(s);
	return n;
    }
    return s;
}


PRIVATE char * unwrap ARGS1(char *, s)
{
    if (s && !strncmp(s, "file:", 5)) {
	char * t;
	char * n = NULL;

	if (!strncmp(s, "file://", 7))
	    t = strchr(s+7, '/');
	else t = s+5;

	if (!t) {
	    free(s);
	    return NULL;
	}
	StrAllocCopy(n, t);
	free(s);
	return n;
    }
    return s;
}



/*	Translate by rules					HTTranslate()
**	------------------
**
** ATTENTION:
**	THIS FUNCTION HAS BEEN OBSOLITED BY HTTranslateReq()
**	ON SERVER SIDE -- ON BROWSER SIDE THIS IS STILL USED!
**	Don't add new server features to this, this already has
**	more than it can handle cleanly.
**
**	The most recently defined rules are applied last.
**
** On entry,
**	required	points to a string whose equivalent value is neeed
** On exit,
**	returns		the address of the equivalent string allocated from
**			the heap which the CALLER MUST FREE. If no translation
**			occured, then it is a copy of te original.
** NEW FEATURES:
**			When a "protect" or "defprot" rule is mathed,
**			a call to HTAA_setCurrentProtection() or
**			HTAA_setDefaultProtection() is made to notify
**			the Access Authorization module that the file is
**			protected, and so it knows how to handle it.
**								-- AL
*/
PUBLIC char * HTTranslate ARGS1(CONST char *, required)
{
    rule * r;
    char *current = NULL;
    StrAllocCopy(current, required);

#ifdef OLD_CODE
    HTAA_clearProtections();	/* Reset from previous call -- AL */
#endif

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

#ifdef ACCESS_AUTH
	case HT_DefProt:
	case HT_Protect:
	    {
		char *local_copy = NULL;
		char *p;
		char *eff_ids = NULL;
		char *prot_file = NULL;

		if (TRACE) fprintf(stderr,
				   "HTRule: `%s' matched %s %s: `%s'\n",
				   current,
				   (r->op==HT_Protect ? "Protect" : "DefProt"),
				   "rule, setup",
				   (r->equiv ? r->equiv :
				    (r->op==HT_Protect ?"DEFAULT" :"NULL!!")));

		if (r->equiv) {
		    StrAllocCopy(local_copy, r->equiv);
		    p = local_copy;
		    prot_file = HTNextField(&p);
		    eff_ids = HTNextField(&p);
		}

#ifdef THESE_NO_LONGER_WORK
		if (r->op == HT_Protect)
		    HTAA_setCurrentProtection(current, prot_file, eff_ids);
		else
		    HTAA_setDefaultProtection(current, prot_file, eff_ids);
#endif
		FREE(local_copy);

		/* continue translating rules */
	    }
	    break;
#endif ACCESS_AUTH

	case HT_Pass:				/* Authorised */
    		if (!r->equiv) {
		    CTRACE(stderr, "HTRule: Pass `%s'\n", current);
		    return wrap(current);
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
		    return wrap(current);
		}
		break;

	case HT_Exec:
	case HT_Invalid:
	case HT_Fail:				/* Unauthorised */
	default:
	    CTRACE(stderr,"HTRule: *** FAIL `%s'\n", current);
	    return (char *)0;
		    		    
	} /* if tail matches ... switch operation */

    } /* loop over rules */


    return wrap(current);
}



/*	Translate by rules				HTTranslateReq()
**	------------------
**
** On entry,
**	req		request structure.
**	req->arg_path	simplified pathname (no ..'s etc in it),
**			which will be translated.
**			If this starts with /htbin/ it is taken
**			to be a script call request.
**
** On exit,
**	returns		YES on success, NO on failure (Forbidden).
**	req->translated	contains the translated filename;
**			NULL if a script call.
**	req->script	contains the executable script name;
**			NULL if not a script call.
*/
PUBLIC BOOL HTTranslateReq ARGS1(HTRequest *, req)
{
    rule * r;
    char *current = NULL;

    if (!req  ||  !req->arg_path)
	return NO;

    StrAllocCopy(current, req->arg_path);

    if (HTUserDir && current[0] == '/' && current[1] == '~') {	/* User dir */
	char * username = current+2;
	char * end = strchr(username, '/');

	if (end)  *end++ = 0;

	if (*username) {
	    struct passwd * pw = getpwnam(username);

	    if (pw && pw->pw_dir) {
		int homelen = strlen(pw->pw_dir);
		char * d = (char*)malloc(homelen +
					 strlen(HTUserDir) +
					 (end ? strlen(end) : 0) + 3);
		strcpy(d, pw->pw_dir);
		if (pw->pw_dir[homelen-1] != '/' && HTUserDir[0] != '/')
		    strcat(d, "/");
		strcat(d, HTUserDir);
		if (end) {
		    if (HTUserDir[strlen(HTUserDir)-1] != '/')
			strcat(d, "/");
		    strcat(d, end);
		}
		CTRACE(stderr, "\"%s\" --user--> \"%s\"\n", current, d);
		free(current);
		current = d;
	    }
	    else CTRACE(stderr, "HTRule: User dir for '%s' not found\n",
			current);
	}
	else CTRACE(stderr, "HTRule: Invalid user dir request '%s'\n",
		    current);
    }

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

#ifdef ACCESS_AUTH
	case HT_DefProt:
	case HT_Protect:
	    {
		char *local_copy = NULL;
		char *p;
		char *eff_ids = NULL;
		char *prot_file = NULL;

		if (TRACE) fprintf(stderr,
				   "HTRule: `%s' matched %s %s: `%s'\n",
				   current,
				   (r->op==HT_Protect ? "Protect" : "DefProt"),
				   "rule, setup",
				   (r->equiv ? r->equiv :
				    (r->op==HT_Protect ?"DEFAULT" :"NULL!!")));

		if (r->equiv) {
		    StrAllocCopy(local_copy, r->equiv);
		    p = local_copy;
		    prot_file = HTNextField(&p);
		    eff_ids = HTNextField(&p);
		}

		if (r->op == HT_Protect)
		    HTAA_setCurrentProtection(req, prot_file, eff_ids);
		else
		    HTAA_setDefaultProtection(req, prot_file, eff_ids);

		FREE(local_copy);

		/* continue translating rules */
	    }
	    break;
#endif ACCESS_AUTH

	case HT_Exec:
	    if (!r->equiv) {
		if (TRACE) fprintf(stderr,
				   "HTRule: Exec `%s', no extra pathinfo\n",
				   current);
		req->script = current;
		req->script_pathinfo = NULL;
		req->script_pathtrans = NULL;
		return YES;
	    }
	    else if (*p == *q || !strchr(r->equiv, '*')) { /* No wildcards */
		if (TRACE) fprintf(stderr,
				   "HTRule: Exec `%s', no extra pathinfo\n",
				   r->equiv);
		StrAllocCopy(req->script, r->equiv);
		req->script_pathinfo = NULL;
		req->script_pathtrans = NULL;
		return YES;
	    }
	    else {
		char *ins = strchr(r->equiv, '*');
		char *pathinfo;

		req->script = (char*)malloc(strlen(r->equiv) + m);
		if (!req->script) outofmem(__FILE__, "HTTranslateReq");

		strncpy(req->script, r->equiv, ins-r->equiv);
		strncpy(req->script+(ins-r->equiv), q, m);
		strcpy(req->script+(ins-r->equiv)+m, ins+1);
		for (pathinfo = req->script+(ins-r->equiv)+1;
		     *pathinfo && *pathinfo != '/';
		     pathinfo++)
		    ;
		FREE(req->script_pathinfo);
		FREE(req->script_pathtrans);
		if (*pathinfo) {
		    StrAllocCopy(req->script_pathinfo, pathinfo);
		    *pathinfo = 0;
		    req->script_pathtrans =
			unwrap(HTTranslate(req->script_pathinfo));
		}
		return YES;
	    }
	    break;

	  case HT_Redirect:
	    if (!r->equiv) {
		CTRACE(stderr,
		       "HTRule: ERROR: No destination for redirect %s\n",
		       r->pattern);
		req->reason = HTAA_INVALID_REDIRECT;
		return NO;
	    }
	    else req->reason = HTAA_OK_REDIRECT;
	    /* And fall through to HT_Map... */

	case HT_Pass:				/* Authorised */
    		if (!r->equiv) {
		    if (TRACE) fprintf(stderr, "HTRule: Pass `%s'\n", current);
		    req->translated = unwrap(current);
		    return YES;
	        }
		/* Else fall through ...to map and pass */
		
	case HT_Map:
	    if (*p == *q) { /* End of both strings, no wildcard */
		CTRACE(stderr,"For `%s' using `%s'\n", current, r->equiv);  
		StrAllocCopy(current, r->equiv); /* use entire translation */
	    } else {
		char * ins = strchr(r->equiv, '*');	/* Insertion point */
		if (ins) {	/* Consistent rule!!! */
		    char * temp=(char*)malloc(strlen(r->equiv)-1 + m + 1);
		    if (!temp) outofmem(__FILE__, "HTTranslateReq");

		    strncpy(temp, 	r->equiv, ins-r->equiv);
		    /* Note: temp may be unterminated now! */
		    strncpy(temp+(ins-r->equiv), q, m);  /* Matched bit */
		    strcpy (temp+(ins-r->equiv)+m, ins+1);	/* Last bit */
		    CTRACE(stderr,"\"%s\" --map*-> \"%s\"\n",current,temp);
		    free(current);
		    current = temp;			/* Use this */

		} else {	/* No insertion point */
		    char * temp = (char *)malloc(strlen(r->equiv)+1);
		    if (temp==NULL) 
			outofmem(__FILE__, "HTTranslateReq"); /* NT & AS */
		    strcpy(temp, r->equiv);
		    CTRACE(stderr,"\"%s\" --map--> \"%s\"\n",current,temp);
		    free(current);
		    current = temp;			/* Use this */
		} /* If no insertion point exists */
	    }
	    if (r->op == HT_Pass) {
		CTRACE(stderr, "HTRule: Pass `%s'\n", current);
		req->translated = unwrap(current);
		return YES;
	    }
	    else if (r->op == HT_Redirect) {
		CTRACE(stderr, "HTRule: Redirected to `%s'\n", current);
		req->location = current;
		return YES;
	    }
	    break;

	case HT_Invalid:
	case HT_Fail:				/* Unauthorised */
	    CTRACE(stderr, "HTRule: *** FAIL `%s'\n", current);
	    return NO;
	    break;
	} /* if tail matches ... switch operation */

    } /* loop over rules */

    /* Actually here failing might be more appropriate?? */
    req->translated = unwrap(current);
    return YES;
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

    if (0==strcasecomp(word1, "defprot") ||
	0==strcasecomp(word1, "protect"))
	word3 = pointer;  /* The rest of the line to be parsed by AA module */
    else
	word3 = HTNextField(&pointer);	/* Just the next word */

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

    } else if (0==strcasecomp(word1, "addencoding") ||
	       0==strcasecomp(word1, "encoding")) {
	if (pointer)
	    status = sscanf(pointer, "%f", &quality);
	else status = 0;
	HTAddEncoding(word2, word3,
		      status >= 1 ? quality : 1.0);

    } else if (0==strcasecomp(word1, "addlanguage") ||
	       0==strcasecomp(word1, "language")) {
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

    } else if (0==strncasecomp(word1, "putscript", 9) ||
	       0==strncasecomp(word1, "put-script", 10)) {
	StrAllocCopy(HTPutScript, word2);

    } else if (0==strncasecomp(word1, "postscript", 10) ||
	       0==strncasecomp(word1, "post-script", 11)) {
	StrAllocCopy(HTPostScript, word2);

    } else if (0==strncasecomp(word1, "userdir", 7)) {
	CTRACE(stderr,
	       "User supported directories are '%s' under each user's home\n",
	       word2);
	StrAllocCopy(HTUserDir, word2);

    } else if (0==strncasecomp(word1, "htbin", 5) ||
	       0==strncasecomp(word1, "bindir", 6)) {
	char *bindir = (char*)malloc(strlen(word2) + 3);
	if (!bindir) outofmem(__FILE__, "HTSetConfiguration");
	strcpy(bindir, word2);
	strcat(bindir, "/*");
	HTAddRule(HT_Exec, "/htbin/*", bindir);

	/*
	** Physical /htbin location -- this is almost obsolite
	** (only search may need it).
	*/
	StrAllocCopy(HTBinDir, word2);

    } else if (0==strncasecomp(word1, "search", 6)) {
	if (strchr(word2, '/'))
	    StrAllocCopy(HTSearchScript, word2); /* Full search script path */
	else if (HTBinDir) {
	    if (!(HTSearchScript =
		  (char*)malloc(strlen(HTBinDir) + strlen(word2) + 2)))
		outofmem(__FILE__, "HTSetConfiguration");
	    strcpy(HTSearchScript, HTBinDir);
	    strcat(HTSearchScript, "/");
	    strcat(HTSearchScript, word2);
	}
	else if (TRACE) fprintf(stderr,
		"HTRule: Search rule without HTBin rule before ignored\n");
	if (TRACE) {
	    if (HTSearchScript)
		fprintf(stderr, "HTRule: Search script set to `%s'\n",
			HTSearchScript);
	    else fprintf(stderr, "HTRule: Search script not set\n");
	}

    } else {
	op =	0==strcasecomp(word1, "map")  ?	HT_Map
	    :	0==strcasecomp(word1, "pass") ?	HT_Pass
	    :	0==strcasecomp(word1, "fail") ?	HT_Fail
	    :   0==strcasecomp(word1, "exec") ? HT_Exec
	    :   0==strcasecomp(word1, "redirect")? HT_Redirect
	    :   0==strcasecomp(word1, "defprot") ? HT_DefProt
	    :	0==strcasecomp(word1, "protect") ? HT_Protect
	    :						HT_Invalid;
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


