/* 								     HTAAUtil.c
**	COMMON PARTS OF ACCESS AUTHORIZATION MODULE
**	FOR BOTH SERVER AND BROWSER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
** IMPORTANT:
**	Routines in this module use dynamic allocation, but free
**	automatically all the memory reserved by them.
**
**	Therefore the caller never has to (and never should)
**	free() any object returned by these functions.
**
**	Therefore also all the strings returned by this package
**	are only valid until the next call to the same function
**	is made. This approach is selected, because of the nature
**	of access authorization: no string returned by the package
**	needs to be valid longer than until the next call.
**
**	This also makes it easy to plug the AA package in:
**	you don't have to ponder whether to free() something
**	here or is it done somewhere else (because it is always
**	done somewhere else).
**
**	The strings that the package needs to store are copied
**	so the original strings given as parameters to AA
**	functions may be freed or modified with no side effects.
**
**	The AA package does not free() anything else than what
**	it has itself allocated.
**
**	AA (Access Authorization) package means modules which
**	names start with HTAA.
**
** AUTHORS:
**	AL	Ari Luotonen	luotonen@dxcern.cern.ch
**	MD 	Mark Donszelmann    duns@vxdeop.cern.ch
**
** HISTORY:
**	 8 Nov 93  MD	(VMS only) Added case insensitive comparison
**			in HTAA_templateCaseMatch
**
**
** BUGS:
**
**
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTAAUtil.h"	/* Implemented here	*/
#include "HTAssoc.h"	/* Assoc list		*/


/* PUBLIC						HTAAScheme_enum()
**		TRANSLATE SCHEME NAME INTO
**		A SCHEME ENUMERATION
**
** ON ENTRY:
**	name		is a string representing the scheme name.
**
** ON EXIT:
**	returns		the enumerated constant for that scheme.
*/
PUBLIC HTAAScheme HTAAScheme_enum (CONST char* name)
{
    static char *upcased = NULL;
    char *cur;

    if (!name) return HTAA_UNKNOWN;

    StrAllocCopy(upcased, name);
    cur = upcased;
    while (*cur) {
	*cur = TOUPPER(*cur);
	cur++;
    }
    
    if      (!strncmp(upcased, "NONE", 4))	   return HTAA_NONE;
    else if (!strncmp(upcased, "BASIC", 5))	   return HTAA_BASIC;
    else if (!strncmp(upcased, "PUBKEY", 6))	   return HTAA_PUBKEY;
    else if (!strncmp(upcased, "KERBEROSV4", 10))  return HTAA_KERBEROS_V4;
    else if (!strncmp(upcased, "KERBEROSV5", 10))  return HTAA_KERBEROS_V5;
    else					   return HTAA_UNKNOWN;
}


/* PUBLIC						HTAAScheme_name()
**			GET THE NAME OF A GIVEN SCHEME
** ON ENTRY:
**	scheme		is one of the scheme enum values:
**			HTAA_NONE, HTAA_BASIC, HTAA_PUBKEY, ...
**
** ON EXIT:
**	returns		the name of the scheme, i.e.
**			"None", "Basic", "Pubkey", ...
*/
PUBLIC char *HTAAScheme_name (HTAAScheme scheme)
{
    switch (scheme) {
      case HTAA_NONE:		return "None";          break;
      case HTAA_BASIC:		return "Basic";         break;
      case HTAA_PUBKEY:		return "Pubkey";        break;
      case HTAA_KERBEROS_V4:	return "KerberosV4";	break;
      case HTAA_KERBEROS_V5:	return "KerberosV5";	break;
      case HTAA_UNKNOWN:	return "UNKNOWN";       break;
      default:			return "THIS-IS-A-BUG";
    }
}



/* PUBLIC						HTAA_templateMatch()
**		STRING COMPARISON FUNCTION FOR FILE NAMES
**		   WITH ONE WILDCARD * IN THE TEMPLATE
** NOTE:
**	This is essentially the same code as in HTRules.c, but it
**	cannot be used because it is embedded in between other code.
**	(In fact, HTRules.c should use this routine, but then this
**	 routine would have to be more sophisticated... why is life
**	 sometimes so hard...)
**
** ON ENTRY:
**	template	is a template string to match the file name
**			agaist, may contain a single wildcard
**			character * which matches zero or more
**			arbitrary characters.
**	filename	is the filename (or pathname) to be matched
**			agaist the template.
**
** ON EXIT:
**	returns		YES, if filename matches the template.
**			NO, otherwise.
*/
PUBLIC BOOL HTAA_templateMatch (CONST char * tmplate, 
				     CONST char * filename)
{
    CONST char *p = tmplate;
    CONST char *q = filename;
    int m;

    if (!tmplate || !filename) {
	if (PROT_TRACE)
	    TTYPrint(TDEST, "HTAA_templateMatch: invalid param: %s is NULL!!\n",
		    (tmplate ? "filename" : "template"));
	return NO;
    }

    for( ; *p  &&  *q  &&  *p == *q; p++, q++)	/* Find first mismatch */
	; /* do nothing else */

    if (!*p && !*q)	return YES;	/* Equally long equal strings */
    else if ('*' == *p) {		/* Wildcard */
	p++;				/* Skip wildcard character */
	m = strlen(q) - strlen(p);	/* Amount to match to wildcard */
	if (m < 0) return NO;		/* No match, filename too short */
	else {			/* Skip the matched characters and compare */
	    if (strcmp(p, q+m))	return NO;	/* Tail mismatch */
	    else                return YES;	/* Tail match */
	}
    }	/* if wildcard */
    else		return NO;	/* Length or character mismatch */
}    


/* PUBLIC					HTAA_templateCaseMatch()
**		STRING COMPARISON FUNCTION FOR FILE NAMES
**	   WITH ONE WILDCARD * IN THE TEMPLATE (Case Insensitive)
** NOTE:
**	This is essentially the same code as in HTAA_templateMatch, but
**	it compares case insensitive (for VMS). Reason for this routine
**	is that HTAA_templateMatch gets called from several places, also 
**	there where a case sensitive match is needed, so one cannot just
**	change the HTAA_templateMatch routine for VMS.
**
** ON ENTRY:
**	tmplate		is a template string to match the file name
**			agaist, may contain a single wildcard
**			character * which matches zero or more
**			arbitrary characters.
**	filename	is the filename (or pathname) to be matched
**			agaist the template.
**
** ON EXIT:
**	returns		YES, if filename matches the template.
**			NO, otherwise.
*/
PUBLIC BOOL HTAA_templateCaseMatch (CONST char * tmplate, 
			        	 CONST char * filename)
{
    CONST char *p = tmplate;
    CONST char *q = filename;
    int m;

    if (!tmplate || !filename) {
	if (PROT_TRACE)
	    TTYPrint(TDEST,
		    "HTAA_templateCaseMatch: invalid param: %s is NULL!!\n",
		    (tmplate ? "filename" : "template"));
	return NO;
    }

    for( ; *p  &&  *q  &&  TOUPPER(*p) == TOUPPER(*q); p++, q++) /* Find first mismatch */
	; /* do nothing else */

    if (!*p && !*q)	return YES;	/* Equally long equal strings */
    else if ('*' == *p) {		/* Wildcard */
	p++;				/* Skip wildcard character */
	m = strlen(q) - strlen(p);	/* Amount to match to wildcard */
	if (m < 0) return NO;		/* No match, filename too short */
	else {			/* Skip the matched characters and compare */
	    if (strcasecomp(p, q+m))	return NO;	/* Tail mismatch */
	    else                return YES;	/* Tail match */
	}
    }	/* if wildcard */
    else		return NO;	/* Length or character mismatch */
}    


/* PUBLIC					HTAA_makeProtectionTemplate()
**		CREATE A PROTECTION TEMPLATE FOR THE FILES
**		IN THE SAME DIRECTORY AS THE GIVEN FILE
**		(Used by server if there is no fancier way for
**		it to tell the client, and by browser if server
**		didn't send WWW-ProtectionTemplate: field)
** ON ENTRY:
**	docname	is the document pathname (from URL).
**
** ON EXIT:
**	returns	a template matching docname, and other files
**		files in that directory.
**
**		E.g.  /foo/bar/x.html  =>  /foo/bar/ *
**						    ^
**				Space only to prevent it from
**				being a comment marker here,
**				there really isn't any space.
*/
PUBLIC char *HTAA_makeProtectionTemplate (CONST char * docname)
{
    char *tmplate = NULL;
    char *slash = NULL;

    if (docname) {
	StrAllocCopy(tmplate, docname);
	slash = strrchr(tmplate, '/');
	if (slash) slash++;
	else slash = tmplate;
	*slash = (char)0;
	StrAllocCat(tmplate, "*");
    }
    else StrAllocCopy(tmplate, "*");

    if (PROT_TRACE)
	TTYPrint(TDEST, "make_template: made template `%s' for file `%s'\n",
		tmplate, docname);

    return tmplate;
}




/*
** Skip leading whitespace from *s forward
*/
#define SKIPWS(s) while (*s==' ' || *s=='\t') s++;

/*
** Kill trailing whitespace starting from *(s-1) backwords
*/
#define KILLWS(s) {char *c=s-1; while (*c==' ' || *c=='\t') *(c--)=(char)0;}


/* PUBLIC						HTAA_parseArgList()
**		PARSE AN ARGUMENT LIST GIVEN IN A HEADER FIELD
** ON ENTRY:
**	str	is a comma-separated list:
**
**			item, item, item
**		where
**			item ::= value
**			       | name=value
**			       | name="value"
**
**		Leading and trailing whitespace is ignored
**		everywhere except inside quotes, so the following
**		examples are equal:
**
**			name=value,foo=bar
**			 name="value",foo="bar"
**			  name = value ,  foo = bar
**			   name = "value" ,  foo = "bar"
**
** ON EXIT:
**	returns	a list of name-value pairs (actually HTAssocList*).
**		For items with no name, just value, the name is
**		the number of order number of that item. E.g.
**		"1" for the first, etc.
*/
PUBLIC HTAssocList *HTAA_parseArgList (char * str)
{
    HTAssocList *assoc_list = HTAssocList_new();
    char *cur = NULL;
    char *name = NULL;
    int index = 0;

    if (!str) return assoc_list;

    while (*str) {
	SKIPWS(str);				/* Skip leading whitespace */
	cur = str;
	index++;

	while (*cur  &&  *cur != '='  &&  *cur != ',')
	    cur++;	/* Find end of name (or lonely value without a name) */
	KILLWS(cur);	/* Kill trailing whitespace */

	if (*cur == '=') {			/* Name followed by a value */
	    *(cur++) = (char)0;			/* Terminate name */
	    StrAllocCopy(name, str);
	    SKIPWS(cur);			/* Skip WS leading the value */
	    str = cur;
	    if (*str == '"') {			/* Quoted value */
		str++;
		cur = str;
		while (*cur  &&  *cur != '"') cur++;
		if (*cur == '"')
		    *(cur++) = (char)0;	/* Terminate value */
		/* else it is lacking terminating quote */
		SKIPWS(cur);			/* Skip WS leading comma */
		if (*cur == ',') cur++;		/* Skip separating colon */
	    }
	    else {				/* Unquoted value */
		while (*cur  &&  *cur != ',') cur++;
		KILLWS(cur);			/* Kill trailing whitespace */
		if (*cur == ',')
		    *(cur++) = (char)0;
		/* else *cur already NULL */
	    }
	}
	else {	/* No name, just a value */
	    if (*cur == ',') 
		*(cur++) = (char)0;		/* Terminate value */
	    /* else last value on line (already terminated by NULL) */
	    StrAllocCopy(name, "nnn");	/* Room for item order number */
	    sprintf(name, "%d", index); /* Item order number for name */
	}
	HTAssocList_add(assoc_list, name, str);
	str = cur;
    } /* while *str */
    HT_FREE(name);                                         /* Henrik 14/03-94 */
    return assoc_list;
}






