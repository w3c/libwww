/*
**	CONTENT NEGOTIATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** History:
**	March 94  AL	Separated from HTFile.c because
**			multiformat handling would be a mess in VMS.
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTMulti.h"
#include "HTBind.h"
#include "HTFile.h"

#define MULTI_SUFFIX	".multi"/* Extension for scanning formats */
#define MAX_SUFF	15	/* Maximum number of suffixes for a file */
#define VARIANTS	4	/* We start with this array size */

typedef struct _HTContentDescription {
    char *	filename;
    HTFormat	content_type;
    HTLanguage	content_language;
    HTEncoding	content_encoding;
    HTEncoding	content_transfer;
    int		content_length;
    double	quality;
} HTContentDescription;

PRIVATE HTList * welcome_names = NULL;	/* Welcome.html, index.html etc. */

/* ------------------------------------------------------------------------- */

/*
**  Sort the q values in descending order
*/
PRIVATE int VariantSort (const void * a, const void * b)
{
    HTContentDescription * aa = *(HTContentDescription **) a;
    HTContentDescription * bb = *(HTContentDescription **) b;
    if (aa && bb) return (aa->quality > bb->quality) ? -1 : 1;
    return bb - aa;
}

/*
 * Added by takada@seraph.ntt.jp (94/04/08)
 */
PRIVATE BOOL lang_match (HTAtom * tmplate, HTAtom * actual)
{
    const char *t, *a;
    char *st, *sa;
    BOOL match = NO;

    if (tmplate && actual &&
	(t = HTAtom_name(tmplate)) && (a = HTAtom_name(actual))) {
	st = strchr(t, '_');
	sa = strchr(a, '_');
	if ((st != NULL) && (sa != NULL)) {
	    if (!strcasecomp(t, a))
	      match = YES;
	    else
	      match = NO;
	}
	else {
	    if (st != NULL) *st = 0;
	    if (sa != NULL) *sa = 0;
	    if (!strcasecomp(t, a))
	      match = YES;
	    else
	      match = NO;
	    if (st != NULL) *st = '_';
	    if (sa != NULL) *sa = '_';
	}
    }
    return match;
}

PRIVATE double type_value (HTAtom * content_type, HTList * accepted)
{
    if (!content_type) return (1.0);
    if (accepted) {
	HTList * cur = accepted;
	HTPresentation * pres;
	HTPresentation * wild = NULL;
	while ((pres = (HTPresentation *) HTList_nextObject(cur))) {
	    if (pres->rep == content_type)
		return pres->quality;
	    else if (HTMIMEMatch(pres->rep, content_type))
		wild = pres;
	}
	if (wild) return wild->quality;
	else return (0.0);				  /* Nothing matched */
    }
    return (1.0);				      /* We accept all types */
}

PRIVATE double lang_value (HTAtom * language, HTList * accepted)
{
    if (!language) return (1.0);
    if (accepted) {
	HTList * cur = accepted;
	HTAcceptNode * node;
	HTAcceptNode * wild = NULL;
	while ((node = (HTAcceptNode *) HTList_nextObject(cur))) {
	    if (node->atom == language)
		return node->quality;
	    /*
	     * patch by takada@seraph.ntt.jp (94/04/08)
	     * the original line was
	     * else if (HTMIMEMatch(node->atom, language)) {
	     * and the new line is
	     */
	    else if (lang_match(node->atom, language))
		wild = node;
	}
	if (wild) return wild->quality;
	else return (0.0);				  /* Nothing matched */
    }
    return (1.0);				  /* We accept all languages */
}

PRIVATE double encoding_value (HTAtom * encoding, HTList * accepted)
{
    if (!encoding) return (1.0);
    if (accepted) {
	HTList * cur = accepted;
	HTAcceptNode * node;
	HTAcceptNode * wild = NULL;
	const char * e = HTAtom_name(encoding);
	if (!strcmp(e, "7bit") || !strcmp(e, "8bit") || !strcmp(e, "binary"))
	    return (1.0);
	while ((node = (HTAcceptNode*)HTList_nextObject(cur))) {
	    if (node->atom == encoding)
		return node->quality;
	    else if (HTMIMEMatch(node->atom, encoding))
		wild = node;
	}
	if (wild) return wild->quality;
	else return (0.0);				  /* Nothing matched */
    }
    return (1.0);				  /* We accept all encodings */
}

PRIVATE BOOL HTRank (HTRequest * request, HTArray * variants)
{
    HTContentDescription * cd;
    void ** data;
    if (!variants) {
	HTTRACE(PROT_TRACE, "Ranking..... No variants\n");
	return NO;
    }
    /* 
    **  Walk through the list of local and global preferences and find the
    **  overall q factor for each variant
    */
    cd = (HTContentDescription *) HTArray_firstObject(variants, data);
    while (cd) {
	double ctq_local  = type_value(cd->content_type, HTRequest_conversion(request));
	double ctq_global = type_value(cd->content_type, HTFormat_conversion());
	double clq_local  = lang_value(cd->content_language, HTRequest_language(request));
	double clq_global = lang_value(cd->content_language, HTFormat_language());
	double ceq_local  = encoding_value(cd->content_encoding, HTRequest_encoding(request));
	double ceq_global = encoding_value(cd->content_encoding, HTFormat_contentCoding());
	HTTRACE(PROT_TRACE, "Qualities... Content type: %.3f, Content language: %.3f, Content encoding: %.3f\n" _ 
		    HTMAX(ctq_local, ctq_global) _ 
		    HTMAX(clq_local, clq_global) _ 
		    HTMAX(ceq_local, ceq_global));
	cd->quality *= (HTMAX(ctq_local, ctq_global) *
			HTMAX(clq_local, clq_global) *
			HTMAX(ceq_local, ceq_global));
	cd = (HTContentDescription *) HTArray_nextObject(variants, data);
    }

    /* Sort the array of all our accepted preferences */
    HTArray_sort(variants, VariantSort);

    /* Write out the result */
#ifdef HTDEBUG 
    if (PROT_TRACE) {
	int cnt = 1;
	cd = (HTContentDescription *) HTArray_firstObject(variants, data);
	HTTRACE(PROT_TRACE, "Ranking.....\n");
	HTTRACE(PROT_TRACE, "RANK QUALITY CONTENT-TYPE         LANGUAGE ENCODING  FILE\n");
	while (cd) {
	    HTTRACE(PROT_TRACE, "%d.   %.4f  %-20.20s %-8.8s %-10.10s %s\n" _
		    cnt++ _
		    cd->quality _
		    cd->content_type ? HTAtom_name(cd->content_type) : "-" _
		    cd->content_language?HTAtom_name(cd->content_language):"-" _
		    cd->content_encoding?HTAtom_name(cd->content_encoding):"-" _
		    cd->filename ? cd->filename :"-");
	    cd = (HTContentDescription *) HTArray_nextObject(variants, data);
	}
    }
#endif /* HTDEBUG */
    return YES;
}

/* PUBLIC						HTSplitFilename()
**
**	Split the filename to an array of suffixes.
**	Return the number of parts placed to the array.
**	Array should have MAX_SUFF+1 items.
*/
PRIVATE int HTSplitFilename (char * s_str, char ** s_arr)
{
    const char *delimiters = HTBind_delimiters();
    char * start = s_str;
    char * end;
    char save;
    int i;

    if (!s_str || !s_arr) return 0;

    for (i=0; i < MAX_SUFF && *start; i++) {
	for(end=start+1; *end && !strchr(delimiters, *end); end++);
	save = *end;
	*end = 0;
	StrAllocCopy(s_arr[i], start);	/* Frees the previous value */
	*end = save;
	start = end;
    }
    HT_FREE(s_arr[i]);       /* Terminating NULL */
    return i;
}


/*
**	Set default file name for welcome page on each directory.
*/
PUBLIC void HTAddWelcome (char * name)
{
    if (name) {
	char * mycopy = NULL;
	StrAllocCopy(mycopy,name);

	if (!welcome_names)
	    welcome_names = HTList_new();
	HTList_addObject(welcome_names, (void*)mycopy);
    }
}


#ifdef HAVE_READDIR

/* PRIVATE						multi_match()
**
**	Check if actual filename (split in parts) fulfills
**	the requirements.
*/
PRIVATE BOOL multi_match (char ** required, int m, char ** actual, int n)
{
    int c;
    int i,j;

#ifdef VMS
    for(c=0;  c<m && c<n && !strcasecomp(required[c], actual[c]);  c++);
#else /* not VMS */
    for(c=0;  c<m && c<n && !strcmp(required[c], actual[c]);  c++);
#endif /* not VMS */

    if (!c) return NO;		/* Names differ rigth from start */

    for(i=c; i<m; i++) {
	BOOL found = NO;
	for(j=c; j<n; j++) {
#ifdef VMS
	    if (!strcasecomp(required[i], actual[j])) {
#else /* not VMS */
	    if (!strcmp(required[i], actual[j])) {
#endif /* not VMS */
		found = YES;
		break;
	    }
	}
	if (!found) return NO;
    }
    return YES;
}


/*
**	Get multi-match possibilities for a given file
**	----------------------------------------------
** On entry:
**	path	absolute path to one file in a directory,
**		may end in .multi.
** On exit:
**	returns	a list of ContentDesription structures
**		describing the mathing files.
**
*/
PRIVATE HTArray * dir_matches (char * path)
{
    static char * required[MAX_SUFF+1];
    static char * actual[MAX_SUFF+1];
    int m,n;
    char * dirname = NULL;
    char * basename = NULL;
    int baselen;
    char * multi = NULL;
    DIR * dp;
    struct dirent * dirbuf;
    HTArray * matches = NULL;
#ifdef HT_REENTRANT
    struct dirent result;				         /* For readdir_r */
#endif

    if (!path) return NULL;

    StrAllocCopy(dirname, path);
    basename = (strrchr(dirname, '/'));
    if (!basename)
	goto dir_match_failed;
    *basename++ = 0;

    multi = strrchr(basename, MULTI_SUFFIX[0]);
    if (multi && !strcasecomp(multi, MULTI_SUFFIX))
	*multi = 0;
    baselen = strlen(basename);

    m = HTSplitFilename(basename, required);

    dp = opendir(dirname);
    if (!dp) {
	HTTRACE(PROT_TRACE, "Warning..... Can't open directory %s\n" _ dirname);
	goto dir_match_failed;
    }

    matches = HTArray_new(VARIANTS);
#ifdef HAVE_READDIR_R_2
	while ((dirbuf = (struct dirent *) readdir_r(dp, &result))) {
#elif defined(HAVE_READDIR_R_3)
        while (readdir_r(dp, &result, &dirbuf) == 0) {
#else
	while ((dirbuf = readdir(dp))) {
#endif /* HAVE_READDIR_R_2 */
	if (!dirbuf->d_ino) continue;	/* Not in use */
	if (!strcmp(dirbuf->d_name,".") ||
	    !strcmp(dirbuf->d_name,"..") ||
	    !strcmp(dirbuf->d_name, DEFAULT_DIR_FILE))
	    continue;

	/* Use of direct->namlen is only valid in BSD'ish system */
	/* Thanks to chip@chinacat.unicom.com (Chip Rosenthal) */
	/* if ((int)(dirbuf->d_namlen) >= baselen) { */
	if ((int) strlen(dirbuf->d_name) >= baselen) {
	    n = HTSplitFilename(dirbuf->d_name, actual);
	    if (multi_match(required, m, actual, n)) {
		HTContentDescription * cd;
		if ((cd = (HTContentDescription  *)
		     HT_CALLOC(1, sizeof(HTContentDescription))) == NULL)
		    HT_OUTOFMEM("dir_matches");
		if (HTBind_getFormat(dirbuf->d_name,
				     &cd->content_type,
				     &cd->content_encoding,
				     &cd->content_transfer,
				     &cd->content_language,
				     &cd->quality)) {
		    if (cd->content_type) {
			if ((cd->filename = (char *) HT_MALLOC(strlen(dirname) + 2 + strlen(dirbuf->d_name))) == NULL)
			    HT_OUTOFMEM("dir_matches");
			sprintf(cd->filename, "%s/%s", dirname, dirbuf->d_name);
			HTArray_addObject(matches, (void *) cd);
		    } else {
			HT_FREE(cd);
		    }
		} else {
		    HT_FREE(cd);
		}
	    }
	}
    }
    closedir(dp);

  dir_match_failed:
    HT_FREE(dirname);
    return matches;
}


/*
**	Get the best match for a given file
**	-----------------------------------
** On entry:
**	req->conversions  accepted content-types
**	req->encodings	  accepted content-transfer-encodings
**	req->languages	  accepted content-languages
**	path		  absolute pathname of the filename for
**			  which the match is desired.
** On exit:
**	returns	a newly allocated absolute filepath.
*/
PRIVATE char * HTGetBest (HTRequest * req, char * path)
{
    HTArray * variants = NULL;
    char * representation = NULL;

    if (!path || !*path) return NULL;

    if ((variants = dir_matches(path)) == NULL) {
	HTTRACE(PROT_TRACE, "No matches.. for \"%s\"\n" _ path);
	return NULL;
    }

#ifdef HTDEBUG
    if (PROT_TRACE) {
	void ** data;
	HTContentDescription * cd = HTArray_firstObject(variants, data);
	HTTRACE(PROT_TRACE, "Multi....... Possibilities for \"%s\"\n" _ path);
	HTTRACE(PROT_TRACE, "     QUALITY CONTENT-TYPE         LANGUAGE ENCODING  FILE\n");
	while (cd) {
	    HTTRACE(PROT_TRACE, "     %.4f  %-20.20s %-8.8s %-10.10s %s\n" _
		    cd->quality _
		    cd->content_type    ?HTAtom_name(cd->content_type)  :"-\t" _
		    cd->content_language?HTAtom_name(cd->content_language):"-" _
		    cd->content_encoding?HTAtom_name(cd->content_encoding):"-" _
		    cd->filename        ?cd->filename                    :"-");
	    cd = (HTContentDescription *) HTArray_nextObject(variants, data);
	}
    }
#endif /* HTDEBUG */

    /*
    ** Finally get the best variant which is readable
    */
    if (HTRank(req, variants)) {
	void ** data = NULL;
	HTContentDescription * cd = HTArray_firstObject(variants, data);
	while (cd) {
	    if (cd->filename) {
		if (access(cd->filename, R_OK) != -1)
		    StrAllocCopy(representation, cd->filename);
		else HTTRACE(PROT_TRACE, "Multi....... `%s\' is not readable\n" _ 
			    cd->filename);
	    }
	    HT_FREE(cd->filename);
	    HT_FREE(cd);
	    cd = (HTContentDescription *) HTArray_nextObject(variants, data);
	}
    }
    HTArray_delete(variants);
    return representation;
}



PRIVATE int welcome_value (char * name)
{
    HTList * cur = welcome_names;
    char * welcome;
    int v = 0;

    while ((welcome = (char*)HTList_nextObject(cur))) {
	v++;
	if (!strcmp(welcome,name)) return v;
    }
    return 0;
}



PRIVATE char * get_best_welcome (char * path)
{
    char * best_welcome = NULL;
    int best_value = 0;
    DIR * dp;
    struct dirent * dirbuf;
    char * last = strrchr(path, '/');

    if (!welcome_names) {
	HTAddWelcome("Welcome.html");
	HTAddWelcome("welcome.html");
#if 0
	HTAddWelcome("Index.html");
#endif
	HTAddWelcome("index.html");
    }

    if (last && last!=path) *last = 0;
    dp = opendir(path);
    if (last && last!=path) *last='/';
    if (!dp) {
	HTTRACE(PROT_TRACE, "Warning..... Can't open directory %s\n" _ path);
	return NULL;
    }
    while ((dirbuf = readdir(dp))) {
	if (!dirbuf->d_ino ||
	    !strcmp(dirbuf->d_name,".") ||
	    !strcmp(dirbuf->d_name,"..") ||
	    !strcmp(dirbuf->d_name, DEFAULT_DIR_FILE))
	    continue;
	else {
	    int v = welcome_value(dirbuf->d_name);
	    if (v > best_value) {
		best_value = v;
		StrAllocCopy(best_welcome, dirbuf->d_name);
	    }
	}
    }
    closedir(dp);

    if (best_welcome) {
	char * welcome;
	if ((welcome = (char *) HT_MALLOC(strlen(path) + strlen(best_welcome)+2)) == NULL)
	    HT_OUTOFMEM("get_best_welcome");
	sprintf(welcome, "%s%s%s", path, last ? "" : "/", best_welcome);
	HT_FREE(best_welcome);
	HTTRACE(PROT_TRACE, "Welcome..... \"%s\"\n" _ welcome);
	return welcome;
    }
    return NULL;
}

#endif /* HAVE_READDIR */


/*
**	Do multiformat handling
**	-----------------------
** On entry:
**	req->conversions  accepted content-types
**	req->encodings	  accepted content-transfer-encodings
**	req->languages	  accepted content-languages
**	path		  absolute pathname of the filename for
**			  which the match is desired.
**	stat_info	  pointer to result space.
**
** On exit:
**	returns	a newly allocated absolute filepath of the best
**		match, or NULL if no match.
**	stat_info	  will contain inode information as
**			  returned by stat().
*/
PUBLIC char * HTMulti (HTRequest *	req,
		       char *		path,
		       struct stat *	stat_info)
{
    char * new_path = NULL;
    int stat_status = -1;

    if (!req || !path || !*path || !stat_info)
	return NULL;

#ifdef HAVE_READDIR
    if (*(path+strlen(path)-1) == '/') {	/* Find welcome page */
	new_path = get_best_welcome(path);
	if (new_path) path = new_path;
    } else{
	char * multi = strrchr(path, MULTI_SUFFIX[0]);
	if (multi && !strcasecomp(multi, MULTI_SUFFIX)) {
	    HTTRACE(PROT_TRACE, "Multi....... by %s suffix\n" _ MULTI_SUFFIX);
	    if (!(new_path = HTGetBest(req, path))) {
		HTTRACE(PROT_TRACE, "Multi....... failed -- giving up\n");
		return NULL;
	    }
	    path = new_path;
	} else {
	    stat_status = HT_STAT(path, stat_info);
	    if (stat_status == -1) {
		HTTRACE(PROT_TRACE, "AutoMulti... can't stat \"%s\"(errno %d)\n" _ 
			    path _ errno);
		if (!(new_path = HTGetBest(req, path))) {
		    HTTRACE(PROT_TRACE, "AutoMulti... failed -- giving up\n");
		    return NULL;
		}
		path = new_path;
	    }
	}
    }
#endif /* HAVE_READDIR */

    if (stat_status == -1)
	stat_status = HT_STAT(path, stat_info);
    if (stat_status == -1) {
	HTTRACE(PROT_TRACE, "Stat fails.. on \"%s\" -- giving up (errno %d)\n" _ 
		    path _ errno);
	return NULL;
    } else {
	if (!new_path) {
	    StrAllocCopy(new_path, path);
	    return new_path;
	}
	else return path;
    }
}


