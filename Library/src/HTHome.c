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
    char* default_default = NULL;	      /* Parse home relative to this */
    char * host = HTGetHostName(); 
    StrAllocCopy(default_default, "file://");
    if (host) {
	StrAllocCat(default_default, host);
	HT_FREE(host);
    } else
	StrAllocCat(default_default, "localhost");
    {
	char wd[HT_MAX_PATH+1];

#ifdef HAVE_GETCWD	      /* System V variant SIGN CHANGED TBL 921006 !! */
	char *result = (char *) getcwd(wd, sizeof(wd)); 
#else
#ifdef HAVE_GETWD
	char *result = (char *) getwd(wd);
#else
#error "This platform does not support neither getwd nor getcwd\n"
	char *result = NULL;
#endif /* HAVE_GETWD */
#endif /* HAVE_GETCWD */
	*(wd+HT_MAX_PATH) = '\0';
	if (result) {
#ifdef VMS 
            /* convert directory name to Unix-style syntax */
	    char * disk = strchr (wd, ':');
	    char * dir = strchr (wd, '[');
	    if (disk) {
	        *disk = '\0';
		StrAllocCat (default_default, "/");  /* needs delimiter */
		StrAllocCat (default_default, wd);
	    }
	    if (dir) {
		char *p;
		*dir = '/';  /* Convert leading '[' */
		for (p = dir ; *p != ']'; ++p)
			if (*p == '.') *p = '/';
		*p = '\0';  /* Cut on final ']' */
		StrAllocCat (default_default, dir);
	    }
#else  /* not VMS */
#ifdef WIN32
	    char * p = wd ;	/* a colon */
	    StrAllocCat(default_default, "/");
	    while( *p != 0 ) { 
		if (*p == '\\')		         /* change to one true slash */
		    *p = '/' ;
		p++;
	    }
	    StrAllocCat( default_default, wd);
#else /* not WIN32 */
	    StrAllocCat (default_default, wd);
#endif /* not WIN32 */
#endif /* not VMS */
	}
    }
    StrAllocCat(default_default, "/default.html");
    return default_default;
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
	    if (WWWTRACE)
		HTTrace("Home Anchor. No local home document in ~/%s or %s\n",
			PERSONAL_DEFAULT, LOCAL_DEFAULT_FILE);
	    HT_FREE(my_home_document);
	    my_home_document = NULL;
	}
    }

    ref = HTParse(my_home_document ? my_home_document :
		  HTLib_secure() ? REMOTE_ADDRESS : LAST_RESORT, "file:",
		  PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
    if (my_home_document) {
	if (WWWTRACE)
	    HTTrace("Home Anchor. `%s\' used for custom home page as\n`%s\'\n",
		    my_home_document, ref);
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
    time_t t = time(NULL);
    char * tmpfile = HTGetTmpFileName(HTUserProfile_tmp(up));
    char * tmpurl = HTParse(tmpfile, "file:", PARSE_ALL);
    if (tmpfile && tmpurl && t >= 0) {
	char * result;
	if (!(result = (char *) HT_MALLOC(strlen(tmpurl)+20)))
	    HT_OUTOFMEM("HTTmpAnchor");
	sprintf(result, "%s.%ld.%d", tmpurl, t, offset++);
	if (APP_TRACE) HTTrace("Tmp Anchor.. With location `%s\'\n", result);
	return HTAnchor_parent(HTAnchor_findAddress(result));
	HT_FREE(result);
    }
    HT_FREE(tmpfile);
    HT_FREE(tmpurl);
    return NULL;
}

/*
**	Standard interface to libwww TRACE messages. Pass this function a
**	string of characters and it will set up the appropriate TRACE flags.
**	The shortnames for the trace messages are not as intuitive as they
**	could be :-(. The string must be null terminated
*/
PUBLIC int HTSetTraceMessageMask (const char * shortnames)
{
#ifdef WWWTRACE
    WWWTRACE = 0;
    if (shortnames && *shortnames) {
	char * ptr = (char *) shortnames;
	for(; *ptr; ptr++) {
	    switch (*ptr) {
	    case 'f': WWWTRACE |= SHOW_UTIL_TRACE; 	break;
	    case 'l': WWWTRACE |= SHOW_APP_TRACE; 	break;
	    case 'c': WWWTRACE |= SHOW_CACHE_TRACE; 	break;
	    case 'g': WWWTRACE |= SHOW_SGML_TRACE; 	break;
	    case 'b': WWWTRACE |= SHOW_BIND_TRACE; 	break;
	    case 't': WWWTRACE |= SHOW_THREAD_TRACE; 	break;
	    case 's': WWWTRACE |= SHOW_STREAM_TRACE; 	break;
	    case 'p': WWWTRACE |= SHOW_PROTOCOL_TRACE; 	break;
	    case 'm': WWWTRACE |= SHOW_MEM_TRACE; 	break;
	    case 'u': WWWTRACE |= SHOW_URI_TRACE; 	break;
	    case 'h': WWWTRACE |= SHOW_AUTH_TRACE; 	break;
	    case 'a': WWWTRACE |= SHOW_ANCHOR_TRACE; 	break;
	    case 'i': WWWTRACE |= SHOW_PICS_TRACE; 	break;
	    case 'o': WWWTRACE |= SHOW_CORE_TRACE; 	break;
	    default:
		if (WWWTRACE) HTTrace("Trace....... Bad argument\n");
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
