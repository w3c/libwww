/*								     HTHome.c
**	ANCHOR TRANSLATIONS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** Authors
**	TBL	Tim Berners-Lee timbl@w3.org
**	JFG	Jean-Francois Groff jfg@dxcern.cern.ch
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
**	HFN	Henrik Frystyk
** History
**       8 Jun 92 Telnet hopping prohibited as telnet is not secure TBL
**	26 Jun 92 When over DECnet, suppressed FTP, Gopher and News. JFG
**	 6 Oct 92 Moved HTClientHost and HTlogfile into here. TBL
**	17 Dec 92 Tn3270 added, bug fix. DD
**	 4 Feb 93 Access registration, Search escapes bad chars TBL
**		  PARAMETERS TO HTSEARCH AND HTLOADRELATIVE CHANGED
**	28 May 93 WAIS gateway explicit if no WAIS library linked in.
**	   Dec 93 Bug change around, more reentrant, etc
**	09 May 94 logfile renamed to HTlogfile to avoid clash with WAIS
**	 8 Jul 94 Insulate free from _free structure element.
**	   Sep 95 Rewritten, HFN
**	   Nov 95 Spawned from HTAccess.c
*/

/* Library include files */
#include "WWWLib.h"
#include "HTHome.h"					 /* Implemented here */

/* ------------------------------------------------------------------------- */

/*	Find Related Name
**	-----------------
**	Creates a string that can be used as a related name when 
**	calling HTParse initially. 
**  
**	The code for this routine originates from the Linemode 
**	browser and was moved here by howcome@w3.org
**	in order for all clients to take advantage.
**	The string returned must be freed by the caller
*/
PUBLIC char * HTGetCurrentDirectoryURL (void)
{
    char wd[HT_MAX_PATH+2];

#ifdef HAVE_GETCWD	      /* System V variant SIGN CHANGED TBL 921006 !! */
    char * result = (char *) getcwd(wd, sizeof(wd)); 
#else
#ifdef HAVE_GETWD
    char * result = (char *) getwd(wd);
#else
#error "This platform does not support neither getwd nor getcwd\n"
    char * result = NULL;
#endif /* HAVE_GETWD */
#endif /* HAVE_GETCWD */
    if (result) {
      *(wd+HT_MAX_PATH) = '\0';
      if (*(wd+strlen(wd)-1) != '/') strcat(wd, "/");
    }
    return result ? HTLocalToWWW(result, NULL) : NULL;
}


/*	Generate the anchor for the home page
**	-------------------------------------
**
**	As it involves file access, this should only be done once
**	when the program first runs.
**	This is a default algorithm -- browser don't HAVE to use this.
**	But consistency betwen browsers is STRONGLY recommended!
**
**	Priority order is:
**
**		1	WWW_HOME environment variable (logical name, etc)
**		2	~/WWW/default.html
**		3	/usr/local/bin/default.html
**		4	http://www.w3.org/default.html
**
*/
PUBLIC HTParentAnchor * HTHomeAnchor (void)
{
    char * my_home_document = NULL;
    char * home = (char *) getenv(LOGICAL_DEFAULT);
    char * ref;
    HTParentAnchor * anchor;
    
    /* Someone telnets in, they get a special home */
    if (home) {
        StrAllocCopy(my_home_document, home);
    } else  if (HTLib_secure()) {				    /* Telnet server */
    	FILE * fp = fopen(REMOTE_POINTER, "r");
	char * status;
	if (fp) {
	    if ((my_home_document = (char *) HT_MALLOC(HT_MAX_PATH)) == NULL)
	        HT_OUTOFMEM("my_home_document ");
	    status = fgets(my_home_document, HT_MAX_PATH, fp);
	    if (!status) {
	        HT_FREE(my_home_document);
		my_home_document = NULL;
	    }
	    fclose(fp);
	}
	if (!my_home_document) StrAllocCopy(my_home_document, REMOTE_ADDRESS);
    }

    if (!my_home_document) {
	FILE * fp = NULL;
	char * home = (char *) getenv("HOME");
	if (home) { 
	    if ((my_home_document = (char  *) HT_MALLOC(strlen(home)+1+ strlen(PERSONAL_DEFAULT)+1)) == NULL)
	        HT_OUTOFMEM("HTLocalName");
	    sprintf(my_home_document, "%s/%s", home, PERSONAL_DEFAULT);
	    fp = fopen(my_home_document, "r");
	}
	
	if (!fp) {
	    StrAllocCopy(my_home_document, LOCAL_DEFAULT_FILE);
	    fp = fopen(my_home_document, "r");
	}
	if (fp) {
	    fclose(fp);
	} else {
	    HTTRACE(APP_TRACE, "Home Anchor. No local home document in ~/%s or %s\n" _ 
			PERSONAL_DEFAULT _ LOCAL_DEFAULT_FILE);
	    HT_FREE(my_home_document);
	    my_home_document = NULL;
	}
    }

    ref = HTParse(my_home_document ? my_home_document :
		  HTLib_secure() ? REMOTE_ADDRESS : LAST_RESORT, "file:",
		  PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
    if (my_home_document) {
	HTTRACE(APP_TRACE, "Home Anchor. `%s\' used for custom home page as\n`%s\'\n" _ 
		    my_home_document _ ref);
	HT_FREE(my_home_document);
    }
    anchor = (HTParentAnchor*) HTAnchor_findAddress(ref);
    HT_FREE(ref);
    return anchor;
}

/*
**	Creates a temporary anchor that doesn't exist
*/
PUBLIC HTParentAnchor * HTTmpAnchor (HTUserProfile * up)
{
    static int offset = 0;			    /* Just keep counting... */
    HTParentAnchor * htpa = NULL;
    time_t t = time(NULL);
    char * tmpfile = HTGetTmpFileName(HTUserProfile_tmp(up));
    char * tmpurl = HTParse(tmpfile, "file:", PARSE_ALL);
    if (tmpfile && tmpurl && t >= 0) {
	char * result;
	if (!(result = (char *) HT_MALLOC(strlen(tmpurl)+20)))
	    HT_OUTOFMEM("HTTmpAnchor");
#ifdef HAVE_LONG_TIME_T
	sprintf(result, "%s.%ld.%d", tmpurl, t, offset++);
#else
	sprintf(result, "%s.%d.%d", tmpurl, t, offset++);
#endif
	HTTRACE(APP_TRACE, "Tmp Anchor.. With location `%s\'\n" _ result);
	htpa = HTAnchor_parent(HTAnchor_findAddress(result));
	HT_FREE(result);
    }
    HT_FREE(tmpfile);
    HT_FREE(tmpurl);
    return htpa;
}

/*
**	Takes a string of the form "a=b" containing HTML form data, escapes
**	it accordingly and puts it into the association list so that it
**	readily can be passed to any of the HTAccess function that handles
**	HTML form data.
*/
PUBLIC BOOL HTParseFormInput (HTAssocList * list, const char * str)
{
    if (list && str) {
	char * me = NULL;
	char * name = NULL;
	char * value = NULL;
	StrAllocCopy(me, str);
	value = strchr(me, '=');
	if (value) 
	    *value++ = '\0';
	else
	    value = "";
	name = HTStrip(me);

	/* Escape the name and value */
	if (name) {
	    char * escaped_name = HTEscape(name, URL_XALPHAS);
	    char * escaped_value = HTEscape(value, URL_XALPHAS);
	    HTTRACE(APP_TRACE, "Form data... Adding name `%s\' with value `%s\' to %p\n" _ 
			escaped_name _ escaped_value _ list);
	    HTAssocList_addObject(list, escaped_name, escaped_value);
	    HT_FREE(escaped_name);
	    HT_FREE(escaped_value);
	}
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/*
**	Standard interface to libwww TRACE messages. Pass this function a
**	string of characters and it will set up the appropriate TRACE flags.
**	The shortnames for the trace messages are not as intuitive as they
**	could be :-(. The string must be null terminated
*/
PUBLIC int HTSetTraceMessageMask (const char * shortnames)
{
#if defined(HTDEBUG) && defined(WWWTRACE)
    WWWTRACE = 0;
    if (shortnames && *shortnames) {
	char * ptr = (char *) shortnames;
	for(; *ptr; ptr++) {
	    switch (*ptr) {
	    case 'a': WWWTRACE |= SHOW_ANCHOR_TRACE; 	break;
	    case 'b': WWWTRACE |= SHOW_BIND_TRACE; 	break;
	    case 'c': WWWTRACE |= SHOW_CACHE_TRACE; 	break;
	    case 'e': WWWTRACE |= SHOW_MUX_TRACE; 	break;
	    case 'f': WWWTRACE |= SHOW_UTIL_TRACE; 	break;
	    case 'g': WWWTRACE |= SHOW_SGML_TRACE; 	break;
	    case 'h': WWWTRACE |= SHOW_AUTH_TRACE; 	break;
	    case 'i': WWWTRACE |= SHOW_PICS_TRACE; 	break;
	    case 'l': WWWTRACE |= SHOW_APP_TRACE; 	break;
	    case 'm': WWWTRACE |= SHOW_MEM_TRACE; 	break;
	    case 'o': WWWTRACE |= SHOW_CORE_TRACE; 	break;
	    case 'p': WWWTRACE |= SHOW_PROTOCOL_TRACE; 	break;
	    case 'q': WWWTRACE |= SHOW_SQL_TRACE; 	break;
	    case 's': WWWTRACE |= SHOW_STREAM_TRACE; 	break;
	    case 't': WWWTRACE |= SHOW_THREAD_TRACE; 	break;
	    case 'u': WWWTRACE |= SHOW_URI_TRACE; 	break;
	    case 'x': WWWTRACE |= SHOW_XML_TRACE; 	break;
	    case '*': WWWTRACE |= SHOW_ALL_TRACE; 	break;
	    default:
		if (WWWTRACE) HTTRACE(APP_TRACE, "Trace....... Bad argument\n");
	    }
	}
	if (!WWWTRACE) WWWTRACE = SHOW_ALL_TRACE;
    } else {
	WWWTRACE = SHOW_ALL_TRACE;
    }
    return WWWTRACE;
#else
    return 0;
#endif
}
