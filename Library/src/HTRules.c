/*								      HTRules.c
**	CONFIGURATION MANAGER FOR CLIENTS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module manages rule files in the Library
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
**      28 Sep 94  HWL  Added field to HTPresentation_add call
**	15 Nov 95  HFN	Made a stream, fixed interface and made new translater
**
** BUGS: We only have one wildcard match pr rule!
*/

/* Library include files */
#include "WWWLib.h"
#include "WWWFile.h"
#include "HTProxy.h"
#include "HTRules.h"					 /* Implemented here */

struct _HTStream {
    const HTStreamClass *	isa;
    HTRequest *			request;
    HTChunk *			buffer;
    HTEOLState			EOLstate;
};

struct _HTRule {
    HTRuleOp	op;
    char *	pattern;
    char *	replace;
    int   	insert;		       /* Index into any wildcard in replace */
};

PRIVATE HTList * rules = NULL;

/* ------------------------------------------------------------------------- */

/*
**	Rules are handled as list as everything else that has to do with
**	preferences. We provide two functions for getting and setting the
**	global rules
*/
PUBLIC HTList * HTRule_global (void)
{
    if (!rules) rules = HTList_new();
    return rules;
}

PUBLIC BOOL HTRule_setGlobal(HTList * list)
{
    if (rules) HTRule_deleteAll(rules);
    rules = list;
    return YES;
}

PUBLIC BOOL HTRule_addGlobal(HTRuleOp op,
			     const char * pattern, const char * replace)
{
    if (!rules) rules = HTList_new();
    return HTRule_add(rules, op, pattern, replace);
}

/*	Add rule to the list
**	--------------------
**	This function adds a rule to the list of rules. The
**	pattern is a 0-terminated string containing a single
**	"*". <CODE>equiv</CODE> points to the equivalent string with * for the
**	place where the text matched by * goes.
**  On entry,
**	pattern		points to 0-terminated string containing a single "*"
**	replace		points to the equivalent string with * for the
**			place where the text matched by * goes.
**  On exit,
**	returns		YES if OK, else NO
*/
PUBLIC BOOL HTRule_add (HTList * list, HTRuleOp op,
			const char * pattern, const char * replace)
{
    if (list && pattern) {
	HTRule * me;
	if ((me = (HTRule  *) HT_CALLOC(1, sizeof(HTRule))) == NULL)
	    HT_OUTOFMEM("HTRule_add");
	me->op = op;
	StrAllocCopy(me->pattern, pattern);
	if (replace) {
	    char *ptr = strchr(replace, '*');
	    StrAllocCopy(me->replace, replace);
	    me->insert = ptr ? ptr-replace : -1;
	    HTTRACE(APP_TRACE, "Rule Add.... For `%s\' op %d `%s\'\n" _ 
			 pattern _ op _ replace);
	} else {
	    HTTRACE(APP_TRACE, "Rule Add.... For `%s\' op %d\n" _ pattern _ op);
	}
	return HTList_appendObject(list, (void *) me);
    }
    return NO;
}

/*	Delete all rules
**	----------------
**	Deletes all the rules registered by this module
*/
PUBLIC BOOL HTRule_deleteAll (HTList * list)
{
    if (list) {
	HTList *cur = list;
	HTRule *pres;
	while ((pres = (HTRule *) HTList_nextObject(cur))) {
	    HT_FREE(pres->pattern);
	    HT_FREE(pres->replace);
	    HT_FREE(pres);
	}
	return HTList_delete(list);
    }
    return NO;
}

/*	Translate by rules
**	------------------
**	The most recently defined rules are applied last.
**	This function walks through the list of rules and translates the
**	reference when matches are found. The list is traversed in order
**	starting from the head of the list. It returns the address of the
**	equivalent string allocated from the heap which the CALLER MUST
**	FREE.
*/
PUBLIC char * HTRule_translate (HTList * list, const char * token,
				BOOL ignore_case)
{
    HTRule * pres;
    char * replace = NULL;
    if (!token || !list) return NULL;
    HTTRACE(APP_TRACE, "Check rules. for `%s\'\n" _ token);
    while ((pres = (HTRule *) HTList_nextObject(list))) {
	char * rest = ignore_case ? HTStrCaseMatch(pres->pattern, token) :
	    HTStrMatch(pres->pattern, token);
	if (!rest) continue;				  /* No match at all */
    
	/* We found a match for this entry, now do operation */
	switch (pres->op) {

          case HT_Pass:
	  case HT_Map:
	    if (!pres->replace) {			       /* No replace */
		StrAllocCopy(replace, token);

	    } else if (*rest && pres->insert >= 0) {
		if ((replace = (char  *) HT_MALLOC(strlen(pres->replace)+strlen(rest))) == NULL)
		    HT_OUTOFMEM("HTRule_translate");
		strcpy(replace, pres->replace);
		strcpy(replace+pres->insert, rest);

	    } else {		       /* Perfect match or no insetion point */
		StrAllocCopy(replace, pres->replace);
	    }

	    if (pres->op == HT_Pass) {
		HTTRACE(APP_TRACE, "............ map into `%s'\n" _ replace);
		return replace;
	    }
	    break;
	    
	  case HT_Fail:

	  default:
	    HTTRACE(APP_TRACE, "............ FAIL `%s'\n" _ token);
	    return NULL;
	}
    }
    if (!replace) StrAllocCopy(replace, token);
    return replace;
}

/*	Load one line of configuration
**	------------------------------
**	Call this, for example, to load a X resource with config info.
**	Returns YES if line OK, else NO
*/
PUBLIC BOOL HTRule_parseLine (HTList * list, const char * config)
{
    HTRuleOp op;
    char * line = NULL;
    char * ptr;
    char * word1, * word2, * word3;
    int status;
    if (!config) return NO;
    if ((ptr = strchr(config, '#'))) *ptr = '\0';
    StrAllocCopy(line, config);				 /* Get our own copy */
    ptr = line;
    HTTRACE(APP_TRACE, "Rule Parse.. `%s\'\n" _ config ? config : "<null>");
    if ((word1 = HTNextField(&ptr)) == NULL) {		       /* Empty line */
	HT_FREE(line);
	return YES;
    }
    if ((word2 = HTNextField(&ptr)) == NULL) {
	HTTRACE(APP_TRACE, "Rule Parse.. Insufficient operands: `%s\'\n" _ line);
	HT_FREE(line);
	return NO;
    }
    word3 = HTNextField(&ptr);

    /* Look for things we recognize */
    if (!strcasecomp(word1, "addtype")) {
	double quality;
        char * encoding = HTNextField(&ptr);
	status = ptr ? sscanf(ptr, "%lf", &quality) : 0;
	HTBind_add(word2,				/* suffix */
		   word3,				/* type */
		   encoding ? encoding : "binary",	/* encoding */
		   NULL,				/* cte */
		   NULL,				/* language */
		   status >= 1? quality : 1.0);		/* quality */

    } else if (!strcasecomp(word1, "addencoding")) {
	double quality;
	status = ptr ? sscanf(ptr, "%lf", &quality) : 0;
	HTBind_addEncoding(word2, word3, status >= 1 ? quality : 1.0);

    } else if (!strcasecomp(word1, "addlanguage")) {
	double quality;
	status = ptr ? sscanf(ptr, "%lf", &quality) : 0;
	HTBind_addLanguage(word2, word3, status >= 1 ? quality : 1.0);

    } else if (!strcasecomp(word1, "presentation")) {
	HTList * converters = HTFormat_conversion();
	double quality, secs, secs_per_byte;
        status = ptr ? sscanf(ptr,"%lf%lf%lf",&quality,&secs,&secs_per_byte):0;
	HTPresentation_add(converters, word2, word3, NULL,
			   status >= 1 ? quality : 1.0,
			   status >= 2 ? secs : 0.0,
			   status >= 3 ? secs_per_byte : 0.0);

    } else if (!strcasecomp(word1, "proxy")) {
	HTProxy_add(word2, word3);
	
    } else if (!strcasecomp(word1, "noproxy")) {
	int port = 0;
        status = ptr ? sscanf(ptr, "%d", &port) : 0;
	HTNoProxy_add(word2, word3, port);

    } else if (!strcasecomp(word1, "gateway")) {
	HTGateway_add(word2, word3);

    } else {
	op =	0==strcasecomp(word1, "map")  ?	HT_Map
	    :	0==strcasecomp(word1, "pass") ?	HT_Pass
	    :	0==strcasecomp(word1, "fail") ?	HT_Fail
	    :					HT_Invalid;
	if (op == HT_Invalid) {
	    HTTRACE(APP_TRACE, "Rule Parse.. Bad or unknown: `%s'\n" _ config);
	} else
	    HTRule_add(list, op, word2, word3);
    }
    HT_FREE(line);
    return YES;
}

/*
**	Folding is either of CF LWS, LF LWS, CRLF LWS
*/
PRIVATE int HTRule_put_block (HTStream * me, const char * b, int l)
{
    while (l > 0) {
	if (me->EOLstate == EOL_FCR) {
	    if (*b == LF)				   	     /* CRLF */
		me->EOLstate = EOL_FLF;
	    else if (isspace((int) *b))				   /* Folding: CR SP */
		me->EOLstate = EOL_DOT;
	    else {						 /* New line */
		HTRule_parseLine(rules, HTChunk_data(me->buffer));
		me->EOLstate = EOL_BEGIN;
		HTChunk_clear(me->buffer);
		continue;
	    }
	} else if (me->EOLstate == EOL_FLF) {
	    if (isspace((int) *b))		       /* Folding: LF SP or CR LF SP */
		me->EOLstate = EOL_DOT;
	    else {						/* New line */
		HTRule_parseLine(rules, HTChunk_data(me->buffer));
		me->EOLstate = EOL_BEGIN;
		HTChunk_clear(me->buffer);
		continue;
	    }
	} else if (me->EOLstate == EOL_DOT) {
	    if (isspace((int) *b)) {
		me->EOLstate = EOL_BEGIN;
		HTChunk_putc(me->buffer, ' ');
	    } else {
		HTRule_parseLine(rules, HTChunk_data(me->buffer));
		me->EOLstate = EOL_BEGIN;
		HTChunk_clear(me->buffer);
		continue;
	    }
	} else if (*b == CR) {
	    me->EOLstate = EOL_FCR;
	} else if (*b == LF) {
	    me->EOLstate = EOL_FLF;			       /* Line found */
	} else
	    HTChunk_putc(me->buffer, *b);
	l--; b++;
    }
    return HT_OK;
}

PRIVATE int HTRule_put_character (HTStream * me, char c)
{
    return HTRule_put_block(me, &c, 1);
}

PRIVATE int HTRule_put_string (HTStream * me, const char * s)
{
    return HTRule_put_block(me, s, (int) strlen(s));
}

PRIVATE int HTRule_flush (HTStream * me)
{
    if (me) {
	char * flush = HTChunk_data(me->buffer);
	if (flush) HTRule_parseLine(rules, flush);
	HTChunk_clear(me->buffer);
    }
    return HT_OK;
}

PRIVATE int HTRule_free (HTStream * me)
{
    if (me) {
	int status = HTRule_flush(me);
	HTTRACE(APP_TRACE, "Rules....... FREEING....\n");
	HTChunk_delete(me->buffer);
	HT_FREE(me);
	return status;
    }
    return HT_ERROR;
}

PRIVATE int HTRule_abort (HTStream * me, HTList * e)
{
    if (me) {
	int status = HT_ERROR;
	HTTRACE(APP_TRACE, "Rules....... ABORTING...\n");
	HTChunk_delete(me->buffer);
	HT_FREE(me);
	return status;
    }
    return HT_ERROR;
}

/*	Structured Object Class
**	-----------------------
*/
PRIVATE const HTStreamClass HTRuleClass =
{		
    "RuleParser",
    HTRule_flush,
    HTRule_free,
    HTRule_abort,
    HTRule_put_character,
    HTRule_put_string,
    HTRule_put_block
};

PUBLIC HTStream * HTRules (HTRequest *	request,
			   void *	param,
			   HTFormat	input_format,
			   HTFormat	output_format,
			   HTStream *	output_stream)
{
    HTAlertCallback *cbf = HTAlert_find(HT_A_CONFIRM);
    
    /*
    **  If the library has been compiled so that we automatically accept
    **  rule files then it's OK not to ask the user.
    */
#ifdef HT_AUTOMATIC_RULES
    if (!cbf || (cbf && (*cbf)(request,HT_A_CONFIRM, HT_MSG_RULES, NULL,NULL,NULL))) {
#else
    if ((cbf && (*cbf)(request,HT_A_CONFIRM, HT_MSG_RULES, NULL,NULL,NULL))) {
#endif
	HTStream * me;
	HTTRACE(APP_TRACE, "Rule file... Parser object created\n");
	if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	    HT_OUTOFMEM("HTRules");
	me->isa = &HTRuleClass;
	me->request = request;
	me->buffer = HTChunk_new(512);
	me->EOLstate = EOL_BEGIN;
	if (!rules) rules = HTList_new();
	return me;
    } else {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_NO_AUTO_RULES,
			   NULL, 0, "HTRules");
	return HTErrorStream();
    }
}

/*
**  Parse a rule file - don't ask don't tell - be carefull with this one!
*/
PUBLIC HTStream * HTRules_parseAutomatically (HTRequest *	request,
					      void *		param,
					      HTFormat		input_format,
					      HTFormat		output_format,
					      HTStream *	output_stream)
{
    if (request) {
	HTStream * me;
	HTTRACE(APP_TRACE, "Rule file... Automatic parser object created\n");
	if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	    HT_OUTOFMEM("HTRules");
	me->isa = &HTRuleClass;
	me->request = request;
	me->buffer = HTChunk_new(512);
	me->EOLstate = EOL_BEGIN;
	if (!rules) rules = HTList_new();
	return me;
    } else {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_NO_AUTO_RULES,
			   NULL, 0, "HTRules");
	return HTErrorStream();
    }
}
